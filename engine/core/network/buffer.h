/**
 * @file buffer.h
 * @brief Ring buffer for network I/O
 *
 * Efficient circular buffer for read/write operations.
 */

#ifndef ENGINE_CORE_NETWORK_BUFFER_H
#define ENGINE_CORE_NETWORK_BUFFER_H

#include "../types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RING_BUFFER_DEFAULT_SIZE 65536

/* Ring buffer structure */
typedef struct {
    u8*    data;
    u32    capacity;
    u32    read_pos;
    u32    write_pos;
    u32    size;       /* Current data size */
} RingBuffer;

/**
 * Initialize ring buffer
 * @param buffer   Buffer instance
 * @param capacity Buffer capacity (0 for default)
 * @return ENGINE_OK on success
 */
int ring_buffer_init(RingBuffer* buffer, u32 capacity);

/**
 * Cleanup and free buffer
 * @param buffer  Buffer instance
 */
void ring_buffer_cleanup(RingBuffer* buffer);

/**
 * Reset buffer to empty state
 * @param buffer  Buffer instance
 */
void ring_buffer_reset(RingBuffer* buffer);

/**
 * Get available data size
 * @param buffer  Buffer instance
 * @return Number of bytes available to read
 */
u32 ring_buffer_readable(const RingBuffer* buffer);

/**
 * Get available write space
 * @param buffer  Buffer instance
 * @return Number of bytes available to write
 */
u32 ring_buffer_writable(const RingBuffer* buffer);

/**
 * Check if buffer is empty
 * @param buffer  Buffer instance
 * @return 1 if empty, 0 otherwise
 */
int ring_buffer_is_empty(const RingBuffer* buffer);

/**
 * Check if buffer is full
 * @param buffer  Buffer instance
 * @return 1 if full, 0 otherwise
 */
int ring_buffer_is_full(const RingBuffer* buffer);

/**
 * Write data to buffer
 * @param buffer  Buffer instance
 * @param data    Data to write
 * @param len     Data length
 * @return Number of bytes written
 */
u32 ring_buffer_write(RingBuffer* buffer, const u8* data, u32 len);

/**
 * Read data from buffer (and remove)
 * @param buffer  Buffer instance
 * @param data    Output buffer
 * @param len     Maximum bytes to read
 * @return Number of bytes read
 */
u32 ring_buffer_read(RingBuffer* buffer, u8* data, u32 len);

/**
 * Peek data from buffer (without removing)
 * @param buffer  Buffer instance
 * @param data    Output buffer
 * @param len     Maximum bytes to peek
 * @return Number of bytes peeked
 */
u32 ring_buffer_peek(const RingBuffer* buffer, u8* data, u32 len);

/**
 * Skip bytes from buffer
 * @param buffer  Buffer instance
 * @param len     Bytes to skip
 * @return Number of bytes skipped
 */
u32 ring_buffer_skip(RingBuffer* buffer, u32 len);

/**
 * Get contiguous read pointer
 * @param buffer  Buffer instance
 * @param len     Output: contiguous length available
 * @return Pointer to read data
 */
const u8* ring_buffer_read_ptr(const RingBuffer* buffer, u32* len);

/**
 * Get contiguous write pointer
 * @param buffer  Buffer instance
 * @param len     Output: contiguous space available
 * @return Pointer to write location
 */
u8* ring_buffer_write_ptr(RingBuffer* buffer, u32* len);

/**
 * Commit written bytes
 * @param buffer  Buffer instance
 * @param len     Bytes written to write pointer
 */
void ring_buffer_commit_write(RingBuffer* buffer, u32 len);

/**
 * Commit read bytes
 * @param buffer  Buffer instance
 * @param len     Bytes consumed from read pointer
 */
void ring_buffer_commit_read(RingBuffer* buffer, u32 len);

#ifdef __cplusplus
}
#endif

#endif /* ENGINE_CORE_NETWORK_BUFFER_H */
