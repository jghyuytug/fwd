/**
 * @file test_img_decoder.c
 * @brief Standalone test for NPK/IMG parsing
 *
 * Compile:
 *   gcc -o test_img_decoder test_img_decoder.c \
 *       ../src/resource/npk_parser.c \
 *       ../src/resource/img_decoder.c \
 *       -I../include -lz
 *
 * Run:
 *   ./test_img_decoder /path/to/sprite_character.NPK
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "resource/npk_parser.h"
#include "resource/img_decoder.h"

/* Write RGBA data as PPM */
static int save_ppm(const char* path, uint32_t* rgba, int width, int height)
{
    FILE* f = fopen(path, "wb");
    if (!f) return -1;

    fprintf(f, "P6\n%d %d\n255\n", width, height);
    for (int i = 0; i < width * height; i++) {
        uint32_t p = rgba[i];
        /* RGBA format: R in high byte for our convert functions */
        uint8_t r = (p >> 24) & 0xFF;
        uint8_t g = (p >> 16) & 0xFF;
        uint8_t b = (p >> 8) & 0xFF;
        fputc(r, f);
        fputc(g, f);
        fputc(b, f);
    }
    fclose(f);
    return 0;
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        printf("Usage: %s <npk_file>\n", argv[0]);
        printf("Example: %s sprite_character.NPK\n", argv[0]);
        return 1;
    }

    const char* npk_path = argv[1];

    /* Initialize NPK decrypt key */
    NPK_Initialize();
    printf("NPK decrypt key initialized\n");

    /* Open NPK */
    NPKArchive archive;
    int result = NPK_Open(npk_path, &archive);
    if (result != NPK_SUCCESS) {
        printf("Failed to open NPK: %s (error %d)\n", npk_path, result);
        return 1;
    }

    printf("\n=== NPK: %s ===\n", npk_path);
    printf("File count: %u\n", NPK_GetFileCount(&archive));
    printf("Encrypted: %s\n", NPK_IsEncrypted(&archive) ? "yes" : "no");

    /* List first 10 files */
    printf("\nFirst 10 files:\n");
    for (uint32_t i = 0; i < 10 && i < archive.file_count; i++) {
        NPKFileEntry* entry = NPK_GetEntry(&archive, i);
        printf("  [%u] %s (offset=0x%X, size=%u)\n",
               i, entry->name, entry->offset, entry->size);
    }

    /* Find and decode first .img file */
    printf("\n=== Testing IMG decoding ===\n");

    int decoded_count = 0;
    int failed_count = 0;

    for (uint32_t i = 0; i < archive.file_count && decoded_count < 3; i++) {
        NPKFileEntry* entry = NPK_GetEntry(&archive, i);

        /* Check for .img extension */
        size_t len = strlen(entry->name);
        if (len < 4 || strcmp(entry->name + len - 4, ".img") != 0) {
            continue;
        }

        printf("\nProcessing: %s\n", entry->name);

        /* Allocate buffer and read */
        uint8_t* buffer = (uint8_t*)malloc(entry->size);
        if (!buffer) {
            printf("  Memory allocation failed\n");
            continue;
        }

        result = NPK_ReadFile(&archive, entry, buffer, entry->size);
        if (result != NPK_SUCCESS) {
            printf("  Read failed (error %d)\n", result);
            free(buffer);
            continue;
        }

        printf("  Read %u bytes from offset 0x%X\n", entry->size, entry->offset);

        /* Parse IMG */
        IMGFile img;
        result = IMG_Parse(buffer, entry->size, &img);
        if (result != IMG_SUCCESS) {
            printf("  IMG parse failed (error %d)\n", result);
            printf("  First 16 bytes: ");
            for (int j = 0; j < 16 && j < (int)entry->size; j++) {
                printf("%02X ", buffer[j]);
            }
            printf("\n");
            free(buffer);
            failed_count++;
            continue;
        }

        printf("  Version: %u\n", img.version);
        printf("  Frames: %u\n", img.frame_count);
        printf("  Max size: %dx%d\n", img.max_width, img.max_height);

        /* Show first 5 frames */
        for (uint32_t f = 0; f < 5 && f < img.frame_count; f++) {
            IMGFrame* frame = IMG_GetFrame(&img, f);
            printf("  Frame %u: %dx%d, format=%u (%s), compress=%u, size=%u\n",
                   f, frame->width, frame->height,
                   frame->color_format, IMG_GetColorFormatName(frame->color_format),
                   frame->compress, frame->data_size);
        }

        /* Decode first frame */
        if (img.frame_count > 0) {
            IMGPixels pixels;
            result = IMG_DecodeFrame(&img, 0, &pixels);
            if (result == IMG_SUCCESS) {
                printf("  Decoded frame 0: %dx%d pixels\n", pixels.width, pixels.height);

                /* Save as PPM */
                char out_path[256];
                const char* base = strrchr(entry->name, '/');
                if (base) base++; else base = entry->name;
                snprintf(out_path, sizeof(out_path), "output/%s_frame0.ppm", base);

                /* Replace special chars */
                for (char* p = out_path; *p; p++) {
                    if (*p == '(' || *p == ')') *p = '_';
                }

                if (save_ppm(out_path, pixels.data, pixels.width, pixels.height) == 0) {
                    printf("  Saved: %s\n", out_path);
                    decoded_count++;
                }

                IMG_FreePixels(&pixels);
            } else {
                printf("  Decode failed (error %d)\n", result);
                failed_count++;
            }
        }

        IMG_Free(&img);
        free(buffer);
    }

    NPK_Close(&archive);

    printf("\n=== Summary ===\n");
    printf("Decoded: %d\n", decoded_count);
    printf("Failed: %d\n", failed_count);

    return (decoded_count > 0) ? 0 : 1;
}
