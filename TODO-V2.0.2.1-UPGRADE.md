# TODO: V2.0.2.0 → V2.0.2.1 Upgrade

Source base: V2.0.2.0 (`Hi3516CV500_MPP_V2.0.2.0 B030 Release`)
Target: V2.0.2.1 (`Hi3516CV500_MPP_V2.0.2.1 B030 Release`)

Vendor disassembly reference: `sdk/vendor-disasm/{v2_0_2_0,v2_0_2_1}/` (94 .S files, 40 diffs)

---

## libmpi (22→25 .o files, 759→782 symbols)

### 1. New module: mpi_mcf (Multi-Channel Fusion)

**Status:** Not started — no V2.0.2.0 source exists
**Source:** `sdk/vendor-disasm/v2_0_2_1/libmpi/mpi_mcf.S`
**Action:** RE all 15 functions from V2.0.2.1 disassembly, create `sdk/libraries/mpi/mpi_mcf.c`

Functions (15):
- [ ] `hi_mpi_mcf_create_grp`
- [ ] `hi_mpi_mcf_destroy_grp`
- [ ] `hi_mpi_mcf_set_grp_attr`
- [ ] `hi_mpi_mcf_get_grp_attr`
- [ ] `hi_mpi_mcf_start_grp`
- [ ] `hi_mpi_mcf_stop_grp`
- [ ] `hi_mpi_mcf_enable_chn`
- [ ] `hi_mpi_mcf_disable_chn`
- [ ] `hi_mpi_mcf_set_alg_param`
- [ ] `hi_mpi_mcf_get_alg_param`
- [ ] `hi_mpi_mcf_close_fd`
- [ ] `mpi_mcf_check_grp_id.part.0` (internal)
- [ ] `mpi_mcf_check_grp_open` (internal)
- [ ] `mpi_mcf_check_chn_open` (internal)
- [ ] `mpi_mcf_check_null_ptr.part.1` (internal)

**Depends on:** V2.0.2.1 headers — check `/tmp/Hi3516CV500_SDK_V2.0.2.1/` for `mpi_mcf.h`

### 2. VENC: new APIs + adapt wrappers

**Status:** Partially done — we have the uppercase `HI_MPI_VENC_*` functions
**Source:** `sdk/vendor-disasm/diffs/libmpi_mpi_venc.diff` (+690 lines)
**File:** `sdk/libraries/mpi/mpi_venc.c`

New public APIs to RE (10 functions):
- [ ] `HI_MPI_VENC_EnableSvc` / `hi_mpi_venc_enable_svc`
- [ ] `HI_MPI_VENC_SetSvcParam` / `hi_mpi_venc_set_svc_param`
- [ ] `HI_MPI_VENC_GetSvcParam` / `hi_mpi_venc_get_svc_param`
- [ ] `HI_MPI_VENC_GetSvcSceneComplexity` / `hi_mpi_venc_get_svc_scene_complexity`
- [ ] `HI_MPI_VENC_SetChnConfig` / `hi_mpi_venc_set_chn_config`
- [ ] `HI_MPI_VENC_GetChnConfig` / `hi_mpi_venc_get_chn_config`
- [ ] `HI_MPI_VENC_SetSearchWindow` / `hi_mpi_venc_set_search_window`
- [ ] `HI_MPI_VENC_GetSearchWindow` / `hi_mpi_venc_get_search_window`
- [ ] `HI_MPI_VENC_SendMultiFrame` / `hi_mpi_venc_send_multi_frame`
- [ ] `HI_MPI_VENC_SendSvcRegion` / `hi_mpi_venc_send_svc_region`

Renamed functions (update existing):
- [ ] `hi_mpi_venc_get_cu_prediction` → `hi_mpi_venc_get_cu_pred`
- [ ] `hi_mpi_venc_set_cu_prediction` → `hi_mpi_venc_set_cu_pred`
- [ ] `hi_mpi_venc_get_foreground_protect` → `hi_mpi_venc_get_fg_protect`
- [ ] `hi_mpi_venc_set_foreground_protect` → `hi_mpi_venc_set_fg_protect`
- [ ] `hi_mpi_venc_get_h265_pred_unit` → `hi_mpi_venc_get_h265_pu`
- [ ] `hi_mpi_venc_set_h265_pred_unit` → `hi_mpi_venc_set_h265_pu`
- [ ] `hi_mpi_venc_start_recv_frame` → `hi_mpi_venc_start_chn`
- [ ] `hi_mpi_venc_stop_recv_frame` → `hi_mpi_venc_stop_chn`

New internal function:
- [ ] `mpi_venc_open` (replaces `MPI_VENC_OPEN` macro or old `MPI_VENC_Init`)

### 3. Audio adapt layer (replaces old .o files)

**Status:** Our existing mpi_ai.c/mpi_ao.c/etc have the implementations but V2.0.2.1 restructured into `*_adapt.o` with different naming
**Source:** `sdk/vendor-disasm/v2_0_2_1/libmpi/mpi_ai_adapt.S` etc.
**Action:** Verify our implementations match V2.0.2.1 behavior, add missing functions

New AI functions:
- [ ] `HI_MPI_AI_SetChnAttr` / `HI_MPI_AI_GetChnAttr`
- [ ] `HI_MPI_AI_SetTalkVqeV2Attr` / `HI_MPI_AI_GetTalkVqeV2Attr` (alias `hi_mpi_ai_set_talk_vqe_v2_attr`)
- [ ] `HI_MPI_AI_SetClkDir` / `HI_MPI_AI_GetClkDir`

New AO function:
- [ ] `HI_MPI_AO_GetChnDelay`

### 4. af_buf / as_buf (audio buffer management)

**Status:** Currently empty compilation units
**Source:** `sdk/vendor-disasm/v2_0_2_1/libmpi/af_buf.S` (253 lines), `as_buf.S` (109 lines)
**File:** `sdk/libraries/mpi/af_buf.c`, `sdk/libraries/mpi/as_buf.c`

Functions to implement:
- [ ] `af_buf_init`
- [ ] `af_buf_reset`
- [ ] `af_buf_get_free`
- [ ] `af_buf_get_busy`
- [ ] `af_buf_is_list_mem`
- [ ] `af_buf_is_free_list_mem`
- [ ] `af_buf_is_busy_list_mem`
- [ ] `as_buf_init`
- [ ] `as_buf_get_free`
- [ ] `as_buf_get_busy`

### 5. Other libmpi changes (minor)

**mpi_sys.c:**
- [ ] `HI_MPI_SYS_GetUniqueId` (new API)
- [ ] `mpi_sys_get_hr_timer` (new internal)

**mpi_vi.c:**
- [ ] `HI_MPI_VI_SetChnDISParam` / `HI_MPI_VI_GetChnDISParam` — currently ioctl passthroughs, verify ioctl codes match V2.0.2.1

**mpi_vb.c:**
- [ ] `mpi_vb_init_ctx` / `mpi_vb_exit_ctx` (new internals)

**mpi_vpss.c:**
- [ ] `mpi_vpss_check_fisheye_region_index` (new internal)
- [ ] `mpi_vpss_set_gdc_comm_cfg` (new internal)
- [ ] Various `mpi_vpss_check_*_return.part.*` (compiler-generated split functions)

**mpi_bind.c:**
- [ ] `mpi_sys_bind_unregister_sender` / `mpi_sys_bind_unregister_receiver` (renamed from `un_register`)
- [ ] `mpi_sys_deinit_send_bind_src_mem` (new)
- [ ] `mpi_sys_send_check_reciever_avalid` (new)

**GDC naming aliases:**
- [ ] Add lowercase wrappers: `gdc_fisheye_configure` → `GDC_Fisheye_CFG`, etc.

### 6. VQE adapters

**Source:** `sdk/vendor-disasm/v2_0_2_1/libmpi/hi_dnvqe_api_adp.S`, `hi_upvqe_api_adp.S`
**Status:** These are thin wrappers — each has ~1 function. Low priority.

- [ ] `hi_dnvqe_api_adp` — adapt layer for dnvqe
- [ ] `hi_upvqe_api_adp` — adapt layer for upvqe

---

## lib_hiae (9→10 .o files)

### 7. New module: hi_ae_quick_start (extracted from hi_ae_adp)

**Source:** `sdk/vendor-disasm/v2_0_2_1/lib_hiae/hi_ae_quick_start.S`
**Action:** Create `sdk/libraries/hiae/hi_ae_quick_start.c`

Functions (36):
- [ ] `ae_sup_quick_start_initialize`
- [ ] `ae_sup_quick_start_reinitialize`
- [ ] `ae_quick_start_state_update`
- [ ] `ae_quick_start_set_cmos_status`
- [ ] `ae_quick_start_complete`
- [ ] `ae_sup_quick_start_check_iso_state`
- [ ] `ae_sup_quick_start_init` / `ae_sup_quick_start_init_ir`
- [ ] `ae_sup_quick_start_process`
- [ ] `ae_quick_start_process_delay_3frm` / `ae_quick_start_process_delay_3frm_ir`
- [ ] `ae_delay3_case1` / `ae_delay3_case2` / `ae_delay3_case3`
- [ ] `ae_delay3_default` / `ae_delay3_default_ir`
- [ ] `ae_delay3_default_over_target` / `ae_delay3_default_less_target`
- [ ] 17 more `ae_d3c*` / `ae_d3def*` sub-functions

### 8. hi_ae_adp: refactored (56→84 functions, renamed to snake_case)

**Status:** Our V2.0.2.0 implementation has 55 functions with PascalCase names
**Action:** V2.0.2.1 renamed all functions to snake_case AND added ~28 new sub-functions (split from large functions)
**Diff:** `sdk/vendor-disasm/diffs/lib_hiae_hi_ae_adp.diff`

Key changes:
- [ ] All function names renamed (e.g., `AeExit` → `ae_exit`, `AeInit` → `ae_init`)
- [ ] Large functions split: `ae_ext_regs_read` split into `ae_ext_regs_ae_debug`, `ae_ext_regs_ae_delay`, `ae_ext_regs_ae_flicker`, `ae_ext_regs_ae_iris`, etc.
- [ ] New: `ae_ext_write_ae_route`, `ae_ext_write_ae_route_ex`, `ae_ext_write_exp_time`
- [ ] New: `ae_hist_large_color_proc`, `ae_calc_time_step_limit`, `ae_set_sensor_calc_ratio`
- [ ] New: `ae_exp_param_convert`, `isp_get_ae_ctx`
- [ ] New: `ae_increment_ext_regs_read`

### 9. mpi_isp_ae: new APIs

**Source:** `sdk/vendor-disasm/diffs/lib_hiae_mpi_isp_ae.diff`
**File:** `sdk/libraries/hiae/mpi_isp_ae.c`

- [ ] `HI_MPI_ISP_SetExpConvert` / `HI_MPI_ISP_GetExpConvert`
- [ ] `ae_check_dev_open` / `ae_check_mem_init_func` (refactored helpers)

### 10. hi_iris_pwm: new PWM approach

**Source:** `sdk/vendor-disasm/diffs/lib_hiae_hi_iris_pwm.diff` (+302 lines)

- [ ] `write_sys_pwm` / `write_int` / `ioctl_dev_pwm` (sysfs-based PWM instead of ioctl-only)
- [ ] `ae_dc_iris_register_callback` (renamed from `AeDCiris_register_callback`)

### 11. hi_ae_increment: expanded (16→23 functions)

**Source:** `sdk/vendor-disasm/diffs/lib_hiae_hi_ae_increment.diff`

New functions:
- [ ] `ae_calc_bias`, `ae_calc_tunnel_in_bias`, `ae_calc_tunnel_out_bias`
- [ ] `ae_hdr_hist_calc_wdr`
- [ ] `ae_histogram_mem_array_calc.isra.2`
- [ ] `ae_interpulate`
- [ ] `ae_max_n_min_zone_calc`, `ae_mid_zone_calc`

---

## libive (5 .o files, moderate changes)

### 12. mpi_ive.c: +3435 lines of implementation changes

**Source:** `sdk/vendor-disasm/diffs/libive_mpi_ive.diff`
**Status:** Function count unchanged (84) but implementations significantly modified
**Action:** Diff function-by-function to identify behavioral changes

- [ ] Audit all 84 functions for ioctl code changes
- [ ] Check for new struct field accesses or validation logic

---

## libmd (4 .o files, minor changes)

### 13. ivs_md.c: +99 lines

**Source:** `sdk/vendor-disasm/diffs/libmd_ivs_md.diff`
- [ ] Review 12 functions for behavioral changes

---

## libdnvqe (5 .o files, moderate changes)

### 14. Function naming: all renamed to snake_case

V2.0.2.1 renamed all functions:
- `DNVQE_Create` → `dnvqe_create`
- `HI_DNVQE_Create` → `hi_dnvqe_create`
- `RES_ReSampler_Create` → `RES_resampler_create`
- etc.

### 15. New functions

- [ ] `hi_resample_work.c`: `update_cache.isra.0` (new), `RES_resampler_process_frame` (+288 lines)
- [ ] `hi_audio_module_wrap.c`: `audio_get_module_dl_handle` (new)

---

## Action plan (priority order)

### Phase 1: Mechanical (low risk, high coverage)
1. Add GDC lowercase aliases (`gdc_fisheye_configure` etc.) — trivial wrappers
2. Add VENC renamed functions as aliases (old names call new names)
3. Add `HI_MPI_SYS_GetUniqueId` — single ioctl function
4. Add `HI_MPI_AO_GetChnDelay` — single ioctl function
5. Verify VI DIS ioctl codes match V2.0.2.1

### Phase 2: New APIs (RE from V2.0.2.1 disassembly)
6. VENC new APIs: SVC, ChnConfig, SearchWindow, SendMultiFrame (10 functions)
7. AI new APIs: ChnAttr, TalkVqeV2, ClkDir (6 functions)
8. af_buf / as_buf implementations (10 functions, small)
9. mpi_mcf module (15 functions, new)

### Phase 3: Refactoring (match V2.0.2.1 structure)
10. hiae function renaming to snake_case
11. hiae hi_ae_quick_start extraction (36 functions)
12. hiae hi_ae_adp split functions (28 new sub-functions)
13. dnvqe function renaming + new functions
14. libive mpi_ive behavioral audit

### Phase 4: Validation
15. Symbol table comparison: our build vs V2.0.2.1 vendor .a
16. Ioctl code audit across all modules
17. Hardware testing on V2.0.2.1 production system
