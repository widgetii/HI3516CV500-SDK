/**
 * Audio Frame Buffer — empty compilation unit.
 *
 * The vendor SDK's af_buf.o also exports zero symbols.
 * This file exists as a build system placeholder; no audio frame
 * buffer management functions are implemented here. The actual
 * audio frame handling is done inline in mpi_ai.c, mpi_ao.c,
 * mpi_adec.c, and mpi_aenc.c via ioctl calls to the kernel driver.
 */
