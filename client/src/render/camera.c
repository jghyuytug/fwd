/**
 * @file camera.c
 * @brief Camera system for viewport control
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

#define CAMERA_SMOOTH_SPEED     5.0f
#define CAMERA_DEADZONE         50.0f
#define CAMERA_LOOKAHEAD        100.0f
#define CAMERA_DEFAULT_ZOOM     1.0f
#define CAMERA_MIN_ZOOM         0.25f
#define CAMERA_MAX_ZOOM         4.0f
#define SHAKE_DECAY             5.0f

/*
 * ============================================================================
 * Camera Types
 * ============================================================================
 */

typedef enum {
    CAMERA_MODE_FREE,           /* No target following */
    CAMERA_MODE_FOLLOW,         /* Follow target with smoothing */
    CAMERA_MODE_LOCKED,         /* Locked to target position */
    CAMERA_MODE_CINEMATIC       /* Scripted movement */
} CameraMode;

typedef struct {
    /* Position */
    float       x;
    float       y;
    float       target_x;
    float       target_y;

    /* Zoom */
    float       zoom;
    float       target_zoom;
    float       zoom_speed;

    /* Bounds */
    float       min_x;
    float       min_y;
    float       max_x;
    float       max_y;
    bool        has_bounds;

    /* Following */
    CameraMode  mode;
    float       follow_speed;
    float       deadzone_x;
    float       deadzone_y;
    float       lookahead_x;
    float       lookahead_y;
    float       offset_x;
    float       offset_y;

    /* Shake */
    float       shake_intensity;
    float       shake_duration;
    float       shake_elapsed;
    float       shake_offset_x;
    float       shake_offset_y;
    float       shake_frequency;

    /* Viewport */
    int         viewport_width;
    int         viewport_height;

    /* Cinematic */
    float       cine_start_x;
    float       cine_start_y;
    float       cine_end_x;
    float       cine_end_y;
    float       cine_duration;
    float       cine_elapsed;
    int         cine_ease_type;
    void        (*cine_callback)(void* user_data);
    void*       cine_user_data;
} Camera;

static Camera g_camera = {0};

/*
 * ============================================================================
 * Easing (from animation.c)
 * ============================================================================
 */

extern float animation_ease(int type, float t);

/*
 * ============================================================================
 * Camera Initialization
 * ============================================================================
 */

void camera_init(int viewport_width, int viewport_height)
{
    memset(&g_camera, 0, sizeof(Camera));
    g_camera.zoom = CAMERA_DEFAULT_ZOOM;
    g_camera.target_zoom = CAMERA_DEFAULT_ZOOM;
    g_camera.zoom_speed = 3.0f;
    g_camera.follow_speed = CAMERA_SMOOTH_SPEED;
    g_camera.deadzone_x = CAMERA_DEADZONE;
    g_camera.deadzone_y = CAMERA_DEADZONE;
    g_camera.viewport_width = viewport_width;
    g_camera.viewport_height = viewport_height;
    g_camera.shake_frequency = 30.0f;
    g_camera.mode = CAMERA_MODE_FREE;
}

void camera_set_viewport(int width, int height)
{
    g_camera.viewport_width = width;
    g_camera.viewport_height = height;
}

/*
 * ============================================================================
 * Position Control
 * ============================================================================
 */

void camera_set_position(float x, float y)
{
    g_camera.x = x;
    g_camera.y = y;
    g_camera.target_x = x;
    g_camera.target_y = y;
}

void camera_move(float dx, float dy)
{
    g_camera.target_x += dx;
    g_camera.target_y += dy;
}

void camera_get_position(float* x, float* y)
{
    *x = g_camera.x + g_camera.shake_offset_x;
    *y = g_camera.y + g_camera.shake_offset_y;
}

/*
 * ============================================================================
 * Zoom Control
 * ============================================================================
 */

void camera_set_zoom(float zoom)
{
    if (zoom < CAMERA_MIN_ZOOM) zoom = CAMERA_MIN_ZOOM;
    if (zoom > CAMERA_MAX_ZOOM) zoom = CAMERA_MAX_ZOOM;
    g_camera.zoom = zoom;
    g_camera.target_zoom = zoom;
}

void camera_zoom_to(float zoom, float speed)
{
    if (zoom < CAMERA_MIN_ZOOM) zoom = CAMERA_MIN_ZOOM;
    if (zoom > CAMERA_MAX_ZOOM) zoom = CAMERA_MAX_ZOOM;
    g_camera.target_zoom = zoom;
    g_camera.zoom_speed = speed;
}

float camera_get_zoom(void)
{
    return g_camera.zoom;
}

/*
 * ============================================================================
 * Bounds
 * ============================================================================
 */

void camera_set_bounds(float min_x, float min_y, float max_x, float max_y)
{
    g_camera.min_x = min_x;
    g_camera.min_y = min_y;
    g_camera.max_x = max_x;
    g_camera.max_y = max_y;
    g_camera.has_bounds = true;
}

void camera_clear_bounds(void)
{
    g_camera.has_bounds = false;
}

static void apply_bounds(void)
{
    if (!g_camera.has_bounds) return;

    float half_w = g_camera.viewport_width / (2.0f * g_camera.zoom);
    float half_h = g_camera.viewport_height / (2.0f * g_camera.zoom);

    /* Clamp target */
    if (g_camera.target_x - half_w < g_camera.min_x) {
        g_camera.target_x = g_camera.min_x + half_w;
    }
    if (g_camera.target_x + half_w > g_camera.max_x) {
        g_camera.target_x = g_camera.max_x - half_w;
    }
    if (g_camera.target_y - half_h < g_camera.min_y) {
        g_camera.target_y = g_camera.min_y + half_h;
    }
    if (g_camera.target_y + half_h > g_camera.max_y) {
        g_camera.target_y = g_camera.max_y - half_h;
    }

    /* Handle case where viewport is larger than bounds */
    if (g_camera.max_x - g_camera.min_x < g_camera.viewport_width / g_camera.zoom) {
        g_camera.target_x = (g_camera.min_x + g_camera.max_x) / 2;
    }
    if (g_camera.max_y - g_camera.min_y < g_camera.viewport_height / g_camera.zoom) {
        g_camera.target_y = (g_camera.min_y + g_camera.max_y) / 2;
    }
}

/*
 * ============================================================================
 * Following
 * ============================================================================
 */

void camera_set_mode(CameraMode mode)
{
    g_camera.mode = mode;
}

void camera_follow(float target_x, float target_y, float dir_x, float dir_y)
{
    float offset_x = g_camera.offset_x + g_camera.lookahead_x * dir_x;
    float offset_y = g_camera.offset_y + g_camera.lookahead_y * dir_y;

    switch (g_camera.mode) {
        case CAMERA_MODE_LOCKED:
            g_camera.target_x = target_x + offset_x;
            g_camera.target_y = target_y + offset_y;
            g_camera.x = g_camera.target_x;
            g_camera.y = g_camera.target_y;
            break;

        case CAMERA_MODE_FOLLOW:
            {
                float dx = target_x + offset_x - g_camera.target_x;
                float dy = target_y + offset_y - g_camera.target_y;

                /* Apply deadzone */
                if (fabsf(dx) > g_camera.deadzone_x) {
                    g_camera.target_x += dx - (dx > 0 ? g_camera.deadzone_x : -g_camera.deadzone_x);
                }
                if (fabsf(dy) > g_camera.deadzone_y) {
                    g_camera.target_y += dy - (dy > 0 ? g_camera.deadzone_y : -g_camera.deadzone_y);
                }
            }
            break;

        case CAMERA_MODE_CINEMATIC:
            /* Don't update target during cinematic */
            break;

        default:
            break;
    }

    apply_bounds();
}

void camera_set_follow_params(float speed, float deadzone_x, float deadzone_y,
                               float lookahead_x, float lookahead_y)
{
    g_camera.follow_speed = speed;
    g_camera.deadzone_x = deadzone_x;
    g_camera.deadzone_y = deadzone_y;
    g_camera.lookahead_x = lookahead_x;
    g_camera.lookahead_y = lookahead_y;
}

void camera_set_offset(float offset_x, float offset_y)
{
    g_camera.offset_x = offset_x;
    g_camera.offset_y = offset_y;
}

/*
 * ============================================================================
 * Shake
 * ============================================================================
 */

void camera_shake(float intensity, float duration)
{
    g_camera.shake_intensity = intensity;
    g_camera.shake_duration = duration;
    g_camera.shake_elapsed = 0;
}

void camera_shake_add(float intensity, float duration)
{
    /* Add to existing shake */
    if (g_camera.shake_elapsed < g_camera.shake_duration) {
        g_camera.shake_intensity += intensity;
        if (duration > g_camera.shake_duration - g_camera.shake_elapsed) {
            g_camera.shake_duration = g_camera.shake_elapsed + duration;
        }
    } else {
        camera_shake(intensity, duration);
    }
}

void camera_stop_shake(void)
{
    g_camera.shake_intensity = 0;
    g_camera.shake_offset_x = 0;
    g_camera.shake_offset_y = 0;
}

/*
 * ============================================================================
 * Cinematic
 * ============================================================================
 */

void camera_move_to(float x, float y, float duration, int ease_type,
                    void (*callback)(void*), void* user_data)
{
    g_camera.mode = CAMERA_MODE_CINEMATIC;
    g_camera.cine_start_x = g_camera.x;
    g_camera.cine_start_y = g_camera.y;
    g_camera.cine_end_x = x;
    g_camera.cine_end_y = y;
    g_camera.cine_duration = duration;
    g_camera.cine_elapsed = 0;
    g_camera.cine_ease_type = ease_type;
    g_camera.cine_callback = callback;
    g_camera.cine_user_data = user_data;
}

/*
 * ============================================================================
 * Update
 * ============================================================================
 */

void camera_update(float delta)
{
    /* Update cinematic movement */
    if (g_camera.mode == CAMERA_MODE_CINEMATIC) {
        g_camera.cine_elapsed += delta;
        float t = g_camera.cine_elapsed / g_camera.cine_duration;

        if (t >= 1.0f) {
            t = 1.0f;
            g_camera.x = g_camera.cine_end_x;
            g_camera.y = g_camera.cine_end_y;
            g_camera.mode = CAMERA_MODE_FREE;

            if (g_camera.cine_callback) {
                g_camera.cine_callback(g_camera.cine_user_data);
            }
        } else {
            float eased_t = animation_ease(g_camera.cine_ease_type, t);
            g_camera.x = g_camera.cine_start_x +
                        (g_camera.cine_end_x - g_camera.cine_start_x) * eased_t;
            g_camera.y = g_camera.cine_start_y +
                        (g_camera.cine_end_y - g_camera.cine_start_y) * eased_t;
        }
    }
    else if (g_camera.mode != CAMERA_MODE_LOCKED) {
        /* Smooth follow */
        float dx = g_camera.target_x - g_camera.x;
        float dy = g_camera.target_y - g_camera.y;

        g_camera.x += dx * g_camera.follow_speed * delta;
        g_camera.y += dy * g_camera.follow_speed * delta;
    }

    /* Update zoom */
    if (g_camera.zoom != g_camera.target_zoom) {
        float dz = g_camera.target_zoom - g_camera.zoom;
        g_camera.zoom += dz * g_camera.zoom_speed * delta;

        /* Snap if close enough */
        if (fabsf(g_camera.zoom - g_camera.target_zoom) < 0.01f) {
            g_camera.zoom = g_camera.target_zoom;
        }
    }

    /* Update shake */
    if (g_camera.shake_elapsed < g_camera.shake_duration) {
        g_camera.shake_elapsed += delta;
        float t = g_camera.shake_elapsed / g_camera.shake_duration;
        float intensity = g_camera.shake_intensity * (1.0f - t);

        float time = g_camera.shake_elapsed * g_camera.shake_frequency;
        g_camera.shake_offset_x = sinf(time * 1.1f) * intensity;
        g_camera.shake_offset_y = cosf(time * 1.3f) * intensity;
    } else {
        g_camera.shake_offset_x *= (1.0f - SHAKE_DECAY * delta);
        g_camera.shake_offset_y *= (1.0f - SHAKE_DECAY * delta);
    }
}

/*
 * ============================================================================
 * Coordinate Conversion
 * ============================================================================
 */

void camera_screen_to_world(int screen_x, int screen_y, float* world_x, float* world_y)
{
    float cam_x = g_camera.x + g_camera.shake_offset_x;
    float cam_y = g_camera.y + g_camera.shake_offset_y;

    *world_x = cam_x + (screen_x - g_camera.viewport_width / 2.0f) / g_camera.zoom;
    *world_y = cam_y + (screen_y - g_camera.viewport_height / 2.0f) / g_camera.zoom;
}

void camera_world_to_screen(float world_x, float world_y, int* screen_x, int* screen_y)
{
    float cam_x = g_camera.x + g_camera.shake_offset_x;
    float cam_y = g_camera.y + g_camera.shake_offset_y;

    *screen_x = (int)((world_x - cam_x) * g_camera.zoom + g_camera.viewport_width / 2.0f);
    *screen_y = (int)((world_y - cam_y) * g_camera.zoom + g_camera.viewport_height / 2.0f);
}

/*
 * ============================================================================
 * Visibility
 * ============================================================================
 */

bool camera_is_visible(float x, float y, float width, float height)
{
    float cam_x = g_camera.x + g_camera.shake_offset_x;
    float cam_y = g_camera.y + g_camera.shake_offset_y;

    float half_w = g_camera.viewport_width / (2.0f * g_camera.zoom);
    float half_h = g_camera.viewport_height / (2.0f * g_camera.zoom);

    float left = cam_x - half_w;
    float right = cam_x + half_w;
    float top = cam_y - half_h;
    float bottom = cam_y + half_h;

    return !(x + width < left || x > right || y + height < top || y > bottom);
}

Rect camera_get_visible_rect(void)
{
    float cam_x = g_camera.x + g_camera.shake_offset_x;
    float cam_y = g_camera.y + g_camera.shake_offset_y;

    float half_w = g_camera.viewport_width / (2.0f * g_camera.zoom);
    float half_h = g_camera.viewport_height / (2.0f * g_camera.zoom);

    Rect rect = {cam_x - half_w, cam_y - half_h, half_w * 2, half_h * 2};
    return rect;
}

/*
 * ============================================================================
 * Debug
 * ============================================================================
 */

void camera_debug_draw(Renderer* renderer)
{
    /* Draw crosshair at camera center */
    int cx = g_camera.viewport_width / 2;
    int cy = g_camera.viewport_height / 2;

    Rect h_line = {(float)(cx - 10), (float)cy, 20, 1};
    Rect v_line = {(float)cx, (float)(cy - 10), 1, 20};
    Color white = {255, 255, 255, 128};

    renderer_draw_rect(renderer, h_line, white, true);
    renderer_draw_rect(renderer, v_line, white, true);

    /* Draw deadzone */
    if (g_camera.mode == CAMERA_MODE_FOLLOW) {
        float dz_w = g_camera.deadzone_x * g_camera.zoom;
        float dz_h = g_camera.deadzone_y * g_camera.zoom;
        Rect deadzone = {cx - dz_w, cy - dz_h, dz_w * 2, dz_h * 2};
        Color yellow = {255, 255, 0, 50};
        renderer_draw_rect(renderer, deadzone, yellow, true);
    }
}
