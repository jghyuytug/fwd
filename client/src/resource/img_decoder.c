/**
 * @file img_decoder.c
 * @brief DNF IMG Image Decoder Implementation
 *
 * Based on reverse engineering from Taiwan DNF and SkillTreeViewer.
 *
 * IMG Format (Interleaved):
 * ========================
 * Header (32 bytes):
 *   [0-17]  Magic: "Neople Img File" (null-padded)
 *   [18-27] Reserved (index_size, unknown, version)
 *   [28-31] Image count (uint32)
 *
 * Frame Data (Interleaved - Entry followed by compressed data):
 *   For each frame:
 *     Entry (36 bytes):
 *       [0-3]   Color format (14=ARGB1555, 15=ARGB4444, 16=ARGB8888, 0=link)
 *       [4-7]   Compress type (6=zlib, 5=raw, 0=link)
 *       [8-11]  Width
 *       [12-15] Height
 *       [16-19] Compressed data size
 *       [20-23] Key X (offset)
 *       [24-27] Key Y (offset)
 *       [28-31] Max Width
 *       [32-35] Max Height
 *     Data (size bytes): Compressed pixel data (immediately follows entry)
 *
 * Key discovery: Entry and data are INTERLEAVED, not separated!
 *
 * @date 2026-01-04
 */

#include "resource/img_decoder.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <zlib.h>

#ifndef IMG_NO_SDL
#ifdef _WIN32
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif
#endif

/* ========================================================================== */
/* Internal Helpers                                                            */
/* ========================================================================== */

/* Read little-endian uint32_t */
static uint32_t read_u32(const uint8_t** p)
{
    uint32_t v = (*p)[0] | ((*p)[1] << 8) | ((*p)[2] << 16) | ((*p)[3] << 24);
    *p += 4;
    return v;
}

/* Read little-endian int32_t */
static int32_t read_i32(const uint8_t** p)
{
    return (int32_t)read_u32(p);
}

/* Decompress zlib data */
static int decompress_zlib(const uint8_t* src, size_t src_size,
                           uint8_t* dst, size_t dst_size)
{
    uLongf actual_size = (uLongf)dst_size;
    int ret = uncompress(dst, &actual_size, src, (uLong)src_size);
    if (ret != Z_OK) {
        return IMG_ERROR_DECOMPRESS;
    }
    return IMG_SUCCESS;
}

/* Convert ARGB1555 to RGBA8888 */
static void convert_argb1555_to_rgba(const uint16_t* src, uint32_t* dst,
                                     int width, int height)
{
    int count = width * height;
    for (int i = 0; i < count; i++) {
        uint16_t p = src[i];
        uint8_t a = (p & 0x8000) ? 255 : 0;
        uint8_t r = ((p >> 10) & 0x1F) * 255 / 31;
        uint8_t g = ((p >> 5) & 0x1F) * 255 / 31;
        uint8_t b = (p & 0x1F) * 255 / 31;
        dst[i] = (r << 24) | (g << 16) | (b << 8) | a;
    }
}

/* Convert ARGB4444 to RGBA8888 */
static void convert_argb4444_to_rgba(const uint16_t* src, uint32_t* dst,
                                     int width, int height)
{
    int count = width * height;
    for (int i = 0; i < count; i++) {
        uint16_t p = src[i];
        uint8_t a = ((p >> 12) & 0x0F) * 255 / 15;
        uint8_t r = ((p >> 8) & 0x0F) * 255 / 15;
        uint8_t g = ((p >> 4) & 0x0F) * 255 / 15;
        uint8_t b = (p & 0x0F) * 255 / 15;
        dst[i] = (r << 24) | (g << 16) | (b << 8) | a;
    }
}

/* Convert indexed 8-bit to RGBA8888 */
static void convert_index8_to_rgba(const uint8_t* src, uint32_t* dst,
                                   int width, int height,
                                   const IMGPalette* palette)
{
    int count = width * height;
    for (int i = 0; i < count; i++) {
        uint8_t idx = src[i];
        uint8_t r = palette->colors[idx][0];
        uint8_t g = palette->colors[idx][1];
        uint8_t b = palette->colors[idx][2];
        uint8_t a = palette->colors[idx][3];
        dst[i] = (r << 24) | (g << 16) | (b << 8) | a;
    }
}

/* Convert ARGB8888 to RGBA8888 (swap A position) */
static void convert_argb8888_to_rgba(const uint32_t* src, uint32_t* dst,
                                     int width, int height)
{
    int count = width * height;
    for (int i = 0; i < count; i++) {
        uint32_t p = src[i];
        uint8_t a = (p >> 24) & 0xFF;
        uint8_t r = (p >> 16) & 0xFF;
        uint8_t g = (p >> 8) & 0xFF;
        uint8_t b = p & 0xFF;
        dst[i] = (r << 24) | (g << 16) | (b << 8) | a;
    }
}

/* ========================================================================== */
/* DDS/DXT Decoding Implementation                                             */
/* ========================================================================== */

/* Decode RGB565 color to R, G, B components */
static void decode_rgb565(uint16_t c, uint8_t* r, uint8_t* g, uint8_t* b)
{
    *r = (uint8_t)(((c >> 11) & 0x1F) * 255 / 31);
    *g = (uint8_t)(((c >> 5) & 0x3F) * 255 / 63);
    *b = (uint8_t)((c & 0x1F) * 255 / 31);
}

/* Decode a single DXT1 4x4 block */
static void decode_dxt1_block(const uint8_t* block, uint32_t* out, int stride)
{
    uint16_t c0 = block[0] | (block[1] << 8);
    uint16_t c1 = block[2] | (block[3] << 8);
    uint32_t bits = block[4] | (block[5] << 8) | (block[6] << 16) | (block[7] << 24);

    uint8_t r0, g0, b0, r1, g1, b1;
    decode_rgb565(c0, &r0, &g0, &b0);
    decode_rgb565(c1, &r1, &g1, &b1);

    uint8_t colors[4][4]; /* [index][RGBA] */
    colors[0][0] = r0; colors[0][1] = g0; colors[0][2] = b0; colors[0][3] = 255;
    colors[1][0] = r1; colors[1][1] = g1; colors[1][2] = b1; colors[1][3] = 255;

    if (c0 > c1) {
        /* 4-color block */
        colors[2][0] = (2 * r0 + r1) / 3;
        colors[2][1] = (2 * g0 + g1) / 3;
        colors[2][2] = (2 * b0 + b1) / 3;
        colors[2][3] = 255;
        colors[3][0] = (r0 + 2 * r1) / 3;
        colors[3][1] = (g0 + 2 * g1) / 3;
        colors[3][2] = (b0 + 2 * b1) / 3;
        colors[3][3] = 255;
    } else {
        /* 3-color block + transparent */
        colors[2][0] = (r0 + r1) / 2;
        colors[2][1] = (g0 + g1) / 2;
        colors[2][2] = (b0 + b1) / 2;
        colors[2][3] = 255;
        colors[3][0] = 0;
        colors[3][1] = 0;
        colors[3][2] = 0;
        colors[3][3] = 0; /* Transparent */
    }

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            int idx = (bits >> (2 * (y * 4 + x))) & 0x03;
            out[y * stride + x] = (colors[idx][0] << 24) |
                                  (colors[idx][1] << 16) |
                                  (colors[idx][2] << 8) |
                                  colors[idx][3];
        }
    }
}

/* Decode a single DXT3 4x4 block (explicit alpha) */
static void decode_dxt3_block(const uint8_t* block, uint32_t* out, int stride)
{
    /* First 8 bytes: explicit alpha (4 bits per pixel) */
    const uint8_t* alpha_block = block;
    /* Next 8 bytes: DXT1 color block */
    const uint8_t* color_block = block + 8;

    uint16_t c0 = color_block[0] | (color_block[1] << 8);
    uint16_t c1 = color_block[2] | (color_block[3] << 8);
    uint32_t bits = color_block[4] | (color_block[5] << 8) |
                    (color_block[6] << 16) | (color_block[7] << 24);

    uint8_t r0, g0, b0, r1, g1, b1;
    decode_rgb565(c0, &r0, &g0, &b0);
    decode_rgb565(c1, &r1, &g1, &b1);

    uint8_t colors[4][3];
    colors[0][0] = r0; colors[0][1] = g0; colors[0][2] = b0;
    colors[1][0] = r1; colors[1][1] = g1; colors[1][2] = b1;
    colors[2][0] = (2 * r0 + r1) / 3;
    colors[2][1] = (2 * g0 + g1) / 3;
    colors[2][2] = (2 * b0 + b1) / 3;
    colors[3][0] = (r0 + 2 * r1) / 3;
    colors[3][1] = (g0 + 2 * g1) / 3;
    colors[3][2] = (b0 + 2 * b1) / 3;

    for (int y = 0; y < 4; y++) {
        uint16_t alpha_row = alpha_block[y * 2] | (alpha_block[y * 2 + 1] << 8);
        for (int x = 0; x < 4; x++) {
            int idx = (bits >> (2 * (y * 4 + x))) & 0x03;
            uint8_t a = ((alpha_row >> (x * 4)) & 0x0F) * 17; /* Scale 0-15 to 0-255 */
            out[y * stride + x] = (colors[idx][0] << 24) |
                                  (colors[idx][1] << 16) |
                                  (colors[idx][2] << 8) |
                                  a;
        }
    }
}

/* Decode a single DXT5 4x4 block (interpolated alpha) */
static void decode_dxt5_block(const uint8_t* block, uint32_t* out, int stride)
{
    /* First 8 bytes: interpolated alpha */
    uint8_t a0 = block[0];
    uint8_t a1 = block[1];
    uint64_t alpha_bits = 0;
    for (int i = 0; i < 6; i++) {
        alpha_bits |= ((uint64_t)block[2 + i]) << (8 * i);
    }

    uint8_t alphas[8];
    alphas[0] = a0;
    alphas[1] = a1;
    if (a0 > a1) {
        alphas[2] = (6 * a0 + 1 * a1) / 7;
        alphas[3] = (5 * a0 + 2 * a1) / 7;
        alphas[4] = (4 * a0 + 3 * a1) / 7;
        alphas[5] = (3 * a0 + 4 * a1) / 7;
        alphas[6] = (2 * a0 + 5 * a1) / 7;
        alphas[7] = (1 * a0 + 6 * a1) / 7;
    } else {
        alphas[2] = (4 * a0 + 1 * a1) / 5;
        alphas[3] = (3 * a0 + 2 * a1) / 5;
        alphas[4] = (2 * a0 + 3 * a1) / 5;
        alphas[5] = (1 * a0 + 4 * a1) / 5;
        alphas[6] = 0;
        alphas[7] = 255;
    }

    /* Next 8 bytes: DXT1 color block */
    const uint8_t* color_block = block + 8;
    uint16_t c0 = color_block[0] | (color_block[1] << 8);
    uint16_t c1 = color_block[2] | (color_block[3] << 8);
    uint32_t bits = color_block[4] | (color_block[5] << 8) |
                    (color_block[6] << 16) | (color_block[7] << 24);

    uint8_t r0, g0, b0, r1, g1, b1;
    decode_rgb565(c0, &r0, &g0, &b0);
    decode_rgb565(c1, &r1, &g1, &b1);

    uint8_t colors[4][3];
    colors[0][0] = r0; colors[0][1] = g0; colors[0][2] = b0;
    colors[1][0] = r1; colors[1][1] = g1; colors[1][2] = b1;
    colors[2][0] = (2 * r0 + r1) / 3;
    colors[2][1] = (2 * g0 + g1) / 3;
    colors[2][2] = (2 * b0 + b1) / 3;
    colors[3][0] = (r0 + 2 * r1) / 3;
    colors[3][1] = (g0 + 2 * g1) / 3;
    colors[3][2] = (b0 + 2 * b1) / 3;

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            int pixel_idx = y * 4 + x;
            int color_idx = (bits >> (2 * pixel_idx)) & 0x03;
            int alpha_idx = (alpha_bits >> (3 * pixel_idx)) & 0x07;
            out[y * stride + x] = (colors[color_idx][0] << 24) |
                                  (colors[color_idx][1] << 16) |
                                  (colors[color_idx][2] << 8) |
                                  alphas[alpha_idx];
        }
    }
}

/* Decode DXT1 compressed image */
static void decode_dxt1(const uint8_t* src, uint32_t* dst, int width, int height)
{
    int blocks_x = (width + 3) / 4;
    int blocks_y = (height + 3) / 4;

    for (int by = 0; by < blocks_y; by++) {
        for (int bx = 0; bx < blocks_x; bx++) {
            uint32_t block_pixels[16];
            decode_dxt1_block(src, block_pixels, 4);
            src += 8;

            /* Copy block to destination */
            for (int y = 0; y < 4 && (by * 4 + y) < height; y++) {
                for (int x = 0; x < 4 && (bx * 4 + x) < width; x++) {
                    dst[(by * 4 + y) * width + (bx * 4 + x)] = block_pixels[y * 4 + x];
                }
            }
        }
    }
}

/* Decode DXT3 compressed image */
static void decode_dxt3(const uint8_t* src, uint32_t* dst, int width, int height)
{
    int blocks_x = (width + 3) / 4;
    int blocks_y = (height + 3) / 4;

    for (int by = 0; by < blocks_y; by++) {
        for (int bx = 0; bx < blocks_x; bx++) {
            uint32_t block_pixels[16];
            decode_dxt3_block(src, block_pixels, 4);
            src += 16;

            for (int y = 0; y < 4 && (by * 4 + y) < height; y++) {
                for (int x = 0; x < 4 && (bx * 4 + x) < width; x++) {
                    dst[(by * 4 + y) * width + (bx * 4 + x)] = block_pixels[y * 4 + x];
                }
            }
        }
    }
}

/* Decode DXT5 compressed image */
static void decode_dxt5(const uint8_t* src, uint32_t* dst, int width, int height)
{
    int blocks_x = (width + 3) / 4;
    int blocks_y = (height + 3) / 4;

    for (int by = 0; by < blocks_y; by++) {
        for (int bx = 0; bx < blocks_x; bx++) {
            uint32_t block_pixels[16];
            decode_dxt5_block(src, block_pixels, 4);
            src += 16;

            for (int y = 0; y < 4 && (by * 4 + y) < height; y++) {
                for (int x = 0; x < 4 && (bx * 4 + x) < width; x++) {
                    dst[(by * 4 + y) * width + (bx * 4 + x)] = block_pixels[y * 4 + x];
                }
            }
        }
    }
}

/* ========================================================================== */
/* Parsing Implementation                                                      */
/* ========================================================================== */

static int parse_frames_interleaved(const uint8_t* base, const uint8_t* end, IMGFile* img)
{
    const uint8_t* p = base + IMG_HEADER_SIZE;

    img->max_width = 0;
    img->max_height = 0;

    for (uint32_t i = 0; i < img->frame_count; i++) {
        IMGFrame* frame = &img->frames[i];
        frame->link_index = -1;

        if (p + IMG_ENTRY_SIZE > end) {
            return IMG_ERROR_INVALID_MAGIC;
        }

        frame->color_format = read_u32(&p);
        frame->compress = read_u32(&p);
        frame->width = read_i32(&p);
        frame->height = read_i32(&p);
        frame->data_size = read_u32(&p);
        frame->offset_x = read_i32(&p);
        frame->offset_y = read_i32(&p);
        frame->max_width = read_i32(&p);
        frame->max_height = read_i32(&p);

        if (frame->color_format == 0 && frame->compress == 0 &&
            frame->width == 0 && frame->height == 0) {
            frame->is_link = true;
            frame->link_index = (int)frame->data_size;
            frame->data_size = 0;
        } else {
            frame->is_link = false;
        }

        frame->data_offset = (uint32_t)(p - base);

        switch (frame->color_format) {
            case IMG_COLOR_INDEX8:
                frame->raw_size = frame->width * frame->height;
                break;
            case IMG_COLOR_ARGB1555:
            case IMG_COLOR_ARGB4444:
                frame->raw_size = frame->width * frame->height * 2;
                break;
            case IMG_COLOR_ARGB8888:
                frame->raw_size = frame->width * frame->height * 4;
                break;
            default:
                frame->raw_size = 0;
                break;
        }

        if (!frame->is_link && frame->data_size > 0) {
            if (p + frame->data_size > end) {
                return IMG_ERROR_INVALID_MAGIC;
            }
            p += frame->data_size;
        }

        if (frame->width > img->max_width) {
            img->max_width = frame->width;
        }
        if (frame->height > img->max_height) {
            img->max_height = frame->height;
        }
    }

    return IMG_SUCCESS;
}

static int parse_frames_index_table(const uint8_t* base, const uint8_t* end, IMGFile* img)
{
    const uint8_t* p = base + IMG_HEADER_SIZE;
    const size_t size = (size_t)(end - base);

    const size_t index_bytes = (size_t)img->frame_count * IMG_ENTRY_SIZE;
    if ((size_t)(p - base) + index_bytes > size) {
        return IMG_ERROR_INVALID_MAGIC;
    }

    img->max_width = 0;
    img->max_height = 0;

    for (uint32_t i = 0; i < img->frame_count; i++) {
        IMGFrame* frame = &img->frames[i];
        frame->link_index = -1;

        frame->color_format = read_u32(&p);
        frame->compress = read_u32(&p);
        frame->width = read_i32(&p);
        frame->height = read_i32(&p);
        frame->data_size = read_u32(&p);
        frame->offset_x = read_i32(&p);
        frame->offset_y = read_i32(&p);
        frame->max_width = read_i32(&p);
        frame->max_height = read_i32(&p);

        if (frame->color_format == 0 && frame->compress == 0 &&
            frame->width == 0 && frame->height == 0) {
            frame->is_link = true;
            frame->link_index = (int)frame->data_size;
            frame->data_size = 0;
        } else {
            frame->is_link = false;
        }

        switch (frame->color_format) {
            case IMG_COLOR_INDEX8:
                frame->raw_size = frame->width * frame->height;
                break;
            case IMG_COLOR_ARGB1555:
            case IMG_COLOR_ARGB4444:
                frame->raw_size = frame->width * frame->height * 2;
                break;
            case IMG_COLOR_ARGB8888:
                frame->raw_size = frame->width * frame->height * 4;
                break;
            default:
                frame->raw_size = 0;
                break;
        }

        if (frame->width > img->max_width) {
            img->max_width = frame->width;
        }
        if (frame->height > img->max_height) {
            img->max_height = frame->height;
        }
    }

    size_t data_pos = IMG_HEADER_SIZE + index_bytes;
    for (uint32_t i = 0; i < img->frame_count; i++) {
        IMGFrame* frame = &img->frames[i];
        frame->data_offset = (uint32_t)data_pos;

        if (!frame->is_link && frame->data_size > 0) {
            if (data_pos + frame->data_size > size) {
                return IMG_ERROR_INVALID_MAGIC;
            }
            data_pos += frame->data_size;
        }
    }

    return IMG_SUCCESS;
}

int IMG_Parse(const void* data, size_t size, IMGFile* img)
{
    if (data == NULL || img == NULL || size < IMG_HEADER_SIZE) {
        return IMG_ERROR_INVALID_PARAM;
    }

    memset(img, 0, sizeof(IMGFile));
    img->raw_data = (const uint8_t*)data;
    img->raw_size = size;

    const uint8_t* base = (const uint8_t*)data;
    const uint8_t* p = base;
    const uint8_t* end = base + size;

    /* ========================================== */
    /* Parse Header (32 bytes)                    */
    /* ========================================== */

    /* Check magic string: "Neople Img File" at bytes 0-17 */
    if (memcmp(p, "Neople", 6) != 0) {
        return IMG_ERROR_INVALID_MAGIC;
    }

    /* Skip to image count at offset 28 */
    p = base + 28;
    img->frame_count = read_u32(&p);

    /* Sanity check */
    if (img->frame_count == 0 || img->frame_count > IMG_MAX_FRAMES) {
        return IMG_ERROR_INVALID_MAGIC;
    }

    /* Version is typically at offset 24 */
    img->version = *(uint32_t*)(base + 24);
    if (img->version == 0) {
        img->version = 2;  /* Default */
    }

    /* Allocate frames */
    img->frames = (IMGFrame*)calloc(img->frame_count, sizeof(IMGFrame));
    if (img->frames == NULL) {
        return IMG_ERROR_MEMORY;
    }

    /* ========================================== */
    /* Parse Frames (INTERLEAVED format)          */
    /* Entry (36 bytes) followed by Data (size)   */
    /* ========================================== */

    /* Some datasets use an interleaved layout (entry->data->entry->data...),
     * others store a full index table then pack all frame data afterwards.
     * Try interleaved first; if it doesn't fit, fall back to the table layout. */
    int pr = parse_frames_interleaved(base, end, img);
    if (pr != IMG_SUCCESS) {
        memset(img->frames, 0, img->frame_count * sizeof(IMGFrame));
        pr = parse_frames_index_table(base, end, img);
        if (pr != IMG_SUCCESS) {
            IMG_Free(img);
            return pr;
        }
    }

    /* Initialize default grayscale palette for indexed images */
    for (int i = 0; i < 256; i++) {
        img->palette.colors[i][0] = (uint8_t)i;  /* R */
        img->palette.colors[i][1] = (uint8_t)i;  /* G */
        img->palette.colors[i][2] = (uint8_t)i;  /* B */
        img->palette.colors[i][3] = 255;          /* A */
    }
    img->palette.count = 256;
    img->has_palette = true;

    return IMG_SUCCESS;
}

void IMG_Free(IMGFile* img)
{
    if (img == NULL) {
        return;
    }

    if (img->frames != NULL) {
        free(img->frames);
        img->frames = NULL;
    }

    memset(img, 0, sizeof(IMGFile));
}

uint32_t IMG_GetFrameCount(IMGFile* img)
{
    if (img == NULL) {
        return 0;
    }
    return img->frame_count;
}

IMGFrame* IMG_GetFrame(IMGFile* img, uint32_t index)
{
    if (img == NULL || index >= img->frame_count) {
        return NULL;
    }
    return &img->frames[index];
}

/* ========================================================================== */
/* Decoding Implementation                                                     */
/* ========================================================================== */

int IMG_DecodeFrame(IMGFile* img, uint32_t index, IMGPixels* pixels)
{
    if (img == NULL || pixels == NULL || index >= img->frame_count) {
        return IMG_ERROR_INVALID_PARAM;
    }

    IMGFrame* frame = &img->frames[index];

    /* Handle link frames - follow the chain */
    int max_links = 10;  /* Prevent infinite loops */
    while (frame->is_link && frame->link_index >= 0 &&
           (uint32_t)frame->link_index < img->frame_count && max_links-- > 0) {
        frame = &img->frames[frame->link_index];
    }

    if (frame->width <= 0 || frame->height <= 0) {
        return IMG_ERROR_INVALID_FRAME;
    }

    /* Initialize output */
    memset(pixels, 0, sizeof(IMGPixels));
    pixels->width = frame->width;
    pixels->height = frame->height;
    pixels->offset_x = frame->offset_x;
    pixels->offset_y = frame->offset_y;
    pixels->data_size = frame->width * frame->height * sizeof(uint32_t);

    /* Allocate RGBA buffer */
    pixels->data = (uint32_t*)calloc(frame->width * frame->height, sizeof(uint32_t));
    if (pixels->data == NULL) {
        return IMG_ERROR_MEMORY;
    }

    /* Locate frame data in raw buffer (data_offset is now ABSOLUTE) */
    if (frame->data_offset + frame->data_size > img->raw_size) {
        /* Data out of bounds - return magenta error pattern */
        for (int i = 0; i < frame->width * frame->height; i++) {
            pixels->data[i] = 0xFFFF00FF;  /* Magenta = error */
        }
        return IMG_SUCCESS;
    }

    const uint8_t* frame_data = img->raw_data + frame->data_offset;

    /* Decode the pixel data */
    if (frame->data_size > 0) {
        uint8_t* decompressed = NULL;
        const uint8_t* pixel_data = frame_data;
        size_t pixel_data_size = frame->data_size;
        bool decode_success = false;

        /* Decompress if needed (compress=6 is zlib) */
        if (frame->compress == IMG_COMPRESS_ZLIB && frame->raw_size > 0) {
            /* Check for zlib header (78 9C, 78 01, or 78 DA) */
            if (frame->data_size >= 2 && frame_data[0] == 0x78) {
                decompressed = (uint8_t*)malloc(frame->raw_size);
                if (decompressed != NULL) {
                    if (decompress_zlib(frame_data, frame->data_size,
                                       decompressed, frame->raw_size) == IMG_SUCCESS) {
                        pixel_data = decompressed;
                        pixel_data_size = frame->raw_size;
                        decode_success = true;
                    }
                }
            }
        } else if (frame->compress == IMG_COMPRESS_NONE) {
            /* Raw data - no decompression needed */
            pixel_data_size = frame->data_size;
            decode_success = true;
        }

        /* Convert based on color format */
        if (decode_success || frame->compress == 0) {
            switch (frame->color_format) {
                case IMG_COLOR_INDEX8:
                    if (pixel_data_size >= (size_t)(frame->width * frame->height)) {
                        convert_index8_to_rgba(pixel_data, pixels->data,
                                              frame->width, frame->height,
                                              &img->palette);
                    }
                    break;

                case IMG_COLOR_ARGB1555:
                    if (pixel_data_size >= (size_t)(frame->width * frame->height * 2)) {
                        convert_argb1555_to_rgba((const uint16_t*)pixel_data,
                                                pixels->data,
                                                frame->width, frame->height);
                    }
                    break;

                case IMG_COLOR_ARGB4444:
                    if (pixel_data_size >= (size_t)(frame->width * frame->height * 2)) {
                        convert_argb4444_to_rgba((const uint16_t*)pixel_data,
                                                pixels->data,
                                                frame->width, frame->height);
                    }
                    break;

                case IMG_COLOR_ARGB8888:
                    if (pixel_data_size >= (size_t)(frame->width * frame->height * 4)) {
                        convert_argb8888_to_rgba((const uint32_t*)pixel_data,
                                                pixels->data,
                                                frame->width, frame->height);
                    }
                    break;

                case IMG_COLOR_DDS_DXT1:
                    {
                        /* DXT1: 8 bytes per 4x4 block */
                        int blocks_x = (frame->width + 3) / 4;
                        int blocks_y = (frame->height + 3) / 4;
                        size_t expected_size = (size_t)(blocks_x * blocks_y * 8);
                        if (pixel_data_size >= expected_size) {
                            decode_dxt1(pixel_data, pixels->data,
                                       frame->width, frame->height);
                        }
                    }
                    break;

                case IMG_COLOR_DDS_DXT3:
                    {
                        /* DXT3: 16 bytes per 4x4 block */
                        int blocks_x = (frame->width + 3) / 4;
                        int blocks_y = (frame->height + 3) / 4;
                        size_t expected_size = (size_t)(blocks_x * blocks_y * 16);
                        if (pixel_data_size >= expected_size) {
                            decode_dxt3(pixel_data, pixels->data,
                                       frame->width, frame->height);
                        }
                    }
                    break;

                case IMG_COLOR_DDS_DXT5:
                    {
                        /* DXT5: 16 bytes per 4x4 block */
                        int blocks_x = (frame->width + 3) / 4;
                        int blocks_y = (frame->height + 3) / 4;
                        size_t expected_size = (size_t)(blocks_x * blocks_y * 16);
                        if (pixel_data_size >= expected_size) {
                            decode_dxt5(pixel_data, pixels->data,
                                       frame->width, frame->height);
                        }
                    }
                    break;

                default:
                    /* Unknown format - create gray pattern */
                    for (int i = 0; i < frame->width * frame->height; i++) {
                        pixels->data[i] = 0xFF888888;
                    }
                    break;
            }
        } else {
            /* Decompression failed - create error pattern */
            for (int y = 0; y < frame->height; y++) {
                for (int x = 0; x < frame->width; x++) {
                    int idx = y * frame->width + x;
                    if ((x / 8 + y / 8) % 2 == 0) {
                        pixels->data[idx] = 0xFFFF0000;  /* Red */
                    } else {
                        pixels->data[idx] = 0xFF000000;  /* Black */
                    }
                }
            }
        }

        if (decompressed != NULL) {
            free(decompressed);
        }
    }

    return IMG_SUCCESS;
}

void IMG_FreePixels(IMGPixels* pixels)
{
    if (pixels == NULL) {
        return;
    }

    if (pixels->data != NULL) {
        free(pixels->data);
        pixels->data = NULL;
    }

    memset(pixels, 0, sizeof(IMGPixels));
}

#ifndef IMG_NO_SDL
SDL_Surface* IMG_DecodeToSurface(IMGFile* img, uint32_t index)
{
    IMGPixels pixels;
    if (IMG_DecodeFrame(img, index, &pixels) != IMG_SUCCESS) {
        return NULL;
    }

    /* Create SDL surface from RGBA data */
    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormatFrom(
        pixels.data,
        pixels.width,
        pixels.height,
        32,
        pixels.width * 4,
        SDL_PIXELFORMAT_RGBA8888
    );

    if (surface == NULL) {
        IMG_FreePixels(&pixels);
        return NULL;
    }

    /* We need to copy the data since SDL_FreeSurface won't free our buffer */
    SDL_Surface* copy = SDL_ConvertSurface(surface, surface->format, 0);
    SDL_FreeSurface(surface);
    IMG_FreePixels(&pixels);

    return copy;
}

SDL_Texture* IMG_CreateTexture(SDL_Renderer* renderer, IMGFile* img, uint32_t index)
{
    SDL_Surface* surface = IMG_DecodeToSurface(img, index);
    if (surface == NULL) {
        return NULL;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    return texture;
}

/* ========================================================================== */
/* Sprite Sheet Implementation                                                 */
/* ========================================================================== */

IMGSpriteSheet* IMG_CreateSpriteSheet(SDL_Renderer* renderer, IMGFile* img)
{
    if (renderer == NULL || img == NULL || img->frame_count == 0) {
        return NULL;
    }

    IMGSpriteSheet* sheet = (IMGSpriteSheet*)calloc(1, sizeof(IMGSpriteSheet));
    if (sheet == NULL) {
        return NULL;
    }

    sheet->frame_count = img->frame_count;

    /* Calculate sheet dimensions (simple row layout) */
    int cols = (int)SDL_ceil(SDL_sqrt((double)img->frame_count));
    int rows = (img->frame_count + cols - 1) / cols;
    sheet->texture_width = cols * img->max_width;
    sheet->texture_height = rows * img->max_height;

    /* Allocate frame rectangles */
    sheet->frame_rects = calloc(img->frame_count, sizeof(*sheet->frame_rects));
    if (sheet->frame_rects == NULL) {
        free(sheet);
        return NULL;
    }

    /* Create combined surface */
    SDL_Surface* combined = SDL_CreateRGBSurfaceWithFormat(
        0, sheet->texture_width, sheet->texture_height, 32,
        SDL_PIXELFORMAT_RGBA8888
    );
    if (combined == NULL) {
        free(sheet->frame_rects);
        free(sheet);
        return NULL;
    }

    /* Clear to transparent */
    SDL_FillRect(combined, NULL, 0);

    /* Decode and blit each frame */
    for (uint32_t i = 0; i < img->frame_count; i++) {
        IMGFrame* frame = &img->frames[i];
        int col = i % cols;
        int row = i / cols;

        sheet->frame_rects[i].x = col * img->max_width;
        sheet->frame_rects[i].y = row * img->max_height;
        sheet->frame_rects[i].w = frame->width;
        sheet->frame_rects[i].h = frame->height;
        sheet->frame_rects[i].offset_x = frame->offset_x;
        sheet->frame_rects[i].offset_y = frame->offset_y;

        SDL_Surface* frame_surface = IMG_DecodeToSurface(img, i);
        if (frame_surface != NULL) {
            SDL_Rect dst = {
                sheet->frame_rects[i].x,
                sheet->frame_rects[i].y,
                frame->width,
                frame->height
            };
            SDL_BlitSurface(frame_surface, NULL, combined, &dst);
            SDL_FreeSurface(frame_surface);
        }
    }

    /* Create texture */
    sheet->texture = SDL_CreateTextureFromSurface(renderer, combined);
    SDL_FreeSurface(combined);

    if (sheet->texture == NULL) {
        free(sheet->frame_rects);
        free(sheet);
        return NULL;
    }

    return sheet;
}

void IMG_FreeSpriteSheet(IMGSpriteSheet* sheet)
{
    if (sheet == NULL) {
        return;
    }

    if (sheet->texture != NULL) {
        SDL_DestroyTexture(sheet->texture);
    }

    if (sheet->frame_rects != NULL) {
        free(sheet->frame_rects);
    }

    free(sheet);
}
#else
SDL_Surface* IMG_DecodeToSurface(IMGFile* img, uint32_t index)
{
    (void)img;
    (void)index;
    return NULL;
}

SDL_Texture* IMG_CreateTexture(SDL_Renderer* renderer, IMGFile* img, uint32_t index)
{
    (void)renderer;
    (void)img;
    (void)index;
    return NULL;
}

IMGSpriteSheet* IMG_CreateSpriteSheet(SDL_Renderer* renderer, IMGFile* img)
{
    (void)renderer;
    (void)img;
    return NULL;
}

void IMG_FreeSpriteSheet(IMGSpriteSheet* sheet)
{
    (void)sheet;
}
#endif

/* ========================================================================== */
/* Utility Functions                                                           */
/* ========================================================================== */

const char* IMG_GetColorFormatName(uint32_t format)
{
    switch (format) {
        case IMG_COLOR_ARGB1555: return "ARGB1555";
        case IMG_COLOR_ARGB4444: return "ARGB4444";
        case IMG_COLOR_ARGB8888: return "ARGB8888";
        case IMG_COLOR_INDEX8:   return "INDEX8";
        case IMG_COLOR_DDS_DXT1: return "DXT1";
        case IMG_COLOR_DDS_DXT3: return "DXT3";
        case IMG_COLOR_DDS_DXT5: return "DXT5";
        default: return "UNKNOWN";
    }
}

bool IMG_IsVersionSupported(uint32_t version)
{
    return (version >= 1 && version <= 6);
}

void IMG_PrintInfo(IMGFile* img)
{
    if (img == NULL) {
        printf("IMG: (null)\n");
        return;
    }

    printf("IMG Info:\n");
    printf("  Version: %u\n", img->version);
    printf("  Frames: %u\n", img->frame_count);
    printf("  Max Size: %dx%d\n", img->max_width, img->max_height);
    printf("  Has Palette: %s\n", img->has_palette ? "yes" : "no");

    for (uint32_t i = 0; i < img->frame_count && i < 10; i++) {
        IMGFrame* f = &img->frames[i];
        printf("  Frame %u: %dx%d, format=%s, compress=%u, size=%u\n",
               i, f->width, f->height,
               IMG_GetColorFormatName(f->color_format),
               f->compress, f->data_size);
    }
    if (img->frame_count > 10) {
        printf("  ... and %u more frames\n", img->frame_count - 10);
    }
}
