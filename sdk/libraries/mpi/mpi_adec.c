/**
 * Reverse Engineered by TekuConcept on April 26, 2021
 *
 * HI_MPI_ADEC - Audio Decoder MPI implementation
 * Reconstructed from mpi_adec.S (4,745 lines ARM assembly)
 */

#include "re_mpi_adec.h"
#include "re_mpi_bind.h"
#include "hi_comm_aio.h"
#include "hi_comm_adec.h"
#include "mpi_audio.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/prctl.h>
#include <sys/ioctl.h>

extern HI_S32 memcpy_s(void *dest, size_t destMax, const void *src, size_t count);
extern HI_S32 memset_s(void *dest, size_t destMax, int c, size_t count);
extern HI_VOID HI_MPI_ADEC_VoiceInit(HI_VOID);
extern HI_S32 mpi_sys_bind_register_sender(SYS_BIND_SENDER_INFO_S *pstBindInfo);
extern HI_S32 mpi_sys_bind_un_register_sender(MOD_ID_E ModId);
extern HI_S32 mpi_sys_bind_send_data(MOD_ID_E mod_id, HI_S32 dev_id, HI_S32 chn_id,
    HI_U32 flag, MPP_DATA_TYPE_E data_type, HI_VOID *pvData);
extern HI_S32 HI_MPI_SYS_GetBindbySrc(const MPP_CHN_S *pstSrcChn, MPP_BIND_DEST_S *pstBindDest);

/* Global state */
static ADEC_CHN_CTX_S          g_stAdec[ADEC_MAX_CHN_NUM];
static pthread_mutex_t         s_AdecFdmutex;
static pthread_mutex_t         s_Adecmutex;
static HI_BOOL                 s_bAdecInit = HI_FALSE;
static ADEC_DECODER_REGISTRY_S s_stDecoderCtx;
static HI_S32                  s_s32Adecfd[ADEC_MAX_CHN_NUM] = {
    [0 ... 31] = -1
};

/* Forward declarations */
HI_S32 MPI_ADEC_Init(HI_VOID);
static HI_S32 MPI_ADEC_DestroyChn(ADEC_CHN AdChn);

/* ========================================================================== */
/* Internal helpers                                                           */
/* ========================================================================== */

static HI_S32 AdecCheckOpen(ADEC_CHN AdChn)
{
    HI_S32 s32Ret;

    pthread_mutex_lock(&s_AdecFdmutex);

    if (s_s32Adecfd[AdChn] >= 0) {
        pthread_mutex_unlock(&s_AdecFdmutex);
        return HI_SUCCESS;
    }

    s_s32Adecfd[AdChn] = open("/dev/adec", O_RDWR);
    if (s_s32Adecfd[AdChn] < 0) {
        puts("open adec dev fail");
        pthread_mutex_unlock(&s_AdecFdmutex);
        return HI_ERR_ADEC_SYS_NOTREADY;
    }

    s32Ret = ioctl(s_s32Adecfd[AdChn], IOC_ADEC_INIT_CHN, &AdChn);
    if (s32Ret != HI_SUCCESS) {
        close(s_s32Adecfd[AdChn]);
        s_s32Adecfd[AdChn] = -1;
        pthread_mutex_unlock(&s_AdecFdmutex);
        return HI_ERR_ADEC_NOT_PERM;
    }

    pthread_mutex_unlock(&s_AdecFdmutex);
    return HI_SUCCESS;
}

static HI_S32 AdecCheckAttr(const ADEC_CHN_ATTR_S *pstAttr)
{
    if (pstAttr->pValue == HI_NULL)
        return HI_ERR_ADEC_NULL_PTR;

    if ((HI_U32)pstAttr->enType >= 0x3F0) {
        HI_TRACE_ADEC(HI_DBG_ERR, "invalid param: payload type %d\n", pstAttr->enType);
        return HI_ERR_ADEC_ILLEGAL_PARAM;
    }

    if (pstAttr->u32BufSize < 2 || pstAttr->u32BufSize > 300) {
        HI_TRACE_ADEC(HI_DBG_ERR, "invalid param: u32BufSize %d\n", pstAttr->u32BufSize);
        return HI_ERR_ADEC_ILLEGAL_PARAM;
    }

    if (pstAttr->enMode > ADEC_MODE_STREAM) {
        HI_TRACE_ADEC(HI_DBG_ERR, "invalid param: enMode %d\n", pstAttr->enMode);
        return HI_ERR_ADEC_ILLEGAL_PARAM;
    }

    if (pstAttr->enMode == ADEC_MODE_PACK) {
        /* Pack mode: requires G726 or ADPCM */
        if (pstAttr->enType != PT_G726 && pstAttr->enType != PT_ADPCMA) {
            HI_TRACE_ADEC(HI_DBG_ERR, "invalid param: payload type %d\n", pstAttr->enType);
            return HI_ERR_ADEC_ILLEGAL_PARAM;
        }
    } else {
        /* Stream mode: requires AAC */
        if (pstAttr->enType != PT_AAC) {
            HI_TRACE_ADEC(HI_DBG_ERR, "invalid param: payload type %d\n", pstAttr->enType);
            return HI_ERR_ADEC_ILLEGAL_PARAM;
        }
    }

    return HI_SUCCESS;
}

HI_S32 MPI_ADEC_Init(HI_VOID)
{
    HI_S32 s32Ret;
    HI_U32 i;
    SYS_BIND_SENDER_INFO_S stSenderInfo;

    if (s_bAdecInit == HI_TRUE)
        return HI_SUCCESS;

    /* Register as bind sender: mod_id=HI_ID_ADEC(24), max_dev=1, max_chn=32 */
    stSenderInfo.mod_id = HI_ID_ADEC;
    stSenderInfo.max_dev_cnt = 1;
    stSenderInfo.max_chn_cnt = 32;
    stSenderInfo.give_bind_call_back = HI_NULL;

    s32Ret = mpi_sys_bind_register_sender(&stSenderInfo);
    if (s32Ret != HI_SUCCESS)
        return HI_ERR_ADEC_NOT_PERM;

    /* Initialize per-channel mutexes */
    for (i = 0; i < ADEC_MAX_CHN_NUM; i++) {
        s32Ret = pthread_mutex_init(&g_stAdec[i].mutex, NULL);
        if (s32Ret != 0) {
            /* Destroy already-created mutexes in reverse */
            while (i > 0) {
                i--;
                pthread_mutex_destroy(&g_stAdec[i].mutex);
            }
            return HI_ERR_ADEC_NOMEM;
        }
    }

    /* Initialize decoder registry */
    memset_s(&s_stDecoderCtx, sizeof(s_stDecoderCtx), 0, sizeof(s_stDecoderCtx));
    s32Ret = pthread_mutex_init(&s_stDecoderCtx.mutex, NULL);
    if (s32Ret != 0) {
        for (i = 0; i < ADEC_MAX_CHN_NUM; i++)
            pthread_mutex_destroy(&g_stAdec[i].mutex);
        return HI_ERR_ADEC_NOMEM;
    }

    for (i = 0; i < ADEC_MAX_DECODER_NUM; i++)
        s_stDecoderCtx.entries[i].s32Handle = -1;

    HI_MPI_ADEC_VoiceInit();

    s_bAdecInit = HI_TRUE;
    return HI_SUCCESS;
}

static HI_VOID MPI_ADEC_Exit(HI_VOID)
{
    HI_U32 i;

    if (!s_bAdecInit)
        return;

    pthread_mutex_lock(&s_Adecmutex);

    for (i = 0; i < ADEC_MAX_CHN_NUM; i++)
        MPI_ADEC_DestroyChn(i);

    pthread_mutex_unlock(&s_Adecmutex);

    for (i = 0; i < ADEC_MAX_CHN_NUM; i++)
        pthread_mutex_destroy(&g_stAdec[i].mutex);

    mpi_sys_bind_un_register_sender(HI_ID_ADEC);

    pthread_mutex_destroy(&s_stDecoderCtx.mutex);
    memset_s(&s_stDecoderCtx, sizeof(s_stDecoderCtx), 0, sizeof(s_stDecoderCtx));

    s_bAdecInit = HI_FALSE;
}

/* ========================================================================== */
/* AF/TST buffer linked list helpers                                          */
/* ========================================================================== */

static HI_S32 AdecInitAfBuf(ADEC_CHN_CTX_S *pstChn, HI_U32 u32BufSize)
{
    HI_U32 i;
    ADEC_AF_META_S *pstMeta;
    ADEC_AF_NODE_S *pstNode;
    HI_U8 *pu8FrameData;

    if (pstChn->pAfBuf == HI_NULL)
        return HI_FAILURE;

    pstMeta = (ADEC_AF_META_S *)((HI_U8 *)pstChn->pAfBuf + ADEC_AF_META_OFFSET);

    /* Allocate frame data buffer */
    pu8FrameData = (HI_U8 *)malloc(u32BufSize * ADEC_FRAME_DATA_SIZE);
    if (pu8FrameData == HI_NULL)
        return HI_FAILURE;

    pstMeta->pu8FrameDataBuf = pu8FrameData;
    pstMeta->u32TotalCount = u32BufSize;
    pstMeta->u32FreeCount = u32BufSize;
    pstMeta->u32BusyCount = 0;

    /* Initialize free list sentinel */
    memset_s(&pstMeta->stFreeHead, sizeof(ADEC_AF_NODE_S), 0, sizeof(ADEC_AF_NODE_S));
    memset_s(&pstMeta->stBusyHead, sizeof(ADEC_AF_NODE_S), 0, sizeof(ADEC_AF_NODE_S));

    /* Build free list from AF buffer nodes */
    pstNode = (ADEC_AF_NODE_S *)pstChn->pAfBuf;
    for (i = 0; i < u32BufSize; i++) {
        memset_s(pstNode, sizeof(ADEC_AF_NODE_S), 0, sizeof(ADEC_AF_NODE_S));
        pstNode->pu8FrameData = pu8FrameData + (i * ADEC_FRAME_DATA_SIZE);
        pstNode->pu8FrameData2 = HI_NULL;
        pstNode->field_44 = i;

        /* Insert into free list (append to tail) */
        if (i == 0) {
            pstMeta->stFreeHead.pNext = pstNode;
            pstNode->pPrev = &pstMeta->stFreeHead;
        } else {
            ADEC_AF_NODE_S *pstPrev = (ADEC_AF_NODE_S *)((HI_U8 *)pstChn->pAfBuf +
                (i - 1) * ADEC_AF_NODE_SIZE);
            pstPrev->pNext = pstNode;
            pstNode->pPrev = pstPrev;
        }
        pstNode->pNext = HI_NULL;

        pstNode = (ADEC_AF_NODE_S *)((HI_U8 *)pstNode + ADEC_AF_NODE_SIZE);
    }

    return HI_SUCCESS;
}

static HI_S32 AdecInitTstBuf(ADEC_CHN_CTX_S *pstChn)
{
    HI_U32 i;
    ADEC_TST_META_S *pstMeta;
    ADEC_TST_NODE_S *pstNode;

    if (pstChn->pu8TstBuf == HI_NULL)
        return HI_FAILURE;

    pstMeta = (ADEC_TST_META_S *)((HI_U8 *)pstChn->pu8TstBuf + ADEC_TST_META_OFFSET);

    pstMeta->u32TotalCount = ADEC_TST_MAX_COUNT;
    pstMeta->u32FreeCount = ADEC_TST_MAX_COUNT;
    pstMeta->u32BusyCount = 0;

    memset_s(&pstMeta->stFreeHead, sizeof(ADEC_TST_NODE_S), 0, sizeof(ADEC_TST_NODE_S));
    memset_s(&pstMeta->stBusyHead, sizeof(ADEC_TST_NODE_S), 0, sizeof(ADEC_TST_NODE_S));

    /* Build free list from TST buffer nodes */
    pstNode = (ADEC_TST_NODE_S *)pstChn->pu8TstBuf;
    for (i = 0; i < ADEC_TST_MAX_COUNT; i++) {
        memset_s(pstNode, sizeof(ADEC_TST_NODE_S), 0, sizeof(ADEC_TST_NODE_S));

        if (i == 0) {
            pstMeta->stFreeHead.pNext = pstNode;
            pstNode->pPrev = &pstMeta->stFreeHead;
        } else {
            ADEC_TST_NODE_S *pstPrev = (ADEC_TST_NODE_S *)((HI_U8 *)pstChn->pu8TstBuf +
                (i - 1) * ADEC_TST_NODE_SIZE);
            pstPrev->pNext = pstNode;
            pstNode->pPrev = pstPrev;
        }
        pstNode->pNext = HI_NULL;

        pstNode = (ADEC_TST_NODE_S *)((HI_U8 *)pstNode + ADEC_TST_NODE_SIZE);
    }

    return HI_SUCCESS;
}

/* ========================================================================== */
/* Internal MPI_ADEC_SendEndOfStream                                          */
/* ========================================================================== */

static HI_S32 MPI_ADEC_SendEndOfStream(ADEC_CHN AdChn)
{
    ADEC_CHN_CTX_S *pstChn;
    HI_S32 s32DecoderIdx;

    if (AdChn >= ADEC_MAX_CHN_NUM)
        return HI_ERR_ADEC_INVALID_CHNID;

    pstChn = &g_stAdec[AdChn];

    if (pstChn->bCreated == 0)
        return HI_ERR_ADEC_UNEXIST;

    if (pstChn->u32DecMode == 0)
        return HI_ERR_ADEC_NOT_PERM;

    s32DecoderIdx = pstChn->s32DecoderIdx;
    if (s32DecoderIdx == -1) {
        printf("\nASSERT at:\n  >Function : %s\n  >Line No. : %d\n  >Condition: %s\n",
            "MPI_ADEC_SendEndOfStream", 400,
            "HI_INVALID_HANDLE != pstAdecChn->s32Handle");
        _exit(s32DecoderIdx);
    }

    if (s_stDecoderCtx.entries[s32DecoderIdx].s32Handle == -1) {
        HI_TRACE_ADEC(HI_DBG_ERR, "The decoder has been unregistered!\n");
        return HI_ERR_ADEC_DECODER_ERR;
    }

    /* Call pfnResetDecoder if available */
    if (s_stDecoderCtx.entries[s32DecoderIdx].pfnResetDecoder != HI_NULL) {
        HI_S32 s32Ret;
        s32Ret = s_stDecoderCtx.entries[s32DecoderIdx].pfnResetDecoder(pstChn->pDecoder);
        if (s32Ret != HI_SUCCESS) {
            HI_TRACE_ADEC(HI_DBG_ERR, "Reset Decoder failed!\n");
            return s32Ret;
        }
    }

    /* Reset stream buffer state */
    pstChn->u32StrmUsedLen = 0;
    pstChn->u32StrmReadPos = (HI_U32)(HI_UL)pstChn->pu8StrmBuf;
    pstChn->u32EndOfStream = 0;

    return HI_SUCCESS;
}

/* ========================================================================== */
/* Thread functions                                                           */
/* ========================================================================== */

static HI_VOID *ADEC_DecProc(HI_VOID *pArg)
{
    ADEC_THREAD_ARG_S *pstThreadArg = (ADEC_THREAD_ARG_S *)pArg;
    ADEC_CHN AdChn;
    ADEC_CHN_CTX_S *pstChn;
    ADEC_AF_META_S *pstAfMeta;
    HI_S32 s32DecoderIdx;
    HI_S32 s32Ret;

    if (pstThreadArg == HI_NULL)
        return HI_NULL;

    AdChn = pstThreadArg->s32AdChn;
    prctl(PR_SET_NAME, "hi_Adec_Dec", 0, 0, 0);

    pstChn = &g_stAdec[AdChn];

    while (pstChn->u32DecMode == 1 && s_bAdecInit) {
        pthread_mutex_lock(&pstChn->mutex);

        if (pstChn->bCreated == 0 || pstChn->pAfBuf == HI_NULL ||
            pstChn->u32StrmUsedLen == 0) {
            pthread_mutex_unlock(&pstChn->mutex);
            usleep(10000);
            continue;
        }

        pstAfMeta = (ADEC_AF_META_S *)((HI_U8 *)pstChn->pAfBuf + ADEC_AF_META_OFFSET);

        /* Check if there's a free AF node */
        if (pstAfMeta->u32FreeCount == 0) {
            pthread_mutex_unlock(&pstChn->mutex);
            usleep(10000);
            continue;
        }

        s32DecoderIdx = pstChn->s32DecoderIdx;
        if (s32DecoderIdx == -1) {
            printf("\nASSERT at:\n  >Function : %s\n  >Line No. : %d\n  >Condition: %s\n",
                "ADEC_DecProc", 0,
                "HI_INVALID_HANDLE != pstMpiAdecCtx->s32Handle");
            _exit(s32DecoderIdx);
        }

        /* Take a node from the free list */
        {
            ADEC_AF_NODE_S *pstNode = pstAfMeta->stFreeHead.pNext;
            HI_U8 *pu8InBuf;
            HI_S32 s32LeftBytes;
            HI_U16 *pu16OutBuf;
            HI_U32 u32OutLen = 0;
            HI_U32 u32Chns = 0;

            if (pstNode == HI_NULL) {
                pthread_mutex_unlock(&pstChn->mutex);
                usleep(10000);
                continue;
            }

            /* Remove from free list */
            pstAfMeta->stFreeHead.pNext = pstNode->pNext;
            if (pstNode->pNext != HI_NULL)
                pstNode->pNext->pPrev = &pstAfMeta->stFreeHead;
            pstNode->pNext = HI_NULL;
            pstNode->pPrev = HI_NULL;
            pstAfMeta->u32FreeCount--;

            /* Set up decode parameters */
            pu8InBuf = (HI_U8 *)(HI_UL)pstChn->u32StrmReadPos;
            s32LeftBytes = (HI_S32)pstChn->u32StrmUsedLen;
            pu16OutBuf = (HI_U16 *)pstNode->pu8FrameData;

            /* Call decoder */
            s32Ret = s_stDecoderCtx.entries[s32DecoderIdx].pfnDecodeFrm(
                pstChn->pDecoder, &pu8InBuf, &s32LeftBytes,
                pu16OutBuf, &u32OutLen, &u32Chns);

            if (s32Ret == HI_SUCCESS && u32OutLen > 0) {
                /* Decode success: add to busy list */
                ADEC_AF_NODE_S *pstBusyTail;

                /* Find busy list tail */
                pstBusyTail = &pstAfMeta->stBusyHead;
                while (pstBusyTail->pNext != HI_NULL)
                    pstBusyTail = pstBusyTail->pNext;

                pstBusyTail->pNext = pstNode;
                pstNode->pPrev = pstBusyTail;
                pstNode->pNext = HI_NULL;
                pstAfMeta->u32BusyCount++;

                /* Update stream position */
                pstChn->u32StrmUsedLen = (HI_U32)s32LeftBytes;
                pstChn->u32StrmReadPos = (HI_U32)(HI_UL)pu8InBuf;

                /* Handle TST timestamps */
                if (pstChn->pu8TstBuf != HI_NULL) {
                    ADEC_TST_META_S *pstTstMeta = (ADEC_TST_META_S *)(
                        (HI_U8 *)pstChn->pu8TstBuf + ADEC_TST_META_OFFSET);
                    if (pstTstMeta->u32BusyCount > 0 &&
                        pstTstMeta->stBusyHead.pNext != HI_NULL) {
                        ADEC_TST_NODE_S *pstTstNode = pstTstMeta->stBusyHead.pNext;
                        pstChn->u64TimeStamp = pstTstNode->u64TimeStamp;

                        /* Remove TST node from busy list, return to free */
                        pstTstMeta->stBusyHead.pNext = pstTstNode->pNext;
                        if (pstTstNode->pNext != HI_NULL)
                            pstTstNode->pNext->pPrev = &pstTstMeta->stBusyHead;
                        pstTstMeta->u32BusyCount--;

                        /* Add to free list head */
                        pstTstNode->pNext = pstTstMeta->stFreeHead.pNext;
                        if (pstTstMeta->stFreeHead.pNext != HI_NULL)
                            pstTstMeta->stFreeHead.pNext->pPrev = pstTstNode;
                        pstTstMeta->stFreeHead.pNext = pstTstNode;
                        pstTstNode->pPrev = &pstTstMeta->stFreeHead;
                        pstTstMeta->u32FreeCount++;
                    }
                }

                /* Signal read semaphore */
                sem_post(&pstChn->semRead);
                pstChn->u32DbgDecSuccCount++;
            } else {
                /* Decode failure: return node to free list */
                pstNode->pNext = pstAfMeta->stFreeHead.pNext;
                if (pstAfMeta->stFreeHead.pNext != HI_NULL)
                    pstAfMeta->stFreeHead.pNext->pPrev = pstNode;
                pstAfMeta->stFreeHead.pNext = pstNode;
                pstNode->pPrev = &pstAfMeta->stFreeHead;
                pstAfMeta->u32FreeCount++;

                if (s32Ret == (HI_S32)HI_ERR_ADEC_BUF_LACK) {
                    /* Not enough data for a full frame */
                    if (pstChn->u32EndOfStream) {
                        MPI_ADEC_SendEndOfStream(AdChn);
                    }
                } else {
                    HI_TRACE_ADEC(HI_DBG_ERR, "s32LeftBytes:%d, maxlen:%d, ret:0x%x\n",
                        s32LeftBytes, ADEC_STRM_BUF_SIZE, s32Ret);
                }

                /* Clamp stream position */
                if (pstChn->u32StrmUsedLen >= ADEC_STRM_BUF_SIZE) {
                    pstChn->u32StrmUsedLen = 0;
                }
            }

            /* Update debug info */
            pstChn->u32DbgEncCount++;
            ioctl(s_s32Adecfd[AdChn], IOC_ADEC_SET_DBG_INFO, &pstChn->u32DbgEncCount);
        }

        pthread_mutex_unlock(&pstChn->mutex);
        usleep(10000);
    }

    return HI_NULL;
}

static HI_VOID *ADEC_SendAoProc(HI_VOID *pArg)
{
    ADEC_THREAD_ARG_S *pstThreadArg = (ADEC_THREAD_ARG_S *)pArg;
    ADEC_CHN AdChn;
    ADEC_CHN_CTX_S *pstChn;
    AUDIO_FRAME_INFO_S stFrmInfo;
    HI_U8 au8FrmData[56];
    MPP_CHN_S stSrcChn;
    MPP_BIND_DEST_S stBindDest;
    HI_S32 s32Ret;

    if (pstThreadArg == HI_NULL)
        return HI_NULL;

    AdChn = pstThreadArg->s32AdChn;
    prctl(PR_SET_NAME, "hi_Adec_Send", 0, 0, 0);

    pstChn = &g_stAdec[AdChn];

    stSrcChn.enModId = HI_ID_ADEC;
    stSrcChn.s32DevId = 0;
    stSrcChn.s32ChnId = AdChn;

    while (pstChn->u32SendAoRunning == 1) {
        s32Ret = HI_MPI_SYS_GetBindbySrc(&stSrcChn, &stBindDest);
        if (s32Ret != HI_SUCCESS) {
            usleep(10000);
            continue;
        }

        if (pstChn->u32RetryFlag) {
            /* Retry sending last frame */
            goto send_data;
        }

        /* Get a decoded frame */
        s32Ret = HI_MPI_ADEC_GetFrame(AdChn, &stFrmInfo, HI_FALSE);
        if (s32Ret != HI_SUCCESS) {
            if (s_bAdecInit)
                usleep(10000);
            else
                usleep(1000);
            continue;
        }

        memcpy_s(au8FrmData, sizeof(au8FrmData), &stFrmInfo, sizeof(AUDIO_FRAME_INFO_S));

send_data:
        /* Send to bound AO */
        s32Ret = mpi_sys_bind_send_data(HI_ID_ADEC, 0, AdChn, 0,
            MPP_DATA_AUDIO_FRAME, au8FrmData);

        if (s32Ret == (HI_S32)0xa0168009) {
            /* AO busy, retry next iteration */
            pstChn->u32RetryFlag = 1;
        } else {
            pstChn->u32RetryFlag = 0;
            HI_MPI_ADEC_ReleaseFrame(AdChn, &stFrmInfo);
        }

        usleep(1000);
    }

    /* Clean up on exit: release any pending retry frame */
    if (pstChn->u32RetryFlag) {
        stFrmInfo.pstFrame = (AUDIO_FRAME_S *)au8FrmData;
        HI_MPI_ADEC_ReleaseFrame(AdChn, &stFrmInfo);
        pstChn->u32RetryFlag = 0;
    }

    return HI_NULL;
}

/* ========================================================================== */
/* Internal channel create/destroy                                            */
/* ========================================================================== */

static HI_S32 MPI_ADEC_CreateChn(ADEC_CHN AdChn, const ADEC_CHN_ATTR_S *pstAttr)
{
    HI_S32 s32Ret;
    HI_U32 i;
    ADEC_CHN_CTX_S *pstChn = &g_stAdec[AdChn];
    ADEC_THREAD_ARG_S *pstSendArg = HI_NULL;
    ADEC_THREAD_ARG_S *pstDecArg = HI_NULL;
    pthread_t tid;

    pthread_mutex_lock(&pstChn->mutex);

    if (pstChn->bCreated == 1) {
        HI_TRACE_ADEC(HI_DBG_ERR, "adec chn %d has been created\n", AdChn);
        pthread_mutex_unlock(&pstChn->mutex);
        return HI_ERR_ADEC_EXIST;
    }

    if (pstChn->bDestroying == 1) {
        HI_TRACE_ADEC(HI_DBG_ERR, "adec chn %d is being destroyed\n", AdChn);
        pthread_mutex_unlock(&pstChn->mutex);
        return HI_ERR_ADEC_BUF_FULL;
    }

    s32Ret = AdecCheckAttr(pstAttr);
    if (s32Ret != HI_SUCCESS) {
        pthread_mutex_unlock(&pstChn->mutex);
        return s32Ret;
    }

    /* Zero out channel fields */
    pstChn->pAfBuf = HI_NULL;
    pstChn->pDecoder = HI_NULL;
    pstChn->s32DecoderIdx = -1;
    pstChn->u64TimeStamp = 0;
    pstChn->u32StrmUsedLen = 0;
    pstChn->u32StrmReadPos = 0;
    pstChn->pu8StrmBuf = HI_NULL;
    pstChn->pu8TstBuf = HI_NULL;
    pstChn->u32SendAoRunning = 0;
    pstChn->u32RetryFlag = 0;
    pstChn->s32DecThread = -1;
    pstChn->s32SendAoThread = -1;
    pstChn->field_B0 = 0;
    pstChn->field_B4 = 0;
    pstChn->field_B8 = -1;
    pstChn->field_BC = -1;
    pstChn->field_C0 = 0;
    pstChn->field_C8 = 0;
    pstChn->field_CC = -1;
    pstChn->field_D0 = 0;
    pstChn->field_D4 = 0;
    pstChn->field_D8 = 0;
    pstChn->field_DC = 0;
    pstChn->field_E0 = 0;
    pstChn->u32EndOfStream = 0;
    pstChn->u32DecodedCount = 0;
    pstChn->u32DbgEncCount = 0;
    pstChn->u32DbgDecSuccCount = 0;
    pstChn->u32DbgGetFrmCount = 0;
    pstChn->u32DbgRelFrmCount = 0;
    pstChn->u32G726Bps = 8;
    pstChn->u32AdpcmType = 3;
    pstChn->u32DecReadyFlag = 0;
    pstChn->u32RefCount = 0;

    /* Copy channel attributes */
    memcpy_s(&pstChn->stAttr, sizeof(ADEC_CHN_ATTR_S), pstAttr, sizeof(ADEC_CHN_ATTR_S));

    /* Find matching decoder in registry */
    {
        HI_S32 s32DecIdx = -1;
        pthread_mutex_lock(&s_stDecoderCtx.mutex);
        for (i = 0; i < ADEC_MAX_DECODER_NUM; i++) {
            if (s_stDecoderCtx.entries[i].s32Handle == (HI_S32)pstAttr->enType) {
                s32DecIdx = i;
                break;
            }
        }
        pthread_mutex_unlock(&s_stDecoderCtx.mutex);

        if (s32DecIdx == -1) {
            HI_TRACE_ADEC(HI_DBG_ERR,
                "the decoder maybe has not been registered, AdChn %d.\n", AdChn);
            pthread_mutex_unlock(&pstChn->mutex);
            return HI_ERR_ADEC_NOT_SUPPORT;
        }
        pstChn->s32DecoderIdx = s32DecIdx;
    }

    /* Open decoder */
    s32Ret = s_stDecoderCtx.entries[pstChn->s32DecoderIdx].pfnOpenDecoder(
        pstAttr->pValue, &pstChn->pDecoder);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_ADEC(HI_DBG_ERR, "open the decoder failed, AdChn %d.\n", AdChn);
        pthread_mutex_unlock(&pstChn->mutex);
        return s32Ret;
    }

    /* Allocate AF buffer */
    pstChn->pAfBuf = malloc(ADEC_AF_BUF_SIZE);
    if (pstChn->pAfBuf == HI_NULL) {
        HI_TRACE_ADEC(HI_DBG_ERR, "adec chn %d malloc AF buffer failed!\n", AdChn);
        goto err_close_decoder;
    }
    memset_s(pstChn->pAfBuf, ADEC_AF_BUF_SIZE, 0, ADEC_AF_BUF_SIZE);

    /* Initialize AF buffer linked lists */
    s32Ret = AdecInitAfBuf(pstChn, pstAttr->u32BufSize);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_ADEC(HI_DBG_ERR, "adec chn %d init AF buffer failed!\n", AdChn);
        goto err_free_af_buf;
    }

    /* For stream mode: allocate stream and TST buffers */
    if (pstAttr->enMode == ADEC_MODE_STREAM) {
        pstChn->pu8StrmBuf = (HI_U8 *)malloc(ADEC_STRM_BUF_SIZE);
        if (pstChn->pu8StrmBuf == HI_NULL) {
            HI_TRACE_ADEC(HI_DBG_ERR, "adec chn %d malloc buffer failed!\n", AdChn);
            goto err_free_frame_data;
        }

        pstChn->pu8TstBuf = (HI_U8 *)malloc(ADEC_TST_BUF_SIZE);
        if (pstChn->pu8TstBuf == HI_NULL) {
            HI_TRACE_ADEC(HI_DBG_ERR, "adec chn %d malloc TST buffer failed!\n", AdChn);
            goto err_free_strm_buf;
        }
        memset_s(pstChn->pu8TstBuf, ADEC_TST_BUF_SIZE, 0, ADEC_TST_BUF_SIZE);

        s32Ret = AdecInitTstBuf(pstChn);
        if (s32Ret != HI_SUCCESS) {
            HI_TRACE_ADEC(HI_DBG_ERR, "adec chn %d init TST buffer failed!\n", AdChn);
            goto err_free_tst_buf;
        }

        pstChn->u32StrmReadPos = (HI_U32)(HI_UL)pstChn->pu8StrmBuf;
        pstChn->u32StrmUsedLen = 0;
    }

    /* Initialize semaphores */
    sem_init(&pstChn->semRead, 0, 0);
    sem_init(&pstChn->semWrite, 0, pstAttr->u32BufSize - 1);

    /* Set decode mode */
    pstChn->u32DecMode = (pstAttr->enMode == ADEC_MODE_STREAM) ? 1 : 0;
    pstChn->bCreated = 1;
    pstChn->u32SendAoRunning = 1;
    pstChn->s32AdChn = AdChn;

    /* Create SendAo thread */
    pstSendArg = (ADEC_THREAD_ARG_S *)malloc(sizeof(ADEC_THREAD_ARG_S));
    if (pstSendArg != HI_NULL) {
        memset_s(pstSendArg, sizeof(ADEC_THREAD_ARG_S), 0, sizeof(ADEC_THREAD_ARG_S));
        pstSendArg->s32AdChn = AdChn;
    }

    if (pthread_create(&tid, HI_NULL, ADEC_SendAoProc, pstSendArg) != 0) {
        HI_TRACE_ADEC(HI_DBG_ERR,
            "adec chn %d create send ao thread failed!\n", AdChn);
        goto err_cleanup_sems;
    }
    pstChn->s32SendAoThread = (HI_S32)tid;

    /* Create DecProc thread for stream mode */
    if (pstAttr->enMode == ADEC_MODE_STREAM) {
        pstDecArg = (ADEC_THREAD_ARG_S *)malloc(sizeof(ADEC_THREAD_ARG_S));
        if (pstDecArg != HI_NULL) {
            memset_s(pstDecArg, sizeof(ADEC_THREAD_ARG_S), 0, sizeof(ADEC_THREAD_ARG_S));
            pstDecArg->s32AdChn = AdChn;
        }

        if (pthread_create(&tid, HI_NULL, ADEC_DecProc, pstDecArg) != 0) {
            HI_TRACE_ADEC(HI_DBG_ERR,
                "adec chn %d create decoder stream thread failed!\n", AdChn);
            /* Stop SendAo thread */
            pstChn->u32SendAoRunning = 0;
            pthread_mutex_unlock(&pstChn->mutex);
            pthread_join((pthread_t)(HI_UL)pstChn->s32SendAoThread, HI_NULL);
            pthread_mutex_lock(&pstChn->mutex);
            goto err_cleanup_sems;
        }
        pstChn->s32DecThread = (HI_S32)tid;
    }

    /* Set channel attributes in kernel */
    ioctl(s_s32Adecfd[AdChn], IOC_ADEC_SET_ATTR, &pstChn->stAttr);

    pthread_mutex_unlock(&pstChn->mutex);
    return HI_SUCCESS;

err_cleanup_sems:
    sem_destroy(&pstChn->semRead);
    sem_destroy(&pstChn->semWrite);
    pstChn->bCreated = 0;
    pstChn->u32SendAoRunning = 0;
    if (pstSendArg) free(pstSendArg);
    if (pstDecArg) free(pstDecArg);
err_free_tst_buf:
    if (pstChn->pu8TstBuf) {
        free(pstChn->pu8TstBuf);
        pstChn->pu8TstBuf = HI_NULL;
    }
err_free_strm_buf:
    if (pstChn->pu8StrmBuf) {
        free(pstChn->pu8StrmBuf);
        pstChn->pu8StrmBuf = HI_NULL;
    }
err_free_frame_data:
    {
        ADEC_AF_META_S *pstMeta = (ADEC_AF_META_S *)(
            (HI_U8 *)pstChn->pAfBuf + ADEC_AF_META_OFFSET);
        if (pstMeta->pu8FrameDataBuf) {
            free(pstMeta->pu8FrameDataBuf);
            pstMeta->pu8FrameDataBuf = HI_NULL;
        }
    }
err_free_af_buf:
    free(pstChn->pAfBuf);
    pstChn->pAfBuf = HI_NULL;
err_close_decoder:
    if (pstChn->pDecoder && pstChn->s32DecoderIdx >= 0) {
        s_stDecoderCtx.entries[pstChn->s32DecoderIdx].pfnCloseDecoder(pstChn->pDecoder);
        pstChn->pDecoder = HI_NULL;
    }
    pthread_mutex_unlock(&pstChn->mutex);
    return s32Ret;
}

static HI_S32 MPI_ADEC_DestroyChn(ADEC_CHN AdChn)
{
    ADEC_CHN_CTX_S *pstChn = &g_stAdec[AdChn];
    HI_S32 s32DecoderIdx;
    HI_S32 s32Ret;

    pthread_mutex_lock(&pstChn->mutex);

    if (pstChn->bCreated != 1) {
        pthread_mutex_unlock(&pstChn->mutex);
        return HI_SUCCESS;
    }

    s32DecoderIdx = pstChn->s32DecoderIdx;
    if (s32DecoderIdx == -1) {
        printf("\nASSERT at:\n  >Function : %s\n  >Line No. : %d\n  >Condition: %s\n",
            "MPI_ADEC_DestroyChn", 0,
            "HI_INVALID_HANDLE != pstMpiAdecCtx->s32Handle");
        _exit(s32DecoderIdx);
    }

    if (s_stDecoderCtx.entries[s32DecoderIdx].s32Handle == -1) {
        printf("\nASSERT at:\n  >Function : %s\n  >Line No. : %d\n  >Condition: %s\n",
            "MPI_ADEC_DestroyChn", 0,
            "HI_INVALID_HANDLE != pstMpiAdecCtx->s32Handle");
        _exit(s32DecoderIdx);
    }

    pstChn->bDestroying = 1;
    pstChn->bCreated = 0;

    /* Post semaphores to unblock any waiting threads */
    {
        int val;
        sem_getvalue(&pstChn->semRead, &val);
        if (val == 0)
            sem_post(&pstChn->semRead);
        sem_getvalue(&pstChn->semWrite, &val);
        if (val == 0)
            sem_post(&pstChn->semWrite);
    }

    /* Close decoder */
    s_stDecoderCtx.entries[s32DecoderIdx].pfnCloseDecoder(pstChn->pDecoder);
    pstChn->pDecoder = HI_NULL;

    /* Stop and join SendAo thread */
    if (pstChn->u32SendAoRunning) {
        pstChn->u32SendAoRunning = 0;
        pthread_mutex_unlock(&pstChn->mutex);
        pthread_join((pthread_t)(HI_UL)pstChn->s32SendAoThread, HI_NULL);
        pthread_mutex_lock(&pstChn->mutex);
    }

    /* Stop and join Dec thread (stream mode) */
    if (pstChn->u32DecMode == 1) {
        pstChn->u32DecMode = 0;
        pthread_mutex_unlock(&pstChn->mutex);
        pthread_join((pthread_t)(HI_UL)pstChn->s32DecThread, HI_NULL);
        pthread_mutex_lock(&pstChn->mutex);
    }

    /* Free AF frame data buffer and AF buffer */
    if (pstChn->pAfBuf != HI_NULL) {
        ADEC_AF_META_S *pstMeta = (ADEC_AF_META_S *)(
            (HI_U8 *)pstChn->pAfBuf + ADEC_AF_META_OFFSET);
        if (pstMeta->pu8FrameDataBuf) {
            free(pstMeta->pu8FrameDataBuf);
            pstMeta->pu8FrameDataBuf = HI_NULL;
        }
        free(pstChn->pAfBuf);
        pstChn->pAfBuf = HI_NULL;
    }

    /* Free stream buffer */
    if (pstChn->pu8StrmBuf != HI_NULL) {
        free(pstChn->pu8StrmBuf);
        pstChn->pu8StrmBuf = HI_NULL;
    }

    /* Free TST buffer */
    if (pstChn->pu8TstBuf != HI_NULL) {
        free(pstChn->pu8TstBuf);
        pstChn->pu8TstBuf = HI_NULL;
    }

    /* Destroy semaphores */
    s32Ret = sem_destroy(&pstChn->semRead);
    if (s32Ret != 0)
        HI_TRACE_ADEC(HI_DBG_ERR, "destroy semRead err %d!\n", s32Ret);

    s32Ret = sem_destroy(&pstChn->semWrite);
    if (s32Ret != 0)
        HI_TRACE_ADEC(HI_DBG_ERR, "destroy semWrite err %d!\n", s32Ret);

    pstChn->bDestroying = 0;

    /* Notify kernel */
    ioctl(s_s32Adecfd[AdChn], IOC_ADEC_DESTROY_CHN);

    pthread_mutex_unlock(&pstChn->mutex);
    return HI_SUCCESS;
}

/* ========================================================================== */
/* Clear channel buffer                                                       */
/* ========================================================================== */

static HI_S32 MPI_ADEC_ClearChnBuf(ADEC_CHN AdChn)
{
    ADEC_CHN_CTX_S *pstChn = &g_stAdec[AdChn];
    ADEC_AF_META_S *pstAfMeta;
    int val;

    pthread_mutex_lock(&pstChn->mutex);

    if (pstChn->bCreated != 1) {
        pthread_mutex_unlock(&pstChn->mutex);
        return HI_ERR_ADEC_UNEXIST;
    }

    /* Reset stream buffer for stream mode */
    if (pstChn->u32DecMode == 1) {
        pstChn->u32StrmUsedLen = 0;
        pstChn->u32StrmReadPos = (HI_U32)(HI_UL)pstChn->pu8StrmBuf;
    }

    pstChn->u32RefCount++;

    /* Move all busy AF nodes back to free */
    pstAfMeta = (ADEC_AF_META_S *)((HI_U8 *)pstChn->pAfBuf + ADEC_AF_META_OFFSET);
    {
        ADEC_AF_NODE_S *pstNode = pstAfMeta->stBusyHead.pNext;
        while (pstNode != HI_NULL) {
            ADEC_AF_NODE_S *pstNext = pstNode->pNext;

            /* Return to free list head */
            pstNode->pNext = pstAfMeta->stFreeHead.pNext;
            if (pstAfMeta->stFreeHead.pNext != HI_NULL)
                pstAfMeta->stFreeHead.pNext->pPrev = pstNode;
            pstAfMeta->stFreeHead.pNext = pstNode;
            pstNode->pPrev = &pstAfMeta->stFreeHead;

            pstNode = pstNext;
        }
        pstAfMeta->stBusyHead.pNext = HI_NULL;
        pstAfMeta->u32FreeCount = pstAfMeta->u32TotalCount;
        pstAfMeta->u32BusyCount = 0;
    }

    /* Reset TST nodes for stream mode */
    if (pstChn->u32DecMode == 1 && pstChn->pu8TstBuf != HI_NULL) {
        ADEC_TST_META_S *pstTstMeta = (ADEC_TST_META_S *)(
            (HI_U8 *)pstChn->pu8TstBuf + ADEC_TST_META_OFFSET);

        ADEC_TST_NODE_S *pstNode = pstTstMeta->stBusyHead.pNext;
        while (pstNode != HI_NULL) {
            ADEC_TST_NODE_S *pstNext = pstNode->pNext;

            pstNode->pNext = pstTstMeta->stFreeHead.pNext;
            if (pstTstMeta->stFreeHead.pNext != HI_NULL)
                pstTstMeta->stFreeHead.pNext->pPrev = pstNode;
            pstTstMeta->stFreeHead.pNext = pstNode;
            pstNode->pPrev = &pstTstMeta->stFreeHead;

            pstNode = pstNext;
        }
        pstTstMeta->stBusyHead.pNext = HI_NULL;
        pstTstMeta->u32FreeCount = pstTstMeta->u32TotalCount;
        pstTstMeta->u32BusyCount = 0;
    }

    /* Drain semRead */
    while (sem_trywait(&pstChn->semRead) == 0)
        ;

    /* Reset semWrite to BufSize - 1 */
    sem_getvalue(&pstChn->semWrite, &val);
    while (val < (HI_S32)(pstChn->stAttr.u32BufSize - 1)) {
        sem_post(&pstChn->semWrite);
        sem_getvalue(&pstChn->semWrite, &val);
    }

    pstChn->u32RefCount--;

    pthread_mutex_unlock(&pstChn->mutex);
    return HI_SUCCESS;
}

/* ========================================================================== */
/* Internal send stream                                                       */
/* ========================================================================== */

static HI_S32 AdecSendStream(ADEC_CHN AdChn, const AUDIO_STREAM_S *pstStream, HI_BOOL bBlock)
{
    ADEC_CHN_CTX_S *pstChn = &g_stAdec[AdChn];
    HI_U32 u32Len;
    HI_U32 u32Offset;

    pthread_mutex_lock(&pstChn->mutex);

    if (pstChn->bCreated != 1) {
        pthread_mutex_unlock(&pstChn->mutex);
        return HI_ERR_ADEC_UNEXIST;
    }

    pstChn->u32RefCount++;
    u32Len = pstStream->u32Len;

    if (u32Len > ADEC_STRM_BUF_SIZE) {
        HI_TRACE_ADEC(HI_DBG_ERR, "u32Len:%d, maxlen:%d\n", u32Len, ADEC_STRM_BUF_SIZE);
        pstChn->u32RefCount--;
        pthread_mutex_unlock(&pstChn->mutex);
        return HI_ERR_ADEC_ILLEGAL_PARAM;
    }

    /* Copy timestamp */
    pstChn->u64TimeStamp = pstStream->u64TimeStamp;

    /* Copy data into stream buffer */
    u32Offset = pstChn->u32StrmUsedLen;
    if (u32Offset + u32Len <= ADEC_STRM_BUF_SIZE) {
        memcpy_s(pstChn->pu8StrmBuf + u32Offset, ADEC_STRM_BUF_SIZE - u32Offset,
            pstStream->pStream, u32Len);
        pstChn->u32StrmUsedLen += u32Len;
    } else {
        /* Buffer full: need to wait for decoder to consume */
        pstChn->u32RefCount--;
        pthread_mutex_unlock(&pstChn->mutex);

        if (!bBlock)
            return HI_ERR_ADEC_BUF_FULL;

        /* Blocking: wait and retry */
        usleep(10000);
        return AdecSendStream(AdChn, pstStream, bBlock);
    }

    /* Handle TST timestamp entry */
    if (pstChn->pu8TstBuf != HI_NULL) {
        ADEC_TST_META_S *pstTstMeta = (ADEC_TST_META_S *)(
            (HI_U8 *)pstChn->pu8TstBuf + ADEC_TST_META_OFFSET);

        if (pstTstMeta->u32FreeCount > 0 && pstTstMeta->stFreeHead.pNext != HI_NULL) {
            ADEC_TST_NODE_S *pstNode = pstTstMeta->stFreeHead.pNext;

            /* Remove from free list */
            pstTstMeta->stFreeHead.pNext = pstNode->pNext;
            if (pstNode->pNext != HI_NULL)
                pstNode->pNext->pPrev = &pstTstMeta->stFreeHead;
            pstTstMeta->u32FreeCount--;

            /* Set timestamp info */
            pstNode->u64TimeStamp = pstStream->u64TimeStamp;
            pstNode->u64Offset = u32Offset;
            pstNode->u32Len = u32Len;

            /* Add to busy list tail */
            {
                ADEC_TST_NODE_S *pstTail = &pstTstMeta->stBusyHead;
                while (pstTail->pNext != HI_NULL)
                    pstTail = pstTail->pNext;
                pstTail->pNext = pstNode;
                pstNode->pPrev = pstTail;
                pstNode->pNext = HI_NULL;
                pstTstMeta->u32BusyCount++;
            }
        }
    }

    /* Update debug info */
    pstChn->u32DbgEncCount++;
    ioctl(s_s32Adecfd[AdChn], IOC_ADEC_SET_DBG_INFO, &pstChn->u32DbgEncCount);

    pstChn->u32RefCount--;
    pthread_mutex_unlock(&pstChn->mutex);
    return HI_SUCCESS;
}

static HI_S32 AdecSendPack(ADEC_CHN AdChn, const AUDIO_STREAM_S *pstStream, HI_BOOL bBlock)
{
    ADEC_CHN_CTX_S *pstChn = &g_stAdec[AdChn];
    ADEC_AF_META_S *pstAfMeta;
    ADEC_AF_NODE_S *pstNode;
    HI_U8 *pu8InBuf;
    HI_S32 s32LeftBytes;
    HI_U16 *pu16OutBuf;
    HI_U32 u32OutLen = 0;
    HI_U32 u32Chns = 0;
    HI_S32 s32Ret;

    pthread_mutex_lock(&pstChn->mutex);

    if (pstChn->bCreated != 1) {
        pthread_mutex_unlock(&pstChn->mutex);
        return HI_ERR_ADEC_UNEXIST;
    }

    pstChn->u32RefCount++;

    /* Block on semWrite */
    if (bBlock) {
        pthread_mutex_unlock(&pstChn->mutex);
        sem_wait(&pstChn->semWrite);
        pthread_mutex_lock(&pstChn->mutex);

        if (pstChn->bCreated != 1) {
            pstChn->u32RefCount--;
            pthread_mutex_unlock(&pstChn->mutex);
            return HI_ERR_ADEC_UNEXIST;
        }
    } else {
        int val;
        sem_getvalue(&pstChn->semWrite, &val);
        if (val == 0) {
            pstChn->u32RefCount--;
            pthread_mutex_unlock(&pstChn->mutex);
            return HI_ERR_ADEC_BUF_FULL;
        }
        sem_wait(&pstChn->semWrite);
    }

    pstAfMeta = (ADEC_AF_META_S *)((HI_U8 *)pstChn->pAfBuf + ADEC_AF_META_OFFSET);

    /* Take a node from the free list */
    pstNode = pstAfMeta->stFreeHead.pNext;
    if (pstNode == HI_NULL) {
        pstChn->u32RefCount--;
        pthread_mutex_unlock(&pstChn->mutex);
        return HI_ERR_ADEC_BUF_FULL;
    }

    /* Remove from free list */
    pstAfMeta->stFreeHead.pNext = pstNode->pNext;
    if (pstNode->pNext != HI_NULL)
        pstNode->pNext->pPrev = &pstAfMeta->stFreeHead;
    pstNode->pNext = HI_NULL;
    pstNode->pPrev = HI_NULL;
    pstAfMeta->u32FreeCount--;

    /* Decode the frame */
    pu8InBuf = pstStream->pStream;
    s32LeftBytes = (HI_S32)pstStream->u32Len;
    pu16OutBuf = (HI_U16 *)pstNode->pu8FrameData;

    s32Ret = s_stDecoderCtx.entries[pstChn->s32DecoderIdx].pfnDecodeFrm(
        pstChn->pDecoder, &pu8InBuf, &s32LeftBytes,
        pu16OutBuf, &u32OutLen, &u32Chns);

    if (s32Ret == HI_SUCCESS && u32OutLen > 0) {
        /* Success: add to busy list */
        ADEC_AF_NODE_S *pstBusyTail = &pstAfMeta->stBusyHead;
        while (pstBusyTail->pNext != HI_NULL)
            pstBusyTail = pstBusyTail->pNext;

        pstBusyTail->pNext = pstNode;
        pstNode->pPrev = pstBusyTail;
        pstNode->pNext = HI_NULL;
        pstAfMeta->u32BusyCount++;

        /* Copy timestamp */
        pstChn->u64TimeStamp = pstStream->u64TimeStamp;

        sem_post(&pstChn->semRead);
        pstChn->u32DbgDecSuccCount++;
    } else {
        /* Failure: return node to free list */
        pstNode->pNext = pstAfMeta->stFreeHead.pNext;
        if (pstAfMeta->stFreeHead.pNext != HI_NULL)
            pstAfMeta->stFreeHead.pNext->pPrev = pstNode;
        pstAfMeta->stFreeHead.pNext = pstNode;
        pstNode->pPrev = &pstAfMeta->stFreeHead;
        pstAfMeta->u32FreeCount++;

        sem_post(&pstChn->semWrite);

        if (s32Ret != HI_SUCCESS) {
            HI_TRACE_ADEC(HI_DBG_ERR, "s32LeftBytes:%d, maxlen:%d\n",
                s32LeftBytes, ADEC_STRM_BUF_SIZE);
        }
    }

    pstChn->u32DbgEncCount++;
    ioctl(s_s32Adecfd[AdChn], IOC_ADEC_SET_DBG_INFO, &pstChn->u32DbgEncCount);

    pstChn->u32RefCount--;
    pthread_mutex_unlock(&pstChn->mutex);

    if (s32Ret != HI_SUCCESS)
        return HI_ERR_ADEC_DECODER_ERR;

    return HI_SUCCESS;
}

static HI_S32 MPI_ADEC_SendStream(ADEC_CHN AdChn, const AUDIO_STREAM_S *pstStream,
    HI_BOOL bBlock)
{
    ADEC_CHN_CTX_S *pstChn = &g_stAdec[AdChn];

    if (pstChn->u32DecMode == 1) {
        /* Stream mode */
        return AdecSendStream(AdChn, pstStream, bBlock);
    } else {
        /* Pack mode */
        return AdecSendPack(AdChn, pstStream, bBlock);
    }
}

/* ========================================================================== */
/* Public API functions                                                       */
/* ========================================================================== */

HI_S32 HI_MPI_ADEC_RegisterDecoder(HI_S32 *ps32Handle, const ADEC_DECODER_S *pstDecoder)
{
    HI_U32 i;

    if (ps32Handle == HI_NULL || pstDecoder == HI_NULL)
        return HI_ERR_ADEC_NULL_PTR;

    if (pstDecoder->pfnOpenDecoder == HI_NULL ||
        pstDecoder->pfnDecodeFrm == HI_NULL ||
        pstDecoder->pfnCloseDecoder == HI_NULL ||
        pstDecoder->pfnGetFrmInfo == HI_NULL)
        return HI_ERR_ADEC_NULL_PTR;

    pthread_mutex_lock(&s_stDecoderCtx.mutex);

    if (s_stDecoderCtx.u32Count >= ADEC_MAX_DECODER_NUM) {
        pthread_mutex_unlock(&s_stDecoderCtx.mutex);
        return HI_ERR_ADEC_BUF_FULL;
    }

    /* Check for duplicate payload type */
    for (i = 0; i < ADEC_MAX_DECODER_NUM; i++) {
        if (s_stDecoderCtx.entries[i].s32Handle == (HI_S32)pstDecoder->enType) {
            pthread_mutex_unlock(&s_stDecoderCtx.mutex);
            return HI_ERR_ADEC_BUF_FULL;
        }
    }

    /* Find empty slot */
    for (i = 0; i < ADEC_MAX_DECODER_NUM; i++) {
        if (s_stDecoderCtx.entries[i].s32Handle == -1) {
            memcpy_s(&s_stDecoderCtx.entries[i].s32Handle, 44, pstDecoder, 44);
            *ps32Handle = i;
            s_stDecoderCtx.u32Count++;
            pthread_mutex_unlock(&s_stDecoderCtx.mutex);
            return HI_SUCCESS;
        }
    }

    pthread_mutex_unlock(&s_stDecoderCtx.mutex);
    return HI_ERR_ADEC_BUF_FULL;
}

HI_S32 HI_MPI_ADEC_UnRegisterDecoder(HI_S32 s32Handle)
{
    HI_U32 i;

    if ((HI_U32)s32Handle >= ADEC_MAX_DECODER_NUM)
        return HI_ERR_ADEC_ILLEGAL_PARAM;

    pthread_mutex_lock(&s_stDecoderCtx.mutex);

    if (s_stDecoderCtx.u32Count == 0 ||
        s_stDecoderCtx.entries[s32Handle].s32Handle == -1) {
        pthread_mutex_unlock(&s_stDecoderCtx.mutex);
        return HI_ERR_ADEC_BUF_FULL;
    }

    /* Check no channel is using this decoder */
    for (i = 0; i < ADEC_MAX_CHN_NUM; i++) {
        pthread_mutex_lock(&g_stAdec[i].mutex);
        if (g_stAdec[i].s32DecoderIdx == s32Handle &&
            (g_stAdec[i].bCreated == 1 || g_stAdec[i].bDestroying == 1)) {
            HI_TRACE_ADEC(HI_DBG_ERR,
                "Adec chn%d is created by this decoder, please destroy it first!\n", i);
            pthread_mutex_unlock(&g_stAdec[i].mutex);
            pthread_mutex_unlock(&s_stDecoderCtx.mutex);
            return HI_ERR_ADEC_BUF_FULL;
        }
        pthread_mutex_unlock(&g_stAdec[i].mutex);
    }

    /* Clear the entry */
    memset_s(&s_stDecoderCtx.entries[s32Handle].s32Handle + 1, 40, 0, 40);
    s_stDecoderCtx.entries[s32Handle].s32Handle = -1;
    s_stDecoderCtx.u32Count--;
    pthread_mutex_unlock(&s_stDecoderCtx.mutex);
    return HI_SUCCESS;
}

HI_S32 HI_MPI_ADEC_CreateChn(ADEC_CHN AdChn, const ADEC_CHN_ATTR_S *pstAttr)
{
    HI_S32 s32Ret;
    ADEC_CHN_CTX_S *pstChn;

    if (AdChn >= ADEC_MAX_CHN_NUM)
        return HI_ERR_ADEC_INVALID_CHNID;

    if (pstAttr == HI_NULL)
        return HI_ERR_ADEC_NULL_PTR;

    s32Ret = AdecCheckOpen(AdChn);
    if (s32Ret != HI_SUCCESS)
        return s32Ret;

    s32Ret = MPI_ADEC_Init();
    if (s32Ret != HI_SUCCESS)
        return s32Ret;

    pstChn = &g_stAdec[AdChn];

    pthread_mutex_lock(&s_Adecmutex);

    s32Ret = MPI_ADEC_CreateChn(AdChn, pstAttr);
    if (s32Ret != HI_SUCCESS) {
        pthread_mutex_unlock(&s_Adecmutex);
        return s32Ret;
    }

    /* Set G726 BPS from pValue */
    if (pstAttr->enType == PT_G726) {
        if (pstAttr->pValue != HI_NULL)
            pstChn->u32G726Bps = *(HI_U32 *)pstAttr->pValue;
    } else {
        pstChn->u32G726Bps = 8;
    }

    /* Set ADPCM type from pValue */
    if (pstAttr->enType == PT_ADPCMA) {
        if (pstAttr->pValue != HI_NULL)
            pstChn->u32AdpcmType = *(HI_U32 *)pstAttr->pValue;
    } else {
        pstChn->u32AdpcmType = 3;
    }

    /* Set decode ready flag */
    pstChn->u32DecReadyFlag = (pstChn->u32DecMode == 1) ? 1 : 0;

    /* Set debug info */
    ioctl(s_s32Adecfd[AdChn], IOC_ADEC_SET_DBG_INFO, &pstChn->u32DbgEncCount);

    pthread_mutex_unlock(&s_Adecmutex);
    return HI_SUCCESS;
}

HI_S32 HI_MPI_ADEC_DestroyChn(ADEC_CHN AdChn)
{
    HI_S32 s32Ret;

    if (AdChn >= ADEC_MAX_CHN_NUM)
        return HI_ERR_ADEC_INVALID_CHNID;

    pthread_mutex_lock(&s_Adecmutex);
    s32Ret = MPI_ADEC_DestroyChn(AdChn);
    pthread_mutex_unlock(&s_Adecmutex);

    return s32Ret;
}

HI_S32 HI_MPI_ADEC_SendStream(ADEC_CHN AdChn, const AUDIO_STREAM_S *pstStream, HI_BOOL bBlock)
{
    if (AdChn >= ADEC_MAX_CHN_NUM)
        return HI_ERR_ADEC_INVALID_CHNID;

    if (pstStream == HI_NULL || pstStream->pStream == HI_NULL)
        return HI_ERR_ADEC_NULL_PTR;

    if (bBlock > 1) {
        HI_TRACE_ADEC(HI_DBG_ERR, "bBlock should be 0 or 1.\n");
        return HI_ERR_ADEC_ILLEGAL_PARAM;
    }

    return MPI_ADEC_SendStream(AdChn, pstStream, bBlock);
}

HI_S32 HI_MPI_ADEC_GetFrame(ADEC_CHN AdChn, AUDIO_FRAME_INFO_S *pstFrmInfo, HI_BOOL bBlock)
{
    ADEC_CHN_CTX_S *pstChn;
    ADEC_AF_META_S *pstAfMeta;
    ADEC_AF_NODE_S *pstNode;

    if (AdChn >= ADEC_MAX_CHN_NUM)
        return HI_ERR_ADEC_INVALID_CHNID;

    if (pstFrmInfo == HI_NULL)
        return HI_ERR_ADEC_NULL_PTR;

    pstChn = &g_stAdec[AdChn];

    pthread_mutex_lock(&pstChn->mutex);

    if (bBlock > 1) {
        HI_TRACE_ADEC(HI_DBG_ERR, "bBlock should be 0 or 1.\n");
        pthread_mutex_unlock(&pstChn->mutex);
        return HI_ERR_ADEC_ILLEGAL_PARAM;
    }

    if (pstChn->bCreated != 1) {
        pthread_mutex_unlock(&pstChn->mutex);
        return HI_ERR_ADEC_UNEXIST;
    }

    pstChn->u32RefCount++;

    if (bBlock) {
        pthread_mutex_unlock(&pstChn->mutex);
        sem_wait(&pstChn->semRead);
        pthread_mutex_lock(&pstChn->mutex);

        if (pstChn->bCreated != 1) {
            pstChn->u32RefCount--;
            pthread_mutex_unlock(&pstChn->mutex);
            return HI_ERR_ADEC_UNEXIST;
        }
    } else {
        int val;
        sem_getvalue(&pstChn->semRead, &val);
        if (val == 0) {
            pstChn->u32RefCount--;
            pthread_mutex_unlock(&pstChn->mutex);
            return HI_ERR_ADEC_BUF_EMPTY;
        }
        sem_wait(&pstChn->semRead);
    }

    /* Take from busy list head */
    pstAfMeta = (ADEC_AF_META_S *)((HI_U8 *)pstChn->pAfBuf + ADEC_AF_META_OFFSET);
    pstNode = pstAfMeta->stBusyHead.pNext;

    if (pstNode == HI_NULL) {
        pstChn->u32RefCount--;
        pthread_mutex_unlock(&pstChn->mutex);
        return HI_ERR_ADEC_BUF_EMPTY;
    }

    /* Fill output */
    pstFrmInfo->pstFrame = (AUDIO_FRAME_S *)pstNode;
    pstFrmInfo->u32Id = pstNode->field_44;

    pstChn->u32DbgGetFrmCount++;
    ioctl(s_s32Adecfd[AdChn], IOC_ADEC_SET_DBG_INFO, &pstChn->u32DbgEncCount);

    pstChn->u32RefCount--;
    pthread_mutex_unlock(&pstChn->mutex);
    return HI_SUCCESS;
}

HI_S32 HI_MPI_ADEC_ReleaseFrame(ADEC_CHN AdChn, const AUDIO_FRAME_INFO_S *pstFrmInfo)
{
    ADEC_CHN_CTX_S *pstChn;
    ADEC_AF_META_S *pstAfMeta;
    ADEC_AF_NODE_S *pstNode;
    ADEC_AF_NODE_S *pstIter;
    int val;

    if (AdChn >= ADEC_MAX_CHN_NUM)
        return HI_ERR_ADEC_INVALID_CHNID;

    if (pstFrmInfo == HI_NULL || pstFrmInfo->pstFrame == HI_NULL)
        return HI_ERR_ADEC_NULL_PTR;

    pstChn = &g_stAdec[AdChn];

    pthread_mutex_lock(&pstChn->mutex);

    if (pstChn->bCreated != 1) {
        pthread_mutex_unlock(&pstChn->mutex);
        return HI_ERR_ADEC_UNEXIST;
    }

    pstAfMeta = (ADEC_AF_META_S *)((HI_U8 *)pstChn->pAfBuf + ADEC_AF_META_OFFSET);
    pstNode = (ADEC_AF_NODE_S *)pstFrmInfo->pstFrame;

    /* Validate node is within AF buffer range */
    if (pstFrmInfo->u32Id >= pstAfMeta->u32TotalCount) {
        HI_TRACE_ADEC(HI_DBG_ERR, "FrmInfo is invalid.\n");
        pthread_mutex_unlock(&pstChn->mutex);
        return HI_ERR_ADEC_ILLEGAL_PARAM;
    }

    /* Find and remove node from busy list */
    pstIter = &pstAfMeta->stBusyHead;
    while (pstIter->pNext != HI_NULL) {
        if (pstIter->pNext == pstNode) {
            /* Remove from busy list */
            pstIter->pNext = pstNode->pNext;
            if (pstNode->pNext != HI_NULL)
                pstNode->pNext->pPrev = pstIter;
            pstAfMeta->u32BusyCount--;

            /* Add to free list */
            pstNode->pNext = pstAfMeta->stFreeHead.pNext;
            if (pstAfMeta->stFreeHead.pNext != HI_NULL)
                pstAfMeta->stFreeHead.pNext->pPrev = pstNode;
            pstAfMeta->stFreeHead.pNext = pstNode;
            pstNode->pPrev = &pstAfMeta->stFreeHead;
            pstAfMeta->u32FreeCount++;

            /* Post semWrite if room available */
            sem_getvalue(&pstChn->semWrite, &val);
            if (val < (HI_S32)(pstChn->stAttr.u32BufSize - 1))
                sem_post(&pstChn->semWrite);

            pstChn->u32DbgRelFrmCount++;
            ioctl(s_s32Adecfd[AdChn], IOC_ADEC_SET_DBG_INFO, &pstChn->u32DbgEncCount);

            pstChn->u32RefCount--;
            pthread_mutex_unlock(&pstChn->mutex);
            return HI_SUCCESS;
        }
        pstIter = pstIter->pNext;
    }

    HI_TRACE_ADEC(HI_DBG_ERR, "FrmInfo is invalid.\n");
    pthread_mutex_unlock(&pstChn->mutex);
    return HI_ERR_ADEC_ILLEGAL_PARAM;
}

HI_S32 HI_MPI_ADEC_ClearChnBuf(ADEC_CHN AdChn)
{
    if (AdChn >= ADEC_MAX_CHN_NUM)
        return HI_ERR_ADEC_INVALID_CHNID;

    return MPI_ADEC_ClearChnBuf(AdChn);
}

HI_S32 HI_MPI_ADEC_GetFrmInfo(ADEC_CHN AdChn, HI_VOID *pInfo)
{
    ADEC_CHN_CTX_S *pstChn;
    HI_S32 s32DecoderIdx;

    if (AdChn >= ADEC_MAX_CHN_NUM)
        return HI_ERR_ADEC_INVALID_CHNID;

    if (pInfo == HI_NULL)
        return HI_ERR_ADEC_NULL_PTR;

    pstChn = &g_stAdec[AdChn];

    if (pstChn->bCreated != 1)
        return HI_ERR_ADEC_UNEXIST;

    s32DecoderIdx = pstChn->s32DecoderIdx;
    if (s32DecoderIdx == -1) {
        printf("\nASSERT at:\n  >Function : %s\n  >Line No. : %d\n  >Condition: %s\n",
            "HI_MPI_ADEC_GetFrmInfo", 0,
            "HI_INVALID_HANDLE != pstMpiAdecCtx->s32Handle");
        _exit(s32DecoderIdx);
    }

    if (s_stDecoderCtx.entries[s32DecoderIdx].s32Handle == -1) {
        HI_TRACE_ADEC(HI_DBG_ERR, "The decoder has been unregistered!\n");
        return HI_ERR_ADEC_DECODER_ERR;
    }

    if (s_stDecoderCtx.entries[s32DecoderIdx].pfnGetFrmInfo(
            pstChn->pDecoder, pInfo) != HI_SUCCESS)
        return HI_ERR_ADEC_NOT_PERM;

    return HI_SUCCESS;
}

HI_S32 HI_MPI_ADEC_SendEndOfStream(ADEC_CHN AdChn, HI_BOOL bInstant)
{
    HI_S32 s32Ret;
    ADEC_CHN_CTX_S *pstChn;

    if (AdChn >= ADEC_MAX_CHN_NUM)
        return HI_ERR_ADEC_INVALID_CHNID;

    s32Ret = AdecCheckOpen(AdChn);
    if (s32Ret != HI_SUCCESS)
        return s32Ret;

    if (bInstant > 1) {
        HI_TRACE_ADEC(HI_DBG_ERR, "bInstant should be 0 or 1.\n");
        return HI_ERR_ADEC_ILLEGAL_PARAM;
    }

    pstChn = &g_stAdec[AdChn];

    pthread_mutex_lock(&pstChn->mutex);

    if (pstChn->bCreated != 1) {
        pthread_mutex_unlock(&pstChn->mutex);
        return HI_ERR_ADEC_UNEXIST;
    }

    if (bInstant == HI_FALSE) {
        /* Deferred: just set flag, decoder thread will handle it */
        pstChn->u32EndOfStream = 1;
        pthread_mutex_unlock(&pstChn->mutex);
        return HI_SUCCESS;
    }

    /* Instant: reset decoder and clear buffer now */
    {
        HI_S32 s32DecoderIdx = pstChn->s32DecoderIdx;
        if (s32DecoderIdx == -1) {
            printf("\nASSERT at:\n  >Function : %s\n  >Line No. : %d\n  >Condition: %s\n",
                "HI_MPI_ADEC_SendEndOfStream", 0,
                "HI_INVALID_HANDLE != pstAdecChn->s32Handle");
            _exit(s32DecoderIdx);
        }

        if (s_stDecoderCtx.entries[s32DecoderIdx].s32Handle == -1) {
            HI_TRACE_ADEC(HI_DBG_ERR, "The decoder has been unregistered!\n");
            pthread_mutex_unlock(&pstChn->mutex);
            return HI_ERR_ADEC_DECODER_ERR;
        }

        if (s_stDecoderCtx.entries[s32DecoderIdx].pfnResetDecoder != HI_NULL) {
            s32Ret = s_stDecoderCtx.entries[s32DecoderIdx].pfnResetDecoder(pstChn->pDecoder);
            if (s32Ret != HI_SUCCESS) {
                HI_TRACE_ADEC(HI_DBG_ERR, "Reset Decoder failed!\n");
                pthread_mutex_unlock(&pstChn->mutex);
                return s32Ret;
            }
        }
    }

    pthread_mutex_unlock(&pstChn->mutex);

    s32Ret = HI_MPI_ADEC_ClearChnBuf(AdChn);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_ADEC(HI_DBG_ERR, "HI_MPI_ADEC_ClearChnBuf failed!\n");
        return s32Ret;
    }

    pthread_mutex_lock(&pstChn->mutex);
    pstChn->u32EndOfStream = 0;
    pthread_mutex_unlock(&pstChn->mutex);

    return HI_SUCCESS;
}

HI_S32 HI_MPI_ADEC_QueryChnStat(ADEC_CHN AdChn, ADEC_CHN_STATE_S *pstBufferStatus)
{
    ADEC_CHN_CTX_S *pstChn;
    ADEC_AF_META_S *pstAfMeta;

    if (AdChn >= ADEC_MAX_CHN_NUM)
        return HI_ERR_ADEC_INVALID_CHNID;

    if (pstBufferStatus == HI_NULL)
        return HI_ERR_ADEC_NULL_PTR;

    pstChn = &g_stAdec[AdChn];

    pthread_mutex_lock(&pstChn->mutex);

    if (pstChn->bCreated != 1) {
        pthread_mutex_unlock(&pstChn->mutex);
        return HI_ERR_ADEC_UNEXIST;
    }

    pstAfMeta = (ADEC_AF_META_S *)((HI_U8 *)pstChn->pAfBuf + ADEC_AF_META_OFFSET);

    pstBufferStatus->bEndOfStream = pstChn->u32EndOfStream;
    pstBufferStatus->u32BufferFrmNum = pstAfMeta->u32TotalCount;
    pstBufferStatus->u32BufferFreeNum = pstAfMeta->u32FreeCount;
    pstBufferStatus->u32BufferBusyNum = pstAfMeta->u32BusyCount;

    pthread_mutex_unlock(&pstChn->mutex);
    return HI_SUCCESS;
}
