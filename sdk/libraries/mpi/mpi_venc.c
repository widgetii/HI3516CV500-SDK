/**
 * Reverse Engineered from mpi_venc.S vendor assembly
 */

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "re_mpi_venc.h"
#include "re_debug.h"
#include "hi_defines.h"
#include "securec.h"

// ============================================================================
// Global state
// ============================================================================

static pthread_mutex_t s_VencMutex;
static HI_BOOL         s_bMpiVencInit = HI_FALSE;
static HI_S32          s32VencMemFd = -1;
static VENC_CHN_CTX_S  g_stMpiVencChn[VENC_MAX_CHN_NUM];

// ============================================================================
// Internal helpers
// ============================================================================

static HI_S32
MpiVencCheckChn(VENC_CHN VeChn)
{
    HI_RETRACE(HI_ERR_VENC_INVALID_CHNID,
        "func:%s, chn id %d err,should in [0,VENC_MAX_CHN_NUM)\n",
        __FUNCTION__, VeChn);
}

static HI_S32
MpiVencCheckNull(void)
{
    HI_RETRACE(HI_ERR_VENC_NULL_PTR,
        "func:%s,NULL pointer detected\n", __FUNCTION__);
}

static HI_S32
MpiVencCheckRoiIndex(HI_U32 u32Index)
{
    HI_RETRACE(HI_ERR_VENC_ILLEGAL_PARAM,
        "func:%s, RoiIndex  %d err,should in [0,VENC_MAX_ROI_NUM)\n",
        __FUNCTION__, u32Index);
}

static HI_S32
MpiVencCheckSseIndex(HI_U32 u32Index)
{
    HI_RETRACE(HI_ERR_VENC_ILLEGAL_PARAM,
        "func:%s, SseIndex  %d err,should in [0,VENC_MAX_SSE_NUM)\n",
        __FUNCTION__, u32Index);
}

HI_S32
MPI_VENC_Init(void)
{
    HI_S32 result;
    HI_S32 i;

    pthread_mutex_lock(&s_VencMutex);

    if (s_bMpiVencInit == HI_TRUE) {
        pthread_mutex_unlock(&s_VencMutex);
        return HI_SUCCESS;
    }

    memset_s(g_stMpiVencChn, sizeof(g_stMpiVencChn), 0, sizeof(g_stMpiVencChn));

    for (i = 0; i < VENC_MAX_CHN_NUM; i++) {
        g_stMpiVencChn[i].s32Fd = -1;
        result = pthread_mutex_init(&g_stMpiVencChn[i].mutex, NULL);
        if (result != 0) {
            fprintf(stderr,
                "Mpi venc init failed in line %d\n", __LINE__);
            pthread_mutex_unlock(&s_VencMutex);
            return -1;
        }
    }

    s_bMpiVencInit = HI_TRUE;
    pthread_mutex_unlock(&s_VencMutex);
    return HI_SUCCESS;
}

HI_S32
MPI_VENC_OPEN(VENC_CHN VeChn)
{
    HI_S32 result;
    HI_S32 fd;
    char devName[128];

    result = MPI_VENC_Init();
    if (result != HI_SUCCESS) {
        fprintf(stderr,
            "func:%s, sys not ready in line:%d\n", __FUNCTION__, __LINE__);
        return HI_ERR_VENC_SYS_NOTREADY;
    }

    pthread_mutex_lock(&g_stMpiVencChn[VeChn].mutex);

    if (g_stMpiVencChn[VeChn].s32Fd >= 0) {
        pthread_mutex_unlock(&g_stMpiVencChn[VeChn].mutex);
        return HI_SUCCESS;
    }

    memset_s(devName, sizeof(devName), 0, sizeof(devName));
    snprintf_s(devName, sizeof(devName), 9, VENC_DEV_NAME);

    fd = open(devName, O_RDWR, 0);
    if (fd < 0) {
        g_stMpiVencChn[VeChn].s32Fd = -1;
        pthread_mutex_unlock(&g_stMpiVencChn[VeChn].mutex);
        fprintf(stderr,
            "func:%s, Chn %d open err!\n", __FUNCTION__, VeChn);
        return HI_ERR_VENC_SYS_NOTREADY;
    }

    g_stMpiVencChn[VeChn].s32Fd = fd;

    if (ioctl(fd, VENC_CTL_SET_CHN_ID, &VeChn) != HI_SUCCESS) {
        close(g_stMpiVencChn[VeChn].s32Fd);
        g_stMpiVencChn[VeChn].s32Fd = -1;
        pthread_mutex_unlock(&g_stMpiVencChn[VeChn].mutex);
        fprintf(stderr,
            "func:%s, Chn %d open err!\n", __FUNCTION__, VeChn);
        return HI_ERR_VENC_SYS_NOTREADY;
    }

    pthread_mutex_unlock(&g_stMpiVencChn[VeChn].mutex);
    return HI_SUCCESS;
}

static HI_S32
MpiVencCheckMemFd(VENC_CHN VeChn)
{
    pthread_mutex_lock(&g_stMpiVencChn[VeChn].mutex);

    if (s32VencMemFd >= 0) {
        pthread_mutex_unlock(&g_stMpiVencChn[VeChn].mutex);
        return HI_SUCCESS;
    }

    s32VencMemFd = open(VENC_DEV_NAME, O_RDWR, 0);
    if (s32VencMemFd < 0) {
        pthread_mutex_unlock(&g_stMpiVencChn[VeChn].mutex);
        fprintf(stderr,
            "func:%s line:%d,HI_MPI_VENC_Open: open mem device failed\n",
            __FUNCTION__, __LINE__);
        return HI_ERR_VENC_SYS_NOTREADY;
    }

    pthread_mutex_unlock(&g_stMpiVencChn[VeChn].mutex);
    return HI_SUCCESS;
}

static HI_S32
memmap_venc(VENC_CHN VeChn)
{
    VENC_CHN_CTX_S *pCtx = &g_stMpiVencChn[VeChn];
    HI_VOID *pAddr;

    if (pCtx->u64BufLen == 0 || pCtx->u64PhyAddr == 0)
        return HI_SUCCESS;

    pAddr = mmap(NULL, (size_t)pCtx->u64BufLen,
        PROT_READ | PROT_WRITE, MAP_SHARED,
        s32VencMemFd, (off_t)pCtx->u64PhyAddr);

    if (pAddr == MAP_FAILED) {
        fprintf(stderr,
            "mmap err,page addr:0x%llx, u32PagePhy[%u] size:%llu\n",
            pCtx->u64PhyAddr, (HI_U32)VeChn, pCtx->u64BufLen);
        return HI_FAILURE;
    }

    pCtx->pUserAddr = pAddr;
    return HI_SUCCESS;
}

static HI_U64
VencVirt2User(VENC_CHN VeChn, HI_U64 u64Addr)
{
    VENC_CHN_CTX_S *pCtx = &g_stMpiVencChn[VeChn];

    if (u64Addr < pCtx->u64VirtAddr)
        return 0;
    if (u64Addr >= pCtx->u64VirtAddr + pCtx->u64BufLen)
        return 0;

    return (u64Addr - pCtx->u64VirtAddr) + (HI_U64)(HI_UL)pCtx->pUserAddr;
}

static HI_U64
VencVirt2Phy(VENC_CHN VeChn, HI_U64 u64Addr)
{
    VENC_CHN_CTX_S *pCtx = &g_stMpiVencChn[VeChn];

    if (u64Addr < pCtx->u64VirtAddr)
        return 0;
    if (u64Addr >= pCtx->u64VirtAddr + pCtx->u64BufLen)
        return 0;

    return (u64Addr - pCtx->u64VirtAddr) + pCtx->u64PhyAddr;
}

static HI_U64
VencPhy2User(VENC_CHN VeChn, HI_U64 u64Addr)
{
    VENC_CHN_CTX_S *pCtx = &g_stMpiVencChn[VeChn];

    if (u64Addr < pCtx->u64PhyAddr)
        return 0;
    if (u64Addr > pCtx->u64PhyAddr + pCtx->u64BufLen)
        return 0;
    if (u64Addr == pCtx->u64PhyAddr + pCtx->u64BufLen)
        return (HI_U64)(HI_UL)pCtx->pUserAddr;

    return (u64Addr - pCtx->u64PhyAddr) + (HI_U64)(HI_UL)pCtx->pUserAddr;
}

static HI_U64
VencPhy2Virt(VENC_CHN VeChn, HI_U64 u64Addr)
{
    VENC_CHN_CTX_S *pCtx = &g_stMpiVencChn[VeChn];

    if (u64Addr < pCtx->u64PhyAddr)
        return 0;
    if (u64Addr >= pCtx->u64PhyAddr + pCtx->u64BufLen)
        return 0;

    return (u64Addr - pCtx->u64PhyAddr) + pCtx->u64VirtAddr;
}

// ============================================================================
// Tier 3: Complex functions
// ============================================================================

HI_S32
HI_MPI_VENC_CreateChn(VENC_CHN VeChn, const VENC_CHN_ATTR_S *pstAttr)
{
    HI_S32 s32Ret;
    VENC_STREAM_BUF_INFO_KERN_S stBufInfo;
    VENC_CHN_CTX_S *pCtx;
    HI_U32 i;

    if (VeChn > (VENC_MAX_CHN_NUM - 1))
        return MpiVencCheckChn(VeChn);

    s32Ret = MPI_VENC_OPEN(VeChn);
    if (s32Ret != HI_SUCCESS) return s32Ret;

    if (pstAttr == HI_NULL)
        return MpiVencCheckNull();

    s32Ret = MpiVencCheckMemFd(VeChn);
    if (s32Ret != HI_SUCCESS) return s32Ret;

    pCtx = &g_stMpiVencChn[VeChn];
    pthread_mutex_lock(&pCtx->mutex);

    s32Ret = ioctl(pCtx->s32Fd, VENC_CTL_CREATE_CHN, pstAttr);
    if (s32Ret != HI_SUCCESS) {
        pthread_mutex_unlock(&pCtx->mutex);
        return s32Ret;
    }

    s32Ret = ioctl(pCtx->s32Fd, VENC_CTL_GET_STREAM_BUF_INFO, &stBufInfo);
    if (s32Ret != HI_SUCCESS) {
        pthread_mutex_unlock(&pCtx->mutex);
        ioctl(pCtx->s32Fd, VENC_CTL_DESTROY_CHN);
        return s32Ret;
    }

    pCtx->u64PhyAddr  = stBufInfo.u64PhyAddr;
    pCtx->u64VirtAddr = stBufInfo.u64VirtAddr;
    pCtx->u64BufLen   = (HI_U64)stBufInfo.u32BufLen;
    pCtx->u32CodingType = stBufInfo.u32Field14;
    pCtx->u32PackCnt  = stBufInfo.u32Field18;
    pCtx->pUserAddr   = HI_NULL;
    pCtx->reserved_34 = 0;

    memmap_venc(VeChn);

    /* Verify all pack slots are mapped */
    for (i = 0; i < pCtx->u32PackCnt; i++) {
        if (pCtx->pUserAddr == HI_NULL) {
            pthread_mutex_unlock(&pCtx->mutex);
            ioctl(pCtx->s32Fd, VENC_CTL_DESTROY_CHN);
            return HI_ERR_VENC_NOMEM;
        }
    }

    pthread_mutex_unlock(&pCtx->mutex);
    return HI_SUCCESS;
}

HI_S32
HI_MPI_VENC_DestroyChn(VENC_CHN VeChn)
{
    HI_S32 s32Ret;
    VENC_CHN_CTX_S *pCtx;

    if (VeChn > (VENC_MAX_CHN_NUM - 1))
        return MpiVencCheckChn(VeChn);

    s32Ret = MPI_VENC_OPEN(VeChn);
    if (s32Ret != HI_SUCCESS) return s32Ret;

    pCtx = &g_stMpiVencChn[VeChn];

    s32Ret = ioctl(pCtx->s32Fd, VENC_CTL_DESTROY_CHN);

    if (pCtx->pUserAddr != HI_NULL) {
        munmap(pCtx->pUserAddr, (size_t)pCtx->u64BufLen);
        pCtx->pUserAddr = HI_NULL;
    }

    pCtx->u64PhyAddr = 0;
    pCtx->u64VirtAddr = 0;
    pCtx->u64BufLen = 0;

    return s32Ret;
}

HI_S32
HI_MPI_VENC_ResetChn(VENC_CHN VeChn)
{
    HI_S32 s32Ret;

    if (VeChn > (VENC_MAX_CHN_NUM - 1))
        return MpiVencCheckChn(VeChn);

    s32Ret = MPI_VENC_OPEN(VeChn);
    if (s32Ret != HI_SUCCESS) return s32Ret;

    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_RESET_CHN);
}

HI_S32
HI_MPI_VENC_StartRecvFrame(VENC_CHN VeChn, const VENC_RECV_PIC_PARAM_S *pstRecvParam)
{
    HI_S32 s32Ret;

    if (VeChn > (VENC_MAX_CHN_NUM - 1))
        return MpiVencCheckChn(VeChn);

    s32Ret = MPI_VENC_OPEN(VeChn);
    if (s32Ret != HI_SUCCESS) return s32Ret;

    if (pstRecvParam == HI_NULL)
        return MpiVencCheckNull();

    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_START_RECV_FRAME, pstRecvParam);
}

HI_S32
HI_MPI_VENC_StopRecvFrame(VENC_CHN VeChn)
{
    HI_S32 s32Ret;

    if (VeChn > (VENC_MAX_CHN_NUM - 1))
        return MpiVencCheckChn(VeChn);

    s32Ret = MPI_VENC_OPEN(VeChn);
    if (s32Ret != HI_SUCCESS) return s32Ret;

    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_STOP_RECV_FRAME);
}

HI_S32
HI_MPI_VENC_QueryStatus(VENC_CHN VeChn, VENC_CHN_STATUS_S *pstStatus)
{
    HI_S32 s32Ret;

    if (VeChn > (VENC_MAX_CHN_NUM - 1))
        return MpiVencCheckChn(VeChn);

    s32Ret = MPI_VENC_OPEN(VeChn);
    if (s32Ret != HI_SUCCESS) return s32Ret;

    if (pstStatus == HI_NULL)
        return MpiVencCheckNull();

    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_QUERY_STATUS, pstStatus);
}

HI_S32
HI_MPI_VENC_SetChnAttr(VENC_CHN VeChn, const VENC_CHN_ATTR_S *pstChnAttr)
{
    HI_S32 s32Ret;

    if (VeChn > (VENC_MAX_CHN_NUM - 1))
        return MpiVencCheckChn(VeChn);

    s32Ret = MPI_VENC_OPEN(VeChn);
    if (s32Ret != HI_SUCCESS) return s32Ret;

    if (pstChnAttr == HI_NULL)
        return MpiVencCheckNull();

    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_SET_CHN_ATTR, pstChnAttr);
}

HI_S32
HI_MPI_VENC_GetChnAttr(VENC_CHN VeChn, VENC_CHN_ATTR_S *pstChnAttr)
{
    HI_S32 s32Ret;

    if (VeChn > (VENC_MAX_CHN_NUM - 1))
        return MpiVencCheckChn(VeChn);

    s32Ret = MPI_VENC_OPEN(VeChn);
    if (s32Ret != HI_SUCCESS) return s32Ret;

    if (pstChnAttr == HI_NULL)
        return MpiVencCheckNull();

    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_GET_CHN_ATTR, pstChnAttr);
}

HI_S32
HI_MPI_VENC_GetStream(VENC_CHN VeChn, VENC_STREAM_S *pstStream, HI_S32 s32MilliSec)
{
    HI_S32 s32Ret;
    VENC_STREAM_S stLocalStream;
    VENC_CHN_CTX_S *pCtx;
    VENC_STREAM_BUF_INFO_KERN_S stBufInfo;
    HI_U32 u32StreamOffset;
    HI_U32 i;
    struct {
        HI_S32 s32MilliSec;
        VENC_STREAM_S stStream;
    } stIocData;

    if (VeChn > (VENC_MAX_CHN_NUM - 1))
        return MpiVencCheckChn(VeChn);

    s32Ret = MPI_VENC_OPEN(VeChn);
    if (s32Ret != HI_SUCCESS) return s32Ret;

    if (pstStream == HI_NULL)
        return MpiVencCheckNull();

    if (pstStream->pstPack == HI_NULL)
        return MpiVencCheckNull();

    s32Ret = MpiVencCheckMemFd(VeChn);
    if (s32Ret != HI_SUCCESS) return s32Ret;

    if (s32MilliSec < -1) {
        fprintf(stderr,
            "The param:s32MilliSec %d should larger than %d in func:%s\n",
            s32MilliSec, -1, __FUNCTION__);
        return HI_ERR_VENC_ILLEGAL_PARAM;
    }

    pCtx = &g_stMpiVencChn[VeChn];

    /* Query stream buffer info if not yet mapped */
    pthread_mutex_lock(&pCtx->mutex);

    s32Ret = ioctl(pCtx->s32Fd, VENC_CTL_GET_STREAM_BUF_INFO, &stBufInfo);
    if (s32Ret != HI_SUCCESS) {
        fprintf(stderr,
            "chnl:%d,Venc Get Stream Info err 0x%x\n", VeChn, s32Ret);
        pthread_mutex_unlock(&pCtx->mutex);
        return s32Ret;
    }

    /* If buffer info changed, re-map */
    if (pCtx->pUserAddr == HI_NULL) {
        pCtx->u64PhyAddr  = stBufInfo.u64PhyAddr;
        pCtx->u64VirtAddr = stBufInfo.u64VirtAddr;
        pCtx->u64BufLen   = (HI_U64)stBufInfo.u32BufLen;
        pCtx->u32CodingType = stBufInfo.u32Field14;
        pCtx->u32PackCnt  = stBufInfo.u32Field18;
        pCtx->reserved_34 = 0;

        memmap_venc(VeChn);

        /* Verify mapping */
        if (pCtx->u32PackCnt > 0 && pCtx->pUserAddr == HI_NULL) {
            pthread_mutex_unlock(&pCtx->mutex);
            return HI_ERR_VENC_NOMEM;
        }
    }

    pthread_mutex_unlock(&pCtx->mutex);

    /* Issue GetStream ioctl */
    stIocData.s32MilliSec = s32MilliSec;
    memcpy_s(&stIocData.stStream, sizeof(VENC_STREAM_S),
             pstStream, sizeof(VENC_STREAM_S));

    pthread_mutex_lock(&pCtx->mutex);

    s32Ret = ioctl(pCtx->s32Fd, VENC_CTL_GET_STREAM, &stIocData);

    if (s32Ret != HI_SUCCESS) {
        pthread_mutex_unlock(&pCtx->mutex);
        return s32Ret;
    }

    /* Determine stream header offset based on codec type */
    switch (pCtx->u32CodingType) {
    case PT_H264:
    case PT_H265:
        u32StreamOffset = 64;
        break;
    case PT_MJPEG:
        u32StreamOffset = 128;
        break;
    default:
        u32StreamOffset = 64;
        break;
    }

    /* Fixup pack addresses from kernel-virtual to user-virtual */
    memcpy_s(&stLocalStream, sizeof(VENC_STREAM_S),
             &stIocData.stStream, sizeof(VENC_STREAM_S));

    if (stLocalStream.u32PackCount > 0) {
        VENC_PACK_S *pPack = stLocalStream.pstPack;
        for (i = 0; i < stLocalStream.u32PackCount; i++) {
            pPack[i].u32Len -= u32StreamOffset;
            pPack[i].u64PhyAddr += (HI_U64)u32StreamOffset;
            pPack[i].pu8Addr = (HI_U8*)(HI_UL)VencPhy2User(VeChn,
                pPack[i].u64PhyAddr);
        }
    }

    memcpy_s(pstStream, sizeof(VENC_STREAM_S),
             &stLocalStream, sizeof(VENC_STREAM_S));

    pthread_mutex_unlock(&pCtx->mutex);
    return HI_SUCCESS;
}

HI_S32
HI_MPI_VENC_ReleaseStream(VENC_CHN VeChn, VENC_STREAM_S *pstStream)
{
    HI_S32 s32Ret;
    VENC_CHN_CTX_S *pCtx;
    HI_U32 u32StreamOffset;
    HI_U32 i;

    if (VeChn > (VENC_MAX_CHN_NUM - 1))
        return MpiVencCheckChn(VeChn);

    s32Ret = MPI_VENC_OPEN(VeChn);
    if (s32Ret != HI_SUCCESS) return s32Ret;

    if (pstStream == HI_NULL)
        return MpiVencCheckNull();

    if (pstStream->pstPack == HI_NULL)
        return MpiVencCheckNull();

    pCtx = &g_stMpiVencChn[VeChn];

    /* Determine stream header offset */
    if (pCtx->u32CodingType == PT_MJPEG)
        u32StreamOffset = 128;
    else
        u32StreamOffset = 64;

    pthread_mutex_lock(&pCtx->mutex);

    /* Reverse the address fixup: user-virtual back to kernel-virtual */
    if (pstStream->u32PackCount > 0) {
        VENC_PACK_S *pPack = pstStream->pstPack;
        for (i = 0; i < pstStream->u32PackCount; i++) {
            pPack[i].u32Len += u32StreamOffset;
            pPack[i].u64PhyAddr -= (HI_U64)u32StreamOffset;
            pPack[i].pu8Addr = (HI_U8*)(HI_UL)VencPhy2Virt(VeChn,
                pPack[i].u64PhyAddr);
        }
    }

    s32Ret = ioctl(pCtx->s32Fd, VENC_CTL_RELEASE_STREAM, pstStream);

    /* If release failed, undo the address fixup */
    if (s32Ret != HI_SUCCESS && pstStream->u32PackCount > 0) {
        VENC_PACK_S *pPack = pstStream->pstPack;
        for (i = 0; i < pstStream->u32PackCount; i++) {
            pPack[i].u32Len -= u32StreamOffset;
            pPack[i].u64PhyAddr += (HI_U64)u32StreamOffset;
            pPack[i].pu8Addr = (HI_U8*)(HI_UL)VencPhy2User(VeChn,
                pPack[i].u64PhyAddr);
        }
    }

    pthread_mutex_unlock(&pCtx->mutex);
    return s32Ret;
}

HI_S32
HI_MPI_VENC_InsertUserData(VENC_CHN VeChn, HI_U8 *pu8Data, HI_U32 u32Len)
{
    HI_S32 s32Ret;
    VENC_USER_DATA_S stUserData;

    if (VeChn > (VENC_MAX_CHN_NUM - 1))
        return MpiVencCheckChn(VeChn);

    s32Ret = MPI_VENC_OPEN(VeChn);
    if (s32Ret != HI_SUCCESS) return s32Ret;

    if (pu8Data == HI_NULL)
        return MpiVencCheckNull();

    stUserData.pu8Data = pu8Data;
    stUserData.u32Len  = u32Len;

    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_INSERT_USERDATA, &stUserData);
}

HI_S32
HI_MPI_VENC_SendFrame(VENC_CHN VeChn, const VIDEO_FRAME_INFO_S *pstFrame, HI_S32 s32MilliSec)
{
    HI_S32 s32Ret;

    if (VeChn > (VENC_MAX_CHN_NUM - 1))
        return MpiVencCheckChn(VeChn);

    s32Ret = MPI_VENC_OPEN(VeChn);
    if (s32Ret != HI_SUCCESS) return s32Ret;

    if (pstFrame == HI_NULL)
        return MpiVencCheckNull();

    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_SEND_FRAME, pstFrame);
}

HI_S32
HI_MPI_VENC_SendFrameEx(VENC_CHN VeChn, const USER_FRAME_INFO_S *pstFrame, HI_S32 s32MilliSec)
{
    HI_S32 s32Ret;

    if (VeChn > (VENC_MAX_CHN_NUM - 1))
        return MpiVencCheckChn(VeChn);

    s32Ret = MPI_VENC_OPEN(VeChn);
    if (s32Ret != HI_SUCCESS) return s32Ret;

    if (pstFrame == HI_NULL)
        return MpiVencCheckNull();

    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_SEND_FRAME_EX, pstFrame);
}

HI_S32
HI_MPI_VENC_RequestIDR(VENC_CHN VeChn, HI_BOOL bInstant)
{
    HI_S32 s32Ret;

    if (VeChn > (VENC_MAX_CHN_NUM - 1))
        return MpiVencCheckChn(VeChn);

    s32Ret = MPI_VENC_OPEN(VeChn);
    if (s32Ret != HI_SUCCESS) return s32Ret;

    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_REQUEST_IDR, &bInstant);
}

HI_S32
HI_MPI_VENC_GetFd(VENC_CHN VeChn)
{
    HI_S32 s32Ret;

    if (VeChn > (VENC_MAX_CHN_NUM - 1))
        return MpiVencCheckChn(VeChn);

    s32Ret = MPI_VENC_OPEN(VeChn);
    if (s32Ret != HI_SUCCESS) return s32Ret;

    return g_stMpiVencChn[VeChn].s32Fd;
}

HI_S32
HI_MPI_VENC_CloseFd(VENC_CHN VeChn)
{
    VENC_CHN_CTX_S *pCtx;

    if (VeChn > (VENC_MAX_CHN_NUM - 1))
        return HI_SUCCESS;

    pCtx = &g_stMpiVencChn[VeChn];

    pthread_mutex_lock(&pCtx->mutex);

    if (pCtx->s32Fd >= 0) {
        if (close(pCtx->s32Fd) != 0)
            fprintf(stderr, "Close VENC Channel %d Fd Fail\n", VeChn);
        pCtx->s32Fd = -1;
    }

    if (pCtx->pUserAddr != HI_NULL) {
        munmap(pCtx->pUserAddr, (size_t)pCtx->u64BufLen);
        pCtx->pUserAddr = HI_NULL;
    }

    pCtx->u64PhyAddr = 0;
    pCtx->u64VirtAddr = 0;
    pCtx->u64BufLen = 0;

    pthread_mutex_unlock(&pCtx->mutex);

    /* Close mem fd if this is the last channel */
    if (s32VencMemFd >= 0) {
        if (close(s32VencMemFd) != 0)
            fprintf(stderr, "Close Venc Mem Fd Fail\n");
        s32VencMemFd = -1;
    }

    return HI_SUCCESS;
}

HI_S32
MPI_VENC_Exit(void)
{
    HI_S32 i;

    pthread_mutex_lock(&s_VencMutex);

    if (s_bMpiVencInit == HI_FALSE) {
        pthread_mutex_unlock(&s_VencMutex);
        return HI_SUCCESS;
    }

    for (i = 0; i < VENC_MAX_CHN_NUM; i++) {
        HI_MPI_VENC_CloseFd(i);
        pthread_mutex_destroy(&g_stMpiVencChn[i].mutex);
    }

    s_bMpiVencInit = HI_FALSE;
    pthread_mutex_unlock(&s_VencMutex);
    return HI_SUCCESS;
}

// ============================================================================
// Tier 2: ROI / SSE / ModParam (extra validation)
// ============================================================================

HI_S32
HI_MPI_VENC_SetRoiAttr(VENC_CHN VeChn, const VENC_ROI_ATTR_S *pstRoiAttr)
{
    HI_S32 s32Ret;

    if (VeChn > (VENC_MAX_CHN_NUM - 1))
        return MpiVencCheckChn(VeChn);

    s32Ret = MPI_VENC_OPEN(VeChn);
    if (s32Ret != HI_SUCCESS) return s32Ret;

    if (pstRoiAttr == HI_NULL)
        return MpiVencCheckNull();

    if (pstRoiAttr->u32Index >= 8)
        return MpiVencCheckRoiIndex(pstRoiAttr->u32Index);

    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_SET_ROI_ATTR, pstRoiAttr);
}

HI_S32
HI_MPI_VENC_GetRoiAttr(VENC_CHN VeChn, HI_U32 u32Index, VENC_ROI_ATTR_S *pstRoiAttr)
{
    HI_S32 s32Ret;

    if (VeChn > (VENC_MAX_CHN_NUM - 1))
        return MpiVencCheckChn(VeChn);

    s32Ret = MPI_VENC_OPEN(VeChn);
    if (s32Ret != HI_SUCCESS) return s32Ret;

    if (pstRoiAttr == HI_NULL)
        return MpiVencCheckNull();

    if (u32Index >= 8)
        return MpiVencCheckRoiIndex(u32Index);

    pstRoiAttr->u32Index = u32Index;

    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_GET_ROI_ATTR, pstRoiAttr);
}

HI_S32
HI_MPI_VENC_GetRoiAttrEx(VENC_CHN VeChn, HI_U32 u32Index, VENC_ROI_ATTR_EX_S *pstRoiAttrEx)
{
    HI_S32 s32Ret;

    if (VeChn > (VENC_MAX_CHN_NUM - 1))
        return MpiVencCheckChn(VeChn);

    s32Ret = MPI_VENC_OPEN(VeChn);
    if (s32Ret != HI_SUCCESS) return s32Ret;

    if (pstRoiAttrEx == HI_NULL)
        return MpiVencCheckNull();

    if (u32Index >= 8)
        return MpiVencCheckRoiIndex(u32Index);

    pstRoiAttrEx->u32Index = u32Index;

    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_GET_ROI_ATTR_EX, pstRoiAttrEx);
}

HI_S32
HI_MPI_VENC_SetRoiAttrEx(VENC_CHN VeChn, const VENC_ROI_ATTR_EX_S *pstRoiAttrEx)
{
    HI_S32 s32Ret;

    if (VeChn > (VENC_MAX_CHN_NUM - 1))
        return MpiVencCheckChn(VeChn);

    s32Ret = MPI_VENC_OPEN(VeChn);
    if (s32Ret != HI_SUCCESS) return s32Ret;

    if (pstRoiAttrEx == HI_NULL)
        return MpiVencCheckNull();

    if (pstRoiAttrEx->u32Index >= 8)
        return MpiVencCheckRoiIndex(pstRoiAttrEx->u32Index);

    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_SET_ROI_ATTR_EX, pstRoiAttrEx);
}

HI_S32
HI_MPI_VENC_SetSSERegion(VENC_CHN VeChn, const VENC_SSE_CFG_S *pstSSECfg)
{
    HI_S32 s32Ret;

    if (VeChn > (VENC_MAX_CHN_NUM - 1))
        return MpiVencCheckChn(VeChn);

    s32Ret = MPI_VENC_OPEN(VeChn);
    if (s32Ret != HI_SUCCESS) return s32Ret;

    if (pstSSECfg == HI_NULL)
        return MpiVencCheckNull();

    if (pstSSECfg->u32Index >= 8)
        return MpiVencCheckSseIndex(pstSSECfg->u32Index);

    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_SET_SSE_REGION, pstSSECfg);
}

HI_S32
HI_MPI_VENC_GetSSERegion(VENC_CHN VeChn, HI_U32 u32Index, VENC_SSE_CFG_S *pstSSECfg)
{
    HI_S32 s32Ret;

    if (VeChn > (VENC_MAX_CHN_NUM - 1))
        return MpiVencCheckChn(VeChn);

    s32Ret = MPI_VENC_OPEN(VeChn);
    if (s32Ret != HI_SUCCESS) return s32Ret;

    if (pstSSECfg == HI_NULL)
        return MpiVencCheckNull();

    if (u32Index >= 8)
        return MpiVencCheckSseIndex(u32Index);

    pstSSECfg->u32Index = u32Index;

    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_GET_SSE_REGION, pstSSECfg);
}

HI_S32
HI_MPI_VENC_SetModParam(const VENC_PARAM_MOD_S *pstModParam)
{
    HI_S32 fd;
    HI_S32 s32Ret;

    if (pstModParam == HI_NULL)
        return MpiVencCheckNull();

    fd = open(VENC_DEV_NAME, O_RDWR, 0);
    if (fd < 0) {
        fprintf(stderr, "open venc err\n");
        return HI_ERR_VENC_SYS_NOTREADY;
    }

    s32Ret = ioctl(fd, VENC_CTL_SET_MOD_PARAM, pstModParam);
    close(fd);
    return s32Ret;
}

HI_S32
HI_MPI_VENC_GetModParam(VENC_PARAM_MOD_S *pstModParam)
{
    HI_S32 fd;
    HI_S32 s32Ret;

    if (pstModParam == HI_NULL)
        return MpiVencCheckNull();

    fd = open(VENC_DEV_NAME, O_RDWR, 0);
    if (fd < 0) {
        fprintf(stderr, "open venc err\n");
        return HI_ERR_VENC_SYS_NOTREADY;
    }

    s32Ret = ioctl(fd, VENC_CTL_GET_MOD_PARAM, pstModParam);
    close(fd);
    return s32Ret;
}

HI_S32
HI_MPI_VENC_GetStreamBufInfo(VENC_CHN VeChn, VENC_STREAM_BUF_INFO_S *pstStreamBufInfo)
{
    HI_S32 s32Ret;
    VENC_STREAM_BUF_INFO_KERN_S stBufInfo;
    VENC_CHN_CTX_S *pCtx;

    if (VeChn > (VENC_MAX_CHN_NUM - 1))
        return MpiVencCheckChn(VeChn);

    s32Ret = MPI_VENC_OPEN(VeChn);
    if (s32Ret != HI_SUCCESS) return s32Ret;

    if (pstStreamBufInfo == HI_NULL)
        return MpiVencCheckNull();

    s32Ret = MpiVencCheckMemFd(VeChn);
    if (s32Ret != HI_SUCCESS) return s32Ret;

    pCtx = &g_stMpiVencChn[VeChn];

    pthread_mutex_lock(&pCtx->mutex);

    s32Ret = ioctl(pCtx->s32Fd, VENC_CTL_GET_STREAM_BUF_INFO, &stBufInfo);
    if (s32Ret != HI_SUCCESS) {
        pthread_mutex_unlock(&pCtx->mutex);
        return s32Ret;
    }

    /* Update context if needed */
    if (pCtx->pUserAddr == HI_NULL) {
        pCtx->u64PhyAddr  = stBufInfo.u64PhyAddr;
        pCtx->u64VirtAddr = stBufInfo.u64VirtAddr;
        pCtx->u64BufLen   = (HI_U64)stBufInfo.u32BufLen;
        pCtx->u32CodingType = stBufInfo.u32Field14;
        pCtx->u32PackCnt  = stBufInfo.u32Field18;
        pCtx->pUserAddr   = HI_NULL;
        pCtx->reserved_34 = 0;

        memmap_venc(VeChn);
    }

    pstStreamBufInfo->u64PhyAddr[0]  = pCtx->u64PhyAddr;
    pstStreamBufInfo->pUserAddr[0]   = pCtx->pUserAddr;
    pstStreamBufInfo->u64BufSize[0]  = pCtx->u64BufLen;

    pthread_mutex_unlock(&pCtx->mutex);
    return HI_SUCCESS;
}

HI_S32
HI_MPI_VENC_AttachVbPool(VENC_CHN VeChn, const VENC_CHN_POOL_S *pstPool)
{
    HI_S32 s32Ret;

    if (VeChn > (VENC_MAX_CHN_NUM - 1))
        return MpiVencCheckChn(VeChn);

    s32Ret = MPI_VENC_OPEN(VeChn);
    if (s32Ret != HI_SUCCESS) return s32Ret;

    if (pstPool == HI_NULL)
        return MpiVencCheckNull();

    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_ATTACH_VB_POOL, pstPool);
}

HI_S32
HI_MPI_VENC_DetachVbPool(VENC_CHN VeChn)
{
    HI_S32 s32Ret;

    if (VeChn > (VENC_MAX_CHN_NUM - 1))
        return MpiVencCheckChn(VeChn);

    s32Ret = MPI_VENC_OPEN(VeChn);
    if (s32Ret != HI_SUCCESS) return s32Ret;

    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_DETACH_VB_POOL);
}

// ============================================================================
// Tier 1: Simple Set/Get pairs
// ============================================================================

/* RC params */

HI_S32
HI_MPI_VENC_SetRcParam(VENC_CHN VeChn, const VENC_RC_PARAM_S *pstRcParam)
{
    HI_S32 s32Ret;

    if (VeChn > (VENC_MAX_CHN_NUM - 1))
        return MpiVencCheckChn(VeChn);

    s32Ret = MPI_VENC_OPEN(VeChn);
    if (s32Ret != HI_SUCCESS) return s32Ret;

    if (pstRcParam == HI_NULL)
        return MpiVencCheckNull();

    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_SET_RC_PARAM, pstRcParam);
}

HI_S32
HI_MPI_VENC_GetRcParam(VENC_CHN VeChn, VENC_RC_PARAM_S *pstRcParam)
{
    HI_S32 s32Ret;

    if (VeChn > (VENC_MAX_CHN_NUM - 1))
        return MpiVencCheckChn(VeChn);

    s32Ret = MPI_VENC_OPEN(VeChn);
    if (s32Ret != HI_SUCCESS) return s32Ret;

    if (pstRcParam == HI_NULL)
        return MpiVencCheckNull();

    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_GET_RC_PARAM, pstRcParam);
}

/* H.264 params */

HI_S32
HI_MPI_VENC_SetH264SliceSplit(VENC_CHN VeChn, const VENC_H264_SLICE_SPLIT_S *pstSliceSplit)
{
    HI_S32 s32Ret;

    if (VeChn > (VENC_MAX_CHN_NUM - 1))
        return MpiVencCheckChn(VeChn);

    s32Ret = MPI_VENC_OPEN(VeChn);
    if (s32Ret != HI_SUCCESS) return s32Ret;

    if (pstSliceSplit == HI_NULL)
        return MpiVencCheckNull();

    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_SET_H264_SLICE_SPLIT, pstSliceSplit);
}

HI_S32
HI_MPI_VENC_GetH264SliceSplit(VENC_CHN VeChn, VENC_H264_SLICE_SPLIT_S *pstSliceSplit)
{
    HI_S32 s32Ret;

    if (VeChn > (VENC_MAX_CHN_NUM - 1))
        return MpiVencCheckChn(VeChn);

    s32Ret = MPI_VENC_OPEN(VeChn);
    if (s32Ret != HI_SUCCESS) return s32Ret;

    if (pstSliceSplit == HI_NULL)
        return MpiVencCheckNull();

    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_GET_H264_SLICE_SPLIT, pstSliceSplit);
}

HI_S32
HI_MPI_VENC_SetH264IntraPred(VENC_CHN VeChn, const VENC_H264_INTRA_PRED_S *pstH264IntraPred)
{
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn);
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstH264IntraPred == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_SET_H264_INTRA_PRED, pstH264IntraPred);
}

HI_S32
HI_MPI_VENC_GetH264IntraPred(VENC_CHN VeChn, VENC_H264_INTRA_PRED_S *pstH264IntraPred)
{
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn);
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstH264IntraPred == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_GET_H264_INTRA_PRED, pstH264IntraPred);
}

HI_S32 HI_MPI_VENC_SetH264Trans(VENC_CHN VeChn, const VENC_H264_TRANS_S *pstH264Trans) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstH264Trans == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_SET_H264_TRANS, pstH264Trans);
}

HI_S32 HI_MPI_VENC_GetH264Trans(VENC_CHN VeChn, VENC_H264_TRANS_S *pstH264Trans) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstH264Trans == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_GET_H264_TRANS, pstH264Trans);
}

HI_S32 HI_MPI_VENC_SetH264Entropy(VENC_CHN VeChn, const VENC_H264_ENTROPY_S *pstH264EntropyEnc) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstH264EntropyEnc == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_SET_H264_ENTROPY, pstH264EntropyEnc);
}

HI_S32 HI_MPI_VENC_GetH264Entropy(VENC_CHN VeChn, VENC_H264_ENTROPY_S *pstH264EntropyEnc) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstH264EntropyEnc == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_GET_H264_ENTROPY, pstH264EntropyEnc);
}

HI_S32 HI_MPI_VENC_SetH264Dblk(VENC_CHN VeChn, const VENC_H264_DBLK_S *pstH264Dblk) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstH264Dblk == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_SET_H264_DBLK, pstH264Dblk);
}

HI_S32 HI_MPI_VENC_GetH264Dblk(VENC_CHN VeChn, VENC_H264_DBLK_S *pstH264Dblk) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstH264Dblk == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_GET_H264_DBLK, pstH264Dblk);
}

HI_S32 HI_MPI_VENC_SetH264Vui(VENC_CHN VeChn, const VENC_H264_VUI_S *pstH264Vui) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstH264Vui == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_SET_H264_VUI, pstH264Vui);
}

HI_S32 HI_MPI_VENC_GetH264Vui(VENC_CHN VeChn, VENC_H264_VUI_S *pstH264Vui) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstH264Vui == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_GET_H264_VUI, pstH264Vui);
}

/* H.265 params */

HI_S32 HI_MPI_VENC_SetH265SliceSplit(VENC_CHN VeChn, const VENC_H265_SLICE_SPLIT_S *pstSliceSplit) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstSliceSplit == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_SET_H265_SLICE_SPLIT, pstSliceSplit);
}

HI_S32 HI_MPI_VENC_GetH265SliceSplit(VENC_CHN VeChn, VENC_H265_SLICE_SPLIT_S *pstSliceSplit) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstSliceSplit == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_GET_H265_SLICE_SPLIT, pstSliceSplit);
}

HI_S32 HI_MPI_VENC_SetH265PredUnit(VENC_CHN VeChn, const VENC_H265_PU_S *pstPredUnit) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstPredUnit == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_SET_H265_PRED_UNIT, pstPredUnit);
}

HI_S32 HI_MPI_VENC_GetH265PredUnit(VENC_CHN VeChn, VENC_H265_PU_S *pstPredUnit) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstPredUnit == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_GET_H265_PRED_UNIT, pstPredUnit);
}

HI_S32 HI_MPI_VENC_SetH265Trans(VENC_CHN VeChn, const VENC_H265_TRANS_S *pstH265Trans) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstH265Trans == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_SET_H265_TRANS, pstH265Trans);
}

HI_S32 HI_MPI_VENC_GetH265Trans(VENC_CHN VeChn, VENC_H265_TRANS_S *pstH265Trans) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstH265Trans == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_GET_H265_TRANS, pstH265Trans);
}

HI_S32 HI_MPI_VENC_SetH265Entropy(VENC_CHN VeChn, const VENC_H265_ENTROPY_S *pstH265Entropy) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstH265Entropy == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_SET_H265_ENTROPY, pstH265Entropy);
}

HI_S32 HI_MPI_VENC_GetH265Entropy(VENC_CHN VeChn, VENC_H265_ENTROPY_S *pstH265Entropy) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstH265Entropy == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_GET_H265_ENTROPY, pstH265Entropy);
}

HI_S32 HI_MPI_VENC_SetH265Dblk(VENC_CHN VeChn, const VENC_H265_DBLK_S *pstH265Dblk) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstH265Dblk == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_SET_H265_DBLK, pstH265Dblk);
}

HI_S32 HI_MPI_VENC_GetH265Dblk(VENC_CHN VeChn, VENC_H265_DBLK_S *pstH265Dblk) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstH265Dblk == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_GET_H265_DBLK, pstH265Dblk);
}

HI_S32 HI_MPI_VENC_SetH265Sao(VENC_CHN VeChn, const VENC_H265_SAO_S *pstH265Sao) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstH265Sao == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_SET_H265_SAO, pstH265Sao);
}

HI_S32 HI_MPI_VENC_GetH265Sao(VENC_CHN VeChn, VENC_H265_SAO_S *pstH265Sao) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstH265Sao == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_GET_H265_SAO, pstH265Sao);
}

HI_S32 HI_MPI_VENC_SetH265Vui(VENC_CHN VeChn, const VENC_H265_VUI_S *pstH265Vui) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstH265Vui == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_SET_H265_VUI, pstH265Vui);
}

HI_S32 HI_MPI_VENC_GetH265Vui(VENC_CHN VeChn, VENC_H265_VUI_S *pstH265Vui) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstH265Vui == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_GET_H265_VUI, pstH265Vui);
}

/* JPEG/MJPEG params */

HI_S32 HI_MPI_VENC_SetJpegParam(VENC_CHN VeChn, const VENC_JPEG_PARAM_S *pstJpegParam) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstJpegParam == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_SET_JPEG_PARAM, pstJpegParam);
}

HI_S32 HI_MPI_VENC_GetJpegParam(VENC_CHN VeChn, VENC_JPEG_PARAM_S *pstJpegParam) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstJpegParam == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_GET_JPEG_PARAM, pstJpegParam);
}

HI_S32 HI_MPI_VENC_SetMjpegParam(VENC_CHN VeChn, const VENC_MJPEG_PARAM_S *pstMjpegParam) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstMjpegParam == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_SET_MJPEG_PARAM, pstMjpegParam);
}

HI_S32 HI_MPI_VENC_GetMjpegParam(VENC_CHN VeChn, VENC_MJPEG_PARAM_S *pstMjpegParam) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstMjpegParam == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_GET_MJPEG_PARAM, pstMjpegParam);
}

/* JPEG encode mode */

HI_S32 HI_MPI_VENC_SetJpegEncodeMode(VENC_CHN VeChn, const VENC_JPEG_ENCODE_MODE_E enJpegEncodeMode) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_SET_JPEG_ENC_MODE, &enJpegEncodeMode);
}

HI_S32 HI_MPI_VENC_GetJpegEncodeMode(VENC_CHN VeChn, VENC_JPEG_ENCODE_MODE_E *penJpegEncodeMode) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (penJpegEncodeMode == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_GET_JPEG_ENC_MODE, penJpegEncodeMode);
}

/* Ref params */

HI_S32 HI_MPI_VENC_SetRefParam(VENC_CHN VeChn, const VENC_REF_PARAM_S *pstRefParam) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstRefParam == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_SET_REF_PARAM, pstRefParam);
}

HI_S32 HI_MPI_VENC_GetRefParam(VENC_CHN VeChn, VENC_REF_PARAM_S *pstRefParam) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstRefParam == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_GET_REF_PARAM, pstRefParam);
}

/* EnableIDR */

HI_S32 HI_MPI_VENC_EnableIDR(VENC_CHN VeChn, HI_BOOL bEnableIDR) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_ENABLE_IDR, &bEnableIDR);
}

/* ROI BG frame rate */

HI_S32 HI_MPI_VENC_SetRoiBgFrameRate(VENC_CHN VeChn, const VENC_ROIBG_FRAME_RATE_S *pstRoiBgFrmRate) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstRoiBgFrmRate == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_SET_ROI_BG_FR, pstRoiBgFrmRate);
}

HI_S32 HI_MPI_VENC_GetRoiBgFrameRate(VENC_CHN VeChn, VENC_ROIBG_FRAME_RATE_S *pstRoiBgFrmRate) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstRoiBgFrmRate == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_GET_ROI_BG_FR, pstRoiBgFrmRate);
}

/* Frame lost / super frame / intra refresh */

HI_S32 HI_MPI_VENC_SetFrameLostStrategy(VENC_CHN VeChn, const VENC_FRAMELOST_S *pstFrmLostParam) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstFrmLostParam == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_SET_FRAME_LOST, pstFrmLostParam);
}

HI_S32 HI_MPI_VENC_GetFrameLostStrategy(VENC_CHN VeChn, VENC_FRAMELOST_S *pstFrmLostParam) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstFrmLostParam == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_GET_FRAME_LOST, pstFrmLostParam);
}

HI_S32 HI_MPI_VENC_SetSuperFrameStrategy(VENC_CHN VeChn, const VENC_SUPERFRAME_CFG_S *pstSuperFrmParam) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstSuperFrmParam == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_SET_SUPER_FRAME, pstSuperFrmParam);
}

HI_S32 HI_MPI_VENC_GetSuperFrameStrategy(VENC_CHN VeChn, VENC_SUPERFRAME_CFG_S *pstSuperFrmParam) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstSuperFrmParam == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_GET_SUPER_FRAME, pstSuperFrmParam);
}

HI_S32 HI_MPI_VENC_SetIntraRefresh(VENC_CHN VeChn, const VENC_INTRA_REFRESH_S *pstIntraRefresh) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstIntraRefresh == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_SET_INTRA_REFRESH, pstIntraRefresh);
}

HI_S32 HI_MPI_VENC_GetIntraRefresh(VENC_CHN VeChn, VENC_INTRA_REFRESH_S *pstIntraRefresh) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstIntraRefresh == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_GET_INTRA_REFRESH, pstIntraRefresh);
}

/* Channel config / scene mode */

HI_S32 HI_MPI_VENC_SetChnParam(VENC_CHN VeChn, const VENC_CHN_PARAM_S *pstChnParam) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstChnParam == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_SET_CHN_PARAM, pstChnParam);
}

HI_S32 HI_MPI_VENC_GetChnParam(VENC_CHN VeChn, VENC_CHN_PARAM_S *pstChnParam) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstChnParam == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_GET_CHN_PARAM, pstChnParam);
}

HI_S32 HI_MPI_VENC_SetSceneMode(VENC_CHN VeChn, const VENC_SCENE_MODE_E enSceneMode) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_SET_SCENE_MODE, &enSceneMode);
}

HI_S32 HI_MPI_VENC_GetSceneMode(VENC_CHN VeChn, VENC_SCENE_MODE_E *penSceneMode) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (penSceneMode == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_GET_SCENE_MODE, penSceneMode);
}

/* Foreground protect */

HI_S32 HI_MPI_VENC_SetForegroundProtect(VENC_CHN VeChn, const VENC_FOREGROUND_PROTECT_S *pstForegroundProtect) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstForegroundProtect == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_SET_FG_PROTECT, pstForegroundProtect);
}

HI_S32 HI_MPI_VENC_GetForegroundProtect(VENC_CHN VeChn, VENC_FOREGROUND_PROTECT_S *pstForegroundProtect) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstForegroundProtect == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_GET_FG_PROTECT, pstForegroundProtect);
}

/* DeBreath / CuPrediction / SkipBias / HierarchicalQp / RcAdvParam */

HI_S32 HI_MPI_VENC_SetDeBreathEffect(VENC_CHN VeChn, const VENC_DEBREATHEFFECT_S *pstDeBreathEffect) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstDeBreathEffect == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_SET_DEBREATH, pstDeBreathEffect);
}

HI_S32 HI_MPI_VENC_GetDeBreathEffect(VENC_CHN VeChn, VENC_DEBREATHEFFECT_S *pstDeBreathEffect) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstDeBreathEffect == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_GET_DEBREATH, pstDeBreathEffect);
}

HI_S32 HI_MPI_VENC_SetCuPrediction(VENC_CHN VeChn, const VENC_CU_PREDICTION_S *pstCuPrediction) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstCuPrediction == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_SET_CU_PREDICTION, pstCuPrediction);
}

HI_S32 HI_MPI_VENC_GetCuPrediction(VENC_CHN VeChn, VENC_CU_PREDICTION_S *pstCuPrediction) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstCuPrediction == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_GET_CU_PREDICTION, pstCuPrediction);
}

HI_S32 HI_MPI_VENC_SetSkipBias(VENC_CHN VeChn, const VENC_SKIP_BIAS_S *pstSkipBias) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstSkipBias == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_SET_SKIP_BIAS, pstSkipBias);
}

HI_S32 HI_MPI_VENC_GetSkipBias(VENC_CHN VeChn, VENC_SKIP_BIAS_S *pstSkipBias) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstSkipBias == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_GET_SKIP_BIAS, pstSkipBias);
}

HI_S32 HI_MPI_VENC_SetHierarchicalQp(VENC_CHN VeChn, const VENC_HIERARCHICAL_QP_S *pstHierarchicalQp) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstHierarchicalQp == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_SET_HIERARCHICAL_QP, pstHierarchicalQp);
}

HI_S32 HI_MPI_VENC_GetHierarchicalQp(VENC_CHN VeChn, VENC_HIERARCHICAL_QP_S *pstHierarchicalQp) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstHierarchicalQp == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_GET_HIERARCHICAL_QP, pstHierarchicalQp);
}

HI_S32 HI_MPI_VENC_SetRcAdvParam(VENC_CHN VeChn, const VENC_RC_ADVPARAM_S *pstRcAdvParam) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstRcAdvParam == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_SET_RC_ADV_PARAM, pstRcAdvParam);
}

HI_S32 HI_MPI_VENC_GetRcAdvParam(VENC_CHN VeChn, VENC_RC_ADVPARAM_S *pstRcAdvParam) {
    HI_S32 s32Ret;
    if (VeChn > (VENC_MAX_CHN_NUM - 1)) return MpiVencCheckChn(VeChn);
    s32Ret = MPI_VENC_OPEN(VeChn); if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstRcAdvParam == HI_NULL) return MpiVencCheckNull();
    return ioctl(g_stMpiVencChn[VeChn].s32Fd, VENC_CTL_GET_RC_ADV_PARAM, pstRcAdvParam);
}
