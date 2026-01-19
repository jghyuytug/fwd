/**
 * @file list_pvf_ani.c
 * @brief List .ani files from Script.pvf index (no extraction)
 *
 * Usage:
 *   list_pvf_ani <Script.pvf> [contains] [limit]
 *
 * Example:
 *   list_pvf_ani ./Script.pvf creature/ 50
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../../include/pvf/pvf.h"

static int endswith_ani(const char* s)
{
    size_t n;
    if (s == NULL) return 0;
    n = strlen(s);
    if (n < 4) return 0;
    return (tolower((unsigned char)s[n - 4]) == '.' &&
            tolower((unsigned char)s[n - 3]) == 'a' &&
            tolower((unsigned char)s[n - 2]) == 'n' &&
            tolower((unsigned char)s[n - 1]) == 'i');
}

static int contains_substr(const char* s, const char* needle)
{
    if (needle == NULL || needle[0] == '\0') return 1;
    if (s == NULL) return 0;
    return strstr(s, needle) != NULL;
}

int main(int argc, char* argv[])
{
    const char* pvf_path;
    const char* contains = "";
    int limit = 200;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <Script.pvf> [contains] [limit]\n", argv[0]);
        return 2;
    }

    pvf_path = argv[1];
    if (argc >= 3) contains = argv[2];
    if (argc >= 4) {
        limit = atoi(argv[3]);
        if (limit <= 0) limit = 200;
    }

    PVF_Initialize();

    PackSetInternal pack;
    if (PackSet_Open(pvf_path, &pack) != PVF_SUCCESS) {
        fprintf(stderr, "[FAIL] PackSet_Open failed\n");
        return 1;
    }

    int printed = 0;
    int matched = 0;
    for (uint32_t i = 0; i < pack.header.index_count; i++) {
        const char* name = pack.indexes[i].name;
        if (!endswith_ani(name)) continue;
        if (!contains_substr(name, contains)) continue;

        matched++;
        if (printed < limit) {
            printf("%s\n", name);
            printed++;
        }
    }

    printf("\nTotal .ani matched: %d\n", matched);
    printf("Printed: %d (limit=%d)\n", printed, limit);

    PackSet_Close(&pack);
    return 0;
}

