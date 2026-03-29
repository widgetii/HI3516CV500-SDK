/**
 * Reverse Engineered by TekuConcept on October 25, 2020
 */

#ifndef RE_DNVQE_WORK_H
#define RE_DNVQE_WORK_H

#include "re_dnvqe_comm.h"
#include "re_dnvqe_audio_module_wrap.h"
#include "dnvqe_errno.h"
#include "securec.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DNVQE_MAX_MODULE_SLOTS  10
#define DNVQE_PROC_BUF_SIZE     0x2000

typedef struct hiDNVQE_MODULE_SLOT {    /* sizeof=0x40 */
    HI_VOID *hInstance;                 /* +0x00: handle returned by module Init */
    HI_S32 s32IsEQ;                     /* +0x04: 1 if EQ module, 0 otherwise */
    HI_S32 s32HasSubParam;              /* +0x08: AGC sub-param flag */
    AUDIO_SAMPLE_RATE_E enRate;         /* +0x0C: working sample rate */
    HI_VOID *pConfig;                   /* +0x10: pointer to module config */
    HI_S32 field_14;                    /* +0x14: init to -1 */
    DNVQE_MODULE_HANDLE stModule;       /* +0x18: dlopen'd module handle */
} DNVQE_MODULE_SLOT;

typedef struct hiDNVQE_WORK_CTX {       /* sizeof=0x46CC */
    DNVQE_ATTR stAttr;                  /* +0x0000 (0x43C bytes) */
    HI_S32 s32FrameSample;              /* +0x043C */
    HI_S32 field_440;                   /* +0x0440 */
    HI_S32 field_444;                   /* +0x0444 */
    DNVQE_MODULE_SLOT astSlots[DNVQE_MAX_MODULE_SLOTS]; /* +0x0448 (0x280 bytes) */
    HI_U32 u32ModuleCount;              /* +0x06C8 */
    HI_S16 as16InBuf[DNVQE_PROC_BUF_SIZE / 2];  /* +0x06CC */
    HI_S16 as16OutBuf[DNVQE_PROC_BUF_SIZE / 2]; /* +0x26CC */
} DNVQE_WORK_CTX;

HI_VOID DNVQE_Destroy(DNVQE_WORK_CTX *pCtx);
HI_S32  DNVQE_Create(DNVQE_CTX *pOutCtx, HI_U32 *pFieldC,
    HI_S32 *ps32CacheSize, DNVQE_ATTR *pAttr);
HI_S32  DNVQE_ProcessFrame(DNVQE_WORK_CTX *pCtx,
    HI_S16 *ps16SinBuf, HI_S16 *ps16SouBuf);

#endif
