/**
 * Reverse Engineered by TekuConcept on October 25, 2020
 */

#include "re_dnvqe_work.h"


HI_VOID
DNVQE_Destroy(DNVQE_WORK_CTX *pCtx)
{
    HI_U32 i;

    if (pCtx == HI_NULL)
        return;

    for (i = 0; i < pCtx->u32ModuleCount; i++) {
        DNVQE_MODULE_SLOT *pSlot = &pCtx->astSlots[i];
        pSlot->stModule.Resampler_DeInit(pSlot->hInstance);
        MODULE_HI_Audio_ModuleHandleDeInit(&pSlot->stModule);
    }

    free(pCtx);
}


HI_S32
DNVQE_Create(
    DNVQE_CTX *pOutCtx,
    HI_U32 *pFieldC,
    HI_S32 *ps32CacheSize,
    DNVQE_ATTR *pAttr)
{
    DNVQE_WORK_CTX *pCtx;
    HI_S32 s32FrameSample;
    HI_U32 u32ModuleCount;
    HI_U32 u32TotalEnable;
    HI_U32 i;
    HI_S32 s32Ret;

    s32FrameSample = pAttr->field_20;

    /* Validate s32FrameSample: must be in range [80..4096] */
    if ((HI_U32)(s32FrameSample - 80) > 4016) {
        fprintf(stderr, "DNVQE invalid s32FrameSample(%d)\n", s32FrameSample);
        return ERR_DNVQE_ILLEGAL_PARAM;
    }

    /* Check if any module is enabled */
    u32TotalEnable = pAttr->u32HpfEnable + pAttr->u32AnrEnable +
                     pAttr->u32AgcEnable + pAttr->u32EqEnable;

    if (u32TotalEnable > 0) {
        AUDIO_SAMPLE_RATE_E enRate = pAttr->enSamplerate;

        if (enRate == AUDIO_SAMPLE_RATE_8000) {
            s32FrameSample = 80;
        } else if (enRate == AUDIO_SAMPLE_RATE_16000) {
            s32FrameSample = 160;
        } else if (enRate == AUDIO_SAMPLE_RATE_48000) {
            s32FrameSample = 480;
        } else {
            fprintf(stderr, "DNVQE invalid s32WorkSampleRate(%d)\n", enRate);
            return ERR_DNVQE_ILLEGAL_PARAM;
        }
    }

    /* Allocate work context */
    pCtx = (DNVQE_WORK_CTX *)malloc(sizeof(DNVQE_WORK_CTX));
    if (pCtx == HI_NULL)
        return ERR_DNVQE_NOMEM;

    memset_s(pCtx, sizeof(DNVQE_WORK_CTX), 0, sizeof(DNVQE_WORK_CTX));

    pCtx->field_440 = -1;
    pCtx->field_444 = -1;

    u32ModuleCount = 0;

    /* HPF module */
    if (pAttr->u32HpfEnable) {
        DNVQE_MODULE_SLOT *pSlot = &pCtx->astSlots[u32ModuleCount];
        s32Ret = MODULE_HI_Audio_ModuleHandleInit(&pSlot->stModule, "hive", "HPF");
        if (s32Ret != HI_SUCCESS) {
            pCtx->u32ModuleCount = 0;
            goto fail;
        }
        pSlot->field_14    = -1;
        pSlot->enRate      = pAttr->field_24;
        pSlot->pConfig     = &pAttr->stHpfCfg;
        u32ModuleCount = 1;
    }

    /* ANR module */
    if (pAttr->u32AnrEnable) {
        DNVQE_MODULE_SLOT *pSlot = &pCtx->astSlots[u32ModuleCount];
        s32Ret = MODULE_HI_Audio_ModuleHandleInit(&pSlot->stModule, "hive", "ANR");
        if (s32Ret != HI_SUCCESS) {
            pCtx->u32ModuleCount = u32ModuleCount;
            goto fail;
        }
        pSlot->field_14    = -1;
        pSlot->enRate      = pAttr->field_24;
        pSlot->pConfig     = &pAttr->stAnrCfg;
        u32ModuleCount++;
    }

    /* AGC module */
    if (pAttr->u32AgcEnable) {
        DNVQE_MODULE_SLOT *pSlot = &pCtx->astSlots[u32ModuleCount];
        s32Ret = MODULE_HI_Audio_ModuleHandleInit(&pSlot->stModule, "hive", "AGC");
        if (s32Ret != HI_SUCCESS) {
            pCtx->u32ModuleCount = u32ModuleCount;
            goto fail;
        }
        pSlot->s32IsEQ       = 0;
        pSlot->s32HasSubParam = (pAttr->u32EqEnable != 0) ? 1 : 0;
        pSlot->field_14       = -1;
        pSlot->enRate         = pAttr->field_24;
        pSlot->pConfig        = &pAttr->stAgcCfg;
        u32ModuleCount++;
    }

    /* EQ module */
    if (pAttr->u32EqEnable) {
        DNVQE_MODULE_SLOT *pSlot = &pCtx->astSlots[u32ModuleCount];
        s32Ret = MODULE_HI_Audio_ModuleHandleInit(&pSlot->stModule, "hive", "EQ");
        if (s32Ret != HI_SUCCESS) {
            pCtx->u32ModuleCount = u32ModuleCount;
            goto fail;
        }
        pSlot->s32IsEQ       = 1;
        pSlot->s32HasSubParam = 0;
        pSlot->field_14       = -1;
        pSlot->enRate         = pAttr->field_24;
        pSlot->pConfig        = &pAttr->stEqCfg;
        u32ModuleCount++;
    }

    /* MBC module */
    if (pAttr->u32MbcEnable) {
        DNVQE_MODULE_SLOT *pSlot = &pCtx->astSlots[u32ModuleCount];
        s32Ret = MODULE_HI_Audio_ModuleHandleInit(&pSlot->stModule, "hive", "MBC");
        if (s32Ret != HI_SUCCESS) {
            pCtx->u32ModuleCount = u32ModuleCount;
            goto fail;
        }
        pSlot->field_14 = -1;
        pSlot->pConfig  = &pAttr->field_64;
        u32ModuleCount++;
    }

    pCtx->u32ModuleCount = u32ModuleCount;

    /* Initialize each loaded module */
    for (i = 0; i < u32ModuleCount; i++) {
        DNVQE_MODULE_SLOT *pSlot = &pCtx->astSlots[i];
        pSlot->stModule.Resampler_Init(
            &pSlot->hInstance,
            pAttr->enSamplerate,
            &pSlot->s32IsEQ);
        if (pSlot->hInstance == HI_NULL) {
            fprintf(stderr, "%s_Init Fail!\n", pSlot->stModule.pSymData);
            goto fail;
        }
    }

    /* Copy the full attribute to the work context */
    memcpy_s(&pCtx->stAttr, sizeof(DNVQE_ATTR), pAttr, sizeof(DNVQE_ATTR));

    /* Store output values */
    pOutCtx->pWorkCtx = pCtx;
    pCtx->s32FrameSample = s32FrameSample;
    *ps32CacheSize = s32FrameSample;

    return HI_SUCCESS;

fail:
    DNVQE_Destroy(pCtx);
    return ERR_DNVQE_MODULE_INIT;
}


HI_S32
DNVQE_ProcessFrame(
    DNVQE_WORK_CTX *pCtx,
    HI_S16 *ps16SinBuf,
    HI_S16 *ps16SouBuf)
{
    HI_U32 u32ModuleCount;
    HI_S32 s32BytesPerFrame;
    HI_S16 *pSin, *pSou, *pTmp;
    HI_U32 i;
    HI_S32 s32Ret;

    if (pCtx == HI_NULL) {
        fputs("DNVQE invalid hDnVqe\n", stderr);
        return ERR_DNVQE_NULL_PTR;
    }

    u32ModuleCount = pCtx->u32ModuleCount;
    s32BytesPerFrame = pCtx->s32FrameSample * 2;

    /* No modules loaded: just copy input to output */
    if (u32ModuleCount == 0) {
        memcpy_s(ps16SouBuf, s32BytesPerFrame,
            ps16SinBuf, s32BytesPerFrame);
        return HI_SUCCESS;
    }

    pSin = ps16SinBuf;
    pSou = pCtx->as16InBuf;

    for (i = 0; i < u32ModuleCount; i++) {
        DNVQE_MODULE_SLOT *pSlot = &pCtx->astSlots[i];

        /* Last module writes directly to output buffer */
        if (i == u32ModuleCount - 1)
            pSou = ps16SouBuf;

        s32Ret = pSlot->stModule.Resampler_Process(
            pSlot->hInstance, pSin, pSou);

        if (s32Ret != HI_SUCCESS)
            return ERR_DNVQE_PROCESS_FAIL;

        /* After first module, redirect sin to second scratch buffer */
        if (i == 0)
            pSin = pCtx->as16OutBuf;

        /* Swap sin/sou for next module in chain */
        pTmp = pSin;
        pSin = pSou;
        pSou = pTmp;
    }

    return HI_SUCCESS;
}
