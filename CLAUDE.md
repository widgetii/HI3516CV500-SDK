# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Embedded systems SDK for the HiSilicon HI3516CV500 video processing SoC. Contains reverse-engineered and contributed kernel drivers, user-space libraries, camera sensor drivers, and tooling for the chip's media processing pipeline (video input/output, ISP, encoders, audio, neural network inference).

The repository is the work of a single developer (`TekuConcept`), with a short active history from April–May 2021. Much of the code was reverse-engineered earlier (Sep–Oct 2020) but bulk-imported on April 11, 2021. The practical goal was building a usable open `libmpi` subset, not completing the entire SDK.

## Build System

CMake-based with cross-compilation for ARM Linux. Two build targets controlled by options:

- `BUILD_KERNEL_MODULES` (OFF by default) — builds kernel modules (.ko) from `sdk/drivers/`
- `BUILD_USER_SPACE_LIBRARIES` (ON by default) — builds shared/static libs from `sdk/libraries/`

### Build Commands

```bash
# Out-of-source build (hard-float ARM toolchain)
mkdir build && cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../CMake/ArmhfToolchain.cmake ..
make

# With kernel modules enabled
cmake -DCMAKE_TOOLCHAIN_FILE=../CMake/ArmhfToolchain.cmake -DBUILD_KERNEL_MODULES=ON ..

# Soft-float variant
cmake -DCMAKE_TOOLCHAIN_FILE=../CMake/ArmelToolchain.cmake ..
```

### Toolchain Requirements

- **Hard-float:** `arm-linux-gnueabihf-gcc` / `arm-linux-gnueabihf-g++`
- **Soft-float:** `arm-linux-gnueabi-gcc` / `arm-linux-gnueabi-g++`
- Kernel modules require a kernel build directory at `/lib/modules/$(uname -r)/build`

### CMake Infrastructure

- `CMake/KernelBuildTools.cmake` — kernel module build macros: `ADD_KMODULE()`, `TARGET_KINCLUDE_DIRECTORIES()`, `TARGET_KMODULE_CFLAGS()`, `TARGET_KMODULE_AFLAGS()`, `TARGET_KMODULE_LDFLAGS()`, `TARGET_KMODULE_SYMVERS()`, `WRITE_KBUILD()`
- `CMake/ArmhfToolchain.cmake` / `CMake/ArmelToolchain.cmake` — cross-compilation toolchain files

## Architecture

### Layered Design

1. **Kernel drivers** (`sdk/drivers/`) — 33 kernel modules for hardware interfaces
2. **OS Abstraction** (`hi_osal`) — base layer all drivers depend on
3. **User-space libraries** (`sdk/libraries/`) — 37 libs providing MPI (Media Processing Interface) and algorithms
4. **Common headers** (`sdk/common/`) — shared API definitions (`hi_comm_*.h`, `mpi_*.h`, `hi_*.h`)

### Driver Subsystems (sdk/drivers/)

Naming convention: `hi3516cv500_<subsystem>` for SoC-specific, `hi_<name>` for generic.

- **Base/infra:** `hi_osal`, `hi3516cv500_base`, `sys_config`
- **Video pipeline:** `vi` (input) → `isp` (processing) → `vpss` (scaling) → `venc`/`h264e`/`h265e`/`jpege` (encoding) → `vo` (output)
- **Graphics/overlay:** `vgs`, `gdc`, `tde`, `rgn`, `hifb`
- **Audio pipeline:** `aio` → `ai`/`ao` → `aenc`/`adec`, `acodec`
- **AI/ML:** `nnie` (Neural Network Inference Engine), `ive` (Image Vector Engine)
- **Hardware interfaces:** `hi_mipi_rx`, `hi_mipi_tx`, `hi_piris`, `hi_pwm`, `hi_sensor_i2c`, `hi_sensor_spi`

### Library Subsystems (sdk/libraries/)

- **Core:** `mpi` (main MPI library, C), `ive` (Image Vector Engine, C), `securec` (safe C functions)
- **ISP algorithms:** `isp`, `hiae`, `hiawb`, `hildci`, `hidehaze`, `hiawb_natura`
- **Camera sensors:** `sns_imx307`, `sns_imx327`, `sns_imx335`, `sns_gc2053`, `sns_os05a`, etc.
- **Audio:** `aacdec`, `aacenc`, `dnvqe`, `upvqe`, `VoiceEngine`
- **AI:** `ive` (Image Vector Engine), `nnie`, `svpruntime`
- **Other:** `hi_cipher`, `md` (motion detection), `hifisheyecalibrate`, `hdmi`

### Other Components

- `sdk/bootrom/` — reverse-engineered bootrom analysis (C++ documentation, function mapping, SRAM layout)
- `sdk/fastburn/` — C++11 USB fast-boot/flash utility (links pthread)

## Module Completion Status

Understanding what is actually finished vs. scaffolded is critical for working in this codebase.

### Completed (8 libraries building)

Eight fully buildable library artifacts are enabled in `sdk/libraries/CMakeLists.txt`:

**`libmpi.a` / `libmpi.so`** (755 symbols, 589 HI_MPI_* API functions):
- 22 C source files covering all core media pipeline modules
- `mpi/mpi_vi.c` (104 API), `mpi/mpi_vo.c` (91), `mpi/mpi_venc.c` (90), `mpi/mpi_vpss.c` (69), `mpi/mpi_sys.c` (40), `mpi/mpi_ai.c` (37), `mpi/mpi_vdec.c` (32), `mpi/mpi_ao.c` (30), `mpi/mpi_vb.c` (23), `mpi/mpi_region.c` (14), `mpi/mpi_vgs.c` (12), `mpi/mpi_aenc.c` (12), `mpi/mpi_adec.c` (12), `mpi/mpi_snap.c` (10), `mpi/mpi_gdc.c` (8), `mpi/mpi_log.c` (5), `mpi/mpi_audio.c` (3)
- Support: `audio_voice_adp.c`, `audio_comm.c`, `hiisp_gdc_fw_pointquery.c`, `hiisp_gdc_fw_user.c`, `mpi_bind.c`
- Assembly `.S` files retained as reference for hardware verification

**`libive.a` / `libive.so`** (144 symbols, 61 public API):
- 4 C files, fully RE'd from 24,223 lines of vendor ARM assembly

**`libmd.a` / `libmd.so`** (22 exported symbols, 8 public API):
- 3 C files, fully RE'd from 5,099 lines of vendor ARM assembly

**`libhiae.a` / `libhiae.so`** (182 functions, 9 source files):
- `hiae/hi_ae_adp.c` (55 functions), `hiae/hi_auto_exposure.c` (34), `hiae/mpi_isp_ae.c` (19), `hiae/hi_ae_increment.c` (15), `hiae/hi_ae_route.c` (23), `hiae/hi_ae_route_ex.c` (16), `hiae/hi_auto_iris.c` (11), `hiae/hi_iris_pwm.c` (4), `hiae/hi_piris_gpio.c` (6)
- Fully RE'd from 35,493 lines of vendor ARM assembly

**`libdnvqe.a` / `libdnvqe.so`** (5 source files):
- `dnvqe/dnvqe_work.c`, `dnvqe/hi_dnvqe_wrap.c`, `dnvqe/hi_audio_dl_work.c`, `dnvqe/hi_audio_module_wrap.c`, `dnvqe/hi_resample_work.c`
- Fully RE'd from ~106K lines of vendor ARM assembly

**`libsecurec.a` / `libsecurec.so`** — 39 C files, Huawei safe C library (vendor drop)

**`libhi_cipher.a` / `libhi_cipher.so`** — 4 C files, crypto library (vendor drop)

**`libaacenc.a` / `libaacenc.so`** — 1 C file, AAC encoder wrapper (vendor drop, requires libfdk-aac)

### Imported but not actively developed here

- Drivers: `hi_osal`, `hi_mipi_rx`, `hi_mipi_tx`, `hifb`, `hi3516cv500_isp`, `sys_config`, `hi_piris`, `hi_pwm`, `hi_sensor_i2c`, `hi_sensor_spi`
- Libraries: `securec`, `hi_cipher`, `aacenc`
- `aacdec` — vendor source drop but disabled in build due to conflicting type definitions between `fdk-aac-mod.h` and `FDK_audio.h`

### In-progress / half-done

- **`sns_gc2053`** — active commits but build still uses assembly (`gc2053_cmos.S`); C rewrites commented out; `cmos_get_inttime_max()` is empty
- **`bootrom-re`** — serious reverse-engineering effort in the final commits; `bootloader.c` is large but several major routines still marked TODO (`sub_1DC`, `sub_1150`, `secure_fast_boot`)

### Scaffold-only (zero-byte stubs + assembly)

Most media drivers and many libraries are reverse-engineering skeletons: zero-byte `.c` placeholder files alongside `.S` assembly. These are not buildable C implementations.

Representative drivers:
- `hi3516cv500_vi`: 33 `.c` files, 32 zero-byte
- `hi3516cv500_vpss`: 13 `.c` files, 12 zero-byte
- `hi3516cv500_gdc`: 11 `.c` files, 10 zero-byte
- `hi3516cv500_h264e`: 10 `.c` files, 9 zero-byte

Representative libraries:
- `isp`: 54 zero-byte `.c` files + 54 `.S`
- `svpruntime`: 30 zero-byte `.c` files + 30 `.S`
- `hiawb`: 5 zero-byte `.c` files + 5 `.S`
- `VoiceEngine`: 6 zero-byte `.c` files + 6 `.S`
- `nnie`: 3 zero-byte `.c` files + 3 `.S`

The pattern across the codebase: `.S` files hold the real (disassembled vendor) logic; `.c` files are empty placeholders awaiting future reverse-engineering into readable C.
