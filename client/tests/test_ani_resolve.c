/**
 * @file test_ani_resolve.c
 * @brief Resolve PVF ANI -> NPK IMG references (no rendering)
 *
 * Purpose:
 * - Validate that ANI files inside Script.pvf can be parsed
 * - Validate that referenced NPK archives and IMG paths exist locally
 *
 * Usage:
 *   test_ani_resolve <Script.pvf> <ImagePacks2_dir> <ani_path_in_pvf> [max_frames]
 *
 * Example:
 *   test_ani_resolve ./Script.pvf C:/DNF/imagepacks2 creature/xxx/animation/run.ani 60
 *
 * Notes:
 * - This tool does NOT decode IMG pixels (so it does not require zlib).
 * - It only checks existence and reads small metadata from NPK index.
 *
 * @date 2026-01-15
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include "../../include/pvf/pvf.h"
#include "../include/resource/ani_loader.h"
#include "../include/resource/npk_parser.h"

static void path_join(char* out, size_t out_size, const char* dir, const char* name)
{
    size_t n = 0;
    if (out == NULL || out_size == 0) return;
    out[0] = '\0';

    if (dir == NULL) dir = "";
    if (name == NULL) name = "";

    n = strlen(dir);
    if (n > 0) {
        snprintf(out, out_size, "%s", dir);
        if (out[strlen(out) - 1] != '/' && out[strlen(out) - 1] != '\\') {
            strncat(out, "/", out_size - strlen(out) - 1);
        }
    }
    strncat(out, name, out_size - strlen(out) - 1);
}

static void normalize_slashes(char* s)
{
    if (s == NULL) return;
    for (; *s; s++) {
        if (*s == '\\') *s = '/';
    }
}

static void normalize_backslashes(char* s)
{
    if (s == NULL) return;
    for (; *s; s++) {
        if (*s == '/') *s = '\\';
    }
}

static int starts_with_ci(const char* s, const char* prefix)
{
    size_t i = 0;
    if (s == NULL || prefix == NULL) return 0;
    while (prefix[i] != '\0') {
        unsigned char a = (unsigned char)s[i];
        unsigned char b = (unsigned char)prefix[i];
        if (a == '\0') return 0;
        if (tolower(a) != tolower(b)) return 0;
        i++;
    }
    return 1;
}

static int try_open_npk(const char* npk_dir, const char* npk_name, NPKArchive* out)
{
    char full[512];
    char alt[512];

    if (npk_dir == NULL || npk_name == NULL || out == NULL) {
        return NPK_ERROR_INVALID_PARAM;
    }

    path_join(full, sizeof(full), npk_dir, npk_name);
    if (NPK_Open(full, out) == NPK_SUCCESS) {
        return NPK_SUCCESS;
    }

    /* Try uppercase extension */
    snprintf(alt, sizeof(alt), "%s", npk_name);
    {
        size_t len = strlen(alt);
        if (len >= 4) {
            if (alt[len - 4] == '.' &&
                (alt[len - 3] == 'n' || alt[len - 3] == 'N') &&
                (alt[len - 2] == 'p' || alt[len - 2] == 'P') &&
                (alt[len - 1] == 'k' || alt[len - 1] == 'K')) {
                alt[len - 3] = 'N';
                alt[len - 2] = 'P';
                alt[len - 1] = 'K';
            }
        }
    }
    path_join(full, sizeof(full), npk_dir, alt);
    if (NPK_Open(full, out) == NPK_SUCCESS) {
        return NPK_SUCCESS;
    }

    /* Try lowercase extension */
    snprintf(alt, sizeof(alt), "%s", npk_name);
    {
        size_t len = strlen(alt);
        if (len >= 4) {
            if (alt[len - 4] == '.' &&
                (alt[len - 3] == 'n' || alt[len - 3] == 'N') &&
                (alt[len - 2] == 'p' || alt[len - 2] == 'P') &&
                (alt[len - 1] == 'k' || alt[len - 1] == 'K')) {
                alt[len - 3] = 'n';
                alt[len - 2] = 'p';
                alt[len - 1] = 'k';
            }
        }
    }
    path_join(full, sizeof(full), npk_dir, alt);
    return NPK_Open(full, out);
}

static int try_open_npk_by_img_path(const char* npk_dir, const char* img_path,
                                    NPKArchive* out_archive, NPKFileEntry** out_entry)
{
    char tmp[512];
    char* parts[8] = {0};
    int part_count = 0;

    if (npk_dir == NULL || img_path == NULL || out_archive == NULL || out_entry == NULL) {
        return NPK_ERROR_INVALID_PARAM;
    }

    snprintf(tmp, sizeof(tmp), "%s", img_path);
    normalize_slashes(tmp);

    if (!starts_with_ci(tmp, "sprite/")) {
        return NPK_ERROR_NOT_FOUND;
    }

    char* p = tmp + 7;
    while (*p && part_count < 8) {
        parts[part_count++] = p;
        char* slash = strchr(p, '/');
        if (slash == NULL) break;
        *slash = '\0';
        p = slash + 1;
    }

    if (part_count < 1) {
        return NPK_ERROR_NOT_FOUND;
    }

    char npk_name[128];
    if (part_count >= 3) {
        snprintf(npk_name, sizeof(npk_name), "sprite_%s_%s_%s.NPK", parts[0], parts[1], parts[2]);
    } else if (part_count >= 2) {
        snprintf(npk_name, sizeof(npk_name), "sprite_%s_%s.NPK", parts[0], parts[1]);
    } else {
        snprintf(npk_name, sizeof(npk_name), "sprite_%s.NPK", parts[0]);
    }

    char full[512];
    path_join(full, sizeof(full), npk_dir, npk_name);
    NPKArchive archive;
    if (NPK_Open(full, &archive) != NPK_SUCCESS) {
        size_t len = strlen(full);
        if (len >= 4) {
            full[len - 3] = 'n';
            full[len - 2] = 'p';
            full[len - 1] = 'k';
        }
        if (NPK_Open(full, &archive) != NPK_SUCCESS) {
            return NPK_ERROR_NOT_FOUND;
        }
    }

    NPKFileEntry* e = NPK_FindFile(&archive, img_path);
    if (e == NULL) {
        NPK_Close(&archive);
        return NPK_ERROR_NOT_FOUND;
    }

    *out_archive = archive;
    *out_entry = e;
    return NPK_SUCCESS;
}

static int find_img_in_npk_dir(const char* npk_dir, const char* img_path,
                               NPKArchive* out_archive, NPKFileEntry** out_entry)
{
#ifdef _WIN32
    char dir_win[512];
    char pattern[512];
    const char* prefix = "";

    if (npk_dir == NULL || img_path == NULL || out_archive == NULL || out_entry == NULL) {
        return NPK_ERROR_INVALID_PARAM;
    }

    /* Fast path: derive NPK name from img_path */
    int direct = try_open_npk_by_img_path(npk_dir, img_path, out_archive, out_entry);
    if (direct == NPK_SUCCESS) {
        return NPK_SUCCESS;
    }

    snprintf(dir_win, sizeof(dir_win), "%s", npk_dir);
    normalize_backslashes(dir_win);

    if (starts_with_ci(img_path, "creature/") || starts_with_ci(img_path, "passiveobject/creature/")) {
        prefix = "sprite_creature";
    }

    snprintf(pattern, sizeof(pattern), "%s\\%s*.npk", dir_win, prefix);

    WIN32_FIND_DATAA ffd;
    HANDLE h = FindFirstFileA(pattern, &ffd);
    if (h == INVALID_HANDLE_VALUE) {
        return NPK_ERROR_NOT_FOUND;
    }

    do {
        if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            continue;
        }

        char full[512];
        path_join(full, sizeof(full), npk_dir, ffd.cFileName);

        NPKArchive archive;
        int o = NPK_Open(full, &archive);
        if (o != NPK_SUCCESS) {
            continue;
        }

        NPKFileEntry* e = NPK_FindFile(&archive, img_path);
        if (e != NULL) {
            *out_archive = archive; /* keep open */
            *out_entry = e;
            FindClose(h);
            return NPK_SUCCESS;
        }

        NPK_Close(&archive);
    } while (FindNextFileA(h, &ffd));

    FindClose(h);
    return NPK_ERROR_NOT_FOUND;
#else
    (void)npk_dir;
    (void)img_path;
    (void)out_archive;
    (void)out_entry;
    return NPK_ERROR_NOT_FOUND;
#endif
}

int main(int argc, char* argv[])
{
    const char* pvf_path;
    const char* npk_dir;
    const char* ani_path;
    int max_frames = 80;

    if (argc < 4) {
        fprintf(stderr,
                "Usage: %s <Script.pvf> <ImagePacks2_dir> <ani_path_in_pvf> [max_frames]\n",
                argv[0]);
        return 2;
    }

    pvf_path = argv[1];
    npk_dir = argv[2];
    ani_path = argv[3];
    if (argc >= 5) {
        max_frames = atoi(argv[4]);
        if (max_frames <= 0) max_frames = 80;
    }

    printf("=== ANI Resolve Test ===\n");
    printf("PVF: %s\n", pvf_path);
    printf("NPK dir: %s\n", npk_dir);
    printf("ANI: %s\n", ani_path);
    printf("Max frames/action: %d\n\n", max_frames);

    PVF_Initialize();
    NPK_Initialize();

    PackSetInternal pack;
    if (PackSet_Open(pvf_path, &pack) != PVF_SUCCESS) {
        fprintf(stderr, "[FAIL] PackSet_Open failed\n");
        return 1;
    }

    ANIDefinition* def = NULL;
    int ret = ANI_LoadFromPVF(&pack, ani_path, &def);
    if (ret != ANI_SUCCESS || def == NULL) {
        fprintf(stderr, "[FAIL] ANI_LoadFromPVF failed: %d\n", ret);
        PackSet_Close(&pack);
        return 1;
    }

    printf("[OK] ANI parsed: actions=%d, default_action=%d\n",
           def->action_count, def->default_action);

    int total_missing = 0;
    int total_checked = 0;

    for (int ai = 0; ai < def->action_count; ai++) {
        ANIAction* action = &def->actions[ai];

        printf("\n--- Action[%d] name='%s' id=%d frames=%d npk='%s' ---\n",
               ai, action->name, action->id, action->frame_count, action->npk_name);

        char img_path_buf[ANI_MAX_IMG_PATH];
        snprintf(img_path_buf, sizeof(img_path_buf), "%s", action->img_path);
        normalize_slashes(img_path_buf);

        if (img_path_buf[0] == '\0') {
            printf("[FAIL] Action[%d] has empty img_path\n", ai);
            total_missing++;
            continue;
        }

        NPKArchive archive;
        NPKFileEntry* img_entry = NULL;

        if (action->npk_name[0] != '\0') {
            int npk_open = try_open_npk(npk_dir, action->npk_name, &archive);
            if (npk_open != NPK_SUCCESS) {
                printf("[FAIL] NPK open failed: %s (err=%d)\n", action->npk_name, npk_open);
                total_missing++;
                continue;
            }
            img_entry = NPK_FindFile(&archive, img_path_buf);
        } else {
            int fr = find_img_in_npk_dir(npk_dir, img_path_buf, &archive, &img_entry);
            if (fr != NPK_SUCCESS) {
                printf("[FAIL] Could not locate IMG in any NPK: '%s' (err=%d)\n", img_path_buf, fr);
                total_missing++;
                continue;
            }
        }

        if (img_entry == NULL) {
            printf("[MISS] IMG not found in NPK: '%s'\n", img_path_buf);
            total_missing++;
            NPK_Close(&archive);
            continue;
        }

        printf("[OK] IMG '%s' size=%u\n", img_entry->name, img_entry->size);

        /* Read once to validate data access */
        if (img_entry->size > 0 && img_entry->size < 50u * 1024u * 1024u) {
            void* buf = malloc(img_entry->size);
            if (buf != NULL) {
                int rr = NPK_ReadFile(&archive, img_entry, buf, img_entry->size);
                if (rr == NPK_SUCCESS) {
                    const unsigned char* b = (const unsigned char*)buf;
                    const char* detected = "unknown";
                    if (img_entry->size >= 18) {
                        if (strncmp((const char*)b, "Neople Img File", 15) == 0 ||
                            strncmp((const char*)b, "Neople Image File", 17) == 0) {
                            detected = "IMG";
                        }
                    }
                    printf("       [READ] ok, type=%s, first8=%02X %02X %02X %02X %02X %02X %02X %02X\n",
                           detected,
                           b[0], b[1], b[2], b[3], b[4], b[5], b[6], b[7]);
                } else {
                    printf("       [READ] failed err=%d\n", rr);
                }
                free(buf);
            } else {
                printf("       [READ] skipped (malloc failed)\n");
            }
        }

        int to_check = action->frame_count;
        if (to_check > max_frames) to_check = max_frames;

        for (int fi = 0; fi < to_check; fi++) {
            ANIFrame* frame = &action->frames[fi];
            printf("  frame=%d img_index=%d duration=%d offset=(%d,%d)\n",
                   fi, frame->img_index, frame->duration, frame->offset.x, frame->offset.y);
            total_checked++;
        }

        NPK_Close(&archive);
    }

    printf("\n=== Summary ===\n");
    printf("Checked: %d\n", total_checked);
    printf("Missing: %d\n", total_missing);

    ANI_FreeDefinition(def);
    PackSet_Close(&pack);

    return (total_missing == 0) ? 0 : 1;
}
