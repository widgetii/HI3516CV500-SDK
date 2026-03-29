# Example Makefile — copy and adapt to your module.
#
# Usage:
#   make test        — run blob-only under qemu-arm
#   make compare-c   — build both, run both, diff traces

FRAMEWORK_DIR = ..

# --- Module-specific configuration ---
MODULE_NAME = my_module
BLOB_OBJ    = path/to/my_module.o
C_SRCS      = path/to/my_module_funcs.c
C_INCLUDE   = -I path/to/my_module/src/
HARNESS_SRCS = harness.c

# List every C function that replaces a blob function
REPLACED_FUNCS = \
	MyModule_ModInit MyModule_ModExit \
	my_module_create_chn my_module_destroy_chn

# Optional: expose anonymous blob labels as callable symbols
# EXTRA_OBJCOPY_FLAGS = --add-symbol my_helper=.text:0x90,global,function

include $(FRAMEWORK_DIR)/Makefile.inc
