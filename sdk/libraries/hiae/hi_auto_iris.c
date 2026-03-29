/**
 * Reverse Engineered by TekuConcept on April 28, 2021
 * DC iris and P-iris control algorithms for AE
 */

#include <stdio.h>
#include <string.h>

#include "re_hi_ae_adp.h"
#include "hi_ae_comm.h"
#include "securec.h"

#define RE_DBG_LVL HI_DBG_ERR

#define AE_CTX_BYTE(ctx, off)   (*(HI_U8  *)((HI_U8 *)(ctx) + (off)))
#define AE_CTX_HALF(ctx, off)   (*(HI_U16 *)((HI_U8 *)(ctx) + (off)))
#define AE_CTX_SHALF(ctx, off)  (*(HI_S16 *)((HI_U8 *)(ctx) + (off)))
#define AE_CTX_WORD(ctx, off)   (*(HI_U32 *)((HI_U8 *)(ctx) + (off)))
#define AE_CTX_SWORD(ctx, off)  (*(HI_S32 *)((HI_U8 *)(ctx) + (off)))
#define AE_CTX_PTR(ctx, off)    (*(void  **)((HI_U8 *)(ctx) + (off)))
#define AE_CTX_ADDR(ctx, off)   ((void *)((HI_U8 *)(ctx) + (off)))
#define AE_SIZEOF 0x28F0

extern ISP_AE_CTX_S g_astAeCtx[AE_CTX_SIZE];

/* DC iris coefficient tables (from RODATA) */
static const HI_S32 g_as32DcIrisCoefPos[9] = {
    100, 95, 90, 85, 70, 60, 40, 20, 10
};
static const HI_S32 g_as32DcIrisCoefNeg[9] = {
    200, 180, 160, 140, 100, 60, 40, 20, 10
};

/* ========================================================================== */

void AeDcIrisCtrlInit(HI_S32 s32Handle)
{
    HI_U8 *pCtx;
    HI_S32 as32CoefTable[18];

    /* Build combined coefficient table: pos[9] + neg[9] */
    memcpy(as32CoefTable, g_as32DcIrisCoefPos, 36);
    memcpy(as32CoefTable + 9, g_as32DcIrisCoefNeg, 36);

    pCtx = (HI_U8 *)&g_astAeCtx[0] + (HI_U32)s32Handle * AE_SIZEOF;

    AE_CTX_WORD(pCtx, 0x1728) = 0;          /* PID prev output */
    AE_CTX_SWORD(pCtx, 0x171c) = 950;       /* Ki */
    AE_CTX_SWORD(pCtx, 0x1720) = 250;       /* Ki min */
    AE_CTX_SWORD(pCtx, 0x1724) = 800;       /* Ki max */
    AE_CTX_WORD(pCtx, 0x16b0) = 0;          /* prev state */
    AE_CTX_WORD(pCtx, 0x16c4) = 1000000;    /* max PWM duty */
    AE_CTX_WORD(pCtx, 0x172c) = 0;          /* iris mode */
    AE_CTX_WORD(pCtx, 0x16ac) = 1;          /* DC iris enable */
    AE_CTX_WORD(pCtx, 0x16b8) = 1;          /* first run */
    AE_CTX_WORD(pCtx, 0x16c8) = 0;
    AE_CTX_WORD(pCtx, 0x1748) = 0;          /* wait count */
    AE_CTX_SWORD(pCtx, 0x16b4) = 7000;      /* Kp */
    AE_CTX_SWORD(pCtx, 0x16bc) = 3000;      /* Kd */
    AE_CTX_WORD(pCtx, 0x174c) = 950;        /* PWM duty */
    AE_CTX_WORD(pCtx, 0x1750) = 0;
    AE_CTX_WORD(pCtx, 0x16c0) = 0;          /* target */

    /* Copy coefficient table to context at 0x16d4 (72 bytes) */
    memcpy_s(AE_CTX_ADDR(pCtx, 0x16d4), 72, as32CoefTable, 72);

    AE_CTX_WORD(pCtx, 0x16cc) = 0;
    AE_CTX_WORD(pCtx, 0x16d0) = 0;
}


HI_S32 AeDcIrisDebug(HI_S32 s32Handle)
{
    HI_U8 *pCtx = (HI_U8 *)&g_astAeCtx[0] + (HI_U32)s32Handle * AE_SIZEOF;
    HI_S32 s32DevHandle = AE_CTX_SWORD(pCtx, 0x1c4c);
    HI_S32 s32RunMode = AE_CTX_WORD(pCtx, 0x1740);
    HI_S32 (*pfnUpdate)(HI_S32, HI_S32);

    if (s32RunMode == 1) {
        pfnUpdate = (HI_S32 (*)(HI_S32, HI_S32))AE_CTX_PTR(pCtx, 0x169c);
        if (pfnUpdate != HI_NULL)
            pfnUpdate(s32DevHandle, 1000);
        return 0;
    }
    if (s32RunMode == 2) {
        pfnUpdate = (HI_S32 (*)(HI_S32, HI_S32))AE_CTX_PTR(pCtx, 0x169c);
        if (pfnUpdate != HI_NULL)
            pfnUpdate(s32DevHandle, 100);
        return 0;
    }
    return 0;
}


/*
 * dcIrisCtrlerProcess — PID controller for DC iris
 * R0 = pError, R1 = pOutput, R2 = pCtrl (context at offset 0x174c area)
 */
static HI_S32 dcIrisCtrlerProcess(HI_S32 *pError, HI_S32 *pOutput, HI_U8 *pCtrl)
{
    HI_S32 s32Error, s32PrevInput, s32DeltaError;
    HI_S32 s32AbsError, s32AbsDelta;
    HI_U32 u32IpWeight;
    HI_S32 s32Kp, s32Ki, s32Kd;
    HI_S32 s32PropTerm, s32IntTerm;
    HI_S32 s32FbTerm, s32DerivTerm, s32PidOutput;
    HI_S32 s32Isqrt, s32Integral;

    if (pError == HI_NULL) {
        HI_TRACE_ISP(RE_DBG_LVL, "Null Pointer in %s!\n", __FUNCTION__);
        return HI_ERR_ISP_NULL_PTR;
    }
    if (pOutput == HI_NULL) {
        HI_TRACE_ISP(RE_DBG_LVL, "Null Pointer in %s!\n", __FUNCTION__);
        return HI_ERR_ISP_NULL_PTR;
    }

    s32Error = *pError;
    s32PrevInput = *(HI_S32 *)(pCtrl + 32);  /* +0x20: current input */
    *(HI_S32 *)(pCtrl + 36) = s32PrevInput;  /* +0x24: save as prev */
    *(HI_S32 *)(pCtrl + 32) = s32Error;
    s32DeltaError = s32Error - s32PrevInput;

    /* Integer square root of |error| */
    s32AbsError = (s32Error < 0) ? -s32Error : s32Error;
    s32Isqrt = 0;
    {
        HI_S32 n = 0;
        while (1) {
            HI_S32 next = n + 1;
            if ((HI_U32)(next * n) >= (HI_U32)s32AbsError)
                break;
            n = next;
        }
        s32Isqrt = n;
    }

    s32AbsDelta = (s32DeltaError < 0) ? -s32DeltaError : s32DeltaError;

    /* Compute interpolation weight based on error magnitude */
    if (s32Error <= 15) {
        u32IpWeight = 32;
    } else if (s32Error <= 47) {
        u32IpWeight = (HI_U16)(s32Error + 16);
    } else if (s32Error <= 63) {
        HI_S32 tmp = (s32Error - 48) << 6;
        u32IpWeight = (HI_U16)((tmp >> 5) + 64);
    } else {
        u32IpWeight = 96;
    }

    /* Load PID coefficients and scale by weight */
    s32Kp = *(HI_S32 *)(pCtrl + 8);
    s32Ki = *(HI_S32 *)(pCtrl + 16);
    s32Kd = *(HI_S32 *)(pCtrl + 12);

    s32PropTerm = s32Kp * (HI_S32)u32IpWeight;
    s32IntTerm = s32Ki * (HI_S32)u32IpWeight;
    s32PropTerm = (s32PropTerm < 0) ? (s32PropTerm + 31) >> 5 : s32PropTerm >> 5;
    s32IntTerm = (s32IntTerm < 0) ? (s32IntTerm + 31) >> 5 : s32IntTerm >> 5;

    /* Check if error+255 is within coefficient table range */
    if ((HI_U32)(s32Error + 255) <= 510) {
        HI_S32 s32TableBase = (s32Error >= 0) ? 9 : 0;
        HI_S32 s32Frac = (HI_S32)u32IpWeight & 31;
        HI_S32 s32Idx = s32TableBase + ((HI_S32)u32IpWeight >> 5);
        HI_S32 s32CoefVal = *(HI_S32 *)(pCtrl + 40 + s32Idx * 4);
        HI_S32 s32CoefNext = *(HI_S32 *)(pCtrl + 40 + (s32Idx + 1) * 4);
        HI_S32 s32Diff = s32CoefNext - s32CoefVal;
        s32Diff = s32Frac * s32Diff;
        s32Diff = (s32Diff < 0) ? (s32Diff + 31) >> 5 : s32Diff >> 5;
        s32Integral = s32CoefVal + s32Diff;
        s32Integral = s32Error * s32Integral;
    } else {
        s32Integral = *(HI_S32 *)(pCtrl + 24);
    }

    /* Compute PID output using fixed-point division:
       /100000 via magic 0x14F8B589, /100 via magic 0x10624DD3 */
    {
        HI_S64 s64Tmp;
        HI_S32 s32MulKd = s32Integral * s32Kd;

        s64Tmp = (HI_S64)s32MulKd * 0x14F8B589LL;
        s32FbTerm = (HI_S32)((s64Tmp >> 32) >> 13) - (s32MulKd >> 31);

        s64Tmp = (HI_S64)(s32PropTerm * s32Error) * 0x10624DD3LL;
        s32PropTerm = (HI_S32)((s64Tmp >> 32) >> 6) - ((s32PropTerm * s32Error) >> 31);

        s64Tmp = (HI_S64)(s32IntTerm * s32DeltaError) * 0x10624DD3LL;
        s32DerivTerm = (HI_S32)((s64Tmp >> 32) >> 6) - ((s32IntTerm * s32DeltaError) >> 31);

        s32PidOutput = s32PropTerm + s32FbTerm + s32DerivTerm;
    }

    *(HI_S32 *)(pCtrl + 20) = s32Error;
    *(HI_S32 *)(pCtrl + 28) = s32DeltaError;

    /* Clamp output */
    {
        HI_S32 s32Max = *(HI_S32 *)(pCtrl + 112);  /* +0x70 */
        HI_S32 s32Min = *(HI_S32 *)(pCtrl + 116);  /* +0x74 */
        if (s32PidOutput >= s32Min) {
            /* ok */
        } else if (s32PidOutput < s32Max) {
            s32PidOutput = s32Max;
        }
    }

    /* Clamp integral to [100000, 1000000] */
    s32Integral = *(HI_S32 *)(pCtrl + 24) + s32Kd;
    if (s32Integral >= 1000000) s32Integral = 1000000;
    if (s32Integral < 100000)  s32Integral = 100000;

    /* Damping logic */
    if (s32Isqrt > s32AbsDelta) {
        *(HI_S32 *)(pCtrl + 164) = 0;
    } else {
        HI_S32 s32DampThresh = *(HI_S32 *)(pCtrl + 120);
        if ((HI_U32)s32DampThresh > (HI_U32)s32PidOutput) {
            *(HI_S32 *)(pCtrl + 164) = 0;
        } else {
            HI_FLOAT f32Fps = *(HI_FLOAT *)(pCtrl + 152);
            HI_S32 s32MaxDamp = (f32Fps >= 30.0f) ? 25 : 50;
            HI_S32 s32Count = *(HI_S32 *)(pCtrl + 164);
            *(HI_S32 *)(pCtrl + 164) = s32Count + 1;
            if ((HI_U32)s32MaxDamp < (HI_U32)s32Count) {
                *(HI_S32 *)(pCtrl + 164) = 0;
                *(HI_S32 *)(pCtrl + 124) = 0;
            }
        }
    }

    *(HI_S32 *)(pCtrl + 24) = s32Integral;
    *pOutput = s32PidOutput;
    return 0;
}


/*
 * dcIrisCtrlerDoCmd — Controller commands
 * cmd=1: get hold flag, cmd=2: reset/init controller
 */
static HI_S32 dcIrisCtrlerDoCmd(HI_S32 s32Cmd, HI_S32 *pOutput, HI_U8 *pCtrl)
{
    if (pOutput == HI_NULL) {
        HI_TRACE_ISP(RE_DBG_LVL, "Null Pointer in %s!\n", __FUNCTION__);
        return HI_ERR_ISP_NULL_PTR;
    }

    if (s32Cmd == 1) {
        *pOutput = *(HI_S32 *)(pCtrl + 124);  /* hold flag at +0x7c */
        return 0;
    }

    if (s32Cmd == 2) {
        HI_S32 s32DampThresh = *(HI_S32 *)(pCtrl + 120);  /* +0x78 */
        HI_S32 s32InitDuty;

        *(HI_S32 *)(pCtrl + 124) = 1;  /* set hold flag */
        *(HI_S32 *)(pCtrl + 20) = 0;   /* clear error */

        if ((HI_U32)(s32DampThresh - 200) <= 99) {
            s32InitDuty = 100000;
        } else {
            s32InitDuty = 1000 * s32DampThresh - 200000;
            if ((HI_U32)s32InitDuty >= 1000000)
                s32InitDuty = 1000000;
        }

        *(HI_S32 *)(pCtrl + 24) = s32InitDuty;
        *(HI_S32 *)(pCtrl + 28) = 0;
        *(HI_S32 *)(pCtrl + 32) = 0;
        *(HI_S32 *)(pCtrl + 36) = 0;
        return 0;
    }

    return 0;
}


HI_S32 AeDcIrisAuto(HI_S32 s32Handle)
{
    HI_U8 *pBase = (HI_U8 *)&g_astAeCtx[0];
    HI_U8 *pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    HI_S32 s32Error;
    HI_S32 s32DevHandle;
    HI_S32 s32Result;
    HI_S32 s32HoldFlag;
    HI_S32 (*pfnUpdate)(HI_S32, HI_S32);

    /* Compute error from AE compensation */
    {
        HI_S16 s16Comp = AE_CTX_SHALF(pCtx, 0xdc);
        HI_U8 u8RunInterval = AE_CTX_BYTE(pCtx, 0xc4);
        if (u8RunInterval == 0) u8RunInterval = 1;
        s32Error = (s16Comp * 7 * 8) / u8RunInterval;
    }

    s32DevHandle = AE_CTX_SWORD(pCtx, 0x1c4c);

    /* Store FPS into DC iris state */
    AE_CTX_WORD(pCtx, 0x1744) = AE_CTX_WORD(pCtx, 0xbc);

    /* Check if iris needs full open */
    {
        HI_U32 u32IntUpper = AE_CTX_WORD(pCtx, 0x514);
        HI_U32 u32IntField = AE_CTX_WORD(pCtx, 0x4fc);
        HI_U32 u32Ratio2 = AE_CTX_WORD(pCtx, 0x54c);
        HI_U32 u32Ratio1 = AE_CTX_WORD(pCtx, 0x534);
        HI_BOOL bNeedFullOpen = HI_FALSE;

        if (u32IntUpper > u32IntField || u32Ratio2 > u32Ratio1) {
            bNeedFullOpen = HI_TRUE;
        } else {
            HI_U32 u32R3 = AE_CTX_WORD(pCtx, 0x560);
            HI_U32 u32R4 = AE_CTX_WORD(pCtx, 0x56c);
            if (u32R4 > u32R3 * 2) {
                bNeedFullOpen = HI_TRUE;
            }
        }

        if (bNeedFullOpen) {
            if (AE_CTX_WORD(pCtx, 0x16ac) == 0)
                goto save_prev;
            goto do_full_open;
        }

        if (AE_CTX_WORD(pCtx, 0x16ac) == 0)
            goto save_prev;

        if (AE_CTX_WORD(pCtx, 0x4c8) > AE_CTX_WORD(pCtx, 0x520))
            goto do_full_open;
    }

    /* Check iris mode */
    {
        HI_U8 *pSL = pCtx + 0x16ac; /* SL points to DC iris state base */
        HI_S32 s32Mode = AE_CTX_WORD(pCtx, 0x172c);

        if (s32Mode == 0) {
            /* Waiting mode */
            HI_S32 s32WaitCnt = AE_CTX_WORD(pCtx, 0x1748);
            AE_CTX_WORD(pCtx, 0x1748) = s32WaitCnt + 1;
            if (s32WaitCnt <= 5) {
                s32Result = 1;
                goto iris_update;
            }
            /* Transition to running */
            AE_CTX_WORD(pCtx, 0x1748) = 0;
            AE_CTX_WORD(pCtx, 0x172c) = 1;
            dcIrisCtrlerDoCmd(2, &s32HoldFlag, pSL);
            s32Result = (AE_CTX_WORD(pCtx, 0x172c) != 1) ? 1 : 0;
            goto iris_update;
        }

        if (s32Mode != 1) {
            s32Result = 1;
            goto iris_update;
        }

        /* Mode 1: running — execute PID */
        dcIrisCtrlerProcess(&s32Error, &s32HoldFlag, pSL);
        dcIrisCtrlerDoCmd(1, &s32HoldFlag, pSL);

        if (s32HoldFlag == 0) {
            AE_CTX_WORD(pCtx, 0x172c) = 0;
            s32Result = 1;
        } else {
            s32Result = (AE_CTX_WORD(pCtx, 0x172c) != 1) ? 1 : 0;
        }
        goto iris_update;
    }

do_full_open:
    {
        HI_U8 *pC = pBase + (HI_U32)s32Handle * AE_SIZEOF;
        AE_CTX_WORD(pC, 0x172c) = 0;
        AE_CTX_WORD(pC, 0x1748) = 0;
        AE_CTX_WORD(pC, 0x174c) = 950;
        s32Result = 1;
    }

iris_update:
    {
        HI_U8 *pC = pBase + (HI_U32)s32Handle * AE_SIZEOF;
        AE_CTX_WORD(pC, 0x42c) = s32Result;

        pfnUpdate = (HI_S32 (*)(HI_S32, HI_S32))AE_CTX_PTR(pC, 0x169c);
        if (pfnUpdate != HI_NULL)
            pfnUpdate(s32DevHandle, AE_CTX_WORD(pC, 0x174c));
    }

save_prev:
    {
        HI_U8 *pC = pBase + (HI_U32)s32Handle * AE_SIZEOF;
        AE_CTX_WORD(pC, 0x16b0) = AE_CTX_WORD(pC, 0x16ac);
    }
    return 0;
}


HI_S32 AeDcIrisManu(HI_S32 s32Handle)
{
    HI_U8 *pCtx = (HI_U8 *)&g_astAeCtx[0] + (HI_U32)s32Handle * AE_SIZEOF;
    HI_S32 (*pfnUpdate)(HI_S32, HI_S32);

    pfnUpdate = (HI_S32 (*)(HI_S32, HI_S32))AE_CTX_PTR(pCtx, 0x169c);
    if (pfnUpdate == HI_NULL)
        return 0;

    pfnUpdate(AE_CTX_SWORD(pCtx, 0x1c4c), (HI_S32)AE_CTX_HALF(pCtx, 0x173c));
    return 0;
}


HI_S32 AeDcIrisExit(HI_S32 s32Handle)
{
    HI_U8 *pCtx = (HI_U8 *)&g_astAeCtx[0] + (HI_U32)s32Handle * AE_SIZEOF;
    HI_S32 s32DevHandle = AE_CTX_SWORD(pCtx, 0x1c4c);
    HI_S32 (*pfnUpdate)(HI_S32, HI_S32);
    HI_S32 (*pfnPIrisExit)(HI_S32);

    pfnUpdate = (HI_S32 (*)(HI_S32, HI_S32))AE_CTX_PTR(pCtx, 0x169c);
    if (pfnUpdate != HI_NULL)
        pfnUpdate(s32DevHandle, 0);

    pCtx = (HI_U8 *)&g_astAeCtx[0] + (HI_U32)s32Handle * AE_SIZEOF;
    pfnPIrisExit = (HI_S32 (*)(HI_S32))AE_CTX_PTR(pCtx, 0x16a0);
    if (pfnPIrisExit != HI_NULL)
        return pfnPIrisExit(s32DevHandle);

    return 0;
}


void AePIrisCtrlInit(HI_S32 s32Handle)
{
    HI_U8 *pCtx = (HI_U8 *)&g_astAeCtx[0] + (HI_U32)s32Handle * AE_SIZEOF;

    AE_CTX_WORD(pCtx, 0x1754) = AE_CTX_WORD(pCtx, 0xe84);
    AE_CTX_HALF(pCtx, 0x175e) = AE_CTX_HALF(pCtx, 0xe8a);
    AE_CTX_HALF(pCtx, 0x175c) = AE_CTX_HALF(pCtx, 0xe88);
    AE_CTX_WORD(pCtx, 0x1758) = AE_CTX_WORD(pCtx, 0xe80);
    AE_CTX_WORD(pCtx, 0x1760) = 1;      /* first run */
    AE_CTX_WORD(pCtx, 0x1768) = 10;     /* max iteration step */
    AE_CTX_WORD(pCtx, 0x1764) = 0;
    AE_CTX_WORD(pCtx, 0x1774) = 0;
}


HI_S32 AePIrisExit(HI_S32 s32Handle)
{
    HI_U8 *pCtx = (HI_U8 *)&g_astAeCtx[0] + (HI_U32)s32Handle * AE_SIZEOF;
    HI_S32 (*pfnPIrisExit)(HI_S32);

    pfnPIrisExit = (HI_S32 (*)(HI_S32))AE_CTX_PTR(pCtx, 0x16a8);
    if (pfnPIrisExit == HI_NULL)
        return 0;

    return pfnPIrisExit(AE_CTX_SWORD(pCtx, 0x1c4c));
}


HI_S32 HI_MPI_AE_IrisRegisterCallBack(ALG_LIB_S *pstAeLib, HI_VOID *pstIrisCb)
{
    HI_S32 s32Id;

    if (pstAeLib == HI_NULL) {
        HI_TRACE_ISP(RE_DBG_LVL, "Null Pointer in %s!\n", __FUNCTION__);
        return HI_ERR_ISP_NULL_PTR;
    }
    if (pstIrisCb == HI_NULL) {
        HI_TRACE_ISP(RE_DBG_LVL, "Null Pointer in %s!\n", __FUNCTION__);
        return HI_ERR_ISP_NULL_PTR;
    }

    s32Id = pstAeLib->s32Id;
    if ((HI_U32)s32Id > 3) {
        HI_TRACE_ISP(RE_DBG_LVL, "Illegal handle id %d in %s!\n",
            s32Id, __FUNCTION__);
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    if (strcmp(pstAeLib->acLibName, HI_AE_LIB_NAME) != 0) {
        HI_TRACE_ISP(RE_DBG_LVL, "Illegal lib name %s in %s!\n",
            pstAeLib->acLibName, __FUNCTION__);
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    {
        HI_U8 *pDst = (HI_U8 *)&g_astAeCtx[0] +
                       (HI_U32)s32Id * AE_SIZEOF + 0x1698;
        memcpy_s(pDst, 20, pstIrisCb, 20);
    }

    return HI_SUCCESS;
}


HI_S32 HI_MPI_AE_IrisUnRegisterCallBack(ALG_LIB_S *pstAeLib)
{
    HI_S32 s32Id;

    if (pstAeLib == HI_NULL) {
        HI_TRACE_ISP(RE_DBG_LVL, "Null Pointer in %s!\n", __FUNCTION__);
        return HI_ERR_ISP_NULL_PTR;
    }

    s32Id = pstAeLib->s32Id;
    if ((HI_U32)s32Id > 3) {
        HI_TRACE_ISP(RE_DBG_LVL, "Illegal handle id %d in %s!\n",
            s32Id, __FUNCTION__);
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    if (strcmp(pstAeLib->acLibName, HI_AE_LIB_NAME) != 0) {
        HI_TRACE_ISP(RE_DBG_LVL, "Illegal lib name %s in %s!\n",
            pstAeLib->acLibName, __FUNCTION__);
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    {
        HI_U8 *pDst = (HI_U8 *)&g_astAeCtx[0] +
                       (HI_U32)s32Id * AE_SIZEOF + 0x1698;
        memset_s(pDst, 20, 0, 20);
    }

    return HI_SUCCESS;
}
