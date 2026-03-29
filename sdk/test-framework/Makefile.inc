# Makefile.inc — Includeable Makefile fragment for blob trace-comparison testing.
#
# Include this from your test harness Makefile after setting these variables:
#
#   FRAMEWORK_DIR  = path to blob-test-framework/
#   MODULE_NAME    = short name (e.g. h264e, jpege)
#   BLOB_OBJ       = path to original ARM .o blob
#   C_SRCS          = list of C replacement source files
#   C_INCLUDE       = -I flags for C sources (optional)
#   REPLACED_FUNCS  = list of function names replaced by C code
#   HARNESS_SRCS    = your harness.c (and any extra test sources)
#   EXTRA_OBJCOPY_FLAGS = extra objcopy flags (e.g. --add-symbol, optional)
#
# Provided targets:
#   test_$(MODULE_NAME)_orig   — blob-only binary
#   test_$(MODULE_NAME)_c      — C-replaced binary
#   compare-c                  — run both under qemu-arm, diff normalized traces
#   test                       — run blob-only under qemu-arm
#   clean
#
# Requirements:
#   arm-linux-gnueabihf-gcc    — cross-compiler (static linking)
#   arm-none-eabi-objcopy      — for stripping ARM attributes / weakening
#   qemu-arm                   — for running the static ARM binaries

CC ?= arm-linux-gnueabihf-gcc
CFLAGS ?= -static -Wall -O0 -g

LIB_DIR = $(FRAMEWORK_DIR)/lib
LIB_SRCS = $(LIB_DIR)/osal_mock.c $(LIB_DIR)/cmpi_mock.c

WEAKEN_FLAGS = $(foreach f,$(REPLACED_FUNCS),--weaken-symbol=$(f))

ORIG_BIN = test_$(MODULE_NAME)_orig
C_BIN    = test_$(MODULE_NAME)_c

# Strip ARM attributes (and apply any extra objcopy flags like --add-symbol)
%.noattr.o: %.o
	arm-none-eabi-objcopy --remove-section=.ARM.attributes \
		$(EXTRA_OBJCOPY_FLAGS) $< $@

# Blob object with attributes stripped (for the orig binary)
BLOB_NOATTR = $(BLOB_OBJ:.o=.noattr.o)

# Original blob binary
$(ORIG_BIN): $(HARNESS_SRCS) $(LIB_SRCS) $(BLOB_NOATTR)
	$(CC) $(CFLAGS) $(C_INCLUDE) -I $(LIB_DIR) -o $@ $^

# C-replaced binary: compile C sources, weaken blob, link C first
$(C_BIN): $(HARNESS_SRCS) $(LIB_SRCS) $(C_SRCS) $(BLOB_OBJ)
	@# Compile each C replacement source to /tmp
	@for f in $(C_SRCS); do \
		$(CC) $(CFLAGS) $(C_INCLUDE) -I $(LIB_DIR) \
			-c -o /tmp/$$(basename $$f .c).o $$f; \
	done
	@# Weaken replaced symbols in the blob + strip ARM attributes
	arm-none-eabi-objcopy $(WEAKEN_FLAGS) --remove-section=.ARM.attributes \
		$(EXTRA_OBJCOPY_FLAGS) \
		$(BLOB_OBJ) /tmp/$(MODULE_NAME)_weakened.o
	@# Link: harness + mocks + C objects first, then weakened blob as fallback
	$(CC) $(CFLAGS) $(C_INCLUDE) -I $(LIB_DIR) -o $@ \
		$(HARNESS_SRCS) $(LIB_SRCS) \
		$(patsubst %.c,/tmp/%.o,$(notdir $(C_SRCS))) \
		/tmp/$(MODULE_NAME)_weakened.o

.PHONY: test compare-c clean

test: $(ORIG_BIN)
	qemu-arm ./$(ORIG_BIN) trace.orig.log

compare-c: $(ORIG_BIN) $(C_BIN)
	qemu-arm ./$(ORIG_BIN) trace.orig.log
	qemu-arm ./$(C_BIN) trace.c.log
	@# Normalize: replace hex addresses so pointer differences don't cause mismatches
	sed 's/0x[0-9a-f]\+/ADDR/g' trace.orig.log > /tmp/trace_$(MODULE_NAME).orig.norm
	sed 's/0x[0-9a-f]\+/ADDR/g' trace.c.log > /tmp/trace_$(MODULE_NAME).c.norm
	diff /tmp/trace_$(MODULE_NAME).orig.norm /tmp/trace_$(MODULE_NAME).c.norm \
		&& echo "C TRACES MATCH" || echo "C TRACES DIFFER"

clean:
	rm -f $(ORIG_BIN) $(C_BIN) trace.*.log \
		$(BLOB_NOATTR) /tmp/$(MODULE_NAME)_*.o \
		$(patsubst %.c,/tmp/%.o,$(notdir $(C_SRCS)))
