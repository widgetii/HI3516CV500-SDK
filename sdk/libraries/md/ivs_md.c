/**
 * Reverse Engineered by TekuConcept
 * MD (Motion Detection) Main Implementation
 */

#include "re_ivs_md.h"

/* ========================================================================== */
/* Global State                                                               */
/* ========================================================================== */

static MD_CHN_CTX_S    s_astMdChnCtx[MD_MAX_CHN];
static pthread_mutex_t s_astMdChnMutex[MD_MAX_CHN];
static HI_U32          s_enMdState;
static pthread_mutex_t s_stMdMutex = PTHREAD_MUTEX_INITIALIZER;

/* Proc stat info: [0]=virAddr, [2]=phyAddr(u64), [4]=size */
static HI_U8 s_stStatInfo[24];

#define STAT_VIR_ADDR  (*(HI_VOID **)&s_stStatInfo[0])
#define STAT_PHY_ADDR  (*(HI_U64 *)&s_stStatInfo[8])
#define STAT_SIZE      (*(HI_U32 *)&s_stStatInfo[16])

#define ALIGN_UP(x, a) (((x) + ((a) - 1)) & ~((a) - 1))

/* ========================================================================== */
/* Internal: MD_ConvertObj                                                    */
/* ========================================================================== */

/*
 * Post-process CCL output: scale coordinates by step, compact valid entries.
 * CCL output is 254 entries of 12 bytes each:
 *   [0-3]: area (u32)
 *   [4-5]: left (u16)
 *   [6-7]: top (u16)
 *   [8-9]: right (u16)
 *   [10-11]: bottom (u16)
 * Returns count of valid (non-zero area) objects.
 */
static HI_U8 MD_ConvertObj(HI_VOID *pData, HI_U16 u16Step)
{
    HI_U8 *pEntry = (HI_U8 *)pData;
    HI_U32 u32AreaScale = (HI_U32)u16Step * (HI_U32)u16Step;
    HI_U8 u8DstIdx = 0;
    HI_U32 i;

    for (i = 0; i < 254; i++) {
        HI_U32 *pArea = (HI_U32 *)pEntry;
        HI_U16 *pLeft   = (HI_U16 *)(pEntry + 4);
        HI_U16 *pTop    = (HI_U16 *)(pEntry + 6);
        HI_U16 *pRight  = (HI_U16 *)(pEntry + 8);
        HI_U16 *pBottom = (HI_U16 *)(pEntry + 10);

        HI_U32 u32Area = *pArea * u32AreaScale;

        if (u32Area != 0) {
            /* Scale coordinates */
            HI_U16 u16Left   = *pLeft   * u16Step;
            HI_U16 u16Top    = *pTop    * u16Step;
            HI_U16 u16Right  = *pRight  * u16Step;
            HI_U16 u16Bottom = *pBottom * u16Step;

            *pArea   = u32Area;
            *pLeft   = u16Left;
            *pTop    = u16Top;
            *pRight  = u16Right;
            *pBottom = u16Bottom;

            /* Compact: if dst != src, copy and clear src */
            if ((HI_U8)i != u8DstIdx) {
                HI_U8 *pDst = (HI_U8 *)pData + (HI_S16)u8DstIdx * 12;
                memcpy_s(pDst, 12, pEntry, 12);
                memset_s(pEntry, 12, 0, 12);
            }
            u8DstIdx++;
        }

        pEntry += 12;
    }

    return u8DstIdx;
}

/* ========================================================================== */
/* Internal: HI_IVS_MD_WriteProc                                             */
/* ========================================================================== */

static HI_VOID HI_IVS_MD_WriteProc(HI_VOID)
{
    HI_U32 i;
    HI_CHAR *pVirAddr = (HI_CHAR *)STAT_VIR_ADDR;
    HI_U32 u32Size = STAT_SIZE;
    HI_U32 u32Offset = 0;

    if (pVirAddr == NULL || u32Size == 0)
        return;

    for (i = 0; i < MD_MAX_CHN; i++) {
        MD_CHN_CTX_S *pCtx = &s_astMdChnCtx[i];
        if (!pCtx->bCreated)
            continue;

        u32Offset += snprintf_s(pVirAddr + u32Offset, u32Size - u32Offset,
            u32Size - u32Offset - 1,
            "%3d%5d%5d%6d%10d%12d%12d%10d%10d%8d%8d%8d%10d%15u\n",
            i, pCtx->enAlgMode, pCtx->enSadMode, pCtx->enSadOutCtrl,
            pCtx->u32Width, pCtx->u32Height,
            pCtx->u32SadStride, pCtx->u32SadWidth, pCtx->u32SadHeight,
            pCtx->u16SadThr, pCtx->stCclCtrl.enMode,
            pCtx->stCclCtrl.u16Step, pCtx->stAddCtrl.u0q16X,
            pCtx->u32ProcCnt);
    }
}

/* ========================================================================== */
/* Internal: MD_DmaImage                                                      */
/* ========================================================================== */

static HI_S32 MD_DmaImage(IVE_IMAGE_S *pstSrc, IVE_IMAGE_S *pstDst)
{
    IVE_HANDLE hHandle;
    IVE_DMA_CTRL_S stDmaCtrl;
    HI_BOOL bFinish = HI_FALSE;
    HI_S32 s32Ret;
    struct timespec ts;

    memset_s(&stDmaCtrl, sizeof(stDmaCtrl), 0, sizeof(stDmaCtrl));
    stDmaCtrl.enMode = IVE_DMA_MODE_DIRECT_COPY;

    s32Ret = HI_MPI_IVE_DMA(&hHandle, (IVE_DATA_S *)pstSrc, (IVE_DST_DATA_S *)pstDst, &stDmaCtrl, HI_TRUE);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_MD(HI_DBG_ERR, "HI_MPI_IVE_DMA fail,Error(%#x)!\n", s32Ret);
        return s32Ret;
    }

    /* Poll until complete */
    ts.tv_sec = 0;
    ts.tv_nsec = 100000; /* 100 microseconds */

    do {
        s32Ret = HI_MPI_IVE_Query(hHandle, &bFinish, HI_TRUE);
        if (s32Ret == HI_ERR_IVE_QUERY_TIMEOUT) {
            nanosleep(&ts, NULL);
            continue;
        }
        if (s32Ret != HI_SUCCESS) {
            HI_TRACE_MD(HI_DBG_ERR, "HI_MPI_IVE_Query fail,Error(%#x)!\n", s32Ret);
            return s32Ret;
        }
    } while (!bFinish);

    return HI_SUCCESS;
}

/* ========================================================================== */
/* Internal: MD_UpdateBg                                                      */
/* ========================================================================== */

static HI_S32 MD_UpdateBg(IVE_IMAGE_S *pstBg, IVE_SRC_IMAGE_S *pstCur,
    IVE_ADD_CTRL_S *pstAddCtrl)
{
    IVE_HANDLE hHandle;
    HI_BOOL bFinish = HI_FALSE;
    HI_S32 s32Ret;
    struct timespec ts;

    s32Ret = HI_MPI_IVE_Add(&hHandle, pstBg, pstCur, pstBg, pstAddCtrl, HI_TRUE);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_MD(HI_DBG_ERR, "HI_MPI_IVE_Add fail,Error(%#x)!\n", s32Ret);
        return s32Ret;
    }

    ts.tv_sec = 0;
    ts.tv_nsec = 100000;

    do {
        s32Ret = HI_MPI_IVE_Query(hHandle, &bFinish, HI_TRUE);
        if (s32Ret == HI_ERR_IVE_QUERY_TIMEOUT) {
            nanosleep(&ts, NULL);
            continue;
        }
        if (s32Ret != HI_SUCCESS) {
            HI_TRACE_MD(HI_DBG_ERR, "HI_MPI_IVE_Query fail,Error(%#x)!\n", s32Ret);
            return s32Ret;
        }
    } while (!bFinish);

    return HI_SUCCESS;
}

/* ========================================================================== */
/* HI_IVS_MD_Init                                                            */
/* ========================================================================== */

HI_S32 HI_IVS_MD_Init(HI_VOID)
{
    HI_U64 u64PhyAddr = 0;
    HI_U32 u32Size = 0;
    HI_VOID *pVirAddr = NULL;
    HI_S32 s32Ret;
    HI_U32 i;

    pthread_mutex_lock(&s_stMdMutex);

    if (s_enMdState == 1) {
        HI_TRACE_MD(HI_DBG_ERR, "md already init!\n");
        pthread_mutex_unlock(&s_stMdMutex);
        return HI_ERR_MD_EXIST;
    }

    memset_s(s_stStatInfo, sizeof(s_stStatInfo), 0, sizeof(s_stStatInfo));

    s32Ret = MPI_IVE_MdProcInit(&u64PhyAddr, &u32Size);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_MD(HI_DBG_ERR, "md proc init failed,Error(%#x)!\n", s32Ret);
        pthread_mutex_unlock(&s_stMdMutex);
        return s32Ret;
    }

    pVirAddr = HI_MPI_SYS_Mmap(u64PhyAddr, u32Size);
    if (pVirAddr == NULL) {
        HI_TRACE_MD(HI_DBG_ERR, "mmap failed!\n");
        MPI_IVE_MdProcExit();
        pthread_mutex_unlock(&s_stMdMutex);
        return HI_ERR_IVE_NOMEM;
    }

    STAT_VIR_ADDR = pVirAddr;
    STAT_PHY_ADDR = u64PhyAddr;
    STAT_SIZE = u32Size;

    memset_s(s_astMdChnCtx, sizeof(s_astMdChnCtx), 0, sizeof(s_astMdChnCtx));

    for (i = 0; i < MD_MAX_CHN; i++)
        pthread_mutex_init(&s_astMdChnMutex[i], NULL);

    s_enMdState = 1;

    pthread_mutex_unlock(&s_stMdMutex);
    return HI_SUCCESS;
}

/* ========================================================================== */
/* HI_IVS_MD_Exit                                                            */
/* ========================================================================== */

HI_S32 HI_IVS_MD_Exit(HI_VOID)
{
    HI_S32 s32Ret;
    HI_U32 i;

    pthread_mutex_lock(&s_stMdMutex);

    if (s_enMdState != 1) {
        HI_TRACE_MD(HI_DBG_ERR, "md is not ready!\n");
        pthread_mutex_unlock(&s_stMdMutex);
        return HI_ERR_MD_NOT_PERM;
    }

    /* Check no channels are still created */
    for (i = 0; i < MD_MAX_CHN; i++) {
        if (s_astMdChnCtx[i].bCreated) {
            HI_TRACE_MD(HI_DBG_ERR, "md is busy!\n");
            pthread_mutex_unlock(&s_stMdMutex);
            return HI_ERR_MD_BUSY;
        }
    }

    /* Write final proc stats */
    s32Ret = MPI_IVE_MdProcBeginWrite();
    if (s32Ret != HI_SUCCESS)
        HI_TRACE_MD(HI_DBG_ERR, "md proc begin write failed,Error(%#x)!\n", s32Ret);
    else {
        HI_IVS_MD_WriteProc();
        MPI_IVE_MdProcEndWrite();
    }

    /* Unmap and exit proc */
    if (STAT_VIR_ADDR != NULL) {
        HI_MPI_SYS_Munmap(STAT_VIR_ADDR, STAT_SIZE);
        STAT_VIR_ADDR = NULL;
    }

    MPI_IVE_MdProcExit();

    for (i = 0; i < MD_MAX_CHN; i++)
        pthread_mutex_destroy(&s_astMdChnMutex[i]);

    s_enMdState = 0;

    pthread_mutex_unlock(&s_stMdMutex);
    return HI_SUCCESS;
}

/* ========================================================================== */
/* HI_IVS_MD_CreateChn                                                       */
/* ========================================================================== */

HI_S32 HI_IVS_MD_CreateChn(MD_CHN MdChn, MD_ATTR_S *pstMdAttr)
{
    MD_CHN_CTX_S *pCtx;
    HI_S32 s32Ret;
    HI_U32 u32BlockSize;
    HI_U32 u32SadWidth, u32SadHeight, u32SadStride;
    HI_U32 u32AllocSize;

    if (s_enMdState != 1) {
        HI_TRACE_MD(HI_DBG_ERR, "md is not ready!\n");
        return HI_ERR_MD_NOT_PERM;
    }

    if (pstMdAttr == NULL) {
        HI_TRACE_MD(HI_DBG_ERR, "pstMdAttr can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    if (MdChn < 0 || MdChn >= MD_MAX_CHN) {
        HI_TRACE_MD(HI_DBG_ERR, "MdChn(%d) must be in [%d,%d)!\n", MdChn, 0, MD_MAX_CHN);
        return HI_ERR_MD_INVALID_CHNID;
    }

    s32Ret = MD_CheckAttr(pstMdAttr);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_MD(HI_DBG_ERR, "check md attr fail!\n");
        return s32Ret;
    }

    pthread_mutex_lock(&s_astMdChnMutex[MdChn]);
    pCtx = &s_astMdChnCtx[MdChn];

    if (pCtx->bCreated) {
        HI_TRACE_MD(HI_DBG_ERR, "md chn(%d) has been created!\n", MdChn);
        pthread_mutex_unlock(&s_astMdChnMutex[MdChn]);
        return HI_ERR_MD_EXIST;
    }

    /* Calculate SAD output dimensions */
    u32BlockSize = 4 << pstMdAttr->enSadMode;
    u32SadWidth = pstMdAttr->u32Width / u32BlockSize;
    u32SadHeight = pstMdAttr->u32Height / u32BlockSize;
    u32SadStride = ALIGN_UP(u32SadWidth, 16);

    pCtx->u32Status = 0;
    pCtx->u32SadStride = u32SadStride;
    pCtx->u32SadWidth = u32SadWidth;
    pCtx->u32SadHeight = u32SadHeight;

    u32AllocSize = u32SadStride * u32SadHeight;

    if (pstMdAttr->enAlgMode == MD_ALG_MODE_BG) {
        /* BG mode: allocate background image + SAD output buffer */
        HI_U32 u32BgStride = ALIGN_UP(pstMdAttr->u32Width, 16);
        HI_U32 u32TotalSize;

        pCtx->bDualOutput = 0;
        pCtx->u32BgReserved = 0;
        pCtx->u32BgHeight = pstMdAttr->u32Height;
        pCtx->u32BgStride = u32BgStride;

        /* Total allocation: SAD buffer + BG image + BG ref buffer */
        u32TotalSize = u32AllocSize +
            u32BgStride * pstMdAttr->u32Height +
            u32BgStride * pstMdAttr->u32Height * 2;

        s32Ret = IveMalloc(&pCtx->stBgImage.au64PhyAddr[0],
            (HI_VOID **)&pCtx->stBgImage.au64VirAddr[0],
            "MD_ASSIST", u32TotalSize);
        if (s32Ret != HI_SUCCESS) {
            HI_TRACE_MD(HI_DBG_ERR, "malloc fail!\n");
            pthread_mutex_unlock(&s_astMdChnMutex[MdChn]);
            return HI_ERR_IVE_NOMEM;
        }

        pCtx->u32BgAllocated = 0;

        /* Partition the allocation */
        pCtx->u64SadPhyAddr = pCtx->stBgImage.au64PhyAddr[0] +
            u32SadStride * u32SadHeight;
        pCtx->u64SadVirAddr = pCtx->stBgImage.au64VirAddr[0] +
            u32SadStride * u32SadHeight;

        pCtx->u32BgRefWidth = pstMdAttr->u32Width;
        pCtx->u32BgRefHeight = pstMdAttr->u32Height;
        pCtx->u32BgRefStride = u32BgStride;

        pCtx->u64BgPhyAddr2 = pCtx->u64SadPhyAddr +
            u32BgStride * pstMdAttr->u32Height;
        pCtx->u64BgVirAddr2 = pCtx->u64SadVirAddr +
            u32BgStride * pstMdAttr->u32Height;

        /* Set up BG image struct */
        pCtx->stBgImage.enType = IVE_IMAGE_TYPE_U8C1;
        pCtx->stBgImage.u32Width = pstMdAttr->u32Width;
        pCtx->stBgImage.u32Height = pstMdAttr->u32Height;
        pCtx->stBgImage.au32Stride[0] = u32BgStride;
    } else {
        /* REF mode: allocate only SAD output buffer */
        s32Ret = IveMalloc(&pCtx->stBgImage.au64PhyAddr[0],
            (HI_VOID **)&pCtx->stBgImage.au64VirAddr[0],
            "MD_ASSIST", u32AllocSize);
        if (s32Ret != HI_SUCCESS) {
            HI_TRACE_MD(HI_DBG_ERR, "malloc fail!\n");
            pthread_mutex_unlock(&s_astMdChnMutex[MdChn]);
            return HI_ERR_IVE_NOMEM;
        }
    }

    /* Store attributes in context */
    pCtx->enAlgMode = pstMdAttr->enAlgMode;
    pCtx->enSadMode = pstMdAttr->enSadMode;
    pCtx->enSadOutCtrl = pstMdAttr->enSadOutCtrl;
    pCtx->u32Width = pstMdAttr->u32Width;
    pCtx->u32Height = pstMdAttr->u32Height;
    memcpy_s(&pCtx->stAddCtrl, sizeof(IVE_ADD_CTRL_S),
        &pstMdAttr->stAddCtrl, sizeof(IVE_ADD_CTRL_S));
    memcpy_s(&pCtx->stCclCtrl, sizeof(IVE_CCL_CTRL_S),
        &pstMdAttr->stCclCtrl, sizeof(IVE_CCL_CTRL_S));

    pCtx->bCreated = 1;
    pCtx->bFirstFrame = 1;
    pCtx->u16SadThr = pstMdAttr->u16SadThr;
    pCtx->u32ProcCnt = 0;
    pCtx->u32TotalTime = 0;
    pCtx->u32MaxTime = 0;
    pCtx->u32MinTime = 0;

    /* Update proc */
    s32Ret = MPI_IVE_MdProcBeginWrite();
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_MD(HI_DBG_ERR, "md proc begin write failed,Error(%#x)!\n", s32Ret);
    } else {
        HI_IVS_MD_WriteProc();
        s32Ret = MPI_IVE_MdProcEndWrite();
        if (s32Ret != HI_SUCCESS)
            HI_TRACE_MD(HI_DBG_ERR, "md proc end write failed,Error(%#x)!\n", s32Ret);
    }

    pthread_mutex_unlock(&s_astMdChnMutex[MdChn]);
    return HI_SUCCESS;
}

/* ========================================================================== */
/* HI_IVS_MD_DestroyChn                                                      */
/* ========================================================================== */

HI_S32 HI_IVS_MD_DestroyChn(MD_CHN MdChn)
{
    MD_CHN_CTX_S *pCtx;

    if (s_enMdState != 1) {
        HI_TRACE_MD(HI_DBG_ERR, "md is not ready!\n");
        return HI_ERR_MD_NOT_PERM;
    }

    if (MdChn < 0 || MdChn >= MD_MAX_CHN) {
        HI_TRACE_MD(HI_DBG_ERR, "MdChn(%d) must be in [%d,%d)!\n", MdChn, 0, MD_MAX_CHN);
        return HI_ERR_MD_INVALID_CHNID;
    }

    pthread_mutex_lock(&s_astMdChnMutex[MdChn]);
    pCtx = &s_astMdChnCtx[MdChn];

    if (!pCtx->bCreated) {
        HI_TRACE_MD(HI_DBG_ERR, "md chn(%d) does not exist!\n", MdChn);
        pthread_mutex_unlock(&s_astMdChnMutex[MdChn]);
        return HI_ERR_MD_UNEXIST;
    }

    /* Free allocated buffer */
    IveFree(pCtx->stBgImage.au64PhyAddr[0],
        (HI_VOID *)(HI_UL)pCtx->stBgImage.au64VirAddr[0]);

    /* Update proc */
    MPI_IVE_MdProcBeginWrite();
    HI_IVS_MD_WriteProc();
    MPI_IVE_MdProcEndWrite();

    memset_s(pCtx, sizeof(MD_CHN_CTX_S), 0, sizeof(MD_CHN_CTX_S));

    pthread_mutex_unlock(&s_astMdChnMutex[MdChn]);
    return HI_SUCCESS;
}

/* ========================================================================== */
/* HI_IVS_MD_SetChnAttr                                                      */
/* ========================================================================== */

HI_S32 HI_IVS_MD_SetChnAttr(MD_CHN MdChn, MD_ATTR_S *pstMdAttr)
{
    MD_CHN_CTX_S *pCtx;
    HI_S32 s32Ret;

    if (s_enMdState != 1) {
        HI_TRACE_MD(HI_DBG_ERR, "md is not ready!\n");
        return HI_ERR_MD_NOT_PERM;
    }

    if (pstMdAttr == NULL) {
        HI_TRACE_MD(HI_DBG_ERR, "pstMdAttr can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    if (MdChn < 0 || MdChn >= MD_MAX_CHN) {
        HI_TRACE_MD(HI_DBG_ERR, "MdChn(%d) must be in [%d,%d)!\n", MdChn, 0, MD_MAX_CHN);
        return HI_ERR_MD_INVALID_CHNID;
    }

    s32Ret = MD_CheckAttr(pstMdAttr);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_MD(HI_DBG_ERR, "check md attr fail!\n");
        return s32Ret;
    }

    pthread_mutex_lock(&s_astMdChnMutex[MdChn]);
    pCtx = &s_astMdChnCtx[MdChn];

    if (!pCtx->bCreated) {
        HI_TRACE_MD(HI_DBG_ERR, "md chn(%d) does not exist!\n", MdChn);
        pthread_mutex_unlock(&s_astMdChnMutex[MdChn]);
        return HI_ERR_MD_UNEXIST;
    }

    /* Warn about static attribute changes */
    if (pCtx->enAlgMode != pstMdAttr->enAlgMode)
        HI_TRACE_MD(HI_DBG_WARN,
            "md chn(%d) static attr(alg mode(%d)->(%d) be changed!\n",
            MdChn, pCtx->enAlgMode, pstMdAttr->enAlgMode);

    if (pCtx->enSadMode != pstMdAttr->enSadMode)
        HI_TRACE_MD(HI_DBG_WARN,
            "md chn(%d) static attr(sad mode(%d)->(%d) be changed!\n",
            MdChn, pCtx->enSadMode, pstMdAttr->enSadMode);

    if (pCtx->u32Width != pstMdAttr->u32Width)
        HI_TRACE_MD(HI_DBG_WARN,
            "md chn(%d) static attr(width(%d)->(%d) be changed!\n",
            MdChn, pCtx->u32Width, pstMdAttr->u32Width);

    if (pCtx->u32Height != pstMdAttr->u32Height)
        HI_TRACE_MD(HI_DBG_WARN,
            "md chn(%d) static attr(height(%d)->(%d) be changed!\n",
            MdChn, pCtx->u32Height, pstMdAttr->u32Height);

    /* Update mutable attributes */
    pCtx->u16SadThr = pstMdAttr->u16SadThr;
    pCtx->enSadOutCtrl = pstMdAttr->enSadOutCtrl;
    memcpy_s(&pCtx->stCclCtrl, sizeof(IVE_CCL_CTRL_S),
        &pstMdAttr->stCclCtrl, sizeof(IVE_CCL_CTRL_S));
    memcpy_s(&pCtx->stAddCtrl, sizeof(IVE_ADD_CTRL_S),
        &pstMdAttr->stAddCtrl, sizeof(IVE_ADD_CTRL_S));

    /* Update proc */
    MPI_IVE_MdProcBeginWrite();
    HI_IVS_MD_WriteProc();
    MPI_IVE_MdProcEndWrite();

    pthread_mutex_unlock(&s_astMdChnMutex[MdChn]);
    return HI_SUCCESS;
}

/* ========================================================================== */
/* HI_IVS_MD_GetChnAttr                                                      */
/* ========================================================================== */

HI_S32 HI_IVS_MD_GetChnAttr(MD_CHN MdChn, MD_ATTR_S *pstMdAttr)
{
    MD_CHN_CTX_S *pCtx;

    if (s_enMdState != 1) {
        HI_TRACE_MD(HI_DBG_ERR, "md is not ready!\n");
        return HI_ERR_MD_NOT_PERM;
    }

    if (pstMdAttr == NULL) {
        HI_TRACE_MD(HI_DBG_ERR, "pstMdAttr can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    if (MdChn < 0 || MdChn >= MD_MAX_CHN) {
        HI_TRACE_MD(HI_DBG_ERR, "MdChn(%d) must be in [%d,%d)!\n", MdChn, 0, MD_MAX_CHN);
        return HI_ERR_MD_INVALID_CHNID;
    }

    pthread_mutex_lock(&s_astMdChnMutex[MdChn]);
    pCtx = &s_astMdChnCtx[MdChn];

    if (!pCtx->bCreated) {
        HI_TRACE_MD(HI_DBG_ERR, "md chn(%d) does not exist!\n", MdChn);
        pthread_mutex_unlock(&s_astMdChnMutex[MdChn]);
        return HI_ERR_MD_UNEXIST;
    }

    pstMdAttr->enAlgMode = pCtx->enAlgMode;
    pstMdAttr->enSadMode = pCtx->enSadMode;
    pstMdAttr->enSadOutCtrl = pCtx->enSadOutCtrl;
    pstMdAttr->u32Width = pCtx->u32Width;
    pstMdAttr->u32Height = pCtx->u32Height;
    pstMdAttr->u16SadThr = pCtx->u16SadThr;
    memcpy_s(&pstMdAttr->stCclCtrl, sizeof(IVE_CCL_CTRL_S),
        &pCtx->stCclCtrl, sizeof(IVE_CCL_CTRL_S));
    memcpy_s(&pstMdAttr->stAddCtrl, sizeof(IVE_ADD_CTRL_S),
        &pCtx->stAddCtrl, sizeof(IVE_ADD_CTRL_S));

    pthread_mutex_unlock(&s_astMdChnMutex[MdChn]);
    return HI_SUCCESS;
}

/* ========================================================================== */
/* HI_IVS_MD_GetBg                                                           */
/* ========================================================================== */

HI_S32 HI_IVS_MD_GetBg(MD_CHN MdChn, IVE_DST_IMAGE_S *pstBg)
{
    MD_CHN_CTX_S *pCtx;
    HI_S32 s32Ret;

    if (s_enMdState != 1) {
        HI_TRACE_MD(HI_DBG_ERR, "md is not ready!\n");
        return HI_ERR_MD_NOT_PERM;
    }

    if (pstBg == NULL) {
        HI_TRACE_MD(HI_DBG_ERR, "pstBg can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    if (MdChn < 0 || MdChn >= MD_MAX_CHN) {
        HI_TRACE_MD(HI_DBG_ERR, "MdChn(%d) must be in [%d,%d)!\n", MdChn, 0, MD_MAX_CHN);
        return HI_ERR_MD_INVALID_CHNID;
    }

    pthread_mutex_lock(&s_astMdChnMutex[MdChn]);
    pCtx = &s_astMdChnCtx[MdChn];

    if (!pCtx->bCreated) {
        HI_TRACE_MD(HI_DBG_ERR, "md chn(%d) does not exist!\n", MdChn);
        pthread_mutex_unlock(&s_astMdChnMutex[MdChn]);
        return HI_ERR_MD_UNEXIST;
    }

    if (pCtx->enAlgMode != MD_ALG_MODE_BG) {
        HI_TRACE_MD(HI_DBG_ERR, "md alg mode(%d) must be %d!\n",
            pCtx->enAlgMode, MD_ALG_MODE_BG);
        pthread_mutex_unlock(&s_astMdChnMutex[MdChn]);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }

    s32Ret = MdCheckImageUser(pstBg, 64, 1920, 64, 1080, 16, 1);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_MD(HI_DBG_ERR, "check image fail,Error(%#x)!\n", s32Ret);
        pthread_mutex_unlock(&s_astMdChnMutex[MdChn]);
        return s32Ret;
    }

    if (pstBg->enType != IVE_IMAGE_TYPE_U8C1) {
        HI_TRACE_MD(HI_DBG_ERR, "pstBg->enType(%d) must be U8C1(%d)!\n",
            pstBg->enType, IVE_IMAGE_TYPE_U8C1);
        pthread_mutex_unlock(&s_astMdChnMutex[MdChn]);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }

    s32Ret = MD_DmaImage(&pCtx->stBgImage, pstBg);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_MD(HI_DBG_ERR, "dma fail,Error(%#x)!\n", s32Ret);
        pthread_mutex_unlock(&s_astMdChnMutex[MdChn]);
        return s32Ret;
    }

    pthread_mutex_unlock(&s_astMdChnMutex[MdChn]);
    return HI_SUCCESS;
}

/* ========================================================================== */
/* HI_IVS_MD_Process                                                         */
/* ========================================================================== */

HI_S32 HI_IVS_MD_Process(MD_CHN MdChn, IVE_SRC_IMAGE_S *pstCur,
    IVE_SRC_IMAGE_S *pstRef, IVE_DST_IMAGE_S *pstSad,
    IVE_DST_MEM_INFO_S *pstBlob)
{
    MD_CHN_CTX_S *pCtx;
    IVE_HANDLE hHandle;
    IVE_SAD_CTRL_S stSadCtrl;
    IVE_CCL_CTRL_S stCclCtrl;
    IVE_IMAGE_S stSadOut;
    IVE_IMAGE_S stThrOut;
    HI_BOOL bFinish = HI_FALSE;
    HI_S32 s32Ret;
    struct timespec ts;
    struct timeval tvStart, tvEnd;
    HI_U32 u32CostTime;
    HI_U32 u32BlockSize;

    if (s_enMdState != 1) {
        HI_TRACE_MD(HI_DBG_ERR, "md is not ready!\n");
        return HI_ERR_MD_NOT_PERM;
    }

    if (pstCur == NULL) {
        HI_TRACE_MD(HI_DBG_ERR, "pstCur can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    if (pstRef == NULL) {
        HI_TRACE_MD(HI_DBG_ERR, "pstRef can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    if (pstBlob == NULL) {
        HI_TRACE_MD(HI_DBG_ERR, "pstBlob can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    if (pstBlob->u32Size < 3052) {
        HI_TRACE_MD(HI_DBG_ERR,
            "pstBlob->u32Size(%u) must be greater than or equal to %u!\n",
            pstBlob->u32Size, 3052u);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }

    if (pstBlob->u64VirAddr == 0) {
        HI_TRACE_MD(HI_DBG_ERR, "pstBlob->u64VirAddr can't be 0!\n");
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }

    if (pstBlob->u64PhyAddr == 0) {
        HI_TRACE_MD(HI_DBG_ERR, "pstBlob->u64PhyAddr can't be 0!\n");
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }

    if (pstBlob->u64PhyAddr & 0xF) {
        HI_TRACE_MD(HI_DBG_ERR,
            "pstBlob->u64PhyAddr(0x%llx) must be %d byte align!\n",
            (unsigned long long)pstBlob->u64PhyAddr, 16);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }

    if (MdChn < 0 || MdChn >= MD_MAX_CHN) {
        HI_TRACE_MD(HI_DBG_ERR, "MdChn(%d) must be in [%d,%d)!\n", MdChn, 0, MD_MAX_CHN);
        return HI_ERR_MD_INVALID_CHNID;
    }

    gettimeofday(&tvStart, NULL);

    pthread_mutex_lock(&s_astMdChnMutex[MdChn]);
    pCtx = &s_astMdChnCtx[MdChn];

    if (!pCtx->bCreated) {
        HI_TRACE_MD(HI_DBG_ERR, "md chn(%d) does not exist!\n", MdChn);
        pthread_mutex_unlock(&s_astMdChnMutex[MdChn]);
        return HI_ERR_MD_UNEXIST;
    }

    /* Validate current image */
    s32Ret = MdCheckImageUser(pstCur, 64, 1920, 64, 1080, 16, 1);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_MD(HI_DBG_ERR, "check pstCur fail,Error(%#x)!\n", s32Ret);
        pthread_mutex_unlock(&s_astMdChnMutex[MdChn]);
        return s32Ret;
    }
    if (pstCur->enType != IVE_IMAGE_TYPE_U8C1) {
        HI_TRACE_MD(HI_DBG_ERR, "pstCur->enType(%d) must be U8C1(%d)!\n",
            pstCur->enType, IVE_IMAGE_TYPE_U8C1);
        pthread_mutex_unlock(&s_astMdChnMutex[MdChn]);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }

    /* Validate reference image */
    s32Ret = MdCheckImageUser(pstRef, 64, 1920, 64, 1080, 16, 1);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_MD(HI_DBG_ERR, "check pstRef fail,Error(%#x)!\n", s32Ret);
        pthread_mutex_unlock(&s_astMdChnMutex[MdChn]);
        return s32Ret;
    }
    if (pstRef->enType != IVE_IMAGE_TYPE_U8C1) {
        HI_TRACE_MD(HI_DBG_ERR, "pstRef->enType(%d) must be U8C1(%d)!\n",
            pstRef->enType, IVE_IMAGE_TYPE_U8C1);
        pthread_mutex_unlock(&s_astMdChnMutex[MdChn]);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }

    /* Validate pstSad if provided and in REF mode with non-threshold output */
    if (pstSad != NULL) {
        s32Ret = MdCheckImageUser(pstSad, 64, 1920, 64, 1080, 16, 1);
        if (s32Ret != HI_SUCCESS) {
            HI_TRACE_MD(HI_DBG_ERR, "check pstSad fail,Error(%#x)!\n", s32Ret);
            pthread_mutex_unlock(&s_astMdChnMutex[MdChn]);
            return s32Ret;
        }
        if (pstSad->enType != IVE_IMAGE_TYPE_U16C1) {
            HI_TRACE_MD(HI_DBG_ERR, "pstRef->enType(%d) must be U16C1(%d)!\n",
                pstSad->enType, IVE_IMAGE_TYPE_U16C1);
            pthread_mutex_unlock(&s_astMdChnMutex[MdChn]);
            return HI_ERR_IVE_ILLEGAL_PARAM;
        }
    }

    u32BlockSize = 4 << pCtx->enSadMode;
    ts.tv_sec = 0;
    ts.tv_nsec = 100000;

    if (pCtx->enAlgMode == MD_ALG_MODE_BG && pCtx->bFirstFrame) {
        /* First frame in BG mode: DMA current into background */
        s32Ret = MD_DmaImage(pstCur, &pCtx->stBgImage);
        if (s32Ret != HI_SUCCESS) {
            HI_TRACE_MD(HI_DBG_ERR, "MD_DmaImage fail,Error(%#x)!\n", s32Ret);
            pthread_mutex_unlock(&s_astMdChnMutex[MdChn]);
            return s32Ret;
        }
        pCtx->bFirstFrame = 0;
        pthread_mutex_unlock(&s_astMdChnMutex[MdChn]);
        return HI_SUCCESS;
    }

    /* Set up SAD control */
    memset_s(&stSadCtrl, sizeof(stSadCtrl), 0, sizeof(stSadCtrl));
    stSadCtrl.enMode = pCtx->enSadMode;
    stSadCtrl.enOutCtrl = pCtx->enSadOutCtrl;
    stSadCtrl.u16Thr = pCtx->u16SadThr;

    /* Set up internal SAD output image */
    memset_s(&stSadOut, sizeof(stSadOut), 0, sizeof(stSadOut));
    memset_s(&stThrOut, sizeof(stThrOut), 0, sizeof(stThrOut));

    if (pstSad != NULL) {
        /* User-provided SAD output */
        memcpy_s(&stSadOut, sizeof(IVE_IMAGE_S), pstSad, sizeof(IVE_IMAGE_S));
    } else {
        /* Use internal buffer */
        stSadOut.enType = IVE_IMAGE_TYPE_U16C1;
        stSadOut.u32Width = pCtx->u32SadWidth;
        stSadOut.u32Height = pCtx->u32SadHeight;
        stSadOut.au32Stride[0] = pCtx->u32SadStride * 2;
        stSadOut.au64PhyAddr[0] = pCtx->u64SadPhyAddr;
        stSadOut.au64VirAddr[0] = pCtx->u64SadVirAddr;
    }

    /* Set up threshold output for CCL */
    stThrOut.enType = IVE_IMAGE_TYPE_U8C1;
    stThrOut.u32Width = pCtx->u32SadWidth;
    stThrOut.u32Height = pCtx->u32SadHeight;
    stThrOut.au32Stride[0] = pCtx->u32SadStride;
    stThrOut.au64PhyAddr[0] = pCtx->stBgImage.au64PhyAddr[0];
    stThrOut.au64VirAddr[0] = pCtx->stBgImage.au64VirAddr[0];

    /* Determine which reference to use */
    if (pCtx->enAlgMode == MD_ALG_MODE_BG) {
        /* BG mode: compare current against background */
        s32Ret = HI_MPI_IVE_SAD(&hHandle, pstCur, &pCtx->stBgImage,
            &stSadOut, &stThrOut, &stSadCtrl, HI_TRUE);
    } else {
        /* REF mode: compare current against reference */
        s32Ret = HI_MPI_IVE_SAD(&hHandle, pstCur, pstRef,
            &stSadOut, &stThrOut, &stSadCtrl, HI_TRUE);
    }

    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_MD(HI_DBG_ERR, "HI_MPI_IVE_SAD fail,Error(%#x)!\n", s32Ret);
        pthread_mutex_unlock(&s_astMdChnMutex[MdChn]);
        return s32Ret;
    }

    /* Wait for SAD to complete */
    do {
        s32Ret = HI_MPI_IVE_Query(hHandle, &bFinish, HI_TRUE);
        if (s32Ret == HI_ERR_IVE_QUERY_TIMEOUT) {
            nanosleep(&ts, NULL);
            continue;
        }
        if (s32Ret != HI_SUCCESS) {
            pthread_mutex_unlock(&s_astMdChnMutex[MdChn]);
            return s32Ret;
        }
    } while (!bFinish);

    /* Run CCL on threshold output */
    stCclCtrl = pCtx->stCclCtrl;
    bFinish = HI_FALSE;

    s32Ret = HI_MPI_IVE_CCL(&hHandle, &stThrOut, pstBlob, &stCclCtrl, HI_TRUE);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_MD(HI_DBG_ERR, "HI_MPI_IVE_CCL fail,Error(%#x)!\n", s32Ret);
        pthread_mutex_unlock(&s_astMdChnMutex[MdChn]);
        return s32Ret;
    }

    /* Wait for CCL to complete */
    do {
        s32Ret = HI_MPI_IVE_Query(hHandle, &bFinish, HI_TRUE);
        if (s32Ret == HI_ERR_IVE_QUERY_TIMEOUT) {
            nanosleep(&ts, NULL);
            continue;
        }
        if (s32Ret != HI_SUCCESS) {
            pthread_mutex_unlock(&s_astMdChnMutex[MdChn]);
            return s32Ret;
        }
    } while (!bFinish);

    /* In BG mode, update the background model */
    if (pCtx->enAlgMode == MD_ALG_MODE_BG) {
        s32Ret = MD_UpdateBg(&pCtx->stBgImage, pstCur, &pCtx->stAddCtrl);
        if (s32Ret != HI_SUCCESS) {
            HI_TRACE_MD(HI_DBG_ERR, "MD_UpdateBg fail,Error(%#x)!\n", s32Ret);
            pthread_mutex_unlock(&s_astMdChnMutex[MdChn]);
            return s32Ret;
        }
    }

    /* Post-process: scale CCL results by block size */
    {
        IVE_CCBLOB_S *pCcBlob = (IVE_CCBLOB_S *)(HI_UL)pstBlob->u64VirAddr;
        pCcBlob->u8RegionNum = MD_ConvertObj(&pCcBlob->astRegion[0],
            (HI_U16)u32BlockSize);
    }

    /* Update stats */
    gettimeofday(&tvEnd, NULL);
    u32CostTime = (tvEnd.tv_sec - tvStart.tv_sec) * 1000000 +
                  (tvEnd.tv_usec - tvStart.tv_usec);

    pCtx->u32ProcCnt++;
    pCtx->u32TotalTime += u32CostTime;
    if (u32CostTime > pCtx->u32MaxTime || pCtx->u32MaxTime == 0)
        pCtx->u32MaxTime = u32CostTime;
    if (u32CostTime < pCtx->u32MinTime || pCtx->u32MinTime == 0)
        pCtx->u32MinTime = u32CostTime;

    /* Update proc */
    MPI_IVE_MdProcBeginWrite();
    HI_IVS_MD_WriteProc();
    MPI_IVE_MdProcEndWrite();

    pthread_mutex_unlock(&s_astMdChnMutex[MdChn]);
    return HI_SUCCESS;
}
