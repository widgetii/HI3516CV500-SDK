# libmpi: Vendor Comparison & Status

Comparison against vendor SDK V2.0.2.1 `libmpi.a` (dated May 28, 2020).

## Size Comparison

| | Vendor | Ours |
|---|---|---|
| Static lib (.a) | 743K (stripped) | ~350K (with debug) |
| Shared lib (.so) | 355K (stripped) | ~320K (with debug) |
| Object files | 25 | 22 |
| Exported symbols | 782 | 755 |
| HI_MPI_* API functions | ~588 | 589 |
| Our-only symbols (internal) | — | ~166 |
| Vendor-only symbols | ~27 | — |

## Per-Module API Coverage (589/~588 — 100%+)

### Fully implemented (all in build)

| Module | Vendor symbols | Our symbols | Coverage |
|--------|---------------|-------------|----------|
| mpi_vi | 104 | 104 | 100% |
| mpi_vo | 91 | 91 | 100% |
| mpi_venc | 88 | 90 | 100%+ (includes SliceSplit) |
| mpi_vpss | 69 | 69 | 100% |
| mpi_sys | 40 | 40 | 100% |
| mpi_ai | 37 | 37 | 100% |
| mpi_vdec | 32 | 32 | 100% |
| mpi_ao | 30 | 30 | 100% |
| mpi_vb | 23 | 23 | 100% |
| mpi_region | 14 | 14 | 100% |
| mpi_vgs | 12 | 12 | 100% |
| mpi_aenc | 11 | 12 | 100%+ (extra VoiceInit) |
| mpi_adec | 11 | 12 | 100%+ (extra VoiceInit) |
| mpi_snap | 10 | 10 | 100% |
| mpi_gdc | 8 | 8 | 100% |
| mpi_log | 5 | 5 | 100% |
| mpi_audio | 3 | 3 | 100% |

### Also in build (support files)

| Module | Our .c | Notes |
|--------|--------|-------|
| audio_voice_adp | audio_voice_adp.c | 38 internal functions, VQE adaptation layer |
| hiisp_gdc_fw_pointquery | hiisp_gdc_fw_pointquery.c | 13 GDC point query functions |
| hiisp_gdc_fw_user | hiisp_gdc_fw_user.c + .S | GDC firmware entry + lookup tables |
| audio_comm | audio_comm.c | 2 audio utilities |
| af_buf | af_buf.c | Stub (no-content, reserved) |
| as_buf | as_buf.c | Stub (no-content, reserved) |

### Vendor-only modules (not applicable)

| Module | Notes |
|--------|-------|
| mpi_mcf | Multi-Channel Fusion — no header exists, likely not for HI3516CV500 |
| hi_dnvqe_api_adp | VQE adapter bundled in vendor libmpi; we have separate libdnvqe |
| hi_upvqe_api_adp | VQE adapter bundled in vendor libmpi; we have separate libupvqe (scaffold) |

## Companion Libraries (complete, building)

| Library | Files | Symbols | Source |
|---------|-------|---------|--------|
| libive.a/so | 4 .c | 144 | Fully RE'd from 24,223 lines vendor ARM assembly |
| libmd.a/so | 3 .c | 22 | Fully RE'd from 5,099 lines vendor ARM assembly |
| libdnvqe.a/so | 5 .c | ~28 | Fully RE'd from 106K lines vendor ARM assembly |
| libhiae.a/so | 9 .c | ~182 | Fully RE'd from 35,493 lines vendor ARM assembly |
| libsecurec.a/so | 39 .c | ~49 | Vendor source drop (Huawei safe C) |
| libhi_cipher.a/so | 4 .c | ~44 | Vendor source drop (crypto) |
| libaacenc.a/so | 1 .c | ~6 | Vendor source drop (AAC encoder wrapper) |

**Total: 8 libraries building clean** with zero errors.

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
| `HI_MPI_VI_FisheyePosQueryDst2Src` | Full — delegates to `hi_mpi_vi_fisheye_pos_query_dst_to_src` (GDC point query) |

### Internal helpers also implemented

- `hi_mpi_vi_set_chn_spread_attr` — RE'd from decompiler output, calls `gdc_spread_configure`
- `mpi_vi_set_gdc_comm_cfg` — RE'd from decompiler output, queries channel dynamic range
- `gdc_spread_configure` — RE'd from `hiisp_gdc_fw_user.S` (`GDC_Spread_CFG`), fixed-point spread math
