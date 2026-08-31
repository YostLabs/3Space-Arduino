#ifndef __RING_BUF2_H__
#define __RING_BUF2_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define TSS_RING_POW_2(x) ((x) && ((x) & ((x) - 1)) == 0)

struct TSS_Ring_Buf2 {
    size_t w_index;
    size_t r_index;
    uint8_t *data;

    //This MUST be a power of 2
    size_t capacity;
};

inline static size_t ring_size(const struct TSS_Ring_Buf2 *ring) {
    return ring->w_index - ring->r_index;
}

inline static size_t ring_space(const struct TSS_Ring_Buf2 *ring) {
    return ring->capacity - ring_size(ring);
}

inline static bool ring_full(const struct TSS_Ring_Buf2 *ring) {
    return ring_size(ring) == ring->capacity;
}

inline static bool ring_empty(const struct TSS_Ring_Buf2 *ring) {
    return ring->r_index == ring->w_index;
}

inline static size_t ring_index(const struct TSS_Ring_Buf2 *ring, size_t index) {
    return index & (ring->capacity-1);
}

inline static void ring_push(struct TSS_Ring_Buf2 *ring, uint8_t value) {
    //Not going to check if ring is already full, because whether you choose to overwrite or
    //not include the element, you have an issue. The code around the ring buf should be written
    //in such a way to avoid this.
    ring->data[ring_index(ring, ring->w_index++)] = value;
}

inline static uint8_t ring_pop(struct TSS_Ring_Buf2 *ring) {
    return ring->data[ring_index(ring, ring->r_index++)];
}

inline static void ring_advance(struct TSS_Ring_Buf2 *ring, size_t count) {
    ring->r_index += count;
}

inline static void ring_clear(struct TSS_Ring_Buf2 *ring)
{
    ring_advance(ring, ring_size(ring));
}

inline static uint8_t ring_read(const struct TSS_Ring_Buf2 *ring, size_t index) {
    return ring->data[ring_index(ring, index + ring->r_index)];
}


#endif