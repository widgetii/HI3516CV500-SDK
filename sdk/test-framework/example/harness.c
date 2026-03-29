/*
 * Example test harness — adapt this to your module.
 *
 * This file exercises the blob's entry points and writes a trace log.
 * Both the blob-only and C-replaced binaries use the same harness,
 * producing traces that should be identical after address normalization.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "cmpi_mock.h"
#include "osal_mock.h"

/*
 * Declare your blob's entry points here. These are the global symbols
 * exported by the .o blob that you want to test.
 */
extern int MyModule_ModInit(void);
extern void MyModule_ModExit(void);
extern int my_module_create_chn(unsigned int chn_id, unsigned int *params);
extern int my_module_destroy_chn(unsigned int chn_id);

/*
 * Declare any blob globals you need to set before ModInit.
 * These are typically module parameters (one_stream_buffer, mini_buf_mode, etc.)
 */
// extern unsigned int g_my_module_one_stream_buffer;

/* ======== Test 1: Module lifecycle ======== */
static int test_mod_lifecycle(FILE *trace) {
    fprintf(stderr, "\n--- Test: ModInit / ModExit ---\n");

    int ret = MyModule_ModInit();
    fprintf(trace, "ModInit() = %d\n", ret);
    if (ret != 0) {
        fprintf(stderr, "FAIL: ModInit returned %d\n", ret);
        return 1;
    }

    /*
     * After ModInit, the module should have registered itself via
     * CMPI_RegisterModule. Find it by scanning the module table.
     */
    UMAP_MODULE_S *mod = NULL;
    for (int i = 0; i < 64; i++) {
        UMAP_MODULE_S *m = cmpi_get_module(i);
        /* Skip pre-registered mock modules */
        if (m && m != cmpi_get_module(1) && m != cmpi_get_module(2) &&
            m != cmpi_get_module(8) && m != cmpi_get_module(25)) {
            fprintf(stderr, "Found registered module at id=%d\n", i);
            mod = m;
            break;
        }
    }
    fprintf(trace, "module_registered = %s\n", mod ? "true" : "false");

    MyModule_ModExit();
    fprintf(trace, "ModExit() done\n");
    return 0;
}

/* ======== Test 2: Channel create/destroy ======== */
static int test_create_destroy(FILE *trace) {
    fprintf(stderr, "\n--- Test: Create / Destroy ---\n");

    int ret = MyModule_ModInit();
    fprintf(trace, "ModInit() = %d\n", ret);
    if (ret != 0) return 1;

    /*
     * Build your module's create_chn parameter structure here.
     * The layout is module-specific — reverse-engineer it from the blob.
     */
    unsigned int params[8] = {0};
    /* ... fill in params ... */

    ret = my_module_create_chn(0, params);
    fprintf(trace, "create_chn(0) = 0x%x\n", ret);

    if (ret == 0) {
        ret = my_module_destroy_chn(0);
        fprintf(trace, "destroy_chn(0) = 0x%x\n", ret);
    }

    MyModule_ModExit();
    fprintf(trace, "ModExit() done\n");
    return 0;
}

/* ======== Main ======== */
int main(int argc, char *argv[]) {
    const char *trace_path = "trace.log";
    if (argc > 1) trace_path = argv[1];

    FILE *trace = fopen(trace_path, "w");
    if (!trace) { perror("fopen"); return 1; }
    osal_mock_set_trace(trace);
    cmpi_mock_init();

    /*
     * If your module needs extra dependency modules beyond the defaults
     * (VB, SYS, VPSS, VI, RC, VEDU), register them here:
     *
     *   static void *venc_funcs[64] = { [0 ... 63] = stub_return_0 };
     *   cmpi_mock_register(14, "venc", venc_funcs);
     */

    /* Set module parameters before init */
    // g_my_module_one_stream_buffer = 0;

    fprintf(trace, "=== my_module harness ===\n");
    int failures = 0;
    failures += test_mod_lifecycle(trace);
    failures += test_create_destroy(trace);

    fprintf(trace, "failures = %d\n", failures);
    fclose(trace);
    fprintf(stderr, "\nResults: %d failures\n", failures);
    return failures ? 1 : 0;
}
