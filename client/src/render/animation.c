/**
 * @file animation.c
 * @brief Animation system for sprites and effects
 */

#include "../../include/renderer.h"
#include "../../include/client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/*
 * ============================================================================
 * Constants
 * ============================================================================
 */

#define MAX_ANIMATIONS      256
#define MAX_ANIM_INSTANCES  512
#define MAX_ANIM_FRAMES     64

/*
 * ============================================================================
 * Animation Types
 * ============================================================================
 */

typedef enum {
    ANIM_LOOP,          /* Loop forever */
    ANIM_ONCE,          /* Play once and stop */
    ANIM_PINGPONG,      /* Forward then backward */
    ANIM_ONCE_DESTROY   /* Play once then mark for removal */
} AnimPlayMode;

typedef enum {
    EASE_LINEAR,
    EASE_IN_QUAD,
    EASE_OUT_QUAD,
    EASE_IN_OUT_QUAD,
    EASE_IN_CUBIC,
    EASE_OUT_CUBIC,
    EASE_IN_OUT_CUBIC,
    EASE_IN_BACK,
    EASE_OUT_BACK,
    EASE_OUT_BOUNCE
} EaseType;

/*
 * ============================================================================
 * Animation Frame
 * ============================================================================
 */

typedef struct {
    int         src_x;
    int         src_y;
    int         src_w;
    int         src_h;
    float       duration;   /* Duration of this frame in seconds */
    float       offset_x;   /* Position offset during this frame */
    float       offset_y;
    int         event_id;   /* Optional event to trigger (0 = none) */
} AnimFrame;

/*
 * ============================================================================
 * Animation Definition
 * ============================================================================
 */

typedef struct {
    u32         id;
    char        name[32];
    SDL_Texture* texture;
    AnimFrame   frames[MAX_ANIM_FRAMES];
    int         frame_count;
    AnimPlayMode mode;
    float       total_duration;
    bool        in_use;
} Animation;

/*
 * ============================================================================
 * Animation Instance
 * ============================================================================
 */

typedef struct {
    u32         anim_id;
    float       time;
    float       speed;
    int         current_frame;
    bool        playing;
    bool        finished;
    bool        forward;    /* For pingpong */
    bool        in_use;

    /* Transform */
    float       x;
    float       y;
    float       scale_x;
    float       scale_y;
    float       rotation;
    u8          alpha;
    bool        flip_x;
    bool        flip_y;

    /* Callbacks */
    void        (*on_frame)(int frame, int event_id, void* user_data);
    void        (*on_complete)(void* user_data);
    void*       user_data;
} AnimInstance;

/*
 * ============================================================================
 * Animation Manager
 * ============================================================================
 */

static struct {
    Animation       anims[MAX_ANIMATIONS];
    AnimInstance    instances[MAX_ANIM_INSTANCES];
    int             anim_count;
    int             instance_count;
} g_anim_mgr = {0};

/*
 * ============================================================================
 * Easing Functions
 * ============================================================================
 */

static float ease_linear(float t) { return t; }

static float ease_in_quad(float t) { return t * t; }
static float ease_out_quad(float t) { return t * (2 - t); }
static float ease_in_out_quad(float t) {
    return t < 0.5f ? 2 * t * t : -1 + (4 - 2 * t) * t;
}

static float ease_in_cubic(float t) { return t * t * t; }
static float ease_out_cubic(float t) { float f = t - 1; return f * f * f + 1; }
static float ease_in_out_cubic(float t) {
    return t < 0.5f ? 4 * t * t * t : (t - 1) * (2 * t - 2) * (2 * t - 2) + 1;
}

static float ease_in_back(float t) {
    const float c1 = 1.70158f;
    return (c1 + 1) * t * t * t - c1 * t * t;
}

static float ease_out_back(float t) {
    const float c1 = 1.70158f;
    float f = t - 1;
    return 1 + (c1 + 1) * f * f * f + c1 * f * f;
}

static float ease_out_bounce(float t) {
    const float n1 = 7.5625f;
    const float d1 = 2.75f;
    if (t < 1 / d1) {
        return n1 * t * t;
    } else if (t < 2 / d1) {
        t -= 1.5f / d1;
        return n1 * t * t + 0.75f;
    } else if (t < 2.5f / d1) {
        t -= 2.25f / d1;
        return n1 * t * t + 0.9375f;
    } else {
        t -= 2.625f / d1;
        return n1 * t * t + 0.984375f;
    }
}

float animation_ease(EaseType type, float t)
{
    if (t < 0) t = 0;
    if (t > 1) t = 1;

    switch (type) {
        case EASE_IN_QUAD:      return ease_in_quad(t);
        case EASE_OUT_QUAD:     return ease_out_quad(t);
        case EASE_IN_OUT_QUAD:  return ease_in_out_quad(t);
        case EASE_IN_CUBIC:     return ease_in_cubic(t);
        case EASE_OUT_CUBIC:    return ease_out_cubic(t);
        case EASE_IN_OUT_CUBIC: return ease_in_out_cubic(t);
        case EASE_IN_BACK:      return ease_in_back(t);
        case EASE_OUT_BACK:     return ease_out_back(t);
        case EASE_OUT_BOUNCE:   return ease_out_bounce(t);
        default:                return ease_linear(t);
    }
}

/*
 * ============================================================================
 * Animation Creation
 * ============================================================================
 */

int animation_create(const char* name, SDL_Texture* texture, AnimPlayMode mode)
{
    Animation* anim;
    int i;

    /* Find free slot */
    for (i = 0; i < MAX_ANIMATIONS; i++) {
        if (!g_anim_mgr.anims[i].in_use) {
            anim = &g_anim_mgr.anims[i];
            memset(anim, 0, sizeof(Animation));
            anim->id = i;
            anim->in_use = true;
            anim->texture = texture;
            anim->mode = mode;
            strncpy(anim->name, name, 31);
            g_anim_mgr.anim_count++;
            return i;
        }
    }

    fprintf(stderr, "Animation limit reached\n");
    return -1;
}

int animation_add_frame(int anim_id, int src_x, int src_y, int src_w, int src_h,
                        float duration, float offset_x, float offset_y, int event_id)
{
    Animation* anim;

    if (anim_id < 0 || anim_id >= MAX_ANIMATIONS) return -1;
    anim = &g_anim_mgr.anims[anim_id];
    if (!anim->in_use) return -1;
    if (anim->frame_count >= MAX_ANIM_FRAMES) return -1;

    AnimFrame* frame = &anim->frames[anim->frame_count];
    frame->src_x = src_x;
    frame->src_y = src_y;
    frame->src_w = src_w;
    frame->src_h = src_h;
    frame->duration = duration;
    frame->offset_x = offset_x;
    frame->offset_y = offset_y;
    frame->event_id = event_id;

    anim->total_duration += duration;
    anim->frame_count++;

    return anim->frame_count - 1;
}

int animation_create_from_strip(const char* name, SDL_Texture* texture,
                                 int frame_w, int frame_h, int frame_count,
                                 float fps, AnimPlayMode mode)
{
    int anim_id;
    int tex_w, tex_h;
    int columns;
    float duration;
    int i;

    anim_id = animation_create(name, texture, mode);
    if (anim_id < 0) return -1;

    SDL_QueryTexture(texture, NULL, NULL, &tex_w, &tex_h);
    columns = tex_w / frame_w;
    if (columns <= 0) columns = 1;

    duration = 1.0f / fps;

    for (i = 0; i < frame_count; i++) {
        int src_x = (i % columns) * frame_w;
        int src_y = (i / columns) * frame_h;
        animation_add_frame(anim_id, src_x, src_y, frame_w, frame_h,
                           duration, 0, 0, 0);
    }

    return anim_id;
}

void animation_destroy(int anim_id)
{
    if (anim_id < 0 || anim_id >= MAX_ANIMATIONS) return;
    if (!g_anim_mgr.anims[anim_id].in_use) return;

    g_anim_mgr.anims[anim_id].in_use = false;
    g_anim_mgr.anim_count--;
}

/*
 * ============================================================================
 * Animation Instance Management
 * ============================================================================
 */

int animation_play(int anim_id, float x, float y)
{
    AnimInstance* inst;
    int i;

    if (anim_id < 0 || anim_id >= MAX_ANIMATIONS) return -1;
    if (!g_anim_mgr.anims[anim_id].in_use) return -1;

    /* Find free instance slot */
    for (i = 0; i < MAX_ANIM_INSTANCES; i++) {
        if (!g_anim_mgr.instances[i].in_use) {
            inst = &g_anim_mgr.instances[i];
            memset(inst, 0, sizeof(AnimInstance));
            inst->anim_id = anim_id;
            inst->x = x;
            inst->y = y;
            inst->speed = 1.0f;
            inst->scale_x = 1.0f;
            inst->scale_y = 1.0f;
            inst->alpha = 255;
            inst->playing = true;
            inst->forward = true;
            inst->in_use = true;
            g_anim_mgr.instance_count++;
            return i;
        }
    }

    return -1;
}

void animation_stop(int instance_id)
{
    if (instance_id < 0 || instance_id >= MAX_ANIM_INSTANCES) return;
    g_anim_mgr.instances[instance_id].playing = false;
}

void animation_destroy_instance(int instance_id)
{
    if (instance_id < 0 || instance_id >= MAX_ANIM_INSTANCES) return;
    if (!g_anim_mgr.instances[instance_id].in_use) return;

    g_anim_mgr.instances[instance_id].in_use = false;
    g_anim_mgr.instance_count--;
}

void animation_set_position(int instance_id, float x, float y)
{
    if (instance_id < 0 || instance_id >= MAX_ANIM_INSTANCES) return;
    g_anim_mgr.instances[instance_id].x = x;
    g_anim_mgr.instances[instance_id].y = y;
}

void animation_set_transform(int instance_id, float scale_x, float scale_y,
                              float rotation, u8 alpha)
{
    AnimInstance* inst;
    if (instance_id < 0 || instance_id >= MAX_ANIM_INSTANCES) return;
    inst = &g_anim_mgr.instances[instance_id];
    inst->scale_x = scale_x;
    inst->scale_y = scale_y;
    inst->rotation = rotation;
    inst->alpha = alpha;
}

void animation_set_flip(int instance_id, bool flip_x, bool flip_y)
{
    if (instance_id < 0 || instance_id >= MAX_ANIM_INSTANCES) return;
    g_anim_mgr.instances[instance_id].flip_x = flip_x;
    g_anim_mgr.instances[instance_id].flip_y = flip_y;
}

void animation_set_speed(int instance_id, float speed)
{
    if (instance_id < 0 || instance_id >= MAX_ANIM_INSTANCES) return;
    g_anim_mgr.instances[instance_id].speed = speed;
}

void animation_set_callbacks(int instance_id,
                              void (*on_frame)(int, int, void*),
                              void (*on_complete)(void*),
                              void* user_data)
{
    AnimInstance* inst;
    if (instance_id < 0 || instance_id >= MAX_ANIM_INSTANCES) return;
    inst = &g_anim_mgr.instances[instance_id];
    inst->on_frame = on_frame;
    inst->on_complete = on_complete;
    inst->user_data = user_data;
}

/*
 * ============================================================================
 * Animation Update
 * ============================================================================
 */

void animation_update_all(float delta)
{
    int i;

    for (i = 0; i < MAX_ANIM_INSTANCES; i++) {
        AnimInstance* inst = &g_anim_mgr.instances[i];
        if (!inst->in_use || !inst->playing || inst->finished) continue;

        Animation* anim = &g_anim_mgr.anims[inst->anim_id];
        if (!anim->in_use || anim->frame_count == 0) continue;

        int old_frame = inst->current_frame;
        inst->time += delta * inst->speed;

        /* Calculate current frame */
        float time_in_anim = inst->time;
        float accumulated = 0;
        int frame = 0;

        if (anim->mode == ANIM_PINGPONG && !inst->forward) {
            /* Playing backward */
            for (frame = anim->frame_count - 1; frame >= 0; frame--) {
                accumulated += anim->frames[frame].duration;
                if (accumulated >= time_in_anim) break;
            }
            if (frame < 0) frame = 0;
        } else {
            /* Playing forward */
            for (frame = 0; frame < anim->frame_count; frame++) {
                accumulated += anim->frames[frame].duration;
                if (accumulated >= time_in_anim) break;
            }
        }

        /* Handle animation end */
        if (inst->time >= anim->total_duration) {
            switch (anim->mode) {
                case ANIM_LOOP:
                    inst->time = fmodf(inst->time, anim->total_duration);
                    break;

                case ANIM_PINGPONG:
                    inst->time = 0;
                    inst->forward = !inst->forward;
                    break;

                case ANIM_ONCE:
                    inst->time = anim->total_duration;
                    inst->playing = false;
                    inst->finished = true;
                    frame = anim->frame_count - 1;
                    if (inst->on_complete) {
                        inst->on_complete(inst->user_data);
                    }
                    break;

                case ANIM_ONCE_DESTROY:
                    inst->finished = true;
                    if (inst->on_complete) {
                        inst->on_complete(inst->user_data);
                    }
                    animation_destroy_instance(i);
                    continue;
            }
        }

        inst->current_frame = frame;

        /* Trigger frame event */
        if (frame != old_frame && inst->on_frame) {
            int event_id = anim->frames[frame].event_id;
            inst->on_frame(frame, event_id, inst->user_data);
        }
    }
}

/*
 * ============================================================================
 * Animation Draw
 * ============================================================================
 */

void animation_draw_all(Renderer* renderer, float cam_x, float cam_y, float zoom)
{
    int i;

    for (i = 0; i < MAX_ANIM_INSTANCES; i++) {
        AnimInstance* inst = &g_anim_mgr.instances[i];
        if (!inst->in_use) continue;

        Animation* anim = &g_anim_mgr.anims[inst->anim_id];
        if (!anim->in_use || !anim->texture) continue;

        AnimFrame* frame = &anim->frames[inst->current_frame];

        float screen_x = (inst->x - cam_x) * zoom + renderer->width / 2.0f;
        float screen_y = (inst->y - cam_y) * zoom + renderer->height / 2.0f;

        float draw_w = frame->src_w * inst->scale_x * zoom;
        float draw_h = frame->src_h * inst->scale_y * zoom;

        Rect src = {(float)frame->src_x, (float)frame->src_y,
                   (float)frame->src_w, (float)frame->src_h};
        Rect dst = {screen_x - draw_w / 2 + frame->offset_x * zoom,
                   screen_y - draw_h / 2 + frame->offset_y * zoom,
                   draw_w, draw_h};

        SDL_SetTextureAlphaMod(anim->texture, inst->alpha);
        renderer_draw_texture_ex(renderer, anim->texture, src, dst,
                                inst->rotation, inst->flip_x, inst->flip_y);
        SDL_SetTextureAlphaMod(anim->texture, 255);
    }
}

/*
 * ============================================================================
 * Cleanup
 * ============================================================================
 */

void animation_cleanup(void)
{
    /* Note: textures are not owned by animation system */
    memset(&g_anim_mgr, 0, sizeof(g_anim_mgr));
}

/*
 * ============================================================================
 * Query Functions
 * ============================================================================
 */

bool animation_is_playing(int instance_id)
{
    if (instance_id < 0 || instance_id >= MAX_ANIM_INSTANCES) return false;
    return g_anim_mgr.instances[instance_id].playing;
}

bool animation_is_finished(int instance_id)
{
    if (instance_id < 0 || instance_id >= MAX_ANIM_INSTANCES) return true;
    return g_anim_mgr.instances[instance_id].finished;
}

int animation_get_current_frame(int instance_id)
{
    if (instance_id < 0 || instance_id >= MAX_ANIM_INSTANCES) return 0;
    return g_anim_mgr.instances[instance_id].current_frame;
}

float animation_get_progress(int instance_id)
{
    AnimInstance* inst;
    Animation* anim;

    if (instance_id < 0 || instance_id >= MAX_ANIM_INSTANCES) return 0;
    inst = &g_anim_mgr.instances[instance_id];
    if (!inst->in_use) return 0;

    anim = &g_anim_mgr.anims[inst->anim_id];
    if (!anim->in_use || anim->total_duration <= 0) return 0;

    return inst->time / anim->total_duration;
}
