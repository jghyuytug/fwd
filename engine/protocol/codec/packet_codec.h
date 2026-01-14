/**
 * @file packet_codec.h
 * @brief Packet encoding/decoding
 *
 * Handles packet serialization following the game protocol.
 *
 * Packet format:
 * [1] class      - 0 or 1
 * [4] length     - payload length (little endian)
 * [2] msg_no     - message number (little endian)
 * [4] checksum   - CRC32 or simple sum
 * [2] sequence   - packet sequence number
 * [N] payload    - message data
 */

#ifndef ENGINE_PROTOCOL_CODEC_PACKET_CODEC_H
#define ENGINE_PROTOCOL_CODEC_PACKET_CODEC_H

#include "../../core/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PACKET_HEADER_SIZE    13
#define PACKET_MAX_SIZE       8192
#define PACKET_MAX_PAYLOAD    (PACKET_MAX_SIZE - PACKET_HEADER_SIZE)

/* Packet header */
typedef struct {
    u8      classification;
    u32     length;
    u16     msg_no;
    u32     checksum;
    u16     sequence;
} PacketHeader;

/* Packet structure */
typedef struct {
    PacketHeader header;
    u8           payload[PACKET_MAX_PAYLOAD];
    int          payload_len;
} Packet;

/* Packet builder context */
typedef struct {
    u8      buffer[PACKET_MAX_SIZE];
    int     offset;
    u16     msg_no;
} PacketBuilder;

/**
 * Initialize packet builder
 * @param builder Builder instance
 * @param msg_no  Message number
 */
void packet_builder_init(PacketBuilder* builder, u16 msg_no);

/**
 * Write u8 to packet
 * @param builder Builder instance
 * @param value   Value to write
 * @return ENGINE_OK on success
 */
int packet_write_u8(PacketBuilder* builder, u8 value);

/**
 * Write u16 to packet (little endian)
 * @param builder Builder instance
 * @param value   Value to write
 * @return ENGINE_OK on success
 */
int packet_write_u16(PacketBuilder* builder, u16 value);

/**
 * Write u32 to packet (little endian)
 * @param builder Builder instance
 * @param value   Value to write
 * @return ENGINE_OK on success
 */
int packet_write_u32(PacketBuilder* builder, u32 value);

/**
 * Write i32 to packet (little endian)
 * @param builder Builder instance
 * @param value   Value to write
 * @return ENGINE_OK on success
 */
int packet_write_i32(PacketBuilder* builder, i32 value);

/**
 * Write bytes to packet
 * @param builder Builder instance
 * @param data    Data to write
 * @param len     Data length
 * @return ENGINE_OK on success
 */
int packet_write_bytes(PacketBuilder* builder, const u8* data, int len);

/**
 * Write string to packet (with length prefix)
 * @param builder Builder instance
 * @param str     String to write
 * @param max_len Maximum length
 * @return ENGINE_OK on success
 */
int packet_write_string(PacketBuilder* builder, const char* str, int max_len);

/**
 * Write fixed-length string (padded with zeros)
 * @param builder Builder instance
 * @param str     String to write
 * @param len     Fixed length
 * @return ENGINE_OK on success
 */
int packet_write_fixed_string(PacketBuilder* builder, const char* str, int len);

/**
 * Finalize packet and get result
 * @param builder  Builder instance
 * @param sequence Packet sequence number
 * @param output   Output buffer
 * @param max_len  Output buffer size
 * @return Packet length, or negative error code
 */
int packet_builder_finish(PacketBuilder* builder, u16 sequence,
                          u8* output, int max_len);

/* Packet reader context */
typedef struct {
    const u8* data;
    int       len;
    int       offset;
} PacketReader;

/**
 * Initialize packet reader
 * @param reader Reader instance
 * @param data   Packet data (payload only)
 * @param len    Data length
 */
void packet_reader_init(PacketReader* reader, const u8* data, int len);

/**
 * Read u8 from packet
 * @param reader Reader instance
 * @return Value, or 0 on error
 */
u8 packet_read_u8(PacketReader* reader);

/**
 * Read u16 from packet (little endian)
 * @param reader Reader instance
 * @return Value, or 0 on error
 */
u16 packet_read_u16(PacketReader* reader);

/**
 * Read u32 from packet (little endian)
 * @param reader Reader instance
 * @return Value, or 0 on error
 */
u32 packet_read_u32(PacketReader* reader);

/**
 * Read i32 from packet (little endian)
 * @param reader Reader instance
 * @return Value, or 0 on error
 */
i32 packet_read_i32(PacketReader* reader);

/**
 * Read bytes from packet
 * @param reader Reader instance
 * @param buf    Output buffer
 * @param len    Bytes to read
 * @return Bytes read
 */
int packet_read_bytes(PacketReader* reader, u8* buf, int len);

/**
 * Read string from packet
 * @param reader  Reader instance
 * @param buf     Output buffer
 * @param max_len Maximum length
 * @return String length
 */
int packet_read_string(PacketReader* reader, char* buf, int max_len);

/**
 * Read fixed-length string
 * @param reader Reader instance
 * @param buf    Output buffer
 * @param len    Fixed length to read
 * @return ENGINE_OK on success
 */
int packet_read_fixed_string(PacketReader* reader, char* buf, int len);

/**
 * Skip bytes
 * @param reader Reader instance
 * @param len    Bytes to skip
 * @return Bytes skipped
 */
int packet_reader_skip(PacketReader* reader, int len);

/**
 * Get remaining bytes
 * @param reader Reader instance
 * @return Remaining bytes
 */
int packet_reader_remaining(const PacketReader* reader);

/**
 * Parse packet header from raw data
 * @param data   Raw data
 * @param len    Data length
 * @param header Output header
 * @return ENGINE_OK on success, ENGINE_ERROR if incomplete
 */
int packet_parse_header(const u8* data, int len, PacketHeader* header);

/**
 * Calculate packet checksum
 * @param data Packet data
 * @param len  Data length
 * @return Checksum value
 */
u32 packet_calculate_checksum(const u8* data, int len);

/**
 * Build response packet (convenience function)
 * @param msg_no   Message number
 * @param payload  Payload data
 * @param payload_len Payload length
 * @param sequence Sequence number
 * @param output   Output buffer
 * @param max_len  Output buffer size
 * @return Packet length, or negative error code
 */
int packet_build_response(u16 msg_no, const u8* payload, int payload_len,
                          u16 sequence, u8* output, int max_len);

#ifdef __cplusplus
}
#endif

#endif /* ENGINE_PROTOCOL_CODEC_PACKET_CODEC_H */
