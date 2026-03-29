/**
 * Reverse Engineered by TekuConcept
 * IVE Parameter Validation Functions
 */

#include "re_mpi_ive.h"

/* Helper: Check ROI is valid (used by multiple validators)
 * From assembly: IveCheckRoi.isra.0 */
static HI_S32 IveCheckRoi(IVE_IMAGE_S *pstImage, IVE_RECT_U16_S *pstRoi)
{
    if (pstRoi->u16X + pstRoi->u16Width > pstImage->u32Width) {
        HI_TRACE_IVE(HI_DBG_ERR, "roi x(%d) + width(%d) must be <= image width(%d)!\n",
            pstRoi->u16X, pstRoi->u16Width, pstImage->u32Width);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    if (pstRoi->u16Y + pstRoi->u16Height > pstImage->u32Height) {
        HI_TRACE_IVE(HI_DBG_ERR, "roi y(%d) + height(%d) must be <= image height(%d)!\n",
            pstRoi->u16Y, pstRoi->u16Height, pstImage->u32Height);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    return HI_SUCCESS;
}

/* ========================================================================== */
/* DMA parameter check */
/* ========================================================================== */
HI_S32 IveCheckDMAParamUser(IVE_DATA_S *pstSrc, IVE_DATA_S *pstDst, IVE_DMA_CTRL_S *pstCtrl)
{
    if (pstSrc == NULL || pstDst == NULL || pstCtrl == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "NULL pointer!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (pstSrc->u64PhyAddr == 0) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstSrc->u64PhyAddr can't be 0!\n");
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    if (pstDst->u64PhyAddr == 0) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstDst->u64PhyAddr can't be 0!\n");
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    if (pstCtrl->enMode >= IVE_DMA_MODE_BUTT) {
        HI_TRACE_IVE(HI_DBG_ERR, "invalid DMA mode(%d)!\n", pstCtrl->enMode);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    return HI_SUCCESS;
}

/* ========================================================================== */
/* Generic two-image + one-image check pattern (And/Or/Xor share this) */
/* ========================================================================== */
static HI_S32 IveCheckTwoSrcOneDstParamUser(IVE_SRC_IMAGE_S *pstSrc1,
    IVE_SRC_IMAGE_S *pstSrc2, IVE_DST_IMAGE_S *pstDst)
{
    HI_S32 s32Ret;

    if (pstSrc1 == NULL || pstSrc2 == NULL || pstDst == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "NULL pointer!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    s32Ret = IveCheckImageUser(pstSrc1, 64, 1920, 64, 1080, 16, 0);
    if (s32Ret != HI_SUCCESS)
        return s32Ret;
    s32Ret = IveCheckImageUser(pstSrc2, 64, 1920, 64, 1080, 16, 0);
    if (s32Ret != HI_SUCCESS)
        return s32Ret;
    s32Ret = IveCheckImageUser(pstDst, 64, 1920, 64, 1080, 16, 0);
    if (s32Ret != HI_SUCCESS)
        return s32Ret;
    s32Ret = IveCheckResRelationUser(pstSrc1, pstSrc2);
    if (s32Ret != HI_SUCCESS)
        return s32Ret;
    s32Ret = IveCheckResRelationUser(pstSrc1, pstDst);
    return s32Ret;
}

HI_S32 IveCheckAndParamUser(IVE_SRC_IMAGE_S *pstSrc1, IVE_SRC_IMAGE_S *pstSrc2, IVE_DST_IMAGE_S *pstDst)
{
    return IveCheckTwoSrcOneDstParamUser(pstSrc1, pstSrc2, pstDst);
}

HI_S32 IveCheckOrParamUser(IVE_SRC_IMAGE_S *pstSrc1, IVE_SRC_IMAGE_S *pstSrc2, IVE_DST_IMAGE_S *pstDst)
{
    return IveCheckTwoSrcOneDstParamUser(pstSrc1, pstSrc2, pstDst);
}

HI_S32 IveCheckXorParamUser(IVE_SRC_IMAGE_S *pstSrc1, IVE_SRC_IMAGE_S *pstSrc2, IVE_DST_IMAGE_S *pstDst)
{
    return IveCheckTwoSrcOneDstParamUser(pstSrc1, pstSrc2, pstDst);
}

/* ========================================================================== */
/* Generic src+dst image check pattern */
/* ========================================================================== */
static HI_S32 IveCheckSrcDstParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDst)
{
    HI_S32 s32Ret;

    if (pstSrc == NULL || pstDst == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "NULL pointer!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    s32Ret = IveCheckImageUser(pstSrc, 64, 1920, 64, 1080, 16, 0);
    if (s32Ret != HI_SUCCESS)
        return s32Ret;
    s32Ret = IveCheckImageUser(pstDst, 64, 1920, 64, 1080, 16, 0);
    if (s32Ret != HI_SUCCESS)
        return s32Ret;
    return IveCheckResRelationUser(pstSrc, pstDst);
}

HI_S32 IveCheckFilterParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDst, IVE_FILTER_CTRL_S *pstCtrl)
{
    HI_S32 s32Ret;
    if (pstCtrl == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstCtrl is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    s32Ret = IveCheckSrcDstParamUser(pstSrc, pstDst);
    return s32Ret;
}

HI_S32 IveCheckCSCParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDst, IVE_CSC_CTRL_S *pstCtrl)
{
    if (pstCtrl == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstCtrl is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (pstCtrl->enMode >= IVE_CSC_MODE_BUTT) {
        HI_TRACE_IVE(HI_DBG_ERR, "invalid CSC mode(%d)!\n", pstCtrl->enMode);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    return IveCheckSrcDstParamUser(pstSrc, pstDst);
}

HI_S32 IveCheckFilterAndCSCParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDst, IVE_FILTER_AND_CSC_CTRL_S *pstCtrl)
{
    if (pstCtrl == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstCtrl is NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (pstCtrl->enMode >= IVE_CSC_MODE_BUTT) {
        HI_TRACE_IVE(HI_DBG_ERR, "invalid CSC mode(%d)!\n", pstCtrl->enMode);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    return IveCheckSrcDstParamUser(pstSrc, pstDst);
}

HI_S32 IveCheckSobelParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDstH, IVE_DST_IMAGE_S *pstDstV, IVE_SOBEL_CTRL_S *pstCtrl)
{
    HI_S32 s32Ret;
    if (pstSrc == NULL || pstCtrl == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "NULL pointer!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (pstCtrl->enOutCtrl >= IVE_SOBEL_OUT_CTRL_BUTT) {
        HI_TRACE_IVE(HI_DBG_ERR, "invalid Sobel output control(%d)!\n", pstCtrl->enOutCtrl);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    s32Ret = IveCheckImageUser(pstSrc, 64, 1920, 64, 1080, 16, 0);
    if (s32Ret != HI_SUCCESS)
        return s32Ret;
    if (pstCtrl->enOutCtrl == IVE_SOBEL_OUT_CTRL_BOTH || pstCtrl->enOutCtrl == IVE_SOBEL_OUT_CTRL_HOR) {
        if (pstDstH == NULL) return HI_ERR_IVE_NULL_PTR;
        s32Ret = IveCheckImageUser(pstDstH, 64, 1920, 64, 1080, 16, 0);
        if (s32Ret != HI_SUCCESS) return s32Ret;
    }
    if (pstCtrl->enOutCtrl == IVE_SOBEL_OUT_CTRL_BOTH || pstCtrl->enOutCtrl == IVE_SOBEL_OUT_CTRL_VER) {
        if (pstDstV == NULL) return HI_ERR_IVE_NULL_PTR;
        s32Ret = IveCheckImageUser(pstDstV, 64, 1920, 64, 1080, 16, 0);
        if (s32Ret != HI_SUCCESS) return s32Ret;
    }
    return HI_SUCCESS;
}

HI_S32 IveCheckMagAndAngParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDstMag, IVE_DST_IMAGE_S *pstDstAng, IVE_MAG_AND_ANG_CTRL_S *pstCtrl)
{
    HI_S32 s32Ret;
    if (pstSrc == NULL || pstDstMag == NULL || pstCtrl == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "NULL pointer!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    s32Ret = IveCheckImageUser(pstSrc, 64, 1920, 64, 1080, 16, 0);
    if (s32Ret != HI_SUCCESS) return s32Ret;
    s32Ret = IveCheckImageUser(pstDstMag, 64, 1920, 64, 1080, 16, 0);
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstCtrl->enOutCtrl == IVE_MAG_AND_ANG_OUT_CTRL_MAG_AND_ANG) {
        if (pstDstAng == NULL) return HI_ERR_IVE_NULL_PTR;
        s32Ret = IveCheckImageUser(pstDstAng, 64, 1920, 64, 1080, 16, 0);
        if (s32Ret != HI_SUCCESS) return s32Ret;
    }
    return HI_SUCCESS;
}

HI_S32 IveCheckDilateParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDst, IVE_DILATE_CTRL_S *pstCtrl)
{
    if (pstCtrl == NULL) { HI_TRACE_IVE(HI_DBG_ERR, "pstCtrl is NULL!\n"); return HI_ERR_IVE_NULL_PTR; }
    return IveCheckSrcDstParamUser(pstSrc, pstDst);
}

HI_S32 IveCheckErodeParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDst, IVE_ERODE_CTRL_S *pstCtrl)
{
    if (pstCtrl == NULL) { HI_TRACE_IVE(HI_DBG_ERR, "pstCtrl is NULL!\n"); return HI_ERR_IVE_NULL_PTR; }
    return IveCheckSrcDstParamUser(pstSrc, pstDst);
}

HI_S32 IveCheckThreshParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDst, IVE_THRESH_CTRL_S *pstCtrl)
{
    if (pstCtrl == NULL) { HI_TRACE_IVE(HI_DBG_ERR, "pstCtrl is NULL!\n"); return HI_ERR_IVE_NULL_PTR; }
    if (pstCtrl->enMode >= IVE_THRESH_MODE_BUTT) {
        HI_TRACE_IVE(HI_DBG_ERR, "invalid Thresh mode(%d)!\n", pstCtrl->enMode);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    return IveCheckSrcDstParamUser(pstSrc, pstDst);
}

HI_S32 IveCheckSubParamUser(IVE_SRC_IMAGE_S *pstSrc1, IVE_SRC_IMAGE_S *pstSrc2, IVE_DST_IMAGE_S *pstDst, IVE_SUB_CTRL_S *pstCtrl)
{
    if (pstCtrl == NULL) { HI_TRACE_IVE(HI_DBG_ERR, "pstCtrl is NULL!\n"); return HI_ERR_IVE_NULL_PTR; }
    if (pstCtrl->enMode >= IVE_SUB_MODE_BUTT) {
        HI_TRACE_IVE(HI_DBG_ERR, "invalid Sub mode(%d)!\n", pstCtrl->enMode);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    return IveCheckTwoSrcOneDstParamUser(pstSrc1, pstSrc2, pstDst);
}

HI_S32 IveCheckIntegParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDst, IVE_INTEG_CTRL_S *pstCtrl)
{
    if (pstCtrl == NULL) { HI_TRACE_IVE(HI_DBG_ERR, "pstCtrl is NULL!\n"); return HI_ERR_IVE_NULL_PTR; }
    if (pstCtrl->enOutCtrl >= IVE_INTEG_OUT_CTRL_BUTT) {
        HI_TRACE_IVE(HI_DBG_ERR, "invalid Integ output control(%d)!\n", pstCtrl->enOutCtrl);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    return IveCheckSrcDstParamUser(pstSrc, pstDst);
}

HI_S32 IveCheckHistParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_DST_MEM_INFO_S *pstDst)
{
    HI_S32 s32Ret;
    if (pstSrc == NULL || pstDst == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "NULL pointer!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    s32Ret = IveCheckImageUser(pstSrc, 64, 1920, 64, 1080, 16, 0);
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstDst->u64PhyAddr == 0) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstDst->u64PhyAddr can't be 0!\n");
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    return HI_SUCCESS;
}

HI_S32 IveCheckThresh_S16ParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDst, IVE_THRESH_S16_CTRL_S *pstCtrl)
{
    if (pstCtrl == NULL) { HI_TRACE_IVE(HI_DBG_ERR, "pstCtrl is NULL!\n"); return HI_ERR_IVE_NULL_PTR; }
    if (pstCtrl->enMode >= IVE_THRESH_S16_MODE_BUTT) {
        HI_TRACE_IVE(HI_DBG_ERR, "invalid ThreshS16 mode(%d)!\n", pstCtrl->enMode);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    return IveCheckSrcDstParamUser(pstSrc, pstDst);
}

HI_S32 IveCheckThresh_U16ParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDst, IVE_THRESH_U16_CTRL_S *pstCtrl)
{
    if (pstCtrl == NULL) { HI_TRACE_IVE(HI_DBG_ERR, "pstCtrl is NULL!\n"); return HI_ERR_IVE_NULL_PTR; }
    if (pstCtrl->enMode >= IVE_THRESH_U16_MODE_BUTT) {
        HI_TRACE_IVE(HI_DBG_ERR, "invalid ThreshU16 mode(%d)!\n", pstCtrl->enMode);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    return IveCheckSrcDstParamUser(pstSrc, pstDst);
}

HI_S32 IveCheck16BitTo8BitParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDst, IVE_16BIT_TO_8BIT_CTRL_S *pstCtrl)
{
    if (pstCtrl == NULL) { HI_TRACE_IVE(HI_DBG_ERR, "pstCtrl is NULL!\n"); return HI_ERR_IVE_NULL_PTR; }
    if (pstCtrl->enMode >= IVE_16BIT_TO_8BIT_MODE_BUTT) {
        HI_TRACE_IVE(HI_DBG_ERR, "invalid 16BitTo8Bit mode(%d)!\n", pstCtrl->enMode);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    return IveCheckSrcDstParamUser(pstSrc, pstDst);
}

HI_S32 IveCheckOrdStatFilterParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDst, IVE_ORD_STAT_FILTER_CTRL_S *pstCtrl)
{
    if (pstCtrl == NULL) { HI_TRACE_IVE(HI_DBG_ERR, "pstCtrl is NULL!\n"); return HI_ERR_IVE_NULL_PTR; }
    if (pstCtrl->enMode >= IVE_ORD_STAT_FILTER_MODE_BUTT) {
        HI_TRACE_IVE(HI_DBG_ERR, "invalid OrdStatFilter mode(%d)!\n", pstCtrl->enMode);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    return IveCheckSrcDstParamUser(pstSrc, pstDst);
}

HI_S32 IveCheckEqualizeHistParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDst, IVE_EQUALIZE_HIST_CTRL_S *pstCtrl)
{
    if (pstCtrl == NULL) { HI_TRACE_IVE(HI_DBG_ERR, "pstCtrl is NULL!\n"); return HI_ERR_IVE_NULL_PTR; }
    return IveCheckSrcDstParamUser(pstSrc, pstDst);
}

HI_S32 IveCheckAddParamUser(IVE_SRC_IMAGE_S *pstSrc1, IVE_SRC_IMAGE_S *pstSrc2, IVE_DST_IMAGE_S *pstDst, IVE_ADD_CTRL_S *pstCtrl)
{
    if (pstCtrl == NULL) { HI_TRACE_IVE(HI_DBG_ERR, "pstCtrl is NULL!\n"); return HI_ERR_IVE_NULL_PTR; }
    return IveCheckTwoSrcOneDstParamUser(pstSrc1, pstSrc2, pstDst);
}

HI_S32 IveCheckNCCParamUser(IVE_SRC_IMAGE_S *pstSrc1, IVE_SRC_IMAGE_S *pstSrc2, IVE_DST_MEM_INFO_S *pstDst)
{
    HI_S32 s32Ret;
    if (pstSrc1 == NULL || pstSrc2 == NULL || pstDst == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "NULL pointer!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    s32Ret = IveCheckImageUser(pstSrc1, 64, 1920, 64, 1080, 16, 0);
    if (s32Ret != HI_SUCCESS) return s32Ret;
    s32Ret = IveCheckImageUser(pstSrc2, 64, 1920, 64, 1080, 16, 0);
    if (s32Ret != HI_SUCCESS) return s32Ret;
    s32Ret = IveCheckResRelationUser(pstSrc1, pstSrc2);
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstDst->u64PhyAddr == 0) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstDst->u64PhyAddr can't be 0!\n");
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    return HI_SUCCESS;
}

HI_S32 IveCheckGMMParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstFg, IVE_DST_IMAGE_S *pstBg, IVE_MEM_INFO_S *pstModel, IVE_GMM_CTRL_S *pstCtrl)
{
    HI_S32 s32Ret;
    if (pstSrc == NULL || pstFg == NULL || pstBg == NULL || pstModel == NULL || pstCtrl == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "NULL pointer!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    s32Ret = IveCheckImageUser(pstSrc, 64, 1920, 64, 1080, 16, 0);
    if (s32Ret != HI_SUCCESS) return s32Ret;
    s32Ret = IveCheckImageUser(pstFg, 64, 1920, 64, 1080, 16, 0);
    if (s32Ret != HI_SUCCESS) return s32Ret;
    s32Ret = IveCheckImageUser(pstBg, 64, 1920, 64, 1080, 16, 0);
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstModel->u64PhyAddr == 0) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstModel->u64PhyAddr can't be 0!\n");
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    return HI_SUCCESS;
}

HI_S32 IveCheckCannyHysEdgeParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstEdge, IVE_DST_MEM_INFO_S *pstStack, IVE_CANNY_HYS_EDGE_CTRL_S *pstCtrl)
{
    HI_S32 s32Ret;
    if (pstSrc == NULL || pstEdge == NULL || pstStack == NULL || pstCtrl == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "NULL pointer!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    s32Ret = IveCheckImageUser(pstSrc, 64, 1920, 64, 1080, 16, 0);
    if (s32Ret != HI_SUCCESS) return s32Ret;
    s32Ret = IveCheckImageUser(pstEdge, 64, 1920, 64, 1080, 16, 0);
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstStack->u64PhyAddr == 0) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstStack->u64PhyAddr can't be 0!\n");
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    return HI_SUCCESS;
}

HI_S32 IveCheckCannyEdgeParamUser(IVE_DST_IMAGE_S *pstEdge, IVE_MEM_INFO_S *pstStack)
{
    HI_S32 s32Ret;
    if (pstEdge == NULL || pstStack == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "NULL pointer!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    s32Ret = IveCheckImageUser(pstEdge, 64, 1920, 64, 1080, 16, 0);
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstStack->u64PhyAddr == 0 || pstStack->u64VirAddr == 0) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstStack address can't be 0!\n");
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    return HI_SUCCESS;
}

HI_S32 IveCheckLBPParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDst, IVE_LBP_CTRL_S *pstCtrl)
{
    if (pstCtrl == NULL) { HI_TRACE_IVE(HI_DBG_ERR, "pstCtrl is NULL!\n"); return HI_ERR_IVE_NULL_PTR; }
    if (pstCtrl->enMode >= IVE_LBP_CMP_MODE_BUTT) {
        HI_TRACE_IVE(HI_DBG_ERR, "invalid LBP mode(%d)!\n", pstCtrl->enMode);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    return IveCheckSrcDstParamUser(pstSrc, pstDst);
}

HI_S32 IveCheckNormGradParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDstH, IVE_DST_IMAGE_S *pstDstV, IVE_DST_IMAGE_S *pstDstHV, IVE_NORM_GRAD_CTRL_S *pstCtrl)
{
    HI_S32 s32Ret;
    if (pstSrc == NULL || pstCtrl == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "NULL pointer!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (pstCtrl->enOutCtrl >= IVE_NORM_GRAD_OUT_CTRL_BUTT) {
        HI_TRACE_IVE(HI_DBG_ERR, "invalid NormGrad output control(%d)!\n", pstCtrl->enOutCtrl);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    s32Ret = IveCheckImageUser(pstSrc, 64, 1920, 64, 1080, 16, 0);
    if (s32Ret != HI_SUCCESS) return s32Ret;
    return HI_SUCCESS;
}

HI_S32 IveCheckSTCandiCornerParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDst, IVE_ST_CANDI_CORNER_CTRL_S *pstCtrl)
{
    if (pstCtrl == NULL) { HI_TRACE_IVE(HI_DBG_ERR, "pstCtrl is NULL!\n"); return HI_ERR_IVE_NULL_PTR; }
    return IveCheckSrcDstParamUser(pstSrc, pstDst);
}

HI_S32 IveCheckGradFgParamUser(IVE_SRC_IMAGE_S *pstBgDiffFg, IVE_SRC_IMAGE_S *pstCurGrad, IVE_SRC_IMAGE_S *pstBgGrad, IVE_DST_IMAGE_S *pstGradFg, IVE_GRAD_FG_CTRL_S *pstCtrl)
{
    HI_S32 s32Ret;
    if (pstBgDiffFg == NULL || pstCurGrad == NULL || pstBgGrad == NULL || pstGradFg == NULL || pstCtrl == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "NULL pointer!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    s32Ret = IveCheckImageUser(pstBgDiffFg, 64, 1920, 64, 1080, 16, 0);
    if (s32Ret != HI_SUCCESS) return s32Ret;
    s32Ret = IveCheckImageUser(pstCurGrad, 64, 1920, 64, 1080, 16, 0);
    if (s32Ret != HI_SUCCESS) return s32Ret;
    s32Ret = IveCheckImageUser(pstBgGrad, 64, 1920, 64, 1080, 16, 0);
    if (s32Ret != HI_SUCCESS) return s32Ret;
    s32Ret = IveCheckImageUser(pstGradFg, 64, 1920, 64, 1080, 16, 0);
    if (s32Ret != HI_SUCCESS) return s32Ret;
    return HI_SUCCESS;
}

HI_S32 IveCheckMatchBgModelParamUser(IVE_SRC_IMAGE_S *pstCurImg, IVE_DATA_S *pstBgModel, IVE_IMAGE_S *pstFgFlag, IVE_DST_IMAGE_S *pstDiffFg, IVE_DST_MEM_INFO_S *pstStatData, IVE_MATCH_BG_MODEL_CTRL_S *pstCtrl)
{
    HI_S32 s32Ret;
    if (pstCurImg == NULL || pstBgModel == NULL || pstFgFlag == NULL || pstDiffFg == NULL || pstStatData == NULL || pstCtrl == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "NULL pointer!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    s32Ret = IveCheckImageUser(pstCurImg, 64, 1920, 64, 1080, 16, 0);
    if (s32Ret != HI_SUCCESS) return s32Ret;
    s32Ret = IveCheckImageUser(pstFgFlag, 64, 1920, 64, 1080, 16, 0);
    if (s32Ret != HI_SUCCESS) return s32Ret;
    s32Ret = IveCheckImageUser(pstDiffFg, 64, 1920, 64, 1080, 16, 0);
    if (s32Ret != HI_SUCCESS) return s32Ret;
    return HI_SUCCESS;
}

HI_S32 IveCheckUpdateBgModelParamUser(IVE_DATA_S *pstBgModel, IVE_IMAGE_S *pstFgFlag, IVE_DST_IMAGE_S *pstBgImg, IVE_DST_IMAGE_S *pstChgSta, IVE_DST_MEM_INFO_S *pstStatData, IVE_UPDATE_BG_MODEL_CTRL_S *pstCtrl)
{
    HI_S32 s32Ret;
    if (pstBgModel == NULL || pstFgFlag == NULL || pstBgImg == NULL || pstChgSta == NULL || pstStatData == NULL || pstCtrl == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "NULL pointer!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    s32Ret = IveCheckImageUser(pstFgFlag, 64, 1920, 64, 1080, 16, 0);
    if (s32Ret != HI_SUCCESS) return s32Ret;
    s32Ret = IveCheckImageUser(pstBgImg, 64, 1920, 64, 1080, 16, 0);
    if (s32Ret != HI_SUCCESS) return s32Ret;
    s32Ret = IveCheckImageUser(pstChgSta, 64, 1920, 64, 1080, 16, 0);
    if (s32Ret != HI_SUCCESS) return s32Ret;
    return HI_SUCCESS;
}

HI_S32 IveCheckSADParamUser(IVE_SRC_IMAGE_S *pstSrc1, IVE_SRC_IMAGE_S *pstSrc2, IVE_DST_IMAGE_S *pstSad, IVE_DST_IMAGE_S *pstThr, IVE_SAD_CTRL_S *pstCtrl)
{
    HI_S32 s32Ret;
    if (pstSrc1 == NULL || pstSrc2 == NULL || pstSad == NULL || pstCtrl == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "NULL pointer!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (pstCtrl->enMode >= IVE_SAD_MODE_BUTT) {
        HI_TRACE_IVE(HI_DBG_ERR, "invalid SAD mode(%d)!\n", pstCtrl->enMode);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    s32Ret = IveCheckImageUser(pstSrc1, 64, 1920, 64, 1080, 16, 0);
    if (s32Ret != HI_SUCCESS) return s32Ret;
    s32Ret = IveCheckImageUser(pstSrc2, 64, 1920, 64, 1080, 16, 0);
    if (s32Ret != HI_SUCCESS) return s32Ret;
    s32Ret = IveCheckResRelationUser(pstSrc1, pstSrc2);
    if (s32Ret != HI_SUCCESS) return s32Ret;
    return HI_SUCCESS;
}

HI_S32 IveCheckCCLParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_DST_MEM_INFO_S *pstDst, IVE_CCL_CTRL_S *pstCtrl)
{
    HI_S32 s32Ret;
    if (pstSrc == NULL || pstDst == NULL || pstCtrl == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "NULL pointer!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    s32Ret = IveCheckImageUser(pstSrc, 64, 1920, 64, 1080, 16, 0);
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstDst->u64PhyAddr == 0) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstDst->u64PhyAddr can't be 0!\n");
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    return HI_SUCCESS;
}

HI_S32 IveCheckGMM2ParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_SRC_IMAGE_S *pstFactor, IVE_DST_IMAGE_S *pstFg, IVE_DST_IMAGE_S *pstBg, IVE_DST_IMAGE_S *pstMatchModelInfo, IVE_MEM_INFO_S *pstModel, IVE_GMM2_CTRL_S *pstCtrl)
{
    HI_S32 s32Ret;
    if (pstSrc == NULL || pstFg == NULL || pstBg == NULL || pstModel == NULL || pstCtrl == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "NULL pointer!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    s32Ret = IveCheckImageUser(pstSrc, 64, 1920, 64, 1080, 16, 0);
    if (s32Ret != HI_SUCCESS) return s32Ret;
    s32Ret = IveCheckImageUser(pstFg, 64, 1920, 64, 1080, 16, 0);
    if (s32Ret != HI_SUCCESS) return s32Ret;
    s32Ret = IveCheckImageUser(pstBg, 64, 1920, 64, 1080, 16, 0);
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstModel->u64PhyAddr == 0) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstModel->u64PhyAddr can't be 0!\n");
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    return HI_SUCCESS;
}

HI_S32 IveCheckLKOpticalFlowPyrParamUser(IVE_SRC_IMAGE_S astSrcPrevPyr[], IVE_SRC_IMAGE_S astSrcNextPyr[], IVE_SRC_MEM_INFO_S *pstPrevPts, IVE_MEM_INFO_S *pstNextPts, IVE_DST_MEM_INFO_S *pstStatus, IVE_DST_MEM_INFO_S *pstErr, IVE_LK_OPTICAL_FLOW_PYR_CTRL_S *pstCtrl)
{
    HI_S32 s32Ret;
    if (astSrcPrevPyr == NULL || astSrcNextPyr == NULL || pstPrevPts == NULL || pstNextPts == NULL || pstStatus == NULL || pstCtrl == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "NULL pointer!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    s32Ret = IveCheckImageUser(&astSrcPrevPyr[0], 64, 1920, 64, 1080, 16, 0);
    if (s32Ret != HI_SUCCESS) return s32Ret;
    s32Ret = IveCheckImageUser(&astSrcNextPyr[0], 64, 1920, 64, 1080, 16, 0);
    if (s32Ret != HI_SUCCESS) return s32Ret;
    return HI_SUCCESS;
}

HI_S32 IveCheckMapParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_SRC_MEM_INFO_S *pstMap, IVE_DST_IMAGE_S *pstDst, IVE_MAP_CTRL_S *pstCtrl)
{
    HI_S32 s32Ret;
    if (pstSrc == NULL || pstMap == NULL || pstDst == NULL || pstCtrl == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "NULL pointer!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (pstCtrl->enMode >= IVE_MAP_MODE_BUTT) {
        HI_TRACE_IVE(HI_DBG_ERR, "invalid Map mode(%d)!\n", pstCtrl->enMode);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    s32Ret = IveCheckImageUser(pstSrc, 64, 1920, 64, 1080, 16, 0);
    if (s32Ret != HI_SUCCESS) return s32Ret;
    s32Ret = IveCheckImageUser(pstDst, 64, 1920, 64, 1080, 16, 0);
    if (s32Ret != HI_SUCCESS) return s32Ret;
    if (pstMap->u64PhyAddr == 0) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstMap->u64PhyAddr can't be 0!\n");
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    return HI_SUCCESS;
}

HI_S32 IveCheckResizeParamUser(IVE_SRC_IMAGE_S astSrc[], IVE_DST_IMAGE_S astDst[], IVE_RESIZE_CTRL_S *pstCtrl)
{
    HI_S32 s32Ret;
    HI_U32 i;
    if (astSrc == NULL || astDst == NULL || pstCtrl == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "NULL pointer!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (pstCtrl->enMode >= IVE_RESIZE_MODE_BUTT) {
        HI_TRACE_IVE(HI_DBG_ERR, "invalid Resize mode(%d)!\n", pstCtrl->enMode);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    for (i = 0; i < pstCtrl->u16Num; i++) {
        s32Ret = IveCheckImageUser(&astSrc[i], 32, 1920, 32, 1080, 16, 0);
        if (s32Ret != HI_SUCCESS) return s32Ret;
        s32Ret = IveCheckImageUser(&astDst[i], 32, 1920, 32, 1080, 16, 0);
        if (s32Ret != HI_SUCCESS) return s32Ret;
    }
    return HI_SUCCESS;
}

HI_S32 IveCheckCNNPredictParamUser(IVE_SRC_IMAGE_S astSrc[], IVE_SRC_MEM_INFO_S *pstModel, IVE_DST_BLOB_S astDst[], IVE_CNN_CTRL_S *pstCtrl)
{
    if (astSrc == NULL || pstModel == NULL || astDst == NULL || pstCtrl == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "NULL pointer!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (pstModel->u64PhyAddr == 0) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstModel->u64PhyAddr can't be 0!\n");
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    return HI_SUCCESS;
}

/* ========================================================================== */
/* ANN MLP Predict parameter check - 5 null checks only */
/* ========================================================================== */
HI_S32 IveCheckANNMLPPredictParamUser(IVE_SRC_DATA_S *pstSrc, IVE_LOOK_UP_TABLE_S *pstActivFuncTab,
    IVE_ANN_MLP_MODEL_S *pstAnnMlpModel, IVE_DST_DATA_S *pstDst, IVE_SRC_MEM_INFO_S *pstModel)
{
    if (pstSrc == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstSrc can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (pstActivFuncTab == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstActivFuncTab can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (pstAnnMlpModel == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstAnnMlpModel can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (pstDst == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstDst can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (pstModel == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstModel can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    return HI_SUCCESS;
}

/* ========================================================================== */
/* SVM Predict parameter check - 5 null checks only */
/* ========================================================================== */
HI_S32 IveCheckSVMPredictParamUser(IVE_SRC_DATA_S *pstSrc, IVE_LOOK_UP_TABLE_S *pstKernelTab,
    IVE_SVM_MODEL_S *pstSvmModel, IVE_DST_DATA_S *pstDstVote, IVE_SRC_MEM_INFO_S *pstModel)
{
    if (pstSrc == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstSrc can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (pstKernelTab == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstKernelTab can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (pstSvmModel == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstSvmModel can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (pstDstVote == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstDstVote can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (pstModel == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstModel can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    return HI_SUCCESS;
}

/* ========================================================================== */
/* Perspective Transform parameter check */
/* ========================================================================== */
HI_S32 IveCheckPerspTransParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_RECT_U32_S astRoi[],
    IVE_SRC_MEM_INFO_S astPointPair[], IVE_DST_IMAGE_S astDst[], IVE_MEM_INFO_S *pstMem,
    IVE_PERSP_TRANS_CTRL_S *pstPerspTransCtrl)
{
    if (pstSrc == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstSrc can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (astRoi == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "astRoi can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (astPointPair == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "astPointPair can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (astDst == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "astDst can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (pstMem == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstMem can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (pstPerspTransCtrl == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstPerspTransCtrl can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    /* u16RoiNum must be in [1, 64] */
    if (pstPerspTransCtrl->u16RoiNum < 1 || pstPerspTransCtrl->u16RoiNum > 64) {
        HI_TRACE_IVE(HI_DBG_ERR, "u16RoiNum(%d) must be in [1, 64]!\n",
            pstPerspTransCtrl->u16RoiNum);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    /* u16PointPairNum must be in [2, 68] */
    if (pstPerspTransCtrl->u16PointPairNum < 2 || pstPerspTransCtrl->u16PointPairNum > 68) {
        HI_TRACE_IVE(HI_DBG_ERR, "u16PointPairNum(%d) must be in [2, 68]!\n",
            pstPerspTransCtrl->u16PointPairNum);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    return HI_SUCCESS;
}

/* ========================================================================== */
/* ST Corner parameter check */
/* ========================================================================== */
HI_S32 IveCheckSTCornerParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_DST_IMAGE_S *pstDst,
    IVE_POINT_U16_S *pstCorner, IVE_ST_CANDI_CORNER_CTRL_S *pstCtrl)
{
    HI_S32 s32Ret;
    if (pstSrc == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstSrc can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (pstDst == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstDst can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (pstCorner == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstCorner can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    /* Width must be in [1, 501] (SUB-1, CMP 500, BCS pattern) */
    if (pstCorner->u16X < 1 || pstCorner->u16X > 501) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstCorner width(%d) must be in [1, 501]!\n", pstCorner->u16X);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    /* Height must be in [1, 255] */
    if (pstCorner->u16Y < 1 || pstCorner->u16Y > 255) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstCorner height(%d) must be in [1, 255]!\n", pstCorner->u16Y);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    s32Ret = IveCheckImageUser(pstSrc, 64, 1280, 64, 720, 16, 1);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "IveCheckImageUser for pstSrc failed!\n");
        return s32Ret;
    }
    if (pstSrc->enType != 0) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstSrc type(%d) must be U8C1!\n", pstSrc->enType);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    if (pstDst->au64PhyAddr[0] == 0) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstDst->au64PhyAddr[0] can't be 0!\n");
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    if (pstDst->au64VirAddr[0] == 0) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstDst->au64VirAddr[0] can't be 0!\n");
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    if (pstDst->au32Stride[0] > 2001) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstDst stride(%d) exceeds max 2002!\n", pstDst->au32Stride[0]);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    return HI_SUCCESS;
}

/* ========================================================================== */
/* KCF GaussPeak parameter check */
/* ========================================================================== */
HI_S32 IveCheckKcfGaussPeakParamUser(HI_U3Q5 u3q5Padding, IVE_DST_MEM_INFO_S *pstGaussPeak)
{
    /* u3q5Padding must be in [48, 160] (SUB 48, CMP 112 pattern) */
    if ((HI_U32)u3q5Padding - 48 > 112) {
        HI_TRACE_IVE(HI_DBG_ERR, "u3q5Padding(%d) must be in [48, 160]!\n", u3q5Padding);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    if (pstGaussPeak == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstGaussPeak can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (pstGaussPeak->u64PhyAddr == 0) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstGaussPeak->u64PhyAddr can't be 0!\n");
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    if (pstGaussPeak->u64VirAddr == 0) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstGaussPeak->u64VirAddr can't be 0!\n");
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    /* u32Size must be > 0x6f3ff (455679), i.e. >= 455680 = 0x6f400 */
    if (pstGaussPeak->u32Size <= 0x6f3ff) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstGaussPeak->u32Size(%d) too small, need >= %d!\n",
            pstGaussPeak->u32Size, 0x6f400);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    return HI_SUCCESS;
}

/* ========================================================================== */
/* KCF CosWin parameter check */
/* ========================================================================== */
HI_S32 IveCheckKcfCosWinParamUser(IVE_DST_MEM_INFO_S *pstCosWinX, IVE_DST_MEM_INFO_S *pstCosWinY)
{
    if (pstCosWinX == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstCosWinX can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (pstCosWinY == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstCosWinY can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    /* CosWinX: phyAddr, virAddr, u32Size >= 832 */
    if (pstCosWinX->u64PhyAddr == 0) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstCosWinX->u64PhyAddr can't be 0!\n");
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    if (pstCosWinX->u64VirAddr == 0) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstCosWinX->u64VirAddr can't be 0!\n");
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    if (pstCosWinX->u32Size < 832) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstCosWinX->u32Size(%d) must be >= 832!\n",
            pstCosWinX->u32Size);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    /* CosWinY: phyAddr, virAddr, u32Size >= 832 */
    if (pstCosWinY->u64PhyAddr == 0) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstCosWinY->u64PhyAddr can't be 0!\n");
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    if (pstCosWinY->u64VirAddr == 0) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstCosWinY->u64VirAddr can't be 0!\n");
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    if (pstCosWinY->u32Size < 832) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstCosWinY->u32Size(%d) must be >= 832!\n",
            pstCosWinY->u32Size);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    return HI_SUCCESS;
}

/* ========================================================================== */
/* KCF ObjList parameter check */
/* ========================================================================== */
HI_S32 IveCheckKcfObjListParamUser(IVE_MEM_INFO_S *pstMem, HI_U32 u32MaxObjNum,
    IVE_KCF_OBJ_LIST_S *pstObjList)
{
    if (pstMem == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstMem can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (pstObjList == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstObjList can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    /* u32MaxObjNum must be in [1, 64] */
    if (u32MaxObjNum < 1 || u32MaxObjNum > 64) {
        HI_TRACE_IVE(HI_DBG_ERR, "u32MaxObjNum(%d) must be in [1, 64]!\n", u32MaxObjNum);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    /* Memory size must be >= u32MaxObjNum * 55824 (0xDA10) */
    if ((HI_U32)(55824 * u32MaxObjNum) > pstMem->u32Size) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstMem->u32Size(%d) too small, need >= %d!\n",
            pstMem->u32Size, 55824 * u32MaxObjNum);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    if (pstMem->u64PhyAddr == 0) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstMem->u64PhyAddr can't be 0!\n");
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    if (pstMem->u64VirAddr == 0) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstMem->u64VirAddr can't be 0!\n");
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    return HI_SUCCESS;
}

/* ========================================================================== */
/* KCF GetTrainObj parameter check */
/* ========================================================================== */
HI_S32 IveCheckKcfGetTrainObjParamUser(HI_U3Q5 u3q5Padding, IVE_ROI_INFO_S astRoiInfo[],
    HI_U32 u32ObjNum, IVE_MEM_INFO_S *pstCosWinX, IVE_MEM_INFO_S *pstCosWinY,
    IVE_MEM_INFO_S *pstGaussPeak, IVE_KCF_OBJ_LIST_S *pstObjList)
{
    HI_S32 s32Ret;
    HI_U32 i;
    if (astRoiInfo == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "astRoiInfo can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (pstCosWinX == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstCosWinX can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (pstCosWinY == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstCosWinY can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (pstGaussPeak == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstGaussPeak can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (pstObjList == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstObjList can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    /* enListState must be IVE_KCF_LIST_STATE_CREATE (1) */
    if (pstObjList->enListState != IVE_KCF_LIST_STATE_CREATE) {
        HI_TRACE_IVE(HI_DBG_ERR, "enListState(%d) must be CREATE(%d)!\n",
            pstObjList->enListState, IVE_KCF_LIST_STATE_CREATE);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    /* u32FreeObjNum must be in [1, 64] */
    if (pstObjList->u32FreeObjNum < 1 || pstObjList->u32FreeObjNum > 64) {
        HI_TRACE_IVE(HI_DBG_ERR, "u32FreeObjNum(%d) must be in [1, 64]!\n",
            pstObjList->u32FreeObjNum);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    /* u32FreeObjNum must be >= u32ObjNum */
    if (pstObjList->u32FreeObjNum < u32ObjNum) {
        HI_TRACE_IVE(HI_DBG_ERR, "u32FreeObjNum(%d) must be >= u32ObjNum(%d)!\n",
            pstObjList->u32FreeObjNum, u32ObjNum);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    /* u3q5Padding must be in [48, 160] */
    if ((HI_U32)u3q5Padding - 48 > 112) {
        HI_TRACE_IVE(HI_DBG_ERR, "u3q5Padding(%d) must be in [48, 160]!\n", u3q5Padding);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    /* GaussPeak memory checks */
    if (pstGaussPeak->u64PhyAddr == 0) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstGaussPeak->u64PhyAddr can't be 0!\n");
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    if (pstGaussPeak->u64VirAddr == 0) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstGaussPeak->u64VirAddr can't be 0!\n");
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    if (pstGaussPeak->u32Size <= 0x6f3ff) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstGaussPeak->u32Size(%d) too small!\n",
            pstGaussPeak->u32Size);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    /* CosWinX memory checks */
    if (pstCosWinX->u64PhyAddr == 0) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstCosWinX->u64PhyAddr can't be 0!\n");
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    if (pstCosWinX->u64VirAddr == 0) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstCosWinX->u64VirAddr can't be 0!\n");
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    if (pstCosWinX->u32Size < 832) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstCosWinX->u32Size(%d) must be >= 832!\n",
            pstCosWinX->u32Size);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    /* CosWinY memory checks */
    if (pstCosWinY->u64PhyAddr == 0) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstCosWinY->u64PhyAddr can't be 0!\n");
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    if (pstCosWinY->u64VirAddr == 0) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstCosWinY->u64VirAddr can't be 0!\n");
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    if (pstCosWinY->u32Size < 832) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstCosWinY->u32Size(%d) must be >= 832!\n",
            pstCosWinY->u32Size);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    /* Validate ROIs */
    for (i = 0; i < u32ObjNum; i++) {
        s32Ret = IveCheckRoi((IVE_IMAGE_S *)&astRoiInfo[i], (IVE_RECT_U16_S *)&astRoiInfo[i].stRoi);
        if (s32Ret != HI_SUCCESS) {
            HI_TRACE_IVE(HI_DBG_ERR, "ROI[%d] check failed(0x%x)!\n", i, s32Ret);
            return s32Ret;
        }
    }
    return HI_SUCCESS;
}

/* ========================================================================== */
/* Get HOG feature rectangle dimensions (pure computation, no validation) */
/* ========================================================================== */
HI_VOID IveGetHogFeatureRect(IVE_ROI_INFO_S *pstRoiInfo, HI_U3Q5 u3q5Padding,
    HI_U32 *pu32Height, HI_U32 *pu32Width)
{
    HI_U32 u32Width, u32Height;
    HI_U32 u32RoiWidth = ((HI_U32 *)pstRoiInfo)[3];  /* offset 12 */
    HI_U32 u32RoiHeight = ((HI_U32 *)pstRoiInfo)[2]; /* offset 8 */

    u32Width = (u32RoiWidth * (HI_U32)u3q5Padding) >> 8;
    u32Height = (u32RoiHeight * (HI_U32)u3q5Padding) >> 8;
    u32Width = (u32Width + 1) << 3;
    u32Height = (u32Height + 1) << 3;
    if (u32Width > 136) u32Width = 136;
    if (u32Height > 136) u32Height = 136;
    u32Width = (u32Width >> 2) - 2;
    u32Height = (u32Height >> 2) - 2;

    *pu32Height = u32Height;
    *pu32Width = u32Width;
}

/* ========================================================================== */
/* KCF Process parameter check */
/* ========================================================================== */
HI_S32 IveCheckKcfParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_SRC_IMAGE_S *pstBg,
    IVE_KCF_OBJ_LIST_S *pstObjList, IVE_KCF_PRO_CTRL_S *pstKcfProCtrl)
{
    HI_U32 u32MaxObjNum;
    if (pstSrc == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstSrc can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (pstBg == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstBg can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (pstObjList == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstObjList can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (pstKcfProCtrl == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstKcfProCtrl can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    /* pu8TmpBuf must not be NULL */
    if (pstObjList->pu8TmpBuf == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstObjList->pu8TmpBuf can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    /* enListState must be IVE_KCF_LIST_STATE_CREATE (1) */
    if (pstObjList->enListState != IVE_KCF_LIST_STATE_CREATE) {
        HI_TRACE_IVE(HI_DBG_ERR, "enListState(%d) must be CREATE!\n",
            pstObjList->enListState);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    /* u32MaxObjNum clamp to 64, must be >= u32TrainObjNum and u32TrackObjNum */
    u32MaxObjNum = pstObjList->u32MaxObjNum;
    if (u32MaxObjNum > 64) u32MaxObjNum = 64;
    if (u32MaxObjNum < pstObjList->u32TrainObjNum) {
        HI_TRACE_IVE(HI_DBG_ERR, "u32TrainObjNum(%d) exceeds maxObj(%d)!\n",
            pstObjList->u32TrainObjNum, u32MaxObjNum);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    if (u32MaxObjNum < pstObjList->u32TrackObjNum) {
        HI_TRACE_IVE(HI_DBG_ERR, "u32TrackObjNum(%d) exceeds maxObj(%d)!\n",
            pstObjList->u32TrackObjNum, u32MaxObjNum);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    return HI_SUCCESS;
}

/* ========================================================================== */
/* KCF GetObj parameter check */
/* ========================================================================== */
HI_S32 IveCheckKcfGetObjParamUser(IVE_KCF_OBJ_LIST_S *pstObjList, IVE_KCF_BBOX_S astBbox[],
    HI_U32 *pu32BboxObjNum, IVE_KCF_BBOX_CTRL_S *pstKcfBboxCtrl)
{
    if (pstObjList == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstObjList can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (astBbox == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "astBbox can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (pu32BboxObjNum == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pu32BboxObjNum can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (pstKcfBboxCtrl == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstKcfBboxCtrl can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    return HI_SUCCESS;
}

/* ========================================================================== */
/* KCF Judge ObjBbox parameter check */
/* ========================================================================== */
HI_S32 IveCheckKcfJudgeObjBboxParamUser(IVE_ROI_INFO_S *pstRoiInfo, IVE_KCF_BBOX_S *pstBbox,
    HI_BOOL *pbTrackOk)
{
    if (pstRoiInfo == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstRoiInfo can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (pstBbox == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstBbox can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (pbTrackOk == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pbTrackOk can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    return HI_SUCCESS;
}

/* ========================================================================== */
/* KCF ObjUpdate parameter check */
/* ========================================================================== */
HI_S32 IveCheckKcfObjUpdateParamUser(IVE_KCF_OBJ_LIST_S *pstObjList, IVE_KCF_BBOX_S astBbox[],
    HI_U32 u32BboxObjNum)
{
    if (pstObjList == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstObjList can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (astBbox == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "astBbox can't be NULL!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    /* enListState must be IVE_KCF_LIST_STATE_CREATE */
    if (pstObjList->enListState != IVE_KCF_LIST_STATE_CREATE) {
        HI_TRACE_IVE(HI_DBG_ERR, "enListState(%d) must be CREATE!\n",
            pstObjList->enListState);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    if (u32BboxObjNum > pstObjList->u32TrackObjNum) {
        HI_TRACE_IVE(HI_DBG_ERR, "u32BboxObjNum(%d) exceeds u32TrackObjNum(%d)!\n",
            u32BboxObjNum, pstObjList->u32TrackObjNum);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    return HI_SUCCESS;
}

/* ========================================================================== */
/* HOG parameter check */
/* ========================================================================== */
HI_S32 IveCheckHogParamUser(IVE_SRC_IMAGE_S *pstSrc, IVE_RECT_U32_S astRoi[],
    IVE_DST_BLOB_S astDst[], IVE_HOG_CTRL_S *pstHogCtrl)
{
    if (pstSrc == NULL || astRoi == NULL || astDst == NULL || pstHogCtrl == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "NULL pointer!\n");
        return HI_ERR_IVE_NULL_PTR;
    }
    if (pstHogCtrl->u32RoiNum < 1 || pstHogCtrl->u32RoiNum > 64) {
        HI_TRACE_IVE(HI_DBG_ERR, "u32RoiNum(%d) must be in [1, 64]!\n",
            pstHogCtrl->u32RoiNum);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }
    return HI_SUCCESS;
}
