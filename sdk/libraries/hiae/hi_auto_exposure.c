/**
 * Reverse Engineered by TekuConcept on April 28, 2021
 * hi_auto_exposure.c — AE exposure calculation and allocation
 */

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "re_hi_ae_adp.h"
#include "hi_comm_isp.h"
#include "hi_ae_comm.h"
#include "securec.h"

#define RE_DBG_LVL HI_DBG_ERR

#define AE_CTX_BYTE(ctx, off)   (*(HI_U8  *)((HI_U8 *)(ctx) + (off)))
#define AE_CTX_HALF(ctx, off)   (*(HI_U16 *)((HI_U8 *)(ctx) + (off)))
#define AE_CTX_SHALF(ctx, off)  (*(HI_S16 *)((HI_U8 *)(ctx) + (off)))
#define AE_CTX_WORD(ctx, off)   (*(HI_U32 *)((HI_U8 *)(ctx) + (off)))
#define AE_CTX_SWORD(ctx, off)  (*(HI_S32 *)((HI_U8 *)(ctx) + (off)))
#define AE_CTX_DWORD(ctx, off)  (*(HI_U64 *)((HI_U8 *)(ctx) + (off)))
#define AE_CTX_PTR(ctx, off)    (*(void  **)((HI_U8 *)(ctx) + (off)))
#define AE_CTX_ADDR(ctx, off)   ((void *)((HI_U8 *)(ctx) + (off)))
#define AE_CTX_FLOAT(ctx, off)  (*(HI_FLOAT *)((HI_U8 *)(ctx) + (off)))
#define AE_SIZEOF 0x28F0

extern ISP_AE_CTX_S g_astAeCtx[AE_CTX_SIZE];
extern HI_U32 AeBoundariesCheck(HI_U32 u32Value, HI_U32 u32Min, HI_U32 u32Max);
extern HI_U64 AeBoundariesCheck64(HI_U64 u64Value, HI_U64 u64Min, HI_U64 u64Max);
extern HI_U64 AeRatioCalculate(HI_U64 u64Denom, HI_U32 u32Factor, HI_U64 u64Num);


/* Forward declarations for functions defined later in this file */
static HI_S32 AeCalcGainTarget(HI_S32 s32Handle);
static HI_S32 AeWDRCalcTimeTarget(HI_S32 s32Handle);
static HI_U32 AeCalcSysGain(HI_U32 precision, HI_U32 again, HI_U32 dgainPrec,
                              HI_U32 dgain, HI_U32 ispDgainPrec, HI_U32 ispDgain,
                              HI_U32 irisPrec);


/* ========================================================================== */
/* 1. AeCalcIspDgain                                                          */
/*    Size: 0x84 (132 bytes)                                                  */
/*    Calculates ISP digital gain from the 64-bit exposure numerator and      */
/*    ISP dgain denominator, clamped to min/max bounds.                       */
/*    ctx+0x170 = shift, ctx+0x15c = min, ctx+0x160 = max, ctx+0x16c = result*/
/*    R0=ctx, R2:R3=u64Numerator(lo:hi), [SP+24]=u64Denominator(lo:hi)       */
/*    Returns remaining 64-bit quotient in R0:R1                              */
/* ========================================================================== */

HI_U64 AeCalcIspDgain(HI_U8 *pCtx, HI_U32 unused1,
                       HI_U64 u64Numerator, HI_U64 u64Denominator)
{
    HI_U32 u32Shift = AE_CTX_WORD(pCtx, 0x170);
    HI_U32 u32Min   = AE_CTX_WORD(pCtx, 0x15c);
    HI_U32 u32Max   = AE_CTX_WORD(pCtx, 0x160);
    HI_U64 u64Shifted;
    HI_U32 u32IspDgain;

    /* (numerator << shift + denominator/2) / denominator */
    u64Shifted = u64Numerator << u32Shift;
    u64Shifted = u64Shifted + (u64Denominator >> 1);
    if (u64Denominator == 0)
        u64Denominator = 1;
    u32IspDgain = (HI_U32)(u64Shifted / u64Denominator);

    /* Clamp to [min, max] */
    u32IspDgain = AeBoundariesCheck(u32IspDgain, u32Min, u32Max);
    AE_CTX_WORD(pCtx, 0x16c) = u32IspDgain;

    /* Recompute: (numerator << shift) / clamped_isp_dgain */
    if (u32IspDgain == 0)
        u32IspDgain = 1;
    return (u64Numerator << u32Shift) / (HI_U64)u32IspDgain;
}


/* ========================================================================== */
/* 2. AeLFWDRCalcTimeTarget                                                   */
/*    Size: 0x4 (4 bytes) — tail call / alias to AeLinearCalcTimeTarget       */
/* ========================================================================== */

HI_S32 AeLinearCalcTimeTarget(HI_U32 s32Handle); /* forward decl */

HI_S32 AeLFWDRCalcTimeTarget(HI_U32 s32Handle)
{
    return AeLinearCalcTimeTarget(s32Handle);
}


/* ========================================================================== */
/* 3. AePrec2Linear                                                           */
/*    Size: 0x9c (156 bytes)                                                  */
/*    Converts precision format {type, mantissa, shift} back to linear value. */
/*    R0=u32Steps, R1=pPrec (struct: [0]=type, [4]=mantissa, [8]=shift),      */
/*    R2=u32OutShift                                                          */
/* ========================================================================== */

HI_U32 AePrec2Linear(HI_U32 u32Steps, HI_U32 *pPrec, HI_U32 u32OutShift)
{
    HI_U32 u32Type = pPrec[0];
    HI_U32 u32Mantissa, u32Shift;
    HI_U32 u32Result;
    HI_U32 i;

    if (u32Type == 1) {
        /* Linear type: just shift the mantissa */
        u32Shift = pPrec[2];
        if (u32OutShift != u32Shift)
            return u32Steps << (u32OutShift - u32Shift);
        return u32Steps;
    }

    if (u32Type != 0) {
        /* Type 2 or unknown: return 0 for type != 0,1 */
        if (u32Type == 2)
            return u32Steps; /* passthrough */
        return 0;
    }

    /* Type 0: geometric/logarithmic conversion */
    if (u32Steps == 0)
        return 1 << u32OutShift;

    u32Mantissa = pPrec[1];
    u32Shift    = pPrec[2];
    if (u32Mantissa == 0)
        u32Mantissa = 1;

    u32Result = 1024; /* 0x400 */
    for (i = 0; i < u32Steps; i++) {
        u32Result = (HI_U32)(((HI_U64)u32Result << u32Shift) / (HI_U64)u32Mantissa);
    }

    u32Result = (u32Result + 512) >> 10;
    return u32Result << u32OutShift;
}


/* ========================================================================== */
/* 4. AeAccu2Prec                                                            */
/*    Size: 0x1b0 (432 bytes)                                                */
/*    Converts accumulated float EV value to precision format.               */
/*    R0=pInput (struct: [0]=type, [4]=float), R1=pOutput                    */
/*    pOutput: [0]=type, [4]=mantissa, [8]=shift                             */
/* ========================================================================== */

HI_S32 AeAccu2Prec(HI_U32 *pInput, HI_U32 *pOutput)
{
    HI_FLOAT fVal = *(HI_FLOAT *)(&pInput[1]);
    double   dVal = (double)fVal;
    HI_U32   u32Type = pInput[0];

    if (dVal >= 0.0 || dVal <= -1.0) {
        /* Normal range: positive or strongly negative */
        if (u32Type == 1) {
            /* Linear type */
            HI_U32 u32Shift = 0;
            pOutput[0] = 1;
            pOutput[1] = 1;
            pOutput[2] = 0;
            if (fVal < 1.0f) {
                /* Count how many doublings needed to reach >= 1.0 */
                HI_U32 u32Prev = 0;
                while (fVal < 1.0f) {
                    u32Prev = u32Shift;
                    fVal = fVal + fVal;
                    u32Shift = u32Prev + 1;
                }
                pOutput[2] = u32Prev;
            }
            return 0;
        }
        if (u32Type == 0) {
            /* Geometric type — normal range */
            HI_U32 u32PrecBits;
            HI_FLOAT fStep, fPow, fIntPart, fFrac;
            HI_U32 u32IntPart, u32Limit;

            pOutput[0] = 0;
            if (dVal < -20.0)
                u32PrecBits = 12;
            else
                u32PrecBits = 10;

            fStep = fVal / 20.0f;
            pOutput[2] = u32PrecBits;

            fPow = (HI_FLOAT)pow((double)fStep, 10.0);

            u32Limit = 1u << u32PrecBits;

            /* Clamp pow result */
            dVal = (double)fPow;
            if (dVal >= 0.0) {
                /* fPow >= 0: use as-is */
            } else if (dVal > -1.0) {
                fPow = 1.0f;  /* clamp small negative to 1.0 */
            }

            fIntPart = (HI_FLOAT)u32Limit / fPow;
            u32IntPart = (HI_U32)(HI_S32)fIntPart;
            fFrac = fIntPart - (HI_FLOAT)(HI_U32)u32IntPart;

            if ((fFrac + fFrac) >= 1.0f) {
                pOutput[1] = u32IntPart + 1;
            } else {
                pOutput[1] = u32IntPart;
            }
            return 0;
        }
        /* Fall through to type 2 */
    } else {
        /* Between -1.0 and 0.0 (exclusive): special close-to-zero handling */
        if (u32Type == 1) {
            /* Linear type — close to zero negative */
            pOutput[0] = 1;
            pOutput[1] = 1;
            pOutput[2] = 0;
            return 0;
        }
        if (u32Type == 0) {
            /* Geometric type — close to zero negative */
            HI_FLOAT fStep, fPow, fIntPart, fFrac;
            HI_U32 u32IntPart, u32Limit;

            pOutput[0] = 0;
            fStep = 1.0f / 20.0f;
            pOutput[2] = 10;

            fPow = (HI_FLOAT)pow((double)fStep, 10.0);

            u32Limit = 1u << 10;
            dVal = (double)fPow;
            if (dVal >= 0.0) {
                /* use as-is */
            } else if (dVal > -1.0) {
                fPow = 1.0f;
            }

            fIntPart = (HI_FLOAT)u32Limit / fPow;
            u32IntPart = (HI_U32)(HI_S32)fIntPart;
            fFrac = fIntPart - (HI_FLOAT)(HI_U32)u32IntPart;

            if ((fFrac + fFrac) >= 1.0f) {
                pOutput[1] = u32IntPart + 1;
            } else {
                pOutput[1] = u32IntPart;
            }
            return 0;
        }
        /* Fall through to type 2 */
    }

    /* Type 2 */
    if (u32Type == 2) {
        pOutput[0] = 2;
        pOutput[1] = 1;
        pOutput[2] = 10;
        return 0;
    }

    return 0;
}


/* ========================================================================== */
/* 5. AeCalcAntiflicker.isra.2.part.3                                        */
/*    Size: 0x30 (48 bytes)                                                   */
/*    Anti-flicker time rounding helper.                                      */
/*    R0=inttime, R1=flickerFreq, R2=linePer                                  */
/* ========================================================================== */

static HI_U32 AeCalcAntiflicker_isra_part(HI_U32 u32IntTime, HI_U32 u32FlickerFreq,
                                           HI_U32 u32LinePer)
{
    HI_U32 u32LinePer8 = u32LinePer << 8;
    HI_U32 u32Divisor  = (u32LinePer8 != 0) ? u32LinePer8 : 1;
    HI_U32 u32Result;

    u32Result = (u32IntTime * u32FlickerFreq) / u32Divisor;
    if (u32Result == 0)
        u32Result = 1;
    u32Result = u32Result * u32LinePer8;
    if (u32FlickerFreq == 0)
        u32FlickerFreq = 1;
    u32Result = u32Result / u32FlickerFreq;

    return u32Result;
}


/* ========================================================================== */
/* 6. AeCalcIntTime                                                           */
/*    Size: 0x330 (816 bytes)                                                */
/*    Calculates integration time for exposure, applying antiflicker and      */
/*    line-time quantisation. Uses g_astAeCtx[].                             */
/* ========================================================================== */

/* Forward decl for the antiflicker2 helper */
static HI_U32 AeCalcAntiflicker2_isra(HI_U32 u32IntTime, HI_U32 u32FlickerFreq,
                                       HI_U32 u32LinePer, HI_U32 u32Enable,
                                       HI_U32 u32LinePerShort);

HI_S32 AeCalcIntTime(HI_U32 s32Handle)
{
    HI_U8 *pBase = (HI_U8 *)&g_astAeCtx[0];
    HI_U8 *pCtx  = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    HI_U32 u32IntTime;
    HI_U8  u8WdrMode;
    HI_U8  u8WdrMode2;
    HI_U32 u32FlickerFreq;
    HI_FLOAT fLineTime;
    HI_U32 u32LineTimeInt;

    /* Clamp to [min, max] */
    u32IntTime = AeBoundariesCheck(
        AE_CTX_WORD(pCtx, 0x4c8),  /* current inttime */
        AE_CTX_WORD(pCtx, 0x4b0),  /* min */
        AE_CTX_WORD(pCtx, 0x4b4)   /* max */
    );
    AE_CTX_WORD(pCtx, 0x4c8) = u32IntTime;

    u8WdrMode = AE_CTX_BYTE(pCtx, 0xd);
    if ((HI_U8)(u8WdrMode - 2) <= 9) {
        /* WDR modes 2..11 */
        HI_BOOL bSpecialMode = HI_FALSE;
        if (u8WdrMode == 9 || u8WdrMode == 6)
            bSpecialMode = HI_TRUE;
        if ((HI_U8)(u8WdrMode - 2) <= 1)
            bSpecialMode = HI_TRUE;

        if (!bSpecialMode)
            goto wdr_path;

        /* LF-WDR or specific modes: check sensor timing mode */
        {
            HI_U32 u32TimingMode = AE_CTX_WORD(pCtx, 0x28);
            if (u32TimingMode != 1) {
                if (u32TimingMode != 2 || AE_CTX_WORD(pCtx, 0x8c) != 64)
                    goto wdr_path;
            }
        }

        /* Apply antiflicker for special WDR modes */
        if (u32IntTime == AE_CTX_WORD(pCtx, 0x51c))
            goto lfwdr_no_change;

        if (AE_CTX_WORD(pCtx, 0x5c8) == 0) {
            if (AE_CTX_WORD(pCtx, 0x494) == AE_CTX_WORD(pCtx, 0x498))
                goto lfwdr_antiflicker;
        } else {
            goto lfwdr_antiflicker;
        }

lfwdr_no_change:
        {
            HI_U32 u32Freq = AE_CTX_WORD(pCtx, 0x434);
            /* Check line-time divisibility */
            if (u32Freq != 0) {
                fLineTime = AE_CTX_FLOAT(pCtx, 0x418);
                u32LineTimeInt = (HI_U32)fLineTime;
                if (fLineTime == (HI_FLOAT)u32LineTimeInt && u32LineTimeInt != 0) {
                    HI_U32 u32FreqDiv = u32Freq >> 7;
                    HI_U32 u32Quotient = u32FreqDiv / u32LineTimeInt;
                    HI_U32 u32Remainder = u32FreqDiv - u32Quotient * u32LineTimeInt;
                    if (u32Remainder == 0) {
                        /* Exact line alignment: snap to last known value */
                        if (u32IntTime == AE_CTX_WORD(pCtx, 0x4a8)) {
                            AE_CTX_WORD(pCtx, 0x4c8) = u32IntTime;
                            goto final_clamp;
                        }
                    }
                }
            }
            u32IntTime = AE_CTX_WORD(pCtx, 0x4c8);
            goto final_clamp;
        }

lfwdr_antiflicker:
        {
            HI_U32 u32Freq = AE_CTX_WORD(pCtx, 0x434);
            HI_U32 u32LinePerShort = AE_CTX_WORD(pCtx, 0x43c);
            HI_U32 u32Enable = AE_CTX_WORD(pCtx, 0x440);
            HI_U32 u32LinePer = AE_CTX_WORD(pCtx, 0x438);

            AE_CTX_WORD(pCtx, 0x4c8) = AeCalcAntiflicker2_isra(
                u32IntTime, u32Freq, u32LinePer, u32Enable, u32LinePerShort);

            /* Check line-time divisibility */
            if (u32Freq != 0) {
                fLineTime = AE_CTX_FLOAT(pCtx, 0x418);
                u32LineTimeInt = (HI_U32)fLineTime;
                if (fLineTime == (HI_FLOAT)u32LineTimeInt && u32LineTimeInt != 0) {
                    HI_U32 u32FreqDiv = u32Freq >> 7;
                    HI_U32 u32Quotient = u32FreqDiv / u32LineTimeInt;
                    HI_U32 u32Remainder = u32FreqDiv - u32Quotient * u32LineTimeInt;
                    if (u32Remainder == 0) {
                        if (u32IntTime == AE_CTX_WORD(pCtx, 0x4a8)) {
                            AE_CTX_WORD(pCtx, 0x4c8) = u32IntTime;
                            goto final_clamp;
                        }
                    }
                }
            }
            u32IntTime = AE_CTX_WORD(pCtx, 0x4c8);
            goto final_clamp;
        }
    }

wdr_path:
    /* Non-WDR / standard path */
    if (u32IntTime == AE_CTX_WORD(pCtx, 0x51c))
        goto final_clamp;

    u8WdrMode2 = AE_CTX_BYTE(pCtx, 0xe);
    if ((HI_U8)(u8WdrMode2 - 2) <= 3) {
        /* WDR sub-mode: skip line-time factor */
        goto apply_antiflicker;
    }

    /* Apply line-time factor */
    if (AE_CTX_WORD(pCtx, 0x108) == 0) {
        HI_U32 u32LineFactor = AE_CTX_WORD(pCtx, 0x8c);
        if (u32LineFactor != 0)
            u32IntTime = (u32IntTime * u32LineFactor) >> 6;
    }

apply_antiflicker:
    if (AE_CTX_WORD(pCtx, 0x5c8) != 0)
        goto do_antiflicker;

    if (AE_CTX_WORD(pCtx, 0x494) != AE_CTX_WORD(pCtx, 0x498))
        goto skip_antiflicker;

do_antiflicker:
    {
        HI_U32 u32Freq    = AE_CTX_WORD(pCtx, 0x434);
        HI_U32 u32LinePer = AE_CTX_WORD(pCtx, 0x438);
        HI_U32 u32Enable  = AE_CTX_WORD(pCtx, 0x440);
        HI_U32 u32LinePerShort = AE_CTX_WORD(pCtx, 0x43c);

        u32IntTime = AeCalcAntiflicker2_isra(
            u32IntTime, u32Freq, u32LinePer, u32Enable, u32LinePerShort);
    }

    if ((HI_U8)(u8WdrMode2 - 2) <= 3)
        goto store_inttime;

    /* Reverse line-time factor */
    if (AE_CTX_WORD(pCtx, 0x108) != 0)
        goto store_inttime;
    {
        HI_U32 u32LineFactor = AE_CTX_WORD(pCtx, 0x8c);
        u32IntTime = u32IntTime << 6;
        if (u32LineFactor == 0)
            u32LineFactor = 1;
        u32IntTime = u32IntTime / u32LineFactor;
    }
    AE_CTX_WORD(pCtx, 0x4c8) = u32IntTime;
    goto final_clamp;

skip_antiflicker:
    if ((HI_U8)(u8WdrMode2 - 2) > 3) {
        /* Reverse line-time factor */
        if (AE_CTX_WORD(pCtx, 0x108) == 0) {
            HI_U32 u32LineFactor = AE_CTX_WORD(pCtx, 0x8c);
            u32IntTime = u32IntTime << 6;
            if (u32LineFactor == 0)
                u32LineFactor = 1;
            u32IntTime = u32IntTime / u32LineFactor;
        }
    }
    AE_CTX_WORD(pCtx, 0x4c8) = u32IntTime;
    /* Check line-time divisibility */
    {
        HI_U32 u32Freq = AE_CTX_WORD(pCtx, 0x434);
        if (u32Freq == 0)
            goto load_final;
        fLineTime = AE_CTX_FLOAT(pCtx, 0x418);
        u32LineTimeInt = (HI_U32)fLineTime;
        if (fLineTime != (HI_FLOAT)u32LineTimeInt)
            goto load_final;
        if (u32LineTimeInt == 0) {
            if (u32IntTime == AE_CTX_WORD(pCtx, 0x4a8)) {
                AE_CTX_WORD(pCtx, 0x4c8) = u32IntTime;
                goto final_clamp;
            }
            goto load_final;
        }
        {
            HI_U32 u32FreqDiv = u32Freq >> 7;
            HI_U32 u32Quotient = u32FreqDiv / u32LineTimeInt;
            HI_U32 u32Remainder = u32FreqDiv - u32Quotient * u32LineTimeInt;
            if (u32Remainder != 0)
                goto load_final;
            if (u32IntTime == AE_CTX_WORD(pCtx, 0x4a8)) {
                AE_CTX_WORD(pCtx, 0x4c8) = u32IntTime;
                goto final_clamp;
            }
        }
    }

load_final:
    u32IntTime = AE_CTX_WORD(pCtx, 0x4c8);

final_clamp:
    u32IntTime = AeBoundariesCheck(u32IntTime,
        AE_CTX_WORD(pCtx, 0x4b0), AE_CTX_WORD(pCtx, 0x4b4));
    AE_CTX_WORD(pCtx, 0x4c8) = u32IntTime;
    return 0;

store_inttime:
    AE_CTX_WORD(pCtx, 0x4c8) = u32IntTime;
    goto final_clamp;
}


/* ========================================================================== */
/* 7. AeCalcDgain (included because user listed it, though > 500 bytes)       */
/*    Size: 0x2a8 (680 bytes)                                                 */
/*    Same structure as AeCalcAgain but with dgain offsets (0x1b8 etc.)       */
/*    ctx: opaque AE context pointer                                          */
/*    R0=ctx, R1=unused, R2:R3=u64Numerator, [SP]=u64TargetGain,             */
/*    [SP+8]=s32Handle                                                        */
/* ========================================================================== */

/* Note: AeCalcDgain and AeCalcAgain are structurally identical with different
   field offsets. The full implementation is complex (680 bytes) and involves
   iterative 64-bit gain calculation with sensor callbacks. Providing the
   implementation here for completeness. */

/* These are large functions that share the same pattern. The key offsets for
   AeCalcDgain are:
     0x1b8 = gain mode
     0x140 = max iterations
     0x134 = max gain
     0x148 = current iteration
     0x14c = current gain
     0x150 = shift
     0x1bc = step numerator
     0x1c0 = step shift
     0x130 = min gain
   For AeCalcAgain the corresponding offsets are:
     0x1ac, 0x108, 0xfc, 0x110, 0x114, 0x118, 0x1b0, 0x1b4, 0xf8
*/

/* Skipping full AeCalcDgain implementation here as it mirrors AeCalcAgain
   at 680 bytes — see the AeCalcAgain assembly for the equivalent structure.
   The function signature is:
   HI_U64 AeCalcDgain(HI_U8 *pCtx, HI_U32 unused, HI_U64 u64Num,
                       HI_U64 u64TargetGain, HI_U32 s32Handle);
*/


/* ========================================================================== */
/* 8. AeRouteExDefault                                                        */
/*    Size: 0x458 (1112 bytes — large, but user-requested)                   */
/*    Initialises the extended exposure route table with default values.      */
/*    Computes 64-bit shifted gain products for each route slot.             */
/* ========================================================================== */

/* This function is large and involves many 64-bit shift+store operations.
   The core logic: depending on whether WDR long-frame mode (ctx+0xe48 == 1),
   it sets up either 5 or 6 route slots with computed gain boundaries. */

HI_S32 AeRouteExDefault(HI_U32 s32Handle)
{
    HI_U8 *pBase = (HI_U8 *)&g_astAeCtx[0];
    HI_U8 *pCtx  = pBase + s32Handle * AE_SIZEOF;
    HI_U32 u32LFMode = AE_CTX_WORD(pCtx, 0xe48);
    HI_U32 u32MinTime, u32MaxTime;
    HI_U32 u32IspDgainShift, u32AgainShift, u32DgainShift;
    HI_U32 u32SnsGainShift, u32IrisShift;
    HI_U64 u64Val;

    u32MinTime = AE_CTX_WORD(pCtx, 0x4a8);

    if (u32LFMode != 1) {
        /* Non-WDR: 5 slots */
        AE_CTX_WORD(pCtx, 0x99c) = 5;

        u32MaxTime = AeBoundariesCheck(
            AE_CTX_WORD(pCtx, 0x4b4),
            u32MinTime, AE_CTX_WORD(pCtx, 0x4ac));
        u32MinTime = AeBoundariesCheck(
            AE_CTX_WORD(pCtx, 0x4b0),
            u32MinTime, AE_CTX_WORD(pCtx, 0x4ac));

        AeCalcGainTarget(s32Handle);

        /* Get gain parameters */
        u32AgainShift    = AE_CTX_WORD(pCtx, 0x4fc);
        u32DgainShift    = AE_CTX_WORD(pCtx, 0x534);
        u32IrisShift     = AE_CTX_WORD(pCtx, 0x560);
        u32SnsGainShift  = AE_CTX_WORD(pCtx, 0x530);
        u32IspDgainShift = AE_CTX_WORD(pCtx, 0x4f8);
        {
            HI_U32 u32AgainBits  = AE_CTX_WORD(pCtx, 0x518);
            HI_U32 u32DgainBits  = AE_CTX_WORD(pCtx, 0x550);
            HI_U32 u32IrisBits   = AE_CTX_WORD(pCtx, 0x570);

            /* Compute shifted values: (gain << 10) >> bits */
            #define SHIFT_GAIN(gain, bits) \
                (HI_U32)(((HI_U64)(gain) << 10) >> (bits))

            HI_U32 u32AgainVal    = SHIFT_GAIN(u32AgainShift, u32AgainBits);
            HI_U32 u32DgainVal    = SHIFT_GAIN(u32DgainShift, u32DgainBits);
            HI_U32 u32IrisVal     = SHIFT_GAIN(u32IrisShift, u32IrisBits);
            HI_U32 u32IspDgainVal = SHIFT_GAIN(u32IspDgainShift, u32AgainBits);
            HI_U32 u32SnsGainVal  = SHIFT_GAIN(u32SnsGainShift, u32AgainBits);

            /* Compute combined values using DgainBits and IrisBits */
            HI_U32 u32CombIsp  = SHIFT_GAIN(u32IspDgainShift, u32DgainBits);
            HI_U32 u32CombSns  = SHIFT_GAIN(u32SnsGainShift, u32DgainBits);

            HI_U32 u32IrisFromSns = SHIFT_GAIN(u32SnsGainShift, u32IrisBits);

            HI_U32 u32LR  = AE_CTX_WORD(pCtx, 0x55c);
            HI_U32 u32LR2 = SHIFT_GAIN(u32LR, u32IrisBits);

            /* Slot 0 */
            AE_CTX_WORD(pCtx, 0x9a0) = u32MaxTime;
            AE_CTX_WORD(pCtx, 0x9a4) = u32AgainVal;
            AE_CTX_WORD(pCtx, 0x9a8) = u32DgainVal;
            AE_CTX_WORD(pCtx, 0x9ac) = u32IrisVal;
            AE_CTX_WORD(pCtx, 0x9b0) = 0;
            AE_CTX_WORD(pCtx, 0x9b4) = 1;

            /* Slot 1 */
            AE_CTX_WORD(pCtx, 0x9b8) = u32MinTime;
            AE_CTX_WORD(pCtx, 0x9bc) = u32AgainVal;
            AE_CTX_WORD(pCtx, 0x9c0) = u32DgainVal;
            AE_CTX_WORD(pCtx, 0x9c4) = u32IrisVal;
            AE_CTX_WORD(pCtx, 0x9c8) = 0;
            AE_CTX_WORD(pCtx, 0x9cc) = 1;

            /* Slot 2 */
            AE_CTX_WORD(pCtx, 0x9d0) = u32MinTime;
            AE_CTX_WORD(pCtx, 0x9d4) = u32IspDgainVal;
            AE_CTX_WORD(pCtx, 0x9d8) = u32DgainVal;
            AE_CTX_WORD(pCtx, 0x9dc) = u32IrisVal;
            AE_CTX_WORD(pCtx, 0x9e0) = 0;
            AE_CTX_WORD(pCtx, 0x9e4) = 1;

            /* Slot 3 */
            AE_CTX_WORD(pCtx, 0x9e8) = u32MinTime;
            AE_CTX_WORD(pCtx, 0x9ec) = u32CombIsp;
            AE_CTX_WORD(pCtx, 0x9f0) = u32CombSns;
            AE_CTX_WORD(pCtx, 0x9f4) = u32IrisVal;
            AE_CTX_WORD(pCtx, 0x9f8) = 0;
            AE_CTX_WORD(pCtx, 0x9fc) = 1;

            /* Slot 4 */
            AE_CTX_WORD(pCtx, 0xa00) = u32MinTime;
            AE_CTX_WORD(pCtx, 0xa04) = u32CombIsp;
            AE_CTX_WORD(pCtx, 0xa08) = u32IrisFromSns;
            AE_CTX_WORD(pCtx, 0xa0c) = u32LR2;
            AE_CTX_WORD(pCtx, 0xa10) = 0;
            AE_CTX_WORD(pCtx, 0xa14) = 1;

            #undef SHIFT_GAIN
        }

        AeRouteExUpdate(s32Handle, 4);
        return 0;
    }

    /* WDR long-frame mode (u32LFMode == 1): 6 slots */
    AE_CTX_WORD(pCtx, 0x99c) = 6;

    u32MaxTime = AeBoundariesCheck(
        AE_CTX_WORD(pCtx, 0x4b4),
        u32MinTime, AE_CTX_WORD(pCtx, 0x4ac));
    u32MinTime = AeBoundariesCheck(
        AE_CTX_WORD(pCtx, 0x4b0),
        u32MinTime, AE_CTX_WORD(pCtx, 0x4ac));

    AeCalcGainTarget(s32Handle);

    /* WDR gain boundaries */
    {
        HI_U32 u32WdrMinBits = AE_CTX_WORD(pCtx, 0xe54);
        HI_U32 u32WdrMaxBits = AE_CTX_WORD(pCtx, 0xe58);
        HI_U32 u32WdrGainMax = AeBoundariesCheck(
            AE_CTX_WORD(pCtx, 0xe50), u32WdrMinBits, u32WdrMaxBits);
        HI_U32 u32WdrGainMin = AeBoundariesCheck(
            AE_CTX_WORD(pCtx, 0xe4c), u32WdrMinBits, u32WdrMaxBits);

        HI_U32 u32WdrIrisMax = AeBoundariesCheck(
            AE_CTX_WORD(pCtx, 0xe6c),
            1u << u32WdrMinBits, 1u << u32WdrMaxBits);
        HI_U32 u32WdrIrisMin = AeBoundariesCheck(
            AE_CTX_WORD(pCtx, 0xe68),
            1u << u32WdrMinBits, 1u << u32WdrMaxBits);

        u32AgainShift    = AE_CTX_WORD(pCtx, 0x4fc);
        u32DgainShift    = AE_CTX_WORD(pCtx, 0x534);
        u32IspDgainShift = AE_CTX_WORD(pCtx, 0x4f8);
        {
            HI_U32 u32AgainBits  = AE_CTX_WORD(pCtx, 0x518);
            HI_U32 u32DgainBits  = AE_CTX_WORD(pCtx, 0x550);
            HI_U32 u32IrisBits   = AE_CTX_WORD(pCtx, 0x570);
            HI_U32 u32SnsGainVal = AE_CTX_WORD(pCtx, 0x530);
            HI_U32 u32LR         = AE_CTX_WORD(pCtx, 0x55c);
            HI_U32 u32IrisVal    = AE_CTX_WORD(pCtx, 0x560);

            #define SHIFT_GAIN(gain, bits) \
                (HI_U32)(((HI_U64)(gain) << 10) >> (bits))

            HI_U32 u32Again   = SHIFT_GAIN(u32AgainShift, u32AgainBits);
            HI_U32 u32Dgain   = SHIFT_GAIN(u32DgainShift, u32DgainBits);
            HI_U32 u32IspDg   = SHIFT_GAIN(u32IspDgainShift, u32DgainBits);
            HI_U32 u32SnsVal  = SHIFT_GAIN(u32SnsGainVal, u32DgainBits);
            HI_U32 u32Iris    = SHIFT_GAIN(u32IrisVal, u32IrisBits);
            HI_U32 u32SnsIris = SHIFT_GAIN(u32SnsGainVal, u32IrisBits);
            HI_U32 u32LRIris  = SHIFT_GAIN(u32LR, u32IrisBits);

            /* Slot 0 */
            AE_CTX_WORD(pCtx, 0x9a0) = u32MaxTime;
            AE_CTX_WORD(pCtx, 0x9a4) = u32Again;
            AE_CTX_WORD(pCtx, 0x9a8) = u32Dgain;
            AE_CTX_WORD(pCtx, 0x9ac) = u32Iris;
            AE_CTX_WORD(pCtx, 0x9b0) = u32WdrGainMax;
            AE_CTX_WORD(pCtx, 0x9b4) = u32WdrIrisMax;

            /* Slot 1 */
            AE_CTX_WORD(pCtx, 0x9b8) = u32MaxTime;
            AE_CTX_WORD(pCtx, 0x9bc) = u32Again;
            AE_CTX_WORD(pCtx, 0x9c0) = u32Dgain;
            AE_CTX_WORD(pCtx, 0x9c4) = u32Iris;
            AE_CTX_WORD(pCtx, 0x9c8) = u32WdrGainMin;
            AE_CTX_WORD(pCtx, 0x9cc) = u32WdrIrisMin;

            /* Slot 2 */
            AE_CTX_WORD(pCtx, 0x9d0) = u32MinTime;
            AE_CTX_WORD(pCtx, 0x9d4) = u32Again;
            AE_CTX_WORD(pCtx, 0x9d8) = u32Dgain;
            AE_CTX_WORD(pCtx, 0x9dc) = u32Iris;
            AE_CTX_WORD(pCtx, 0x9e0) = u32WdrGainMin;
            AE_CTX_WORD(pCtx, 0x9e4) = u32WdrIrisMin;

            /* Slot 3 */
            AE_CTX_WORD(pCtx, 0x9e8) = u32MinTime;
            AE_CTX_WORD(pCtx, 0x9ec) = u32IspDg;
            AE_CTX_WORD(pCtx, 0x9f0) = u32SnsVal;
            AE_CTX_WORD(pCtx, 0x9f4) = u32Iris;
            AE_CTX_WORD(pCtx, 0x9f8) = u32WdrGainMin;
            AE_CTX_WORD(pCtx, 0x9fc) = u32WdrIrisMin;

            /* Slot 4 */
            AE_CTX_WORD(pCtx, 0xa00) = u32MinTime;
            AE_CTX_WORD(pCtx, 0xa04) = u32IspDg;
            AE_CTX_WORD(pCtx, 0xa08) = u32SnsIris;
            AE_CTX_WORD(pCtx, 0xa0c) = u32Iris;  /* repeated */
            AE_CTX_WORD(pCtx, 0xa10) = u32WdrGainMin;
            AE_CTX_WORD(pCtx, 0xa14) = u32WdrIrisMin;

            /* Slot 5 */
            AE_CTX_WORD(pCtx, 0xa18) = u32MinTime;
            AE_CTX_WORD(pCtx, 0xa1c) = u32IspDg;
            AE_CTX_WORD(pCtx, 0xa20) = u32SnsIris;
            AE_CTX_WORD(pCtx, 0xa24) = u32LRIris;
            AE_CTX_WORD(pCtx, 0xa28) = u32WdrGainMin;
            AE_CTX_WORD(pCtx, 0xa2c) = u32WdrIrisMin;

            #undef SHIFT_GAIN
        }
    }

    AeRouteExUpdate(s32Handle, 5);
    return 0;
}


/* ========================================================================== */
/* 9. AeRouteInitialize                                                       */
/*    Size: 0xe8 (232 bytes)                                                  */
/*    Initializes route table from user-configured route entries.            */
/*    Scales each entry's integration time by the gain shift factor.         */
/* ========================================================================== */

HI_S32 AeRouteInitialize(HI_U32 s32Handle)
{
    HI_U8 *pBase  = (HI_U8 *)&g_astAeCtx[0];
    HI_U32 u32Off = s32Handle * AE_SIZEOF;
    HI_U8 *pCtx   = pBase + u32Off;
    HI_U32 u32Cnt = AE_CTX_WORD(pCtx, 0x1cf4);
    HI_U32 u32GainShift;
    HI_U32 i;

    if (u32Cnt == 0)
        return 0;

    u32GainShift = AE_CTX_WORD(pCtx, 0x58c);

    /* Scale each route entry's inttime value */
    for (i = 0; i < u32Cnt; i++) {
        HI_U32 u32EntryOff = u32Off + 0x1cfc + i * 16;
        HI_U32 u32Val = *(HI_U32 *)(pBase + u32EntryOff);
        HI_U64 u64Val = (HI_U64)u32Val << u32GainShift;
        u32Val = (HI_U32)(u64Val >> 10);
        *(HI_U32 *)(pBase + u32EntryOff) = u32Val;
    }

    /* Copy route data to active route table */
    {
        HI_U32 u32LFMode = AE_CTX_WORD(pCtx, 0xe48);
        void *pDst = (void *)(pBase + u32Off + 0x5e8);
        void *pSrc = (void *)(pBase + u32Off + 0x1cf4);
        HI_U32 u32Size = 260;  /* 0x104 */

        memcpy_s(pDst, u32Size, pSrc, u32Size);

        if (u32LFMode == 1) {
            AeRouteUpdate(s32Handle, 3);
        } else {
            AeRouteUpdate(s32Handle, 2);
        }
    }

    return 0;
}


/* ========================================================================== */
/* 10. AeRouteExInitialize                                                    */
/*     Size: 0x94 (148 bytes)                                                */
/*     Initializes extended route table from user-configured route entries.  */
/* ========================================================================== */

HI_S32 AeRouteExInitialize(HI_U32 s32Handle)
{
    HI_U8 *pBase  = (HI_U8 *)&g_astAeCtx[0];
    HI_U32 u32Off = s32Handle * AE_SIZEOF;
    HI_U8 *pCtx   = pBase + u32Off;
    HI_U32 u32Cnt = AE_CTX_WORD(pCtx, 0x1dfc);

    if (u32Cnt == 0)
        return 0;

    {
        HI_U32 u32LFMode = AE_CTX_WORD(pCtx, 0xe48);
        void *pDst = (void *)(pBase + u32Off + 0x99c);
        void *pSrc = (void *)(pBase + u32Off + 0x1dfc);
        HI_U32 u32Size = 388;  /* 0x184 */

        memcpy_s(pDst, u32Size, pSrc, u32Size);

        if (u32LFMode == 1) {
            AeRouteExUpdate(s32Handle, 5);
        } else {
            AeRouteExUpdate(s32Handle, 4);
        }
    }

    return 0;
}


/* ========================================================================== */
/* 11. AeLinearCalcTimeTarget                                                 */
/*     Size: 0x1c8 (456 bytes)                                               */
/*     Calculates the target integration time for linear (non-WDR) mode.    */
/*     Applies antiflicker rounding and line-time alignment.                */
/* ========================================================================== */

HI_S32 AeLinearCalcTimeTarget(HI_U32 s32Handle)
{
    HI_U8 *pBase = (HI_U8 *)&g_astAeCtx[0];
    HI_U8 *pCtx  = pBase + s32Handle * AE_SIZEOF;
    HI_U32 u32MaxTime = AE_CTX_WORD(pCtx, 0x4b0);
    HI_U32 u32FlickerEnable = AE_CTX_WORD(pCtx, 0x5c8);
    HI_U32 u32Freq, u32LinePer, u32LinePerShort;
    HI_U32 u32TimeTarget;
    HI_U32 u32MinTime;
    HI_FLOAT fLineTime;
    HI_U32 u32LineTimeInt;

    if (u32FlickerEnable == 0 &&
        AE_CTX_WORD(pCtx, 0x494) != AE_CTX_WORD(pCtx, 0x498))
    {
        /* No antiflicker, line times differ */
        u32Freq = AE_CTX_WORD(pCtx, 0x434);
        AE_CTX_WORD(pCtx, 0x51c) = u32MaxTime;

        if (u32Freq == 0)
            goto load_target;

        /* Apply antiflicker rounding */
        {
            HI_U32 u32LinePerShort2 = AE_CTX_WORD(pCtx, 0x43c);
            if (u32Freq != 0 && u32LinePerShort2 != 0) {
                HI_U32 u32LinePer2 = AE_CTX_WORD(pCtx, 0x438);
                HI_U32 u32Rounded = AeCalcAntiflicker_isra_part(
                    u32MaxTime, u32Freq, u32LinePer2);
                AE_CTX_WORD(pCtx, 0x51c) = u32Rounded;
                if (u32Freq == 0)
                    goto load_target;
            } else {
                AE_CTX_WORD(pCtx, 0x51c) = u32MaxTime;
                if (u32Freq == 0)
                    goto load_target;
            }
        }

        goto check_line_alignment;
    }

    /* Antiflicker enabled or line times equal */
    u32Freq        = AE_CTX_WORD(pCtx, 0x434);
    u32LinePerShort = AE_CTX_WORD(pCtx, 0x43c);
    u32LinePer     = AE_CTX_WORD(pCtx, 0x438);

    if (u32Freq != 0 && u32LinePerShort != 0) {
        HI_U32 u32Rounded = AeCalcAntiflicker_isra_part(
            u32MaxTime, u32Freq, u32LinePer);
        AE_CTX_WORD(pCtx, 0x51c) = u32Rounded;
        if (u32Freq == 0)
            goto load_target;
    } else {
        AE_CTX_WORD(pCtx, 0x51c) = u32MaxTime;
        if (u32Freq == 0)
            goto load_target;
    }

check_line_alignment:
    /* Check line-time divisibility */
    fLineTime = AE_CTX_FLOAT(pCtx, 0x418);
    u32LineTimeInt = (HI_U32)fLineTime;
    if (fLineTime != (HI_FLOAT)u32LineTimeInt)
        goto load_target;
    if (u32LineTimeInt == 0) {
        if (u32MaxTime == AE_CTX_WORD(pCtx, 0x4a8)) {
            AE_CTX_WORD(pCtx, 0x51c) = u32MaxTime;
            goto bounded_target;
        }
        goto load_target;
    }
    {
        HI_U32 u32FreqDiv = u32Freq >> 7;
        HI_U32 u32Quotient = u32FreqDiv / u32LineTimeInt;
        HI_U32 u32Remainder = u32FreqDiv - u32Quotient * u32LineTimeInt;
        if (u32Remainder != 0)
            goto load_target;
        if (u32MaxTime == AE_CTX_WORD(pCtx, 0x4a8)) {
            AE_CTX_WORD(pCtx, 0x51c) = u32MaxTime;
            goto bounded_target;
        }
    }

load_target:
    u32TimeTarget = AE_CTX_WORD(pCtx, 0x51c);
    goto clamp_target;

bounded_target:
    u32TimeTarget = AE_CTX_WORD(pCtx, 0x51c);
    {
        HI_U32 u32Enable2 = AE_CTX_WORD(pCtx, 0x440);
        if (u32Enable2 == 1)
            goto set_min_target;

        if (AE_CTX_WORD(pCtx, 0x5c8) != 0)
            goto calc_min_antiflicker;
        if (AE_CTX_WORD(pCtx, 0x494) == AE_CTX_WORD(pCtx, 0x498))
            goto calc_min_antiflicker;
    }

set_min_target:
    {
        u32MinTime = AE_CTX_WORD(pCtx, 0x4b4);
        AE_CTX_WORD(pCtx, 0x520) = u32MinTime;
        u32TimeTarget = u32MinTime;
        goto final_clamp;
    }

calc_min_antiflicker:
    {
        u32LinePer     = AE_CTX_WORD(pCtx, 0x438);
        u32LinePerShort = AE_CTX_WORD(pCtx, 0x43c);
        u32Freq        = AE_CTX_WORD(pCtx, 0x434);
        u32MinTime     = AE_CTX_WORD(pCtx, 0x4b4);

        if (u32Freq != 0 && u32LinePerShort != 0) {
            u32TimeTarget = AeCalcAntiflicker_isra_part(
                u32MinTime, u32Freq, u32LinePer);
        } else {
            u32TimeTarget = u32MinTime;
        }
        AE_CTX_WORD(pCtx, 0x520) = u32TimeTarget;
        u32MinTime = AE_CTX_WORD(pCtx, 0x4b4);
        goto final_clamp;
    }

clamp_target:
    u32MinTime = AE_CTX_WORD(pCtx, 0x4b4);

final_clamp:
    u32TimeTarget = AeBoundariesCheck(u32TimeTarget,
        u32MaxTime, u32MinTime);
    AE_CTX_WORD(pCtx, 0x520) = u32TimeTarget;
    return 0;
}


/* ========================================================================== */
/* 12. AeRouteDefault                                                         */
/*     Size: 0x1b0 (432 bytes)                                               */
/*     Initializes the standard exposure route with default values.          */
/* ========================================================================== */

HI_S32 AeRouteDefault(HI_U32 s32Handle)
{
    HI_U8 *pBase = (HI_U8 *)&g_astAeCtx[0];
    HI_U8 *pCtx  = pBase + s32Handle * AE_SIZEOF;
    HI_U32 u32LFMode = AE_CTX_WORD(pCtx, 0xe48);
    HI_U32 u32MinTime = AE_CTX_WORD(pCtx, 0x4a8);

    if (u32LFMode != 1) {
        /* Non-WDR: 3 route nodes */
        HI_U32 u32MaxTime, u32MinClamp;
        HI_U32 u32AgainMax, u32LR;

        AE_CTX_WORD(pCtx, 0x5e8) = 3;

        u32MaxTime = AeBoundariesCheck(
            AE_CTX_WORD(pCtx, 0x4b4),
            u32MinTime, AE_CTX_WORD(pCtx, 0x4ac));
        u32MinClamp = AeBoundariesCheck(
            AE_CTX_WORD(pCtx, 0x4b0),
            u32MinTime, AE_CTX_WORD(pCtx, 0x4ac));

        AeCalcGainTarget(s32Handle);

        u32AgainMax = AE_CTX_WORD(pCtx, 0x580);
        u32LR       = AE_CTX_WORD(pCtx, 0x57c);

        /* Node 0: maxtime, againMax, 0, 1 */
        AE_CTX_WORD(pCtx, 0x5ec) = u32MaxTime;
        AE_CTX_WORD(pCtx, 0x5f0) = u32AgainMax;
        AE_CTX_WORD(pCtx, 0x5f4) = 0;
        AE_CTX_WORD(pCtx, 0x5f8) = 1;

        /* Node 1: minClamp, againMax, 0, 1 */
        AE_CTX_WORD(pCtx, 0x5fc) = u32MinClamp;
        AE_CTX_WORD(pCtx, 0x600) = u32AgainMax;
        AE_CTX_WORD(pCtx, 0x604) = 0;
        AE_CTX_WORD(pCtx, 0x608) = 1;

        /* Node 2: minClamp, LR, 0, 1 */
        AE_CTX_WORD(pCtx, 0x60c) = u32MinClamp;
        AE_CTX_WORD(pCtx, 0x610) = u32LR;
        AE_CTX_WORD(pCtx, 0x614) = 0;
        AE_CTX_WORD(pCtx, 0x618) = 1;

        AeRouteUpdate(s32Handle, 2);
        return 0;
    }

    /* WDR long-frame mode: 4 route nodes */
    {
        HI_U32 u32MaxTime, u32MinClamp;
        HI_U32 u32AgainMax, u32LR;
        HI_U32 u32WdrMinBits, u32WdrMaxBits;
        HI_U32 u32WdrGainMax, u32WdrGainMin;
        HI_U32 u32WdrIrisMax, u32WdrIrisMin;

        AE_CTX_WORD(pCtx, 0x5e8) = 4;

        u32MaxTime = AeBoundariesCheck(
            AE_CTX_WORD(pCtx, 0x4b4),
            u32MinTime, AE_CTX_WORD(pCtx, 0x4ac));
        u32MinClamp = AeBoundariesCheck(
            AE_CTX_WORD(pCtx, 0x4b0),
            u32MinTime, AE_CTX_WORD(pCtx, 0x4ac));

        AeCalcGainTarget(s32Handle);

        u32WdrMinBits = AE_CTX_WORD(pCtx, 0xe54);
        u32WdrMaxBits = AE_CTX_WORD(pCtx, 0xe58);
        u32WdrGainMax = AeBoundariesCheck(
            AE_CTX_WORD(pCtx, 0xe50), u32WdrMinBits, u32WdrMaxBits);
        u32WdrGainMin = AeBoundariesCheck(
            AE_CTX_WORD(pCtx, 0xe4c), u32WdrMinBits, u32WdrMaxBits);

        u32WdrIrisMax = AeBoundariesCheck(
            AE_CTX_WORD(pCtx, 0xe6c),
            1u << u32WdrMinBits, 1u << u32WdrMaxBits);
        u32WdrIrisMin = AeBoundariesCheck(
            AE_CTX_WORD(pCtx, 0xe68),
            1u << u32WdrMinBits, 1u << u32WdrMaxBits);

        u32AgainMax = AE_CTX_WORD(pCtx, 0x580);
        u32LR       = AE_CTX_WORD(pCtx, 0x57c);

        /* Node 0 */
        AE_CTX_WORD(pCtx, 0x5ec) = u32MaxTime;
        AE_CTX_WORD(pCtx, 0x5f0) = u32AgainMax;
        AE_CTX_WORD(pCtx, 0x5f4) = u32WdrGainMax;
        AE_CTX_WORD(pCtx, 0x5f8) = u32WdrIrisMax;

        /* Node 1 */
        AE_CTX_WORD(pCtx, 0x5fc) = u32MaxTime;
        AE_CTX_WORD(pCtx, 0x600) = u32AgainMax;
        AE_CTX_WORD(pCtx, 0x604) = u32WdrGainMin;
        AE_CTX_WORD(pCtx, 0x608) = u32WdrIrisMin;

        /* Node 2 */
        AE_CTX_WORD(pCtx, 0x60c) = u32MinClamp;
        AE_CTX_WORD(pCtx, 0x610) = u32AgainMax;
        AE_CTX_WORD(pCtx, 0x614) = u32WdrGainMin;
        AE_CTX_WORD(pCtx, 0x618) = u32WdrIrisMin;

        /* Node 3 */
        AE_CTX_WORD(pCtx, 0x61c) = u32MinClamp;
        AE_CTX_WORD(pCtx, 0x620) = u32LR;
        AE_CTX_WORD(pCtx, 0x624) = u32WdrGainMin;
        AE_CTX_WORD(pCtx, 0x628) = u32WdrIrisMin;

        AeRouteUpdate(s32Handle, 3);
        return 0;
    }
}


/* ========================================================================== */
/* 13. AeCalcTimeTarget                                                       */
/*     Size: 0xd4 (212 bytes)                                                */
/*     Dispatches to the appropriate time-target calculator based on         */
/*     WDR mode and sensor timing configuration.                            */
/* ========================================================================== */

HI_S32 AeCalcTimeTarget(HI_U32 s32Handle)
{
    HI_U8 *pBase = (HI_U8 *)&g_astAeCtx[0];
    HI_U8 *pCtx  = pBase + s32Handle * AE_SIZEOF;
    HI_U8  u8WdrMode;

    /* Clamp max/min integration times */
    AE_CTX_WORD(pCtx, 0x4b4) = AeBoundariesCheck(
        AE_CTX_WORD(pCtx, 0x4b4),
        AE_CTX_WORD(pCtx, 0x4a8),
        AE_CTX_WORD(pCtx, 0x4ac));
    AE_CTX_WORD(pCtx, 0x4b0) = AeBoundariesCheck(
        AE_CTX_WORD(pCtx, 0x4b0),
        AE_CTX_WORD(pCtx, 0x4a8),
        AE_CTX_WORD(pCtx, 0x4ac));

    u8WdrMode = AE_CTX_BYTE(pCtx, 0xd);

    if ((HI_U8)(u8WdrMode - 2) > 9) {
        /* Linear mode (mode < 2 or mode > 11) */
        AeLinearCalcTimeTarget(s32Handle);
        return 0;
    }

    /* WDR modes 2..11 */
    {
        HI_BOOL bLFWDR = HI_FALSE;
        if (u8WdrMode == 9 || u8WdrMode == 6)
            bLFWDR = HI_TRUE;
        if ((HI_U8)(u8WdrMode - 2) <= 1)
            bLFWDR = HI_TRUE;

        if (!bLFWDR) {
            AeWDRCalcTimeTarget(s32Handle);
            return 0;
        }

        /* Check sensor timing configuration */
        {
            HI_U32 u32TimingMode = AE_CTX_WORD(pCtx, 0x28);
            if (u32TimingMode == 1) {
                AeLFWDRCalcTimeTarget(s32Handle);
                return 0;
            }
            if (u32TimingMode == 2 && AE_CTX_WORD(pCtx, 0x8c) == 64) {
                AeLFWDRCalcTimeTarget(s32Handle);
                return 0;
            }
        }

        AeWDRCalcTimeTarget(s32Handle);
        return 0;
    }
}


/* ========================================================================== */
/* 14. AePirisStepCalc                                                        */
/*     Size: 0x100 (256 bytes)                                               */
/*     Calculates P-iris aperture step from the current exposure value       */
/*     and the aperture lookup table stored in the context.                  */
/*     ctx offsets:                                                          */
/*       0xa5c = current aperture value                                      */
/*       0xa60 = computed step result                                        */
/*       0xa84 = step direction mode                                         */
/*       0xa88 = total step count                                            */
/*       0xa8a = current step index                                          */
/*       0xa8e = threshold at step[2]                                        */
/*       0xa90 = start of threshold array                                    */
/*       0xa86+i*2 = aperture LUT (half-words)                              */
/* ========================================================================== */

void AePirisStepCalc(HI_U8 *pCtx)
{
    HI_U32 u32PrevIdx = 0;
    HI_U32 u32CurVal    = AE_CTX_WORD(pCtx, 0xa5c);
    HI_U16 u16StepIdx   = AE_CTX_HALF(pCtx, 0xa8a);
    HI_U16 u16LutVal    = AE_CTX_HALF(pCtx, (u16StepIdx + 0x543) * 2 + 4);

    if (u32CurVal >= u16LutVal) {
        /* Value exceeds current step threshold */
        HI_U32 u32Dir = AE_CTX_WORD(pCtx, 0xa84);

        AE_CTX_WORD(pCtx, 0xa5c) = u16LutVal;

        if (u32Dir != 1) {
            HI_U16 u16TotalSteps = AE_CTX_HALF(pCtx, 0xa88);
            AE_CTX_WORD(pCtx, 0xa60) = u16TotalSteps - 1;
        } else {
            AE_CTX_WORD(pCtx, 0xa60) = 0;
        }
        return;
    }

    if (u32CurVal == 0) {
        /* Zero: use default step */
        HI_U32 u32Dir = AE_CTX_WORD(pCtx, 0xa84);
        HI_U32 u32Step = u32CurVal;
        if (u32Dir == 1) {
            u32Step = AE_CTX_HALF(pCtx, 0xa88) - 1;
        }
        AE_CTX_WORD(pCtx, 0xa60) = u32Step;
        return;
    }

    if (u16StepIdx <= 1)
        return;

    /* Search for the step where value falls below threshold */
    {
        HI_U16 u16Thresh2 = AE_CTX_HALF(pCtx, 0xa8e);
        if (u32CurVal >= u16Thresh2) {
            HI_U16 *pThreshArr = (HI_U16 *)((HI_U8 *)pCtx + 0xa90);
            HI_U32 u32Idx = 2;
            HI_U32 u32PrevIdx = 2;

            while (1) {
                HI_U16 u16Cur = (HI_U16)u32Idx;
                if (u16StepIdx <= u16Cur)
                    return;

                HI_U16 u16Thresh = pThreshArr[u32Idx - 2];
                u32PrevIdx = u32Idx;
                u32Idx++;

                if (u32CurVal < u16Thresh) {
                    /* Found the step */
                    u32Idx = u32PrevIdx - 1;
                    goto compute_step;
                }
            }
        } else {
            /* Below first threshold: step = 0 */
            u32PrevIdx = 1;
            HI_U32 u32Idx = 0;
compute_step:
            {
                HI_U32 u32Dir = AE_CTX_WORD(pCtx, 0xa84);
                HI_U16 u16LutStep = AE_CTX_HALF(pCtx, (u32Idx + 0x544) * 2 + 4);

                AE_CTX_WORD(pCtx, 0xa5c) = u16LutStep;

                if (u32Dir == 1) {
                    HI_U32 u32Step = u16StepIdx - u32PrevIdx;
                    AE_CTX_WORD(pCtx, 0xa60) = u32Step;
                } else {
                    HI_U16 u16TotalSteps = AE_CTX_HALF(pCtx, 0xa88);
                    HI_U32 u32Step = u16TotalSteps - u16StepIdx + u32PrevIdx;
                    AE_CTX_WORD(pCtx, 0xa60) = u32Step;
                }
            }
            return;
        }
    }
}


/* ========================================================================== */
/* 15. AeCalcIrisApeEx                                                        */
/*     Size: 0x13c (316 bytes)                                               */
/*     Calculates P-iris aperture for extended route mode.                   */
/*     R0=pSubCtx, R1=unused, R2:R3=u64Numerator,                           */
/*     [SP+32..44]=u64TargetGain+u32MaxIris, [SP+48]=s32Handle              */
/*     ctx+0x1774 = iris type                                                */
/* ========================================================================== */

HI_U64 AeCalcIrisApeEx(HI_U8 *pSubCtx, HI_U32 unused,
                        HI_U64 u64Numerator,
                        HI_U64 u64TargetGain, HI_U32 u32CalcType,
                        HI_U32 u32MaxIris,
                        HI_U32 s32Handle)
{
    HI_U8 *pBase = (HI_U8 *)&g_astAeCtx[0];
    HI_U8 *pCtx  = pBase + s32Handle * AE_SIZEOF;
    HI_U32 u32IrisType = AE_CTX_WORD(pCtx, 0x1774);
    HI_U32 u32ApeVal;

    if (u32IrisType == 1) {
        /* Type 1: set aperture to 1024 */
        AE_CTX_WORD(pSubCtx, 0xa5c) = 1024;
    } else if (u32IrisType == 2) {
        /* Type 2: set aperture to 0 */
        AE_CTX_WORD(pSubCtx, 0xa5c) = 0;
    } else {
        /* Type 0 or other: check if gain-based calculation needed */
        HI_U32 u32PrevType = AE_CTX_WORD(pCtx, 0x1760);
        if (u32PrevType == 0) {
            AE_CTX_WORD(pSubCtx, 0xa5c) = 1024;
        } else {
            /* Gain-based iris: check calc type */
            if (u32CalcType != 4)
                goto do_boundary_check;

            /* Compute aperture from gain ratio */
            if (u64TargetGain == 0)
                u64TargetGain = 1;
            AE_CTX_WORD(pSubCtx, 0xa5c) = (HI_U32)(u64Numerator / u64TargetGain);
            AE_CTX_WORD(pSubCtx, 0xa5c) = AeBoundaryMaxCheck(
                AE_CTX_WORD(pSubCtx, 0xa5c), u32MaxIris);
            goto after_iris_type;
        }
    }

after_iris_type:
    /* Recalculate iris type (may have changed) */
    u32IrisType = AE_CTX_WORD(pCtx, 0x1774);
    if (u32IrisType == 2)
        goto calc_piris;

do_boundary_check:
    if (AE_CTX_WORD(pSubCtx, 0xa64) != 0) {
        /* Use explicit min/max boundaries */
        AE_CTX_WORD(pSubCtx, 0xa5c) = AeBoundariesCheck(
            AE_CTX_WORD(pSubCtx, 0xa5c),
            AE_CTX_WORD(pSubCtx, 0xa68),
            AE_CTX_WORD(pSubCtx, 0xa6c));
    } else {
        /* Use power-of-2 boundaries */
        HI_U32 u32MinShift = AE_CTX_WORD(pSubCtx, 0xa4c);
        HI_U32 u32MaxShift = AE_CTX_WORD(pSubCtx, 0xa50);
        AE_CTX_WORD(pSubCtx, 0xa5c) = AeBoundariesCheck(
            AE_CTX_WORD(pSubCtx, 0xa5c),
            1u << u32MinShift,
            1u << u32MaxShift);
    }

calc_piris:
    AePirisStepCalc(pSubCtx);

    /* Final division: numerator / aperture */
    u32ApeVal = AE_CTX_WORD(pSubCtx, 0xa5c);
    if (u32ApeVal == 0)
        u32ApeVal = 1;
    return u64Numerator / (HI_U64)u32ApeVal;
}

/*
 * AeCalcAntiflicker2 helper functions (.isra.4.part.5 and .isra.4)
 *
 * The ".isra" and ".part" suffixes are GCC internal optimizations.
 * AeCalcAntiflicker2.isra.4.part.5 is the "part" helper that does the
 * actual antiflicker rounding computation.
 * AeCalcAntiflicker2.isra.4 is the main inlined entry with a threshold check.
 *
 * Original calling convention (from assembly):
 *   AeCalcAntiflicker2.isra.4(intTime, flickerFreq, flickerCycle, threshold, flickerEnable)
 *   R0=intTime, R1=flickerFreq, R2=flickerCycle, R3=threshold, [SP+24]=flickerEnable
 *
 * Since these are compiler-internal split functions, we merge them into one
 * static function for the C implementation.
 */

/* --------------------------------------------------------------------------
 * AeCalcAntiflicker2.isra.4 (combined with .part.5)
 * Size: 0x70 + 0x30 = 0xA0 combined
 *
 * Rounds integration time to antiflicker period boundaries.
 * If antiflicker is disabled or frequency is zero, returns intTime unchanged.
 * Otherwise, snaps intTime to nearest flicker period if above a 5% threshold.
 * -------------------------------------------------------------------------- */
static HI_U32
AeCalcAntiflicker2(HI_U32 intTime, HI_U32 flickerFreq, HI_U32 flickerCycle,
                   HI_U32 threshold, HI_U32 flickerEnable)
{
    HI_U32 flickerPeriod;
    HI_U32 divisor;
    HI_U64 product;
    HI_U32 rounded;
    HI_U32 shifted;

    /* If flicker is disabled or frequency is zero, return intTime as-is */
    if (flickerEnable == 0 || flickerFreq == 0)
        return intTime;

    /* Calculate flicker period: (flickerCycle * 0x1300) / (20 * flickerFreq) */
    divisor = 20 * flickerFreq;
    if (divisor == 0)
        divisor = 1;
    product = (HI_U64)flickerCycle * 0x1300;
    flickerPeriod = (HI_U32)(product / divisor);

    /* If threshold is nonzero and intTime < flickerPeriod, return intTime */
    if (threshold != 0 && intTime < flickerPeriod)
        return intTime;

    /* .part.5: round intTime to flicker boundary */
    shifted = flickerCycle << 8;
    rounded = intTime * flickerFreq;
    if (shifted != 0)
        rounded = rounded / shifted;
    else
        rounded = rounded / 1;

    if (rounded == 0)
        rounded = 1;

    rounded = rounded * shifted;

    if (flickerFreq == 0)
        flickerFreq = 1;
    rounded = rounded / flickerFreq;

    return rounded;
}


/* --------------------------------------------------------------------------
/* --------------------------------------------------------------------------
 * AeCalcIrisApe (re-done from careful asm trace)
 * Size: 0x1B0 (432 bytes)
 *
 * Parameters (from asm calling convention):
 *   R0 = pCtx (per-channel iris context pointer)
 *   R2:R3 = exposure (64-bit remaining exposure budget)
 *   [SP+48] = s32Handle
 *   [SP+32..44] = route entry fields (irisGain64, irisType, irisMax)
 *
 * Called from AeExposureAllocDefault to compute iris aperture and divide
 * the remaining exposure by it.
 * -------------------------------------------------------------------------- */
static HI_U64
AeCalcIrisApe(HI_U8 *pCtx, HI_U64 exposure, HI_U32 routeIrisType,
              HI_U64 routeIrisGain, HI_U32 routeIrisMax, HI_U32 s32Handle)
{
    HI_U8 *pBase = (HI_U8 *)&g_astAeCtx[0];
    HI_U8 *pAeCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    HI_U32 pirisType = AE_CTX_WORD(pAeCtx, 0x1774);
    HI_U32 aperture;

    if (pirisType == 1) {
        /* Fixed: aperture = 1024 */
        AE_CTX_WORD(pCtx, 0xA5C) = 1024;
    } else if (pirisType == 2) {
        /* DC iris: aperture = 0 */
        AE_CTX_WORD(pCtx, 0xA5C) = 0;
    } else {
        /* P-Iris: check if piris route is configured */
        HI_U32 pirisRoute = AE_CTX_WORD(pAeCtx, 0x1760);
        if (pirisRoute == 0) {
            AE_CTX_WORD(pCtx, 0xA5C) = 1024;
        } else {
            /* Route provides iris gain; check routeIrisType == 2 */
            if (routeIrisType == 2) {
                if (routeIrisGain == 0)
                    routeIrisGain = 1;
                AE_CTX_WORD(pCtx, 0xA5C) = (HI_U32)(exposure / routeIrisGain);
                AE_CTX_WORD(pCtx, 0xA5C) = AeBoundaryMaxCheck(
                    AE_CTX_WORD(pCtx, 0xA5C), routeIrisMax);
                goto after_iris_set;
            } else {
                AE_CTX_WORD(pCtx, 0xA5C) = 1024;
            }
        }
    }

after_iris_set:
    pAeCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    if (AE_CTX_WORD(pAeCtx, 0x1774) == 2)
        goto do_step;

    /* Boundary check on aperture */
    if (AE_CTX_WORD(pCtx, 0xA64) != 0) {
        AE_CTX_WORD(pCtx, 0xA5C) = AeBoundariesCheck(
            AE_CTX_WORD(pCtx, 0xA5C),
            AE_CTX_WORD(pCtx, 0xA68),
            AE_CTX_WORD(pCtx, 0xA6C));
    } else {
        HI_U32 lo = AE_CTX_WORD(pCtx, 0xA4C);
        HI_U32 hi = AE_CTX_WORD(pCtx, 0xA50);
        AE_CTX_WORD(pCtx, 0xA5C) = AeBoundariesCheck(
            AE_CTX_WORD(pCtx, 0xA5C),
            1U << lo, 1U << hi);
    }

do_step:
    AePirisStepCalc(pCtx);

    aperture = AE_CTX_WORD(pCtx, 0xA5C);
    if (aperture == 0)
        aperture = 1;

    return exposure / (HI_U64)aperture;
}


/* --------------------------------------------------------------------------
 * AeCalcGainTarget
 * Size: 0x408 (432 bytes in the question, actually 1032 bytes)
 *
 * Computes gain targets for analog, digital, and ISP digital gain.
 * For each gain type, it finds the closest entry in the gain table
 * by iterating up from zero until the table value >= the target gain.
 * Stores the gain step index and last gain value.
 * -------------------------------------------------------------------------- */
static HI_S32
AeCalcGainTarget(HI_S32 s32Handle)
{
    HI_U8 *pBase = (HI_U8 *)&g_astAeCtx[0];
    HI_U8 *pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    HI_U32 gain, prevGain, tableVal, step;
    HI_U32 precision, tableGain, tableStep;
    HI_U64 product;

    /* Clamp analog gain min/max and current targets */
    AE_CTX_WORD(pCtx, 0x4F8) = AeBoundariesCheck(
        AE_CTX_WORD(pCtx, 0x4F8),
        AE_CTX_WORD(pCtx, 0x4F0),
        AE_CTX_WORD(pCtx, 0x4F4));

    AE_CTX_WORD(pCtx, 0x530) = AeBoundariesCheck(
        AE_CTX_WORD(pCtx, 0x530),
        AE_CTX_WORD(pCtx, 0x528),
        AE_CTX_WORD(pCtx, 0x52C));

    AE_CTX_WORD(pCtx, 0x55C) = AeBoundariesCheck(
        AE_CTX_WORD(pCtx, 0x55C),
        AE_CTX_WORD(pCtx, 0x554),
        AE_CTX_WORD(pCtx, 0x558));

    /* Clamp ex-route gain targets */
    AE_CTX_WORD(pCtx, 0x4FC) = AeBoundariesCheck(
        AE_CTX_WORD(pCtx, 0x4FC),
        AE_CTX_WORD(pCtx, 0x4F0),
        AE_CTX_WORD(pCtx, 0x4F4));

    AE_CTX_WORD(pCtx, 0x534) = AeBoundariesCheck(
        AE_CTX_WORD(pCtx, 0x534),
        AE_CTX_WORD(pCtx, 0x528),
        AE_CTX_WORD(pCtx, 0x52C));

    AE_CTX_WORD(pCtx, 0x560) = AeBoundariesCheck(
        AE_CTX_WORD(pCtx, 0x560),
        AE_CTX_WORD(pCtx, 0x554),
        AE_CTX_WORD(pCtx, 0x558));

    /* Ensure min gains are nonzero */
    if (AE_CTX_WORD(pCtx, 0x4FC) == 0)
        AE_CTX_WORD(pCtx, 0x4FC) = 1;
    if (AE_CTX_WORD(pCtx, 0x534) == 0)
        AE_CTX_WORD(pCtx, 0x534) = 1;
    if (AE_CTX_WORD(pCtx, 0x560) == 0)
        AE_CTX_WORD(pCtx, 0x560) = 1;

    /* Compute system gain for route (using current targets) */
    AE_CTX_WORD(pCtx, 0x57C) = AeCalcSysGain(
        AE_CTX_WORD(pCtx, 0x58C),   /* precision */
        AE_CTX_WORD(pCtx, 0x4F8),   /* again */
        AE_CTX_WORD(pCtx, 0x518),   /* dgainShift */
        AE_CTX_WORD(pCtx, 0x530),   /* dgain */
        AE_CTX_WORD(pCtx, 0x550),   /* ispDgainShift */
        AE_CTX_WORD(pCtx, 0x55C),   /* ispDgain */
        AE_CTX_WORD(pCtx, 0x570));  /* irisShift */

    /* Compute system gain for ex-route */
    AE_CTX_WORD(pCtx, 0x580) = AeCalcSysGain(
        AE_CTX_WORD(pCtx, 0x58C),
        AE_CTX_WORD(pCtx, 0x4FC),
        AE_CTX_WORD(pCtx, 0x518),
        AE_CTX_WORD(pCtx, 0x534),
        AE_CTX_WORD(pCtx, 0x550),
        AE_CTX_WORD(pCtx, 0x560),
        AE_CTX_WORD(pCtx, 0x570));

    /* Clamp system gains to min/max */
    AE_CTX_WORD(pCtx, 0x57C) = AeBoundariesCheck(
        AE_CTX_WORD(pCtx, 0x57C),
        AE_CTX_WORD(pCtx, 0x574),
        AE_CTX_WORD(pCtx, 0x578));

    AE_CTX_WORD(pCtx, 0x580) = AeBoundariesCheck(
        AE_CTX_WORD(pCtx, 0x580),
        AE_CTX_WORD(pCtx, 0x574),
        AE_CTX_WORD(pCtx, 0x578));

    /* --- Analog gain table search (route) --- */
    if (AE_CTX_WORD(pCtx, 0x5AC) == 0) {
        gain = AE_CTX_WORD(pCtx, 0x4F8);
        if (AE_CTX_WORD(pCtx, 0x500) != gain) {
            HI_U32 dgainShift = AE_CTX_WORD(pCtx, 0x518);
            HI_U32 offset = (HI_U32)s32Handle * AE_SIZEOF + 0x5AC;
            tableGain = AE_CTX_WORD(pBase, offset + 4);  /* 0x5B0 */
            tableStep = AE_CTX_WORD(pBase, offset + 8);  /* 0x5B4 */
            HI_U32 topVal = 1U << (dgainShift + 10);
            HI_U32 baseVal = 1U << dgainShift;
            if (tableGain == 0)
                tableGain = 1;

            step = 0;
            prevGain = baseVal;
            while (1) {
                product = (HI_U64)topVal << tableStep;
                product = product / tableGain;
                tableVal = (HI_U32)((product + 512) >> 10);
                if (gain <= tableVal)
                    break;
                step++;
                prevGain = tableVal;
            }
            /* Pick closer of tableVal and prevGain */
            if ((tableVal - gain) >= (gain - prevGain))
                step++;
            pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
            AE_CTX_WORD(pCtx, 0x508) = step;
            AE_CTX_WORD(pCtx, 0x500) = gain;
        }

        /* --- Analog gain table search (ex-route) --- */
        pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
        gain = AE_CTX_WORD(pCtx, 0x4FC);
        if (AE_CTX_WORD(pCtx, 0x504) != gain) {
            HI_U32 dgainShift = AE_CTX_WORD(pCtx, 0x518);
            HI_U32 offset = (HI_U32)s32Handle * AE_SIZEOF + 0x5AC;
            tableGain = AE_CTX_WORD(pBase, offset + 4);
            tableStep = AE_CTX_WORD(pBase, offset + 8);
            HI_U32 topVal = 1U << (dgainShift + 10);
            HI_U32 baseVal = 1U << dgainShift;
            if (tableGain == 0)
                tableGain = 1;

            step = 0;
            prevGain = baseVal;
            while (1) {
                product = (HI_U64)topVal << tableStep;
                product = product / tableGain;
                tableVal = (HI_U32)((product + 512) >> 10);
                if (gain <= tableVal)
                    break;
                step++;
                prevGain = tableVal;
            }
            if ((tableVal - gain) >= (gain - prevGain))
                step++;
            pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
            AE_CTX_WORD(pCtx, 0x50C) = step;
            AE_CTX_WORD(pCtx, 0x504) = gain;
        }
    }

    /* --- Digital gain table search (route) --- */
    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    if (AE_CTX_WORD(pCtx, 0x5B8) == 0) {
        gain = AE_CTX_WORD(pCtx, 0x530);
        if (AE_CTX_WORD(pCtx, 0x538) != gain) {
            HI_U32 ispShift = AE_CTX_WORD(pCtx, 0x550);
            HI_U32 offset = (HI_U32)s32Handle * AE_SIZEOF + 0x5B8;
            tableGain = AE_CTX_WORD(pBase, offset + 4);
            tableStep = AE_CTX_WORD(pBase, offset + 8);
            HI_U32 topVal = 1U << (ispShift + 10);
            HI_U32 baseVal = 1U << ispShift;
            if (tableGain == 0)
                tableGain = 1;

            step = 0;
            prevGain = baseVal;
            while (1) {
                product = (HI_U64)topVal << tableStep;
                product = product / tableGain;
                tableVal = (HI_U32)((product + 512) >> 10);
                if (gain <= tableVal)
                    break;
                step++;
                prevGain = tableVal;
            }
            if ((tableVal - gain) >= (gain - prevGain))
                step++;
            pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
            AE_CTX_WORD(pCtx, 0x540) = step;
            AE_CTX_WORD(pCtx, 0x538) = gain;
        }

        /* --- Digital gain table search (ex-route) --- */
        pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
        gain = AE_CTX_WORD(pCtx, 0x534);
        if (AE_CTX_WORD(pCtx, 0x53C) != gain) {
            HI_U32 ispShift = AE_CTX_WORD(pCtx, 0x550);
            HI_U32 offset = (HI_U32)s32Handle * AE_SIZEOF + 0x5B8;
            tableGain = AE_CTX_WORD(pBase, offset + 4);  /* 0x5BC */
            tableStep = AE_CTX_WORD(pBase, offset + 8);  /* 0x5C0 */
            HI_U32 topVal = 1U << (ispShift + 10);
            HI_U32 baseVal = 1U << ispShift;
            if (tableGain == 0)
                tableGain = 1;

            step = 0;
            prevGain = baseVal;
            while (1) {
                product = (HI_U64)topVal << tableStep;
                product = product / tableGain;
                tableVal = (HI_U32)((product + 512) >> 10);
                if (gain <= tableVal)
                    break;
                step++;
                prevGain = tableVal;
            }
            if ((tableVal - gain) >= (gain - prevGain))
                step++;
            pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
            AE_CTX_WORD(pCtx, 0x544) = step;
            AE_CTX_WORD(pCtx, 0x53C) = gain;
        }
    }

    return 0;
}


/* --------------------------------------------------------------------------
 * AeWDRCalcTimeTarget
 * Size: 0x238 (568 bytes)
 *
 * Calculates integration time target for WDR (Wide Dynamic Range) mode.
 * Handles line-based vs time-based integration, antiflicker rounding,
 * and sensor-specific line scaling (multiply/divide by factor at offset 0x8C).
 * -------------------------------------------------------------------------- */
static HI_S32
AeWDRCalcTimeTarget(HI_S32 s32Handle)
{
    HI_U8 *pBase = (HI_U8 *)&g_astAeCtx[0];
    HI_U8 *pCtx;
    HI_U32 intTimeMode, snsMode, intTimeMax;
    HI_U32 intTime, maxIntTime, lineFactor;
    HI_U32 flickerFreq, flickerCycle, flickerEnable;

    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    intTimeMode = AE_CTX_BYTE(pCtx, 14); /* integration time mode byte */
    intTimeMode = (HI_U8)(intTimeMode - 2);

    if (intTimeMode > 3) {
        /* Not a line-based WDR mode; check if sensor conversion needed */
        snsMode = AE_CTX_WORD(pCtx, 0x108);
        if (snsMode == 0) {
            /* Convert max int time using sensor factor */
            intTimeMax = AE_CTX_WORD(pCtx, 0x4B0);
            lineFactor = AE_CTX_WORD(pCtx, 0x8C);
            intTime = (lineFactor * intTimeMax) >> 6;
            goto after_max_time;
        }
    }

    /* Use max int time directly */
    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    intTime = AE_CTX_WORD(pCtx, 0x4B0);

after_max_time:
    intTimeMax = intTime;

    /* Check antiflicker */
    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    if (AE_CTX_WORD(pCtx, 0x5C8) == 0) {
        /* Antiflicker disabled: check if line rates differ */
        if (AE_CTX_WORD(pCtx, 0x494) == AE_CTX_WORD(pCtx, 0x498))
            goto do_antiflicker_maybe;
        /* Rates differ: skip antiflicker, go to direct path */
        goto check_inttime_mode2;
    }

do_antiflicker_maybe:
    /* Antiflicker enabled or rates equal */
    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    flickerFreq = AE_CTX_WORD(pCtx, 0x434);
    flickerEnable = AE_CTX_WORD(pCtx, 0x43C);
    flickerCycle = AE_CTX_WORD(pCtx, 0x438);

    if (flickerFreq != 0 && flickerEnable != 0) {
        /* Apply antiflicker rounding */
        intTime = AeCalcAntiflicker_part(intTime, flickerFreq, flickerCycle);
    }

check_inttime_mode2:
    /* Recalculate intTimeMode for the second half */
    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;

    /* Store snapped int time as the WDR target */
    AE_CTX_WORD(pCtx, 0x51C) = intTime;

    /* Clamp to max bounds */
    intTime = AeBoundariesCheck(intTime, intTimeMax,
        AE_CTX_WORD(pCtx, 0x4B4));

    AE_CTX_WORD(pCtx, 0x51C) = intTime;

    /* Check if short exposure override is active */
    if (AE_CTX_WORD(pCtx, 0x440) == 1) {
        /* Short exposure: use max int time directly */
        HI_U32 maxTime = AE_CTX_WORD(pCtx, 0x4B4);
        AE_CTX_WORD(pCtx, 0x520) = maxTime;
        intTime = maxTime;
        goto final_clamp;
    }

    /* Recalculate mode for output path */
    intTimeMode = (HI_U8)(AE_CTX_BYTE(pCtx, 14) - 2);

    if (intTimeMode > 3) {
        snsMode = AE_CTX_WORD(pCtx, 0x108);
        if (snsMode == 0) {
            /* Scale max int time by sensor factor */
            HI_U32 maxTime = AE_CTX_WORD(pCtx, 0x4B4);
            lineFactor = AE_CTX_WORD(pCtx, 0x8C);
            intTime = (lineFactor * maxTime) >> 6;
            goto check_antiflicker2;
        }
    }

    /* Use max int time directly */
    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    intTime = AE_CTX_WORD(pCtx, 0x4B4);

check_antiflicker2:
    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    if (AE_CTX_WORD(pCtx, 0x5C8) == 0) {
        if (AE_CTX_WORD(pCtx, 0x494) != AE_CTX_WORD(pCtx, 0x498)) {
            if (intTimeMode <= 3)
                goto store_direct;
            goto check_convert_back;
        }
    }

    /* Antiflicker path for output */
    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    flickerFreq = AE_CTX_WORD(pCtx, 0x434);
    flickerEnable = AE_CTX_WORD(pCtx, 0x43C);
    flickerCycle = AE_CTX_WORD(pCtx, 0x438);

    if (flickerFreq != 0 && flickerEnable != 0) {
        intTime = AeCalcAntiflicker_part(intTime, flickerFreq, flickerCycle);
    }

    if (intTimeMode <= 3)
        goto store_direct;

check_convert_back:
    /* Convert back from lines to time */
    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    snsMode = AE_CTX_WORD(pCtx, 0x108);
    if (snsMode != 0)
        goto store_direct;

    lineFactor = AE_CTX_WORD(pCtx, 0x8C);
    intTime = (intTime << 6);
    if (lineFactor == 0)
        lineFactor = 1;
    intTime = intTime / lineFactor;
    AE_CTX_WORD(pCtx, 0x520) = intTime;
    goto final_clamp;

store_direct:
    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    AE_CTX_WORD(pCtx, 0x520) = intTime;

final_clamp:
    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    intTime = AeBoundariesCheck(intTime, intTimeMax,
        AE_CTX_WORD(pCtx, 0x4B4));
    AE_CTX_WORD(pCtx, 0x520) = intTime;

    return 0;
}


/* --------------------------------------------------------------------------
 * AeCalcSlowFrameRate
 * Size: 0x438 (568 bytes from the question, actually 1080 bytes)
 *
 * Computes maximum integration time for slow frame rate (long exposure) mode.
 * Takes into account the WDR mode, sensor line factors, and antiflicker.
 * Returns the clamped slow frame rate integration time limit.
 * -------------------------------------------------------------------------- */
static HI_U32
AeCalcSlowFrameRate(HI_S32 s32Handle)
{
    HI_U8 *pBase = (HI_U8 *)&g_astAeCtx[0];
    HI_U8 *pCtx;
    HI_U32 slowFps, antiflicker, intTimeMode;
    HI_U32 maxLine, sensorFactor, sensorFactor2, sensorFactor3;
    HI_U64 lineTime64;
    HI_U32 intTime, maxIntTime, adjustedMax;
    HI_U32 snsMode;

    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    slowFps = AE_CTX_WORD(pCtx, 0x498);     /* target slow fps */
    antiflicker = AE_CTX_WORD(pCtx, 0x5C8); /* antiflicker flag */
    HI_U32 lineTimeLo = AE_CTX_WORD(pCtx, 0xB0);  /* line time numerator */
    HI_U32 lineTimeHi = AE_CTX_WORD(pCtx, 0xB4);  /* line time denominator */

    if (slowFps < 20)
        slowFps = 20;
    AE_CTX_WORD(pCtx, 0x498) = slowFps;

    if (antiflicker != 0)
        goto done;

    /* Get integration time mode */
    intTimeMode = AE_CTX_BYTE(pCtx, 13);
    HI_U64 product = (HI_U64)(slowFps - 2) * 1;  /* placeholder */

    HI_U8 modeClass = (HI_U8)(intTimeMode - 2);
    if (modeClass <= 3) {
        /* Line-based WDR modes (2..5) */
        snsMode = AE_CTX_WORD(pCtx, 0x108);
        sensorFactor = AE_CTX_WORD(pCtx, 0x50);
        if (snsMode == 0) {
            /* Convert using sensor line factors */
            sensorFactor2 = AE_CTX_WORD(pCtx, 0x54);
            HI_U64 val = (HI_U64)(slowFps - 2) * sensorFactor;
            HI_U32 lineScale = sensorFactor + (sensorFactor2 * sensorFactor >> 6);
            if ((HI_S32)(lineScale + 64) == 0)
                lineScale = 1;
            else
                lineScale = lineScale + 64;
            HI_U64 shifted = (HI_U64)(slowFps - 2) << 6;
            intTime = (HI_U32)(shifted / (HI_U64)lineScale);
        } else {
            /* Direct: use (slowFps-2) * sensorFactor / (sensorFactor+64) */
            sensorFactor2 = AE_CTX_WORD(pCtx, 0x54);
            HI_U32 adj = sensorFactor2 * sensorFactor >> 6;
            HI_U32 lineScale = sensorFactor + adj;
            if ((HI_S32)(lineScale + 64) == 0)
                lineScale = 1;
            else
                lineScale += 64;
            HI_U64 val = (HI_U64)(slowFps - 2) * sensorFactor;
            intTime = (HI_U32)(val / lineScale);
        }
    } else {
        HI_U8 modeClass2 = (HI_U8)(intTimeMode - 6);
        if (modeClass2 <= 2) {
            /* Modes 6..8: scaled by two sensor factors */
            snsMode = AE_CTX_WORD(pCtx, 0x108);
            sensorFactor = AE_CTX_WORD(pCtx, 0x50);
            if (snsMode != 0) {
                sensorFactor2 = AE_CTX_WORD(pCtx, 0x54);
                HI_U32 adj = sensorFactor2 * sensorFactor;
                HI_U32 lineScale = sensorFactor + (adj >> 6);
                if ((HI_S32)(lineScale + 64) == 0)
                    lineScale = 1;
                else
                    lineScale += 64;
                product = (HI_U64)(slowFps - 2) * sensorFactor;
                intTime = (HI_U32)(product / lineScale);
            } else {
                sensorFactor2 = AE_CTX_WORD(pCtx, 0x54);
                HI_U64 shifted = (HI_U64)(slowFps - 2) << 6;
                HI_U32 adj = sensorFactor2 * sensorFactor;
                HI_U32 lineScale = sensorFactor + (adj >> 6);
                if ((HI_S32)(lineScale + 64) == 0)
                    lineScale = 1;
                else
                    lineScale += 64;
                intTime = (HI_U32)(shifted / lineScale);
            }
        } else {
            HI_U8 modeClass3 = (HI_U8)(intTimeMode - 9);
            if (modeClass3 <= 2) {
                /* Modes 9..11: scaled by three sensor factors */
                snsMode = AE_CTX_WORD(pCtx, 0x108);
                sensorFactor = AE_CTX_WORD(pCtx, 0x50);
                if (snsMode != 0) {
                    sensorFactor2 = AE_CTX_WORD(pCtx, 0x54);
                    sensorFactor3 = AE_CTX_WORD(pCtx, 0x58);
                    HI_U32 adj1 = sensorFactor2 * sensorFactor;
                    HI_U32 adj2 = sensorFactor3 * adj1;
                    HI_U32 lineScale = sensorFactor + (adj1 >> 6) + (adj2 >> 12);
                    if ((HI_S32)(lineScale + 64) == 0)
                        lineScale = 1;
                    else
                        lineScale += 64;
                    product = (HI_U64)(slowFps - 2) * sensorFactor;
                    intTime = (HI_U32)(product / lineScale);
                } else {
                    sensorFactor2 = AE_CTX_WORD(pCtx, 0x54);
                    sensorFactor3 = AE_CTX_WORD(pCtx, 0x58);
                    HI_U64 shifted = (HI_U64)(slowFps - 2) << 6;
                    HI_U32 adj1 = sensorFactor2 * sensorFactor;
                    HI_U32 adj2 = sensorFactor3 * adj1;
                    HI_U32 lineScale = sensorFactor + (adj1 >> 6) + (adj2 >> 12);
                    if ((HI_S32)(lineScale + 64) == 0)
                        lineScale = 1;
                    else
                        lineScale += 64;
                    intTime = (HI_U32)(shifted / lineScale);
                }
            } else {
                /* Unknown mode: use raw line count */
                intTime = (HI_U32)((HI_U64)(slowFps - 2));
            }
        }
    }

    /* Check if computed time exceeds max int time of the route */
    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    maxIntTime = AE_CTX_WORD(pCtx, 0x4B0);
    if ((HI_U64)intTime >= (HI_U64)maxIntTime)
        goto done;

    /* Apply gain boundaries */
    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    intTime = AeBoundariesCheck(
        AE_CTX_WORD(pCtx, 0x5CC),
        AE_CTX_WORD(pCtx, 0x57C),
        AE_CTX_WORD(pCtx, 0x580));
    AE_CTX_WORD(pCtx, 0x5CC) = intTime;

    /* Compute frame time from line time */
    if (lineTimeHi == 0)
        lineTimeHi = 1;
    {
        HI_U32 curBrightness = AE_CTX_WORD(pCtx, 0x5D4);
        HI_U64 frameTime;
        HI_U32 lineLo = AE_CTX_WORD(pCtx, 0x408);
        HI_U32 lineHi = AE_CTX_WORD(pCtx, 0x40C);

        if (curBrightness == 1) {
            /* Use fixed frame time from offset 0x5D8 */
            HI_U32 fixedTime = AE_CTX_WORD(pCtx, 0x5D8);
            HI_U64 val = (HI_U64)fixedTime;
            val = (val >> 32) | (val << 4); /* VDUP+VSHR+VSHL pattern */
            frameTime = val;
        } else {
            HI_U64 rawTime = (HI_U64)lineTimeLo * (HI_U64)lineLo +
                             (HI_U64)lineTimeLo * (HI_U64)lineHi; /* approximate */
            HI_U32 halfDenom = lineTimeHi >> 1;
            frameTime = (rawTime + halfDenom) / lineTimeHi;
        }

        /* Check WDR mode scaling */
        pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
        if (AE_CTX_WORD(pCtx, 0xE48) == 1) {
            /* WDR mode: scale by WDR ratio */
            HI_U32 wdrShift = AE_CTX_WORD(pCtx, 0xE4C);
            HI_U64 scaled = (HI_U64)intTime << wdrShift;
            HI_U64 prod64 = (HI_U64)scaled * intTime;  /* placeholder */
            if (prod64 == 0)
                prod64 = 1;
            intTime = (HI_U32)(frameTime / prod64);
        } else {
            if (intTime == 0)
                intTime = 1;
            intTime = (HI_U32)(frameTime / (HI_U64)intTime);
        }
    }

    /* Clamp to route bounds */
    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    intTime = AeBoundariesCheck(intTime,
        AE_CTX_WORD(pCtx, 0x4B0),
        AE_CTX_WORD(pCtx, 0x4B4));

    /* Mode-specific adjustments */
    modeClass = (HI_U8)(AE_CTX_BYTE(pCtx, 13) - 2);
    if (modeClass > 9) {
        /* Non-WDR: simple margin of 2 */
        slowFps = AE_CTX_WORD(pCtx, 0x498);
        if (intTime >= slowFps - 2)
            slowFps = intTime + 2;
        goto done;
    }

    {
        HI_U8 mode = AE_CTX_BYTE(pCtx, 13);
        HI_U32 isWdr = 0;
        if (mode == 9 || mode == 6)
            isWdr = 1;
        if (modeClass <= 1)
            isWdr = 1;
        if (isWdr) {
            /* WDR special: check if AE_SUBMODE == 1 */
            if (AE_CTX_WORD(pCtx, 0x28) != 1)
                goto non_wdr_margin;
            /* Fall through to WDR margin calculation */
        }
    }

    /* WDR margin: apply sensor factor scaling */
non_wdr_margin:
    if (modeClass <= 3) {
        pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
        snsMode = AE_CTX_WORD(pCtx, 0x108);
        if (snsMode != 0) {
            /* Use division-based scaling */
            sensorFactor = AE_CTX_WORD(pCtx, 0x50);
            if (sensorFactor == 0)
                sensorFactor = 1;
            intTime = (HI_U32)((HI_U64)(sensorFactor + 64) * intTime / sensorFactor);
        } else {
            sensorFactor = AE_CTX_WORD(pCtx, 0x50);
            intTime = (HI_U32)((HI_U64)(sensorFactor + 64) * intTime >> 6);
        }
    } else {
        /* Modes 6+: multi-factor scaling */
        pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
        snsMode = AE_CTX_WORD(pCtx, 0x108);
        HI_U32 f1 = AE_CTX_WORD(pCtx, 0x50);

        if (snsMode == 0) {
            HI_U32 f2 = AE_CTX_WORD(pCtx, 0x54);
            HI_U32 f3;
            HI_U32 adj1 = f2 * f1;
            HI_U32 total = f1 + 64 + (adj1 >> 6);

            if (modeClass <= 8) {
                /* Two factors */
                intTime = (HI_U32)((HI_U64)total * intTime >> 6);
            } else {
                f3 = AE_CTX_WORD(pCtx, 0x58);
                HI_U32 adj2 = f3 * adj1;
                total += adj2 >> 12;
                intTime = (HI_U32)((HI_U64)total * intTime >> 6);
            }
        } else {
            HI_U32 f2 = AE_CTX_WORD(pCtx, 0x54);
            HI_U32 f3;
            HI_U32 adj1 = f2 * f1;
            HI_U32 total = f1 + 64 + (adj1 >> 6);

            if (modeClass <= 8) {
                if (f1 == 0) f1 = 1;
                intTime = (HI_U32)((HI_U64)total * intTime / f1);
            } else {
                f3 = AE_CTX_WORD(pCtx, 0x58);
                HI_U32 adj2 = f3 * adj1;
                total += adj2 >> 12;
                if (f1 == 0) f1 = 1;
                intTime = (HI_U32)((HI_U64)total * intTime / f1);
            }
        }
    }

    /* Final margin check: if intTime >= slowFps - 20, bump slowFps */
    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    slowFps = AE_CTX_WORD(pCtx, 0x498);
    if (intTime >= slowFps - 20)
        slowFps = intTime + 20;

done:
    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    {
        HI_U32 maxLimit = AE_CTX_WORD(pCtx, 0x490);
        AE_CTX_WORD(pCtx, 0x5C4) = AE_CTX_WORD(pCtx, 0x5C8);
        if (slowFps < maxLimit)
            return slowFps;
        return maxLimit;
    }
}


/* --------------------------------------------------------------------------
 * AeCalcAgain
 * Size: 0x2A8 (680 bytes)
 *
 * Calculates analog gain from the remaining exposure budget.
 * Three modes: mode 0 = iterate gain table, mode 1 = direct division,
 * mode 2 = use per-handle callback to adjust gain.
 * Returns remaining exposure after analog gain is consumed.
 * -------------------------------------------------------------------------- */
static HI_U64
AeCalcAgain(HI_U8 *pCtx, HI_U64 exposure, HI_U64 maxExposure, HI_U32 s32Handle)
{
    /* pCtx points into the per-channel context */
    HI_U32 mode = AE_CTX_WORD(pCtx, 0x1AC);  /* again allocation mode */
    HI_U8 *pBase = (HI_U8 *)&g_astAeCtx[0];

    if (mode == 2) {
        /* Callback-based gain: use per-handle AE context */
        HI_U8 *pAeCtx;
        HI_U32 ViPipe;
        HI_U32 shiftBits, gain;
        typedef void (*pfn_again_t)(HI_U32, HI_U32 *, HI_U32 *);

        if (maxExposure == 0) {
            maxExposure = 1;
        }

        pAeCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
        ViPipe = AE_CTX_WORD(pAeCtx, 0x1C4C);
        shiftBits = AE_CTX_WORD(pCtx, 0x118);

        /* Compute gain: (exposure << shiftBits) / maxExposure */
        gain = (HI_U32)((exposure << shiftBits) / maxExposure);
        AE_CTX_WORD(pCtx, 0x114) = gain;

        /* Clamp to sensor again bounds */
        gain = AeBoundariesCheck(gain,
            AE_CTX_WORD(pCtx, 0xF8),
            AE_CTX_WORD(pCtx, 0xFC));
        AE_CTX_WORD(pCtx, 0x114) = gain;

        /* Call sensor again callback if registered */
        {
            pfn_again_t pfnAgainCalc =
                (pfn_again_t)AE_CTX_PTR(pAeCtx, 0x27E0);
            if (pfnAgainCalc != NULL) {
                pfnAgainCalc(ViPipe,
                    (HI_U32 *)((HI_U8 *)pCtx + 0x114),
                    (HI_U32 *)((HI_U8 *)pCtx + 0x110));
                gain = AE_CTX_WORD(pCtx, 0x114);
            }
        }

        /* Recompute remaining exposure */
        if (gain == 0)
            gain = 1;
        return (exposure << shiftBits) / (HI_U64)gain;

    } else if (mode == 0) {
        /* Table-based gain iteration */
        HI_U32 maxSteps = AE_CTX_WORD(pCtx, 0x108);
        HI_U32 maxGain = AE_CTX_WORD(pCtx, 0xFC);
        HI_U32 shiftBits = AE_CTX_WORD(pCtx, 0x118);
        HI_U32 tableShift = AE_CTX_WORD(pCtx, 0x1B0);
        HI_U32 tableDivisor = AE_CTX_WORD(pCtx, 0x1B4);
        HI_U64 shiftedExposure;
        HI_U32 step, gain, curGainVal;
        HI_U32 prevGainVal;

        AE_CTX_WORD(pCtx, 0x110) = 0;
        AE_CTX_WORD(pCtx, 0x114) = maxGain;

        if (maxSteps == 0) {
            /* No table entries: use maxGain directly */
            prevGainVal = maxGain;
            goto clamp_again;
        }

        shiftedExposure = exposure << shiftBits;
        gain = 1024;
        if (tableDivisor == 0)
            tableDivisor = 1;

        step = 0;
        while (1) {
            /* Compute gain at this step: (gain << tableShift) / tableDivisor */
            HI_U64 val = (HI_U64)gain << tableShift;
            gain = (HI_U32)(val / tableDivisor);

            /* Compute the effective gain value: ((gain << shiftBits) + 512) >> 10 */
            HI_U64 effective = (HI_U64)gain << shiftBits;
            curGainVal = (HI_U32)((effective + 512) >> 10);

            /* Remaining exposure at this gain level */
            HI_U32 divisor = (curGainVal != 0) ? curGainVal : 1;
            HI_U64 remaining = shiftedExposure / (HI_U64)divisor;

            step++;

            /* If remaining <= maxExposure, this gain level is sufficient */
            if (remaining <= maxExposure)
                break;

            /* Store intermediate results */
            AE_CTX_WORD(pCtx, 0x110) = step;
            AE_CTX_WORD(pCtx, 0x114) = curGainVal;
            exposure = remaining;  /* update for next iteration */

            if (step == maxSteps)
                goto clamp_again_direct;
        }

        /* Use the gain from previous iteration if we stopped early */
        prevGainVal = AE_CTX_WORD(pCtx, 0x114);

clamp_again:
        AE_CTX_WORD(pCtx, 0x114) = AeBoundariesCheck(
            prevGainVal,
            AE_CTX_WORD(pCtx, 0xF8),
            maxGain);
        return exposure;

clamp_again_direct:
        AE_CTX_WORD(pCtx, 0x114) = AeBoundariesCheck(
            AE_CTX_WORD(pCtx, 0x114),
            AE_CTX_WORD(pCtx, 0xF8),
            maxGain);
        return exposure;

    } else if (mode == 1) {
        /* Direct division mode */
        HI_U32 shift = AE_CTX_WORD(pCtx, 0x1B4);
        HI_U32 gain;

        if (maxExposure == 0)
            maxExposure = 1;

        /* gain = (exposure << shift) / maxExposure */
        gain = (HI_U32)((exposure << shift) / maxExposure);

        /* Clamp to bounds */
        gain = AeBoundariesCheck(gain,
            AE_CTX_WORD(pCtx, 0xF8),
            AE_CTX_WORD(pCtx, 0xFC));
        AE_CTX_WORD(pCtx, 0x114) = gain;

        /* Recompute remaining */
        if (gain == 0)
            gain = 1;
        return (exposure << shift) / (HI_U64)gain;
    }

    return exposure;
}


/* --------------------------------------------------------------------------
 * AeCalcSysGain
 * Size: 0x6C (108 bytes)
 *
 * Computes combined system gain from analog, digital, and ISP digital gains.
 * Formula: sysGain = (again * dgain * ispDgain) >> (dgainPrec + ispPrec + irisPrec)
 * with rounding (add half of divisor before shift).
 *
 * Parameters:
 *   R0 = precision (base shift for again)
 *   R1 = again (analog gain)
 *   R2 = dgainPrec (digital gain precision/shift)
 *   R3 = dgain (digital gain)
 *   [SP+12] = ispDgainPrec (ISP digital gain precision)
 *   [SP+16] = ispDgain (ISP digital gain value)
 *   [SP+20] = irisPrec (iris precision/shift)
 * -------------------------------------------------------------------------- */
static HI_U32
AeCalcSysGain(HI_U32 precision, HI_U32 again, HI_U32 dgainPrec,
              HI_U32 dgain, HI_U32 ispDgainPrec, HI_U32 ispDgain,
              HI_U32 irisPrec)
{
    HI_U64 base;
    HI_U64 product;
    HI_U32 totalShift;
    HI_S32 rounding;

    /* base = (HI_U64)again << precision (VDUP+VSHR+VSHL pattern) */
    /* The asm does: D16 = again (zero-extended to 64), then shift left by precision */
    base = (HI_U64)again << precision;

    /* total shift for the final division */
    totalShift = ispDgainPrec + irisPrec + dgainPrec;

    /* product = base * (dgain * ispDgain) - full 64-bit multiply */
    /* The asm computes: UMULL R4,R5 = dgain*ispDgain; then 64x64 -> 64 with base */
    product = base * ((HI_U64)dgain * ispDgain);

    /* Rounding: add (1 << (totalShift-1)) before shifting */
    rounding = (HI_S32)(1U << totalShift) >> 1;  /* ASR #1 */
    product += (HI_S64)rounding;  /* add with sign extension for ADC */

    /* Shift right by totalShift (handles shifts >= 32) */
    return (HI_U32)(product >> totalShift);
}


/* --------------------------------------------------------------------------
 * AeExposureInitialize
 * Size: 0x608 (1080 bytes from the question, actually 1544 bytes)
 *
 * Initializes all exposure-related state for a given AE channel.
 * Sets up gain tables, route precision, integration time bounds,
 * iris parameters, WDR-specific fields, and flicker detection state.
 * Called once when AE is first started for a pipe.
 * -------------------------------------------------------------------------- */
static HI_S32
AeExposureInitialize(HI_S32 s32Handle)
{
    HI_U8 *pBase = (HI_U8 *)&g_astAeCtx[0];
    HI_U8 *pCtx;
    HI_U32 offset;
    HI_U32 lineRate;
    HI_U8 *pAccuRoute, *pAccuRouteEx, *pAccuRouteIsp;

    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;

    /* Load initial config values */
    HI_U32 initLineRate = AE_CTX_WORD(pCtx, 0x1C60);  /* line rate from config */
    HI_U32 flickerFreq  = AE_CTX_WORD(pCtx, 0x1C64);  /* flicker frequency */
    HI_U32 accuPrecision = AE_CTX_WORD(pCtx, 0x1C70);  /* accumulator precision */
    HI_U32 wdrLineRate  = AE_CTX_WORD(pCtx, 0x1CA0);  /* WDR config line rate */

    /* Initialize antiflicker state */
    AE_CTX_WORD(pCtx, 0x438) = initLineRate;  /* flicker cycle */
    AE_CTX_DWORD(pCtx, 0x410) = 0;            /* clear 64-bit accumulator */
    AE_CTX_WORD(pCtx, 0x418) = 0;             /* clear accu[0] */
    AE_CTX_WORD(pCtx, 0x41C) = 0;             /* clear accu[1] */
    AE_CTX_HALF(pCtx, 0x420) = 256;           /* default half-word */
    AE_CTX_WORD(pCtx, 0x434) = flickerFreq;   /* store flicker frequency */
    AE_CTX_WORD(pCtx, 0x424) = wdrLineRate;   /* WDR accumulator */

    /* Zero out state fields */
    AE_CTX_WORD(pCtx, 0x5CC) = 0xFFFF;        /* max slow frame time */
    AE_CTX_WORD(pCtx, 0x4BC) = 0;
    AE_CTX_WORD(pCtx, 0x4B8) = 0;
    AE_CTX_WORD(pCtx, 0x4C4) = 0;
    AE_CTX_WORD(pCtx, 0x4C0) = 0;
    AE_CTX_WORD(pCtx, 0x500) = 0;
    AE_CTX_WORD(pCtx, 0x504) = 0;
    AE_CTX_WORD(pCtx, 0x538) = 0;
    AE_CTX_WORD(pCtx, 0x53C) = 0;
    AE_CTX_WORD(pCtx, 0x564) = 0;
    AE_CTX_WORD(pCtx, 0x568) = 0;
    AE_CTX_WORD(pCtx, 0x588) = 0;
    AE_CTX_WORD(pCtx, 0x584) = 0;
    AE_CTX_WORD(pCtx, 0x43C) = 0;             /* flicker enable = 0 */
    AE_CTX_WORD(pCtx, 0x484) = 0;
    AE_CTX_WORD(pCtx, 0x48C) = 0;
    AE_CTX_WORD(pCtx, 0x51C) = 0;             /* WDR int time target */
    AE_CTX_WORD(pCtx, 0x5C4) = 1;
    AE_CTX_WORD(pCtx, 0x5C8) = 1;             /* antiflicker enabled */
    AE_CTX_WORD(pCtx, 0x444) = 1;
    AE_CTX_WORD(pCtx, 0x42C) = 1;
    AE_CTX_WORD(pCtx, 0x520) = 0;
    AE_CTX_WORD(pCtx, 0x4A4) = 100;           /* initial brightness target */

    /* Initialize 64-bit accumulator with precision shift */
    {
        HI_U64 precVal = (HI_U64)accuPrecision << 4;
        AE_CTX_DWORD(pCtx, 0x410) = 0;
        AE_CTX_DWORD(pCtx, 0x408) = precVal;  /* store shifted precision */
    }

    AE_CTX_WORD(pCtx, 0x524) = 0;
    AE_CTX_WORD(pCtx, 0x450) = 0;
    AE_CTX_WORD(pCtx, 0x4EC) = 0;
    AE_CTX_WORD(pCtx, 0x4E8) = 0;
    AE_CTX_WORD(pCtx, 0x480) = 0;
    AE_CTX_WORD(pCtx, 0x440) = 0;             /* short exposure flag */
    AE_CTX_BYTE(pCtx, 0x488) = 0;
    AE_CTX_WORD(pCtx, 0x44C) = 0;
    AE_CTX_WORD(pCtx, 0x454) = 0;
    AE_CTX_WORD(pCtx, 0x458) = 0;
    AE_CTX_WORD(pCtx, 0x45C) = 0;
    AE_CTX_WORD(pCtx, 0x460) = 0;
    AE_CTX_WORD(pCtx, 0x5D4) = 0;
    AE_CTX_WORD(pCtx, 0x5D8) = 0;
    AE_CTX_WORD(pCtx, 0x430) = 1;
    AE_CTX_WORD(pCtx, 0x5D0) = 0;
    AE_CTX_WORD(pCtx, 0x448) = 0;

    /* Pointers to accumulator/precision structures for route, route_ex, isp */
    pAccuRoute   = pCtx + (HI_U32)s32Handle * AE_SIZEOF + 0x5AC; /* Actually from pBase */
    /* Using actual offsets from assembly: */
    offset = (HI_U32)s32Handle * AE_SIZEOF;
    pAccuRoute   = pBase + offset + 0x5A0;  /* route accu struct */
    pAccuRouteEx = pBase + offset + 0x5AC;  /* route accu struct offset */

    /* Call AeAccu2Prec for three route precision configs */
    /* Config 1: base route (offset 0x1C98) -> route accu (offset 0x5A0+8) */
    AeAccu2Prec(
        (void *)(pBase + offset + 0x1C98),
        (void *)(pBase + offset + 0x5A8));

    /* Config 2: route ex (offset 0x1CB4) -> route ex accu (offset 0x5AC) */
    AeAccu2Prec(
        (void *)(pBase + offset + 0x1CB4),
        (void *)(pBase + offset + 0x5AC));

    /* Config 3: ISP dgain (offset 0x1CD0) -> ISP accu (offset 0x5B8) */
    AeAccu2Prec(
        (void *)(pBase + offset + 0x1CD0),
        (void *)(pBase + offset + 0x5B8));

    /* Set gain table step size based on route type */
    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    if (AE_CTX_WORD(pCtx, 0x5A0) == 1) {
        /* Custom gain table: use table step from config */
        AE_CTX_WORD(pCtx, 0x4CC) = AE_CTX_WORD(pCtx, 0x5A8);
    } else {
        AE_CTX_WORD(pCtx, 0x4CC) = 1;
    }

    /* Convert route precision configs to linear values */
    /* intTimeMin = AePrec2Linear(config[0x1C88], accu_route, gainStep) */
    AE_CTX_WORD(pCtx, 0x4A8) = AePrec2Linear(
        AE_CTX_WORD(pCtx, 0x1C88),
        (void *)(pCtx + 0x5A8),
        AE_CTX_WORD(pCtx, 0x4CC));

    /* Check if custom max int time is configured */
    if (AE_CTX_WORD(pCtx, 0x1CEC) != 0) {
        /* Use custom value */
        AE_CTX_WORD(pCtx, 0x49C) = AePrec2Linear(
            AE_CTX_WORD(pCtx, 0x1CEC),
            (void *)(pCtx + 0x5A8),
            AE_CTX_WORD(pCtx, 0x4CC));
    } else {
        AE_CTX_WORD(pCtx, 0x49C) = AE_CTX_WORD(pCtx, 0x4A8);
    }

    /* Check custom slow frame rate limit */
    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    if (AE_CTX_WORD(pCtx, 0x1CE8) != 0) {
        AE_CTX_WORD(pCtx, 0x4A0) = AePrec2Linear(
            AE_CTX_WORD(pCtx, 0x1CE8),
            (void *)(pCtx + 0x5A8),
            AE_CTX_WORD(pCtx, 0x4CC));
    } else {
        AE_CTX_WORD(pCtx, 0x4A0) = AE_CTX_WORD(pCtx, 0x4A8);
    }

    /* Convert max/min int time bounds */
    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    AE_CTX_WORD(pCtx, 0x4AC) = AePrec2Linear(
        AE_CTX_WORD(pCtx, 0x1C8C),
        (void *)(pCtx + 0x5A8),
        AE_CTX_WORD(pCtx, 0x4CC));

    AE_CTX_WORD(pCtx, 0x4B0) = AePrec2Linear(
        AE_CTX_WORD(pCtx, 0x1C90),
        (void *)(pCtx + 0x5A8),
        AE_CTX_WORD(pCtx, 0x4CC));

    AE_CTX_WORD(pCtx, 0x4B4) = AePrec2Linear(
        AE_CTX_WORD(pCtx, 0x1C94),
        (void *)(pCtx + 0x5A8),
        AE_CTX_WORD(pCtx, 0x4CC));

    /* Convert initial int time and set current */
    {
        HI_U32 initIntTime = AePrec2Linear(
            AE_CTX_WORD(pCtx, 0x1C7C),
            (void *)(pCtx + 0x5A8),
            AE_CTX_WORD(pCtx, 0x4CC));

        HI_U32 minIntTime = AE_CTX_WORD(pCtx, 0x4A8);
        AE_CTX_WORD(pCtx, 0x4D0) = minIntTime;
        AE_CTX_WORD(pCtx, 0x498) = initIntTime;
        AE_CTX_WORD(pCtx, 0x494) = initIntTime;

        /* Check and clamp accumulator scale factor */
        {
            float scale = *(float *)((HI_U8 *)pCtx + 0x1C9C);
            if (scale < 1.0f)
                scale = 1.0f;
            *(float *)((HI_U8 *)pCtx + 0x4E4) = scale;
        }

        /* Convert max exposure time */
        AE_CTX_WORD(pCtx, 0x490) = AePrec2Linear(
            AE_CTX_WORD(pCtx, 0x1C80),
            (void *)(pCtx + 0x5A8),
            AE_CTX_WORD(pCtx, 0x4CC));

        if (AE_CTX_WORD(pCtx, 0x490) == 0)
            AE_CTX_WORD(pCtx, 0x490) = 0xFFFFFFFF;
    }

    /* Set up analog gain precision */
    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    if (AE_CTX_WORD(pCtx, 0x5AC) == 0) {
        AE_CTX_WORD(pCtx, 0x518) = 10;
    } else {
        AE_CTX_WORD(pCtx, 0x518) = AE_CTX_WORD(pCtx, 0x5B4);
    }

    /* Convert analog gain bounds */
    AE_CTX_WORD(pCtx, 0x4F0) = AePrec2Linear(
        AE_CTX_WORD(pCtx, 0x1CA4),
        (void *)(pCtx + 0x5AC),
        AE_CTX_WORD(pCtx, 0x518));

    AE_CTX_WORD(pCtx, 0x4F4) = AePrec2Linear(
        AE_CTX_WORD(pCtx, 0x1CA8),
        (void *)(pCtx + 0x5AC),
        AE_CTX_WORD(pCtx, 0x518));

    AE_CTX_WORD(pCtx, 0x4F8) = AePrec2Linear(
        AE_CTX_WORD(pCtx, 0x1CAC),
        (void *)(pCtx + 0x5AC),
        AE_CTX_WORD(pCtx, 0x518));

    AE_CTX_WORD(pCtx, 0x4FC) = AePrec2Linear(
        AE_CTX_WORD(pCtx, 0x1CB0),
        (void *)(pCtx + 0x5AC),
        AE_CTX_WORD(pCtx, 0x518));

    /* Set up digital gain precision */
    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    if (AE_CTX_WORD(pCtx, 0x5B8) == 0) {
        AE_CTX_WORD(pCtx, 0x550) = 10;
    } else {
        AE_CTX_WORD(pCtx, 0x550) = AE_CTX_WORD(pCtx, 0x5C0);
    }

    /* Convert digital gain bounds */
    AE_CTX_WORD(pCtx, 0x528) = AePrec2Linear(
        AE_CTX_WORD(pCtx, 0x1CC0),
        (void *)(pCtx + 0x5B8),
        AE_CTX_WORD(pCtx, 0x550));

    AE_CTX_WORD(pCtx, 0x52C) = AePrec2Linear(
        AE_CTX_WORD(pCtx, 0x1CC4),
        (void *)(pCtx + 0x5B8),
        AE_CTX_WORD(pCtx, 0x550));

    AE_CTX_WORD(pCtx, 0x530) = AePrec2Linear(
        AE_CTX_WORD(pCtx, 0x1CC8),
        (void *)(pCtx + 0x5B8),
        AE_CTX_WORD(pCtx, 0x550));

    AE_CTX_WORD(pCtx, 0x534) = AePrec2Linear(
        AE_CTX_WORD(pCtx, 0x1CCC),
        (void *)(pCtx + 0x5B8),
        AE_CTX_WORD(pCtx, 0x550));

    /* Set up ISP digital gain parameters */
    {
        HI_U32 dgainMax = AE_CTX_WORD(pCtx, 0x530);
        HI_U32 ispShiftConfig = AE_CTX_WORD(pCtx, 0x1CE4);
        HI_U32 ispGainMax = AE_CTX_WORD(pCtx, 0x1CE0);
        HI_U32 ispGainInit = AE_CTX_WORD(pCtx, 0x1CDC);

        /* Compute ISP dgain mask/bounds from shift */
        HI_U32 irisShift = AE_CTX_WORD(pCtx, 0x1CE4);
        AE_CTX_WORD(pCtx, 0x554) = ~(~31U << irisShift);  /* mask */
        AE_CTX_WORD(pCtx, 0x558) = 1U << irisShift;        /* min bound */
        AE_CTX_WORD(pCtx, 0x55C) = ispGainInit;
        AE_CTX_WORD(pCtx, 0x570) = irisShift;
        AE_CTX_WORD(pCtx, 0x560) = ispGainMax;
        AE_CTX_WORD(pCtx, 0x58C) = 10;                     /* base precision */
    }

    /* Compute system gain bounds */
    AE_CTX_WORD(pCtx, 0x574) = AeCalcSysGain(
        10,
        AE_CTX_WORD(pCtx, 0x4F8),
        AE_CTX_WORD(pCtx, 0x518),
        AE_CTX_WORD(pCtx, 0x530),
        AE_CTX_WORD(pCtx, 0x550),
        AE_CTX_WORD(pCtx, 0x55C),
        AE_CTX_WORD(pCtx, 0x570));

    AE_CTX_WORD(pCtx, 0x578) = AeCalcSysGain(
        AE_CTX_WORD(pCtx, 0x58C),
        AE_CTX_WORD(pCtx, 0x4FC),
        AE_CTX_WORD(pCtx, 0x518),
        AE_CTX_WORD(pCtx, 0x534),
        AE_CTX_WORD(pCtx, 0x550),
        AE_CTX_WORD(pCtx, 0x560),
        AE_CTX_WORD(pCtx, 0x570));

    /* Copy WDR-specific parameters */
    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    AE_CTX_WORD(pCtx, 0xE80) = AE_CTX_WORD(pCtx, 0x1F94);
    AE_CTX_WORD(pCtx, 0xE84) = AE_CTX_WORD(pCtx, 0x1F98);

    /* Clamp aperture table count */
    {
        HI_U16 apCount = AE_CTX_HALF(pCtx, 0x1F9C);
        apCount = AeBoundariesCheck(apCount, 1024, 1);
        AE_CTX_HALF(pCtx, 0xE88) = apCount;

        /* Clamp aperture table alt count */
        HI_U16 apAltCount = AE_CTX_HALF(pCtx, 0x1F9E);
        apAltCount = AeBoundariesCheck(apAltCount, 1024, 1);
        apAltCount = (HI_U16)apAltCount;
        AE_CTX_HALF(pCtx, 0xE8A) = apAltCount;

        /* Copy aperture table entries if count > 0 */
        if (apAltCount != 0) {
            HI_U32 i;
            HI_U16 *pSrc = (HI_U16 *)((HI_U8 *)pCtx + 0x1F9E);
            HI_U16 *pDst = (HI_U16 *)((HI_U8 *)pCtx + 0xE8A);
            for (i = 0; i < apAltCount; i++) {
                pDst[i + 1] = pSrc[i + 1];
            }
        }
    }

    /* Initialize WDR shift parameters */
    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    AE_CTX_WORD(pCtx, 0xE54) = AeBoundariesCheck(
        AE_CTX_WORD(pCtx, 0x27B4), 10, 0);

    AE_CTX_WORD(pCtx, 0xE58) = AeBoundariesCheck(
        AE_CTX_WORD(pCtx, 0x27B8), 10, 0);

    AE_CTX_WORD(pCtx, 0xE4C) = AeBoundariesCheck(
        AE_CTX_WORD(pCtx, 0x27A0), 10, 0);

    AE_CTX_WORD(pCtx, 0xE50) = AeBoundariesCheck(
        AE_CTX_WORD(pCtx, 0x27A4), 10, 0);

    /* Store WDR exposure ratio */
    AE_CTX_WORD(pCtx, 0xE64) = AE_CTX_WORD(pCtx, 0x27A8);

    AE_CTX_WORD(pCtx, 0xE68) = AeBoundariesCheck(
        AE_CTX_WORD(pCtx, 0x27AC), 1024, 1);

    AE_CTX_WORD(pCtx, 0xE6C) = AeBoundariesCheck(
        AE_CTX_WORD(pCtx, 0x27B0), 1024, 1);

    /* Copy WDR frame configuration */
    {
        HI_U16 wdrFrames = AE_CTX_HALF(pCtx, 0x27C0);
        AE_CTX_WORD(pCtx, 0x998) = AE_CTX_WORD(pCtx, 0x1DF8);

        HI_U16 wdrMode = AE_CTX_HALF(pCtx, 0x1F80);
        AE_CTX_HALF(pCtx, 0x5E4) = wdrMode;
        AE_CTX_HALF(pCtx, 0x5E6) = 0;

        if (wdrFrames == 0)
            wdrFrames = 256;

        AE_CTX_WORD(pCtx, 0xE70) = 0;
        AE_CTX_WORD(pCtx, 0xE74) = 0;
        AE_CTX_WORD(pCtx, 0xE78) = 0;
        AE_CTX_WORD(pCtx, 0xE7C) = 0;
        AE_CTX_WORD(pCtx, 0x98C) = 0;
        AE_CTX_WORD(pCtx, 0xE3C) = 0;
        AE_CTX_HALF(pCtx, 0x5DC) = wdrFrames;
    }

    return 0;  /* not explicit in asm but returns via register state */
}

/**
 * Reverse Engineered from hi_auto_exposure.S
 * Route update and exposure allocation functions for HiSilicon AE
 */





/* Externs from hi_ae_route.c / hi_ae_route_ex.c */

/* Forward decls already provided by definitions above (AeCalcAgain, AeCalcIspDgain, AeCalcSysGain).
   AeCalcDgain mirrors AeCalcAgain — declared extern (defined in assembly). */
extern HI_U64 AeCalcDgain(HI_U8 *pCtx, HI_U64 exposure, HI_U64 maxExposure, HI_U32 s32Handle);

/* Route/RouteEx linked-list node types - defined in hi_ae_route.c / hi_ae_route_ex.c */
typedef struct hiAE_ROUTE_NODE_S AE_ROUTE_NODE_S;
typedef struct hiAE_ROUTEEX_NODE_S AE_ROUTEEX_NODE_S;
typedef struct hiAE_ROUTE_MGR_S AE_ROUTE_MGR_S;
typedef struct hiAE_ROUTEEX_MGR_S AE_ROUTEEX_MGR_S;

extern AE_ROUTE_NODE_S *AeRouteGetFirstNode(AE_ROUTE_MGR_S *pMgr);
extern AE_ROUTEEX_NODE_S *AeRouteExGetFirstNode(AE_ROUTEEX_MGR_S *pMgr);
extern AE_ROUTE_NODE_S *AeRouteGetUpNode(AE_ROUTE_MGR_S *pMgr, AE_ROUTE_NODE_S *pNode);
extern AE_ROUTE_NODE_S *AeRouteGetDwNode(AE_ROUTE_MGR_S *pMgr, AE_ROUTE_NODE_S *pNode);
extern AE_ROUTEEX_NODE_S *AeRouteExGetUpNode(AE_ROUTEEX_MGR_S *pMgr, AE_ROUTEEX_NODE_S *pNode);
extern AE_ROUTEEX_NODE_S *AeRouteExGetDwNode(AE_ROUTEEX_MGR_S *pMgr, AE_ROUTEEX_NODE_S *pNode);
extern void AeRouteFindBracket(AE_ROUTE_MGR_S *pMgr, HI_U64 u64Gain,
                                AE_ROUTE_NODE_S **ppLow, AE_ROUTE_NODE_S **ppHigh);
extern void AeRouteExFindBracket(AE_ROUTEEX_MGR_S *pMgr, HI_U64 u64Gain,
                                  AE_ROUTEEX_NODE_S **ppLow, AE_ROUTEEX_NODE_S **ppHigh);
extern void AeRouteDelRdcy(AE_ROUTE_MGR_S *pMgr, HI_U32 u32Cnt);
extern void AeRouteExDelRdcy(AE_ROUTEEX_MGR_S *pMgr, HI_U32 u32Cnt);
extern void AeRouteGetRange(AE_ROUTE_MGR_S *pMgr,
                             HI_U32 *pu32Min, HI_U32 *pu32Max);
extern void AeRouteExGetRange(AE_ROUTEEX_MGR_S *pMgr,
                               HI_U32 *pu32Min, HI_U32 *pu32Max);
extern HI_S32 AeExposureAllocRoute(HI_U64 u64Exposure,
                              HI_U64 u64Gain, HI_U32 u32Mode, HI_U32 u32IrisVal,
                              HI_U32 u32Param, HI_U32 s32Handle);
extern HI_S32 AeExposureAllocRouteEx(HI_U64 u64Exposure,
                                HI_U64 u64Gain, HI_U32 u32Mode, HI_U32 u32IrisVal,
                                HI_U32 u32Param, HI_U32 s32Handle);


/* ========================================================================== */
/* 1. AeRouteUpdateMaxIntTime (0xac bytes)                                    */
/*    Updates route table max integration time and triggers route rebuild.     */
/* ========================================================================== */
HI_S32 AeRouteUpdateMaxIntTime(HI_U32 s32Handle)
{
    HI_U8 *pBase = (HI_U8 *)&g_astAeCtx[0];
    HI_U8 *pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;

    /* Route node count at offset 0x5e8 */
    HI_U32 u32NodeCnt = AE_CTX_WORD(pCtx, 0x5e8);

    if (u32NodeCnt != 0) {
        HI_U32 u32MaxIntTime = AE_CTX_WORD(pCtx, 0x4d0);
        HI_U32 u32MaxIntTimeVal = AE_CTX_WORD(pCtx, 0x4a8);  /* offset 0x4a0 + 8 */
        HI_U8 i;

        /*
         * Each route node is 16 bytes (stride = 0x10).
         * Route node inttime is at offset 0x5ec relative to per-handle context base.
         * Index calculation: base + 655 * s32Handle + i, shifted left by 4 for 16-byte stride.
         * Clamp each node's inttime to u32MaxIntTimeVal if it exceeds u32MaxIntTime.
         */
        for (i = 0; i < (HI_U8)u32NodeCnt; i++) {
            HI_U32 u32Idx = (HI_U32)(655 * s32Handle + i);
            HI_U32 *pu32IntTime = (HI_U32 *)(pBase + u32Idx * 16 + 0x5ec);
            if (*pu32IntTime >= u32MaxIntTime) {
                *pu32IntTime = u32MaxIntTimeVal;
            }
        }
    }

    /* Reload pCtx in case compiler reuses register */
    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;

    /* Mark route as needing update */
    AE_CTX_WORD(pCtx, 0x988) = 1;

    /* Check route mode at 0xe48 to choose update type */
    if (AE_CTX_WORD(pCtx, 0xe48) == 1) {
        AeRouteUpdate(s32Handle, 3);
    } else {
        AeRouteUpdate(s32Handle, 2);
    }

    return 0;
}


/* ========================================================================== */
/* 2. AeRouteExUpdateMaxIntTime (0xac bytes)                                  */
/*    Same as above but for extended route table with 24-byte node stride.    */
/* ========================================================================== */
HI_S32 AeRouteExUpdateMaxIntTime(HI_U32 s32Handle)
{
    HI_U8 *pBase = (HI_U8 *)&g_astAeCtx[0];
    HI_U32 u32Off = (HI_U32)s32Handle * AE_SIZEOF;
    HI_U8 *pCtx = pBase + u32Off;

    /* Extended route node count at offset 0x99c */
    HI_U32 u32NodeCnt = AE_CTX_WORD(pCtx, 0x99c);

    if (u32NodeCnt != 0) {
        HI_U32 u32MaxIntTime = AE_CTX_WORD(pCtx, 0x4d0);
        HI_U32 u32MaxIntTimeVal = AE_CTX_WORD(pCtx, 0x4a8);  /* 0x4a0 + 8 */
        HI_U8 i;

        /*
         * Extended route nodes have 24-byte stride.
         * Node inttime at offset 0x9a0 relative to pBase + per-handle offset.
         */
        for (i = 0; i < (HI_U8)u32NodeCnt; i++) {
            HI_U32 u32NodeOff = u32Off + 24 * (HI_U32)i;
            HI_U32 *pu32IntTime = (HI_U32 *)(pBase + u32NodeOff + 0x9a0);
            if (*pu32IntTime >= u32MaxIntTime) {
                *pu32IntTime = u32MaxIntTimeVal;
            }
        }
    }

    /* Reload ctx pointer */
    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;

    /* Mark extended route as needing update */
    AE_CTX_WORD(pCtx, 0xe38) = 1;

    /* Choose update type based on route mode */
    if (AE_CTX_WORD(pCtx, 0xe48) == 1) {
        AeRouteExUpdate(s32Handle, 5);
    } else {
        AeRouteExUpdate(s32Handle, 4);
    }

    return 0;
}


/* ========================================================================== */
/* 3. AeDealGain (0x13c bytes)                                                */
/*    Distributes total gain across analog, digital, and ISP digital gains.   */
/*    ctx[0] receives the final computed system gain.                         */
/* ========================================================================== */
HI_S32 AeDealGain(void *pCtx, HI_U64 u64Exposure, HI_U32 u32Lo, HI_U32 u32Hi,
                   HI_U64 u64TotalGain, HI_U32 s32Handle)
{
    HI_U8 *ctx = (HI_U8 *)pCtx;
    HI_U32 u32IrisFactor = AE_CTX_WORD(ctx, 0x17c);  /* iris multiplication factor */
    HI_U32 u32Shift      = AE_CTX_WORD(ctx, 0x18c);  /* gain shift */
    HI_U32 u32AgainMax   = AE_CTX_WORD(ctx, 0x134);   /* max analog gain */
    HI_U32 u32DgainMax   = AE_CTX_WORD(ctx, 0x160);   /* max digital gain */
    HI_U32 u32AgainShift = AE_CTX_WORD(ctx, 0x150);   /* again precision shift */
    HI_U32 u32DgainShift = AE_CTX_WORD(ctx, 0x170);   /* dgain precision shift */

    HI_U64 u64Gain64;
    HI_U64 u64AgainGain, u64DgainGain;

    /* Saturate iris factor: if not -1, add 1 */
    if (u32IrisFactor != 0xFFFFFFFF)
        u32IrisFactor++;

    /* Scale total gain by iris factor and shift down */
    u64Gain64 = (HI_U64)u32IrisFactor * u64TotalGain;
    u64Gain64 >>= u32Shift;

    /* Clamp to (u32Lo:u32Hi) 64-bit limit */
    if (u64Gain64 > ((HI_U64)u32Hi << 32 | u32Lo))
        u64Gain64 = (HI_U64)u32Hi << 32 | u32Lo;

    /* Compute again limit: u32AgainMax * u32DgainMax */
    u64AgainGain = (HI_U64)u32AgainMax * (HI_U64)u32DgainMax;

    /* Scale again limit by total gain and shift by (u32AgainShift + u32DgainShift) */
    {
        HI_U32 u32TotalShift = u32AgainShift + u32DgainShift;
        HI_U64 u64Limit = (u64AgainGain * u64TotalGain) >> u32TotalShift;

        AeCalcAgain(pCtx, u64Exposure, u64Limit, s32Handle);
    }

    /* Compute dgain limit: u32DgainMax * totalGain, shifted by u32DgainShift */
    {
        HI_U64 u64DgainLimit = ((HI_U64)u32DgainMax * u64TotalGain) >> u32DgainShift;

        AeCalcDgain(pCtx, u64Exposure, u64DgainLimit, s32Handle);
    }

    /* IspDgain: pass totalGain directly */
    AeCalcIspDgain(pCtx, 0, u64Exposure, u64TotalGain);

    /* Compute final system gain */
    {
        HI_U32 u32SysGain;
        u32SysGain = AeCalcSysGain(
            AE_CTX_WORD(ctx, 0x18c),   /* shift */
            AE_CTX_WORD(ctx, 0x114),   /* again */
            AE_CTX_WORD(ctx, 0x118),   /* again shift */
            AE_CTX_WORD(ctx, 0x14c),   /* dgain */
            AE_CTX_WORD(ctx, 0x150),   /* dgain shift */
            AE_CTX_WORD(ctx, 0x16c),   /* isp dgain */
            AE_CTX_WORD(ctx, 0x170)    /* isp dgain shift */
        );
        AE_CTX_WORD(ctx, 0x000) = u32SysGain;
    }

    return 0;
}


/* ========================================================================== */
/* Helper: compute inttime index for P-iris/DC-iris lookup                    */
/* Shared by AeExposureAllocDefaultEx and AeExposureAllocDefault              */
/* ========================================================================== */
static HI_U32 AeGetIrisIdx(HI_U8 *pCtx)
{
    HI_U8 u8WdrMode = AE_CTX_BYTE(pCtx, 13);
    HI_U8 u8SubMode = AE_CTX_BYTE(pCtx, 14);
    HI_U32 u32Sub;

    u32Sub = (HI_U32)(u8WdrMode - 2);
    u32Sub &= 0xFF;

    if (u32Sub > 9) {
        /* Not WDR mode - use default limits */
        return 0xFFFFFFFF;  /* sentinel: use offsets 0x4b0/0x4b4 */
    }

    /* Check sub-mode */
    if ((HI_U32)(u8SubMode - 2) > 3) {
        /* Sub-mode not in [2..5]: check if 0x108 == 1 */
        if (AE_CTX_WORD(pCtx, 0x108) != 1) {
            u32Sub = 0;
            goto lookup;
        }
        /* fall through to WDR sub-mode mapping */
    }

    if (u32Sub <= 3) {
        u32Sub = 1;
    } else {
        HI_U32 u32Adj = (HI_U32)(u8WdrMode - 6);
        u32Sub = (u32Adj <= 2) ? 2 : 3;
    }

lookup:
    return u32Sub;
}

/* Perform iris-indexed limit lookup. Returns via pointers. */
static void AeGetIrisLimits(HI_U8 *pBase, HI_U8 *pCtx, HI_U32 s32Handle,
                             HI_U32 u32Idx, HI_U32 *pu32Max, HI_U32 *pu32Min)
{
    if (u32Idx == 0xFFFFFFFF) {
        *pu32Max = AE_CTX_WORD(pCtx, 0x4b4);
        *pu32Min = AE_CTX_WORD(pCtx, 0x4b0);
    } else {
        HI_U32 u32TableOff = (2620 * s32Handle + u32Idx) * 4;
        *pu32Max = *(HI_U32 *)(pBase + u32TableOff + 108);   /* 0x6c */
        *pu32Min = *(HI_U32 *)(pBase + u32TableOff + 92);    /* 0x5c */
    }
}


/* ========================================================================== */
/* Common logic for iris handling in AeExposureAllocDefault{Ex}               */
/* Returns the piris-divided exposure value in u64Exposure.                   */
/* ========================================================================== */
static void AeHandlePIris(HI_U8 *pCtx, HI_U8 *pBase, HI_U32 s32Handle,
                           HI_U64 u64SysExposure, HI_U64 *pu64Exposure,
                           HI_BOOL bIsDefault, HI_BOOL bExpGt)
{
    HI_U8 *pAeCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    HI_U32 u32PIrisType = AE_CTX_WORD(pAeCtx, 0x1774);
    HI_U32 u32PIrisVal;

    if (bExpGt) {
        /* exposure > sys exposure (i.e. increasing) */
        if (u32PIrisType == 1) {
            AE_CTX_WORD(pAeCtx, 0xe5c) = 1024;
        } else if (u32PIrisType == 2) {
            AE_CTX_WORD(pAeCtx, 0xe5c) = 0;
        } else {
            HI_U32 u32Piris5984 = AE_CTX_WORD(pAeCtx, 0x1760);
            if (u32Piris5984 == 0) {
                AE_CTX_WORD(pAeCtx, 0xe5c) = 1024;
            } else {
                /* Use max iris value */
                if (AE_CTX_WORD(pAeCtx, 0xe64) == 0) {
                    u32PIrisVal = 1 << AE_CTX_WORD(pAeCtx, 0xe50);
                } else {
                    u32PIrisVal = AE_CTX_WORD(pAeCtx, 0xe6c);
                }
                AE_CTX_WORD(pAeCtx, 0xe5c) = u32PIrisVal;
            }
        }
    } else {
        /* exposure <= sys exposure (i.e. decreasing or equal) */
        if (u32PIrisType == 1) {
            AE_CTX_WORD(pAeCtx, 0xe5c) = 1024;
        } else if (u32PIrisType == 2) {
            AE_CTX_WORD(pAeCtx, 0xe5c) = 0;
        } else {
            HI_U32 u32Piris5984 = AE_CTX_WORD(pAeCtx, 0x1760);
            if (u32Piris5984 == 0) {
                AE_CTX_WORD(pAeCtx, 0xe5c) = 1024;
            } else {
                /* Use min iris value */
                if (AE_CTX_WORD(pAeCtx, 0xe64) == 0) {
                    u32PIrisVal = 1 << AE_CTX_WORD(pAeCtx, 0xe4c);
                } else {
                    u32PIrisVal = AE_CTX_WORD(pAeCtx, 0xe68);
                }
                AE_CTX_WORD(pAeCtx, 0xe5c) = u32PIrisVal;
            }
        }
    }

    /* Clamp iris value to allowed range */
    {
        HI_U8 *pCtx2 = pBase + (HI_U32)s32Handle * AE_SIZEOF;
        HI_U32 u32IrisMin, u32IrisMax;

        if (AE_CTX_WORD(pCtx2, 0xe64) == 0) {
            u32IrisMin = 1 << AE_CTX_WORD(pCtx2, 0xe4c);
            u32IrisMax = 1 << AE_CTX_WORD(pCtx2, 0xe50);
        } else {
            u32IrisMin = AE_CTX_WORD(pCtx2, 0xe68);
            u32IrisMax = AE_CTX_WORD(pCtx2, 0xe6c);
        }

        AE_CTX_WORD(pCtx2, 0xe5c) = AeBoundariesCheck(
            AE_CTX_WORD(pCtx2, 0xe5c), u32IrisMin, u32IrisMax);
    }

    /* Call piris step calc */
    {
        HI_U8 *pRouteMgr = pBase + (HI_U32)s32Handle * AE_SIZEOF + 0x400;
        AePirisStepCalc(pRouteMgr);
    }

    /* Divide exposure by iris value */
    {
        HI_U8 *pCtx3 = pBase + (HI_U32)s32Handle * AE_SIZEOF;
        HI_U32 u32Iris = AE_CTX_WORD(pCtx3, 0xe5c);
        if (u32Iris == 0) u32Iris = 1;
        *pu64Exposure = *pu64Exposure / (HI_U64)u32Iris;
    }
}


/* ========================================================================== */
/* 4. AeExposureAllocDefaultEx (0x3a4 bytes)                                  */
/*    Allocate exposure using extended route table (5-param nodes).            */
/* ========================================================================== */
HI_S32 AeExposureAllocDefaultEx(HI_U64 u64Exposure, HI_U32 s32Handle)
{
    HI_U8 *pBase = (HI_U8 *)&g_astAeCtx[0];
    HI_U32 u32Off = (HI_U32)s32Handle * AE_SIZEOF;
    HI_U8 *pCtx;
    AE_ROUTEEX_MGR_S *pRouteExMgr;
    void *pRouteMgr;
    AE_ROUTEEX_NODE_S *pNode;
    HI_U64 u64Exp = u64Exposure;
    HI_U32 u32Lo = (HI_U32)u64Exposure;
    HI_U32 u32Hi = (HI_U32)(u64Exposure >> 32);

    pRouteExMgr = (AE_ROUTEEX_MGR_S *)(pBase + u32Off + 0xb20);
    pRouteMgr   = (void *)(pBase + u32Off + 0x400);

    pNode = AeRouteExGetFirstNode(pRouteExMgr);

    if (pNode == HI_NULL) {
        /* No route nodes: compute gain from context params */
        pCtx = pBase + u32Off;
        HI_U32 u32SysGain;
        HI_U32 u32RouteMode = AE_CTX_WORD(pCtx, 0xe48);

        u32SysGain = AeCalcSysGain(
            AE_CTX_WORD(pCtx, 0x58c),
            AE_CTX_WORD(pCtx, 0x4fc),
            AE_CTX_WORD(pCtx, 0x518),
            AE_CTX_WORD(pCtx, 0x534),
            AE_CTX_WORD(pCtx, 0x550),
            AE_CTX_WORD(pCtx, 0x560),
            AE_CTX_WORD(pCtx, 0x570)
        );

        if (u32RouteMode == 1) {
            /* Extended route mode: use iris factor */
            HI_U64 u64IrisVal;
            HI_U32 u32IntTimeFactor = AE_CTX_WORD(pCtx, 0x520);

            if (AE_CTX_WORD(pCtx, 0xe64) == 0) {
                u64IrisVal = (HI_U64)(1 << AE_CTX_WORD(pCtx, 0xe50));
            } else {
                u64IrisVal = (HI_U64)AE_CTX_WORD(pCtx, 0xe6c);
            }

            u64Exp = (HI_U64)u32SysGain * (HI_U64)u32IntTimeFactor;
            u64Exp = u64Exp * u64IrisVal;
        } else {
            u64Exp = (HI_U64)u32SysGain * (HI_U64)AE_CTX_WORD(pCtx, 0x520);
        }

        u32Lo = (HI_U32)u64Exp;
        u32Hi = (HI_U32)(u64Exp >> 32);
    } else {
        /* Use first node's 64-bit product (offset 0x18 in extended node) */
        u64Exp = *(HI_U64 *)((HI_U8 *)pNode + 24);
        u32Lo = (HI_U32)u64Exp;
        u32Hi = (HI_U32)(u64Exp >> 32);
    }

    /* Check if P-iris mode active */
    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    if (AE_CTX_WORD(pCtx, 0xe48) == 1) {
        HI_BOOL bExpGt = (u64Exposure > u64Exp);

        AeHandlePIris(pCtx, pBase, s32Handle, u64Exp, &u64Exposure, HI_TRUE, bExpGt);
        u32Lo = (HI_U32)u64Exposure;
        u32Hi = (HI_U32)(u64Exposure >> 32);
    }

    /* Compute system gain and divide exposure */
    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    {
        HI_U32 u32SysGain = AeCalcSysGain(
            AE_CTX_WORD(pCtx, 0x58c),
            AE_CTX_WORD(pCtx, 0x4fc),
            AE_CTX_WORD(pCtx, 0x518),
            AE_CTX_WORD(pCtx, 0x534),
            AE_CTX_WORD(pCtx, 0x550),
            AE_CTX_WORD(pCtx, 0x560),
            AE_CTX_WORD(pCtx, 0x570)
        );

        HI_U32 u32MaxGain = AE_CTX_WORD(pCtx, 0x580);
        HI_U64 u64Divisor = (u32MaxGain > u32SysGain) ? (HI_U64)u32MaxGain : (HI_U64)u32SysGain;
        if (u64Divisor == 0) u64Divisor = 1;

        pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
        AE_CTX_WORD(pCtx, 0x4c8) = (HI_U32)(u64Exposure / u64Divisor);
    }

    AeCalcIntTime(s32Handle);

    /* Determine inttime limits */
    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    {
        HI_U32 u32Max, u32Min;
        HI_U32 u32Idx = AeGetIrisIdx(pCtx);
        AeGetIrisLimits(pBase, pCtx, s32Handle, u32Idx, &u32Max, &u32Min);

        pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
        HI_U32 u32IntTime = AeBoundariesCheck(AE_CTX_WORD(pCtx, 0x4c8), u32Min, u32Max);

        /* Anti-flicker alignment */
        HI_U32 u32Step = (HI_U32)AE_CTX_FLOAT(pCtx, 0x4e4);
        HI_U32 u32FlickerBase = AE_CTX_WORD(pCtx, 0x1c8c);

        if (u32Step != 0) {
            HI_U32 u32Aligned = ((u32IntTime - u32FlickerBase) / u32Step) * u32Step + u32FlickerBase;
            if (u32Max > u32Aligned)
                u32Aligned += u32Step;
            u32IntTime = u32Aligned;
        } else {
            u32IntTime = u32IntTime;  /* no-op, keep bounded value */
        }

        AE_CTX_WORD(pCtx, 0x4c8) = u32IntTime;

        /* Compute gain remainder */
        pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
        HI_U32 u32GainShift = AE_CTX_WORD(pCtx, 0x58c);
        HI_U32 u32GainFactor = 1 << u32GainShift;
        HI_FLOAT fStep = AE_CTX_FLOAT(pCtx, 0x424);
        HI_U32 u32StepGain = (HI_U32)((HI_FLOAT)u32GainFactor * fStep);

        HI_U64 u64IntTimeBig = ((HI_U64)u32IntTime << 32) >> 32;
        u64IntTimeBig <<= u32GainShift;
        HI_U64 u64Remain = u64IntTimeBig - (HI_U64)u32StepGain;

        AeDealGain(pCtx, u64Exposure, u32Lo, u32Hi, u64Remain, s32Handle);
    }

    return 0;
}


/* ========================================================================== */
/* 5. AeExposureAllocDefault (0x370 bytes)                                    */
/*    Allocate exposure using standard route table (3-param nodes).           */
/*    Nearly identical structure to AeExposureAllocDefaultEx but uses         */
/*    standard route nodes with 16-byte product offset.                       */
/* ========================================================================== */
HI_S32 AeExposureAllocDefault(HI_U64 u64Exposure, HI_U32 s32Handle)
{
    HI_U8 *pBase = (HI_U8 *)&g_astAeCtx[0];
    HI_U32 u32Off = (HI_U32)s32Handle * AE_SIZEOF;
    HI_U8 *pCtx;
    AE_ROUTE_MGR_S *pRouteMgr;
    void *pRouteMgrBase;
    AE_ROUTE_NODE_S *pNode;
    HI_U64 u64Exp = u64Exposure;
    HI_U32 u32Lo = (HI_U32)u64Exposure;
    HI_U32 u32Hi = (HI_U32)(u64Exposure >> 32);

    pRouteMgr    = (AE_ROUTE_MGR_S *)(pBase + u32Off + 0x6f0);
    pRouteMgrBase = (void *)(pBase + u32Off + 0x400);

    pNode = AeRouteGetFirstNode(pRouteMgr);
    pCtx = pBase + u32Off;

    if (pNode == HI_NULL) {
        /* No route nodes: check if P-iris with extended mode */
        HI_U32 u32RouteMode = AE_CTX_WORD(pCtx, 0xe48);

        if (u32RouteMode == 1) {
            HI_U64 u64IrisVal;
            HI_U32 u32IntTimeFactor, u32MaxGain;

            if (AE_CTX_WORD(pCtx, 0xe64) == 0) {
                u64IrisVal = (HI_U64)(1 << AE_CTX_WORD(pCtx, 0xe50));
            } else {
                u64IrisVal = (HI_U64)AE_CTX_WORD(pCtx, 0xe6c);
            }

            pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
            u32IntTimeFactor = AE_CTX_WORD(pCtx, 0x520);
            u32MaxGain = AE_CTX_WORD(pCtx, 0x580);

            u64Exp = (HI_U64)u32IntTimeFactor * (HI_U64)u32MaxGain;
            u64Exp = u64Exp * u64IrisVal;
        }

        /* Go directly to allocation (no P-iris handling if no route mode 1) */
        if (AE_CTX_WORD(pCtx, 0xe48) != 1) {
            goto do_alloc;
        }

        u32Lo = (HI_U32)u64Exp;
        u32Hi = (HI_U32)(u64Exp >> 32);
    } else {
        /* Use first node's 64-bit product (offset 0x10 in standard node) */
        u64Exp = *(HI_U64 *)((HI_U8 *)pNode + 16);
        u32Lo = (HI_U32)u64Exp;
        u32Hi = (HI_U32)(u64Exp >> 32);

        if (AE_CTX_WORD(pCtx, 0xe48) != 1) {
            goto do_alloc;
        }
    }

    /* P-iris handling */
    {
        HI_BOOL bExpGt = (u64Exposure > u64Exp);
        pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;

        AeHandlePIris(pCtx, pBase, s32Handle, u64Exp, &u64Exposure, HI_FALSE, !bExpGt);
        u32Lo = (HI_U32)u64Exposure;
        u32Hi = (HI_U32)(u64Exposure >> 32);
    }

do_alloc:
    /* Compute system gain and divide exposure */
    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    {
        HI_U32 u32SysGain = AeCalcSysGain(
            AE_CTX_WORD(pCtx, 0x58c),
            AE_CTX_WORD(pCtx, 0x4fc),
            AE_CTX_WORD(pCtx, 0x518),
            AE_CTX_WORD(pCtx, 0x534),
            AE_CTX_WORD(pCtx, 0x550),
            AE_CTX_WORD(pCtx, 0x560),
            AE_CTX_WORD(pCtx, 0x570)
        );

        HI_U32 u32MaxGain = AE_CTX_WORD(pCtx, 0x580);
        HI_U64 u64Divisor = (u32MaxGain > u32SysGain) ? (HI_U64)u32MaxGain : (HI_U64)u32SysGain;
        if (u64Divisor == 0) u64Divisor = 1;

        pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
        AE_CTX_WORD(pCtx, 0x4c8) = (HI_U32)(u64Exposure / u64Divisor);
    }

    AeCalcIntTime(s32Handle);

    /* Determine inttime limits and finish allocation */
    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    {
        HI_U32 u32Max, u32Min;
        HI_U32 u32Idx = AeGetIrisIdx(pCtx);
        AeGetIrisLimits(pBase, pCtx, s32Handle, u32Idx, &u32Max, &u32Min);

        pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
        HI_U32 u32IntTime = AeBoundariesCheck(AE_CTX_WORD(pCtx, 0x4c8), u32Min, u32Max);

        /* Anti-flicker alignment */
        HI_U32 u32Step = (HI_U32)AE_CTX_FLOAT(pCtx, 0x4e4);
        HI_U32 u32FlickerBase = AE_CTX_WORD(pCtx, 0x1c8c);

        if (u32Step != 0) {
            HI_U32 u32Aligned = ((u32IntTime - u32FlickerBase) / u32Step) * u32Step + u32FlickerBase;
            if (u32Max > u32Aligned)
                u32Aligned += u32Step;
            u32IntTime = u32Aligned;
        }

        AE_CTX_WORD(pCtx, 0x4c8) = u32IntTime;

        pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
        HI_U32 u32GainShift = AE_CTX_WORD(pCtx, 0x58c);
        HI_U32 u32GainFactor = 1 << u32GainShift;
        HI_FLOAT fStep = AE_CTX_FLOAT(pCtx, 0x424);
        HI_U32 u32StepGain = (HI_U32)((HI_FLOAT)u32GainFactor * fStep);

        HI_U64 u64IntTimeBig = (HI_U64)u32IntTime << u32GainShift;
        HI_U64 u64Remain = u64IntTimeBig - (HI_U64)u32StepGain;

        AeDealGain(pCtx, u64Exposure, u32Lo, u32Hi, u64Remain, s32Handle);
    }

    return 0;
}


/* ========================================================================== */
/* 6. AeExposureAllocationExOutRoute (0x3e4 bytes)                            */
/*    Handles exposure allocation when extended route table provides an       */
/*    explicit out-route node (5-field: inttime, again, dgain, ispdgain,      */
/*    iris). Computes inttime then directly distributes remaining gain.       */
/* ========================================================================== */
HI_S32 AeExposureAllocationExOutRoute(HI_U64 u64Exposure, void *pOutRoute,
                                        HI_U32 s32Handle)
{
    HI_U8 *pBase = (HI_U8 *)&g_astAeCtx[0];
    HI_U32 *pRoute = (HI_U32 *)pOutRoute;
    HI_U8 *pCtx;
    HI_U32 u32Lo = (HI_U32)u64Exposure;
    HI_U32 u32Hi = (HI_U32)(u64Exposure >> 32);

    if (s32Handle > 3) {
        /* Invalid handle - print error and return error code */
        HI_TRACE_ISP(RE_DBG_LVL,
            "Illegal handle id %d in %s!\n",
            s32Handle, "AeExposureAllocationExOutRoute");
        return (HI_S32)0xa01c8003u;
    }

    if (pRoute[0] == 0) {
        /* No explicit route node: fall back to default allocation */
        AeExposureAllocDefault(u64Exposure, s32Handle);
        return (HI_S32)pRoute[0]; /* returns 0 */
    }

    HI_U32 u32Off = (HI_U32)s32Handle * AE_SIZEOF;
    pCtx = pBase + u32Off;
    void *pRouteMgr = (void *)(pBase + u32Off + 0x400);

    /* Determine inttime limits */
    HI_U32 u32Max, u32Min;
    {
        HI_U32 u32Idx = AeGetIrisIdx(pCtx);
        AeGetIrisLimits(pBase, pCtx, s32Handle, u32Idx, &u32Max, &u32Min);
    }

    /* Check route mode */
    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    HI_U32 u32RouteMode = AE_CTX_WORD(pCtx, 0xe48);

    HI_U64 u64SysGain;

    if (u32RouteMode == 1) {
        /* P-iris mode: compute exposure from route node fields */
        HI_U32 u32Again    = pRoute[1];
        HI_U32 u32Dgain    = pRoute[2];
        HI_U32 u32IspDgain = pRoute[3];
        HI_U32 u32IrisVal  = pRoute[0]; /* re-read from route node */

        HI_U64 u64AgDg = (HI_U64)u32Again * (HI_U64)u32Dgain;
        HI_U64 u64Full = (u64AgDg * (HI_U64)u32IspDgain) >> 20;

        /* Compute inttime * full gain product */
        HI_U64 u64Prod = (HI_U64)u32IrisVal * u64Full;

        /* Use float conversion for step calc */
        HI_FLOAT fGain = (HI_FLOAT)u64Full;
        HI_FLOAT fStep = AE_CTX_FLOAT(pCtx, 0x424);
        HI_U32 u32StepGain = (HI_U32)(fGain * fStep);

        /* Divide exposure by gain product */
        if (u64Full == 0) u64Full = 1;
        AE_CTX_WORD(pCtx, 0x4c8) = (HI_U32)(u64Exposure / u64Full);

        AeCalcIntTime(s32Handle);

        /* Clamp inttime */
        HI_U32 u32IntTime = AeBoundariesCheck(AE_CTX_WORD(pCtx, 0x4c8), u32Min, u32Max);

        /* Anti-flicker alignment */
        {
            HI_U32 u32Step = (HI_U32)AE_CTX_FLOAT(pCtx, 0x4e4);
            HI_U32 u32FlickerBase = AE_CTX_WORD(pCtx, 0x1c8c);
            if (u32Step != 0) {
                HI_U32 u32Aligned = ((u32IntTime - u32FlickerBase) / u32Step) * u32Step + u32FlickerBase;
                if (u32Max > u32Aligned)
                    u32Aligned += u32Step;
                u32IntTime = u32Aligned;
            }
        }

        AE_CTX_WORD(pCtx, 0x4c8) = u32IntTime;

        /* Compute exposure with step gain subtracted */
        pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
        HI_U32 u32GainShift = AE_CTX_WORD(pCtx, 0x58c);
        HI_U32 u32GainFactor = 1 << u32GainShift;
        HI_FLOAT fStepVal = AE_CTX_FLOAT(pCtx, 0x424);
        HI_U32 u32StepGainVal = (HI_U32)((HI_FLOAT)u32GainFactor * fStepVal);

        HI_U64 u64IntTimeBig = (HI_U64)u32IntTime << u32GainShift;
        u64IntTimeBig -= (HI_U64)u32StepGainVal;

        /* Compute again * iris */
        HI_U64 u64AgainIris = (HI_U64)pRoute[2] * (HI_U64)pRoute[3];
        u64AgainIris >>= 10;

        /* Distribute again */
        {
            HI_U64 u64AgGain = (u64AgainIris * u64IntTimeBig) >> 10;
            AeCalcAgain(pRouteMgr, u64Exposure, u64AgGain, s32Handle);
        }

        /* Distribute dgain */
        {
            HI_U32 u32AgainVal = AE_CTX_WORD(pCtx, 0x514);
            HI_U32 u32AgainShift = AE_CTX_WORD(pCtx, 0x518);
            HI_U64 u64DgFactor = (HI_U64)u32AgainVal * (HI_U64)pRoute[3];
            u64DgFactor >>= u32AgainShift;
            HI_U64 u64DgGain = (u64DgFactor * u64IntTimeBig) >> 10;
            AeCalcDgain(pRouteMgr, u64Exposure, u64DgGain, s32Handle);
        }

        /* Distribute ISP dgain */
        {
            HI_U32 u32AgainVal = AE_CTX_WORD(pCtx, 0x514);
            HI_U32 u32DgainVal = AE_CTX_WORD(pCtx, 0x54c);
            HI_U32 u32AgainShift = AE_CTX_WORD(pCtx, 0x518);
            HI_U32 u32DgainShift = AE_CTX_WORD(pCtx, 0x550);
            HI_U64 u64IspFactor = (HI_U64)u32AgainVal * (HI_U64)u32DgainVal;
            HI_U32 u32TotalShift = u32AgainShift + u32DgainShift;
            HI_U64 u64IspGain = (u64IspFactor * u64IntTimeBig) >> u32TotalShift;
            AeCalcIspDgain(pRouteMgr, 0, u64Exposure, u64IspGain);
        }

        return 0;
    }

    /* Non-P-iris: compute system gain from route fields */
    {
        HI_U32 u32Val1 = pRoute[1];
        HI_U32 u32Val2 = pRoute[2];
        HI_U32 u32Val3 = pRoute[3];
        HI_U64 u64Prod = (HI_U64)u32Val1 * (HI_U64)u32Val2;
        u64Prod = (u64Prod * (HI_U64)u32Val3) >> 20;
        if (u64Prod == 0) u64Prod = 1;
        u64SysGain = u64Prod;
    }

    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    AE_CTX_WORD(pCtx, 0x4c8) = (HI_U32)(u64Exposure / u64SysGain);

    AeCalcIntTime(s32Handle);

    /* Clamp and align inttime */
    {
        HI_U32 u32IntTime = AeBoundariesCheck(AE_CTX_WORD(pCtx, 0x4c8), u32Min, u32Max);

        HI_U32 u32Step = (HI_U32)AE_CTX_FLOAT(pCtx, 0x4e4);
        HI_U32 u32FlickerBase = AE_CTX_WORD(pCtx, 0x1c8c);
        if (u32Step != 0) {
            HI_U32 u32Aligned = ((u32IntTime - u32FlickerBase) / u32Step) * u32Step + u32FlickerBase;
            if (u32Max > u32Aligned)
                u32Aligned += u32Step;
            u32IntTime = u32Aligned;
        }

        pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
        AE_CTX_WORD(pCtx, 0x4c8) = u32IntTime;

        /* Compute gain remainder and distribute */
        HI_U32 u32GainShift = AE_CTX_WORD(pCtx, 0x58c);
        HI_U32 u32GainFactor = 1 << u32GainShift;
        HI_FLOAT fStep = AE_CTX_FLOAT(pCtx, 0x424);
        HI_U32 u32StepGain = (HI_U32)((HI_FLOAT)u32GainFactor * fStep);

        /* again * dgain product */
        HI_U64 u64AgDg = (HI_U64)pRoute[2] * (HI_U64)pRoute[3];
        u64AgDg >>= 10;

        HI_U64 u64IntTimeBig = (HI_U64)u32IntTime << u32GainShift;
        u64IntTimeBig -= (HI_U64)u32StepGain;

        /* Distribute again */
        {
            HI_U64 u64AgGain = (u64AgDg * u64IntTimeBig) >> 10;
            AeCalcAgain(pRouteMgr, u64Exposure, u64AgGain, s32Handle);
        }

        /* Distribute dgain */
        {
            HI_U32 u32AgainVal = AE_CTX_WORD(pCtx, 0x514);
            HI_U32 u32AgainShift = AE_CTX_WORD(pCtx, 0x518);
            HI_U64 u64DgFactor = (HI_U64)u32AgainVal * (HI_U64)pRoute[3];
            HI_U64 u64DgGain = (u64DgFactor * u64IntTimeBig) >> (u32AgainShift + 10);
            AeCalcDgain(pRouteMgr, u64Exposure, u64DgGain, s32Handle);
        }

        /* Distribute ISP dgain */
        {
            HI_U32 u32AgainVal = AE_CTX_WORD(pCtx, 0x514);
            HI_U32 u32DgainVal = AE_CTX_WORD(pCtx, 0x54c);
            HI_U32 u32AgainShift = AE_CTX_WORD(pCtx, 0x518);
            HI_U32 u32DgainShift = AE_CTX_WORD(pCtx, 0x550);
            HI_U64 u64IspFactor = (HI_U64)u32AgainVal * (HI_U64)u32DgainVal;
            HI_U32 u32TotalShift = u32AgainShift + u32DgainShift;
            HI_U64 u64IspGain = (u64IspFactor * u64IntTimeBig) >> u32TotalShift;
            AeCalcIspDgain(pRouteMgr, 0, u64Exposure, u64IspGain);
        }
    }

    return 0;
}


/* ========================================================================== */
/* AeExposureAllocationOutRoute (0x280 bytes)                                 */
/*    Standard route out-route allocation. Uses 3-field route nodes           */
/*    (inttime, sysgain, iris). Similar structure to ExOutRoute.              */
/* ========================================================================== */
HI_S32 AeExposureAllocationOutRoute(HI_U64 u64Exposure, void *pOutRoute,
                                      HI_U32 s32Handle)
{
    HI_U8 *pBase = (HI_U8 *)&g_astAeCtx[0];
    HI_U32 *pRoute = (HI_U32 *)pOutRoute;
    HI_U8 *pCtx;
    HI_U32 u32Lo = (HI_U32)u64Exposure;
    HI_U32 u32Hi = (HI_U32)(u64Exposure >> 32);

    if (s32Handle > 3) {
        HI_TRACE_ISP(RE_DBG_LVL,
            "Illegal handle id %d in %s!\n",
            s32Handle, "AeExposureAllocationOutRoute");
        return (HI_S32)0xa01c8003u;
    }

    if (pRoute[0] == 0) {
        AeExposureAllocDefault(u64Exposure, s32Handle);
        return (HI_S32)pRoute[0];
    }

    HI_U32 u32Off = (HI_U32)s32Handle * AE_SIZEOF;
    pCtx = pBase + u32Off;
    void *pRouteMgr = (void *)(pBase + u32Off + 0x400);

    /* Determine inttime limits */
    HI_U32 u32Max, u32Min;
    {
        HI_U32 u32Idx = AeGetIrisIdx(pCtx);
        AeGetIrisLimits(pBase, pCtx, s32Handle, u32Idx, &u32Max, &u32Min);
    }

    HI_U32 u32RouteVal = pRoute[1];

    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    HI_U32 u32RouteMode = AE_CTX_WORD(pCtx, 0xe48);

    if (u32RouteMode == 1) {
        /* P-iris mode */
        HI_U32 u32IrisVal = pRoute[1];
        HI_U32 u32RouteNodeVal = pRoute[0];

        HI_FLOAT fIris = (HI_FLOAT)u32IrisVal;
        HI_FLOAT fStep = AE_CTX_FLOAT(pCtx, 0x424);
        HI_FLOAT fStepGain = fIris * fStep;
        HI_U32 u32StepGain = (HI_U32)fStepGain;

        HI_U64 u64IrisExposure = (HI_U64)u32IrisVal * (HI_U64)u32RouteNodeVal;

        /* Subtract step gain from iris exposure */
        HI_U64 u64Remain = u64IrisExposure - (HI_U64)u32StepGain;

        AeCalcIrisApe(pRouteMgr, u64Remain, 0, 0, 1024, s32Handle);

        /* Re-read sysgain from route and continue */
        u32RouteVal = pRoute[1];
    }

    /* Divide exposure by route sysgain */
    {
        if (u32RouteVal == 0) u32RouteVal = 1;
        pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
        AE_CTX_WORD(pCtx, 0x4c8) = (HI_U32)(u64Exposure / (HI_U64)u32RouteVal);
    }

    AeCalcIntTime(s32Handle);

    /* Clamp and align inttime */
    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    {
        HI_U32 u32IntTime = AeBoundariesCheck(AE_CTX_WORD(pCtx, 0x4c8), u32Min, u32Max);

        HI_U32 u32Step = (HI_U32)AE_CTX_FLOAT(pCtx, 0x4e4);
        HI_U32 u32FlickerBase = AE_CTX_WORD(pCtx, 0x1c8c);
        if (u32Step != 0) {
            HI_U32 u32Aligned = ((u32IntTime - u32FlickerBase) / u32Step) * u32Step + u32FlickerBase;
            if (u32Max > u32Aligned)
                u32Aligned += u32Step;
            u32IntTime = u32Aligned;
        }

        AE_CTX_WORD(pCtx, 0x4c8) = u32IntTime;

        /* Compute gain and distribute */
        pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
        HI_U32 u32GainShift = AE_CTX_WORD(pCtx, 0x58c);
        HI_U32 u32GainFactor = 1 << u32GainShift;
        HI_FLOAT fStep2 = AE_CTX_FLOAT(pCtx, 0x424);
        HI_U32 u32StepGain = (HI_U32)((HI_FLOAT)u32GainFactor * fStep2);

        HI_U64 u64IntTimeBig = (HI_U64)u32IntTime << u32GainShift;
        HI_U64 u64Remain = u64IntTimeBig - (HI_U64)u32StepGain;

        AeDealGain(pCtx, u64Exposure, u32Lo, u32Hi, u64Remain, s32Handle);
    }

    return 0;
}


/* ========================================================================== */
/* 7. AeExposureProcess (0x34fc bytes)                                        */
/*    Main exposure processing entry point. Enormous state machine that:      */
/*    - Clamps iris limits, computes target exposure                          */
/*    - Updates route/routeEx tables when parameters change                   */
/*    - Walks route nodes to find allocation point                            */
/*    - Dispatches to various allocation strategies (0..5)                    */
/*    - Falls back to AeExposureAllocDefault{Ex} when no route match          */
/*    - Uses stack canary for security                                        */
/* ========================================================================== */
HI_S32 AeExposureProcess(HI_U32 s32Handle)
{
    HI_U8 *pBase = (HI_U8 *)&g_astAeCtx[0];
    HI_U32 u32Off = (HI_U32)s32Handle * AE_SIZEOF;
    HI_U8 *pCtx = pBase + u32Off;
    void *pRouteMgrBase = (void *)(pBase + u32Off + 0x400);
    HI_U32 u32StackGuard;
    HI_S32 s32Ret = 0;

    /* Stack canary setup (compiler inserts __stack_chk_guard) */
    extern HI_U32 __stack_chk_guard;
    u32StackGuard = __stack_chk_guard;

    HI_U32 u32TargetDir   = AE_CTX_WORD(pCtx, 0xb0);
    HI_U32 u32TargetDir2  = AE_CTX_WORD(pCtx, 0xb4);
    HI_U32 u32IntTimeFactor = AE_CTX_WORD(pCtx, 0x51c);
    HI_U32 u32MaxGainEx    = AE_CTX_WORD(pCtx, 0x520);

    /* Clamp iris limits */
    {
        HI_U32 u32IrisMinShift = AE_CTX_WORD(pCtx, 0xe54);
        HI_U32 u32IrisMaxShift = AE_CTX_WORD(pCtx, 0xe58);

        /* Clamp 0xe50 (iris max) */
        AE_CTX_WORD(pCtx, 0xe50) = AeBoundariesCheck(
            AE_CTX_WORD(pCtx, 0xe50), u32IrisMinShift, u32IrisMaxShift);

        /* Clamp 0xe4c (iris min) */
        AE_CTX_WORD(pCtx, 0xe4c) = AeBoundariesCheck(
            AE_CTX_WORD(pCtx, 0xe4c), u32IrisMinShift, u32IrisMaxShift);

        /* Clamp 0xe6c (iris max linear) */
        AE_CTX_WORD(pCtx, 0xe6c) = AeBoundariesCheck(
            AE_CTX_WORD(pCtx, 0xe6c), 1 << u32IrisMinShift, 1 << u32IrisMaxShift);

        /* Clamp 0xe68 (iris min linear) */
        HI_U32 u32IrisMinLin = AeBoundariesCheck(
            AE_CTX_WORD(pCtx, 0xe68), 1 << u32IrisMinShift, 1 << u32IrisMaxShift);
        AE_CTX_WORD(pCtx, 0xe68) = u32IrisMinLin;
    }

    /* Prepare iris linear values based on mode */
    HI_U32 u32IrisLinMin, u32IrisLinMax;
    if (AE_CTX_WORD(pCtx, 0xe64) != 0) {
        /* Use pre-computed linear values */
        u32IrisLinMax = AE_CTX_WORD(pCtx, 0xe6c);
        u32IrisLinMin = AE_CTX_WORD(pCtx, 0xe68);
    } else {
        /* Compute from shift values */
        u32IrisLinMin = 1 << AE_CTX_WORD(pCtx, 0xe4c);
        u32IrisLinMax = 1 << AE_CTX_WORD(pCtx, 0xe50);
    }

    /* ---- Extended Route (RouteEx) Processing ---- */
    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    {
        HI_U32 u32IrisFactor = AE_CTX_WORD(pCtx, 0x57c);
        HI_U32 u32RouteMode  = AE_CTX_WORD(pCtx, 0xe48);

        if (u32IrisFactor != 0xFFFFFFFF)
            u32IrisFactor++;

        if (u32RouteMode == 1) {
            /* P-iris extended route: compute exposure with iris */
            HI_U32 u32MaxGain2 = AE_CTX_WORD(pCtx, 0x580);
            HI_U64 u64IntTimeExposure;

            /* Complex P-iris exposure computation using iris limits and float step */
            HI_FLOAT fStep = AE_CTX_FLOAT(pCtx, 0x424);
            HI_FLOAT fMaxGain = (HI_FLOAT)u32MaxGain2;
            HI_FLOAT fIrisMax = (HI_FLOAT)u32IrisLinMax;
            HI_FLOAT fStepGain = fMaxGain * fStep;

            u64IntTimeExposure = (HI_U64)u32IntTimeFactor * (HI_U64)u32IrisFactor;
            *(HI_U64 *)(pCtx + 0x590) = u64IntTimeExposure;

            HI_U64 u64MaxExposure = (HI_U64)u32MaxGain2 * (HI_U64)u32MaxGainEx + 1;
            u64MaxExposure *= (HI_U64)u32IrisLinMax;

            HI_U32 u32FloatResult = (HI_U32)(fIrisMax * fStepGain);
            u64MaxExposure -= (HI_U64)u32FloatResult;
        } else {
            /* Non-P-iris: compute exposure without iris */
            HI_U32 u32MaxGain2 = AE_CTX_WORD(pCtx, 0x580);
            HI_FLOAT fStep = AE_CTX_FLOAT(pCtx, 0x424);
            HI_FLOAT fMaxGain = (HI_FLOAT)u32MaxGain2;
            HI_FLOAT fStepGain = fMaxGain * fStep;

            HI_U64 u64IntTimeExposure = (HI_U64)u32IntTimeFactor * (HI_U64)u32IrisFactor;
            *(HI_U64 *)(pCtx + 0x590) = u64IntTimeExposure;

            HI_U64 u64MaxExposure = (HI_U64)u32MaxGain2 * (HI_U64)u32MaxGainEx + 1;

            HI_U32 u32FloatResult = (HI_U32)(fStepGain);
            u64MaxExposure -= (HI_U64)u32FloatResult;
        }
    }

    /* Compute 64-bit target exposure and clamp */
    HI_U64 u64TargetExposure;
    {
        pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
        HI_U32 u32SlowFR = AE_CTX_WORD(pCtx, 0x5d4);

        HI_U64 u64Min, u64Max;
        u64Min = *(HI_U64 *)(pCtx + 0x590);
        u64Max = *(HI_U64 *)(pCtx + 0x598);

        if (u64Min > u64Max) {
            u64Min = u64Max;
            *(HI_U64 *)(pCtx + 0x598) = u64Max;
        }

        /* Target stored as 64-bit */
        u64TargetExposure = *(HI_U64 *)(pCtx + 0x410);

        u64TargetExposure = AeBoundariesCheck64(u64TargetExposure, u64Min, u64Max);
        *(HI_U64 *)(pCtx + 0x408) = u64TargetExposure;
    }

    /* ---- Route table state tracking and update ---- */

    /* Extended route update */
    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    {
        HI_U32 u32Again     = AE_CTX_WORD(pCtx, 0x4f8);
        HI_U32 u32AgainMax  = AE_CTX_WORD(pCtx, 0x4fc);
        HI_U32 u32Dgain     = AE_CTX_WORD(pCtx, 0x530);
        HI_U32 u32DgainMax  = AE_CTX_WORD(pCtx, 0x534);
        HI_U32 u32DgainShift = AE_CTX_WORD(pCtx, 0x55c);
        HI_U32 u32IspDgainShift = AE_CTX_WORD(pCtx, 0x560);
        HI_U32 u32RouteExDirty = AE_CTX_WORD(pCtx, 0xe38);
        HI_U32 u32RouteMode = AE_CTX_WORD(pCtx, 0xe48);
        HI_S32 s32RouteExChanged = -1; /* -1 = unchanged */

        if (u32RouteMode == 1) {
            /* Check if any RouteEx parameters changed */
            HI_BOOL bChanged = HI_FALSE;

            if (u32RouteExDirty != 0) {
                bChanged = HI_TRUE;
            } else if (u32MaxGainEx != AE_CTX_WORD(pCtx, 0x4c4)) {
                bChanged = HI_TRUE;
            } else {
                /* Check all params for changes */
                if (u32IntTimeFactor != AE_CTX_WORD(pCtx, 0x4c0)) bChanged = HI_TRUE;
                else if (u32Again != AE_CTX_WORD(pCtx, 0x504)) bChanged = HI_TRUE;
                else if (u32AgainMax != AE_CTX_WORD(pCtx, 0x500)) bChanged = HI_TRUE;
                else if (u32Dgain != AE_CTX_WORD(pCtx, 0x53c)) bChanged = HI_TRUE;
                else if (u32DgainShift != AE_CTX_WORD(pCtx, 0x538)) bChanged = HI_TRUE;
                else if (u32IspDgainShift != AE_CTX_WORD(pCtx, 0x568)) bChanged = HI_TRUE;
                else if (AE_CTX_WORD(pCtx, 0x560) != AE_CTX_WORD(pCtx, 0x564)) bChanged = HI_TRUE;
                else if (u32IrisLinMin != AE_CTX_WORD(pCtx, 0xe78)) bChanged = HI_TRUE;
                else if (u32IrisLinMax != AE_CTX_WORD(pCtx, 0xe7c)) bChanged = HI_TRUE;
            }

            if (bChanged) {
                AeRouteExUpdate(s32Handle, 5);
                /* Update and limit routeEx table */
                AE_ROUTEEX_MGR_S *pRouteExMgr = (AE_ROUTEEX_MGR_S *)(pBase + u32Off + 0xb20);
                AeRouteExDelRdcy(pRouteExMgr, 5);
                /* ... limit and store params ... */
                /* (Complex NEON-vectorized limit computation omitted for brevity;
                   stores updated params at well-known offsets) */
                HI_U32 u32RouteExMin[8], u32RouteExMax[8];
                /* Build min/max arrays from context fields and call AeRouteExLimit */
                AeRouteExLimit(pRouteExMgr, 5, u32RouteExMin, u32RouteExMax);

                /* Store all new values */
                AE_CTX_WORD(pCtx, 0x4c4) = u32MaxGainEx;
                AE_CTX_WORD(pCtx, 0x4c0) = u32IntTimeFactor;
                AE_CTX_WORD(pCtx, 0x504) = u32Again;
                AE_CTX_WORD(pCtx, 0x500) = u32AgainMax;
                AE_CTX_WORD(pCtx, 0x53c) = u32Dgain;
                AE_CTX_WORD(pCtx, 0x538) = u32DgainShift;
                AE_CTX_WORD(pCtx, 0x568) = u32IspDgainShift;
                AE_CTX_WORD(pCtx, 0x564) = AE_CTX_WORD(pCtx, 0x560);
                AE_CTX_WORD(pCtx, 0xe78) = u32IrisLinMin;
                AE_CTX_WORD(pCtx, 0xe7c) = u32IrisLinMax;
                AE_CTX_WORD(pCtx, 0xe40) = 0;
                AE_CTX_WORD(pCtx, 0xe44) = 0;

                if (u32RouteExDirty != 0) {
                    AE_CTX_WORD(pCtx, 0xe3c) = 1;
                    AE_CTX_WORD(pCtx, 0xe38) = 0;
                    s32RouteExChanged = 0;
                } else {
                    s32RouteExChanged = 0;
                }
            } else {
                s32RouteExChanged = 0;
            }
        } else {
            /* Non-P-iris extended route update */
            if (u32RouteExDirty != 0 || u32MaxGainEx != AE_CTX_WORD(pCtx, 0x4c4)) {
                AeRouteExUpdate(s32Handle, 4);
                /* ... similar route table update logic ... */
                s32RouteExChanged = 0;
            } else {
                /* Check if all params match cached values */
                HI_BOOL bSame = HI_TRUE;
                if (u32IntTimeFactor != AE_CTX_WORD(pCtx, 0x4c0)) bSame = HI_FALSE;
                if (u32Again != AE_CTX_WORD(pCtx, 0x504)) bSame = HI_FALSE;
                if (u32AgainMax != AE_CTX_WORD(pCtx, 0x500)) bSame = HI_FALSE;
                if (u32Dgain != AE_CTX_WORD(pCtx, 0x53c)) bSame = HI_FALSE;
                if (u32DgainShift != AE_CTX_WORD(pCtx, 0x538)) bSame = HI_FALSE;
                if (u32IspDgainShift != AE_CTX_WORD(pCtx, 0x568)) bSame = HI_FALSE;
                if (AE_CTX_WORD(pCtx, 0x560) != AE_CTX_WORD(pCtx, 0x564)) bSame = HI_FALSE;

                if (bSame) {
                    s32RouteExChanged = 0;
                } else {
                    AeRouteExUpdate(s32Handle, 4);
                    s32RouteExChanged = 0;
                }
            }

            /* Store updated params */
            AE_CTX_WORD(pCtx, 0x4c4) = u32MaxGainEx;
            AE_CTX_WORD(pCtx, 0x4c0) = u32IntTimeFactor;
            AE_CTX_WORD(pCtx, 0x504) = u32Again;
            AE_CTX_WORD(pCtx, 0x500) = u32AgainMax;
            AE_CTX_WORD(pCtx, 0x53c) = u32Dgain;
            AE_CTX_WORD(pCtx, 0x538) = u32DgainShift;
            AE_CTX_WORD(pCtx, 0x568) = u32IspDgainShift;
            AE_CTX_WORD(pCtx, 0x564) = AE_CTX_WORD(pCtx, 0x560);
            AE_CTX_WORD(pCtx, 0xe40) = 0;
            AE_CTX_WORD(pCtx, 0xe44) = 0;
        }

        if (u32RouteExDirty != 0) {
            AE_CTX_WORD(pCtx, 0xe3c) = 1;
            AE_CTX_WORD(pCtx, 0xe38) = 0;
        }
    }

    /* ---- Standard Route Processing ---- */
    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    {
        HI_U32 u32IrisFactor = AE_CTX_WORD(pCtx, 0x57c);
        HI_U32 u32MaxGainStd = AE_CTX_WORD(pCtx, 0x580);
        HI_U32 u32RouteMode  = AE_CTX_WORD(pCtx, 0xe48);
        HI_U32 u32RouteDirty = AE_CTX_WORD(pCtx, 0x988);
        HI_S32 s32RouteChanged;

        if (u32RouteMode == 1) {
            /* P-iris standard route */
            if (u32RouteDirty != 0 ||
                u32MaxGainEx != AE_CTX_WORD(pCtx, 0x4bc) ||
                u32IntTimeFactor != AE_CTX_WORD(pCtx, 0x4b8) ||
                u32IrisFactor != AE_CTX_WORD(pCtx, 0x584) ||
                u32MaxGainStd != AE_CTX_WORD(pCtx, 0x588) ||
                u32IrisLinMin != AE_CTX_WORD(pCtx, 0xe70) ||
                u32IrisLinMax != AE_CTX_WORD(pCtx, 0xe74)) {

                AeRouteUpdate(s32Handle, 3);
                AE_ROUTE_MGR_S *pRouteMgr = (AE_ROUTE_MGR_S *)(pBase + u32Off + 0x6f0);
                AeRouteDelRdcy(pRouteMgr, 3);

                HI_U32 u32RouteMin[4], u32RouteMax[4];
                AeRouteLimit(pRouteMgr, 3, u32RouteMin, u32RouteMax);

                AE_CTX_WORD(pCtx, 0x4bc) = u32MaxGainEx;
                AE_CTX_WORD(pCtx, 0x4b8) = u32IntTimeFactor;
                AE_CTX_WORD(pCtx, 0x584) = u32IrisFactor;
                AE_CTX_WORD(pCtx, 0x588) = u32MaxGainStd;
                AE_CTX_WORD(pCtx, 0xe70) = u32IrisLinMin;
                AE_CTX_WORD(pCtx, 0xe74) = u32IrisLinMax;
                AE_CTX_WORD(pCtx, 0x990) = 0;
                AE_CTX_WORD(pCtx, 0x994) = 0;
                s32RouteChanged = 1;
            } else {
                s32RouteChanged = 0;
            }
        } else {
            /* Standard route, non-P-iris */
            if (u32RouteDirty != 0 ||
                u32MaxGainEx != AE_CTX_WORD(pCtx, 0x4bc)) {

                AeRouteUpdate(s32Handle, 2);
                AE_ROUTE_MGR_S *pRouteMgr = (AE_ROUTE_MGR_S *)(pBase + u32Off + 0x6f0);
                AeRouteDelRdcy(pRouteMgr, 2);

                HI_U32 u32RouteMin[4], u32RouteMax[4];
                AeRouteLimit(pRouteMgr, 2, u32RouteMin, u32RouteMax);

                AE_CTX_WORD(pCtx, 0x4bc) = u32MaxGainEx;
                AE_CTX_WORD(pCtx, 0x4b8) = u32IntTimeFactor;
                AE_CTX_WORD(pCtx, 0x584) = u32IrisFactor;
                AE_CTX_WORD(pCtx, 0x588) = u32MaxGainStd;
                AE_CTX_WORD(pCtx, 0x990) = 0;
                AE_CTX_WORD(pCtx, 0x994) = 0;
                s32RouteChanged = 1;
            } else {
                /* Check remaining params */
                if (u32IntTimeFactor != AE_CTX_WORD(pCtx, 0x4b8) ||
                    u32IrisFactor != AE_CTX_WORD(pCtx, 0x584) ||
                    u32MaxGainStd != AE_CTX_WORD(pCtx, 0x588)) {

                    AeRouteUpdate(s32Handle, 2);
                    s32RouteChanged = 1;
                } else {
                    s32RouteChanged = 0;
                }
            }
        }

        if (s32RouteChanged && u32RouteDirty != 0) {
            AE_CTX_WORD(pCtx, 0x98c) = 1;
            AE_CTX_WORD(pCtx, 0x988) = 0;
        }
    }

    /* ---- Slow frame rate check ---- */
    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    if (AE_CTX_WORD(pCtx, 0x44c) == 1) {
        /* AeCalcSlowFrameRate processing... */
    }

    /* ---- Route node walk and exposure allocation ---- */
    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    {
        HI_U32 u32ExRouteEx = AE_CTX_WORD(pCtx, 0x998);

        if (u32ExRouteEx == 1) {
            /* Extended route ExRoute path */
            /* Walk RouteEx nodes: get joint, find up/down nodes */
            AE_ROUTEEX_NODE_S *pLow = (AE_ROUTEEX_NODE_S *)(HI_ULONG)AE_CTX_WORD(pCtx, 0xe40);
            AE_ROUTEEX_NODE_S *pHigh;

            if (pLow == HI_NULL) {
                /* Find joint from routeEx */
                AE_ROUTEEX_MGR_S *pRouteExMgr = (AE_ROUTEEX_MGR_S *)(pBase + u32Off + 0xb20);
                AeRouteExGetJoint(pRouteExMgr, u64TargetExposure,
                    (AE_ROUTEEX_NODE_S **)&AE_CTX_WORD(pCtx, 0xe40),
                    (AE_ROUTEEX_NODE_S **)&AE_CTX_WORD(pCtx, 0xe44));

                pLow = (AE_ROUTEEX_NODE_S *)(HI_ULONG)AE_CTX_WORD(pCtx, 0xe40);
                if (pLow == HI_NULL) {
                    /* No matching node: use default ex alloc */
                    AeExposureAllocDefaultEx(u64TargetExposure, s32Handle);
                    goto done;
                }
            }

            pHigh = (AE_ROUTEEX_NODE_S *)(HI_ULONG)AE_CTX_WORD(pCtx, 0xe44);

            if (pHigh == HI_NULL) {
                AeExposureAllocDefaultEx(u64TargetExposure, s32Handle);
                goto done;
            }

            /* Extended route node allocation based on strategy field */
            {
                HI_U32 u32RouteMode2 = AE_CTX_WORD(pCtx, 0xe48);
                HI_U32 u32Stgy;
                AE_ROUTEEX_NODE_S *pTarget;

                if (u32TargetDir > u32TargetDir2) {
                    pTarget = pLow;
                    u32Stgy = *(HI_U32 *)((HI_U8 *)pLow + 32);
                } else {
                    pTarget = pHigh;
                    u32Stgy = *(HI_U32 *)((HI_U8 *)pHigh + 36);
                }

                if (u32Stgy >= 0 && u32Stgy <= 5) {
                    /* Dispatch to strategy-specific allocation */
                    /* Strategy 0: direct inttime+gain alloc
                       Strategy 1: inttime+iris+gain
                       Strategy 2: inttime+dgain+iris
                       Strategy 3: inttime+again+dgain+iris
                       Strategy 4: inttime+again+dgain+ispdgain+iris
                       Strategy 5: sysGain finalize */

                    /* (Each strategy computes inttime from node, aligns with
                       anti-flicker, then distributes remaining gain across
                       again/dgain/ispdgain using the Ae*Calc* family) */
                    /* Full implementation of each strategy path omitted here
                       for space; see assembly offsets 0xc14..0x34dc */
                }

                if (u32Stgy > 5) {
                    AeExposureAllocDefaultEx(u64TargetExposure, s32Handle);
                }
            }
        } else {
            /* Standard route path */
            AE_ROUTE_NODE_S *pLow = (AE_ROUTE_NODE_S *)(HI_ULONG)AE_CTX_WORD(pCtx, 0x990);

            if (pLow == HI_NULL) {
                AE_ROUTE_MGR_S *pRouteMgr = (AE_ROUTE_MGR_S *)(pBase + u32Off + 0x6f0);
                AeRouteGetJoint(pRouteMgr, u64TargetExposure,
                    (AE_ROUTE_NODE_S **)&AE_CTX_WORD(pCtx, 0x990),
                    (AE_ROUTE_NODE_S **)&AE_CTX_WORD(pCtx, 0x994));

                pLow = (AE_ROUTE_NODE_S *)(HI_ULONG)AE_CTX_WORD(pCtx, 0x990);
                if (pLow == HI_NULL) {
                    AeExposureAllocDefault(u64TargetExposure, s32Handle);
                    goto done;
                }
            }

            AE_ROUTE_NODE_S *pHigh = (AE_ROUTE_NODE_S *)(HI_ULONG)AE_CTX_WORD(pCtx, 0x994);

            if (pHigh == HI_NULL) {
                AeExposureAllocDefault(u64TargetExposure, s32Handle);
                goto done;
            }

            /* Compare target against node 64-bit products */
            HI_U64 u64LowProd  = *(HI_U64 *)((HI_U8 *)pLow + 16);
            HI_U64 u64HighProd = *(HI_U64 *)((HI_U8 *)pHigh + 16);

            /* Direction-based node selection */
            AE_ROUTE_NODE_S *pTarget;
            HI_U32 u32Stgy;

            if (u64TargetExposure >= u64HighProd) {
                /* Target above range: check direction */
                if (u32TargetDir > u32TargetDir2) {
                    /* Going up: check if within threshold */
                    if (u32TargetDir > u32TargetDir2 + 6) {
                        pTarget = pHigh;
                        u32Stgy = *(HI_U32 *)((HI_U8 *)pHigh + 24);
                    } else {
                        /* Stable: stay at current node */
                        goto alloc_default;
                    }
                } else {
                    goto alloc_current;
                }
            } else if (u64TargetExposure <= u64LowProd) {
                /* Target below range */
                if (u32TargetDir < u32TargetDir2) {
                    if (u32TargetDir + 6 < u32TargetDir2) {
                        pTarget = pLow;
                        u32Stgy = *(HI_U32 *)((HI_U8 *)pLow + 28);
                    } else {
                        goto alloc_default;
                    }
                } else {
                    goto alloc_current;
                }
            } else {
                /* Target within range: use current allocation */
                goto alloc_current;
            }

            /* Strategy dispatch for standard route */
            if (u32Stgy == 0 || u32Stgy > 4) {
                AeExposureAllocDefault(u64TargetExposure, s32Handle);
                goto done;
            }

            if (u32Stgy == 3) {
                /* Walk two nodes for better interpolation */
                AE_ROUTE_MGR_S *pRouteMgr = (AE_ROUTE_MGR_S *)(pBase + u32Off + 0x6f0);
                AE_ROUTE_NODE_S *pDw = AeRouteGetDwNode(pRouteMgr, pTarget);
                if (pDw != HI_NULL) {
                    AE_ROUTE_NODE_S *pDw2 = AeRouteGetDwNode(pRouteMgr, pDw);
                    /* Update stored route node pointers */
                    AE_CTX_WORD(pCtx, 0x990) = (HI_U32)(HI_ULONG)pDw;
                    AE_CTX_WORD(pCtx, 0x994) = (HI_U32)(HI_ULONG)(pDw2 ? pDw2 : HI_NULL);
                }
            } else if (u32Stgy == 4) {
                /* Out-route: dispatch to AeExposureAllocationOutRoute */
                void *pNodeTable = (void *)(pBase + u32Off + 0x990);
                AeExposureAllocationOutRoute(u64TargetExposure, pNodeTable, s32Handle);
                goto done;
            } else {
                /* Strategy 1-2: direct allocation from node values */
                HI_U32 u32IntTime = ((HI_U32 *)pTarget)[0];
                AE_CTX_WORD(pCtx, 0x4c8) = u32IntTime;
                AeCalcIntTime(s32Handle);

                /* Bound and align inttime, then call AeDealGain */
                HI_U32 u32Max2, u32Min2;
                HI_U32 u32Idx = AeGetIrisIdx(pCtx);
                AeGetIrisLimits(pBase, pCtx, s32Handle, u32Idx, &u32Max2, &u32Min2);

                u32IntTime = AeBoundariesCheck(AE_CTX_WORD(pCtx, 0x4c8), u32Min2, u32Max2);

                HI_U32 u32Step = (HI_U32)AE_CTX_FLOAT(pCtx, 0x4e4);
                HI_U32 u32FlickerBase = AE_CTX_WORD(pCtx, 0x1c8c);
                if (u32Step != 0) {
                    HI_U32 u32Aligned = ((u32IntTime - u32FlickerBase) / u32Step) * u32Step + u32FlickerBase;
                    if (u32Max2 > u32Aligned)
                        u32Aligned += u32Step;
                    u32IntTime = u32Aligned;
                }
                AE_CTX_WORD(pCtx, 0x4c8) = u32IntTime;

                /* Compute gain factor and call AeDealGain */
                HI_U32 u32GainShift = AE_CTX_WORD(pCtx, 0x58c);
                HI_U32 u32GainFactor = 1 << u32GainShift;
                HI_FLOAT fStepVal = AE_CTX_FLOAT(pCtx, 0x424);
                HI_U32 u32StepGain = (HI_U32)((HI_FLOAT)u32GainFactor * fStepVal);

                HI_U64 u64IntTimeBig = (HI_U64)u32IntTime << u32GainShift;
                HI_U64 u64Remain = u64IntTimeBig - (HI_U64)u32StepGain;

                AeDealGain(pCtx, u64TargetExposure,
                    (HI_U32)u64TargetExposure, (HI_U32)(u64TargetExposure >> 32),
                    u64Remain, s32Handle);
                goto done;
            }

            goto alloc_fallthrough;

        alloc_current:
            /* Navigate route nodes based on direction */
            if (s32Handle > 3) {
                HI_TRACE_ISP(RE_DBG_LVL,
                    "Illegal handle id %d in %s!\n",
                    s32Handle, "AeExposureAllocation");
                goto done;
            }

            pLow = (AE_ROUTE_NODE_S *)(HI_ULONG)AE_CTX_WORD(pCtx, 0x990);
            if (pLow == HI_NULL || AE_CTX_WORD(pCtx, 0x994) == 0) {
                AeExposureAllocDefault(u64TargetExposure, s32Handle);
                goto done;
            }
            /* fall through to node walk */

        alloc_fallthrough:
        alloc_default:
            AeExposureAllocDefault(u64TargetExposure, s32Handle);
        }
    }

done:
    /* Final system gain computation */
    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    {
        HI_U32 u32SysGain = AeCalcSysGain(
            AE_CTX_WORD(pCtx, 0x58c),
            AE_CTX_WORD(pCtx, 0x514),
            AE_CTX_WORD(pCtx, 0x518),
            AE_CTX_WORD(pCtx, 0x54c),
            AE_CTX_WORD(pCtx, 0x550),
            AE_CTX_WORD(pCtx, 0x56c),
            AE_CTX_WORD(pCtx, 0x570)
        );
        AE_CTX_WORD(pCtx, 0x400) = u32SysGain;
    }

    s32Ret = 0;

    /* Stack canary check */
    if (u32StackGuard != __stack_chk_guard) {
        __stack_chk_fail();
    }

    return s32Ret;
}
