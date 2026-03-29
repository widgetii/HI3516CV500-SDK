/*
 * CMPI mock — public API for test harnesses.
 *
 * Provides the kernel module infrastructure (CMPI_RegisterModule,
 * CMPI_GetModuleFuncById, MMZ alloc/free) that ARM .o blobs expect.
 */
#ifndef CMPI_MOCK_H
#define CMPI_MOCK_H

#include <stdio.h>

/* Opaque module struct — matches kernel UMAP_MODULE_S layout */
typedef struct UMAP_MODULE_S UMAP_MODULE_S;

/*
 * Initialize mock module system.
 * Pre-registers stub modules for common dependencies (VB, SYS, VPSS, etc.).
 * Call this before invoking any blob ModInit function.
 */
void cmpi_mock_init(void);

/*
 * Register an additional mock dependency module.
 * Use this to add module-specific dependencies (e.g. VENC for JPEGE).
 *   mod_id:     module ID (0-63)
 *   name:       short name (max 15 chars)
 *   func_table: pointer to an array of function pointers (the export table)
 */
void cmpi_mock_register(int mod_id, const char *name, void *func_table);

/* Look up a registered module by ID (returns NULL if not registered) */
UMAP_MODULE_S *cmpi_get_module(int id);

#endif /* CMPI_MOCK_H */
