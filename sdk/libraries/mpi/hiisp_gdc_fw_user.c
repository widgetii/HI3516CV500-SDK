/**
 * Reverse Engineered by TekuConcept on September 20, 2020
 */

#include <stdio.h>
#include <pthread.h>

#include "re_hiisp_gdc_fw_user.h"
#include "re_mpi_gdc.h"

//gdc_fisheye_rectlinear_cfg

//gdc_fisheye_configure

//gdc_trapzoid_cfg

//GDC_LDC_CFG

/*
 * GDC_Spread_CFG — reverse-engineered from hiisp_gdc_fw_user.S
 *
 * Validates src/dst image dimensions and spread coefficient,
 * then computes fixed-point spread parameters.
 *
 * pstOutput layout (5 x HI_U32):
 *   [0] = 4 (type/mode)
 *   [1] = srcW
 *   [2] = srcH
 *   [3] = dstW
 *   [4] = dstH
 *
 * pstParams layout (80 bytes, 10 x HI_S64 at byte offsets):
 *   [ 0] = (u32) 1
 *   [ 8] = (-coef * 69632) + 0x200000
 *   [16] = -(coef * 0x8800 * dstW) / dstH
 *   [24] = coef * 0x8800 * dstW
 *   [32] = 0
 *   [40] = (-coef * 69632) + 0x200000    (same as [8])
 *   [48] = 0
 *   [56] = 0
 *   [64] = (-coef * 69632) / dstH
 *   [72] = 0x200000
 */
HI_S32
gdc_spread_configure(
    const HI_U32 *pstSrcSize,
    const HI_U32 *pstDstSize,
    HI_U32        u32SpreadCoef,
    HI_U32       *pstOutput,
    HI_U32       *pstParams)
{
    HI_U32 srcW = pstSrcSize[0];
    HI_U32 srcH = pstSrcSize[1];
    HI_U32 dstW, dstH;
    HI_S64 scaled, biased, product;
    HI_S64 quotient1, quotient2;
    HI_S64 *params = (HI_S64 *)pstParams;

    if (srcW < 640 || srcW > 8192 || srcH < 480 || srcH > 8192) {
        fprintf(stderr,
            "[Func]:%s [Line]:%d [Info]:SrcImgSize u32Width should be configured "
            "between [640,8192], SrcImgSize u32Height should be configured "
            "between [480,8192]...\n",
            "GDC_Spread_MPI_Check", 652);
        return 6;
    }

    dstW = pstDstSize[0];
    dstH = pstDstSize[1];

    if (dstW < 640 || dstW > 8192 || dstH < 480 || dstH > 8192) {
        fprintf(stderr,
            "[Func]:%s [Line]:%d [Info]:DstImgSize u32Width should be configured "
            "between [640,8192], DstImgSize u32Height should be configured "
            "between [480,8192]...\n",
            "GDC_Spread_MPI_Check", 659);
        return 6;
    }

    if (u32SpreadCoef > 18) {
        fprintf(stderr,
            "[Func]:%s [Line]:%d [Info]:The spread coeficient should between "
            "0 and 18...\n",
            "GDC_Spread_MPI_Check", 665);
        return 6;
    }

    pstOutput[0] = 4;
    pstOutput[1] = srcW;
    pstOutput[2] = srcH;
    pstOutput[3] = dstW;
    pstOutput[4] = dstH;

    /* fixed-point spread computation */
    /* scaled = -coef * 17 * 4096 = -coef * 69632 */
    scaled = -(HI_S64)u32SpreadCoef * 69632;
    biased = scaled + 0x200000;

    /* product = coef * 0x8800 * dstW */
    product = (HI_U64)u32SpreadCoef * 0x8800 * dstW;

    if (dstH == 0) dstH = 1;

    quotient1 = product / (HI_S64)dstH;
    quotient2 = scaled  / (HI_S64)dstH;

    pstParams[0] = 1;           /* byte offset 0: u32 flag */
    params[1] = biased;         /* byte offset 8 */
    params[2] = -quotient1;     /* byte offset 16 */
    params[3] = product;        /* byte offset 24 */
    params[4] = 0;              /* byte offset 32 */
    params[5] = biased;         /* byte offset 40 */
    params[6] = 0;              /* byte offset 48 */
    params[7] = 0;              /* byte offset 56 */
    params[8] = quotient2;      /* byte offset 64 */
    params[9] = 0x200000;       /* byte offset 72 */

    return 0;
}

//gdc_free_angle_rotation_cfg
