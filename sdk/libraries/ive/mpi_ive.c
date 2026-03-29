/**
 * Reverse Engineered by TekuConcept
 * IVE (Image Vector Engine) main API implementation
 *
 * All 59 public HI_MPI_IVE_* functions + 10 internal helpers.
 * Communicates with /dev/ive kernel driver via ioctl.
 */

#include "re_mpi_ive.h"
#include <math.h>

/* Global state: single fd + mutex, shared across all IVE and MD operations */
HI_S32 s_s32IveFd = -1;
pthread_mutex_t s_IveMutex = PTHREAD_MUTEX_INITIALIZER;

/* -------------------------------------------------------------------------- */
/*  Internal: fd-open helper                                                  */
/* -------------------------------------------------------------------------- */

static HI_S32 IVE_IOCTL_Init(void)
{
    pthread_mutex_lock(&s_IveMutex);
    if (s_s32IveFd < 0) {
        s_s32IveFd = open("/dev/ive", O_RDWR);
        if (s_s32IveFd < 0) {
            pthread_mutex_unlock(&s_IveMutex);
            HI_TRACE_IVE(HI_DBG_ERR, "open /dev/ive err\n");
            return HI_ERR_IVE_NOTREADY;
        }
    }
    pthread_mutex_unlock(&s_IveMutex);
    return HI_SUCCESS;
}

static HI_S32 IVE_MD_Init(void)
{
    pthread_mutex_lock(&s_IveMutex);
    if (s_s32IveFd < 0) {
        s_s32IveFd = open("/dev/ive", O_RDWR);
        if (s_s32IveFd < 0) {
            pthread_mutex_unlock(&s_IveMutex);
            HI_TRACE_MD(HI_DBG_ERR, "open /dev/ive err\n");
            return HI_ERR_IVE_NOTREADY;
        }
    }
    pthread_mutex_unlock(&s_IveMutex);
    return HI_SUCCESS;
}

/* -------------------------------------------------------------------------- */
/*  Internal: HI_Comp - qsort comparator for STCorner (descending by byte)   */
/* -------------------------------------------------------------------------- */

static int HI_Comp(const void *a, const void *b)
{
    HI_U8 va = **(const HI_U8 *const *)a;
    HI_U8 vb = **(const HI_U8 *const *)b;
    return (int)vb - (int)va;
}

/* -------------------------------------------------------------------------- */
/*  Internal: IveGetGaussPeakMem - compute Gaussian peak memory for KCF      */
/* -------------------------------------------------------------------------- */

static HI_VOID IveGetGaussPeakMem(HI_U32 u32Height, HI_U32 u32Width,
    HI_U64 u64Acc1, HI_U64 u64Acc2, HI_U64 *pu64Out)
{
    HI_U32 padH = (u32Height + 4) << 1;
    HI_U32 padW = (u32Width + 4) << 1;
    HI_U64 memSize;
    HI_U32 cellSize;

    if (padH <= 16 && padW <= 16) {
        cellSize = 1024;
        memSize = (HI_U64)(u32Height + 5 * u32Width) << 10;
        pu64Out[0] = u64Acc1 + memSize;
        pu64Out[1] = u64Acc2 + memSize;
    } else if (padH > 16 && padH <= 32 && padW <= 16) {
        cellSize = 2048;
        memSize = (HI_U64)(u32Height + u32Width * 8 - 5) << 11;
        pu64Out[0] = u64Acc1 + 25600 + memSize;
        pu64Out[1] = u64Acc2 + 25600 + memSize;
    } else if (padH <= 16 && padW > 16 && padW <= 32) {
        cellSize = 2048;
        memSize = (HI_U64)(u32Height + 5 * u32Width - 25) << 11;
        pu64Out[0] = u64Acc1 + 107520 + memSize;
        pu64Out[1] = u64Acc2 + 107520 + memSize;
    } else {
        cellSize = 4096;
        memSize = (HI_U64)(u32Height + u32Width * 8 - 45) << 12;
        pu64Out[0] = u64Acc1 + 189440 + memSize;
        pu64Out[1] = u64Acc2 + 189440 + memSize;
    }
    *(HI_U32 *)&pu64Out[2] = cellSize;
}

/* -------------------------------------------------------------------------- */
/*  Internal: IveCalcCosWindow - Hann cosine window for KCF tracking          */
/* -------------------------------------------------------------------------- */

static HI_VOID IveCalcCosWindow(HI_U32 u32Size, HI_U16 *pu16Buf)
{
    HI_S32 half = (HI_S32)(u32Size >> 1);
    HI_S32 padHalf;
    HI_S32 i, idx;
    HI_DOUBLE dSizeM1 = (HI_DOUBLE)((HI_S32)u32Size - 1);
    HI_DOUBLE dAngle, dCos, dVal;

    padHalf = (u32Size < 17) ? 8 : 16;

    for (i = -padHalf; i < padHalf; i++) {
        if (i < -half)
            idx = -half;
        else if (i >= half)
            idx = half - 1;
        else
            idx = i;
        idx += half;

        dAngle = 6.283185307179586 * (HI_DOUBLE)idx / dSizeM1;
        dCos = cos(dAngle);
        dVal = (1.0 - dCos) * 0.5 * 1024.0 * 16.0;
        *pu16Buf++ = (HI_U16)(HI_U32)dVal;
    }
}

/* -------------------------------------------------------------------------- */
/*  Internal: CNN_GetCtrlSize - compute assist buffer size for CNN predict     */
/* -------------------------------------------------------------------------- */

static HI_U32 CNN_GetCtrlSize(IVE_CNN_MODEL_S *pstCnnModel, HI_U32 u32Num)
{
    HI_U32 u32ChnNum;
    HI_U32 u32NeuronBytes;
    HI_U32 u32Padding;
    HI_U32 u32BaseSize;

    if (pstCnnModel->enType == IVE_IMAGE_TYPE_U8C3_PLANAR)
        u32ChnNum = 3;
    else
        u32ChnNum = 1;

    u32NeuronBytes = (HI_U32)pstCnnModel->stFullConnect.au16LayerCnt[0] << 2;
    u32Padding = (0 - u32NeuronBytes) & 15;
    u32BaseSize = u32ChnNum * u32Num * 4;

    return u32Num * (u32NeuronBytes + u32Padding) + u32BaseSize +
           ((0 - u32BaseSize) & 15);
}

/* -------------------------------------------------------------------------- */
/*  HI_MPI_IVE_Query                                                          */
/* -------------------------------------------------------------------------- */

HI_S32 HI_MPI_IVE_Query(IVE_HANDLE IveHandle, HI_BOOL *pbFinish, HI_BOOL bBlock)
{
    HI_S32 s32Ret;
    HI_U8 au8Buf[12];

    s32Ret = IVE_IOCTL_Init();
    if (s32Ret != HI_SUCCESS)
        return s32Ret;

    if (pbFinish == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pbFinish is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    *(IVE_HANDLE *)&au8Buf[0] = IveHandle;
    *(HI_BOOL *)&au8Buf[4] = bBlock;
    *(HI_BOOL *)&au8Buf[8] = 0;

    s32Ret = ioctl(s_s32IveFd, IOC_IVE_QUERY, au8Buf);

    *pbFinish = *(HI_BOOL *)&au8Buf[8];
    return s32Ret;
}

/* -------------------------------------------------------------------------- */
/*  Standard ioctl image operations                                           */
/* -------------------------------------------------------------------------- */

HI_S32 HI_MPI_IVE_DMA(IVE_HANDLE *pIveHandle, IVE_DATA_S *pstSrc,
    IVE_DST_DATA_S *pstDst, IVE_DMA_CTRL_S *pstDmaCtrl, HI_BOOL bInstant)
{
    HI_S32 s32Ret;
    HI_U8 au8Buf[104];

    s32Ret = IVE_IOCTL_Init();
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pIveHandle == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pIveHandle is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    s32Ret = IveCheckDMAParamUser(pstSrc, pstDst, pstDmaCtrl);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check DMA parameters failed!\n", s32Ret);
        return s32Ret;
    }

    memset_s(au8Buf, sizeof(au8Buf), 0, sizeof(au8Buf));
    memcpy_s(&au8Buf[8], sizeof(IVE_DATA_S), pstSrc, sizeof(IVE_DATA_S));
    memcpy_s(&au8Buf[40], sizeof(IVE_DATA_S), pstDst, sizeof(IVE_DATA_S));
    memcpy_s(&au8Buf[72], sizeof(IVE_DMA_CTRL_S), pstDmaCtrl, sizeof(IVE_DMA_CTRL_S));
    *(HI_BOOL *)&au8Buf[96] = bInstant;

    s32Ret = ioctl(s_s32IveFd, IOC_IVE_DMA, au8Buf);
    *pIveHandle = *(IVE_HANDLE *)&au8Buf[0];
    return s32Ret;
}

HI_S32 HI_MPI_IVE_Filter(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
    IVE_DST_IMAGE_S *pstDst, IVE_FILTER_CTRL_S *pstFltCtrl, HI_BOOL bInstant)
{
    HI_S32 s32Ret;
    HI_U8 au8Buf[184];

    s32Ret = IVE_IOCTL_Init();
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pIveHandle == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pIveHandle is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    s32Ret = IveCheckFilterParamUser(pstSrc, pstDst, pstFltCtrl);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check Filter parameters failed!\n", s32Ret);
        return s32Ret;
    }

    memset_s(au8Buf, sizeof(au8Buf), 0, sizeof(au8Buf));
    memcpy_s(&au8Buf[8], sizeof(IVE_IMAGE_S), pstSrc, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[80], sizeof(IVE_IMAGE_S), pstDst, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[152], sizeof(IVE_FILTER_CTRL_S), pstFltCtrl, sizeof(IVE_FILTER_CTRL_S));
    *(HI_BOOL *)&au8Buf[180] = bInstant;

    s32Ret = ioctl(s_s32IveFd, IOC_IVE_FILTER, au8Buf);
    *pIveHandle = *(IVE_HANDLE *)&au8Buf[0];
    return s32Ret;
}

HI_S32 HI_MPI_IVE_CSC(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
    IVE_DST_IMAGE_S *pstDst, IVE_CSC_CTRL_S *pstCscCtrl, HI_BOOL bInstant)
{
    HI_S32 s32Ret;
    HI_U8 au8Buf[160];

    s32Ret = IVE_IOCTL_Init();
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pIveHandle == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pIveHandle is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    s32Ret = IveCheckCSCParamUser(pstSrc, pstDst, pstCscCtrl);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check CSC parameters failed!\n", s32Ret);
        return s32Ret;
    }

    memset_s(au8Buf, sizeof(au8Buf), 0, sizeof(au8Buf));
    memcpy_s(&au8Buf[8], sizeof(IVE_IMAGE_S), pstSrc, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[80], sizeof(IVE_IMAGE_S), pstDst, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[152], sizeof(IVE_CSC_CTRL_S), pstCscCtrl, sizeof(IVE_CSC_CTRL_S));
    *(HI_BOOL *)&au8Buf[156] = bInstant;

    s32Ret = ioctl(s_s32IveFd, IOC_IVE_CSC, au8Buf);
    *pIveHandle = *(IVE_HANDLE *)&au8Buf[0];
    return s32Ret;
}

HI_S32 HI_MPI_IVE_FilterAndCSC(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
    IVE_DST_IMAGE_S *pstDst, IVE_FILTER_AND_CSC_CTRL_S *pstFltCscCtrl, HI_BOOL bInstant)
{
    HI_S32 s32Ret;
    HI_U8 au8Buf[192];

    s32Ret = IVE_IOCTL_Init();
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pIveHandle == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pIveHandle is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    s32Ret = IveCheckFilterAndCSCParamUser(pstSrc, pstDst, pstFltCscCtrl);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check FilterAndCSC parameters failed!\n", s32Ret);
        return s32Ret;
    }

    memset_s(au8Buf, sizeof(au8Buf), 0, sizeof(au8Buf));
    memcpy_s(&au8Buf[8], sizeof(IVE_IMAGE_S), pstSrc, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[80], sizeof(IVE_IMAGE_S), pstDst, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[152], sizeof(IVE_FILTER_AND_CSC_CTRL_S), pstFltCscCtrl, sizeof(IVE_FILTER_AND_CSC_CTRL_S));
    *(HI_BOOL *)&au8Buf[184] = bInstant;

    s32Ret = ioctl(s_s32IveFd, IOC_IVE_FILTER_AND_CSC, au8Buf);
    *pIveHandle = *(IVE_HANDLE *)&au8Buf[0];
    return s32Ret;
}

HI_S32 HI_MPI_IVE_Sobel(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
    IVE_DST_IMAGE_S *pstDstH, IVE_DST_IMAGE_S *pstDstV,
    IVE_SOBEL_CTRL_S *pstSobelCtrl, HI_BOOL bInstant)
{
    HI_S32 s32Ret;
    HI_U8 au8Buf[264];

    s32Ret = IVE_IOCTL_Init();
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pIveHandle == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pIveHandle is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    s32Ret = IveCheckSobelParamUser(pstSrc, pstDstH, pstDstV, pstSobelCtrl);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check Sobel parameters failed!\n", s32Ret);
        return s32Ret;
    }

    memset_s(au8Buf, sizeof(au8Buf), 0, sizeof(au8Buf));
    memcpy_s(&au8Buf[8], sizeof(IVE_IMAGE_S), pstSrc, sizeof(IVE_IMAGE_S));
    if (pstSobelCtrl->enOutCtrl != IVE_SOBEL_OUT_CTRL_VER)
        memcpy_s(&au8Buf[80], sizeof(IVE_IMAGE_S), pstDstH, sizeof(IVE_IMAGE_S));
    if (pstSobelCtrl->enOutCtrl != IVE_SOBEL_OUT_CTRL_HOR)
        memcpy_s(&au8Buf[152], sizeof(IVE_IMAGE_S), pstDstV, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[224], sizeof(IVE_SOBEL_CTRL_S), pstSobelCtrl, sizeof(IVE_SOBEL_CTRL_S));
    *(HI_BOOL *)&au8Buf[256] = bInstant;

    s32Ret = ioctl(s_s32IveFd, IOC_IVE_SOBEL, au8Buf);
    *pIveHandle = *(IVE_HANDLE *)&au8Buf[0];
    return s32Ret;
}

HI_S32 HI_MPI_IVE_MagAndAng(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
    IVE_DST_IMAGE_S *pstDstMag, IVE_DST_IMAGE_S *pstDstAng,
    IVE_MAG_AND_ANG_CTRL_S *pstMagAndAngCtrl, HI_BOOL bInstant)
{
    HI_S32 s32Ret;
    HI_U8 au8Buf[264];

    s32Ret = IVE_IOCTL_Init();
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pIveHandle == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pIveHandle is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    s32Ret = IveCheckMagAndAngParamUser(pstSrc, pstDstMag, pstDstAng, pstMagAndAngCtrl);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check MagAndAng parameters failed!\n", s32Ret);
        return s32Ret;
    }

    memset_s(au8Buf, sizeof(au8Buf), 0, sizeof(au8Buf));
    memcpy_s(&au8Buf[8], sizeof(IVE_IMAGE_S), pstSrc, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[80], sizeof(IVE_IMAGE_S), pstDstMag, sizeof(IVE_IMAGE_S));
    if (pstMagAndAngCtrl->enOutCtrl == IVE_MAG_AND_ANG_OUT_CTRL_MAG_AND_ANG)
        memcpy_s(&au8Buf[152], sizeof(IVE_IMAGE_S), pstDstAng, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[224], sizeof(IVE_MAG_AND_ANG_CTRL_S), pstMagAndAngCtrl, sizeof(IVE_MAG_AND_ANG_CTRL_S));
    *(HI_BOOL *)&au8Buf[256] = bInstant;

    s32Ret = ioctl(s_s32IveFd, IOC_IVE_MAG_AND_ANG, au8Buf);
    *pIveHandle = *(IVE_HANDLE *)&au8Buf[0];
    return s32Ret;
}

HI_S32 HI_MPI_IVE_Dilate(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
    IVE_DST_IMAGE_S *pstDst, IVE_DILATE_CTRL_S *pstDilateCtrl, HI_BOOL bInstant)
{
    HI_S32 s32Ret;
    HI_U8 au8Buf[184];

    s32Ret = IVE_IOCTL_Init();
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pIveHandle == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pIveHandle is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    s32Ret = IveCheckDilateParamUser(pstSrc, pstDst, pstDilateCtrl);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check Dilate parameters failed!\n", s32Ret);
        return s32Ret;
    }

    memset_s(au8Buf, sizeof(au8Buf), 0, sizeof(au8Buf));
    memcpy_s(&au8Buf[8], sizeof(IVE_IMAGE_S), pstSrc, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[80], sizeof(IVE_IMAGE_S), pstDst, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[152], sizeof(IVE_DILATE_CTRL_S), pstDilateCtrl, sizeof(IVE_DILATE_CTRL_S));
    *(HI_BOOL *)&au8Buf[180] = bInstant;

    s32Ret = ioctl(s_s32IveFd, IOC_IVE_DILATE, au8Buf);
    *pIveHandle = *(IVE_HANDLE *)&au8Buf[0];
    return s32Ret;
}

HI_S32 HI_MPI_IVE_Erode(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
    IVE_DST_IMAGE_S *pstDst, IVE_ERODE_CTRL_S *pstErodeCtrl, HI_BOOL bInstant)
{
    HI_S32 s32Ret;
    HI_U8 au8Buf[184];

    s32Ret = IVE_IOCTL_Init();
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pIveHandle == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pIveHandle is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    s32Ret = IveCheckErodeParamUser(pstSrc, pstDst, pstErodeCtrl);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check Erode parameters failed!\n", s32Ret);
        return s32Ret;
    }

    memset_s(au8Buf, sizeof(au8Buf), 0, sizeof(au8Buf));
    memcpy_s(&au8Buf[8], sizeof(IVE_IMAGE_S), pstSrc, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[80], sizeof(IVE_IMAGE_S), pstDst, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[152], sizeof(IVE_ERODE_CTRL_S), pstErodeCtrl, sizeof(IVE_ERODE_CTRL_S));
    *(HI_BOOL *)&au8Buf[180] = bInstant;

    s32Ret = ioctl(s_s32IveFd, IOC_IVE_ERODE, au8Buf);
    *pIveHandle = *(IVE_HANDLE *)&au8Buf[0];
    return s32Ret;
}

HI_S32 HI_MPI_IVE_Thresh(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
    IVE_DST_IMAGE_S *pstDst, IVE_THRESH_CTRL_S *pstThreshCtrl, HI_BOOL bInstant)
{
    HI_S32 s32Ret;
    HI_U8 au8Buf[168];

    s32Ret = IVE_IOCTL_Init();
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pIveHandle == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pIveHandle is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    s32Ret = IveCheckThreshParamUser(pstSrc, pstDst, pstThreshCtrl);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check Thresh parameters failed!\n", s32Ret);
        return s32Ret;
    }

    memset_s(au8Buf, sizeof(au8Buf), 0, sizeof(au8Buf));
    memcpy_s(&au8Buf[8], sizeof(IVE_IMAGE_S), pstSrc, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[80], sizeof(IVE_IMAGE_S), pstDst, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[152], sizeof(IVE_THRESH_CTRL_S), pstThreshCtrl, sizeof(IVE_THRESH_CTRL_S));
    *(HI_BOOL *)&au8Buf[164] = bInstant;

    s32Ret = ioctl(s_s32IveFd, IOC_IVE_THRESH, au8Buf);
    *pIveHandle = *(IVE_HANDLE *)&au8Buf[0];
    return s32Ret;
}

HI_S32 HI_MPI_IVE_And(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc1,
    IVE_SRC_IMAGE_S *pstSrc2, IVE_DST_IMAGE_S *pstDst, HI_BOOL bInstant)
{
    HI_S32 s32Ret;
    HI_U8 au8Buf[232];

    s32Ret = IVE_IOCTL_Init();
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pIveHandle == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pIveHandle is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    s32Ret = IveCheckAndParamUser(pstSrc1, pstSrc2, pstDst);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check And parameters failed!\n", s32Ret);
        return s32Ret;
    }

    memset_s(au8Buf, sizeof(au8Buf), 0, sizeof(au8Buf));
    memcpy_s(&au8Buf[8], sizeof(IVE_IMAGE_S), pstSrc1, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[80], sizeof(IVE_IMAGE_S), pstSrc2, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[152], sizeof(IVE_IMAGE_S), pstDst, sizeof(IVE_IMAGE_S));
    *(HI_BOOL *)&au8Buf[224] = bInstant;

    s32Ret = ioctl(s_s32IveFd, IOC_IVE_AND, au8Buf);
    *pIveHandle = *(IVE_HANDLE *)&au8Buf[0];
    return s32Ret;
}

HI_S32 HI_MPI_IVE_Sub(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc1,
    IVE_SRC_IMAGE_S *pstSrc2, IVE_DST_IMAGE_S *pstDst,
    IVE_SUB_CTRL_S *pstSubCtrl, HI_BOOL bInstant)
{
    HI_S32 s32Ret;
    HI_U8 au8Buf[232];

    s32Ret = IVE_IOCTL_Init();
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pIveHandle == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pIveHandle is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    s32Ret = IveCheckSubParamUser(pstSrc1, pstSrc2, pstDst, pstSubCtrl);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check Sub parameters failed!\n", s32Ret);
        return s32Ret;
    }

    memset_s(au8Buf, sizeof(au8Buf), 0, sizeof(au8Buf));
    memcpy_s(&au8Buf[8], sizeof(IVE_IMAGE_S), pstSrc1, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[80], sizeof(IVE_IMAGE_S), pstSrc2, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[152], sizeof(IVE_IMAGE_S), pstDst, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[224], sizeof(IVE_SUB_CTRL_S), pstSubCtrl, sizeof(IVE_SUB_CTRL_S));
    *(HI_BOOL *)&au8Buf[228] = bInstant;

    s32Ret = ioctl(s_s32IveFd, IOC_IVE_SUB, au8Buf);
    *pIveHandle = *(IVE_HANDLE *)&au8Buf[0];
    return s32Ret;
}

HI_S32 HI_MPI_IVE_Or(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc1,
    IVE_SRC_IMAGE_S *pstSrc2, IVE_DST_IMAGE_S *pstDst, HI_BOOL bInstant)
{
    HI_S32 s32Ret;
    HI_U8 au8Buf[232];

    s32Ret = IVE_IOCTL_Init();
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pIveHandle == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pIveHandle is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    s32Ret = IveCheckOrParamUser(pstSrc1, pstSrc2, pstDst);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check Or parameters failed!\n", s32Ret);
        return s32Ret;
    }

    memset_s(au8Buf, sizeof(au8Buf), 0, sizeof(au8Buf));
    memcpy_s(&au8Buf[8], sizeof(IVE_IMAGE_S), pstSrc1, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[80], sizeof(IVE_IMAGE_S), pstSrc2, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[152], sizeof(IVE_IMAGE_S), pstDst, sizeof(IVE_IMAGE_S));
    *(HI_BOOL *)&au8Buf[224] = bInstant;

    s32Ret = ioctl(s_s32IveFd, IOC_IVE_OR, au8Buf);
    *pIveHandle = *(IVE_HANDLE *)&au8Buf[0];
    return s32Ret;
}

HI_S32 HI_MPI_IVE_Xor(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc1,
    IVE_SRC_IMAGE_S *pstSrc2, IVE_DST_IMAGE_S *pstDst, HI_BOOL bInstant)
{
    HI_S32 s32Ret;
    HI_U8 au8Buf[232];

    s32Ret = IVE_IOCTL_Init();
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pIveHandle == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pIveHandle is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    s32Ret = IveCheckXorParamUser(pstSrc1, pstSrc2, pstDst);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check Xor parameters failed!\n", s32Ret);
        return s32Ret;
    }

    memset_s(au8Buf, sizeof(au8Buf), 0, sizeof(au8Buf));
    memcpy_s(&au8Buf[8], sizeof(IVE_IMAGE_S), pstSrc1, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[80], sizeof(IVE_IMAGE_S), pstSrc2, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[152], sizeof(IVE_IMAGE_S), pstDst, sizeof(IVE_IMAGE_S));
    *(HI_BOOL *)&au8Buf[224] = bInstant;

    s32Ret = ioctl(s_s32IveFd, IOC_IVE_XOR, au8Buf);
    *pIveHandle = *(IVE_HANDLE *)&au8Buf[0];
    return s32Ret;
}

HI_S32 HI_MPI_IVE_Integ(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
    IVE_DST_IMAGE_S *pstDst, IVE_INTEG_CTRL_S *pstIntegCtrl, HI_BOOL bInstant)
{
    HI_S32 s32Ret;
    HI_U8 au8Buf[160];

    s32Ret = IVE_IOCTL_Init();
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pIveHandle == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pIveHandle is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    s32Ret = IveCheckIntegParamUser(pstSrc, pstDst, pstIntegCtrl);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check Integ parameters failed!\n", s32Ret);
        return s32Ret;
    }

    memset_s(au8Buf, sizeof(au8Buf), 0, sizeof(au8Buf));
    memcpy_s(&au8Buf[8], sizeof(IVE_IMAGE_S), pstSrc, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[80], sizeof(IVE_IMAGE_S), pstDst, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[152], sizeof(IVE_INTEG_CTRL_S), pstIntegCtrl, sizeof(IVE_INTEG_CTRL_S));
    *(HI_BOOL *)&au8Buf[156] = bInstant;

    s32Ret = ioctl(s_s32IveFd, IOC_IVE_INTEG, au8Buf);
    *pIveHandle = *(IVE_HANDLE *)&au8Buf[0];
    return s32Ret;
}

HI_S32 HI_MPI_IVE_Hist(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
    IVE_DST_MEM_INFO_S *pstDst, HI_BOOL bInstant)
{
    HI_S32 s32Ret;
    HI_U8 au8Buf[112];

    s32Ret = IVE_IOCTL_Init();
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pIveHandle == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pIveHandle is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    s32Ret = IveCheckHistParamUser(pstSrc, pstDst);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check Hist parameters failed!\n", s32Ret);
        return s32Ret;
    }

    memset_s(au8Buf, sizeof(au8Buf), 0, sizeof(au8Buf));
    memcpy_s(&au8Buf[8], sizeof(IVE_IMAGE_S), pstSrc, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[80], sizeof(IVE_MEM_INFO_S), pstDst, sizeof(IVE_MEM_INFO_S));
    *(HI_BOOL *)&au8Buf[104] = bInstant;

    s32Ret = ioctl(s_s32IveFd, IOC_IVE_HIST, au8Buf);
    *pIveHandle = *(IVE_HANDLE *)&au8Buf[0];
    return s32Ret;
}

HI_S32 HI_MPI_IVE_Thresh_S16(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
    IVE_DST_IMAGE_S *pstDst, IVE_THRESH_S16_CTRL_S *pstThrS16Ctrl, HI_BOOL bInstant)
{
    HI_S32 s32Ret;
    HI_U8 au8Buf[168];

    s32Ret = IVE_IOCTL_Init();
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pIveHandle == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pIveHandle is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    s32Ret = IveCheckThresh_S16ParamUser(pstSrc, pstDst, pstThrS16Ctrl);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check Thresh_S16 parameters failed!\n", s32Ret);
        return s32Ret;
    }

    memset_s(au8Buf, sizeof(au8Buf), 0, sizeof(au8Buf));
    memcpy_s(&au8Buf[8], sizeof(IVE_IMAGE_S), pstSrc, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[80], sizeof(IVE_IMAGE_S), pstDst, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[152], sizeof(IVE_THRESH_S16_CTRL_S), pstThrS16Ctrl, sizeof(IVE_THRESH_S16_CTRL_S));
    *(HI_BOOL *)&au8Buf[164] = bInstant;

    s32Ret = ioctl(s_s32IveFd, IOC_IVE_THRESH_S16, au8Buf);
    *pIveHandle = *(IVE_HANDLE *)&au8Buf[0];
    return s32Ret;
}

HI_S32 HI_MPI_IVE_Thresh_U16(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
    IVE_DST_IMAGE_S *pstDst, IVE_THRESH_U16_CTRL_S *pstThrU16Ctrl, HI_BOOL bInstant)
{
    HI_S32 s32Ret;
    HI_U8 au8Buf[168];

    s32Ret = IVE_IOCTL_Init();
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pIveHandle == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pIveHandle is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    s32Ret = IveCheckThresh_U16ParamUser(pstSrc, pstDst, pstThrU16Ctrl);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check Thresh_U16 parameters failed!\n", s32Ret);
        return s32Ret;
    }

    memset_s(au8Buf, sizeof(au8Buf), 0, sizeof(au8Buf));
    memcpy_s(&au8Buf[8], sizeof(IVE_IMAGE_S), pstSrc, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[80], sizeof(IVE_IMAGE_S), pstDst, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[152], sizeof(IVE_THRESH_U16_CTRL_S), pstThrU16Ctrl, sizeof(IVE_THRESH_U16_CTRL_S));
    *(HI_BOOL *)&au8Buf[164] = bInstant;

    s32Ret = ioctl(s_s32IveFd, IOC_IVE_THRESH_U16, au8Buf);
    *pIveHandle = *(IVE_HANDLE *)&au8Buf[0];
    return s32Ret;
}

HI_S32 HI_MPI_IVE_16BitTo8Bit(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
    IVE_DST_IMAGE_S *pstDst, IVE_16BIT_TO_8BIT_CTRL_S *pst16BitTo8BitCtrl, HI_BOOL bInstant)
{
    HI_S32 s32Ret;
    HI_U8 au8Buf[168];

    s32Ret = IVE_IOCTL_Init();
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pIveHandle == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pIveHandle is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    s32Ret = IveCheck16BitTo8BitParamUser(pstSrc, pstDst, pst16BitTo8BitCtrl);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check 16BitTo8Bit parameters failed!\n", s32Ret);
        return s32Ret;
    }

    memset_s(au8Buf, sizeof(au8Buf), 0, sizeof(au8Buf));
    memcpy_s(&au8Buf[8], sizeof(IVE_IMAGE_S), pstSrc, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[80], sizeof(IVE_IMAGE_S), pstDst, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[152], sizeof(IVE_16BIT_TO_8BIT_CTRL_S), pst16BitTo8BitCtrl, sizeof(IVE_16BIT_TO_8BIT_CTRL_S));
    *(HI_BOOL *)&au8Buf[160] = bInstant;

    s32Ret = ioctl(s_s32IveFd, IOC_IVE_16BIT_TO_8BIT, au8Buf);
    *pIveHandle = *(IVE_HANDLE *)&au8Buf[0];
    return s32Ret;
}

HI_S32 HI_MPI_IVE_OrdStatFilter(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
    IVE_DST_IMAGE_S *pstDst, IVE_ORD_STAT_FILTER_CTRL_S *pstOrdStatFltCtrl, HI_BOOL bInstant)
{
    HI_S32 s32Ret;
    HI_U8 au8Buf[160];

    s32Ret = IVE_IOCTL_Init();
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pIveHandle == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pIveHandle is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    s32Ret = IveCheckOrdStatFilterParamUser(pstSrc, pstDst, pstOrdStatFltCtrl);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check OrdStatFilter parameters failed!\n", s32Ret);
        return s32Ret;
    }

    memset_s(au8Buf, sizeof(au8Buf), 0, sizeof(au8Buf));
    memcpy_s(&au8Buf[8], sizeof(IVE_IMAGE_S), pstSrc, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[80], sizeof(IVE_IMAGE_S), pstDst, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[152], sizeof(IVE_ORD_STAT_FILTER_CTRL_S), pstOrdStatFltCtrl, sizeof(IVE_ORD_STAT_FILTER_CTRL_S));
    *(HI_BOOL *)&au8Buf[156] = bInstant;

    s32Ret = ioctl(s_s32IveFd, IOC_IVE_ORD_STAT_FILTER, au8Buf);
    *pIveHandle = *(IVE_HANDLE *)&au8Buf[0];
    return s32Ret;
}

HI_S32 HI_MPI_IVE_Map(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
    IVE_SRC_MEM_INFO_S *pstMap, IVE_DST_IMAGE_S *pstDst,
    IVE_MAP_CTRL_S *pstMapCtrl, HI_BOOL bInstant)
{
    HI_S32 s32Ret;
    HI_U8 au8Buf[184];

    s32Ret = IVE_IOCTL_Init();
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pIveHandle == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pIveHandle is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    s32Ret = IveCheckMapParamUser(pstSrc, pstMap, pstDst, pstMapCtrl);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check Map parameters failed!\n", s32Ret);
        return s32Ret;
    }

    memset_s(au8Buf, sizeof(au8Buf), 0, sizeof(au8Buf));
    memcpy_s(&au8Buf[8], sizeof(IVE_IMAGE_S), pstSrc, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[80], sizeof(IVE_MEM_INFO_S), pstMap, sizeof(IVE_MEM_INFO_S));
    memcpy_s(&au8Buf[104], sizeof(IVE_IMAGE_S), pstDst, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[176], sizeof(IVE_MAP_CTRL_S), pstMapCtrl, sizeof(IVE_MAP_CTRL_S));
    *(HI_BOOL *)&au8Buf[180] = bInstant;

    s32Ret = ioctl(s_s32IveFd, IOC_IVE_MAP, au8Buf);
    *pIveHandle = *(IVE_HANDLE *)&au8Buf[0];
    return s32Ret;
}

HI_S32 HI_MPI_IVE_EqualizeHist(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
    IVE_DST_IMAGE_S *pstDst, IVE_EQUALIZE_HIST_CTRL_S *pstEqualizeHistCtrl, HI_BOOL bInstant)
{
    HI_S32 s32Ret;
    HI_U8 au8Buf[184];

    s32Ret = IVE_IOCTL_Init();
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pIveHandle == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pIveHandle is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    s32Ret = IveCheckEqualizeHistParamUser(pstSrc, pstDst, pstEqualizeHistCtrl);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check EqualizeHist parameters failed!\n", s32Ret);
        return s32Ret;
    }

    memset_s(au8Buf, sizeof(au8Buf), 0, sizeof(au8Buf));
    memcpy_s(&au8Buf[8], sizeof(IVE_IMAGE_S), pstSrc, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[80], sizeof(IVE_IMAGE_S), pstDst, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[152], sizeof(IVE_EQUALIZE_HIST_CTRL_S), pstEqualizeHistCtrl, sizeof(IVE_EQUALIZE_HIST_CTRL_S));
    *(HI_BOOL *)&au8Buf[176] = bInstant;

    s32Ret = ioctl(s_s32IveFd, IOC_IVE_EQUALIZE_HIST, au8Buf);
    *pIveHandle = *(IVE_HANDLE *)&au8Buf[0];
    return s32Ret;
}

HI_S32 HI_MPI_IVE_Add(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc1,
    IVE_SRC_IMAGE_S *pstSrc2, IVE_DST_IMAGE_S *pstDst,
    IVE_ADD_CTRL_S *pstAddCtrl, HI_BOOL bInstant)
{
    HI_S32 s32Ret;
    HI_U8 au8Buf[232];

    s32Ret = IVE_IOCTL_Init();
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pIveHandle == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pIveHandle is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    s32Ret = IveCheckAddParamUser(pstSrc1, pstSrc2, pstDst, pstAddCtrl);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check Add parameters failed!\n", s32Ret);
        return s32Ret;
    }

    memset_s(au8Buf, sizeof(au8Buf), 0, sizeof(au8Buf));
    memcpy_s(&au8Buf[8], sizeof(IVE_IMAGE_S), pstSrc1, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[80], sizeof(IVE_IMAGE_S), pstSrc2, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[152], sizeof(IVE_IMAGE_S), pstDst, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[224], sizeof(IVE_ADD_CTRL_S), pstAddCtrl, sizeof(IVE_ADD_CTRL_S));
    *(HI_BOOL *)&au8Buf[228] = bInstant;

    s32Ret = ioctl(s_s32IveFd, IOC_IVE_ADD, au8Buf);
    *pIveHandle = *(IVE_HANDLE *)&au8Buf[0];
    return s32Ret;
}

HI_S32 HI_MPI_IVE_NCC(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc1,
    IVE_SRC_IMAGE_S *pstSrc2, IVE_DST_MEM_INFO_S *pstDst, HI_BOOL bInstant)
{
    HI_S32 s32Ret;
    HI_U8 au8Buf[184];

    s32Ret = IVE_IOCTL_Init();
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pIveHandle == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pIveHandle is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    s32Ret = IveCheckNCCParamUser(pstSrc1, pstSrc2, pstDst);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check NCC parameters failed!\n", s32Ret);
        return s32Ret;
    }

    memset_s(au8Buf, sizeof(au8Buf), 0, sizeof(au8Buf));
    memcpy_s(&au8Buf[8], sizeof(IVE_IMAGE_S), pstSrc1, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[80], sizeof(IVE_IMAGE_S), pstSrc2, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[152], sizeof(IVE_MEM_INFO_S), pstDst, sizeof(IVE_MEM_INFO_S));
    *(HI_BOOL *)&au8Buf[176] = bInstant;

    s32Ret = ioctl(s_s32IveFd, IOC_IVE_NCC, au8Buf);
    *pIveHandle = *(IVE_HANDLE *)&au8Buf[0];
    return s32Ret;
}

HI_S32 HI_MPI_IVE_CCL(IVE_HANDLE *pIveHandle, IVE_IMAGE_S *pstSrcDst,
    IVE_DST_MEM_INFO_S *pstBlob, IVE_CCL_CTRL_S *pstCclCtrl, HI_BOOL bInstant)
{
    HI_S32 s32Ret;
    HI_U8 au8Buf[120];

    s32Ret = IVE_IOCTL_Init();
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pIveHandle == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pIveHandle is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    s32Ret = IveCheckCCLParamUser(pstSrcDst, pstBlob, pstCclCtrl);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check CCL parameters failed!\n", s32Ret);
        return s32Ret;
    }

    memset_s(au8Buf, sizeof(au8Buf), 0, sizeof(au8Buf));
    memcpy_s(&au8Buf[8], sizeof(IVE_IMAGE_S), pstSrcDst, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[80], sizeof(IVE_MEM_INFO_S), pstBlob, sizeof(IVE_MEM_INFO_S));
    memcpy_s(&au8Buf[104], sizeof(IVE_CCL_CTRL_S), pstCclCtrl, sizeof(IVE_CCL_CTRL_S));
    *(HI_BOOL *)&au8Buf[112] = bInstant;

    s32Ret = ioctl(s_s32IveFd, IOC_IVE_CCL, au8Buf);
    *pIveHandle = *(IVE_HANDLE *)&au8Buf[0];
    return s32Ret;
}

HI_S32 HI_MPI_IVE_GMM(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
    IVE_DST_IMAGE_S *pstFg, IVE_DST_IMAGE_S *pstBg,
    IVE_MEM_INFO_S *pstModel, IVE_GMM_CTRL_S *pstGmmCtrl, HI_BOOL bInstant)
{
    HI_S32 s32Ret;
    HI_U8 au8Buf[280];

    s32Ret = IVE_IOCTL_Init();
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pIveHandle == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pIveHandle is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    s32Ret = IveCheckGMMParamUser(pstSrc, pstFg, pstBg, pstModel, pstGmmCtrl);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check GMM parameters failed!\n", s32Ret);
        return s32Ret;
    }

    memset_s(au8Buf, sizeof(au8Buf), 0, sizeof(au8Buf));
    memcpy_s(&au8Buf[8], sizeof(IVE_IMAGE_S), pstSrc, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[80], sizeof(IVE_IMAGE_S), pstFg, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[152], sizeof(IVE_IMAGE_S), pstBg, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[224], sizeof(IVE_MEM_INFO_S), pstModel, sizeof(IVE_MEM_INFO_S));
    memcpy_s(&au8Buf[248], sizeof(IVE_GMM_CTRL_S), pstGmmCtrl, sizeof(IVE_GMM_CTRL_S));
    *(HI_BOOL *)&au8Buf[272] = bInstant;

    s32Ret = ioctl(s_s32IveFd, IOC_IVE_GMM, au8Buf);
    *pIveHandle = *(IVE_HANDLE *)&au8Buf[0];
    return s32Ret;
}

HI_S32 HI_MPI_IVE_GMM2(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
    IVE_SRC_IMAGE_S *pstFactor, IVE_DST_IMAGE_S *pstFg,
    IVE_DST_IMAGE_S *pstBg, IVE_DST_IMAGE_S *pstMatchModelInfo,
    IVE_MEM_INFO_S *pstModel, IVE_GMM2_CTRL_S *pstGmm2Ctrl, HI_BOOL bInstant)
{
    HI_S32 s32Ret;
    HI_U8 au8Buf[424];

    s32Ret = IVE_IOCTL_Init();
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pIveHandle == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pIveHandle is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    s32Ret = IveCheckGMM2ParamUser(pstSrc, pstFactor, pstFg, pstBg, pstMatchModelInfo, pstModel, pstGmm2Ctrl);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check GMM2 parameters failed!\n", s32Ret);
        return s32Ret;
    }

    memset_s(au8Buf, sizeof(au8Buf), 0, sizeof(au8Buf));
    memcpy_s(&au8Buf[8], sizeof(IVE_IMAGE_S), pstSrc, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[80], sizeof(IVE_IMAGE_S), pstFactor, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[152], sizeof(IVE_IMAGE_S), pstFg, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[224], sizeof(IVE_IMAGE_S), pstBg, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[296], sizeof(IVE_IMAGE_S), pstMatchModelInfo, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[368], sizeof(IVE_MEM_INFO_S), pstModel, sizeof(IVE_MEM_INFO_S));
    memcpy_s(&au8Buf[392], sizeof(IVE_GMM2_CTRL_S), pstGmm2Ctrl, sizeof(IVE_GMM2_CTRL_S));
    *(HI_BOOL *)&au8Buf[420] = bInstant;

    s32Ret = ioctl(s_s32IveFd, IOC_IVE_GMM2, au8Buf);
    *pIveHandle = *(IVE_HANDLE *)&au8Buf[0];
    return s32Ret;
}

HI_S32 HI_MPI_IVE_CannyHysEdge(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
    IVE_DST_IMAGE_S *pstEdge, IVE_DST_MEM_INFO_S *pstStack,
    IVE_CANNY_HYS_EDGE_CTRL_S *pstCannyHysEdgeCtrl, HI_BOOL bInstant)
{
    HI_S32 s32Ret;
    HI_U8 au8Buf[240];

    s32Ret = IVE_IOCTL_Init();
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pIveHandle == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pIveHandle is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    s32Ret = IveCheckCannyHysEdgeParamUser(pstSrc, pstEdge, pstStack, pstCannyHysEdgeCtrl);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check CannyHysEdge parameters failed!\n", s32Ret);
        return s32Ret;
    }

    memset_s(au8Buf, sizeof(au8Buf), 0, sizeof(au8Buf));
    memcpy_s(&au8Buf[8], sizeof(IVE_IMAGE_S), pstSrc, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[80], sizeof(IVE_IMAGE_S), pstEdge, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[152], sizeof(IVE_MEM_INFO_S), pstStack, sizeof(IVE_MEM_INFO_S));
    memcpy_s(&au8Buf[176], sizeof(IVE_CANNY_HYS_EDGE_CTRL_S), pstCannyHysEdgeCtrl, sizeof(IVE_CANNY_HYS_EDGE_CTRL_S));
    *(HI_BOOL *)&au8Buf[232] = bInstant;

    s32Ret = ioctl(s_s32IveFd, IOC_IVE_CANNY_HYS_EDGE, au8Buf);
    *pIveHandle = *(IVE_HANDLE *)&au8Buf[0];
    return s32Ret;
}

HI_S32 HI_MPI_IVE_LBP(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
    IVE_DST_IMAGE_S *pstDst, IVE_LBP_CTRL_S *pstLbpCtrl, HI_BOOL bInstant)
{
    HI_S32 s32Ret;
    HI_U8 au8Buf[168];

    s32Ret = IVE_IOCTL_Init();
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pIveHandle == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pIveHandle is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    s32Ret = IveCheckLBPParamUser(pstSrc, pstDst, pstLbpCtrl);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check LBP parameters failed!\n", s32Ret);
        return s32Ret;
    }

    memset_s(au8Buf, sizeof(au8Buf), 0, sizeof(au8Buf));
    memcpy_s(&au8Buf[8], sizeof(IVE_IMAGE_S), pstSrc, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[80], sizeof(IVE_IMAGE_S), pstDst, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[152], sizeof(IVE_LBP_CTRL_S), pstLbpCtrl, sizeof(IVE_LBP_CTRL_S));
    *(HI_BOOL *)&au8Buf[160] = bInstant;

    s32Ret = ioctl(s_s32IveFd, IOC_IVE_LBP, au8Buf);
    *pIveHandle = *(IVE_HANDLE *)&au8Buf[0];
    return s32Ret;
}

HI_S32 HI_MPI_IVE_NormGrad(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
    IVE_DST_IMAGE_S *pstDstH, IVE_DST_IMAGE_S *pstDstV,
    IVE_DST_IMAGE_S *pstDstHV, IVE_NORM_GRAD_CTRL_S *pstNormGradCtrl, HI_BOOL bInstant)
{
    HI_S32 s32Ret;
    HI_U8 au8Buf[336];

    s32Ret = IVE_IOCTL_Init();
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pIveHandle == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pIveHandle is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    s32Ret = IveCheckNormGradParamUser(pstSrc, pstDstH, pstDstV, pstDstHV, pstNormGradCtrl);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check NormGrad parameters failed!\n", s32Ret);
        return s32Ret;
    }

    memset_s(au8Buf, sizeof(au8Buf), 0, sizeof(au8Buf));
    memcpy_s(&au8Buf[8], sizeof(IVE_IMAGE_S), pstSrc, sizeof(IVE_IMAGE_S));
    if (pstNormGradCtrl->enOutCtrl == IVE_NORM_GRAD_OUT_CTRL_HOR_AND_VER ||
        pstNormGradCtrl->enOutCtrl == IVE_NORM_GRAD_OUT_CTRL_HOR)
        memcpy_s(&au8Buf[80], sizeof(IVE_IMAGE_S), pstDstH, sizeof(IVE_IMAGE_S));
    if (pstNormGradCtrl->enOutCtrl == IVE_NORM_GRAD_OUT_CTRL_HOR_AND_VER ||
        pstNormGradCtrl->enOutCtrl == IVE_NORM_GRAD_OUT_CTRL_VER)
        memcpy_s(&au8Buf[152], sizeof(IVE_IMAGE_S), pstDstV, sizeof(IVE_IMAGE_S));
    if (pstNormGradCtrl->enOutCtrl == IVE_NORM_GRAD_OUT_CTRL_COMBINE)
        memcpy_s(&au8Buf[224], sizeof(IVE_IMAGE_S), pstDstHV, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[296], sizeof(IVE_NORM_GRAD_CTRL_S), pstNormGradCtrl, sizeof(IVE_NORM_GRAD_CTRL_S));
    *(HI_BOOL *)&au8Buf[328] = bInstant;

    s32Ret = ioctl(s_s32IveFd, IOC_IVE_NORM_GRAD, au8Buf);
    *pIveHandle = *(IVE_HANDLE *)&au8Buf[0];
    return s32Ret;
}

HI_S32 HI_MPI_IVE_LKOpticalFlowPyr(IVE_HANDLE *pIveHandle,
    IVE_SRC_IMAGE_S astSrcPrevPyr[], IVE_SRC_IMAGE_S astSrcNextPyr[],
    IVE_SRC_MEM_INFO_S *pstPrevPts, IVE_MEM_INFO_S *pstNextPts,
    IVE_DST_MEM_INFO_S *pstStatus, IVE_DST_MEM_INFO_S *pstErr,
    IVE_LK_OPTICAL_FLOW_PYR_CTRL_S *pstLkOptiFlowCtrl, HI_BOOL bInstant)
{
    HI_S32 s32Ret;
    HI_U8 au8Buf[704];

    s32Ret = IVE_IOCTL_Init();
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pIveHandle == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pIveHandle is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    s32Ret = IveCheckLKOpticalFlowPyrParamUser(astSrcPrevPyr, astSrcNextPyr,
        pstPrevPts, pstNextPts, pstStatus, pstErr, pstLkOptiFlowCtrl);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check LKOpticalFlowPyr parameters failed!\n", s32Ret);
        return s32Ret;
    }

    memset_s(au8Buf, sizeof(au8Buf), 0, sizeof(au8Buf));
    memcpy_s(&au8Buf[8], 4 * sizeof(IVE_IMAGE_S), astSrcPrevPyr, 4 * sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[296], 4 * sizeof(IVE_IMAGE_S), astSrcNextPyr, 4 * sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[584], sizeof(IVE_MEM_INFO_S), pstPrevPts, sizeof(IVE_MEM_INFO_S));
    memcpy_s(&au8Buf[608], sizeof(IVE_MEM_INFO_S), pstNextPts, sizeof(IVE_MEM_INFO_S));
    memcpy_s(&au8Buf[632], sizeof(IVE_MEM_INFO_S), pstStatus, sizeof(IVE_MEM_INFO_S));
    memcpy_s(&au8Buf[656], sizeof(IVE_MEM_INFO_S), pstErr, sizeof(IVE_MEM_INFO_S));
    memcpy_s(&au8Buf[680], sizeof(IVE_LK_OPTICAL_FLOW_PYR_CTRL_S), pstLkOptiFlowCtrl, sizeof(IVE_LK_OPTICAL_FLOW_PYR_CTRL_S));
    *(HI_BOOL *)&au8Buf[696] = bInstant;

    s32Ret = ioctl(s_s32IveFd, IOC_IVE_LK_OPTICAL_FLOW_PYR, au8Buf);
    *pIveHandle = *(IVE_HANDLE *)&au8Buf[0];
    return s32Ret;
}

HI_S32 HI_MPI_IVE_STCandiCorner(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
    IVE_DST_IMAGE_S *pstCandiCorner, IVE_ST_CANDI_CORNER_CTRL_S *pstStCandiCornerCtrl,
    HI_BOOL bInstant)
{
    HI_S32 s32Ret;
    HI_U8 au8Buf[192];

    s32Ret = IVE_IOCTL_Init();
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pIveHandle == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pIveHandle is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    s32Ret = IveCheckSTCandiCornerParamUser(pstSrc, pstCandiCorner, pstStCandiCornerCtrl);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check STCandiCorner parameters failed!\n", s32Ret);
        return s32Ret;
    }

    memset_s(au8Buf, sizeof(au8Buf), 0, sizeof(au8Buf));
    memcpy_s(&au8Buf[8], sizeof(IVE_IMAGE_S), pstSrc, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[80], sizeof(IVE_IMAGE_S), pstCandiCorner, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[152], sizeof(IVE_ST_CANDI_CORNER_CTRL_S), pstStCandiCornerCtrl, sizeof(IVE_ST_CANDI_CORNER_CTRL_S));
    *(HI_BOOL *)&au8Buf[184] = bInstant;

    s32Ret = ioctl(s_s32IveFd, IOC_IVE_ST_CANDI_CORNER, au8Buf);
    *pIveHandle = *(IVE_HANDLE *)&au8Buf[0];
    return s32Ret;
}

HI_S32 HI_MPI_IVE_GradFg(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstBgDiffFg,
    IVE_SRC_IMAGE_S *pstCurGrad, IVE_SRC_IMAGE_S *pstBgGrad,
    IVE_DST_IMAGE_S *pstGradFg, IVE_GRAD_FG_CTRL_S *pstGradFgCtrl, HI_BOOL bInstant)
{
    HI_S32 s32Ret;
    HI_U8 au8Buf[312];

    s32Ret = IVE_IOCTL_Init();
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pIveHandle == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pIveHandle is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    s32Ret = IveCheckGradFgParamUser(pstBgDiffFg, pstCurGrad, pstBgGrad, pstGradFg, pstGradFgCtrl);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check GradFg parameters failed!\n", s32Ret);
        return s32Ret;
    }

    memset_s(au8Buf, sizeof(au8Buf), 0, sizeof(au8Buf));
    memcpy_s(&au8Buf[8], sizeof(IVE_IMAGE_S), pstBgDiffFg, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[80], sizeof(IVE_IMAGE_S), pstCurGrad, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[152], sizeof(IVE_IMAGE_S), pstBgGrad, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[224], sizeof(IVE_IMAGE_S), pstGradFg, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[296], sizeof(IVE_GRAD_FG_CTRL_S), pstGradFgCtrl, sizeof(IVE_GRAD_FG_CTRL_S));
    *(HI_BOOL *)&au8Buf[308] = bInstant;

    s32Ret = ioctl(s_s32IveFd, IOC_IVE_GRAD_FG, au8Buf);
    *pIveHandle = *(IVE_HANDLE *)&au8Buf[0];
    return s32Ret;
}

HI_S32 HI_MPI_IVE_MatchBgModel(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstCurImg,
    IVE_DATA_S *pstBgModel, IVE_IMAGE_S *pstFgFlag, IVE_DST_IMAGE_S *pstBgDiffFg,
    IVE_DST_IMAGE_S *pstFrmDiffFg, IVE_DST_MEM_INFO_S *pstStatData,
    IVE_MATCH_BG_MODEL_CTRL_S *pstMatchBgModelCtrl, HI_BOOL bInstant)
{
    HI_S32 s32Ret;
    HI_U8 au8Buf[448];

    s32Ret = IVE_IOCTL_Init();
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pIveHandle == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pIveHandle is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    s32Ret = IveCheckMatchBgModelParamUser(pstCurImg, pstBgModel, pstFgFlag,
        pstBgDiffFg, pstStatData, pstMatchBgModelCtrl);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check MatchBgModel parameters failed!\n", s32Ret);
        return s32Ret;
    }

    memset_s(au8Buf, sizeof(au8Buf), 0, sizeof(au8Buf));
    memcpy_s(&au8Buf[8], sizeof(IVE_IMAGE_S), pstCurImg, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[80], sizeof(IVE_DATA_S), pstBgModel, sizeof(IVE_DATA_S));
    memcpy_s(&au8Buf[112], sizeof(IVE_IMAGE_S), pstFgFlag, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[184], sizeof(IVE_IMAGE_S), pstBgDiffFg, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[256], sizeof(IVE_IMAGE_S), pstFrmDiffFg, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[328], sizeof(IVE_MEM_INFO_S), pstStatData, sizeof(IVE_MEM_INFO_S));
    memcpy_s(&au8Buf[352], sizeof(IVE_MATCH_BG_MODEL_CTRL_S), pstMatchBgModelCtrl, sizeof(IVE_MATCH_BG_MODEL_CTRL_S));
    *(HI_BOOL *)&au8Buf[440] = bInstant;

    s32Ret = ioctl(s_s32IveFd, IOC_IVE_MATCH_BG_MODEL, au8Buf);
    *pIveHandle = *(IVE_HANDLE *)&au8Buf[0];
    return s32Ret;
}

HI_S32 HI_MPI_IVE_UpdateBgModel(IVE_HANDLE *pIveHandle, IVE_DATA_S *pstBgModel,
    IVE_IMAGE_S *pstFgFlag, IVE_DST_IMAGE_S *pstBgImg, IVE_DST_IMAGE_S *pstChgStaImg,
    IVE_DST_IMAGE_S *pstChgStaFg, IVE_DST_IMAGE_S *pstChgStaLife,
    IVE_DST_MEM_INFO_S *pstStatData, IVE_UPDATE_BG_MODEL_CTRL_S *pstUpdateBgModelCtrl,
    HI_BOOL bInstant)
{
    HI_S32 s32Ret;
    HI_U8 au8Buf[616];

    s32Ret = IVE_IOCTL_Init();
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pIveHandle == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pIveHandle is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    s32Ret = IveCheckUpdateBgModelParamUser(pstBgModel, pstFgFlag,
        pstBgImg, pstChgStaImg, pstStatData, pstUpdateBgModelCtrl);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check UpdateBgModel parameters failed!\n", s32Ret);
        return s32Ret;
    }

    memset_s(au8Buf, sizeof(au8Buf), 0, sizeof(au8Buf));
    memcpy_s(&au8Buf[8], sizeof(IVE_DATA_S), pstBgModel, sizeof(IVE_DATA_S));
    memcpy_s(&au8Buf[40], sizeof(IVE_IMAGE_S), pstFgFlag, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[112], sizeof(IVE_IMAGE_S), pstBgImg, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[184], sizeof(IVE_IMAGE_S), pstChgStaImg, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[256], sizeof(IVE_IMAGE_S), pstChgStaFg, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[328], sizeof(IVE_IMAGE_S), pstChgStaLife, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[400], sizeof(IVE_MEM_INFO_S), pstStatData, sizeof(IVE_MEM_INFO_S));
    memcpy_s(&au8Buf[424], sizeof(IVE_UPDATE_BG_MODEL_CTRL_S), pstUpdateBgModelCtrl, sizeof(IVE_UPDATE_BG_MODEL_CTRL_S));
    *(HI_BOOL *)&au8Buf[608] = bInstant;

    s32Ret = ioctl(s_s32IveFd, IOC_IVE_UPDATE_BG_MODEL, au8Buf);
    *pIveHandle = *(IVE_HANDLE *)&au8Buf[0];
    return s32Ret;
}

HI_S32 HI_MPI_IVE_SAD(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc1,
    IVE_SRC_IMAGE_S *pstSrc2, IVE_DST_IMAGE_S *pstSad,
    IVE_DST_IMAGE_S *pstThr, IVE_SAD_CTRL_S *pstSadCtrl, HI_BOOL bInstant)
{
    HI_S32 s32Ret;
    HI_U8 au8Buf[312];

    s32Ret = IVE_IOCTL_Init();
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pIveHandle == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pIveHandle is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    s32Ret = IveCheckSADParamUser(pstSrc1, pstSrc2, pstSad, pstThr, pstSadCtrl);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check SAD parameters failed!\n", s32Ret);
        return s32Ret;
    }

    memset_s(au8Buf, sizeof(au8Buf), 0, sizeof(au8Buf));
    memcpy_s(&au8Buf[8], sizeof(IVE_IMAGE_S), pstSrc1, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[80], sizeof(IVE_IMAGE_S), pstSrc2, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[152], sizeof(IVE_IMAGE_S), pstSad, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[224], sizeof(IVE_IMAGE_S), pstThr, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[296], sizeof(IVE_SAD_CTRL_S), pstSadCtrl, sizeof(IVE_SAD_CTRL_S));
    *(HI_BOOL *)&au8Buf[308] = bInstant;

    s32Ret = ioctl(s_s32IveFd, IOC_IVE_SAD, au8Buf);
    *pIveHandle = *(IVE_HANDLE *)&au8Buf[0];
    return s32Ret;
}

/* -------------------------------------------------------------------------- */
/*  Complex userspace-only operations                                         */
/* -------------------------------------------------------------------------- */

HI_S32 HI_MPI_IVE_CannyEdge(IVE_IMAGE_S *pstEdge, IVE_MEM_INFO_S *pstStack)
{
    HI_S32 s32Ret;
    HI_U32 u32Width, u32Height, u32Stride;
    HI_U8 *pu8Img;
    HI_U32 *pu32StackBase;
    HI_U32 u32StackCnt;
    HI_U32 *pu32StackPtr;
    HI_U16 u16Row, u16Col;
    HI_U16 u16MaxCol, u16MaxRow;
    HI_S32 rMin, rMax, cMin, cMax;
    HI_S32 nr, nc;
    HI_U32 i, j;

    s32Ret = IveCheckCannyEdgeParamUser(pstEdge, pstStack);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check CannyEdge parameters failed!\n", s32Ret);
        return s32Ret;
    }

    u32Width  = pstEdge->u32Width;
    u32Height = pstEdge->u32Height;
    u32Stride = pstEdge->au32Stride[0];
    pu8Img    = (HI_U8 *)(HI_UINTPTR_T)pstEdge->au64VirAddr[0];
    pu32StackBase = (HI_U32 *)(HI_UINTPTR_T)pstStack->u64VirAddr;

    u32StackCnt = pu32StackBase[u32Width * u32Height];
    pu32StackPtr = pu32StackBase + u32StackCnt;

    u16MaxCol = (HI_U16)(u32Width - 1);
    u16MaxRow = (HI_U16)(u32Height - 1);

    /* BFS edge tracing: pop confirmed edge, promote weak neighbors */
    while (u32StackCnt > 0) {
        u32StackCnt--;
        pu32StackBase[u32Width * u32Height] = u32StackCnt;
        pu32StackPtr--;

        u16Col = (HI_U16)(pu32StackPtr[0] & 0xFFFF);
        u16Row = (HI_U16)(pu32StackPtr[0] >> 16);

        rMin = (u16Row > 0) ? (HI_S32)(u16Row - 1) : 0;
        rMax = (u16Row < u16MaxRow) ? (HI_S32)(u16Row + 1) : (HI_S32)u16MaxRow;
        cMin = (u16Col > 0) ? (HI_S32)(u16Col - 1) : 0;
        cMax = (u16Col < u16MaxCol) ? (HI_S32)(u16Col + 1) : (HI_S32)u16MaxCol;

        for (nr = rMin; nr <= rMax; nr++) {
            for (nc = cMin; nc <= cMax; nc++) {
                if (nr == (HI_S32)u16Row && nc == (HI_S32)u16Col)
                    continue;
                if (pu8Img[(HI_U32)nr * u32Stride + (HI_U32)nc] == 0) {
                    pu8Img[(HI_U32)nr * u32Stride + (HI_U32)nc] = 2;
                    ((HI_U16 *)pu32StackPtr)[0] = (HI_U16)nc;
                    ((HI_U16 *)pu32StackPtr)[1] = (HI_U16)nr;
                    pu32StackPtr++;
                    u32StackCnt++;
                    pu32StackBase[u32Width * u32Height] = u32StackCnt;
                }
            }
        }
    }

    /* Final pass: 2 -> 0xFF (strong), everything else -> 0 */
    if (u32Height > 0) {
        HI_U8 *pRow = pu8Img;
        for (i = 0; i < u32Height; i++) {
            for (j = 0; j < u32Width; j++)
                pRow[j] = (HI_U8)(0 - (pRow[j] >> 1));
            pRow += u32Stride;
        }
    }

    return s32Ret;
}

HI_S32 HI_MPI_IVE_STCorner(IVE_SRC_IMAGE_S *pstCandiCorner,
    IVE_DST_MEM_INFO_S *pstCorner, IVE_ST_CORNER_CTRL_S *pstStCornerCtrl)
{
    HI_U32 u32Width, u32Height, u32Stride;
    HI_U16 u16MaxCornerNum, u16MinDist;
    HI_U8 *pu8Img;
    HI_U8 **ppSortBuf;
    HI_U32 u32NonZeroCnt;
    IVE_ST_CORNER_INFO_S *pstCornerInfo;
    HI_U32 i, j;

    if (pstCandiCorner == NULL || pstCorner == NULL || pstStCornerCtrl == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "NULL pointer!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    pstCornerInfo = (IVE_ST_CORNER_INFO_S *)(HI_UINTPTR_T)pstCorner->u64VirAddr;
    u32Width  = pstCandiCorner->u32Width;
    u32Height = pstCandiCorner->u32Height;
    u32Stride = pstCandiCorner->au32Stride[0];
    pu8Img    = (HI_U8 *)(HI_UINTPTR_T)pstCandiCorner->au64VirAddr[0];
    u16MaxCornerNum = pstStCornerCtrl->u16MaxCornerNum;
    u16MinDist = pstStCornerCtrl->u16MinDist;

    ppSortBuf = (HI_U8 **)malloc(u32Width * u32Height * sizeof(HI_U8 *));
    if (ppSortBuf == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "malloc ppSortBuf failed!\n");
        return HI_ERR_IVE_NOMEM;
    }

    /* Collect pointers to non-zero pixels (skip border rows/cols) */
    u32NonZeroCnt = 0;
    if (u32Height > 2) {
        HI_U8 *pRow = pu8Img + u32Stride;
        for (i = 1; i < u32Height - 1; i++) {
            for (j = 1; j < u32Width - 1; j++) {
                if (pRow[j] != 0)
                    ppSortBuf[u32NonZeroCnt++] = &pRow[j];
            }
            pRow += u32Stride;
        }
    }

    qsort(ppSortBuf, u32NonZeroCnt, sizeof(HI_U8 *), HI_Comp);

    if (u16MinDist == 0) {
        HI_U16 u16Count = (u32NonZeroCnt < u16MaxCornerNum) ?
                          (HI_U16)u32NonZeroCnt : u16MaxCornerNum;
        for (i = 0; i < u16Count; i++) {
            HI_U32 u32Off = (HI_U32)(ppSortBuf[i] - pu8Img);
            pstCornerInfo->astCorner[i].u16X = (HI_U16)(u32Off % u32Stride);
            pstCornerInfo->astCorner[i].u16Y = (HI_U16)(u32Off / u32Stride);
        }
        pstCornerInfo->u16CornerNum = u16Count;
        free(ppSortBuf);
        return HI_SUCCESS;
    }

    /* Grid-based NMS */
    {
        HI_U32 u32GridW = (u32Width + u16MinDist - 1) / u16MinDist;
        HI_U32 u32GridH = (u32Height + u16MinDist - 1) / u16MinDist;
        HI_U32 u32GridTotal = u32GridW * u32GridH;
        HI_U32 u32MinDistSq = (HI_U32)u16MinDist * u16MinDist;
        HI_U16 *pu16GridCnt;
        IVE_POINT_U16_S **ppGridCorners;
        HI_U16 u16CornerCnt = 0;

        pu16GridCnt = (HI_U16 *)malloc(u32GridTotal * sizeof(HI_U16));
        ppGridCorners = (IVE_POINT_U16_S **)malloc(u32GridTotal * sizeof(IVE_POINT_U16_S *));
        if (pu16GridCnt == NULL || ppGridCorners == NULL) {
            free(ppSortBuf);
            if (pu16GridCnt) free(pu16GridCnt);
            if (ppGridCorners) free(ppGridCorners);
            return HI_ERR_IVE_NOMEM;
        }
        memset(pu16GridCnt, 0, u32GridTotal * sizeof(HI_U16));

        for (i = 0; i < u32GridTotal; i++) {
            ppGridCorners[i] = (IVE_POINT_U16_S *)malloc(
                (HI_U32)u16MinDist * u16MinDist * sizeof(IVE_POINT_U16_S));
            if (ppGridCorners[i] == NULL) {
                for (j = 0; j < i; j++) free(ppGridCorners[j]);
                free(ppGridCorners);
                free(pu16GridCnt);
                free(ppSortBuf);
                return HI_ERR_IVE_NOMEM;
            }
        }

        for (i = 0; i < u32NonZeroCnt && u16CornerCnt < u16MaxCornerNum; i++) {
            HI_U32 u32Off = (HI_U32)(ppSortBuf[i] - pu8Img);
            HI_U32 u32PixRow = u32Off / u32Stride;
            HI_U32 u32PixCol = u32Off % u32Stride;
            HI_U32 u32GR = u32PixRow / u16MinDist;
            HI_U32 u32GC = u32PixCol / u16MinDist;
            HI_S32 grMin = ((HI_S32)u32GR > 0) ? (HI_S32)u32GR - 1 : 0;
            HI_S32 grMax = (u32GR + 1 < u32GridH) ? (HI_S32)(u32GR + 1) : (HI_S32)(u32GridH - 1);
            HI_S32 gcMin = ((HI_S32)u32GC > 0) ? (HI_S32)u32GC - 1 : 0;
            HI_S32 gcMax = (u32GC + 1 < u32GridW) ? (HI_S32)(u32GC + 1) : (HI_S32)(u32GridW - 1);
            HI_BOOL bTooClose = HI_FALSE;
            HI_S32 gr, gc;

            for (gr = grMin; gr <= grMax && !bTooClose; gr++) {
                for (gc = gcMin; gc <= gcMax && !bTooClose; gc++) {
                    HI_U32 cellIdx = (HI_U32)gr * u32GridW + (HI_U32)gc;
                    for (j = 0; j < pu16GridCnt[cellIdx]; j++) {
                        HI_S32 dx = (HI_S32)u32PixCol - (HI_S32)ppGridCorners[cellIdx][j].u16X;
                        HI_S32 dy = (HI_S32)u32PixRow - (HI_S32)ppGridCorners[cellIdx][j].u16Y;
                        if ((HI_U32)(dx * dx + dy * dy) < u32MinDistSq) {
                            bTooClose = HI_TRUE;
                            break;
                        }
                    }
                }
            }

            if (!bTooClose) {
                HI_U32 cellIdx = u32GR * u32GridW + u32GC;
                HI_U16 idx = pu16GridCnt[cellIdx];
                ppGridCorners[cellIdx][idx].u16X = (HI_U16)u32PixCol;
                ppGridCorners[cellIdx][idx].u16Y = (HI_U16)u32PixRow;
                pu16GridCnt[cellIdx] = idx + 1;

                pstCornerInfo->astCorner[u16CornerCnt].u16X = (HI_U16)u32PixCol;
                pstCornerInfo->astCorner[u16CornerCnt].u16Y = (HI_U16)u32PixRow;
                u16CornerCnt++;
            }
        }

        for (i = 0; i < u32GridTotal; i++) free(ppGridCorners[i]);
        free(ppGridCorners);
        free(pu16GridCnt);
        free(ppSortBuf);

        pstCornerInfo->u16CornerNum = u16CornerCnt;
    }

    return HI_SUCCESS;
}

/* -------------------------------------------------------------------------- */
/*  Resize (pointer indirection ioctl)                                        */
/* -------------------------------------------------------------------------- */

HI_S32 HI_MPI_IVE_Resize(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S astSrc[],
    IVE_DST_IMAGE_S astDst[], IVE_RESIZE_CTRL_S *pstResizeCtrl, HI_BOOL bInstant)
{
    HI_S32 s32Ret;
    HI_U8 au8DataBuf[9260]; /* src(4608) + dst(4608) + ctrl(40) + bInstant(4) */
    HI_U8 au8IoctlBuf[8];
    HI_U32 u32Num;

    s32Ret = IVE_IOCTL_Init();
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pIveHandle == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pIveHandle is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    s32Ret = IveCheckResizeParamUser(astSrc, astDst, pstResizeCtrl);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check Resize parameters failed!\n", s32Ret);
        return s32Ret;
    }

    u32Num = pstResizeCtrl->u16Num;
    memset_s(au8DataBuf, sizeof(au8DataBuf), 0, sizeof(au8DataBuf));
    memcpy_s(&au8DataBuf[0], u32Num * sizeof(IVE_IMAGE_S), astSrc, u32Num * sizeof(IVE_IMAGE_S));
    memcpy_s(&au8DataBuf[4608], u32Num * sizeof(IVE_IMAGE_S), astDst, u32Num * sizeof(IVE_IMAGE_S));
    memcpy_s(&au8DataBuf[9216], sizeof(IVE_RESIZE_CTRL_S), pstResizeCtrl, sizeof(IVE_RESIZE_CTRL_S));
    *(HI_BOOL *)&au8DataBuf[9256] = bInstant;

    /* Pointer indirection: ioctl buf = {handle, pointer_to_data} */
    *(IVE_HANDLE *)&au8IoctlBuf[0] = 0;
    *(HI_U32 *)&au8IoctlBuf[4] = (HI_U32)(HI_UINTPTR_T)au8DataBuf;

    s32Ret = ioctl(s_s32IveFd, IOC_IVE_RESIZE, au8IoctlBuf);
    *pIveHandle = *(IVE_HANDLE *)&au8IoctlBuf[0];
    return s32Ret;
}

/* -------------------------------------------------------------------------- */
/*  HOG (large ioctl)                                                         */
/* -------------------------------------------------------------------------- */

HI_S32 HI_MPI_IVE_Hog(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
    IVE_RECT_U32_S astRoi[], IVE_DST_BLOB_S astDst[],
    IVE_HOG_CTRL_S *pstHogCtrl, HI_BOOL bInstant)
{
    HI_S32 s32Ret;
    HI_U8 au8Buf[4200];

    s32Ret = IVE_IOCTL_Init();
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pIveHandle == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pIveHandle is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    s32Ret = IveCheckHogParamUser(pstSrc, astRoi, astDst, pstHogCtrl);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check Hog parameters failed!\n", s32Ret);
        return s32Ret;
    }

    memset_s(au8Buf, sizeof(au8Buf), 0, sizeof(au8Buf));
    memcpy_s(&au8Buf[8], sizeof(IVE_IMAGE_S), pstSrc, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[80], 64 * sizeof(IVE_RECT_U32_S), astRoi, pstHogCtrl->u32RoiNum * sizeof(IVE_RECT_U32_S));
    memcpy_s(&au8Buf[1104], 64 * sizeof(IVE_BLOB_S), astDst, pstHogCtrl->u32RoiNum * sizeof(IVE_BLOB_S));
    memcpy_s(&au8Buf[4176], sizeof(IVE_HOG_CTRL_S), pstHogCtrl, sizeof(IVE_HOG_CTRL_S));
    *(HI_BOOL *)&au8Buf[4192] = bInstant;

    s32Ret = ioctl(s_s32IveFd, IOC_IVE_HOG, au8Buf);
    *pIveHandle = *(IVE_HANDLE *)&au8Buf[0];
    return s32Ret;
}

/* -------------------------------------------------------------------------- */
/*  PerspTrans (large ioctl)                                                  */
/* -------------------------------------------------------------------------- */

HI_S32 HI_MPI_IVE_PerspTrans(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
    IVE_RECT_U32_S astRoi[], IVE_SRC_MEM_INFO_S astPointPair[],
    IVE_DST_IMAGE_S astDst[], IVE_PERSP_TRANS_CTRL_S *pstPerspTransCtrl,
    HI_BOOL bInstant)
{
    HI_S32 s32Ret;
    HI_U8 au8Buf[7264];

    s32Ret = IVE_IOCTL_Init();
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pIveHandle == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pIveHandle is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    s32Ret = IveCheckPerspTransParamUser(pstSrc, astRoi, astPointPair, astDst,
        NULL, pstPerspTransCtrl);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check PerspTrans parameters failed!\n", s32Ret);
        return s32Ret;
    }

    memset_s(au8Buf, sizeof(au8Buf), 0, sizeof(au8Buf));
    memcpy_s(&au8Buf[8], sizeof(IVE_IMAGE_S), pstSrc, sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[80], 64 * sizeof(IVE_RECT_U32_S), astRoi,
        pstPerspTransCtrl->u16RoiNum * sizeof(IVE_RECT_U32_S));
    memcpy_s(&au8Buf[1104], 64 * sizeof(IVE_MEM_INFO_S), astPointPair,
        pstPerspTransCtrl->u16RoiNum * sizeof(IVE_MEM_INFO_S));
    memcpy_s(&au8Buf[2640], 64 * sizeof(IVE_IMAGE_S), astDst,
        pstPerspTransCtrl->u16RoiNum * sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[7248], sizeof(IVE_PERSP_TRANS_CTRL_S), pstPerspTransCtrl, sizeof(IVE_PERSP_TRANS_CTRL_S));
    *(HI_BOOL *)&au8Buf[7260] = bInstant;

    s32Ret = ioctl(s_s32IveFd, IOC_IVE_PERSP_TRANS, au8Buf);
    *pIveHandle = *(IVE_HANDLE *)&au8Buf[0];
    return s32Ret;
}

/* -------------------------------------------------------------------------- */
/*  CNN operations                                                            */
/* -------------------------------------------------------------------------- */

HI_S32 HI_MPI_IVE_CNN_Predict(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S astSrc[],
    IVE_CNN_MODEL_S *pstCnnModel, IVE_DST_DATA_S *pstDst,
    IVE_CNN_CTRL_S *pstCnnCtrl, HI_BOOL bInstant)
{
    HI_S32 s32Ret;
    HI_U8 au8Buf[4920];

    s32Ret = IVE_IOCTL_Init();
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pIveHandle == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pIveHandle is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    s32Ret = IveCheckCNNPredictParamUser(astSrc, (IVE_SRC_MEM_INFO_S *)&pstCnnModel->stConvKernelBias,
        (IVE_DST_BLOB_S *)pstDst, pstCnnCtrl);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check CNN_Predict parameters failed!\n", s32Ret);
        return s32Ret;
    }

    memset_s(au8Buf, sizeof(au8Buf), 0, sizeof(au8Buf));
    memcpy_s(&au8Buf[8], 64 * sizeof(IVE_IMAGE_S), astSrc,
        pstCnnCtrl->u32Num * sizeof(IVE_IMAGE_S));
    memcpy_s(&au8Buf[4616], sizeof(IVE_CNN_MODEL_S), pstCnnModel, sizeof(IVE_CNN_MODEL_S));
    memcpy_s(&au8Buf[4848], sizeof(IVE_DATA_S), pstDst, sizeof(IVE_DATA_S));
    memcpy_s(&au8Buf[4880], sizeof(IVE_CNN_CTRL_S), pstCnnCtrl, sizeof(IVE_CNN_CTRL_S));
    *(HI_BOOL *)&au8Buf[4912] = bInstant;

    s32Ret = ioctl(s_s32IveFd, IOC_IVE_CNN_PREDICT, au8Buf);
    *pIveHandle = *(IVE_HANDLE *)&au8Buf[0];
    return s32Ret;
}

HI_S32 HI_MPI_IVE_CNN_LoadModel(const HI_CHAR *pchFileName,
    IVE_CNN_MODEL_S *pstCnnModel)
{
    HI_S32 s32Ret = HI_SUCCESS;
    FILE *fp;
    HI_CHAR achMagic[4];
    HI_U32 u32ConvKernelBiasSize, u32FCLWgtBiasSize, u32TotalSize;

    if (pchFileName == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pchFileName is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (pstCnnModel == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstCnnModel is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    fp = IveOpenFile(pchFileName, "rb");
    if (fp == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "open file %s failed!\n", pchFileName);
        return HI_ERR_IVE_OPEN_FILE;
    }

    if (fread(achMagic, 1, 4, fp) != 4 ||
        achMagic[0] != 'I' || achMagic[1] != 'V' || achMagic[2] != 'E') {
        HI_TRACE_IVE(HI_DBG_ERR, "invalid CNN model magic!\n");
        IveCloseFile(fp);
        return HI_ERR_IVE_READ_FILE;
    }

    if (fread(pstCnnModel, 1, sizeof(IVE_CNN_MODEL_S), fp) != sizeof(IVE_CNN_MODEL_S)) {
        HI_TRACE_IVE(HI_DBG_ERR, "read CNN model header failed!\n");
        IveCloseFile(fp);
        return HI_ERR_IVE_READ_FILE;
    }

    if (pstCnnModel->u8ConvPoolLayerNum == 0 || pstCnnModel->u8ConvPoolLayerNum > 8) {
        HI_TRACE_IVE(HI_DBG_ERR, "u8ConvPoolLayerNum(%d) out of [1,8]!\n",
            pstCnnModel->u8ConvPoolLayerNum);
        IveCloseFile(fp);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }

    if (pstCnnModel->stFullConnect.u8LayerNum < 3 ||
        pstCnnModel->stFullConnect.u8LayerNum > 8) {
        HI_TRACE_IVE(HI_DBG_ERR, "u8LayerNum(%d) out of [3,8]!\n",
            pstCnnModel->stFullConnect.u8LayerNum);
        IveCloseFile(fp);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }

    u32ConvKernelBiasSize = pstCnnModel->u32ConvKernelBiasSize;
    u32FCLWgtBiasSize = pstCnnModel->u32FCLWgtBiasSize;
    u32TotalSize = u32ConvKernelBiasSize + u32FCLWgtBiasSize;
    pstCnnModel->u32TotalMemSize = u32TotalSize;

    s32Ret = IveMalloc(&pstCnnModel->stConvKernelBias.u64PhyAddr,
        (HI_VOID **)&pstCnnModel->stConvKernelBias.u64VirAddr,
        "CnnModel", u32TotalSize);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "IveMalloc CNN model failed!\n");
        IveCloseFile(fp);
        return HI_ERR_IVE_NOMEM;
    }
    pstCnnModel->stConvKernelBias.u32Size = u32ConvKernelBiasSize;

    pstCnnModel->stFCLWgtBias.u64PhyAddr = pstCnnModel->stConvKernelBias.u64PhyAddr + u32ConvKernelBiasSize;
    pstCnnModel->stFCLWgtBias.u64VirAddr = pstCnnModel->stConvKernelBias.u64VirAddr + u32ConvKernelBiasSize;
    pstCnnModel->stFCLWgtBias.u32Size = u32FCLWgtBiasSize;

    if (fread((HI_VOID *)(HI_UINTPTR_T)pstCnnModel->stConvKernelBias.u64VirAddr,
              1, u32TotalSize, fp) != u32TotalSize) {
        HI_TRACE_IVE(HI_DBG_ERR, "read CNN model data failed!\n");
        IveFree(pstCnnModel->stConvKernelBias.u64PhyAddr,
                (HI_VOID *)(HI_UINTPTR_T)pstCnnModel->stConvKernelBias.u64VirAddr);
        memset_s(pstCnnModel, sizeof(IVE_CNN_MODEL_S), 0, sizeof(IVE_CNN_MODEL_S));
        IveCloseFile(fp);
        return HI_ERR_IVE_READ_FILE;
    }

    IveCloseFile(fp);
    return s32Ret;
}

HI_VOID HI_MPI_IVE_CNN_UnloadModel(IVE_CNN_MODEL_S *pstCnnModel)
{
    if (pstCnnModel == NULL) return;

    if (pstCnnModel->stConvKernelBias.u64PhyAddr != 0) {
        IveFree(pstCnnModel->stConvKernelBias.u64PhyAddr,
                (HI_VOID *)(HI_UINTPTR_T)pstCnnModel->stConvKernelBias.u64VirAddr);
    }
    memset_s(&pstCnnModel->stConvKernelBias, sizeof(IVE_MEM_INFO_S), 0, sizeof(IVE_MEM_INFO_S));
    memset_s(&pstCnnModel->stFCLWgtBias, sizeof(IVE_MEM_INFO_S), 0, sizeof(IVE_MEM_INFO_S));
    pstCnnModel->u32ConvKernelBiasSize = 0;
    pstCnnModel->u32FCLWgtBiasSize = 0;
    pstCnnModel->u32TotalMemSize = 0;
}

HI_S32 HI_MPI_IVE_CNN_GetResult(IVE_SRC_DATA_S *pstSrc, IVE_DST_MEM_INFO_S *pstDst,
    IVE_CNN_MODEL_S *pstCnnModel, IVE_CNN_CTRL_S *pstCnnCtrl)
{
    HI_S32 s32Ret = HI_SUCCESS;
    HI_DOUBLE adSoftmax[256];
    HI_U16 u16ClassCount;
    HI_U32 u32NumImages, u32SrcStride;
    HI_S32 *ps32SrcData;
    IVE_CNN_RESULT_S *pstResult;
    HI_U32 n, c;
    HI_DOUBLE dMax, dSum;
    HI_S32 s32MaxIdx, s32MaxConf;

    if (pstCnnModel == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstCnnModel is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (pstSrc == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstSrc is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (pstDst == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstDst is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (pstCnnCtrl == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstCnnCtrl is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    u16ClassCount = pstCnnModel->u16ClassCount;
    if (u16ClassCount == 0 || u16ClassCount > 256) {
        HI_TRACE_IVE(HI_DBG_ERR, "u16ClassCount(%d) out of [1,256]!\n", u16ClassCount);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }

    u32NumImages = pstCnnCtrl->u32Num;
    if (u32NumImages == 0 || u32NumImages > 64) {
        HI_TRACE_IVE(HI_DBG_ERR, "u32Num(%d) out of [1,64]!\n", u32NumImages);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }

    ps32SrcData = (HI_S32 *)(HI_UINTPTR_T)pstSrc->u64VirAddr;
    pstResult = (IVE_CNN_RESULT_S *)(HI_UINTPTR_T)pstDst->u64VirAddr;
    u32SrcStride = pstSrc->u32Stride;

    for (n = 0; n < u32NumImages; n++) {
        HI_S32 *pScores = (HI_S32 *)((HI_U8 *)ps32SrcData + n * u32SrcStride);

        dMax = (HI_DOUBLE)pScores[0] * (1.0 / 16384.0);
        adSoftmax[0] = dMax;
        for (c = 1; c < u16ClassCount; c++) {
            adSoftmax[c] = (HI_DOUBLE)pScores[c] * (1.0 / 16384.0);
            if (adSoftmax[c] > dMax)
                dMax = adSoftmax[c];
        }

        dSum = 0.0;
        for (c = 0; c < u16ClassCount; c++) {
            adSoftmax[c] = exp(adSoftmax[c] - dMax);
            dSum += adSoftmax[c];
        }

        s32MaxIdx = 0;
        s32MaxConf = (HI_S32)(adSoftmax[0] / dSum * 32768.0);
        for (c = 1; c < u16ClassCount; c++) {
            HI_S32 conf = (HI_S32)(adSoftmax[c] / dSum * 32768.0);
            if (conf > s32MaxConf) {
                s32MaxConf = conf;
                s32MaxIdx = (HI_S32)c;
            }
        }

        pstResult[n].s32ClassIdx = s32MaxIdx;
        pstResult[n].s32Confidence = s32MaxConf;
    }

    return s32Ret;
}

/* -------------------------------------------------------------------------- */
/*  ANN MLP operations                                                        */
/* -------------------------------------------------------------------------- */

HI_S32 HI_MPI_IVE_ANN_MLP_LoadModel(const HI_CHAR *pchFileName,
    IVE_ANN_MLP_MODEL_S *pstAnnMlpModel)
{
    HI_S32 s32Ret = HI_SUCCESS;
    FILE *fp;
    HI_CHAR achMagic[4];
    HI_U32 u32WeightSize;

    if (pchFileName == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pchFileName is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (pstAnnMlpModel == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstAnnMlpModel is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    fp = IveOpenFile(pchFileName, "rb");
    if (fp == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "open file %s failed!\n", pchFileName);
        return HI_ERR_IVE_OPEN_FILE;
    }

    if (fread(achMagic, 1, 4, fp) != 4 ||
        achMagic[0] != 'I' || achMagic[1] != 'V' || achMagic[2] != 'E') {
        HI_TRACE_IVE(HI_DBG_ERR, "invalid ANN MLP model magic!\n");
        IveCloseFile(fp);
        return HI_ERR_IVE_READ_FILE;
    }

    if (fread(pstAnnMlpModel, 1, sizeof(IVE_ANN_MLP_MODEL_S), fp) != sizeof(IVE_ANN_MLP_MODEL_S)) {
        HI_TRACE_IVE(HI_DBG_ERR, "read ANN MLP model header failed!\n");
        IveCloseFile(fp);
        return HI_ERR_IVE_READ_FILE;
    }

    if (pstAnnMlpModel->u8LayerNum < 3 || pstAnnMlpModel->u8LayerNum > 8) {
        HI_TRACE_IVE(HI_DBG_ERR, "u8LayerNum(%d) out of [3,8]!\n",
            pstAnnMlpModel->u8LayerNum);
        IveCloseFile(fp);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }

    u32WeightSize = pstAnnMlpModel->u32TotalWeightSize;

    s32Ret = IveMalloc(&pstAnnMlpModel->stWeight.u64PhyAddr,
        (HI_VOID **)&pstAnnMlpModel->stWeight.u64VirAddr,
        "AnnMlpModel", u32WeightSize);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "IveMalloc ANN MLP model failed!\n");
        IveCloseFile(fp);
        return HI_ERR_IVE_NOMEM;
    }
    pstAnnMlpModel->stWeight.u32Size = u32WeightSize;

    if (fread((HI_VOID *)(HI_UINTPTR_T)pstAnnMlpModel->stWeight.u64VirAddr,
              1, u32WeightSize, fp) != u32WeightSize) {
        HI_TRACE_IVE(HI_DBG_ERR, "read ANN MLP model data failed!\n");
        IveFree(pstAnnMlpModel->stWeight.u64PhyAddr,
                (HI_VOID *)(HI_UINTPTR_T)pstAnnMlpModel->stWeight.u64VirAddr);
        memset_s(pstAnnMlpModel, sizeof(IVE_ANN_MLP_MODEL_S), 0, sizeof(IVE_ANN_MLP_MODEL_S));
        IveCloseFile(fp);
        return HI_ERR_IVE_READ_FILE;
    }

    IveCloseFile(fp);
    return s32Ret;
}

HI_VOID HI_MPI_IVE_ANN_MLP_UnloadModel(IVE_ANN_MLP_MODEL_S *pstAnnMlpModel)
{
    if (pstAnnMlpModel == NULL) return;

    if (pstAnnMlpModel->stWeight.u64PhyAddr != 0) {
        IveFree(pstAnnMlpModel->stWeight.u64PhyAddr,
                (HI_VOID *)(HI_UINTPTR_T)pstAnnMlpModel->stWeight.u64VirAddr);
    }
    memset_s(&pstAnnMlpModel->stWeight, sizeof(IVE_MEM_INFO_S), 0, sizeof(IVE_MEM_INFO_S));
    pstAnnMlpModel->u32TotalWeightSize = 0;
}

HI_S32 HI_MPI_IVE_ANN_MLP_Predict(IVE_HANDLE *pIveHandle, IVE_SRC_DATA_S *pstSrc,
    IVE_LOOK_UP_TABLE_S *pstActivFuncTab, IVE_ANN_MLP_MODEL_S *pstAnnMlpModel,
    IVE_DST_DATA_S *pstDst, HI_BOOL bInstant)
{
    HI_S32 s32Ret;
    HI_U8 au8Buf[176];

    s32Ret = IVE_IOCTL_Init();
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pIveHandle == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pIveHandle is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    s32Ret = IveCheckANNMLPPredictParamUser(pstSrc, pstActivFuncTab, pstAnnMlpModel,
        pstDst, (IVE_SRC_MEM_INFO_S *)&pstAnnMlpModel->stWeight);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check ANN_MLP_Predict parameters failed!\n", s32Ret);
        return s32Ret;
    }

    memset_s(au8Buf, sizeof(au8Buf), 0, sizeof(au8Buf));
    memcpy_s(&au8Buf[8], sizeof(IVE_DATA_S), pstSrc, sizeof(IVE_DATA_S));
    memcpy_s(&au8Buf[40], sizeof(IVE_LOOK_UP_TABLE_S), pstActivFuncTab, sizeof(IVE_LOOK_UP_TABLE_S));
    memcpy_s(&au8Buf[80], sizeof(IVE_ANN_MLP_MODEL_S), pstAnnMlpModel, sizeof(IVE_ANN_MLP_MODEL_S));
    memcpy_s(&au8Buf[136], sizeof(IVE_DATA_S), pstDst, sizeof(IVE_DATA_S));
    *(HI_BOOL *)&au8Buf[168] = bInstant;

    s32Ret = ioctl(s_s32IveFd, IOC_IVE_ANN_MLP_PREDICT, au8Buf);
    *pIveHandle = *(IVE_HANDLE *)&au8Buf[0];
    return s32Ret;
}

/* -------------------------------------------------------------------------- */
/*  SVM operations                                                            */
/* -------------------------------------------------------------------------- */

HI_S32 HI_MPI_IVE_SVM_LoadModel(const HI_CHAR *pchFileName,
    IVE_SVM_MODEL_S *pstSvmModel)
{
    HI_S32 s32Ret = HI_SUCCESS;
    FILE *fp;
    HI_CHAR achMagic[4];
    HI_U32 u32SvSize, u32DfSize, u32TotalSize;

    if (pchFileName == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pchFileName is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (pstSvmModel == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstSvmModel is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    fp = IveOpenFile(pchFileName, "rb");
    if (fp == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "open file %s failed!\n", pchFileName);
        return HI_ERR_IVE_OPEN_FILE;
    }

    if (fread(achMagic, 1, 4, fp) != 4 ||
        achMagic[0] != 'I' || achMagic[1] != 'V' || achMagic[2] != 'E') {
        HI_TRACE_IVE(HI_DBG_ERR, "invalid SVM model magic!\n");
        IveCloseFile(fp);
        return HI_ERR_IVE_READ_FILE;
    }

    if (fread(pstSvmModel, 1, sizeof(IVE_SVM_MODEL_S), fp) != sizeof(IVE_SVM_MODEL_S)) {
        HI_TRACE_IVE(HI_DBG_ERR, "read SVM model header failed!\n");
        IveCloseFile(fp);
        return HI_ERR_IVE_READ_FILE;
    }

    u32SvSize = pstSvmModel->stSv.u32Size;
    u32DfSize = pstSvmModel->u32TotalDfSize;
    u32TotalSize = u32SvSize + u32DfSize;

    s32Ret = IveMalloc(&pstSvmModel->stSv.u64PhyAddr,
        (HI_VOID **)&pstSvmModel->stSv.u64VirAddr,
        "SvmModel", u32TotalSize);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "IveMalloc SVM model failed!\n");
        IveCloseFile(fp);
        return HI_ERR_IVE_NOMEM;
    }

    pstSvmModel->stDf.u64PhyAddr = pstSvmModel->stSv.u64PhyAddr + u32SvSize;
    pstSvmModel->stDf.u64VirAddr = pstSvmModel->stSv.u64VirAddr + u32SvSize;
    pstSvmModel->stDf.u32Size = u32DfSize;

    if (fread((HI_VOID *)(HI_UINTPTR_T)pstSvmModel->stSv.u64VirAddr,
              1, u32TotalSize, fp) != u32TotalSize) {
        HI_TRACE_IVE(HI_DBG_ERR, "read SVM model data failed!\n");
        IveFree(pstSvmModel->stSv.u64PhyAddr,
                (HI_VOID *)(HI_UINTPTR_T)pstSvmModel->stSv.u64VirAddr);
        memset_s(pstSvmModel, sizeof(IVE_SVM_MODEL_S), 0, sizeof(IVE_SVM_MODEL_S));
        IveCloseFile(fp);
        return HI_ERR_IVE_READ_FILE;
    }

    IveCloseFile(fp);
    return s32Ret;
}

HI_VOID HI_MPI_IVE_SVM_UnloadModel(IVE_SVM_MODEL_S *pstSvmModel)
{
    if (pstSvmModel == NULL) return;

    if (pstSvmModel->stSv.u64PhyAddr != 0) {
        IveFree(pstSvmModel->stSv.u64PhyAddr,
                (HI_VOID *)(HI_UINTPTR_T)pstSvmModel->stSv.u64VirAddr);
    }
    memset_s(&pstSvmModel->stSv, sizeof(IVE_MEM_INFO_S), 0, sizeof(IVE_MEM_INFO_S));
    memset_s(&pstSvmModel->stDf, sizeof(IVE_MEM_INFO_S), 0, sizeof(IVE_MEM_INFO_S));
    pstSvmModel->u32TotalDfSize = 0;
}

HI_S32 HI_MPI_IVE_SVM_Predict(IVE_HANDLE *pIveHandle, IVE_SRC_DATA_S *pstSrc,
    IVE_LOOK_UP_TABLE_S *pstKernelTab, IVE_SVM_MODEL_S *pstSvmModel,
    IVE_DST_DATA_S *pstDstVote, HI_BOOL bInstant)
{
    HI_S32 s32Ret;
    HI_U8 au8Buf[192];

    s32Ret = IVE_IOCTL_Init();
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pIveHandle == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pIveHandle is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    s32Ret = IveCheckSVMPredictParamUser(pstSrc, pstKernelTab, pstSvmModel,
        pstDstVote, (IVE_SRC_MEM_INFO_S *)&pstSvmModel->stSv);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check SVM_Predict parameters failed!\n", s32Ret);
        return s32Ret;
    }

    memset_s(au8Buf, sizeof(au8Buf), 0, sizeof(au8Buf));
    memcpy_s(&au8Buf[8], sizeof(IVE_DATA_S), pstSrc, sizeof(IVE_DATA_S));
    memcpy_s(&au8Buf[40], sizeof(IVE_LOOK_UP_TABLE_S), pstKernelTab, sizeof(IVE_LOOK_UP_TABLE_S));
    memcpy_s(&au8Buf[80], sizeof(IVE_SVM_MODEL_S), pstSvmModel, sizeof(IVE_SVM_MODEL_S));
    memcpy_s(&au8Buf[152], sizeof(IVE_DATA_S), pstDstVote, sizeof(IVE_DATA_S));
    *(HI_BOOL *)&au8Buf[184] = bInstant;

    s32Ret = ioctl(s_s32IveFd, IOC_IVE_SVM_PREDICT, au8Buf);
    *pIveHandle = *(IVE_HANDLE *)&au8Buf[0];
    return s32Ret;
}

/* -------------------------------------------------------------------------- */
/*  KCF operations                                                            */
/* -------------------------------------------------------------------------- */

HI_S32 HI_MPI_IVE_KCF_GetMemSize(HI_U32 u32MaxObjNum, HI_U32 *pu32Size)
{
    if (pu32Size == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pu32Size is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (u32MaxObjNum == 0 || u32MaxObjNum > 32) {
        HI_TRACE_IVE(HI_DBG_ERR, "u32MaxObjNum(%d) out of [1,32]!\n", u32MaxObjNum);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }

    *pu32Size = u32MaxObjNum * 55824;
    return HI_SUCCESS;
}

HI_S32 HI_MPI_IVE_KCF_CreateObjList(IVE_MEM_INFO_S *pstMem, HI_U32 u32MaxObjNum,
    IVE_KCF_OBJ_LIST_S *pstObjList)
{
    HI_S32 s32Ret;

    s32Ret = IveCheckKcfObjListParamUser(pstMem, u32MaxObjNum, pstObjList);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check KCF ObjList parameters failed!\n", s32Ret);
        return s32Ret;
    }

    s32Ret = IVE_CreateObjList(pstMem, pstObjList, u32MaxObjNum);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "IVE_CreateObjList failed!\n");
        return s32Ret;
    }

    /* Allocate cached TmpBuf for KCF processing */
    {
        HI_U64 u64TmpPhyAddr;
        s32Ret = IveMalloc_Cached(&u64TmpPhyAddr,
            (HI_VOID **)&pstObjList->pu8TmpBuf, IVE_KCF_TMP_BUF_SIZE);
        if (s32Ret != HI_SUCCESS) {
            HI_TRACE_IVE(HI_DBG_ERR, "IveMalloc_Cached TmpBuf failed!\n");
            IVE_DestroyObjList(pstObjList);
            return s32Ret;
        }
    }

    return HI_SUCCESS;
}

HI_S32 HI_MPI_IVE_KCF_DestroyObjList(IVE_KCF_OBJ_LIST_S *pstObjList)
{
    if (pstObjList == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstObjList is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    if (pstObjList->pu8TmpBuf != NULL) {
        IveFree(0, pstObjList->pu8TmpBuf);
        pstObjList->pu8TmpBuf = NULL;
    }

    IVE_DestroyObjList(pstObjList);
    return HI_SUCCESS;
}

HI_S32 HI_MPI_IVE_KCF_CreateGaussPeak(HI_U3Q5 u3q5Padding,
    IVE_DST_MEM_INFO_S *pstGaussPeak)
{
    HI_S32 s32Ret;
    HI_U64 au64Out[3];

    s32Ret = IveCheckKcfGaussPeakParamUser(u3q5Padding, pstGaussPeak);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check KCF GaussPeak parameters failed!\n", s32Ret);
        return s32Ret;
    }

    au64Out[0] = 0;
    au64Out[1] = 0;
    au64Out[2] = 0;
    IveGetGaussPeakMem(160, 160, 0, 0, au64Out);

    s32Ret = IveMalloc(&pstGaussPeak->u64PhyAddr,
        (HI_VOID **)&pstGaussPeak->u64VirAddr,
        "GaussPeak", (HI_U32)au64Out[0]);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "IveMalloc GaussPeak failed!\n");
        return s32Ret;
    }
    pstGaussPeak->u32Size = (HI_U32)au64Out[0];

    return HI_SUCCESS;
}

HI_S32 HI_MPI_IVE_KCF_CreateCosWin(IVE_DST_MEM_INFO_S *pstCosWinX,
    IVE_DST_MEM_INFO_S *pstCosWinY)
{
    HI_S32 s32Ret;
    HI_U16 au16BufX[32], au16BufY[32];

    s32Ret = IveCheckKcfCosWinParamUser(pstCosWinX, pstCosWinY);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check KCF CosWin parameters failed!\n", s32Ret);
        return s32Ret;
    }

    s32Ret = IveMalloc(&pstCosWinX->u64PhyAddr,
        (HI_VOID **)&pstCosWinX->u64VirAddr, "CosWinX", 64);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "IveMalloc CosWinX failed!\n");
        return s32Ret;
    }
    pstCosWinX->u32Size = 64;

    s32Ret = IveMalloc(&pstCosWinY->u64PhyAddr,
        (HI_VOID **)&pstCosWinY->u64VirAddr, "CosWinY", 64);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "IveMalloc CosWinY failed!\n");
        IveFree(pstCosWinX->u64PhyAddr,
                (HI_VOID *)(HI_UINTPTR_T)pstCosWinX->u64VirAddr);
        memset_s(pstCosWinX, sizeof(IVE_MEM_INFO_S), 0, sizeof(IVE_MEM_INFO_S));
        return s32Ret;
    }
    pstCosWinY->u32Size = 64;

    IveCalcCosWindow(16, au16BufX);
    memcpy_s((HI_VOID *)(HI_UINTPTR_T)pstCosWinX->u64VirAddr, 64, au16BufX, 32);
    IveCalcCosWindow(16, au16BufY);
    memcpy_s((HI_VOID *)(HI_UINTPTR_T)pstCosWinY->u64VirAddr, 64, au16BufY, 32);

    return HI_SUCCESS;
}

HI_S32 HI_MPI_IVE_KCF_GetTrainObj(HI_U3Q5 u3q5Padding, IVE_ROI_INFO_S astRoiInfo[],
    HI_U32 u32ObjNum, IVE_MEM_INFO_S *pstCosWinX, IVE_MEM_INFO_S *pstCosWinY,
    IVE_MEM_INFO_S *pstGaussPeak, IVE_KCF_OBJ_LIST_S *pstObjList)
{
    HI_S32 s32Ret;
    HI_U32 i;
    IVE_KCF_OBJ_NODE_S *pstNode;

    s32Ret = IveCheckKcfGetTrainObjParamUser(u3q5Padding, astRoiInfo, u32ObjNum,
        pstCosWinX, pstCosWinY, pstGaussPeak, pstObjList);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check KCF GetTrainObj parameters failed!\n", s32Ret);
        return s32Ret;
    }

    for (i = 0; i < u32ObjNum; i++) {
        pstNode = IVE_ObjListGetFree(pstObjList);
        if (pstNode == NULL) {
            HI_TRACE_IVE(HI_DBG_ERR, "no free obj node!\n");
            return HI_ERR_IVE_NOMEM;
        }

        pstNode->stKcfObj.stRoiInfo = astRoiInfo[i];
        pstNode->stKcfObj.u3q5Padding = u3q5Padding;
        pstNode->stKcfObj.stCosWinX = *pstCosWinX;
        pstNode->stKcfObj.stCosWinY = *pstCosWinY;
        pstNode->stKcfObj.stGaussPeak = *pstGaussPeak;

        IVE_ObjListPutTrain(pstObjList, pstNode);
    }

    return HI_SUCCESS;
}

HI_S32 HI_MPI_IVE_KCF_Process(IVE_HANDLE *pIveHandle, IVE_SRC_IMAGE_S *pstSrc,
    IVE_KCF_OBJ_LIST_S *pstObjList, IVE_KCF_PRO_CTRL_S *pstKcfProCtrl,
    HI_BOOL bInstant)
{
    HI_S32 s32Ret;
    HI_U8 au8IoctlBuf[8];
    HI_U8 *pu8TmpBuf;
    HI_U32 u32Offset;
    IVE_KCF_OBJ_NODE_S *pstNode;

    s32Ret = IVE_IOCTL_Init();
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pIveHandle == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pIveHandle is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    s32Ret = IveCheckKcfParamUser(pstSrc, NULL, pstObjList, pstKcfProCtrl);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check KCF Process parameters failed!\n", s32Ret);
        return s32Ret;
    }

    pu8TmpBuf = pstObjList->pu8TmpBuf;
    u32Offset = 0;

    memcpy_s(&pu8TmpBuf[u32Offset], sizeof(IVE_IMAGE_S), pstSrc, sizeof(IVE_IMAGE_S));
    u32Offset += sizeof(IVE_IMAGE_S);

    *(HI_U32 *)&pu8TmpBuf[u32Offset] = pstObjList->u32TrainObjNum;
    u32Offset += 4;

    pstNode = IVE_ObjListQueryTrain(pstObjList, NULL);
    while (pstNode != NULL) {
        memcpy_s(&pu8TmpBuf[u32Offset], sizeof(IVE_KCF_OBJ_S),
            &pstNode->stKcfObj, sizeof(IVE_KCF_OBJ_S));
        u32Offset += sizeof(IVE_KCF_OBJ_S);
        pstNode = IVE_ObjListQueryTrain(pstObjList, pstNode);
    }

    *(HI_U32 *)&pu8TmpBuf[u32Offset] = pstObjList->u32TrackObjNum;
    u32Offset += 4;

    pstNode = IVE_ObjListQueryTrack(pstObjList, NULL);
    while (pstNode != NULL) {
        memcpy_s(&pu8TmpBuf[u32Offset], sizeof(IVE_KCF_OBJ_S),
            &pstNode->stKcfObj, sizeof(IVE_KCF_OBJ_S));
        u32Offset += sizeof(IVE_KCF_OBJ_S);
        pstNode = IVE_ObjListQueryTrack(pstObjList, pstNode);
    }

    memcpy_s(&pu8TmpBuf[u32Offset], sizeof(IVE_KCF_PRO_CTRL_S),
        pstKcfProCtrl, sizeof(IVE_KCF_PRO_CTRL_S));
    u32Offset += sizeof(IVE_KCF_PRO_CTRL_S);

    *(HI_BOOL *)&pu8TmpBuf[u32Offset] = bInstant;

    IveFlushCache(0, pu8TmpBuf, IVE_KCF_TMP_BUF_SIZE);

    *(IVE_HANDLE *)&au8IoctlBuf[0] = 0;
    *(HI_U32 *)&au8IoctlBuf[4] = (HI_U32)(HI_UINTPTR_T)pu8TmpBuf;

    s32Ret = ioctl(s_s32IveFd, IOC_IVE_KCF_PROCESS, au8IoctlBuf);
    *pIveHandle = *(IVE_HANDLE *)&au8IoctlBuf[0];

    /* Deserialize results back */
    u32Offset = sizeof(IVE_IMAGE_S);
    u32Offset += 4;
    pstNode = IVE_ObjListQueryTrain(pstObjList, NULL);
    while (pstNode != NULL) {
        memcpy_s(&pstNode->stKcfObj, sizeof(IVE_KCF_OBJ_S),
            &pu8TmpBuf[u32Offset], sizeof(IVE_KCF_OBJ_S));
        u32Offset += sizeof(IVE_KCF_OBJ_S);
        pstNode = IVE_ObjListQueryTrain(pstObjList, pstNode);
    }

    u32Offset += 4;
    pstNode = IVE_ObjListQueryTrack(pstObjList, NULL);
    while (pstNode != NULL) {
        memcpy_s(&pstNode->stKcfObj, sizeof(IVE_KCF_OBJ_S),
            &pu8TmpBuf[u32Offset], sizeof(IVE_KCF_OBJ_S));
        u32Offset += sizeof(IVE_KCF_OBJ_S);
        pstNode = IVE_ObjListQueryTrack(pstObjList, pstNode);
    }

    return s32Ret;
}

HI_S32 HI_MPI_IVE_KCF_GetObjBbox(IVE_KCF_OBJ_LIST_S *pstObjList,
    IVE_KCF_BBOX_S astBbox[], HI_U32 *pu32BboxObjNum,
    IVE_KCF_BBOX_CTRL_S *pstKcfBboxCtrl)
{
    HI_S32 s32Ret;
    IVE_KCF_OBJ_NODE_S *pstNode;
    HI_U32 u32Count = 0;

    s32Ret = IveCheckKcfGetObjParamUser(pstObjList, astBbox, pu32BboxObjNum, pstKcfBboxCtrl);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check KCF GetObjBbox parameters failed!\n", s32Ret);
        return s32Ret;
    }

    pstNode = IVE_ObjListQueryTrack(pstObjList, NULL);
    while (pstNode != NULL && u32Count < pstKcfBboxCtrl->u32MaxBboxNum) {
        /* Read response from the dst memory area (output of KCF process) */
        HI_S32 *ps32Dst = (HI_S32 *)(HI_UINTPTR_T)pstNode->stKcfObj.stDst.u64VirAddr;
        HI_S32 s32Response = (ps32Dst != NULL) ? ps32Dst[0] : 0;

        if (s32Response >= pstKcfBboxCtrl->s32RespThr) {
            astBbox[u32Count].pstNode = pstNode;
            astBbox[u32Count].s32Response = s32Response;
            astBbox[u32Count].stRoiInfo = pstNode->stKcfObj.stRoiInfo;
            astBbox[u32Count].bTrackOk = HI_TRUE;
            astBbox[u32Count].bRoiRefresh = HI_FALSE;
            u32Count++;
        }
        pstNode = IVE_ObjListQueryTrack(pstObjList, pstNode);
    }

    *pu32BboxObjNum = u32Count;
    return HI_SUCCESS;
}

HI_S32 HI_MPI_IVE_KCF_JudgeObjBboxTrackState(IVE_ROI_INFO_S *pstRoiInfo,
    IVE_KCF_BBOX_S *pstBbox, HI_BOOL *pbTrackOk)
{
    HI_S32 s32Ret;
    HI_S32 s32OverlapX, s32OverlapY;

    s32Ret = IveCheckKcfJudgeObjBboxParamUser(pstRoiInfo, pstBbox, pbTrackOk);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check KCF JudgeObjBbox parameters failed!\n", s32Ret);
        return s32Ret;
    }

    /* Compute overlap between detection ROI and tracked bbox ROI */
    s32OverlapX = (HI_S32)pstRoiInfo->stRoi.u32Width + (HI_S32)pstBbox->stRoiInfo.stRoi.u32Width -
                  abs((HI_S32)pstRoiInfo->stRoi.s24q8X - (HI_S32)pstBbox->stRoiInfo.stRoi.s24q8X);
    s32OverlapY = (HI_S32)pstRoiInfo->stRoi.u32Height + (HI_S32)pstBbox->stRoiInfo.stRoi.u32Height -
                  abs((HI_S32)pstRoiInfo->stRoi.s24q8Y - (HI_S32)pstBbox->stRoiInfo.stRoi.s24q8Y);

    if (s32OverlapX > 0 && s32OverlapY > 0) {
        HI_S32 s32Overlap = s32OverlapX * s32OverlapY;
        HI_S32 s32Area1 = (HI_S32)(pstRoiInfo->stRoi.u32Width * pstRoiInfo->stRoi.u32Height);
        HI_S32 s32Area2 = (HI_S32)(pstBbox->stRoiInfo.stRoi.u32Width * pstBbox->stRoiInfo.stRoi.u32Height);
        if (s32Overlap * 2 > (s32Area1 < s32Area2 ? s32Area1 : s32Area2))
            *pbTrackOk = HI_TRUE;
        else
            *pbTrackOk = HI_FALSE;
    } else {
        *pbTrackOk = HI_FALSE;
    }

    return HI_SUCCESS;
}

HI_S32 HI_MPI_IVE_KCF_ObjUpdate(IVE_KCF_OBJ_LIST_S *pstObjList,
    IVE_KCF_BBOX_S astBbox[], HI_U32 u32BboxObjNum)
{
    HI_S32 s32Ret;
    HI_U32 i;
    IVE_KCF_OBJ_NODE_S *pstNode;

    s32Ret = IveCheckKcfObjUpdateParamUser(pstObjList, astBbox, u32BboxObjNum);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "Error(%#x): check KCF ObjUpdate parameters failed!\n", s32Ret);
        return s32Ret;
    }

    /* Move all track nodes to free */
    while ((pstNode = IVE_ObjListGetTrack(pstObjList)) != NULL) {
        IVE_ObjListPutFree(pstObjList, pstNode);
    }

    /* Move train nodes to track list */
    while ((pstNode = IVE_ObjListGetTrain(pstObjList)) != NULL) {
        IVE_ObjListPutTrack(pstObjList, pstNode);
    }

    /* Update tracked bboxes */
    for (i = 0; i < u32BboxObjNum; i++) {
        if (astBbox[i].bRoiRefresh && astBbox[i].pstNode != NULL) {
            astBbox[i].pstNode->stKcfObj.stRoiInfo = astBbox[i].stRoiInfo;
        }
    }

    return HI_SUCCESS;
}

/* -------------------------------------------------------------------------- */
/*  MdProc functions (internal, not in public header)                         */
/* -------------------------------------------------------------------------- */

HI_S32 MPI_IVE_MdProcInit(HI_U64 *pstPhyAddr, HI_U32 *pu32Size)
{
    HI_S32 s32Ret;
    HI_U8 au8Buf[16];

    s32Ret = IVE_MD_Init();
    if (s32Ret != HI_SUCCESS) return s32Ret;

    if (pstPhyAddr == NULL) {
        HI_TRACE_MD(HI_DBG_ERR, "pstPhyAddr is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (pu32Size == NULL) {
        HI_TRACE_MD(HI_DBG_ERR, "pu32Size is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }

    memset_s(au8Buf, sizeof(au8Buf), 0, sizeof(au8Buf));
    s32Ret = ioctl(s_s32IveFd, IOC_IVE_MD_PROC_INIT, au8Buf);

    *pstPhyAddr = *(HI_U64 *)&au8Buf[8];
    *pu32Size   = *(HI_U32 *)&au8Buf[0];
    return s32Ret;
}

HI_S32 MPI_IVE_MdProcExit(void)
{
    HI_S32 s32Ret;

    s32Ret = IVE_MD_Init();
    if (s32Ret != HI_SUCCESS) return s32Ret;

    return ioctl(s_s32IveFd, IOC_IVE_MD_PROC_EXIT);
}

HI_S32 MPI_IVE_MdProcBeginWrite(void)
{
    HI_S32 s32Ret;

    s32Ret = IVE_MD_Init();
    if (s32Ret != HI_SUCCESS) return s32Ret;

    return ioctl(s_s32IveFd, IOC_IVE_MD_PROC_BEGIN);
}

HI_S32 MPI_IVE_MdProcEndWrite(void)
{
    HI_S32 s32Ret;

    s32Ret = IVE_MD_Init();
    if (s32Ret != HI_SUCCESS) return s32Ret;

    return ioctl(s_s32IveFd, IOC_IVE_MD_PROC_END);
}
