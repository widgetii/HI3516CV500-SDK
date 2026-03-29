/*
 * Mock CMPI module system for userspace testing of ARM kernel blobs.
 *
 * Provides:
 *  - CMPI_RegisterModule / CMPI_UnRegisterModule / CMPI_GetModuleFuncById
 *  - MMZ alloc/free with tracking
 *  - VB (Video Buffer) mock with handle-based allocation
 *  - SYS module mock (alignment, MMZ name)
 *  - Stub function tables for other dependency modules
 *  - HI_LOG, cache flush stubs
 *
 * All function tables are pre-filled with stub_return_0 so that any
 * function pointer dereference returns 0/success by default.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>

/* Types matching the kernel definitions */
typedef int GK_S32;
typedef unsigned int GK_U32;
typedef unsigned long long GK_U64;
typedef unsigned long GK_UL;
typedef void GK_VOID;
typedef char GK_CHAR;

struct osal_list_head {
    struct osal_list_head *next, *prev;
};

typedef GK_S32 FN_MOD_Init(GK_VOID *);
typedef GK_VOID FN_MOD_Exit(GK_VOID);
typedef GK_VOID FN_MOD_Notify(int);
typedef GK_VOID FN_MOD_QueryState(int *);
typedef GK_U32 FN_MOD_VerChecker(GK_VOID);

typedef struct UMAP_MODULE_S {
    struct osal_list_head list;
    GK_CHAR aModName[16];
    int enModId;
    FN_MOD_Init *pfnInit;
    FN_MOD_Exit *pfnExit;
    FN_MOD_QueryState *pfnQueryState;
    FN_MOD_Notify *pfnNotify;
    FN_MOD_VerChecker *pfnVerChecker;
    int bInited;
    GK_VOID *pstExportFuncs;
    GK_VOID *pData;
    GK_CHAR *pVersion;
} UMAP_MODULE_S;

/* ---- Module registry (up to 64 modules) ---- */
static UMAP_MODULE_S *registered_modules[64] = {0};

/* ---- Stub functions ---- */
static int stub_return_0(void) { return 0; }

/* ---- Per-module function pointer tables ---- */
#define TABLE_SIZE 64
static void *generic_func_table[TABLE_SIZE];
static void *sys_func_table[TABLE_SIZE];
static void *vb_func_table[TABLE_SIZE];
static void *vedu_func_table[TABLE_SIZE];

/* ---- SYS module (id=2) ---- */
static int sys_get_max_size(unsigned int width, unsigned int *out) {
    if (out) *out = ((width + 15) >> 4) << 4;
    return 0;
}

static int sys_get_mmz_name(void *params, void *pool_out) {
    return 0;
}

/* ---- VB module (id=1) ---- */
static unsigned int vb_next_handle = 0x10000;

static int vb_malloc(unsigned int *handle, unsigned int a, unsigned int size,
                     unsigned int b, char *name, unsigned int c) {
    *handle = vb_next_handle++;
    return 0;
}
static int vb_free(unsigned int handle) { return 0; }
static int vb_get_blk(unsigned int handle, unsigned int type) { return -1; }
static unsigned int vb_handle2pool(unsigned int h) { return 0; }
static unsigned long long vb_handle2phys(unsigned int h) {
    return (unsigned long long)0x80000000ULL + h * 0x10000;
}
static unsigned int vb_handle2size(unsigned int h) { return 0x400000; }
static int vb_user_sub(unsigned int pool, unsigned int p1, unsigned int p2,
                       unsigned int type) { return 0; }
static int vb_get_blk_info(void *a, void *b) { return 0; }

/* ---- Static mock module storage ---- */
static UMAP_MODULE_S mock_modules_storage[64];
static int mock_modules_used = 0;

static void init_all_tables(void) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        generic_func_table[i] = (void *)stub_return_0;
        sys_func_table[i]     = (void *)stub_return_0;
        vb_func_table[i]      = (void *)stub_return_0;
        vedu_func_table[i]    = (void *)stub_return_0;
    }

    /* SYS overrides */
    sys_func_table[4]  = (void *)sys_get_max_size;
    sys_func_table[15] = (void *)sys_get_mmz_name;

    /* VB overrides */
    vb_func_table[0]  = (void *)vb_malloc;
    vb_func_table[1]  = (void *)vb_free;
    vb_func_table[2]  = (void *)vb_get_blk;
    vb_func_table[10] = (void *)vb_handle2pool;
    vb_func_table[13] = (void *)vb_handle2phys;
    vb_func_table[14] = (void *)vb_handle2size;
    vb_func_table[15] = (void *)vb_get_blk_info;
    vb_func_table[16] = (void *)vb_user_sub;
}

static void register_builtin(int mod_id, const char *name, void *func_table) {
    UMAP_MODULE_S *m = &mock_modules_storage[mock_modules_used++];
    memset(m, 0, sizeof(*m));
    m->enModId = mod_id;
    strncpy(m->aModName, name, 15);
    m->pstExportFuncs = func_table;
    registered_modules[mod_id] = m;
}

void cmpi_mock_init(void) {
    memset(registered_modules, 0, sizeof(registered_modules));
    mock_modules_used = 0;
    init_all_tables();

    /* Common dependencies shared by most HiSilicon kernel modules */
    register_builtin(1,  "vb",   vb_func_table);
    register_builtin(2,  "sys",  sys_func_table);
    register_builtin(8,  "vpss", generic_func_table);
    register_builtin(10, "vi",   generic_func_table);
    register_builtin(19, "rc",   generic_func_table);
    register_builtin(25, "vedu", vedu_func_table);
}

void cmpi_mock_register(int mod_id, const char *name, void *func_table) {
    if (mod_id < 0 || mod_id >= 64) return;
    UMAP_MODULE_S *m = &mock_modules_storage[mock_modules_used++];
    memset(m, 0, sizeof(*m));
    m->enModId = mod_id;
    if (name) strncpy(m->aModName, name, 15);
    m->pstExportFuncs = func_table;
    registered_modules[mod_id] = m;
}

UMAP_MODULE_S *cmpi_get_module(int id) {
    return (id >= 0 && id < 64) ? registered_modules[id] : NULL;
}

/* ---- CMPI kernel API ---- */
GK_S32 CMPI_RegisterModule(UMAP_MODULE_S *pstModule) {
    if (!pstModule) return -1;
    int id = pstModule->enModId;
    if (id < 0 || id >= 64) return -1;
    registered_modules[id] = pstModule;
    fprintf(stderr, "[CMPI] RegisterModule: id=%d name='%s' export=%p\n",
            id, pstModule->aModName, pstModule->pstExportFuncs);
    return 0;
}

GK_VOID CMPI_UnRegisterModule(int enModId) {
    if (enModId >= 0 && enModId < 64) {
        fprintf(stderr, "[CMPI] UnRegisterModule: id=%d\n", enModId);
        registered_modules[enModId] = NULL;
    }
}

GK_VOID *CMPI_GetModuleFuncById(int enModId) {
    if (enModId >= 0 && enModId < 64 && registered_modules[enModId])
        return registered_modules[enModId]->pstExportFuncs;
    return NULL;
}

/* ---- MMZ (Media Memory Zone) ---- */
#define MAX_MMZ_ALLOCS 64
static struct {
    GK_U64 phys;
    void *virt;
    GK_UL size;
} mmz_allocs[MAX_MMZ_ALLOCS];
static int mmz_alloc_count = 0;

GK_S32 CMPI_MmzMallocNocache(GK_CHAR *cpMmzName, const char *pBufName,
                              GK_U64 *pu64PhyAddr, GK_VOID **ppVirAddr,
                              GK_UL ulLen) {
    void *p = calloc(1, ulLen + 64);
    if (!p) return -1;
    *ppVirAddr = p;
    *pu64PhyAddr = (GK_U64)(uintptr_t)p;
    if (mmz_alloc_count < MAX_MMZ_ALLOCS) {
        mmz_allocs[mmz_alloc_count].phys = *pu64PhyAddr;
        mmz_allocs[mmz_alloc_count].virt = p;
        mmz_allocs[mmz_alloc_count].size = ulLen;
        mmz_alloc_count++;
    }
    fprintf(stderr, "[MMZ] MallocNocache(%s, %lu) = %p\n",
            pBufName ? pBufName : "", ulLen, p);
    return 0;
}

GK_S32 CMPI_MmzMallocCached(GK_CHAR *cpMmzName, const char *pBufName,
                             GK_U64 *pu64PhyAddr, GK_VOID **ppVirAddr,
                             GK_UL ulLen) {
    return CMPI_MmzMallocNocache(cpMmzName, pBufName, pu64PhyAddr, ppVirAddr, ulLen);
}

GK_VOID CMPI_MmzFree(GK_U64 u64PhyAddr, GK_VOID *pVirAddr) {
    void *to_free = NULL;
    for (int i = 0; i < mmz_alloc_count; i++) {
        GK_U64 base = mmz_allocs[i].phys;
        GK_U64 end = base + mmz_allocs[i].size + 64;
        if ((u64PhyAddr >= base && u64PhyAddr < end) ||
            mmz_allocs[i].virt == pVirAddr) {
            to_free = mmz_allocs[i].virt;
            mmz_allocs[i] = mmz_allocs[--mmz_alloc_count];
            break;
        }
    }
    fprintf(stderr, "[MMZ] MmzFree(%p)\n", to_free ? to_free : pVirAddr);
    if (to_free) free(to_free);
}

/* ---- Cache operations (no-ops) ---- */
void hil_mmb_flush_dcache_byaddr(void *addr, unsigned long phys,
                                 unsigned long len) {
    (void)addr; (void)phys; (void)len;
}

void hil_mmb_invalid_cache_byaddr(void *addr, unsigned long phys,
                                  unsigned long len) {
    (void)addr; (void)phys; (void)len;
}

/* ---- Logging ---- */
int HI_LOG(int level, int mod, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int ret = vfprintf(stderr, fmt, ap);
    va_end(ap);
    return ret;
}

/* ---- Shared globals (used by venc-family blobs) ---- */
int g_frame_buf_recycle = 0;
int g_venc_buffer_cache = 0;
