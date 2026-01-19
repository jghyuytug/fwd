/**
 * @file ani_loader.c
 * @brief DNF ANI Animation Loader Implementation
 *
 * Implementation of ANI file parsing and animation instance management.
 *
 * ANI file format (reverse engineered from DNF):
 *
 * Header:
 *   [0x00] uint32_t magic       - "ANI1" or "ANI2"
 *   [0x04] uint32_t version     - Format version
 *   [0x08] uint32_t action_count
 *   [0x0C] char[64] name        - Animation set name
 *
 * Each Action:
 *   [0x00] char[64] name        - Action name
 *   [0x40] uint32_t id          - Action ID
 *   [0x44] uint32_t frame_count
 *   [0x48] uint8_t  loop        - Loop flag
 *   [0x49] uint8_t  loop_start  - Loop start frame
 *   [0x4A] uint16_t reserved
 *   [0x4C] char[64] npk_name    - Associated NPK
 *   [0x8C] char[256] img_path   - Default IMG path
 *
 * Each Frame:
 *   [0x00] char[256] img_path   - IMG path (or empty to use default)
 *   [0x100] uint32_t img_index  - Frame index in IMG
 *   [0x104] uint16_t duration   - Duration (ms)
 *   [0x106] uint16_t delay      - Delay (ms)
 *   [0x108] int16_t offset_x    - X offset
 *   [0x10A] int16_t offset_y    - Y offset
 *   [0x10C] int16_t anchor_x    - Anchor X
 *   [0x10E] int16_t anchor_y    - Anchor Y
 *   [0x110] uint8_t alpha       - Alpha
 *   [0x111] uint8_t blend       - Blend mode
 *   [0x112] uint8_t flags       - Frame flags
 *   [0x113] uint8_t hitbox_count
 *   [0x114+] Hitbox data...
 *
 * @date 2026-01-04
 */

#include "resource/ani_loader.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

/* PVF library includes */
#include "pvf/pvf.h"

/* ========================================================================== */
/* Static Helper Functions                                                     */
/* ========================================================================== */

/**
 * @brief Read uint32_t from buffer (little-endian)
 */
static uint32_t read_u32(const uint8_t* p)
{
    return (uint32_t)p[0] |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

/**
 * @brief Read uint16_t from buffer (little-endian)
 */
static uint16_t read_u16(const uint8_t* p)
{
    return (uint16_t)p[0] |
           ((uint16_t)p[1] << 8);
}

/**
 * @brief Read int16_t from buffer (little-endian)
 */
static int16_t read_i16(const uint8_t* p)
{
    return (int16_t)read_u16(p);
}

static int32_t read_i32(const uint8_t* p)
{
    return (int32_t)read_u32(p);
}

/**
 * @brief Copy string from buffer (null-terminated)
 */
static void read_string(const uint8_t* p, char* out, size_t max_len)
{
    size_t i;
    for (i = 0; i < max_len - 1 && p[i] != '\0'; i++) {
        out[i] = (char)p[i];
    }
    out[i] = '\0';
}

/**
 * @brief Parse hitbox data
 */
static int parse_hitbox(const uint8_t* data, size_t size, ANIHitbox* box)
{
    if (size < 20) {
        return -1;
    }

    box->type = (ANIHitboxType)data[0];
    /* byte 1: reserved */
    box->rect.x = read_i16(data + 2);
    box->rect.y = read_i16(data + 4);
    box->rect.w = read_i16(data + 6);
    box->rect.h = read_i16(data + 8);
    box->damage = read_u16(data + 10);
    box->hitstun = read_u16(data + 12);
    box->knockback_x = read_i16(data + 14);
    box->knockback_y = read_i16(data + 16);
    box->element = data[18];

    return 20; /* Bytes consumed */
}

/**
 * @brief Parse frame data
 */
static int parse_frame(const uint8_t* data, size_t size, ANIFrame* frame)
{
    const uint8_t* p = data;
    int hitbox_count;
    int i;

    if (size < 0x114) {
        return -1;
    }

    /* Initialize frame */
    memset(frame, 0, sizeof(ANIFrame));

    /* IMG path */
    read_string(p, frame->img_path, ANI_MAX_IMG_PATH);
    p += 256;

    /* IMG index */
    frame->img_index = (int)read_u32(p);
    p += 4;

    /* Timing */
    frame->duration = read_u16(p);
    p += 2;
    frame->delay = read_u16(p);
    p += 2;

    /* Offsets */
    frame->offset.x = read_i16(p);
    p += 2;
    frame->offset.y = read_i16(p);
    p += 2;
    frame->anchor.x = read_i16(p);
    p += 2;
    frame->anchor.y = read_i16(p);
    p += 2;

    /* Transform */
    frame->alpha = *p++;
    frame->blend = (ANIBlendMode)*p++;

    /* Flags */
    {
        uint8_t flags = *p++;
        frame->is_key_frame = (flags & 0x01) != 0;
        frame->can_cancel = (flags & 0x02) != 0;
        frame->is_loop_point = (flags & 0x04) != 0;
        frame->flip_x = (flags & 0x08) != 0;
        frame->flip_y = (flags & 0x10) != 0;
    }

    /* Default scale */
    frame->scale_x = 1.0f;
    frame->scale_y = 1.0f;
    frame->rotation = 0.0f;

    /* Hitbox count */
    hitbox_count = *p++;
    if (hitbox_count > ANI_MAX_HITBOXES) {
        hitbox_count = ANI_MAX_HITBOXES;
    }
    frame->hitbox_count = hitbox_count;

    /* Parse hitboxes */
    for (i = 0; i < hitbox_count; i++) {
        int consumed = parse_hitbox(p, size - (p - data), &frame->hitboxes[i]);
        if (consumed < 0) {
            break;
        }
        p += consumed;
    }

    return (int)(p - data);
}

/**
 * @brief Parse event data
 */
static int parse_event(const uint8_t* data, size_t size, ANIEvent* event)
{
    const uint8_t* p = data;

    if (size < 8) {
        return -1;
    }

    memset(event, 0, sizeof(ANIEvent));

    /* Event type */
    event->type = (ANIEventType)read_u32(p);
    p += 4;

    /* Frame to trigger */
    event->frame = (int)read_u32(p);
    p += 4;

    /* Parse type-specific data */
    switch (event->type) {
        case EVENT_SOUND:
            if (size >= 8 + 64 + 4 + 1) {
                read_string(p, event->data.sound.path, 64);
                p += 64;
                /* Volume as fixed-point (0-100 -> 0.0-1.0) */
                event->data.sound.volume = (float)read_u32(p) / 100.0f;
                p += 4;
                event->data.sound.loop = (*p++ != 0);
            }
            break;

        case EVENT_EFFECT:
            if (size >= 8 + 64 + 8 + 1) {
                read_string(p, event->data.effect.path, 64);
                p += 64;
                event->data.effect.offset_x = read_i32(p);
                p += 4;
                event->data.effect.offset_y = read_i32(p);
                p += 4;
                event->data.effect.follow = (*p++ != 0);
            }
            break;

        case EVENT_PROJECTILE:
            if (size >= 8 + 64 + 20) {
                read_string(p, event->data.projectile.path, 64);
                p += 64;
                event->data.projectile.offset_x = read_i32(p);
                p += 4;
                event->data.projectile.offset_y = read_i32(p);
                p += 4;
                event->data.projectile.velocity_x = read_i32(p);
                p += 4;
                event->data.projectile.velocity_y = read_i32(p);
                p += 4;
                event->data.projectile.damage = read_i32(p);
                p += 4;
            }
            break;

        case EVENT_MOVE:
            if (size >= 8 + 8) {
                event->data.move.delta_x = read_i32(p);
                p += 4;
                event->data.move.delta_y = read_i32(p);
                p += 4;
            }
            break;

        case EVENT_CANCEL:
            if (size >= 8 + 8) {
                event->data.cancel.cancel_window = read_i32(p);
                p += 4;
                event->data.cancel.allowed_actions = read_i32(p);
                p += 4;
            }
            break;

        case EVENT_LOOP:
            if (size >= 8 + 8) {
                event->data.loop.target_frame = read_i32(p);
                p += 4;
                event->data.loop.loop_count = read_i32(p);
                p += 4;
            }
            break;

        case EVENT_DAMAGE:
        case EVENT_GRAB:
        case EVENT_END:
        case EVENT_NONE:
        default:
            /* No additional data */
            break;
    }

    return (int)(p - data);
}

/**
 * @brief Parse action data
 */
static int parse_action(const uint8_t* data, size_t size, ANIAction* action)
{
    const uint8_t* p = data;
    uint32_t frame_count;
    uint32_t event_count;
    int i;

    if (size < 0x18C) {
        return -1;
    }

    /* Initialize action */
    memset(action, 0, sizeof(ANIAction));

    /* Name */
    read_string(p, action->name, 64);
    p += 64;

    /* ID */
    action->id = (int)read_u32(p);
    p += 4;

    /* Frame count */
    frame_count = read_u32(p);
    p += 4;
    if (frame_count > ANI_MAX_FRAMES) {
        frame_count = ANI_MAX_FRAMES;
    }
    action->frame_count = (int)frame_count;

    /* Loop settings */
    action->loop = (*p++ != 0);
    action->loop_start = *p++;
    p += 2; /* Reserved */

    /* NPK name */
    read_string(p, action->npk_name, 64);
    p += 64;

    /* IMG path */
    read_string(p, action->img_path, ANI_MAX_IMG_PATH);
    p += 256;

    /* Default speed */
    action->speed = 1.0f;

    /* Allocate frames */
    if (frame_count > 0) {
        action->frames = (ANIFrame*)calloc(frame_count, sizeof(ANIFrame));
        if (action->frames == NULL) {
            return -1;
        }

        /* Parse frames */
        for (i = 0; i < (int)frame_count; i++) {
            int consumed = parse_frame(p, size - (p - data), &action->frames[i]);
            if (consumed < 0) {
                /* Parse error, truncate */
                action->frame_count = i;
                break;
            }
            p += consumed;
        }
    }

    /* Parse events (if present) */
    if ((size_t)(p - data) + 4 <= size) {
        event_count = read_u32(p);
        p += 4;

        if (event_count > 0 && event_count <= ANI_MAX_EVENTS) {
            action->events = (ANIEvent*)calloc(event_count, sizeof(ANIEvent));
            if (action->events != NULL) {
                action->event_count = 0;
                for (i = 0; i < (int)event_count; i++) {
                    int consumed = parse_event(p, size - (p - data), &action->events[i]);
                    if (consumed < 0) {
                        break;
                    }
                    action->event_count++;
                    p += consumed;
                }
            }
        }
    }

    return (int)(p - data);
}

static void normalize_slashes(char* s)
{
    if (s == NULL) return;
    for (; *s; s++) {
        if (*s == '\\') *s = '/';
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
        if ((unsigned char)tolower(a) != (unsigned char)tolower(b)) return 0;
        i++;
    }
    return 1;
}

static void normalize_img_path_to_npk(char* s, size_t s_size)
{
    char tmp[ANI_MAX_IMG_PATH];
    size_t i, n;

    if (s == NULL || s_size == 0) return;

    snprintf(tmp, sizeof(tmp), "%s", s);
    normalize_slashes(tmp);

    /* Trim leading spaces */
    while (tmp[0] == ' ') {
        memmove(tmp, tmp + 1, strlen(tmp));
    }

    /* Lowercase + normalize slashes */
    n = strlen(tmp);
    for (i = 0; i < n; i++) {
        unsigned char c = (unsigned char)tmp[i];
        if (c == '\\') c = '/';
        tmp[i] = (char)tolower(c);
    }

    /* Many PVF .ani references omit the "sprite/" prefix used inside NPKs. */
    if (!starts_with_ci(tmp, "sprite/")) {
        if (snprintf(s, s_size, "sprite/%s", tmp) >= (int)s_size) {
            s[0] = '\0';
            return;
        }
    } else {
        snprintf(s, s_size, "%s", tmp);
    }
}

static int clamp_i32_to_i16(int32_t v)
{
    if (v > 32767) return 32767;
    if (v < -32768) return -32768;
    return (int16_t)v;
}

static int ANI_LoadCompactFromMemory(const void* data, size_t size, ANIDefinition** out_def)
{
    const uint8_t* p = (const uint8_t*)data;
    ANIDefinition* def = NULL;
    ANIAction* action = NULL;
    uint16_t frame_count_u16;
    uint32_t img_len;
    size_t pos;

    if (data == NULL || out_def == NULL || size < 8) {
        return ANI_ERROR_INVALID;
    }

    frame_count_u16 = read_u16(p + 0);
    img_len = read_u32(p + 4);

    if (img_len == 0 || img_len > 4096 || (size_t)8 + (size_t)img_len > size) {
        return ANI_ERROR_PARSE;
    }

    /* Allocate definition with a single action */
    def = (ANIDefinition*)calloc(1, sizeof(ANIDefinition));
    if (def == NULL) {
        return ANI_ERROR_MEMORY;
    }

    def->version = 0;
    def->action_count = 1;
    def->default_action = 0;
    def->ref_count = 1;

    def->actions = (ANIAction*)calloc(1, sizeof(ANIAction));
    if (def->actions == NULL) {
        free(def);
        return ANI_ERROR_MEMORY;
    }

    action = &def->actions[0];
    strncpy(action->name, "default", sizeof(action->name) - 1);
    action->id = 0;
    action->loop = true;
    action->loop_start = 0;
    action->speed = 1.0f;

    /* Default IMG path (these .ani commonly reference a single .img) */
    {
        size_t to_copy = img_len;
        if (to_copy >= sizeof(action->img_path)) {
            to_copy = sizeof(action->img_path) - 1;
        }
        memcpy(action->img_path, p + 8, to_copy);
        action->img_path[to_copy] = '\0';
        normalize_img_path_to_npk(action->img_path, sizeof(action->img_path));
    }

    /* Frames */
    if (frame_count_u16 > ANI_MAX_FRAMES) {
        frame_count_u16 = ANI_MAX_FRAMES;
    }
    action->frame_count = (int)frame_count_u16;

    if (action->frame_count > 0) {
        action->frames = (ANIFrame*)calloc((size_t)action->frame_count, sizeof(ANIFrame));
        if (action->frames == NULL) {
            ANI_FreeDefinition(def);
            return ANI_ERROR_MEMORY;
        }
    }

    /* Compact frame record parsing (observed in PVF .ani files):
     *   u16 frame_count
     *   u16 version(?)
     *   u32 img_path_len
     *   char img_path[img_path_len]
     *   u16 unknown_a
     *   u16 unknown_b
     *   u32 unknown_c
     *   repeat frame_count times:
     *     u8  pad0
     *     u16 frame_id
     *     i32 offset_x
     *     i32 offset_y
     *     u16 unk1
     *     u16 unk2
     *     u16 unk3
     *     u16 unk4
     *     u16 duration
     *     u32 unk5
     *     u8  pad1
     */
    pos = 8 + (size_t)img_len;

    if (pos + 8 > size) {
        *out_def = def;
        return ANI_SUCCESS;
    }

    pos += 2; /* unknown_a */
    pos += 2; /* unknown_b */
    pos += 4; /* unknown_c */

    for (int fi = 0; fi < action->frame_count; fi++) {
        ANIFrame* f = &action->frames[fi];

        /* Defaults */
        f->img_path[0] = '\0';
        f->img_index = fi;
        f->duration = 100;
        f->delay = 0;
        f->offset.x = 0;
        f->offset.y = 0;
        f->anchor.x = 0;
        f->anchor.y = 0;
        f->scale_x = 1.0f;
        f->scale_y = 1.0f;
        f->rotation = 0.0f;
        f->alpha = 255;
        f->blend = 0;
        f->hitbox_count = 0;
        f->is_key_frame = false;
        f->can_cancel = false;
        f->is_loop_point = false;
        f->flip_x = false;
        f->flip_y = false;

        /* Minimum bytes to parse up to duration: 1 + 2 + 4 + 4 + (u16*4) + 2 = 21 */
        if (pos + 21 > size) {
            break;
        }

        pos += 1; /* pad0 */
        uint16_t frame_id = read_u16(p + pos);
        pos += 2;

        int32_t ox = read_i32(p + pos);
        pos += 4;
        int32_t oy = read_i32(p + pos);
        pos += 4;

        /* Skip 4 unknown u16 values */
        pos += 2 * 4;

        uint16_t duration = read_u16(p + pos);
        pos += 2;

        /* Optional tail (often 5 bytes: u32 0 + u8 0). Some files end with a short trailer. */
        if (size - pos >= 5) {
            pos += 4; /* unk5 */
            pos += 1; /* pad1 */
        } else {
            /* Leave remaining bytes (likely trailer) */
        }

        f->img_index = (int)frame_id;
        f->offset.x = clamp_i32_to_i16(ox);
        f->offset.y = clamp_i32_to_i16(oy);
        f->duration = duration;
    }

    *out_def = def;
    return ANI_SUCCESS;
}

/* ========================================================================== */
/* ANI Loading API Implementation                                              */
/* ========================================================================== */

int ANI_LoadFromMemory(const void* data, size_t size, ANIDefinition** out_def)
{
    const uint8_t* p;
    ANIDefinition* def;
    uint32_t magic;
    uint32_t action_count;
    int i;

    if (data == NULL || out_def == NULL || size < 8) {
        return ANI_ERROR_INVALID;
    }

    p = (const uint8_t*)data;

    /* Check magic */
    magic = read_u32(p);
    if (magic != ANI_MAGIC_V1 && magic != ANI_MAGIC_V2) {
        /* Try alternate text magic */
        if (strncmp((const char*)p, ANI_MAGIC, 10) != 0) {
            /* Try compact PVF .ani format (no "ANI1"/"ANI2" magic) */
            return ANI_LoadCompactFromMemory(data, size, out_def);
        }
        magic = ANI_MAGIC_V1;
    }
    p += 4;

    if (size < 0x4C) {
        return ANI_ERROR_PARSE;
    }

    /* Allocate definition */
    def = (ANIDefinition*)calloc(1, sizeof(ANIDefinition));
    if (def == NULL) {
        return ANI_ERROR_MEMORY;
    }

    /* Version */
    def->version = read_u32(p);
    p += 4;

    /* Action count */
    action_count = read_u32(p);
    p += 4;
    if (action_count > ANI_MAX_ACTIONS) {
        action_count = ANI_MAX_ACTIONS;
    }
    def->action_count = (int)action_count;

    /* Name */
    read_string(p, def->name, 64);
    p += 64;

    /* Default NPK (optional, at offset 0x4C) */
    if (size >= 0x8C) {
        read_string(p, def->default_npk, 64);
        p += 64;
    }

    /* Default action and base offset (optional) */
    if (size >= 0x94) {
        def->default_action = (int)read_u32(p);
        p += 4;
        def->base_offset.x = read_i16(p);
        p += 2;
        def->base_offset.y = read_i16(p);
        p += 2;
    }

    /* Shadow info (optional) */
    if (size >= 0xA0) {
        def->has_shadow = (*p++ != 0);
        p++; /* padding */
        def->shadow_rect.x = read_i16(p);
        p += 2;
        def->shadow_rect.y = read_i16(p);
        p += 2;
        def->shadow_rect.w = read_i16(p);
        p += 2;
        def->shadow_rect.h = read_i16(p);
        p += 2;
    }

    /* Allocate actions */
    if (action_count > 0) {
        def->actions = (ANIAction*)calloc(action_count, sizeof(ANIAction));
        if (def->actions == NULL) {
            free(def);
            return ANI_ERROR_MEMORY;
        }

        /* Parse actions */
        for (i = 0; i < (int)action_count; i++) {
            int consumed = parse_action(p, size - (p - (const uint8_t*)data),
                                        &def->actions[i]);
            if (consumed < 0) {
                /* Parse error, truncate */
                def->action_count = i;
                break;
            }
            p += consumed;
        }
    }

    /* Initialize reference count */
    def->ref_count = 1;

    *out_def = def;
    return ANI_SUCCESS;
}

int ANI_LoadFromPVF(PackSetInternal* pvf, const char* ani_path,
                    ANIDefinition** out_def)
{
    PVFIndexEntry* entry;
    void* buffer = NULL;
    int result;

    if (pvf == NULL || ani_path == NULL || out_def == NULL) {
        return ANI_ERROR_INVALID;
    }

    /* Find file in PVF */
    entry = PackSet_GetFile(pvf, ani_path);
    if (entry == NULL || entry->size == 0) {
        return ANI_ERROR_NOT_FOUND;
    }

    buffer = malloc(entry->size);
    if (buffer == NULL) {
        return ANI_ERROR_MEMORY;
    }

    /* Extract file from PVF */
    result = PackSet_ExtractFile(pvf, entry, buffer, entry->size);
    if (result != PVF_SUCCESS) {
        free(buffer);
        return ANI_ERROR_PVF;
    }

    /* Parse ANI data */
    result = ANI_LoadFromMemory(buffer, entry->size, out_def);

    /* Free PVF buffer */
    free(buffer);

    return result;
}

void ANI_FreeDefinition(ANIDefinition* def)
{
    int i;

    if (def == NULL) {
        return;
    }

    /* Free actions */
    if (def->actions != NULL) {
        for (i = 0; i < def->action_count; i++) {
            ANIAction* action = &def->actions[i];
            if (action->frames != NULL) {
                free(action->frames);
            }
            if (action->events != NULL) {
                free(action->events);
            }
        }
        free(def->actions);
    }

    free(def);
}

void ANI_AddRef(ANIDefinition* def)
{
    if (def != NULL) {
        def->ref_count++;
    }
}

bool ANI_Release(ANIDefinition* def)
{
    if (def == NULL) {
        return false;
    }

    def->ref_count--;
    if (def->ref_count <= 0) {
        ANI_FreeDefinition(def);
        return true;
    }
    return false;
}

/* ========================================================================== */
/* ANI Instance API Implementation                                             */
/* ========================================================================== */

ANIInstance* ANI_CreateInstance(ANIDefinition* def)
{
    ANIInstance* inst;

    if (def == NULL) {
        return NULL;
    }

    inst = (ANIInstance*)calloc(1, sizeof(ANIInstance));
    if (inst == NULL) {
        return NULL;
    }

    inst->definition = def;
    ANI_AddRef(def);

    /* Set default action */
    if (def->action_count > 0) {
        int default_idx = def->default_action;
        if (default_idx < 0 || default_idx >= def->action_count) {
            default_idx = 0;
        }
        inst->current_action = &def->actions[default_idx];
    }

    /* Initialize state */
    inst->current_frame = 0;
    inst->frame_timer = 0.0f;
    inst->speed = 1.0f;
    inst->playing = false;
    inst->paused = false;
    inst->finished = false;
    inst->loop_count = 0;
    inst->last_event = -1;

    /* Default transform */
    inst->flip_x = false;
    inst->flip_y = false;
    inst->alpha = 255;
    inst->scale = 1.0f;

    return inst;
}

void ANI_FreeInstance(ANIInstance* inst)
{
    if (inst == NULL) {
        return;
    }

    if (inst->definition != NULL) {
        ANI_Release(inst->definition);
    }

    free(inst);
}

void ANI_Update(ANIInstance* inst, float delta_ms)
{
    ANIAction* action;
    ANIFrame* frame;
    float effective_speed;
    float frame_duration;

    if (inst == NULL || !inst->playing || inst->paused) {
        return;
    }

    action = inst->current_action;
    if (action == NULL || action->frame_count == 0) {
        return;
    }

    /* Get current frame */
    frame = &action->frames[inst->current_frame];

    /* Calculate effective speed */
    effective_speed = inst->speed * action->speed;
    if (effective_speed <= 0.0f) {
        effective_speed = 1.0f;
    }

    /* Update timer */
    inst->frame_timer += delta_ms * effective_speed;

    /* Get frame duration (with delay) */
    frame_duration = (float)(frame->duration + frame->delay);
    if (frame_duration <= 0.0f) {
        frame_duration = 16.67f; /* Default ~60fps */
    }

    /* Check for frame advance */
    while (inst->frame_timer >= frame_duration) {
        int old_frame = inst->current_frame;
        int next_frame = inst->current_frame + 1;

        inst->frame_timer -= frame_duration;

        /* Check for animation end */
        if (next_frame >= action->frame_count) {
            if (action->loop) {
                /* Loop back */
                next_frame = action->loop_start;
                if (next_frame < 0 || next_frame >= action->frame_count) {
                    next_frame = 0;
                }
                inst->loop_count++;
            } else {
                /* Animation finished */
                inst->finished = true;
                inst->playing = false;
                next_frame = action->frame_count - 1;

                /* Callback */
                if (inst->on_action_end != NULL) {
                    inst->on_action_end(inst);
                }
                break;
            }
        }

        /* Advance frame */
        inst->current_frame = next_frame;

        /* Frame change callback */
        if (inst->on_frame_change != NULL && old_frame != next_frame) {
            inst->on_frame_change(inst, old_frame, next_frame);
        }

        /* Update frame reference */
        frame = &action->frames[inst->current_frame];
        frame_duration = (float)(frame->duration + frame->delay);
        if (frame_duration <= 0.0f) {
            frame_duration = 16.67f;
        }
    }

    /* Process events for current frame */
    if (action->events != NULL && action->event_count > 0) {
        int i;
        for (i = 0; i < action->event_count; i++) {
            ANIEvent* event = &action->events[i];
            if (event->frame == inst->current_frame && i > inst->last_event) {
                inst->last_event = i;
                if (inst->on_event != NULL) {
                    inst->on_event(inst, event);
                }
            }
        }
    }
}

int ANI_SetAction(ANIInstance* inst, const char* action_name)
{
    ANIAction* action;

    if (inst == NULL || action_name == NULL) {
        return ANI_ERROR_INVALID;
    }

    action = ANI_GetActionByName(inst->definition, action_name);
    if (action == NULL) {
        return ANI_ERROR_NOT_FOUND;
    }

    inst->current_action = action;
    ANI_Reset(inst);

    return ANI_SUCCESS;
}

int ANI_SetActionById(ANIInstance* inst, int action_id)
{
    ANIAction* action;

    if (inst == NULL) {
        return ANI_ERROR_INVALID;
    }

    action = ANI_GetActionById(inst->definition, action_id);
    if (action == NULL) {
        return ANI_ERROR_NOT_FOUND;
    }

    inst->current_action = action;
    ANI_Reset(inst);

    return ANI_SUCCESS;
}

void ANI_Play(ANIInstance* inst)
{
    if (inst == NULL) {
        return;
    }

    inst->playing = true;
    inst->paused = false;
    inst->finished = false;
}

void ANI_Pause(ANIInstance* inst)
{
    if (inst == NULL) {
        return;
    }

    inst->paused = true;
}

void ANI_Stop(ANIInstance* inst)
{
    if (inst == NULL) {
        return;
    }

    inst->playing = false;
    inst->paused = false;
    ANI_Reset(inst);
}

void ANI_Reset(ANIInstance* inst)
{
    if (inst == NULL) {
        return;
    }

    inst->current_frame = 0;
    inst->frame_timer = 0.0f;
    inst->finished = false;
    inst->loop_count = 0;
    inst->last_event = -1;
}

void ANI_SetSpeed(ANIInstance* inst, float speed)
{
    if (inst == NULL) {
        return;
    }

    inst->speed = speed;
}

void ANI_GotoFrame(ANIInstance* inst, int frame)
{
    if (inst == NULL || inst->current_action == NULL) {
        return;
    }

    if (frame < 0) {
        frame = 0;
    } else if (frame >= inst->current_action->frame_count) {
        frame = inst->current_action->frame_count - 1;
    }

    inst->current_frame = frame;
    inst->frame_timer = 0.0f;
}

/* ========================================================================== */
/* ANI Query API Implementation                                                */
/* ========================================================================== */

ANIFrame* ANI_GetCurrentFrame(ANIInstance* inst)
{
    if (inst == NULL || inst->current_action == NULL) {
        return NULL;
    }

    if (inst->current_frame < 0 ||
        inst->current_frame >= inst->current_action->frame_count) {
        return NULL;
    }

    return &inst->current_action->frames[inst->current_frame];
}

ANIFrame* ANI_GetFrame(ANIInstance* inst, int index)
{
    if (inst == NULL || inst->current_action == NULL) {
        return NULL;
    }

    if (index < 0 || index >= inst->current_action->frame_count) {
        return NULL;
    }

    return &inst->current_action->frames[index];
}

ANIAction* ANI_GetActionByName(ANIDefinition* def, const char* name)
{
    int i;

    if (def == NULL || name == NULL) {
        return NULL;
    }

    for (i = 0; i < def->action_count; i++) {
        if (strcmp(def->actions[i].name, name) == 0) {
            return &def->actions[i];
        }
    }

    return NULL;
}

ANIAction* ANI_GetActionById(ANIDefinition* def, int id)
{
    int i;

    if (def == NULL) {
        return NULL;
    }

    for (i = 0; i < def->action_count; i++) {
        if (def->actions[i].id == id) {
            return &def->actions[i];
        }
    }

    return NULL;
}

int ANI_GetDuration(ANIAction* action)
{
    int total = 0;
    int i;

    if (action == NULL || action->frames == NULL) {
        return 0;
    }

    for (i = 0; i < action->frame_count; i++) {
        total += action->frames[i].duration + action->frames[i].delay;
    }

    return total;
}

bool ANI_IsPlaying(ANIInstance* inst)
{
    if (inst == NULL) {
        return false;
    }
    return inst->playing && !inst->paused;
}

bool ANI_IsFinished(ANIInstance* inst)
{
    if (inst == NULL) {
        return false;
    }
    return inst->finished;
}

int ANI_GetHitboxes(ANIInstance* inst, ANIHitbox* out_boxes, int max_boxes)
{
    ANIFrame* frame;
    int count;
    int i;

    if (inst == NULL || out_boxes == NULL || max_boxes <= 0) {
        return 0;
    }

    frame = ANI_GetCurrentFrame(inst);
    if (frame == NULL) {
        return 0;
    }

    count = frame->hitbox_count;
    if (count > max_boxes) {
        count = max_boxes;
    }

    for (i = 0; i < count; i++) {
        out_boxes[i] = frame->hitboxes[i];

        /* Apply instance flip */
        if (inst->flip_x) {
            out_boxes[i].rect.x = -out_boxes[i].rect.x - out_boxes[i].rect.w;
            out_boxes[i].knockback_x = -out_boxes[i].knockback_x;
        }
        if (inst->flip_y) {
            out_boxes[i].rect.y = -out_boxes[i].rect.y - out_boxes[i].rect.h;
            out_boxes[i].knockback_y = -out_boxes[i].knockback_y;
        }
    }

    return count;
}

bool ANI_CanCancel(ANIInstance* inst)
{
    ANIFrame* frame;

    frame = ANI_GetCurrentFrame(inst);
    if (frame == NULL) {
        return false;
    }

    return frame->can_cancel;
}

/* ========================================================================== */
/* Utility Functions                                                           */
/* ========================================================================== */

const char* ANI_GetHitboxTypeName(ANIHitboxType type)
{
    static const char* names[] = {
        "None",
        "Collision",
        "Attack",
        "Grab",
        "Invincible",
        "SuperArmor"
    };

    if (type < 0 || type >= HITBOX_COUNT) {
        return "Unknown";
    }

    return names[type];
}

const char* ANI_GetEventTypeName(ANIEventType type)
{
    static const char* names[] = {
        "None",
        "Sound",
        "Effect",
        "Projectile",
        "Damage",
        "Grab",
        "Move",
        "Cancel",
        "Loop",
        "End"
    };

    if (type < 0 || type >= EVENT_COUNT) {
        return "Unknown";
    }

    return names[type];
}

void ANI_PrintInfo(ANIDefinition* def)
{
    int i;

    if (def == NULL) {
        printf("ANI: NULL\n");
        return;
    }

    printf("=== ANI Definition ===\n");
    printf("  Name: %s\n", def->name);
    printf("  Version: %u\n", def->version);
    printf("  Actions: %d\n", def->action_count);
    printf("  Default NPK: %s\n", def->default_npk);
    printf("  Default Action: %d\n", def->default_action);
    printf("  Base Offset: (%d, %d)\n", def->base_offset.x, def->base_offset.y);
    printf("  Has Shadow: %s\n", def->has_shadow ? "Yes" : "No");

    for (i = 0; i < def->action_count; i++) {
        ANIAction* action = &def->actions[i];
        printf("\n  Action %d: %s (ID=%d)\n", i, action->name, action->id);
        printf("    Frames: %d\n", action->frame_count);
        printf("    Loop: %s (start=%d)\n",
               action->loop ? "Yes" : "No", action->loop_start);
        printf("    NPK: %s\n", action->npk_name);
        printf("    IMG: %s\n", action->img_path);
        printf("    Duration: %d ms\n", ANI_GetDuration(action));
    }
}

void ANI_PrintFrame(ANIFrame* frame)
{
    int i;

    if (frame == NULL) {
        printf("Frame: NULL\n");
        return;
    }

    printf("=== ANI Frame ===\n");
    printf("  IMG: %s [%d]\n", frame->img_path, frame->img_index);
    printf("  Duration: %u ms (+%u delay)\n", frame->duration, frame->delay);
    printf("  Offset: (%d, %d)\n", frame->offset.x, frame->offset.y);
    printf("  Anchor: (%d, %d)\n", frame->anchor.x, frame->anchor.y);
    printf("  Alpha: %u\n", frame->alpha);
    printf("  Flags: key=%d cancel=%d loop=%d flip_x=%d flip_y=%d\n",
           frame->is_key_frame, frame->can_cancel, frame->is_loop_point,
           frame->flip_x, frame->flip_y);
    printf("  Hitboxes: %d\n", frame->hitbox_count);

    for (i = 0; i < frame->hitbox_count; i++) {
        ANIHitbox* box = &frame->hitboxes[i];
        printf("    [%d] %s: (%d,%d %dx%d) dmg=%d stun=%d kb=(%d,%d)\n",
               i, ANI_GetHitboxTypeName(box->type),
               box->rect.x, box->rect.y, box->rect.w, box->rect.h,
               box->damage, box->hitstun, box->knockback_x, box->knockback_y);
    }
}
