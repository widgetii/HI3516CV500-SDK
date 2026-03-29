/**
 * Reverse Engineered by TekuConcept on September 20, 2020
 */

#ifndef RE_HIISP_GDC_FW_USER_H
#define RE_HIISP_GDC_FW_USER_H

// #include "hiisp_gdc_fw_user.h"
#include "re_mpi_comm.h"
#include "re_mpi_gdc.h"
#include "mpi_errno.h"

HI_S32 GDC_Fisheye_CFG(const GDC_DATA_S *pstData, const HI_VOID *pstGdcFisheyeCfg,
    HI_U32 *pstOutput1, HI_U32 *pstFwFisheyeParams);
HI_S32 GDC_Trapzoid_CFG(const SIZE_S *pstSize, HI_U32 u32TrapezoidCoef,
    HI_S64 *pstParams);
HI_S32 GDC_LDC_CFG(GDC_DATA_S *pstData, const LDC_ATTR_S *pstAttr,
    HI_U32 *pstOutput, HI_U32 *pstParams);
HI_S32 GDC_FreeAngleRotation_CFG(GDC_DATA_S *pstData, const ROTATION_EX_S *pstRotationEx,
    HI_U32 *pstOutput, HI_U32 *pstParams);

HI_S32 gdc_spread_configure(
    const HI_U32 *pstSrcSize,
    const HI_U32 *pstDstSize,
    HI_U32        u32SpreadCoef,
    HI_U32       *pstOutput,
    HI_U32       *pstParams);

#endif
