/**
 * Audio Stream Buffer — empty compilation unit.
 *
 * The vendor SDK's as_buf.o also exports zero symbols.
 * This file exists as a build system placeholder; no audio stream
 * buffer management functions are implemented here. The actual
 * audio stream handling is done inline in mpi_adec.c and mpi_aenc.c
 * via ioctl calls to the kernel driver.
 */
