/*
 * ioctl_mock.h — User-space syscall mock for MPI library testing.
 *
 * Intercepts open/close/ioctl/mmap so MPI libraries can run under
 * QEMU without actual kernel drivers. All calls are logged to a
 * trace file for comparison between vendor and C implementations.
 */
#ifndef IOCTL_MOCK_H
#define IOCTL_MOCK_H

#include <stdio.h>

void ioctl_mock_set_trace(FILE *trace);
void ioctl_mock_reset(void);

/* Statistics */
int ioctl_mock_get_open_count(void);
int ioctl_mock_get_ioctl_count(void);

#endif /* IOCTL_MOCK_H */
