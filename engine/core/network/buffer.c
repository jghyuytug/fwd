/**
 * @file buffer.c
 * @brief Ring buffer implementation
 */

#include "buffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int ring_buffer_init(RingBuffer* buffer, u32 capacity)
{
    if (!buffer) {
        return ENGINE_ERROR_PARAM;
    }

    if (capacity == 0) {
        capacity = RING_BUFFER_DEFAULT_SIZE;
    }

    buffer->data = (u8*)malloc(capacity);
    if (!buffer->data) {
        return ENGINE_ERROR_MEMORY;
    }

    buffer->capacity = capacity;
    buffer->read_pos = 0;
    buffer->write_pos = 0;
    buffer->size = 0;

    return ENGINE_OK;
}

void ring_buffer_cleanup(RingBuffer* buffer)
{
    if (!buffer) {
        return;
    }

    if (buffer->data) {
        free(buffer->data);
        buffer->data = NULL;
    }

    buffer->capacity = 0;
    buffer->read_pos = 0;
    buffer->write_pos = 0;
    buffer->size = 0;
}

void ring_buffer_reset(RingBuffer* buffer)
{
    if (buffer) {
        buffer->read_pos = 0;
        buffer->write_pos = 0;
        buffer->size = 0;
    }
}

u32 ring_buffer_readable(const RingBuffer* buffer)
{
    return buffer ? buffer->size : 0;
}

u32 ring_buffer_writable(const RingBuffer* buffer)
{
    return buffer ? (buffer->capacity - buffer->size) : 0;
}

int ring_buffer_is_empty(const RingBuffer* buffer)
{
    return buffer ? (buffer->size == 0) : 1;
}

int ring_buffer_is_full(const RingBuffer* buffer)
{
    return buffer ? (buffer->size >= buffer->capacity) : 1;
}

u32 ring_buffer_write(RingBuffer* buffer, const u8* data, u32 len)
{
    u32 available;
    u32 to_write;
    u32 first_part;
    u32 second_part;

    if (!buffer || !buffer->data || !data || len == 0) {
        return 0;
    }

    available = buffer->capacity - buffer->size;
    to_write = MIN(len, available);

    if (to_write == 0) {
        return 0;
    }

    /* Write in one or two parts (wrap around) */
    first_part = MIN(to_write, buffer->capacity - buffer->write_pos);
    memcpy(buffer->data + buffer->write_pos, data, first_part);

    second_part = to_write - first_part;
    if (second_part > 0) {
        memcpy(buffer->data, data + first_part, second_part);
    }

    buffer->write_pos = (buffer->write_pos + to_write) % buffer->capacity;
    buffer->size += to_write;

    return to_write;
}

u32 ring_buffer_read(RingBuffer* buffer, u8* data, u32 len)
{
    u32 to_read;
    u32 first_part;
    u32 second_part;

    if (!buffer || !buffer->data || !data || len == 0) {
        return 0;
    }

    to_read = MIN(len, buffer->size);

    if (to_read == 0) {
        return 0;
    }

    /* Read in one or two parts (wrap around) */
    first_part = MIN(to_read, buffer->capacity - buffer->read_pos);
    memcpy(data, buffer->data + buffer->read_pos, first_part);

    second_part = to_read - first_part;
    if (second_part > 0) {
        memcpy(data + first_part, buffer->data, second_part);
    }

    buffer->read_pos = (buffer->read_pos + to_read) % buffer->capacity;
    buffer->size -= to_read;

    return to_read;
}

u32 ring_buffer_peek(const RingBuffer* buffer, u8* data, u32 len)
{
    u32 to_read;
    u32 first_part;
    u32 second_part;
    u32 read_pos;

    if (!buffer || !buffer->data || !data || len == 0) {
        return 0;
    }

    to_read = MIN(len, buffer->size);

    if (to_read == 0) {
        return 0;
    }

    read_pos = buffer->read_pos;

    first_part = MIN(to_read, buffer->capacity - read_pos);
    memcpy(data, buffer->data + read_pos, first_part);

    second_part = to_read - first_part;
    if (second_part > 0) {
        memcpy(data + first_part, buffer->data, second_part);
    }

    return to_read;
}

u32 ring_buffer_skip(RingBuffer* buffer, u32 len)
{
    u32 to_skip;

    if (!buffer || len == 0) {
        return 0;
    }

    to_skip = MIN(len, buffer->size);
    buffer->read_pos = (buffer->read_pos + to_skip) % buffer->capacity;
    buffer->size -= to_skip;

    return to_skip;
}

const u8* ring_buffer_read_ptr(const RingBuffer* buffer, u32* len)
{
    u32 contiguous;

    if (!buffer || !buffer->data || !len) {
        if (len) *len = 0;
        return NULL;
    }

    if (buffer->size == 0) {
        *len = 0;
        return NULL;
    }

    /* Calculate contiguous readable bytes */
    if (buffer->write_pos > buffer->read_pos) {
        contiguous = buffer->write_pos - buffer->read_pos;
    } else {
        contiguous = buffer->capacity - buffer->read_pos;
    }

    *len = contiguous;
    return buffer->data + buffer->read_pos;
}

u8* ring_buffer_write_ptr(RingBuffer* buffer, u32* len)
{
    u32 available;
    u32 contiguous;

    if (!buffer || !buffer->data || !len) {
        if (len) *len = 0;
        return NULL;
    }

    available = buffer->capacity - buffer->size;
    if (available == 0) {
        *len = 0;
        return NULL;
    }

    /* Calculate contiguous writable bytes */
    if (buffer->read_pos > buffer->write_pos) {
        contiguous = buffer->read_pos - buffer->write_pos;
    } else {
        contiguous = buffer->capacity - buffer->write_pos;
        if (buffer->read_pos == 0 && contiguous == buffer->capacity) {
            /* Full circle case */
        }
    }

    contiguous = MIN(contiguous, available);
    *len = contiguous;
    return buffer->data + buffer->write_pos;
}

void ring_buffer_commit_write(RingBuffer* buffer, u32 len)
{
    u32 available;

    if (!buffer || len == 0) {
        return;
    }

    available = buffer->capacity - buffer->size;
    len = MIN(len, available);

    buffer->write_pos = (buffer->write_pos + len) % buffer->capacity;
    buffer->size += len;
}

void ring_buffer_commit_read(RingBuffer* buffer, u32 len)
{
    if (!buffer || len == 0) {
        return;
    }

    len = MIN(len, buffer->size);
    buffer->read_pos = (buffer->read_pos + len) % buffer->capacity;
    buffer->size -= len;
}
