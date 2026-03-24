# libmpi: Vendor Comparison & Status

Comparison against vendor SDK V2.0.2.1 `libmpi.a` (dated May 28, 2020).

## Size Comparison

| | Vendor | Ours |
|---|---|---|
| Static lib (.a) | 743K (stripped) | 200K (with debug) |
| Shared lib (.so) | 355K (stripped) | 185K (with debug) |
| Object files | 25 | 11 |
| Exported symbols | 782 | 316 |
| Shared symbols | 258 | 258 |
| Our-only symbols (internal) | — | 58 |
| Vendor-only symbols | 524 | — |

## Per-Module API Coverage (258/782 shared — 33%)

### Fully implemented (in build)

| Module | Vendor symbols | Our symbols | Coverage |
|--------|---------------|-------------|----------|
| mpi_vi | 104 | 104 | 104/104 (100%) |
| mpi_vb | 23 | 23 | 23/23 (100%) |
| mpi_region | 14 | 14 | 14/14 (100%) |
| mpi_snap | 10 | 10 | 10/10 (100%) |
| mpi_gdc | 8 | 8 | 8/8 (100%) |
| mpi_log | 5 | 5 | 5/5 (100%) |
| mpi_audio | 3 | 3 | 3/3 (100%) |

### Nearly complete (in build)

| Module | Vendor symbols | Our symbols | Coverage | Missing |
|--------|---------------|-------------|----------|---------|
| mpi_sys | 40 | 39 | 39/40 (97%) | `HI_MPI_SYS_GetUniqueId` |
| mpi_ao | 30 | 29 | 29/30 (96%) | `HI_MPI_AO_GetChnDelay` |

### Also in build (support files)

| Module | Vendor .o | Our .c |
|--------|-----------|--------|
| af_buf | af_buf.o | af_buf.c |
| as_buf | as_buf.o | as_buf.c |
| audio_comm | audio_comm.o | audio_comm.c |

### Not implemented (vendor-only)

| Module | Vendor .o | Symbols | Source exists in repo? |
|--------|-----------|---------|----------------------|
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

### Symbols only in our build (58)

Internal/helper functions (lowercase `hi_mpi_*`, `mpi_vi_*`, `mpi_ao_*`, `ao_check_*`, etc.) that the vendor strips from their release. Expected — our build is not stripped.

## mpi_vi Integration Notes

`mpi_vi.c` was added to the build with all 104/104 vendor API symbols.

### What was fixed

1. **`re_mpi_vi.h` duplicate struct** — removed second `hiVI_TIME_FRAME2_S` definition (size 0x168 variant, kept size 0x020 variant)
2. **Ioctl macro redefinitions** — renamed duplicate `VI_CTL_SETPIPEATTR`/`VI_CTL_GETPIPEATTR` at 0x1E/0x1F to `VI_CTL_SETPIPEFRMINTERRUPTATTR2`/`VI_CTL_GETPIPEFRMINTERRUPTATTR2`
3. **`HI_ASSERT` conflict** — guarded with `#ifndef` in `re_debug.h`
4. **Missing includes** — added `mpi_sys.h`, `stdio.h`, `string.h`, `unistd.h`, `sys/ioctl.h`, `fcntl.h`
5. **`inline` warning** — removed `inline` from `MPI_VI_CheckStitchId` declaration

### 11 missing APIs — now implemented

| Function | Implementation |
|----------|---------------|
| `HI_MPI_VI_CloseFd` | Full — closes all vi fds (dev/pipe/chn) |
| `HI_MPI_VI_SendPipeYUV` | Full — ioctl via `VI_CTL_SENDPIPEYUV` |
| `HI_MPI_VI_SendPipeRaw` | Full — ioctl via `VI_CTL_SENDPIPERAW` |
| `HI_MPI_VI_SetChnRotation` | Full — ioctl via `VI_CTL_SETCHNROTATION` |
| `HI_MPI_VI_SetChnRotationEx` | Full — ioctl via `VI_CTL_SETCHNROTATIONEX` |
| `HI_MPI_VI_SetChnLDCAttr` | Full — ioctl via `VI_CTL_SETCHNLDCATTR` |
| `HI_MPI_VI_SetChnSpreadAttr` | Full — ioctl via `VI_CTL_SETCHNSPREADATTR` |
| `HI_MPI_VI_SetChnDISParam` | Passthrough — ioctl via `VI_CTL_SETCHNDISATTR` (param type unknown) |
| `HI_MPI_VI_GetChnDISParam` | Passthrough — ioctl via `VI_CTL_GETCHNDISATTR` (param type unknown) |
| `HI_MPI_VI_SetExtChnFisheye` | Full — ioctl via `VI_CTL_SETEXTCHNFISHEYE` |
| `HI_MPI_VI_FisheyePosQueryDst2Src` | Stub — returns `ERR_VI_NOT_SUPPORT` (needs GDC point query impl) |

### Remaining internal stub

`hi_mpi_vi_set_chn_spread_attr` (lowercase internal helper) — returns `ERR_VI_NOT_SUPPORT`. The full RE implementation exists commented out but was incomplete.
