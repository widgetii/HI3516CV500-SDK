# blob-test-framework

A QEMU-based trace-comparison testing framework for validating C reimplementations of ARM binary blobs (`.o` kernel module objects).

## What This Does

Given an ARM `.o` blob and your C source replacement, this framework:

1. Builds two static ARM binaries — one linking the original blob, one linking your C code (with the blob as fallback for unreplaced functions)
2. Runs both under `qemu-arm` with a mock kernel environment (OSAL, CMPI, MMZ, VB)
3. Logs every kernel API call to a trace file
4. Normalizes traces (strips hex addresses) and diffs them
5. **"C TRACES MATCH"** = your C code is behaviorally identical to the blob

## Key Concepts

### Weaken-and-Replace Linking

The core technique: `objcopy --weaken-symbol` makes blob functions weak, then your C implementations (strong symbols) take priority at link time. Unreplaced functions fall back to the blob.

**Critical**: Only weaken TEXT (code) symbols, never BSS/DATA — weak BSS causes crashes on real `insmod`.

### Mock Layers

- **`lib/osal_mock.c`** — Maps kernel OSAL functions (osal_vmalloc, osal_semaphore, osal_proc, etc.) to libc equivalents. Allocation calls are traced.
- **`lib/cmpi_mock.c`** — Provides CMPI_RegisterModule, CMPI_GetModuleFuncById, MMZ alloc/free, VB (Video Buffer) mock, SYS module mock. Pre-registers common dependencies (VB=1, SYS=2, VPSS=8, VI=10, RC=19, VEDU=25).

### Trace Comparison

Both binaries run the same test scenarios. The OSAL mock logs every allocation/free/proc call. The harness logs every function return value. After normalizing pointer addresses (`0x[0-9a-f]+` → `ADDR`), the traces must match exactly.

## How To Use

### 1. Create your test directory

```
my_test/
  harness.c      # Your test scenarios (see example/)
  Makefile        # Set variables, include Makefile.inc
```

### 2. Configure your Makefile

```makefile
FRAMEWORK_DIR  = /path/to/blob-test-framework
MODULE_NAME    = my_module
BLOB_OBJ       = /path/to/my_module.o       # Original ARM .o blob
C_SRCS          = src/my_func1.c src/my_func2.c  # Your C replacements
C_INCLUDE       = -I src/
HARNESS_SRCS    = harness.c

REPLACED_FUNCS  = func1 func2 func3   # Functions you've reimplemented in C

# Optional: expose blob-internal anonymous labels
# EXTRA_OBJCOPY_FLAGS = --add-symbol helper=.text:0x90,global,function

include $(FRAMEWORK_DIR)/Makefile.inc
```

### 3. Write your harness

The harness exercises the blob's API: ModInit/ModExit, create/destroy channels, getters/setters, validation functions. See `example/harness.c`.

Key pattern:
```c
#include "cmpi_mock.h"
#include "osal_mock.h"

int main(int argc, char *argv[]) {
    FILE *trace = fopen(argv[1], "w");
    osal_mock_set_trace(trace);
    cmpi_mock_init();

    // Optional: register extra dependency modules
    // cmpi_mock_register(14, "venc", my_venc_func_table);

    // Set blob globals, call ModInit, exercise functions, log to trace
    ...
}
```

### 4. Run

```bash
make compare-c    # Build both, run under qemu-arm, diff traces
```

Output: `C TRACES MATCH` or `C TRACES DIFFER` (with diff output showing divergence).

## Requirements

- `arm-linux-gnueabihf-gcc` — ARM cross-compiler (static linking, gnueabihf ABI)
- `arm-none-eabi-objcopy` — for `.ARM.attributes` removal and symbol weakening
- `qemu-arm` — userspace ARM emulator

## File Layout

```
blob-test-framework/
  lib/
    osal_mock.c   — Kernel OSAL → libc shim (traced)
    osal_mock.h
    cmpi_mock.c   — Module system + MMZ + VB mock
    cmpi_mock.h
  Makefile.inc    — Include in your Makefile for build/compare targets
  example/
    harness.c     — Template test harness
    Makefile       — Template Makefile
```

## Tips for Claude Code sessions consuming this framework

- Start by getting `make test` working (blob-only). This validates your harness + mock setup.
- Add C functions incrementally. After each batch, run `make compare-c`.
- When traces differ, the diff shows exactly which API call diverged — use this to find bugs.
- If the blob uses softfp ABI but has no float ops, gnueabihf toolchain works after stripping `.ARM.attributes`.
- For blob-internal helper calls (anonymous labels), use `EXTRA_OBJCOPY_FLAGS = --add-symbol name=.text:OFFSET,global,function`.
