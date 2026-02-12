#ifndef RINGBUFFER_STATIC_H
#define RINGBUFFER_STATIC_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct ringbuffer_control *ringbuffer_t;

/**
 * Init med statisk backing storage.
 * storage_bytes skal være >= capacity * elem_size.
 */
bool ringbuffer_init_static(ringbuffer_t rb,
                            void *storage,
                            size_t storage_bytes,
                            size_t capacity,
                            size_t elem_size);

void ringbuffer_reset(ringbuffer_t rb);

bool ringbuffer_push(ringbuffer_t rb, const void *item); // overwriter ældste hvis fuld
bool ringbuffer_pop(ringbuffer_t rb, void *item);

bool   ringbuffer_is_empty(const ringbuffer_t rb);
bool   ringbuffer_is_full(const ringbuffer_t rb);
size_t ringbuffer_size(const ringbuffer_t rb);

#endif