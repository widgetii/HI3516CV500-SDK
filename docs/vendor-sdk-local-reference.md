# Vendor SDK Local Reference

Local copy of the HiSilicon Hi3516CV500 vendor SDK release **V2.0.2.1** is available on this machine at `~/Hi3516CV500/`.

## Directory Layout

```
~/Hi3516CV500/
├── toolchain/
│   └── arm-himix200-linux.tgz
├── Hi3516CV500R001C02SPC021/
│   ├── 00.hardware/
│   │   ├── board/
│   │   └── chip/
│   ├── 01.software/
│   │   ├── board/
│   │   └── pc/
│   └── 02.only for reference/
│       ├── hardware/
│       ├── software/
│       ├── uboot/
│       └── SVB/
└── ReleaseDoc/
    ├── en/
    └── zh/
```

## Toolchain

`~/Hi3516CV500/toolchain/arm-himix200-linux.tgz`

HiSilicon's official ARM cross-compiler for this chip family. This is what the vendor build system expects (as opposed to generic `arm-linux-gnueabihf`).

## SDK Release (V2.0.2.1)

`~/Hi3516CV500/Hi3516CV500R001C02SPC021/`

### Pre-built Flash Images

`~/Hi3516CV500/Hi3516CV500R001C02SPC021/hi3516cv500_spi_smp_image_glibc/`

- `u-boot-hi3516cv500.bin` — U-Boot binary
- `uImage_hi3516cv500_smp` — Linux kernel image
- `rootfs_hi3516cv500_*.{jffs2,ubifs,yaffs2}` — Root filesystems for various flash geometries (64k/128k/256k erase, JFFS2/UBIFS/YAFFS2)

Equivalent image sets also exist for `hi3516av300` and `hi3516dv300`.

### Source Packages

`~/Hi3516CV500/Hi3516CV500R001C02SPC021/01.software/board/`

| Package | Size | Contents |
|---------|------|----------|
| `Hi3516CV500_SDK_V2.0.2.1.tgz` | 1.1 GB | Full SDK (drivers, MPP, OSAL, OS drivers) |
| `Hi3516CV500_Middleware_V2.0.2.1.tgz` | 14 MB | Middleware components |
| `Hi3516CV500_Histreaming_V2.0.2.1.tgz` | 7 MB | HiStreaming media streaming |

The main SDK tarball unpacks via `sdk.unpack` into:

- **`package/drv.tgz`** (284 files) — external/internal hardware drivers (SPI displays, audio codecs, sensors, MIPI, watchdog, IR, cipher, ADC)
- **`package/osal.tgz`** (74 files) — OS abstraction layer (kernel MMZ, himedia, osal wrappers)
- **`package/mpp_smp_linux.tgz`** (1227 files) — Media Processing Platform: the main package
- **`package/osdrv.tgz`** (165 files) — Linux 4.9.37 kernel patch, U-Boot 2016.11, BusyBox 1.26.2, rootfs scripts

#### MPP Package Contents

- **`ko/`** — Pre-built kernel modules for all subsystems (hi3516cv500_base.ko, hi3516cv500_isp.ko, hi3516cv500_venc.ko, etc.)
- **`lib/`** — Pre-built vendor libraries (.a + .so): libmpi, libisp, libnnie, libive, all sensor libs, audio libs, securec, cipher, TDE, etc. These are the closed-source binaries this repository aims to replace.
- **`include/`** — 112 header files defining the public API
- **`sample/`** — Sample applications: audio, vio, venc, vdec, vo, hifb, tde, region, svp, dis, fisheye, snap, scene_auto, uvc_app, awb_online_calibration, lsc_online_cali, calcflicker, bitrate_auto, traffic_capture
- **`init/`** — Module init source files (acodec_init.c, base_init.c, isp_init.c, etc.)
- **`component/`** — Buildable component source (hifb, etc.)
- **`obj/`** — Object files
- **`tools/`** — Utilities

### PC Tools

`~/Hi3516CV500/Hi3516CV500R001C02SPC021/01.software/pc/`

- **DEC_LIB** — H.265 PC decoding library
- **HiPro-usb** — Mass production USB burning tool
- **Middleware** — PC-side middleware
- **PQTOOLS** — Picture quality tuning tools
- **usb_tools** — USB utilities

### Hardware Reference

`~/Hi3516CV500/Hi3516CV500R001C02SPC021/00.hardware/`

- **`board/`** — PCB design files for demo boards: HI3516CV500DMEB, HI3516CV500DDR4DMEB, HI3516AV300DMEB, HI3516DV300DMEB (+ DDR4, LITE, PRO variants)
- **`chip/`** — BSDL/IBIS boundary scan files for Hi3516CV500, Hi3516AV300, Hi3516DV300

### DDR Timing Spreadsheets

`~/Hi3516CV500/Hi3516CV500R001C02SPC021/02.only for reference/uboot/`

U-Boot DDR initialization configuration spreadsheets (.xlsm) for various board and memory combinations:

- Hi3516CV500: DDR3 2133MHz 256MB 16bit, DDR4 1800/2133MHz 1GB 16bit
- Hi3516AV300: DDR3 1800/2133MHz, DDR4 2133MHz, various board layouts (2L/4L/6L)
- Hi3516DV300: DDR3 2133MHz, DDR4 1800/2133MHz, various board layouts

## Documentation

`~/Hi3516CV500/ReleaseDoc/en/`

105 PDF documents in English (Chinese translations in `zh/`). Organized by topic:

### Chip & Board Hardware

- Hi3516C V500 Data Sheet (full + brief)
- Hi3516C V500 Demo Board User Guide
- Hi3516C V500 Hardware Design User Guide
- Equivalent docs for Hi3516AV300 and Hi3516DV300

### MPP (Media Processing Platform)

- **HiMPP Media Processing Software V4.0 Development Reference** — the primary API reference
- HiMPP Media Processing Software V4.0 FAQs
- Audio Components API Reference
- Bit Rate Control Application Notes
- Cipher API Reference
- Graphics Development User Guide
- HDMI Development Reference
- HiFB API Reference / Development Guide
- MIPI User Guide
- RTC Application Guide
- Smart Coding User Guide
- Snapshot User Guide
- Startup Screen User Guide
- TDE API Reference
- MPI Differences docs (CV500 vs AV300 vs DV300 vs Hi3519AV100)

### ISP (Image Signal Processor)

- HiISP Development Reference
- HiISP FAQs
- ISP Algorithms Differences (DV300/CV500/AV300 vs Hi3519AV100)
- ISP MPI Differences (DV300/CV500/AV300 vs Hi3519AV100)
- Sensor support list (Hi3516CV500).xlsx (`~/Hi3516CV500/ReleaseDoc/en/01.software/board/ISP/`)

### SVP (Smart Vision Platform)

- HiSVP Development Guide
- HiSVP API Reference
- HiIVE API Reference
- HiIVS API Reference

### OS Drivers

- Hi35xx Development Environment User Guide
- Hi35xx U-boot Porting Development Guide
- Bare and Non-Bare Chip Burning Upgrade Operation Guide
- UBIFS User Guide
- Peripheral Driver Operation Guide (`~/Hi3516CV500/ReleaseDoc/en/01.software/board/OSDRV/`)

### HiGV (GUI Framework)

- HiGV API Reference
- HiGV Development Guide
- HiGV FAQs
- HiGV Labels User Guide

### Other

- Hi35xx Secure Boot User Guide
- HiSysLink API Development Reference
- SDK/chip Differences docs (between CV500, AV300, DV300, Hi3519AV100)

### Tuning & Debug Guides (Reference)

`~/Hi3516CV500/ReleaseDoc/en/02.only for reference/software/`

- Audio Tuning Guide
- DIS Tuning Guide
- GDC Debugging Guide
- HiISP Tuning Guide
- HiISP Color Optimization Description
- 3DNR Parameter Configuration Description
- Scene-auto Usage Guide
- Sensor Debugging Guide
- HiSilicon IP Camera Image Quality Test Standards
- Panel Interconnection User Guide
- USB Pipe Application Notes
- Wi-Fi User Guide
- Guide for Porting the Flash Based on HiFMC V100

### DDR & Hardware Reference Docs

`~/Hi3516CV500/ReleaseDoc/en/02.only for reference/hardware/`

- DDR3/DDR4 Configuration Guides (per chip variant)
- DDR DQ Window Check Method and Result Analysis
- High-Speed Signal Test Guide
- IPC Correction-Free Auto IRIS Application Notes
- SVB Voltages and Registers Mapping

### Test Reports

`~/Hi3516CV500/ReleaseDoc/en/02.only for reference/test report/`

- Power Consumption Test Reports (per chip)
- DDR Signal Integrity Reports
- HDMI Test Reports (480P/720P/1080P)
- USB 2.0 Test Reports
- 100M RMII Ethernet Reports

### PC Software Docs

- H.265 PC Decoding Library API Reference
- HiBurn / HiTool User Guides
- Mass Production Burning User Guide
- PQ Tools User Guide
- HiSVP GFPQ Library User Guide
- HiGVBuilder User Guide
