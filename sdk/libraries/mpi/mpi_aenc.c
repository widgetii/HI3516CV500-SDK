/**
 * Reverse Engineered by TekuConcept on April 26, 2021
 *
 * HI_MPI_AENC - Audio Encoder MPI implementation
 * Reconstructed from mpi_aenc.S (4,397 lines ARM assembly)
 */

#include "re_mpi_aenc.h"
#include "hi_comm_aio.h"
#include "hi_comm_aenc.h"
#include "mpi_errno.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/prctl.h>
#include <sys/ioctl.h>

extern HI_S32 memcpy_s(void *dest, size_t destMax, const void *src, size_t count);
extern HI_S32 memset_s(void *dest, size_t destMax, int c, size_t count);
extern HI_S32 audio_alloc(const HI_CHAR *acName, HI_U64 *pu64PhyAddr, HI_U32 **pu32VirAddr, HI_U32 u32Size);
extern HI_S32 audio_free(HI_U64 u64PhyAddr);
extern HI_S32 HI_MPI_VB_GetBlockVirAddr(HI_U32 u32Pool, HI_U64 u64PhyAddr, HI_U64 u64Padding, HI_VOID **ppVirAddr);
extern HI_VOID HI_MPI_AENC_VoiceInit(HI_VOID);

/* Global state */
static AENC_CHN_CTX_S       g_stAenc[AENC_MAX_CHN_NUM];
static pthread_mutex_t      s_AencFdmutex;
static pthread_mutex_t      s_Aencmutex;
static HI_BOOL              s_bAencInit = HI_FALSE;
static AENC_ENCODER_REGISTRY_S s_stEncoderCtx;
static HI_S32               s_s32Aencfd[AENC_MAX_CHN_NUM] = {
    [0 ... 31] = -1
};

/* ========================================================================== */
/* Internal helpers                                                           */
/* ========================================================================== */

HI_S32 MPI_AENC_Init(HI_VOID);

static HI_S32 AencCheckOpen(AENC_CHN AeChn)
{
    HI_S32 s32Ret;

    if (s_s32Aencfd[AeChn] >= 0)
        return HI_SUCCESS;

    pthread_mutex_lock(&s_AencFdmutex);

    if (s_s32Aencfd[AeChn] >= 0) {
        pthread_mutex_unlock(&s_AencFdmutex);
        return HI_SUCCESS;
    }

    s32Ret = MPI_AENC_Init();
    if (s32Ret != HI_SUCCESS) {
        pthread_mutex_unlock(&s_AencFdmutex);
        return s32Ret;
    }

    s_s32Aencfd[AeChn] = open("/dev/aenc", O_RDWR);
    if (s_s32Aencfd[AeChn] < 0) {
        HI_TRACE_AENC(HI_DBG_ERR, "open aenc dev fail\n");
        pthread_mutex_unlock(&s_AencFdmutex);
        return HI_ERR_AENC_SYS_NOTREADY;
    }

    s32Ret = ioctl(s_s32Aencfd[AeChn], IOC_AENC_INIT_CHN, &AeChn);
    if (s32Ret != HI_SUCCESS) {
        close(s_s32Aencfd[AeChn]);
        s_s32Aencfd[AeChn] = -1;
        HI_TRACE_AENC(HI_DBG_ERR, "failed\n");
        pthread_mutex_unlock(&s_AencFdmutex);
        return s32Ret;
    }

    pthread_mutex_unlock(&s_AencFdmutex);
    return HI_SUCCESS;
}

static HI_S32 AencCheckAttr(const AENC_CHN_ATTR_S *pstAttr)
{
    PAYLOAD_TYPE_E enType = pstAttr->enType;

    switch (enType) {
    case PT_G711A:
    case PT_G711U:
    case PT_G726:
    case PT_LPCM:
        break;
    case PT_ADPCMA:
        if (pstAttr->u32PtNumPerFrm != 320 &&
            pstAttr->u32PtNumPerFrm != 160 &&
            pstAttr->u32PtNumPerFrm != 80) {
            HI_TRACE_AENC(HI_DBG_ERR, "invalid u32PtNumPerFrm(%d)\n", pstAttr->u32PtNumPerFrm);
            return HI_ERR_AENC_ILLEGAL_PARAM;
        }
        if (pstAttr->pValue != HI_NULL) {
            HI_U32 u32AdpcmType = *(HI_U32 *)pstAttr->pValue;
            if (u32AdpcmType > 1) {
                HI_TRACE_AENC(HI_DBG_ERR, "invalid enADPCMType(%d)\n", u32AdpcmType);
                return HI_ERR_AENC_ILLEGAL_PARAM;
            }
        }
        break;
    case PT_AAC:
        if (pstAttr->u32PtNumPerFrm != 1152) {
            HI_TRACE_AENC(HI_DBG_ERR, "invalid u32PtNumPerFrm(%d)\n", pstAttr->u32PtNumPerFrm);
            return HI_ERR_AENC_ILLEGAL_PARAM;
        }
        break;
    default:
        if (enType > PT_BUTT) {
            HI_TRACE_AENC(HI_DBG_ERR, "invalid payload type\n");
            return HI_ERR_AENC_ILLEGAL_PARAM;
        }
        break;
    }

    if (pstAttr->u32BufSize < 2 || pstAttr->u32BufSize > 100) {
        HI_TRACE_AENC(HI_DBG_ERR, "invalid u32BufSize(%d)\n", pstAttr->u32BufSize);
        return HI_ERR_AENC_ILLEGAL_PARAM;
    }

    if (pstAttr->u32PtNumPerFrm < 80 || pstAttr->u32PtNumPerFrm > 4096) {
        HI_TRACE_AENC(HI_DBG_ERR, "invalid u32PtNumPerFrm(%d)\n", pstAttr->u32PtNumPerFrm);
        return HI_ERR_AENC_ILLEGAL_PARAM;
    }

    return HI_SUCCESS;
}

static HI_S32 AencCheckFrame(AENC_CHN AeChn, const AUDIO_FRAME_S *pstData)
{
    HI_U32 i;
    HI_U32 u32ChnNum;

    if (pstData->u32Len == 0) {
        HI_TRACE_AENC(HI_DBG_ERR, "invalid param, AeChn:%d, pData->u32Len:%d\n",
            AeChn, pstData->u32Len);
        return HI_ERR_AENC_ILLEGAL_PARAM;
    }

    if (pstData->enSoundmode > AUDIO_SOUND_MODE_STEREO) {
        HI_TRACE_AENC(HI_DBG_ERR, "invalid param, AeChn:%d, pData->enSoundmode:%d\n",
            AeChn, pstData->enSoundmode);
        return HI_ERR_AENC_ILLEGAL_PARAM;
    }

    if (pstData->enBitwidth > AUDIO_BIT_WIDTH_24) {
        HI_TRACE_AENC(HI_DBG_ERR, "invalid param, AeChn:%d, pData->enBitwidth:%d\n",
            AeChn, pstData->enBitwidth);
        return HI_ERR_AENC_ILLEGAL_PARAM;
    }

    u32ChnNum = (pstData->enSoundmode == AUDIO_SOUND_MODE_STEREO) ? 2 : 1;
    for (i = 0; i < u32ChnNum; i++) {
        if (pstData->u64VirAddr[i] == 0) {
            HI_TRACE_AENC(HI_DBG_ERR,
                "invalid param, AeChn:%d, pData->u64VirAddr[%d]:%p\n",
                AeChn, i, (HI_VOID *)(HI_UL)pstData->u64VirAddr[i]);
            return HI_ERR_AENC_ILLEGAL_PARAM;
        }
    }

    return HI_SUCCESS;
}

static HI_S32 MPI_AENC_SetDbgInfo(AENC_CHN AeChn, HI_VOID *pDbgInfo)
{
    return ioctl(s_s32Aencfd[AeChn], IOC_AENC_SET_DBG_INFO, pDbgInfo);
}

HI_S32 MPI_AENC_Init(HI_VOID)
{
    HI_S32 s32Ret;
    HI_U32 i;

    if (s_bAencInit == HI_TRUE)
        return HI_SUCCESS;

    memset_s(&s_stEncoderCtx, sizeof(s_stEncoderCtx), 0, sizeof(s_stEncoderCtx));
    s32Ret = pthread_mutex_init(&s_stEncoderCtx.mutex, NULL);
    if (s32Ret != 0)
        return HI_ERR_AENC_NOMEM;

    /* Initialize all encoder registry entries to empty (-1) */
    for (i = 0; i < AENC_MAX_ENCODER_NUM; i++)
        s_stEncoderCtx.entries[i].s32Handle = -1;

    HI_MPI_AENC_VoiceInit();

    /* Initialize per-channel contexts */
    memset_s(g_stAenc, sizeof(g_stAenc), 0, sizeof(g_stAenc));
    for (i = 0; i < AENC_MAX_CHN_NUM; i++) {
        s32Ret = pthread_mutex_init(&g_stAenc[i].mutex, NULL);
        if (s32Ret != 0) {
            pthread_mutex_destroy(&s_stEncoderCtx.mutex);
            return HI_ERR_AENC_NOMEM;
        }
        g_stAenc[i].enType1 = 8;
        g_stAenc[i].enType3 = 3;
    }

    s_bAencInit = HI_TRUE;
    return HI_SUCCESS;
}

HI_VOID MPI_AENC_Exit(HI_VOID)
{
    HI_U32 i;

    if (!s_bAencInit)
        return;

    for (i = 0; i < AENC_MAX_CHN_NUM; i++) {
        HI_MPI_AENC_DestroyChn(i);
        pthread_mutex_destroy(&g_stAenc[i].mutex);
    }

    pthread_mutex_destroy(&s_stEncoderCtx.mutex);
    memset_s(&s_stEncoderCtx, sizeof(s_stEncoderCtx), 0, sizeof(s_stEncoderCtx));
    s_bAencInit = HI_FALSE;
}

static HI_VOID MPI_AENC_StrmBufExit(AENC_CHN_CTX_S *pstAencChn)
{
    if (pstAencChn->u64StrmPhyAddr != 0) {
        audio_free(pstAencChn->u64StrmPhyAddr);
        pstAencChn->u64StrmPhyAddr = 0;
        pstAencChn->pu8StrmVirtAddr = HI_NULL;
        pstAencChn->u32StrmBufLen = 0;
    }
}

/* ========================================================================== */
/* Circular buffer operations                                                 */
/* ========================================================================== */

static HI_S32 MPI_AENC_QueryCircleBufferWriteData(AENC_CHN AeChn, HI_U32 u32BufIdx,
    const AUDIO_FRAME_S *pstData)
{
    AENC_CHN_CTX_S *pstChn = &g_stAenc[AeChn];
    AENC_CIRBUF_S *pstBuf = (u32BufIdx == 0) ?
        &pstChn->stWriteBuf : &pstChn->stReadBuf;
    HI_U32 u32Write = pstBuf->u32Write;
    HI_U32 u32Read  = pstBuf->u32Read;
    HI_U32 u32DataLen = pstData->u32Len;

    if (u32Write > u32Read) {
        HI_U32 u32End = u32Write + u32DataLen;
        if (u32End <= pstBuf->u32TotalLen)
            return HI_SUCCESS;
        u32End -= pstBuf->u32TotalLen;
        return (u32Read >= u32End) ? HI_SUCCESS : HI_FAILURE;
    } else if (u32Write < u32Read) {
        return (u32Read >= u32Write + u32DataLen) ? HI_SUCCESS : HI_FAILURE;
    } else {
        /* write == read: buffer empty if wrap flag is 0 */
        HI_U32 u32WrapFlag = pstChn->stReadBuf.u32Write; /* wrap indicator */
        return (u32WrapFlag != 0) ? HI_FAILURE : HI_SUCCESS;
    }
}

static HI_S32 MPI_AENC_PutDataToCircleBufferAndUpdateWritePtr(AENC_CHN AeChn,
    HI_U32 u32BufIdx, const AUDIO_FRAME_S *pstData)
{
    AENC_CHN_CTX_S *pstChn = &g_stAenc[AeChn];
    AENC_CIRBUF_S *pstBuf = (u32BufIdx == 0) ?
        &pstChn->stWriteBuf : &pstChn->stReadBuf;
    HI_U32 u32Write = pstBuf->u32Write;
    HI_U32 u32Read  = pstBuf->u32Read;
    HI_U32 u32DataLen = pstData->u32Len;
    HI_U32 u32TotalLen = pstBuf->u32TotalLen;
    HI_U8 *pu8Base = (HI_U8 *)pstBuf->pu8VirtAddr;
    HI_U8 *pu8Src = (HI_U8 *)(HI_UL)pstData->u64VirAddr[u32BufIdx];

    if (u32Write > u32Read) {
        HI_U32 u32End = u32Write + u32DataLen;
        if (u32End <= u32TotalLen) {
            memcpy_s(pu8Base + u32Write, u32TotalLen, pu8Src, u32DataLen);
            pstBuf->u32Write += u32DataLen;
        } else if (u32Read >= (u32End - u32TotalLen)) {
            HI_U32 u32First = u32TotalLen - u32Write;
            memcpy_s(pu8Base + u32Write, u32TotalLen, pu8Src, u32First);
            memcpy_s(pu8Base, u32TotalLen, pu8Src + u32First, u32DataLen - u32First);
            pstBuf->u32Write = u32DataLen - u32First;
        } else {
            HI_TRACE_AENC(HI_DBG_ERR,
                "It's no free buffer to save data! frame len:%d, u32Write:%d, u32Read:%d\n",
                u32DataLen, pstBuf->u32Write, u32Read);
            return HI_FAILURE;
        }
    } else if (u32Write < u32Read) {
        if (u32Read >= u32Write + u32DataLen) {
            memcpy_s(pu8Base + u32Write, u32TotalLen, pu8Src, u32DataLen);
            pstBuf->u32Write += u32DataLen;
        } else {
            HI_TRACE_AENC(HI_DBG_ERR,
                "It's no free buffer to save data! frame len:%d, u32Write:%d, u32Read:%d\n",
                u32DataLen, pstBuf->u32Write, u32Read);
            return HI_FAILURE;
        }
    } else {
        /* write == read */
        HI_U32 u32End = u32Write + u32DataLen;
        if (u32End <= u32TotalLen) {
            memcpy_s(pu8Base + u32Write, u32TotalLen, pu8Src, u32DataLen);
            pstBuf->u32Write += u32DataLen;
        } else {
            HI_U32 u32First = u32TotalLen - u32Write;
            memcpy_s(pu8Base + u32Write, u32TotalLen, pu8Src, u32First);
            memcpy_s(pu8Base, u32TotalLen, pu8Src + u32First, u32DataLen - u32First);
            pstBuf->u32Write = u32DataLen - u32First;
        }
    }

    /* Check if write wrapped to totalLen, reset to 0 */
    if (pstBuf->u32Write == pstBuf->u32TotalLen)
        pstBuf->u32Write = 0;

    return HI_SUCCESS;
}

static HI_S32 MPI_AENC_QueryCircleBufferReadData(AENC_CHN AeChn, HI_U32 u32BufIdx,
    HI_U32 u32Bitwidth)
{
    AENC_CHN_CTX_S *pstChn = &g_stAenc[AeChn];
    AENC_CIRBUF_S *pstBuf = (u32BufIdx == 0) ?
        &pstChn->stWriteBuf : &pstChn->stReadBuf;
    HI_U32 u32Read  = pstBuf->u32Read;
    HI_U32 u32Write = pstBuf->u32Write;
    HI_U32 u32BytesPerSample = (u32Bitwidth == 2) ? 4 : (u32Bitwidth + 1);
    HI_U32 u32FrameBytes = pstChn->u32PtNumPerFrm * u32BytesPerSample;

    if (u32Read > u32Write) {
        HI_U32 u32End = u32FrameBytes + u32Read;
        if (u32End <= pstBuf->u32TotalLen)
            return HI_SUCCESS;
        u32End = u32Read - pstBuf->u32TotalLen + u32FrameBytes;
        return (u32Write >= u32End) ? HI_SUCCESS : HI_FAILURE;
    } else if (u32Read < u32Write) {
        return (u32Write >= u32Read + u32FrameBytes) ? HI_SUCCESS : HI_FAILURE;
    } else {
        /* read == write: empty unless wrap flag set */
        return HI_FAILURE;
    }
}

static HI_S32 MPI_AENC_UpdateCircleBufferReadPtr(AENC_CHN AeChn, HI_U32 u32BufIdx,
    HI_U32 u32Bitwidth)
{
    AENC_CHN_CTX_S *pstChn = &g_stAenc[AeChn];
    AENC_CIRBUF_S *pstBuf = (u32BufIdx == 0) ?
        &pstChn->stWriteBuf : &pstChn->stReadBuf;
    HI_U32 u32Read  = pstBuf->u32Read;
    HI_U32 u32Write = pstBuf->u32Write;
    HI_U32 u32BytesPerSample = (u32Bitwidth == 2) ? 4 : (u32Bitwidth + 1);
    HI_U32 u32FrameBytes = pstChn->u32PtNumPerFrm * u32BytesPerSample;
    HI_U32 u32TotalLen = pstBuf->u32TotalLen;

    if (u32Read > u32Write) {
        HI_U32 u32NewRead = u32Read + u32FrameBytes;
        if (u32NewRead <= u32TotalLen) {
            pstBuf->u32Read = u32NewRead;
            u32Read = u32NewRead;
        } else {
            u32Read = u32Read - u32TotalLen + u32FrameBytes;
            pstBuf->u32Read = u32Read;
        }
    } else if (u32Read < u32Write) {
        u32Read += u32FrameBytes;
        pstBuf->u32Read = u32Read;
    } else {
        /* read == write: check wrap */
        HI_U32 u32NewRead = u32Read + u32FrameBytes;
        if (u32NewRead <= u32TotalLen) {
            pstBuf->u32Read = u32NewRead;
            u32Read = u32NewRead;
        } else {
            u32Read = u32FrameBytes - (u32TotalLen - u32Read);
            pstBuf->u32Read = u32Read;
        }
    }

    if (u32Read == u32TotalLen)
        pstBuf->u32Read = 0;

    return HI_SUCCESS;
}

/* ========================================================================== */
/* Internal channel create/destroy                                            */
/* ========================================================================== */

static HI_S32 MPI_AENC_CreateChn(AENC_CHN AeChn, const AENC_CHN_ATTR_S *pstAttr)
{
    HI_S32 s32Ret;
    HI_U32 i;
    AENC_CHN_CTX_S *pstChn = &g_stAenc[AeChn];
    AENC_CREATE_INFO_S stCreateInfo;
    HI_U32 u32PackLen;
    HI_U32 u32BufLen;
    HI_CHAR acMmzName[32];

    if (AeChn >= AENC_MAX_CHN_NUM)
        return HI_ERR_AENC_INVALID_CHNID;

    s32Ret = AencCheckOpen(AeChn);
    if (s32Ret != HI_SUCCESS)
        return s32Ret;

    if (pstAttr == HI_NULL)
        return HI_ERR_AENC_NULL_PTR;

    if (pstChn->u32Created == 1) {
        HI_TRACE_AENC(HI_DBG_ERR, "aenc chn:%d is exist!\n", AeChn);
        return HI_ERR_AENC_EXIST;
    }

    s32Ret = AencCheckAttr(pstAttr);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_AENC(HI_DBG_ERR, "aenc chn:%d attr is illegal!\n", AeChn);
        return s32Ret;
    }

    /* Initialize channel context */
    pstChn->u32StrmLen = 0;
    pstChn->u64StrmPhyAddr = 0;
    pstChn->pu8StrmVirtAddr = HI_NULL;
    pstChn->s32EncoderIdx = -1;
    pstChn->s32Handle1 = -1;
    pstChn->s32Handle2 = -1;
    pstChn->s32Handle3 = -1;
    pstChn->u32StrmBufLen = 0;
    pstChn->u32StrmPackLen = 0;
    pstChn->s32EncCount = 0;
    pstChn->s32DecCount = 0;
    pstChn->field_28 = 0;
    pstChn->u32DbgField34 = 0;
    pstChn->field_38 = 0;
    pstChn->field_3C = 0;
    pstChn->pEncoder = HI_NULL;
    pstChn->field_48 = 0;
    pstChn->u32PtNumPerFrm = 0;
    pstChn->u32StreamReady = 0;
    memset_s(&pstChn->enType1, 16, 0, 16);
    memset_s(&pstChn->stWriteBuf, sizeof(AENC_CIRBUF_S), 0, sizeof(AENC_CIRBUF_S));
    memset_s(&pstChn->stReadBuf, sizeof(AENC_CIRBUF_S), 0, sizeof(AENC_CIRBUF_S));
    memset_s(&pstChn->u32EncErrCnt, 32, 0, 32);

    pstChn->enType2 = 8;
    pstChn->enType3 = 3;

    /* Look up encoder in registry */
    if (s_stEncoderCtx.u32Count == 0) {
        /* No encoders registered yet */
    }

    /* Find matching encoder by payload type */
    {
        HI_S32 s32EncIdx = -1;
        pthread_mutex_lock(&s_stEncoderCtx.mutex);
        for (i = 0; i < AENC_MAX_ENCODER_NUM; i++) {
            if (s_stEncoderCtx.entries[i].s32Handle == (HI_S32)pstAttr->enType) {
                s32EncIdx = i;
                break;
            }
        }
        pthread_mutex_unlock(&s_stEncoderCtx.mutex);

        if (s32EncIdx == -1) {
            HI_TRACE_AENC(HI_DBG_ERR, "aenc chn:%d the payload type%d not registered\n",
                AeChn, pstAttr->enType);
            return HI_ERR_AENC_NOT_SUPPORT;
        }

        pstChn->s32EncoderIdx = s32EncIdx;
    }

    /* Open encoder */
    if (s_stEncoderCtx.entries[pstChn->s32EncoderIdx].s32Handle == -1) {
        HI_TRACE_AENC(HI_DBG_ERR, "The encoder has been unregistered!\n");
        return HI_ERR_AENC_ENCODER_ERR;
    }

    s32Ret = s_stEncoderCtx.entries[pstChn->s32EncoderIdx].pfnOpenEncoder(
        pstAttr->pValue, &pstChn->pEncoder);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_AENC(HI_DBG_ERR, "aenc chn:%d open encoder failed!\n", AeChn);
        return s32Ret;
    }

    /* Prepare stream buffer parameters */
    u32PackLen = s_stEncoderCtx.entries[pstChn->s32EncoderIdx].u32MaxFrmLen;
    u32PackLen = (u32PackLen + 63) & ~63u; /* Align to 64 */

    pstChn->u32StrmPackLen = u32PackLen;
    pstChn->u32PtNumPerFrm = pstAttr->u32PtNumPerFrm;
    pstChn->enType1 = pstAttr->enType;

    /* Create kernel channel */
    stCreateInfo.enType = pstAttr->enType;
    stCreateInfo.u32BufSize = pstAttr->u32BufSize;
    stCreateInfo.u32PtNumPerFrm = pstAttr->u32PtNumPerFrm;
    stCreateInfo.pValue = pstAttr->pValue;
    stCreateInfo.field_10 = 0;

    s32Ret = ioctl(s_s32Aencfd[AeChn], IOC_AENC_CREATE_CHN, &stCreateInfo);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_AENC(HI_DBG_ERR, "aenc chn:%d, create chn failed!\n", AeChn);
        goto err_close_encoder;
    }

    pstChn->u32DbgField34 = stCreateInfo.field_10;

    /* Allocate stream buffer (via MMZ) */
    u32BufLen = (u32PackLen + sizeof(AENC_STREAM_HEADER_S)) * pstAttr->u32BufSize;

    snprintf(acMmzName, sizeof(acMmzName), "AENC(%d)StrmBuf", AeChn);
    s32Ret = audio_alloc(acMmzName, &pstChn->u64StrmPhyAddr,
        &pstChn->pu8StrmVirtAddr, u32BufLen);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_AENC(HI_DBG_ERR, "aenc chn:%d alloc mmb fail, len:%d\n", AeChn, u32BufLen);
        goto err_destroy_kern_chn;
    }

    pstChn->u32StrmBufLen = u32BufLen;
    pstChn->u32StreamReady = 1;

    /* Initialize stream circular buffer for kernel read */
    {
        HI_U8 strmBufInfo[24];
        memset_s(strmBufInfo, 24, 0, 24);
        memcpy_s(strmBufInfo, 24, &pstChn->u64StrmPhyAddr, 8);
        memcpy_s(strmBufInfo + 8, 16, &u32PackLen, 4);
        memcpy_s(strmBufInfo + 12, 12, &u32BufLen, 4);

        s32Ret = ioctl(s_s32Aencfd[AeChn], IOC_AENC_SET_STRM_BUF, strmBufInfo);
        if (s32Ret != HI_SUCCESS) {
            HI_TRACE_AENC(HI_DBG_ERR,
                "aenc chn:%d read buffer init in kernel mode failed!\n", AeChn);
            goto err_free_strm_buf;
        }
    }

    /* Allocate circular buffer for write path */
    {
        HI_U32 u32CirBufLen = pstAttr->u32PtNumPerFrm * 4 * pstAttr->u32BufSize;
        HI_U64 u64CirPhyAddr = 0;
        HI_U32 *pu32CirVirtAddr = HI_NULL;

        snprintf(acMmzName, sizeof(acMmzName), "AENC(%d) CirBuf", AeChn);
        s32Ret = audio_alloc(acMmzName, &u64CirPhyAddr, &pu32CirVirtAddr, u32CirBufLen);
        if (s32Ret != HI_SUCCESS) {
            HI_TRACE_AENC(HI_DBG_ERR, "alloc mmb fail, len:%d\n", u32CirBufLen);
            goto err_free_strm_buf;
        }

        pstChn->stWriteBuf.u64PhyAddr = u64CirPhyAddr;
        pstChn->stWriteBuf.pu8VirtAddr = pu32CirVirtAddr;
        pstChn->stWriteBuf.u32PackLen = u32PackLen;
        pstChn->stWriteBuf.u32Size = u32CirBufLen;
        pstChn->stWriteBuf.u32TotalLen = u32CirBufLen;
        pstChn->stWriteBuf.u32Read = 0;
        pstChn->stWriteBuf.u32Write = 0;
    }

    /* Allocate encode working buffers */
    pstChn->pu8EncBuf[0] = (HI_U8 *)malloc(16384);
    if (pstChn->pu8EncBuf[0] == HI_NULL) {
        HI_TRACE_AENC(HI_DBG_ERR, "malloc err when create aenc chn!\n");
        goto err_free_cirbuf;
    }

    pstChn->pu8EncBuf[1] = (HI_U8 *)malloc(16384);
    if (pstChn->pu8EncBuf[1] == HI_NULL) {
        HI_TRACE_AENC(HI_DBG_ERR, "malloc err when create aenc chn!\n");
        free(pstChn->pu8EncBuf[0]);
        pstChn->pu8EncBuf[0] = HI_NULL;
        goto err_free_cirbuf;
    }

    pstChn->s32FrameCount = -1;
    pstChn->u32Created = 1;
    return HI_SUCCESS;

err_free_cirbuf:
    if (pstChn->stWriteBuf.u64PhyAddr) {
        audio_free(pstChn->stWriteBuf.u64PhyAddr);
        pstChn->stWriteBuf.u64PhyAddr = 0;
        pstChn->stWriteBuf.pu8VirtAddr = HI_NULL;
    }
err_free_strm_buf:
    MPI_AENC_StrmBufExit(pstChn);
err_destroy_kern_chn:
    ioctl(s_s32Aencfd[AeChn], IOC_AENC_DESTROY_CHN);
err_close_encoder:
    if (pstChn->pEncoder && pstChn->s32EncoderIdx >= 0) {
        if (s_stEncoderCtx.entries[pstChn->s32EncoderIdx].pfnCloseEncoder)
            s_stEncoderCtx.entries[pstChn->s32EncoderIdx].pfnCloseEncoder(pstChn->pEncoder);
        pstChn->pEncoder = HI_NULL;
    }
    return s32Ret;
}

static HI_S32 MPI_AENC_DestroyChn(AENC_CHN AeChn)
{
    HI_S32 s32Ret;
    AENC_CHN_CTX_S *pstChn = &g_stAenc[AeChn];

    if (AeChn >= AENC_MAX_CHN_NUM)
        return HI_ERR_AENC_INVALID_CHNID;

    s32Ret = AencCheckOpen(AeChn);
    if (s32Ret != HI_SUCCESS)
        return s32Ret;

    /* Check for unreleased stream buffers */
    if (pstChn->s32EncCount != pstChn->s32DecCount) {
        HI_TRACE_AENC(HI_DBG_WARN,
            "There is stream buffer need to release in aenc chn:%d !\n", AeChn);
        return HI_ERR_AENC_BUF_FULL;
    }

    /* Clear handles */
    pstChn->s32Handle2 = -1;
    pstChn->s32Handle3 = -1;
    pstChn->u32Created = 0;

    /* Issue destroy + clear stream buffer ioctls */
    ioctl(s_s32Aencfd[AeChn], IOC_AENC_DESTROY_CHN);
    ioctl(s_s32Aencfd[AeChn], IOC_AENC_CLR_STRM_BUF);

    /* Free stream buffer */
    if (pstChn->u32StrmBufReady)
        pstChn->u32StrmBufReady = 0;

    if (pstChn->u32StreamReady)
        MPI_AENC_StrmBufExit(pstChn);

    /* Free circular write buffer */
    if (pstChn->stWriteBuf.u64PhyAddr) {
        audio_free(pstChn->stWriteBuf.u64PhyAddr);
    }

    /* Zero out circular buffers */
    pstChn->stWriteBuf.u64PhyAddr = 0;
    pstChn->stWriteBuf.pu8VirtAddr = HI_NULL;
    pstChn->stWriteBuf.u32PackLen = 0;
    pstChn->stReadBuf.u64PhyAddr = 0;
    pstChn->stReadBuf.pu8VirtAddr = HI_NULL;
    pstChn->stReadBuf.u32PackLen = 0;

    /* Free encode working buffers */
    if (pstChn->pu8EncBuf[0]) {
        free(pstChn->pu8EncBuf[0]);
        pstChn->pu8EncBuf[0] = HI_NULL;
    }

    if (pstChn->pu8EncBuf[1]) {
        free(pstChn->pu8EncBuf[1]);
        pstChn->pu8EncBuf[1] = HI_NULL;
    }

    return s32Ret;
}

/* ========================================================================== */
/* Frame get/release (for bound source)                                       */
/* ========================================================================== */

static HI_S32 MPI_AENC_GetFrame(AENC_CHN AeChn, HI_U8 *pstFrmBuf)
{
    HI_S32 s32Ret;
    HI_U32 u32ChnNum, i;
    HI_U8 *pCur, *pEnd, *pPhyOff;

    if (AeChn >= AENC_MAX_CHN_NUM)
        return HI_ERR_AENC_INVALID_CHNID;

    s32Ret = AencCheckOpen(AeChn);
    if (s32Ret != HI_SUCCESS)
        return s32Ret;

    if (pstFrmBuf == HI_NULL)
        return HI_ERR_AENC_NULL_PTR;

    /* IOC_AENC_GET_FRAME: get frame from bound source */
    s32Ret = ioctl(s_s32Aencfd[AeChn], IOC_AENC_GET_FRAME, pstFrmBuf);
    if (s32Ret != HI_SUCCESS)
        return s32Ret;

    /* Resolve physical addresses to virtual */
    u32ChnNum = (*(HI_U32 *)(pstFrmBuf + 4) == 1) ? 2 : 1; /* enSoundmode check */
    pCur = pstFrmBuf + 48; /* offset to VirAddr[0] in AUDIO_FRAME_S */
    pPhyOff = pstFrmBuf + 16; /* offset to PhyAddr[0] */
    pEnd = pstFrmBuf + ((u32ChnNum == 1) ? 52 : 56);

    while (pCur != pEnd) {
        HI_U32 u32Pool = *(HI_U32 *)(pCur - 40);
        HI_U64 u64PhyAddr;
        memcpy_s(&u64PhyAddr, 8, pPhyOff, 8);

        s32Ret = HI_MPI_VB_GetBlockVirAddr(u32Pool, u64PhyAddr, 0, (HI_VOID **)(pCur));
        if (s32Ret != HI_SUCCESS) {
            /* Release frame on VB addr resolution failure */
            ioctl(s_s32Aencfd[AeChn], IOC_AENC_RELEASE_FRAME, pstFrmBuf);
            if (s32Ret == HI_SUCCESS)
                s32Ret = HI_FAILURE;
            HI_TRACE_AENC(HI_DBG_ERR, "aenc chn:%d AENC_RELEASE_FRAME failed!\n", AeChn);
            return s32Ret;
        }

        /* Handle AEC frame if present */
        if (*(HI_U32 *)(pstFrmBuf + 112) == 1) {
            HI_U64 u64AecPhyAddr;
            memcpy_s(&u64AecPhyAddr, 8, pPhyOff + 48, 8);
            s32Ret = HI_MPI_VB_GetBlockVirAddr(*(HI_U32 *)(pCur + 52), u64AecPhyAddr, 0,
                (HI_VOID **)(pCur + 16));
            if (s32Ret != HI_SUCCESS)
                break;
        }
        pCur += 4;
        pPhyOff += 8;
    }

    return s32Ret;
}

static HI_S32 MPI_AENC_ReleaseFrame(AENC_CHN AeChn, HI_U8 *pstFrmBuf)
{
    HI_S32 s32Ret;

    if (AeChn >= AENC_MAX_CHN_NUM)
        return HI_ERR_AENC_INVALID_CHNID;

    s32Ret = AencCheckOpen(AeChn);
    if (s32Ret != HI_SUCCESS)
        return s32Ret;

    if (pstFrmBuf == HI_NULL)
        return HI_ERR_AENC_NULL_PTR;

    return ioctl(s_s32Aencfd[AeChn], IOC_AENC_RELEASE_FRAME, pstFrmBuf);
}

/* ========================================================================== */
/* Frame processing thread                                                    */
/* ========================================================================== */

static HI_VOID *MPI_AENC_ChnGetFrmProc(HI_VOID *pArg)
{
    AENC_CHN_CTX_S *pstChn = (AENC_CHN_CTX_S *)pArg;
    AENC_CHN AeChn;
    HI_U8 stFrmBuf[128];
    HI_U8 stAecBuf[56];
    HI_S32 s32Ret;

    if (pstChn == HI_NULL)
        return HI_NULL;

    AeChn = pstChn->s32Handle1;
    prctl(PR_SET_NAME, "hi_Aenc_Get", 0, 0, 0);

    while (pstChn->field_28 == 1) {
        while (1) {
            s32Ret = MPI_AENC_GetFrame(AeChn, stFrmBuf);
            if (s32Ret == HI_SUCCESS)
                break;

            usleep(1000);

            if (pstChn->field_28 != 1)
                goto exit;
        }

        /* Encode the frame */
        {
            HI_BOOL bHasAec = (*(HI_U32 *)(stFrmBuf + 112) == 1);
            HI_MPI_AENC_SendFrame(AeChn, (const AUDIO_FRAME_S *)stFrmBuf,
                bHasAec ? (const AEC_FRAME_S *)stAecBuf : HI_NULL);
        }

        s32Ret = MPI_AENC_ReleaseFrame(AeChn, stFrmBuf);
        if (s32Ret != HI_SUCCESS) {
            HI_TRACE_AENC(HI_DBG_ERR,
                "Aenc chn %d release frame failed, s32Ret=0x%x!\n", AeChn, s32Ret);
        }
    }

exit:
    pstChn->field_28 = 0;
    return HI_NULL;
}

static HI_S32 MPI_AENC_CreateGetFrmProc(AENC_CHN AeChn)
{
    AENC_CHN_CTX_S *pstChn = &g_stAenc[AeChn];
    pthread_t tid;

    if (pstChn->field_28 == 1)
        return HI_SUCCESS;

    pstChn->s32Handle1 = AeChn;
    pstChn->field_28 = 1;

    if (pthread_create(&tid, NULL, MPI_AENC_ChnGetFrmProc, pstChn) != 0)
        return HI_ERR_AENC_NOT_PERM;

    pstChn->s32FrameCount = (HI_S32)tid;
    return HI_SUCCESS;
}

static HI_VOID MPI_AENC_DestroyGetFrmProc(AENC_CHN AeChn)
{
    AENC_CHN_CTX_S *pstChn = &g_stAenc[AeChn];

    if (pstChn->field_28 != 1)
        return;

    pstChn->field_28 = 0;
    pthread_join((pthread_t)pstChn->s32FrameCount, NULL);
}

/* ========================================================================== */
/* Public API functions                                                       */
/* ========================================================================== */

HI_S32 HI_MPI_AENC_RegisterEncoder(HI_S32 *ps32Handle, const AENC_ENCODER_S *pstEncoder)
{
    HI_U32 i;

    if (ps32Handle == HI_NULL || pstEncoder == HI_NULL)
        return HI_ERR_AENC_NULL_PTR;

    if (pstEncoder->pfnOpenEncoder == HI_NULL ||
        pstEncoder->pfnEncodeFrm == HI_NULL ||
        pstEncoder->pfnCloseEncoder == HI_NULL)
        return HI_ERR_AENC_NULL_PTR;

    pthread_mutex_lock(&s_stEncoderCtx.mutex);

    if (s_stEncoderCtx.u32Count >= AENC_MAX_ENCODER_NUM) {
        pthread_mutex_unlock(&s_stEncoderCtx.mutex);
        return HI_ERR_AENC_BUF_FULL;
    }

    /* Check for duplicate payload type */
    for (i = 0; i < AENC_MAX_ENCODER_NUM; i++) {
        if (s_stEncoderCtx.entries[i].s32Handle == (HI_S32)pstEncoder->enType) {
            pthread_mutex_unlock(&s_stEncoderCtx.mutex);
            return HI_ERR_AENC_BUF_FULL;
        }
    }

    /* Find empty slot */
    for (i = 0; i < AENC_MAX_ENCODER_NUM; i++) {
        if (s_stEncoderCtx.entries[i].s32Handle == -1) {
            memcpy_s(&s_stEncoderCtx.entries[i].s32Handle, 40, pstEncoder, 40);
            *ps32Handle = i;
            s_stEncoderCtx.u32Count++;
            pthread_mutex_unlock(&s_stEncoderCtx.mutex);
            return HI_SUCCESS;
        }
    }

    pthread_mutex_unlock(&s_stEncoderCtx.mutex);
    return HI_SUCCESS;
}

HI_S32 HI_MPI_AENC_UnRegisterEncoder(HI_S32 s32Handle)
{
    HI_U32 i;

    if ((HI_U32)s32Handle >= AENC_MAX_ENCODER_NUM)
        return HI_ERR_AENC_ILLEGAL_PARAM;

    pthread_mutex_lock(&s_stEncoderCtx.mutex);

    if (s_stEncoderCtx.u32Count == 0 ||
        s_stEncoderCtx.entries[s32Handle].s32Handle == -1) {
        pthread_mutex_unlock(&s_stEncoderCtx.mutex);
        return HI_ERR_AENC_BUF_FULL;
    }

    /* Check no channel is using this encoder */
    for (i = 0; i < AENC_MAX_CHN_NUM; i++) {
        pthread_mutex_lock(&g_stAenc[i].mutex);
        if (g_stAenc[i].s32EncoderIdx == s32Handle &&
            g_stAenc[i].u32Created == 1) {
            HI_TRACE_AENC(HI_DBG_ERR,
                "Aenc chn%d is created by this encoder, please destroy it first!\n", i);
            pthread_mutex_unlock(&g_stAenc[i].mutex);
            pthread_mutex_unlock(&s_stEncoderCtx.mutex);
            return HI_ERR_AENC_BUF_FULL;
        }
        pthread_mutex_unlock(&g_stAenc[i].mutex);
    }

    /* Clear the entry */
    memset_s(&s_stEncoderCtx.entries[s32Handle].s32Handle + 1, 40, 0, 40);
    s_stEncoderCtx.entries[s32Handle].s32Handle = -1;
    s_stEncoderCtx.u32Count--;
    pthread_mutex_unlock(&s_stEncoderCtx.mutex);
    return HI_SUCCESS;
}

HI_S32 HI_MPI_AENC_CreateChn(AENC_CHN AeChn, const AENC_CHN_ATTR_S *pstAttr)
{
    HI_S32 s32Ret;
    AENC_CHN_CTX_S *pstChn;

    if (AeChn >= AENC_MAX_CHN_NUM)
        return HI_ERR_AENC_INVALID_CHNID;

    s32Ret = AencCheckOpen(AeChn);
    if (s32Ret != HI_SUCCESS)
        return s32Ret;

    if (pstAttr == HI_NULL)
        return HI_ERR_AENC_NULL_PTR;

    pstChn = &g_stAenc[AeChn];

    pthread_mutex_lock(&s_Aencmutex);
    pthread_mutex_lock(&pstChn->mutex);

    s32Ret = MPI_AENC_CreateChn(AeChn, pstAttr);
    if (s32Ret != HI_SUCCESS) {
        pthread_mutex_unlock(&s_Aencmutex);
        pthread_mutex_unlock(&pstChn->mutex);
        return s32Ret;
    }

    /* Set codec type fields based on payload type */
    if (pstAttr->enType == PT_G726) {
        if (pstAttr->pValue != HI_NULL) {
            pstChn->enType2 = *(PAYLOAD_TYPE_E *)pstAttr->pValue;
        }
    } else {
        pstChn->enType2 = 8;
        if (pstAttr->enType == PT_ADPCMA) {
            if (pstAttr->pValue != HI_NULL)
                pstChn->enType3 = *(PAYLOAD_TYPE_E *)pstAttr->pValue;
        } else {
            pstChn->enType3 = 3;
        }
    }

    /* Set debug info via ioctl */
    ioctl(s_s32Aencfd[AeChn], IOC_AENC_SET_DBG_INFO, &pstChn->u32EncErrCnt);

    /* Create frame processing thread */
    s32Ret = MPI_AENC_CreateGetFrmProc(AeChn);
    if (s32Ret != HI_SUCCESS) {
        pthread_mutex_unlock(&s_Aencmutex);
        pthread_mutex_unlock(&pstChn->mutex);
        return s32Ret;
    }

    pthread_mutex_unlock(&s_Aencmutex);
    pthread_mutex_unlock(&pstChn->mutex);
    return HI_SUCCESS;
}

HI_S32 HI_MPI_AENC_DestroyChn(AENC_CHN AeChn)
{
    HI_S32 s32Ret;
    AENC_CHN_CTX_S *pstChn;

    if (AeChn >= AENC_MAX_CHN_NUM)
        return HI_ERR_AENC_INVALID_CHNID;

    pstChn = &g_stAenc[AeChn];

    pthread_mutex_lock(&s_Aencmutex);
    pthread_mutex_lock(&pstChn->mutex);

    if (pstChn->u32Created != 1) {
        pthread_mutex_unlock(&s_Aencmutex);
        pthread_mutex_unlock(&pstChn->mutex);
        return HI_SUCCESS;
    }

    s32Ret = MPI_AENC_DestroyChn(AeChn);
    if (s32Ret != HI_SUCCESS) {
        pthread_mutex_unlock(&s_Aencmutex);
        pthread_mutex_unlock(&pstChn->mutex);
        return s32Ret;
    }

    /* Unlock before joining thread (thread may need lock) */
    pthread_mutex_unlock(&pstChn->mutex);
    MPI_AENC_DestroyGetFrmProc(AeChn);
    pthread_mutex_lock(&pstChn->mutex);

    /* Close encoder */
    if (pstChn->s32EncoderIdx != -1) {
        if (s_stEncoderCtx.entries[pstChn->s32EncoderIdx].s32Handle == -1) {
            HI_TRACE_AENC(HI_DBG_ERR,
                "The encoder[%d] s32Handle has been Reset!\n", pstChn->s32EncoderIdx);
            s32Ret = HI_ERR_AENC_ENCODER_ERR;
        } else {
            s_stEncoderCtx.entries[pstChn->s32EncoderIdx].pfnCloseEncoder(pstChn->pEncoder);
        }
    } else {
        HI_TRACE_AENC(HI_DBG_ERR,
            "The encoder[%d] s32Handle has been Reset!\n", pstChn->s32EncoderIdx);
        s32Ret = HI_ERR_AENC_ENCODER_ERR;
    }

    pthread_mutex_unlock(&s_Aencmutex);
    pthread_mutex_unlock(&pstChn->mutex);
    return s32Ret;
}

HI_S32 HI_MPI_AENC_SendFrame(AENC_CHN AeChn, const AUDIO_FRAME_S *pstFrm,
    const AEC_FRAME_S *pstAecFrm)
{
    HI_S32 s32Ret;
    AENC_CHN_CTX_S *pstChn;
    AUDIO_FRAME_S stLocalFrm;
    HI_U32 u32StrmLen = 0;
    HI_U32 u32PtNum;

    if (AeChn >= AENC_MAX_CHN_NUM)
        return HI_ERR_AENC_INVALID_CHNID;

    s32Ret = AencCheckOpen(AeChn);
    if (s32Ret != HI_SUCCESS)
        return s32Ret;

    if (pstFrm == HI_NULL)
        return HI_ERR_AENC_NULL_PTR;

    pstChn = &g_stAenc[AeChn];

    if (pstChn->u32Created != 1)
        return HI_ERR_AENC_UNEXIST;

    /* Validate frame */
    pstChn->u32EncErrCnt++;
    s32Ret = AencCheckFrame(AeChn, pstFrm);
    if (s32Ret != HI_SUCCESS) {
        pstChn->u32CheckFrameErr++;
        MPI_AENC_SetDbgInfo(AeChn, &pstChn->u32EncErrCnt);
        return s32Ret;
    }

    memcpy_s(&stLocalFrm, sizeof(AUDIO_FRAME_S), pstFrm, sizeof(AUDIO_FRAME_S));

    /* Check point number fits channel configuration */
    u32PtNum = stLocalFrm.u32Len >> stLocalFrm.enBitwidth;
    if (pstChn->u32PtNumPerFrm < u32PtNum) {
        HI_TRACE_AENC(HI_DBG_ERR,
            "frame point num:%d error, it's bigger than aenc chn frame point num:%d\n",
            u32PtNum, pstChn->u32PtNumPerFrm);
        pstChn->u32CheckFrameErr++;
        MPI_AENC_SetDbgInfo(AeChn, &pstChn->u32EncErrCnt);
        return HI_ERR_AENC_BUF_FULL;
    }

    /* Check if encoder is bound (pstAecFrm != NULL means voice encode path) */
    if (pstAecFrm != HI_NULL) {
        /* Voice encode path: check point limit */
        u32PtNum = (pstFrm->enBitwidth == 2) ? 4 : (pstFrm->enBitwidth + 1);
        u32PtNum = pstFrm->u32Len / u32PtNum;
        if (u32PtNum > 480) {
            HI_TRACE_AENC(HI_DBG_ERR,
                "point num (%d) of this frame is larger than MAX_VOICE_POINT_NUM(%d) for voie encode\n",
                u32PtNum, 480);
            return HI_ERR_AENC_NOT_SUPPORT;
        }

        /* Use kernel ioctl path for bound source */
        s32Ret = AencCheckOpen(AeChn);
        if (s32Ret != HI_SUCCESS)
            return s32Ret;

        s32Ret = ioctl(s_s32Aencfd[AeChn], IOC_AENC_SEND_FRAME, &stLocalFrm);
        return s32Ret;
    }

    /* Software encode path */
    if (pstChn->s32EncoderIdx == -1)
        return HI_ERR_AENC_ENCODER_ERR;

    /* Check encoder still registered */
    if (s_stEncoderCtx.entries[pstChn->s32EncoderIdx].s32Handle == -1) {
        HI_TRACE_AENC(HI_DBG_ERR, "The encoder has been unregistered!\n");
        return HI_ERR_AENC_ENCODER_ERR;
    }

    pthread_mutex_lock(&pstChn->mutex);

    if (pstChn->u32Created != 1 || pstChn->u32StreamReady != 1) {
        pthread_mutex_unlock(&pstChn->mutex);
        return HI_ERR_AENC_UNEXIST;
    }

    /* Call encoder */
    s32Ret = s_stEncoderCtx.entries[pstChn->s32EncoderIdx].pfnEncodeFrm(
        pstChn->pEncoder, &stLocalFrm,
        pstChn->pu8EncBuf[0], &u32StrmLen);

    if (s32Ret == AENC_ADAPT_MAGIC) {
        /* Adaptation magic — skip silently */
        pthread_mutex_unlock(&pstChn->mutex);
        return HI_SUCCESS;
    }

    if (s32Ret != HI_SUCCESS) {
        pstChn->u32CheckFrameErr++;
        MPI_AENC_SetDbgInfo(AeChn, &pstChn->u32EncErrCnt);
        pthread_mutex_unlock(&pstChn->mutex);
        return HI_ERR_AENC_ENCODER_ERR;
    }

    /* Write encoded data to stream buffer with header */
    {
        AENC_STREAM_HEADER_S stHdr;
        HI_U32 u32TotalWrite;
        HI_U8 *pu8WriteAddr;
        HI_U32 *pu32WritePtr;
        HI_U32 *pu32ReadPtr;
        HI_U32 u32WriteOff, u32ReadOff;
        HI_U32 u32PackLen = pstChn->u32StrmPackLen;

        u32TotalWrite = u32StrmLen + sizeof(AENC_STREAM_HEADER_S);

        stHdr.u32Len = u32StrmLen;
        stHdr.u64TimeStamp = stLocalFrm.u64TimeStamp;
        stHdr.u32Seq = pstChn->field_48;
        pstChn->field_48++;

        /* Write header + data to stream buffer */
        pu8WriteAddr = (HI_U8 *)pstChn->pu8StrmVirtAddr;
        pu32WritePtr = (HI_U32 *)((HI_U8 *)pstChn->pu8StrmVirtAddr + pstChn->u32StrmBufLen - 16);
        pu32ReadPtr = (HI_U32 *)((HI_U8 *)pstChn->pu8StrmVirtAddr + pstChn->u32StrmBufLen - 8);

        u32WriteOff = *pu32WritePtr;
        u32ReadOff = *pu32ReadPtr;

        /* Copy header */
        memcpy_s(pu8WriteAddr + u32WriteOff, sizeof(AENC_STREAM_HEADER_S),
            &stHdr, sizeof(AENC_STREAM_HEADER_S));

        /* Advance write pointer */
        u32WriteOff += u32PackLen;
        if (u32WriteOff >= pstChn->u32StrmBufLen - 16)
            u32WriteOff -= (pstChn->u32StrmBufLen - 16);
        *pu32WritePtr = u32WriteOff;

        /* Sync write to read pointer */
        *pu32ReadPtr = u32WriteOff;
    }

    /* Notify kernel of updated write position + counters */
    pstChn->field_7C++;
    MPI_AENC_SetDbgInfo(AeChn, &pstChn->u32EncErrCnt);

    pthread_mutex_unlock(&pstChn->mutex);
    return HI_SUCCESS;
}

HI_S32 HI_MPI_AENC_GetStream(AENC_CHN AeChn, AUDIO_STREAM_S *pstStream,
    HI_S32 s32MilliSec)
{
    HI_S32 s32Ret;
    AENC_CHN_CTX_S *pstChn;
    HI_U8 stBuf[48];

    if (AeChn >= AENC_MAX_CHN_NUM)
        return HI_ERR_AENC_INVALID_CHNID;

    s32Ret = AencCheckOpen(AeChn);
    if (s32Ret != HI_SUCCESS)
        return s32Ret;

    if (pstStream == HI_NULL)
        return HI_ERR_AENC_NULL_PTR;

    if (s32MilliSec < -1) {
        HI_TRACE_AENC(HI_DBG_ERR, "param s32MilliSec = %d error\n", s32MilliSec);
        return HI_ERR_AENC_ILLEGAL_PARAM;
    }

    pstStream->pStream = HI_NULL;

    /* Get stream from kernel */
    memset_s(stBuf, sizeof(stBuf), 0, sizeof(stBuf));
    *(HI_S32 *)stBuf = s32MilliSec;

    s32Ret = ioctl(s_s32Aencfd[AeChn], IOC_AENC_GET_STREAM, stBuf);
    if (s32Ret != HI_SUCCESS)
        return s32Ret;

    pstChn = &g_stAenc[AeChn];

    pthread_mutex_lock(&pstChn->mutex);

    if (pstChn->u32Created != 1 || pstChn->u32StreamReady != 1) {
        pthread_mutex_unlock(&pstChn->mutex);
        return HI_ERR_AENC_UNEXIST;
    }

    /* Copy stream info from ioctl result */
    pstStream->u32Len = *(HI_U32 *)(stBuf + 16);
    pstStream->u64TimeStamp = *(HI_U64 *)(stBuf + 24);
    pstStream->u32Seq = *(HI_U32 *)(stBuf + 32);

    /* Resolve physical address to virtual */
    if (pstChn->u32StrmBufReady) {
        HI_U32 u32PhyOff = *(HI_U32 *)(stBuf + 16) - pstChn->enType1;
        pstStream->pStream = (HI_U8 *)pstChn->pu8StrmVirtAddr + u32PhyOff;

        /* Advance read pointer */
        HI_U32 *pu32ReadPtr = (HI_U32 *)((HI_U8 *)pstChn->pu8StrmVirtAddr +
            pstChn->u32StrmBufLen - 8);
        HI_U32 u32ReadOff = *pu32ReadPtr + pstChn->u32StrmPackLen;
        if (u32ReadOff >= pstChn->u32StrmBufLen - 16)
            u32ReadOff -= (pstChn->u32StrmBufLen - 16);
        *pu32ReadPtr = u32ReadOff;
    }

    pthread_mutex_unlock(&pstChn->mutex);
    return HI_SUCCESS;
}

HI_S32 HI_MPI_AENC_ReleaseStream(AENC_CHN AeChn, const AUDIO_STREAM_S *pstStream)
{
    HI_S32 s32Ret;
    AENC_CHN_CTX_S *pstChn;

    if (AeChn >= AENC_MAX_CHN_NUM)
        return HI_ERR_AENC_INVALID_CHNID;

    s32Ret = AencCheckOpen(AeChn);
    if (s32Ret != HI_SUCCESS)
        return s32Ret;

    if (pstStream == HI_NULL || pstStream->pStream == HI_NULL)
        return HI_ERR_AENC_NULL_PTR;

    pstChn = &g_stAenc[AeChn];

    pthread_mutex_lock(&pstChn->mutex);

    if (pstChn->u32Created != 1 || pstChn->u32StreamReady != 1) {
        pthread_mutex_unlock(&pstChn->mutex);
        return HI_ERR_AENC_UNEXIST;
    }

    /* Verify stream pointer matches expected offset */
    s32Ret = ioctl(s_s32Aencfd[AeChn], IOC_AENC_RELEASE_STREAM, pstStream);
    if (s32Ret != HI_SUCCESS) {
        pthread_mutex_unlock(&pstChn->mutex);
        return s32Ret;
    }

    /* Advance kernel read pointer */
    {
        HI_U32 *pu32ReadPtr = (HI_U32 *)((HI_U8 *)pstChn->pu8StrmVirtAddr +
            pstChn->u32StrmBufLen - 8);
        HI_U32 u32ReadOff = *pu32ReadPtr + pstChn->u32StrmPackLen;
        HI_U32 u32BufCapacity = pstChn->u32StrmBufLen - 16;
        if (u32ReadOff >= u32BufCapacity)
            u32ReadOff -= u32BufCapacity;
        *pu32ReadPtr = u32ReadOff;
    }

    pthread_mutex_unlock(&pstChn->mutex);
    return HI_SUCCESS;
}

HI_S32 HI_MPI_AENC_GetFd(AENC_CHN AeChn)
{
    HI_S32 s32Ret;

    if (AeChn >= AENC_MAX_CHN_NUM)
        return HI_ERR_AENC_INVALID_CHNID;

    s32Ret = AencCheckOpen(AeChn);
    if (s32Ret != HI_SUCCESS)
        return s32Ret;

    return s_s32Aencfd[AeChn];
}

HI_S32 HI_MPI_AENC_GetStreamBufInfo(AENC_CHN AeChn, HI_U64 *pu64PhysAddr, HI_U32 *pu32Size)
{
    AENC_CHN_CTX_S *pstChn;

    if (AeChn >= AENC_MAX_CHN_NUM)
        return HI_ERR_AENC_INVALID_CHNID;

    if (pu64PhysAddr == HI_NULL || pu32Size == HI_NULL)
        return HI_ERR_AENC_NULL_PTR;

    pstChn = &g_stAenc[AeChn];

    pthread_mutex_lock(&pstChn->mutex);

    if (pstChn->u32Created != 1) {
        HI_TRACE_AENC(HI_DBG_ERR, "chn%d is not created!\n", AeChn);
        pthread_mutex_unlock(&pstChn->mutex);
        return HI_ERR_AENC_UNEXIST;
    }

    if (pstChn->u32StreamReady != 1) {
        HI_TRACE_AENC(HI_DBG_ERR, "chn%d stream buf is not ready!\n", AeChn);
        pthread_mutex_unlock(&pstChn->mutex);
        return HI_ERR_AENC_NOT_PERM;
    }

    *pu64PhysAddr = pstChn->u64StrmPhyAddr;
    *pu32Size = pstChn->u32StrmBufLen;

    pthread_mutex_unlock(&pstChn->mutex);
    return HI_SUCCESS;
}

HI_S32 HI_MPI_AENC_SetMute(AENC_CHN AeChn, HI_BOOL bEnable)
{
    HI_S32 s32Ret;

    if (AeChn >= AENC_MAX_CHN_NUM)
        return HI_ERR_AENC_INVALID_CHNID;

    s32Ret = AencCheckOpen(AeChn);
    if (s32Ret != HI_SUCCESS)
        return s32Ret;

    return ioctl(s_s32Aencfd[AeChn], IOC_AENC_SET_MUTE, &bEnable);
}

HI_S32 HI_MPI_AENC_GetMute(AENC_CHN AeChn, HI_BOOL *pbEnable)
{
    HI_S32 s32Ret;

    if (AeChn >= AENC_MAX_CHN_NUM)
        return HI_ERR_AENC_INVALID_CHNID;

    if (pbEnable == HI_NULL)
        return HI_ERR_AENC_NULL_PTR;

    s32Ret = AencCheckOpen(AeChn);
    if (s32Ret != HI_SUCCESS)
        return s32Ret;

    return ioctl(s_s32Aencfd[AeChn], IOC_AENC_GET_MUTE, pbEnable);
}

/* V2.0.2.1 lowercase aliases */
HI_S32 mpi_aenc_init(HI_VOID) { return MPI_AENC_Init(); }
HI_S32 mpi_aenc_exit(HI_VOID) { MPI_AENC_Exit(); return 0; }
HI_S32 mpi_aenc_create_chn(AENC_CHN c, const AENC_CHN_ATTR_S *a) { return HI_MPI_AENC_CreateChn(c, a); }
HI_S32 mpi_aenc_destroy_chn(AENC_CHN c) { return HI_MPI_AENC_DestroyChn(c); }
HI_S32 mpi_aenc_chn_get_frm_proc(AENC_CHN c) { return 0; }
HI_S32 hi_mpi_aenc_voice_init(HI_VOID) { HI_MPI_AENC_VoiceInit(); return 0; }
