/**
 * Reverse Engineered from V2.0.2.1 vendor libmpi as_buf.o
 *
 * Audio Stream Buffer — linked-list management for audio stream nodes.
 * Same pattern as af_buf but for stream (encoded) audio data.
 *
 * Layout within the AS buffer (base pointer = ctx):
 *   Node stride: 0x4030 (16432) bytes per node
 *   Node data starts at +0x28 from each node base (node + 0x28 = data pointer stored)
 *   Linked list pointers at node + 0x4028 (next) and node + 0x402C (prev)
 *
 *   ctx + 0x4B3844: free list head (sentinel next/prev)
 *   ctx + 0x4B384C: busy list head (sentinel next/prev)
 *   ctx + 0x4B3000 + 0x844 = free head sentinel
 *   ctx + 0x4B3000 + 0x848 = free head prev (tail)
 *   ctx + 0x4B3000 + 0x84C = busy head sentinel
 *   ctx + 0x4B3000 + 0x850 = busy head prev (tail)
 */

#include <stddef.h>

/* Offsets from base */
#define AS_NODE_STRIDE       0x4030  /* 16432 bytes per node */
#define AS_NODE_LINK_OFF     0x4028  /* offset of next/prev within node (from node data start) */
#define AS_NODE_DATA_OFF     0x28    /* data pointer offset within each node */

#define AS_META_BASE         0x4B3000
#define AS_FREE_HEAD_OFF     0x4B3844  /* free list sentinel (next, prev) */
#define AS_BUSY_HEAD_OFF     0x4B384C  /* busy list sentinel (next, prev) */

/* Doubly-linked list node */
typedef struct as_list_node {
    struct as_list_node *next;
    struct as_list_node *prev;
} as_list_node_t;

#define AS_NODE_LINK(base)   ((as_list_node_t *)((char *)(base) + AS_NODE_LINK_OFF))


int
as_buf_init(void *ctx, unsigned int count)
{
    unsigned int i;
    as_list_node_t *free_head;
    as_list_node_t *busy_head;
    as_list_node_t *link;
    as_list_node_t *prev_link;
    char *node;
    char *end;

    if (ctx == NULL)
        return -1;

    if (count > 300)
        count = 300;

    free_head = (as_list_node_t *)((char *)ctx + AS_FREE_HEAD_OFF);
    busy_head = (as_list_node_t *)((char *)ctx + AS_BUSY_HEAD_OFF);

    /* Initialize both sentinel heads to point to themselves */
    free_head->next = free_head;
    free_head->prev = free_head;
    busy_head->next = busy_head;
    busy_head->prev = busy_head;

    if (count == 0)
        goto done;

    /* Build the free list */
    end = (char *)ctx + AS_NODE_LINK_OFF + count * AS_NODE_STRIDE;
    prev_link = free_head;
    for (i = 0; i < count; i++) {
        node = (char *)ctx + i * AS_NODE_STRIDE;
        link = AS_NODE_LINK(node);

        /* Store the data pointer (node + 0x28) at node + 0x00 */
        *(char **)(node) = node + AS_NODE_DATA_OFF;

        /* Link into free list */
        link->next = free_head;
        link->prev = prev_link;
        prev_link->next = link;
        prev_link = link;
    }

    /* Update free list tail */
    free_head->prev = prev_link;

done:
    return 0;
}


void *
as_buf_get_free(void *ctx)
{
    as_list_node_t *free_head;
    as_list_node_t *node;
    as_list_node_t *next;
    as_list_node_t *prev;

    if (ctx == NULL)
        return NULL;

    free_head = (as_list_node_t *)((char *)ctx + AS_FREE_HEAD_OFF);

    node = free_head->next;
    if (node == free_head)
        return NULL;

    /* Remove node from free list */
    next = node->next;
    prev = node->prev;
    prev->next = next;
    next->prev = prev;

    /* Return pointer to the node base (node - 0x4028 - 0x28 = node - 0x4050) */
    /* Actually: node is at nodebase + 0x4028, data is at nodebase + 0x28 */
    /* The assembly does: sub r0, r2, #16384; sub r0, r0, #40 = sub r0, r2, #0x4028 */
    return (char *)node - AS_NODE_LINK_OFF;
}


void *
as_buf_get_busy(void *ctx)
{
    as_list_node_t *busy_head;
    as_list_node_t *node;
    as_list_node_t *next;
    as_list_node_t *prev;

    if (ctx == NULL)
        return NULL;

    busy_head = (as_list_node_t *)((char *)ctx + AS_BUSY_HEAD_OFF);

    node = busy_head->next;
    if (node == busy_head)
        return NULL;

    /* Remove node from busy list */
    next = node->next;
    prev = node->prev;
    prev->next = next;
    next->prev = prev;

    /* Return pointer to the node base */
    return (char *)node - AS_NODE_LINK_OFF;
}