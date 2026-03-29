/*
 * ioctl_mock.c — User-space syscall mock for MPI library testing.
 *
 * Provides fake implementations of open/close/ioctl/mmap/munmap that
 * the HiSilicon MPI libraries use to communicate with kernel drivers.
 * All calls are logged to a trace file.
 *
 * Device FDs start at 1000 to avoid collision with real fds.
 * ioctl GET commands fill the output buffer with zeros.
 * ioctl SET commands log a CRC32 hash of the input data.
 * mmap returns heap-allocated buffers tracked for munmap.
 */

#include "ioctl_mock.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <errno.h>

/* ioctl direction/size decoding (Linux ioctl encoding) */
#define _IOC_NRBITS    8
#define _IOC_TYPEBITS  8
#define _IOC_SIZEBITS  14
#define _IOC_DIRBITS   2

#define _IOC_NRMASK    ((1 << _IOC_NRBITS)   - 1)
#define _IOC_TYPEMASK  ((1 << _IOC_TYPEBITS) - 1)
#define _IOC_SIZEMASK  ((1 << _IOC_SIZEBITS) - 1)
#define _IOC_DIRMASK   ((1 << _IOC_DIRBITS)  - 1)

#define _IOC_NRSHIFT   0
#define _IOC_TYPESHIFT (_IOC_NRSHIFT   + _IOC_NRBITS)
#define _IOC_SIZESHIFT (_IOC_TYPESHIFT + _IOC_TYPEBITS)
#define _IOC_DIRSHIFT  (_IOC_SIZESHIFT + _IOC_SIZEBITS)

#define _IOC_DIR(nr)   (((nr) >> _IOC_DIRSHIFT)  & _IOC_DIRMASK)
#define _IOC_TYPE(nr)  (((nr) >> _IOC_TYPESHIFT)  & _IOC_TYPEMASK)
#define _IOC_NR(nr)    (((nr) >> _IOC_NRSHIFT)    & _IOC_NRMASK)
#define _IOC_SIZE(nr)  (((nr) >> _IOC_SIZESHIFT)  & _IOC_SIZEMASK)

#define _IOC_WRITE 1
#define _IOC_READ  2

/* ---- State ---- */

#define MAX_FDS      256
#define MAX_MMAPS    64
#define FD_BASE      1000

static FILE *g_trace = NULL;
static int g_open_count = 0;
static int g_ioctl_count = 0;

static struct {
    int in_use;
    char path[128];
} g_fds[MAX_FDS];

static struct {
    void *addr;
    size_t length;
} g_mmaps[MAX_MMAPS];

/* ---- Simple CRC32 for data hashing ---- */

static uint32_t crc32_buf(const void *buf, size_t len) {
    const uint8_t *p = (const uint8_t *)buf;
    uint32_t crc = 0xFFFFFFFF;
    size_t i;
    int j;
    for (i = 0; i < len; i++) {
        crc ^= p[i];
        for (j = 0; j < 8; j++)
            crc = (crc >> 1) ^ (0xEDB88320 & -(crc & 1));
    }
    return ~crc;
}

/* ---- Public API ---- */

void ioctl_mock_set_trace(FILE *trace) { g_trace = trace; }

void ioctl_mock_reset(void) {
    memset(g_fds, 0, sizeof(g_fds));
    memset(g_mmaps, 0, sizeof(g_mmaps));
    g_open_count = 0;
    g_ioctl_count = 0;
}

int ioctl_mock_get_open_count(void)  { return g_open_count; }
int ioctl_mock_get_ioctl_count(void) { return g_ioctl_count; }

/* ---- Syscall overrides ---- */

int open(const char *pathname, int flags, ...) {
    int i;

    /* Only intercept /dev/* paths */
    if (strncmp(pathname, "/dev/", 5) != 0) {
        /* Non-device files: use the real open via syscall() */
        extern long syscall(long number, ...);
        int mode = 0;
        if (flags & 0100) { /* O_CREAT */
            va_list ap;
            va_start(ap, flags);
            mode = va_arg(ap, int);
            va_end(ap);
        }
        return (int)syscall(5 /* __NR_open */, pathname, flags, mode);
    }

    for (i = 0; i < MAX_FDS; i++) {
        if (!g_fds[i].in_use) {
            g_fds[i].in_use = 1;
            strncpy(g_fds[i].path, pathname, sizeof(g_fds[i].path) - 1);
            g_open_count++;
            if (g_trace)
                fprintf(g_trace, "open(%s) = %d\n", pathname, FD_BASE + i);
            return FD_BASE + i;
        }
    }
    errno = EMFILE;
    return -1;
}

int close(int fd) {
    if (fd >= FD_BASE && fd < FD_BASE + MAX_FDS) {
        int idx = fd - FD_BASE;
        if (g_fds[idx].in_use) {
            if (g_trace)
                fprintf(g_trace, "close(%s)\n", g_fds[idx].path);
            g_fds[idx].in_use = 0;
            g_fds[idx].path[0] = '\0';
            return 0;
        }
    }
    /* Real close for non-mock fds */
    extern long syscall(long number, ...);
    return (int)syscall(6 /* __NR_close */, fd);
}

int ioctl(int fd, unsigned long request, ...) {
    va_list ap;
    void *arg;
    unsigned int dir, size;
    int idx;
    const char *dev = "unknown";

    va_start(ap, request);
    arg = va_arg(ap, void *);
    va_end(ap);

    if (fd >= FD_BASE && fd < FD_BASE + MAX_FDS) {
        idx = fd - FD_BASE;
        if (g_fds[idx].in_use)
            dev = g_fds[idx].path;
    }

    dir  = _IOC_DIR(request);
    size = _IOC_SIZE(request);

    g_ioctl_count++;

    if (g_trace) {
        fprintf(g_trace, "ioctl(%s, cmd=0x%08lx, sz=%u",
            dev, request, size);

        if (arg && size > 0) {
            /* Always log input data hash for comparability */
            fprintf(g_trace, ", data_crc=0x%08x", crc32_buf(arg, size));
        }
        fprintf(g_trace, ") = 0\n");
    }

    /* Do NOT memset output buffer — the caller already zeroes their struct.
     * Writing _IOC_SIZE bytes can overflow the caller's stack buffer if the
     * ioctl encoding size exceeds the actual struct size (common in HiSilicon). */

    return 0;
}

void *mmap(void *addr, size_t length, int prot, int flags, int fd, long offset) {
    void *buf;
    int i;
    (void)addr; (void)prot; (void)flags; (void)fd; (void)offset;

    buf = calloc(1, length);
    if (!buf) return (void *)-1;

    for (i = 0; i < MAX_MMAPS; i++) {
        if (!g_mmaps[i].addr) {
            g_mmaps[i].addr = buf;
            g_mmaps[i].length = length;
            break;
        }
    }

    if (g_trace)
        fprintf(g_trace, "mmap(len=%zu) = %p\n", length, buf);

    return buf;
}

int munmap(void *addr, size_t length) {
    int i;
    (void)length;

    for (i = 0; i < MAX_MMAPS; i++) {
        if (g_mmaps[i].addr == addr) {
            if (g_trace)
                fprintf(g_trace, "munmap(%p, %zu)\n", addr, g_mmaps[i].length);
            free(g_mmaps[i].addr);
            g_mmaps[i].addr = NULL;
            g_mmaps[i].length = 0;
            return 0;
        }
    }
    return -1;
}

/* Override pthread to avoid futex deadlocks in static QEMU */
#include <pthread.h>
int pthread_mutex_lock(pthread_mutex_t *m)   { (void)m; return 0; }
int pthread_mutex_unlock(pthread_mutex_t *m) { (void)m; return 0; }
int pthread_mutex_init(pthread_mutex_t *m, const pthread_mutexattr_t *a) { (void)m; (void)a; return 0; }
int pthread_mutex_destroy(pthread_mutex_t *m) { (void)m; return 0; }
int pthread_join(pthread_t t, void **v) { (void)t; (void)v; return 0; }
int pthread_create(pthread_t *t, const pthread_attr_t *a, void*(*f)(void*), void *arg)
{ (void)t; (void)a; (void)f; (void)arg; return 0; }

/* Stubs for other syscalls MPI might call */
int fstat(int fd, void *buf) {
    (void)fd;
    memset(buf, 0, 128);
    return 0;
}
