/**
 * Reverse Engineered by TekuConcept
 * MD Internal Communication / Utility Functions
 * (Identical to IVE inner_comm_user.c — vendor statically linked duplicates)
 */

#include "re_ivs_md.h"
#include <limits.h>

extern HI_S32 HI_MPI_SYS_MmzAlloc(HI_U64 *pu64PhyAddr, HI_VOID **ppVirAddr,
    const HI_CHAR *pstrMmb, const HI_CHAR *pstrZone, HI_U32 u32Len);
extern HI_S32 HI_MPI_SYS_MmzAlloc_Cached(HI_U64 *pu64PhyAddr, HI_VOID **ppVirAddr,
    const HI_CHAR *pstrMmb, const HI_CHAR *pstrZone, HI_U32 u32Len);
extern HI_S32 HI_MPI_SYS_MmzFree(HI_U64 u64PhyAddr, HI_VOID *pVirAddr);
extern HI_S32 HI_MPI_SYS_MmzFlushCache(HI_U64 u64PhyAddr, HI_VOID *pVirAddr, HI_U32 u32Size);

FILE *IveOpenFile(const HI_CHAR *pchFileName, const HI_CHAR *pchMode)
{
    char achRealPath[PATH_MAX];

    if (realpath(pchFileName, achRealPath) == NULL) {
        HI_TRACE_IVE(HI_DBG_ERR, "pchFileName(%s) is invalid!\n", pchFileName);
        return NULL;
    }

    return fopen(achRealPath, pchMode);
}

HI_VOID IveCloseFile(FILE *fp)
{
    if (fp != NULL)
        fclose(fp);
}

HI_S32 IveMalloc(HI_U64 *pu64PhyAddr, HI_VOID **ppVirAddr, const HI_CHAR *pchName, HI_U32 u32Size)
{
    return HI_MPI_SYS_MmzAlloc(pu64PhyAddr, ppVirAddr, pchName, NULL, u32Size);
}

HI_VOID IveFree(HI_U64 u64PhyAddr, HI_VOID *pVirAddr)
{
    if (pVirAddr == NULL || u64PhyAddr == 0)
        return;
    HI_MPI_SYS_MmzFree(u64PhyAddr, pVirAddr);
}

HI_S32 IveMalloc_Cached(HI_U64 *pu64PhyAddr, HI_VOID **ppVirAddr, HI_U32 u32Size)
{
    return HI_MPI_SYS_MmzAlloc_Cached(pu64PhyAddr, ppVirAddr, NULL, NULL, u32Size);
}

HI_S32 IveFlushCache(HI_U64 u64PhyAddr, HI_VOID *pVirAddr, HI_U32 u32Size)
{
    return HI_MPI_SYS_MmzFlushCache(u64PhyAddr, pVirAddr, u32Size);
}

HI_S32 IveCheckStrideUser(HI_U32 u32Stride, HI_U32 u32Width, HI_U32 u32Align)
{
    if (u32Stride < u32Width) {
        HI_TRACE_IVE(HI_DBG_ERR, "stride(%d) must be greater than or equal to width(%d)!\n",
            u32Stride, u32Width);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }

    if (u32Stride % u32Align != 0) {
        HI_TRACE_IVE(HI_DBG_ERR, "stride(%d) must be %d align!\n", u32Stride, u32Align);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }

    return HI_SUCCESS;
}

HI_S32 MdCheckStrideUser(HI_U32 u32Stride, HI_U32 u32Width, HI_U32 u32Align)
{
    if (u32Stride < u32Width) {
        HI_TRACE_MD(HI_DBG_ERR, "stride(%d) must be greater than or equal to width(%d)!\n",
            u32Stride, u32Width);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }

    if (u32Stride % u32Align != 0) {
        HI_TRACE_MD(HI_DBG_ERR, "stride(%d) must be %d align!\n", u32Stride, u32Align);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }

    return HI_SUCCESS;
}

HI_S32 IveCheckWAndHUser(HI_U32 u32Width, HI_U32 u32Height)
{
    if (u32Width & 1) {
        HI_TRACE_IVE(HI_DBG_ERR, "image width(%d) must be a multiply of 2!\n", u32Width);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }

    if (u32Height & 1) {
        HI_TRACE_IVE(HI_DBG_ERR, "image height(%d) must be a multiply of 2!\n", u32Height);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }

    return HI_SUCCESS;
}

HI_S32 MdCheckWAndHUser(HI_U32 u32Width, HI_U32 u32Height)
{
    if (u32Width & 1) {
        HI_TRACE_MD(HI_DBG_ERR, "image width(%d) must be a multiply of 2!\n", u32Width);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }

    if (u32Height & 1) {
        HI_TRACE_MD(HI_DBG_ERR, "image height(%d) must be a multiply of 2!\n", u32Height);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }

    return HI_SUCCESS;
}

HI_S32 IveCheckImageUser(IVE_IMAGE_S *pstImage, HI_U32 u32MinWidth, HI_U32 u32MaxWidth,
    HI_U32 u32MinHeight, HI_U32 u32MaxHeight, HI_U32 u32Align, HI_U8 u8PlaneCheck)
{
    HI_S32 s32Ret;

    if (pstImage->au64PhyAddr[0] == 0) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstImage->au64PhyAddr[0] can't be 0!\n");
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }

    if (pstImage->u32Width < u32MinWidth || pstImage->u32Width > u32MaxWidth) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstImage->u32Width(%d) must be in [%d, %d]!\n",
            pstImage->u32Width, u32MinWidth, u32MaxWidth);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }

    if (pstImage->u32Height < u32MinHeight || pstImage->u32Height > u32MaxHeight) {
        HI_TRACE_IVE(HI_DBG_ERR, "pstImage->u32Height(%d) must be in [%d ,%d]!\n",
            pstImage->u32Height, u32MinHeight, u32MaxHeight);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }

    s32Ret = IveCheckWAndHUser(pstImage->u32Width, pstImage->u32Height);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "check image width and height failed!\n");
        return s32Ret;
    }

    s32Ret = IveCheckStrideUser(pstImage->au32Stride[0], pstImage->u32Width, u32Align);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_IVE(HI_DBG_ERR, "check pstImage->au32Stride[0] failed!\n");
        return s32Ret;
    }

    if (u8PlaneCheck == 2) {
        if (pstImage->au64PhyAddr[1] == 0) {
            HI_TRACE_IVE(HI_DBG_ERR, "pstImage->pu8PhyAddr[1] can't be 0!\n");
            return HI_ERR_IVE_ILLEGAL_PARAM;
        }
        s32Ret = IveCheckStrideUser(pstImage->au32Stride[1], pstImage->u32Width, u32Align);
        if (s32Ret != HI_SUCCESS) {
            HI_TRACE_IVE(HI_DBG_ERR, "check pstImage->au32Stride[1] failed!\n");
            return s32Ret;
        }
    }

    if (u8PlaneCheck == 3) {
        if (pstImage->au64PhyAddr[1] == 0) {
            HI_TRACE_IVE(HI_DBG_ERR, "pstImage->pu8PhyAddr[1] can't be 0!\n");
            return HI_ERR_IVE_ILLEGAL_PARAM;
        }
        s32Ret = IveCheckStrideUser(pstImage->au32Stride[1], pstImage->u32Width, u32Align);
        if (s32Ret != HI_SUCCESS) {
            HI_TRACE_IVE(HI_DBG_ERR, "check pstImage->au32Stride[1] failed!\n");
            return s32Ret;
        }

        if (pstImage->au64PhyAddr[2] == 0) {
            HI_TRACE_IVE(HI_DBG_ERR, "pstImage->pu8PhyAddr[2] can't be 0!\n");
            return HI_ERR_IVE_ILLEGAL_PARAM;
        }
        s32Ret = IveCheckStrideUser(pstImage->au32Stride[2], pstImage->u32Width, u32Align);
        if (s32Ret != HI_SUCCESS) {
            HI_TRACE_IVE(HI_DBG_ERR, "check pstImage->au32Stride[2] failed!\n");
            return s32Ret;
        }
    }

    return HI_SUCCESS;
}

HI_S32 MdCheckImageUser(IVE_IMAGE_S *pstImage, HI_U32 u32MinWidth, HI_U32 u32MaxWidth,
    HI_U32 u32MinHeight, HI_U32 u32MaxHeight, HI_U32 u32Align, HI_U8 u8PlaneCheck)
{
    HI_S32 s32Ret;

    if (pstImage->au64PhyAddr[0] == 0) {
        HI_TRACE_MD(HI_DBG_ERR, "pstImage->au64PhyAddr[0] can't be 0!\n");
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }

    if (pstImage->au64PhyAddr[0] & 0xF) {
        HI_TRACE_MD(HI_DBG_ERR,
            "pstImage->au64PhyAddr[0](0x%llx) must be %d byte align!\n",
            (unsigned long long)pstImage->au64PhyAddr[0], 16);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }

    if (pstImage->u32Width < u32MinWidth || pstImage->u32Width > u32MaxWidth) {
        HI_TRACE_MD(HI_DBG_ERR, "pstImage->u32Width(%d) must be in [%d, %d]!\n",
            pstImage->u32Width, u32MinWidth, u32MaxWidth);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }

    if (pstImage->u32Height < u32MinHeight || pstImage->u32Height > u32MaxHeight) {
        HI_TRACE_MD(HI_DBG_ERR, "pstImage->u32Height(%d) must be in [%d ,%d]!\n",
            pstImage->u32Height, u32MinHeight, u32MaxHeight);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }

    s32Ret = MdCheckWAndHUser(pstImage->u32Width, pstImage->u32Height);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_MD(HI_DBG_ERR, "check image width and height failed!\n");
        return s32Ret;
    }

    s32Ret = MdCheckStrideUser(pstImage->au32Stride[0], pstImage->u32Width, u32Align);
    if (s32Ret != HI_SUCCESS) {
        HI_TRACE_MD(HI_DBG_ERR, "check pstImage->au32Stride[0] failed!\n");
        return s32Ret;
    }

    if (u8PlaneCheck == 2) {
        if (pstImage->au64PhyAddr[1] == 0) {
            HI_TRACE_MD(HI_DBG_ERR, "pstImage->pu8PhyAddr[1] can't be 0!\n");
            return HI_ERR_IVE_ILLEGAL_PARAM;
        }
        s32Ret = MdCheckStrideUser(pstImage->au32Stride[1], pstImage->u32Width, u32Align);
        if (s32Ret != HI_SUCCESS) {
            HI_TRACE_MD(HI_DBG_ERR, "check pstImage->au32Stride[1] failed!\n");
            return s32Ret;
        }
        if (pstImage->au64PhyAddr[1] & 0xF) {
            HI_TRACE_MD(HI_DBG_ERR,
                "pstImage->au64PhyAddr[1](0x%llx) must be %d byte align!\n",
                (unsigned long long)pstImage->au64PhyAddr[1], 16);
            return HI_ERR_IVE_ILLEGAL_PARAM;
        }
    }

    if (u8PlaneCheck == 3) {
        if (pstImage->au64PhyAddr[1] == 0) {
            HI_TRACE_MD(HI_DBG_ERR, "pstImage->pu8PhyAddr[1] can't be 0!\n");
            return HI_ERR_IVE_ILLEGAL_PARAM;
        }
        s32Ret = MdCheckStrideUser(pstImage->au32Stride[1], pstImage->u32Width, u32Align);
        if (s32Ret != HI_SUCCESS) {
            HI_TRACE_MD(HI_DBG_ERR, "check pstImage->au32Stride[1] failed!\n");
            return s32Ret;
        }

        if (pstImage->au64PhyAddr[2] == 0) {
            HI_TRACE_MD(HI_DBG_ERR, "pstImage->pu8PhyAddr[2] can't be 0!\n");
            return HI_ERR_IVE_ILLEGAL_PARAM;
        }
        s32Ret = MdCheckStrideUser(pstImage->au32Stride[2], pstImage->u32Width, u32Align);
        if (s32Ret != HI_SUCCESS) {
            HI_TRACE_MD(HI_DBG_ERR, "check pstImage->au32Stride[2] failed!\n");
            return s32Ret;
        }
        if (pstImage->au64PhyAddr[2] & 0xF) {
            HI_TRACE_MD(HI_DBG_ERR,
                "pstImage->au64PhyAddr[2](0x%llx) must be %d byte align!\n",
                (unsigned long long)pstImage->au64PhyAddr[2], 16);
            return HI_ERR_IVE_ILLEGAL_PARAM;
        }
    }

    return HI_SUCCESS;
}

HI_S32 IveCheckResRelationUser(IVE_IMAGE_S *pstSrc, IVE_IMAGE_S *pstDst)
{
    if (pstSrc->u32Width != pstDst->u32Width) {
        HI_TRACE_IVE(HI_DBG_ERR, "src width(%d) must equal dst width(%d)!\n",
            pstSrc->u32Width, pstDst->u32Width);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }

    if (pstSrc->u32Height != pstDst->u32Height) {
        HI_TRACE_IVE(HI_DBG_ERR, "src height(%d) must equal dst height(%d)!\n",
            pstSrc->u32Height, pstDst->u32Height);
        return HI_ERR_IVE_ILLEGAL_PARAM;
    }

    return HI_SUCCESS;
}
