#!/bin/bash
#
# gen_per_func.sh — Generate per-module weaken-and-replace tests for libmpi
#
# Usage: ./gen_per_func.sh <vendor_libmpi.a> <our_source_dir> <output_dir>
#
# For each .o in the vendor .a, generates:
#   <output_dir>/<module>/Makefile
#   <output_dir>/<module>/harness.c
#
set -e

VENDOR_A="${1:?Usage: $0 <vendor_libmpi.a> <our_source_dir> <output_dir>}"
OUR_SRC="${2:?}"
OUT_DIR="${3:?}"
FRAMEWORK_DIR="$(cd "$(dirname "$0")/.." && pwd)"

# Map vendor .o names to our .c file names and module prefixes
declare -A OBJ_TO_SRC=(
    [mpi_sys]="mpi_sys.c"
    [mpi_vb]="mpi_vb.c"
    [mpi_vi]="mpi_vi.c"
    [mpi_vo]="mpi_vo.c"
    [mpi_vpss]="mpi_vpss.c"
    [mpi_venc]="mpi_venc.c"
    [mpi_vdec]="mpi_vdec.c"
    [mpi_region]="mpi_region.c"
    [mpi_gdc]="mpi_gdc.c"
    [mpi_vgs]="mpi_vgs.c"
    [mpi_mcf]="mpi_mcf.c"
    [hiisp_gdc_fw_user]="hiisp_gdc_fw_user.c"
    [hiisp_gdc_fw_pointquery]="hiisp_gdc_fw_pointquery.c"
)

# Module prefix for HI_MPI_ functions
declare -A OBJ_TO_PREFIX=(
    [mpi_sys]="HI_MPI_SYS_|HI_MPI_LOG_"
    [mpi_vb]="HI_MPI_VB_"
    [mpi_vi]="HI_MPI_VI_|HI_MPI_SNAP_"
    [mpi_vo]="HI_MPI_VO_"
    [mpi_vpss]="HI_MPI_VPSS_"
    [mpi_venc]="HI_MPI_VENC_"
    [mpi_vdec]="HI_MPI_VDEC_"
    [mpi_region]="HI_MPI_RGN_"
    [mpi_gdc]="HI_MPI_GDC_"
    [mpi_vgs]="HI_MPI_VGS_"
    [mpi_mcf]="hi_mpi_mcf_"
    [hiisp_gdc_fw_user]="GDC_|gdc_"
    [hiisp_gdc_fw_pointquery]="gdc_point_query|gdc_fisheye_point"
)

# Extract all .o from vendor .a into temp dir
EXTRACT_DIR=$(mktemp -d)
trap "rm -rf $EXTRACT_DIR" EXIT
(cd "$EXTRACT_DIR" && arm-linux-gnueabihf-ar x "$VENDOR_A")

echo "Extracted $(ls "$EXTRACT_DIR"/*.o | wc -l) objects from $VENDOR_A"

for mod in "${!OBJ_TO_SRC[@]}"; do
    src_file="${OBJ_TO_SRC[$mod]}"
    prefix="${OBJ_TO_PREFIX[$mod]}"

    # Find the vendor .o (may be named differently, e.g. mpi_ai_adapt.o)
    vendor_o=""
    for candidate in "$EXTRACT_DIR/${mod}.o" "$EXTRACT_DIR/${mod}_adapt.o"; do
        [ -f "$candidate" ] && vendor_o="$candidate" && break
    done
    [ -z "$vendor_o" ] && echo "SKIP: $mod (no vendor .o found)" && continue

    # Check our source exists
    our_src="$OUR_SRC/$src_file"
    [ -f "$our_src" ] || { echo "SKIP: $mod (no source $our_src)"; continue; }

    # Get all TEXT symbols from the vendor .o that match our prefix
    funcs=$(arm-linux-gnueabihf-nm --defined-only "$vendor_o" | grep ' T ' | awk '{print $3}' | grep -E "$prefix" | sort)
    func_count=$(echo "$funcs" | wc -l)

    [ "$func_count" -lt 1 ] && echo "SKIP: $mod (no matching functions)" && continue

    # Create output directory
    mod_dir="$OUT_DIR/$mod"
    mkdir -p "$mod_dir"

    # Copy vendor .o
    cp "$vendor_o" "$mod_dir/vendor.o"

    # Generate REPLACED_FUNCS list
    replaced=$(echo "$funcs" | tr '\n' ' ')

    # Generate Makefile
    cat > "$mod_dir/Makefile" << MKEOF
# Auto-generated per-function test for $mod ($func_count functions)
FRAMEWORK_DIR = $FRAMEWORK_DIR
MODULE_NAME = $mod
BLOB_OBJ = vendor.o
C_SRCS = $our_src
C_INCLUDE = -I $OUR_SRC -I $(dirname "$OUR_SRC")/../common -I $(dirname "$OUR_SRC")/../common/adapt -I $(dirname "$OUR_SRC")/../drivers/hi3516cv500_isp -I $(dirname "$OUR_SRC")/../drivers/hi3516cv500_isp/adapt -I $(dirname "$OUR_SRC")/../libraries/securec
HARNESS_SRCS = harness.c \$(FRAMEWORK_DIR)/lib/ioctl_mock.c \$(FRAMEWORK_DIR)/lib/voice_stub.c
REPLACED_FUNCS = $replaced
EXTRA_LIBS = -lm -Wl,--whole-archive -lpthread -Wl,--no-whole-archive -ldl
CFLAGS = -static -Wall -O0 -g -Wl,--no-warn-mismatch -Wno-incompatible-pointer-types

include \$(FRAMEWORK_DIR)/Makefile.inc

# Override the link step to add extra libs
\$(ORIG_BIN): \$(HARNESS_SRCS) \$(BLOB_NOATTR)
	\$(CC) \$(CFLAGS) \$(C_INCLUDE) -I \$(LIB_DIR) -o \$@ \$^ \$(EXTRA_LIBS)

\$(C_BIN): \$(HARNESS_SRCS) \$(C_SRCS) \$(BLOB_OBJ)
	@for f in \$(C_SRCS); do \\
		\$(CC) \$(CFLAGS) \$(C_INCLUDE) -I \$(LIB_DIR) \\
			-c -o /tmp/\$\$(basename \$\$f .c).o \$\$f; \\
	done
	arm-none-eabi-objcopy \$(WEAKEN_FLAGS) --remove-section=.ARM.attributes \\
		\$(EXTRA_OBJCOPY_FLAGS) \$(BLOB_OBJ) /tmp/\$(MODULE_NAME)_weakened.o
	\$(CC) \$(CFLAGS) \$(C_INCLUDE) -I \$(LIB_DIR) -o \$@ \\
		\$(HARNESS_SRCS) \\
		\$(patsubst %.c,/tmp/%.o,\$(notdir \$(C_SRCS))) \\
		/tmp/\$(MODULE_NAME)_weakened.o \$(EXTRA_LIBS)
MKEOF

    # Generate harness.c
    cat > "$mod_dir/harness.c" << 'HDREOF'
/*
 * Auto-generated per-function test harness
 * Tests each HI_MPI_* function with default (zeroed) parameters
 */
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>
#include "ioctl_mock.h"

/* Crash recovery */
static sigjmp_buf g_jmp;
static const char *g_func_name;
static FILE *g_trace;

static void crash_handler(int sig) {
    (void)sig;
    if (g_trace)
        fprintf(g_trace, "%s = CRASH\n", g_func_name);
    siglongjmp(g_jmp, 1);
}

#define SAFE_CALL(trace, name, call) do { \
    g_func_name = name; g_trace = trace; \
    fflush(trace); \
    if (sigsetjmp(g_jmp, 1) == 0) { \
        int _r = (call); \
        fprintf(trace, "%s = 0x%x\n", name, _r); \
    } \
    fflush(trace); \
} while(0)

HDREOF

    # Add includes based on module
    case "$mod" in
        mpi_sys)  echo '#include "mpi_sys.h"' >> "$mod_dir/harness.c"
                  echo '#include "hi_comm_sys.h"' >> "$mod_dir/harness.c" ;;
        mpi_vb)   echo '#include "mpi_vb.h"' >> "$mod_dir/harness.c"
                  echo '#include "hi_comm_vb.h"' >> "$mod_dir/harness.c" ;;
        mpi_vi)   echo '#include "mpi_vi.h"' >> "$mod_dir/harness.c"
                  echo '#include "hi_comm_vi.h"' >> "$mod_dir/harness.c" ;;
        mpi_vo)   echo '#include "mpi_vo.h"' >> "$mod_dir/harness.c"
                  echo '#include "hi_comm_vo.h"' >> "$mod_dir/harness.c" ;;
        mpi_vpss) echo '#include "mpi_vpss.h"' >> "$mod_dir/harness.c"
                  echo '#include "hi_comm_vpss.h"' >> "$mod_dir/harness.c" ;;
        mpi_venc) echo '#include "mpi_venc.h"' >> "$mod_dir/harness.c"
                  echo '#include "hi_comm_venc.h"' >> "$mod_dir/harness.c" ;;
        mpi_vdec) echo '#include "mpi_vdec.h"' >> "$mod_dir/harness.c"
                  echo '#include "hi_comm_vdec.h"' >> "$mod_dir/harness.c" ;;
        mpi_region) echo '#include "mpi_region.h"' >> "$mod_dir/harness.c"
                    echo '#include "hi_comm_region.h"' >> "$mod_dir/harness.c" ;;
        mpi_gdc)  echo '#include "mpi_gdc.h"' >> "$mod_dir/harness.c"
                  echo '#include "hi_comm_gdc.h"' >> "$mod_dir/harness.c" ;;
        mpi_vgs)  echo '#include "mpi_vgs.h"' >> "$mod_dir/harness.c"
                  echo '#include "hi_comm_vgs.h"' >> "$mod_dir/harness.c" ;;
        *)        echo '#include "hi_type.h"' >> "$mod_dir/harness.c" ;;
    esac

    # Generate test function body
    cat >> "$mod_dir/harness.c" << MAINEOF

int main(int argc, char *argv[]) {
    const char *path = "trace.log";
    if (argc > 1) path = argv[1];
    FILE *trace = fopen(path, "w");
    if (!trace) { perror("fopen"); return 1; }
    ioctl_mock_set_trace(trace);
    ioctl_mock_reset();

    struct sigaction sa = { .sa_handler = crash_handler };
    sigemptyset(&sa.sa_mask);
    sigaction(SIGSEGV, &sa, NULL);
    sigaction(SIGBUS, &sa, NULL);

    fprintf(trace, "=== $mod per-func test ($func_count functions) ===\n");

MAINEOF

    # Generate a SAFE_CALL for each function
    for func in $funcs; do
        # Determine parameter pattern from function name
        # Most HI_MPI functions take (dev/pipe/chn, struct*) or just (struct*)
        case "$func" in
            *Init|*Exit|*CloseFd)
                echo "    SAFE_CALL(trace, \"$func\", $func());" >> "$mod_dir/harness.c"
                ;;
            *Set*|*Create*|*Enable*|*Start*|*Send*|*Bind*|*Attach*|*Request*)
                echo "    { char _buf[4096] = {0}; SAFE_CALL(trace, \"$func\", $func(0, (void*)_buf)); }" >> "$mod_dir/harness.c"
                ;;
            *Get*|*Query*|*Disable*|*Stop*|*Destroy*|*Release*|*Detach*|*UnBind*)
                echo "    { char _buf[4096] = {0}; SAFE_CALL(trace, \"$func\", $func(0, (void*)_buf)); }" >> "$mod_dir/harness.c"
                ;;
            *)
                echo "    { char _buf[4096] = {0}; SAFE_CALL(trace, \"$func\", $func(0, (void*)_buf)); }" >> "$mod_dir/harness.c"
                ;;
        esac
    done

    cat >> "$mod_dir/harness.c" << 'ENDEOF'

    fprintf(trace, "=== done ===\n");
    fclose(trace);
    return 0;
}
ENDEOF

    echo "OK: $mod — $func_count functions, harness + Makefile generated"
done

echo ""
echo "Done. Run tests with:"
echo "  for d in $OUT_DIR/*/; do (cd \"\$d\" && make compare-c); done"
