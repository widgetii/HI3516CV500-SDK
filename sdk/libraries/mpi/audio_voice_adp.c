/**
 * Reverse Engineered by TekuConcept on April 24, 2021
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mpi_audio.h"

/* External VoiceEngine API (resolved at link time) */
extern HI_S32 HI_VOICE_EncReset(HI_VOID *pState, HI_S32 s32Type);
extern HI_S32 HI_VOICE_EncodeFrame(HI_VOID *pState, HI_VOID *pInBuf,
                                    HI_VOID *pOutBuf, HI_S32 s32Param);
extern HI_S32 HI_VOICE_DecReset(HI_VOID *pState, HI_S32 s32Type);
extern HI_S32 HI_VOICE_DecodeFrame(HI_VOID *pState, HI_VOID *pInBuf,
                                    HI_VOID *pOutBuf, HI_S16 *ps16OutLen);

/* Per-codec context structures */
typedef struct {
    HI_S32 codec_type;
    HI_U8  voice_state[8];
} G711_CTX_S; /* 12 bytes */

typedef struct {
    HI_S32 codec_type;
    HI_S32 adpcm_type;
    HI_U8  voice_state[16];
} ADPCM_CTX_S; /* 24 bytes */

typedef struct {
    HI_S32 codec_type;
    HI_U8  voice_state[128];
} G726_CTX_S; /* 132 bytes */


/* ========== LPCM ========== */

HI_S32
OpenLPCMEncoder(HI_VOID *pEncoderAttr, HI_VOID **ppEncoder)
{ return HI_SUCCESS; }

HI_S32
CloseLPCMEncoder(HI_VOID *pEncoder)
{ return HI_SUCCESS; }

HI_S32
EncodeLPCMFrm(
    HI_VOID             *pEncoder,
    const AUDIO_FRAME_S *pstData,
    HI_U8               *pu8Outbuf,
    HI_U32              *pu32OutLen)
{
    if (!pu32OutLen || !pu8Outbuf || !pstData)
        return -1;

    memcpy_s(pu8Outbuf, pstData->u32Len,
             pstData->u64VirAddr[0], pstData->u32Len);
    *pu32OutLen = pstData->u32Len;
    return HI_SUCCESS;
}

HI_S32
OpenLPCMDecoder(HI_VOID *pDecoderAttr, HI_VOID **ppDecoder)
{ return HI_SUCCESS; }

HI_S32
CloseLPCMDecoder(HI_VOID *pDecoder)
{ return HI_SUCCESS; }

HI_S32
GetFrmInfo(HI_VOID *pDecoder, HI_VOID *pInfo)
{ return HI_SUCCESS; }

HI_S32
DecodeLPCMFrm(
    HI_VOID  *pDecoder,
    HI_U8   **pu8Inbuf,
    HI_S32   *ps32LeftByte,
    HI_U16   *pu16Outbuf,
    HI_U32   *pu32OutLen,
    HI_U32   *pu32Chns)
{
    if (!pu8Inbuf || !ps32LeftByte || !pu16Outbuf || !pu32OutLen || !pu32Chns)
        return -1;

    *pu32Chns = 1;
    memcpy_s(pu16Outbuf, *ps32LeftByte, *pu8Inbuf, *ps32LeftByte);
    *pu32OutLen = *ps32LeftByte;
    *ps32LeftByte = 0;
    return HI_SUCCESS;
}


/* ========== G711A Encoder ========== */

HI_S32
OpenG711AEncoder(HI_VOID *pEncoderAttr, HI_VOID **ppEncoder)
{
    G711_CTX_S *ctx;
    HI_S32 ret;

    if (!ppEncoder) return -1;

    ctx = (G711_CTX_S *)malloc(sizeof(G711_CTX_S));
    if (!ctx) {
        printf("%s, %d, malloc G711A encoder context failed!\n",
               __FUNCTION__, __LINE__);
        return HI_ERR_AENC_NOMEM;
    }

    memset_s(ctx, sizeof(G711_CTX_S), 0, sizeof(G711_CTX_S));
    *ppEncoder = ctx;
    ctx->codec_type = 1;

    ret = HI_VOICE_EncReset(ctx->voice_state, 1);
    if (ret != 0) {
        free(ctx);
        *ppEncoder = HI_NULL;
    }
    return ret;
}

HI_S32
EncodeG711AFrm(
    HI_VOID             *pEncoder,
    const AUDIO_FRAME_S *pstData,
    HI_U8               *pu8Outbuf,
    HI_U32              *pu32OutLen)
{
    G711_CTX_S *ctx = (G711_CTX_S *)pEncoder;
    HI_U32 ptNum;

    if (!pEncoder || !pstData || !pu8Outbuf || !pu32OutLen)
        return -1;

    ptNum = pstData->u32Len / (pstData->enBitwidth + 1);
    if (ptNum != 80 && ptNum != 160 && ptNum != 240 &&
        ptNum != 320 && ptNum != 480) {
        printf("%s, %d, points:%d of each frame is illegal!\n",
               __FUNCTION__, __LINE__, ptNum);
        return HI_ERR_AENC_ILLEGAL_PARAM;
    }

    HI_VOICE_EncodeFrame(ctx->voice_state, pstData->u64VirAddr[0],
                          pu8Outbuf, (HI_S16)(pstData->u32Len >> 1));

    *pu32OutLen = (*(HI_U16 *)(pu8Outbuf + 2) + 2) * 2;
    return HI_SUCCESS;
}

HI_S32
CloseG711AEncoder(HI_VOID *pEncoder);


/* ========== G711U Encoder ========== */

HI_S32
OpenG711UEncoder(HI_VOID *pEncoderAttr, HI_VOID **ppEncoder)
{
    G711_CTX_S *ctx;
    HI_S32 ret;

    if (!ppEncoder || !pEncoderAttr) return -1;

    ctx = (G711_CTX_S *)malloc(sizeof(G711_CTX_S));
    if (!ctx) {
        printf("%s, %d, malloc G711U encoder context failed!\n",
               __FUNCTION__, __LINE__);
        return HI_ERR_AENC_NOMEM;
    }

    memset_s(ctx, sizeof(G711_CTX_S), 0, sizeof(G711_CTX_S));
    *ppEncoder = ctx;
    ctx->codec_type = 2;

    ret = HI_VOICE_EncReset(ctx->voice_state, 2);
    if (ret != 0) {
        free(ctx);
        *ppEncoder = HI_NULL;
    }
    return ret;
}

HI_S32
EncodeG711UFrm(
    HI_VOID             *pEncoder,
    const AUDIO_FRAME_S *pstData,
    HI_U8               *pu8Outbuf,
    HI_U32              *pu32OutLen)
{
    G711_CTX_S *ctx = (G711_CTX_S *)pEncoder;
    HI_U32 ptNum;

    if (!pEncoder || !pstData || !pu8Outbuf || !pu32OutLen)
        return -1;

    ptNum = pstData->u32Len / (pstData->enBitwidth + 1);
    if (ptNum != 80 && ptNum != 160 && ptNum != 240 &&
        ptNum != 320 && ptNum != 480) {
        printf("%s, %d, points:%d of each frame is illegal!\n",
               __FUNCTION__, __LINE__, ptNum);
        return HI_ERR_AENC_ILLEGAL_PARAM;
    }

    HI_VOICE_EncodeFrame(ctx->voice_state, pstData->u64VirAddr[0],
                          pu8Outbuf, (HI_S16)(pstData->u32Len >> 1));

    *pu32OutLen = (*(HI_U16 *)(pu8Outbuf + 2) + 2) * 2;
    return HI_SUCCESS;
}

HI_S32
CloseG711UEncoder(HI_VOID *pEncoder)
{
    G711_CTX_S *ctx = (G711_CTX_S *)pEncoder;
    if (!ctx) return -1;

    HI_VOICE_EncReset(ctx->voice_state, (HI_S16)ctx->codec_type);
    free(ctx);
    return HI_SUCCESS;
}

HI_S32
CloseG711AEncoder(HI_VOID *pEncoder)
{
    return CloseG711UEncoder(pEncoder);
}


/* ========== ADPCM Encoder ========== */

HI_S32
OpenADPCMEncoder(HI_VOID *pEncoderAttr, HI_VOID **ppEncoder)
{
    AENC_ATTR_ADPCM_S *pAttr = (AENC_ATTR_ADPCM_S *)pEncoderAttr;
    ADPCM_CTX_S *ctx;
    HI_S32 ret;

    if (!pEncoderAttr || !ppEncoder) return -1;

    if (pAttr->enADPCMType > ADPCM_TYPE_ORG_DVI4) {
        printf("%s, %d, type:%d of ADPCM encoder is illegal!\n",
               __FUNCTION__, __LINE__, pAttr->enADPCMType);
        return HI_ERR_AENC_ILLEGAL_PARAM;
    }

    ctx = (ADPCM_CTX_S *)malloc(sizeof(ADPCM_CTX_S));
    if (!ctx) {
        printf("%s, %d, malloc ADPCM encoder context failed!\n",
               __FUNCTION__, __LINE__);
        return HI_ERR_AENC_NOMEM;
    }

    memset_s(ctx, sizeof(ADPCM_CTX_S), 0, sizeof(ADPCM_CTX_S));
    *ppEncoder = ctx;

    switch (pAttr->enADPCMType) {
    case ADPCM_TYPE_DVI4:     ctx->codec_type = 3;    break;
    case ADPCM_TYPE_IMA:      ctx->codec_type = 0x23; break;
    case ADPCM_TYPE_ORG_DVI4: ctx->codec_type = 0x43; break;
    default:
        printf("%s, %d, type:%d of ADPCM encoder is not support!\n",
               __FUNCTION__, __LINE__, pAttr->enADPCMType);
        free(ctx);
        *ppEncoder = HI_NULL;
        return HI_ERR_AENC_NOT_SUPPORT;
    }

    memcpy_s(&ctx->adpcm_type, sizeof(ctx->adpcm_type),
             pAttr, sizeof(HI_S32));

    ret = HI_VOICE_EncReset(ctx->voice_state, (HI_S16)ctx->codec_type);
    if (ret != 0) {
        free(ctx);
        *ppEncoder = HI_NULL;
    }
    return ret;
}

HI_S32
CloseADPCMEncoder(HI_VOID *pEncoder)
{
    ADPCM_CTX_S *ctx = (ADPCM_CTX_S *)pEncoder;
    if (!ctx) return -1;

    HI_VOICE_EncReset(ctx->voice_state, (HI_S16)ctx->codec_type);
    free(ctx);
    return HI_SUCCESS;
}

HI_S32
EncodeADPCMFrm(
    HI_VOID             *pEncoder,
    const AUDIO_FRAME_S *pstData,
    HI_U8               *pu8Outbuf,
    HI_U32              *pu32OutLen)
{
    ADPCM_CTX_S *ctx = (ADPCM_CTX_S *)pEncoder;
    HI_U32 ptNum;

    if (!pu32OutLen || !pu8Outbuf || !pstData || !pEncoder)
        return -1;

    ptNum = pstData->u32Len / (pstData->enBitwidth + 1);

    switch (ctx->adpcm_type) {
    case ADPCM_TYPE_DVI4:
    case ADPCM_TYPE_ORG_DVI4:
        if (ptNum != 80 && ptNum != 160 && ptNum != 240 &&
            ptNum != 320 && ptNum != 480) {
            printf("[Func]:%s [Line]:%d [Info]:the u32PtNumPerFrm%d is illegal for ADPCM_TYPE_DVI4\n",
                   __FUNCTION__, __LINE__, ptNum);
            return HI_ERR_AENC_ILLEGAL_PARAM;
        }
        break;
    case ADPCM_TYPE_IMA:
        if (ptNum != 81 && ptNum != 161 && ptNum != 241 &&
            ptNum != 321 && ptNum != 481) {
            printf("[Func]:%s [Line]:%d [Info]:the u32PtNumPerFrm%d is illegal for ADPCM_TYPE_IMA\n",
                   __FUNCTION__, __LINE__, ptNum);
            return HI_ERR_AENC_ILLEGAL_PARAM;
        }
        break;
    default:
        printf("[Func]:%s [Line]:%d [Info]:the u32PtNumPerFrm%d is illegal for ADPCM_TYPE_DVI4\n",
               __FUNCTION__, __LINE__, ptNum);
        return HI_ERR_AENC_ILLEGAL_PARAM;
    }

    HI_VOICE_EncodeFrame(ctx->voice_state, pstData->u64VirAddr[0],
                          pu8Outbuf, (HI_S16)(pstData->u32Len >> 1));

    *pu32OutLen = (*(HI_U16 *)(pu8Outbuf + 2) + 2) * 2;
    return HI_SUCCESS;
}


/* ========== G726 Encoder ========== */

HI_S32
OpenG726Encoder(HI_VOID *pEncoderAttr, HI_VOID **ppEncoder)
{
    AENC_ATTR_G726_S *pAttr = (AENC_ATTR_G726_S *)pEncoderAttr;
    G726_CTX_S *ctx;
    HI_S32 ret;

    if (!pEncoderAttr || !ppEncoder) return -1;

    if (pAttr->enG726bps > MEDIA_G726_40K) {
        printf("%s, %d, bps:%d of G726 encoder is illegal!\n",
               __FUNCTION__, __LINE__, pAttr->enG726bps);
        return HI_ERR_AENC_ILLEGAL_PARAM;
    }

    ctx = (G726_CTX_S *)malloc(sizeof(G726_CTX_S));
    if (!ctx) {
        printf("%s, %d, malloc G726 encoder context failed!\n",
               __FUNCTION__, __LINE__);
        return HI_ERR_AENC_NOMEM;
    }

    memset_s(ctx, sizeof(G726_CTX_S), 0, sizeof(G726_CTX_S));
    *ppEncoder = ctx;

    switch (pAttr->enG726bps) {
    case G726_16K:       ctx->codec_type = 4;    break;
    case G726_24K:       ctx->codec_type = 5;    break;
    case G726_32K:       ctx->codec_type = 6;    break;
    case G726_40K:       ctx->codec_type = 7;    break;
    case MEDIA_G726_16K: ctx->codec_type = 0x24; break;
    case MEDIA_G726_24K: ctx->codec_type = 0x25; break;
    case MEDIA_G726_32K: ctx->codec_type = 0x26; break;
    case MEDIA_G726_40K: ctx->codec_type = 0x27; break;
    default:
        printf("%s, %d, bps:%d of G726 encoder is not support!\n",
               __FUNCTION__, __LINE__, pAttr->enG726bps);
        free(ctx);
        *ppEncoder = HI_NULL;
        return HI_ERR_AENC_NOT_SUPPORT;
    }

    ret = HI_VOICE_EncReset(ctx->voice_state, ctx->codec_type);
    if (ret != 0) {
        free(ctx);
        *ppEncoder = HI_NULL;
    }
    return ret;
}

HI_S32
CloseG726Encoder(HI_VOID *pEncoder)
{
    G726_CTX_S *ctx = (G726_CTX_S *)pEncoder;
    if (!ctx) return -1;

    HI_VOICE_EncReset(ctx->voice_state, (HI_S16)ctx->codec_type);
    free(ctx);
    return HI_SUCCESS;
}

HI_S32
EncodeG726Frm(
    HI_VOID             *pEncoder,
    const AUDIO_FRAME_S *pstData,
    HI_U8               *pu8Outbuf,
    HI_U32              *pu32OutLen)
{
    G726_CTX_S *ctx = (G726_CTX_S *)pEncoder;
    HI_U32 ptNum;

    if (!pEncoder || !pstData || !pu8Outbuf || !pu32OutLen)
        return -1;

    ptNum = pstData->u32Len / (pstData->enBitwidth + 1);
    if (ptNum != 80 && ptNum != 160 && ptNum != 240 &&
        ptNum != 320 && ptNum != 480) {
        printf("%s, %d, points:%d of each frame is illegal!\n",
               __FUNCTION__, __LINE__, ptNum);
        return HI_ERR_AENC_ILLEGAL_PARAM;
    }

    HI_VOICE_EncodeFrame(ctx->voice_state, pstData->u64VirAddr[0],
                          pu8Outbuf, (HI_S16)(pstData->u32Len >> 1));

    *pu32OutLen = (*(HI_U16 *)(pu8Outbuf + 2) + 2) * 2;
    return HI_SUCCESS;
}


/* ========== G711A Decoder ========== */

HI_S32
OpenG711ADecoder(HI_VOID *pDecoderAttr, HI_VOID **ppDecoder)
{
    G711_CTX_S *ctx;
    HI_S32 ret;

    if (!ppDecoder) return -1;

    ctx = (G711_CTX_S *)malloc(sizeof(G711_CTX_S));
    if (!ctx) {
        printf("%s, %d, malloc G711A decoder context failed!\n",
               __FUNCTION__, __LINE__);
        return HI_ERR_ADEC_NOMEM;
    }

    memset_s(ctx, sizeof(G711_CTX_S), 0, sizeof(G711_CTX_S));
    *ppDecoder = ctx;
    ctx->codec_type = 1;

    ret = HI_VOICE_DecReset(ctx->voice_state, 1);
    if (ret != 0) {
        free(ctx);
        *ppDecoder = HI_NULL;
    }
    return ret;
}

HI_S32 CloseG711UDecoder(HI_VOID *pDecoder);
HI_S32 ResetG711UDecoder(HI_VOID *pDecoder);
HI_S32 DecodeG711UFrm(HI_VOID *pDecoder, HI_U8 **pu8Inbuf,
                       HI_S32 *ps32LeftByte, HI_U16 *pu16Outbuf,
                       HI_U32 *pu32OutLen, HI_U32 *pu32Chns);

HI_S32
CloseG711ADecoder(HI_VOID *pDecoder)
{
    return CloseG711UDecoder(pDecoder);
}

HI_S32
ResetG711ADecoder(HI_VOID *pDecoder)
{
    return ResetG711UDecoder(pDecoder);
}

HI_S32
DecodeG711AFrm(
    HI_VOID  *pDecoder,
    HI_U8   **pu8Inbuf,
    HI_S32   *ps32LeftByte,
    HI_U16   *pu16Outbuf,
    HI_U32   *pu32OutLen,
    HI_U32   *pu32Chns)
{
    return DecodeG711UFrm(pDecoder, pu8Inbuf, ps32LeftByte,
                           pu16Outbuf, pu32OutLen, pu32Chns);
}


/* ========== G711U Decoder ========== */

HI_S32
OpenG711UDecoder(HI_VOID *pDecoderAttr, HI_VOID **ppDecoder)
{
    G711_CTX_S *ctx;
    HI_S32 ret;

    if (!ppDecoder || !pDecoderAttr) return -1;

    ctx = (G711_CTX_S *)malloc(sizeof(G711_CTX_S));
    if (!ctx) {
        printf("%s, %d, malloc G711U decoder context failed!\n",
               __FUNCTION__, __LINE__);
        return HI_ERR_ADEC_NOMEM;
    }

    memset_s(ctx, sizeof(G711_CTX_S), 0, sizeof(G711_CTX_S));
    *ppDecoder = ctx;
    ctx->codec_type = 2;

    ret = HI_VOICE_DecReset(ctx->voice_state, 2);
    if (ret != 0) {
        free(ctx);
        *ppDecoder = HI_NULL;
    }
    return ret;
}

HI_S32
CloseG711UDecoder(HI_VOID *pDecoder)
{
    G711_CTX_S *ctx = (G711_CTX_S *)pDecoder;
    if (!ctx) return -1;

    HI_VOICE_DecReset(ctx->voice_state, (HI_S16)ctx->codec_type);
    free(ctx);
    return HI_SUCCESS;
}

HI_S32
ResetG711UDecoder(HI_VOID *pDecoder)
{
    G711_CTX_S *ctx = (G711_CTX_S *)pDecoder;
    if (!ctx) return -1;

    HI_VOICE_DecReset(ctx->voice_state, (HI_S16)ctx->codec_type);
    return HI_SUCCESS;
}

HI_S32
DecodeG711UFrm(
    HI_VOID  *pDecoder,
    HI_U8   **pu8Inbuf,
    HI_S32   *ps32LeftByte,
    HI_U16   *pu16Outbuf,
    HI_U32   *pu32OutLen,
    HI_U32   *pu32Chns)
{
    G711_CTX_S *ctx = (G711_CTX_S *)pDecoder;
    HI_S32 frameLen;
    HI_S16 outSamples;

    if (!pDecoder || !pu8Inbuf || !ps32LeftByte ||
        !pu16Outbuf || !pu32OutLen || !pu32Chns)
        return -1;

    *pu32Chns = 1;
    frameLen = ((*pu8Inbuf)[2] + 2) * 2;

    if (frameLen > *ps32LeftByte)
        return HI_ERR_ADEC_BUF_LACK;

    HI_VOICE_DecodeFrame(ctx->voice_state, *pu8Inbuf,
                          pu16Outbuf, &outSamples);

    *ps32LeftByte -= frameLen;
    *pu32OutLen = (HI_U32)((HI_S32)outSamples * 2);
    *pu8Inbuf += frameLen;
    return HI_SUCCESS;
}


/* ========== ADPCM Decoder ========== */

HI_S32
OpenADPCMDecoder(HI_VOID *pDecoderAttr, HI_VOID **ppDecoder)
{
    ADEC_ATTR_ADPCM_S *pAttr = (ADEC_ATTR_ADPCM_S *)pDecoderAttr;
    ADPCM_CTX_S *ctx;
    HI_S32 ret;

    if (!pDecoderAttr || !ppDecoder) return -1;

    if (pAttr->enADPCMType > ADPCM_TYPE_ORG_DVI4) {
        printf("%s, %d, type:%d of ADPCM decoder is illegal!\n",
               __FUNCTION__, __LINE__, pAttr->enADPCMType);
        return HI_ERR_ADEC_ILLEGAL_PARAM;
    }

    ctx = (ADPCM_CTX_S *)malloc(sizeof(ADPCM_CTX_S));
    if (!ctx) {
        printf("%s, %d, malloc ADPCM decoder context failed!\n",
               __FUNCTION__, __LINE__);
        return HI_ERR_ADEC_NOMEM;
    }

    memset_s(ctx, sizeof(ADPCM_CTX_S), 0, sizeof(ADPCM_CTX_S));
    *ppDecoder = ctx;

    switch (pAttr->enADPCMType) {
    case ADPCM_TYPE_DVI4:     ctx->codec_type = 3;    break;
    case ADPCM_TYPE_IMA:      ctx->codec_type = 0x23; break;
    case ADPCM_TYPE_ORG_DVI4: ctx->codec_type = 0x43; break;
    default:
        printf("%s, %d, type:%d of ADPCM decoder is not support!\n",
               __FUNCTION__, __LINE__, pAttr->enADPCMType);
        free(ctx);
        *ppDecoder = HI_NULL;
        return HI_ERR_ADEC_NOT_SUPPORT;
    }

    memcpy_s(&ctx->adpcm_type, sizeof(ctx->adpcm_type),
             pAttr, sizeof(HI_S32));

    ret = HI_VOICE_DecReset(ctx->voice_state, (HI_S16)ctx->codec_type);
    if (ret != 0) {
        free(ctx);
        *ppDecoder = HI_NULL;
    }
    return ret;
}

HI_S32
CloseADPCMDecoder(HI_VOID *pDecoder)
{
    ADPCM_CTX_S *ctx = (ADPCM_CTX_S *)pDecoder;
    if (!ctx) return -1;

    HI_VOICE_DecReset(ctx->voice_state, (HI_S16)ctx->codec_type);
    free(ctx);
    return HI_SUCCESS;
}

HI_S32
ResetADPCMDecoder(HI_VOID *pDecoder)
{
    ADPCM_CTX_S *ctx = (ADPCM_CTX_S *)pDecoder;
    if (!ctx) return -1;

    HI_VOICE_DecReset(ctx->voice_state, (HI_S16)ctx->codec_type);
    return HI_SUCCESS;
}

HI_S32
DecodeADPCMFrm(
    HI_VOID  *pDecoder,
    HI_U8   **pu8Inbuf,
    HI_S32   *ps32LeftByte,
    HI_U16   *pu16Outbuf,
    HI_U32   *pu32OutLen,
    HI_U32   *pu32Chns)
{
    ADPCM_CTX_S *ctx = (ADPCM_CTX_S *)pDecoder;
    HI_S32 frameLen;
    HI_S16 outSamples;

    if (!pDecoder || !pu8Inbuf || !ps32LeftByte ||
        !pu16Outbuf || !pu32OutLen || !pu32Chns)
        return -1;

    *pu32Chns = 1;
    frameLen = ((*pu8Inbuf)[2] + 2) * 2;

    if (frameLen > *ps32LeftByte)
        return HI_ERR_ADEC_BUF_LACK;

    HI_VOICE_DecodeFrame(ctx->voice_state, *pu8Inbuf,
                          pu16Outbuf, &outSamples);

    *ps32LeftByte -= frameLen;
    *pu32OutLen = (HI_U32)((HI_S32)outSamples * 2);
    *pu8Inbuf += frameLen;
    return HI_SUCCESS;
}


/* ========== G726 Decoder ========== */

HI_S32
OpenG726Decoder(HI_VOID *pDecoderAttr, HI_VOID **ppDecoder)
{
    ADEC_ATTR_G726_S *pAttr = (ADEC_ATTR_G726_S *)pDecoderAttr;
    G726_CTX_S *ctx;
    HI_S32 ret;

    if (!pDecoderAttr || !ppDecoder) return -1;

    if (pAttr->enG726bps > MEDIA_G726_40K) {
        printf("%s, %d, bps:%d of G726 decoder is illegal!\n",
               __FUNCTION__, __LINE__, pAttr->enG726bps);
        return HI_ERR_ADEC_ILLEGAL_PARAM;
    }

    ctx = (G726_CTX_S *)malloc(sizeof(G726_CTX_S));
    if (!ctx) {
        printf("%s, %d, malloc G726 decoder context failed!\n",
               __FUNCTION__, __LINE__);
        return HI_ERR_ADEC_NOMEM;
    }

    memset_s(ctx, sizeof(G726_CTX_S), 0, sizeof(G726_CTX_S));
    *ppDecoder = ctx;

    switch (pAttr->enG726bps) {
    case G726_16K:       ctx->codec_type = 4;    break;
    case G726_24K:       ctx->codec_type = 5;    break;
    case G726_32K:       ctx->codec_type = 6;    break;
    case G726_40K:       ctx->codec_type = 7;    break;
    case MEDIA_G726_16K: ctx->codec_type = 0x24; break;
    case MEDIA_G726_24K: ctx->codec_type = 0x25; break;
    case MEDIA_G726_32K: ctx->codec_type = 0x26; break;
    case MEDIA_G726_40K: ctx->codec_type = 0x27; break;
    default:
        printf("%s, %d, bps:%d of G726 decoder is not support!\n",
               __FUNCTION__, __LINE__, pAttr->enG726bps);
        free(ctx);
        *ppDecoder = HI_NULL;
        return HI_ERR_ADEC_NOT_SUPPORT;
    }

    ret = HI_VOICE_DecReset(ctx->voice_state, ctx->codec_type);
    if (ret != 0) {
        free(ctx);
        *ppDecoder = HI_NULL;
    }
    return ret;
}

HI_S32
CloseG726Decoder(HI_VOID *pDecoder)
{
    G726_CTX_S *ctx = (G726_CTX_S *)pDecoder;
    if (!ctx) return -1;

    HI_VOICE_DecReset(ctx->voice_state, (HI_S16)ctx->codec_type);
    free(ctx);
    return HI_SUCCESS;
}

HI_S32
ResetG726Decoder(HI_VOID *pDecoder)
{
    G726_CTX_S *ctx = (G726_CTX_S *)pDecoder;
    if (!ctx) return -1;

    HI_VOICE_DecReset(ctx->voice_state, (HI_S16)ctx->codec_type);
    return HI_SUCCESS;
}

HI_S32
DecodeG726Frm(
    HI_VOID  *pDecoder,
    HI_U8   **pu8Inbuf,
    HI_S32   *ps32LeftByte,
    HI_U16   *pu16Outbuf,
    HI_U32   *pu32OutLen,
    HI_U32   *pu32Chns)
{
    G726_CTX_S *ctx = (G726_CTX_S *)pDecoder;
    HI_S32 frameLen;
    HI_S16 outSamples;

    if (!pDecoder || !pu8Inbuf || !ps32LeftByte ||
        !pu16Outbuf || !pu32OutLen || !pu32Chns)
        return -1;

    *pu32Chns = 1;
    frameLen = ((*pu8Inbuf)[2] + 2) * 2;

    if (frameLen > *ps32LeftByte)
        return HI_ERR_ADEC_BUF_LACK;

    HI_VOICE_DecodeFrame(ctx->voice_state, *pu8Inbuf,
                          pu16Outbuf, &outSamples);

    *ps32LeftByte -= frameLen;
    *pu32OutLen = (HI_U32)((HI_S32)outSamples * 2);
    *pu8Inbuf += frameLen;
    return HI_SUCCESS;
}


/* ========== VoiceInit ========== */

HI_S32
HI_MPI_AENC_VoiceInit(HI_VOID)
{
    HI_S32 handle;
    AENC_ENCODER_S enc;
    HI_S32 ret;

    /* LPCM encoder */
    memset(&enc, 0, sizeof(enc));
    enc.enType = 23;
    enc.u32MaxFrmLen = 8192;
    snprintf_s(enc.aszName, sizeof(enc.aszName),
               sizeof(enc.aszName) - 1, "%s", "Lpcm");
    enc.pfnOpenEncoder = OpenLPCMEncoder;
    enc.pfnEncodeFrm = EncodeLPCMFrm;
    enc.pfnCloseEncoder = CloseLPCMEncoder;
    ret = HI_MPI_AENC_RegisterEncoder(&handle, &enc);
    if (ret != HI_SUCCESS) return ret;

    /* G711A encoder */
    memset(&enc, 0, sizeof(enc));
    enc.enType = 19;
    enc.u32MaxFrmLen = 960;
    snprintf_s(enc.aszName, sizeof(enc.aszName),
               sizeof(enc.aszName) - 1, "%s", "G711a");
    enc.pfnOpenEncoder = OpenG711AEncoder;
    enc.pfnEncodeFrm = EncodeG711AFrm;
    enc.pfnCloseEncoder = CloseG711AEncoder;
    ret = HI_MPI_AENC_RegisterEncoder(&handle, &enc);
    if (ret != HI_SUCCESS) return ret;

    /* G711U encoder */
    memset(&enc, 0, sizeof(enc));
    enc.enType = 20;
    enc.u32MaxFrmLen = 960;
    snprintf_s(enc.aszName, sizeof(enc.aszName),
               sizeof(enc.aszName) - 1, "%s", "G711u");
    enc.pfnOpenEncoder = OpenG711UEncoder;
    enc.pfnEncodeFrm = EncodeG711UFrm;
    enc.pfnCloseEncoder = CloseG711UEncoder;
    ret = HI_MPI_AENC_RegisterEncoder(&handle, &enc);
    if (ret != HI_SUCCESS) return ret;

    /* ADPCM encoder */
    memset(&enc, 0, sizeof(enc));
    enc.enType = 49;
    enc.u32MaxFrmLen = 480;
    snprintf_s(enc.aszName, sizeof(enc.aszName),
               sizeof(enc.aszName) - 1, "%s", "Adpcm");
    enc.pfnOpenEncoder = OpenADPCMEncoder;
    enc.pfnEncodeFrm = EncodeADPCMFrm;
    enc.pfnCloseEncoder = CloseADPCMEncoder;
    ret = HI_MPI_AENC_RegisterEncoder(&handle, &enc);
    if (ret != HI_SUCCESS) return ret;

    /* G726 encoder */
    memset(&enc, 0, sizeof(enc));
    enc.enType = 21;
    enc.u32MaxFrmLen = 960;
    snprintf_s(enc.aszName, sizeof(enc.aszName),
               sizeof(enc.aszName) - 1, "%s", "G726");
    enc.pfnOpenEncoder = OpenG726Encoder;
    enc.pfnEncodeFrm = EncodeG726Frm;
    enc.pfnCloseEncoder = CloseG726Encoder;
    ret = HI_MPI_AENC_RegisterEncoder(&handle, &enc);
    return ret;
}

HI_S32
HI_MPI_ADEC_VoiceInit(HI_VOID)
{
    HI_S32 handle;
    ADEC_DECODER_S dec;
    HI_S32 ret;

    /* LPCM decoder */
    memset(&dec, 0, sizeof(dec));
    dec.enType = 23;
    snprintf_s(dec.aszName, sizeof(dec.aszName),
               sizeof(dec.aszName) - 1, "%s", "Lpcm");
    dec.pfnOpenDecoder = OpenLPCMDecoder;
    dec.pfnDecodeFrm = DecodeLPCMFrm;
    dec.pfnGetFrmInfo = GetFrmInfo;
    dec.pfnCloseDecoder = CloseLPCMDecoder;
    dec.pfnResetDecoder = HI_NULL;
    ret = HI_MPI_ADEC_RegisterDecoder(&handle, &dec);
    if (ret != HI_SUCCESS) return ret;

    /* G711A decoder */
    memset(&dec, 0, sizeof(dec));
    dec.enType = 19;
    snprintf_s(dec.aszName, sizeof(dec.aszName),
               sizeof(dec.aszName) - 1, "%s", "G711a");
    dec.pfnOpenDecoder = OpenG711ADecoder;
    dec.pfnDecodeFrm = DecodeG711AFrm;
    dec.pfnGetFrmInfo = GetFrmInfo;
    dec.pfnCloseDecoder = CloseG711ADecoder;
    dec.pfnResetDecoder = ResetG711ADecoder;
    ret = HI_MPI_ADEC_RegisterDecoder(&handle, &dec);
    if (ret != HI_SUCCESS) return ret;

    /* G711U decoder */
    memset(&dec, 0, sizeof(dec));
    dec.enType = 20;
    snprintf_s(dec.aszName, sizeof(dec.aszName),
               sizeof(dec.aszName) - 1, "%s", "G711u");
    dec.pfnOpenDecoder = OpenG711UDecoder;
    dec.pfnDecodeFrm = DecodeG711UFrm;
    dec.pfnGetFrmInfo = GetFrmInfo;
    dec.pfnCloseDecoder = CloseG711UDecoder;
    dec.pfnResetDecoder = ResetG711UDecoder;
    ret = HI_MPI_ADEC_RegisterDecoder(&handle, &dec);
    if (ret != HI_SUCCESS) return ret;

    /* ADPCM decoder */
    memset(&dec, 0, sizeof(dec));
    dec.enType = 49;
    snprintf_s(dec.aszName, sizeof(dec.aszName),
               sizeof(dec.aszName) - 1, "%s", "Adpcm");
    dec.pfnOpenDecoder = OpenADPCMDecoder;
    dec.pfnDecodeFrm = DecodeADPCMFrm;
    dec.pfnGetFrmInfo = GetFrmInfo;
    dec.pfnCloseDecoder = CloseADPCMDecoder;
    dec.pfnResetDecoder = ResetADPCMDecoder;
    ret = HI_MPI_ADEC_RegisterDecoder(&handle, &dec);
    if (ret != HI_SUCCESS) return ret;

    /* G726 decoder */
    memset(&dec, 0, sizeof(dec));
    dec.enType = 21;
    snprintf_s(dec.aszName, sizeof(dec.aszName),
               sizeof(dec.aszName) - 1, "%s", "G726");
    dec.pfnOpenDecoder = OpenG726Decoder;
    dec.pfnDecodeFrm = DecodeG726Frm;
    dec.pfnGetFrmInfo = GetFrmInfo;
    dec.pfnCloseDecoder = CloseG726Decoder;
    dec.pfnResetDecoder = ResetG726Decoder;
    ret = HI_MPI_ADEC_RegisterDecoder(&handle, &dec);
    return ret;
}
