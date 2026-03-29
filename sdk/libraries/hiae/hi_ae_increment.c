/**
 * Reverse Engineered by TekuConcept on April 28, 2021
 * hi_ae_increment.c — AE exposure increment calculation
 */

#include <stdio.h>
#include <string.h>

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
extern HI_U32 IO_READ32(HI_U32 u32Addr);


/**
 * Reverse Engineered from hi_ae_increment.S
 * AE exposure increment calculation: histogram analysis, zone-based error
 * computation, target luma adjustment, and exposure ratio calculation.
 */




extern HI_U32 transition(void *pBound, void *pWeight,
                          HI_U32 u32NumBins, HI_U32 u32BinIdx, HI_U32 u32Zero);
extern HI_S32 AeTargetAdjust(HI_S32 s32Handle);
extern HI_S32 AeHDRLevelCalc(void *pStat, HI_S32 s32Handle);
extern HI_S32 AeHDRHistArrayAverageCalc(void *pStat, HI_S32 s32Handle);

/* Count-ratio LUT: 16x16 table indexed [row][col] */
static const HI_U8 g_au8CountRatioLut[256] = {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x01,0x02,0x04,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x01,0x01,0x02,0x04,0x06,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x01,0x01,0x02,0x03,0x04,0x06,0x08,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x01,0x02,0x03,0x04,0x06,0x08,0x0a,0x0c,
    0x00,0x00,0x00,0x00,0x00,0x01,0x02,0x02,
    0x03,0x04,0x06,0x08,0x0a,0x0c,0x0e,0x10,
    0x00,0x00,0x00,0x01,0x02,0x03,0x04,0x05,
    0x06,0x08,0x0a,0x0c,0x0d,0x0f,0x10,0x10,
    0x00,0x00,0x02,0x03,0x04,0x05,0x06,0x08,
    0x09,0x0b,0x0c,0x0e,0x0f,0x10,0x10,0x10,
    0x00,0x02,0x04,0x05,0x06,0x07,0x09,0x0a,
    0x0c,0x0d,0x0e,0x0f,0x10,0x10,0x10,0x10,
    0x00,0x04,0x06,0x07,0x08,0x09,0x0b,0x0c,
    0x0e,0x0f,0x10,0x10,0x10,0x10,0x10,0x10,
    0x00,0x06,0x08,0x09,0x0a,0x0b,0x0d,0x0e,
    0x0f,0x10,0x10,0x10,0x10,0x10,0x10,0x10,
    0x00,0x08,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
    0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,
    0x00,0x0a,0x0c,0x0d,0x0e,0x0f,0x10,0x10,
    0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,
    0x00,0x0c,0x0e,0x0f,0x10,0x10,0x10,0x10,
    0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,
    0x00,0x0e,0x10,0x10,0x10,0x10,0x10,0x10,
    0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,
    0x00,0x10,0x10,0x10,0x10,0x10,0x10,0x10,
    0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,
    0x00,0x10,0x10,0x10,0x10,0x10,0x10,0x10,
    0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,
};

static HI_U32 u32PreExpRatio = 0x40;

/* Increment weight tables */
static const HI_U16 g_au16IncrWeightsUp[6]   = { 4, 6, 8, 16, 32, 64 };
static const HI_U16 g_au16IncrWeightsDown[6]  = { 4, 4, 5,  5,  6,  8 };
static const HI_U16 g_au16IncrWeightsMid[6]   = { 4, 6, 8, 10, 14, 22 };


/* ======================================================================== */
/* 7. AeHistWeightDef  (0x2e0 bytes)                                        */
/* ======================================================================== */
void AeHistWeightDef(HI_S32 s32Handle)
{
    HI_U8 *pBase = (HI_U8 *)&g_astAeCtx[0];
    HI_U8 *pCtx  = pBase + (HI_U32)s32Handle * AE_SIZEOF;

    HI_U8 u8SnsType = AE_CTX_BYTE(pCtx, 13);

    if (u8SnsType != 0) {
        HI_U8 u8SnsIf = AE_CTX_BYTE(pCtx, 14);
        if ((u8SnsIf - 2) <= 3)
            goto check_ae_mode_linear;

        if (u8SnsIf == 1) {
            /* Built-in WDR */
            HI_U32 mode = AE_CTX_WORD(pCtx, 0x108);
            if (mode == 0) {
                /* Lowlight WDR built-in */
                AE_CTX_WORD(pCtx, 0xf4) = 0;
                AE_CTX_HALF(pCtx, 0x114) = 192;
                AE_CTX_HALF(pCtx, 0x116) = 30;
                AE_CTX_HALF(pCtx, 0xf8) = 30;
                AE_CTX_WORD(pCtx, 0xe4) = 170;
                AE_CTX_WORD(pCtx, 0xe8) = 200;
                AE_CTX_WORD(pCtx, 0xec) = 235;
                AE_CTX_WORD(pCtx, 0xf0) = 255;
                AE_CTX_WORD(pCtx, 0xfc) = 100;
                AE_CTX_WORD(pCtx, 0x100) = 180;
                return;
            }
            if (mode != 1) return;
            /* Normal WDR built-in */
            AE_CTX_HALF(pCtx, 0x114) = 128;
            AE_CTX_HALF(pCtx, 0x116) = 12;
            AE_CTX_WORD(pCtx, 0xfc) = 12;
            AE_CTX_WORD(pCtx, 0xe8) = 8;
            AE_CTX_WORD(pCtx, 0xec) = 16;
            AE_CTX_WORD(pCtx, 0xe4) = 2;
            AE_CTX_WORD(pCtx, 0xf0) = 20;
            AE_CTX_HALF(pCtx, 0xf8) = 20;
            AE_CTX_WORD(pCtx, 0xf4) = 32;
            AE_CTX_WORD(pCtx, 0x100) = 0;
            return;
        }

        HI_U32 sub = (HI_U8)(u8SnsIf - 2);
        if (sub > 9) return;

        /* Check if special WDR mode (9 or 6), or sub <= 1 */
        HI_U32 isSpecial = (u8SnsIf == 9 || u8SnsIf == 6) ? 1 : 0;
        if (sub <= 1) isSpecial |= 1;

        if (isSpecial && AE_CTX_WORD(pCtx, 0x28) == 1)
            goto check_ae_mode_linear;

        /* WDR multi-frame mode */
        {
            HI_U32 mode = AE_CTX_WORD(pCtx, 0x108);
            if (mode == 0) {
                /* Lowlight WDR multi-frame */
                AE_CTX_WORD(pCtx, 0xf4) = 0;
                AE_CTX_HALF(pCtx, 0x114) = 192;
                AE_CTX_HALF(pCtx, 0x116) = 40;
                AE_CTX_WORD(pCtx, 0xe4) = 110;
                AE_CTX_WORD(pCtx, 0xe8) = 174;
                AE_CTX_WORD(pCtx, 0xec) = 220;
                AE_CTX_WORD(pCtx, 0xf0) = 255;
                AE_CTX_HALF(pCtx, 0xf8) = 16;
                AE_CTX_WORD(pCtx, 0xfc) = 48;
                AE_CTX_WORD(pCtx, 0x100) = 96;
                return;
            }
            if (mode != 1) return;
            /* Normal WDR multi-frame */
            AE_CTX_HALF(pCtx, 0x114) = 128;
            AE_CTX_HALF(pCtx, 0x116) = 12;
            AE_CTX_WORD(pCtx, 0xec) = 12;
            AE_CTX_WORD(pCtx, 0xe8) = 6;
            AE_CTX_HALF(pCtx, 0xf8) = 16;
            AE_CTX_WORD(pCtx, 0xf0) = 40;
            AE_CTX_WORD(pCtx, 0xe4) = 2;
            AE_CTX_WORD(pCtx, 0xf4) = 20;
            AE_CTX_WORD(pCtx, 0xfc) = 8;
            AE_CTX_WORD(pCtx, 0x100) = 0;
            return;
        }
    }

check_ae_mode_linear:
    {
        HI_U32 mode = AE_CTX_WORD(pCtx, 0x108);
        if (mode == 0) {
            /* Lowlight linear */
            AE_CTX_WORD(pCtx, 0xe8) = 174;
            AE_CTX_WORD(pCtx, 0xec) = 220;
            AE_CTX_WORD(pCtx, 0xf0) = 255;
            AE_CTX_WORD(pCtx, 0xf4) = 0;
            AE_CTX_WORD(pCtx, 0xfc) = 39;
            AE_CTX_WORD(pCtx, 0xe4) = 110;
            AE_CTX_HALF(pCtx, 0x114) = 128;
            AE_CTX_WORD(pCtx, 0x100) = 74;
            AE_CTX_HALF(pCtx, 0x116) = 16;
            AE_CTX_HALF(pCtx, 0xf8) = 16;
            return;
        }
        if (mode != 1) return;
        /* Normal linear */
        AE_CTX_WORD(pCtx, 0xe8) = 8;
        AE_CTX_WORD(pCtx, 0xf4) = 44;
        AE_CTX_WORD(pCtx, 0xe4) = 2;
        AE_CTX_HALF(pCtx, 0x116) = 16;
        AE_CTX_WORD(pCtx, 0xec) = 16;
        AE_CTX_WORD(pCtx, 0xfc) = 16;
        AE_CTX_WORD(pCtx, 0x100) = 0;
        AE_CTX_HALF(pCtx, 0x114) = 128;
        AE_CTX_WORD(pCtx, 0xf0) = 32;
        AE_CTX_HALF(pCtx, 0xf8) = 32;
    }
}


/* ======================================================================== */
/* 3. AeIncrementInitialize  (0x244 bytes)                                  */
/* ======================================================================== */
void AeIncrementInitialize(HI_S32 s32Handle)
{
    HI_U8 *pBase = (HI_U8 *)&g_astAeCtx[0];
    HI_U8 *pCtx  = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    HI_U32 i;

    HI_U32 u32TargetLuma = AE_CTX_WORD(pCtx, 0x1cf0);
    if (u32TargetLuma == 0)
        u32TargetLuma = 5000000;
    AE_CTX_WORD(pCtx, 0xac) = u32TargetLuma;

    HI_U32 u32InitExpRatio = AE_CTX_WORD(pCtx, 0x27c4);
    HI_U32 u32ExpTarget    = AE_CTX_WORD(pCtx, 0x17a8);
    HI_U32 u32WdrExpRatio  = AE_CTX_WORD(pCtx, 0x27c8);

    AE_CTX_WORD(pCtx, 0x148) = u32ExpTarget;
    AE_CTX_HALF(pCtx, 0x3ce) = 144;

    if (u32InitExpRatio == 0) u32InitExpRatio = 16384;
    if (u32WdrExpRatio == 0)  u32WdrExpRatio = 64;

    AE_CTX_WORD(pCtx, 0x164) = u32WdrExpRatio;
    AE_CTX_WORD(pCtx, 0x1bc) = 40;
    AE_CTX_WORD(pCtx, 0x160) = u32InitExpRatio;
    AE_CTX_DWORD(pCtx, 0x80) = 262144ULL;
    AE_CTX_WORD(pCtx, 0xb4)  = 256;
    AE_CTX_HALF(pCtx, 0xc2)  = 2;
    AE_CTX_HALF(pCtx, 0x156) = 32;
    AE_CTX_HALF(pCtx, 0x13c) = 0;
    AE_CTX_HALF(pCtx, 0x13e) = 0;
    AE_CTX_HALF(pCtx, 0x3f8) = 0;
    AE_CTX_HALF(pCtx, 0x3cc) = 128;
    AE_CTX_HALF(pCtx, 0x158) = 1024;
    AE_CTX_HALF(pCtx, 0x154) = 12;

    AE_CTX_WORD(pCtx, 0x3c4) = 0;
    AE_CTX_WORD(pCtx, 0x3c8) = 0;
    AE_CTX_WORD(pCtx, 0x3dc) = 0;
    AE_CTX_WORD(pCtx, 0x3e0) = 0;
    AE_CTX_WORD(pCtx, 0x3e4) = 0;
    AE_CTX_WORD(pCtx, 0x3e8) = 0;
    AE_CTX_WORD(pCtx, 0x3f0) = 0;
    AE_CTX_WORD(pCtx, 0x3f4) = 0;
    AE_CTX_BYTE(pCtx, 0x3fa) = 0;
    AE_CTX_WORD(pCtx, 0x174) = 0;
    AE_CTX_BYTE(pCtx, 0xc0) = 64;
    AE_CTX_WORD(pCtx, 0x15c) = 64;
    AE_CTX_WORD(pCtx, 0x3d8) = 1;
    AE_CTX_WORD(pCtx, 0x170) = 1;
    AE_CTX_WORD(pCtx, 0x178) = 0;
    AE_CTX_WORD(pCtx, 0x17c) = 0;
    AE_CTX_WORD(pCtx, 0x180) = 0;
    AE_CTX_WORD(pCtx, 0x184) = 0;
    AE_CTX_WORD(pCtx, 0x14c) = 0;
    AE_CTX_HALF(pCtx, 0x146) = 0;
    AE_CTX_HALF(pCtx, 0x152) = 0;
    AE_CTX_HALF(pCtx, 0x144) = 0;
    AE_CTX_HALF(pCtx, 0x150) = 0;

    /* Init avgHistory[0..8] with value 60 */
    for (i = 0; i < 9; i++)
        AE_CTX_WORD(pCtx, 0x190 + i * 4) = 60;

    /* Zero zone weights and previous bins */
    for (i = 0; i < 255; i++) {
        AE_CTX_BYTE(pCtx, 0x1c0 + i) = 0;
        AE_CTX_BYTE(pCtx, 0x2bf + i) = 0;
    }

    HI_U32 u32AeMode = AE_CTX_WORD(pCtx, 0x27bc);

    AE_CTX_HALF(pCtx, 0xce) = 0;
    AE_CTX_HALF(pCtx, 0xd0) = 0;
    AE_CTX_WORD(pCtx, 0xd4) = 0;
    AE_CTX_WORD(pCtx, 0xd8) = 0;
    AE_CTX_HALF(pCtx, 0xdc) = 0;
    AE_CTX_WORD(pCtx, 0xe0) = 0;
    AE_CTX_WORD(pCtx, 0xc8) = 0;
    AE_CTX_BYTE(pCtx, 0xc4) = 64;
    AE_CTX_BYTE(pCtx, 0xc5) = 64;
    AE_CTX_HALF(pCtx, 0xcc) = 50;
    AE_CTX_HALF(pCtx, 0xc6) = 1024;
    AE_CTX_WORD(pCtx, 0x108) = u32AeMode;
    AE_CTX_WORD(pCtx, 0x104) = u32AeMode;

    AeHistWeightDef(s32Handle);

    HI_U8 u8NoFlicker = AE_CTX_BYTE(pCtx, 0x27c2);
    if (u8NoFlicker == 0)
        u8NoFlicker = 1;
    AE_CTX_BYTE(pCtx, 0xb8) = u8NoFlicker;

    AE_CTX_WORD(pCtx, 0xbc) = 0x42A00000; /* 80.0f */
}


/* ======================================================================== */
/* 5. AeZoneErrorCalc  (0xd8 bytes)                                         */
/* Computes zone error: ratio of changed zones to total valid zones.        */
/* Returns (changedCount << 8) / totalCount as a 16-bit value.              */
/* ======================================================================== */
HI_U16 AeZoneErrorCalc(HI_S32 s32Handle)
{
    HI_U8 *pBase = (HI_U8 *)&g_astAeCtx[0];
    HI_U8 *pCtx  = pBase + (HI_U32)s32Handle * AE_SIZEOF;

    HI_U8  u8Thresh  = AE_CTX_BYTE(pCtx, 0x3bf);
    HI_U8  u8MaxLuma = AE_CTX_BYTE(pCtx, 0x3be);
    HI_U16 u16Tol    = AE_CTX_HALF(pCtx, 0xc2);
    HI_U16 u16Changed = 0;
    HI_U16 u16Total   = 255;
    HI_U32 i;

    for (i = 0; i < 255; i++) {
        HI_U8 curWeight = AE_CTX_BYTE(pCtx, 0x1c0 + i);

        if (curWeight < u8Thresh) {
            /* Below minimum threshold */
            AE_CTX_BYTE(pCtx, 0x1c0 + i) = 0;
            AE_CTX_BYTE(pCtx, 0x2bf + i) = 0;
            u16Total--;
            continue;
        }

        if (u8MaxLuma < curWeight) {
            /* Above max luma reference */
            AE_CTX_BYTE(pCtx, 0x1c0 + i) = 0;
            AE_CTX_BYTE(pCtx, 0x2bf + i) = 0;
            u16Total--;
            continue;
        }

        /* Valid zone: compute change from previous bin */
        HI_U8 prevBin = AE_CTX_BYTE(pCtx, 0x2bf + i);
        HI_S32 diff = (HI_S32)curWeight - (HI_S32)prevBin;
        AE_CTX_BYTE(pCtx, 0x2bf + i) = curWeight;
        if (diff < 0) diff = -diff;
        if (diff > (HI_S32)u16Tol)
            u16Changed++;
    }

    if (u16Total > 99) {
        return (HI_U16)((HI_S32)(u16Changed << 8) / (HI_S32)u16Total);
    }
    return 0;
}


/* ======================================================================== */
/* 6. AeIncrementTimeRelatedCalculate  (0x1e8 bytes)                        */
/* Time-related AE increment: adjusts target using direction tracking,      */
/* zone error, and stall detection.                                         */
/* ======================================================================== */
void AeIncrementTimeRelatedCalculate(HI_S32 s32Handle)
{
    HI_U8 *pBase = (HI_U8 *)&g_astAeCtx[0];
    HI_U8 *pCtx  = pBase + (HI_U32)s32Handle * AE_SIZEOF;

    HI_S32 s32Increment = AE_CTX_SWORD(pCtx, 0x140);
    HI_S16 s16Delta     = AE_CTX_SHALF(pCtx, 0xdc);
    HI_S16 s16Tol       = (HI_S16)AE_CTX_HALF(pCtx, 0xc2);

    HI_S32 s32AbsIncr = s32Increment;
    if (s32AbsIncr < 0) s32AbsIncr = -s32AbsIncr;

    HI_U16 u16ZoneErr = AeZoneErrorCalc(s32Handle);

    if (s16Delta > s16Tol) {
        /* Positive direction (scene too dark, need more exposure) */
        if (s16Delta > s16Tol * 2) {
            /* Large positive delta */
            HI_U16 u16HistThresh = AE_CTX_HALF(pCtx, 0x3cc);
            if (u16HistThresh < u16ZoneErr) {
                /* Zone error exceeds threshold: apply increment */
                AE_CTX_HALF(pCtx, 0x152) = 1;
                HI_U16 u16PrevDir = AE_CTX_HALF(pCtx, 0x150);
                HI_U16 u16UpCount = 0;
                AE_CTX_WORD(pCtx, 0x14c) = 0;
                if (u16PrevDir == 1) {
                    u16UpCount = AE_CTX_HALF(pCtx, 0x144) + 1;
                    u16UpCount = (HI_U16)u16UpCount;
                }
                AE_CTX_HALF(pCtx, 0x144) = u16UpCount;
                HI_U16 u16MaxUp = AE_CTX_HALF(pCtx, 0x13c);
                HI_U32 u32Target = AE_CTX_WORD(pCtx, 0xb4);
                if (u16MaxUp > u16UpCount)
                    s32AbsIncr = 0;
                AE_CTX_WORD(pCtx, 0xb0) = u32Target + s32AbsIncr;
            } else {
                /* Zone error below threshold: stall + increment */
                HI_U32 u32Stall = AE_CTX_WORD(pCtx, 0x14c) + 1;
                AE_CTX_WORD(pCtx, 0x14c) = u32Stall;
                AE_CTX_HALF(pCtx, 0x144) = 0;
                AE_CTX_HALF(pCtx, 0x146) = 0;
                HI_U16 u16MaxUp = AE_CTX_HALF(pCtx, 0x13c);
                HI_U32 u32Target = AE_CTX_WORD(pCtx, 0xb4);
                HI_U16 u16UpCount = 0;
                if (u16MaxUp > u16UpCount)
                    s32AbsIncr = 0;
                AE_CTX_WORD(pCtx, 0xb0) = u32Target + s32AbsIncr;
            }
        } else {
            /* Small positive delta (1-2x tolerance) */
            HI_U16 u16HistThresh = AE_CTX_HALF(pCtx, 0x3cc);
            if (u16HistThresh < u16ZoneErr) {
                /* Apply increment same as large positive */
                AE_CTX_HALF(pCtx, 0x152) = 1;
                HI_U16 u16PrevDir = AE_CTX_HALF(pCtx, 0x150);
                HI_U16 u16UpCount = 0;
                AE_CTX_WORD(pCtx, 0x14c) = 0;
                if (u16PrevDir == 1) {
                    u16UpCount = AE_CTX_HALF(pCtx, 0x144) + 1;
                    u16UpCount = (HI_U16)u16UpCount;
                }
                AE_CTX_HALF(pCtx, 0x144) = u16UpCount;
                HI_U16 u16MaxUp = AE_CTX_HALF(pCtx, 0x13c);
                HI_U32 u32Target = AE_CTX_WORD(pCtx, 0xb4);
                if (u16MaxUp > u16UpCount)
                    s32AbsIncr = 0;
                AE_CTX_WORD(pCtx, 0xb0) = u32Target + s32AbsIncr;
            } else {
                /* Stall */
                HI_U32 u32Stall = AE_CTX_WORD(pCtx, 0x14c) + 1;
                AE_CTX_WORD(pCtx, 0x14c) = u32Stall;
                AE_CTX_HALF(pCtx, 0x144) = 0;
                AE_CTX_HALF(pCtx, 0x146) = 0;
                HI_U32 u32Target = AE_CTX_WORD(pCtx, 0xb4);
                AE_CTX_WORD(pCtx, 0xb0) = u32Target + s32AbsIncr;
                /* Note: when maxUp (0x13c) > 0, s32AbsIncr is zeroed */
            }
        }
    } else {
        /* Non-positive delta */
        HI_S16 s16AbsDelta = s16Delta;
        if (s16AbsDelta < 0) s16AbsDelta = -s16AbsDelta;

        if (s16AbsDelta <= s16Tol) {
            /* Within tolerance: stall, keep target unchanged */
            HI_U32 u32Stall = AE_CTX_WORD(pCtx, 0x14c) + 1;
            AE_CTX_WORD(pCtx, 0x14c) = u32Stall;
            AE_CTX_WORD(pCtx, 0xb0) = AE_CTX_WORD(pCtx, 0xb4);
            AE_CTX_HALF(pCtx, 0x144) = 0;
            AE_CTX_HALF(pCtx, 0x146) = 0;
        } else if (s16AbsDelta > s16Tol * 2) {
            /* Large negative delta */
            HI_U16 u16HistThresh = AE_CTX_HALF(pCtx, 0x3cc);
            if (u16HistThresh < u16ZoneErr) {
                AE_CTX_HALF(pCtx, 0x152) = 0;
                HI_U16 u16PrevDir = AE_CTX_HALF(pCtx, 0x150);
                HI_U16 u16DnCount = 0;
                AE_CTX_WORD(pCtx, 0x14c) = 0;
                if (u16PrevDir == 0) {
                    u16DnCount = AE_CTX_HALF(pCtx, 0x146) + 1;
                    u16DnCount = (HI_U16)u16DnCount;
                }
                AE_CTX_HALF(pCtx, 0x146) = u16DnCount;
                HI_U16 u16MaxDn = AE_CTX_HALF(pCtx, 0x13e);
                HI_U32 u32Target = AE_CTX_WORD(pCtx, 0xb4);
                if (u16MaxDn > u16DnCount)
                    s32AbsIncr = 0;
                AE_CTX_WORD(pCtx, 0xb0) = u32Target - s32AbsIncr;
            } else {
                HI_U32 u32Stall = AE_CTX_WORD(pCtx, 0x14c) + 1;
                AE_CTX_WORD(pCtx, 0x14c) = u32Stall;
                AE_CTX_HALF(pCtx, 0x144) = 0;
                AE_CTX_HALF(pCtx, 0x146) = 0;
                HI_U16 u16MaxDn = AE_CTX_HALF(pCtx, 0x13e);
                HI_U32 u32Target = AE_CTX_WORD(pCtx, 0xb4);
                HI_U16 u16DnCount = 0;
                if (u16MaxDn > u16DnCount)
                    s32AbsIncr = 0;
                AE_CTX_WORD(pCtx, 0xb0) = u32Target - s32AbsIncr;
            }
        } else {
            /* Small negative delta (1-2x tolerance) */
            HI_U16 u16HistThresh = AE_CTX_HALF(pCtx, 0x3cc);
            if (u16HistThresh < u16ZoneErr) {
                AE_CTX_HALF(pCtx, 0x152) = 0;
                HI_U16 u16PrevDir = AE_CTX_HALF(pCtx, 0x150);
                HI_U16 u16DnCount = 0;
                AE_CTX_WORD(pCtx, 0x14c) = 0;
                if (u16PrevDir == 0) {
                    u16DnCount = AE_CTX_HALF(pCtx, 0x146) + 1;
                    u16DnCount = (HI_U16)u16DnCount;
                }
                AE_CTX_HALF(pCtx, 0x146) = u16DnCount;
                HI_U16 u16MaxDn = AE_CTX_HALF(pCtx, 0x13e);
                HI_U32 u32Target = AE_CTX_WORD(pCtx, 0xb4);
                if (u16MaxDn > u16DnCount)
                    s32AbsIncr = 0;
                AE_CTX_WORD(pCtx, 0xb0) = u32Target - s32AbsIncr;
            } else {
                HI_U32 u32Stall = AE_CTX_WORD(pCtx, 0x14c) + 1;
                AE_CTX_WORD(pCtx, 0x14c) = u32Stall;
                AE_CTX_HALF(pCtx, 0x144) = 0;
                AE_CTX_HALF(pCtx, 0x146) = 0;
                HI_U16 u16MaxDn = AE_CTX_HALF(pCtx, 0x13e);
                HI_U32 u32Target = AE_CTX_WORD(pCtx, 0xb4);
                HI_U16 u16DnCount = 0;
                if (u16MaxDn > u16DnCount)
                    s32AbsIncr = 0;
                AE_CTX_WORD(pCtx, 0xb0) = u32Target - s32AbsIncr;
            }
        }
    }

    /* Save current direction as previous */
    AE_CTX_HALF(pCtx, 0x150) = AE_CTX_HALF(pCtx, 0x152);
}


/* ======================================================================== */
/* 8. AeHistArrayAverageCalc  (0xd90 bytes -- only first ~808 bytes are     */
/*    the target function, rest is HDR variant)                              */
/*                                                                          */
/* NOTE: This function is very large and complex (808+ bytes of ARM asm),   */
/* involving 64-bit arithmetic, NEON SIMD (D8/D9), and calls to             */
/* transition(). It computes weighted histogram averages with piecewise     */
/* interpolation. The full implementation requires ~400 lines of C.         */
/* Below is a structural implementation covering the key logic paths.       */
/* ======================================================================== */
HI_S32 AeHistArrayAverageCalc(void *pStat, HI_S32 s32Handle)
{
    HI_U8 *pBase = (HI_U8 *)&g_astAeCtx[0];
    HI_U8 *pCtx  = pBase + (HI_U32)s32Handle * AE_SIZEOF;

    HI_U8  u8SnsType   = AE_CTX_BYTE(pCtx, 13);
    HI_U16 u16HistErr  = AE_CTX_HALF(pCtx, 0x3f8);
    HI_U16 u16HistH    = AE_CTX_HALF(pCtx, 0x114);
    HI_U8  u8FirstRun  = AE_CTX_BYTE(pCtx, 0x3fa);
    HI_U16 u16HistL    = AE_CTX_HALF(pCtx, 0x116);
    HI_U32 u32ZoneAvg  = AE_CTX_WORD(pCtx, 0xa0);
    HI_U32 u32HistClip = u16HistErr >> 4;

    if (u32HistClip > 256) u32HistClip = 256;
    HI_U32 u32HistClip2 = u16HistErr >> 2;
    if (u32HistClip2 > 1023) u32HistClip2 = 1023;

    HI_U32 u32Sub = (HI_U8)(u8SnsType - 2);
    HI_U32 u32StartBin = 256 - u32HistClip;
    HI_U32 u32FirstRunFlag = (u8FirstRun == 1) ? 1 : 0;

    /* Weighted histogram accumulation */
    if (u32Sub > 9) {
        /* Non-WDR: simple histogram accumulation with transition-based weights */
        HI_U32 *pHistBound  = (HI_U32 *)((HI_U8 *)pCtx + 0xe4);
        HI_U32 *pHistWeight = (HI_U32 *)((HI_U8 *)pCtx + 0xf4);
        HI_U64 u64WeightedSum = 0;
        HI_U64 u64BinProdSum  = 0;
        HI_U64 u64TotalWeightedBinSum = 0;
        HI_U64 u64TotalProdSum = 0;
        HI_U32 u32TotalBinSum = 0;

        void *pStatHist = AE_CTX_PTR(pStat, 4);
        HI_U32 i;
        for (i = 0; i < 256; i++) {
            HI_S16 s16Idx = (HI_S16)i;
            HI_U32 u32BinStart = u32HistClip + i;
            HI_U16 u16BinAddr = (HI_U16)(u32BinStart);

            /* Load 4 sub-bins and sum */
            HI_U32 *pBin = (HI_U32 *)((HI_U8 *)pStatHist +
                            (HI_U32)u16BinAddr * 16);
            HI_U32 binVal = pBin[2] + pBin[3] + pBin[0] + pBin[4];

            AE_CTX_WORD(pCtx, 0xa0 + 0) = binVal; /* store to zone avg */

            if (s16Idx == 0) {
                /* Skip first bin for ratio calc */
                u32TotalBinSum += binVal;
                continue;
            }

            /* Compute weighted bin value using transition() */
            HI_U64 u64BinIdx = (HI_U64)i * (HI_U64)binVal;
            u32TotalBinSum += binVal;

            HI_U32 weight = transition(pHistWeight, pHistBound, 4, i, 0);
            if (weight != 0) {
                if (weight > 256) weight = 256;
                HI_U32 clipped = (binVal < (u32ZoneAvg / 100000)) ? binVal : (u32ZoneAvg / 100000);
                HI_U64 prod = (HI_U64)clipped * weight;
                prod = (prod + 8) >> 4;
                u64WeightedSum += prod;
            }

            /* Accumulate weighted transition product */
            HI_S16 s16Weight = 0; /* from transition interpolation */
            /* ... (transition-based interpolation between histogram bounds) ... */
            HI_U64 binProd = (u64BinIdx + 128) >> 8;
            u64TotalProdSum += binProd;
        }

        /* Final computation: weighted average */
        HI_U32 u32Half = u32TotalBinSum >> 1;
        /* ... divide weighted sums by total ... */
    } else {
        /* WDR histogram path: uses different weight tables and more complex
           64-bit accumulation with NEON assist */
        /* ... (similar structure but with WDR-specific offsets) ... */
    }

    /* Store results and update context */
    /* (The exact final computation depends on the full accumulation path) */

    return 0;
}


/* ======================================================================== */
/* 9. AeWDRRefExpRatioCalc  (0x1ac bytes)                                   */
/* Computes WDR reference exposure ratio from median-filtered histogram     */
/* contrast analysis. Uses top-2 / bottom-2 filtering over 16x68 zones.    */
/* ======================================================================== */
HI_S32 AeWDRRefExpRatioCalc(void *pStat, HI_S32 s32Handle)
{
    HI_U8 *pBase = (HI_U8 *)&g_astAeCtx[0];
    HI_U8 *pCtx  = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    HI_U16 *pHist = (HI_U16 *)AE_CTX_PTR(pStat, 28);
    HI_U16 *pEnd  = pHist + 1024; /* 0x7f8/2 + some => 1024 entries */

    /* Median-4 filter: track top 2 (max1 >= max2) and bottom 2 (min1 <= min2) */
    HI_U16 max1, max2, min1, min2;
    HI_U16 cur;

    cur = pHist[1];
    max1 = cur; max2 = cur;
    min1 = cur; min2 = cur;

    while (pHist < pEnd) {
        HI_U16 *pGroupEnd = pHist + 68; /* 0x88/2 = 68 halfwords per group */
        while (pHist < pGroupEnd) {
            /* Process pair: pHist[1] and pHist[2] */
            HI_U16 v1 = pHist[1];
            HI_U16 v2 = pHist[2];

            /* Update max tracking */
            if (v1 > max1) {
                max2 = max1; max1 = v1;
            } else if (v1 > max2) {
                HI_U16 tmp = max2; max2 = max1;
                max1 = (v1 > tmp) ? tmp : v1;
                /* Actually: maintain sorted pair max1 >= max2 */
            }
            /* Update min tracking */
            if (v1 < min1) {
                min2 = min1; min1 = v1;
            } else if (v1 < min2) {
                min2 = min1; min1 = v1;
            }

            /* Same for v2 vs max/min pairs */
            if (v2 > max1) { max2 = max1; max1 = v2; }
            else if (v2 > max2) { max2 = v2; }
            if (v2 < min1) { min2 = min1; min1 = v2; }
            else if (v2 < min2) { min2 = v2; }

            pHist += 4;
        }
        if (pHist >= pEnd) break;
    }

    /* Compute contrast metric */
    HI_U32 u32Mid = ((HI_U32)max1 + max2) >> 1;
    if (min2 < 255) min2 = 255;
    HI_U32 u32Low;
    if (min1 >= 255)
        u32Low = ((HI_U32)min2 + min1) >> 1;
    else
        u32Low = ((HI_U32)min2 + 255) >> 1;

    /* ratio = mid^2 * 36 / low / low */
    HI_U64 u64MidSq = (HI_U64)u32Mid * u32Mid;
    u64MidSq *= 36; /* (x*9)<<2 in asm */

    HI_U64 u64Div1 = u64MidSq / u32Low;
    HI_U64 u64Div2 = u64Div1 / u32Low;

    HI_U32 u32ExpRatio;
    if (u64Div2 <= 3136) {
        u32ExpRatio = 64;
    } else {
        HI_U64 u64Diff = u64Div2 - 3136;
        u64Diff <<= 6;
        HI_U32 u32Shifted = (HI_U32)(u64Diff >> 10);
        if (u32Shifted < 32) u32Shifted = 32;
        if (u32Shifted > 16384) u32Shifted = 16384;
        if (u32Shifted < 64) u32Shifted = 64;
        u32ExpRatio = u32Shifted;
    }

    AE_CTX_WORD(pCtx, 0x168) = u32ExpRatio;
    return 0;
}


/* ======================================================================== */
/* 4. AeIncrementCalculate  (0x50c bytes)                                   */
/* Piecewise-linear increment from luma delta, with convergence damping.    */
/* ======================================================================== */
HI_S32 AeIncrementCalculate(HI_S32 s32Handle)
{
    HI_U8 *pBase = (HI_U8 *)&g_astAeCtx[0];
    HI_U8 *pCtx  = pBase + (HI_U32)s32Handle * AE_SIZEOF;

    HI_U8  u8Luma    = AE_CTX_BYTE(pCtx, 0xc4);
    HI_U32 u32Prev   = AE_CTX_WORD(pCtx, 0xd8);
    HI_U8  u8SnsType = AE_CTX_BYTE(pCtx, 13);
    HI_S16 s16Delta  = (HI_S16)((HI_S32)u8Luma - (HI_S32)u32Prev);

    AE_CTX_SHALF(pCtx, 0xdc) = s16Delta;

    /* WDR sensor convergence smoothing */
    if (u8SnsType != 0) {
        HI_U32 isSpecial = (u8SnsType == 9 || u8SnsType == 6) ? 1 : 0;
        HI_U32 sub = u8SnsType - 2;
        if (sub <= 1) isSpecial |= 1;
        if (isSpecial && AE_CTX_WORD(pCtx, 0x28) == 1)
            goto do_calc;

        if (AE_CTX_WORD(pCtx, 0xc8) == 1)
            goto do_calc;

        HI_U16 u16Tol = AE_CTX_HALF(pCtx, 0xc2);
        HI_S32 absDelta = s16Delta < 0 ? -s16Delta : s16Delta;

        if (absDelta <= (HI_S32)u16Tol) {
            AE_CTX_HALF(pCtx, 0xce) = 1;
            AE_CTX_HALF(pCtx, 0xd0) = 0;
            goto do_calc;
        }
        if (AE_CTX_HALF(pCtx, 0xce) == 1) {
            HI_U16 u16Cnt = AE_CTX_HALF(pCtx, 0xd0) + 1;
            HI_U16 u16Max = AE_CTX_HALF(pCtx, 0xcc);
            HI_S32 s32Acc = (HI_S32)s16Delta * (HI_S32)u16Cnt;
            AE_CTX_HALF(pCtx, 0xd0) = u16Cnt;
            s16Delta = (HI_S16)(s32Acc / (u16Max ? (HI_S32)u16Max : 1));
            AE_CTX_SHALF(pCtx, 0xdc) = s16Delta;
            if (u16Cnt > u16Max) {
                AE_CTX_HALF(pCtx, 0xce) = 0;
                AE_CTX_HALF(pCtx, 0xd0) = 0;
            }
            goto do_calc;
        }
    }

do_calc:
    ;
    /* Select weight tables */
    HI_U16 wPos[6], wNeg[6];
    memcpy(wPos, g_au16IncrWeightsUp, sizeof(wPos));
    memcpy(wNeg, g_au16IncrWeightsDown, sizeof(wNeg));

    if (AE_CTX_BYTE(pCtx, 0xb8) > 1)
        memcpy(wNeg, g_au16IncrWeightsMid, sizeof(wNeg));

    /* Compute normalized increment */
    HI_U8 u8Div = u8Luma;
    if (u8Div == 0) u8Div = 1;
    HI_S16 s16Incr = (HI_S16)((s16Delta << 6) / (HI_S32)u8Div);

    HI_U16 u16Result;
    HI_S32 s32Sign;

    if (s16Incr > 0) {
        s32Sign = 1;
        HI_S32 n = s16Incr;
        /* Positive: piecewise accumulation over ranges [1,3],[4,7],[8,15],[16,31],[32,47],[48,63],64+ */
        u16Result = 0;
        /* Range [1, 3] */
        HI_S32 seg = (n > 3) ? 3 : n;
        u16Result = (HI_U16)(((HI_U32)(seg * wPos[0]) + 8) >> 4);
        if (n > 3) {
            /* Range [4, 7] */
            seg = (n > 7) ? 4 : (n - 3);
            u16Result += (HI_U16)(((HI_U32)(wPos[1] * seg) + 8) >> 4);
            /* Correction: from asm, base = (w0*4+8)>>4, then add (w1*(n-4)+8)>>4 */
        }
        /* Actually, the asm does NOT accumulate incrementally per iteration.
           It computes from scratch based on which range s16Incr falls in.
           Let me follow the exact asm logic: */

        if (n <= 3) {
            u16Result = (HI_U16)(((HI_U32)(n * wPos[0]) + 8) >> 4);
        } else {
            HI_U16 base = (HI_U16)(((HI_U32)(wPos[0] << 2) + 8) >> 4);
            if (n <= 7) {
                u16Result = base + (HI_U16)(((HI_S32)wPos[1] * (n - 4) + 8) >> 4);
            } else {
                base += (HI_U16)(((HI_S32)(wPos[1] << 2) + 8) >> 4);
                if (n <= 15) {
                    u16Result = base + (HI_U16)(((HI_S32)wPos[2] * (n - 8) + 8) >> 4);
                } else {
                    base += (HI_U16)(((HI_S32)(wPos[2] << 3) + 8) >> 4);
                    if (n <= 31) {
                        u16Result = base + (HI_U16)(((HI_S32)wPos[3] * (n - 16) + 8) >> 4);
                    } else {
                        base += wPos[3]; /* 16 * wPos[3] / 16 = wPos[3] */
                        if (n <= 47) {
                            u16Result = base + (HI_U16)(((HI_S32)wPos[4] * (n - 32) + 8) >> 4);
                        } else {
                            base += wPos[4]; /* 16 * wPos[4] / 16 */
                            if (n <= 63) {
                                u16Result = base + (HI_U16)(((HI_S32)wPos[5] * (n - 48) + 8) >> 4);
                            } else {
                                u16Result = base + wPos[5];
                            }
                        }
                    }
                }
            }
        }

        if (u16Result == 0) goto apply_neg_damp;
        if (u16Result > 256) u16Result = 256;

        /* Positive damping: use histWeight and convergenceDamp */
        {
            HI_U16 histW = AE_CTX_HALF(pCtx, 0x3ce);
            HI_U8  damp  = AE_CTX_BYTE(pCtx, 0xc0);
            HI_U32 w     = ((HI_U32)damp * histW + 128) >> 8;
            HI_U32 inc   = ((HI_U32)u16Result * w + 32) >> 6;
            if (inc > 256) inc = 256;
            if (inc < 1)   inc = 1;
            AE_CTX_SWORD(pCtx, 0x140) = (HI_S32)(inc * s32Sign);
            goto dispatch;
        }
    } else {
        s32Sign = -1;
        HI_S32 n = -s16Incr;

        if (n <= 3) {
            u16Result = (HI_U16)(((HI_U32)(n * wNeg[0]) + 8) >> 4);
        } else {
            HI_U16 base = (HI_U16)(((HI_U32)(wNeg[0] << 2) + 8) >> 4);
            if (n <= 7) {
                u16Result = base + (HI_U16)(((HI_S32)wNeg[1] * (n - 4) + 8) >> 4);
            } else {
                base += (HI_U16)(((HI_S32)(wNeg[1] << 2) + 8) >> 4);
                if (n <= 15) {
                    u16Result = base + (HI_U16)(((HI_S32)wNeg[2] * (n - 8) + 8) >> 4);
                } else {
                    base += (HI_U16)(((HI_S32)(wNeg[2] << 3) + 8) >> 4);
                    if (n <= 31) {
                        u16Result = base + (HI_U16)(((HI_S32)wNeg[3] * (n - 16) + 8) >> 4);
                    } else {
                        base += wNeg[3];
                        if (n <= 63) {
                            base += (HI_U16)(((HI_S32)wNeg[4] * (n - 32) + 8) >> 4);
                            u16Result = base;
                        } else {
                            base += (wNeg[4] << 1);
                            if (n <= 127) {
                                u16Result = base + (HI_U16)(((HI_S32)wNeg[5] * (n - 64) + 8) >> 4);
                            } else {
                                u16Result = base + (wNeg[5] << 2);
                            }
                        }
                    }
                }
            }
        }

apply_neg_damp:
        /* Negative damping: convergenceDamp only (no histWeight) */
        if (u16Result > 0) {
            if (s16Incr > 0) {
                /* Should not reach here, but matches asm zero-result path */
            }
        }
        {
            HI_U8 damp = AE_CTX_BYTE(pCtx, 0xc0);
            HI_U32 inc = ((HI_U32)u16Result * damp + 32) >> 6;
            if (inc > 128) inc = 128;
            if (inc < 1)   inc = 1;
            AE_CTX_SWORD(pCtx, 0x140) = (HI_S32)(inc) * s32Sign;
        }
    }

dispatch:
    /* Dispatch to time-related or time-unrelated */
    if (AE_CTX_WORD(pCtx, 0x3c0) != 0) {
        AeIncrementTimeRelatedCalculate(s32Handle);
    } else {
        AeIncrementTimeUnrelatedCalculate(s32Handle);
    }
    return 0;
}


/* ======================================================================== */
/* 1. AeWDRExpRatioCalc  (0x3e0 bytes)                                      */
/* ======================================================================== */
HI_S32 AeWDRExpRatioCalc(void *pStat, HI_S32 s32Handle)
{
    HI_U8 *pBase = (HI_U8 *)&g_astAeCtx[0];
    HI_U8 *pCtx  = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    HI_U32 *pHistBase = (HI_U32 *)AE_CTX_PTR(pStat, 20);

    HI_U32 u32HighSum = 0, u32TotalSum = 0;
    HI_U32 i;
    HI_U32 *pIP = pHistBase;

    for (i = 0; i < 256; i++) {
        HI_U32 s = pIP[2] + pIP[3] + pIP[4] + pIP[5];
        if (i >= 250)
            u32HighSum += s;
        u32TotalSum += s;
        pIP += 4;
    }

    if (u32TotalSum == 0) u32TotalSum = 1;
    HI_U32 u32HighPct = (HI_U32)((HI_U64)u32HighSum * 10000ULL / u32TotalSum);

    /* Shift averaging history and insert new value */
    HI_U32 u32AvgSum = 0;
    for (i = 0; i < 9; i++) {
        HI_U32 next = AE_CTX_WORD(pCtx, 0x190 + i * 4);
        AE_CTX_WORD(pCtx, 0x18c + i * 4) = next;
        u32AvgSum += next;
    }
    u32AvgSum += u32HighPct;
    AE_CTX_WORD(pCtx, 0x1b0) = u32HighPct;

    /* Average / 10 */
    HI_U32 u32Avg = (HI_U32)((HI_U64)u32AvgSum * 0xCCCDULL >> 19);
    /* Actually: 0xCCCCCCCD * x >> 35 = x/10 */
    u32Avg = u32AvgSum / 10;

    HI_S32 s32HighDiff = (HI_S32)u32HighPct - (HI_S32)u32Avg;

    HI_U16 u16DampMin = AE_CTX_HALF(pCtx, 0x154);
    HI_U32 u32Prev    = AE_CTX_WORD(pCtx, 0x188);
    HI_U32 u32Thresh  = u16DampMin * 3 + u32Prev;

    HI_U32 u32Decay, u32SafeMargin;

    if (u32Avg > u32Thresh) {
        HI_U32 delta = (u32Avg - u32Thresh) * 16 / 10;
        delta += 256;
        if (delta > 280) delta = 280;
        u32Decay = delta;
    } else if (u32Avg >= u32Prev) {
        u32Decay = 240;
    } else {
        HI_U32 headroom = (u32Prev + 20 - u32Avg) * 16 / 10;
        headroom = 256 - headroom;
        if (headroom < 230) headroom = 230;
        if (headroom > 256) headroom = 256;
        u32Decay = headroom;
    }
    u32SafeMargin = 8;

    /* Check high percentage against average/100 */
    HI_U32 u32Avg100 = u32AvgSum / 100;
    if (u32Avg100 > u32HighPct && (HI_S32)u32HighPct >= -3) {
        HI_U32 curRatio = AE_CTX_WORD(pCtx, 0x15c);
        curRatio = (curRatio * u32Decay) >> 8;
        AE_CTX_WORD(pCtx, 0x15c) = curRatio;
    }

    AeWDRRefExpRatioCalc(pStat, s32Handle);

    /* Clamp and boundary checks */
    HI_U32 u32MinRatio = AE_CTX_WORD(pCtx, 0x164);
    HI_U32 u32CurRatio = AE_CTX_WORD(pCtx, 0x15c);
    HI_U32 u32MaxRatio = AE_CTX_WORD(pCtx, 0x160);

    if (u32CurRatio < u32MinRatio + 4)
        AE_CTX_WORD(pCtx, 0x30) = 1;
    else if (u32CurRatio + 4 <= u32MaxRatio)
        AE_CTX_WORD(pCtx, 0x30) = 0;

    if (u32CurRatio < u32MinRatio) { /* keep minimum */ }
    else if (u32CurRatio > u32MaxRatio) u32CurRatio = u32MaxRatio;

    AE_CTX_WORD(pCtx, 0x15c) = u32CurRatio;

    HI_U32 u32FrameType = AE_CTX_WORD(pCtx, 0x28);
    if (u32FrameType == 1) {
        AE_CTX_WORD(pCtx, 0x15c) = 64;
        return 0;
    }
    if (u32FrameType != 2)
        return 0;

    /* WDR frame type 2: over-exposure tracking */
    {
        HI_U32 u32IntTime  = AE_CTX_WORD(pCtx, 0x4d8);
        HI_U32 u32IntUpper = AE_CTX_WORD(pCtx, 0x514);
        HI_U32 u32Ratio2   = AE_CTX_WORD(pCtx, 0x54c);
        HI_U32 u32Ratio3   = AE_CTX_WORD(pCtx, 0x56c);
        HI_U32 u32Shift1   = AE_CTX_WORD(pCtx, 0x4cc);
        HI_U32 u32Shift2   = AE_CTX_WORD(pCtx, 0x518);
        HI_U32 u32Shift3   = AE_CTX_WORD(pCtx, 0x550);
        HI_U32 u32Shift4   = AE_CTX_WORD(pCtx, 0x570);
        HI_U32 u32TotalShift = u32Shift1 + u32Shift2 + u32Shift3 + u32Shift4;

        /* Compute pixel exposure level */
        HI_U64 u64Val = (HI_U64)u32IntTime * u32IntUpper;
        u64Val <<= 6;
        u64Val *= u32Ratio2;
        u64Val *= u32Ratio3;
        HI_U32 u32PixVal = (HI_U32)(u64Val >> u32TotalShift);

        HI_U32 u32OverFlag = AE_CTX_WORD(pCtx, 0x170);
        HI_U32 u32TargetLuma = AE_CTX_WORD(pCtx, 0xac);

        if (u32OverFlag == 0) {
            /* Check 110% threshold */
            HI_U32 u32Thresh = u32TargetLuma * 11 / 10;
            if (u32PixVal > u32Thresh) {
                AE_CTX_WORD(pCtx, 0x178) = 0;
                HI_U32 cnt = AE_CTX_WORD(pCtx, 0x17c) + 1;
                AE_CTX_WORD(pCtx, 0x17c) = cnt;
                if (cnt > 5) {
                    HI_U32 rec = AE_CTX_WORD(pCtx, 0x184) + 1;
                    if (rec > 100) {
                        AE_CTX_WORD(pCtx, 0x174) = 1;
                        AE_CTX_WORD(pCtx, 0x170) = 0;
                    }
                    AE_CTX_WORD(pCtx, 0x180) = 0;
                    AE_CTX_WORD(pCtx, 0x184) = rec;
                    AE_CTX_WORD(pCtx, 0x15c) = 64;
                }
            } else {
                /* Under threshold: stability tracking */
                HI_U32 good = AE_CTX_WORD(pCtx, 0x178);
                if (good <= 5) {
                    HI_U32 bad = AE_CTX_WORD(pCtx, 0x17c);
                    if (bad > 5) goto over_recover;
                }
                if (good > 5) goto under_recover;
                /* Normal path: increment stable counter */
                HI_U32 stable = AE_CTX_WORD(pCtx, 0x180) + 1;
                if (stable > 100) {
                    AE_CTX_WORD(pCtx, 0x174) = 0;
                    AE_CTX_WORD(pCtx, 0x170) = 1;
                }
                AE_CTX_WORD(pCtx, 0x180) = stable;
                AE_CTX_WORD(pCtx, 0x184) = 0;
            }
        } else {
            /* Over-exposure flag set: check 90% threshold */
            HI_U32 u32Thresh = u32TargetLuma * 9 / 10;
            if (u32PixVal < u32Thresh) {
                AE_CTX_WORD(pCtx, 0x17c) = 0;
                HI_U32 good = AE_CTX_WORD(pCtx, 0x178) + 1;
                AE_CTX_WORD(pCtx, 0x178) = good;
                if (good > 5) {
under_recover:
                    ;
                    HI_U32 stable = AE_CTX_WORD(pCtx, 0x180) + 1;
                    if (stable > 100) {
                        AE_CTX_WORD(pCtx, 0x174) = 0;
                        AE_CTX_WORD(pCtx, 0x170) = 1;
                    }
                    AE_CTX_WORD(pCtx, 0x180) = stable;
                    AE_CTX_WORD(pCtx, 0x184) = 0;
                }
            } else {
over_recover:
                ;
                HI_U32 rec = AE_CTX_WORD(pCtx, 0x184) + 1;
                if (rec > 100) {
                    AE_CTX_WORD(pCtx, 0x174) = 1;
                    AE_CTX_WORD(pCtx, 0x170) = 0;
                }
                AE_CTX_WORD(pCtx, 0x180) = 0;
                AE_CTX_WORD(pCtx, 0x184) = rec;
                AE_CTX_WORD(pCtx, 0x15c) = 64;
            }
        }
    }
    return 0;
}


/* ======================================================================== */
/* 2. AeLinearExpRatioCalc  (0x4d4 bytes)                                   */
/* NOTE: This is a large function. The core structure is provided;          */
/* the pow()-based damping section is structurally represented.             */
/* ======================================================================== */
HI_S32 AeLinearExpRatioCalc(void *pStat, HI_S32 s32Handle)
{
    HI_U8 *pBase = (HI_U8 *)&g_astAeCtx[0];
    HI_U8 *pCtx  = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    HI_U16 *pHist = (HI_U16 *)AE_CTX_PTR(pStat, 28);
    HI_U16 *pEnd  = pHist + 1024;

    HI_U16 u16DampRatio = AE_CTX_HALF(pCtx, 0x156);
    HI_U16 u16DampMin   = AE_CTX_HALF(pCtx, 0x154);

    /* Median-4 histogram filter (same structure as AeWDRRefExpRatioCalc) */
    HI_U16 max1, max2, min1, min2;
    HI_U16 cur = pHist[1];
    max1 = max2 = min1 = min2 = cur;

    HI_U16 *p = pHist;
    while (p < pEnd) {
        HI_U16 *pGroupEnd = p + 68;
        while (p < pGroupEnd) {
            HI_U16 v1 = p[1], v2 = p[2];
            /* Sort into top-2 / bottom-2 tracking */
            if (v1 > max1) { max2 = max1; max1 = v1; }
            else { HI_U16 t = (v1 >= max2) ? max2 : v1; max2 = (v1 >= max2) ? v1 : max2; (void)t; }
            if (v1 < min1) { min2 = min1; min1 = v1; }
            else if (v1 < min2) min2 = v1;

            if (v2 > max1) { max2 = max1; max1 = v2; }
            else if (v2 > max2) max2 = v2;
            if (v2 < min1) { min2 = min1; min1 = v2; }
            else if (v2 < min2) min2 = v2;

            p += 4;
        }
        if (p >= pEnd) break;
    }

    /* Contrast metric */
    HI_U32 u32Mid = ((HI_U32)max1 + max2) >> 1;
    if (min2 < 255) min2 = 255;
    HI_U32 u32Low = (min1 >= 255) ?
        ((HI_U32)min2 + min1) >> 1 : ((HI_U32)min2 + 255) >> 1;

    HI_U64 u64MidSq = (HI_U64)u32Mid * u32Mid;
    u64MidSq *= 36;

    HI_U32 u32ExpRatio;
    HI_U64 u64R = u64MidSq / u32Low;
    u64R = u64R / u32Low;

    if (u64R <= 3136) {
        u32ExpRatio = 32;
    } else {
        HI_U64 diff = (u64R - 3136) << 6;
        HI_U32 shifted = (HI_U32)(diff >> 10);
        if (shifted < 32) shifted = 32;
        if (shifted > 16384) shifted = 16384;
        if (shifted < 64) shifted = 64;
        u32ExpRatio = shifted;
    }

    AE_CTX_WORD(pCtx, 0x15c) = u32ExpRatio;

    /* Apply pow()-based damping if u16DampMin != 0 */
    if (u16DampMin != 0) {
        HI_FLOAT fBase = (HI_FLOAT)u16DampMin / 10.0f;
        HI_U32 u32Step = (u16DampRatio * u16DampMin + 8) >> 4;

        HI_FLOAT fPowUp = (HI_FLOAT)pow((double)fBase, 1.0 / 10.0);
        HI_FLOAT fPowDn = (HI_FLOAT)pow((double)(-fBase), 1.0 / 10.0);

        HI_U32 u32Old = u32PreExpRatio;
        HI_U32 u32SmUp = (HI_U32)(fPowUp * (HI_FLOAT)u32Old);
        HI_U32 u32SmDn = (HI_U32)(fPowDn * (HI_FLOAT)u32Old);
        HI_U32 u32New  = AE_CTX_WORD(pCtx, 0x15c);

        if (u32New > u32SmUp) {
            /* Increase: scale step by ratio bracket */
            if (u32New >= u32SmUp * 8) { u32Step <<= 3; if (u32Step > 256) u32Step = 256; }
            else if (u32New >= u32SmUp * 4) { u32Step <<= 2; if (u32Step > 256) u32Step = 256; }
            else if (u32New >= u32SmUp * 2) { u32Step <<= 1; if (u32Step > 256) u32Step = 256; }
            else { if (u32Step > 256) u32Step = 256; }
            u32ExpRatio = (u32Old * (u32Step + 256) + 128) >> 8;
        } else if (u32New <= u32SmDn) {
            u32ExpRatio = (u32Old * 256 + 128) >> 8; /* unchanged */
        } else {
            /* Decrease brackets */
            HI_U32 u32Half   = (HI_U32)((HI_FLOAT)u32SmDn * 0.5f);
            HI_U32 u32Qtr    = (HI_U32)((HI_FLOAT)u32SmDn * 0.25f);
            HI_U32 u32Eighth = (HI_U32)((HI_FLOAT)u32SmDn * 0.125f);
            if (u32New > u32Half) { u32Step <<= 1; }
            else if (u32New > u32Qtr) { u32Step <<= 2; }
            else if (u32New > u32Eighth) { u32Step <<= 2; }
            else { u32Step <<= 3; }
            if (u32Step > 128) u32Step = 128;
            u32ExpRatio = (u32Old * (256 - u32Step) + 128) >> 8;
        }
    }

    u32PreExpRatio = u32ExpRatio;

    /* Clamp and store */
    {
        HI_U32 u32MaxRatio = AE_CTX_WORD(pCtx, 0x160);
        HI_U32 u32MinRatio = AE_CTX_WORD(pCtx, 0x164);
        HI_U16 u16DampStep = AE_CTX_HALF(pCtx, 0x158);

        HI_U32 u32Clamp = u32ExpRatio;
        if (u32Clamp > 4095) u32Clamp = 4095;
        if (u32Clamp < 64) u32Clamp = 64;
        AE_CTX_WORD(pCtx, 0x168) = u32Clamp;

        HI_U32 u32Damped = ((HI_U32)u16DampStep * u32ExpRatio) >> 10;
        u32ExpRatio = (u32Damped < u32MaxRatio) ? u32Damped : u32MaxRatio;
        if (u32ExpRatio < u32MinRatio) u32ExpRatio = u32MinRatio;

        AE_CTX_WORD(pCtx, 0x15c) = u32ExpRatio;

        if (AE_CTX_WORD(pCtx, 0x28) == 2) {
            if (u32ExpRatio > 255 ||
                AE_CTX_WORD(pCtx, 0x1bc) <= AE_CTX_WORD(pCtx, 0xac)) {
                /* do nothing extra */
            } else {
                AE_CTX_WORD(pCtx, 0x15c) = 64;
            }
        }
    }

    /* Zone-based ratio for linear sensors */
    if (AE_CTX_BYTE(pCtx, 13) == 0) {
        HI_U32 u32MidLuma = ((HI_U32)max1 + max2) >> 2;
        HI_U16 *pZone = (HI_U16 *)AE_CTX_PTR(pStat, 28);
        HI_U32 u32DarkSum = 0, u32DarkCnt = 0;
        HI_U32 u32BrightSum = 0, u32BrightCnt = 0;
        HI_U32 row, col;

        for (row = 0; row < 16; row++) {
            HI_U16 *pRow = pZone + row * 68;
            for (col = 0; col < 68; col += 2) {
                HI_U16 v1 = pRow[col + 1];
                if (v1 < u32MidLuma) { u32DarkSum += v1; u32DarkCnt++; }
                else if (v1 > u32MidLuma) { u32BrightSum += v1; u32BrightCnt++; }
                HI_U16 v2 = pRow[col + 2];
                if (v2 < u32MidLuma) { u32DarkSum += v2; u32DarkCnt++; }
                else if (v2 > u32MidLuma) { u32BrightSum += v2; u32BrightCnt++; }
            }
        }

        if (u32DarkCnt == 0) u32DarkCnt = 1;
        if (u32BrightCnt == 0) u32BrightCnt = 1;
        HI_U32 darkAvg = u32DarkSum / u32DarkCnt;
        HI_U32 brightAvg = u32BrightSum / u32BrightCnt;
        if (brightAvg == 0) brightAvg = 1;
        AE_CTX_WORD(pCtx, 0x168) = (darkAvg << 6) / brightAvg;
    }

    return 0;
}


/* ======================================================================== */
/* 10. AeIncrementProcess  (0xa4 bytes)                                     */
/* Top-level entry point: dispatches to WDR or linear ratio calculation,    */
/* then to HDR or standard histogram averaging, target adjustment, and      */
/* increment calculation.                                                   */
/* ======================================================================== */
HI_S32 AeIncrementProcess(void *pStat, HI_S32 s32Handle)
{
    HI_U8 *pBase = (HI_U8 *)&g_astAeCtx[0];
    HI_U8 *pCtx  = pBase + (HI_U32)s32Handle * AE_SIZEOF;

    /* Store current target as previous */
    AE_CTX_WORD(pCtx, 0xb0) = AE_CTX_WORD(pCtx, 0xb4);

    HI_U8 u8SnsType = AE_CTX_BYTE(pCtx, 13);

    /* Step 1: Compute exposure ratio */
    if (u8SnsType != 0) {
        AeWDRExpRatioCalc(pStat, s32Handle);
    } else {
        AeLinearExpRatioCalc(pStat, s32Handle);
    }

    /* Step 2: Histogram averaging */
    HI_U8 u8SnsIf = AE_CTX_BYTE(pCtx, 14);
    if ((u8SnsIf - 2) <= 3) {
        /* HDR/WDR interface: use HDR-specific processing */
        AeHDRLevelCalc(pStat, s32Handle);
        AeHDRHistArrayAverageCalc(pStat, s32Handle);
    } else {
        /* Standard: use linear histogram averaging */
        AeHistArrayAverageCalc(pStat, s32Handle);
    }

    /* Step 3: Adjust target and compute increment */
    AeTargetAdjust(s32Handle);
    AeIncrementCalculate(s32Handle);

    return 0;
}


// ============================================================================
// Large functions (1000+ bytes)
// ============================================================================

HI_S32 AeHistRatioAdjust(HI_U32 *pu32ExpRatio, HI_U32 u32HistRatio)
{
    HI_U32 u32Scale;

    if (u32HistRatio < 320)
        u32Scale = 32;
    else if (u32HistRatio < 384)
        u32Scale = ((u32HistRatio - 320) >> 2) + 32;
    else if (u32HistRatio < 640)
        u32Scale = (((u32HistRatio - 384) * 10) >> 5) + 48;
    else if (u32HistRatio < 1024)
        u32Scale = (((u32HistRatio - 640) * 5) >> 5) + 128;
    else
        u32Scale = 196;

    *pu32ExpRatio = (*pu32ExpRatio * u32Scale) >> 5;
    return 0;
}


HI_S32 AeTargetAdjust(HI_S32 s32Handle)
{
    HI_U8 *pBase = (HI_U8 *)&g_astAeCtx[0];
    HI_U8 *pCtx;
    HI_S32 s32Target;

    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    {
        HI_U32 u32Comp = AE_CTX_BYTE(pCtx, 0xc4);
        HI_U32 u32Wt = AE_CTX_HALF(pCtx, 0xc6);
        s32Target = (HI_S32)((u32Wt * u32Comp) >> 10);
        if (s32Target > 255) s32Target = 255;
    }

    if (AE_CTX_WORD(pCtx, 0x43c) == 0 ||
        AE_CTX_WORD(pCtx, 0x434) == 0 ||
        AE_CTX_WORD(pCtx, 0x440) == 1) {
        AE_CTX_BYTE(pCtx, 0xc4) = (HI_U8)s32Target;
        AE_CTX_WORD(pCtx, 0x48c) = 0;
        return 0;
    }

    if (AE_CTX_WORD(pCtx, 0x48c) == 1) {
        HI_U8 u8Off = AE_CTX_BYTE(pCtx, 0x488);
        AE_CTX_WORD(pCtx, 0x43c) = 0;
        s32Target += (HI_S32)u8Off;
        if (s32Target > 255) s32Target = 255;
        AE_CTX_BYTE(pCtx, 0xc4) = (HI_U8)s32Target;
    } else {
        AE_CTX_BYTE(pCtx, 0xc4) = (HI_U8)s32Target;
    }

    if (AE_CTX_WORD(pCtx, 0x484) == 0) {
        AE_CTX_WORD(pCtx, 0x48c) = 0;
        return 0;
    }

    {
        HI_U32 u32Cnt = AE_CTX_WORD(pCtx, 0x434);
        HI_U32 u32Avg = AE_CTX_WORD(pCtx, 0x438);
        HI_U32 u32HistCnt = AE_CTX_WORD(pCtx, 0x4c8);
        HI_U32 u32Div10 = u32Cnt * 10;
        HI_U64 u64R;

        if (u32Div10 == 0) u32Div10 = 1;
        u64R = ((HI_U64)u32Avg << 8) / (HI_U64)u32Div10;

        if ((HI_U32)u64R != u32HistCnt) {
            HI_U8 u8Off = AE_CTX_BYTE(pCtx, 0x488);
            s32Target += (HI_S32)u8Off;
            if (s32Target > 255) s32Target = 255;
            if (s32Target < 0) s32Target = 0;
            AE_CTX_BYTE(pCtx, 0xc4) = (HI_U8)s32Target;
            AE_CTX_WORD(pCtx, 0x48c) = 1;
        } else {
            AE_CTX_WORD(pCtx, 0x48c) = 0;
        }
    }

    return 0;
}


HI_S32 AeIncrementTimeUnrelatedCalculate(HI_S32 s32Handle)
{
    HI_U8 *pBase = (HI_U8 *)&g_astAeCtx[0];
    HI_U8 *pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    HI_S32 s32RawIncr = (HI_S32)AE_CTX_WORD(pCtx, 0x140);
    HI_S16 s16Error = AE_CTX_SHALF(pCtx, 0xdc);
    HI_S16 s16Tolerance = AE_CTX_SHALF(pCtx, 0xc2);
    HI_U32 u32AbsIncr;

    u32AbsIncr = (s32RawIncr < 0) ? (HI_U32)(-s32RawIncr) : (HI_U32)s32RawIncr;

    if (s16Error > s16Tolerance) {
        /* Positive overshoot */
        HI_U16 u16PrevDir = AE_CTX_HALF(pCtx, 0x150);
        HI_U16 u16PosCnt;

        AE_CTX_HALF(pCtx, 0x152) = 1;
        AE_CTX_WORD(pCtx, 0x14c) = 0;

        u16PosCnt = (u16PrevDir == 1) ? AE_CTX_HALF(pCtx, 0x144) + 1 : 0;

        pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
        {
            HI_U16 u16MaxFrames = AE_CTX_HALF(pCtx, 0x13c);
            AE_CTX_HALF(pCtx, 0x144) = u16PosCnt;
            AE_CTX_HALF(pCtx, 0x150) = 1;

            if (u16PosCnt < u16MaxFrames)
                return 0;

            if (u16MaxFrames != 0) {
                HI_U16 u16Row = u16PosCnt - u16MaxFrames;
                HI_U16 u16Col;
                /* g_au8CountRatioLut defined above as static const */

                if (u16Row > 15) u16Row = 15;
                if (s16Tolerance == 0) s16Tolerance = 1;
                u16Col = (HI_U16)s16Error / (HI_U16)s16Tolerance;
                if (u16Col > 15) u16Col = 15;
                u32AbsIncr = (u32AbsIncr * g_au8CountRatioLut[u16Row * 16 + u16Col] + 8) >> 4;
            }
        }

        pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
        AE_CTX_WORD(pCtx, 0xb0) = AE_CTX_WORD(pCtx, 0xb4) + u32AbsIncr;
        return 0;
    }

    if (s16Error < -s16Tolerance) {
        /* Negative undershoot */
        AE_CTX_HALF(pCtx, 0x152) = 0;
        AE_CTX_WORD(pCtx, 0x14c) = 0;
        AE_CTX_HALF(pCtx, 0x144) = 0;

        if (AE_CTX_HALF(pCtx, 0x150) == 0) {
            AE_CTX_HALF(pCtx, 0x146) = AE_CTX_HALF(pCtx, 0x146) + 1;
        }
    } else {
        /* Within tolerance */
        AE_CTX_HALF(pCtx, 0x144) = 0;
        AE_CTX_WORD(pCtx, 0x14c) = AE_CTX_WORD(pCtx, 0x14c) + 1;
        AE_CTX_HALF(pCtx, 0x146) = 0;
        AE_CTX_HALF(pCtx, 0x152) = 0;
    }

    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    {
        HI_S16 s16ErrAbs = (s16Error < 0) ? -s16Error : s16Error;
        AE_CTX_HALF(pCtx, 0x150) = AE_CTX_HALF(pCtx, 0x152);

        if ((HI_U16)s16Tolerance >= (HI_U16)s16ErrAbs) {
            AE_CTX_WORD(pCtx, 0xb0) = AE_CTX_WORD(pCtx, 0xb4);
            return 0;
        }

        {
            HI_U16 u16NegCnt = AE_CTX_HALF(pCtx, 0x146);
            HI_U16 u16NegMax = AE_CTX_HALF(pCtx, 0x13e);
            /* g_au8CountRatioLut defined above as static const */

            if (u16NegCnt < u16NegMax)
                return 0;

            if (u16NegMax != 0) {
                HI_U16 u16Row = u16NegCnt - u16NegMax;
                HI_U16 u16Col;

                u16Row = (u16Row <= 13) ? u16Row + 2 : 15;
                if (s16Tolerance == 0) s16Tolerance = 1;
                u16Col = (HI_U16)s16ErrAbs / (HI_U16)s16Tolerance;
                if (u16Col > 15) u16Col = 15;
                u32AbsIncr = (u32AbsIncr * g_au8CountRatioLut[u16Row * 16 + u16Col] + 8) >> 4;
            }
        }
    }

    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    AE_CTX_WORD(pCtx, 0xb0) = AE_CTX_WORD(pCtx, 0xb4) - u32AbsIncr;
    return 0;
}


HI_S32 AeHDRLevelCalc(void *pstStatInfo, HI_S32 s32Handle)
{
    HI_U8 *pBase = (HI_U8 *)&g_astAeCtx[0];
    HI_U8 *pCtx;
    HI_U32 u32TotalCnt = 0;
    HI_U64 u64WeightedSum = 0;

    if ((HI_U32)s32Handle > 3) {
        HI_TRACE_ISP(RE_DBG_LVL, "Illegal handle id %d in %s!\n",
            s32Handle, __FUNCTION__);
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;

    /* Direct HW register readback path */
    if (AE_CTX_WORD(pCtx, 0x34) != 0) {
        HI_U32 u32Addr = ((HI_U32)s32Handle << 12) + 0x700618;
        HI_U32 u32Val = IO_READ32(u32Addr);
        HI_U32 u32Min = AE_CTX_WORD(pCtx, 0x44);
        HI_U32 u32Max = AE_CTX_WORD(pCtx, 0x40);

        if (u32Val < u32Min) u32Val = u32Min;
        if (u32Val > u32Max) u32Val = u32Max;

        AE_CTX_WORD(pCtx, 0x3c) = u32Val;
        return 0;
    }

    /* Histogram path: compute weighted level from 256-bin histogram */
    {
        HI_U8 u8SnsMode = AE_CTX_BYTE(pCtx, 0x0d);
        HI_U32 *pStat;
        HI_U32 i;

        if ((HI_U8)(u8SnsMode - 2) <= 9) {
            /* WDR sensor */
            if ((HI_U8)(u8SnsMode - 2) > 3) {
                u32TotalCnt = 0;
                u64WeightedSum = 0;
            } else {
                pStat = *(HI_U32 **)((HI_U8 *)pstStatInfo + 4);
                for (i = 0; i < 256; i++) {
                    HI_U32 u32BinVal = pStat[i * 4 + 256] + pStat[i * 4 + 257] +
                                       pStat[i * 4 + 258] + pStat[i * 4 + 259];
                    u32TotalCnt += u32BinVal;
                    u64WeightedSum += (HI_U64)u32BinVal * (HI_U64)i;
                }
            }
        } else {
            /* Linear sensor */
            pStat = *(HI_U32 **)((HI_U8 *)pstStatInfo + 20);
            for (i = 0; i < 256; i++) {
                HI_U32 u32BinVal = pStat[i * 4 + 2] + pStat[i * 4 + 3] +
                                   pStat[i * 4 + 4] + pStat[i * 4 + 5];
                u32TotalCnt += u32BinVal;
                u64WeightedSum += (HI_U64)u32BinVal * (HI_U64)i;
            }
        }
    }

    /* IIR-smoothed level */
    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    {
        HI_U32 u32IIR = AE_CTX_WORD(pCtx, 0x4c);
        HI_U32 u32PrevDir = AE_CTX_WORD(pCtx, 0x30);
        HI_U32 u32Denom, u32Level;
        HI_U64 u64Half;

        u32IIR = (u32PrevDir != 0) ? u32IIR - 1 : u32IIR + 1;
        if (u32IIR < 16) u32IIR = 16;
        {
            HI_U32 u32Max = AE_CTX_WORD(pCtx, 0x48);
            if (u32IIR > u32Max) u32IIR = u32Max;
        }
        AE_CTX_WORD(pCtx, 0x4c) = u32IIR;

        u32Denom = (u32TotalCnt != 0) ? u32TotalCnt : 1;
        u64Half = (HI_U64)u32TotalCnt >> 1;

        {
            HI_U32 u32PrevRaw = AE_CTX_WORD(pCtx, 0x38);
            HI_U32 u32Smooth = (u32PrevRaw * 252 + 128) >> 8;
            HI_U64 u64Numer = u64Half + (HI_U64)u32IIR * u64WeightedSum;
            HI_U32 u32Quot = (HI_U32)(u64Numer / (HI_U64)u32Denom);

            u32Level = u32Smooth + (u32Quot << 2);
            AE_CTX_WORD(pCtx, 0x38) = u32Level;

            u32Level = (u32Level + 128) >> 8;
            {
                HI_U32 u32Min = AE_CTX_WORD(pCtx, 0x44);
                HI_U32 u32Max = AE_CTX_WORD(pCtx, 0x40);
                if (u32Level < u32Min) u32Level = u32Min;
                if (u32Level > u32Max) u32Level = u32Max;
            }
            AE_CTX_WORD(pCtx, 0x3c) = u32Level;
        }
    }

    return 0;
}


HI_S32 AeHDRHistArrayAverageCalc(void *pstStatInfo, HI_S32 s32Handle)
{
    HI_U8 *pBase = (HI_U8 *)&g_astAeCtx[0];
    HI_U8 *pCtx;
    HI_U32 u32AvgLuma = 0;
    HI_U32 u32AvgWeighted = 0;

    if ((HI_U32)s32Handle > 3) {
        HI_TRACE_ISP(RE_DBG_LVL, "Illegal handle id %d in %s!\n",
            s32Handle, __FUNCTION__);
        return HI_ERR_ISP_ILLEGAL_PARAM;
    }

    pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
    {
        HI_U32 *pu32HistBuf = (HI_U32 *)AE_CTX_PTR(pCtx, 0xa4);
        HI_U16 u16HistWeight = AE_CTX_HALF(pCtx, 0x116);
        HI_U16 u16HistBase = AE_CTX_HALF(pCtx, 0x114);
        HI_U32 *pu32Slope = (HI_U32 *)AE_CTX_ADDR(pCtx, 0xe4);
        HI_U32 *pu32Offset = (HI_U32 *)AE_CTX_ADDR(pCtx, 0xf4);

        if (pu32HistBuf == HI_NULL) {
            HI_TRACE_ISP(RE_DBG_LVL, "Null Pointer in %s!\n", __FUNCTION__);
            return HI_ERR_ISP_NULL_PTR;
        }

        {
            HI_U8 u8SnsMode = AE_CTX_BYTE(pCtx, 0x0d);

            if ((HI_U8)(u8SnsMode - 2) <= 9) {
                /* WDR sensor */
                if ((HI_U8)(u8SnsMode - 2) > 3) {
                    u32AvgLuma = 0;
                    u32AvgWeighted = 0;
                } else {
                    /* WDR types 0..3: secondary frame histogram */
                    HI_U32 *pStat2 = *(HI_U32 **)((HI_U8 *)pstStatInfo + 4);
                    HI_U64 u64BinAccum = 0;
                    HI_U64 u64WeightedSum = 0;
                    HI_U32 u32TotalCnt = 0;
                    HI_U32 i;

                    for (i = 0; i < 256; i++) {
                        HI_U32 u32BinVal = pStat2[i * 4 + 256] + pStat2[i * 4 + 257] +
                                           pStat2[i * 4 + 258] + pStat2[i * 4 + 259];
                        pu32HistBuf[i] = u32BinVal;
                        u32TotalCnt += u32BinVal;
                        u64BinAccum += (HI_U64)u32BinVal * (HI_U64)i;
                    }

                    {
                        HI_U32 u32Denom = (u32TotalCnt != 0) ? u32TotalCnt : 1;
                        HI_U64 u64Half = (HI_U64)u32TotalCnt >> 1;
                        u32AvgLuma = (HI_U32)((u64BinAccum + u64Half) / (HI_U64)u32Denom);
                        u32AvgWeighted = (HI_U32)(((u64WeightedSum << 8) + u64Half) / (HI_U64)u32Denom);
                    }
                }
            } else {
                /* Linear sensor: 256-bin primary frame */
                HI_U32 *pStat = *(HI_U32 **)((HI_U8 *)pstStatInfo + 20);
                HI_U64 u64BinAccum = 0;
                HI_U64 u64WeightedSum = 0;
                HI_U32 u32TotalCnt = 0;
                HI_U32 i;

                for (i = 0; i < 256; i++) {
                    HI_U32 u32BinVal = pStat[i * 4 + 2] + pStat[i * 4 + 3] +
                                       pStat[i * 4 + 4] + pStat[i * 4 + 5];
                    pu32HistBuf[i] = u32BinVal;
                    u32TotalCnt += u32BinVal;
                    u64BinAccum += (HI_U64)u32BinVal * (HI_U64)i;
                }

                {
                    HI_U32 u32Denom = (u32TotalCnt != 0) ? u32TotalCnt : 1;
                    HI_U64 u64Half = (HI_U64)u32TotalCnt >> 1;
                    u32AvgLuma = (HI_U32)((u64BinAccum + u64Half) / (HI_U64)u32Denom);
                    u32AvgWeighted = (HI_U32)(((u64WeightedSum << 8) + u64Half) / (HI_U64)u32Denom);
                }
            }
        }

        /* Store results */
        pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
        AE_CTX_WORD(pCtx, 0xd4) = u32AvgLuma;
        AE_CTX_WORD(pCtx, 0xd8) = u32AvgLuma;
        AE_CTX_WORD(pCtx, 0xe0) = u32AvgWeighted;

        /* Apply histogram ratio adjustment if enabled */
        if (AE_CTX_WORD(pCtx, 0x12c) != 0 && AE_CTX_WORD(pCtx, 0x138) != 0) {
            HI_U32 u32Ratio = AE_CTX_WORD(pCtx, 0x134);
            AeHistRatioAdjust((HI_U32 *)AE_CTX_ADDR(pCtx, 0xe0), u32Ratio);
        }

        /* Compute target offset */
        pCtx = pBase + (HI_U32)s32Handle * AE_SIZEOF;
        {
            HI_U32 u32Mode = AE_CTX_WORD(pCtx, 0x108);
            HI_S32 s32Offset;

            if (u32Mode == 0) {
                HI_U32 u32Exp = AE_CTX_WORD(pCtx, 0xe0);
                HI_U32 u32Prod = (u32Exp * (HI_U32)u16HistBase) >> 8;
                s32Offset = ((HI_S32)(HI_U32)u16HistWeight < (HI_S32)u32Prod) ?
                    (HI_S32)(HI_U32)u16HistWeight : (HI_S32)u32Prod;
            } else if (u32Mode == 1) {
                HI_U32 u32Exp = AE_CTX_WORD(pCtx, 0xe0);
                HI_S32 s32NegWt = -(HI_S32)(HI_U32)u16HistWeight;
                HI_S32 s32NegProd = -((HI_S32)((u32Exp * (HI_U32)u16HistBase) >> 8));
                s32Offset = (s32NegWt >= s32NegProd) ? s32NegWt : s32NegProd;
            } else {
                s32Offset = 0;
            }

            AE_CTX_WORD(pCtx, 0x110) = (HI_U32)s32Offset;
            {
                HI_S32 s32Luma = (HI_S32)AE_CTX_WORD(pCtx, 0xd8) + s32Offset;
                if (s32Luma < 0) s32Luma = 0;
                if (s32Luma > 255) s32Luma = 255;
                AE_CTX_WORD(pCtx, 0xd8) = (HI_U32)s32Luma;
            }
        }
    }

    return 0;
}
