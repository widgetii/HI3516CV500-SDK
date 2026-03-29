/**
 * Reverse Engineered by TekuConcept on September 20, 2020
 */

#include <stdio.h>
#include <math.h>
#include <pthread.h>

#include "re_hiisp_gdc_fw_user.h"
#include "re_mpi_gdc.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* FOV radius LUT for LDC distortion ratios 1..17 (from RODATA) */
static const HI_U32 s_ldc_fov_lut[17] = {
    30867, 21845, 17852, 15474, 13853, 12657, 11728, 10980,
    10361,  9838,  9388,  8996,  8651,  8343,  8067,  7818,
     7591
};


/*
 * GDC_Fisheye_CFG — reverse-engineered from hiisp_gdc_fw_user.S
 *
 * Validates fisheye correction parameters and computes the transform
 * configuration for a single fisheye region.
 *
 * Only FISHEYE_VIEW_NORMAL + FISHEYE_WALL_MOUNT is supported.
 * bLMF (Lens Mapping Function from user) is not supported.
 */
HI_S32
GDC_Fisheye_CFG(
    const GDC_DATA_S *pstData,
    const HI_VOID *pstGdcFisheyeCfg,
    HI_U32 *pstOutput1,
    HI_U32 *pstFwFisheyeParams)
{
    const HI_U32 *cfg = (const HI_U32 *)pstGdcFisheyeCfg;
    HI_U32 srcW, srcH, outW, outH;
    HI_U32 u32OutRadius, u32InRadius;
    HI_S32 s32HorOffset, s32VerOffset;
    HI_U32 u32Pan, u32Tilt, u32HorZoom, u32VerZoom, u32TrapezoidCoef;
    HI_U32 bEnable, bLMF, enViewMode, enMountMode;
    HI_U32 maxDim;
    HI_S32 horCenter, verCenter;
    HI_S32 tilt_scaled, pan_scaled;
    HI_U32 u32Pix2MaxAngle;
    double sin_tilt, cos_tilt, sin_pan, cos_pan;
    double tan_result, outR_16, scale;
    HI_S32 s32StartX;

    /*
     * The fisheye config struct layout (from assembly field access):
     * +0x00: bEnable
     * +0x04: bLMF
     * +0x08: (reserved)
     * +0x0C: enMountMode
     * +0x10: enViewMode
     * +0x14: u32InRadius
     * +0x18: u32OutRadius
     * +0x1C: s32HorOffset
     * +0x20: s32VerOffset
     * +0x24: u32Pan
     * +0x28: u32Tilt
     * +0x2C: u32HorZoom
     * +0x30: u32VerZoom
     * +0x34: (reserved - outRect.x)
     * +0x38: stOutSize.u32Width
     * +0x3C: stOutSize.u32Height
     * +0x40: s32FanStrength
     * +0x44: u32TrapezoidCoef
     */
    bEnable         = cfg[0];
    bLMF            = cfg[1];
    enMountMode     = cfg[3];
    enViewMode      = cfg[4];
    u32InRadius     = cfg[5];
    u32OutRadius    = cfg[6];
    s32HorOffset    = (HI_S32)cfg[7];
    s32VerOffset    = (HI_S32)cfg[8];
    u32Pan          = cfg[9];
    u32Tilt         = cfg[10];
    u32HorZoom      = cfg[11];
    u32VerZoom      = cfg[12];
    outW            = cfg[14];
    outH            = cfg[15];
    u32TrapezoidCoef = cfg[17];

    if (enViewMode != 2 /* FISHEYE_VIEW_NORMAL */) {
        fprintf(stderr,
            "[Func]:%s [Line]:%d [Info]:Wrong View Mode Configuration "
            "for Fisheye Correction...\n", "GDC_Fisheye_CFG", 66);
        return 1;
    }

    if (enMountMode != 2 /* FISHEYE_WALL_MOUNT */) {
        fprintf(stderr,
            "[Func]:%s [Line]:%d [Info]:Mode is not wall mount...\n",
            "GDC_Fisheye_CFG", 73);
        return 1;
    }

    if (bLMF == 1) {
        fprintf(stderr,
            "[Func]:%s [Line]:%d [Info]:Lmf is not support...\n",
            "GDC_Fisheye_CFG", 80);
        return 1;
    }

    srcW = pstData->stSize.u32Width;
    srcH = pstData->stSize.u32Height;

    maxDim = (srcH >= srcW) ? srcH : srcW;
    if (u32OutRadius == 0 || u32OutRadius > maxDim) {
        fprintf(stderr,
            "[Func]:%s [Line]:%d [Info]:The OutRadius cannot exceed "
            "both the image width and height...\n", "GDC_Fisheye_CFG", 88);
        return 1;
    }

    if (u32OutRadius <= u32InRadius) {
        fprintf(stderr,
            "[Func]:%s [Line]:%d [Info]:The InRadius cannot exceed "
            "or equal to the OutRadius...\n", "GDC_Fisheye_CFG", 95);
        return 1;
    }

    if ((s32HorOffset < 0 ? -s32HorOffset : s32HorOffset) >= 512 ||
        (s32VerOffset < 0 ? -s32VerOffset : s32VerOffset) >= 512) {
        fprintf(stderr,
            "[Func]:%s [Line]:%d [Info]:The center offset should be "
            "limited between %d and %d...\n", "GDC_Fisheye_CFG", 103, -511, 511);
        return 1;
    }

    if (u32Pan > 360) {
        fprintf(stderr,
            "[Func]:%s [Line]:%d [Info]:The value of u32Pan should be "
            "limited between 0 and 360...\n", "GDC_Fisheye_CFG", 110);
        return 1;
    }

    if (u32Tilt > 360) {
        fprintf(stderr,
            "[Func]:%s [Line]:%d [Info]:The value of u32Tilt should be "
            "limited between 0 and 360...\n", "GDC_Fisheye_CFG", 117);
        return 1;
    }

    if (u32HorZoom - 1 > 4094 || u32VerZoom - 1 > 4094) {
        fprintf(stderr,
            "[Func]:%s [Line]:%d [Info]:The value of u32HorZoom and "
            "u3VerZoom cannot exceed %d...\n", "GDC_Fisheye_CFG", 127, 4095);
        return 1;
    }

    if (u32TrapezoidCoef > 32) {
        fprintf(stderr,
            "[Func]:%s [Line]:%d [Info]:The value of u32TrapezoidCoef "
            "cannot exceed %d...\n", "GDC_Fisheye_CFG", 134, 32);
        return 1;
    }

    if (outH < 360 || outH > 8192 || outW < 640 || outW > 8192) {
        fprintf(stderr,
            "[Func]:%s [Line]:%d [Info]:Wrong Configuration of the output "
            "image size...\n The width should between 640 and 8192 \n "
            "The height should between 360 and 8192\n", "GDC_Fisheye_CFG", 144);
        return 1;
    }

    horCenter = s32HorOffset + (HI_S32)(srcW >> 1);
    verCenter = s32VerOffset + (HI_S32)(srcH >> 1);

    if (horCenter < 0 || horCenter > (HI_S32)(srcW - 1) ||
        verCenter < 0 || verCenter > (HI_S32)(srcH - 1)) {
        fprintf(stderr,
            "[Func]:%s [Line]:%d [Info]:Wrong configuration of offset\n",
            "GDC_Fisheye_CFG", 277);
        return -1;
    }

    /* Fill output region descriptor */
    pstOutput1[0]  = 2;
    pstOutput1[1]  = srcW;
    pstOutput1[2]  = srcH;
    pstOutput1[3]  = outH;
    pstOutput1[4]  = outW;
    pstOutput1[5]  = outW;
    pstOutput1[6]  = outH;
    pstOutput1[7]  = horCenter;
    pstOutput1[8]  = verCenter;
    pstOutput1[9]  = outW >> 1;
    pstOutput1[10] = outH >> 1;

    /* Angle scaling: tilt and pan from degrees to internal representation */
    tilt_scaled = (HI_S32)((HI_S64)u32Tilt * 262142 / 360);
    tilt_scaled = (tilt_scaled - 262143 + ((HI_U32)(tilt_scaled - 262143) >> 31)) >> 1;

    pan_scaled = (HI_S32)((HI_S64)u32Pan * 262142 / 360);
    pan_scaled = (pan_scaled + 1 + ((HI_U32)(pan_scaled + 1) >> 31)) >> 1;

    u32Pix2MaxAngle = (HI_U32)((double)(u32HorZoom << 6) * 0.03125);

    /* Fill transform params header */
    pstFwFisheyeParams[0]  = 2;
    pstFwFisheyeParams[1]  = u32OutRadius;
    pstFwFisheyeParams[2]  = pan_scaled;
    pstFwFisheyeParams[3]  = tilt_scaled;
    pstFwFisheyeParams[4]  = 1024;
    pstFwFisheyeParams[5]  = 1024;
    pstFwFisheyeParams[6]  = bLMF;
    pstFwFisheyeParams[79] = 0;
    pstFwFisheyeParams[82] = 0;
    pstFwFisheyeParams[89] = u32Pix2MaxAngle;
    pstFwFisheyeParams[90] = u32VerZoom << 6;

    /* Compute trig for rotation matrix */
    {
        double tilt_rad = (double)tilt_scaled * (M_PI / 32768.0) * 0.125;
        double pan_rad  = (double)pan_scaled  * (M_PI / 32768.0) * 0.125;

        sin_tilt = sin(tilt_rad);
        cos_tilt = cos(tilt_rad);
        sin_pan  = sin(pan_rad);
        cos_pan  = cos(pan_rad);

        tan_result = tan((double)u32Pix2MaxAngle * (M_PI / 32768.0) * 2.0);
    }

    maxDim = (outH >= outW) ? outH : outW;

    outR_16 = (double)u32OutRadius * 16.0;
    s32StartX = (HI_S32)(2.0 * (double)u32OutRadius * 8.0 * tan_result) / (HI_S32)maxDim;
    scale = (double)s32StartX;

    /* Rotation matrix elements */
    pstFwFisheyeParams[80] = (HI_U32)(HI_S32)( cos_pan * scale);
    pstFwFisheyeParams[81] = (HI_U32)(HI_S32)(-sin_pan * scale);
    pstFwFisheyeParams[83] = (HI_U32)(HI_S32)( sin_pan * sin_tilt * scale);
    pstFwFisheyeParams[84] = (HI_U32)(HI_S32)( cos_pan * sin_tilt * scale);
    pstFwFisheyeParams[85] = (HI_U32)(HI_S32)(-cos_tilt * scale);
    pstFwFisheyeParams[86] = (HI_U32)(HI_S32)( sin_pan * cos_tilt * outR_16);
    pstFwFisheyeParams[87] = (HI_U32)(HI_S32)( cos_pan * cos_tilt * outR_16);
    pstFwFisheyeParams[88] = (HI_U32)(HI_S32)( sin_tilt * outR_16);

    return 0;
}


/*
 * GDC_Trapzoid_CFG — reverse-engineered from hiisp_gdc_fw_user.S
 *
 * Computes trapezoid (keystone) correction as a 3x3 projective mapping
 * matrix in Q22 fixed-point (0x400000 = 1.0).
 */
HI_S32
GDC_Trapzoid_CFG(
    const SIZE_S *pstSize,
    HI_U32 u32TrapezoidCoef,
    HI_S64 *pstParams)
{
    HI_U32 width, height, halfH;
    HI_U64 trap_base, trap_double;

    if (u32TrapezoidCoef == 0)
        return 0;

    if (u32TrapezoidCoef > 32) {
        fprintf(stderr,
            "[Func]:%s [Line]:%d [Info]:The trapzoid coeficient should "
            "between 0 and 32...\n", "GDC_Trapzoid_MPI_Check", 310);
        return 1;
    }

    width  = pstSize->u32Width;
    height = pstSize->u32Height;
    halfH  = height >> 1;

    trap_base   = (HI_U64)u32TrapezoidCoef * 26112;
    trap_double = (HI_U64)u32TrapezoidCoef * 52224;

    *(HI_U32 *)pstParams = 1;

    pstParams[1] = 0x400000;
    pstParams[2] = (HI_S64)(trap_base * width + halfH) / height;
    pstParams[3] = 0;
    pstParams[4] = 0;
    pstParams[5] = (HI_S64)(trap_double + 0x400000);
    pstParams[6] = 0;
    pstParams[7] = 0;
    pstParams[8] = (HI_S64)(trap_double + halfH) / height;
    pstParams[9] = 0x400000;

    return 0;
}


/*
 * GDC_LDC_CFG — reverse-engineered from hiisp_gdc_fw_user.S
 *
 * Computes Lens Distortion Correction parameters. Uses a precomputed
 * FOV LUT for small ratios, rational formula for large ratios, and
 * sin/tan for the radial distortion model.
 */
HI_S32
GDC_LDC_CFG(
    GDC_DATA_S *pstData,
    const LDC_ATTR_S *pstAttr,
    HI_U32 *pstOutput,
    HI_U32 *pstParams)
{
    HI_U32 srcW, srcH;
    HI_S32 s32XRatio, s32YRatio, s32XYRatio;
    HI_S32 s32CenterXOff, s32CenterYOff;
    HI_S32 s32DistortionRatio;
    HI_S32 bAspect, isNegative;
    HI_U32 absDist, maxDim, halfMax, fovRadius;
    double scaleX, scaleY, coefX, coefY;

    s32XRatio  = pstAttr->s32XRatio;
    s32YRatio  = pstAttr->s32YRatio;
    s32XYRatio = pstAttr->s32XYRatio;

    if ((HI_U32)s32XRatio > 100 || (HI_U32)s32YRatio > 100 || (HI_U32)s32XYRatio > 100) {
        fprintf(stderr,
            "[Func]:%s [Line]:%d [Info]:XRatio, YRatio and XYRation must be "
            "configured between [0,100]...\n", "GDC_LDC_MPI_Check", 367);
        return 2;
    }

    s32CenterXOff = pstAttr->s32CenterXOffset;
    s32CenterYOff = pstAttr->s32CenterYOffset;

    {
        HI_S32 absX = s32CenterXOff < 0 ? -s32CenterXOff : s32CenterXOff;
        HI_S32 absY = s32CenterYOff < 0 ? -s32CenterYOff : s32CenterYOff;
        if (absX >= 512 || absY >= 512) {
            fprintf(stderr,
                "[Func]:%s [Line]:%d [Info]:Center offset must be configured "
                "between [-511,511]...\n", "GDC_LDC_MPI_Check", 375);
            return 2;
        }
    }

    s32DistortionRatio = pstAttr->s32DistortionRatio;
    if (s32DistortionRatio < -300 || s32DistortionRatio > 500) {
        fprintf(stderr,
            "[Func]:%s [Line]:%d [Info]:Distortion should be configured "
            "between [-300,500]...\n", "GDC_LDC_MPI_Check", 383);
        return 2;
    }

    srcW = pstData->stSize.u32Width;
    srcH = pstData->stSize.u32Height;

    if (srcW < 640 || srcW > 8192 || srcH < 480 || srcH > 8192) {
        fprintf(stderr,
            "[Func]:%s [Line]:%d [Info]:stSrcImgSize u32Width should be "
            "configured between [640,8192], stSrcImgSize u32Height should "
            "be configured between [480,8192]...\n", "GDC_LDC_MPI_Check", 393);
        return 2;
    }

    if (s32DistortionRatio == 0) {
        fprintf(stderr,
            "[Func]:%s [Line]:%d [Info]:Please call GDC_LDCRatio0_CFG with "
            "s32DistortionRatio=0 ...\n", "GDC_LDC_MPI_Check", 400);
        return 2;
    }

    /* Output dimensions (LDC: dst = src) */
    {
        HI_S32 cx = s32CenterXOff + (HI_S32)(srcW >> 1);
        HI_S32 cy = s32CenterYOff + (HI_S32)(srcH >> 1);

        if (cx < 0 || cy < 0 || cx > (HI_S32)(srcW - 1) || cy > (HI_S32)(srcH - 1)) {
            fprintf(stderr,
                "[Func]:%s [Line]:%d [Info]:Center offset must be configured "
                "between [-511,511]...\n", "GDC_LDC_CFG", 522);
            return -1;
        }

        pstOutput[0]  = 3;
        pstOutput[1]  = srcW;  pstOutput[2]  = srcH;
        pstOutput[3]  = srcW;  pstOutput[4]  = srcH;
        pstOutput[5]  = srcW;  pstOutput[6]  = srcH;
        pstOutput[7]  = (HI_U32)cx;  pstOutput[8]  = (HI_U32)cy;
        pstOutput[9]  = (HI_U32)cx;  pstOutput[10] = (HI_U32)cy;
    }

    /* FOV radius from distortion ratio */
    isNegative = (s32DistortionRatio < 0) ? 1 : 0;
    absDist = isNegative ? (HI_U32)(-s32DistortionRatio) : (HI_U32)s32DistortionRatio;
    maxDim = (srcH >= srcW) ? srcH : srcW;
    halfMax = maxDim >> 1;

    pstParams[79] = isNegative;

    if (absDist > 17) {
        double eff = isNegative
            ? ((double)absDist * 287.0 / 300.0 + 13.0)
            : ((double)(absDist + 13));
        double d_fovRadius = (1.0 / eff - 1.0 / 49.0) * 3460.0;
        d_fovRadius = d_fovRadius / (1.0 / 49.0 - 1.0 / 500.0) + 5110.0;
        fovRadius = (HI_U32)d_fovRadius;
    } else {
        fovRadius = s_ldc_fov_lut[(absDist < 1 ? 1 : absDist) - 1];
    }

    /* Compute FOV radius in pixels and hardware params */
    {
        double angle_factor;
        HI_U32 fovPixels;

        if ((double)fovRadius > 40000.0)
            angle_factor = 1280000.0 * M_PI / 131072.0;
        else if ((double)fovRadius >= 1650.0)
            angle_factor = (double)(fovRadius << 5) * M_PI / 131072.0;
        else
            angle_factor = 1.265534;

        fovPixels = (HI_U32)(0.5 + (double)halfMax * angle_factor);
        if (fovPixels > 0x3FFFF) fovPixels = 0x3FFFF;

        pstParams[91] = fovPixels << 5;
        pstParams[1]  = fovPixels;
        pstParams[87] = (HI_U32)(-(HI_S32)(halfMax * 16));
        pstParams[86] = 0;
        pstParams[88] = 0;
    }

    /* Per-axis distortion scales */
    {
        HI_S32 absXOff = s32CenterXOff < 0 ? -s32CenterXOff : s32CenterXOff;
        HI_S32 absYOff = s32CenterYOff < 0 ? -s32CenterYOff : s32CenterYOff;
        double d_fovR2 = (fovRadius == 0) ? 1.0 : (double)(fovRadius << 1);

        if (isNegative) {
            double adjH = (double)srcH + (double)(absYOff * 2) * 1.05;
            double adjW = (double)srcW + (double)(absXOff * 2) * 1.05;
            double diagSq = adjW * adjW + adjH * adjH;
            double halfDiag = sqrt(diagSq) * 0.5;
            double sinVal = sin(halfDiag * M_PI / d_fovR2);
            double sinScaled = sinVal * 1048576.0 * (double)fovRadius;
            if (halfDiag == 0.0) halfDiag = 1.0;
            scaleX = sinScaled / halfDiag;
            scaleY = scaleX;
        } else {
            double tanX = tan((double)((srcW >> 1) - absXOff - 1) * M_PI / d_fovR2);
            double tanY = tan((double)((srcH >> 1) - absYOff - 1) * M_PI / d_fovR2);
            HI_U32 effW = (srcW >> 1) - absXOff;
            HI_U32 effH = (srcH >> 1) - absYOff;
            tanX *= 1048576.0 * (double)fovRadius;
            tanY *= 1048576.0 * (double)fovRadius;
            scaleX = (effW != 0) ? tanX / (double)effW : tanX;
            scaleY = (effH != 0) ? tanY / (double)effH : tanY;
        }
    }

    /* Apply aspect ratio scaling */
    bAspect = pstAttr->bAspect;
    {
        double absScaleX = scaleX < 0.0 ? -scaleX : scaleX;
        double absScaleY = scaleY < 0.0 ? -scaleY : scaleY;
        double minScale = (absScaleX >= absScaleY) ? scaleY : scaleX;

        if (bAspect == 1) {
            double third = (minScale + minScale) / 3.0;
            double adj = (double)s32XYRatio * (minScale - third) / 100.0;
            coefX = third + adj;
            coefY = coefX;
        } else {
            double third_x = (scaleX + scaleX) / 3.0;
            double third_y = (scaleY + scaleY) / 3.0;
            coefX = third_x + (double)s32XRatio * (scaleX - third_x) / 100.0;
            coefY = third_y + (double)s32YRatio * (scaleY - third_y) / 100.0;
        }
    }

    /* Store LDC hardware parameters */
    pstParams[0]  = 2;
    pstParams[2]  = 0x10000;
    pstParams[3]  = 0xFFFF0000;
    pstParams[4]  = 1024;
    pstParams[5]  = 1024;
    pstParams[6]  = 0;
    pstParams[78] = 0;
    pstParams[80] = (HI_S32)(coefX / 1024.0);
    pstParams[81] = 0;
    pstParams[82] = 0;
    pstParams[83] = 0;
    pstParams[84] = 0;
    pstParams[85] = (HI_S32)(coefY / 1024.0);

    return 0;
}


/*
 * GDC_Spread_CFG — reverse-engineered from hiisp_gdc_fw_user.S
 *
 * Validates src/dst image dimensions and spread coefficient,
 * then computes fixed-point spread parameters.
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

    scaled = -(HI_S64)u32SpreadCoef * 69632;
    biased = scaled + 0x200000;
    product = (HI_U64)u32SpreadCoef * 0x8800 * dstW;
    if (dstH == 0) dstH = 1;
    quotient1 = product / (HI_S64)dstH;
    quotient2 = scaled  / (HI_S64)dstH;

    pstParams[0] = 1;
    params[1] = biased;
    params[2] = -quotient1;
    params[3] = product;
    params[4] = 0;
    params[5] = biased;
    params[6] = 0;
    params[7] = 0;
    params[8] = quotient2;
    params[9] = 0x200000;

    return 0;
}


/*
 * GDC_FreeAngleRotation_CFG — reverse-engineered from hiisp_gdc_fw_user.S
 *
 * Computes a free-angle rotation 2D affine transform with view-type
 * dependent scaling (TYPICAL/ALL/INSIDE).
 */
HI_S32
GDC_FreeAngleRotation_CFG(
    GDC_DATA_S *pstData,
    const ROTATION_EX_S *pstRotationEx,
    HI_U32 *pstOutput,
    HI_U32 *pstParams)
{
    HI_U32 srcW, srcH, dstW, dstH;
    HI_U32 u32Angle;
    ROTATION_VIEW_TYPE_E enViewType;
    HI_S64 *params = (HI_S64 *)pstParams;
    double h_scale, v_scale, aspect;
    double sin_val, cos_val, abs_sin, abs_cos;
    double row1_c1, row1_c2, row2_c1, row2_c2;
    double cx_dst, cy_dst, cx_src, cy_src;
    double tx, ty, offX, offY;

    enViewType = pstRotationEx->enViewType;

    if ((HI_U32)enViewType > 2) {
        fprintf(stderr,
            "[Func]:%s [Line]:%d [Info]:Wrong configuration of enViewType...\n",
            "GDC_FreeAngleRot_MPI_Check", 720);
        return 3;
    }

    {
        HI_S32 absX = pstRotationEx->s32CenterXOffset;
        HI_S32 absY = pstRotationEx->s32CenterYOffset;
        if (absX < 0) absX = -absX;
        if (absY < 0) absY = -absY;
        if (absX >= 512 || absY >= 512) {
            fprintf(stderr,
                "[Func]:%s [Line]:%d [Info]:Wrong configuration of enViewType...\n",
                "GDC_FreeAngleRot_MPI_Check", 728);
            return 3;
        }
    }

    u32Angle = pstRotationEx->u32Angle;
    if (u32Angle > 360) {
        fprintf(stderr,
            "[Func]:%s [Line]:%d [Info]:Rotation angle must be configured "
            "between [0,%d]...\n", "GDC_FreeAngleRot_MPI_Check", 735, 360);
        return 3;
    }

    dstH = pstRotationEx->stDestSize.u32Height;
    dstW = pstRotationEx->stDestSize.u32Width;

    if (dstH < 360 || dstH > 8192 || dstW < 480 || dstW > 8192) {
        fprintf(stderr,
            "[Func]:%s [Line]:%d [Info]:Wrong Configuration of the output "
            "image size...\n The width [%d] should between 480 and 8192 \n "
            "The height [%d] should between 360 and 8192\n",
            "GDC_FreeAngleRot_MPI_Check", 746, dstW, dstH);
        return 3;
    }

    srcW = pstData->stSize.u32Width;
    srcH = pstData->stSize.u32Height;

    pstOutput[0] = 4;
    pstOutput[1] = srcW;
    pstOutput[2] = srcH;
    pstOutput[3] = dstW;
    pstOutput[4] = dstH;

    /* Center offsets: only used for VIEW_TYPICAL */
    if ((HI_U32)enViewType & 1) {
        offX = (double)pstRotationEx->s32CenterXOffset;
        offY = (double)pstRotationEx->s32CenterYOffset;
    } else {
        offX = 0.0;
        offY = 0.0;
    }

    h_scale = (double)srcW / (double)dstW;
    v_scale = (double)srcH / (double)dstH;
    aspect  = (double)srcH / (double)srcW;

    if (u32Angle >= 360) {
        sin_val = 1.0; cos_val = 0.0;
        abs_sin = 0.0; abs_cos = 0.0;
    } else {
        double rad = (double)u32Angle * 2.0 * M_PI / 360.0;
        sin_val = sin(rad);
        cos_val = cos(rad);
        abs_sin = sin_val < 0.0 ? -sin_val : sin_val;
        abs_cos = cos_val < 0.0 ? -cos_val : cos_val;
    }

    /* View-type dependent scaling */
    if (enViewType == ROTATION_VIEW_TYPE_ALL) {
        double scale_x = abs_sin + aspect * abs_cos;
        double scale_y = (aspect == 0.0) ? (abs_cos + abs_sin)
                                          : (abs_cos / aspect + abs_sin);
        scale_x = h_scale * scale_x;
        scale_y = v_scale * scale_y;
        if (scale_x < scale_y) scale_x = scale_y;
        h_scale = scale_x;
        v_scale = scale_x;
    } else if (enViewType != ROTATION_VIEW_TYPE_TYPICAL) {
        /* VIEW_INSIDE */
        double sum1 = abs_cos / aspect + abs_sin;
        double sum2 = abs_sin * aspect + abs_cos;
        double inv_scale = 1.0;
        if (sum1 != 0.0 && sum2 != 0.0) {
            double inv1 = 1.0 / sum1;
            double inv2 = 1.0 / sum2;
            inv_scale = (inv1 > inv2) ? inv2 : inv1;
        } else if (sum1 != 0.0) {
            inv_scale = 1.0 / sum1;
        } else if (sum2 != 0.0) {
            inv_scale = 1.0 / sum2;
        }
        h_scale *= inv_scale;
        v_scale *= inv_scale;
    }

    /* Build rotation matrix */
    row1_c1 = h_scale * sin_val;
    row1_c2 = v_scale * cos_val;
    row2_c1 = h_scale * (-cos_val);
    row2_c2 = v_scale * sin_val;

    cx_dst = offX + (double)(dstW - 1) * 0.5;
    cy_dst = offY + (double)(dstH - 1) * 0.5;
    cx_src = offX + (double)(srcW - 1) * 0.5;
    cy_src = offY + (double)(srcH - 1) * 0.5;

    tx = cx_src - row1_c1 * cx_dst - row1_c2 * cy_dst;
    ty = cy_src - row2_c1 * cx_dst - row2_c2 * cy_dst;

    /* Store fixed-point coefficients (scaled by 1024) */
    pstParams[0] = 1;
    params[1] = (HI_S64)round(row1_c1 * 1024.0);
    params[2] = (HI_S64)round(row1_c2 * 1024.0);
    params[3] = (HI_S64)round(tx * 1024.0);
    params[4] = (HI_S64)round(row2_c1 * 1024.0);
    params[5] = (HI_S64)round(row2_c2 * 1024.0);
    params[6] = (HI_S64)round(ty * 1024.0);
    params[7] = 0;
    params[8] = 0;
    params[9] = 1024;

    return 0;
}
