# Makefile for libmpi trace comparison tests
#
# Usage:
#   make compare-sys    — compare SYS/VB traces (vendor vs ours)
#   make test-vendor    — run vendor-only under qemu-arm
#   make test-ours      — run our build under qemu-arm

CC = arm-linux-gnueabihf-gcc
# Vendor libs are softfp; we force link with --no-warn-mismatch
# (safe: MPI functions don't pass floats in registers)
CFLAGS = -static -Wall -O0 -g -Wl,--no-warn-mismatch

FRAMEWORK_DIR = ../..
LIB_DIR = $(FRAMEWORK_DIR)/lib
SDK_ROOT = ../../../..
SDK_COMMON = $(SDK_ROOT)/sdk/common
SDK_ADAPT = $(SDK_ROOT)/sdk/common/adapt
SDK_MPI_HEADERS = $(SDK_ROOT)/sdk/libraries/mpi

INCLUDES = -I$(LIB_DIR) -I$(SDK_COMMON) -I$(SDK_ADAPT) -I$(SDK_MPI_HEADERS)

VENDOR_SDK = /tmp/Hi3516CV500_SDK_V2.0.2.1/smp/a7_linux/mpp/lib
# Use vendor libmpi + our securec (vendor's is softfp, ours is hardfp)
VENDOR_LIB = $(VENDOR_SDK)/libmpi.a $(VENDOR_SDK)/libdnvqe.a $(BUILD_LIB)/libsecurec.a
BUILD_LIB = $(SDK_ROOT)/build/sdk/libraries
OUR_LIB = $(BUILD_LIB)/libmpi.a $(BUILD_LIB)/libdnvqe.a $(BUILD_LIB)/libsecurec.a

MOCK_SRCS = $(LIB_DIR)/ioctl_mock.c $(LIB_DIR)/voice_stub.c

# ---- SYS/VB test ----

test_sys_vendor: harness_sys.c $(MOCK_SRCS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(VENDOR_LIB) -lm -lpthread -ldl

test_sys_ours: harness_sys.c $(MOCK_SRCS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(OUR_LIB) -lm -Wl,--whole-archive -lpthread -Wl,--no-whole-archive -ldl

compare-sys: test_sys_vendor test_sys_ours
	qemu-arm ./test_sys_vendor trace.sys.vendor.log
	qemu-arm ./test_sys_ours trace.sys.ours.log
	@sed 's/0x[0-9a-f]\+/ADDR/g' trace.sys.vendor.log > /tmp/trace_sys_vendor.norm
	@sed 's/0x[0-9a-f]\+/ADDR/g' trace.sys.ours.log > /tmp/trace_sys_ours.norm
	@diff /tmp/trace_sys_vendor.norm /tmp/trace_sys_ours.norm \
		&& echo "SYS TRACES MATCH" || (echo "SYS TRACES DIFFER"; diff /tmp/trace_sys_vendor.norm /tmp/trace_sys_ours.norm | head -30)

test-vendor: test_sys_vendor
	qemu-arm ./test_sys_vendor trace.sys.vendor.log
	@cat trace.sys.vendor.log

test-ours: test_sys_ours
	qemu-arm ./test_sys_ours trace.sys.ours.log
	@cat trace.sys.ours.log

# ---- ALL modules test ----

test_all_vendor: harness_all.c $(MOCK_SRCS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(VENDOR_LIB) -lm -lpthread -ldl

test_all_ours: harness_all.c $(MOCK_SRCS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(OUR_LIB) -lm -Wl,--whole-archive -lpthread -Wl,--no-whole-archive -ldl

compare-all: test_all_vendor test_all_ours
	qemu-arm ./test_all_vendor trace.all.vendor.log
	qemu-arm ./test_all_ours trace.all.ours.log
	@sed 's/0x[0-9a-f]\+/ADDR/g' trace.all.vendor.log > /tmp/trace_all_vendor.norm
	@sed 's/0x[0-9a-f]\+/ADDR/g' trace.all.ours.log > /tmp/trace_all_ours.norm
	@diff /tmp/trace_all_vendor.norm /tmp/trace_all_ours.norm \
		&& echo "ALL TRACES MATCH" || (echo "ALL TRACES DIFFER"; diff /tmp/trace_all_vendor.norm /tmp/trace_all_ours.norm | head -60)

test-all-vendor: test_all_vendor
	qemu-arm ./test_all_vendor trace.all.vendor.log
	@cat trace.all.vendor.log

test-all-ours: test_all_ours
	qemu-arm ./test_all_ours trace.all.ours.log
	@cat trace.all.ours.log

.PHONY: compare-sys test-vendor test-ours compare-all test-all-vendor test-all-ours clean

clean:
	rm -f test_sys_vendor test_sys_ours test_all_vendor test_all_ours trace.*.log
