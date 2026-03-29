# HI3516CV500-SDK

Contributions and SDK source code for the HiSilicon HI3516CV500 chip.

## SDK Version Notice

This project was originally reverse-engineered from **V2.0.2.0** vendor SDK sources. Our production environment runs **V2.0.2.1** (`Hi3516CV500_MPP_V2.0.2.1 B030 Release`). All C implementations need validation against the V2.0.2.1 binary before deployment.

### V2.0.2.0 vs V2.0.2.1 differences (libmpi)

| Area | V2.0.2.0 (our source) | V2.0.2.1 (production) |
|------|----------------------|----------------------|
| Total exported symbols | 759 | 782 |
| HI_MPI_* API functions | 589 | 597 |
| Version string | `Hi3516CV500_MPP_V2.0.2.0 B030` | `Hi3516CV500_MPP_V2.0.2.1 B030` |

#### New in V2.0.2.1 (not in our build)

- **`mpi_mcf` module** (9 functions) — Multi-Channel Fusion (`hi_mpi_mcf_create_grp`, `set_grp_attr`, `start_grp`, etc.). No header available in V2.0.2.0 source.
- **`af_buf` / `as_buf` implementations** (10 functions) — Audio frame/stream buffer management (`af_buf_init`, `af_buf_get_free`, `af_buf_get_busy`, `as_buf_init`, etc.). These are empty compilation units in V2.0.2.0.
- **VENC adapt layer** (~80 lowercase `hi_mpi_venc_*` wrappers) — V2.0.2.1 added a secondary API layer with lowercase function names wrapping the existing uppercase `HI_MPI_VENC_*` functions.
- **New VENC APIs** — `HI_MPI_VENC_EnableSvc`, `HI_MPI_VENC_SendMultiFrame`, `HI_MPI_VENC_SetChnConfig`, `HI_MPI_VENC_GetSearchWindow` and their lowercase equivalents.
- **GDC function naming** — V2.0.2.0 uses `GDC_Fisheye_CFG` (uppercase), V2.0.2.1 uses `gdc_fisheye_configure` (lowercase). Same applies to `GDC_Trapzoid_CFG`, `GDC_LDC_CFG`, `GDC_FreeAngleRotation_CFG`.

#### Validation required

All reverse-engineered C implementations were derived from V2.0.2.0 disassembly (`.S` files). Before production use on V2.0.2.1 systems:

1. **Compare ioctl command codes** — verify that ioctl numbers in our C match the V2.0.2.1 kernel driver
2. **Verify struct layouts** — field offsets in ISP/VQE/AE context structures may have shifted
3. **Test GDC firmware functions** — fisheye, LDC, trapezoid, and rotation algorithms involve fixed-point math that needs hardware validation
4. **Test AE algorithm** — the 182-function `libhiae` was RE'd entirely from V2.0.2.0 assembly and uses byte-offset access into a 10KB opaque context struct
5. **Validate DNVQE** — audio VQE cache/resampler logic depends on dlopen'd vendor `.so` files (`libhive_HPF.so`, etc.)

The `.S` assembly reference files are retained alongside the C implementations specifically for this validation — they serve as ground truth to diff against when hunting bugs on real hardware.

## Build

```bash
mkdir build && cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../CMake/ArmhfToolchain.cmake ..
make -j$(nproc)
```

### Libraries produced (8)

| Library | Source | Description |
|---------|--------|-------------|
| `libmpi.so/a` | 22 C files, 759 symbols | Media Processing Interface — full video/audio pipeline |
| `libhiae.so/a` | 9 C files, 182 functions | Auto-Exposure algorithm (RE'd from 35K lines ASM) |
| `libive.so/a` | 4 C files, 144 symbols | Image Vector Engine (RE'd from 24K lines ASM) |
| `libmd.so/a` | 3 C files, 22 symbols | Motion Detection (RE'd from 5K lines ASM) |
| `libdnvqe.so/a` | 5 C files | Downlink Voice Quality Enhancement (RE'd from 106K lines ASM) |
| `libsecurec.so/a` | 39 C files | Huawei safe C library (vendor drop) |
| `libhi_cipher.so/a` | 4 C files | Crypto library (vendor drop) |
| `libaacenc.so/a` | 1 C file | AAC encoder wrapper (vendor drop, requires libfdk-aac) |

### Toolchain

- **Hard-float:** `arm-linux-gnueabihf-gcc` / `arm-linux-gnueabihf-g++`
- **Soft-float:** `arm-linux-gnueabi-gcc` / `arm-linux-gnueabi-g++`
