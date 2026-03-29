/**
 * Reverse Engineered by TekuConcept on October 23, 2020
 */

#include "hi_types.h"
#include "hi_comm_aio.h"
#include "re_dnvqe_comm.h"
#include "re_dnvqe_audio_module_wrap.h"
#include "re_dnvqe_resampler_work.h"
#include "dnvqe_errno.h"
#include "securec.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

HI_S32
RES_ReSampler_Create(
    DNVQE_RESAMPLER_S **pstReSampler,
    AUDIO_SAMPLE_RATE_E enInRate,
    AUDIO_SAMPLE_RATE_E enOutRate,
    DNVQE_RESAMPLER_TYPE_E enReSamplerType)
{
    HI_S32 result;
    DNVQE_RESAMPLER_S *resampler;
    DNVQE_RESAMP_ATTR attr;
    size_t bufSize;

    if (enInRate == enOutRate || enInRate > 64000 || enOutRate > 48000) {
        *pstReSampler = HI_NULL;
        return HI_SUCCESS;
    }

    resampler = (DNVQE_RESAMPLER_S *)malloc(sizeof(DNVQE_RESAMPLER_S));
    if ( !resampler ) {
        fputs("Malloc ReSampler Fail!\n", stderr);
        return ERR_RESAMPLER_NOMEM;
    }
    memset_s(resampler, sizeof(DNVQE_RESAMPLER_S), 0, sizeof(DNVQE_RESAMPLER_S));

    /* load libhive_RES.so */
    result = MODULE_HI_Audio_ModuleHandleInit(&resampler->hResampler, "hive", "RES");
    if ( result ) {
        fputs("Resample Module Init Fail!\n", stderr);
        free(resampler);
        return result;
    }

    resampler->field_0 = enInRate / enOutRate;
    if ( enInRate != enOutRate * resampler->field_0 )
        resampler->field_0 = 0;

    attr.enOutSampleRate = enOutRate;
    attr.field_4 = 0;
    attr.field_8 = 1;

    switch (enReSamplerType) {
    case DNVQE_RESAMPLER_TYPE_READ_CACHE: {
        resampler->hResampler.Resampler_Init(&resampler->hBaseReadCache, enInRate, &attr);
        if ( resampler->hBaseReadCache == HI_NULL ) {
            fprintf(stderr, "%d: Resampler_Init Fail.\n", __LINE__);
            result = ERR_DNVQE_NULL_PTR;
            goto error;
        }

        if ( resampler->field_0 == 0 ) goto done;

        bufSize = 2 * (resampler->field_0 + 1);
        resampler->pS16ReadCacheBuf = (HI_S16 *)malloc(bufSize);
        if ( resampler->pS16ReadCacheBuf == HI_NULL ) {
            fputs("Malloc pS16ReadCacheBuf Fail!\n", stderr);
            resampler->hResampler.Resampler_DeInit(resampler->hBaseReadCache);
            result = ERR_RESAMPLER_NOMEM;
            goto error;
        }
    } break;

    case DNVQE_RESAMPLER_TYPE_RESAMPLER: {
        resampler->hResampler.Resampler_Init(&resampler->hBaseResampler, enInRate, &attr);
        if ( resampler->hBaseResampler == HI_NULL ) {
            result = ERR_DNVQE_NULL_PTR;
            goto error;
        }

        bufSize = 2 * (enOutRate / 100 + 1);
        resampler->ReSamplerBuf = (HI_S16 *)malloc(bufSize);
        if ( resampler->ReSamplerBuf == HI_NULL ) {
            fputs("Malloc DQVE ReSamplerBuf Fail!\n", stderr);
            resampler->hResampler.Resampler_DeInit(resampler->hBaseResampler);
            result = ERR_RESAMPLER_NOMEM;
            goto error;
        }
    } break;

    default: {
        fprintf(stderr, "Resample can't use param %d!\n", enReSamplerType);
            result = ERR_RESAMPLER_ILLEGAL_PARAM;
            goto error;
    } break;
    } /* switch */

    memset_s(resampler->ReSamplerBuf, bufSize, 0, bufSize);

  done:
    resampler->enInRate  = enInRate;
    resampler->enOutRate = enOutRate;
    resampler->field_14  = 0;
    *pstReSampler        = resampler;
    return HI_SUCCESS;

  error:
    MODULE_HI_Audio_ModuleHandleDeInit(&resampler->hResampler);
    free(resampler);
    return result;
}


HI_S32
RES_ReSampler_Destory(DNVQE_RESAMPLER_S *pstReSampler)
{
    if ( pstReSampler == HI_NULL ) return HI_FAILURE;

    if ( pstReSampler->hBaseReadCache ) {
        pstReSampler->hResampler.Resampler_DeInit(pstReSampler->hBaseReadCache);
        if ( pstReSampler->pS16ReadCacheBuf )
            free(pstReSampler->pS16ReadCacheBuf);
        pstReSampler->pS16ReadCacheBuf = HI_NULL;
    }

    if ( pstReSampler->hBaseResampler ) {
        pstReSampler->hResampler.Resampler_DeInit(pstReSampler->hBaseResampler);
        if ( pstReSampler->ReSamplerBuf )
            free(pstReSampler->ReSamplerBuf);
        pstReSampler->ReSamplerBuf = HI_NULL;
    }

    MODULE_HI_Audio_ModuleHandleDeInit(&pstReSampler->hResampler);
    free(pstReSampler);

    return HI_SUCCESS;
}


HI_S32
RES_ReSampler_GetInputNum(
    DNVQE_RESAMPLER_S *pstReSampler,
    HI_S32 s32OutSamps,
    DNVQE_RESAMPLER_TYPE_E enReSamplerType)
{
    HI_S32 result;

    if ( pstReSampler == HI_NULL ) return HI_FAILURE;

    if ( enReSamplerType != DNVQE_RESAMPLER_TYPE_RESAMPLER ) return 0;

    result = pstReSampler->enInRate * (s32OutSamps - pstReSampler->field_14) / pstReSampler->enOutRate;
    if ( pstReSampler->field_0 == 0 ) result++;

    return result;
}


HI_S32
RES_ReSampler_ProcessFrame(
    DNVQE_RESAMPLER_S *pstReSampler,
    HI_S16 *pS16OutBuf,
    HI_S16 *pS16InBuf,
    HI_S32 s32InSamps,
    HI_S32 *pS32OutSamps,
    DNVQE_RESAMPLER_TYPE_E enReSamplerType)
{
    HI_S32 s32Ret, s32Ret2;
    HI_S32 s32Ratio, s32Remainder, s32Field14;
    HI_S32 s32ReadSample, s32Excess;
    HI_S32 s32InputBytes;
    HI_S16 *pInPtr;
    HI_VOID *hBase;
    HI_CHAR inputAttr[20];
    HI_CHAR outputAttr[20];

    if ( pS16OutBuf == HI_NULL || pS16InBuf == HI_NULL || pstReSampler == HI_NULL )
        return HI_FAILURE;

    memset_s(inputAttr, 20, 0, 20);
    memset_s(outputAttr, 20, 0, 20);

    if ( enReSamplerType == DNVQE_RESAMPLER_TYPE_READ_CACHE ) {
        /* --- READ_CACHE path --- */
        s32Ratio = pstReSampler->field_0;
        if ( s32Ratio == 0 )
            goto read_cache_call;

        s32Remainder = s32InSamps % s32Ratio;
        s32Field14 = pstReSampler->field_14;

        if ( s32Remainder == 0 && s32Field14 <= 0 )
            goto read_cache_call;

        /* Copy leftover samples from previous call into work buffer */
        pInPtr = (HI_S16 *)&pstReSampler->field_1C;
        if ( s32Field14 > 0 ) {
            memcpy_s(pInPtr, s32Field14 * 2,
                     pstReSampler->pS16ReadCacheBuf, s32Field14 * 2);
        }

        s32Remainder = (s32InSamps + s32Field14) % s32Ratio;

        memcpy_s(pInPtr + s32Field14, (s32InSamps - s32Remainder) * 2,
                 pS16InBuf, (s32InSamps - s32Remainder) * 2);

        if ( s32Remainder != 0 ) {
            memcpy_s(pstReSampler->pS16ReadCacheBuf, s32Remainder * 2,
                     pS16InBuf + (s32InSamps - s32Remainder),
                     s32Remainder * 2);
        }

        s32InSamps = s32InSamps + s32Field14 - s32Remainder;
        pstReSampler->field_14 = s32Remainder;
        pS16InBuf = pInPtr;

    read_cache_call:
        *(HI_S16 **)&inputAttr[0]  = pS16InBuf;
        *(HI_S32 *)&inputAttr[16]  = s32InSamps * 2;
        *(HI_S16 **)&outputAttr[0] = pS16OutBuf;

        s32Ret = pstReSampler->hResampler.Resampler_Process(
            pstReSampler->hBaseReadCache, inputAttr, outputAttr);
        *pS32OutSamps = s32Ret;
        return HI_SUCCESS;

    } else if ( enReSamplerType == DNVQE_RESAMPLER_TYPE_RESAMPLER ) {
        /* --- RESAMPLER path (type 1) --- */
        s32ReadSample = *pS32OutSamps;
        hBase = pstReSampler->hBaseResampler;

        if ( pstReSampler->field_0 == 0 && s32InSamps != 0 )
            s32InSamps--;

        s32InputBytes = s32InSamps * 2;
        *(HI_S16 **)&inputAttr[0]  = pS16InBuf;
        *(HI_S32 *)&inputAttr[16]  = s32InputBytes;
        *(HI_S16 **)&outputAttr[0] = pS16OutBuf;

        /* Prepend leftover from previous call */
        s32Field14 = pstReSampler->field_14;
        if ( s32Field14 > 0 ) {
            memcpy_s(pS16OutBuf, s32Field14 * 2,
                     pstReSampler->ReSamplerBuf, s32Field14 * 2);
            s32ReadSample -= s32Field14;
            *(HI_S16 **)&outputAttr[0] = pS16OutBuf + s32Field14;
            pS16OutBuf += s32Field14;
            pstReSampler->field_14 = 0;
        }

        s32Ret = pstReSampler->hResampler.Resampler_Process(hBase, inputAttr, outputAttr);

        if ( s32ReadSample < s32Ret ) {
            fprintf(stderr,
                "%d: Err: ReSampler_ProcessFrame Err, s32Ret is %d, s32ReadSample is %d\n",
                __LINE__, s32Ret, s32ReadSample);
            return ERR_RESAMPLER_NULL_PTR;
        }

        if ( pstReSampler->field_0 != 0 ) {
            if ( s32ReadSample > s32Ret ) {
                fprintf(stderr,
                    "%d: Err: ReSampler_ProcessFrame Err, s32Ret is %d, s32ReadSample is %d\n",
                    __LINE__, s32Ret, s32ReadSample);
                return ERR_RESAMPLER_NULL_PTR;
            }
            return HI_SUCCESS;
        }

        /* field_0 == 0: process one extra sample for fractional resampling */
        *(HI_S16 **)&inputAttr[0]  = pS16InBuf + s32InSamps;
        *(HI_S32 *)&inputAttr[16]  = 2;
        *(HI_S16 **)&outputAttr[0] = pstReSampler->ReSamplerBuf;

        s32Ret2 = pstReSampler->hResampler.Resampler_Process(hBase, inputAttr, outputAttr);

        if ( s32ReadSample <= s32Ret ) {
            pstReSampler->field_14 = s32Ret2;
            return HI_SUCCESS;
        }

        s32Excess = s32ReadSample - s32Ret;
        pstReSampler->field_14 = s32Ret2 - s32Excess;

        if ( pstReSampler->field_14 < 0 ) {
            fprintf(stderr,
                "%d: Err: ReSampler_ProcessFrame Err, s32Ret is %d, s32ReadSample is %d\n",
                __LINE__, s32Ret2, s32ReadSample);
            return ERR_RESAMPLER_NULL_PTR;
        }

        memcpy_s(pS16OutBuf + s32Ret, s32Excess * 2,
                 pstReSampler->ReSamplerBuf, s32Excess * 2);

        memmove_s(pstReSampler->ReSamplerBuf, pstReSampler->field_14 * 2,
                  pstReSampler->ReSamplerBuf + s32Excess,
                  pstReSampler->field_14 * 2);
        return HI_SUCCESS;

    } else if ( enReSamplerType == DNVQE_RESAMPLER_TYPE_BUTT ) {
        /* --- Partial-read resampler path (type 2) --- */
        s32ReadSample = *pS32OutSamps;
        hBase = pstReSampler->hBaseResampler;

        if ( pstReSampler->field_0 == 0 && s32InSamps != 0 )
            s32InSamps--;

        s32InputBytes = s32InSamps * 2;
        *(HI_S16 **)&inputAttr[0]  = pS16InBuf;
        *(HI_S32 *)&inputAttr[16]  = s32InputBytes;
        *(HI_S16 **)&outputAttr[0] = pS16OutBuf;

        s32Field14 = pstReSampler->field_14;
        if ( s32Field14 > 0 ) {
            memcpy_s(pS16OutBuf, s32Field14 * 2,
                     pstReSampler->ReSamplerBuf, s32Field14 * 2);
            s32ReadSample -= s32Field14;
            *(HI_S16 **)&outputAttr[0] = pS16OutBuf + s32Field14;
            pS16OutBuf += s32Field14;
            pstReSampler->field_14 = 0;
        }

        s32Ret = pstReSampler->hResampler.Resampler_Process(hBase, inputAttr, outputAttr);

        if ( s32ReadSample < s32Ret ) {
            fprintf(stderr,
                "%d: Err: ReSampler_ProcessFrame Err, s32Ret is %d, s32ReadSample is %d\n",
                __LINE__, s32Ret, s32ReadSample);
            return ERR_RESAMPLER_NULL_PTR;
        }

        if ( pstReSampler->field_0 != 0 ) {
            if ( s32ReadSample > s32Ret ) {
                *pS32OutSamps -= (s32ReadSample - s32Ret);
            }
            return HI_SUCCESS;
        }

        /* field_0 == 0: process one extra sample for fractional resampling */
        *(HI_S16 **)&inputAttr[0]  = pS16InBuf + s32InSamps;
        *(HI_S32 *)&inputAttr[16]  = 2;
        *(HI_S16 **)&outputAttr[0] = pstReSampler->ReSamplerBuf;

        s32Ret2 = pstReSampler->hResampler.Resampler_Process(hBase, inputAttr, outputAttr);
        pstReSampler->field_14 = s32Ret2;

        if ( s32ReadSample <= s32Ret )
            return HI_SUCCESS;

        s32Excess = s32ReadSample - s32Ret;
        if ( s32Ret2 >= s32Excess ) {
            pstReSampler->field_14 = s32Ret2 - s32Excess;

            memcpy_s(pS16OutBuf + s32Ret, s32Excess * 2,
                     pstReSampler->ReSamplerBuf, s32Excess * 2);

            memmove_s(pstReSampler->ReSamplerBuf, pstReSampler->field_14 * 2,
                      pstReSampler->ReSamplerBuf + s32Excess,
                      pstReSampler->field_14 * 2);
            return HI_SUCCESS;
        } else {
            /* Not enough samples in resample buf */
            memcpy_s(pS16OutBuf + s32Ret, s32Ret2 * 2,
                     pstReSampler->ReSamplerBuf, s32Ret2 * 2);
            *pS32OutSamps = s32Ret2 + s32Ret;
            pstReSampler->field_14 = 0;
            return HI_SUCCESS;
        }

    } else {
        return ERR_RESAMPLER_ILLEGAL_PARAM;
    }
}
