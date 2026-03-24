# libmpi: Vendor Comparison & Status

Comparison against vendor SDK V2.0.2.1 `libmpi.a` (dated May 28, 2020).

## Size Comparison

| | Vendor | Ours |
|---|---|---|
| Static lib (.a) | 743K (stripped) | 151K (with debug) |
| Shared lib (.so) | 355K (stripped) | 138K (with debug) |
| Object files | 25 | 10 |
| Exported symbols | 782 | 181 |
| Shared symbols | 140 | 140 |

## Module Coverage (10 of 25)

### Implemented (in build)

| Module | Vendor .o | Our .c | API match |
|--------|-----------|--------|-----------|
| af_buf | af_buf.o | af_buf.c | yes |
| as_buf | as_buf.o | as_buf.c | yes |
| audio_comm | audio_comm.o | audio_comm.c | yes |
| mpi_ao | mpi_ao_adapt.o | mpi_ao.c | yes (140 shared symbols) |
| mpi_audio | mpi_audio_adapt.o | mpi_audio.c | yes |
| mpi_bind | mpi_bind.o | mpi_bind.c | yes |
| mpi_gdc | mpi_gdc.o | mpi_gdc.c | yes |
| mpi_region | mpi_region.o | mpi_region.c | yes |
| mpi_sys | mpi_sys.o | mpi_sys.c | yes |
| mpi_vb | mpi_vb.o | mpi_vb.c | yes |

### Not implemented (vendor-only)

| Module | Vendor .o | Symbols | Source exists in repo? |
|--------|-----------|---------|----------------------|
| mpi_vi | mpi_vi.o | 104 | **yes** — mpi_vi.c (5369 lines, 93/104 APIs) |
| mpi_venc | mpi_venc.o | 100 | yes — mpi_venc.c (commented out, unverified) |
| mpi_vo | mpi_vo.o | 91 | yes — mpi_vo.c (commented out, unverified) |
| mpi_vpss | mpi_vpss.o | 69 | yes — mpi_vpss.c (commented out, unverified) |
| mpi_ai | mpi_ai_adapt.o | 34 | yes — mpi_ai.c (commented out, has TODO) |
| mpi_vdec | mpi_vdec.o | 32 | yes — mpi_vdec.c (commented out, unverified) |
| mpi_vgs | mpi_vgs.o | 12 | yes — mpi_vgs.c (commented out, unverified) |
| mpi_adec | mpi_adec_adapt.o | 11 | yes — mpi_adec.c (commented out) |
| mpi_aenc | mpi_aenc_adapt.o | 11 | yes — mpi_aenc.c (commented out) |
| mpi_mcf | mpi_mcf.o | — | no |
| audio_voice_adp | audio_voice_adp.o | — | yes — audio_voice_adp.c (commented out) |
| hiisp_gdc_fw_pointquery | hiisp_gdc_fw_pointquery.o | — | yes (commented out) |
| hiisp_gdc_fw_user | hiisp_gdc_fw_user.o | — | yes (commented out) |
| hi_dnvqe_api_adp | hi_dnvqe_api_adp.o | — | no |
| hi_upvqe_api_adp | hi_upvqe_api_adp.o | — | no |

### Symbols only in our build (41)

These are internal/helper functions (lowercase `hi_mpi_*`, `mpi_ao_*`, `ao_check_*`, etc.) that the vendor strips from their release. Expected — our build is not stripped.

## mpi_vi Assessment

`mpi_vi.c` is the closest candidate for inclusion in the build.

### What exists

- 5369 lines of reverse-engineered C (dated October 3, 2020)
- 93 of 104 vendor `HI_MPI_VI_*` functions implemented
- Full ioctl command table in `re_mpi_vi.h` (0x00–0x73)
- Internal helpers: pipe/dev/chn open, mutex locking, validation
- Reverse-engineered internal structs with size annotations

### Missing APIs (11)

```
HI_MPI_VI_CloseFd
HI_MPI_VI_FisheyePosQueryDst2Src
HI_MPI_VI_GetChnDISParam
HI_MPI_VI_SendPipeRaw
HI_MPI_VI_SendPipeYUV
HI_MPI_VI_SetChnDISParam
HI_MPI_VI_SetChnLDCAttr
HI_MPI_VI_SetChnRotation
HI_MPI_VI_SetChnRotationEx
HI_MPI_VI_SetChnSpreadAttr
HI_MPI_VI_SetExtChnFisheye
```

### Compile blockers (2 errors, 9 warnings)

**Errors:**
1. `re_mpi_vi.h:44` — duplicate definition of `struct hiVI_TIME_FRAME2_S` (defined at line 20 and again at line 44 with different layout, appears to be WIP notes)
2. `re_mpi_vi.h:52` — conflicting types for `VI_TIME_FRAME2_S` (consequence of #1)

**Warnings:**
- `re_mpi_vi.h:193,194` — `VI_CTL_SETPIPEATTR` / `VI_CTL_GETPIPEATTR` redefined with different ioctl numbers (lines 184-185 vs 193-194, likely version A vs version B of the ioctl table)
- `re_debug.h:8` — `HI_ASSERT` redefined (conflicts with `hi_debug.h` from sdk/common)
- `mpi_vi.c:30` — implicit declaration of `HI_MPI_SYS_GetVIVPSSMode` (needs header or forward declaration)
- Other implicit function declaration warnings

### What it would take to add mpi_vi

1. **Fix `re_mpi_vi.h` duplicate struct** — remove or ifdef the second `hiVI_TIME_FRAME2_S` definition (line 44-52). The developer left two competing RE interpretations in the file.
2. **Fix ioctl macro redefinitions** — choose the correct `VI_CTL_SETPIPEATTR`/`VI_CTL_GETPIPEATTR` values (one pair should be removed or renamed).
3. **Fix `HI_ASSERT` conflict** — either remove the redefinition in `re_debug.h` or guard it with `#ifndef`.
4. **Add missing forward declaration** for `HI_MPI_SYS_GetVIVPSSMode` or include the right header.
5. **Implement 11 missing API stubs** — at minimum, provide error-returning stubs for the 11 missing functions so the symbol table matches vendor.
6. **Uncomment in CMakeLists.txt** — add `"mpi/mpi_vi.c"` to MPI_FILES.
7. **Test compile** and fix any remaining warnings.

Estimated effort: the header cleanup is straightforward. The 11 missing functions need ioctl wiring (the ioctl codes are already defined in `re_mpi_vi.h`). Most follow the same pattern as existing functions: validate args → open fd → ioctl → return.
