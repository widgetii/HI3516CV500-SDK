/**
 * Reverse Engineered from V2.0.2.1 vendor libmpi af_buf.o
 *
 * Audio Frame Buffer — linked-list management for audio frame nodes.
 * Each audio channel context (e.g. ADEC_CHN_CTX_S) contains an AF buffer
 * region that this module manages as free/busy doubly-linked lists.
 *
 * Layout within the AF buffer (base pointer = ctx):
 *   ctx + 0x0000 .. ctx + 0x5400 : node array (up to 300 nodes × 72 bytes)
 *   ctx + 0x5000 + 0x460 (0x5460): frame data buffer pointer
 *   ctx + 0x5000 + 0x464 (0x5464): total node count
 *   ctx + 0x5000 + 0x468 (0x5468): free list count
 *   ctx + 0x5000 + 0x46C (0x546C): busy list count
 *   ctx + 0x5400 + 0x070 (0x5470): free list head (sentinel next/prev)
 *   ctx + 0x5400 + 0x078 (0x5478): busy list head (sentinel next/prev)
 *
 * Each node is 72 (0x48) bytes:
 *   +0x00: frame data (8 bytes)
 *   +0x08: pointer to frame data in allocated buffer (offset 0x0000 per node, stride 0x8000)
 *   +0x0C: pointer to frame data channel 2 (offset 0x4000 per node)
 *   +0x10..0x3B: frame metadata
 *   +0x3C: next pointer (doubly-linked list)
 *   +0x40: prev pointer (doubly-linked list)
 *   +0x44: field_44
 */

#include "re_mpi_adec.h"
#include "securec.h"

#include <stdlib.h>
#include <string.h>

/* Offsets relative to the AF buffer base pointer */
#define AF_META_BASE         0x5000
#define AF_DATABUF_OFF       (AF_META_BASE + 0x460)  /* 0x5460 */
#define AF_TOTAL_OFF         (AF_META_BASE + 0x464)  /* 0x5464 */
#define AF_FREE_CNT_OFF      (AF_META_BASE + 0x468)  /* 0x5468 */
#define AF_BUSY_CNT_OFF      (AF_META_BASE + 0x46C)  /* 0x546C */
#define AF_FREE_HEAD_OFF     (0x5400 + 0x70)         /* 0x5470 */
#define AF_BUSY_HEAD_OFF     (0x5400 + 0x78)         /* 0x5478 */

#define AF_NODE_SIZE         0x48  /* 72 bytes per node */
#define AF_NODE_LINK_OFF     0x3C  /* offset of next/prev within node */
#define AF_MAX_COUNT         300
#define AF_FRAME_DATA_SIZE   0x8000  /* 32768 bytes per frame data slot */
#define AF_FRAME_DATA_CH2    0x4000  /* channel 2 offset within slot */

/* Helper macros to access fields by byte offset from base */
#define PTR_AT(base, off)    (*(void **)((char *)(base) + (off)))
#define U32_AT(base, off)    (*(unsigned int *)((char *)(base) + (off)))

/* Doubly-linked list node: next at offset+0, prev at offset+4 */
typedef struct af_list_node {
    struct af_list_node *next;
    struct af_list_node *prev;
} af_list_node_t;

#define NODE_LINK(nodebase)  ((af_list_node_t *)((char *)(nodebase) + AF_NODE_LINK_OFF))


int
af_buf_init(void *ctx, unsigned int count)
{
    unsigned int i;
    unsigned int alloc_size;
    char *data_buf;
    af_list_node_t *free_head;
    af_list_node_t *busy_head;
    char *meta;
    char *node;
    af_list_node_t *link;
    af_list_node_t *prev_link;

    if (ctx == NULL)
        return -1;

    if (count > AF_MAX_COUNT)
        count = AF_MAX_COUNT;

    alloc_size = count << 15;  /* count * 0x8000 */
    if (alloc_size == 0)
        return -1;

    meta = (char *)ctx + AF_META_BASE;

    data_buf = (char *)malloc(alloc_size);
    *(char **)(meta + 0x460) = data_buf;
    if (data_buf == NULL)
        return -1;

    memset_s(data_buf, alloc_size, 0, alloc_size);

    free_head = (af_list_node_t *)((char *)ctx + AF_FREE_HEAD_OFF);
    busy_head = (af_list_node_t *)((char *)ctx + AF_BUSY_HEAD_OFF);

    /* Initialize both sentinel heads to point to themselves */
    free_head->next = free_head;
    free_head->prev = free_head;
    busy_head->next = busy_head;
    busy_head->prev = busy_head;

    if (count == 0)
        goto done;

    /* Build the free list and assign frame data pointers */
    prev_link = free_head;
    for (i = 0; i < count; i++) {
        node = (char *)ctx + i * AF_NODE_SIZE;
        link = NODE_LINK(node);

        /* Set frame data pointers for this node */
        *(char **)(node + 0x08) = data_buf + i * AF_FRAME_DATA_SIZE;
        *(char **)(node + 0x0C) = data_buf + i * AF_FRAME_DATA_SIZE + AF_FRAME_DATA_CH2;

        /* field_44 = 0 */
        *(unsigned int *)(node + 0x44) = 0;

        /* Link into free list */
        link->next = free_head;
        link->prev = prev_link;
        prev_link->next = link;
        prev_link = link;
    }

    /* Update free list tail's prev */
    free_head->prev = prev_link;

done:
    *(unsigned int *)(meta + 0x464) = count;  /* total count */
    *(unsigned int *)(meta + 0x468) = count;  /* free count */
    *(unsigned int *)(meta + 0x46C) = 0;      /* busy count */
    return 0;
}


int
af_buf_reset(void *ctx)
{
    af_list_node_t *free_head;
    af_list_node_t *busy_head;
    af_list_node_t *node;
    af_list_node_t *next;
    af_list_node_t *free_tail;
    char *meta;

    if (ctx == NULL)
        return -1;

    meta = (char *)ctx + AF_META_BASE;
    free_head = (af_list_node_t *)((char *)ctx + AF_FREE_HEAD_OFF);
    busy_head = (af_list_node_t *)((char *)ctx + AF_BUSY_HEAD_OFF);

    /* Move all busy nodes back to the free list */
    node = busy_head->next;
    while (node != busy_head) {
        next = node->next;

        /* Remove from busy list */
        next->prev = node->prev;
        node->prev->next = next;

        /* Append to end of free list */
        free_tail = free_head->prev;
        node->next = free_head;
        node->prev = free_tail;
        free_tail->next = node;

        node = next;
    }

    /* Reset busy head to empty */
    busy_head->next = busy_head;
    busy_head->prev = busy_head;

    /* Restore free count = total, busy count = 0 */
    *(unsigned int *)(meta + 0x468) = *(unsigned int *)(meta + 0x464);
    *(unsigned int *)(meta + 0x46C) = 0;
    return 0;
}


void *
af_buf_get_free(void *ctx)
{
    af_list_node_t *free_head;
    af_list_node_t *node;
    af_list_node_t *next;
    af_list_node_t *prev;
    char *meta;

    if (ctx == NULL)
        return NULL;

    meta = (char *)ctx + AF_META_BASE;
    free_head = (af_list_node_t *)((char *)ctx + AF_FREE_HEAD_OFF);

    node = free_head->next;
    if (node == free_head)
        return NULL;

    /* Remove node from free list */
    next = node->next;
    prev = node->prev;
    prev->next = next;
    next->prev = prev;

    /* Decrement free count */
    *(unsigned int *)(meta + 0x468) -= 1;

    /* Return pointer to the node base (node - 0x3C) */
    return (char *)node - AF_NODE_LINK_OFF;
}


void *
af_buf_get_busy(void *ctx)
{
    af_list_node_t *busy_head;
    af_list_node_t *node;
    af_list_node_t *next;
    af_list_node_t *prev;
    char *meta;

    if (ctx == NULL)
        return NULL;

    meta = (char *)ctx + AF_META_BASE;
    busy_head = (af_list_node_t *)((char *)ctx + AF_BUSY_HEAD_OFF);

    node = busy_head->next;
    if (node == busy_head)
        return NULL;

    /* Remove node from busy list */
    next = node->next;
    prev = node->prev;
    prev->next = next;
    next->prev = prev;

    /* Decrement busy count */
    *(unsigned int *)(meta + 0x46C) -= 1;

    /* Return pointer to the node base (node - 0x3C) */
    return (char *)node - AF_NODE_LINK_OFF;
}


int
af_buf_is_list_mem(void *ctx, void *mem, unsigned int idx)
{
    unsigned int total;
    char *expected;

    if (mem == NULL || ctx == NULL)
        return 0;

    total = *(unsigned int *)((char *)ctx + AF_META_BASE + 0x464);
    if (total <= idx)
        return 0;

    expected = (char *)ctx + idx * AF_NODE_SIZE;
    return (mem == expected) ? 1 : 0;
}


int
af_buf_is_free_list_mem(void *ctx, void *mem)
{
    af_list_node_t *free_head;
    af_list_node_t *node;
    char *node_base;

    if (mem == NULL || ctx == NULL)
        return 0;

    free_head = (af_list_node_t *)((char *)ctx + AF_FREE_HEAD_OFF);
    node = free_head->next;

    while (node != free_head) {
        node_base = (char *)node - AF_NODE_LINK_OFF;
        if (mem == node_base)
            return 1;
        node = node->next;
    }

    return 0;
}


int
af_buf_is_busy_list_mem(void *ctx, void *mem)
{
    af_list_node_t *busy_head;
    af_list_node_t *node;
    char *node_base;

    if (mem == NULL || ctx == NULL)
        return 0;

    busy_head = (af_list_node_t *)((char *)ctx + AF_BUSY_HEAD_OFF);
    node = busy_head->next;

    while (node != busy_head) {
        node_base = (char *)node - AF_NODE_LINK_OFF;
        if (mem == node_base)
            return 1;
        node = node->next;
    }

    return 0;
}