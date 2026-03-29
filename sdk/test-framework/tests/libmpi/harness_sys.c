/*
 * harness_sys.c — Test harness for HI_MPI_SYS and HI_MPI_VB functions.
 *
 * Exercises the system/video-buffer API and logs all return values.
 * Run with both vendor and our libmpi.a to compare traces.
 */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "ioctl_mock.h"

/* MPI headers */
#include "hi_type.h"
#include "hi_common.h"
#include "hi_comm_sys.h"
#include "hi_comm_vb.h"
#include "mpi_sys.h"
#include "mpi_vb.h"

static void test_sys_version(FILE *trace) {
    MPP_VERSION_S ver;
    HI_S32 ret = HI_MPI_SYS_GetVersion(&ver);
    fprintf(trace, "SYS_GetVersion = %d, ver=%s\n", ret,
        ret == 0 ? ver.aVersion : "(err)");
}

static void test_sys_init_exit(FILE *trace) {
    HI_S32 ret;

    ret = HI_MPI_SYS_Init();
    fprintf(trace, "SYS_Init = 0x%x\n", ret);

    ret = HI_MPI_SYS_Exit();
    fprintf(trace, "SYS_Exit = 0x%x\n", ret);
}

static void test_sys_pts(FILE *trace) {
    HI_S32 ret;
    HI_U64 u64PTS = 0;

    ret = HI_MPI_SYS_GetCurPTS(&u64PTS);
    fprintf(trace, "SYS_GetCurPTS = 0x%x\n", ret);

    ret = HI_MPI_SYS_InitPTSBase(1000000);
    fprintf(trace, "SYS_InitPTSBase = 0x%x\n", ret);

    ret = HI_MPI_SYS_SyncPTS(500000);
    fprintf(trace, "SYS_SyncPTS = 0x%x\n", ret);
}

static void test_sys_null_ptr(FILE *trace) {
    HI_S32 ret;

    /* These should return HI_ERR_SYS_NULL_PTR */
    ret = HI_MPI_SYS_GetVersion(NULL);
    fprintf(trace, "SYS_GetVersion(NULL) = 0x%x\n", ret);

    ret = HI_MPI_SYS_GetCurPTS(NULL);
    fprintf(trace, "SYS_GetCurPTS(NULL) = 0x%x\n", ret);
}

static void test_vb_basic(FILE *trace) {
    HI_S32 ret;

    ret = HI_MPI_VB_Init();
    fprintf(trace, "VB_Init = 0x%x\n", ret);

    ret = HI_MPI_VB_Exit();
    fprintf(trace, "VB_Exit = 0x%x\n", ret);

    /* NULL pointer checks */
    ret = HI_MPI_VB_SetConfig(NULL);
    fprintf(trace, "VB_SetConfig(NULL) = 0x%x\n", ret);

    ret = HI_MPI_VB_GetConfig(NULL);
    fprintf(trace, "VB_GetConfig(NULL) = 0x%x\n", ret);
}

static void test_sys_bind(FILE *trace) {
    HI_S32 ret;
    MPP_CHN_S stSrcChn, stDstChn;

    stSrcChn.enModId = HI_ID_VI;
    stSrcChn.s32DevId = 0;
    stSrcChn.s32ChnId = 0;

    stDstChn.enModId = HI_ID_VPSS;
    stDstChn.s32DevId = 0;
    stDstChn.s32ChnId = 0;

    ret = HI_MPI_SYS_Bind(&stSrcChn, &stDstChn);
    fprintf(trace, "SYS_Bind(VI->VPSS) = 0x%x\n", ret);

    ret = HI_MPI_SYS_UnBind(&stSrcChn, &stDstChn);
    fprintf(trace, "SYS_UnBind(VI->VPSS) = 0x%x\n", ret);
}

int main(int argc, char *argv[]) {
    const char *trace_path = "trace.log";
    if (argc > 1) trace_path = argv[1];

    FILE *trace = fopen(trace_path, "w");
    if (!trace) { perror("fopen"); return 1; }
    ioctl_mock_set_trace(trace);
    ioctl_mock_reset();

    fprintf(trace, "=== libmpi SYS/VB harness ===\n");

    test_sys_version(trace);
    test_sys_init_exit(trace);
    test_sys_pts(trace);
    test_sys_null_ptr(trace);
    test_vb_basic(trace);
    test_sys_bind(trace);

    fprintf(trace, "open_count = %d\n", ioctl_mock_get_open_count());
    fprintf(trace, "ioctl_count = %d\n", ioctl_mock_get_ioctl_count());
    fprintf(trace, "=== done ===\n");

    fclose(trace);
    return 0;
}
