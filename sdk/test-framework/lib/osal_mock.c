/*
 * Mock OSAL (OS Abstraction Layer) for userspace testing of ARM kernel blobs.
 * Maps kernel OSAL functions to libc equivalents. Every allocation/proc call
 * is logged to a trace file for comparison between blob and C replacement.
 *
 * This is the unified superset — covers all OSAL functions used by any
 * HiSilicon/Goke kernel module blob.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/time.h>

/* Opaque types — blobs only use pointers to these */
typedef struct { int val; } osal_semaphore_t;
typedef struct { int val; } osal_spinlock_t;
typedef struct { char name[64]; void *read; void *write; } osal_proc_entry_t;

/* ---- Trace infrastructure ---- */
static FILE *trace_fp = NULL;

void osal_mock_set_trace(FILE *fp) { trace_fp = fp; }

static void trace(const char *fmt, ...) {
    if (!trace_fp) return;
    va_list ap;
    va_start(ap, fmt);
    vfprintf(trace_fp, fmt, ap);
    va_end(ap);
    fputc('\n', trace_fp);
}

/* ---- Memory ---- */
void *osal_kmalloc(unsigned long size, unsigned int flags) {
    void *p = malloc(size);
    trace("osal_kmalloc(%lu) = %p", size, p);
    return p;
}

void osal_kfree(const void *addr) {
    trace("osal_kfree(%p)", addr);
    free((void *)addr);
}

void *osal_vmalloc(unsigned long size) {
    void *p = malloc(size);
    trace("osal_vmalloc(%lu) = %p", size, p);
    return p;
}

void osal_vfree(const void *addr) {
    trace("osal_vfree(%p)", addr);
    free((void *)addr);
}

void *osal_memset(void *s, int c, int n) {
    return memset(s, c, n);
}

void *osal_memcpy(void *dst, const void *src, int n) {
    return memcpy(dst, src, n);
}

void *osal_memmove(void *dst, const void *src, int n) {
    return memmove(dst, src, n);
}

unsigned long osal_copy_from_user(void *to, const void *from, unsigned long n) {
    memcpy(to, from, n);
    return 0;
}

/* ---- String ---- */
int osal_snprintf(char *buf, int size, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int ret = vsnprintf(buf, size, fmt, ap);
    va_end(ap);
    return ret;
}

/* ---- Synchronization (all no-ops in single-threaded test) ---- */
int osal_sema_init(osal_semaphore_t *sem, int val) {
    if (sem) sem->val = val;
    return 0;
}

void osal_sema_destory(osal_semaphore_t *sem) { (void)sem; }
int osal_down_interruptible(osal_semaphore_t *sem) { (void)sem; return 0; }
void osal_up(osal_semaphore_t *sem) { (void)sem; }

int osal_spin_lock_init(osal_spinlock_t *lock) { (void)lock; return 0; }
void osal_spin_lock_irqsave(osal_spinlock_t *lock, unsigned long *flags) {
    (void)lock; if (flags) *flags = 0;
}
void osal_spin_unlock_irqrestore(osal_spinlock_t *lock, unsigned long *flags) {
    (void)lock; (void)flags;
}
void osal_spin_lock_destory(osal_spinlock_t *lock) { (void)lock; }

/* ---- Logging ---- */
int osal_printk(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int ret = vfprintf(stderr, fmt, ap);
    va_end(ap);
    return ret;
}

/* ---- Time ---- */
void osal_gettimeofday(void *tv) {
    struct timeval t;
    gettimeofday(&t, NULL);
    /* osal_timeval_t layout: { long sec; long usec; } */
    long *p = (long *)tv;
    p[0] = t.tv_sec;
    p[1] = t.tv_usec;
}

/* ---- Proc filesystem stubs ---- */
osal_proc_entry_t *osal_create_proc_entry(const char *name, void *parent) {
    trace("osal_create_proc_entry(\"%s\")", name);
    osal_proc_entry_t *entry = calloc(1, sizeof(*entry));
    if (entry) strncpy(entry->name, name, sizeof(entry->name) - 1);
    return entry;
}

void osal_remove_proc_entry(const char *name, void *parent) {
    trace("osal_remove_proc_entry(\"%s\")", name);
}

int osal_seq_printf(void *entry, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int ret = vfprintf(stdout, fmt, ap);
    va_end(ap);
    return ret;
}

/* ---- Panic ---- */
void osal_panic(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    abort();
}
