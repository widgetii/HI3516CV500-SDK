/**
 * Reverse Engineered by TekuConcept
 * MD Parameter Validation
 */

#include "re_ivs_md.h"

HI_S32 MD_CheckAttr(MD_ATTR_S *pstMdAttr)
{
    HI_U32 u32BlockSize;

    /* enAlgMode must be in [0, 2) */
    if (pstMdAttr->enAlgMode > 1) {
        HI_TRACE_MD(HI_DBG_ERR, "pMdAttr->enAlgMode(%d) must be in [%d,%d)!\n",
            pstMdAttr->enAlgMode, 0, 2);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }

    /* u32Width must be in [64, 1920] */
    if (pstMdAttr->u32Width < 64 || pstMdAttr->u32Width > 1920) {
        HI_TRACE_MD(HI_DBG_ERR, "pMdAttr->u32Width(%d) must be in [%d, %d]!\n",
            pstMdAttr->u32Width, 64, 1920);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }

    /* u32Height must be in [64, 1080] */
    if (pstMdAttr->u32Height < 64 || pstMdAttr->u32Height > 1080) {
        HI_TRACE_MD(HI_DBG_ERR, "pMdAttr->u32Height(%d) must be in [%d, %d]!\n",
            pstMdAttr->u32Height, 64, 1080);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }

    /* enSadMode must be in [0, 3) */
    if (pstMdAttr->enSadMode > 2) {
        HI_TRACE_MD(HI_DBG_ERR, "pMdAttr->enSadMode(%d) must be in [%d,%d)!\n",
            pstMdAttr->enSadMode, 0, 3);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }

    /* enSadOutCtrl must be one of {4, 1} — CTRL_16BIT_BOTH(0), CTRL_8BIT_BOTH(1), CTRL_THRESH(4) */
    if (pstMdAttr->enSadOutCtrl > 4 ||
        (pstMdAttr->enSadOutCtrl != 4 && pstMdAttr->enSadOutCtrl > 1)) {
        HI_TRACE_MD(HI_DBG_ERR,
            "pMdAttr->enSadOutCtrl(%d) must be in {CTRL_16BIT_BOTH,CTRL_8BIT_BOTH,CTRL_THRESH}!\n",
            pstMdAttr->enSadOutCtrl);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }

    /* Width must be a multiple of blockSize = 4 << enSadMode */
    u32BlockSize = 4 << pstMdAttr->enSadMode;
    if (pstMdAttr->u32Width % u32BlockSize != 0) {
        HI_TRACE_MD(HI_DBG_ERR, "pMdAttr->u32Width(%d) must be a multiply of %d!\n",
            pstMdAttr->u32Width, u32BlockSize);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }

    /* Height must be a multiple of blockSize */
    if (pstMdAttr->u32Height % u32BlockSize != 0) {
        HI_TRACE_MD(HI_DBG_ERR, "pMdAttr->u32Height(%d) must be a multiply of %d!\n",
            pstMdAttr->u32Height, u32BlockSize);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }

    /* If enSadOutCtrl == CTRL_8BIT_BOTH (1), u16SadThr must be <= 255 */
    if (pstMdAttr->enSadOutCtrl == 1 && pstMdAttr->u16SadThr > 255) {
        HI_TRACE_MD(HI_DBG_ERR,
            "pMdAttr->u16SadThr(%d) must be in [%d, %d] in pMdAttr->enSadOutCtrl(%d)!\n",
            pstMdAttr->u16SadThr, 0, 255, pstMdAttr->enSadOutCtrl);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }

    /* stCclCtrl.enMode must be in [0, 2) */
    if (pstMdAttr->stCclCtrl.enMode > 1) {
        HI_TRACE_MD(HI_DBG_ERR, "pMdAttr->stCclCtrl.enMode(%d) must be in [%d,%d)!\n",
            pstMdAttr->stCclCtrl.enMode, 0, 2);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }

    /* stCclCtrl.u16Step can't be 0 */
    if (pstMdAttr->stCclCtrl.u16Step == 0) {
        HI_TRACE_MD(HI_DBG_ERR, "pMdAttr->stCclCtrl.u16Step can't be 0!\n");
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }

    /* stAddCtrl.u0q16X + u0q16Y must equal 65536 */
    if ((HI_U32)pstMdAttr->stAddCtrl.u0q16X + (HI_U32)pstMdAttr->stAddCtrl.u0q16Y != 65536) {
        HI_TRACE_MD(HI_DBG_ERR,
            "pMdAttr->stAddCtrl.u0q16X(%d) + pMdAttr->stAddCtrl.u0q16Y(%d) must be equal to %d!\n",
            pstMdAttr->stAddCtrl.u0q16X, pstMdAttr->stAddCtrl.u0q16Y, 65536);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }

    /* u0q16X can't be 0 */
    if (pstMdAttr->stAddCtrl.u0q16X == 0) {
        HI_TRACE_MD(HI_DBG_ERR, "pMdAttr->stAddCtrl.u0q16X can't be 0!\n");
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }

    /* u0q16Y can't be 0 */
    if (pstMdAttr->stAddCtrl.u0q16Y == 0) {
        HI_TRACE_MD(HI_DBG_ERR, "pMdAttr->stAddCtrl.u0q16Y can't be 0!\n");
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }

    return HI_SUCCESS;
}
