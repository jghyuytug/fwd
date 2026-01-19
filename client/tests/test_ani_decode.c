/**
 * @file test_ani_decode.c
 * @brief PVF ANI -> NPK IMG -> decode frames to PPM (no SDL)
 *
 * Usage:
 *   test_ani_decode <Script.pvf> <ImagePacks2_dir> <ani_path_in_pvf> [max_frames]
 *
 * Output:
 *   Writes PPM files into ./output/
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#endif

#include "../../include/pvf/pvf.h"
#include "../include/resource/ani_loader.h"
#include "../include/resource/npk_parser.h"
#include "../include/resource/img_decoder.h"

static void path_join(char* out, size_t out_size, const char* dir, const char* name)
{
    if (out == NULL || out_size == 0) return;
    out[0] = '\0';
    if (dir == NULL) dir = "";
    if (name == NULL) name = "";

    snprintf(out, out_size, "%s", dir);
    if (out[0] != '\0') {
        size_t n = strlen(out);
        if (n > 0 && out[n - 1] != '/' && out[n - 1] != '\\') {
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

static void sanitize_filename(char* s)
{
    if (s == NULL) return;
    for (; *s; s++) {
        unsigned char c = (unsigned char)*s;
        if (!(isalnum(c) || c == '.' || c == '_' || c == '-')) {
            *s = '_';
        }
    }
}

static int ensure_output_dir(void)
{
#ifdef _WIN32
    _mkdir("output");
    return 0;
#else
    return 0;
#endif
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

static const char* guess_npk_prefix(const char* img_path)
{
    if (img_path == NULL) return "";
    if (starts_with_ci(img_path, "sprite/creature/")) return "sprite_creature";
    if (starts_with_ci(img_path, "sprite/interface/")) return "sprite_interface";
    if (starts_with_ci(img_path, "sprite/character/")) return "sprite_character";
    return "";
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

    /* Split after "sprite/" */
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
        /* Try lowercase extension */
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
    char pattern[512];
    char dir_win[512];
    const char* prefix;

    if (npk_dir == NULL || img_path == NULL || out_archive == NULL || out_entry == NULL) {
        return NPK_ERROR_INVALID_PARAM;
    }

    /* Fast path: derive NPK name from img_path (SkillTreeViewer-style mapping) */
    int direct = try_open_npk_by_img_path(npk_dir, img_path, out_archive, out_entry);
    if (direct == NPK_SUCCESS) {
        return NPK_SUCCESS;
    }

    snprintf(dir_win, sizeof(dir_win), "%s", npk_dir);
    for (char* p = dir_win; *p; p++) {
        if (*p == '/') *p = '\\';
    }

    prefix = guess_npk_prefix(img_path);

    /* Try lowercase and uppercase extensions */
    const char* exts[] = {"npk", "NPK"};
    for (int ei = 0; ei < 2; ei++) {
        snprintf(pattern, sizeof(pattern), "%s\\%s*.%s", dir_win, prefix, exts[ei]);

        WIN32_FIND_DATAA ffd;
        HANDLE h = FindFirstFileA(pattern, &ffd);
        if (h == INVALID_HANDLE_VALUE) {
            continue;
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
    }

    /* If prefix didn't work, fall back to full scan */
    if (prefix[0] != '\0') {
        prefix = "";
        for (int ei = 0; ei < 2; ei++) {
            snprintf(pattern, sizeof(pattern), "%s\\%s*.%s", dir_win, prefix, exts[ei]);

            WIN32_FIND_DATAA ffd;
            HANDLE h = FindFirstFileA(pattern, &ffd);
            if (h == INVALID_HANDLE_VALUE) {
                continue;
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
        }
    }

    return NPK_ERROR_NOT_FOUND;
#else
    (void)npk_dir;
    (void)img_path;
    (void)out_archive;
    (void)out_entry;
    return NPK_ERROR_NOT_FOUND;
#endif
}

static int save_ppm(const char* path, const uint32_t* rgba, int width, int height)
{
    FILE* f = fopen(path, "wb");
    if (!f) return -1;

    fprintf(f, "P6\n%d %d\n255\n", width, height);
    for (int i = 0; i < width * height; i++) {
        uint32_t p = rgba[i];
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

int main(int argc, char* argv[])
{
    const char* pvf_path;
    const char* npk_dir;
    const char* ani_path;
    int max_frames = 10;

    if (argc < 4) {
        fprintf(stderr, "Usage: %s <Script.pvf> <ImagePacks2_dir> <ani_path_in_pvf> [max_frames]\n", argv[0]);
        return 2;
    }

    pvf_path = argv[1];
    npk_dir = argv[2];
    ani_path = argv[3];
    if (argc >= 5) {
        max_frames = atoi(argv[4]);
        if (max_frames <= 0) max_frames = 10;
    }

    ensure_output_dir();

    printf("=== ANI Decode Test (no SDL) ===\n");
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

    printf("[OK] ANI parsed: actions=%d\n", def->action_count);

    int decoded = 0;
    for (int ai = 0; ai < def->action_count; ai++) {
        ANIAction* action = &def->actions[ai];
        int to_decode = action->frame_count;
        if (to_decode > max_frames) to_decode = max_frames;

        if (action->frame_count <= 0) continue;

        /* For compact PVF .ani, action->img_path should already be normalized to sprite/... */
        char base_img_path[ANI_MAX_IMG_PATH];
        snprintf(base_img_path, sizeof(base_img_path), "%s", action->img_path);
        normalize_slashes(base_img_path);
        if (base_img_path[0] == '\0') {
            printf("[WARN] Action[%d] empty img_path\n", ai);
            continue;
        }

        NPKArchive archive;
        NPKFileEntry* entry = NULL;
        int fr = find_img_in_npk_dir(npk_dir, base_img_path, &archive, &entry);
        if (fr != NPK_SUCCESS || entry == NULL) {
            printf("[FAIL] Action[%d] cannot locate IMG: %s\n", ai, base_img_path);
            continue;
        }

        printf("\n--- Action[%d] '%s' frames=%d IMG='%s' (size=%u) ---\n",
               ai, action->name, action->frame_count, entry->name, entry->size);

        uint8_t* img_data = (uint8_t*)malloc(entry->size);
        if (img_data == NULL) {
            printf("[FAIL] malloc IMG buffer\n");
            NPK_Close(&archive);
            continue;
        }

        if (NPK_ReadFile(&archive, entry, img_data, entry->size) != NPK_SUCCESS) {
            printf("[FAIL] NPK_ReadFile\n");
            free(img_data);
            NPK_Close(&archive);
            continue;
        }

        IMGFile img;
        if (IMG_Parse(img_data, entry->size, &img) != IMG_SUCCESS) {
            printf("[FAIL] IMG_Parse\n");
            free(img_data);
            NPK_Close(&archive);
            continue;
        }

        for (int fi = 0; fi < to_decode; fi++) {
            ANIFrame* frame = &action->frames[fi];
            int frame_index = frame->img_index;
            if (frame_index < 0) frame_index = 0;

            IMGPixels pixels;
            int dr = IMG_DecodeFrame(&img, (uint32_t)frame_index, &pixels);
            if (dr != IMG_SUCCESS) {
                printf("  [FAIL] decode frame=%d img_index=%d err=%d\n", fi, frame->img_index, dr);
                continue;
            }

            char namebuf[512];
            snprintf(namebuf, sizeof(namebuf), "%s_action%d_f%03d_idx%d.ppm", ani_path, ai, fi, frame->img_index);
            sanitize_filename(namebuf);

            char outpath[600];
            path_join(outpath, sizeof(outpath), "output", namebuf);

            if (save_ppm(outpath, pixels.data, pixels.width, pixels.height) == 0) {
                printf("  [OK] %s (%dx%d offset=%d,%d dur=%dms)\n",
                       outpath, pixels.width, pixels.height,
                       pixels.offset_x, pixels.offset_y, frame->duration);
                decoded++;
            } else {
                printf("  [FAIL] save %s\n", outpath);
            }

            IMG_FreePixels(&pixels);
        }

        IMG_Free(&img);
        free(img_data);
        NPK_Close(&archive);
    }

    ANI_FreeDefinition(def);
    PackSet_Close(&pack);

    printf("\nDecoded frames: %d\n", decoded);
    return (decoded > 0) ? 0 : 1;
}
