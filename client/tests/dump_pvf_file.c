/**
 * @file dump_pvf_file.c
 * @brief Dump first bytes of a PVF file (for format identification)
 *
 * Usage:
 *   dump_pvf_file <Script.pvf> <path_in_pvf> [bytes]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../../include/pvf/pvf.h"

static void dump_hex(const unsigned char* b, size_t n)
{
    for (size_t i = 0; i < n; i++) {
        if (i % 16 == 0) printf("%04zu: ", i);
        printf("%02X ", b[i]);
        if (i % 16 == 15 || i + 1 == n) printf("\n");
    }
}

static void dump_text_preview(const unsigned char* b, size_t n)
{
    printf("TEXT: ");
    for (size_t i = 0; i < n; i++) {
        unsigned char c = b[i];
        if (c == '\r' || c == '\n' || c == '\t') {
            putchar(' ');
        } else if (isprint((int)c)) {
            putchar((int)c);
        } else {
            putchar('.');
        }
    }
    printf("\n");
}

int main(int argc, char* argv[])
{
    const char* pvf_path;
    const char* pvf_file;
    size_t bytes = 256;

    if (argc < 3) {
        fprintf(stderr, "Usage: %s <Script.pvf> <path_in_pvf> [bytes]\n", argv[0]);
        return 2;
    }

    pvf_path = argv[1];
    pvf_file = argv[2];
    if (argc >= 4) {
        int v = atoi(argv[3]);
        if (v > 0) bytes = (size_t)v;
    }

    PVF_Initialize();

    PackSetInternal pack;
    if (PackSet_Open(pvf_path, &pack) != PVF_SUCCESS) {
        fprintf(stderr, "[FAIL] PackSet_Open\n");
        return 1;
    }

    PVFIndexEntry* entry = PackSet_GetFile(&pack, pvf_file);
    if (entry == NULL) {
        fprintf(stderr, "[FAIL] PackSet_GetFile: not found: %s\n", pvf_file);
        PackSet_Close(&pack);
        return 1;
    }

    printf("PVF file: %s\n", pvf_file);
    printf("Size: %u\n", entry->size);

    unsigned char* buf = (unsigned char*)malloc((size_t)entry->size);
    if (buf == NULL) {
        fprintf(stderr, "[FAIL] malloc\n");
        PackSet_Close(&pack);
        return 1;
    }

    if (PackSet_ExtractFile(&pack, entry, (char*)buf, entry->size) != PVF_SUCCESS) {
        fprintf(stderr, "[FAIL] PackSet_ExtractFile\n");
        free(buf);
        PackSet_Close(&pack);
        return 1;
    }

    size_t n = bytes;
    if (n > entry->size) n = entry->size;

    printf("\nHEX (first %zu bytes):\n", n);
    dump_hex(buf, n);
    printf("\n");
    dump_text_preview(buf, (n > 160 ? 160 : n));

    free(buf);
    PackSet_Close(&pack);
    return 0;
}

