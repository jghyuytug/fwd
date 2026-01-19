#include "sim_app.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <direct.h>
#endif

#include "resource/ani_loader.h"
#include "resource/npk_parser.h"
#include "resource/img_decoder.h"

static void path_normalize_slash(char* s)
{
    if (s == NULL) return;
    for (; *s; s++) {
        if (*s == '\\') *s = '/';
    }
}

static void safe_snprintf(char* out, size_t out_size, const char* fmt, ...)
{
    va_list ap;
    if (out_size == 0) return;
    va_start(ap, fmt);
#ifdef _WIN32
    _vsnprintf(out, out_size, fmt, ap);
#else
    vsnprintf(out, out_size, fmt, ap);
#endif
    va_end(ap);
    out[out_size - 1] = '\0';
}

static void ensure_out_dir(const char* dir)
{
    if (dir == NULL || dir[0] == '\0') return;
#ifdef _WIN32
    _mkdir(dir);
#else
    /* best-effort */
    (void)dir;
#endif
}

static void sanitize_filename(char* s)
{
    if (s == NULL) return;
    for (; *s; s++) {
        char c = *s;
        if (c == '/' || c == '\\' || c == ':' || c == '*' || c == '?' ||
            c == '"' || c == '<' || c == '>' || c == '|') {
            *s = '_';
        }
    }
}

static int write_ppm_rgba32(const char* path, const uint32_t* rgba, int w, int h)
{
    FILE* f = fopen(path, "wb");
    if (!f) return -1;
    fprintf(f, "P6\n%d %d\n255\n", w, h);
    for (int i = 0; i < w * h; i++) {
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

static void path_join(char* out, size_t out_size, const char* a, const char* b)
{
    if (out_size == 0) return;
    if (a == NULL) a = "";
    if (b == NULL) b = "";
    if (a[0] == '\0') {
        strncpy(out, b, out_size - 1);
        out[out_size - 1] = '\0';
        return;
    }
    size_t alen = strlen(a);
    if (alen > 0 && (a[alen - 1] == '/' || a[alen - 1] == '\\')) {
        safe_snprintf(out, out_size, "%s%s", a, b);
    } else {
        safe_snprintf(out, out_size, "%s/%s", a, b);
    }
    path_normalize_slash(out);
}

static int try_open_npk_candidates(const char* npk_dir, const char* img_path,
                                  const char* const* names, int name_count,
                                  NPKArchive* out_archive, NPKFileEntry** out_entry)
{
    for (int i = 0; i < name_count; i++) {
        if (names[i] == NULL || names[i][0] == '\0') continue;
        char full[768];
        path_join(full, sizeof(full), npk_dir, names[i]);

        NPKArchive archive;
        if (NPK_Open(full, &archive) != NPK_SUCCESS) {
            size_t len = strlen(full);
            if (len >= 4) {
                full[len - 3] = 'n';
                full[len - 2] = 'p';
                full[len - 1] = 'k';
            }
            if (NPK_Open(full, &archive) != NPK_SUCCESS) {
                continue;
            }
        }

        NPKFileEntry* e = NPK_FindFile(&archive, img_path);
        if (e == NULL) {
            NPK_Close(&archive);
            continue;
        }

        *out_archive = archive;
        *out_entry = e;
        return 1;
    }

    return 0;
}

static int try_open_npk_by_img_path(const char* npk_dir, const char* img_path,
                                    NPKArchive* out_archive, NPKFileEntry** out_entry)
{
    if (npk_dir == NULL || img_path == NULL || out_archive == NULL || out_entry == NULL) return 0;

    char tmp[512];
    strncpy(tmp, img_path, sizeof(tmp) - 1);
    tmp[sizeof(tmp) - 1] = '\0';
    path_normalize_slash(tmp);

    if (!starts_with_ci(tmp, "sprite/")) return 0;

    char* parts[8] = {0};
    int part_count = 0;
    char* p = tmp + 7;
    while (*p && part_count < 8) {
        parts[part_count++] = p;
        char* slash = strchr(p, '/');
        if (slash == NULL) break;
        *slash = '\0';
        p = slash + 1;
    }
    if (part_count < 1) return 0;

    int usable_parts = part_count;
    if (usable_parts >= 3 && strchr(parts[2], '.') != NULL) {
        usable_parts = 2;
    }

    char npk1[128] = {0};
    char npk2[128] = {0};
    char npk3[128] = {0};
    if (usable_parts >= 3) {
        snprintf(npk1, sizeof(npk1), "sprite_%s_%s_%s.NPK", parts[0], parts[1], parts[2]);
    }
    if (usable_parts >= 2) {
        snprintf(npk2, sizeof(npk2), "sprite_%s_%s.NPK", parts[0], parts[1]);
    }
    snprintf(npk3, sizeof(npk3), "sprite_%s.NPK", parts[0]);

    const char* cands[] = { npk1, npk2, npk3 };
    return try_open_npk_candidates(npk_dir, img_path, cands, 3, out_archive, out_entry);
}

static const char* guess_npk_prefix(const char* img_path)
{
    if (img_path == NULL) return "";
    if (starts_with_ci(img_path, "sprite/creature/")) return "sprite_creature";
    if (starts_with_ci(img_path, "sprite/interface/")) return "sprite_interface";
    if (starts_with_ci(img_path, "sprite/character/")) return "sprite_character";
    if (starts_with_ci(img_path, "sprite/item/")) return "sprite_item";
    if (starts_with_ci(img_path, "sprite/monster/")) return "sprite_monster";
    if (starts_with_ci(img_path, "sprite/npc/")) return "sprite_npc";
    return "sprite_";
}

static int find_img_in_npk_dir(const char* npk_dir, const char* img_path,
                               NPKArchive* out_archive, NPKFileEntry** out_entry)
{
    if (npk_dir == NULL || img_path == NULL || out_archive == NULL || out_entry == NULL) return 0;
    if (try_open_npk_by_img_path(npk_dir, img_path, out_archive, out_entry)) {
        return 1;
    }

#ifdef _WIN32
    char dir_win[512];
    strncpy(dir_win, npk_dir, sizeof(dir_win) - 1);
    dir_win[sizeof(dir_win) - 1] = '\0';
    for (char* p = dir_win; *p; p++) if (*p == '/') *p = '\\';

    const char* prefix = guess_npk_prefix(img_path);
    const char* exts[] = { "npk", "NPK" };

    for (int ei = 0; ei < 2; ei++) {
        char pattern[768];
        snprintf(pattern, sizeof(pattern), "%s\\%s*.%s", dir_win, prefix, exts[ei]);

        WIN32_FIND_DATAA ffd;
        HANDLE h = FindFirstFileA(pattern, &ffd);
        if (h == INVALID_HANDLE_VALUE) continue;

        do {
            if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;

            char full[768];
            path_join(full, sizeof(full), npk_dir, ffd.cFileName);

            NPKArchive archive;
            if (NPK_Open(full, &archive) != NPK_SUCCESS) {
                continue;
            }

            NPKFileEntry* e = NPK_FindFile(&archive, img_path);
            if (e == NULL) {
                NPK_Close(&archive);
                continue;
            }

            FindClose(h);
            *out_archive = archive;
            *out_entry = e;
            return 1;
        } while (FindNextFileA(h, &ffd));

        FindClose(h);
    }
#endif

    return 0;
}

int SimApp_Init(SimApp* app, const SimAppConfig* cfg)
{
    if (app == NULL || cfg == NULL || cfg->pvf_path == NULL || cfg->imagepacks2_dir == NULL) {
        return -1;
    }
    memset(app, 0, sizeof(*app));
    app->cfg = *cfg;

    PVF_Initialize();
    NPK_Initialize();

    if (PackSet_Open(cfg->pvf_path, &app->pvf) != PVF_SUCCESS) {
        return -2;
    }
    app->pvf_open = 1;

    return 0;
}

void SimApp_Shutdown(SimApp* app)
{
    if (app == NULL) return;
    if (app->pvf_open) {
        PackSet_Close(&app->pvf);
        app->pvf_open = 0;
    }
    memset(app, 0, sizeof(*app));
}

int SimApp_ListAni(SimApp* app, const char* contains, int limit)
{
    if (app == NULL || !app->pvf_open) return -1;
    if (limit <= 0) limit = 50;

    int printed = 0;
    uint32_t count = app->pvf.header.index_count;
    for (uint32_t i = 0; i < count; i++) {
        PVFIndexEntry* e = &app->pvf.indexes[i];
        if (e->name == NULL) continue;
        if (strstr(e->name, ".ani") == NULL) continue;
        if (contains != NULL && contains[0] != '\0' && strstr(e->name, contains) == NULL) continue;
        printf("%s\n", e->name);
        printed++;
        if (printed >= limit) break;
    }
    return printed;
}

int SimApp_DumpPvfFile(SimApp* app, const char* path_in_pvf, size_t bytes)
{
    if (app == NULL || !app->pvf_open || path_in_pvf == NULL) return -1;
    if (bytes == 0) bytes = 256;

    PVFIndexEntry* entry = PackSet_GetFile(&app->pvf, path_in_pvf);
    if (entry == NULL) {
        fprintf(stderr, "PVF file not found: %s\n", path_in_pvf);
        return -2;
    }

    uint8_t* buf = (uint8_t*)malloc(entry->size);
    if (!buf) return -3;

    if (PackSet_ExtractFile(&app->pvf, entry, buf, entry->size) != PVF_SUCCESS) {
        free(buf);
        return -4;
    }

    size_t n = bytes;
    if (n > entry->size) n = entry->size;
    for (size_t i = 0; i < n; i++) {
        printf("%02X%s", buf[i], ((i + 1) % 16 == 0) ? "\n" : " ");
    }
    if (n % 16 != 0) printf("\n");

    free(buf);
    return 0;
}

int SimApp_DecodeAniToPpm(SimApp* app, const char* ani_path_in_pvf, int max_frames_per_action)
{
    if (app == NULL || !app->pvf_open || ani_path_in_pvf == NULL) return -1;
    if (max_frames_per_action <= 0) max_frames_per_action = 5;

    ensure_out_dir(app->cfg.out_dir);

    ANIDefinition* def = NULL;
    if (ANI_LoadFromPVF(&app->pvf, ani_path_in_pvf, &def) != ANI_SUCCESS || def == NULL) {
        fprintf(stderr, "ANI parse failed: %s\n", ani_path_in_pvf);
        return -2;
    }

    int decoded = 0;
    for (int ai = 0; ai < def->action_count; ai++) {
        ANIAction* act = &def->actions[ai];
        int frames = act->frame_count;
        if (frames > max_frames_per_action) frames = max_frames_per_action;

        char loaded_img_path[ANI_MAX_IMG_PATH];
        loaded_img_path[0] = '\0';
        uint8_t* img_buf = NULL;
        uint32_t img_buf_size = 0;
        IMGFile img;
        memset(&img, 0, sizeof(img));
        int img_loaded = 0;

        for (int fi = 0; fi < frames; fi++) {
            ANIFrame* frame = &act->frames[fi];
            const char* img_path = frame->img_path[0] ? frame->img_path : act->img_path;
            if (img_path == NULL || img_path[0] == '\0') continue;

            if (!img_loaded || strcmp(loaded_img_path, img_path) != 0) {
                if (img_loaded) {
                    IMG_Free(&img);
                    free(img_buf);
                    img_buf = NULL;
                    img_buf_size = 0;
                    img_loaded = 0;
                }

                NPKArchive npk;
                NPKFileEntry* entry = NULL;
                if (!find_img_in_npk_dir(app->cfg.imagepacks2_dir, img_path, &npk, &entry) || entry == NULL) {
                    fprintf(stderr, "IMG not found in ImagePacks2 for: %s\n", img_path);
                    continue;
                }

                img_buf_size = entry->size;
                img_buf = (uint8_t*)malloc(img_buf_size);
                if (!img_buf) {
                    NPK_Close(&npk);
                    continue;
                }

                if (NPK_ReadFile(&npk, entry, img_buf, img_buf_size) != NPK_SUCCESS) {
                    free(img_buf);
                    img_buf = NULL;
                    img_buf_size = 0;
                    NPK_Close(&npk);
                    continue;
                }
                NPK_Close(&npk);

                memset(&img, 0, sizeof(img));
                if (IMG_Parse(img_buf, img_buf_size, &img) != IMG_SUCCESS) {
                    free(img_buf);
                    img_buf = NULL;
                    img_buf_size = 0;
                    continue;
                }

                strncpy(loaded_img_path, img_path, sizeof(loaded_img_path) - 1);
                loaded_img_path[sizeof(loaded_img_path) - 1] = '\0';
                img_loaded = 1;
            }

            int img_index = frame->img_index;
            if (img_index < 0) img_index = fi;

            IMGPixels pixels;
            memset(&pixels, 0, sizeof(pixels));
            if (IMG_DecodeFrame(&img, (uint32_t)img_index, &pixels) != IMG_SUCCESS || pixels.data == NULL) {
                continue;
            }

            char namebuf[512];
            safe_snprintf(namebuf, sizeof(namebuf), "%s_action%d_f%03d_idx%d.ppm",
                          ani_path_in_pvf, ai, fi, img_index);
            sanitize_filename(namebuf);

            char outpath[768];
            safe_snprintf(outpath, sizeof(outpath), "%s/%s", app->cfg.out_dir, namebuf);
            path_normalize_slash(outpath);

            if (write_ppm_rgba32(outpath, pixels.data, pixels.width, pixels.height) == 0) {
                decoded++;
                printf("[OK] %s (%dx%d)\n", outpath, pixels.width, pixels.height);
            }

            IMG_FreePixels(&pixels);
        }

        if (img_loaded) {
            IMG_Free(&img);
            free(img_buf);
        }
    }

    ANI_FreeDefinition(def);
    return decoded;
}

int SimApp_BatchDecodeAni(SimApp* app, const char* filter, int max_files, int max_frames_per_action)
{
    if (app == NULL || !app->pvf_open) return -1;
    if (max_files <= 0) max_files = 10;
    if (max_frames_per_action <= 0) max_frames_per_action = 3;

    int processed = 0;
    int total_decoded = 0;
    uint32_t count = app->pvf.header.index_count;

    printf("=== Batch ANI Decode ===\n");
    printf("Filter: %s\n", filter ? filter : "(none)");
    printf("Max files: %d\n", max_files);
    printf("Max frames per action: %d\n\n", max_frames_per_action);

    for (uint32_t i = 0; i < count && processed < max_files; i++) {
        PVFIndexEntry* e = &app->pvf.indexes[i];
        if (e->name == NULL) continue;
        if (strstr(e->name, ".ani") == NULL) continue;
        if (filter != NULL && filter[0] != '\0' && strstr(e->name, filter) == NULL) continue;

        printf("[%d/%d] Processing: %s\n", processed + 1, max_files, e->name);

        int decoded = SimApp_DecodeAniToPpm(app, e->name, max_frames_per_action);
        if (decoded > 0) {
            total_decoded += decoded;
            printf("  -> Decoded %d frames\n", decoded);
        } else {
            printf("  -> Failed or no frames\n");
        }

        processed++;
    }

    printf("\n=== Summary ===\n");
    printf("Files processed: %d\n", processed);
    printf("Total frames decoded: %d\n", total_decoded);

    return total_decoded;
}
