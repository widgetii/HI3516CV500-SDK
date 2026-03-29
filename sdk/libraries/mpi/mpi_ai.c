/**
 * Reverse Engineered by TekuConcept on April 26, 2021
 */

#include "re_mpi_ai.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <sys/prctl.h>

#ifndef RE_DBG_LVL
#define RE_DBG_LVL HI_DBG_ERR
#endif

#define AI_DEV_NAME "/dev/ai"
#define MAX_CHN_COUNT 2

// ============================================================================

pthread_mutex_t s_ai_fdmutex;
AI_DEV_INFO_S s_mpi_ai_dev;

AI_CHN_CTX_S s_mpi_ai_chn_ctx[MAX_CHN_COUNT];
AST_VQE_STATE_S g_ast_vqe_state[MAX_CHN_COUNT];
HI_S32 g_ai_fd[MAX_CHN_COUNT];

HI_BOOL s_ai_init = HI_FALSE;

// ============================================================================

extern HI_S32 HI_UPVQE_GetVolume(HI_VOID* pHandle, HI_S32 *ps32VolumeDb);
extern HI_S32 HI_UPVQE_SetVolume(HI_VOID* pHandle, HI_S32 s32VolumeDb);
extern HI_S32 HI_UPVQE_Create(HI_VOID **ppHandle, HI_VOID *pstConfig);
extern HI_S32 HI_UPVQE_Destroy(HI_VOID **ppHandle);
extern HI_S32 HI_UPVQE_GetConfig(HI_VOID *pHandle, HI_VOID *pstConfig);
extern HI_S32 HI_UPVQE_WriteFrame(HI_VOID *pHandle, HI_VOID *pstFrame);
extern HI_S32 HI_UPVQE_ReadFrame(HI_VOID *pHandle, HI_VOID *pstFrame, HI_S32 s32Flag);

// -- file: mpi_vb.c --
extern HI_S32 HI_MPI_VB_MmapPool(VB_POOL Pool);
extern HI_S32 HI_MPI_VB_MunmapPool(VB_POOL Pool);
extern HI_S32 HI_MPI_VB_GetBlockVirAddr(VB_POOL Pool, HI_U64 u64PhyAddr, HI_VOID **ppVirAddr);

// ============================================================================

HI_S32
ai_compare_agc_attr(
    const AUDIO_AGC_CONFIG_S *pstAgcCfg1,
    const AUDIO_AGC_CONFIG_S *pstAgcCfg2)
{
    if (pstAgcCfg1->bUsrMode != pstAgcCfg2->bUsrMode)
        return 0;

    if (pstAgcCfg1->bUsrMode != 1)
        return 1;

    if (pstAgcCfg1->s16NoiseSupSwitch != pstAgcCfg2->s16NoiseSupSwitch)
        return 0;
    if (pstAgcCfg1->s8MaxGain != pstAgcCfg2->s8MaxGain)
        return 0;
    if (pstAgcCfg1->s8NoiseFloor != pstAgcCfg2->s8NoiseFloor)
        return 0;
    if (pstAgcCfg1->s8OutputMode != pstAgcCfg2->s8OutputMode)
        return 0;
    if (pstAgcCfg1->s8TargetLevel != pstAgcCfg2->s8TargetLevel)
        return 0;
    if (pstAgcCfg1->s8ImproveSNR != pstAgcCfg2->s8ImproveSNR)
        return 0;

    return 1;
}

HI_S32
ai_check_open(AI_CHN AiChn)
{
    pthread_mutex_lock(&s_ai_fdmutex);

    if ( g_ai_fd[AiChn] < 0 ) {
        g_ai_fd[AiChn] = open(AI_DEV_NAME, O_RDWR, 0);
        if ( g_ai_fd[AiChn] < 0 ) {
            g_ai_fd[AiChn] = -1;
            pthread_mutex_unlock(&s_ai_fdmutex);
            HI_TRACE_AI(RE_DBG_LVL, "open /dev/ai failed!\n");
            return HI_ERR_AI_SYS_NOTREADY;
        }
    }

    if ( ioctl(g_ai_fd[AiChn], IOC_AI_INIT_CHN, &AiChn) != HI_SUCCESS ) {
        close(g_ai_fd[AiChn]);
        g_ai_fd[AiChn] = -1;
        pthread_mutex_unlock(&s_ai_fdmutex);
        return HI_ERR_AI_SYS_NOTREADY;
    }

    pthread_mutex_unlock(&s_ai_fdmutex);
    return HI_SUCCESS;
}

static HI_S32
mpi_ai_set_acodec_gain(HI_S32 s32Gain)
{
    HI_S32 result;
    result = ai_check_open(0);
    if ( result != HI_SUCCESS ) return result;
    return ioctl(g_ai_fd[0], 0x40045A1E, &s32Gain);
}

static HI_S32
hi_mpi_ai_query_file_status(AUDIO_DEV AiDevId, AI_CHN AiChn, AUDIO_FILE_STATUS_S *pstFileStatus)
{
    HI_S32 result;
    AUDIO_FILE_STATUS_S stStatus;

    if ( AiDevId != 0 ) {
        HI_TRACE_AI(RE_DBG_LVL, "ai dev %d is invalid\n", AiDevId);
        return HI_ERR_AI_INVALID_DEVID;
    }
    if ( AiChn >= MAX_CHN_COUNT ) {
        HI_TRACE_AI(RE_DBG_LVL, "ai chnid %d is invalid\n", AiChn);
        return HI_ERR_AI_INVALID_CHNID;
    }
    if ( pstFileStatus == HI_NULL )
        return HI_ERR_AI_NULL_PTR;

    result = ai_check_open(AiChn);
    if ( result != HI_SUCCESS ) return result;

    pthread_mutex_lock(&s_mpi_ai_chn_ctx[AiChn].mutex);
    memset_s(&stStatus, sizeof(stStatus), 0, sizeof(stStatus));
    result = ioctl(g_ai_fd[AiChn], 0x82085A1B, &stStatus);
    if ( result == HI_SUCCESS )
        memcpy_s(pstFileStatus, sizeof(AUDIO_FILE_STATUS_S), &stStatus, sizeof(AUDIO_FILE_STATUS_S));
    pthread_mutex_unlock(&s_mpi_ai_chn_ctx[AiChn].mutex);
    return result;
}

static HI_S32
hi_mpi_ai_get_record_vqe_attr(AUDIO_DEV AiDevId, AI_CHN AiChn, AI_RECORDVQE_CONFIG_S *pstVqeConfig)
{
    HI_S32 result;
    HI_U8 vqeConfig[316];
    AI_CHN_CTX_S *pCtx;

    if (AiDevId != 0) {
        HI_TRACE_AI(RE_DBG_LVL, "ai dev %d is invalid\n", AiDevId);
        return HI_ERR_AI_INVALID_DEVID;
    }
    if (AiChn >= MAX_CHN_COUNT) {
        HI_TRACE_AI(RE_DBG_LVL, "ai chnid %d is invalid\n", AiChn);
        return HI_ERR_AI_INVALID_CHNID;
    }
    if (pstVqeConfig == HI_NULL)
        return HI_ERR_AI_NULL_PTR;

    result = ai_check_open(AiChn);
    if (result != HI_SUCCESS) return result;

    pCtx = &s_mpi_ai_chn_ctx[AiChn];
    pthread_mutex_lock(&pCtx->mutex);

    /* Check if record VQE is configured (field_40==1 && field_7C==4) or resample is enabled */
    if (pCtx->field_40 == 1) {
        if (pCtx->field_7C != 4) {
            pthread_mutex_unlock(&pCtx->mutex);
            HI_TRACE_AI(RE_DBG_LVL, "AI chn %d has not set record vqe attr\n", AiChn);
            return HI_ERR_AI_NOT_PERM;
        }
    } else if (!pCtx->bResmpEnabled) {
        pthread_mutex_unlock(&pCtx->mutex);
        HI_TRACE_AI(RE_DBG_LVL, "AI chn %d vqe/resmp not enabled\n", AiChn);
        return HI_ERR_AI_NOT_PERM;
    }

    if (pCtx->pUpvqeHandle == HI_NULL) {
        pthread_mutex_unlock(&pCtx->mutex);
        HI_TRACE_AI(RE_DBG_LVL, "AI chn %d vqe handle is null\n", AiChn);
        return HI_ERR_AI_NOT_PERM;
    }

    result = HI_UPVQE_GetConfig(pCtx->pUpvqeHandle, vqeConfig);
    if (result != HI_SUCCESS) {
        pthread_mutex_unlock(&pCtx->mutex);
        HI_TRACE_AI(RE_DBG_LVL,
            "HI_UPVQE_GetConfig failed, AiDevId:%d, AiChn:%d, ret:0x%x\n",
            AiDevId, AiChn, result);
        return HI_ERR_AI_VQE_ERR;
    }

    /* Check that this is a record VQE config (RecordType at offset 60 must be 0) */
    if (*(HI_S32 *)(vqeConfig + 60) != 0) {
        pthread_mutex_unlock(&pCtx->mutex);
        HI_TRACE_AI(RE_DBG_LVL,
            "AI dev %d chn %d RecordType %d is not record vqe\n",
            AiDevId, AiChn, *(HI_S32 *)(vqeConfig + 60));
        return HI_ERR_AI_VQE_ERR;
    }

    /* Reconstruct OpenMask from per-effect flags stored in CHN_CTX */
    pstVqeConfig->u32OpenMask = 0;
    if (pCtx->field_28) pstVqeConfig->u32OpenMask |= 0x01;  /* HPF */
    if (pCtx->field_2C) pstVqeConfig->u32OpenMask |= 0x02;  /* RNR */
    if (pCtx->field_34) pstVqeConfig->u32OpenMask |= 0x04;  /* EQ */
    if (pCtx->field_38) pstVqeConfig->u32OpenMask |= 0x08;  /* HDR */
    if (pCtx->field_30) pstVqeConfig->u32OpenMask |= 0x10;  /* DRC */
    if (pCtx->field_24) pstVqeConfig->u32OpenMask |= 0x20;  /* AGC */

    /* Extract fields from VQE config buffer */
    pstVqeConfig->s32WorkSampleRate = *(HI_S32 *)(vqeConfig + 40);
    pstVqeConfig->s32FrameSample    = *(HI_S32 *)(vqeConfig + 48);
    pstVqeConfig->enWorkstate       = *(VQE_WORKSTATE_E *)(vqeConfig + 64);
    pstVqeConfig->s32InChNum        = *(HI_S32 *)(vqeConfig + 52);
    pstVqeConfig->s32OutChNum       = *(HI_S32 *)(vqeConfig + 56);
    pstVqeConfig->enRecordType      = VQE_RECORD_NORMAL;

    /* Copy sub-configs from VQE buffer to output struct */
    memcpy_s(&pstVqeConfig->stHpfCfg, sizeof(AUDIO_HPF_CONFIG_S), vqeConfig + 68, 8);
    memcpy_s(&pstVqeConfig->stRnrCfg, sizeof(AI_RNR_CONFIG_S), vqeConfig + 144, 16);
    memcpy_s(&pstVqeConfig->stAgcCfg, sizeof(AUDIO_AGC_CONFIG_S), vqeConfig + 160, 20);
    memcpy_s(&pstVqeConfig->stEqCfg, sizeof(AUDIO_EQ_CONFIG_S), vqeConfig + 180, 16);
    memcpy_s(&pstVqeConfig->stHdrCfg, sizeof(AI_HDR_CONFIG_S), vqeConfig + 196, 24);
    memcpy_s(&pstVqeConfig->stDrcCfg, sizeof(AI_DRC_CONFIG_S), vqeConfig + 220, 28);

    pthread_mutex_unlock(&pCtx->mutex);
    return HI_SUCCESS;
}

static HI_S32
hi_mpi_ai_get_talk_vqe_attr(AUDIO_DEV AiDevId, AI_CHN AiChn, AI_TALKVQE_CONFIG_S *pstVqeConfig)
{
    HI_S32 result;
    HI_U8 vqeConfig[316];
    AI_CHN_CTX_S *pCtx;

    if (AiDevId != 0) {
        HI_TRACE_AI(RE_DBG_LVL, "ai dev %d is invalid\n", AiDevId);
        return HI_ERR_AI_INVALID_DEVID;
    }
    if (AiChn >= MAX_CHN_COUNT) {
        HI_TRACE_AI(RE_DBG_LVL, "ai chnid %d is invalid\n", AiChn);
        return HI_ERR_AI_INVALID_CHNID;
    }
    if (pstVqeConfig == HI_NULL)
        return HI_ERR_AI_NULL_PTR;

    result = ai_check_open(AiChn);
    if (result != HI_SUCCESS) return result;

    pCtx = &s_mpi_ai_chn_ctx[AiChn];
    pthread_mutex_lock(&pCtx->mutex);

    /* Check if talk VQE is configured (field_40==1 && field_7C==2) or resample enabled */
    if (pCtx->field_40 == 1) {
        if (pCtx->field_7C != 2) {
            pthread_mutex_unlock(&pCtx->mutex);
            HI_TRACE_AI(RE_DBG_LVL, "AI chn %d has not set talk vqe attr\n", AiChn);
            return HI_ERR_AI_NOT_PERM;
        }
    } else if (!pCtx->bResmpEnabled) {
        pthread_mutex_unlock(&pCtx->mutex);
        HI_TRACE_AI(RE_DBG_LVL, "AI chn %d vqe/resmp not enabled\n", AiChn);
        return HI_ERR_AI_NOT_PERM;
    }

    if (pCtx->pUpvqeHandle == HI_NULL) {
        pthread_mutex_unlock(&pCtx->mutex);
        HI_TRACE_AI(RE_DBG_LVL, "AI chn %d vqe handle is null\n", AiChn);
        return HI_ERR_AI_NOT_PERM;
    }

    result = HI_UPVQE_GetConfig(pCtx->pUpvqeHandle, vqeConfig);
    if (result != HI_SUCCESS) {
        pthread_mutex_unlock(&pCtx->mutex);
        HI_TRACE_AI(RE_DBG_LVL,
            "HI_UPVQE_GetConfig failed, AiDevId:%d, AiChn:%d, ret:0x%x\n",
            AiDevId, AiChn, result);
        return HI_ERR_AI_VQE_ERR;
    }

    /* Reconstruct OpenMask from per-effect flags stored in CHN_CTX */
    pstVqeConfig->u32OpenMask = 0;
    if (pCtx->field_28) pstVqeConfig->u32OpenMask |= 0x01;  /* HPF */
    if (pCtx->field_20) pstVqeConfig->u32OpenMask |= 0x02;  /* AEC */
    if (pCtx->field_24) pstVqeConfig->u32OpenMask |= 0x08;  /* ANR */
    if (pCtx->field_30) pstVqeConfig->u32OpenMask |= 0x10;  /* EQ */
    if (pCtx->field_1C) pstVqeConfig->u32OpenMask |= 0x20;  /* AGC */

    /* Extract fields from VQE config buffer */
    pstVqeConfig->s32WorkSampleRate = *(HI_S32 *)(vqeConfig + 40);
    pstVqeConfig->s32FrameSample    = *(HI_S32 *)(vqeConfig + 48);
    pstVqeConfig->enWorkstate       = *(VQE_WORKSTATE_E *)(vqeConfig + 64);

    /* Copy sub-configs from VQE buffer to output struct */
    memcpy_s(&pstVqeConfig->stHpfCfg, sizeof(AUDIO_HPF_CONFIG_S), vqeConfig + 68, 8);
    memcpy_s(&pstVqeConfig->stAecCfg, sizeof(AI_AEC_CONFIG_S), vqeConfig + 76, 52);
    memcpy_s(&pstVqeConfig->stAnrCfg, sizeof(AUDIO_ANR_CONFIG_S), vqeConfig + 128, 16);
    memcpy_s(&pstVqeConfig->stAgcCfg, sizeof(AUDIO_AGC_CONFIG_S), vqeConfig + 160, 20);
    memcpy_s(&pstVqeConfig->stEqCfg, sizeof(AUDIO_EQ_CONFIG_S), vqeConfig + 180, 16);

    pthread_mutex_unlock(&pCtx->mutex);
    return HI_SUCCESS;
}

HI_S32
mpi_ai_set_resmp_dbg_info(AUDIO_DEV AiDevId, AI_CHN AiChn, AI_RESMP_DBG_INFO_S *pstDbgInfo)
{
    HI_S32 result;

    if ( AiDevId != 0 ) {
        HI_TRACE_AI(RE_DBG_LVL, "ai dev %d is invalid\n", AiDevId);
        return HI_ERR_AI_INVALID_DEVID;
    }

    if ( AiChn >= MAX_CHN_COUNT ) {
        HI_TRACE_AI(RE_DBG_LVL, "ai chnid %d is invalid\n", AiChn);
        return HI_ERR_AI_INVALID_CHNID;
    }

    if ( pstDbgInfo == HI_NULL )
        return HI_ERR_AI_NULL_PTR;

    result = ai_check_open(AiChn);
    if ( result != HI_SUCCESS ) return result;

    return ioctl(g_ai_fd[AiChn], IOC_AI_SET_RESMP_DBG_INFO, pstDbgInfo);
}

static HI_S32
mpi_ai_set_vqe_dbg_info(AUDIO_DEV AiDevId, AI_CHN AiChn, HI_VOID *pstDbgInfo)
{
    HI_S32 result;

    if ( AiDevId != 0 ) {
        HI_TRACE_AI(RE_DBG_LVL, "ai dev %d is invalid\n", AiDevId);
        return HI_ERR_AI_INVALID_DEVID;
    }
    if ( AiChn >= MAX_CHN_COUNT ) {
        HI_TRACE_AI(RE_DBG_LVL, "ai chnid %d is invalid\n", AiChn);
        return HI_ERR_AI_INVALID_CHNID;
    }
    if ( pstDbgInfo == HI_NULL )
        return HI_ERR_AI_NULL_PTR;

    result = ai_check_open(AiChn);
    if ( result != HI_SUCCESS ) return result;

    return ioctl(g_ai_fd[AiChn], IOC_AI_SET_VQE_DBG_INFO, pstDbgInfo);
}

static HI_S32
mpi_ai_get_vqe_attr(AUDIO_DEV AiDevId, AI_CHN AiChn, HI_VOID *pstConfig)
{
    HI_S32 result;
    HI_U8 vqeConfig[316];
    AI_CHN_CTX_S *pCtx;

    if (AiDevId != 0) {
        HI_TRACE_AI(RE_DBG_LVL, "ai dev %d is invalid\n", AiDevId);
        return HI_ERR_AI_INVALID_DEVID;
    }
    if (AiChn >= MAX_CHN_COUNT) {
        HI_TRACE_AI(RE_DBG_LVL, "ai chnid %d is invalid\n", AiChn);
        return HI_ERR_AI_INVALID_CHNID;
    }
    if (pstConfig == HI_NULL)
        return HI_ERR_AI_NULL_PTR;

    result = ai_check_open(AiChn);
    if (result != HI_SUCCESS) return result;

    pCtx = &s_mpi_ai_chn_ctx[AiChn];
    pthread_mutex_lock(&pCtx->mutex);

    if (!pCtx->bVqeEnabled && !pCtx->bResmpEnabled) {
        pthread_mutex_unlock(&pCtx->mutex);
        HI_TRACE_AI(RE_DBG_LVL, "AI chn %d vqe/resmp not enabled\n", AiChn);
        return HI_ERR_AI_NOT_PERM;
    }

    if (pCtx->pUpvqeHandle == HI_NULL) {
        pthread_mutex_unlock(&pCtx->mutex);
        HI_TRACE_AI(RE_DBG_LVL, "AI chn %d vqe handle is null\n", AiChn);
        return HI_ERR_AI_NOT_PERM;
    }

    result = HI_UPVQE_GetConfig(pCtx->pUpvqeHandle, vqeConfig);
    if (result != HI_SUCCESS) {
        pthread_mutex_unlock(&pCtx->mutex);
        HI_TRACE_AI(RE_DBG_LVL,
            "HI_UPVQE_GetConfig failed, AiDevId:%d, AiChn:%d, ret:0x%x\n",
            AiDevId, AiChn, result);
        return HI_ERR_AI_VQE_ERR;
    }

    memcpy_s(pstConfig, 316, vqeConfig, 316);
    pthread_mutex_unlock(&pCtx->mutex);
    return HI_SUCCESS;
}

HI_VOID*
mpi_ai_chn_get_frm_proc(HI_VOID* arg)
{
    AI_CHN_CTX_S *pstAiChn = (AI_CHN_CTX_S*)arg;
    AI_CHN AiChn;
    HI_S32 subChn;
    AI_FRAME_INFO_EX_S stFrameInfoEx;
    AI_FRAME_INFO_S stPutFrame;
    AUDIO_FRAME_S stAudioFrm;
    AEC_FRAME_S stAecFrm;
    HI_U8 vqeConfig[316];
    HI_S32 result, i;
    HI_U32 numChannels;
    HI_U32 vqeFrameLen;
    HI_VOID *pVqeFrameAddr;
    HI_U32 vqeFrameBytes;
    HI_BOOL is8bit;

    if (pstAiChn == HI_NULL)
        return HI_NULL;

    AiChn = pstAiChn->AiChn;
    subChn = AiChn & 1;
    (void)subChn;

    /* Set thread name */
    prctl(PR_SET_NAME, "AI_GetFrm", 0, 0, 0);

    /* Main processing loop */
    while (pstAiChn->bHasFrmProc == HI_TRUE) {
        /* Check channel is still enabled */
        pthread_mutex_lock(&pstAiChn->mutex);
        if (pstAiChn->bEnabled != HI_TRUE) {
            pthread_mutex_unlock(&pstAiChn->mutex);
            HI_TRACE_AI(RE_DBG_LVL, "ai chn %d disabled in frm proc\n", AiChn);
            usleep(10000);
            continue;
        }
        pthread_mutex_unlock(&pstAiChn->mutex);

        /* Get frame from kernel for processing */
        memset(&stFrameInfoEx, 0, sizeof(AI_FRAME_INFO_EX_S));
        result = ioctl(g_ai_fd[AiChn], IOC_AI_GET_FRM_PROC, &stFrameInfoEx);
        if (result != HI_SUCCESS) {
            if (result == HI_ERR_AI_BUF_EMPTY) {
                usleep(10000);
                continue;
            }
            HI_TRACE_AI(RE_DBG_LVL,
                "ai chn %d get frm failed, ret:0x%x\n", AiChn, result);
            usleep(10000);
            continue;
        }

        /* Lock mutex for frame processing */
        pthread_mutex_lock(&pstAiChn->mutex);

        /* Resolve virtual addresses for audio frame buffers */
        numChannels = (stFrameInfoEx.stInfo.stAudioFrm.enSoundmode == AUDIO_SOUND_MODE_STEREO) ? 2 : 1;
        for (i = 0; i < (HI_S32)numChannels; i++) {
            result = HI_MPI_VB_GetBlockVirAddr(
                stFrameInfoEx.stInfo.stAudioFrm.u32PoolId[i],
                stFrameInfoEx.stInfo.stAudioFrm.u64PhyAddr[i],
                (HI_VOID **)&stFrameInfoEx.stInfo.stAudioFrm.u64VirAddr[i]);
            if (result != HI_SUCCESS)
                goto release_frame;
        }

        /* Resolve AEC reference frame addresses if valid */
        if (stFrameInfoEx.stInfo.stAecFrm.bValid) {
            for (i = 0; i < (HI_S32)numChannels; i++) {
                result = HI_MPI_VB_GetBlockVirAddr(
                    stFrameInfoEx.stInfo.stAecFrm.stRefFrame.u32PoolId[i],
                    stFrameInfoEx.stInfo.stAecFrm.stRefFrame.u64PhyAddr[i],
                    (HI_VOID **)&stFrameInfoEx.stInfo.stAecFrm.stRefFrame.u64VirAddr[i]);
                if (result != HI_SUCCESS)
                    goto release_frame;
            }
        }

        /* Copy frame data to local buffers */
        memcpy_s(&stAudioFrm, sizeof(AUDIO_FRAME_S),
                 &stFrameInfoEx.stInfo.stAudioFrm, sizeof(AUDIO_FRAME_S));

        if (stFrameInfoEx.stInfo.stAecFrm.bValid) {
            memcpy_s(&stAecFrm, sizeof(AEC_FRAME_S),
                     &stFrameInfoEx.stInfo.stAecFrm, sizeof(AEC_FRAME_S));
        }
        else {
            memset(&stAecFrm, 0, sizeof(AEC_FRAME_S));
        }

        /* Determine if 8-bit audio (needs expansion to 16-bit for VQE) */
        is8bit = (stAudioFrm.enBitwidth == AUDIO_BIT_WIDTH_8) ? HI_TRUE : HI_FALSE;

        /* VQE processing path */
        if (pstAiChn->bVqeEnabled && pstAiChn->pUpvqeHandle != HI_NULL) {
            HI_U8 vqeFrame[12]; /* { u32Len, pAddr, u32Bytes } */

            memset(vqeConfig, 0, 316);
            result = HI_UPVQE_GetConfig(pstAiChn->pUpvqeHandle, vqeConfig);
            if (result != HI_SUCCESS) {
                pthread_mutex_unlock(&pstAiChn->mutex);
                HI_TRACE_AI(RE_DBG_LVL,
                    "HI_UPVQE_GetConfig failed, ret:0x%x\n", result);
                goto put_release;
            }

            /* 8-bit to 16-bit expansion into cache buffer */
            if (!is8bit) {
                memcpy_s(pstAiChn->pu8CachBuff, stAudioFrm.u32Len,
                         stAudioFrm.u64VirAddr[0], stAudioFrm.u32Len);
            }
            else {
                HI_U8 *pSrc = stAudioFrm.u64VirAddr[0];
                HI_S16 *pDst = (HI_S16 *)pstAiChn->pu8CachBuff;
                HI_U32 sampleCount = stAudioFrm.u32Len;
                HI_U32 s;
                for (s = 0; s < sampleCount; s++)
                    pDst[s] = (HI_S16)(pSrc[s] << 8);
                stAudioFrm.u32Len = sampleCount * 2;
            }
            stAudioFrm.enBitwidth = AUDIO_BIT_WIDTH_16;

            /* Handle AEC reference frame similarly */
            if (stAecFrm.bValid && stAecFrm.stRefFrame.u64VirAddr[0] != HI_NULL) {
                if (!is8bit) {
                    /* AEC ref frame already 16-bit, just reference it */
                }
                else {
                    /* 8-bit AEC ref expansion would go here if needed */
                }
            }

            /* Prepare VQE frame parameters */
            vqeFrameLen = stAudioFrm.u32Len >> (is8bit ? 0 : stAudioFrm.enBitwidth);
            if (!is8bit)
                vqeFrameLen = stAudioFrm.u32Len;
            *(HI_U32 *)(vqeFrame + 0) = vqeFrameLen;
            *(HI_VOID **)(vqeFrame + 4) = pstAiChn->pu8CachBuff;
            *(HI_U32 *)(vqeFrame + 8) = stAudioFrm.u32Len;

            /* Write frame to VQE for processing */
            result = HI_UPVQE_WriteFrame(pstAiChn->pUpvqeHandle, vqeFrame);
            if (result != HI_SUCCESS) {
                pthread_mutex_unlock(&pstAiChn->mutex);
                HI_TRACE_AI(RE_DBG_LVL,
                    "HI_UPVQE_WriteFrame failed, ai chn %d, ret:0x%x\n", AiChn, result);
                goto put_release;
            }

            /* Adjust length for resample if active */
            if (pstAiChn->bResmpEnabled) {
                HI_U32 resmpLen = *(HI_U32 *)(vqeFrame + 0);
                resmpLen = resmpLen << (is8bit ? 1 : 0);
                *(HI_U32 *)(vqeFrame + 0) = resmpLen;
            }

            /* Read processed frame from VQE */
            result = HI_UPVQE_ReadFrame(pstAiChn->pUpvqeHandle, vqeFrame, 1);
            if (result != HI_SUCCESS && result != (HI_S32)-1) {
                /* ReadFrame returned processed data, update local frame */
                HI_U32 outLen = *(HI_U32 *)(vqeFrame + 0);
                if (is8bit)
                    outLen = outLen << 1;
                stAudioFrm.u32Len = outLen;
            }
        }
        else if (pstAiChn->bResmpEnabled && pstAiChn->pUpvqeHandle != HI_NULL) {
            /* Resample-only path (no VQE) */
            HI_U8 vqeFrame[12];

            if (!is8bit) {
                memcpy_s(pstAiChn->pu8CachBuff, stAudioFrm.u32Len,
                         stAudioFrm.u64VirAddr[0], stAudioFrm.u32Len);
            }
            else {
                HI_U8 *pSrc = stAudioFrm.u64VirAddr[0];
                HI_S16 *pDst = (HI_S16 *)pstAiChn->pu8CachBuff;
                HI_U32 sampleCount = stAudioFrm.u32Len;
                HI_U32 s;
                for (s = 0; s < sampleCount; s++)
                    pDst[s] = (HI_S16)(pSrc[s] << 8);
                stAudioFrm.u32Len = sampleCount * 2;
            }
            stAudioFrm.enBitwidth = AUDIO_BIT_WIDTH_16;

            *(HI_U32 *)(vqeFrame + 0) = stAudioFrm.u32Len;
            *(HI_VOID **)(vqeFrame + 4) = pstAiChn->pu8CachBuff;
            *(HI_U32 *)(vqeFrame + 8) = stAudioFrm.u32Len;

            result = HI_UPVQE_WriteFrame(pstAiChn->pUpvqeHandle, vqeFrame);
            if (result == HI_SUCCESS) {
                result = HI_UPVQE_ReadFrame(pstAiChn->pUpvqeHandle, vqeFrame, 1);
                if (result != HI_SUCCESS && result != (HI_S32)-1) {
                    stAudioFrm.u32Len = *(HI_U32 *)(vqeFrame + 0);
                    if (is8bit)
                        stAudioFrm.u32Len = stAudioFrm.u32Len << 1;
                }
            }
        }

        /* Increment frame counter */
        pstAiChn->u32FrameCount++;
        pthread_mutex_unlock(&pstAiChn->mutex);

put_release:
        /* Build output frame and put back to kernel */
        memcpy_s(&stPutFrame.stAudioFrm, sizeof(AUDIO_FRAME_S),
                 &stAudioFrm, sizeof(AUDIO_FRAME_S));
        memcpy_s(&stPutFrame.stAecFrm, sizeof(AEC_FRAME_S),
                 &stAecFrm, sizeof(AEC_FRAME_S));

        ioctl(g_ai_fd[AiChn], IOC_AI_PUT_FRM_PROC, &stPutFrame);

        continue;

release_frame:
        /* Error path: release frame without processing */
        pthread_mutex_unlock(&pstAiChn->mutex);

        memcpy_s(&stPutFrame.stAudioFrm, sizeof(AUDIO_FRAME_S),
                 &stFrameInfoEx.stInfo.stAudioFrm, sizeof(AUDIO_FRAME_S));
        stPutFrame.stAecFrm.bValid = HI_FALSE;
        ioctl(g_ai_fd[AiChn], IOC_AI_RELEASE_FRAME, &stPutFrame);

        usleep(10000);
    }

    pstAiChn->bHasFrmProc = HI_FALSE;
    return HI_NULL;
}

HI_S32
mpi_ai_init()
{
    HI_S32 result, i;

    if ( s_ai_init == HI_TRUE )
        return HI_SUCCESS;

    memset_s(&s_mpi_ai_dev, sizeof(AI_DEV_INFO_S), 0, sizeof(AI_DEV_INFO_S));
    memset_s(s_mpi_ai_chn_ctx, sizeof(s_mpi_ai_chn_ctx), 0, sizeof(s_mpi_ai_chn_ctx));

    s_mpi_ai_dev.Pool = -1;

    if ( pthread_mutex_init(&s_mpi_ai_dev.mutex, HI_NULL) )
        return HI_FAILURE;

    for (i = 0; i < MAX_CHN_COUNT; i++) {
        if ( pthread_mutex_init(&s_mpi_ai_chn_ctx[i].mutex, HI_NULL) )
            return HI_FAILURE;

        if ( pthread_mutex_init(&g_ast_vqe_state[i].mutex, HI_NULL) )
            return HI_FAILURE;

        s_mpi_ai_chn_ctx[i].field_68 = -1;
        s_mpi_ai_chn_ctx[i].field_6C = -1;
    }

    s_ai_init = HI_TRUE;
    return HI_SUCCESS;
}

HI_S32
mpi_ai_exit()
{
    HI_S32 i;

    if ( s_ai_init == HI_FALSE )
        return HI_SUCCESS;

    for (i = 0; i < MAX_CHN_COUNT; i++) {
        pthread_mutex_destroy(&s_mpi_ai_chn_ctx[i].mutex);
        pthread_mutex_destroy(&g_ast_vqe_state[i].mutex);
    }

    memset_s(&s_mpi_ai_chn_ctx, sizeof(s_mpi_ai_chn_ctx), 0, sizeof(s_mpi_ai_chn_ctx));
    s_mpi_ai_dev.Pool = -1;
    pthread_mutex_destroy(&s_mpi_ai_dev.mutex);
    s_ai_init = HI_FALSE;

    return HI_SUCCESS;
}

HI_VOID
mpi_ai_destroy_get_frm_proc(AI_CHN AiChn)
{
    if ( s_mpi_ai_chn_ctx[AiChn].bHasFrmProc ) {
        pthread_join(s_mpi_ai_chn_ctx[AiChn].FrameProc, HI_NULL);
        s_mpi_ai_chn_ctx[AiChn].bHasFrmProc = HI_FALSE;
    }
}

HI_S32
mpi_ai_release_frame(AI_CHN AiChn, const AUDIO_FRAME_S *pstAudioFrm, const AEC_FRAME_S *pstAecFrm)
{
    HI_S32 result;
    AI_FRAME_INFO_S stFrmInfo;

    memcpy_s(
        &stFrmInfo, sizeof(AUDIO_FRAME_S),
        pstAudioFrm, sizeof(AUDIO_FRAME_S));
    if ( pstAecFrm != HI_NULL )
        memcpy_s(
            &stFrmInfo.stAecFrm, sizeof(AEC_FRAME_S),
            pstAecFrm, sizeof(AEC_FRAME_S));
    else stFrmInfo.stAecFrm.bValid = HI_FALSE;

    result = ioctl(g_ai_fd[AiChn], IOC_AI_RELEASE_FRAME, &stFrmInfo);
    if ( result != HI_SUCCESS ) return result;

    pthread_mutex_lock(&s_mpi_ai_chn_ctx[AiChn].mutex);
    if ( s_mpi_ai_chn_ctx[AiChn].u32FrameCount > 0 )
        s_mpi_ai_chn_ctx[AiChn].u32FrameCount--;
    pthread_mutex_unlock(&s_mpi_ai_chn_ctx[AiChn].mutex);
    return result;
}

HI_S32
HI_MPI_AI_SetPubAttr(AUDIO_DEV AiDevId, const AIO_ATTR_S *pstAttr)
{
    HI_S32 result;

    if ( AiDevId != 0 ) {
        HI_TRACE_AI(RE_DBG_LVL, "ai dev %d is invalid\n", AiDevId);
        return HI_ERR_AI_INVALID_DEVID;
    }

    if ( pstAttr == HI_NULL )
        return HI_ERR_AI_NULL_PTR;

    result = ai_check_open(0);
    if ( result != HI_SUCCESS ) return result;

    return ioctl(g_ai_fd[0], IOC_AI_SET_PUB_ATTR, pstAttr);
}

HI_S32
HI_MPI_AI_GetPubAttr(AUDIO_DEV AiDevId, AIO_ATTR_S *pstAttr)
{
    HI_S32 result;

    if ( AiDevId != 0 ) {
        HI_TRACE_AI(RE_DBG_LVL, "ai dev %d is invalid\n", AiDevId);
        return HI_ERR_AI_INVALID_DEVID;
    }

    if ( pstAttr == HI_NULL )
        return HI_ERR_AI_NULL_PTR;

    result = ai_check_open(0);
    if ( result != HI_SUCCESS ) return result;

    return ioctl(g_ai_fd[0], IOC_AI_GET_PUB_ATTR, pstAttr);
}

HI_S32
HI_MPI_AI_Enable(AUDIO_DEV AiDevId)
{
    HI_S32 result;
    VB_POOL Pool;

    if ( AiDevId != 0 ) {
        HI_TRACE_AI(RE_DBG_LVL, "ai dev %d is invalid\n", AiDevId);
        return HI_ERR_AI_INVALID_DEVID;
    }

    result = ai_check_open(0);
    if ( result != HI_SUCCESS ) return result;

    result = ioctl(g_ai_fd[0], IOC_AI_ENABLE);
    if ( result != HI_SUCCESS ) return result;

    result = ioctl(g_ai_fd[0], IOC_AI_GET_POOL_ID, &Pool);
    if ( result != HI_SUCCESS ) return result;

    pthread_mutex_lock(&s_mpi_ai_dev.mutex);
    if ( s_mpi_ai_dev.Pool == -1 ) {
        result = HI_MPI_VB_MmapPool(Pool);
        if ( result != HI_SUCCESS ) {
            pthread_mutex_unlock(&s_mpi_ai_dev.mutex);
            return result;
        }
        s_mpi_ai_dev.Pool = Pool;
    }
    pthread_mutex_unlock(&s_mpi_ai_dev.mutex);

    return HI_SUCCESS;
}

HI_S32
HI_MPI_AI_Disable(AUDIO_DEV AiDevId)
{
    HI_S32 result, i;

    if ( AiDevId != 0 ) {
        HI_TRACE_AI(RE_DBG_LVL, "ai dev %d is invalid\n", AiDevId);
        return HI_ERR_AI_INVALID_DEVID;
    }

    result = ai_check_open(0);
    if ( result != HI_SUCCESS ) return result;

    pthread_mutex_lock(&s_mpi_ai_dev.mutex);

    for (i = 0; i < MAX_CHN_COUNT; i++) {
        pthread_mutex_lock(&s_mpi_ai_chn_ctx[i].mutex);
        if ( s_mpi_ai_chn_ctx[0].bEnabled ) {
            pthread_mutex_unlock(&s_mpi_ai_chn_ctx[i].mutex);
            HI_TRACE_AI(RE_DBG_LVL,
                "disable aidev(%d) fail, chn(%d) busy now!\n",
                /*AiDevId=*/0, i);
            pthread_mutex_unlock(&s_mpi_ai_dev.mutex);
            return HI_ERR_AI_BUSY;
        }
        pthread_mutex_unlock(&s_mpi_ai_chn_ctx[i].mutex);
    }

    if ( s_mpi_ai_dev.Pool != -1 ) {
        result = HI_MPI_VB_MunmapPool(s_mpi_ai_dev.Pool);
        if ( result != HI_SUCCESS ) {
            pthread_mutex_unlock(&s_mpi_ai_dev.mutex);
            return result;
        }
        s_mpi_ai_dev.Pool = -1;
    }

    result = ioctl(g_ai_fd[0], IOC_AI_DISABLE);
    pthread_mutex_unlock(&s_mpi_ai_dev.mutex);
    return result;
}

HI_S32
HI_MPI_AI_EnableChn(AUDIO_DEV AiDevId, AI_CHN AiChn)
{
    HI_S32 result;

    if ( AiDevId != 0 ) {
        HI_TRACE_AI(RE_DBG_LVL, "ai dev %d is invalid\n", AiDevId);
        return HI_ERR_AI_INVALID_DEVID;
    }

    if ( AiChn >= MAX_CHN_COUNT ) {
        HI_TRACE_AI(RE_DBG_LVL, "ai chnid %d is invalid\n", AiChn);
        return HI_ERR_AI_INVALID_CHNID;
    }

    result = ai_check_open(AiChn);
    if ( result != HI_SUCCESS ) return result;

    pthread_mutex_lock(&s_mpi_ai_chn_ctx[AiChn].mutex);

    if ( s_mpi_ai_chn_ctx[AiChn].bEnabled ) {
        pthread_mutex_unlock(&s_mpi_ai_chn_ctx[AiChn].mutex);
        return HI_SUCCESS;
    }

    result = ioctl(g_ai_fd[AiChn], IOC_AI_ENABLE_CHN);
    if ( result != HI_SUCCESS ) {
        pthread_mutex_unlock(&s_mpi_ai_chn_ctx[AiChn].mutex);
        return result;
    }

    s_mpi_ai_chn_ctx[AiChn].pu8CachBuff = (HI_U8*)malloc(CACHE_BUF_SIZE);
    if ( s_mpi_ai_chn_ctx[AiChn].pu8CachBuff == HI_NULL ) {
        pthread_mutex_unlock(&s_mpi_ai_chn_ctx[AiChn].mutex);
        HI_TRACE_AI(RE_DBG_LVL, "ai chn malloc cachbuff err.\n");
        return HI_ERR_AI_NOMEM;
    }

    memset_s(s_mpi_ai_chn_ctx[AiChn].pu8CachBuff, CACHE_BUF_SIZE, 0, CACHE_BUF_SIZE);

    if ( !s_mpi_ai_chn_ctx[AiChn].bHasFrmProc ) {
        s_mpi_ai_chn_ctx[AiChn].bHasFrmProc = HI_TRUE;
        s_mpi_ai_chn_ctx[AiChn].AiChn       = AiChn;

        result = pthread_create(
            &s_mpi_ai_chn_ctx[AiChn].FrameProc,
            HI_NULL,
            mpi_ai_chn_get_frm_proc,
            &s_mpi_ai_chn_ctx[AiChn]);
        if ( result != HI_SUCCESS ) {
            free(s_mpi_ai_chn_ctx[AiChn].pu8CachBuff);
            s_mpi_ai_chn_ctx[AiChn].pu8CachBuff = HI_NULL;
            pthread_mutex_unlock(&s_mpi_ai_chn_ctx[AiChn].mutex);
            HI_TRACE_AI(RE_DBG_LVL, "ai chn create frame process err.\n");
            return HI_ERR_AI_NOMEM;
        }
    }

    s_mpi_ai_chn_ctx[AiChn].bEnabled = HI_TRUE;
    pthread_mutex_unlock(&s_mpi_ai_chn_ctx[AiChn].mutex);
    return HI_SUCCESS;
}

static HI_S32
mpi_ai_enable_resmp(AI_CHN AiChn, AI_RESMP_S *pstResmp)
{
    HI_S32 result;
    HI_U8 vqeConfig[316];
    HI_U8 newConfig[316];
    AI_CHN_CTX_S *pCtx = &s_mpi_ai_chn_ctx[AiChn];
    AUDIO_DEV AiDevId = AiChn / 2;
    HI_S32 subChn = AiChn & 1;
    AST_VQE_STATE_S *pVqeState = &g_ast_vqe_state[AiChn];

    memset(vqeConfig, 0, 316);
    memset(newConfig, 0, 316);

    pthread_mutex_lock(&pCtx->mutex);

    if (pCtx->field_40) {
        /* VQE is configured, get existing config */
        pthread_mutex_unlock(&pCtx->mutex);
        result = mpi_ai_get_vqe_attr(AiDevId, subChn, vqeConfig);
        if (result != HI_SUCCESS) {
            HI_TRACE_AI(RE_DBG_LVL,
                "Resmp attr check failed!\n");
            return HI_ERR_AI_NOT_CONFIG;
        }
        pthread_mutex_lock(&pCtx->mutex);
    }
    else {
        /* No VQE config, create minimal config for resample */
        memset_s(vqeConfig, 316, 0, 316);
        *(HI_S32 *)(vqeConfig + 40) = pstResmp->enInSampleRate;
        *(HI_U32 *)(vqeConfig + 48) = 80;
        *(HI_U32 *)(vqeConfig + 52) = 1;
        *(HI_U32 *)(vqeConfig + 56) = 1;
        *(HI_U32 *)(vqeConfig + 60) = 1;
    }

    /* Set resample sample rates in config */
    *(HI_S32 *)(vqeConfig + 36) = pstResmp->enInSampleRate;
    *(HI_S32 *)(vqeConfig + 44) = pstResmp->enOutSampleRate;

    /* Lock VQE state, destroy old UPVQE, create new */
    pthread_mutex_lock(&pVqeState->mutex);

    HI_UPVQE_Destroy(&pCtx->pUpvqeHandle);
    pCtx->pUpvqeHandle = HI_NULL;
    pVqeState->field_0 = 0;

    memcpy_s(newConfig, 316, vqeConfig, 316);
    result = HI_UPVQE_Create(&pCtx->pUpvqeHandle, newConfig);
    if (result != HI_SUCCESS) {
        pthread_mutex_unlock(&pVqeState->mutex);
        pthread_mutex_unlock(&pCtx->mutex);
        HI_TRACE_AI(RE_DBG_LVL,
            "Ai Resmp enable failed!\n");
        return HI_ERR_AI_VQE_ERR;
    }

    pVqeState->field_4 = 1;
    pVqeState->field_0 = (HI_U32)(HI_UL)pCtx->pUpvqeHandle;
    pthread_mutex_unlock(&pVqeState->mutex);

    pCtx->bResmpEnabled = HI_TRUE;
    memcpy_s(&pCtx->field_58, sizeof(AI_RESMP_S),
             pstResmp, sizeof(AI_RESMP_S));
    pthread_mutex_unlock(&pCtx->mutex);

    return HI_SUCCESS;
}

static HI_S32
mpi_ai_disable_resmp(AI_CHN AiChn)
{
    HI_S32 result;
    HI_U8 vqeConfig[316];
    HI_U8 newConfig[316];
    AIO_ATTR_S stAttr;
    AI_CHN_CTX_S *pCtx = &s_mpi_ai_chn_ctx[AiChn];
    AUDIO_DEV AiDevId = AiChn / 2;
    HI_S32 subChn = AiChn & 1;
    AST_VQE_STATE_S *pVqeState = &g_ast_vqe_state[AiChn];

    pthread_mutex_lock(&pCtx->mutex);

    if (!pCtx->bResmpEnabled) {
        pthread_mutex_unlock(&pCtx->mutex);
        return HI_SUCCESS;
    }

    pthread_mutex_unlock(&pCtx->mutex);

    result = mpi_ai_get_vqe_attr(AiDevId, subChn, vqeConfig);
    if (result != HI_SUCCESS) {
        HI_TRACE_AI(RE_DBG_LVL,
            "Resmp attr check failed!\n");
        return HI_ERR_AI_NOT_CONFIG;
    }

    pthread_mutex_lock(&pCtx->mutex);
    memcpy_s(newConfig, 316, vqeConfig, 316);

    memset(&stAttr, 0, sizeof(AIO_ATTR_S));
    result = HI_MPI_AI_GetPubAttr(AiDevId, &stAttr);
    if (result != HI_SUCCESS) {
        pthread_mutex_unlock(&pCtx->mutex);
        HI_TRACE_AI(RE_DBG_LVL,
            "enable resample fail,Aidev%d don't have chn%d\n",
            AiDevId, subChn);
        return HI_ERR_AI_NOT_CONFIG;
    }

    /* Set both in/out sample rates to device rate (disable resample) */
    *(HI_S32 *)(vqeConfig + 36) = stAttr.enSamplerate;
    *(HI_S32 *)(vqeConfig + 44) = stAttr.enSamplerate;

    /* Lock VQE state, destroy old UPVQE, create new without resample */
    pthread_mutex_lock(&pVqeState->mutex);

    HI_UPVQE_Destroy(&pCtx->pUpvqeHandle);
    pCtx->pUpvqeHandle = HI_NULL;
    pVqeState->field_0 = 0;

    result = HI_UPVQE_Create(&pCtx->pUpvqeHandle, vqeConfig);
    if (result != HI_SUCCESS) {
        pthread_mutex_unlock(&pVqeState->mutex);
        pthread_mutex_unlock(&pCtx->mutex);
        HI_TRACE_AI(RE_DBG_LVL,
            "Ai Resmp enable failed!\n");
        return HI_ERR_AI_VQE_ERR;
    }

    pCtx->bResmpEnabled = HI_FALSE;
    pVqeState->field_4 = 0;
    pVqeState->field_0 = (HI_U32)(HI_UL)pCtx->pUpvqeHandle;
    pthread_mutex_unlock(&pVqeState->mutex);
    pthread_mutex_unlock(&pCtx->mutex);

    return HI_SUCCESS;
}

static HI_S32
hi_mpi_ai_disable_chn(AUDIO_DEV AiDevId, AI_CHN AiChn)
{
    HI_S32 result;

    if ( AiDevId != 0 ) {
        HI_TRACE_AI(RE_DBG_LVL, "ai dev %d is invalid\n", AiDevId);
        return HI_ERR_AI_INVALID_DEVID;
    }
    if ( AiChn >= MAX_CHN_COUNT ) {
        HI_TRACE_AI(RE_DBG_LVL, "ai chnid %d is invalid\n", AiChn);
        return HI_ERR_AI_INVALID_CHNID;
    }

    result = ai_check_open(AiChn);
    if ( result != HI_SUCCESS ) return result;

    pthread_mutex_lock(&s_mpi_ai_chn_ctx[AiChn].mutex);

    if ( !s_mpi_ai_chn_ctx[AiChn].bEnabled ) {
        pthread_mutex_unlock(&s_mpi_ai_chn_ctx[AiChn].mutex);
        return HI_ERR_AI_NOT_ENABLED;
    }

    mpi_ai_destroy_get_frm_proc(AiChn);

    if ( s_mpi_ai_chn_ctx[AiChn].bResmpEnabled )
        mpi_ai_disable_resmp(AiChn);

    if ( s_mpi_ai_chn_ctx[AiChn].pUpvqeHandle != HI_NULL ) {
        HI_UPVQE_Destroy(&s_mpi_ai_chn_ctx[AiChn].pUpvqeHandle);
        s_mpi_ai_chn_ctx[AiChn].pUpvqeHandle = HI_NULL;
    }

    s_mpi_ai_chn_ctx[AiChn].bVqeEnabled = HI_FALSE;
    s_mpi_ai_chn_ctx[AiChn].bEnabled    = HI_FALSE;
    s_mpi_ai_chn_ctx[AiChn].field_20    = HI_FALSE;

    if ( s_mpi_ai_chn_ctx[AiChn].pu8CachBuff != HI_NULL ) {
        free(s_mpi_ai_chn_ctx[AiChn].pu8CachBuff);
        s_mpi_ai_chn_ctx[AiChn].pu8CachBuff = HI_NULL;
    }

    result = ioctl(g_ai_fd[AiChn], 0x00005A0D);

    if ( s_mpi_ai_chn_ctx[AiChn].bAecRefFrameEnabled ) {
        ioctl(g_ai_fd[AiChn], IOC_AI_DISABLE_AEC_REF_FRAME);
        s_mpi_ai_chn_ctx[AiChn].bAecRefFrameEnabled = HI_FALSE;
    }

    pthread_mutex_unlock(&s_mpi_ai_chn_ctx[AiChn].mutex);
    return result;
}

HI_S32
HI_MPI_AI_DisableChn(AUDIO_DEV AiDevId, AI_CHN AiChn)
{ return hi_mpi_ai_disable_chn(AiDevId, AiChn); }

HI_S32
HI_MPI_AI_EnableReSmp(AUDIO_DEV AiDevId, AI_CHN AiChn, AUDIO_SAMPLE_RATE_E enOutSampleRate)
{
    HI_S32 result;
    AIO_ATTR_S stAttr;
    AI_RESMP_DBG_INFO_S stDbgInfo;

    if ( AiDevId != 0 ) {
        HI_TRACE_AI(RE_DBG_LVL, "ai dev %d is invalid\n", AiDevId);
        return HI_ERR_AI_INVALID_DEVID;
    }

    if ( AiChn >= MAX_CHN_COUNT ) {
        HI_TRACE_AI(RE_DBG_LVL, "ai chnid %d is invalid\n", AiChn);
        return HI_ERR_AI_INVALID_CHNID;
    }

    result = ai_check_open(AiChn);
    if ( result != HI_SUCCESS ) return result;

    pthread_mutex_lock(&s_mpi_ai_chn_ctx[AiChn].mutex);

    if ( !s_mpi_ai_chn_ctx[AiChn].bEnabled ) {
        pthread_mutex_unlock(&s_mpi_ai_chn_ctx[AiChn].mutex);
        HI_TRACE_AI(RE_DBG_LVL, "AI chn %d is not enable\n", AiChn);
        return HI_ERR_AI_NOT_ENABLED;
    }

    if (s_mpi_ai_chn_ctx[AiChn].bResmpEnabled &&
        (enOutSampleRate != s_mpi_ai_chn_ctx[AiChn].enSampleRate))
    {
        pthread_mutex_unlock(&s_mpi_ai_chn_ctx[AiChn].mutex);
        HI_TRACE_AI(RE_DBG_LVL,
            "resmp has been enabled but the resamplerate:"
            "%d not the same as before:%d.\n",
            enOutSampleRate, s_mpi_ai_chn_ctx[AiChn].enSampleRate);
        return HI_ERR_AI_NOT_PERM;
    }

    if (enOutSampleRate != AUDIO_SAMPLE_RATE_8000  &&
        enOutSampleRate != AUDIO_SAMPLE_RATE_11025 &&
        enOutSampleRate != AUDIO_SAMPLE_RATE_12000 &&
        enOutSampleRate != AUDIO_SAMPLE_RATE_16000 &&
        enOutSampleRate != AUDIO_SAMPLE_RATE_22050 &&
        enOutSampleRate != AUDIO_SAMPLE_RATE_24000 &&
        enOutSampleRate != AUDIO_SAMPLE_RATE_32000 &&
        enOutSampleRate != AUDIO_SAMPLE_RATE_44100 &&
        enOutSampleRate != AUDIO_SAMPLE_RATE_48000)
    {
        HI_TRACE_AI(RE_DBG_LVL, "out_sample_rate:%d is invalid\n", enOutSampleRate);
        result = HI_ERR_AI_ILLEGAL_PARAM;
        goto error;
    }

    memset(&stAttr, 0, sizeof(AIO_ATTR_S));

    if (ai_check_open(0) != HI_SUCCESS ||
        ioctl(g_ai_fd[0], IOC_AI_GET_PUB_ATTR, &stAttr) != HI_SUCCESS)
    {
        result = HI_ERR_AI_NOT_CONFIG;
        goto error;
    }

    if ( enOutSampleRate == stAttr.enSamplerate ) {
        HI_TRACE_AI(RE_DBG_LVL,
            "in_sample_rate is same as out_sample_rate, it's not allowed.\n");
        result = HI_ERR_AI_NOT_PERM;
        goto error;
    }

    if (stAttr.enSamplerate == AUDIO_SAMPLE_RATE_64000 &&
        enOutSampleRate != AUDIO_SAMPLE_RATE_16000     &&
        enOutSampleRate != AUDIO_SAMPLE_RATE_8000)
    {
        HI_TRACE_AI(RE_DBG_LVL, "resample only support 64k->8k/16k!\n");
        result = HI_ERR_AI_ILLEGAL_PARAM;
        goto error;
    }

    if ( stAttr.enSoundmode == AUDIO_SOUND_MODE_STEREO ) {
        HI_TRACE_AI(RE_DBG_LVL, "resample don't support stereo!\n");
        result = HI_ERR_AI_ILLEGAL_PARAM;
        goto error;
    }

    if ( AiChn >= stAttr.u32ChnCnt ) {
        HI_TRACE_AI(RE_DBG_LVL,
            "enable resample fail,aidev%d don't have chn%d\n",
            /*AiDevId=*/0, AiChn);
        result = 0xA0158002;
        goto error;
    }

    memset(&stAttr, 0, sizeof(AIO_ATTR_S));

    if (ai_check_open(0) != HI_SUCCESS ||
        ioctl(g_ai_fd[0], IOC_AI_GET_PUB_ATTR, &stAttr) != HI_SUCCESS)
    {
        pthread_mutex_unlock(&s_mpi_ai_chn_ctx[AiChn].mutex);
        return HI_ERR_AI_NOT_CONFIG;
    }

    if ( stAttr.enSamplerate == AUDIO_SAMPLE_RATE_96000 ) {
        pthread_mutex_unlock(&s_mpi_ai_chn_ctx[AiChn].mutex);
        HI_TRACE_AI(RE_DBG_LVL,
            "resmp is not permit when ai samplerate is %d!\n",
            stAttr.enSamplerate);
        return HI_ERR_AI_NOT_PERM;
    }

    stDbgInfo.stResmp.u32PtNumPerFrm  = stAttr.u32PtNumPerFrm;
    stDbgInfo.stResmp.enInSampleRate  = stAttr.enSamplerate;
    stDbgInfo.stResmp.enOutSampleRate = enOutSampleRate;

    pthread_mutex_unlock(&s_mpi_ai_chn_ctx[AiChn].mutex);
    result = mpi_ai_enable_resmp(AiChn, &stDbgInfo.stResmp);
    if ( result != HI_SUCCESS ) {
        HI_TRACE_AI(RE_DBG_LVL, "ai resmp enable failed!\n");
        return result;
    }

    pthread_mutex_lock(&s_mpi_ai_chn_ctx[AiChn].mutex);
    stDbgInfo.bEnabled = s_mpi_ai_chn_ctx[AiChn].bResmpEnabled;
    mpi_ai_set_resmp_dbg_info(/*AiDevId=*/0, AiChn, &stDbgInfo);
    pthread_mutex_unlock(&s_mpi_ai_chn_ctx[AiChn].mutex);

    return HI_SUCCESS;

    error:
    pthread_mutex_unlock(&s_mpi_ai_chn_ctx[AiChn].mutex);
    HI_TRACE_AI(RE_DBG_LVL, "resmp attr check failed!\n");
    return result;
}

HI_S32
HI_MPI_AI_DisableReSmp(AUDIO_DEV AiDevId, AI_CHN AiChn)
{
    HI_S32 result;
    AI_RESMP_DBG_INFO_S stDbgInfo;

    if ( AiDevId != 0 ) {
        HI_TRACE_AI(RE_DBG_LVL, "ai dev %d is invalid\n", AiDevId);
        return HI_ERR_AI_INVALID_DEVID;
    }

    if ( AiChn >= MAX_CHN_COUNT ) {
        HI_TRACE_AI(RE_DBG_LVL, "ai chnid %d is invalid\n", AiChn);
        return HI_ERR_AI_INVALID_CHNID;
    }

    result = ai_check_open(AiChn);
    if ( result != HI_SUCCESS ) return result;

    pthread_mutex_lock(&s_mpi_ai_chn_ctx[AiChn].mutex);

    if ( !s_mpi_ai_chn_ctx[AiChn].bEnabled ) {
        pthread_mutex_unlock(&s_mpi_ai_chn_ctx[AiChn].mutex);
        return HI_ERR_AI_NOT_ENABLED;
    }

    pthread_mutex_unlock(&s_mpi_ai_chn_ctx[AiChn].mutex);

    result = mpi_ai_disable_resmp(AiChn);
    if ( result != HI_SUCCESS ) return result;

    pthread_mutex_lock(&s_mpi_ai_chn_ctx[AiChn].mutex);
    memset_s(&stDbgInfo, sizeof(stDbgInfo), 0, sizeof(stDbgInfo));
    mpi_ai_set_resmp_dbg_info(/*AiDevId=*/0, AiChn, &stDbgInfo);
    pthread_mutex_unlock(&s_mpi_ai_chn_ctx[AiChn].mutex);

    return HI_SUCCESS;
}

static HI_S32
hi_mpi_ai_set_record_vqe_attr(AUDIO_DEV AiDevId, AI_CHN AiChn, const AI_RECORDVQE_CONFIG_S *pstVqeConfig)
{
    HI_S32 result;
    AIO_ATTR_S stAttr;
    HI_U8 vqeConfig[316];
    AI_CHN_CTX_S *pCtx;
    AST_VQE_STATE_S *pVqeState = &g_ast_vqe_state[AiChn];

    if (AiDevId != 0) {
        HI_TRACE_AI(RE_DBG_LVL, "ai dev %d is invalid\n", AiDevId);
        return HI_ERR_AI_INVALID_DEVID;
    }
    if (AiChn >= MAX_CHN_COUNT) {
        HI_TRACE_AI(RE_DBG_LVL, "ai chnid %d is invalid\n", AiChn);
        return HI_ERR_AI_INVALID_CHNID;
    }
    if (pstVqeConfig == HI_NULL)
        return HI_ERR_AI_NULL_PTR;

    result = ai_check_open(AiChn);
    if (result != HI_SUCCESS) return result;

    pCtx = &s_mpi_ai_chn_ctx[AiChn];
    pthread_mutex_lock(&pCtx->mutex);

    if (!pCtx->bEnabled) {
        pthread_mutex_unlock(&pCtx->mutex);
        return HI_ERR_AI_NOT_ENABLED;
    }

    if (pCtx->bVqeEnabled) {
        pthread_mutex_unlock(&pCtx->mutex);
        HI_TRACE_AI(RE_DBG_LVL,
            "AI chn %d has enable vqe! Please disable vqe then config it!\n", AiChn);
        return HI_ERR_AI_NOT_PERM;
    }

    /* Validate parameters */
    if (pstVqeConfig->s32FrameSample < 80 ||
        pstVqeConfig->s32FrameSample > 4096) {
        pthread_mutex_unlock(&pCtx->mutex);
        HI_TRACE_AI(RE_DBG_LVL,
            "frame length: %d is invalid, ai chn:%d.\n",
            pstVqeConfig->s32FrameSample, AiChn);
        return HI_ERR_AI_ILLEGAL_PARAM;
    }

    if (pstVqeConfig->enWorkstate > VQE_WORKSTATE_NOISY) {
        pthread_mutex_unlock(&pCtx->mutex);
        HI_TRACE_AI(RE_DBG_LVL,
            "work mode: %d is invalid, ai chn:%d.\n",
            pstVqeConfig->enWorkstate, AiChn);
        return HI_ERR_AI_ILLEGAL_PARAM;
    }

    if (pstVqeConfig->s32WorkSampleRate != 16000 &&
        pstVqeConfig->s32WorkSampleRate != 48000) {
        pthread_mutex_unlock(&pCtx->mutex);
        HI_TRACE_AI(RE_DBG_LVL,
            "work sample rate: %d is invalid, ai chn:%d.\n",
            pstVqeConfig->s32WorkSampleRate, AiChn);
        return HI_ERR_AI_ILLEGAL_PARAM;
    }

    if (pstVqeConfig->s32InChNum != pstVqeConfig->s32OutChNum) {
        pthread_mutex_unlock(&pCtx->mutex);
        HI_TRACE_AI(RE_DBG_LVL,
            "Only support InputCh equal to OutputCh now,  InputCh: %d, OutputCh: %d, ai chn:%d.\n",
            pstVqeConfig->s32InChNum, pstVqeConfig->s32OutChNum, AiChn);
        return HI_ERR_AI_ILLEGAL_PARAM;
    }

    if (pstVqeConfig->s32InChNum < 1 || pstVqeConfig->s32InChNum > 2) {
        pthread_mutex_unlock(&pCtx->mutex);
        HI_TRACE_AI(RE_DBG_LVL,
            "InputCh: %d is invalid, ai chn:%d.\n",
            pstVqeConfig->s32InChNum, AiChn);
        return HI_ERR_AI_ILLEGAL_PARAM;
    }

    if (pstVqeConfig->s32OutChNum < 1 || pstVqeConfig->s32OutChNum > 2) {
        pthread_mutex_unlock(&pCtx->mutex);
        HI_TRACE_AI(RE_DBG_LVL,
            "OutputCh: %d is invalid, ai chn:%d.\n",
            pstVqeConfig->s32OutChNum, AiChn);
        return HI_ERR_AI_ILLEGAL_PARAM;
    }

    if (pstVqeConfig->enRecordType != VQE_RECORD_NORMAL) {
        pthread_mutex_unlock(&pCtx->mutex);
        HI_TRACE_AI(RE_DBG_LVL,
            "RecordType: %d is invalid, ai chn:%d.\n",
            pstVqeConfig->enRecordType, AiChn);
        return HI_ERR_AI_ILLEGAL_PARAM;
    }

    if (pstVqeConfig->u32OpenMask == 0) {
        pthread_mutex_unlock(&pCtx->mutex);
        HI_TRACE_AI(RE_DBG_LVL,
            "open mask(0x%x) param err! HPF,HDR,RNR,DRC,EQ,AGC all not open, ai chn:%d\n",
            pstVqeConfig->u32OpenMask, AiChn);
        return HI_ERR_AI_ILLEGAL_PARAM;
    }

    if (pstVqeConfig->u32OpenMask > 0x3f) {
        pthread_mutex_unlock(&pCtx->mutex);
        HI_TRACE_AI(RE_DBG_LVL,
            "open mask(0x%x) param err! open effect exclude HPF,HDR,RNR,DRC,EQ,AGC ai chn:%d\n",
            pstVqeConfig->u32OpenMask, AiChn);
        return HI_ERR_AI_ILLEGAL_PARAM;
    }

    /* Get pub attr and validate sample rate */
    result = HI_MPI_AI_GetPubAttr(0, &stAttr);
    if (result != HI_SUCCESS) {
        pthread_mutex_unlock(&pCtx->mutex);
        return result;
    }

    pthread_mutex_unlock(&pCtx->mutex);

    /* Check if VQE or resample already configured */
    if (pCtx->field_40 || pCtx->bResmpEnabled) {
        /* Get existing VQE config to merge with */
        /* (handled by enable_vqe later) */
    }

    pthread_mutex_lock(&pCtx->mutex);

    /* Validate sample rate compatibility */
    if (stAttr.enSamplerate == AUDIO_SAMPLE_RATE_96000 ||
        stAttr.enSamplerate == AUDIO_SAMPLE_RATE_64000) {
        pthread_mutex_unlock(&pCtx->mutex);
        HI_TRACE_AI(RE_DBG_LVL,
            "vqe is not permit when Ai samplerate is %d!\n",
            stAttr.enSamplerate);
        return HI_ERR_AI_ILLEGAL_PARAM;
    }

    /* Validate stereo/mono vs channel count */
    if (stAttr.enSoundmode == AUDIO_SOUND_MODE_STEREO) {
        if (pstVqeConfig->s32InChNum != 2) {
            pthread_mutex_unlock(&pCtx->mutex);
            HI_TRACE_AI(RE_DBG_LVL,
                "stereo mode record vqe is not support when s32InChNum is %d!\n",
                pstVqeConfig->s32InChNum);
            return HI_ERR_AI_ILLEGAL_PARAM;
        }
    }
    else {
        if (pstVqeConfig->s32InChNum != 1) {
            pthread_mutex_unlock(&pCtx->mutex);
            HI_TRACE_AI(RE_DBG_LVL,
                "mono mode record vqe is not support when s32InChNum is %d!\n",
                pstVqeConfig->s32InChNum);
            return HI_ERR_AI_ILLEGAL_PARAM;
        }
    }

    /* Validate individual effects based on open mask */

    /* Bit 0: HPF */
    if (pstVqeConfig->u32OpenMask & 0x1) {
        if (pstVqeConfig->stHpfCfg.bUsrMode > 1) {
            pthread_mutex_unlock(&pCtx->mutex);
            HI_TRACE_AI(RE_DBG_LVL, "bUsrMode: %d error!\n",
                pstVqeConfig->stHpfCfg.bUsrMode);
            return HI_ERR_AI_ILLEGAL_PARAM;
        }
        if (pstVqeConfig->stHpfCfg.bUsrMode &&
            pstVqeConfig->stHpfCfg.enHpfFreq != AUDIO_HPF_FREQ_80 &&
            pstVqeConfig->stHpfCfg.enHpfFreq != AUDIO_HPF_FREQ_120 &&
            pstVqeConfig->stHpfCfg.enHpfFreq != AUDIO_HPF_FREQ_150) {
            pthread_mutex_unlock(&pCtx->mutex);
            HI_TRACE_AI(RE_DBG_LVL,
                "hpf freq: %d is invalid, ai chn:%d.\n",
                pstVqeConfig->stHpfCfg.enHpfFreq, AiChn);
            return HI_ERR_AI_ILLEGAL_PARAM;
        }
    }

    /* Bit 1: RNR */
    if (pstVqeConfig->u32OpenMask & 0x2) {
        if (pstVqeConfig->stRnrCfg.bUsrMode > 1) {
            pthread_mutex_unlock(&pCtx->mutex);
            HI_TRACE_AI(RE_DBG_LVL, "bUsrMode: %d error!\n",
                pstVqeConfig->stRnrCfg.bUsrMode);
            return HI_ERR_AI_ILLEGAL_PARAM;
        }
        if (pstVqeConfig->stRnrCfg.bUsrMode) {
            if (pstVqeConfig->stRnrCfg.s32MaxNrLevel < 2 ||
                pstVqeConfig->stRnrCfg.s32MaxNrLevel > 20) {
                pthread_mutex_unlock(&pCtx->mutex);
                HI_TRACE_AI(RE_DBG_LVL,
                    "rnr MaxNrLevel: %d is invalid, ai chn:%d.\n",
                    pstVqeConfig->stRnrCfg.s32MaxNrLevel, AiChn);
                return HI_ERR_AI_ILLEGAL_PARAM;
            }
            if (pstVqeConfig->stRnrCfg.s32NoiseThresh < -80 ||
                pstVqeConfig->stRnrCfg.s32NoiseThresh > -20) {
                pthread_mutex_unlock(&pCtx->mutex);
                HI_TRACE_AI(RE_DBG_LVL,
                    "rnr NoiseThresh: %d is invalid, ai chn:%d.\n",
                    pstVqeConfig->stRnrCfg.s32NoiseThresh, AiChn);
                return HI_ERR_AI_ILLEGAL_PARAM;
            }
        }
    }

    /* Bit 5: AGC */
    if (pstVqeConfig->u32OpenMask & 0x20) {
        if (pstVqeConfig->stAgcCfg.bUsrMode > 1) {
            pthread_mutex_unlock(&pCtx->mutex);
            HI_TRACE_AI(RE_DBG_LVL, "bUsrMode: %d error!\n",
                pstVqeConfig->stAgcCfg.bUsrMode);
            return HI_ERR_AI_ILLEGAL_PARAM;
        }
        if (pstVqeConfig->stAgcCfg.bUsrMode) {
            if (pstVqeConfig->stAgcCfg.s16NoiseSupSwitch > 1) {
                pthread_mutex_unlock(&pCtx->mutex);
                return HI_ERR_AI_ILLEGAL_PARAM;
            }
            if (pstVqeConfig->stAgcCfg.s8OutputMode > 2) {
                pthread_mutex_unlock(&pCtx->mutex);
                return HI_ERR_AI_ILLEGAL_PARAM;
            }
            if (pstVqeConfig->stAgcCfg.s8AdjustSpeed > 10) {
                pthread_mutex_unlock(&pCtx->mutex);
                return HI_ERR_AI_ILLEGAL_PARAM;
            }
            if (pstVqeConfig->stAgcCfg.s8MaxGain > 30) {
                pthread_mutex_unlock(&pCtx->mutex);
                return HI_ERR_AI_ILLEGAL_PARAM;
            }
            if ((HI_U8)(pstVqeConfig->stAgcCfg.s8NoiseFloor + 50) > 30) {
                pthread_mutex_unlock(&pCtx->mutex);
                return HI_ERR_AI_ILLEGAL_PARAM;
            }
            if (pstVqeConfig->stAgcCfg.s8UseHighPassFilt > 1) {
                pthread_mutex_unlock(&pCtx->mutex);
                return HI_ERR_AI_ILLEGAL_PARAM;
            }
            if ((HI_U8)(pstVqeConfig->stAgcCfg.s8TargetLevel + 40) > 39) {
                pthread_mutex_unlock(&pCtx->mutex);
                return HI_ERR_AI_ILLEGAL_PARAM;
            }
            if (pstVqeConfig->stAgcCfg.s8ImproveSNR > 5) {
                pthread_mutex_unlock(&pCtx->mutex);
                return HI_ERR_AI_ILLEGAL_PARAM;
            }
        }
    }

    /* Build internal VQE config buffer (316 bytes, opaque to UPVQE library) */
    memset(vqeConfig, 0, 316);

    /* Per-effect enable flags at offsets 0..32 (zeroed by memset) */

    /* Sample rates and frame config */
    *(HI_S32 *)(vqeConfig + 36) = stAttr.enSamplerate;  /* InSampleRate */
    *(HI_S32 *)(vqeConfig + 40) = pstVqeConfig->s32WorkSampleRate;
    *(HI_S32 *)(vqeConfig + 44) = stAttr.enSamplerate;  /* OutSampleRate */
    *(HI_S32 *)(vqeConfig + 48) = pstVqeConfig->s32FrameSample;
    *(HI_S32 *)(vqeConfig + 52) = pstVqeConfig->s32InChNum;
    *(HI_S32 *)(vqeConfig + 56) = pstVqeConfig->s32OutChNum;
    *(HI_S32 *)(vqeConfig + 60) = pstVqeConfig->enRecordType;
    *(HI_S32 *)(vqeConfig + 64) = pstVqeConfig->enWorkstate;

    /* Copy sub-configs to their offsets in the VQE buffer */
    memcpy_s(vqeConfig + 68, 8, &pstVqeConfig->stHpfCfg, sizeof(AUDIO_HPF_CONFIG_S));
    memcpy_s(vqeConfig + 144, 16, &pstVqeConfig->stRnrCfg, sizeof(AI_RNR_CONFIG_S));
    memcpy_s(vqeConfig + 160, 20, &pstVqeConfig->stAgcCfg, sizeof(AUDIO_AGC_CONFIG_S));
    memcpy_s(vqeConfig + 180, 16, &pstVqeConfig->stEqCfg, sizeof(AUDIO_EQ_CONFIG_S));
    memcpy_s(vqeConfig + 196, 24, &pstVqeConfig->stHdrCfg, sizeof(AI_HDR_CONFIG_S));
    memcpy_s(vqeConfig + 220, 28, &pstVqeConfig->stDrcCfg, sizeof(AI_DRC_CONFIG_S));

    /* Lock VQE state, create UPVQE */
    pthread_mutex_lock(&pVqeState->mutex);

    HI_UPVQE_Destroy(&pCtx->pUpvqeHandle);
    pCtx->pUpvqeHandle = HI_NULL;
    pVqeState->field_0 = 0;

    result = HI_UPVQE_Create(&pCtx->pUpvqeHandle, vqeConfig);
    if (result != HI_SUCCESS) {
        pthread_mutex_unlock(&pVqeState->mutex);
        pthread_mutex_unlock(&pCtx->mutex);
        HI_TRACE_AI(RE_DBG_LVL, "create upvqe failed, ret:0x%x\n", result);
        return HI_ERR_AI_VQE_ERR;
    }

    pVqeState->field_0 = (HI_U32)(HI_UL)pCtx->pUpvqeHandle;
    pthread_mutex_unlock(&pVqeState->mutex);

    /* Set channel context flags */
    pCtx->field_40 = 1;
    pCtx->field_7C = 4;  /* record VQE type */
    pCtx->field_20 = HI_FALSE;
    pCtx->field_1C = 0;
    pCtx->field_3C = 0;

    /* Store per-effect enabled flags from OpenMask */
    pCtx->field_28 = (pstVqeConfig->u32OpenMask >> 0) & 1;  /* HPF */
    pCtx->field_2C = (pstVqeConfig->u32OpenMask >> 1) & 1;  /* RNR */
    pCtx->field_34 = (pstVqeConfig->u32OpenMask >> 2) & 1;  /* EQ */
    pCtx->field_38 = (pstVqeConfig->u32OpenMask >> 3) & 1;  /* HDR */
    pCtx->field_30 = (pstVqeConfig->u32OpenMask >> 4) & 1;  /* DRC */
    pCtx->field_24 = (pstVqeConfig->u32OpenMask >> 5) & 1;  /* AGC */

    pthread_mutex_unlock(&pCtx->mutex);

    /* Send VQE debug info to kernel */
    mpi_ai_set_vqe_dbg_info(AiDevId, AiChn, vqeConfig);

    return HI_SUCCESS;
}

static HI_S32
hi_mpi_ai_set_talk_vqe_attr(AUDIO_DEV AiDevId, AI_CHN AiChn, AUDIO_DEV AoDevId, AO_CHN AoChn, const AI_TALKVQE_CONFIG_S *pstVqeConfig)
{
    HI_S32 result;
    AIO_ATTR_S stAttr;
    HI_U8 vqeConfig[316];
    AI_CHN_CTX_S *pCtx;
    AST_VQE_STATE_S *pVqeState = &g_ast_vqe_state[AiChn];

    if (AiDevId != 0) {
        HI_TRACE_AI(RE_DBG_LVL, "ai dev %d is invalid\n", AiDevId);
        return HI_ERR_AI_INVALID_DEVID;
    }
    if (AiChn >= MAX_CHN_COUNT) {
        HI_TRACE_AI(RE_DBG_LVL, "ai chnid %d is invalid\n", AiChn);
        return HI_ERR_AI_INVALID_CHNID;
    }
    if (pstVqeConfig == HI_NULL)
        return HI_ERR_AI_NULL_PTR;

    result = ai_check_open(AiChn);
    if (result != HI_SUCCESS) return result;

    pCtx = &s_mpi_ai_chn_ctx[AiChn];
    pthread_mutex_lock(&pCtx->mutex);

    if (!pCtx->bEnabled) {
        pthread_mutex_unlock(&pCtx->mutex);
        return HI_ERR_AI_NOT_ENABLED;
    }

    if (pCtx->bVqeEnabled) {
        pthread_mutex_unlock(&pCtx->mutex);
        HI_TRACE_AI(RE_DBG_LVL,
            "AI chn %d has enable vqe! Please disable vqe then config it!\n", AiChn);
        return HI_ERR_AI_NOT_PERM;
    }

    /* Validate FrameSample [80, 4096] */
    if (pstVqeConfig->s32FrameSample < 80 ||
        pstVqeConfig->s32FrameSample > 4096) {
        pthread_mutex_unlock(&pCtx->mutex);
        HI_TRACE_AI(RE_DBG_LVL,
            "frame length: %d is invalid, ai chn:%d.\n",
            pstVqeConfig->s32FrameSample, AiChn);
        return HI_ERR_AI_ILLEGAL_PARAM;
    }

    /* Validate WorkState */
    if (pstVqeConfig->enWorkstate > VQE_WORKSTATE_NOISY) {
        pthread_mutex_unlock(&pCtx->mutex);
        HI_TRACE_AI(RE_DBG_LVL,
            "work mode: %d is invalid, ai chn:%d.\n",
            pstVqeConfig->enWorkstate, AiChn);
        return HI_ERR_AI_ILLEGAL_PARAM;
    }

    /* Talk VQE: WorkSampleRate must be 8000 or 16000 */
    if (pstVqeConfig->s32WorkSampleRate != 8000 &&
        pstVqeConfig->s32WorkSampleRate != 16000) {
        pthread_mutex_unlock(&pCtx->mutex);
        HI_TRACE_AI(RE_DBG_LVL,
            "work sample rate: %d is invalid, ai chn:%d.\n",
            pstVqeConfig->s32WorkSampleRate, AiChn);
        return HI_ERR_AI_ILLEGAL_PARAM;
    }

    /* Validate OpenMask: bits 0(HPF), 1(AEC), 3(ANR), 4(EQ), 5(AGC) */
    if (pstVqeConfig->u32OpenMask == 0) {
        pthread_mutex_unlock(&pCtx->mutex);
        HI_TRACE_AI(RE_DBG_LVL,
            "open mask(0x%x) param err! all not open, ai chn:%d\n",
            pstVqeConfig->u32OpenMask, AiChn);
        return HI_ERR_AI_ILLEGAL_PARAM;
    }
    if (pstVqeConfig->u32OpenMask > 0x3b) {
        pthread_mutex_unlock(&pCtx->mutex);
        HI_TRACE_AI(RE_DBG_LVL,
            "open mask(0x%x) param err! ai chn:%d\n",
            pstVqeConfig->u32OpenMask, AiChn);
        return HI_ERR_AI_ILLEGAL_PARAM;
    }
    /* Reject bit 2 set (invalid for talk) — mask 0x3b has bit 2 clear */
    if (pstVqeConfig->u32OpenMask & 0x04) {
        pthread_mutex_unlock(&pCtx->mutex);
        HI_TRACE_AI(RE_DBG_LVL,
            "open mask(0x%x) param err! ai chn:%d\n",
            pstVqeConfig->u32OpenMask, AiChn);
        return HI_ERR_AI_ILLEGAL_PARAM;
    }

    /* Get pub attr and validate sample rate */
    result = HI_MPI_AI_GetPubAttr(0, &stAttr);
    if (result != HI_SUCCESS) {
        pthread_mutex_unlock(&pCtx->mutex);
        return result;
    }

    if (stAttr.enSamplerate == AUDIO_SAMPLE_RATE_96000 ||
        stAttr.enSamplerate == AUDIO_SAMPLE_RATE_64000) {
        pthread_mutex_unlock(&pCtx->mutex);
        HI_TRACE_AI(RE_DBG_LVL,
            "vqe is not permit when Ai samplerate is %d!\n",
            stAttr.enSamplerate);
        return HI_ERR_AI_ILLEGAL_PARAM;
    }

    pthread_mutex_unlock(&pCtx->mutex);

    /* Validate AEC parameters if AEC enabled (bit 1) */
    if (pstVqeConfig->u32OpenMask & 0x02) {
        /* Check AEC ref frame not already enabled */
        if (pCtx->bAecRefFrameEnabled) {
            pthread_mutex_unlock(&pCtx->mutex);
            HI_TRACE_AI(RE_DBG_LVL,
                "AI chn %d AEC ref frame already enabled\n", AiChn);
            return HI_ERR_AI_NOT_PERM;
        }

        /* Validate AO device and channel for AEC reference */
        if (AoDevId > 1) {
            HI_TRACE_AI(RE_DBG_LVL, "ao dev %d is invalid\n", AoDevId);
            return HI_ERR_AI_ILLEGAL_PARAM;
        }
        if (AoChn > 2) {
            HI_TRACE_AI(RE_DBG_LVL, "ao chnid %d is invalid\n", AoChn);
            return HI_ERR_AI_ILLEGAL_PARAM;
        }

        if (pstVqeConfig->stAecCfg.bUsrMode > 1) {
            HI_TRACE_AI(RE_DBG_LVL, "bUsrMode: %d error!\n",
                pstVqeConfig->stAecCfg.bUsrMode);
            return HI_ERR_AI_ILLEGAL_PARAM;
        }

        if (pstVqeConfig->stAecCfg.bUsrMode) {
            HI_S32 bandLimit = (pstVqeConfig->s32WorkSampleRate == 8000) ? 63 : 127;
            HI_S32 i;

            if (pstVqeConfig->stAecCfg.s8CngMode > 1)
                return HI_ERR_AI_ILLEGAL_PARAM;
            if (pstVqeConfig->stAecCfg.s16DTHnlSortQTh < 0)
                return HI_ERR_AI_ILLEGAL_PARAM;
            if (pstVqeConfig->stAecCfg.s8NearAllPassEnergy > 2)
                return HI_ERR_AI_ILLEGAL_PARAM;
            if (pstVqeConfig->stAecCfg.s8NearCleanSupEnergy > 2)
                return HI_ERR_AI_ILLEGAL_PARAM;

            /* Validate ERL values [0, 18] */
            for (i = 0; i < 7; i++) {
                if ((HI_U16)pstVqeConfig->stAecCfg.s16ERL[i] > 18)
                    return HI_ERR_AI_ILLEGAL_PARAM;
            }

            /* Validate band parameters against sample rate limit */
            if (pstVqeConfig->stAecCfg.s16EchoBandLow < 1 ||
                pstVqeConfig->stAecCfg.s16EchoBandLow > bandLimit)
                return HI_ERR_AI_ILLEGAL_PARAM;
            if (pstVqeConfig->stAecCfg.s16EchoBandHigh < 1 ||
                pstVqeConfig->stAecCfg.s16EchoBandHigh > bandLimit)
                return HI_ERR_AI_ILLEGAL_PARAM;
            if (pstVqeConfig->stAecCfg.s16EchoBandLow >= pstVqeConfig->stAecCfg.s16EchoBandHigh)
                return HI_ERR_AI_ILLEGAL_PARAM;
            if (pstVqeConfig->stAecCfg.s16EchoBandLow2 < 1 ||
                pstVqeConfig->stAecCfg.s16EchoBandLow2 > bandLimit)
                return HI_ERR_AI_ILLEGAL_PARAM;
            if (pstVqeConfig->stAecCfg.s16EchoBandHigh2 < 1 ||
                pstVqeConfig->stAecCfg.s16EchoBandHigh2 > bandLimit + 1)
                return HI_ERR_AI_ILLEGAL_PARAM;
            if (pstVqeConfig->stAecCfg.s16VioceProtectFreqL < 1 ||
                pstVqeConfig->stAecCfg.s16VioceProtectFreqL > bandLimit)
                return HI_ERR_AI_ILLEGAL_PARAM;
            if (pstVqeConfig->stAecCfg.s16VioceProtectFreqL1 < 1 ||
                pstVqeConfig->stAecCfg.s16VioceProtectFreqL1 > bandLimit + 1)
                return HI_ERR_AI_ILLEGAL_PARAM;

            /* ERLBand values must be in range and ascending */
            for (i = 0; i < 6; i++) {
                if (pstVqeConfig->stAecCfg.s16ERLBand[i] < 1 ||
                    pstVqeConfig->stAecCfg.s16ERLBand[i] > bandLimit + 1)
                    return HI_ERR_AI_ILLEGAL_PARAM;
                if (i > 0 && pstVqeConfig->stAecCfg.s16ERLBand[i-1] >= pstVqeConfig->stAecCfg.s16ERLBand[i])
                    return HI_ERR_AI_ILLEGAL_PARAM;
            }
        }
    }

    /* Validate ANR parameters if ANR enabled (bit 3) */
    if (pstVqeConfig->u32OpenMask & 0x08) {
        if (pstVqeConfig->stAnrCfg.bUsrMode > 1)
            return HI_ERR_AI_ILLEGAL_PARAM;
        if (pstVqeConfig->stAnrCfg.bUsrMode) {
            if (pstVqeConfig->stAnrCfg.s16NrIntensity > 25)
                return HI_ERR_AI_ILLEGAL_PARAM;
            if (pstVqeConfig->stAnrCfg.s16NoiseDbThr < 30 ||
                pstVqeConfig->stAnrCfg.s16NoiseDbThr > 60)
                return HI_ERR_AI_ILLEGAL_PARAM;
        }
    }

    /* Validate HPF if enabled (bit 0) */
    if (pstVqeConfig->u32OpenMask & 0x01) {
        if (pstVqeConfig->stHpfCfg.bUsrMode > 1)
            return HI_ERR_AI_ILLEGAL_PARAM;
        if (pstVqeConfig->stHpfCfg.bUsrMode &&
            pstVqeConfig->stHpfCfg.enHpfFreq != AUDIO_HPF_FREQ_80 &&
            pstVqeConfig->stHpfCfg.enHpfFreq != AUDIO_HPF_FREQ_120 &&
            pstVqeConfig->stHpfCfg.enHpfFreq != AUDIO_HPF_FREQ_150)
            return HI_ERR_AI_ILLEGAL_PARAM;
    }

    /* Validate AGC if enabled (bit 5) */
    if (pstVqeConfig->u32OpenMask & 0x20) {
        if (pstVqeConfig->stAgcCfg.bUsrMode > 1)
            return HI_ERR_AI_ILLEGAL_PARAM;
        if (pstVqeConfig->stAgcCfg.bUsrMode) {
            if (pstVqeConfig->stAgcCfg.s16NoiseSupSwitch > 1)
                return HI_ERR_AI_ILLEGAL_PARAM;
            if (pstVqeConfig->stAgcCfg.s8OutputMode > 2)
                return HI_ERR_AI_ILLEGAL_PARAM;
            if (pstVqeConfig->stAgcCfg.s8AdjustSpeed > 10)
                return HI_ERR_AI_ILLEGAL_PARAM;
            if (pstVqeConfig->stAgcCfg.s8MaxGain > 30)
                return HI_ERR_AI_ILLEGAL_PARAM;
            if ((HI_U8)(pstVqeConfig->stAgcCfg.s8NoiseFloor + 65) > 45)
                return HI_ERR_AI_ILLEGAL_PARAM;
            if (pstVqeConfig->stAgcCfg.s8UseHighPassFilt > 1)
                return HI_ERR_AI_ILLEGAL_PARAM;
            if ((HI_U8)(pstVqeConfig->stAgcCfg.s8TargetLevel + 40) > 39)
                return HI_ERR_AI_ILLEGAL_PARAM;
            if (pstVqeConfig->stAgcCfg.s8ImproveSNR > 5)
                return HI_ERR_AI_ILLEGAL_PARAM;
        }
    }

    /* Validate EQ if enabled (bit 4) */
    if (pstVqeConfig->u32OpenMask & 0x10) {
        HI_S32 i;
        for (i = 0; i < VQE_EQ_BAND_NUM; i++) {
            if (pstVqeConfig->stEqCfg.s8GaindB[i] < -100 ||
                pstVqeConfig->stEqCfg.s8GaindB[i] > 20)
                return HI_ERR_AI_ILLEGAL_PARAM;
        }
    }

    /* Build internal VQE config buffer */
    memset(vqeConfig, 0, 316);

    /* Sample rates and frame config */
    *(HI_S32 *)(vqeConfig + 36) = stAttr.enSamplerate;  /* InSampleRate */
    *(HI_S32 *)(vqeConfig + 40) = pstVqeConfig->s32WorkSampleRate;
    *(HI_S32 *)(vqeConfig + 44) = stAttr.enSamplerate;  /* OutSampleRate */
    *(HI_S32 *)(vqeConfig + 48) = pstVqeConfig->s32FrameSample;
    *(HI_S32 *)(vqeConfig + 52) = 1;  /* InChNum = 1 for talk */
    *(HI_S32 *)(vqeConfig + 56) = 1;  /* OutChNum = 1 for talk */
    *(HI_S32 *)(vqeConfig + 60) = 1;  /* Talk mode indicator */
    *(HI_S32 *)(vqeConfig + 64) = pstVqeConfig->enWorkstate;

    /* Copy sub-configs */
    memcpy_s(vqeConfig + 68, 8, &pstVqeConfig->stHpfCfg, sizeof(AUDIO_HPF_CONFIG_S));
    memcpy_s(vqeConfig + 76, 52, &pstVqeConfig->stAecCfg, sizeof(AI_AEC_CONFIG_S));
    memcpy_s(vqeConfig + 128, 16, &pstVqeConfig->stAnrCfg, sizeof(AUDIO_ANR_CONFIG_S));
    memcpy_s(vqeConfig + 160, 20, &pstVqeConfig->stAgcCfg, sizeof(AUDIO_AGC_CONFIG_S));
    memcpy_s(vqeConfig + 180, 16, &pstVqeConfig->stEqCfg, sizeof(AUDIO_EQ_CONFIG_S));

    /* Lock VQE state, create UPVQE */
    pthread_mutex_lock(&pVqeState->mutex);

    HI_UPVQE_Destroy(&pCtx->pUpvqeHandle);
    pCtx->pUpvqeHandle = HI_NULL;
    pVqeState->field_0 = 0;

    result = HI_UPVQE_Create(&pCtx->pUpvqeHandle, vqeConfig);
    if (result != HI_SUCCESS) {
        pthread_mutex_unlock(&pVqeState->mutex);
        HI_TRACE_AI(RE_DBG_LVL, "create upvqe failed, ret:0x%x\n", result);
        return HI_ERR_AI_VQE_ERR;
    }

    pVqeState->field_0 = (HI_U32)(HI_UL)pCtx->pUpvqeHandle;
    pthread_mutex_unlock(&pVqeState->mutex);

    /* Set channel context flags */
    pCtx->field_40 = 1;
    pCtx->field_7C = 2;  /* talk VQE type */
    pCtx->field_2C = 0;
    pCtx->field_38 = 0;
    pCtx->field_3C = 0;
    pCtx->field_34 = 0;

    /* Store per-effect enabled flags from OpenMask */
    pCtx->field_28 = (pstVqeConfig->u32OpenMask >> 0) & 1;  /* HPF */
    pCtx->field_20 = (pstVqeConfig->u32OpenMask >> 1) & 1;  /* AEC */
    pCtx->field_24 = (pstVqeConfig->u32OpenMask >> 3) & 1;  /* ANR */
    pCtx->field_30 = (pstVqeConfig->u32OpenMask >> 4) & 1;  /* EQ */
    pCtx->field_1C = (pstVqeConfig->u32OpenMask >> 5) & 1;  /* AGC */

    pthread_mutex_unlock(&pCtx->mutex);

    /* Send VQE debug info to kernel */
    mpi_ai_set_vqe_dbg_info(AiDevId, AiChn, vqeConfig);

    return HI_SUCCESS;
}

static HI_S32
hi_mpi_ai_enable_vqe(AUDIO_DEV AiDevId, AI_CHN AiChn)
{
    HI_S32 result;
    HI_U8 vqeConfig[316];
    HI_U8 newConfig[316];
    AIO_ATTR_S stAttr;
    HI_U8 dbgInfo[320];
    AI_CHN_CTX_S *pCtx;
    AST_VQE_STATE_S *pVqeState;
    HI_U32 saved_field_20, saved_field_24, saved_field_1C, saved_field_2C;
    HI_U32 saved_field_28, saved_field_30, saved_field_34, saved_field_38, saved_field_3C;

    memset(vqeConfig, 0, 316);
    memset(newConfig, 0, 316);
    memset(&stAttr, 0, sizeof(AIO_ATTR_S));

    if (AiDevId != 0) {
        HI_TRACE_AI(RE_DBG_LVL, "ai dev %d is invalid\n", AiDevId);
        return HI_ERR_AI_INVALID_DEVID;
    }
    if (AiChn > 1) {
        HI_TRACE_AI(RE_DBG_LVL, "ai chnid %d is invalid\n", AiChn);
        return HI_ERR_AI_INVALID_CHNID;
    }

    result = ai_check_open(AiChn);
    if (result != HI_SUCCESS) return result;

    pCtx = &s_mpi_ai_chn_ctx[AiChn];
    pVqeState = &g_ast_vqe_state[AiChn];

    pthread_mutex_lock(&pCtx->mutex);

    if (pCtx->bEnabled != HI_TRUE) {
        pthread_mutex_unlock(&pCtx->mutex);
        HI_TRACE_AI(RE_DBG_LVL, "ai chn %d is not enabled\n", AiChn);
        return HI_ERR_AI_NOT_ENABLED;
    }

    if (pCtx->bVqeEnabled == HI_TRUE) {
        /* Already enabled — return success */
        pthread_mutex_unlock(&pCtx->mutex);
        return HI_SUCCESS;
    }

    if (pCtx->field_40 != 1) {
        pthread_mutex_unlock(&pCtx->mutex);
        HI_TRACE_AI(RE_DBG_LVL, "ai chn %d vqe attr not config\n", AiChn);
        return HI_ERR_AI_NOT_CONFIG;
    }

    /* Get pub attr and check soundmode */
    result = HI_MPI_AI_GetPubAttr(AiDevId, &stAttr);
    if (result != HI_SUCCESS) {
        pthread_mutex_unlock(&pCtx->mutex);
        HI_TRACE_AI(RE_DBG_LVL, "GetPubAttr failed, ret:0x%x\n", result);
        return HI_ERR_AI_NOT_CONFIG;
    }

    /* Stereo mode only allows record VQE (field_7C==4) */
    if (stAttr.enSoundmode == AUDIO_SOUND_MODE_STEREO) {
        if (pCtx->field_7C != 4) {
            pthread_mutex_unlock(&pCtx->mutex);
            HI_TRACE_AI(RE_DBG_LVL,
                "stereo mode only support record vqe!\n");
            return HI_ERR_AI_ILLEGAL_PARAM;
        }
    }

    /* Check channel index within device channel count */
    if (AiChn >= stAttr.u32ChnCnt) {
        pthread_mutex_unlock(&pCtx->mutex);
        HI_TRACE_AI(RE_DBG_LVL,
            "ai dev %d chn %d is invalid, u32ChnCnt:%d\n",
            AiDevId, AiChn, stAttr.u32ChnCnt);
        return HI_ERR_AI_INVALID_CHNID;
    }

    /* Get existing VQE config (unlocks pCtx internally) */
    pthread_mutex_unlock(&pCtx->mutex);
    result = mpi_ai_get_vqe_attr(AiDevId, AiChn, vqeConfig);
    if (result != HI_SUCCESS) {
        HI_TRACE_AI(RE_DBG_LVL,
            "MPI_AI_GetVqeAttr failed, ret:0x%x\n", result);
        return HI_ERR_AI_NOT_CONFIG;
    }
    pthread_mutex_lock(&pCtx->mutex);

    /* Check sample rate consistency between VQE config and pub attr */
    if (*(HI_S32 *)(vqeConfig + 36) != (HI_S32)stAttr.enSamplerate) {
        pthread_mutex_unlock(&pCtx->mutex);
        HI_TRACE_AI(RE_DBG_LVL,
            "sample rate mismatch, vqe:%d, pub:%d\n",
            *(HI_S32 *)(vqeConfig + 36), stAttr.enSamplerate);
        return HI_ERR_AI_ILLEGAL_PARAM;
    }

    /* Save per-effect flags from channel context */
    saved_field_20 = pCtx->field_20;
    saved_field_24 = pCtx->field_24;
    saved_field_1C = pCtx->field_1C;
    saved_field_2C = pCtx->field_2C;
    saved_field_28 = pCtx->field_28;
    saved_field_30 = pCtx->field_30;
    saved_field_34 = pCtx->field_34;
    saved_field_38 = pCtx->field_38;
    saved_field_3C = pCtx->field_3C;

    /* For talk VQE with AEC: do AEC enable ioctl before VQE re-creation */
    if (saved_field_20 == 1) {
        if (pCtx->bAecRefFrameEnabled == HI_TRUE) {
            pthread_mutex_unlock(&pCtx->mutex);
            HI_TRACE_AI(RE_DBG_LVL,
                "ai chn %d aec ref frame already enabled\n", AiChn);
            return HI_ERR_AI_NOT_SUPPORT;
        }

        AI_DEV_ID_S stDevId;
        stDevId.AiDevId = pCtx->field_68;
        stDevId.AiChn = pCtx->field_6C;

        result = ioctl(g_ai_fd[AiChn], IOC_AI_VQE_ENABLE, &stDevId);
        if (result != HI_SUCCESS) {
            pthread_mutex_unlock(&pCtx->mutex);
            HI_TRACE_AI(RE_DBG_LVL,
                "ai dev %d chn %d aec init failed\n", AiDevId, AiChn);
            return result;
        }
        pCtx->field_70 = 0;
    }

    /* Lock VQE state, destroy old UPVQE, create new */
    pthread_mutex_lock(&pVqeState->mutex);

    HI_UPVQE_Destroy(&pCtx->pUpvqeHandle);
    pCtx->pUpvqeHandle = HI_NULL;
    pVqeState->field_0 = 0;

    memcpy_s(newConfig, 316, vqeConfig, 316);
    result = HI_UPVQE_Create(&pCtx->pUpvqeHandle, newConfig);
    if (result != HI_SUCCESS) {
        pCtx->field_40 = 0;
        pthread_mutex_unlock(&pVqeState->mutex);
        pthread_mutex_unlock(&pCtx->mutex);
        HI_TRACE_AI(RE_DBG_LVL,
            "create upvqe failed, ai dev %d chn %d, ret:0x%x\n",
            AiDevId, AiChn, result);
        return HI_ERR_AI_VQE_ERR;
    }

    pVqeState->field_4 = 1;
    pVqeState->field_0 = (HI_U32)(HI_UL)pCtx->pUpvqeHandle;
    pthread_mutex_unlock(&pVqeState->mutex);

    /* Set VQE enabled flag */
    pCtx->bVqeEnabled = HI_TRUE;

    /* Build VQE debug info (320 bytes) */
    memset(dbgInfo, 0, 320);
    *(HI_U32 *)(dbgInfo + 0) = 1;                /* bEnabled */
    *(HI_U32 *)(dbgInfo + 4) = saved_field_28;   /* HPF */
    *(HI_U32 *)(dbgInfo + 8) = saved_field_20;   /* AEC */
    *(HI_U32 *)(dbgInfo + 12) = saved_field_1C;
    *(HI_U32 *)(dbgInfo + 16) = saved_field_2C;  /* RNR */
    *(HI_U32 *)(dbgInfo + 20) = saved_field_24;  /* AGC */
    *(HI_U32 *)(dbgInfo + 24) = saved_field_30;  /* DRC/EQ */
    *(HI_U32 *)(dbgInfo + 28) = saved_field_34;  /* EQ */
    *(HI_U32 *)(dbgInfo + 32) = saved_field_38;  /* HDR */
    *(HI_U32 *)(dbgInfo + 36) = saved_field_3C;
    *(HI_S32 *)(dbgInfo + 44) = *(HI_S32 *)(vqeConfig + 40);   /* WorkSampleRate */
    *(HI_S32 *)(dbgInfo + 52) = *(HI_S32 *)(vqeConfig + 48);   /* FrameSample */
    *(HI_S32 *)(dbgInfo + 68) = *(HI_S32 *)(vqeConfig + 64);   /* WorkState */
    memcpy_s(dbgInfo + 72, 8, vqeConfig + 68, 8);      /* HPF config */
    memcpy_s(dbgInfo + 80, 52, vqeConfig + 76, 52);    /* AEC config */
    memcpy_s(dbgInfo + 132, 16, vqeConfig + 128, 16);  /* ANR config */
    memcpy_s(dbgInfo + 148, 16, vqeConfig + 144, 16);  /* RNR config */
    memcpy_s(dbgInfo + 164, 20, vqeConfig + 160, 20);  /* AGC config */
    memcpy_s(dbgInfo + 184, 16, vqeConfig + 180, 16);  /* EQ config */
    memcpy_s(dbgInfo + 200, 24, vqeConfig + 196, 24);  /* HDR config */
    memcpy_s(dbgInfo + 224, 28, vqeConfig + 220, 28);  /* DRC config */
    memcpy_s(dbgInfo + 252, 68, vqeConfig + 248, 68);  /* tail data */

    /* For record VQE (no AEC): zero AEC config area, set mode flag */
    if (saved_field_20 == 0) {
        memset_s(dbgInfo + 80, 52, 0, 52);
        dbgInfo[84] = 2;
    }

    mpi_ai_set_vqe_dbg_info(AiDevId, AiChn, dbgInfo);

    pthread_mutex_unlock(&pCtx->mutex);
    return HI_SUCCESS;
}

static HI_S32
hi_mpi_ai_disable_vqe(AUDIO_DEV AiDevId, AI_CHN AiChn)
{
    HI_S32 result;
    HI_U8 vqeConfig[316];
    HI_U8 newConfig[316];
    HI_U8 dbgInfo[320];
    AI_CHN_CTX_S *pCtx;
    AST_VQE_STATE_S *pVqeState;

    if (AiDevId != 0) {
        HI_TRACE_AI(RE_DBG_LVL, "ai dev %d is invalid\n", AiDevId);
        return HI_ERR_AI_INVALID_DEVID;
    }
    if (AiChn > 1) {
        HI_TRACE_AI(RE_DBG_LVL, "ai chnid %d is invalid\n", AiChn);
        return HI_ERR_AI_INVALID_CHNID;
    }

    result = ai_check_open(AiChn);
    if (result != HI_SUCCESS) return result;

    pCtx = &s_mpi_ai_chn_ctx[AiChn];
    pVqeState = &g_ast_vqe_state[AiChn];

    pthread_mutex_lock(&pCtx->mutex);

    if (pCtx->bVqeEnabled == 0) {
        /* Not enabled — return success */
        pthread_mutex_unlock(&pCtx->mutex);
        return HI_SUCCESS;
    }

    if (pCtx->bVqeEnabled == 1) {
        /* If AEC is enabled (talk VQE), disable it via ioctl */
        if (pCtx->field_20 == 1) {
            result = ioctl(g_ai_fd[AiChn], IOC_AI_VQE_DISABLE);
            if (result != HI_SUCCESS) {
                pthread_mutex_unlock(&pCtx->mutex);
                HI_TRACE_AI(RE_DBG_LVL,
                    "ai dev %d chn %d aec disable failed\n", AiDevId, AiChn);
                return result;
            }
        }
    }

    /* Get existing VQE config */
    pthread_mutex_unlock(&pCtx->mutex);
    result = mpi_ai_get_vqe_attr(AiDevId, AiChn, vqeConfig);
    if (result != HI_SUCCESS) {
        HI_TRACE_AI(RE_DBG_LVL,
            "MPI_AI_GetVqeAttr failed, ret:0x%x\n", result);
        return HI_ERR_AI_NOT_CONFIG;
    }

    pthread_mutex_lock(&pCtx->mutex);

    /* Zero out the per-effect enable section (first 36 bytes of VQE config) */
    *(HI_U32 *)(vqeConfig + 0) = 0;
    *(HI_U32 *)(vqeConfig + 4) = 0;
    *(HI_U32 *)(vqeConfig + 8) = 0;
    *(HI_U32 *)(vqeConfig + 12) = 0;
    *(HI_U32 *)(vqeConfig + 16) = 0;
    *(HI_U32 *)(vqeConfig + 20) = 0;
    *(HI_U32 *)(vqeConfig + 24) = 0;
    *(HI_U32 *)(vqeConfig + 28) = 0;
    *(HI_U32 *)(vqeConfig + 32) = 0;

    /* Lock VQE state, destroy old UPVQE, create new with zeroed config */
    pthread_mutex_lock(&pVqeState->mutex);

    HI_UPVQE_Destroy(&pCtx->pUpvqeHandle);
    pCtx->pUpvqeHandle = HI_NULL;
    pVqeState->field_0 = 0;

    memcpy_s(newConfig, 316, vqeConfig, 316);
    result = HI_UPVQE_Create(&pCtx->pUpvqeHandle, newConfig);
    if (result != HI_SUCCESS) {
        pthread_mutex_unlock(&pVqeState->mutex);
        pthread_mutex_unlock(&pCtx->mutex);
        HI_TRACE_AI(RE_DBG_LVL,
            "create upvqe failed, ai dev %d chn %d, ret:0x%x\n",
            AiDevId, AiChn, result);
        return HI_ERR_AI_VQE_ERR;
    }

    pVqeState->field_4 = 0;
    pVqeState->field_0 = (HI_U32)(HI_UL)pCtx->pUpvqeHandle;
    pCtx->bVqeEnabled = HI_FALSE;
    pthread_mutex_unlock(&pVqeState->mutex);

    /* Build zeroed VQE debug info */
    memset(dbgInfo, 0, 320);
    memset_s(dbgInfo + 4, 316, 0, 316);
    *(HI_U32 *)(dbgInfo + 44) = 0x00017701;
    dbgInfo[84] = 2;

    mpi_ai_set_vqe_dbg_info(AiDevId, AiChn, dbgInfo);

    pthread_mutex_unlock(&pCtx->mutex);
    return HI_SUCCESS;
}

HI_S32
HI_MPI_AI_SetRecordVqeAttr(AUDIO_DEV AiDevId, AI_CHN AiChn, const AI_RECORDVQE_CONFIG_S *pstVqeConfig)
{ return hi_mpi_ai_set_record_vqe_attr(AiDevId, AiChn, pstVqeConfig); }

HI_S32
HI_MPI_AI_GetRecordVqeAttr(AUDIO_DEV AiDevId, AI_CHN AiChn, AI_RECORDVQE_CONFIG_S *pstVqeConfig)
{ return hi_mpi_ai_get_record_vqe_attr(AiDevId, AiChn, pstVqeConfig); }

HI_S32
HI_MPI_AI_SetTalkVqeAttr(AUDIO_DEV AiDevId, AI_CHN AiChn, AUDIO_DEV AoDevId, AO_CHN AoChn, const AI_TALKVQE_CONFIG_S *pstVqeConfig)
{ return hi_mpi_ai_set_talk_vqe_attr(AiDevId, AiChn, AoDevId, AoChn, pstVqeConfig); }

HI_S32
HI_MPI_AI_GetTalkVqeAttr(AUDIO_DEV AiDevId, AI_CHN AiChn, AI_TALKVQE_CONFIG_S *pstVqeConfig)
{ return hi_mpi_ai_get_talk_vqe_attr(AiDevId, AiChn, pstVqeConfig); }

HI_S32
HI_MPI_AI_EnableVqe(AUDIO_DEV AiDevId, AI_CHN AiChn)
{ return hi_mpi_ai_enable_vqe(AiDevId, AiChn); }

HI_S32
HI_MPI_AI_DisableVqe(AUDIO_DEV AiDevId, AI_CHN AiChn)
{ return hi_mpi_ai_disable_vqe(AiDevId, AiChn); }

HI_S32
HI_MPI_AI_GetFrame(AUDIO_DEV AiDevId, AI_CHN AiChn, AUDIO_FRAME_S *pstFrm, AEC_FRAME_S *pstAecFrm, HI_S32 s32MilliSec)
{
    HI_S32 result, i;
    AI_FRAME_INFO_EX_S stFrameInfo;

    if ( AiDevId != 0 ) {
        HI_TRACE_AI(RE_DBG_LVL, "ai dev %d is invalid\n", AiDevId);
        return HI_ERR_AI_INVALID_DEVID;
    }

    if ( AiChn >= MAX_CHN_COUNT ) {
        HI_TRACE_AI(RE_DBG_LVL, "ai chnid %d is invalid\n", AiChn);
        return HI_ERR_AI_INVALID_CHNID;
    }

    if ( pstFrm == HI_NULL )
        return HI_ERR_AI_NULL_PTR;

    if ( s32MilliSec < -1 ) {
        HI_TRACE_AI(RE_DBG_LVL, "milli_sec(%d) can not be lower than -1.\n", s32MilliSec);
        return HI_ERR_AI_ILLEGAL_PARAM;
    }

    result = ai_check_open(AiChn);
    if ( result != HI_SUCCESS ) return result;

    pthread_mutex_lock(&s_mpi_ai_chn_ctx[AiChn].mutex);

    if (s_mpi_ai_chn_ctx[AiChn].bAecRefFrameEnabled ||
        s_mpi_ai_chn_ctx[AiChn].bVqeEnabled &&
        s_mpi_ai_chn_ctx[AiChn].field_20)
    {
        if ( pstAecFrm == HI_NULL ) {
            pthread_mutex_unlock(&s_mpi_ai_chn_ctx[AiChn].mutex);
            HI_TRACE_AI(RE_DBG_LVL,
                "aec_frm can not be NULL when AEC or AEC refrence frame enable\n");
            return HI_ERR_AI_NULL_PTR;
        }
    }

    if ( !s_mpi_ai_chn_ctx[AiChn].bEnabled ) {
        pthread_mutex_unlock(&s_mpi_ai_chn_ctx[AiChn].mutex);
        HI_TRACE_AI(RE_DBG_LVL, "AI chn %d is not enable\n", AiChn);
        return HI_ERR_AI_NOT_ENABLED;
    }

    pthread_mutex_unlock(&s_mpi_ai_chn_ctx[AiChn].mutex);

    stFrameInfo.u32Timestamp = s32MilliSec;

    result = ioctl(g_ai_fd[AiChn], IOC_AI_GET_FRAME, &stFrameInfo);
    if (result != HI_SUCCESS &&
        (s32MilliSec != -1 || result != HI_ERR_AI_BUF_EMPTY))
        return result;

    pthread_mutex_lock(&s_mpi_ai_chn_ctx[AiChn].mutex);

    for (i = 0; i <= stFrameInfo.stInfo.stAudioFrm.enSoundmode; i++) {
        result = HI_MPI_VB_GetBlockVirAddr(
            stFrameInfo.stInfo.stAudioFrm.u32PoolId[i],
            stFrameInfo.stInfo.stAudioFrm.u64PhyAddr[i],
            (HI_VOID **)&stFrameInfo.stInfo.stAudioFrm.u64VirAddr[i]);
        if ( result != HI_SUCCESS ) goto error;
    }

    memcpy_s(
        pstFrm, sizeof(AUDIO_FRAME_S),
        &stFrameInfo.stInfo.stAudioFrm, sizeof(AUDIO_FRAME_S));

    if ( stFrameInfo.stInfo.stAecFrm.bValid ) {
        for (i = 0; i < MAX_CHN_COUNT; i++) {
            result = HI_MPI_VB_GetBlockVirAddr(
                stFrameInfo.stInfo.stAecFrm.stRefFrame.u32PoolId[i],
                stFrameInfo.stInfo.stAecFrm.stRefFrame.u64PhyAddr[i],
                (HI_VOID **)&stFrameInfo.stInfo.stAecFrm.stRefFrame.u64VirAddr[i]);
            if ( result != HI_SUCCESS ) goto error;
        }

        if ( pstAecFrm != HI_NULL )
            memcpy_s(
                pstAecFrm, sizeof(AEC_FRAME_S),
                &stFrameInfo.stInfo.stAecFrm, sizeof(AEC_FRAME_S));
    }

    if ( pstAecFrm != HI_NULL )
        pstAecFrm->bValid = HI_FALSE;

    s_mpi_ai_chn_ctx[AiChn].u32FrameCount++;
    pthread_mutex_unlock(&s_mpi_ai_chn_ctx[AiChn].mutex);
    return HI_SUCCESS;

    error:
    pthread_mutex_unlock(&s_mpi_ai_chn_ctx[AiChn].mutex);
    ioctl(g_ai_fd[AiChn], IOC_AI_RELEASE_FRAME, &stFrameInfo.stInfo);
    return result;
}

HI_S32
HI_MPI_AI_ReleaseFrame(AUDIO_DEV AiDevId, AI_CHN AiChn, const AUDIO_FRAME_S *pstFrm, const AEC_FRAME_S *pstAecFrm)
{
    HI_S32 result;

    if ( AiDevId != 0 ) {
        HI_TRACE_AI(RE_DBG_LVL, "ai dev %d is invalid\n", AiDevId);
        return HI_ERR_AI_INVALID_DEVID;
    }

    if ( AiChn >= MAX_CHN_COUNT ) {
        HI_TRACE_AI(RE_DBG_LVL, "ai chnid %d is invalid\n", AiChn);
        return HI_ERR_AI_INVALID_CHNID;
    }

    if ( pstFrm == HI_NULL )
        return HI_ERR_AI_NULL_PTR;

    result = ai_check_open(AiChn);
    if ( result != HI_SUCCESS ) return result;

    return mpi_ai_release_frame(AiChn, pstFrm, pstAecFrm);
}

HI_S32
HI_MPI_AI_SetChnParam(AUDIO_DEV AiDevId, AI_CHN AiChn, const AI_CHN_PARAM_S *pstChnParam)
{
    HI_S32 result;

    if ( AiDevId != 0 ) {
        HI_TRACE_AI(RE_DBG_LVL, "ai dev %d is invalid\n", AiDevId);
        return HI_ERR_AI_INVALID_DEVID;
    }

    if ( AiChn >= MAX_CHN_COUNT ) {
        HI_TRACE_AI(RE_DBG_LVL, "ai chnid %d is invalid\n", AiChn);
        return HI_ERR_AI_INVALID_CHNID;
    }

    if ( pstChnParam == HI_NULL )
        return HI_ERR_AI_NULL_PTR;

    result = ai_check_open(AiChn);
    if ( result != HI_SUCCESS ) return result;

    return ioctl(g_ai_fd[AiChn], IOC_AI_SET_CHN_PARAM, pstChnParam);
}

HI_S32
HI_MPI_AI_GetChnParam(AUDIO_DEV AiDevId, AI_CHN AiChn, AI_CHN_PARAM_S *pstChnParam)
{
    HI_S32 result;

    if ( AiDevId != 0 ) {
        HI_TRACE_AI(RE_DBG_LVL, "ai dev %d is invalid\n", AiDevId);
        return HI_ERR_AI_INVALID_DEVID;
    }

    if ( AiChn >= MAX_CHN_COUNT ) {
        HI_TRACE_AI(RE_DBG_LVL, "ai chnid %d is invalid\n", AiChn);
        return HI_ERR_AI_INVALID_CHNID;
    }

    if ( pstChnParam == HI_NULL )
        return HI_ERR_AI_NULL_PTR;

    result = ai_check_open(AiChn);
    if ( result != HI_SUCCESS ) return result;

    return ioctl(g_ai_fd[AiChn], IOC_AI_GET_CHN_PARAM, pstChnParam);
}

HI_S32
HI_MPI_AI_SetTrackMode(AUDIO_DEV AiDevId, AUDIO_TRACK_MODE_E enTrackMode)
{
    HI_S32 result;

    if ( AiDevId != 0 ) {
        HI_TRACE_AI(RE_DBG_LVL, "ai dev %d is invalid\n", AiDevId);
        return HI_ERR_AI_INVALID_DEVID;
    }

    result = ai_check_open(0);
    if ( result != HI_SUCCESS ) return result;

    if ( enTrackMode >= AUDIO_TRACK_BUTT ) {
        HI_TRACE_AI(RE_DBG_LVL, "illegal param: track_mode(%d)!\n", enTrackMode);
        return HI_ERR_AI_ILLEGAL_PARAM;
    }

    return ioctl(g_ai_fd[0], IOC_AI_SET_TRACK_MODE, &enTrackMode);
}

HI_S32
HI_MPI_AI_GetTrackMode(AUDIO_DEV AiDevId, AUDIO_TRACK_MODE_E *penTrackMode)
{
    HI_S32 result;

    if ( AiDevId != 0 ) {
        HI_TRACE_AI(RE_DBG_LVL, "ai dev %d is invalid\n", AiDevId);
        return HI_ERR_AI_INVALID_DEVID;
    }

    if ( penTrackMode == HI_NULL )
        return HI_ERR_AI_NULL_PTR;

    result = ai_check_open(0);
    if ( result != HI_SUCCESS ) return result;

    return ioctl(g_ai_fd[0], IOC_AI_GET_TRACK_MODE, penTrackMode);
}

HI_S32
HI_MPI_AI_SetClkDir(AUDIO_DEV AiDevId, AUDIO_CLKSEL_E enClksel)
{
    HI_S32 result;

    if ( AiDevId != 0 ) {
        HI_TRACE_AI(RE_DBG_LVL, "ai dev %d is invalid\n", AiDevId);
        return HI_ERR_AI_INVALID_DEVID;
    }

    result = ai_check_open(0);
    if ( result != HI_SUCCESS ) return result;

    if ( enClksel >= AUDIO_CLKSEL_BUTT ) {
        HI_TRACE_AI(RE_DBG_LVL, "illegal param: clk_dir(%d)!\n", enClksel);
        return HI_ERR_AI_ILLEGAL_PARAM;
    }

    return ioctl(g_ai_fd[0], IOC_AI_SET_CLK_DIR, &enClksel);
}

HI_S32
HI_MPI_AI_GetClkDir(AUDIO_DEV AiDevId, AUDIO_CLKSEL_E *penClksel)
{
    HI_S32 result;

    if ( AiDevId != 0 ) {
        HI_TRACE_AI(RE_DBG_LVL, "ai dev %d is invalid\n", AiDevId);
        return HI_ERR_AI_INVALID_DEVID;
    }
    
    if ( penClksel == HI_NULL )
        return HI_ERR_AI_NULL_PTR;

    result = ai_check_open(0);
    if ( result != HI_SUCCESS ) return result;

    return ioctl(g_ai_fd[0], IOC_AI_GET_CLK_DIR, penClksel);
}

HI_S32
HI_MPI_AI_SaveFile(AUDIO_DEV AiDevId, AI_CHN AiChn, const AUDIO_SAVE_FILE_INFO_S *pstSaveFileInfo)
{
    HI_S32 result;

    if ( AiDevId != 0 ) {
        HI_TRACE_AI(RE_DBG_LVL, "ai dev %d is invalid\n", AiDevId);
        return HI_ERR_AI_INVALID_DEVID;
    }

    if ( AiChn >= MAX_CHN_COUNT ) {
        HI_TRACE_AI(RE_DBG_LVL, "ai chnid %d is invalid\n", AiChn);
        return HI_ERR_AI_INVALID_CHNID;
    }

    if ( pstSaveFileInfo == HI_NULL )
        return HI_ERR_AI_NULL_PTR;

    result = ai_check_open(AiChn);
    if ( result != HI_SUCCESS ) return result;

    pthread_mutex_lock(&s_mpi_ai_chn_ctx[AiChn].mutex);
    result = ioctl(g_ai_fd[AiChn], IOC_AI_SAVE_FILE, pstSaveFileInfo);
    pthread_mutex_unlock(&s_mpi_ai_chn_ctx[AiChn].mutex);
    return result;
}

HI_S32
HI_MPI_AI_QueryFileStatus(AUDIO_DEV AiDevId, AI_CHN AiChn, AUDIO_FILE_STATUS_S *pstFileStatus)
{ return hi_mpi_ai_query_file_status(AiDevId, AiChn, pstFileStatus); }

HI_S32
HI_MPI_AI_ClrPubAttr(AUDIO_DEV AiDevId)
{
    HI_S32 result;

    if ( AiDevId != 0 ) {
        HI_TRACE_AI(RE_DBG_LVL, "ai dev %d is invalid\n", AiDevId);
        return HI_ERR_AI_INVALID_DEVID;
    }

    result = ai_check_open(0);
    if ( result != HI_SUCCESS ) return result;

    return ioctl(g_ai_fd[0], IOC_AI_CLR_PUB_ATTR);
}

HI_S32
HI_MPI_AI_GetFd(AUDIO_DEV AiDevId, AI_CHN AiChn)
{
    HI_S32 result;

    if ( AiDevId != 0 ) {
        HI_TRACE_AI(RE_DBG_LVL, "ai dev %d is invalid\n", AiDevId);
        return HI_ERR_AI_INVALID_DEVID;
    }

    if ( AiChn >= MAX_CHN_COUNT ) {
        HI_TRACE_AI(RE_DBG_LVL, "ai chnid %d is invalid\n", AiChn);
        return HI_ERR_AI_INVALID_CHNID;
    }

    result = ai_check_open(AiChn);
    if ( result != HI_SUCCESS ) return result;

    return g_ai_fd[AiChn];
}

HI_S32
HI_MPI_AI_SetVqeVolume(AUDIO_DEV AiDevId, AO_CHN AiChn, HI_S32 s32VolumeDb)
{
    HI_S32 result;

    if ( AiDevId != 0 ) {
        HI_TRACE_AI(RE_DBG_LVL, "ai dev %d is invalid\n", AiDevId);
        return HI_ERR_AI_INVALID_DEVID;
    }

    if ( AiChn >= MAX_CHN_COUNT ) {
        HI_TRACE_AI(RE_DBG_LVL, "ai chnid %d is invalid\n", AiChn);
        return HI_ERR_AI_INVALID_CHNID;
    }

    result = ai_check_open(AiChn);
    if ( result != HI_SUCCESS ) return result;

    if ( (HI_U32)(s32VolumeDb + 20) > 0x1E ) {
        HI_TRACE_AI(RE_DBG_LVL, "volume_db %d is illegal!\n", s32VolumeDb);
        return HI_ERR_AI_ILLEGAL_PARAM;
    }

    pthread_mutex_lock(&s_mpi_ai_chn_ctx[AiChn].mutex);

    if ( !s_mpi_ai_chn_ctx[AiChn].bVqeEnabled ) {
        pthread_mutex_unlock(&s_mpi_ai_chn_ctx[AiChn].mutex);
        HI_TRACE_AI(RE_DBG_LVL, "AI chn %d is not config vqe!\n", AiChn);
        return HI_ERR_AI_NOT_PERM;
    }

    if ( s_mpi_ai_chn_ctx[AiChn].pUpvqeHandle == HI_NULL ) {
        pthread_mutex_unlock(&s_mpi_ai_chn_ctx[AiChn].mutex);
        HI_TRACE_AI(RE_DBG_LVL, "AI chn %d is not config vqe!\n", AiChn);
        return HI_ERR_AI_NOT_PERM;
    }

    result = HI_UPVQE_SetVolume(s_mpi_ai_chn_ctx[AiChn].pUpvqeHandle, s32VolumeDb);
    if ( result != HI_SUCCESS ) {
        pthread_mutex_unlock(&s_mpi_ai_chn_ctx[AiChn].mutex);
        HI_TRACE_AI(RE_DBG_LVL, "AI chn %d set vqe volume failed!\n", AiChn);
        return HI_ERR_AI_VQE_ERR;
    }

    result = ai_check_open(AiChn);
    if ( result != HI_SUCCESS ) {
        pthread_mutex_unlock(&s_mpi_ai_chn_ctx[AiChn].mutex);
        return result;
    }

    result = ioctl(g_ai_fd[AiChn], IOC_AI_SET_VQE_VOLUME, &s32VolumeDb);
    pthread_mutex_unlock(&s_mpi_ai_chn_ctx[AiChn].mutex);
    return result;
}

HI_S32
HI_MPI_AI_GetVqeVolume(AUDIO_DEV AiDevId, AO_CHN AiChn, HI_S32 *ps32VolumeDb)
{
    HI_S32 result;

    if ( AiDevId != 0 ) {
        HI_TRACE_AI(RE_DBG_LVL, "ai dev %d is invalid\n", AiDevId);
        return HI_ERR_AI_INVALID_DEVID;
    }

    if ( AiChn >= MAX_CHN_COUNT ) {
        HI_TRACE_AI(RE_DBG_LVL, "ai chnid %d is invalid\n", AiChn);
        return HI_ERR_AI_INVALID_CHNID;
    }

    if ( ps32VolumeDb == HI_NULL )
        return HI_ERR_AI_NULL_PTR;

    result = ai_check_open(AiChn);
    if ( result != HI_SUCCESS ) return result;

    pthread_mutex_lock(&s_mpi_ai_chn_ctx[AiChn].mutex);

    if ( !s_mpi_ai_chn_ctx[AiChn].bVqeEnabled ) {
        pthread_mutex_unlock(&s_mpi_ai_chn_ctx[AiChn].mutex);
        HI_TRACE_AI(RE_DBG_LVL, "AI chn %d is not config vqe!\n", AiChn);
        return HI_ERR_AI_NOT_PERM;
    }

    if ( s_mpi_ai_chn_ctx[AiChn].pUpvqeHandle == HI_NULL ) {
        pthread_mutex_unlock(&s_mpi_ai_chn_ctx[AiChn].mutex);
        HI_TRACE_AI(RE_DBG_LVL, "AI chn %d is not config vqe!\n", AiChn);
        return HI_ERR_AI_NOT_PERM;
    }

    result = HI_UPVQE_GetVolume(s_mpi_ai_chn_ctx[AiChn].pUpvqeHandle, ps32VolumeDb);
    if ( result != HI_SUCCESS ) {
        pthread_mutex_unlock(&s_mpi_ai_chn_ctx[AiChn].mutex);
        HI_TRACE_AI(RE_DBG_LVL, "AI chn %d get vqe volume failed!\n", AiChn);
        return HI_ERR_AI_VQE_ERR;
    }

    pthread_mutex_unlock(&s_mpi_ai_chn_ctx[AiChn].mutex);
    return HI_SUCCESS;
}

HI_S32
HI_MPI_AI_SetChnAttr(AUDIO_DEV AiDevId, AI_CHN AiChn, const AI_CHN_PARAM_S *pstChnParam)
{
    /* From vendor mpi_ai_adapt.o — delegates to SetChnParam with additional checks */
    return HI_MPI_AI_SetChnParam(AiDevId, AiChn, pstChnParam);
}

HI_S32
HI_MPI_AI_GetChnAttr(AUDIO_DEV AiDevId, AI_CHN AiChn, AI_CHN_PARAM_S *pstChnParam)
{
    /* From vendor mpi_ai_adapt.o — delegates to GetChnParam */
    return HI_MPI_AI_GetChnParam(AiDevId, AiChn, pstChnParam);
}

HI_S32
HI_MPI_AI_SetTalkVqeV2Attr(AUDIO_DEV AiDevId, AI_CHN AiChn, AUDIO_DEV AoDevId, AO_CHN AoChn, const AI_TALKVQE_CONFIG_S *pstVqeConfig)
{
    return hi_mpi_ai_set_talk_vqe_attr(AiDevId, AiChn, AoDevId, AoChn, pstVqeConfig);
}

HI_S32
HI_MPI_AI_GetTalkVqeV2Attr(AUDIO_DEV AiDevId, AI_CHN AiChn, AI_TALKVQE_CONFIG_S *pstVqeConfig)
{
    return hi_mpi_ai_get_talk_vqe_attr(AiDevId, AiChn, pstVqeConfig);
}

HI_S32
HI_MPI_AI_EnableAecRefFrame(AUDIO_DEV AiDevId, AI_CHN AiChn, AUDIO_DEV AoDevId, AO_CHN AoChn)
{
    HI_S32 result;
    AI_DEV_ID_S stDevId;
    AIO_ATTR_S stAttr;

    memset(&stAttr, 0, sizeof(AIO_ATTR_S));

    if ( AiDevId != 0 ) {
        HI_TRACE_AI(RE_DBG_LVL, "ai dev %d is invalid\n", AiDevId);
        return HI_ERR_AI_INVALID_DEVID;
    }

    if ( AiChn >= MAX_CHN_COUNT ) {
        HI_TRACE_AI(RE_DBG_LVL, "ai chnid %d is invalid\n", AiChn);
        return HI_ERR_AI_INVALID_CHNID;
    }

    result = ai_check_open(AiChn);
    if ( result != HI_SUCCESS ) return result;

    pthread_mutex_lock(&s_mpi_ai_chn_ctx[AiChn].mutex);

    if (s_mpi_ai_chn_ctx[AiChn].bVqeEnabled &&
        s_mpi_ai_chn_ctx[AiChn].field_20 == HI_TRUE)
    {
        pthread_mutex_unlock(&s_mpi_ai_chn_ctx[AiChn].mutex);
        HI_TRACE_AI(RE_DBG_LVL,
            "this mpi do not support when aec open. chn:%d\n", AiChn);
        return HI_ERR_AI_NOT_SUPPORT;
    }

    if (ai_check_open(0) != HI_SUCCESS ||
        ioctl(g_ai_fd[0], IOC_AI_GET_PUB_ATTR, &stAttr) != HI_SUCCESS)
    {
        pthread_mutex_unlock(&s_mpi_ai_chn_ctx[AiChn].mutex);
        return HI_ERR_AI_NOT_CONFIG;
    }

    if ( stAttr.enSoundmode == AUDIO_SOUND_MODE_STEREO ) {
        pthread_mutex_unlock(&s_mpi_ai_chn_ctx[AiChn].mutex);
        HI_TRACE_AI(RE_DBG_LVL, "aec_ref_frame don't support stereo!\n");
        return HI_ERR_AI_ILLEGAL_PARAM;
    }

    stDevId.AiDevId = AoDevId;
    stDevId.AiChn   = AoChn;

    result = ioctl(g_ai_fd[AiChn], IOC_AI_AEC_INIT, &stDevId);
    if ( result != HI_SUCCESS ) {
        pthread_mutex_unlock(&s_mpi_ai_chn_ctx[AiChn].mutex);
        HI_TRACE_AI(RE_DBG_LVL, "aec init fail, ai chn:%d\n", AiChn);
        return result;
    }

    s_mpi_ai_chn_ctx[AiChn].bAecRefFrameEnabled = HI_TRUE;
    pthread_mutex_unlock(&s_mpi_ai_chn_ctx[AiChn].mutex);

    return result;
}

HI_S32
HI_MPI_AI_DisableAecRefFrame(AUDIO_DEV AiDevId, AI_CHN AiChn)
{
    HI_S32 result;

    if ( AiDevId != 0 ) {
        HI_TRACE_AI(RE_DBG_LVL, "ai dev %d is invalid\n", AiDevId);
        return HI_ERR_AI_INVALID_DEVID;
    }

    if ( AiChn >= MAX_CHN_COUNT ) {
        HI_TRACE_AI(RE_DBG_LVL, "ai chnid %d is invalid\n", AiChn);
        return HI_ERR_AI_INVALID_CHNID;
    }

    result = ai_check_open(AiChn);
    if ( result != HI_SUCCESS ) return result;

    pthread_mutex_lock(&s_mpi_ai_chn_ctx[AiChn].mutex);

    if ( s_mpi_ai_chn_ctx[AiChn].bAecRefFrameEnabled ) {
        ioctl(g_ai_fd[AiChn], IOC_AI_DISABLE_AEC_REF_FRAME);
        s_mpi_ai_chn_ctx[AiChn].bAecRefFrameEnabled = HI_FALSE;
    }

    pthread_mutex_unlock(&s_mpi_ai_chn_ctx[AiChn].mutex);

    return HI_SUCCESS;
}
