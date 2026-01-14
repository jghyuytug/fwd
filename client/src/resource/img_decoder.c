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

#ifdef _WIN32
#include <SDL.h>
#else
#include <SDL2/SDL.h>
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
/* Parsing Implementation                                                      */
/* ========================================================================== */

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

    /* Frame data starts at offset 32 */
    p = base + IMG_HEADER_SIZE;

    for (uint32_t i = 0; i < img->frame_count; i++) {
        IMGFrame* frame = &img->frames[i];
        frame->link_index = -1;

        /* Check bounds for entry */
        if (p + IMG_ENTRY_SIZE > end) {
            IMG_Free(img);
            return IMG_ERROR_INVALID_MAGIC;
        }

        /* Read entry (36 bytes) */
        frame->color_format = read_u32(&p);
        frame->compress = read_u32(&p);
        frame->width = read_i32(&p);
        frame->height = read_i32(&p);
        frame->data_size = read_u32(&p);
        frame->offset_x = read_i32(&p);
        frame->offset_y = read_i32(&p);
        frame->max_width = read_i32(&p);
        frame->max_height = read_i32(&p);

        /* Check for link frame (format=0, compress=0, size points to target) */
        if (frame->color_format == 0 && frame->compress == 0 &&
            frame->width == 0 && frame->height == 0) {
            /* Link frame - data_size contains target frame index */
            frame->is_link = true;
            frame->link_index = (int)frame->data_size;
            frame->data_size = 0;
        } else {
            frame->is_link = false;
        }

        /* Record data offset (ABSOLUTE position in IMG data) */
        /* Data immediately follows this entry */
        frame->data_offset = (uint32_t)(p - base);

        /* Calculate raw (decompressed) size based on color format */
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

        /* Advance past the data to next entry (INTERLEAVED) */
        if (!frame->is_link && frame->data_size > 0) {
            if (p + frame->data_size > end) {
                /* Data extends beyond file - truncate */
                frame->data_size = (uint32_t)(end - p);
            }
            p += frame->data_size;
        }

        /* Track max dimensions */
        if (frame->width > img->max_width) {
            img->max_width = frame->width;
        }
        if (frame->height > img->max_height) {
            img->max_height = frame->height;
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
