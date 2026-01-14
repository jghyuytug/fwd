/**
 * @file packet_codec.c
 * @brief Packet encoding/decoding implementation
 */

#include "packet_codec.h"
#include <stdio.h>
#include <string.h>

/* Write little-endian u16 */
static void write_le16(u8* buf, u16 value)
{
    buf[0] = (u8)(value & 0xFF);
    buf[1] = (u8)((value >> 8) & 0xFF);
}

/* Write little-endian u32 */
static void write_le32(u8* buf, u32 value)
{
    buf[0] = (u8)(value & 0xFF);
    buf[1] = (u8)((value >> 8) & 0xFF);
    buf[2] = (u8)((value >> 16) & 0xFF);
    buf[3] = (u8)((value >> 24) & 0xFF);
}

/* Read little-endian u16 */
static u16 read_le16(const u8* buf)
{
    return (u16)buf[0] | ((u16)buf[1] << 8);
}

/* Read little-endian u32 */
static u32 read_le32(const u8* buf)
{
    return (u32)buf[0] |
           ((u32)buf[1] << 8) |
           ((u32)buf[2] << 16) |
           ((u32)buf[3] << 24);
}

void packet_builder_init(PacketBuilder* builder, u16 msg_no)
{
    if (!builder) return;

    memset(builder, 0, sizeof(PacketBuilder));
    builder->msg_no = msg_no;
    builder->offset = 0;
}

int packet_write_u8(PacketBuilder* builder, u8 value)
{
    if (!builder) return ENGINE_ERROR_PARAM;
    if (builder->offset + 1 > PACKET_MAX_PAYLOAD) return ENGINE_ERROR_FULL;

    builder->buffer[builder->offset++] = value;
    return ENGINE_OK;
}

int packet_write_u16(PacketBuilder* builder, u16 value)
{
    if (!builder) return ENGINE_ERROR_PARAM;
    if (builder->offset + 2 > PACKET_MAX_PAYLOAD) return ENGINE_ERROR_FULL;

    write_le16(builder->buffer + builder->offset, value);
    builder->offset += 2;
    return ENGINE_OK;
}

int packet_write_u32(PacketBuilder* builder, u32 value)
{
    if (!builder) return ENGINE_ERROR_PARAM;
    if (builder->offset + 4 > PACKET_MAX_PAYLOAD) return ENGINE_ERROR_FULL;

    write_le32(builder->buffer + builder->offset, value);
    builder->offset += 4;
    return ENGINE_OK;
}

int packet_write_i32(PacketBuilder* builder, i32 value)
{
    return packet_write_u32(builder, (u32)value);
}

int packet_write_bytes(PacketBuilder* builder, const u8* data, int len)
{
    if (!builder || !data || len <= 0) return ENGINE_ERROR_PARAM;
    if (builder->offset + len > PACKET_MAX_PAYLOAD) return ENGINE_ERROR_FULL;

    memcpy(builder->buffer + builder->offset, data, len);
    builder->offset += len;
    return ENGINE_OK;
}

int packet_write_string(PacketBuilder* builder, const char* str, int max_len)
{
    int len;

    if (!builder) return ENGINE_ERROR_PARAM;

    if (!str) {
        return packet_write_u16(builder, 0);
    }

    len = (int)strlen(str);
    if (len > max_len) len = max_len;

    if (packet_write_u16(builder, (u16)len) != ENGINE_OK) {
        return ENGINE_ERROR_FULL;
    }

    if (len > 0) {
        return packet_write_bytes(builder, (const u8*)str, len);
    }

    return ENGINE_OK;
}

int packet_write_fixed_string(PacketBuilder* builder, const char* str, int len)
{
    int slen;
    int pad;

    if (!builder || len <= 0) return ENGINE_ERROR_PARAM;
    if (builder->offset + len > PACKET_MAX_PAYLOAD) return ENGINE_ERROR_FULL;

    if (str) {
        slen = (int)strlen(str);
        if (slen > len) slen = len;
        memcpy(builder->buffer + builder->offset, str, slen);
        pad = len - slen;
        if (pad > 0) {
            memset(builder->buffer + builder->offset + slen, 0, pad);
        }
    } else {
        memset(builder->buffer + builder->offset, 0, len);
    }

    builder->offset += len;
    return ENGINE_OK;
}

int packet_builder_finish(PacketBuilder* builder, u16 sequence,
                          u8* output, int max_len)
{
    int total_len;
    u32 checksum;

    if (!builder || !output) return ENGINE_ERROR_PARAM;

    total_len = PACKET_HEADER_SIZE + builder->offset;
    if (total_len > max_len) return ENGINE_ERROR_FULL;

    /* Build header */
    output[0] = 0;  /* classification */
    write_le32(output + 1, (u32)builder->offset);  /* payload length */
    write_le16(output + 5, builder->msg_no);       /* msg_no */

    /* Copy payload first for checksum calculation */
    if (builder->offset > 0) {
        memcpy(output + PACKET_HEADER_SIZE, builder->buffer, builder->offset);
    }

    /* Calculate checksum */
    checksum = packet_calculate_checksum(output + PACKET_HEADER_SIZE,
                                          builder->offset);
    write_le32(output + 7, checksum);
    write_le16(output + 11, sequence);

    return total_len;
}

void packet_reader_init(PacketReader* reader, const u8* data, int len)
{
    if (!reader) return;

    reader->data = data;
    reader->len = len;
    reader->offset = 0;
}

u8 packet_read_u8(PacketReader* reader)
{
    if (!reader || reader->offset + 1 > reader->len) return 0;

    return reader->data[reader->offset++];
}

u16 packet_read_u16(PacketReader* reader)
{
    u16 value;

    if (!reader || reader->offset + 2 > reader->len) return 0;

    value = read_le16(reader->data + reader->offset);
    reader->offset += 2;
    return value;
}

u32 packet_read_u32(PacketReader* reader)
{
    u32 value;

    if (!reader || reader->offset + 4 > reader->len) return 0;

    value = read_le32(reader->data + reader->offset);
    reader->offset += 4;
    return value;
}

i32 packet_read_i32(PacketReader* reader)
{
    return (i32)packet_read_u32(reader);
}

int packet_read_bytes(PacketReader* reader, u8* buf, int len)
{
    int available;

    if (!reader || !buf || len <= 0) return 0;

    available = reader->len - reader->offset;
    if (len > available) len = available;

    if (len > 0) {
        memcpy(buf, reader->data + reader->offset, len);
        reader->offset += len;
    }

    return len;
}

int packet_read_string(PacketReader* reader, char* buf, int max_len)
{
    u16 len;
    int copy_len;

    if (!reader || !buf || max_len <= 0) return 0;

    len = packet_read_u16(reader);

    if (len == 0) {
        buf[0] = '\0';
        return 0;
    }

    copy_len = len;
    if (copy_len > max_len - 1) copy_len = max_len - 1;

    packet_read_bytes(reader, (u8*)buf, copy_len);
    buf[copy_len] = '\0';

    /* Skip remaining if string was truncated */
    if (len > copy_len) {
        packet_reader_skip(reader, len - copy_len);
    }

    return copy_len;
}

int packet_read_fixed_string(PacketReader* reader, char* buf, int len)
{
    if (!reader || !buf || len <= 0) return ENGINE_ERROR_PARAM;

    if (reader->offset + len > reader->len) {
        return ENGINE_ERROR_IO;
    }

    memcpy(buf, reader->data + reader->offset, len);
    buf[len - 1] = '\0';  /* Ensure null-terminated */
    reader->offset += len;

    return ENGINE_OK;
}

int packet_reader_skip(PacketReader* reader, int len)
{
    int available;

    if (!reader || len <= 0) return 0;

    available = reader->len - reader->offset;
    if (len > available) len = available;

    reader->offset += len;
    return len;
}

int packet_reader_remaining(const PacketReader* reader)
{
    if (!reader) return 0;
    return reader->len - reader->offset;
}

int packet_parse_header(const u8* data, int len, PacketHeader* header)
{
    if (!data || !header) return ENGINE_ERROR_PARAM;

    if (len < PACKET_HEADER_SIZE) {
        return ENGINE_ERROR;  /* Incomplete header */
    }

    header->classification = data[0];
    header->length = read_le32(data + 1);
    header->msg_no = read_le16(data + 5);
    header->checksum = read_le32(data + 7);
    header->sequence = read_le16(data + 11);

    return ENGINE_OK;
}

u32 packet_calculate_checksum(const u8* data, int len)
{
    u32 sum = 0;
    int i;

    if (!data || len <= 0) return 0;

    /* Simple sum checksum */
    for (i = 0; i < len; i++) {
        sum += data[i];
    }

    return sum;
}

int packet_build_response(u16 msg_no, const u8* payload, int payload_len,
                          u16 sequence, u8* output, int max_len)
{
    PacketBuilder builder;

    packet_builder_init(&builder, msg_no);

    if (payload && payload_len > 0) {
        if (packet_write_bytes(&builder, payload, payload_len) != ENGINE_OK) {
            return ENGINE_ERROR_FULL;
        }
    }

    return packet_builder_finish(&builder, sequence, output, max_len);
}
