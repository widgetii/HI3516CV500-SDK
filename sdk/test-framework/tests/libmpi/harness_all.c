/*
 * harness_all.c -- Combined test harness for ALL HI_MPI_* functions in libmpi.
 *
 * Exercises every public MPI function with zeroed/default parameters and
 * logs return values.  Run with both vendor and our libmpi.a to compare traces.
 *
 * Modules covered:
 *   SYS, LOG, VB, VI, VO, VPSS, VENC, VDEC, RGN, GDC, VGS, SNAP,
 *   AUDIO, AI, AO, AENC, ADEC
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "ioctl_mock.h"

/* MPI / comm headers */
#include "hi_type.h"
#include "hi_common.h"
#include "hi_comm_sys.h"
#include "hi_comm_vb.h"
#include "hi_comm_vi.h"
#include "hi_comm_vo.h"
#include "hi_comm_vpss.h"
#include "hi_comm_venc.h"
#include "hi_comm_vdec.h"
#include "hi_comm_region.h"
#include "hi_comm_gdc.h"
#include "hi_comm_vgs.h"
#include "hi_comm_snap.h"
#include "hi_comm_video.h"
#include "hi_comm_dis.h"
#include "hi_comm_aio.h"
#include "hi_comm_aenc.h"
#include "hi_comm_adec.h"

#include "mpi_sys.h"
#include "mpi_vb.h"
#include "mpi_vi.h"
#include "mpi_vo.h"
#include "mpi_vpss.h"
#include "mpi_venc.h"
#include "mpi_vdec.h"
#include "mpi_region.h"
#include "mpi_gdc.h"
#include "mpi_vgs.h"
#include "mpi_snap.h"
#include "mpi_audio.h"

/* ========================================================================
 *  SYS module  (42 functions + 5 LOG functions = 47)
 * ======================================================================== */

static void test_sys_basic(FILE *trace) {
    HI_S32 ret;
    fprintf(trace, "--- SYS basic ---\n");

    ret = HI_MPI_SYS_Init();
    fprintf(trace, "SYS_Init = 0x%x\n", ret);

    ret = HI_MPI_SYS_Exit();
    fprintf(trace, "SYS_Exit = 0x%x\n", ret);

    {
        MPP_SYS_CONFIG_S cfg;
        memset(&cfg, 0, sizeof(cfg));
        ret = HI_MPI_SYS_SetConfig(&cfg);
        fprintf(trace, "SYS_SetConfig = 0x%x\n", ret);
        ret = HI_MPI_SYS_GetConfig(&cfg);
        fprintf(trace, "SYS_GetConfig = 0x%x\n", ret);
    }

    {
        MPP_CHN_S src, dst;
        memset(&src, 0, sizeof(src));
        memset(&dst, 0, sizeof(dst));
        src.enModId = HI_ID_VI; src.s32DevId = 0; src.s32ChnId = 0;
        dst.enModId = HI_ID_VPSS; dst.s32DevId = 0; dst.s32ChnId = 0;

        ret = HI_MPI_SYS_Bind(&src, &dst);
        fprintf(trace, "SYS_Bind = 0x%x\n", ret);
        ret = HI_MPI_SYS_UnBind(&src, &dst);
        fprintf(trace, "SYS_UnBind = 0x%x\n", ret);

        MPP_CHN_S srcOut;
        memset(&srcOut, 0, sizeof(srcOut));
        ret = HI_MPI_SYS_GetBindbyDest(&dst, &srcOut);
        fprintf(trace, "SYS_GetBindbyDest = 0x%x\n", ret);

        MPP_BIND_DEST_S bindDest;
        memset(&bindDest, 0, sizeof(bindDest));
        ret = HI_MPI_SYS_GetBindbySrc(&src, &bindDest);
        fprintf(trace, "SYS_GetBindbySrc = 0x%x\n", ret);
    }

    {
        MPP_VERSION_S ver;
        memset(&ver, 0, sizeof(ver));
        ret = HI_MPI_SYS_GetVersion(&ver);
        fprintf(trace, "SYS_GetVersion = 0x%x\n", ret);
    }

    {
        HI_U32 chipId = 0;
        ret = HI_MPI_SYS_GetChipId(&chipId);
        fprintf(trace, "SYS_GetChipId = 0x%x\n", ret);
    }

    {
        HI_UNIQUE_ID_S uid;
        memset(&uid, 0, sizeof(uid));
        ret = HI_MPI_SYS_GetUniqueId(&uid);
        fprintf(trace, "SYS_GetUniqueId = 0x%x\n", ret);
    }

    {
        HI_U32 customCode = 0;
        ret = HI_MPI_SYS_GetCustomCode(&customCode);
        fprintf(trace, "SYS_GetCustomCode = 0x%x\n", ret);
    }

    {
        HI_U64 pts = 0;
        ret = HI_MPI_SYS_GetCurPTS(&pts);
        fprintf(trace, "SYS_GetCurPTS = 0x%x\n", ret);

        ret = HI_MPI_SYS_InitPTSBase(1000000);
        fprintf(trace, "SYS_InitPTSBase = 0x%x\n", ret);

        ret = HI_MPI_SYS_SyncPTS(500000);
        fprintf(trace, "SYS_SyncPTS = 0x%x\n", ret);
    }

    {
        HI_U64 phys = 0;
        HI_VOID *virt = NULL;
        ret = HI_MPI_SYS_MmzAlloc(&phys, &virt, "test", NULL, 4096);
        fprintf(trace, "SYS_MmzAlloc = 0x%x\n", ret);

        ret = HI_MPI_SYS_MmzAlloc_Cached(&phys, &virt, "test", NULL, 4096);
        fprintf(trace, "SYS_MmzAlloc_Cached = 0x%x\n", ret);

        ret = HI_MPI_SYS_MmzFree(0, NULL);
        fprintf(trace, "SYS_MmzFree = 0x%x\n", ret);

        ret = HI_MPI_SYS_MmzFlushCache(0, NULL, 0);
        fprintf(trace, "SYS_MmzFlushCache = 0x%x\n", ret);
    }

    {
        HI_VOID *p;
        p = HI_MPI_SYS_Mmap(0, 4096);
        fprintf(trace, "SYS_Mmap = %p\n", p);

        p = HI_MPI_SYS_MmapCache(0, 4096);
        fprintf(trace, "SYS_MmapCache = %p\n", p);

        ret = HI_MPI_SYS_Munmap(NULL, 4096);
        fprintf(trace, "SYS_Munmap = 0x%x\n", ret);

        ret = HI_MPI_SYS_MflushCache(0, NULL, 0);
        fprintf(trace, "SYS_MflushCache = 0x%x\n", ret);
    }

    {
        MPP_CHN_S chn;
        memset(&chn, 0, sizeof(chn));
        HI_CHAR mmzName[16] = {0};

        ret = HI_MPI_SYS_SetMemConfig(&chn, mmzName);
        fprintf(trace, "SYS_SetMemConfig = 0x%x\n", ret);
        ret = HI_MPI_SYS_GetMemConfig(&chn, mmzName);
        fprintf(trace, "SYS_GetMemConfig = 0x%x\n", ret);
    }

    ret = HI_MPI_SYS_CloseFd();
    fprintf(trace, "SYS_CloseFd = 0x%x\n", ret);

    {
        SYS_VIRMEM_INFO_S info;
        memset(&info, 0, sizeof(info));
        int dummy = 0;
        ret = HI_MPI_SYS_GetVirMemInfo(&dummy, &info);
        fprintf(trace, "SYS_GetVirMemInfo = 0x%x\n", ret);
    }

    {
        SCALE_RANGE_S range;
        SCALE_COEFF_LEVEL_S level;
        memset(&range, 0, sizeof(range));
        memset(&level, 0, sizeof(level));
        ret = HI_MPI_SYS_SetScaleCoefLevel(&range, &level);
        fprintf(trace, "SYS_SetScaleCoefLevel = 0x%x\n", ret);
        ret = HI_MPI_SYS_GetScaleCoefLevel(&range, &level);
        fprintf(trace, "SYS_GetScaleCoefLevel = 0x%x\n", ret);
    }

    {
        ret = HI_MPI_SYS_SetTimeZone(0);
        fprintf(trace, "SYS_SetTimeZone = 0x%x\n", ret);
        HI_S32 tz = 0;
        ret = HI_MPI_SYS_GetTimeZone(&tz);
        fprintf(trace, "SYS_GetTimeZone = 0x%x\n", ret);
    }

    {
        GPS_INFO_S gps;
        memset(&gps, 0, sizeof(gps));
        ret = HI_MPI_SYS_SetGPSInfo(&gps);
        fprintf(trace, "SYS_SetGPSInfo = 0x%x\n", ret);
        ret = HI_MPI_SYS_GetGPSInfo(&gps);
        fprintf(trace, "SYS_GetGPSInfo = 0x%x\n", ret);
    }

    {
        ret = HI_MPI_SYS_SetTuningConnect(0);
        fprintf(trace, "SYS_SetTuningConnect = 0x%x\n", ret);
        HI_S32 conn = 0;
        ret = HI_MPI_SYS_GetTuningConnect(&conn);
        fprintf(trace, "SYS_GetTuningConnect = 0x%x\n", ret);
    }

    {
        VI_VPSS_MODE_S mode;
        memset(&mode, 0, sizeof(mode));
        ret = HI_MPI_SYS_SetVIVPSSMode(&mode);
        fprintf(trace, "SYS_SetVIVPSSMode = 0x%x\n", ret);
        ret = HI_MPI_SYS_GetVIVPSSMode(&mode);
        fprintf(trace, "SYS_GetVIVPSSMode = 0x%x\n", ret);
    }

    {
        VPSS_VENC_WRAP_PARAM_S wp;
        memset(&wp, 0, sizeof(wp));
        HI_U32 bufLine = 0;
        ret = HI_MPI_SYS_GetVPSSVENCWrapBufferLine(&wp, &bufLine);
        fprintf(trace, "SYS_GetVPSSVENCWrapBufferLine = 0x%x\n", ret);
    }

    {
        RAW_FRAME_COMPRESS_PARAM_S cp;
        memset(&cp, 0, sizeof(cp));
        ret = HI_MPI_SYS_SetRawFrameCompressParam(&cp);
        fprintf(trace, "SYS_SetRawFrameCompressParam = 0x%x\n", ret);
        ret = HI_MPI_SYS_GetRawFrameCompressParam(&cp);
        fprintf(trace, "SYS_GetRawFrameCompressParam = 0x%x\n", ret);
    }

    /* LOG functions */
    {
        LOG_LEVEL_CONF_S logConf;
        memset(&logConf, 0, sizeof(logConf));
        ret = HI_MPI_LOG_SetLevelConf(&logConf);
        fprintf(trace, "LOG_SetLevelConf = 0x%x\n", ret);
        ret = HI_MPI_LOG_GetLevelConf(&logConf);
        fprintf(trace, "LOG_GetLevelConf = 0x%x\n", ret);
    }

    {
        ret = HI_MPI_LOG_SetWaitFlag(HI_FALSE);
        fprintf(trace, "LOG_SetWaitFlag = 0x%x\n", ret);
    }

    {
        HI_CHAR buf[256];
        memset(buf, 0, sizeof(buf));
        ret = HI_MPI_LOG_Read(buf, sizeof(buf));
        fprintf(trace, "LOG_Read = 0x%x\n", ret);
    }

    HI_MPI_LOG_Close();
    fprintf(trace, "LOG_Close = (void)\n");
}

static void test_sys_null_ptr(FILE *trace) {
    HI_S32 ret;
    fprintf(trace, "--- SYS null_ptr ---\n");

    ret = HI_MPI_SYS_SetConfig(NULL);
    fprintf(trace, "SYS_SetConfig(NULL) = 0x%x\n", ret);
    ret = HI_MPI_SYS_GetConfig(NULL);
    fprintf(trace, "SYS_GetConfig(NULL) = 0x%x\n", ret);

    ret = HI_MPI_SYS_Bind(NULL, NULL);
    fprintf(trace, "SYS_Bind(NULL,NULL) = 0x%x\n", ret);
    ret = HI_MPI_SYS_UnBind(NULL, NULL);
    fprintf(trace, "SYS_UnBind(NULL,NULL) = 0x%x\n", ret);
    ret = HI_MPI_SYS_GetBindbyDest(NULL, NULL);
    fprintf(trace, "SYS_GetBindbyDest(NULL,NULL) = 0x%x\n", ret);
    ret = HI_MPI_SYS_GetBindbySrc(NULL, NULL);
    fprintf(trace, "SYS_GetBindbySrc(NULL,NULL) = 0x%x\n", ret);

    ret = HI_MPI_SYS_GetVersion(NULL);
    fprintf(trace, "SYS_GetVersion(NULL) = 0x%x\n", ret);
    ret = HI_MPI_SYS_GetChipId(NULL);
    fprintf(trace, "SYS_GetChipId(NULL) = 0x%x\n", ret);
    ret = HI_MPI_SYS_GetUniqueId(NULL);
    fprintf(trace, "SYS_GetUniqueId(NULL) = 0x%x\n", ret);
    ret = HI_MPI_SYS_GetCustomCode(NULL);
    fprintf(trace, "SYS_GetCustomCode(NULL) = 0x%x\n", ret);
    ret = HI_MPI_SYS_GetCurPTS(NULL);
    fprintf(trace, "SYS_GetCurPTS(NULL) = 0x%x\n", ret);

    ret = HI_MPI_SYS_MmzAlloc(NULL, NULL, NULL, NULL, 0);
    fprintf(trace, "SYS_MmzAlloc(NULL) = 0x%x\n", ret);
    ret = HI_MPI_SYS_MmzAlloc_Cached(NULL, NULL, NULL, NULL, 0);
    fprintf(trace, "SYS_MmzAlloc_Cached(NULL) = 0x%x\n", ret);

    ret = HI_MPI_SYS_SetMemConfig(NULL, NULL);
    fprintf(trace, "SYS_SetMemConfig(NULL) = 0x%x\n", ret);
    ret = HI_MPI_SYS_GetMemConfig(NULL, NULL);
    fprintf(trace, "SYS_GetMemConfig(NULL) = 0x%x\n", ret);

    ret = HI_MPI_SYS_GetVirMemInfo(NULL, NULL);
    fprintf(trace, "SYS_GetVirMemInfo(NULL) = 0x%x\n", ret);

    ret = HI_MPI_SYS_SetScaleCoefLevel(NULL, NULL);
    fprintf(trace, "SYS_SetScaleCoefLevel(NULL) = 0x%x\n", ret);
    ret = HI_MPI_SYS_GetScaleCoefLevel(NULL, NULL);
    fprintf(trace, "SYS_GetScaleCoefLevel(NULL) = 0x%x\n", ret);

    ret = HI_MPI_SYS_GetTimeZone(NULL);
    fprintf(trace, "SYS_GetTimeZone(NULL) = 0x%x\n", ret);

    ret = HI_MPI_SYS_SetGPSInfo(NULL);
    fprintf(trace, "SYS_SetGPSInfo(NULL) = 0x%x\n", ret);
    ret = HI_MPI_SYS_GetGPSInfo(NULL);
    fprintf(trace, "SYS_GetGPSInfo(NULL) = 0x%x\n", ret);

    ret = HI_MPI_SYS_GetTuningConnect(NULL);
    fprintf(trace, "SYS_GetTuningConnect(NULL) = 0x%x\n", ret);

    ret = HI_MPI_SYS_SetVIVPSSMode(NULL);
    fprintf(trace, "SYS_SetVIVPSSMode(NULL) = 0x%x\n", ret);
    ret = HI_MPI_SYS_GetVIVPSSMode(NULL);
    fprintf(trace, "SYS_GetVIVPSSMode(NULL) = 0x%x\n", ret);

    ret = HI_MPI_SYS_GetVPSSVENCWrapBufferLine(NULL, NULL);
    fprintf(trace, "SYS_GetVPSSVENCWrapBufferLine(NULL) = 0x%x\n", ret);

    ret = HI_MPI_SYS_SetRawFrameCompressParam(NULL);
    fprintf(trace, "SYS_SetRawFrameCompressParam(NULL) = 0x%x\n", ret);
    ret = HI_MPI_SYS_GetRawFrameCompressParam(NULL);
    fprintf(trace, "SYS_GetRawFrameCompressParam(NULL) = 0x%x\n", ret);

    ret = HI_MPI_LOG_SetLevelConf(NULL);
    fprintf(trace, "LOG_SetLevelConf(NULL) = 0x%x\n", ret);
    ret = HI_MPI_LOG_GetLevelConf(NULL);
    fprintf(trace, "LOG_GetLevelConf(NULL) = 0x%x\n", ret);

    ret = HI_MPI_LOG_Read(NULL, 0);
    fprintf(trace, "LOG_Read(NULL) = 0x%x\n", ret);
}

/* ========================================================================
 *  VB module  (21 functions)
 * ======================================================================== */

static void test_vb_basic(FILE *trace) {
    HI_S32 ret;
    fprintf(trace, "--- VB basic ---\n");

    ret = HI_MPI_VB_Init();
    fprintf(trace, "VB_Init = 0x%x\n", ret);
    ret = HI_MPI_VB_Exit();
    fprintf(trace, "VB_Exit = 0x%x\n", ret);

    {
        VB_CONFIG_S cfg;
        memset(&cfg, 0, sizeof(cfg));
        ret = HI_MPI_VB_SetConfig(&cfg);
        fprintf(trace, "VB_SetConfig = 0x%x\n", ret);
        ret = HI_MPI_VB_GetConfig(&cfg);
        fprintf(trace, "VB_GetConfig = 0x%x\n", ret);
    }

    {
        VB_POOL_CONFIG_S poolCfg;
        memset(&poolCfg, 0, sizeof(poolCfg));
        VB_POOL pool = HI_MPI_VB_CreatePool(&poolCfg);
        fprintf(trace, "VB_CreatePool = 0x%x\n", pool);

        ret = HI_MPI_VB_DestroyPool(0);
        fprintf(trace, "VB_DestroyPool = 0x%x\n", ret);
    }

    {
        VB_BLK blk = HI_MPI_VB_GetBlock(0, 4096, NULL);
        fprintf(trace, "VB_GetBlock = 0x%x\n", blk);

        ret = HI_MPI_VB_ReleaseBlock(0);
        fprintf(trace, "VB_ReleaseBlock = 0x%x\n", ret);
    }

    {
        VB_BLK blk = HI_MPI_VB_PhysAddr2Handle(0);
        fprintf(trace, "VB_PhysAddr2Handle = 0x%x\n", blk);

        HI_U64 addr = HI_MPI_VB_Handle2PhysAddr(0);
        fprintf(trace, "VB_Handle2PhysAddr = 0x%llx\n", (unsigned long long)addr);

        VB_POOL pid = HI_MPI_VB_Handle2PoolId(0);
        fprintf(trace, "VB_Handle2PoolId = 0x%x\n", pid);
    }

    {
        ret = HI_MPI_VB_InquireUserCnt(0);
        fprintf(trace, "VB_InquireUserCnt = 0x%x\n", ret);
    }

    {
        VIDEO_SUPPLEMENT_S supp;
        memset(&supp, 0, sizeof(supp));
        ret = HI_MPI_VB_GetSupplementAddr(0, &supp);
        fprintf(trace, "VB_GetSupplementAddr = 0x%x\n", ret);

        VB_SUPPLEMENT_CONFIG_S suppCfg;
        memset(&suppCfg, 0, sizeof(suppCfg));
        ret = HI_MPI_VB_SetSupplementConfig(&suppCfg);
        fprintf(trace, "VB_SetSupplementConfig = 0x%x\n", ret);
        ret = HI_MPI_VB_GetSupplementConfig(&suppCfg);
        fprintf(trace, "VB_GetSupplementConfig = 0x%x\n", ret);
    }

    {
        ret = HI_MPI_VB_MmapPool(0);
        fprintf(trace, "VB_MmapPool = 0x%x\n", ret);
        ret = HI_MPI_VB_MunmapPool(0);
        fprintf(trace, "VB_MunmapPool = 0x%x\n", ret);
    }

    {
        HI_VOID *virAddr = NULL;
        ret = HI_MPI_VB_GetBlockVirAddr(0, 0, &virAddr);
        fprintf(trace, "VB_GetBlockVirAddr = 0x%x\n", ret);
    }

    {
        ret = HI_MPI_VB_InitModCommPool(0);
        fprintf(trace, "VB_InitModCommPool = 0x%x\n", ret);
        ret = HI_MPI_VB_ExitModCommPool(0);
        fprintf(trace, "VB_ExitModCommPool = 0x%x\n", ret);
    }

    {
        VB_CONFIG_S modCfg;
        memset(&modCfg, 0, sizeof(modCfg));
        ret = HI_MPI_VB_SetModPoolConfig(0, &modCfg);
        fprintf(trace, "VB_SetModPoolConfig = 0x%x\n", ret);
        ret = HI_MPI_VB_GetModPoolConfig(0, &modCfg);
        fprintf(trace, "VB_GetModPoolConfig = 0x%x\n", ret);
    }
}

static void test_vb_null_ptr(FILE *trace) {
    HI_S32 ret;
    fprintf(trace, "--- VB null_ptr ---\n");

    ret = HI_MPI_VB_SetConfig(NULL);
    fprintf(trace, "VB_SetConfig(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VB_GetConfig(NULL);
    fprintf(trace, "VB_GetConfig(NULL) = 0x%x\n", ret);

    {
        VB_POOL pool = HI_MPI_VB_CreatePool(NULL);
        fprintf(trace, "VB_CreatePool(NULL) = 0x%x\n", pool);
    }

    ret = HI_MPI_VB_GetSupplementAddr(0, NULL);
    fprintf(trace, "VB_GetSupplementAddr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VB_SetSupplementConfig(NULL);
    fprintf(trace, "VB_SetSupplementConfig(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VB_GetSupplementConfig(NULL);
    fprintf(trace, "VB_GetSupplementConfig(NULL) = 0x%x\n", ret);

    ret = HI_MPI_VB_GetBlockVirAddr(0, 0, NULL);
    fprintf(trace, "VB_GetBlockVirAddr(NULL) = 0x%x\n", ret);

    ret = HI_MPI_VB_SetModPoolConfig(0, NULL);
    fprintf(trace, "VB_SetModPoolConfig(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VB_GetModPoolConfig(0, NULL);
    fprintf(trace, "VB_GetModPoolConfig(NULL) = 0x%x\n", ret);
}

/* ========================================================================
 *  VI module  (90 functions)
 * ======================================================================== */

static void test_vi_basic(FILE *trace) {
    HI_S32 ret;
    fprintf(trace, "--- VI basic ---\n");

    /* Device */
    {
        VI_DEV_ATTR_S attr;
        memset(&attr, 0, sizeof(attr));
        ret = HI_MPI_VI_SetDevAttr(0, &attr);
        fprintf(trace, "VI_SetDevAttr = 0x%x\n", ret);
        ret = HI_MPI_VI_GetDevAttr(0, &attr);
        fprintf(trace, "VI_GetDevAttr = 0x%x\n", ret);
    }

    {
        VI_DEV_ATTR_EX_S attrEx;
        memset(&attrEx, 0, sizeof(attrEx));
        ret = HI_MPI_VI_SetDevAttrEx(0, &attrEx);
        fprintf(trace, "VI_SetDevAttrEx = 0x%x\n", ret);
        ret = HI_MPI_VI_GetDevAttrEx(0, &attrEx);
        fprintf(trace, "VI_GetDevAttrEx = 0x%x\n", ret);
    }

    {
        VI_VS_SIGNAL_ATTR_S vsAttr;
        memset(&vsAttr, 0, sizeof(vsAttr));
        ret = HI_MPI_VI_SetVSSignalAttr(0, &vsAttr);
        fprintf(trace, "VI_SetVSSignalAttr = 0x%x\n", ret);
        ret = HI_MPI_VI_GetVSSignalAttr(0, &vsAttr);
        fprintf(trace, "VI_GetVSSignalAttr = 0x%x\n", ret);
        ret = HI_MPI_VI_TriggerVSSignal(0, HI_FALSE);
        fprintf(trace, "VI_TriggerVSSignal = 0x%x\n", ret);
    }

    ret = HI_MPI_VI_EnableDev(0);
    fprintf(trace, "VI_EnableDev = 0x%x\n", ret);
    ret = HI_MPI_VI_DisableDev(0);
    fprintf(trace, "VI_DisableDev = 0x%x\n", ret);

    {
        ret = HI_MPI_VI_SetMipiBindDev(0, 0);
        fprintf(trace, "VI_SetMipiBindDev = 0x%x\n", ret);
        MIPI_DEV mipi = 0;
        ret = HI_MPI_VI_GetMipiBindDev(0, &mipi);
        fprintf(trace, "VI_GetMipiBindDev = 0x%x\n", ret);
    }

    {
        VI_DEV_BIND_PIPE_S bindPipe;
        memset(&bindPipe, 0, sizeof(bindPipe));
        ret = HI_MPI_VI_SetDevBindPipe(0, &bindPipe);
        fprintf(trace, "VI_SetDevBindPipe = 0x%x\n", ret);
        ret = HI_MPI_VI_GetDevBindPipe(0, &bindPipe);
        fprintf(trace, "VI_GetDevBindPipe = 0x%x\n", ret);
    }

    {
        VI_DEV_TIMING_ATTR_S timAttr;
        memset(&timAttr, 0, sizeof(timAttr));
        ret = HI_MPI_VI_SetDevTimingAttr(0, &timAttr);
        fprintf(trace, "VI_SetDevTimingAttr = 0x%x\n", ret);
        ret = HI_MPI_VI_GetDevTimingAttr(0, &timAttr);
        fprintf(trace, "VI_GetDevTimingAttr = 0x%x\n", ret);
    }

    /* Pipe */
    {
        VI_CMP_PARAM_S cmpParam;
        memset(&cmpParam, 0, sizeof(cmpParam));
        ret = HI_MPI_VI_GetPipeCmpParam(0, &cmpParam);
        fprintf(trace, "VI_GetPipeCmpParam = 0x%x\n", ret);
    }

    {
        VI_USERPIC_ATTR_S usrPic;
        memset(&usrPic, 0, sizeof(usrPic));
        ret = HI_MPI_VI_SetUserPic(0, &usrPic);
        fprintf(trace, "VI_SetUserPic = 0x%x\n", ret);
        ret = HI_MPI_VI_EnableUserPic(0);
        fprintf(trace, "VI_EnableUserPic = 0x%x\n", ret);
        ret = HI_MPI_VI_DisableUserPic(0);
        fprintf(trace, "VI_DisableUserPic = 0x%x\n", ret);
    }

    {
        VI_PIPE_ATTR_S pipeAttr;
        memset(&pipeAttr, 0, sizeof(pipeAttr));
        ret = HI_MPI_VI_CreatePipe(0, &pipeAttr);
        fprintf(trace, "VI_CreatePipe = 0x%x\n", ret);
        ret = HI_MPI_VI_DestroyPipe(0);
        fprintf(trace, "VI_DestroyPipe = 0x%x\n", ret);

        ret = HI_MPI_VI_SetPipeAttr(0, &pipeAttr);
        fprintf(trace, "VI_SetPipeAttr = 0x%x\n", ret);
        ret = HI_MPI_VI_GetPipeAttr(0, &pipeAttr);
        fprintf(trace, "VI_GetPipeAttr = 0x%x\n", ret);
    }

    ret = HI_MPI_VI_StartPipe(0);
    fprintf(trace, "VI_StartPipe = 0x%x\n", ret);
    ret = HI_MPI_VI_StopPipe(0);
    fprintf(trace, "VI_StopPipe = 0x%x\n", ret);

    {
        CROP_INFO_S crop;
        memset(&crop, 0, sizeof(crop));
        ret = HI_MPI_VI_SetPipePreCrop(0, &crop);
        fprintf(trace, "VI_SetPipePreCrop = 0x%x\n", ret);
        ret = HI_MPI_VI_GetPipePreCrop(0, &crop);
        fprintf(trace, "VI_GetPipePreCrop = 0x%x\n", ret);

        ret = HI_MPI_VI_SetPipePostCrop(0, &crop);
        fprintf(trace, "VI_SetPipePostCrop = 0x%x\n", ret);
        ret = HI_MPI_VI_GetPipePostCrop(0, &crop);
        fprintf(trace, "VI_GetPipePostCrop = 0x%x\n", ret);
    }

    {
        FISHEYE_CONFIG_S fishCfg;
        memset(&fishCfg, 0, sizeof(fishCfg));
        ret = HI_MPI_VI_SetPipeFisheyeConfig(0, &fishCfg);
        fprintf(trace, "VI_SetPipeFisheyeConfig = 0x%x\n", ret);
        ret = HI_MPI_VI_GetPipeFisheyeConfig(0, &fishCfg);
        fprintf(trace, "VI_GetPipeFisheyeConfig = 0x%x\n", ret);
    }

    {
        POINT_S dst = {0, 0}, src = {0, 0};
        ret = HI_MPI_VI_FisheyePosQueryDst2Src(0, 0, 0, &dst, &src);
        fprintf(trace, "VI_FisheyePosQueryDst2Src = 0x%x\n", ret);
    }

    {
        VI_DUMP_ATTR_S dumpAttr;
        memset(&dumpAttr, 0, sizeof(dumpAttr));
        ret = HI_MPI_VI_SetPipeDumpAttr(0, &dumpAttr);
        fprintf(trace, "VI_SetPipeDumpAttr = 0x%x\n", ret);
        ret = HI_MPI_VI_GetPipeDumpAttr(0, &dumpAttr);
        fprintf(trace, "VI_GetPipeDumpAttr = 0x%x\n", ret);
    }

    {
        ret = HI_MPI_VI_SetPipeFrameSource(0, 0);
        fprintf(trace, "VI_SetPipeFrameSource = 0x%x\n", ret);
        VI_PIPE_FRAME_SOURCE_E src = 0;
        ret = HI_MPI_VI_GetPipeFrameSource(0, &src);
        fprintf(trace, "VI_GetPipeFrameSource = 0x%x\n", ret);
    }

    {
        VIDEO_FRAME_INFO_S frm;
        memset(&frm, 0, sizeof(frm));
        ret = HI_MPI_VI_GetPipeFrame(0, &frm, 0);
        fprintf(trace, "VI_GetPipeFrame = 0x%x\n", ret);
        ret = HI_MPI_VI_ReleasePipeFrame(0, &frm);
        fprintf(trace, "VI_ReleasePipeFrame = 0x%x\n", ret);

        ret = HI_MPI_VI_SendPipeYUV(0, &frm, 0);
        fprintf(trace, "VI_SendPipeYUV = 0x%x\n", ret);
    }

    {
        VI_PIPE pipeId[1] = {0};
        const VIDEO_FRAME_INFO_S *frms[1];
        VIDEO_FRAME_INFO_S frm0;
        memset(&frm0, 0, sizeof(frm0));
        frms[0] = &frm0;
        ret = HI_MPI_VI_SendPipeRaw(1, pipeId, frms, 0);
        fprintf(trace, "VI_SendPipeRaw = 0x%x\n", ret);
    }

    {
        VI_PIPE_NRX_PARAM_S nrx;
        memset(&nrx, 0, sizeof(nrx));
        ret = HI_MPI_VI_SetPipeNRXParam(0, &nrx);
        fprintf(trace, "VI_SetPipeNRXParam = 0x%x\n", ret);
        ret = HI_MPI_VI_GetPipeNRXParam(0, &nrx);
        fprintf(trace, "VI_GetPipeNRXParam = 0x%x\n", ret);
    }

    {
        ret = HI_MPI_VI_SetPipeRepeatMode(0, 0);
        fprintf(trace, "VI_SetPipeRepeatMode = 0x%x\n", ret);
        VI_PIPE_REPEAT_MODE_E mode = 0;
        ret = HI_MPI_VI_GetPipeRepeatMode(0, &mode);
        fprintf(trace, "VI_GetPipeRepeatMode = 0x%x\n", ret);
    }

    {
        VI_PIPE_STATUS_S status;
        memset(&status, 0, sizeof(status));
        ret = HI_MPI_VI_QueryPipeStatus(0, &status);
        fprintf(trace, "VI_QueryPipeStatus = 0x%x\n", ret);
    }

    ret = HI_MPI_VI_EnablePipeInterrupt(0);
    fprintf(trace, "VI_EnablePipeInterrupt = 0x%x\n", ret);
    ret = HI_MPI_VI_DisablePipeInterrupt(0);
    fprintf(trace, "VI_DisablePipeInterrupt = 0x%x\n", ret);

    {
        ret = HI_MPI_VI_SetPipeVCNumber(0, 0);
        fprintf(trace, "VI_SetPipeVCNumber = 0x%x\n", ret);
        HI_U32 vcNum = 0;
        ret = HI_MPI_VI_GetPipeVCNumber(0, &vcNum);
        fprintf(trace, "VI_GetPipeVCNumber = 0x%x\n", ret);
    }

    {
        FRAME_INTERRUPT_ATTR_S frmInt;
        memset(&frmInt, 0, sizeof(frmInt));
        ret = HI_MPI_VI_SetPipeFrameInterruptAttr(0, &frmInt);
        fprintf(trace, "VI_SetPipeFrameInterruptAttr = 0x%x\n", ret);
        ret = HI_MPI_VI_GetPipeFrameInterruptAttr(0, &frmInt);
        fprintf(trace, "VI_GetPipeFrameInterruptAttr = 0x%x\n", ret);
    }

    {
        BNR_DUMP_ATTR_S bnr;
        memset(&bnr, 0, sizeof(bnr));
        ret = HI_MPI_VI_SetPipeBNRRawDumpAttr(0, &bnr);
        fprintf(trace, "VI_SetPipeBNRRawDumpAttr = 0x%x\n", ret);
        ret = HI_MPI_VI_GetPipeBNRRawDumpAttr(0, &bnr);
        fprintf(trace, "VI_GetPipeBNRRawDumpAttr = 0x%x\n", ret);
    }

    {
        VIDEO_FRAME_INFO_S frm;
        memset(&frm, 0, sizeof(frm));
        ret = HI_MPI_VI_GetPipeBNRRaw(0, &frm, 0);
        fprintf(trace, "VI_GetPipeBNRRaw = 0x%x\n", ret);
        ret = HI_MPI_VI_ReleasePipeBNRRaw(0, &frm);
        fprintf(trace, "VI_ReleasePipeBNRRaw = 0x%x\n", ret);
    }

    ret = HI_MPI_VI_PipeAttachVbPool(0, 0);
    fprintf(trace, "VI_PipeAttachVbPool = 0x%x\n", ret);
    ret = HI_MPI_VI_PipeDetachVbPool(0);
    fprintf(trace, "VI_PipeDetachVbPool = 0x%x\n", ret);

    {
        ret = HI_MPI_VI_GetPipeFd(0);
        fprintf(trace, "VI_GetPipeFd = 0x%x\n", ret);
    }

    /* Channel */
    {
        VI_CHN_ATTR_S chnAttr;
        memset(&chnAttr, 0, sizeof(chnAttr));
        ret = HI_MPI_VI_SetChnAttr(0, 0, &chnAttr);
        fprintf(trace, "VI_SetChnAttr = 0x%x\n", ret);
        ret = HI_MPI_VI_GetChnAttr(0, 0, &chnAttr);
        fprintf(trace, "VI_GetChnAttr = 0x%x\n", ret);
    }

    ret = HI_MPI_VI_EnableChn(0, 0);
    fprintf(trace, "VI_EnableChn = 0x%x\n", ret);
    ret = HI_MPI_VI_DisableChn(0, 0);
    fprintf(trace, "VI_DisableChn = 0x%x\n", ret);

    {
        VI_CROP_INFO_S cropInfo;
        memset(&cropInfo, 0, sizeof(cropInfo));
        ret = HI_MPI_VI_SetChnCrop(0, 0, &cropInfo);
        fprintf(trace, "VI_SetChnCrop = 0x%x\n", ret);
        ret = HI_MPI_VI_GetChnCrop(0, 0, &cropInfo);
        fprintf(trace, "VI_GetChnCrop = 0x%x\n", ret);
    }

    {
        ret = HI_MPI_VI_SetChnRotation(0, 0, 0);
        fprintf(trace, "VI_SetChnRotation = 0x%x\n", ret);
        ROTATION_E rot = 0;
        ret = HI_MPI_VI_GetChnRotation(0, 0, &rot);
        fprintf(trace, "VI_GetChnRotation = 0x%x\n", ret);
    }

    {
        VI_ROTATION_EX_ATTR_S rotEx;
        memset(&rotEx, 0, sizeof(rotEx));
        ret = HI_MPI_VI_SetChnRotationEx(0, 0, &rotEx);
        fprintf(trace, "VI_SetChnRotationEx = 0x%x\n", ret);
        ret = HI_MPI_VI_GetChnRotationEx(0, 0, &rotEx);
        fprintf(trace, "VI_GetChnRotationEx = 0x%x\n", ret);
    }

    {
        VI_LDC_ATTR_S ldc;
        memset(&ldc, 0, sizeof(ldc));
        ret = HI_MPI_VI_SetChnLDCAttr(0, 0, &ldc);
        fprintf(trace, "VI_SetChnLDCAttr = 0x%x\n", ret);
        ret = HI_MPI_VI_GetChnLDCAttr(0, 0, &ldc);
        fprintf(trace, "VI_GetChnLDCAttr = 0x%x\n", ret);
    }

    {
        VI_LDCV2_ATTR_S ldcv2;
        memset(&ldcv2, 0, sizeof(ldcv2));
        ret = HI_MPI_VI_SetChnLDCV2Attr(0, 0, &ldcv2);
        fprintf(trace, "VI_SetChnLDCV2Attr = 0x%x\n", ret);
        ret = HI_MPI_VI_GetChnLDCV2Attr(0, 0, &ldcv2);
        fprintf(trace, "VI_GetChnLDCV2Attr = 0x%x\n", ret);
    }

    {
        VI_LDCV3_ATTR_S ldcv3;
        memset(&ldcv3, 0, sizeof(ldcv3));
        ret = HI_MPI_VI_SetChnLDCV3Attr(0, 0, &ldcv3);
        fprintf(trace, "VI_SetChnLDCV3Attr = 0x%x\n", ret);
        ret = HI_MPI_VI_GetChnLDCV3Attr(0, 0, &ldcv3);
        fprintf(trace, "VI_GetChnLDCV3Attr = 0x%x\n", ret);
    }

    {
        SPREAD_ATTR_S spread;
        memset(&spread, 0, sizeof(spread));
        ret = HI_MPI_VI_SetChnSpreadAttr(0, 0, &spread);
        fprintf(trace, "VI_SetChnSpreadAttr = 0x%x\n", ret);
        ret = HI_MPI_VI_GetChnSpreadAttr(0, 0, &spread);
        fprintf(trace, "VI_GetChnSpreadAttr = 0x%x\n", ret);
    }

    {
        VI_LOW_DELAY_INFO_S lowDelay;
        memset(&lowDelay, 0, sizeof(lowDelay));
        ret = HI_MPI_VI_SetChnLowDelayAttr(0, 0, &lowDelay);
        fprintf(trace, "VI_SetChnLowDelayAttr = 0x%x\n", ret);
        ret = HI_MPI_VI_GetChnLowDelayAttr(0, 0, &lowDelay);
        fprintf(trace, "VI_GetChnLowDelayAttr = 0x%x\n", ret);
    }

    {
        VIDEO_REGION_INFO_S regInfo;
        memset(&regInfo, 0, sizeof(regInfo));
        HI_U64 lumaData = 0;
        ret = HI_MPI_VI_GetChnRegionLuma(0, 0, &regInfo, &lumaData, 0);
        fprintf(trace, "VI_GetChnRegionLuma = 0x%x\n", ret);
    }

    {
        DIS_CONFIG_S disCfg;
        memset(&disCfg, 0, sizeof(disCfg));
        ret = HI_MPI_VI_SetChnDISConfig(0, 0, &disCfg);
        fprintf(trace, "VI_SetChnDISConfig = 0x%x\n", ret);
        ret = HI_MPI_VI_GetChnDISConfig(0, 0, &disCfg);
        fprintf(trace, "VI_GetChnDISConfig = 0x%x\n", ret);

        DIS_ATTR_S disAttr;
        memset(&disAttr, 0, sizeof(disAttr));
        ret = HI_MPI_VI_SetChnDISAttr(0, 0, &disAttr);
        fprintf(trace, "VI_SetChnDISAttr = 0x%x\n", ret);
        ret = HI_MPI_VI_GetChnDISAttr(0, 0, &disAttr);
        fprintf(trace, "VI_GetChnDISAttr = 0x%x\n", ret);
    }

    {
        FISHEYE_ATTR_S fishAttr;
        memset(&fishAttr, 0, sizeof(fishAttr));
        ret = HI_MPI_VI_SetExtChnFisheye(0, 0, &fishAttr);
        fprintf(trace, "VI_SetExtChnFisheye = 0x%x\n", ret);
        ret = HI_MPI_VI_GetExtChnFisheye(0, 0, &fishAttr);
        fprintf(trace, "VI_GetExtChnFisheye = 0x%x\n", ret);
    }

    {
        VI_EXT_CHN_ATTR_S extAttr;
        memset(&extAttr, 0, sizeof(extAttr));
        ret = HI_MPI_VI_SetExtChnAttr(0, 0, &extAttr);
        fprintf(trace, "VI_SetExtChnAttr = 0x%x\n", ret);
        ret = HI_MPI_VI_GetExtChnAttr(0, 0, &extAttr);
        fprintf(trace, "VI_GetExtChnAttr = 0x%x\n", ret);
    }

    {
        VIDEO_FRAME_INFO_S frm;
        memset(&frm, 0, sizeof(frm));
        ret = HI_MPI_VI_GetChnFrame(0, 0, &frm, 0);
        fprintf(trace, "VI_GetChnFrame = 0x%x\n", ret);
        ret = HI_MPI_VI_ReleaseChnFrame(0, 0, &frm);
        fprintf(trace, "VI_ReleaseChnFrame = 0x%x\n", ret);
    }

    {
        VI_EARLY_INTERRUPT_S earlyInt;
        memset(&earlyInt, 0, sizeof(earlyInt));
        ret = HI_MPI_VI_SetChnEarlyInterrupt(0, 0, &earlyInt);
        fprintf(trace, "VI_SetChnEarlyInterrupt = 0x%x\n", ret);
        ret = HI_MPI_VI_GetChnEarlyInterrupt(0, 0, &earlyInt);
        fprintf(trace, "VI_GetChnEarlyInterrupt = 0x%x\n", ret);
    }

    {
        ret = HI_MPI_VI_SetChnAlign(0, 0, 0);
        fprintf(trace, "VI_SetChnAlign = 0x%x\n", ret);
        HI_U32 align = 0;
        ret = HI_MPI_VI_GetChnAlign(0, 0, &align);
        fprintf(trace, "VI_GetChnAlign = 0x%x\n", ret);
    }

    ret = HI_MPI_VI_ChnAttachVbPool(0, 0, 0);
    fprintf(trace, "VI_ChnAttachVbPool = 0x%x\n", ret);
    ret = HI_MPI_VI_ChnDetachVbPool(0, 0);
    fprintf(trace, "VI_ChnDetachVbPool = 0x%x\n", ret);

    {
        VI_CHN_STATUS_S chnStatus;
        memset(&chnStatus, 0, sizeof(chnStatus));
        ret = HI_MPI_VI_QueryChnStatus(0, 0, &chnStatus);
        fprintf(trace, "VI_QueryChnStatus = 0x%x\n", ret);
    }

    ret = HI_MPI_VI_GetChnFd(0, 0);
    fprintf(trace, "VI_GetChnFd = 0x%x\n", ret);

    /* Stitch */
    {
        VI_STITCH_GRP_ATTR_S stitchAttr;
        memset(&stitchAttr, 0, sizeof(stitchAttr));
        ret = HI_MPI_VI_SetStitchGrpAttr(0, &stitchAttr);
        fprintf(trace, "VI_SetStitchGrpAttr = 0x%x\n", ret);
        ret = HI_MPI_VI_GetStitchGrpAttr(0, &stitchAttr);
        fprintf(trace, "VI_GetStitchGrpAttr = 0x%x\n", ret);
    }

    /* Module */
    {
        VI_MOD_PARAM_S modParam;
        memset(&modParam, 0, sizeof(modParam));
        ret = HI_MPI_VI_SetModParam(&modParam);
        fprintf(trace, "VI_SetModParam = 0x%x\n", ret);
        ret = HI_MPI_VI_GetModParam(&modParam);
        fprintf(trace, "VI_GetModParam = 0x%x\n", ret);
    }

    ret = HI_MPI_VI_CloseFd();
    fprintf(trace, "VI_CloseFd = 0x%x\n", ret);
}

static void test_vi_null_ptr(FILE *trace) {
    HI_S32 ret;
    fprintf(trace, "--- VI null_ptr ---\n");

    ret = HI_MPI_VI_SetDevAttr(0, NULL);
    fprintf(trace, "VI_SetDevAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_GetDevAttr(0, NULL);
    fprintf(trace, "VI_GetDevAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_SetDevAttrEx(0, NULL);
    fprintf(trace, "VI_SetDevAttrEx(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_GetDevAttrEx(0, NULL);
    fprintf(trace, "VI_GetDevAttrEx(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_SetVSSignalAttr(0, NULL);
    fprintf(trace, "VI_SetVSSignalAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_GetVSSignalAttr(0, NULL);
    fprintf(trace, "VI_GetVSSignalAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_GetMipiBindDev(0, NULL);
    fprintf(trace, "VI_GetMipiBindDev(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_SetDevBindPipe(0, NULL);
    fprintf(trace, "VI_SetDevBindPipe(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_GetDevBindPipe(0, NULL);
    fprintf(trace, "VI_GetDevBindPipe(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_SetDevTimingAttr(0, NULL);
    fprintf(trace, "VI_SetDevTimingAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_GetDevTimingAttr(0, NULL);
    fprintf(trace, "VI_GetDevTimingAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_GetPipeCmpParam(0, NULL);
    fprintf(trace, "VI_GetPipeCmpParam(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_SetUserPic(0, NULL);
    fprintf(trace, "VI_SetUserPic(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_CreatePipe(0, NULL);
    fprintf(trace, "VI_CreatePipe(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_SetPipeAttr(0, NULL);
    fprintf(trace, "VI_SetPipeAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_GetPipeAttr(0, NULL);
    fprintf(trace, "VI_GetPipeAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_SetPipePreCrop(0, NULL);
    fprintf(trace, "VI_SetPipePreCrop(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_GetPipePreCrop(0, NULL);
    fprintf(trace, "VI_GetPipePreCrop(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_SetPipeFisheyeConfig(0, NULL);
    fprintf(trace, "VI_SetPipeFisheyeConfig(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_GetPipeFisheyeConfig(0, NULL);
    fprintf(trace, "VI_GetPipeFisheyeConfig(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_FisheyePosQueryDst2Src(0, 0, 0, NULL, NULL);
    fprintf(trace, "VI_FisheyePosQueryDst2Src(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_SetPipeDumpAttr(0, NULL);
    fprintf(trace, "VI_SetPipeDumpAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_GetPipeDumpAttr(0, NULL);
    fprintf(trace, "VI_GetPipeDumpAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_GetPipeFrameSource(0, NULL);
    fprintf(trace, "VI_GetPipeFrameSource(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_GetPipeFrame(0, NULL, 0);
    fprintf(trace, "VI_GetPipeFrame(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_ReleasePipeFrame(0, NULL);
    fprintf(trace, "VI_ReleasePipeFrame(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_SendPipeYUV(0, NULL, 0);
    fprintf(trace, "VI_SendPipeYUV(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_SetPipeNRXParam(0, NULL);
    fprintf(trace, "VI_SetPipeNRXParam(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_GetPipeNRXParam(0, NULL);
    fprintf(trace, "VI_GetPipeNRXParam(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_GetPipeRepeatMode(0, NULL);
    fprintf(trace, "VI_GetPipeRepeatMode(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_QueryPipeStatus(0, NULL);
    fprintf(trace, "VI_QueryPipeStatus(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_GetPipeVCNumber(0, NULL);
    fprintf(trace, "VI_GetPipeVCNumber(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_SetPipeFrameInterruptAttr(0, NULL);
    fprintf(trace, "VI_SetPipeFrameInterruptAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_GetPipeFrameInterruptAttr(0, NULL);
    fprintf(trace, "VI_GetPipeFrameInterruptAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_SetPipeBNRRawDumpAttr(0, NULL);
    fprintf(trace, "VI_SetPipeBNRRawDumpAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_GetPipeBNRRawDumpAttr(0, NULL);
    fprintf(trace, "VI_GetPipeBNRRawDumpAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_GetPipeBNRRaw(0, NULL, 0);
    fprintf(trace, "VI_GetPipeBNRRaw(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_ReleasePipeBNRRaw(0, NULL);
    fprintf(trace, "VI_ReleasePipeBNRRaw(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_SetChnAttr(0, 0, NULL);
    fprintf(trace, "VI_SetChnAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_GetChnAttr(0, 0, NULL);
    fprintf(trace, "VI_GetChnAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_SetChnCrop(0, 0, NULL);
    fprintf(trace, "VI_SetChnCrop(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_GetChnCrop(0, 0, NULL);
    fprintf(trace, "VI_GetChnCrop(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_GetChnRotation(0, 0, NULL);
    fprintf(trace, "VI_GetChnRotation(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_SetChnRotationEx(0, 0, NULL);
    fprintf(trace, "VI_SetChnRotationEx(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_GetChnRotationEx(0, 0, NULL);
    fprintf(trace, "VI_GetChnRotationEx(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_SetChnLDCAttr(0, 0, NULL);
    fprintf(trace, "VI_SetChnLDCAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_GetChnLDCAttr(0, 0, NULL);
    fprintf(trace, "VI_GetChnLDCAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_SetChnLDCV2Attr(0, 0, NULL);
    fprintf(trace, "VI_SetChnLDCV2Attr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_GetChnLDCV2Attr(0, 0, NULL);
    fprintf(trace, "VI_GetChnLDCV2Attr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_SetChnLDCV3Attr(0, 0, NULL);
    fprintf(trace, "VI_SetChnLDCV3Attr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_GetChnLDCV3Attr(0, 0, NULL);
    fprintf(trace, "VI_GetChnLDCV3Attr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_SetChnSpreadAttr(0, 0, NULL);
    fprintf(trace, "VI_SetChnSpreadAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_GetChnSpreadAttr(0, 0, NULL);
    fprintf(trace, "VI_GetChnSpreadAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_SetChnLowDelayAttr(0, 0, NULL);
    fprintf(trace, "VI_SetChnLowDelayAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_GetChnLowDelayAttr(0, 0, NULL);
    fprintf(trace, "VI_GetChnLowDelayAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_GetChnRegionLuma(0, 0, NULL, NULL, 0);
    fprintf(trace, "VI_GetChnRegionLuma(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_SetChnDISConfig(0, 0, NULL);
    fprintf(trace, "VI_SetChnDISConfig(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_GetChnDISConfig(0, 0, NULL);
    fprintf(trace, "VI_GetChnDISConfig(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_SetChnDISAttr(0, 0, NULL);
    fprintf(trace, "VI_SetChnDISAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_GetChnDISAttr(0, 0, NULL);
    fprintf(trace, "VI_GetChnDISAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_SetExtChnFisheye(0, 0, NULL);
    fprintf(trace, "VI_SetExtChnFisheye(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_GetExtChnFisheye(0, 0, NULL);
    fprintf(trace, "VI_GetExtChnFisheye(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_SetExtChnAttr(0, 0, NULL);
    fprintf(trace, "VI_SetExtChnAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_GetExtChnAttr(0, 0, NULL);
    fprintf(trace, "VI_GetExtChnAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_GetChnFrame(0, 0, NULL, 0);
    fprintf(trace, "VI_GetChnFrame(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_ReleaseChnFrame(0, 0, NULL);
    fprintf(trace, "VI_ReleaseChnFrame(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_SetChnEarlyInterrupt(0, 0, NULL);
    fprintf(trace, "VI_SetChnEarlyInterrupt(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_GetChnEarlyInterrupt(0, 0, NULL);
    fprintf(trace, "VI_GetChnEarlyInterrupt(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_GetChnAlign(0, 0, NULL);
    fprintf(trace, "VI_GetChnAlign(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_QueryChnStatus(0, 0, NULL);
    fprintf(trace, "VI_QueryChnStatus(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_SetStitchGrpAttr(0, NULL);
    fprintf(trace, "VI_SetStitchGrpAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_GetStitchGrpAttr(0, NULL);
    fprintf(trace, "VI_GetStitchGrpAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_SetModParam(NULL);
    fprintf(trace, "VI_SetModParam(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VI_GetModParam(NULL);
    fprintf(trace, "VI_GetModParam(NULL) = 0x%x\n", ret);
}

/* ========================================================================
 *  VO module  (76 functions)
 * ======================================================================== */

static void test_vo_basic(FILE *trace) {
    HI_S32 ret;
    fprintf(trace, "--- VO basic ---\n");

    /* Device */
    {
        VO_PUB_ATTR_S pub;
        memset(&pub, 0, sizeof(pub));
        ret = HI_MPI_VO_SetPubAttr(0, &pub);
        fprintf(trace, "VO_SetPubAttr = 0x%x\n", ret);
        ret = HI_MPI_VO_GetPubAttr(0, &pub);
        fprintf(trace, "VO_GetPubAttr = 0x%x\n", ret);
    }

    ret = HI_MPI_VO_Enable(0);
    fprintf(trace, "VO_Enable = 0x%x\n", ret);
    ret = HI_MPI_VO_Disable(0);
    fprintf(trace, "VO_Disable = 0x%x\n", ret);

    ret = HI_MPI_VO_CloseFd();
    fprintf(trace, "VO_CloseFd = 0x%x\n", ret);

    {
        VO_USER_INTFSYNC_INFO_S info;
        memset(&info, 0, sizeof(info));
        ret = HI_MPI_VO_SetUserIntfSyncInfo(0, &info);
        fprintf(trace, "VO_SetUserIntfSyncInfo = 0x%x\n", ret);
    }

    /* Video Layer */
    {
        VO_VIDEO_LAYER_ATTR_S layerAttr;
        memset(&layerAttr, 0, sizeof(layerAttr));
        ret = HI_MPI_VO_SetVideoLayerAttr(0, &layerAttr);
        fprintf(trace, "VO_SetVideoLayerAttr = 0x%x\n", ret);
        ret = HI_MPI_VO_GetVideoLayerAttr(0, &layerAttr);
        fprintf(trace, "VO_GetVideoLayerAttr = 0x%x\n", ret);
    }

    ret = HI_MPI_VO_EnableVideoLayer(0);
    fprintf(trace, "VO_EnableVideoLayer = 0x%x\n", ret);
    ret = HI_MPI_VO_DisableVideoLayer(0);
    fprintf(trace, "VO_DisableVideoLayer = 0x%x\n", ret);

    ret = HI_MPI_VO_BindVideoLayer(0, 0);
    fprintf(trace, "VO_BindVideoLayer = 0x%x\n", ret);
    ret = HI_MPI_VO_UnBindVideoLayer(0, 0);
    fprintf(trace, "VO_UnBindVideoLayer = 0x%x\n", ret);

    {
        ret = HI_MPI_VO_SetVideoLayerPriority(0, 0);
        fprintf(trace, "VO_SetVideoLayerPriority = 0x%x\n", ret);
        HI_U32 pri = 0;
        ret = HI_MPI_VO_GetVideoLayerPriority(0, &pri);
        fprintf(trace, "VO_GetVideoLayerPriority = 0x%x\n", ret);
    }

    {
        VO_CSC_S csc;
        memset(&csc, 0, sizeof(csc));
        ret = HI_MPI_VO_SetVideoLayerCSC(0, &csc);
        fprintf(trace, "VO_SetVideoLayerCSC = 0x%x\n", ret);
        ret = HI_MPI_VO_GetVideoLayerCSC(0, &csc);
        fprintf(trace, "VO_GetVideoLayerCSC = 0x%x\n", ret);
    }

    {
        ret = HI_MPI_VO_SetVideoLayerPartitionMode(0, 0);
        fprintf(trace, "VO_SetVideoLayerPartitionMode = 0x%x\n", ret);
        VO_PART_MODE_E mode = 0;
        ret = HI_MPI_VO_GetVideoLayerPartitionMode(0, &mode);
        fprintf(trace, "VO_GetVideoLayerPartitionMode = 0x%x\n", ret);
    }

    ret = HI_MPI_VO_BatchBegin(0);
    fprintf(trace, "VO_BatchBegin = 0x%x\n", ret);
    ret = HI_MPI_VO_BatchEnd(0);
    fprintf(trace, "VO_BatchEnd = 0x%x\n", ret);

    {
        VO_LAYER_BOUNDARY_S bnd;
        memset(&bnd, 0, sizeof(bnd));
        ret = HI_MPI_VO_SetVideoLayerBoundary(0, &bnd);
        fprintf(trace, "VO_SetVideoLayerBoundary = 0x%x\n", ret);
        ret = HI_MPI_VO_GetVideoLayerBoundary(0, &bnd);
        fprintf(trace, "VO_GetVideoLayerBoundary = 0x%x\n", ret);
    }

    {
        VO_LAYER_PARAM_S param;
        memset(&param, 0, sizeof(param));
        ret = HI_MPI_VO_SetVideoLayerParam(0, &param);
        fprintf(trace, "VO_SetVideoLayerParam = 0x%x\n", ret);
        ret = HI_MPI_VO_GetVideoLayerParam(0, &param);
        fprintf(trace, "VO_GetVideoLayerParam = 0x%x\n", ret);
    }

    {
        ret = HI_MPI_VO_SetVideoLayerDecompress(0, HI_FALSE);
        fprintf(trace, "VO_SetVideoLayerDecompress = 0x%x\n", ret);
        HI_BOOL dec = HI_FALSE;
        ret = HI_MPI_VO_GetVideoLayerDecompress(0, &dec);
        fprintf(trace, "VO_GetVideoLayerDecompress = 0x%x\n", ret);
    }

    {
        CROP_INFO_S crop;
        memset(&crop, 0, sizeof(crop));
        ret = HI_MPI_VO_SetVideoLayerCrop(0, &crop);
        fprintf(trace, "VO_SetVideoLayerCrop = 0x%x\n", ret);
        ret = HI_MPI_VO_GetVideoLayerCrop(0, &crop);
        fprintf(trace, "VO_GetVideoLayerCrop = 0x%x\n", ret);
    }

    /* Display */
    {
        ret = HI_MPI_VO_SetPlayToleration(0, 0);
        fprintf(trace, "VO_SetPlayToleration = 0x%x\n", ret);
        HI_U32 tol = 0;
        ret = HI_MPI_VO_GetPlayToleration(0, &tol);
        fprintf(trace, "VO_GetPlayToleration = 0x%x\n", ret);
    }

    {
        VIDEO_FRAME_INFO_S frm;
        memset(&frm, 0, sizeof(frm));
        ret = HI_MPI_VO_GetScreenFrame(0, &frm, 0);
        fprintf(trace, "VO_GetScreenFrame = 0x%x\n", ret);
        ret = HI_MPI_VO_ReleaseScreenFrame(0, &frm);
        fprintf(trace, "VO_ReleaseScreenFrame = 0x%x\n", ret);
    }

    {
        ret = HI_MPI_VO_SetDisplayBufLen(0, 0);
        fprintf(trace, "VO_SetDisplayBufLen = 0x%x\n", ret);
        HI_U32 bufLen = 0;
        ret = HI_MPI_VO_GetDisplayBufLen(0, &bufLen);
        fprintf(trace, "VO_GetDisplayBufLen = 0x%x\n", ret);
    }

    /* Channel */
    {
        VO_CHN_ATTR_S chnAttr;
        memset(&chnAttr, 0, sizeof(chnAttr));
        ret = HI_MPI_VO_SetChnAttr(0, 0, &chnAttr);
        fprintf(trace, "VO_SetChnAttr = 0x%x\n", ret);
        ret = HI_MPI_VO_GetChnAttr(0, 0, &chnAttr);
        fprintf(trace, "VO_GetChnAttr = 0x%x\n", ret);
    }

    ret = HI_MPI_VO_EnableChn(0, 0);
    fprintf(trace, "VO_EnableChn = 0x%x\n", ret);
    ret = HI_MPI_VO_DisableChn(0, 0);
    fprintf(trace, "VO_DisableChn = 0x%x\n", ret);

    {
        VO_CHN_PARAM_S chnParam;
        memset(&chnParam, 0, sizeof(chnParam));
        ret = HI_MPI_VO_SetChnParam(0, 0, &chnParam);
        fprintf(trace, "VO_SetChnParam = 0x%x\n", ret);
        ret = HI_MPI_VO_GetChnParam(0, 0, &chnParam);
        fprintf(trace, "VO_GetChnParam = 0x%x\n", ret);
    }

    {
        POINT_S pos = {0, 0};
        ret = HI_MPI_VO_SetChnDisplayPosition(0, 0, &pos);
        fprintf(trace, "VO_SetChnDisplayPosition = 0x%x\n", ret);
        ret = HI_MPI_VO_GetChnDisplayPosition(0, 0, &pos);
        fprintf(trace, "VO_GetChnDisplayPosition = 0x%x\n", ret);
    }

    {
        ret = HI_MPI_VO_SetChnFrameRate(0, 0, 0);
        fprintf(trace, "VO_SetChnFrameRate = 0x%x\n", ret);
        HI_S32 fps = 0;
        ret = HI_MPI_VO_GetChnFrameRate(0, 0, &fps);
        fprintf(trace, "VO_GetChnFrameRate = 0x%x\n", ret);
    }

    {
        VIDEO_FRAME_INFO_S frm;
        memset(&frm, 0, sizeof(frm));
        ret = HI_MPI_VO_GetChnFrame(0, 0, &frm, 0);
        fprintf(trace, "VO_GetChnFrame = 0x%x\n", ret);
        ret = HI_MPI_VO_ReleaseChnFrame(0, 0, &frm);
        fprintf(trace, "VO_ReleaseChnFrame = 0x%x\n", ret);
    }

    ret = HI_MPI_VO_PauseChn(0, 0);
    fprintf(trace, "VO_PauseChn = 0x%x\n", ret);
    ret = HI_MPI_VO_ResumeChn(0, 0);
    fprintf(trace, "VO_ResumeChn = 0x%x\n", ret);
    ret = HI_MPI_VO_StepChn(0, 0);
    fprintf(trace, "VO_StepChn = 0x%x\n", ret);
    ret = HI_MPI_VO_RefreshChn(0, 0);
    fprintf(trace, "VO_RefreshChn = 0x%x\n", ret);
    ret = HI_MPI_VO_ShowChn(0, 0);
    fprintf(trace, "VO_ShowChn = 0x%x\n", ret);
    ret = HI_MPI_VO_HideChn(0, 0);
    fprintf(trace, "VO_HideChn = 0x%x\n", ret);

    {
        VO_ZOOM_ATTR_S zoom;
        memset(&zoom, 0, sizeof(zoom));
        ret = HI_MPI_VO_SetZoomInWindow(0, 0, &zoom);
        fprintf(trace, "VO_SetZoomInWindow = 0x%x\n", ret);
        ret = HI_MPI_VO_GetZoomInWindow(0, 0, &zoom);
        fprintf(trace, "VO_GetZoomInWindow = 0x%x\n", ret);
    }

    {
        HI_U64 pts = 0;
        ret = HI_MPI_VO_GetChnPTS(0, 0, &pts);
        fprintf(trace, "VO_GetChnPTS = 0x%x\n", ret);
    }

    {
        VO_QUERY_STATUS_S status;
        memset(&status, 0, sizeof(status));
        ret = HI_MPI_VO_QueryChnStatus(0, 0, &status);
        fprintf(trace, "VO_QueryChnStatus = 0x%x\n", ret);
    }

    {
        VIDEO_FRAME_INFO_S frm;
        memset(&frm, 0, sizeof(frm));
        ret = HI_MPI_VO_SendFrame(0, 0, &frm, 0);
        fprintf(trace, "VO_SendFrame = 0x%x\n", ret);
    }

    ret = HI_MPI_VO_ClearChnBuf(0, 0, HI_FALSE);
    fprintf(trace, "VO_ClearChnBuf = 0x%x\n", ret);

    {
        VO_BORDER_S border;
        memset(&border, 0, sizeof(border));
        ret = HI_MPI_VO_SetChnBorder(0, 0, &border);
        fprintf(trace, "VO_SetChnBorder = 0x%x\n", ret);
        ret = HI_MPI_VO_GetChnBorder(0, 0, &border);
        fprintf(trace, "VO_GetChnBorder = 0x%x\n", ret);
    }

    {
        VO_CHN_BOUNDARY_S bnd;
        memset(&bnd, 0, sizeof(bnd));
        ret = HI_MPI_VO_SetChnBoundary(0, 0, &bnd);
        fprintf(trace, "VO_SetChnBoundary = 0x%x\n", ret);
        ret = HI_MPI_VO_GetChnBoundary(0, 0, &bnd);
        fprintf(trace, "VO_GetChnBoundary = 0x%x\n", ret);
    }

    {
        ret = HI_MPI_VO_SetChnRecvThreshold(0, 0, 0);
        fprintf(trace, "VO_SetChnRecvThreshold = 0x%x\n", ret);
        HI_U32 thresh = 0;
        ret = HI_MPI_VO_GetChnRecvThreshold(0, 0, &thresh);
        fprintf(trace, "VO_GetChnRecvThreshold = 0x%x\n", ret);
    }

    {
        ret = HI_MPI_VO_SetChnRotation(0, 0, 0);
        fprintf(trace, "VO_SetChnRotation = 0x%x\n", ret);
        ROTATION_E rot = 0;
        ret = HI_MPI_VO_GetChnRotation(0, 0, &rot);
        fprintf(trace, "VO_GetChnRotation = 0x%x\n", ret);
    }

    {
        VO_REGION_INFO_S regInfo;
        memset(&regInfo, 0, sizeof(regInfo));
        HI_U64 lumaData = 0;
        ret = HI_MPI_VO_GetChnRegionLuma(0, 0, &regInfo, &lumaData, 0);
        fprintf(trace, "VO_GetChnRegionLuma = 0x%x\n", ret);
    }

    /* WBC */
    {
        VO_WBC_SOURCE_S wbcSrc;
        memset(&wbcSrc, 0, sizeof(wbcSrc));
        ret = HI_MPI_VO_SetWBCSource(0, &wbcSrc);
        fprintf(trace, "VO_SetWBCSource = 0x%x\n", ret);
        ret = HI_MPI_VO_GetWBCSource(0, &wbcSrc);
        fprintf(trace, "VO_GetWBCSource = 0x%x\n", ret);
    }

    {
        VO_WBC_ATTR_S wbcAttr;
        memset(&wbcAttr, 0, sizeof(wbcAttr));
        ret = HI_MPI_VO_SetWBCAttr(0, &wbcAttr);
        fprintf(trace, "VO_SetWBCAttr = 0x%x\n", ret);
        ret = HI_MPI_VO_GetWBCAttr(0, &wbcAttr);
        fprintf(trace, "VO_GetWBCAttr = 0x%x\n", ret);
    }

    ret = HI_MPI_VO_EnableWBC(0);
    fprintf(trace, "VO_EnableWBC = 0x%x\n", ret);
    ret = HI_MPI_VO_DisableWBC(0);
    fprintf(trace, "VO_DisableWBC = 0x%x\n", ret);

    {
        ret = HI_MPI_VO_SetWBCMode(0, 0);
        fprintf(trace, "VO_SetWBCMode = 0x%x\n", ret);
        VO_WBC_MODE_E mode = 0;
        ret = HI_MPI_VO_GetWBCMode(0, &mode);
        fprintf(trace, "VO_GetWBCMode = 0x%x\n", ret);
    }

    {
        ret = HI_MPI_VO_SetWBCDepth(0, 0);
        fprintf(trace, "VO_SetWBCDepth = 0x%x\n", ret);
        HI_U32 depth = 0;
        ret = HI_MPI_VO_GetWBCDepth(0, &depth);
        fprintf(trace, "VO_GetWBCDepth = 0x%x\n", ret);
    }

    {
        VIDEO_FRAME_INFO_S frm;
        memset(&frm, 0, sizeof(frm));
        ret = HI_MPI_VO_GetWBCFrame(0, &frm, 0);
        fprintf(trace, "VO_GetWBCFrame = 0x%x\n", ret);
        ret = HI_MPI_VO_ReleaseWBCFrame(0, &frm);
        fprintf(trace, "VO_ReleaseWBCFrame = 0x%x\n", ret);
    }

    /* Graphic */
    ret = HI_MPI_VO_BindGraphicLayer(0, 0);
    fprintf(trace, "VO_BindGraphicLayer = 0x%x\n", ret);
    ret = HI_MPI_VO_UnBindGraphicLayer(0, 0);
    fprintf(trace, "VO_UnBindGraphicLayer = 0x%x\n", ret);

    {
        VO_CSC_S csc;
        memset(&csc, 0, sizeof(csc));
        ret = HI_MPI_VO_SetGraphicLayerCSC(0, &csc);
        fprintf(trace, "VO_SetGraphicLayerCSC = 0x%x\n", ret);
        ret = HI_MPI_VO_GetGraphicLayerCSC(0, &csc);
        fprintf(trace, "VO_GetGraphicLayerCSC = 0x%x\n", ret);
    }

    {
        ret = HI_MPI_VO_SetDevFrameRate(0, 0);
        fprintf(trace, "VO_SetDevFrameRate = 0x%x\n", ret);
        HI_U32 fps = 0;
        ret = HI_MPI_VO_GetDevFrameRate(0, &fps);
        fprintf(trace, "VO_GetDevFrameRate = 0x%x\n", ret);
    }

    /* Module params */
    {
        VO_MOD_PARAM_S modParam;
        memset(&modParam, 0, sizeof(modParam));
        ret = HI_MPI_VO_SetModParam(&modParam);
        fprintf(trace, "VO_SetModParam = 0x%x\n", ret);
        ret = HI_MPI_VO_GetModParam(&modParam);
        fprintf(trace, "VO_GetModParam = 0x%x\n", ret);
    }

    {
        ret = HI_MPI_VO_SetVtth(0, 0);
        fprintf(trace, "VO_SetVtth = 0x%x\n", ret);
        HI_U32 vtth = 0;
        ret = HI_MPI_VO_GetVtth(0, &vtth);
        fprintf(trace, "VO_GetVtth = 0x%x\n", ret);
        ret = HI_MPI_VO_SetVtth2(0, 0);
        fprintf(trace, "VO_SetVtth2 = 0x%x\n", ret);
        ret = HI_MPI_VO_GetVtth2(0, &vtth);
        fprintf(trace, "VO_GetVtth2 = 0x%x\n", ret);
    }
}

static void test_vo_null_ptr(FILE *trace) {
    HI_S32 ret;
    fprintf(trace, "--- VO null_ptr ---\n");

    ret = HI_MPI_VO_SetPubAttr(0, NULL);
    fprintf(trace, "VO_SetPubAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_GetPubAttr(0, NULL);
    fprintf(trace, "VO_GetPubAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_SetUserIntfSyncInfo(0, NULL);
    fprintf(trace, "VO_SetUserIntfSyncInfo(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_SetVideoLayerAttr(0, NULL);
    fprintf(trace, "VO_SetVideoLayerAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_GetVideoLayerAttr(0, NULL);
    fprintf(trace, "VO_GetVideoLayerAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_GetVideoLayerPriority(0, NULL);
    fprintf(trace, "VO_GetVideoLayerPriority(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_SetVideoLayerCSC(0, NULL);
    fprintf(trace, "VO_SetVideoLayerCSC(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_GetVideoLayerCSC(0, NULL);
    fprintf(trace, "VO_GetVideoLayerCSC(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_GetVideoLayerPartitionMode(0, NULL);
    fprintf(trace, "VO_GetVideoLayerPartitionMode(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_SetVideoLayerBoundary(0, NULL);
    fprintf(trace, "VO_SetVideoLayerBoundary(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_GetVideoLayerBoundary(0, NULL);
    fprintf(trace, "VO_GetVideoLayerBoundary(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_SetVideoLayerParam(0, NULL);
    fprintf(trace, "VO_SetVideoLayerParam(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_GetVideoLayerParam(0, NULL);
    fprintf(trace, "VO_GetVideoLayerParam(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_GetVideoLayerDecompress(0, NULL);
    fprintf(trace, "VO_GetVideoLayerDecompress(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_SetVideoLayerCrop(0, NULL);
    fprintf(trace, "VO_SetVideoLayerCrop(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_GetVideoLayerCrop(0, NULL);
    fprintf(trace, "VO_GetVideoLayerCrop(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_GetPlayToleration(0, NULL);
    fprintf(trace, "VO_GetPlayToleration(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_GetScreenFrame(0, NULL, 0);
    fprintf(trace, "VO_GetScreenFrame(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_ReleaseScreenFrame(0, NULL);
    fprintf(trace, "VO_ReleaseScreenFrame(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_GetDisplayBufLen(0, NULL);
    fprintf(trace, "VO_GetDisplayBufLen(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_SetChnAttr(0, 0, NULL);
    fprintf(trace, "VO_SetChnAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_GetChnAttr(0, 0, NULL);
    fprintf(trace, "VO_GetChnAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_SetChnParam(0, 0, NULL);
    fprintf(trace, "VO_SetChnParam(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_GetChnParam(0, 0, NULL);
    fprintf(trace, "VO_GetChnParam(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_SetChnDisplayPosition(0, 0, NULL);
    fprintf(trace, "VO_SetChnDisplayPosition(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_GetChnDisplayPosition(0, 0, NULL);
    fprintf(trace, "VO_GetChnDisplayPosition(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_GetChnFrameRate(0, 0, NULL);
    fprintf(trace, "VO_GetChnFrameRate(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_GetChnFrame(0, 0, NULL, 0);
    fprintf(trace, "VO_GetChnFrame(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_ReleaseChnFrame(0, 0, NULL);
    fprintf(trace, "VO_ReleaseChnFrame(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_SetZoomInWindow(0, 0, NULL);
    fprintf(trace, "VO_SetZoomInWindow(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_GetZoomInWindow(0, 0, NULL);
    fprintf(trace, "VO_GetZoomInWindow(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_GetChnPTS(0, 0, NULL);
    fprintf(trace, "VO_GetChnPTS(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_QueryChnStatus(0, 0, NULL);
    fprintf(trace, "VO_QueryChnStatus(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_SendFrame(0, 0, NULL, 0);
    fprintf(trace, "VO_SendFrame(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_SetChnBorder(0, 0, NULL);
    fprintf(trace, "VO_SetChnBorder(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_GetChnBorder(0, 0, NULL);
    fprintf(trace, "VO_GetChnBorder(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_SetChnBoundary(0, 0, NULL);
    fprintf(trace, "VO_SetChnBoundary(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_GetChnBoundary(0, 0, NULL);
    fprintf(trace, "VO_GetChnBoundary(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_GetChnRecvThreshold(0, 0, NULL);
    fprintf(trace, "VO_GetChnRecvThreshold(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_GetChnRotation(0, 0, NULL);
    fprintf(trace, "VO_GetChnRotation(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_GetChnRegionLuma(0, 0, NULL, NULL, 0);
    fprintf(trace, "VO_GetChnRegionLuma(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_SetWBCSource(0, NULL);
    fprintf(trace, "VO_SetWBCSource(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_GetWBCSource(0, NULL);
    fprintf(trace, "VO_GetWBCSource(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_SetWBCAttr(0, NULL);
    fprintf(trace, "VO_SetWBCAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_GetWBCAttr(0, NULL);
    fprintf(trace, "VO_GetWBCAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_GetWBCMode(0, NULL);
    fprintf(trace, "VO_GetWBCMode(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_GetWBCDepth(0, NULL);
    fprintf(trace, "VO_GetWBCDepth(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_GetWBCFrame(0, NULL, 0);
    fprintf(trace, "VO_GetWBCFrame(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_ReleaseWBCFrame(0, NULL);
    fprintf(trace, "VO_ReleaseWBCFrame(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_SetGraphicLayerCSC(0, NULL);
    fprintf(trace, "VO_SetGraphicLayerCSC(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_GetGraphicLayerCSC(0, NULL);
    fprintf(trace, "VO_GetGraphicLayerCSC(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_GetDevFrameRate(0, NULL);
    fprintf(trace, "VO_GetDevFrameRate(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_SetModParam(NULL);
    fprintf(trace, "VO_SetModParam(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_GetModParam(NULL);
    fprintf(trace, "VO_GetModParam(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_GetVtth(0, NULL);
    fprintf(trace, "VO_GetVtth(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VO_GetVtth2(0, NULL);
    fprintf(trace, "VO_GetVtth2(NULL) = 0x%x\n", ret);
}

/* ========================================================================
 *  VPSS module  (64 functions)
 * ======================================================================== */

static void test_vpss_basic(FILE *trace) {
    HI_S32 ret;
    fprintf(trace, "--- VPSS basic ---\n");

    /* Group */
    {
        VPSS_GRP_ATTR_S grpAttr;
        memset(&grpAttr, 0, sizeof(grpAttr));
        ret = HI_MPI_VPSS_CreateGrp(0, &grpAttr);
        fprintf(trace, "VPSS_CreateGrp = 0x%x\n", ret);
        ret = HI_MPI_VPSS_DestroyGrp(0);
        fprintf(trace, "VPSS_DestroyGrp = 0x%x\n", ret);
    }

    ret = HI_MPI_VPSS_StartGrp(0);
    fprintf(trace, "VPSS_StartGrp = 0x%x\n", ret);
    ret = HI_MPI_VPSS_StopGrp(0);
    fprintf(trace, "VPSS_StopGrp = 0x%x\n", ret);
    ret = HI_MPI_VPSS_ResetGrp(0);
    fprintf(trace, "VPSS_ResetGrp = 0x%x\n", ret);

    {
        VPSS_GRP_ATTR_S grpAttr;
        memset(&grpAttr, 0, sizeof(grpAttr));
        ret = HI_MPI_VPSS_GetGrpAttr(0, &grpAttr);
        fprintf(trace, "VPSS_GetGrpAttr = 0x%x\n", ret);
        ret = HI_MPI_VPSS_SetGrpAttr(0, &grpAttr);
        fprintf(trace, "VPSS_SetGrpAttr = 0x%x\n", ret);
    }

    {
        VPSS_CROP_INFO_S crop;
        memset(&crop, 0, sizeof(crop));
        ret = HI_MPI_VPSS_SetGrpCrop(0, &crop);
        fprintf(trace, "VPSS_SetGrpCrop = 0x%x\n", ret);
        ret = HI_MPI_VPSS_GetGrpCrop(0, &crop);
        fprintf(trace, "VPSS_GetGrpCrop = 0x%x\n", ret);
    }

    {
        VIDEO_FRAME_INFO_S frm;
        memset(&frm, 0, sizeof(frm));
        ret = HI_MPI_VPSS_SendFrame(0, 0, &frm, 0);
        fprintf(trace, "VPSS_SendFrame = 0x%x\n", ret);
        ret = HI_MPI_VPSS_GetGrpFrame(0, 0, &frm);
        fprintf(trace, "VPSS_GetGrpFrame = 0x%x\n", ret);
        ret = HI_MPI_VPSS_ReleaseGrpFrame(0, 0, &frm);
        fprintf(trace, "VPSS_ReleaseGrpFrame = 0x%x\n", ret);
    }

    ret = HI_MPI_VPSS_EnableBackupFrame(0);
    fprintf(trace, "VPSS_EnableBackupFrame = 0x%x\n", ret);
    ret = HI_MPI_VPSS_DisableBackupFrame(0);
    fprintf(trace, "VPSS_DisableBackupFrame = 0x%x\n", ret);

    {
        VPSS_GRP_SHARPEN_ATTR_S sharpen;
        memset(&sharpen, 0, sizeof(sharpen));
        ret = HI_MPI_VPSS_SetGrpSharpen(0, &sharpen);
        fprintf(trace, "VPSS_SetGrpSharpen = 0x%x\n", ret);
        ret = HI_MPI_VPSS_GetGrpSharpen(0, &sharpen);
        fprintf(trace, "VPSS_GetGrpSharpen = 0x%x\n", ret);
    }

    {
        ret = HI_MPI_VPSS_SetGrpDelay(0, 0);
        fprintf(trace, "VPSS_SetGrpDelay = 0x%x\n", ret);
        HI_U32 delay = 0;
        ret = HI_MPI_VPSS_GetGrpDelay(0, &delay);
        fprintf(trace, "VPSS_GetGrpDelay = 0x%x\n", ret);
    }

    {
        FISHEYE_CONFIG_S fishCfg;
        memset(&fishCfg, 0, sizeof(fishCfg));
        ret = HI_MPI_VPSS_SetGrpFisheyeConfig(0, &fishCfg);
        fprintf(trace, "VPSS_SetGrpFisheyeConfig = 0x%x\n", ret);
        ret = HI_MPI_VPSS_GetGrpFisheyeConfig(0, &fishCfg);
        fprintf(trace, "VPSS_GetGrpFisheyeConfig = 0x%x\n", ret);
    }

    ret = HI_MPI_VPSS_EnableUserFrameRateCtrl(0);
    fprintf(trace, "VPSS_EnableUserFrameRateCtrl = 0x%x\n", ret);
    ret = HI_MPI_VPSS_DisableUserFrameRateCtrl(0);
    fprintf(trace, "VPSS_DisableUserFrameRateCtrl = 0x%x\n", ret);

    {
        FRAME_INTERRUPT_ATTR_S frmInt;
        memset(&frmInt, 0, sizeof(frmInt));
        ret = HI_MPI_VPSS_SetGrpFrameInterruptAttr(0, &frmInt);
        fprintf(trace, "VPSS_SetGrpFrameInterruptAttr = 0x%x\n", ret);
        ret = HI_MPI_VPSS_GetGrpFrameInterruptAttr(0, &frmInt);
        fprintf(trace, "VPSS_GetGrpFrameInterruptAttr = 0x%x\n", ret);
    }

    /* Channel */
    {
        VPSS_CHN_ATTR_S chnAttr;
        memset(&chnAttr, 0, sizeof(chnAttr));
        ret = HI_MPI_VPSS_SetChnAttr(0, 0, &chnAttr);
        fprintf(trace, "VPSS_SetChnAttr = 0x%x\n", ret);
        ret = HI_MPI_VPSS_GetChnAttr(0, 0, &chnAttr);
        fprintf(trace, "VPSS_GetChnAttr = 0x%x\n", ret);
    }

    ret = HI_MPI_VPSS_EnableChn(0, 0);
    fprintf(trace, "VPSS_EnableChn = 0x%x\n", ret);
    ret = HI_MPI_VPSS_DisableChn(0, 0);
    fprintf(trace, "VPSS_DisableChn = 0x%x\n", ret);

    {
        VPSS_CROP_INFO_S crop;
        memset(&crop, 0, sizeof(crop));
        ret = HI_MPI_VPSS_SetChnCrop(0, 0, &crop);
        fprintf(trace, "VPSS_SetChnCrop = 0x%x\n", ret);
        ret = HI_MPI_VPSS_GetChnCrop(0, 0, &crop);
        fprintf(trace, "VPSS_GetChnCrop = 0x%x\n", ret);
    }

    {
        ret = HI_MPI_VPSS_SetChnRotation(0, 0, 0);
        fprintf(trace, "VPSS_SetChnRotation = 0x%x\n", ret);
        ROTATION_E rot = 0;
        ret = HI_MPI_VPSS_GetChnRotation(0, 0, &rot);
        fprintf(trace, "VPSS_GetChnRotation = 0x%x\n", ret);
    }

    {
        VPSS_ROTATION_EX_ATTR_S rotEx;
        memset(&rotEx, 0, sizeof(rotEx));
        ret = HI_MPI_VPSS_SetChnRotationEx(0, 0, &rotEx);
        fprintf(trace, "VPSS_SetChnRotationEx = 0x%x\n", ret);
        ret = HI_MPI_VPSS_GetChnRotationEx(0, 0, &rotEx);
        fprintf(trace, "VPSS_GetChnRotationEx = 0x%x\n", ret);
    }

    {
        VPSS_LDC_ATTR_S ldc;
        memset(&ldc, 0, sizeof(ldc));
        ret = HI_MPI_VPSS_SetChnLDCAttr(0, 0, &ldc);
        fprintf(trace, "VPSS_SetChnLDCAttr = 0x%x\n", ret);
        ret = HI_MPI_VPSS_GetChnLDCAttr(0, 0, &ldc);
        fprintf(trace, "VPSS_GetChnLDCAttr = 0x%x\n", ret);
    }

    {
        VPSS_LDCV3_ATTR_S ldcv3;
        memset(&ldcv3, 0, sizeof(ldcv3));
        ret = HI_MPI_VPSS_SetChnLDCV3Attr(0, 0, &ldcv3);
        fprintf(trace, "VPSS_SetChnLDCV3Attr = 0x%x\n", ret);
        ret = HI_MPI_VPSS_GetChnLDCV3Attr(0, 0, &ldcv3);
        fprintf(trace, "VPSS_GetChnLDCV3Attr = 0x%x\n", ret);
    }

    {
        SPREAD_ATTR_S spread;
        memset(&spread, 0, sizeof(spread));
        ret = HI_MPI_VPSS_SetChnSpreadAttr(0, 0, &spread);
        fprintf(trace, "VPSS_SetChnSpreadAttr = 0x%x\n", ret);
        ret = HI_MPI_VPSS_GetChnSpreadAttr(0, 0, &spread);
        fprintf(trace, "VPSS_GetChnSpreadAttr = 0x%x\n", ret);
    }

    {
        VIDEO_FRAME_INFO_S frm;
        memset(&frm, 0, sizeof(frm));
        ret = HI_MPI_VPSS_GetChnFrame(0, 0, &frm, 0);
        fprintf(trace, "VPSS_GetChnFrame = 0x%x\n", ret);
        ret = HI_MPI_VPSS_ReleaseChnFrame(0, 0, &frm);
        fprintf(trace, "VPSS_ReleaseChnFrame = 0x%x\n", ret);
    }

    {
        VIDEO_REGION_INFO_S regInfo;
        memset(&regInfo, 0, sizeof(regInfo));
        HI_U64 lumaData = 0;
        ret = HI_MPI_VPSS_GetRegionLuma(0, 0, &regInfo, &lumaData, 0);
        fprintf(trace, "VPSS_GetRegionLuma = 0x%x\n", ret);
    }

    {
        VPSS_LOW_DELAY_INFO_S lowDelay;
        memset(&lowDelay, 0, sizeof(lowDelay));
        ret = HI_MPI_VPSS_SetLowDelayAttr(0, 0, &lowDelay);
        fprintf(trace, "VPSS_SetLowDelayAttr = 0x%x\n", ret);
        ret = HI_MPI_VPSS_GetLowDelayAttr(0, 0, &lowDelay);
        fprintf(trace, "VPSS_GetLowDelayAttr = 0x%x\n", ret);
    }

    {
        VPSS_CHN_BUF_WRAP_S bufWrap;
        memset(&bufWrap, 0, sizeof(bufWrap));
        ret = HI_MPI_VPSS_SetChnBufWrapAttr(0, 0, &bufWrap);
        fprintf(trace, "VPSS_SetChnBufWrapAttr = 0x%x\n", ret);
        ret = HI_MPI_VPSS_GetChnBufWrapAttr(0, 0, &bufWrap);
        fprintf(trace, "VPSS_GetChnBufWrapAttr = 0x%x\n", ret);
    }

    ret = HI_MPI_VPSS_TriggerSnapFrame(0, 0, 1);
    fprintf(trace, "VPSS_TriggerSnapFrame = 0x%x\n", ret);

    ret = HI_MPI_VPSS_AttachVbPool(0, 0, 0);
    fprintf(trace, "VPSS_AttachVbPool = 0x%x\n", ret);
    ret = HI_MPI_VPSS_DetachVbPool(0, 0);
    fprintf(trace, "VPSS_DetachVbPool = 0x%x\n", ret);

    ret = HI_MPI_VPSS_EnableBufferShare(0, 0);
    fprintf(trace, "VPSS_EnableBufferShare = 0x%x\n", ret);
    ret = HI_MPI_VPSS_DisableBufferShare(0, 0);
    fprintf(trace, "VPSS_DisableBufferShare = 0x%x\n", ret);

    {
        ret = HI_MPI_VPSS_SetChnAlign(0, 0, 0);
        fprintf(trace, "VPSS_SetChnAlign = 0x%x\n", ret);
        HI_U32 align = 0;
        ret = HI_MPI_VPSS_GetChnAlign(0, 0, &align);
        fprintf(trace, "VPSS_GetChnAlign = 0x%x\n", ret);
    }

    {
        ret = HI_MPI_VPSS_SetChnProcMode(0, 0, 0);
        fprintf(trace, "VPSS_SetChnProcMode = 0x%x\n", ret);
        VPSS_CHN_PROC_MODE_E mode = 0;
        ret = HI_MPI_VPSS_GetChnProcMode(0, 0, &mode);
        fprintf(trace, "VPSS_GetChnProcMode = 0x%x\n", ret);
    }

    /* ExtChn */
    {
        VPSS_EXT_CHN_ATTR_S extAttr;
        memset(&extAttr, 0, sizeof(extAttr));
        ret = HI_MPI_VPSS_SetExtChnAttr(0, 0, &extAttr);
        fprintf(trace, "VPSS_SetExtChnAttr = 0x%x\n", ret);
        ret = HI_MPI_VPSS_GetExtChnAttr(0, 0, &extAttr);
        fprintf(trace, "VPSS_GetExtChnAttr = 0x%x\n", ret);
    }

    {
        FISHEYE_ATTR_S fishAttr;
        memset(&fishAttr, 0, sizeof(fishAttr));
        ret = HI_MPI_VPSS_SetExtChnFisheye(0, 0, &fishAttr);
        fprintf(trace, "VPSS_SetExtChnFisheye = 0x%x\n", ret);
        ret = HI_MPI_VPSS_GetExtChnFisheye(0, 0, &fishAttr);
        fprintf(trace, "VPSS_GetExtChnFisheye = 0x%x\n", ret);
    }

    {
        POINT_S dst = {0, 0}, src = {0, 0};
        ret = HI_MPI_VPSS_FisheyePosQueryDst2Src(0, 0, 0, &dst, &src);
        fprintf(trace, "VPSS_FisheyePosQueryDst2Src = 0x%x\n", ret);
    }

    /* 3DNR */
    {
        VPSS_GRP_NRX_PARAM_S nrx;
        memset(&nrx, 0, sizeof(nrx));
        ret = HI_MPI_VPSS_SetGrpNRXParam(0, &nrx);
        fprintf(trace, "VPSS_SetGrpNRXParam = 0x%x\n", ret);
        ret = HI_MPI_VPSS_GetGrpNRXParam(0, &nrx);
        fprintf(trace, "VPSS_GetGrpNRXParam = 0x%x\n", ret);
    }

    /* Module */
    {
        VPSS_MOD_PARAM_S modParam;
        memset(&modParam, 0, sizeof(modParam));
        ret = HI_MPI_VPSS_SetModParam(&modParam);
        fprintf(trace, "VPSS_SetModParam = 0x%x\n", ret);
        ret = HI_MPI_VPSS_GetModParam(&modParam);
        fprintf(trace, "VPSS_GetModParam = 0x%x\n", ret);
    }

    ret = HI_MPI_VPSS_GetChnFd(0, 0);
    fprintf(trace, "VPSS_GetChnFd = 0x%x\n", ret);
    ret = HI_MPI_VPSS_CloseFd();
    fprintf(trace, "VPSS_CloseFd = 0x%x\n", ret);
}

static void test_vpss_null_ptr(FILE *trace) {
    HI_S32 ret;
    fprintf(trace, "--- VPSS null_ptr ---\n");

    ret = HI_MPI_VPSS_CreateGrp(0, NULL);
    fprintf(trace, "VPSS_CreateGrp(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VPSS_GetGrpAttr(0, NULL);
    fprintf(trace, "VPSS_GetGrpAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VPSS_SetGrpAttr(0, NULL);
    fprintf(trace, "VPSS_SetGrpAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VPSS_SetGrpCrop(0, NULL);
    fprintf(trace, "VPSS_SetGrpCrop(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VPSS_GetGrpCrop(0, NULL);
    fprintf(trace, "VPSS_GetGrpCrop(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VPSS_SendFrame(0, 0, NULL, 0);
    fprintf(trace, "VPSS_SendFrame(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VPSS_GetGrpFrame(0, 0, NULL);
    fprintf(trace, "VPSS_GetGrpFrame(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VPSS_ReleaseGrpFrame(0, 0, NULL);
    fprintf(trace, "VPSS_ReleaseGrpFrame(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VPSS_SetGrpSharpen(0, NULL);
    fprintf(trace, "VPSS_SetGrpSharpen(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VPSS_GetGrpSharpen(0, NULL);
    fprintf(trace, "VPSS_GetGrpSharpen(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VPSS_GetGrpDelay(0, NULL);
    fprintf(trace, "VPSS_GetGrpDelay(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VPSS_SetGrpFisheyeConfig(0, NULL);
    fprintf(trace, "VPSS_SetGrpFisheyeConfig(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VPSS_GetGrpFisheyeConfig(0, NULL);
    fprintf(trace, "VPSS_GetGrpFisheyeConfig(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VPSS_SetGrpFrameInterruptAttr(0, NULL);
    fprintf(trace, "VPSS_SetGrpFrameInterruptAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VPSS_GetGrpFrameInterruptAttr(0, NULL);
    fprintf(trace, "VPSS_GetGrpFrameInterruptAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VPSS_SetChnAttr(0, 0, NULL);
    fprintf(trace, "VPSS_SetChnAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VPSS_GetChnAttr(0, 0, NULL);
    fprintf(trace, "VPSS_GetChnAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VPSS_SetChnCrop(0, 0, NULL);
    fprintf(trace, "VPSS_SetChnCrop(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VPSS_GetChnCrop(0, 0, NULL);
    fprintf(trace, "VPSS_GetChnCrop(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VPSS_GetChnRotation(0, 0, NULL);
    fprintf(trace, "VPSS_GetChnRotation(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VPSS_SetChnRotationEx(0, 0, NULL);
    fprintf(trace, "VPSS_SetChnRotationEx(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VPSS_GetChnRotationEx(0, 0, NULL);
    fprintf(trace, "VPSS_GetChnRotationEx(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VPSS_SetChnLDCAttr(0, 0, NULL);
    fprintf(trace, "VPSS_SetChnLDCAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VPSS_GetChnLDCAttr(0, 0, NULL);
    fprintf(trace, "VPSS_GetChnLDCAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VPSS_SetChnLDCV3Attr(0, 0, NULL);
    fprintf(trace, "VPSS_SetChnLDCV3Attr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VPSS_GetChnLDCV3Attr(0, 0, NULL);
    fprintf(trace, "VPSS_GetChnLDCV3Attr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VPSS_SetChnSpreadAttr(0, 0, NULL);
    fprintf(trace, "VPSS_SetChnSpreadAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VPSS_GetChnSpreadAttr(0, 0, NULL);
    fprintf(trace, "VPSS_GetChnSpreadAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VPSS_GetChnFrame(0, 0, NULL, 0);
    fprintf(trace, "VPSS_GetChnFrame(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VPSS_ReleaseChnFrame(0, 0, NULL);
    fprintf(trace, "VPSS_ReleaseChnFrame(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VPSS_GetRegionLuma(0, 0, NULL, NULL, 0);
    fprintf(trace, "VPSS_GetRegionLuma(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VPSS_SetLowDelayAttr(0, 0, NULL);
    fprintf(trace, "VPSS_SetLowDelayAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VPSS_GetLowDelayAttr(0, 0, NULL);
    fprintf(trace, "VPSS_GetLowDelayAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VPSS_SetChnBufWrapAttr(0, 0, NULL);
    fprintf(trace, "VPSS_SetChnBufWrapAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VPSS_GetChnBufWrapAttr(0, 0, NULL);
    fprintf(trace, "VPSS_GetChnBufWrapAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VPSS_GetChnAlign(0, 0, NULL);
    fprintf(trace, "VPSS_GetChnAlign(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VPSS_GetChnProcMode(0, 0, NULL);
    fprintf(trace, "VPSS_GetChnProcMode(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VPSS_SetExtChnAttr(0, 0, NULL);
    fprintf(trace, "VPSS_SetExtChnAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VPSS_GetExtChnAttr(0, 0, NULL);
    fprintf(trace, "VPSS_GetExtChnAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VPSS_SetExtChnFisheye(0, 0, NULL);
    fprintf(trace, "VPSS_SetExtChnFisheye(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VPSS_GetExtChnFisheye(0, 0, NULL);
    fprintf(trace, "VPSS_GetExtChnFisheye(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VPSS_FisheyePosQueryDst2Src(0, 0, 0, NULL, NULL);
    fprintf(trace, "VPSS_FisheyePosQueryDst2Src(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VPSS_SetGrpNRXParam(0, NULL);
    fprintf(trace, "VPSS_SetGrpNRXParam(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VPSS_GetGrpNRXParam(0, NULL);
    fprintf(trace, "VPSS_GetGrpNRXParam(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VPSS_SetModParam(NULL);
    fprintf(trace, "VPSS_SetModParam(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VPSS_GetModParam(NULL);
    fprintf(trace, "VPSS_GetModParam(NULL) = 0x%x\n", ret);
}

/* ========================================================================
 *  VENC module  (70 functions)
 * ======================================================================== */

static void test_venc_basic(FILE *trace) {
    HI_S32 ret;
    fprintf(trace, "--- VENC basic ---\n");

    {
        VENC_CHN_ATTR_S attr;
        memset(&attr, 0, sizeof(attr));
        ret = HI_MPI_VENC_CreateChn(0, &attr);
        fprintf(trace, "VENC_CreateChn = 0x%x\n", ret);
        ret = HI_MPI_VENC_DestroyChn(0);
        fprintf(trace, "VENC_DestroyChn = 0x%x\n", ret);
    }

    ret = HI_MPI_VENC_ResetChn(0);
    fprintf(trace, "VENC_ResetChn = 0x%x\n", ret);

    {
        VENC_RECV_PIC_PARAM_S recvParam;
        memset(&recvParam, 0, sizeof(recvParam));
        ret = HI_MPI_VENC_StartRecvFrame(0, &recvParam);
        fprintf(trace, "VENC_StartRecvFrame = 0x%x\n", ret);
        ret = HI_MPI_VENC_StopRecvFrame(0);
        fprintf(trace, "VENC_StopRecvFrame = 0x%x\n", ret);
    }

    {
        VENC_CHN_STATUS_S status;
        memset(&status, 0, sizeof(status));
        ret = HI_MPI_VENC_QueryStatus(0, &status);
        fprintf(trace, "VENC_QueryStatus = 0x%x\n", ret);
    }

    {
        VENC_CHN_ATTR_S attr;
        memset(&attr, 0, sizeof(attr));
        ret = HI_MPI_VENC_SetChnAttr(0, &attr);
        fprintf(trace, "VENC_SetChnAttr = 0x%x\n", ret);
        ret = HI_MPI_VENC_GetChnAttr(0, &attr);
        fprintf(trace, "VENC_GetChnAttr = 0x%x\n", ret);
    }

    {
        VENC_STREAM_S stream;
        memset(&stream, 0, sizeof(stream));
        ret = HI_MPI_VENC_GetStream(0, &stream, 0);
        fprintf(trace, "VENC_GetStream = 0x%x\n", ret);
        ret = HI_MPI_VENC_ReleaseStream(0, &stream);
        fprintf(trace, "VENC_ReleaseStream = 0x%x\n", ret);
    }

    {
        HI_U8 data[4] = {0};
        ret = HI_MPI_VENC_InsertUserData(0, data, sizeof(data));
        fprintf(trace, "VENC_InsertUserData = 0x%x\n", ret);
    }

    {
        VIDEO_FRAME_INFO_S frm;
        memset(&frm, 0, sizeof(frm));
        ret = HI_MPI_VENC_SendFrame(0, &frm, 0);
        fprintf(trace, "VENC_SendFrame = 0x%x\n", ret);
    }

    {
        USER_FRAME_INFO_S ufrm;
        memset(&ufrm, 0, sizeof(ufrm));
        ret = HI_MPI_VENC_SendFrameEx(0, &ufrm, 0);
        fprintf(trace, "VENC_SendFrameEx = 0x%x\n", ret);
    }

    ret = HI_MPI_VENC_RequestIDR(0, HI_FALSE);
    fprintf(trace, "VENC_RequestIDR = 0x%x\n", ret);

    ret = HI_MPI_VENC_GetFd(0);
    fprintf(trace, "VENC_GetFd = 0x%x\n", ret);
    ret = HI_MPI_VENC_CloseFd(0);
    fprintf(trace, "VENC_CloseFd = 0x%x\n", ret);

    {
        VENC_ROI_ATTR_S roi;
        memset(&roi, 0, sizeof(roi));
        ret = HI_MPI_VENC_SetRoiAttr(0, &roi);
        fprintf(trace, "VENC_SetRoiAttr = 0x%x\n", ret);
        ret = HI_MPI_VENC_GetRoiAttr(0, 0, &roi);
        fprintf(trace, "VENC_GetRoiAttr = 0x%x\n", ret);
    }

    {
        VENC_ROI_ATTR_EX_S roiEx;
        memset(&roiEx, 0, sizeof(roiEx));
        ret = HI_MPI_VENC_SetRoiAttrEx(0, &roiEx);
        fprintf(trace, "VENC_SetRoiAttrEx = 0x%x\n", ret);
        ret = HI_MPI_VENC_GetRoiAttrEx(0, 0, &roiEx);
        fprintf(trace, "VENC_GetRoiAttrEx = 0x%x\n", ret);
    }

    {
        VENC_ROIBG_FRAME_RATE_S roiBg;
        memset(&roiBg, 0, sizeof(roiBg));
        ret = HI_MPI_VENC_SetRoiBgFrameRate(0, &roiBg);
        fprintf(trace, "VENC_SetRoiBgFrameRate = 0x%x\n", ret);
        ret = HI_MPI_VENC_GetRoiBgFrameRate(0, &roiBg);
        fprintf(trace, "VENC_GetRoiBgFrameRate = 0x%x\n", ret);
    }

    /* H264 specific */
    {
        VENC_H264_SLICE_SPLIT_S ss;  memset(&ss, 0, sizeof(ss));
        ret = HI_MPI_VENC_SetH264SliceSplit(0, &ss);   fprintf(trace, "VENC_SetH264SliceSplit = 0x%x\n", ret);
        ret = HI_MPI_VENC_GetH264SliceSplit(0, &ss);   fprintf(trace, "VENC_GetH264SliceSplit = 0x%x\n", ret);
    }
    {
        VENC_H264_INTRA_PRED_S ip;  memset(&ip, 0, sizeof(ip));
        ret = HI_MPI_VENC_SetH264IntraPred(0, &ip);    fprintf(trace, "VENC_SetH264IntraPred = 0x%x\n", ret);
        ret = HI_MPI_VENC_GetH264IntraPred(0, &ip);    fprintf(trace, "VENC_GetH264IntraPred = 0x%x\n", ret);
    }
    {
        VENC_H264_TRANS_S tr;  memset(&tr, 0, sizeof(tr));
        ret = HI_MPI_VENC_SetH264Trans(0, &tr);        fprintf(trace, "VENC_SetH264Trans = 0x%x\n", ret);
        ret = HI_MPI_VENC_GetH264Trans(0, &tr);        fprintf(trace, "VENC_GetH264Trans = 0x%x\n", ret);
    }
    {
        VENC_H264_ENTROPY_S en;  memset(&en, 0, sizeof(en));
        ret = HI_MPI_VENC_SetH264Entropy(0, &en);      fprintf(trace, "VENC_SetH264Entropy = 0x%x\n", ret);
        ret = HI_MPI_VENC_GetH264Entropy(0, &en);      fprintf(trace, "VENC_GetH264Entropy = 0x%x\n", ret);
    }
    {
        VENC_H264_DBLK_S db;  memset(&db, 0, sizeof(db));
        ret = HI_MPI_VENC_SetH264Dblk(0, &db);        fprintf(trace, "VENC_SetH264Dblk = 0x%x\n", ret);
        ret = HI_MPI_VENC_GetH264Dblk(0, &db);        fprintf(trace, "VENC_GetH264Dblk = 0x%x\n", ret);
    }
    {
        VENC_H264_VUI_S vui;  memset(&vui, 0, sizeof(vui));
        ret = HI_MPI_VENC_SetH264Vui(0, &vui);        fprintf(trace, "VENC_SetH264Vui = 0x%x\n", ret);
        ret = HI_MPI_VENC_GetH264Vui(0, &vui);        fprintf(trace, "VENC_GetH264Vui = 0x%x\n", ret);
    }

    /* H265 specific */
    {
        VENC_H265_VUI_S vui;  memset(&vui, 0, sizeof(vui));
        ret = HI_MPI_VENC_SetH265Vui(0, &vui);        fprintf(trace, "VENC_SetH265Vui = 0x%x\n", ret);
        ret = HI_MPI_VENC_GetH265Vui(0, &vui);        fprintf(trace, "VENC_GetH265Vui = 0x%x\n", ret);
    }
    {
        VENC_H265_SLICE_SPLIT_S ss;  memset(&ss, 0, sizeof(ss));
        ret = HI_MPI_VENC_SetH265SliceSplit(0, &ss);   fprintf(trace, "VENC_SetH265SliceSplit = 0x%x\n", ret);
        ret = HI_MPI_VENC_GetH265SliceSplit(0, &ss);   fprintf(trace, "VENC_GetH265SliceSplit = 0x%x\n", ret);
    }
    {
        VENC_H265_PU_S pu;  memset(&pu, 0, sizeof(pu));
        ret = HI_MPI_VENC_SetH265PredUnit(0, &pu);     fprintf(trace, "VENC_SetH265PredUnit = 0x%x\n", ret);
        ret = HI_MPI_VENC_GetH265PredUnit(0, &pu);     fprintf(trace, "VENC_GetH265PredUnit = 0x%x\n", ret);
    }
    {
        VENC_H265_TRANS_S tr;  memset(&tr, 0, sizeof(tr));
        ret = HI_MPI_VENC_SetH265Trans(0, &tr);        fprintf(trace, "VENC_SetH265Trans = 0x%x\n", ret);
        ret = HI_MPI_VENC_GetH265Trans(0, &tr);        fprintf(trace, "VENC_GetH265Trans = 0x%x\n", ret);
    }
    {
        VENC_H265_ENTROPY_S en;  memset(&en, 0, sizeof(en));
        ret = HI_MPI_VENC_SetH265Entropy(0, &en);      fprintf(trace, "VENC_SetH265Entropy = 0x%x\n", ret);
        ret = HI_MPI_VENC_GetH265Entropy(0, &en);      fprintf(trace, "VENC_GetH265Entropy = 0x%x\n", ret);
    }
    {
        VENC_H265_DBLK_S db;  memset(&db, 0, sizeof(db));
        ret = HI_MPI_VENC_SetH265Dblk(0, &db);        fprintf(trace, "VENC_SetH265Dblk = 0x%x\n", ret);
        ret = HI_MPI_VENC_GetH265Dblk(0, &db);        fprintf(trace, "VENC_GetH265Dblk = 0x%x\n", ret);
    }
    {
        VENC_H265_SAO_S sao;  memset(&sao, 0, sizeof(sao));
        ret = HI_MPI_VENC_SetH265Sao(0, &sao);        fprintf(trace, "VENC_SetH265Sao = 0x%x\n", ret);
        ret = HI_MPI_VENC_GetH265Sao(0, &sao);        fprintf(trace, "VENC_GetH265Sao = 0x%x\n", ret);
    }

    /* JPEG/MJPEG */
    {
        VENC_JPEG_PARAM_S jp;  memset(&jp, 0, sizeof(jp));
        ret = HI_MPI_VENC_SetJpegParam(0, &jp);        fprintf(trace, "VENC_SetJpegParam = 0x%x\n", ret);
        ret = HI_MPI_VENC_GetJpegParam(0, &jp);        fprintf(trace, "VENC_GetJpegParam = 0x%x\n", ret);
    }
    {
        VENC_MJPEG_PARAM_S mj;  memset(&mj, 0, sizeof(mj));
        ret = HI_MPI_VENC_SetMjpegParam(0, &mj);      fprintf(trace, "VENC_SetMjpegParam = 0x%x\n", ret);
        ret = HI_MPI_VENC_GetMjpegParam(0, &mj);      fprintf(trace, "VENC_GetMjpegParam = 0x%x\n", ret);
    }

    /* RC */
    {
        VENC_RC_PARAM_S rc;  memset(&rc, 0, sizeof(rc));
        ret = HI_MPI_VENC_SetRcParam(0, &rc);          fprintf(trace, "VENC_SetRcParam = 0x%x\n", ret);
        ret = HI_MPI_VENC_GetRcParam(0, &rc);          fprintf(trace, "VENC_GetRcParam = 0x%x\n", ret);
    }
    {
        VENC_REF_PARAM_S ref;  memset(&ref, 0, sizeof(ref));
        ret = HI_MPI_VENC_SetRefParam(0, &ref);        fprintf(trace, "VENC_SetRefParam = 0x%x\n", ret);
        ret = HI_MPI_VENC_GetRefParam(0, &ref);        fprintf(trace, "VENC_GetRefParam = 0x%x\n", ret);
    }

    {
        ret = HI_MPI_VENC_SetJpegEncodeMode(0, 0);     fprintf(trace, "VENC_SetJpegEncodeMode = 0x%x\n", ret);
        VENC_JPEG_ENCODE_MODE_E mode = 0;
        ret = HI_MPI_VENC_GetJpegEncodeMode(0, &mode); fprintf(trace, "VENC_GetJpegEncodeMode = 0x%x\n", ret);
    }

    ret = HI_MPI_VENC_EnableIDR(0, HI_FALSE);
    fprintf(trace, "VENC_EnableIDR = 0x%x\n", ret);

    {
        VENC_STREAM_BUF_INFO_S bufInfo;  memset(&bufInfo, 0, sizeof(bufInfo));
        ret = HI_MPI_VENC_GetStreamBufInfo(0, &bufInfo);
        fprintf(trace, "VENC_GetStreamBufInfo = 0x%x\n", ret);
    }

    /* Remaining VENC functions */
    {
        VENC_FRAMELOST_S fl;  memset(&fl, 0, sizeof(fl));
        ret = HI_MPI_VENC_SetFrameLostStrategy(0, &fl);    fprintf(trace, "VENC_SetFrameLostStrategy = 0x%x\n", ret);
        ret = HI_MPI_VENC_GetFrameLostStrategy(0, &fl);    fprintf(trace, "VENC_GetFrameLostStrategy = 0x%x\n", ret);
    }
    {
        VENC_SUPERFRAME_CFG_S sf;  memset(&sf, 0, sizeof(sf));
        ret = HI_MPI_VENC_SetSuperFrameStrategy(0, &sf);   fprintf(trace, "VENC_SetSuperFrameStrategy = 0x%x\n", ret);
        ret = HI_MPI_VENC_GetSuperFrameStrategy(0, &sf);   fprintf(trace, "VENC_GetSuperFrameStrategy = 0x%x\n", ret);
    }
    {
        VENC_INTRA_REFRESH_S ir;  memset(&ir, 0, sizeof(ir));
        ret = HI_MPI_VENC_SetIntraRefresh(0, &ir);         fprintf(trace, "VENC_SetIntraRefresh = 0x%x\n", ret);
        ret = HI_MPI_VENC_GetIntraRefresh(0, &ir);         fprintf(trace, "VENC_GetIntraRefresh = 0x%x\n", ret);
    }
    {
        VENC_SSE_CFG_S sse;  memset(&sse, 0, sizeof(sse));
        ret = HI_MPI_VENC_SetSSERegion(0, &sse);           fprintf(trace, "VENC_SetSSERegion = 0x%x\n", ret);
        ret = HI_MPI_VENC_GetSSERegion(0, 0, &sse);        fprintf(trace, "VENC_GetSSERegion = 0x%x\n", ret);
    }
    {
        VENC_CHN_PARAM_S cp;  memset(&cp, 0, sizeof(cp));
        ret = HI_MPI_VENC_SetChnParam(0, &cp);             fprintf(trace, "VENC_SetChnParam = 0x%x\n", ret);
        ret = HI_MPI_VENC_GetChnParam(0, &cp);             fprintf(trace, "VENC_GetChnParam = 0x%x\n", ret);
    }
    {
        VENC_PARAM_MOD_S mp;  memset(&mp, 0, sizeof(mp));
        ret = HI_MPI_VENC_SetModParam(&mp);                fprintf(trace, "VENC_SetModParam = 0x%x\n", ret);
        ret = HI_MPI_VENC_GetModParam(&mp);                fprintf(trace, "VENC_GetModParam = 0x%x\n", ret);
    }
    {
        VENC_FOREGROUND_PROTECT_S fp;  memset(&fp, 0, sizeof(fp));
        ret = HI_MPI_VENC_SetForegroundProtect(0, &fp);    fprintf(trace, "VENC_SetForegroundProtect = 0x%x\n", ret);
        ret = HI_MPI_VENC_GetForegroundProtect(0, &fp);    fprintf(trace, "VENC_GetForegroundProtect = 0x%x\n", ret);
    }
    {
        ret = HI_MPI_VENC_SetSceneMode(0, 0);              fprintf(trace, "VENC_SetSceneMode = 0x%x\n", ret);
        VENC_SCENE_MODE_E sm = 0;
        ret = HI_MPI_VENC_GetSceneMode(0, &sm);            fprintf(trace, "VENC_GetSceneMode = 0x%x\n", ret);
    }
    {
        VENC_CHN_POOL_S pool;  memset(&pool, 0, sizeof(pool));
        ret = HI_MPI_VENC_AttachVbPool(0, &pool);          fprintf(trace, "VENC_AttachVbPool = 0x%x\n", ret);
        ret = HI_MPI_VENC_DetachVbPool(0);                 fprintf(trace, "VENC_DetachVbPool = 0x%x\n", ret);
    }
    {
        VENC_CU_PREDICTION_S cu;  memset(&cu, 0, sizeof(cu));
        ret = HI_MPI_VENC_SetCuPrediction(0, &cu);         fprintf(trace, "VENC_SetCuPrediction = 0x%x\n", ret);
        ret = HI_MPI_VENC_GetCuPrediction(0, &cu);         fprintf(trace, "VENC_GetCuPrediction = 0x%x\n", ret);
    }
    {
        VENC_SKIP_BIAS_S sb;  memset(&sb, 0, sizeof(sb));
        ret = HI_MPI_VENC_SetSkipBias(0, &sb);             fprintf(trace, "VENC_SetSkipBias = 0x%x\n", ret);
        ret = HI_MPI_VENC_GetSkipBias(0, &sb);             fprintf(trace, "VENC_GetSkipBias = 0x%x\n", ret);
    }
    {
        VENC_DEBREATHEFFECT_S de;  memset(&de, 0, sizeof(de));
        ret = HI_MPI_VENC_SetDeBreathEffect(0, &de);       fprintf(trace, "VENC_SetDeBreathEffect = 0x%x\n", ret);
        ret = HI_MPI_VENC_GetDeBreathEffect(0, &de);       fprintf(trace, "VENC_GetDeBreathEffect = 0x%x\n", ret);
    }
    {
        VENC_HIERARCHICAL_QP_S hq;  memset(&hq, 0, sizeof(hq));
        ret = HI_MPI_VENC_SetHierarchicalQp(0, &hq);      fprintf(trace, "VENC_SetHierarchicalQp = 0x%x\n", ret);
        ret = HI_MPI_VENC_GetHierarchicalQp(0, &hq);      fprintf(trace, "VENC_GetHierarchicalQp = 0x%x\n", ret);
    }
    {
        VENC_RC_ADVPARAM_S adv;  memset(&adv, 0, sizeof(adv));
        ret = HI_MPI_VENC_SetRcAdvParam(0, &adv);          fprintf(trace, "VENC_SetRcAdvParam = 0x%x\n", ret);
        ret = HI_MPI_VENC_GetRcAdvParam(0, &adv);          fprintf(trace, "VENC_GetRcAdvParam = 0x%x\n", ret);
    }
    {
        VENC_SLICE_SPLIT_S ss;  memset(&ss, 0, sizeof(ss));
        ret = HI_MPI_VENC_SetSliceSplit(0, &ss);           fprintf(trace, "VENC_SetSliceSplit = 0x%x\n", ret);
        ret = HI_MPI_VENC_GetSliceSplit(0, &ss);           fprintf(trace, "VENC_GetSliceSplit = 0x%x\n", ret);
    }
}

static void test_venc_null_ptr(FILE *trace) {
    HI_S32 ret;
    fprintf(trace, "--- VENC null_ptr ---\n");

    ret = HI_MPI_VENC_CreateChn(0, NULL);           fprintf(trace, "VENC_CreateChn(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VENC_StartRecvFrame(0, NULL);      fprintf(trace, "VENC_StartRecvFrame(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VENC_QueryStatus(0, NULL);          fprintf(trace, "VENC_QueryStatus(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VENC_SetChnAttr(0, NULL);           fprintf(trace, "VENC_SetChnAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VENC_GetChnAttr(0, NULL);           fprintf(trace, "VENC_GetChnAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VENC_GetStream(0, NULL, 0);         fprintf(trace, "VENC_GetStream(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VENC_ReleaseStream(0, NULL);        fprintf(trace, "VENC_ReleaseStream(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VENC_InsertUserData(0, NULL, 0);    fprintf(trace, "VENC_InsertUserData(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VENC_SendFrame(0, NULL, 0);         fprintf(trace, "VENC_SendFrame(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VENC_SendFrameEx(0, NULL, 0);       fprintf(trace, "VENC_SendFrameEx(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VENC_SetRoiAttr(0, NULL);           fprintf(trace, "VENC_SetRoiAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VENC_GetRoiAttr(0, 0, NULL);        fprintf(trace, "VENC_GetRoiAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VENC_SetRoiAttrEx(0, NULL);         fprintf(trace, "VENC_SetRoiAttrEx(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VENC_GetRoiAttrEx(0, 0, NULL);      fprintf(trace, "VENC_GetRoiAttrEx(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VENC_SetRoiBgFrameRate(0, NULL);    fprintf(trace, "VENC_SetRoiBgFrameRate(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VENC_GetRoiBgFrameRate(0, NULL);    fprintf(trace, "VENC_GetRoiBgFrameRate(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VENC_GetStreamBufInfo(0, NULL);      fprintf(trace, "VENC_GetStreamBufInfo(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VENC_SetModParam(NULL);              fprintf(trace, "VENC_SetModParam(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VENC_GetModParam(NULL);              fprintf(trace, "VENC_GetModParam(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VENC_GetSceneMode(0, NULL);          fprintf(trace, "VENC_GetSceneMode(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VENC_AttachVbPool(0, NULL);          fprintf(trace, "VENC_AttachVbPool(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VENC_GetJpegEncodeMode(0, NULL);     fprintf(trace, "VENC_GetJpegEncodeMode(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VENC_SetSliceSplit(0, NULL);         fprintf(trace, "VENC_SetSliceSplit(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VENC_GetSliceSplit(0, NULL);         fprintf(trace, "VENC_GetSliceSplit(NULL) = 0x%x\n", ret);
}

/* ========================================================================
 *  VDEC module  (30 functions)
 * ======================================================================== */

static void test_vdec_basic(FILE *trace) {
    HI_S32 ret;
    fprintf(trace, "--- VDEC basic ---\n");

    {
        VDEC_CHN_ATTR_S attr;
        memset(&attr, 0, sizeof(attr));
        ret = HI_MPI_VDEC_CreateChn(0, &attr);
        fprintf(trace, "VDEC_CreateChn = 0x%x\n", ret);
        ret = HI_MPI_VDEC_DestroyChn(0);
        fprintf(trace, "VDEC_DestroyChn = 0x%x\n", ret);
    }

    {
        VDEC_CHN_ATTR_S attr;
        memset(&attr, 0, sizeof(attr));
        ret = HI_MPI_VDEC_SetChnAttr(0, &attr);    fprintf(trace, "VDEC_SetChnAttr = 0x%x\n", ret);
        ret = HI_MPI_VDEC_GetChnAttr(0, &attr);    fprintf(trace, "VDEC_GetChnAttr = 0x%x\n", ret);
    }

    ret = HI_MPI_VDEC_StartRecvStream(0);           fprintf(trace, "VDEC_StartRecvStream = 0x%x\n", ret);
    ret = HI_MPI_VDEC_StopRecvStream(0);            fprintf(trace, "VDEC_StopRecvStream = 0x%x\n", ret);

    {
        VDEC_CHN_STATUS_S status;  memset(&status, 0, sizeof(status));
        ret = HI_MPI_VDEC_QueryStatus(0, &status);  fprintf(trace, "VDEC_QueryStatus = 0x%x\n", ret);
    }

    ret = HI_MPI_VDEC_GetFd(0);                     fprintf(trace, "VDEC_GetFd = 0x%x\n", ret);
    ret = HI_MPI_VDEC_CloseFd(0);                   fprintf(trace, "VDEC_CloseFd = 0x%x\n", ret);
    ret = HI_MPI_VDEC_ResetChn(0);                  fprintf(trace, "VDEC_ResetChn = 0x%x\n", ret);

    {
        VDEC_CHN_PARAM_S param;  memset(&param, 0, sizeof(param));
        ret = HI_MPI_VDEC_SetChnParam(0, &param);   fprintf(trace, "VDEC_SetChnParam = 0x%x\n", ret);
        ret = HI_MPI_VDEC_GetChnParam(0, &param);   fprintf(trace, "VDEC_GetChnParam = 0x%x\n", ret);
    }
    {
        VDEC_PRTCL_PARAM_S pp;  memset(&pp, 0, sizeof(pp));
        ret = HI_MPI_VDEC_SetProtocolParam(0, &pp); fprintf(trace, "VDEC_SetProtocolParam = 0x%x\n", ret);
        ret = HI_MPI_VDEC_GetProtocolParam(0, &pp); fprintf(trace, "VDEC_GetProtocolParam = 0x%x\n", ret);
    }
    {
        VDEC_STREAM_S stream;  memset(&stream, 0, sizeof(stream));
        ret = HI_MPI_VDEC_SendStream(0, &stream, 0);
        fprintf(trace, "VDEC_SendStream = 0x%x\n", ret);
    }
    {
        VIDEO_FRAME_INFO_S frm;  memset(&frm, 0, sizeof(frm));
        ret = HI_MPI_VDEC_GetFrame(0, &frm, 0);     fprintf(trace, "VDEC_GetFrame = 0x%x\n", ret);
        ret = HI_MPI_VDEC_ReleaseFrame(0, &frm);    fprintf(trace, "VDEC_ReleaseFrame = 0x%x\n", ret);
    }
    {
        VDEC_USERDATA_S ud;  memset(&ud, 0, sizeof(ud));
        ret = HI_MPI_VDEC_GetUserData(0, &ud, 0);   fprintf(trace, "VDEC_GetUserData = 0x%x\n", ret);
        ret = HI_MPI_VDEC_ReleaseUserData(0, &ud);  fprintf(trace, "VDEC_ReleaseUserData = 0x%x\n", ret);
    }
    {
        VIDEO_FRAME_INFO_S pic;  memset(&pic, 0, sizeof(pic));
        ret = HI_MPI_VDEC_SetUserPic(0, &pic);      fprintf(trace, "VDEC_SetUserPic = 0x%x\n", ret);
        ret = HI_MPI_VDEC_EnableUserPic(0, HI_FALSE);
        fprintf(trace, "VDEC_EnableUserPic = 0x%x\n", ret);
        ret = HI_MPI_VDEC_DisableUserPic(0);        fprintf(trace, "VDEC_DisableUserPic = 0x%x\n", ret);
    }
    {
        ret = HI_MPI_VDEC_SetDisplayMode(0, 0);     fprintf(trace, "VDEC_SetDisplayMode = 0x%x\n", ret);
        VIDEO_DISPLAY_MODE_E dm = 0;
        ret = HI_MPI_VDEC_GetDisplayMode(0, &dm);   fprintf(trace, "VDEC_GetDisplayMode = 0x%x\n", ret);
    }
    {
        ret = HI_MPI_VDEC_SetRotation(0, 0);        fprintf(trace, "VDEC_SetRotation = 0x%x\n", ret);
        ROTATION_E rot = 0;
        ret = HI_MPI_VDEC_GetRotation(0, &rot);     fprintf(trace, "VDEC_GetRotation = 0x%x\n", ret);
    }
    {
        VDEC_CHN_POOL_S pool;  memset(&pool, 0, sizeof(pool));
        ret = HI_MPI_VDEC_AttachVbPool(0, &pool);   fprintf(trace, "VDEC_AttachVbPool = 0x%x\n", ret);
        ret = HI_MPI_VDEC_DetachVbPool(0);          fprintf(trace, "VDEC_DetachVbPool = 0x%x\n", ret);
    }
    {
        VDEC_USER_DATA_ATTR_S uda;  memset(&uda, 0, sizeof(uda));
        ret = HI_MPI_VDEC_SetUserDataAttr(0, &uda); fprintf(trace, "VDEC_SetUserDataAttr = 0x%x\n", ret);
        ret = HI_MPI_VDEC_GetUserDataAttr(0, &uda); fprintf(trace, "VDEC_GetUserDataAttr = 0x%x\n", ret);
    }
    {
        VDEC_MOD_PARAM_S mp;  memset(&mp, 0, sizeof(mp));
        ret = HI_MPI_VDEC_SetModParam(&mp);         fprintf(trace, "VDEC_SetModParam = 0x%x\n", ret);
        ret = HI_MPI_VDEC_GetModParam(&mp);         fprintf(trace, "VDEC_GetModParam = 0x%x\n", ret);
    }
}

static void test_vdec_null_ptr(FILE *trace) {
    HI_S32 ret;
    fprintf(trace, "--- VDEC null_ptr ---\n");

    ret = HI_MPI_VDEC_CreateChn(0, NULL);            fprintf(trace, "VDEC_CreateChn(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VDEC_SetChnAttr(0, NULL);           fprintf(trace, "VDEC_SetChnAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VDEC_GetChnAttr(0, NULL);           fprintf(trace, "VDEC_GetChnAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VDEC_QueryStatus(0, NULL);           fprintf(trace, "VDEC_QueryStatus(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VDEC_SetChnParam(0, NULL);           fprintf(trace, "VDEC_SetChnParam(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VDEC_GetChnParam(0, NULL);           fprintf(trace, "VDEC_GetChnParam(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VDEC_SetProtocolParam(0, NULL);      fprintf(trace, "VDEC_SetProtocolParam(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VDEC_GetProtocolParam(0, NULL);      fprintf(trace, "VDEC_GetProtocolParam(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VDEC_SendStream(0, NULL, 0);         fprintf(trace, "VDEC_SendStream(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VDEC_GetFrame(0, NULL, 0);           fprintf(trace, "VDEC_GetFrame(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VDEC_ReleaseFrame(0, NULL);          fprintf(trace, "VDEC_ReleaseFrame(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VDEC_GetUserData(0, NULL, 0);        fprintf(trace, "VDEC_GetUserData(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VDEC_ReleaseUserData(0, NULL);       fprintf(trace, "VDEC_ReleaseUserData(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VDEC_SetUserPic(0, NULL);            fprintf(trace, "VDEC_SetUserPic(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VDEC_GetDisplayMode(0, NULL);        fprintf(trace, "VDEC_GetDisplayMode(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VDEC_GetRotation(0, NULL);           fprintf(trace, "VDEC_GetRotation(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VDEC_AttachVbPool(0, NULL);          fprintf(trace, "VDEC_AttachVbPool(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VDEC_SetUserDataAttr(0, NULL);       fprintf(trace, "VDEC_SetUserDataAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VDEC_GetUserDataAttr(0, NULL);       fprintf(trace, "VDEC_GetUserDataAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VDEC_SetModParam(NULL);              fprintf(trace, "VDEC_SetModParam(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VDEC_GetModParam(NULL);              fprintf(trace, "VDEC_GetModParam(NULL) = 0x%x\n", ret);
}

/* ========================================================================
 *  RGN module  (15 functions)
 * ======================================================================== */

static void test_rgn_basic(FILE *trace) {
    HI_S32 ret;
    fprintf(trace, "--- RGN basic ---\n");

    {
        RGN_ATTR_S attr;
        memset(&attr, 0, sizeof(attr));
        ret = HI_MPI_RGN_Create(0, &attr);          fprintf(trace, "RGN_Create = 0x%x\n", ret);
        ret = HI_MPI_RGN_Destroy(0);                fprintf(trace, "RGN_Destroy = 0x%x\n", ret);
    }
    {
        RGN_ATTR_S attr;
        memset(&attr, 0, sizeof(attr));
        ret = HI_MPI_RGN_GetAttr(0, &attr);         fprintf(trace, "RGN_GetAttr = 0x%x\n", ret);
        ret = HI_MPI_RGN_SetAttr(0, &attr);         fprintf(trace, "RGN_SetAttr = 0x%x\n", ret);
    }
    {
        BITMAP_S bmp;
        memset(&bmp, 0, sizeof(bmp));
        ret = HI_MPI_RGN_SetBitMap(0, &bmp);        fprintf(trace, "RGN_SetBitMap = 0x%x\n", ret);
    }
    {
        MPP_CHN_S chn;
        memset(&chn, 0, sizeof(chn));
        RGN_CHN_ATTR_S chnAttr;
        memset(&chnAttr, 0, sizeof(chnAttr));
        ret = HI_MPI_RGN_AttachToChn(0, &chn, &chnAttr);
        fprintf(trace, "RGN_AttachToChn = 0x%x\n", ret);
        ret = HI_MPI_RGN_DetachFromChn(0, &chn);
        fprintf(trace, "RGN_DetachFromChn = 0x%x\n", ret);
        ret = HI_MPI_RGN_SetDisplayAttr(0, &chn, &chnAttr);
        fprintf(trace, "RGN_SetDisplayAttr = 0x%x\n", ret);
        ret = HI_MPI_RGN_GetDisplayAttr(0, &chn, &chnAttr);
        fprintf(trace, "RGN_GetDisplayAttr = 0x%x\n", ret);
    }
    {
        RGN_CANVAS_INFO_S canvas;
        memset(&canvas, 0, sizeof(canvas));
        ret = HI_MPI_RGN_GetCanvasInfo(0, &canvas); fprintf(trace, "RGN_GetCanvasInfo = 0x%x\n", ret);
        ret = HI_MPI_RGN_UpdateCanvas(0);           fprintf(trace, "RGN_UpdateCanvas = 0x%x\n", ret);
    }
    {
        RGN_HANDLEGROUP grp = 0;
        RGN_HANDLE handles[1] = {0};
        ret = HI_MPI_RGN_BatchBegin(&grp, 1, handles);
        fprintf(trace, "RGN_BatchBegin = 0x%x\n", ret);
        ret = HI_MPI_RGN_BatchEnd(0);               fprintf(trace, "RGN_BatchEnd = 0x%x\n", ret);
    }
    ret = HI_MPI_RGN_GetFd();
    fprintf(trace, "RGN_GetFd = 0x%x\n", ret);
}

static void test_rgn_null_ptr(FILE *trace) {
    HI_S32 ret;
    fprintf(trace, "--- RGN null_ptr ---\n");

    ret = HI_MPI_RGN_Create(0, NULL);               fprintf(trace, "RGN_Create(NULL) = 0x%x\n", ret);
    ret = HI_MPI_RGN_GetAttr(0, NULL);              fprintf(trace, "RGN_GetAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_RGN_SetAttr(0, NULL);              fprintf(trace, "RGN_SetAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_RGN_SetBitMap(0, NULL);             fprintf(trace, "RGN_SetBitMap(NULL) = 0x%x\n", ret);
    ret = HI_MPI_RGN_AttachToChn(0, NULL, NULL);     fprintf(trace, "RGN_AttachToChn(NULL) = 0x%x\n", ret);
    ret = HI_MPI_RGN_DetachFromChn(0, NULL);         fprintf(trace, "RGN_DetachFromChn(NULL) = 0x%x\n", ret);
    ret = HI_MPI_RGN_SetDisplayAttr(0, NULL, NULL);  fprintf(trace, "RGN_SetDisplayAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_RGN_GetDisplayAttr(0, NULL, NULL);  fprintf(trace, "RGN_GetDisplayAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_RGN_GetCanvasInfo(0, NULL);         fprintf(trace, "RGN_GetCanvasInfo(NULL) = 0x%x\n", ret);
    ret = HI_MPI_RGN_BatchBegin(NULL, 0, NULL);      fprintf(trace, "RGN_BatchBegin(NULL) = 0x%x\n", ret);
}

/* ========================================================================
 *  GDC module  (8 functions)
 * ======================================================================== */

static void test_gdc_basic(FILE *trace) {
    HI_S32 ret;
    fprintf(trace, "--- GDC basic ---\n");

    {
        GDC_HANDLE hHandle = 0;
        ret = HI_MPI_GDC_BeginJob(&hHandle);        fprintf(trace, "GDC_BeginJob = 0x%x\n", ret);
        ret = HI_MPI_GDC_EndJob(0);                 fprintf(trace, "GDC_EndJob = 0x%x\n", ret);
        ret = HI_MPI_GDC_CancelJob(0);              fprintf(trace, "GDC_CancelJob = 0x%x\n", ret);
    }
    {
        GDC_TASK_ATTR_S task;
        memset(&task, 0, sizeof(task));
        FISHEYE_ATTR_S fishAttr;
        memset(&fishAttr, 0, sizeof(fishAttr));
        ret = HI_MPI_GDC_AddCorrectionTask(0, &task, &fishAttr);
        fprintf(trace, "GDC_AddCorrectionTask = 0x%x\n", ret);
    }
    {
        GDC_TASK_ATTR_S task;
        memset(&task, 0, sizeof(task));
        FISHEYE_ATTR_EX_S fishEx;
        memset(&fishEx, 0, sizeof(fishEx));
        ret = HI_MPI_GDC_AddCorrectionExTask(0, &task, &fishEx, HI_FALSE);
        fprintf(trace, "GDC_AddCorrectionExTask = 0x%x\n", ret);
    }
    {
        FISHEYE_JOB_CONFIG_S jobCfg;
        memset(&jobCfg, 0, sizeof(jobCfg));
        ret = HI_MPI_GDC_SetConfig(0, &jobCfg);
        fprintf(trace, "GDC_SetConfig = 0x%x\n", ret);
    }
    {
        GDC_TASK_ATTR_S task;
        memset(&task, 0, sizeof(task));
        GDC_PMF_ATTR_S pmf;
        memset(&pmf, 0, sizeof(pmf));
        ret = HI_MPI_GDC_AddPMFTask(0, &task, &pmf);
        fprintf(trace, "GDC_AddPMFTask = 0x%x\n", ret);
    }
    {
        GDC_FISHEYE_POINT_QUERY_ATTR_S queryAttr;
        memset(&queryAttr, 0, sizeof(queryAttr));
        VIDEO_FRAME_INFO_S frm;
        memset(&frm, 0, sizeof(frm));
        POINT_S dst = {0, 0}, src = {0, 0};
        ret = HI_MPI_GDC_FisheyePosQueryDst2Src(&queryAttr, &frm, &dst, &src);
        fprintf(trace, "GDC_FisheyePosQueryDst2Src = 0x%x\n", ret);
    }
}

static void test_gdc_null_ptr(FILE *trace) {
    HI_S32 ret;
    fprintf(trace, "--- GDC null_ptr ---\n");

    ret = HI_MPI_GDC_BeginJob(NULL);                fprintf(trace, "GDC_BeginJob(NULL) = 0x%x\n", ret);
    ret = HI_MPI_GDC_AddCorrectionTask(0, NULL, NULL);
    fprintf(trace, "GDC_AddCorrectionTask(NULL) = 0x%x\n", ret);
    ret = HI_MPI_GDC_AddCorrectionExTask(0, NULL, NULL, HI_FALSE);
    fprintf(trace, "GDC_AddCorrectionExTask(NULL) = 0x%x\n", ret);
    ret = HI_MPI_GDC_SetConfig(0, NULL);             fprintf(trace, "GDC_SetConfig(NULL) = 0x%x\n", ret);
    ret = HI_MPI_GDC_AddPMFTask(0, NULL, NULL);      fprintf(trace, "GDC_AddPMFTask(NULL) = 0x%x\n", ret);
    ret = HI_MPI_GDC_FisheyePosQueryDst2Src(NULL, NULL, NULL, NULL);
    fprintf(trace, "GDC_FisheyePosQueryDst2Src(NULL) = 0x%x\n", ret);
}

/* ========================================================================
 *  VGS module  (11 functions)
 * ======================================================================== */

static void test_vgs_basic(FILE *trace) {
    HI_S32 ret;
    fprintf(trace, "--- VGS basic ---\n");

    {
        VGS_HANDLE hHandle = 0;
        ret = HI_MPI_VGS_BeginJob(&hHandle);        fprintf(trace, "VGS_BeginJob = 0x%x\n", ret);
        ret = HI_MPI_VGS_EndJob(0);                 fprintf(trace, "VGS_EndJob = 0x%x\n", ret);
        ret = HI_MPI_VGS_CancelJob(0);              fprintf(trace, "VGS_CancelJob = 0x%x\n", ret);
    }
    {
        VGS_TASK_ATTR_S task;
        memset(&task, 0, sizeof(task));
        ret = HI_MPI_VGS_AddScaleTask(0, &task, 0);
        fprintf(trace, "VGS_AddScaleTask = 0x%x\n", ret);
    }
    {
        VGS_TASK_ATTR_S task;
        memset(&task, 0, sizeof(task));
        VGS_DRAW_LINE_S line;
        memset(&line, 0, sizeof(line));
        ret = HI_MPI_VGS_AddDrawLineTask(0, &task, &line);
        fprintf(trace, "VGS_AddDrawLineTask = 0x%x\n", ret);

        ret = HI_MPI_VGS_AddDrawLineTaskArray(0, &task, &line, 1);
        fprintf(trace, "VGS_AddDrawLineTaskArray = 0x%x\n", ret);
    }
    {
        VGS_TASK_ATTR_S task;
        memset(&task, 0, sizeof(task));
        VGS_ADD_COVER_S cover;
        memset(&cover, 0, sizeof(cover));
        ret = HI_MPI_VGS_AddCoverTask(0, &task, &cover);
        fprintf(trace, "VGS_AddCoverTask = 0x%x\n", ret);

        ret = HI_MPI_VGS_AddCoverTaskArray(0, &task, &cover, 1);
        fprintf(trace, "VGS_AddCoverTaskArray = 0x%x\n", ret);
    }
    {
        VGS_TASK_ATTR_S task;
        memset(&task, 0, sizeof(task));
        VGS_ADD_OSD_S osd;
        memset(&osd, 0, sizeof(osd));
        ret = HI_MPI_VGS_AddOsdTask(0, &task, &osd);
        fprintf(trace, "VGS_AddOsdTask = 0x%x\n", ret);

        ret = HI_MPI_VGS_AddOsdTaskArray(0, &task, &osd, 1);
        fprintf(trace, "VGS_AddOsdTaskArray = 0x%x\n", ret);
    }
    {
        VGS_TASK_ATTR_S task;
        memset(&task, 0, sizeof(task));
        ret = HI_MPI_VGS_AddRotationTask(0, &task, 0);
        fprintf(trace, "VGS_AddRotationTask = 0x%x\n", ret);
    }
    {
        VGS_TASK_ATTR_S task;
        memset(&task, 0, sizeof(task));
        RECT_S rect = {0, 0, 0, 0};
        HI_U64 luma = 0;
        ret = HI_MPI_VGS_AddLumaTaskArray(0, &task, &rect, 1, &luma);
        fprintf(trace, "VGS_AddLumaTaskArray = 0x%x\n", ret);
    }
}

static void test_vgs_null_ptr(FILE *trace) {
    HI_S32 ret;
    fprintf(trace, "--- VGS null_ptr ---\n");

    ret = HI_MPI_VGS_BeginJob(NULL);                fprintf(trace, "VGS_BeginJob(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VGS_AddScaleTask(0, NULL, 0);      fprintf(trace, "VGS_AddScaleTask(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VGS_AddDrawLineTask(0, NULL, NULL); fprintf(trace, "VGS_AddDrawLineTask(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VGS_AddCoverTask(0, NULL, NULL);    fprintf(trace, "VGS_AddCoverTask(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VGS_AddOsdTask(0, NULL, NULL);      fprintf(trace, "VGS_AddOsdTask(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VGS_AddDrawLineTaskArray(0, NULL, NULL, 0);
    fprintf(trace, "VGS_AddDrawLineTaskArray(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VGS_AddCoverTaskArray(0, NULL, NULL, 0);
    fprintf(trace, "VGS_AddCoverTaskArray(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VGS_AddOsdTaskArray(0, NULL, NULL, 0);
    fprintf(trace, "VGS_AddOsdTaskArray(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VGS_AddRotationTask(0, NULL, 0);    fprintf(trace, "VGS_AddRotationTask(NULL) = 0x%x\n", ret);
    ret = HI_MPI_VGS_AddLumaTaskArray(0, NULL, NULL, 0, NULL);
    fprintf(trace, "VGS_AddLumaTaskArray(NULL) = 0x%x\n", ret);
}

/* ========================================================================
 *  SNAP module  (9 functions)
 * ======================================================================== */

static void test_snap_basic(FILE *trace) {
    HI_S32 ret;
    fprintf(trace, "--- SNAP basic ---\n");

    {
        SNAP_ATTR_S attr;
        memset(&attr, 0, sizeof(attr));
        ret = HI_MPI_SNAP_SetPipeAttr(0, &attr);    fprintf(trace, "SNAP_SetPipeAttr = 0x%x\n", ret);
        ret = HI_MPI_SNAP_GetPipeAttr(0, &attr);    fprintf(trace, "SNAP_GetPipeAttr = 0x%x\n", ret);
    }
    ret = HI_MPI_SNAP_EnablePipe(0);                 fprintf(trace, "SNAP_EnablePipe = 0x%x\n", ret);
    ret = HI_MPI_SNAP_DisablePipe(0);                fprintf(trace, "SNAP_DisablePipe = 0x%x\n", ret);
    ret = HI_MPI_SNAP_TriggerPipe(0);                fprintf(trace, "SNAP_TriggerPipe = 0x%x\n", ret);
    ret = HI_MPI_SNAP_MultiTrigger(0);               fprintf(trace, "SNAP_MultiTrigger = 0x%x\n", ret);

    {
        ISP_PRO_SHARPEN_PARAM_S shp;
        memset(&shp, 0, sizeof(shp));
        ret = HI_MPI_SNAP_SetProSharpenParam(0, &shp);
        fprintf(trace, "SNAP_SetProSharpenParam = 0x%x\n", ret);
        ret = HI_MPI_SNAP_GetProSharpenParam(0, &shp);
        fprintf(trace, "SNAP_GetProSharpenParam = 0x%x\n", ret);
    }
    {
        ISP_PRO_BNR_PARAM_S bnr;
        memset(&bnr, 0, sizeof(bnr));
        ret = HI_MPI_SNAP_SetProBNRParam(0, &bnr);  fprintf(trace, "SNAP_SetProBNRParam = 0x%x\n", ret);
        ret = HI_MPI_SNAP_GetProBNRParam(0, &bnr);  fprintf(trace, "SNAP_GetProBNRParam = 0x%x\n", ret);
    }
}

static void test_snap_null_ptr(FILE *trace) {
    HI_S32 ret;
    fprintf(trace, "--- SNAP null_ptr ---\n");

    ret = HI_MPI_SNAP_SetPipeAttr(0, NULL);          fprintf(trace, "SNAP_SetPipeAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_SNAP_GetPipeAttr(0, NULL);          fprintf(trace, "SNAP_GetPipeAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_SNAP_SetProSharpenParam(0, NULL);   fprintf(trace, "SNAP_SetProSharpenParam(NULL) = 0x%x\n", ret);
    ret = HI_MPI_SNAP_GetProSharpenParam(0, NULL);   fprintf(trace, "SNAP_GetProSharpenParam(NULL) = 0x%x\n", ret);
    ret = HI_MPI_SNAP_SetProBNRParam(0, NULL);       fprintf(trace, "SNAP_SetProBNRParam(NULL) = 0x%x\n", ret);
    ret = HI_MPI_SNAP_GetProBNRParam(0, NULL);       fprintf(trace, "SNAP_GetProBNRParam(NULL) = 0x%x\n", ret);
}

/* ========================================================================
 *  AUDIO/AI/AO/AENC/ADEC module  (65 functions)
 * ======================================================================== */

static void test_audio_basic(FILE *trace) {
    HI_S32 ret;
    fprintf(trace, "--- AUDIO basic ---\n");

    {
        AUDIO_MOD_PARAM_S modParam;
        memset(&modParam, 0, sizeof(modParam));
        ret = HI_MPI_AUDIO_SetModParam(&modParam);
        fprintf(trace, "AUDIO_SetModParam = 0x%x\n", ret);
        ret = HI_MPI_AUDIO_GetModParam(&modParam);
        fprintf(trace, "AUDIO_GetModParam = 0x%x\n", ret);
    }
    {
        AUDIO_VQE_REGISTER_S vqeReg;
        memset(&vqeReg, 0, sizeof(vqeReg));
        ret = HI_MPI_AUDIO_RegisterVQEModule(&vqeReg);
        fprintf(trace, "AUDIO_RegisterVQEModule = 0x%x\n", ret);
    }
}

static void test_ai_basic(FILE *trace) {
    HI_S32 ret;
    fprintf(trace, "--- AI basic ---\n");

    {
        AIO_ATTR_S attr;
        memset(&attr, 0, sizeof(attr));
        ret = HI_MPI_AI_SetPubAttr(0, &attr);       fprintf(trace, "AI_SetPubAttr = 0x%x\n", ret);
        ret = HI_MPI_AI_GetPubAttr(0, &attr);       fprintf(trace, "AI_GetPubAttr = 0x%x\n", ret);
    }
    ret = HI_MPI_AI_Enable(0);                       fprintf(trace, "AI_Enable = 0x%x\n", ret);
    ret = HI_MPI_AI_Disable(0);                      fprintf(trace, "AI_Disable = 0x%x\n", ret);
    ret = HI_MPI_AI_EnableChn(0, 0);                 fprintf(trace, "AI_EnableChn = 0x%x\n", ret);
    ret = HI_MPI_AI_DisableChn(0, 0);                fprintf(trace, "AI_DisableChn = 0x%x\n", ret);

    {
        AUDIO_FRAME_S frm;
        memset(&frm, 0, sizeof(frm));
        AEC_FRAME_S aecFrm;
        memset(&aecFrm, 0, sizeof(aecFrm));
        ret = HI_MPI_AI_GetFrame(0, 0, &frm, &aecFrm, 0);
        fprintf(trace, "AI_GetFrame = 0x%x\n", ret);
        ret = HI_MPI_AI_ReleaseFrame(0, 0, &frm, &aecFrm);
        fprintf(trace, "AI_ReleaseFrame = 0x%x\n", ret);
    }

    {
        AI_CHN_PARAM_S chnParam;
        memset(&chnParam, 0, sizeof(chnParam));
        ret = HI_MPI_AI_SetChnParam(0, 0, &chnParam);
        fprintf(trace, "AI_SetChnParam = 0x%x\n", ret);
        ret = HI_MPI_AI_GetChnParam(0, 0, &chnParam);
        fprintf(trace, "AI_GetChnParam = 0x%x\n", ret);
    }

    {
        AI_RECORDVQE_CONFIG_S vqeCfg;
        memset(&vqeCfg, 0, sizeof(vqeCfg));
        ret = HI_MPI_AI_SetRecordVqeAttr(0, 0, &vqeCfg);
        fprintf(trace, "AI_SetRecordVqeAttr = 0x%x\n", ret);
        ret = HI_MPI_AI_GetRecordVqeAttr(0, 0, &vqeCfg);
        fprintf(trace, "AI_GetRecordVqeAttr = 0x%x\n", ret);
    }

    ret = HI_MPI_AI_EnableVqe(0, 0);                 fprintf(trace, "AI_EnableVqe = 0x%x\n", ret);
    ret = HI_MPI_AI_DisableVqe(0, 0);                fprintf(trace, "AI_DisableVqe = 0x%x\n", ret);

    ret = HI_MPI_AI_EnableReSmp(0, 0, AUDIO_SAMPLE_RATE_8000);
    fprintf(trace, "AI_EnableReSmp = 0x%x\n", ret);
    ret = HI_MPI_AI_DisableReSmp(0, 0);              fprintf(trace, "AI_DisableReSmp = 0x%x\n", ret);

    {
        ret = HI_MPI_AI_SetTrackMode(0, 0);          fprintf(trace, "AI_SetTrackMode = 0x%x\n", ret);
        AUDIO_TRACK_MODE_E trackMode = 0;
        ret = HI_MPI_AI_GetTrackMode(0, &trackMode); fprintf(trace, "AI_GetTrackMode = 0x%x\n", ret);
    }

    {
        AUDIO_SAVE_FILE_INFO_S saveInfo;
        memset(&saveInfo, 0, sizeof(saveInfo));
        ret = HI_MPI_AI_SaveFile(0, 0, &saveInfo);   fprintf(trace, "AI_SaveFile = 0x%x\n", ret);
    }

    {
        AUDIO_FILE_STATUS_S fileStatus;
        memset(&fileStatus, 0, sizeof(fileStatus));
        ret = HI_MPI_AI_QueryFileStatus(0, 0, &fileStatus);
        fprintf(trace, "AI_QueryFileStatus = 0x%x\n", ret);
    }

    ret = HI_MPI_AI_ClrPubAttr(0);                   fprintf(trace, "AI_ClrPubAttr = 0x%x\n", ret);
    ret = HI_MPI_AI_GetFd(0, 0);                     fprintf(trace, "AI_GetFd = 0x%x\n", ret);

    ret = HI_MPI_AI_EnableAecRefFrame(0, 0, 0, 0);   fprintf(trace, "AI_EnableAecRefFrame = 0x%x\n", ret);
    ret = HI_MPI_AI_DisableAecRefFrame(0, 0);        fprintf(trace, "AI_DisableAecRefFrame = 0x%x\n", ret);

    {
        AI_TALKVQE_CONFIG_S talkCfg;
        memset(&talkCfg, 0, sizeof(talkCfg));
        ret = HI_MPI_AI_SetTalkVqeAttr(0, 0, 0, 0, &talkCfg);
        fprintf(trace, "AI_SetTalkVqeAttr = 0x%x\n", ret);
        ret = HI_MPI_AI_GetTalkVqeAttr(0, 0, &talkCfg);
        fprintf(trace, "AI_GetTalkVqeAttr = 0x%x\n", ret);
    }
}

static void test_ao_basic(FILE *trace) {
    HI_S32 ret;
    fprintf(trace, "--- AO basic ---\n");

    {
        AIO_ATTR_S attr;
        memset(&attr, 0, sizeof(attr));
        ret = HI_MPI_AO_SetPubAttr(0, &attr);       fprintf(trace, "AO_SetPubAttr = 0x%x\n", ret);
        ret = HI_MPI_AO_GetPubAttr(0, &attr);       fprintf(trace, "AO_GetPubAttr = 0x%x\n", ret);
    }
    ret = HI_MPI_AO_Enable(0);                       fprintf(trace, "AO_Enable = 0x%x\n", ret);
    ret = HI_MPI_AO_Disable(0);                      fprintf(trace, "AO_Disable = 0x%x\n", ret);
    ret = HI_MPI_AO_EnableChn(0, 0);                 fprintf(trace, "AO_EnableChn = 0x%x\n", ret);
    ret = HI_MPI_AO_DisableChn(0, 0);                fprintf(trace, "AO_DisableChn = 0x%x\n", ret);

    {
        AUDIO_FRAME_S frm;
        memset(&frm, 0, sizeof(frm));
        ret = HI_MPI_AO_SendFrame(0, 0, &frm, 0);   fprintf(trace, "AO_SendFrame = 0x%x\n", ret);
    }

    ret = HI_MPI_AO_EnableReSmp(0, 0, AUDIO_SAMPLE_RATE_8000);
    fprintf(trace, "AO_EnableReSmp = 0x%x\n", ret);
    ret = HI_MPI_AO_DisableReSmp(0, 0);              fprintf(trace, "AO_DisableReSmp = 0x%x\n", ret);

    ret = HI_MPI_AO_ClearChnBuf(0, 0);               fprintf(trace, "AO_ClearChnBuf = 0x%x\n", ret);

    {
        AO_CHN_STATE_S status;
        memset(&status, 0, sizeof(status));
        ret = HI_MPI_AO_QueryChnStat(0, 0, &status); fprintf(trace, "AO_QueryChnStat = 0x%x\n", ret);
    }

    ret = HI_MPI_AO_PauseChn(0, 0);                  fprintf(trace, "AO_PauseChn = 0x%x\n", ret);
    ret = HI_MPI_AO_ResumeChn(0, 0);                 fprintf(trace, "AO_ResumeChn = 0x%x\n", ret);

    {
        ret = HI_MPI_AO_SetVolume(0, 0);             fprintf(trace, "AO_SetVolume = 0x%x\n", ret);
        HI_S32 vol = 0;
        ret = HI_MPI_AO_GetVolume(0, &vol);          fprintf(trace, "AO_GetVolume = 0x%x\n", ret);
    }

    {
        AUDIO_FADE_S fade;
        memset(&fade, 0, sizeof(fade));
        ret = HI_MPI_AO_SetMute(0, HI_FALSE, &fade); fprintf(trace, "AO_SetMute = 0x%x\n", ret);
        HI_BOOL mute = HI_FALSE;
        ret = HI_MPI_AO_GetMute(0, &mute, &fade);   fprintf(trace, "AO_GetMute = 0x%x\n", ret);
    }

    {
        ret = HI_MPI_AO_SetTrackMode(0, 0);          fprintf(trace, "AO_SetTrackMode = 0x%x\n", ret);
        AUDIO_TRACK_MODE_E mode = 0;
        ret = HI_MPI_AO_GetTrackMode(0, &mode);      fprintf(trace, "AO_GetTrackMode = 0x%x\n", ret);
    }

    ret = HI_MPI_AO_GetFd(0, 0);                     fprintf(trace, "AO_GetFd = 0x%x\n", ret);
    ret = HI_MPI_AO_ClrPubAttr(0);                   fprintf(trace, "AO_ClrPubAttr = 0x%x\n", ret);

    {
        AO_VQE_CONFIG_S vqeCfg;
        memset(&vqeCfg, 0, sizeof(vqeCfg));
        ret = HI_MPI_AO_SetVqeAttr(0, 0, &vqeCfg);  fprintf(trace, "AO_SetVqeAttr = 0x%x\n", ret);
        ret = HI_MPI_AO_GetVqeAttr(0, 0, &vqeCfg);  fprintf(trace, "AO_GetVqeAttr = 0x%x\n", ret);
    }

    ret = HI_MPI_AO_EnableVqe(0, 0);                 fprintf(trace, "AO_EnableVqe = 0x%x\n", ret);
    ret = HI_MPI_AO_DisableVqe(0, 0);                fprintf(trace, "AO_DisableVqe = 0x%x\n", ret);

    {
        HI_S32 delay = 0;
        ret = HI_MPI_AO_GetChnDelay(0, 0, &delay);   fprintf(trace, "AO_GetChnDelay = 0x%x\n", ret);
    }
}

static void test_aenc_basic(FILE *trace) {
    HI_S32 ret;
    fprintf(trace, "--- AENC basic ---\n");

    {
        AENC_CHN_ATTR_S attr;
        memset(&attr, 0, sizeof(attr));
        ret = HI_MPI_AENC_CreateChn(0, &attr);      fprintf(trace, "AENC_CreateChn = 0x%x\n", ret);
        ret = HI_MPI_AENC_DestroyChn(0);             fprintf(trace, "AENC_DestroyChn = 0x%x\n", ret);
    }
    {
        AUDIO_FRAME_S frm;
        memset(&frm, 0, sizeof(frm));
        AEC_FRAME_S aec;
        memset(&aec, 0, sizeof(aec));
        ret = HI_MPI_AENC_SendFrame(0, &frm, &aec);  fprintf(trace, "AENC_SendFrame = 0x%x\n", ret);
    }
    {
        AUDIO_STREAM_S stream;
        memset(&stream, 0, sizeof(stream));
        ret = HI_MPI_AENC_GetStream(0, &stream, 0);   fprintf(trace, "AENC_GetStream = 0x%x\n", ret);
        ret = HI_MPI_AENC_ReleaseStream(0, &stream);  fprintf(trace, "AENC_ReleaseStream = 0x%x\n", ret);
    }
    ret = HI_MPI_AENC_GetFd(0);                       fprintf(trace, "AENC_GetFd = 0x%x\n", ret);

    {
        HI_S32 handle = 0;
        AENC_ENCODER_S encoder;
        memset(&encoder, 0, sizeof(encoder));
        ret = HI_MPI_AENC_RegisterEncoder(&handle, &encoder);
        fprintf(trace, "AENC_RegisterEncoder = 0x%x\n", ret);
        ret = HI_MPI_AENC_UnRegisterEncoder(0);       fprintf(trace, "AENC_UnRegisterEncoder = 0x%x\n", ret);
    }
    {
        HI_U64 physAddr = 0;
        HI_U32 size = 0;
        ret = HI_MPI_AENC_GetStreamBufInfo(0, &physAddr, &size);
        fprintf(trace, "AENC_GetStreamBufInfo = 0x%x\n", ret);
    }
    {
        ret = HI_MPI_AENC_SetMute(0, HI_FALSE);       fprintf(trace, "AENC_SetMute = 0x%x\n", ret);
        HI_BOOL mute = HI_FALSE;
        ret = HI_MPI_AENC_GetMute(0, &mute);          fprintf(trace, "AENC_GetMute = 0x%x\n", ret);
    }
}

static void test_adec_basic(FILE *trace) {
    HI_S32 ret;
    fprintf(trace, "--- ADEC basic ---\n");

    {
        ADEC_CHN_ATTR_S attr;
        memset(&attr, 0, sizeof(attr));
        ret = HI_MPI_ADEC_CreateChn(0, &attr);      fprintf(trace, "ADEC_CreateChn = 0x%x\n", ret);
        ret = HI_MPI_ADEC_DestroyChn(0);             fprintf(trace, "ADEC_DestroyChn = 0x%x\n", ret);
    }
    {
        AUDIO_STREAM_S stream;
        memset(&stream, 0, sizeof(stream));
        ret = HI_MPI_ADEC_SendStream(0, &stream, HI_FALSE);
        fprintf(trace, "ADEC_SendStream = 0x%x\n", ret);
    }
    ret = HI_MPI_ADEC_ClearChnBuf(0);                fprintf(trace, "ADEC_ClearChnBuf = 0x%x\n", ret);

    {
        HI_S32 handle = 0;
        ADEC_DECODER_S decoder;
        memset(&decoder, 0, sizeof(decoder));
        ret = HI_MPI_ADEC_RegisterDecoder(&handle, &decoder);
        fprintf(trace, "ADEC_RegisterDecoder = 0x%x\n", ret);
        ret = HI_MPI_ADEC_UnRegisterDecoder(0);       fprintf(trace, "ADEC_UnRegisterDecoder = 0x%x\n", ret);
    }
    {
        AUDIO_FRAME_INFO_S frmInfo;
        memset(&frmInfo, 0, sizeof(frmInfo));
        ret = HI_MPI_ADEC_GetFrame(0, &frmInfo, HI_FALSE);
        fprintf(trace, "ADEC_GetFrame = 0x%x\n", ret);
        ret = HI_MPI_ADEC_ReleaseFrame(0, &frmInfo);  fprintf(trace, "ADEC_ReleaseFrame = 0x%x\n", ret);
    }
    ret = HI_MPI_ADEC_SendEndOfStream(0, HI_FALSE);   fprintf(trace, "ADEC_SendEndOfStream = 0x%x\n", ret);
    {
        ADEC_CHN_STATE_S state;
        memset(&state, 0, sizeof(state));
        ret = HI_MPI_ADEC_QueryChnStat(0, &state);    fprintf(trace, "ADEC_QueryChnStat = 0x%x\n", ret);
    }
}

static void test_audio_null_ptr(FILE *trace) {
    HI_S32 ret;
    fprintf(trace, "--- AUDIO null_ptr ---\n");

    ret = HI_MPI_AUDIO_SetModParam(NULL);             fprintf(trace, "AUDIO_SetModParam(NULL) = 0x%x\n", ret);
    ret = HI_MPI_AUDIO_GetModParam(NULL);             fprintf(trace, "AUDIO_GetModParam(NULL) = 0x%x\n", ret);
    ret = HI_MPI_AUDIO_RegisterVQEModule(NULL);       fprintf(trace, "AUDIO_RegisterVQEModule(NULL) = 0x%x\n", ret);

    ret = HI_MPI_AI_SetPubAttr(0, NULL);              fprintf(trace, "AI_SetPubAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_AI_GetPubAttr(0, NULL);              fprintf(trace, "AI_GetPubAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_AI_GetFrame(0, 0, NULL, NULL, 0);    fprintf(trace, "AI_GetFrame(NULL) = 0x%x\n", ret);
    ret = HI_MPI_AI_ReleaseFrame(0, 0, NULL, NULL);   fprintf(trace, "AI_ReleaseFrame(NULL) = 0x%x\n", ret);
    ret = HI_MPI_AI_SetChnParam(0, 0, NULL);          fprintf(trace, "AI_SetChnParam(NULL) = 0x%x\n", ret);
    ret = HI_MPI_AI_GetChnParam(0, 0, NULL);          fprintf(trace, "AI_GetChnParam(NULL) = 0x%x\n", ret);
    ret = HI_MPI_AI_SetRecordVqeAttr(0, 0, NULL);     fprintf(trace, "AI_SetRecordVqeAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_AI_GetRecordVqeAttr(0, 0, NULL);     fprintf(trace, "AI_GetRecordVqeAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_AI_GetTrackMode(0, NULL);             fprintf(trace, "AI_GetTrackMode(NULL) = 0x%x\n", ret);
    ret = HI_MPI_AI_SaveFile(0, 0, NULL);              fprintf(trace, "AI_SaveFile(NULL) = 0x%x\n", ret);
    ret = HI_MPI_AI_QueryFileStatus(0, 0, NULL);      fprintf(trace, "AI_QueryFileStatus(NULL) = 0x%x\n", ret);
    ret = HI_MPI_AI_SetTalkVqeAttr(0, 0, 0, 0, NULL); fprintf(trace, "AI_SetTalkVqeAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_AI_GetTalkVqeAttr(0, 0, NULL);       fprintf(trace, "AI_GetTalkVqeAttr(NULL) = 0x%x\n", ret);

    ret = HI_MPI_AO_SetPubAttr(0, NULL);              fprintf(trace, "AO_SetPubAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_AO_GetPubAttr(0, NULL);              fprintf(trace, "AO_GetPubAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_AO_SendFrame(0, 0, NULL, 0);         fprintf(trace, "AO_SendFrame(NULL) = 0x%x\n", ret);
    ret = HI_MPI_AO_QueryChnStat(0, 0, NULL);         fprintf(trace, "AO_QueryChnStat(NULL) = 0x%x\n", ret);
    ret = HI_MPI_AO_GetVolume(0, NULL);                fprintf(trace, "AO_GetVolume(NULL) = 0x%x\n", ret);
    ret = HI_MPI_AO_GetMute(0, NULL, NULL);            fprintf(trace, "AO_GetMute(NULL) = 0x%x\n", ret);
    ret = HI_MPI_AO_GetTrackMode(0, NULL);             fprintf(trace, "AO_GetTrackMode(NULL) = 0x%x\n", ret);
    ret = HI_MPI_AO_SetVqeAttr(0, 0, NULL);           fprintf(trace, "AO_SetVqeAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_AO_GetVqeAttr(0, 0, NULL);           fprintf(trace, "AO_GetVqeAttr(NULL) = 0x%x\n", ret);
    ret = HI_MPI_AO_GetChnDelay(0, 0, NULL);          fprintf(trace, "AO_GetChnDelay(NULL) = 0x%x\n", ret);

    ret = HI_MPI_AENC_CreateChn(0, NULL);              fprintf(trace, "AENC_CreateChn(NULL) = 0x%x\n", ret);
    ret = HI_MPI_AENC_SendFrame(0, NULL, NULL);        fprintf(trace, "AENC_SendFrame(NULL) = 0x%x\n", ret);
    ret = HI_MPI_AENC_GetStream(0, NULL, 0);           fprintf(trace, "AENC_GetStream(NULL) = 0x%x\n", ret);
    ret = HI_MPI_AENC_ReleaseStream(0, NULL);          fprintf(trace, "AENC_ReleaseStream(NULL) = 0x%x\n", ret);
    ret = HI_MPI_AENC_RegisterEncoder(NULL, NULL);     fprintf(trace, "AENC_RegisterEncoder(NULL) = 0x%x\n", ret);
    ret = HI_MPI_AENC_GetStreamBufInfo(0, NULL, NULL); fprintf(trace, "AENC_GetStreamBufInfo(NULL) = 0x%x\n", ret);
    ret = HI_MPI_AENC_GetMute(0, NULL);                fprintf(trace, "AENC_GetMute(NULL) = 0x%x\n", ret);

    ret = HI_MPI_ADEC_CreateChn(0, NULL);              fprintf(trace, "ADEC_CreateChn(NULL) = 0x%x\n", ret);
    ret = HI_MPI_ADEC_SendStream(0, NULL, HI_FALSE);   fprintf(trace, "ADEC_SendStream(NULL) = 0x%x\n", ret);
    ret = HI_MPI_ADEC_RegisterDecoder(NULL, NULL);     fprintf(trace, "ADEC_RegisterDecoder(NULL) = 0x%x\n", ret);
    ret = HI_MPI_ADEC_GetFrame(0, NULL, HI_FALSE);     fprintf(trace, "ADEC_GetFrame(NULL) = 0x%x\n", ret);
    ret = HI_MPI_ADEC_ReleaseFrame(0, NULL);           fprintf(trace, "ADEC_ReleaseFrame(NULL) = 0x%x\n", ret);
    ret = HI_MPI_ADEC_QueryChnStat(0, NULL);           fprintf(trace, "ADEC_QueryChnStat(NULL) = 0x%x\n", ret);
}

/* ========================================================================
 *  main
 * ======================================================================== */

/* Crash recovery: catch SIGSEGV and skip to next test */
#include <signal.h>
#include <setjmp.h>

static sigjmp_buf g_jmp;
static FILE *g_trace_global;
static const char *g_current_test;

static void crash_handler(int sig) {
    (void)sig;
    if (g_trace_global)
        fprintf(g_trace_global, "!!! CRASH in %s (SIGSEGV) !!!\n", g_current_test);
    siglongjmp(g_jmp, 1);
}

#define RUN_TEST(name, trace) do { \
    g_current_test = #name; \
    g_trace_global = trace; \
    fflush(trace); \
    if (sigsetjmp(g_jmp, 1) == 0) { \
        name(trace); \
    } \
    fflush(trace); \
} while(0)

int main(int argc, char *argv[]) {
    const char *trace_path = "trace.all.log";
    if (argc > 1) trace_path = argv[1];

    FILE *trace = fopen(trace_path, "w");
    if (!trace) { perror("fopen"); return 1; }
    ioctl_mock_set_trace(trace);
    ioctl_mock_reset();

    /* Install crash handler */
    struct sigaction sa;
    sa.sa_handler = crash_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGSEGV, &sa, NULL);
    sigaction(SIGBUS, &sa, NULL);
    sigaction(SIGABRT, &sa, NULL);

    fprintf(trace, "=== libmpi ALL harness ===\n");

    RUN_TEST(test_sys_basic, trace);
    RUN_TEST(test_sys_null_ptr, trace);
    RUN_TEST(test_vb_basic, trace);
    RUN_TEST(test_vb_null_ptr, trace);
    RUN_TEST(test_vi_basic, trace);
    RUN_TEST(test_vi_null_ptr, trace);
    RUN_TEST(test_vo_basic, trace);
    RUN_TEST(test_vo_null_ptr, trace);
    RUN_TEST(test_vpss_basic, trace);
    RUN_TEST(test_vpss_null_ptr, trace);
    RUN_TEST(test_venc_basic, trace);
    RUN_TEST(test_venc_null_ptr, trace);
    RUN_TEST(test_vdec_basic, trace);
    RUN_TEST(test_vdec_null_ptr, trace);
    RUN_TEST(test_rgn_basic, trace);
    RUN_TEST(test_rgn_null_ptr, trace);
    RUN_TEST(test_gdc_basic, trace);
    RUN_TEST(test_gdc_null_ptr, trace);
    RUN_TEST(test_vgs_basic, trace);
    RUN_TEST(test_vgs_null_ptr, trace);
    RUN_TEST(test_snap_basic, trace);
    RUN_TEST(test_snap_null_ptr, trace);
    RUN_TEST(test_audio_basic, trace);
    RUN_TEST(test_ai_basic, trace);
    RUN_TEST(test_ao_basic, trace);
    RUN_TEST(test_aenc_basic, trace);
    RUN_TEST(test_adec_basic, trace);
    RUN_TEST(test_audio_null_ptr, trace);

    fprintf(trace, "open_count = %d\n", ioctl_mock_get_open_count());
    fprintf(trace, "ioctl_count = %d\n", ioctl_mock_get_ioctl_count());
    fprintf(trace, "=== done ===\n");

    fclose(trace);
    return 0;
}
