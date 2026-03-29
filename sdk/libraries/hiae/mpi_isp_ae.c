/**
 * Reverse Engineered by TekuConcept on May 2, 2021
 */

#include "re_mpi_isp_ae.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define RE_DBG_LVL HI_DBG_ERR
#define ISP_DEV_NAME "/dev/isp_dev"

// ============================================================================

HI_S32 g_as32AeFd[4] = { -1, -1, -1, -1 };

// ============================================================================

extern HI_S32 VReg_Init(VI_PIPE ViPipe, HI_U32 u32BaseAddr, HI_U32 u32Size);
extern HI_S32 VReg_Exit(VI_PIPE ViPipe, HI_U32 u32BaseAddr, HI_U32 u32Size);
extern HI_S32 VReg_ReleaseAll(VI_PIPE ViPipe);
extern HI_VOID *VReg_GetVirtAddrBase(HI_U32 u32BaseAddr);
extern HI_S32 VReg_Munmap(HI_U32 u32BaseAddr, HI_U32 u32Size);
extern HI_U32 IO_READ32(HI_U32 u32Addr);
extern HI_S32 IO_WRITE32(HI_U32 u32Addr, HI_U32 u32Value);
extern HI_S32 IO_WRITE32_Ex(HI_U32 u32Addr, HI_U32 u32Value);
extern HI_S32 IO_READ32_Ex(HI_U32 u32Addr, HI_U32 *pu32Value);
extern HI_U16 IO_READ16(HI_U32 u32Addr);
extern HI_S32 IO_WRITE16(HI_U32 u32Addr, HI_U32 u32Value);
extern HI_U8  IO_READ8(HI_U32 u32Addr);
extern HI_S32 IO_WRITE8(HI_U32 u32Addr, HI_U32 u32Value);

// ============================================================================

HI_S32
HI_MPI_ISP_SetExposureAttr(VI_PIPE ViPipe, const ISP_EXPOSURE_ATTR_S *pstExpAttr)
{
    HI_S32 result;
    HI_BOOL bMemInit;
    HI_CHAR strLibName[20];
    HI_U32 offset;
    HI_U16 tmp16;

    if ( ViPipe > 3 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Err AE dev %d in %s!\n", ViPipe, __FUNCTION__);
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    if ( pstExpAttr == HI_NULL ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Null Pointer in %s!\n", __FUNCTION__);
        return HI_ERR_ISP_NULL_PTR;
    }

    if ( g_as32AeFd[ViPipe] <= 0 ) {
        g_as32AeFd[ViPipe] = open(ISP_DEV_NAME, O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO);
        if ( g_as32AeFd[ViPipe] < 0 ) {
            perror("open isp device error!\n");
            return HI_ERR_ISP_NOT_INIT;
        }

        result = ioctl(g_as32AeFd[ViPipe], ISP_DEV_SET_FD, &ViPipe) == 0;
        if ( result != HI_SUCCESS ) {
            close(g_as32AeFd[ViPipe]);
            g_as32AeFd[ViPipe] = -1;
            return HI_ERR_ISP_NOT_INIT;
        }
    }

    bMemInit = HI_FALSE;
    result = ioctl(g_as32AeFd[ViPipe], ISP_MEM_INFO_GET, &bMemInit);
    if ( result != HI_SUCCESS ) {
        HI_TRACE_ISP(RE_DBG_LVL, "ISP[%d] get Mem info failed!\n", ViPipe);
        return HI_ERR_ISP_MEM_NOT_INIT;
    }

    if ( !bMemInit ) {
        HI_TRACE_ISP(RE_DBG_LVL, "ISP[%d] Mem NOT Init %d!\n", ViPipe, bMemInit);
        return HI_ERR_ISP_MEM_NOT_INIT;
    }

    strncpy_s(strLibName, sizeof(strLibName), "hisi_ae_lib", 11);
    offset = (HI_U8)(IO_READ32((ViPipe << 17) + 0x100034) >> 8);

    /* Validate bByPass */
    if ( pstExpAttr->bByPass > 1 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Invalid AE bByPass input!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate bHistStatAdjust */
    if ( pstExpAttr->bHistStatAdjust > 1 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Invalid AE HistStatAdjust!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate bAERouteExValid */
    if ( pstExpAttr->bAERouteExValid > 1 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Invalid bAERouteExValid!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate enOpType */
    if ( pstExpAttr->enOpType > 1 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Invalid exposure type!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate u8AERunInterval */
    if ( pstExpAttr->u8AERunInterval == 0 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "AE run interval must larger than 0!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Write bByPass */
    IO_WRITE8((offset << 13) + 0x700001, pstExpAttr->bByPass);

    /* Write bHistStatAdjust */
    IO_WRITE16((offset << 13) + 0x700156, pstExpAttr->bHistStatAdjust & 1);

    /* Write bAERouteExValid */
    IO_WRITE8((offset << 13) + 0x7003D4, pstExpAttr->bAERouteExValid & 1);

    /* Write enOpType - bit 0 of register 0x700056 */
    if ( pstExpAttr->enOpType == 0 ) {
        tmp16 = IO_READ16((offset << 13) + 0x700056);
        IO_WRITE16((offset << 13) + 0x700056, (HI_U16)(tmp16 & 0xFFFE));
    } else {
        tmp16 = IO_READ16((offset << 13) + 0x700056);
        IO_WRITE16((offset << 13) + 0x700056, (HI_U16)(tmp16 | 1));
    }

    /* Write u8AERunInterval */
    IO_WRITE8((offset << 13) + 0x700198, pstExpAttr->u8AERunInterval);

    /* Validate stAuto.enAEMode */
    if ( pstExpAttr->stAuto.enAEMode > 1 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Invalid AE mode!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate stAuto.enAEStrategyMode */
    if ( pstExpAttr->stAuto.enAEStrategyMode > 1 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Invalid AE strategy mode!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate ExpTime range */
    if ( pstExpAttr->stAuto.stExpTimeRange.u32Max < pstExpAttr->stAuto.stExpTimeRange.u32Min ) {
        HI_TRACE_ISP(RE_DBG_LVL, "ExpTimeMax should not be less than ExpTimeMin!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate AGain range */
    if ( pstExpAttr->stAuto.stAGainRange.u32Max < pstExpAttr->stAuto.stAGainRange.u32Min ) {
        HI_TRACE_ISP(RE_DBG_LVL, "AGainMax should not be less than AGainMin!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }
    if ( pstExpAttr->stAuto.stAGainRange.u32Min <= 0x3FF ||
         pstExpAttr->stAuto.stAGainRange.u32Max <= 0x3FF ) {
        HI_TRACE_ISP(RE_DBG_LVL, "AGainMax/AGainMin should not be less than 0x400!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate DGain range */
    if ( pstExpAttr->stAuto.stDGainRange.u32Max < pstExpAttr->stAuto.stDGainRange.u32Min ) {
        HI_TRACE_ISP(RE_DBG_LVL, "DGainMax should not be less than DGainMin!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }
    if ( pstExpAttr->stAuto.stDGainRange.u32Min <= 0x3FF ||
         pstExpAttr->stAuto.stDGainRange.u32Max <= 0x3FF ) {
        HI_TRACE_ISP(RE_DBG_LVL, "DGainMax/DGainMin should not be less than 0x400!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate ISPDGain range */
    if ( pstExpAttr->stAuto.stISPDGainRange.u32Min > pstExpAttr->stAuto.stISPDGainRange.u32Max ) {
        HI_TRACE_ISP(RE_DBG_LVL, "ISPDGainMax should not be less than ISPDGainMin!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }
    if ( pstExpAttr->stAuto.stISPDGainRange.u32Min <= 0x3FF ||
         pstExpAttr->stAuto.stISPDGainRange.u32Max <= 0x3FF ) {
        HI_TRACE_ISP(RE_DBG_LVL, "ISPDGainMax/ISPDGainMin should not be less than 0x400!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate SysGain range */
    if ( pstExpAttr->stAuto.stSysGainRange.u32Max < pstExpAttr->stAuto.stSysGainRange.u32Min ) {
        HI_TRACE_ISP(RE_DBG_LVL, "SysGainMax should not be less than SysGainMin!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }
    if ( pstExpAttr->stAuto.stSysGainRange.u32Max < 0x400 ||
         pstExpAttr->stAuto.stSysGainRange.u32Min < 0x400 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "SysGainMax/SysGainMin should not be less than 0x400!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate GainThreshold */
    if ( pstExpAttr->stAuto.u32GainThreshold < 0x400 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Gain threshold for Slow Shutter Mode should not be less than 0x400!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate Antiflicker bEnable */
    if ( pstExpAttr->stAuto.stAntiflicker.bEnable > 1 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Invalid anti flicker bEnable!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate Antiflicker mode */
    if ( pstExpAttr->stAuto.stAntiflicker.enMode > 1 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Invalid anti flicker mode!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate Subflicker bEnable */
    if ( pstExpAttr->stAuto.stSubflicker.bEnable > 1 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Invalid AE Subflicker Enable!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate Subflicker u8LumaDiff */
    if ( pstExpAttr->stAuto.stSubflicker.u8LumaDiff > 0x64 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Subflicker u8LumaDiff should not be larger than 0x64!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate bManualExpValue */
    if ( pstExpAttr->stAuto.bManualExpValue > 1 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Invalid AE bManualExpValue!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate u32ExpValue */
    if ( pstExpAttr->stAuto.u32ExpValue == 0 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "u32ExpValue must larger than 0!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate enFSWDRMode */
    if ( pstExpAttr->stAuto.enFSWDRMode > 2 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Invalid FSWDR running mode!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate bWDRQuick */
    if ( pstExpAttr->stAuto.bWDRQuick > 1 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Invalid AE bWDRQuick!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Write auto exposure ranges */
    IO_WRITE32((offset << 13) + 0x700010, pstExpAttr->stAuto.stExpTimeRange.u32Max);
    IO_WRITE32((offset << 13) + 0x700014, pstExpAttr->stAuto.stExpTimeRange.u32Min);
    IO_WRITE32((offset << 13) + 0x700018, pstExpAttr->stAuto.stAGainRange.u32Max);
    IO_WRITE32((offset << 13) + 0x70001C, pstExpAttr->stAuto.stAGainRange.u32Min);
    IO_WRITE32((offset << 13) + 0x700020, pstExpAttr->stAuto.stDGainRange.u32Max);
    IO_WRITE32((offset << 13) + 0x700024, pstExpAttr->stAuto.stDGainRange.u32Min);
    IO_WRITE32((offset << 13) + 0x700028, pstExpAttr->stAuto.stISPDGainRange.u32Max);
    IO_WRITE32((offset << 13) + 0x70002C, pstExpAttr->stAuto.stISPDGainRange.u32Min);
    IO_WRITE32((offset << 13) + 0x700030, pstExpAttr->stAuto.stSysGainRange.u32Max);
    IO_WRITE32((offset << 13) + 0x700034, pstExpAttr->stAuto.stSysGainRange.u32Min);
    IO_WRITE32((offset << 13) + 0x700038, pstExpAttr->stAuto.u32GainThreshold);
    IO_WRITE8 ((offset << 13) + 0x70000B, pstExpAttr->stAuto.u8Speed);
    IO_WRITE16((offset << 13) + 0x70052E, pstExpAttr->stAuto.u16BlackSpeedBias);
    IO_WRITE8 ((offset << 13) + 0x70000C, pstExpAttr->stAuto.u8Tolerance);
    IO_WRITE8 ((offset << 13) + 0x70000D, pstExpAttr->stAuto.u8Compensation);
    IO_WRITE16((offset << 13) + 0x700158, pstExpAttr->stAuto.u16EVBias);
    IO_WRITE8 ((offset << 13) + 0x70015A, pstExpAttr->stAuto.enAEStrategyMode & 3);
    IO_WRITE16((offset << 13) + 0x70015C, pstExpAttr->stAuto.u16HistRatioSlope);
    IO_WRITE8 ((offset << 13) + 0x70015B, pstExpAttr->stAuto.u8MaxHistOffset);
    IO_WRITE8 ((offset << 13) + 0x700002, pstExpAttr->stAuto.enAEMode);

    /* Write Antiflicker bEnable - bit 4 of 0x700009 */
    {
        HI_U8 val = IO_READ8((offset << 13) + 0x700009);
        val = (val & 0xEF) | ((pstExpAttr->stAuto.stAntiflicker.bEnable << 4) & 0x10);
        IO_WRITE8((offset << 13) + 0x700009, val);
    }

    /* Write Antiflicker u8Frequency */
    IO_WRITE8((offset << 13) + 0x700008, pstExpAttr->stAuto.stAntiflicker.u8Frequency);

    /* Write Antiflicker enMode - bits 0-1 of 0x700009 */
    {
        HI_U8 val = IO_READ8((offset << 13) + 0x700009);
        val = (val & 0xFC) | (pstExpAttr->stAuto.stAntiflicker.enMode & 3);
        IO_WRITE8((offset << 13) + 0x700009, val);
    }

    /* Write Subflicker */
    IO_WRITE8 ((offset << 13) + 0x70019C, pstExpAttr->stAuto.stSubflicker.bEnable & 1);
    IO_WRITE8 ((offset << 13) + 0x70019D, pstExpAttr->stAuto.stSubflicker.u8LumaDiff);

    /* Write AE delay */
    IO_WRITE16((offset << 13) + 0x70003C, pstExpAttr->stAuto.stAEDelayAttr.u16BlackDelayFrame);
    IO_WRITE16((offset << 13) + 0x70003E, pstExpAttr->stAuto.stAEDelayAttr.u16WhiteDelayFrame);

    /* Write bManualExpValue - bit 0 of (offset+0x380) << 13 */
    {
        HI_U8 val = IO_READ8((offset + 0x380) << 13);
        val = (val & 0xFE) | (pstExpAttr->stAuto.bManualExpValue & 1);
        IO_WRITE8((offset + 0x380) << 13, val);
    }

    /* Write u32ExpValue */
    IO_WRITE32((offset << 13) + 0x7001C8, pstExpAttr->stAuto.u32ExpValue);

    /* Write enFSWDRMode */
    IO_WRITE8((offset << 13) + 0x700526, pstExpAttr->stAuto.enFSWDRMode & 3);

    /* Write bWDRQuick */
    IO_WRITE8((offset << 13) + 0x700527, pstExpAttr->stAuto.bWDRQuick & 1);

    /* Write u16ISOCalCoef */
    IO_WRITE16((offset << 13) + 0x700716, pstExpAttr->stAuto.u16ISOCalCoef);

    /* Validate manual params */
    if ( pstExpAttr->stManual.enExpTimeOpType > 1 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Invalid Manual ExpTime Enable!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }
    if ( pstExpAttr->stManual.enAGainOpType > 1 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Invalid Manual AGain Enable!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }
    if ( pstExpAttr->stManual.enDGainOpType > 1 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Invalid Manual DGain Enable!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }
    if ( pstExpAttr->stManual.enISPDGainOpType > 1 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Invalid Manual ISPDGain Enable!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }
    if ( pstExpAttr->stManual.u32AGain < 0x400 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Manual AGain should not be less than 0x400!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }
    if ( pstExpAttr->stManual.u32DGain < 0x400 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Manual DGain should not be less than 0x400!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }
    if ( pstExpAttr->stManual.u32ISPDGain < 0x400 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Manual ISPDGain should not be less than 0x400!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Write manual enExpTimeOpType - bit 3 of 0x700056 */
    {
        tmp16 = IO_READ16((offset << 13) + 0x700056);
        tmp16 = (tmp16 & 0xFFF7) | ((pstExpAttr->stManual.enExpTimeOpType << 3) & 8);
        IO_WRITE16((offset << 13) + 0x700056, tmp16);
    }

    /* Write manual enAGainOpType - bit 9 of 0x700056 */
    {
        tmp16 = IO_READ16((offset << 13) + 0x700056);
        tmp16 = (tmp16 & 0xFDFF) | ((pstExpAttr->stManual.enAGainOpType << 9) & 0x200);
        IO_WRITE16((offset << 13) + 0x700056, tmp16);
    }

    /* Write manual enDGainOpType - bit 10 of 0x700056 */
    {
        tmp16 = IO_READ16((offset << 13) + 0x700056);
        tmp16 = (tmp16 & 0xFBFF) | ((pstExpAttr->stManual.enDGainOpType << 10) & 0x400);
        IO_WRITE16((offset << 13) + 0x700056, tmp16);
    }

    /* Write manual enISPDGainOpType - bit 11 of 0x700056 */
    {
        tmp16 = IO_READ16((offset << 13) + 0x700056);
        tmp16 = (tmp16 & 0xF7FF) | ((pstExpAttr->stManual.enISPDGainOpType << 11) & 0x800);
        IO_WRITE16((offset << 13) + 0x700056, tmp16);
    }

    /* Write manual exposure values */
    IO_WRITE32((offset << 13) + 0x70005C, pstExpAttr->stManual.u32ExpTime);
    IO_WRITE32((offset << 13) + 0x700060, pstExpAttr->stManual.u32AGain);
    IO_WRITE32((offset << 13) + 0x700064, pstExpAttr->stManual.u32DGain);
    IO_WRITE32((offset << 13) + 0x700068, pstExpAttr->stManual.u32ISPDGain);

    return HI_SUCCESS;
}

// ============================================================================

HI_S32
HI_MPI_ISP_SetWDRExposureAttr(VI_PIPE ViPipe, const ISP_WDR_EXPOSURE_ATTR_S *pstWDRExpAttr)
{
    HI_S32 result;
    HI_BOOL bMemInit;
    HI_CHAR strLibName[20];
    HI_U32 offset, i;

    if ( ViPipe > 3 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Err AE dev %d in %s!\n", ViPipe, __FUNCTION__);
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    if ( pstWDRExpAttr == HI_NULL ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Null Pointer in %s!\n", __FUNCTION__);
        return HI_ERR_ISP_NULL_PTR;
    }

    if ( g_as32AeFd[ViPipe] <= 0 ) {
        g_as32AeFd[ViPipe] = open(ISP_DEV_NAME, O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO);
        if ( g_as32AeFd[ViPipe] < 0 ) {
            perror("open isp device error!\n");
            return HI_ERR_ISP_NOT_INIT;
        }

        result = ioctl(g_as32AeFd[ViPipe], ISP_DEV_SET_FD, &ViPipe) == 0;
        if ( result != HI_SUCCESS ) {
            close(g_as32AeFd[ViPipe]);
            g_as32AeFd[ViPipe] = -1;
            return HI_ERR_ISP_NOT_INIT;
        }
    }

    bMemInit = HI_FALSE;
    result = ioctl(g_as32AeFd[ViPipe], ISP_MEM_INFO_GET, &bMemInit);
    if ( result != HI_SUCCESS ) {
        HI_TRACE_ISP(RE_DBG_LVL, "ISP[%d] get Mem info failed!\n", ViPipe);
        return HI_ERR_ISP_MEM_NOT_INIT;
    }

    if ( !bMemInit ) {
        HI_TRACE_ISP(RE_DBG_LVL, "ISP[%d] Mem NOT Init %d!\n", ViPipe, bMemInit);
        return HI_ERR_ISP_MEM_NOT_INIT;
    }

    strncpy_s(strLibName, sizeof(strLibName), "hisi_ae_lib", 11);
    offset = (HI_U8)(IO_READ32((ViPipe << 17) + 0x100034) >> 8);

    /* Validate enExpRatioType */
    if ( pstWDRExpAttr->enExpRatioType > 1 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Invalid enExpRatioType %d!\n", pstWDRExpAttr->enExpRatioType);
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate au32ExpRatio[0..2] range [0x40, 0xFFF] */
    for ( i = 0; i < EXP_RATIO_NUM; i++ ) {
        HI_U32 ratio = pstWDRExpAttr->au32ExpRatio[i];
        if ( (ratio - 0x40) > 0xFC0 ) {
            HI_TRACE_ISP(RE_DBG_LVL, "Invalid au32ExpRatio[%d] %d!\n", i, ratio);
            return HI_ERR_ISP_ILLEGAL_PARAM;
        }
    }

    /* Write enExpRatioType */
    IO_WRITE8((offset << 13) + 0x700003, pstWDRExpAttr->enExpRatioType ? 1 : 0);

    /* Validate u32ExpRatioMax */
    if ( (pstWDRExpAttr->u32ExpRatioMax - 0x40) > 0x3FC0 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "u32ExpRatioMax's range is [0x40, 0x4000]!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate u32ExpRatioMin */
    if ( (pstWDRExpAttr->u32ExpRatioMin - 0x40) > 0x3FC0 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "u32ExpRatioMin's range is [0x40, 0x4000]!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate Min <= Max */
    if ( pstWDRExpAttr->u32ExpRatioMax < pstWDRExpAttr->u32ExpRatioMin ) {
        HI_TRACE_ISP(RE_DBG_LVL, "u32ExpRatioMin %u should not larger than u32ExpRatioMax %u!\n",
                      pstWDRExpAttr->u32ExpRatioMin, pstWDRExpAttr->u32ExpRatioMax);
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate u16Tolerance */
    if ( pstWDRExpAttr->u16Tolerance > 0xFF ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Invalid u16Tolerance %d!\n", pstWDRExpAttr->u16Tolerance);
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate u16Speed */
    if ( pstWDRExpAttr->u16Speed > 0xFF ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Invalid u16Speed %d!\n", pstWDRExpAttr->u16Speed);
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Write au32ExpRatio */
    IO_WRITE16((offset << 13) + 0x700004, pstWDRExpAttr->au32ExpRatio[0] & 0xFFF);
    IO_WRITE16((offset << 13) + 0x70051A, pstWDRExpAttr->au32ExpRatio[1] & 0xFFF);
    IO_WRITE16((offset << 13) + 0x70051C, pstWDRExpAttr->au32ExpRatio[2] & 0xFFF);

    /* Write u32ExpRatioMax/Min */
    IO_WRITE16((offset << 13) + 0x70019E, (HI_U16)pstWDRExpAttr->u32ExpRatioMax);
    IO_WRITE16((offset << 13) + 0x70051E, (HI_U16)pstWDRExpAttr->u32ExpRatioMin);

    /* Write u16Tolerance, u16Speed, u16RatioBias */
    IO_WRITE16((offset << 13) + 0x700520, pstWDRExpAttr->u16Tolerance);
    IO_WRITE16((offset << 13) + 0x700522, pstWDRExpAttr->u16Speed);
    IO_WRITE16((offset << 13) + 0x700524, pstWDRExpAttr->u16RatioBias);

    return HI_SUCCESS;
}

// ============================================================================

HI_S32
HI_MPI_ISP_SetHDRExposureAttr(VI_PIPE ViPipe, const ISP_HDR_EXPOSURE_ATTR_S *pstHDRExpAttr)
{
    HI_S32 result;
    HI_BOOL bMemInit;
    HI_CHAR strLibName[20];
    HI_U32 offset;

    if ( ViPipe > 3 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Err AE dev %d in %s!\n", ViPipe, __FUNCTION__);
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    if ( pstHDRExpAttr == HI_NULL ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Null Pointer in %s!\n", __FUNCTION__);
        return HI_ERR_ISP_NULL_PTR;
    }

    if ( g_as32AeFd[ViPipe] <= 0 ) {
        g_as32AeFd[ViPipe] = open(ISP_DEV_NAME, O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO);
        if ( g_as32AeFd[ViPipe] < 0 ) {
            perror("open isp device error!\n");
            return HI_ERR_ISP_NOT_INIT;
        }

        result = ioctl(g_as32AeFd[ViPipe], ISP_DEV_SET_FD, &ViPipe) == 0;
        if ( result != HI_SUCCESS ) {
            close(g_as32AeFd[ViPipe]);
            g_as32AeFd[ViPipe] = -1;
            return HI_ERR_ISP_NOT_INIT;
        }
    }

    bMemInit = HI_FALSE;
    result = ioctl(g_as32AeFd[ViPipe], ISP_MEM_INFO_GET, &bMemInit);
    if ( result != HI_SUCCESS ) {
        HI_TRACE_ISP(RE_DBG_LVL, "ISP[%d] get Mem info failed!\n", ViPipe);
        return HI_ERR_ISP_MEM_NOT_INIT;
    }

    if ( !bMemInit ) {
        HI_TRACE_ISP(RE_DBG_LVL, "ISP[%d] Mem NOT Init %d!\n", ViPipe, bMemInit);
        return HI_ERR_ISP_MEM_NOT_INIT;
    }

    strncpy_s(strLibName, sizeof(strLibName), "hisi_ae_lib", 11);
    offset = (HI_U8)(IO_READ32((ViPipe << 17) + 0x100034) >> 8);

    /* Validate enExpHDRLvType */
    if ( pstHDRExpAttr->enExpHDRLvType > 1 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Invalid enExpHDRLvType %d!\n", pstHDRExpAttr->enExpHDRLvType);
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Write enExpHDRLvType */
    IO_WRITE8((offset << 13) + 0x700616, pstHDRExpAttr->enExpHDRLvType ? 1 : 0);

    /* Validate u32ExpHDRLv range [0x40, 0x400] */
    if ( (pstHDRExpAttr->u32ExpHDRLv - 0x40) > 0x3C0 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Invalid u32ExpHDRLv %d!\n", pstHDRExpAttr->u32ExpHDRLv);
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate u32ExpHDRLvMax range [0x40, 0x400] */
    if ( (pstHDRExpAttr->u32ExpHDRLvMax - 0x40) > 0x3C0 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "u32ExpHDRLvMax's range is [0x40, 0x4000]!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate u32ExpHDRLvMin range [0x40, 0x400] */
    if ( (pstHDRExpAttr->u32ExpHDRLvMin - 0x40) > 0x3C0 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "u32ExpHDRLvMin's range is [0x40, 0x4000]!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate Min <= Max */
    if ( pstHDRExpAttr->u32ExpHDRLvMax < pstHDRExpAttr->u32ExpHDRLvMin ) {
        HI_TRACE_ISP(RE_DBG_LVL, "u32ExpHDRLvMin %u should not larger than u32ExpHDRLvMax %u!\n",
                      pstHDRExpAttr->u32ExpHDRLvMin, pstHDRExpAttr->u32ExpHDRLvMax);
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate u32ExpHDRLvWeight */
    if ( pstHDRExpAttr->u32ExpHDRLvWeight > 0x400 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Invalid u32ExpHDRLvWeight %d!\n", pstHDRExpAttr->u32ExpHDRLvWeight);
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Write HDR exposure params */
    IO_WRITE32((offset << 13) + 0x700618, pstHDRExpAttr->u32ExpHDRLv);
    IO_WRITE32((offset << 13) + 0x700620, pstHDRExpAttr->u32ExpHDRLvMin);
    IO_WRITE32((offset << 13) + 0x70061C, pstHDRExpAttr->u32ExpHDRLvMax);
    IO_WRITE32((offset << 13) + 0x700624, pstHDRExpAttr->u32ExpHDRLvWeight);

    return HI_SUCCESS;
}

// ============================================================================

static HI_S32
ISP_RouteCheck(HI_U32 offset, const ISP_AE_ROUTE_S *pstRoute)
{
    HI_U32 i, u32Num;
    HI_U32 irisType, bPiris;

    if ( pstRoute == HI_NULL ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Null Pointer in %s!\n", __FUNCTION__);
        return HI_ERR_ISP_NULL_PTR;
    }

    irisType = IO_READ16((offset + 0x700) << 12 | 0x176) & 3;
    bPiris   = IO_READ8 ((offset + 0x700) << 12 | 0x54C) & 1;

    u32Num = pstRoute->u32TotalNum;

    if ( u32Num > ISP_AE_ROUTE_MAX_NODES ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Error! The route node number %d is larger than 16!\n", u32Num);
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    if ( u32Num == 0 )
        return HI_SUCCESS;

    /* First pass: validate each node */
    for ( i = 0; i < u32Num; i++ ) {
        HI_U32 intTime  = pstRoute->astRouteNode[i].u32IntTime;
        HI_U32 sysGain  = pstRoute->astRouteNode[i].u32SysGain;

        if ( irisType != 1 ) {
            /* DC-iris mode */
            if ( intTime == 0 ) {
                HI_TRACE_ISP(RE_DBG_LVL, "Error! The route node's IntTime should not be less than 1!\n");
                return HI_ERR_ISP_ILLEGAL_PARAM;
            }
            if ( sysGain < 0x400 ) {
                HI_TRACE_ISP(RE_DBG_LVL, "Error! The route node's SysGain should not be less than 0x400!\n");
                return HI_ERR_ISP_ILLEGAL_PARAM;
            }
            {
                HI_U64 exposure = (HI_U64)intTime * sysGain;
                if ( (HI_S64)exposure < 0 ) {
                    HI_TRACE_ISP(RE_DBG_LVL, "Error! The route node's Exposure should not be larger than 0x7FFFFFFFFFFFFFFF in DC-Iris mode!\n");
                    return HI_ERR_ISP_ILLEGAL_PARAM;
                }
            }
        } else {
            /* P-iris mode */
            if ( intTime == 0 ) {
                HI_TRACE_ISP(RE_DBG_LVL, "Error! The route node's IntTime should not be less than 1!\n");
                return HI_ERR_ISP_ILLEGAL_PARAM;
            }
            if ( sysGain < 0x400 ) {
                HI_TRACE_ISP(RE_DBG_LVL, "Error! The route node's SysGain should not be less than 0x400!\n");
                return HI_ERR_ISP_ILLEGAL_PARAM;
            }
            if ( pstRoute->astRouteNode[i].enIrisFNO > 10 ) {
                HI_TRACE_ISP(RE_DBG_LVL, "Error! The route node's IrisFNO should not be larger than ISP_IRIS_F_NO_1_0!\n");
                return HI_ERR_ISP_ILLEGAL_PARAM;
            }
            if ( bPiris ) {
                HI_U32 irisLin = pstRoute->astRouteNode[i].u32IrisFNOLin;
                if ( (irisLin - 1) >= 0x400 ) {
                    HI_TRACE_ISP(RE_DBG_LVL, "Error! The route node's IrisFNOLin range is [1, 1024]!\n");
                    return HI_ERR_ISP_ILLEGAL_PARAM;
                }
            }
            {
                HI_U64 exposure = (HI_U64)intTime * sysGain;
                if ( exposure > 0x1FFFFFFFFFFFFFULL ) {
                    HI_TRACE_ISP(RE_DBG_LVL, "Error! The route node's Exposure(IntTime*SysGain) should not be larger than 0x1FFFFFFFFFFFFF in P-Iris mode!\n");
                    return HI_ERR_ISP_ILLEGAL_PARAM;
                }
            }
        }
    }

    if ( u32Num <= 1 )
        return HI_SUCCESS;

    /* Second pass: validate ordering between consecutive nodes */
    for ( i = 0; i < u32Num - 1; i++ ) {
        HI_U64 exp0, exp1;
        HI_U32 changed;

        if ( irisType != 1 ) {
            /* DC-iris mode */
            HI_U32 intTime0 = pstRoute->astRouteNode[i].u32IntTime;
            HI_U32 sysGain0 = pstRoute->astRouteNode[i].u32SysGain;
            HI_U32 intTime1 = pstRoute->astRouteNode[i+1].u32IntTime;
            HI_U32 sysGain1 = pstRoute->astRouteNode[i+1].u32SysGain;

            exp0 = (HI_U64)intTime0 * sysGain0;
            exp1 = (HI_U64)intTime1 * sysGain1;

            if ( exp0 > exp1 ) {
                HI_TRACE_ISP(RE_DBG_LVL, "Error! The route node[%d]'s mutiply is larger than node[%d]!\n", i, i + 1);
                return HI_ERR_ISP_ILLEGAL_PARAM;
            }

            changed = 0;
            if ( sysGain0 != sysGain1 ) changed++;
            if ( intTime0 != intTime1 ) changed++;

            if ( exp0 != exp1 && changed > 1 ) {
                HI_TRACE_ISP(RE_DBG_LVL, "Error! The route node[%d]&node[%d]'s param is illegal!\n", i, i + 1);
                return HI_ERR_ISP_ILLEGAL_PARAM;
            }
        } else {
            /* P-iris mode */
            HI_U64 exp0_full, exp1_full;
            HI_U32 intTime0 = pstRoute->astRouteNode[i].u32IntTime;
            HI_U32 sysGain0 = pstRoute->astRouteNode[i].u32SysGain;
            HI_U32 intTime1 = pstRoute->astRouteNode[i+1].u32IntTime;
            HI_U32 sysGain1 = pstRoute->astRouteNode[i+1].u32SysGain;

            exp0 = (HI_U64)intTime0 * sysGain0;
            exp1 = (HI_U64)intTime1 * sysGain1;

            if ( bPiris ) {
                HI_U32 iris0 = pstRoute->astRouteNode[i].u32IrisFNOLin;
                HI_U32 iris1 = pstRoute->astRouteNode[i+1].u32IrisFNOLin;
                exp0_full = exp0 * iris0;
                exp1_full = exp1 * iris1;
            } else {
                HI_U32 iris0 = pstRoute->astRouteNode[i].enIrisFNO;
                HI_U32 iris1 = pstRoute->astRouteNode[i+1].enIrisFNO;
                HI_U32 shift0 = (HI_U32)1 << iris0;
                HI_U32 shift1 = (HI_U32)1 << iris1;
                exp0_full = (HI_U64)((HI_S64)exp0 * (HI_S32)shift0);
                exp1_full = (HI_U64)((HI_S64)exp1 * (HI_S32)shift1);
            }

            if ( exp0_full > exp1_full ) {
                HI_TRACE_ISP(RE_DBG_LVL, "Error! The route node[%d]'s mutiply is larger than node[%d]!\n", i, i + 1);
                return HI_ERR_ISP_ILLEGAL_PARAM;
            }

            changed = 0;
            if ( intTime0 != intTime1 ) changed++;
            if ( sysGain0 != sysGain1 ) changed++;

            if ( bPiris ) {
                if ( pstRoute->astRouteNode[i].u32IrisFNOLin != pstRoute->astRouteNode[i+1].u32IrisFNOLin )
                    changed++;
            } else {
                if ( pstRoute->astRouteNode[i].enIrisFNO != pstRoute->astRouteNode[i+1].enIrisFNO )
                    changed++;
            }

            if ( exp0_full != exp1_full && changed > 1 ) {
                HI_TRACE_ISP(RE_DBG_LVL, "Error! The route node[%d]&node[%d]'s param is illegal!\n", i, i + 1);
                return HI_ERR_ISP_ILLEGAL_PARAM;
            }
        }
    }

    return HI_SUCCESS;
}

// ============================================================================

HI_S32
HI_MPI_ISP_SetAERouteAttr(VI_PIPE ViPipe, const ISP_AE_ROUTE_S *pstRoute)
{
    HI_S32 result;
    HI_BOOL bMemInit;
    HI_CHAR strLibName[20];
    HI_U32 offset, i;

    if ( ViPipe > 3 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Err AE dev %d in %s!\n", ViPipe, __FUNCTION__);
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    if ( pstRoute == HI_NULL ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Null Pointer in %s!\n", __FUNCTION__);
        return HI_ERR_ISP_NULL_PTR;
    }

    if ( g_as32AeFd[ViPipe] <= 0 ) {
        g_as32AeFd[ViPipe] = open(ISP_DEV_NAME, O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO);
        if ( g_as32AeFd[ViPipe] < 0 ) {
            perror("open isp device error!\n");
            return HI_ERR_ISP_NOT_INIT;
        }

        result = ioctl(g_as32AeFd[ViPipe], ISP_DEV_SET_FD, &ViPipe) == 0;
        if ( result != HI_SUCCESS ) {
            close(g_as32AeFd[ViPipe]);
            g_as32AeFd[ViPipe] = -1;
            return HI_ERR_ISP_NOT_INIT;
        }
    }

    bMemInit = HI_FALSE;
    result = ioctl(g_as32AeFd[ViPipe], ISP_MEM_INFO_GET, &bMemInit);
    if ( result != HI_SUCCESS ) {
        HI_TRACE_ISP(RE_DBG_LVL, "ISP[%d] get Mem info failed!\n", ViPipe);
        return HI_ERR_ISP_MEM_NOT_INIT;
    }

    if ( !bMemInit ) {
        HI_TRACE_ISP(RE_DBG_LVL, "ISP[%d] Mem NOT Init %d!\n", ViPipe, bMemInit);
        return HI_ERR_ISP_MEM_NOT_INIT;
    }

    strncpy_s(strLibName, sizeof(strLibName), "hisi_ae_lib", 11);
    offset = (HI_U8)(IO_READ32((ViPipe << 17) + 0x100034) >> 8);

    result = ISP_RouteCheck(offset, pstRoute);
    if ( result != HI_SUCCESS )
        return result;

    /* Signal update start (write 0) */
    IO_WRITE16((offset << 13) + 0x700080, 0);

    /* Write u32TotalNum */
    IO_WRITE16((offset << 13) + 0x700082, (HI_U8)pstRoute->u32TotalNum);

    /* Write each route node */
    for ( i = 0; i < pstRoute->u32TotalNum; i++ ) {
        HI_U32 nodeBase = i * 12;
        IO_WRITE32(nodeBase + (offset << 13) + 0x700084, pstRoute->astRouteNode[i].u32IntTime);
        IO_WRITE32(nodeBase + (offset << 13) + 0x700088, pstRoute->astRouteNode[i].u32SysGain);
        IO_WRITE32(nodeBase + (offset << 13) + 0x70008C, pstRoute->astRouteNode[i].enIrisFNO);
        IO_WRITE16((offset << 13) + 0x700550 + i * 2, (HI_U16)pstRoute->astRouteNode[i].u32IrisFNOLin);
    }

    /* Signal update end (write 1) */
    IO_WRITE16((offset << 13) + 0x700080, 1);

    return result;
}

// ============================================================================

HI_S32
HI_MPI_ISP_SetIrisAttr(VI_PIPE ViPipe, const ISP_IRIS_ATTR_S *pstIrisAttr)
{
    HI_S32 result;
    HI_BOOL bMemInit;
    HI_CHAR strLibName[20];
    HI_U32 offset;

    if ( ViPipe > 3 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Err AE dev %d in %s!\n", ViPipe, __FUNCTION__);
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    if ( pstIrisAttr == HI_NULL ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Null Pointer in %s!\n", __FUNCTION__);
        return HI_ERR_ISP_NULL_PTR;
    }

    if ( g_as32AeFd[ViPipe] <= 0 ) {
        g_as32AeFd[ViPipe] = open(ISP_DEV_NAME, O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO);
        if ( g_as32AeFd[ViPipe] < 0 ) {
            perror("open isp device error!\n");
            return HI_ERR_ISP_NOT_INIT;
        }

        result = ioctl(g_as32AeFd[ViPipe], ISP_DEV_SET_FD, &ViPipe) == 0;
        if ( result != HI_SUCCESS ) {
            close(g_as32AeFd[ViPipe]);
            g_as32AeFd[ViPipe] = -1;
            return HI_ERR_ISP_NOT_INIT;
        }
    }

    bMemInit = HI_FALSE;
    result = ioctl(g_as32AeFd[ViPipe], ISP_MEM_INFO_GET, &bMemInit);
    if ( result != HI_SUCCESS ) {
        HI_TRACE_ISP(RE_DBG_LVL, "ISP[%d] get Mem info failed!\n", ViPipe);
        return HI_ERR_ISP_MEM_NOT_INIT;
    }

    if ( !bMemInit ) {
        HI_TRACE_ISP(RE_DBG_LVL, "ISP[%d] Mem NOT Init %d!\n", ViPipe, bMemInit);
        return HI_ERR_ISP_MEM_NOT_INIT;
    }

    strncpy_s(strLibName, sizeof(strLibName), "hisi_ae_lib", 11);
    offset = (HI_U8)(IO_READ32((ViPipe << 17) + 0x100034) >> 8);

    /* Validate bEnable */
    if ( pstIrisAttr->bEnable > 1 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Invalid AI bEnable input!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate enOpType */
    if ( pstIrisAttr->enOpType > 1 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Invalid AI type!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate enIrisType */
    if ( pstIrisAttr->enIrisType > 1 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Invalid IRIS type!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate enIrisStatus */
    if ( pstIrisAttr->enIrisStatus > 2 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Invalid AI status!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate stMIAttr.enIrisFNO */
    if ( pstIrisAttr->stMIAttr.enIrisFNO > 10 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Invalid F Number!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate stMIAttr.u32HoldValue */
    if ( pstIrisAttr->stMIAttr.u32HoldValue > 1000 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "HoldValue's range is [0,1000]!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Write iris attributes */
    IO_WRITE16((offset << 13) + 0x70016C, pstIrisAttr->bEnable);
    IO_WRITE16((offset << 13) + 0x70016E, pstIrisAttr->enOpType & 1);
    IO_WRITE16((offset << 13) + 0x700176, pstIrisAttr->enIrisType & 3);
    IO_WRITE8 ((offset << 13) + 0x70017C, pstIrisAttr->enIrisStatus & 3);
    IO_WRITE8 ((offset << 13) + 0x700199, (HI_U8)pstIrisAttr->stMIAttr.enIrisFNO);
    IO_WRITE32((offset << 13) + 0x700170, pstIrisAttr->stMIAttr.u32HoldValue);

    return HI_SUCCESS;
}

// ============================================================================

HI_S32
HI_MPI_ISP_SetDcirisAttr(VI_PIPE ViPipe, const ISP_DCIRIS_ATTR_S *pstDcirisAttr)
{
    HI_S32 result;
    HI_BOOL bMemInit;
    HI_CHAR strLibName[20];
    HI_U32 offset;

    if ( ViPipe > 3 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Err AE dev %d in %s!\n", ViPipe, __FUNCTION__);
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    if ( pstDcirisAttr == HI_NULL ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Null Pointer in %s!\n", __FUNCTION__);
        return HI_ERR_ISP_NULL_PTR;
    }

    if ( g_as32AeFd[ViPipe] <= 0 ) {
        g_as32AeFd[ViPipe] = open(ISP_DEV_NAME, O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO);
        if ( g_as32AeFd[ViPipe] < 0 ) {
            perror("open isp device error!\n");
            return HI_ERR_ISP_NOT_INIT;
        }

        result = ioctl(g_as32AeFd[ViPipe], ISP_DEV_SET_FD, &ViPipe) == 0;
        if ( result != HI_SUCCESS ) {
            close(g_as32AeFd[ViPipe]);
            g_as32AeFd[ViPipe] = -1;
            return HI_ERR_ISP_NOT_INIT;
        }
    }

    bMemInit = HI_FALSE;
    result = ioctl(g_as32AeFd[ViPipe], ISP_MEM_INFO_GET, &bMemInit);
    if ( result != HI_SUCCESS ) {
        HI_TRACE_ISP(RE_DBG_LVL, "ISP[%d] get Mem info failed!\n", ViPipe);
        return HI_ERR_ISP_MEM_NOT_INIT;
    }

    if ( !bMemInit ) {
        HI_TRACE_ISP(RE_DBG_LVL, "ISP[%d] Mem NOT Init %d!\n", ViPipe, bMemInit);
        return HI_ERR_ISP_MEM_NOT_INIT;
    }

    strncpy_s(strLibName, sizeof(strLibName), "hisi_ae_lib", 11);
    offset = (HI_U8)(IO_READ32((ViPipe << 17) + 0x100034) >> 8);

    /* Validate s32Kp */
    if ( (HI_U32)pstDcirisAttr->s32Kp > 100000 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "s32Kp's range must be [0, 100000]!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate s32Ki */
    if ( (HI_U32)pstDcirisAttr->s32Ki > 1000 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "s32Ki's range must be [0, 1000]!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate s32Kd */
    if ( (HI_U32)pstDcirisAttr->s32Kd > 100000 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "s32Kd's range must be [0, 100000]!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate u32MinPwmDuty */
    if ( pstDcirisAttr->u32MinPwmDuty > 1000 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "u32MinPwmDuty's range must be [0, 1000]!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate u32MaxPwmDuty */
    if ( pstDcirisAttr->u32MaxPwmDuty > 1000 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "u32MaxPwmDuty's range must be [0, 1000]!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate Min <= Max */
    if ( pstDcirisAttr->u32MinPwmDuty > pstDcirisAttr->u32MaxPwmDuty ) {
        HI_TRACE_ISP(RE_DBG_LVL, "u32MinPwmDuty should not larger than u32MaxPwmDuty!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate u32OpenPwmDuty */
    if ( pstDcirisAttr->u32OpenPwmDuty > 1000 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "u32OpenPwmDuty's range must be [0, 1000]!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate u32OpenPwmDuty in [Min, Max] */
    if ( pstDcirisAttr->u32MinPwmDuty > pstDcirisAttr->u32OpenPwmDuty ||
         pstDcirisAttr->u32MaxPwmDuty < pstDcirisAttr->u32OpenPwmDuty ) {
        HI_TRACE_ISP(RE_DBG_LVL, "u32OpenPwmDuty should not less than u32MinPwmDuty or larger than u32MaxPwmDuty!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Write DC-iris attributes */
    IO_WRITE32((offset << 13) + 0x7001A0, pstDcirisAttr->s32Kp);
    IO_WRITE32((offset << 13) + 0x7001A4, pstDcirisAttr->s32Ki);
    IO_WRITE32((offset << 13) + 0x7001A8, pstDcirisAttr->s32Kd);
    IO_WRITE32((offset << 13) + 0x7001AC, pstDcirisAttr->u32MinPwmDuty);
    IO_WRITE32((offset << 13) + 0x7001B0, pstDcirisAttr->u32MaxPwmDuty);
    IO_WRITE32((offset << 13) + 0x7001B4, pstDcirisAttr->u32OpenPwmDuty);

    return HI_SUCCESS;
}

// ============================================================================

HI_S32
HI_MPI_ISP_SetPirisAttr(VI_PIPE ViPipe, const ISP_PIRIS_ATTR_S *pstPirisAttr)
{
    HI_S32 result;
    HI_BOOL bMemInit;
    HI_CHAR strLibName[20];
    HI_U32 offset;
    HI_U16 i;

    if ( ViPipe > 3 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Err AE dev %d in %s!\n", ViPipe, __FUNCTION__);
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    if ( pstPirisAttr == HI_NULL ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Null Pointer in %s!\n", __FUNCTION__);
        return HI_ERR_ISP_NULL_PTR;
    }

    if ( g_as32AeFd[ViPipe] <= 0 ) {
        g_as32AeFd[ViPipe] = open(ISP_DEV_NAME, O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO);
        if ( g_as32AeFd[ViPipe] < 0 ) {
            perror("open isp device error!\n");
            return HI_ERR_ISP_NOT_INIT;
        }

        result = ioctl(g_as32AeFd[ViPipe], ISP_DEV_SET_FD, &ViPipe) == 0;
        if ( result != HI_SUCCESS ) {
            close(g_as32AeFd[ViPipe]);
            g_as32AeFd[ViPipe] = -1;
            return HI_ERR_ISP_NOT_INIT;
        }
    }

    bMemInit = HI_FALSE;
    result = ioctl(g_as32AeFd[ViPipe], ISP_MEM_INFO_GET, &bMemInit);
    if ( result != HI_SUCCESS ) {
        HI_TRACE_ISP(RE_DBG_LVL, "ISP[%d] get Mem info failed!\n", ViPipe);
        return HI_ERR_ISP_MEM_NOT_INIT;
    }

    if ( !bMemInit ) {
        HI_TRACE_ISP(RE_DBG_LVL, "ISP[%d] Mem NOT Init %d!\n", ViPipe, bMemInit);
        return HI_ERR_ISP_MEM_NOT_INIT;
    }

    strncpy_s(strLibName, sizeof(strLibName), "hisi_ae_lib", 11);
    offset = (HI_U8)(IO_READ32((ViPipe << 17) + 0x100034) >> 8);

    /* Validate enMaxIrisFNOTarget and enMinIrisFNOTarget */
    if ( pstPirisAttr->enMaxIrisFNOTarget > 10 || pstPirisAttr->enMinIrisFNOTarget > 10 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Invalid Iris FNO input!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate Max >= Min FNO */
    if ( pstPirisAttr->enMaxIrisFNOTarget < pstPirisAttr->enMinIrisFNOTarget ) {
        HI_TRACE_ISP(RE_DBG_LVL, "enMaxIrisFNO should not be less than enMinIrisFNO!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate bZeroIsMax */
    if ( pstPirisAttr->bZeroIsMax > 1 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Invalid bZeroIsMax!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate bStepFNOTableChange */
    if ( pstPirisAttr->bStepFNOTableChange > 1 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Invalid bStepFNOTableChange!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate u16TotalStep */
    if ( (pstPirisAttr->u16TotalStep - 1) >= 0x400 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Piris TotalStep's range must be [1, 1024]!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate u16StepCount */
    if ( (pstPirisAttr->u16StepCount - 1) >= 0x400 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Piris StepCount's range must be [1, 1024]!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate TotalStep >= StepCount */
    if ( pstPirisAttr->u16TotalStep < pstPirisAttr->u16StepCount ) {
        HI_TRACE_ISP(RE_DBG_LVL, "u16TotalStep should not less than u16StepCount!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate au16StepFNOTable entries and ordering */
    for ( i = 0; i < pstPirisAttr->u16StepCount; i++ ) {
        if ( pstPirisAttr->au16StepFNOTable[i] > 1024 ) {
            HI_TRACE_ISP(RE_DBG_LVL, "au16StepFNOTable's range must be [0, 1024]!\n");
            return HI_ERR_ISP_ILLEGAL_PARAM;
        }
        if ( i > 0 && pstPirisAttr->au16StepFNOTable[i] < pstPirisAttr->au16StepFNOTable[i - 1] ) {
            HI_TRACE_ISP(RE_DBG_LVL, "au16StepFNOTable[%d] should not larger than au16StepFNOTable[%d]!\n", i - 1, i);
            return HI_ERR_ISP_ILLEGAL_PARAM;
        }
    }

    /* Validate bFNOExValid */
    if ( pstPirisAttr->bFNOExValid > 1 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Invalid bFNOExValid input!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate u32MaxIrisFNOTarget */
    if ( (pstPirisAttr->u32MaxIrisFNOTarget - 1) >= 0x400 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "u32MaxIrisFNOTarget' range is [1, 1024]!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate u32MinIrisFNOTarget */
    if ( (pstPirisAttr->u32MinIrisFNOTarget - 1) >= 0x400 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "u32MinIrisFNOTarget' range is [1, 1024]!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Validate Max >= Min target */
    if ( pstPirisAttr->u32MaxIrisFNOTarget < pstPirisAttr->u32MinIrisFNOTarget ) {
        HI_TRACE_ISP(RE_DBG_LVL, "u32MaxIrisFNOTarget should not be less than u32MinIrisFNOTarget!\n");
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    /* Write P-iris attributes */
    IO_WRITE8 ((offset << 13) + 0x70019A, pstPirisAttr->enMaxIrisFNOTarget);
    IO_WRITE8 ((offset << 13) + 0x70019B, (HI_U8)pstPirisAttr->enMinIrisFNOTarget);
    IO_WRITE8 ((offset << 13) + 0x7001B9, pstPirisAttr->bZeroIsMax & 1);
    IO_WRITE8 ((offset << 13) + 0x7001B8, pstPirisAttr->bStepFNOTableChange & 1);
    IO_WRITE16((offset << 13) + 0x7001BA, pstPirisAttr->u16TotalStep);
    IO_WRITE16((offset << 13) + 0x7001BC, pstPirisAttr->u16StepCount);

    /* Write au16StepFNOTable */
    for ( i = 0; i < pstPirisAttr->u16StepCount; i++ ) {
        IO_WRITE16((offset << 13) + 0x700800 + i * 2, pstPirisAttr->au16StepFNOTable[i]);
    }

    /* Write bFNOExValid, u32MaxIrisFNOTarget, u32MinIrisFNOTarget */
    IO_WRITE8 ((offset << 13) + 0x70054C, pstPirisAttr->bFNOExValid & 1);
    IO_WRITE16((offset << 13) + 0x700548, (HI_U16)pstPirisAttr->u32MaxIrisFNOTarget);
    IO_WRITE16((offset << 13) + 0x70054A, (HI_U16)pstPirisAttr->u32MinIrisFNOTarget);

    return HI_SUCCESS;
}

// ============================================================================

static HI_S32
ISP_RouteExCheck(HI_U32 offset, const ISP_AE_ROUTE_EX_S *pstRouteEx)
{
    HI_U32 i, u32Num;
    HI_U32 irisType, bPiris;

    if ( pstRouteEx == HI_NULL ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Null Pointer in %s!\n", __FUNCTION__);
        return HI_ERR_ISP_NULL_PTR;
    }

    irisType = IO_READ16((offset + 0x700) << 12 | 0x176) & 3;
    bPiris   = IO_READ8 ((offset + 0x700) << 12 | 0x54C) & 1;

    u32Num = pstRouteEx->u32TotalNum;

    if ( u32Num > ISP_AE_ROUTE_EX_MAX_NODES ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Error! The route_ex node number %d is larger than 16!\n", u32Num);
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    if ( u32Num == 0 )
        return HI_SUCCESS;

    /* First pass: validate each node */
    for ( i = 0; i < u32Num; i++ ) {
        HI_U32 intTime  = pstRouteEx->astRouteExNode[i].u32IntTime;
        HI_U32 again    = pstRouteEx->astRouteExNode[i].u32Again;
        HI_U32 dgain    = pstRouteEx->astRouteExNode[i].u32Dgain;
        HI_U32 ispDgain = pstRouteEx->astRouteExNode[i].u32IspDgain;

        if ( irisType != 1 ) {
            /* DC-iris mode */
            if ( intTime == 0 ) {
                HI_TRACE_ISP(RE_DBG_LVL, "Error! The route_ex node's IntTime should not be less than 1!\n");
                return HI_ERR_ISP_ILLEGAL_PARAM;
            }
            if ( again < 0x400 ) {
                HI_TRACE_ISP(RE_DBG_LVL, "Error! The route_ex node's Again should not be less than 0x400!\n");
                return HI_ERR_ISP_ILLEGAL_PARAM;
            }
            if ( again > 0x3FFFFF ) {
                HI_TRACE_ISP(RE_DBG_LVL, "Error! The route_ex node's Again should not be larger than 0x3FFFFF!\n");
                return HI_ERR_ISP_ILLEGAL_PARAM;
            }
            if ( dgain < 0x400 ) {
                HI_TRACE_ISP(RE_DBG_LVL, "Error! The route_ex node's Dgain should not be less than 0x400!\n");
                return HI_ERR_ISP_ILLEGAL_PARAM;
            }
            if ( dgain > 0x3FFFFF ) {
                HI_TRACE_ISP(RE_DBG_LVL, "Error! The route_ex node's Dgain should not be larger than 0x3FFFFF!\n");
                return HI_ERR_ISP_ILLEGAL_PARAM;
            }
            if ( ispDgain < 0x400 ) {
                HI_TRACE_ISP(RE_DBG_LVL, "Error! The route_ex node's IspDgain should not be less than 0x400!\n");
                return HI_ERR_ISP_ILLEGAL_PARAM;
            }
            if ( ispDgain > 0x40000 ) {
                HI_TRACE_ISP(RE_DBG_LVL, "Error! The route_ex node's IspDgain should not be larger than 0x40000!\n");
                return HI_ERR_ISP_ILLEGAL_PARAM;
            }
            {
                /* Compute equivalent sysGain = (again * dgain * ispDgain) >> 20 and check */
                HI_U64 adGain = (HI_U64)again * dgain;
                HI_U64 sysGain64 = (adGain * ispDgain) >> 20;
                if ( sysGain64 > 0xFFFFFFFFULL ) {
                    HI_TRACE_ISP(RE_DBG_LVL, "Error! The route_ex node's equivalent SysGain should not be larger than 0xFFFFFFFF!\n");
                    return HI_ERR_ISP_ILLEGAL_PARAM;
                }
                {
                    HI_U64 exposure = (HI_U64)intTime * sysGain64;
                    if ( (HI_S64)exposure < 0 ) {
                        HI_TRACE_ISP(RE_DBG_LVL, "Error! The route_ex node's Exposure should not be larger than 0x7FFFFFFFFFFFFFFF in DC-Iris mode!\n");
                        return HI_ERR_ISP_ILLEGAL_PARAM;
                    }
                }
            }
        } else {
            /* P-iris mode */
            if ( intTime == 0 ) {
                HI_TRACE_ISP(RE_DBG_LVL, "Error! The route_ex node's IntTime should not be less than 1!\n");
                return HI_ERR_ISP_ILLEGAL_PARAM;
            }
            if ( again < 0x400 ) {
                HI_TRACE_ISP(RE_DBG_LVL, "Error! The route_ex node's Again should not be less than 0x400!\n");
                return HI_ERR_ISP_ILLEGAL_PARAM;
            }
            if ( again > 0x3FFFFF ) {
                HI_TRACE_ISP(RE_DBG_LVL, "Error! The route_ex node's Again should not be larger than 0x3FFFFF!\n");
                return HI_ERR_ISP_ILLEGAL_PARAM;
            }
            if ( dgain < 0x400 ) {
                HI_TRACE_ISP(RE_DBG_LVL, "Error! The route_ex node's Dgain should not be less than 0x400!\n");
                return HI_ERR_ISP_ILLEGAL_PARAM;
            }
            if ( dgain > 0x3FFFFF ) {
                HI_TRACE_ISP(RE_DBG_LVL, "Error! The route_ex node's Dgain should not be larger than 0x3FFFFF!\n");
                return HI_ERR_ISP_ILLEGAL_PARAM;
            }
            if ( ispDgain < 0x400 ) {
                HI_TRACE_ISP(RE_DBG_LVL, "Error! The route_ex node's IspDgain should not be less than 0x400!\n");
                return HI_ERR_ISP_ILLEGAL_PARAM;
            }
            if ( ispDgain > 0x40000 ) {
                HI_TRACE_ISP(RE_DBG_LVL, "Error! The route_ex node's IspDgain should not be larger than 0x40000!\n");
                return HI_ERR_ISP_ILLEGAL_PARAM;
            }
            if ( pstRouteEx->astRouteExNode[i].enIrisFNO > 10 ) {
                HI_TRACE_ISP(RE_DBG_LVL, "Error! The route_ex node's IrisFNO should not be larger than ISP_IRIS_F_NO_1_0!\n");
                return HI_ERR_ISP_ILLEGAL_PARAM;
            }
            if ( bPiris ) {
                if ( (pstRouteEx->astRouteExNode[i].u32IrisFNOLin - 1) >= 0x400 ) {
                    HI_TRACE_ISP(RE_DBG_LVL, "Error! The route_ex node's IrisFNOLin range is [1, 1024]!\n");
                    return HI_ERR_ISP_ILLEGAL_PARAM;
                }
            }
            {
                HI_U64 adGain = (HI_U64)again * dgain;
                HI_U64 sysGain64 = (adGain * ispDgain) >> 20;
                if ( sysGain64 > 0xFFFFFFFFULL ) {
                    HI_TRACE_ISP(RE_DBG_LVL, "Error! The route_ex node's equivalent SysGain should not be larger than 0xFFFFFFFF!\n");
                    return HI_ERR_ISP_ILLEGAL_PARAM;
                }
                {
                    HI_U64 exposure = (HI_U64)intTime * sysGain64;
                    if ( exposure > 0x1FFFFFFFFFFFFFULL ) {
                        HI_TRACE_ISP(RE_DBG_LVL, "Error! The route_ex node's Exposure(IntTime*SysGain) should not be larger than 0x1FFFFFFFFFFFFF in P-Iris mode!\n");
                        return HI_ERR_ISP_ILLEGAL_PARAM;
                    }
                }
            }
        }
    }

    if ( u32Num <= 1 )
        return HI_SUCCESS;

    /* Second pass: validate ordering */
    for ( i = 0; i < u32Num - 1; i++ ) {
        HI_U64 sysGain0, sysGain1, exp0, exp1;
        HI_U32 changed;

        {
            HI_U64 ad0 = (HI_U64)pstRouteEx->astRouteExNode[i].u32Again * pstRouteEx->astRouteExNode[i].u32Dgain;
            sysGain0 = (ad0 * pstRouteEx->astRouteExNode[i].u32IspDgain) >> 20;
        }
        {
            HI_U64 ad1 = (HI_U64)pstRouteEx->astRouteExNode[i+1].u32Again * pstRouteEx->astRouteExNode[i+1].u32Dgain;
            sysGain1 = (ad1 * pstRouteEx->astRouteExNode[i+1].u32IspDgain) >> 20;
        }

        if ( irisType != 1 ) {
            /* DC-iris mode */
            exp0 = (HI_U64)pstRouteEx->astRouteExNode[i].u32IntTime * sysGain0;
            exp1 = (HI_U64)pstRouteEx->astRouteExNode[i+1].u32IntTime * sysGain1;

            if ( exp0 > exp1 ) {
                HI_TRACE_ISP(RE_DBG_LVL, "Error! The route_ex node[%d]'s mutiply is larger than node[%d]!\n", i, i + 1);
                return HI_ERR_ISP_ILLEGAL_PARAM;
            }

            changed = 0;
            if ( pstRouteEx->astRouteExNode[i].u32IntTime != pstRouteEx->astRouteExNode[i+1].u32IntTime ) changed++;
            if ( pstRouteEx->astRouteExNode[i].u32Again   != pstRouteEx->astRouteExNode[i+1].u32Again )   changed++;
            if ( pstRouteEx->astRouteExNode[i].u32Dgain   != pstRouteEx->astRouteExNode[i+1].u32Dgain )   changed++;
            if ( pstRouteEx->astRouteExNode[i].u32IspDgain != pstRouteEx->astRouteExNode[i+1].u32IspDgain ) changed++;

            if ( exp0 != exp1 && changed > 1 ) {
                HI_TRACE_ISP(RE_DBG_LVL, "Error! The route_ex node[%d]&node[%d]'s param is illegal!\n", i, i + 1);
                return HI_ERR_ISP_ILLEGAL_PARAM;
            }
        } else {
            /* P-iris mode */
            HI_U64 exp0_full, exp1_full;

            exp0 = (HI_U64)pstRouteEx->astRouteExNode[i].u32IntTime * sysGain0;
            exp1 = (HI_U64)pstRouteEx->astRouteExNode[i+1].u32IntTime * sysGain1;

            if ( bPiris ) {
                exp0_full = exp0 * pstRouteEx->astRouteExNode[i].u32IrisFNOLin;
                exp1_full = exp1 * pstRouteEx->astRouteExNode[i+1].u32IrisFNOLin;
            } else {
                HI_U32 shift0 = (HI_U32)1 << pstRouteEx->astRouteExNode[i].enIrisFNO;
                HI_U32 shift1 = (HI_U32)1 << pstRouteEx->astRouteExNode[i+1].enIrisFNO;
                exp0_full = (HI_U64)((HI_S64)exp0 * (HI_S32)shift0);
                exp1_full = (HI_U64)((HI_S64)exp1 * (HI_S32)shift1);
            }

            if ( exp0_full > exp1_full ) {
                HI_TRACE_ISP(RE_DBG_LVL, "Error! The route_ex node[%d]'s mutiply is larger than node[%d]!\n", i, i + 1);
                return HI_ERR_ISP_ILLEGAL_PARAM;
            }

            changed = 0;
            if ( pstRouteEx->astRouteExNode[i].u32IntTime != pstRouteEx->astRouteExNode[i+1].u32IntTime ) changed++;
            if ( pstRouteEx->astRouteExNode[i].u32Again   != pstRouteEx->astRouteExNode[i+1].u32Again )   changed++;
            if ( pstRouteEx->astRouteExNode[i].u32Dgain   != pstRouteEx->astRouteExNode[i+1].u32Dgain )   changed++;
            if ( pstRouteEx->astRouteExNode[i].u32IspDgain != pstRouteEx->astRouteExNode[i+1].u32IspDgain ) changed++;

            if ( bPiris ) {
                if ( pstRouteEx->astRouteExNode[i].u32IrisFNOLin != pstRouteEx->astRouteExNode[i+1].u32IrisFNOLin )
                    changed++;
            } else {
                if ( pstRouteEx->astRouteExNode[i].enIrisFNO != pstRouteEx->astRouteExNode[i+1].enIrisFNO )
                    changed++;
            }

            if ( exp0_full != exp1_full && changed > 1 ) {
                HI_TRACE_ISP(RE_DBG_LVL, "Error! The route_ex node[%d]&node[%d]'s param is illegal!\n", i, i + 1);
                return HI_ERR_ISP_ILLEGAL_PARAM;
            }
        }
    }

    return HI_SUCCESS;
}

// ============================================================================

HI_S32
HI_MPI_ISP_SetAERouteAttrEx(VI_PIPE ViPipe, const ISP_AE_ROUTE_EX_S *pstRouteEx)
{
    HI_S32 result;
    HI_BOOL bMemInit;
    HI_CHAR strLibName[20];
    HI_U32 offset;
    HI_U8 i;

    if ( ViPipe > 3 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Err AE dev %d in %s!\n", ViPipe, __FUNCTION__);
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    if ( pstRouteEx == HI_NULL ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Null Pointer in %s!\n", __FUNCTION__);
        return HI_ERR_ISP_NULL_PTR;
    }

    if ( g_as32AeFd[ViPipe] <= 0 ) {
        g_as32AeFd[ViPipe] = open(ISP_DEV_NAME, O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO);
        if ( g_as32AeFd[ViPipe] < 0 ) {
            perror("open isp device error!\n");
            return HI_ERR_ISP_NOT_INIT;
        }

        result = ioctl(g_as32AeFd[ViPipe], ISP_DEV_SET_FD, &ViPipe) == 0;
        if ( result != HI_SUCCESS ) {
            close(g_as32AeFd[ViPipe]);
            g_as32AeFd[ViPipe] = -1;
            return HI_ERR_ISP_NOT_INIT;
        }
    }

    bMemInit = HI_FALSE;
    result = ioctl(g_as32AeFd[ViPipe], ISP_MEM_INFO_GET, &bMemInit);
    if ( result != HI_SUCCESS ) {
        HI_TRACE_ISP(RE_DBG_LVL, "ISP[%d] get Mem info failed!\n", ViPipe);
        return HI_ERR_ISP_MEM_NOT_INIT;
    }

    if ( !bMemInit ) {
        HI_TRACE_ISP(RE_DBG_LVL, "ISP[%d] Mem NOT Init %d!\n", ViPipe, bMemInit);
        return HI_ERR_ISP_MEM_NOT_INIT;
    }

    strncpy_s(strLibName, sizeof(strLibName), "hisi_ae_lib", 11);
    offset = (HI_U8)(IO_READ32((ViPipe << 17) + 0x100034) >> 8);

    result = ISP_RouteExCheck(offset, pstRouteEx);
    if ( result != HI_SUCCESS )
        return result;

    /* Signal update start (write 0) */
    IO_WRITE8((offset << 13) + 0x7003D5, 0);

    /* Write u32TotalNum */
    IO_WRITE16((offset << 13) + 0x70028E, (HI_U8)pstRouteEx->u32TotalNum);

    /* Write each route_ex node */
    for ( i = 0; i < pstRouteEx->u32TotalNum; i++ ) {
        HI_U32 nodeBase = i * 20;
        HI_U32 nodeOff  = i * 24;
        IO_WRITE32(nodeBase + (offset << 13) + 0x700294, pstRouteEx->astRouteExNode[i].u32IntTime);
        IO_WRITE32(nodeBase + (offset << 13) + 0x700298, pstRouteEx->astRouteExNode[i].u32Again);
        IO_WRITE32(nodeBase + (offset << 13) + 0x70029C, pstRouteEx->astRouteExNode[i].u32Dgain);
        IO_WRITE32(nodeBase + (offset << 13) + 0x7002A0, pstRouteEx->astRouteExNode[i].u32IspDgain);
        IO_WRITE32(nodeBase + (offset << 13) + 0x7002A4, pstRouteEx->astRouteExNode[i].enIrisFNO);
        IO_WRITE16((offset << 13) + 0x700570 + i * 2, (HI_U16)pstRouteEx->astRouteExNode[i].u32IrisFNOLin);
    }

    /* Signal update end (write 1) */
    IO_WRITE8((offset << 13) + 0x7003D5, 1);

    return result;
}

HI_S32
HI_MPI_ISP_GetExposureAttr(VI_PIPE ViPipe, ISP_EXPOSURE_ATTR_S *pstExpAttr)
{
    HI_S32 result;
    HI_BOOL bMemInit;
    HI_CHAR strLibName[20];
    HI_U32 offset, data;

    if ( ViPipe > 3 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Err AE dev %d in %s!\n", ViPipe, __FUNCTION__);
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    if ( pstExpAttr == HI_NULL ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Null Pointer in %s!\n", __FUNCTION__);
        return HI_ERR_ISP_NULL_PTR;
    }

    if ( g_as32AeFd[ViPipe] <= 0 ) {
        g_as32AeFd[ViPipe] = open(ISP_DEV_NAME, O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO);
        if ( g_as32AeFd[ViPipe] < 0 ) {
            perror("open isp device error!\n");
            return HI_ERR_ISP_NOT_INIT;
        }

        result = ioctl(g_as32AeFd[ViPipe], ISP_DEV_SET_FD, &ViPipe) == 0;
        if ( result != HI_SUCCESS ) {
            close(g_as32AeFd[ViPipe]);
            g_as32AeFd[ViPipe] = -1;
            return HI_ERR_ISP_NOT_INIT;
        }
    }

    bMemInit = HI_FALSE;
    result = ioctl(g_as32AeFd[ViPipe], ISP_MEM_INFO_GET, &bMemInit);
    if ( result != HI_SUCCESS ) {
        HI_TRACE_ISP(RE_DBG_LVL, "ISP[%d] get Mem info failed!\n", ViPipe);
        return HI_ERR_ISP_MEM_NOT_INIT;
    }

    if ( !bMemInit ) {
        HI_TRACE_ISP(RE_DBG_LVL, "ISP[%d] Mem NOT Init %d!\n", ViPipe, bMemInit);
        return HI_ERR_ISP_MEM_NOT_INIT;
    }

    strncpy_s(strLibName, sizeof(strLibName), "hisi_ae_lib", 11);
    offset = (HI_U8)(IO_READ32((ViPipe << 17) + 0x100034) >> 8);

    pstExpAttr->bByPass                       =  IO_READ8 ((offset << 13) + 0x700001) & 1;
    pstExpAttr->bHistStatAdjust               =  IO_READ16((offset << 13) + 0x700156) & 1;
    pstExpAttr->bAERouteExValid               =  IO_READ8 ((offset << 13) + 0x7003D4) & 1;
    pstExpAttr->bAEGainSepCfg                 =  IO_READ8 ((offset << 13) + 0x70062D) & 1;
    pstExpAttr->enOpType                      = (IO_READ16((offset << 13) + 0x700056) & 1) != 0;
    pstExpAttr->u8AERunInterval               =  IO_READ8 ((offset << 13) + 0x700198);
    pstExpAttr->stAuto.stExpTimeRange.u32Max  =  IO_READ32((offset << 13) + 0x700010);
    pstExpAttr->stAuto.stExpTimeRange.u32Min  =  IO_READ32((offset << 13) + 0x700014);
    pstExpAttr->stAuto.stAGainRange.u32Max    =  IO_READ32((offset << 13) + 0x700018);
    pstExpAttr->stAuto.stAGainRange.u32Min    =  IO_READ32((offset << 13) + 0x70001C);
    pstExpAttr->stAuto.stDGainRange.u32Max    =  IO_READ32((offset << 13) + 0x700020);
    pstExpAttr->stAuto.stDGainRange.u32Min    =  IO_READ32((offset << 13) + 0x700024);
    pstExpAttr->stAuto.stISPDGainRange.u32Max =  IO_READ32((offset << 13) + 0x700028);
    pstExpAttr->stAuto.stISPDGainRange.u32Min =  IO_READ32((offset << 13) + 0x70002C);
    pstExpAttr->stAuto.stSysGainRange.u32Max  =  IO_READ32((offset << 13) + 0x700030);
    pstExpAttr->stAuto.stSysGainRange.u32Min  =  IO_READ32((offset << 13) + 0x700034);
    pstExpAttr->stAuto.u32GainThreshold       =  IO_READ32((offset << 13) + 0x700038);
    pstExpAttr->stAuto.u8Speed                =  IO_READ8 ((offset << 13) + 0x70000B);
    pstExpAttr->stAuto.u16BlackSpeedBias      =  IO_READ16((offset << 13) + 0x70052E);
    pstExpAttr->stAuto.u8Tolerance            =  IO_READ8 ((offset << 13) + 0x70000C);
    pstExpAttr->stAuto.u8Compensation         =  IO_READ8 ((offset << 13) + 0x70000D);
    pstExpAttr->stAuto.u16EVBias              =  IO_READ16((offset << 13) + 0x700158);

    data = IO_READ8((offset << 13) + 0x700002);
    if ( data >= AE_MODE_BUTT )
        data = AE_MODE_BUTT;
    pstExpAttr->stAuto.enAEMode = data;

    data = IO_READ8((offset << 13) + 0x70015A) & 3;
    if ( data >= AE_STRATEGY_MODE_BUTT )
        data = AE_STRATEGY_MODE_BUTT;
    pstExpAttr->stAuto.enAEStrategyMode = data;

    pstExpAttr->stAuto.u16HistRatioSlope         =  IO_READ16((offset << 13) + 0x70015C);
    pstExpAttr->stAuto.u8MaxHistOffset           =  IO_READ8 ((offset << 13) + 0x70015B);
    pstExpAttr->stAuto.stAntiflicker.bEnable     = (IO_READ8 ((offset << 13) + 0x700009) >> 4) & 1;
    pstExpAttr->stAuto.stAntiflicker.u8Frequency =  IO_READ8 ((offset << 13) + 0x700008);

    data = IO_READ8((offset << 13) + 0x700009) & 3;
    if ( data >= ISP_ANTIFLICKER_MODE_BUTT )
        data = ISP_ANTIFLICKER_MODE_BUTT;
    pstExpAttr->stAuto.stAntiflicker.enMode = data;

    pstExpAttr->stAuto.stSubflicker.bEnable             = IO_READ8 ((offset << 13) + 0x70019C) & 1;
    pstExpAttr->stAuto.stSubflicker.u8LumaDiff          = IO_READ8 ((offset << 13) + 0x70019D);
    pstExpAttr->stAuto.stAEDelayAttr.u16BlackDelayFrame = IO_READ16((offset << 13) + 0x70003C);
    pstExpAttr->stAuto.stAEDelayAttr.u16WhiteDelayFrame = IO_READ16((offset << 13) + 0x70003E);
    pstExpAttr->stAuto.bManualExpValue                  = IO_READ8 ((offset + 0x380) << 13) & 1;
    pstExpAttr->stAuto.u32ExpValue                      = IO_READ32((offset << 13) + 0x7001C8);

    data = IO_READ8((offset << 13) + 0x70062C);
    if ( data == LONG_FRAME || data == SHORT_FRAME )
         pstExpAttr->enPriorFrame = data;
    else pstExpAttr->enPriorFrame = PRIOR_FRAME_BUTT;

    data = IO_READ8((offset << 13) + 0x700526) & 3;
    if (data == ISP_FSWDR_NORMAL_MODE     ||
        data == ISP_FSWDR_LONG_FRAME_MODE ||
        data == ISP_FSWDR_AUTO_LONG_FRAME_MODE)
         pstExpAttr->stAuto.enFSWDRMode   = data;
    else pstExpAttr->stAuto.enFSWDRMode   = ISP_FSWDR_MODE_BUTT;

    pstExpAttr->stAuto.bWDRQuick          =  IO_READ8 ((offset << 13) + 0x700527) & 1;
    pstExpAttr->stAuto.u16ISOCalCoef      =  IO_READ16((offset << 13) + 0x700716);
    pstExpAttr->stManual.enExpTimeOpType  = (IO_READ16((offset << 13) + 0x700056) >> 3) & 1;
    pstExpAttr->stManual.enAGainOpType    = (IO_READ16((offset << 13) + 0x700056) >> 9) & 1;
    pstExpAttr->stManual.enDGainOpType    = (IO_READ16((offset << 13) + 0x700056) >> 10) & 1;
    pstExpAttr->stManual.enISPDGainOpType = (IO_READ16((offset << 13) + 0x700056) >> 11) & 1;
    pstExpAttr->stManual.u32ExpTime       =  IO_READ32((offset << 13) + 0x70005C);
    pstExpAttr->stManual.u32AGain         =  IO_READ32((offset << 13) + 0x700060);
    pstExpAttr->stManual.u32DGain         =  IO_READ32((offset << 13) + 0x700064);
    pstExpAttr->stManual.u32ISPDGain      =  IO_READ32((offset << 13) + 0x700068);

    return HI_SUCCESS;
}


HI_S32
HI_MPI_ISP_GetWDRExposureAttr(VI_PIPE ViPipe, ISP_WDR_EXPOSURE_ATTR_S *pstWDRExpAttr)
{
    HI_S32 result;
    HI_BOOL bMemInit;
    HI_U32 offset;

    if ( ViPipe > 3 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Err AE dev %d in %s!\n", ViPipe, __FUNCTION__);
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    if ( pstWDRExpAttr == HI_NULL ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Null Pointer in %s!\n", __FUNCTION__);
        return HI_ERR_ISP_NULL_PTR;
    }

    if ( g_as32AeFd[ViPipe] <= 0 ) {
        g_as32AeFd[ViPipe] = open(ISP_DEV_NAME, O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO);
        if ( g_as32AeFd[ViPipe] < 0 ) {
            perror("open isp device error!\n");
            return HI_ERR_ISP_NOT_INIT;
        }
        result = ioctl(g_as32AeFd[ViPipe], ISP_DEV_SET_FD, &ViPipe) == 0;
        if ( result != HI_SUCCESS ) {
            close(g_as32AeFd[ViPipe]);
            g_as32AeFd[ViPipe] = -1;
            return HI_ERR_ISP_NOT_INIT;
        }
    }

    bMemInit = HI_FALSE;
    result = ioctl(g_as32AeFd[ViPipe], ISP_MEM_INFO_GET, &bMemInit);
    if ( result != HI_SUCCESS ) {
        HI_TRACE_ISP(RE_DBG_LVL, "ISP[%d] get Mem info failed!\n", ViPipe);
        return HI_ERR_ISP_MEM_NOT_INIT;
    }
    if ( !bMemInit ) {
        HI_TRACE_ISP(RE_DBG_LVL, "ISP[%d] Mem NOT Init %d!\n", ViPipe, bMemInit);
        return HI_ERR_ISP_MEM_NOT_INIT;
    }

    offset = (HI_U8)(IO_READ32((ViPipe << 17) + 0x100034) >> 8);

    pstWDRExpAttr->enExpRatioType    = IO_READ8((offset << 13) + 0x700003) & 1;
    pstWDRExpAttr->au32ExpRatio[0]   = IO_READ16((offset << 13) + 0x700004) & 0xFFF;
    pstWDRExpAttr->au32ExpRatio[1]   = IO_READ16((offset << 13) + 0x70051A) & 0xFFF;
    pstWDRExpAttr->au32ExpRatio[2]   = IO_READ16((offset << 13) + 0x70051C) & 0xFFF;
    pstWDRExpAttr->u32ExpRatioMax    = IO_READ16((offset << 13) + 0x70019E);
    pstWDRExpAttr->u32ExpRatioMin    = IO_READ16((offset << 13) + 0x70051E);
    pstWDRExpAttr->u16Tolerance      = (HI_U16)(IO_READ16((offset << 13) + 0x700520) & 0xFF);
    pstWDRExpAttr->u16Speed          = (HI_U16)(IO_READ16((offset << 13) + 0x700522) & 0xFF);
    pstWDRExpAttr->u16RatioBias      = IO_READ16((offset << 13) + 0x700524);

    return HI_SUCCESS;
}


HI_S32
HI_MPI_ISP_GetHDRExposureAttr(VI_PIPE ViPipe, ISP_HDR_EXPOSURE_ATTR_S *pstHDRExpAttr)
{
    HI_S32 result;
    HI_BOOL bMemInit;
    HI_U32 offset;

    if ( ViPipe > 3 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Err AE dev %d in %s!\n", ViPipe, __FUNCTION__);
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }
    if ( pstHDRExpAttr == HI_NULL ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Null Pointer in %s!\n", __FUNCTION__);
        return HI_ERR_ISP_NULL_PTR;
    }

    if ( g_as32AeFd[ViPipe] <= 0 ) {
        g_as32AeFd[ViPipe] = open(ISP_DEV_NAME, O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO);
        if ( g_as32AeFd[ViPipe] < 0 ) {
            perror("open isp device error!\n");
            return HI_ERR_ISP_NOT_INIT;
        }
        result = ioctl(g_as32AeFd[ViPipe], ISP_DEV_SET_FD, &ViPipe) == 0;
        if ( result != HI_SUCCESS ) {
            close(g_as32AeFd[ViPipe]);
            g_as32AeFd[ViPipe] = -1;
            return HI_ERR_ISP_NOT_INIT;
        }
    }

    bMemInit = HI_FALSE;
    result = ioctl(g_as32AeFd[ViPipe], ISP_MEM_INFO_GET, &bMemInit);
    if ( result != HI_SUCCESS ) {
        HI_TRACE_ISP(RE_DBG_LVL, "ISP[%d] get Mem info failed!\n", ViPipe);
        return HI_ERR_ISP_MEM_NOT_INIT;
    }
    if ( !bMemInit ) {
        HI_TRACE_ISP(RE_DBG_LVL, "ISP[%d] Mem NOT Init %d!\n", ViPipe, bMemInit);
        return HI_ERR_ISP_MEM_NOT_INIT;
    }

    offset = (HI_U8)(IO_READ32((ViPipe << 17) + 0x100034) >> 8);

    pstHDRExpAttr->enExpHDRLvType    = IO_READ8 ((offset << 13) + 0x700616);
    pstHDRExpAttr->u32ExpHDRLv       = IO_READ32((offset << 13) + 0x700618);
    pstHDRExpAttr->u32ExpHDRLvMax    = IO_READ32((offset << 13) + 0x70061C);
    pstHDRExpAttr->u32ExpHDRLvMin    = IO_READ32((offset << 13) + 0x700620);
    pstHDRExpAttr->u32ExpHDRLvWeight = IO_READ32((offset << 13) + 0x700624);

    return HI_SUCCESS;
}


HI_S32
HI_MPI_ISP_GetAERouteAttr(VI_PIPE ViPipe, ISP_AE_ROUTE_S *pstAERouteAttr)
{
    HI_S32 result;
    HI_BOOL bMemInit;
    HI_U32 offset, i;

    if ( ViPipe > 3 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Err AE dev %d in %s!\n", ViPipe, __FUNCTION__);
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }
    if ( pstAERouteAttr == HI_NULL ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Null Pointer in %s!\n", __FUNCTION__);
        return HI_ERR_ISP_NULL_PTR;
    }

    if ( g_as32AeFd[ViPipe] <= 0 ) {
        g_as32AeFd[ViPipe] = open(ISP_DEV_NAME, O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO);
        if ( g_as32AeFd[ViPipe] < 0 ) {
            perror("open isp device error!\n");
            return HI_ERR_ISP_NOT_INIT;
        }
        result = ioctl(g_as32AeFd[ViPipe], ISP_DEV_SET_FD, &ViPipe) == 0;
        if ( result != HI_SUCCESS ) {
            close(g_as32AeFd[ViPipe]);
            g_as32AeFd[ViPipe] = -1;
            return HI_ERR_ISP_NOT_INIT;
        }
    }

    bMemInit = HI_FALSE;
    result = ioctl(g_as32AeFd[ViPipe], ISP_MEM_INFO_GET, &bMemInit);
    if ( result != HI_SUCCESS ) {
        HI_TRACE_ISP(RE_DBG_LVL, "ISP[%d] get Mem info failed!\n", ViPipe);
        return HI_ERR_ISP_MEM_NOT_INIT;
    }
    if ( !bMemInit ) {
        HI_TRACE_ISP(RE_DBG_LVL, "ISP[%d] Mem NOT Init %d!\n", ViPipe, bMemInit);
        return HI_ERR_ISP_MEM_NOT_INIT;
    }

    offset = (HI_U8)(IO_READ32((ViPipe << 17) + 0x100034) >> 8);

    memset_s(pstAERouteAttr, sizeof(ISP_AE_ROUTE_S), 0, sizeof(ISP_AE_ROUTE_S));
    pstAERouteAttr->u32TotalNum = (HI_U8)IO_READ16((offset << 13) + 0x700082);

    for ( i = 0; i < pstAERouteAttr->u32TotalNum; i++ ) {
        HI_U32 nodeBase = i * 12;
        pstAERouteAttr->astRouteNode[i].u32IntTime    = IO_READ32(nodeBase + (offset << 13) + 0x700084);
        pstAERouteAttr->astRouteNode[i].u32SysGain    = IO_READ32(nodeBase + (offset << 13) + 0x700088);
        pstAERouteAttr->astRouteNode[i].enIrisFNO     = IO_READ32(nodeBase + (offset << 13) + 0x70008C);
        pstAERouteAttr->astRouteNode[i].u32IrisFNOLin = IO_READ16((offset << 13) + 0x700550 + (i << 1));
    }

    return HI_SUCCESS;
}


HI_S32
HI_MPI_ISP_QueryExposureInfo(VI_PIPE ViPipe, ISP_EXP_INFO_S *pstExpInfo)
{
    HI_S32 result;
    HI_BOOL bMemInit;
    HI_U32 offset;
    HI_U32 i;

    if ( ViPipe > 3 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Err AE dev %d in %s!\n", ViPipe, __FUNCTION__);
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }
    if ( pstExpInfo == HI_NULL ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Null Pointer in %s!\n", __FUNCTION__);
        return HI_ERR_ISP_NULL_PTR;
    }

    if ( g_as32AeFd[ViPipe] <= 0 ) {
        g_as32AeFd[ViPipe] = open(ISP_DEV_NAME, O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO);
        if ( g_as32AeFd[ViPipe] < 0 ) {
            perror("open isp device error!\n");
            return HI_ERR_ISP_NOT_INIT;
        }
        result = ioctl(g_as32AeFd[ViPipe], ISP_DEV_SET_FD, &ViPipe) == 0;
        if ( result != HI_SUCCESS ) {
            close(g_as32AeFd[ViPipe]);
            g_as32AeFd[ViPipe] = -1;
            return HI_ERR_ISP_NOT_INIT;
        }
    }

    bMemInit = HI_FALSE;
    result = ioctl(g_as32AeFd[ViPipe], ISP_MEM_INFO_GET, &bMemInit);
    if ( result != HI_SUCCESS ) {
        HI_TRACE_ISP(RE_DBG_LVL, "ISP[%d] get Mem info failed!\n", ViPipe);
        return HI_ERR_ISP_MEM_NOT_INIT;
    }
    if ( !bMemInit ) {
        HI_TRACE_ISP(RE_DBG_LVL, "ISP[%d] Mem NOT Init %d!\n", ViPipe, bMemInit);
        return HI_ERR_ISP_MEM_NOT_INIT;
    }

    offset = (HI_U8)(IO_READ32((ViPipe << 17) + 0x100034) >> 8);

    pstExpInfo->u32ExpTime       = IO_READ32((offset << 13) + 0x700040);
    pstExpInfo->u32ShortExpTime  = IO_READ32((offset << 13) + 0x700538);
    pstExpInfo->u32MedianExpTime = IO_READ32((offset << 13) + 0x70053C);
    pstExpInfo->u32LongExpTime   = IO_READ32((offset << 13) + 0x700528);
    pstExpInfo->u32AGain         = IO_READ32((offset << 13) + 0x700044);
    pstExpInfo->u32DGain         = IO_READ32((offset << 13) + 0x700048);
    pstExpInfo->u32AGainSF       = IO_READ32((offset << 13) + 0x70004C);
    pstExpInfo->u32DGainSF       = IO_READ32((offset << 13) + 0x700050);
    pstExpInfo->u32ISPDGain      = IO_READ32((offset << 13) + 0x700540);
    pstExpInfo->u32Exposure      = IO_READ32((offset << 13) + 0x700544);
    pstExpInfo->bExposureIsMAX   = IO_READ16((offset << 13) + 0x700054) & 1;
    pstExpInfo->s16HistError     = IO_READ16((offset << 13) + 0x70000E);
    pstExpInfo->u8AveLum         = IO_READ8 ((offset << 13) + 0x70000D);
    pstExpInfo->u32LinesPer500ms = IO_READ32((offset << 13) + 0x7001C0);
    pstExpInfo->u32PirisFNO      = IO_READ32((offset << 13) + 0x7001C4);
    pstExpInfo->u32Fps           = IO_READ32((offset << 13) + 0x700290);
    pstExpInfo->u32ISO           = IO_READ32((offset << 13) + 0x700530);
    pstExpInfo->u32ISOSF         = IO_READ32((offset << 13) + 0x700708);
    pstExpInfo->u32ISOCalibrate  = IO_READ16((offset << 13) + 0x70052C);
    pstExpInfo->u32RefExpRatio   = IO_READ32((offset << 13) + 0x700534);

    /* Read AE route info */
    {
        HI_U32 n = (HI_U8)IO_READ16((offset << 13) + 0x70028C);
        if ( n > 16 ) n = 16;
        pstExpInfo->stAERoute.u32TotalNum = n;
        for ( i = 0; i < n; i++ ) {
            HI_U32 nodeBase = i * 12;
            pstExpInfo->stAERoute.astRouteNode[i].u32IntTime    = IO_READ32(nodeBase + (offset << 13) + 0x7001CC);
            pstExpInfo->stAERoute.astRouteNode[i].u32SysGain    = IO_READ32(nodeBase + (offset << 13) + 0x7001D0);
            pstExpInfo->stAERoute.astRouteNode[i].enIrisFNO     = IO_READ32(nodeBase + (offset << 13) + 0x7001D4);
            pstExpInfo->stAERoute.astRouteNode[i].u32IrisFNOLin = IO_READ16((offset << 13) + 0x700590 + (i << 1));
        }
    }

    /* Read AE route ex info */
    {
        HI_U32 n = IO_READ8((offset << 13) + 0x7003D6);
        if ( n > 16 ) n = 16;
        pstExpInfo->stAERouteEx.u32TotalNum = n;
        for ( i = 0; i < n; i++ ) {
            HI_U32 nodeBase = i * 20;
            pstExpInfo->stAERouteEx.astRouteExNode[i].u32IntTime    = IO_READ32(nodeBase + (offset << 13) + 0x7003D8);
            pstExpInfo->stAERouteEx.astRouteExNode[i].u32Again      = IO_READ32(nodeBase + (offset << 13) + 0x7003DC);
            pstExpInfo->stAERouteEx.astRouteExNode[i].u32Dgain      = IO_READ32(nodeBase + (offset << 13) + 0x7003E0);
            pstExpInfo->stAERouteEx.astRouteExNode[i].u32IspDgain   = IO_READ32(nodeBase + (offset << 13) + 0x7003E4);
            pstExpInfo->stAERouteEx.astRouteExNode[i].enIrisFNO     = IO_READ32(nodeBase + (offset << 13) + 0x7003E8);
            pstExpInfo->stAERouteEx.astRouteExNode[i].u32IrisFNOLin = IO_READ16((offset << 13) + 0x7005B0 + (i << 1));
        }
    }

    return HI_SUCCESS;
}


HI_S32
HI_MPI_ISP_GetIrisAttr(VI_PIPE ViPipe, ISP_IRIS_ATTR_S *pstIrisAttr)
{
    HI_S32 result;
    HI_BOOL bMemInit;
    HI_U32 offset, data;

    if ( ViPipe > 3 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Err AE dev %d in %s!\n", ViPipe, __FUNCTION__);
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }
    if ( pstIrisAttr == HI_NULL ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Null Pointer in %s!\n", __FUNCTION__);
        return HI_ERR_ISP_NULL_PTR;
    }

    if ( g_as32AeFd[ViPipe] <= 0 ) {
        g_as32AeFd[ViPipe] = open(ISP_DEV_NAME, O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO);
        if ( g_as32AeFd[ViPipe] < 0 ) {
            perror("open isp device error!\n");
            return HI_ERR_ISP_NOT_INIT;
        }
        result = ioctl(g_as32AeFd[ViPipe], ISP_DEV_SET_FD, &ViPipe) == 0;
        if ( result != HI_SUCCESS ) {
            close(g_as32AeFd[ViPipe]);
            g_as32AeFd[ViPipe] = -1;
            return HI_ERR_ISP_NOT_INIT;
        }
    }

    bMemInit = HI_FALSE;
    result = ioctl(g_as32AeFd[ViPipe], ISP_MEM_INFO_GET, &bMemInit);
    if ( result != HI_SUCCESS ) {
        HI_TRACE_ISP(RE_DBG_LVL, "ISP[%d] get Mem info failed!\n", ViPipe);
        return HI_ERR_ISP_MEM_NOT_INIT;
    }
    if ( !bMemInit ) {
        HI_TRACE_ISP(RE_DBG_LVL, "ISP[%d] Mem NOT Init %d!\n", ViPipe, bMemInit);
        return HI_ERR_ISP_MEM_NOT_INIT;
    }

    offset = (HI_U8)(IO_READ32((ViPipe << 17) + 0x100034) >> 8);

    pstIrisAttr->bEnable  = IO_READ16((offset << 13) + 0x70016C) & 1;
    pstIrisAttr->enOpType = IO_READ16((offset << 13) + 0x70016E) & 1;

    data = IO_READ16((offset << 13) + 0x700176) & 3;
    if ( data >= 2 ) data = 2;
    pstIrisAttr->enIrisType = data;

    pstIrisAttr->enIrisStatus          = IO_READ8((offset << 13) + 0x70017C) & 3;
    pstIrisAttr->stMIAttr.enIrisFNO    = IO_READ8((offset << 13) + 0x700199);
    pstIrisAttr->stMIAttr.u32HoldValue = IO_READ32((offset << 13) + 0x700170);

    return HI_SUCCESS;
}


HI_S32
HI_MPI_ISP_GetDcirisAttr(VI_PIPE ViPipe, ISP_DCIRIS_ATTR_S *pstDcirisAttr)
{
    HI_S32 result;
    HI_BOOL bMemInit;
    HI_U32 offset;

    if ( ViPipe > 3 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Err AE dev %d in %s!\n", ViPipe, __FUNCTION__);
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }
    if ( pstDcirisAttr == HI_NULL ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Null Pointer in %s!\n", __FUNCTION__);
        return HI_ERR_ISP_NULL_PTR;
    }

    if ( g_as32AeFd[ViPipe] <= 0 ) {
        g_as32AeFd[ViPipe] = open(ISP_DEV_NAME, O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO);
        if ( g_as32AeFd[ViPipe] < 0 ) {
            perror("open isp device error!\n");
            return HI_ERR_ISP_NOT_INIT;
        }
        result = ioctl(g_as32AeFd[ViPipe], ISP_DEV_SET_FD, &ViPipe) == 0;
        if ( result != HI_SUCCESS ) {
            close(g_as32AeFd[ViPipe]);
            g_as32AeFd[ViPipe] = -1;
            return HI_ERR_ISP_NOT_INIT;
        }
    }

    bMemInit = HI_FALSE;
    result = ioctl(g_as32AeFd[ViPipe], ISP_MEM_INFO_GET, &bMemInit);
    if ( result != HI_SUCCESS ) {
        HI_TRACE_ISP(RE_DBG_LVL, "ISP[%d] get Mem info failed!\n", ViPipe);
        return HI_ERR_ISP_MEM_NOT_INIT;
    }
    if ( !bMemInit ) {
        HI_TRACE_ISP(RE_DBG_LVL, "ISP[%d] Mem NOT Init %d!\n", ViPipe, bMemInit);
        return HI_ERR_ISP_MEM_NOT_INIT;
    }

    offset = (HI_U8)(IO_READ32((ViPipe << 17) + 0x100034) >> 8);

    pstDcirisAttr->s32Kp          = IO_READ32((offset << 13) + 0x7001A0);
    pstDcirisAttr->s32Ki          = IO_READ32((offset << 13) + 0x7001A4);
    pstDcirisAttr->s32Kd          = IO_READ32((offset << 13) + 0x7001A8);
    pstDcirisAttr->u32MinPwmDuty  = IO_READ32((offset << 13) + 0x7001AC);
    pstDcirisAttr->u32MaxPwmDuty  = IO_READ32((offset << 13) + 0x7001B0);
    pstDcirisAttr->u32OpenPwmDuty = IO_READ32((offset << 13) + 0x7001B4);

    return HI_SUCCESS;
}


HI_S32
HI_MPI_ISP_GetPirisAttr(VI_PIPE ViPipe, ISP_PIRIS_ATTR_S *pstPirisAttr)
{
    HI_S32 result;
    HI_BOOL bMemInit;
    HI_U32 offset, i;

    if ( ViPipe > 3 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Err AE dev %d in %s!\n", ViPipe, __FUNCTION__);
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }
    if ( pstPirisAttr == HI_NULL ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Null Pointer in %s!\n", __FUNCTION__);
        return HI_ERR_ISP_NULL_PTR;
    }

    if ( g_as32AeFd[ViPipe] <= 0 ) {
        g_as32AeFd[ViPipe] = open(ISP_DEV_NAME, O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO);
        if ( g_as32AeFd[ViPipe] < 0 ) {
            perror("open isp device error!\n");
            return HI_ERR_ISP_NOT_INIT;
        }
        result = ioctl(g_as32AeFd[ViPipe], ISP_DEV_SET_FD, &ViPipe) == 0;
        if ( result != HI_SUCCESS ) {
            close(g_as32AeFd[ViPipe]);
            g_as32AeFd[ViPipe] = -1;
            return HI_ERR_ISP_NOT_INIT;
        }
    }

    bMemInit = HI_FALSE;
    result = ioctl(g_as32AeFd[ViPipe], ISP_MEM_INFO_GET, &bMemInit);
    if ( result != HI_SUCCESS ) {
        HI_TRACE_ISP(RE_DBG_LVL, "ISP[%d] get Mem info failed!\n", ViPipe);
        return HI_ERR_ISP_MEM_NOT_INIT;
    }
    if ( !bMemInit ) {
        HI_TRACE_ISP(RE_DBG_LVL, "ISP[%d] Mem NOT Init %d!\n", ViPipe, bMemInit);
        return HI_ERR_ISP_MEM_NOT_INIT;
    }

    offset = (HI_U8)(IO_READ32((ViPipe << 17) + 0x100034) >> 8);

    pstPirisAttr->bStepFNOTableChange = 0;
    pstPirisAttr->enMaxIrisFNOTarget  = IO_READ8((offset << 13) + 0x70019A);
    pstPirisAttr->enMinIrisFNOTarget  = IO_READ8((offset << 13) + 0x70019B);
    pstPirisAttr->bZeroIsMax          = IO_READ8((offset << 13) + 0x7001B8) & 1;
    pstPirisAttr->u16TotalStep        = IO_READ16((offset << 13) + 0x7001BA);
    pstPirisAttr->u16StepCount        = IO_READ16((offset << 13) + 0x7001BC);

    for ( i = 0; i < 1024; i++ ) {
        if ( i >= pstPirisAttr->u16StepCount )
            pstPirisAttr->au16StepFNOTable[i] = 0;
        else
            pstPirisAttr->au16StepFNOTable[i] = IO_READ16((offset << 13) + 0x700800 + (i << 1));
    }

    pstPirisAttr->bFNOExValid         = IO_READ8 ((offset << 13) + 0x70054C) & 1;
    pstPirisAttr->u32MaxIrisFNOTarget = IO_READ16((offset << 13) + 0x700548);
    pstPirisAttr->u32MinIrisFNOTarget = IO_READ16((offset << 13) + 0x70054A);

    return HI_SUCCESS;
}


HI_S32
HI_MPI_ISP_GetAERouteAttrEx(VI_PIPE ViPipe, ISP_AE_ROUTE_EX_S *pstAERouteAttrEx)
{
    HI_S32 result;
    HI_BOOL bMemInit;
    HI_U32 offset, i;

    if ( ViPipe > 3 ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Err AE dev %d in %s!\n", ViPipe, __FUNCTION__);
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }
    if ( pstAERouteAttrEx == HI_NULL ) {
        HI_TRACE_ISP(RE_DBG_LVL, "Null Pointer in %s!\n", __FUNCTION__);
        return HI_ERR_ISP_NULL_PTR;
    }

    if ( g_as32AeFd[ViPipe] <= 0 ) {
        g_as32AeFd[ViPipe] = open(ISP_DEV_NAME, O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO);
        if ( g_as32AeFd[ViPipe] < 0 ) {
            perror("open isp device error!\n");
            return HI_ERR_ISP_NOT_INIT;
        }
        result = ioctl(g_as32AeFd[ViPipe], ISP_DEV_SET_FD, &ViPipe) == 0;
        if ( result != HI_SUCCESS ) {
            close(g_as32AeFd[ViPipe]);
            g_as32AeFd[ViPipe] = -1;
            return HI_ERR_ISP_NOT_INIT;
        }
    }

    bMemInit = HI_FALSE;
    result = ioctl(g_as32AeFd[ViPipe], ISP_MEM_INFO_GET, &bMemInit);
    if ( result != HI_SUCCESS ) {
        HI_TRACE_ISP(RE_DBG_LVL, "ISP[%d] get Mem info failed!\n", ViPipe);
        return HI_ERR_ISP_MEM_NOT_INIT;
    }
    if ( !bMemInit ) {
        HI_TRACE_ISP(RE_DBG_LVL, "ISP[%d] Mem NOT Init %d!\n", ViPipe, bMemInit);
        return HI_ERR_ISP_MEM_NOT_INIT;
    }

    offset = (HI_U8)(IO_READ32((ViPipe << 17) + 0x100034) >> 8);

    memset_s(pstAERouteAttrEx, sizeof(ISP_AE_ROUTE_EX_S), 0, sizeof(ISP_AE_ROUTE_EX_S));
    pstAERouteAttrEx->u32TotalNum = (HI_U8)IO_READ16((offset << 13) + 0x70028E);

    for ( i = 0; i < pstAERouteAttrEx->u32TotalNum; i++ ) {
        HI_U32 nodeBase = i * 20;
        pstAERouteAttrEx->astRouteExNode[i].u32IntTime    = IO_READ32(nodeBase + (offset << 13) + 0x700294);
        pstAERouteAttrEx->astRouteExNode[i].u32Again      = IO_READ32(nodeBase + (offset << 13) + 0x700298);
        pstAERouteAttrEx->astRouteExNode[i].u32Dgain      = IO_READ32(nodeBase + (offset << 13) + 0x70029C);
        pstAERouteAttrEx->astRouteExNode[i].u32IspDgain   = IO_READ32(nodeBase + (offset << 13) + 0x7002A0);
        pstAERouteAttrEx->astRouteExNode[i].enIrisFNO     = IO_READ32(nodeBase + (offset << 13) + 0x7002A4);
        pstAERouteAttrEx->astRouteExNode[i].u32IrisFNOLin = IO_READ16((offset << 13) + 0x700570 + (i << 1));
    }

    return HI_SUCCESS;
}