/**
 * @file effects.c
 * @brief Visual effects system - particles, skill effects, screen effects
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

#define MAX_PARTICLES       1024
#define MAX_PARTICLE_EMITTERS 64
#define MAX_SKILL_EFFECTS   32
#define MAX_SCREEN_EFFECTS  8

/*
 * ============================================================================
 * Particle Types
 * ============================================================================
 */

typedef enum {
    PARTICLE_POINT,
    PARTICLE_SQUARE,
    PARTICLE_CIRCLE,
    PARTICLE_SPRITE,
    PARTICLE_TRAIL
} ParticleType;

typedef struct {
    float   x, y;
    float   vx, vy;
    float   ax, ay;         /* Acceleration */
    float   life;
    float   max_life;
    float   size;
    float   size_end;
    float   rotation;
    float   rotation_speed;
    Color   color;
    Color   color_end;
    u8      type;
    bool    active;
} Particle;

/*
 * ============================================================================
 * Particle Emitter
 * ============================================================================
 */

typedef struct {
    bool    active;
    float   x, y;
    float   width, height;  /* Spawn area */

    /* Emission */
    float   emit_rate;      /* Particles per second */
    float   emit_timer;
    int     burst_count;    /* 0 = continuous */

    /* Particle properties */
    ParticleType type;
    float   life_min, life_max;
    float   size_min, size_max;
    float   size_end_min, size_end_max;
    float   speed_min, speed_max;
    float   dir_min, dir_max;       /* Direction in radians */
    float   spread;                 /* Random spread */
    float   gravity;
    float   rotation_min, rotation_max;
    Color   color_start;
    Color   color_end;
    SDL_Texture* texture;

    /* Lifetime */
    float   duration;       /* 0 = infinite */
    float   elapsed;
} ParticleEmitter;

/*
 * ============================================================================
 * Skill Effect
 * ============================================================================
 */

typedef struct {
    bool    active;
    u32     skill_id;
    float   x, y;
    float   dir;
    float   elapsed;
    float   duration;

    /* Visual */
    SDL_Texture* texture;
    int     frame_count;
    int     current_frame;
    float   frame_time;
    float   scale;
    u8      alpha;

    /* Hitbox (for visual only, not collision) */
    float   hit_x, hit_y;
    float   hit_w, hit_h;
} SkillEffect;

/*
 * ============================================================================
 * Screen Effect
 * ============================================================================
 */

typedef enum {
    SCREEN_FLASH,
    SCREEN_SHAKE,
    SCREEN_FADE_IN,
    SCREEN_FADE_OUT,
    SCREEN_TINT,
    SCREEN_BLUR         /* Simulated with overlay */
} ScreenEffectType;

typedef struct {
    bool            active;
    ScreenEffectType type;
    float           duration;
    float           elapsed;
    float           intensity;
    Color           color;
    /* Shake specific */
    float           shake_x;
    float           shake_y;
} ScreenEffect;

/*
 * ============================================================================
 * Effects Manager
 * ============================================================================
 */

static struct {
    Particle        particles[MAX_PARTICLES];
    ParticleEmitter emitters[MAX_PARTICLE_EMITTERS];
    SkillEffect     skills[MAX_SKILL_EFFECTS];
    ScreenEffect    screens[MAX_SCREEN_EFFECTS];
    int             particle_count;
    int             emitter_count;

    /* Screen effect output */
    float           screen_shake_x;
    float           screen_shake_y;
    Color           screen_tint;
    u8              screen_fade;
} g_effects = {0};

/*
 * ============================================================================
 * Helper Functions
 * ============================================================================
 */

static float randf(void)
{
    return (float)rand() / (float)RAND_MAX;
}

static float randf_range(float min, float max)
{
    return min + randf() * (max - min);
}

static Color lerp_color(Color a, Color b, float t)
{
    Color result;
    result.r = (u8)(a.r + (b.r - a.r) * t);
    result.g = (u8)(a.g + (b.g - a.g) * t);
    result.b = (u8)(a.b + (b.b - a.b) * t);
    result.a = (u8)(a.a + (b.a - a.a) * t);
    return result;
}

/*
 * ============================================================================
 * Particle Functions
 * ============================================================================
 */

static Particle* particle_spawn(void)
{
    int i;
    for (i = 0; i < MAX_PARTICLES; i++) {
        if (!g_effects.particles[i].active) {
            g_effects.particles[i].active = true;
            g_effects.particle_count++;
            return &g_effects.particles[i];
        }
    }
    return NULL;
}

void particle_emit(float x, float y, float vx, float vy, float life,
                   float size, Color color, ParticleType type)
{
    Particle* p = particle_spawn();
    if (!p) return;

    p->x = x;
    p->y = y;
    p->vx = vx;
    p->vy = vy;
    p->ax = 0;
    p->ay = 0;
    p->life = life;
    p->max_life = life;
    p->size = size;
    p->size_end = 0;
    p->rotation = 0;
    p->rotation_speed = 0;
    p->color = color;
    p->color_end = color;
    p->color_end.a = 0;
    p->type = type;
}

void particle_burst(float x, float y, int count, float speed,
                    float life, float size, Color color)
{
    int i;
    for (i = 0; i < count; i++) {
        float angle = randf() * 6.28318f;
        float spd = speed * (0.5f + randf() * 0.5f);
        particle_emit(x, y, cosf(angle) * spd, sinf(angle) * spd,
                     life * (0.8f + randf() * 0.4f),
                     size * (0.8f + randf() * 0.4f),
                     color, PARTICLE_CIRCLE);
    }
}

/*
 * ============================================================================
 * Emitter Functions
 * ============================================================================
 */

int emitter_create(float x, float y, float emit_rate)
{
    int i;
    for (i = 0; i < MAX_PARTICLE_EMITTERS; i++) {
        if (!g_effects.emitters[i].active) {
            ParticleEmitter* e = &g_effects.emitters[i];
            memset(e, 0, sizeof(ParticleEmitter));
            e->active = true;
            e->x = x;
            e->y = y;
            e->emit_rate = emit_rate;
            e->life_min = 0.5f;
            e->life_max = 1.5f;
            e->size_min = 5;
            e->size_max = 10;
            e->speed_min = 50;
            e->speed_max = 100;
            e->dir_min = 0;
            e->dir_max = 6.28318f;
            e->color_start = (Color){255, 255, 255, 255};
            e->color_end = (Color){255, 255, 255, 0};
            e->type = PARTICLE_CIRCLE;
            g_effects.emitter_count++;
            return i;
        }
    }
    return -1;
}

void emitter_set_position(int id, float x, float y)
{
    if (id < 0 || id >= MAX_PARTICLE_EMITTERS) return;
    g_effects.emitters[id].x = x;
    g_effects.emitters[id].y = y;
}

void emitter_set_direction(int id, float dir_min, float dir_max)
{
    if (id < 0 || id >= MAX_PARTICLE_EMITTERS) return;
    g_effects.emitters[id].dir_min = dir_min;
    g_effects.emitters[id].dir_max = dir_max;
}

void emitter_set_life(int id, float min, float max)
{
    if (id < 0 || id >= MAX_PARTICLE_EMITTERS) return;
    g_effects.emitters[id].life_min = min;
    g_effects.emitters[id].life_max = max;
}

void emitter_set_size(int id, float min, float max, float end_min, float end_max)
{
    if (id < 0 || id >= MAX_PARTICLE_EMITTERS) return;
    ParticleEmitter* e = &g_effects.emitters[id];
    e->size_min = min;
    e->size_max = max;
    e->size_end_min = end_min;
    e->size_end_max = end_max;
}

void emitter_set_speed(int id, float min, float max)
{
    if (id < 0 || id >= MAX_PARTICLE_EMITTERS) return;
    g_effects.emitters[id].speed_min = min;
    g_effects.emitters[id].speed_max = max;
}

void emitter_set_colors(int id, Color start, Color end)
{
    if (id < 0 || id >= MAX_PARTICLE_EMITTERS) return;
    g_effects.emitters[id].color_start = start;
    g_effects.emitters[id].color_end = end;
}

void emitter_set_gravity(int id, float gravity)
{
    if (id < 0 || id >= MAX_PARTICLE_EMITTERS) return;
    g_effects.emitters[id].gravity = gravity;
}

void emitter_set_duration(int id, float duration)
{
    if (id < 0 || id >= MAX_PARTICLE_EMITTERS) return;
    g_effects.emitters[id].duration = duration;
}

void emitter_burst(int id, int count)
{
    if (id < 0 || id >= MAX_PARTICLE_EMITTERS) return;
    g_effects.emitters[id].burst_count = count;
}

void emitter_destroy(int id)
{
    if (id < 0 || id >= MAX_PARTICLE_EMITTERS) return;
    if (g_effects.emitters[id].active) {
        g_effects.emitters[id].active = false;
        g_effects.emitter_count--;
    }
}

static void emitter_spawn_particle(ParticleEmitter* e)
{
    Particle* p = particle_spawn();
    if (!p) return;

    float dir = randf_range(e->dir_min, e->dir_max);
    float speed = randf_range(e->speed_min, e->speed_max);

    p->x = e->x + randf_range(-e->width/2, e->width/2);
    p->y = e->y + randf_range(-e->height/2, e->height/2);
    p->vx = cosf(dir) * speed;
    p->vy = sinf(dir) * speed;
    p->ax = 0;
    p->ay = e->gravity;
    p->life = randf_range(e->life_min, e->life_max);
    p->max_life = p->life;
    p->size = randf_range(e->size_min, e->size_max);
    p->size_end = randf_range(e->size_end_min, e->size_end_max);
    p->rotation = randf_range(e->rotation_min, e->rotation_max);
    p->rotation_speed = randf_range(-3, 3);
    p->color = e->color_start;
    p->color_end = e->color_end;
    p->type = e->type;
}

/*
 * ============================================================================
 * Skill Effect Functions
 * ============================================================================
 */

int skill_effect_spawn(u32 skill_id, float x, float y, float dir,
                       SDL_Texture* texture, int frame_count, float duration)
{
    int i;
    for (i = 0; i < MAX_SKILL_EFFECTS; i++) {
        if (!g_effects.skills[i].active) {
            SkillEffect* s = &g_effects.skills[i];
            memset(s, 0, sizeof(SkillEffect));
            s->active = true;
            s->skill_id = skill_id;
            s->x = x;
            s->y = y;
            s->dir = dir;
            s->duration = duration;
            s->texture = texture;
            s->frame_count = frame_count > 0 ? frame_count : 1;
            s->scale = 1.0f;
            s->alpha = 255;
            return i;
        }
    }
    return -1;
}

void skill_effect_set_scale(int id, float scale)
{
    if (id < 0 || id >= MAX_SKILL_EFFECTS) return;
    g_effects.skills[id].scale = scale;
}

/*
 * ============================================================================
 * Screen Effect Functions
 * ============================================================================
 */

void screen_flash(Color color, float duration)
{
    int i;
    for (i = 0; i < MAX_SCREEN_EFFECTS; i++) {
        if (!g_effects.screens[i].active) {
            ScreenEffect* s = &g_effects.screens[i];
            s->active = true;
            s->type = SCREEN_FLASH;
            s->color = color;
            s->duration = duration;
            s->elapsed = 0;
            s->intensity = 1.0f;
            return;
        }
    }
}

void screen_shake(float intensity, float duration)
{
    int i;
    for (i = 0; i < MAX_SCREEN_EFFECTS; i++) {
        if (!g_effects.screens[i].active) {
            ScreenEffect* s = &g_effects.screens[i];
            s->active = true;
            s->type = SCREEN_SHAKE;
            s->duration = duration;
            s->elapsed = 0;
            s->intensity = intensity;
            return;
        }
    }
}

void screen_fade(bool fade_in, float duration, Color color)
{
    int i;
    for (i = 0; i < MAX_SCREEN_EFFECTS; i++) {
        if (!g_effects.screens[i].active) {
            ScreenEffect* s = &g_effects.screens[i];
            s->active = true;
            s->type = fade_in ? SCREEN_FADE_IN : SCREEN_FADE_OUT;
            s->color = color;
            s->duration = duration;
            s->elapsed = 0;
            s->intensity = 1.0f;
            return;
        }
    }
}

void screen_tint(Color color, float duration)
{
    int i;
    for (i = 0; i < MAX_SCREEN_EFFECTS; i++) {
        if (!g_effects.screens[i].active) {
            ScreenEffect* s = &g_effects.screens[i];
            s->active = true;
            s->type = SCREEN_TINT;
            s->color = color;
            s->duration = duration;
            s->elapsed = 0;
            s->intensity = 1.0f;
            return;
        }
    }
}

/*
 * ============================================================================
 * Update
 * ============================================================================
 */

void effects_update(float delta)
{
    int i;

    /* Reset screen effects output */
    g_effects.screen_shake_x = 0;
    g_effects.screen_shake_y = 0;
    g_effects.screen_tint = (Color){0, 0, 0, 0};
    g_effects.screen_fade = 0;

    /* Update particles */
    for (i = 0; i < MAX_PARTICLES; i++) {
        Particle* p = &g_effects.particles[i];
        if (!p->active) continue;

        p->life -= delta;
        if (p->life <= 0) {
            p->active = false;
            g_effects.particle_count--;
            continue;
        }

        p->vx += p->ax * delta;
        p->vy += p->ay * delta;
        p->x += p->vx * delta;
        p->y += p->vy * delta;
        p->rotation += p->rotation_speed * delta;
    }

    /* Update emitters */
    for (i = 0; i < MAX_PARTICLE_EMITTERS; i++) {
        ParticleEmitter* e = &g_effects.emitters[i];
        if (!e->active) continue;

        /* Burst mode */
        if (e->burst_count > 0) {
            int j;
            for (j = 0; j < e->burst_count; j++) {
                emitter_spawn_particle(e);
            }
            e->burst_count = 0;
        }

        /* Continuous emission */
        if (e->emit_rate > 0) {
            e->emit_timer += delta;
            float interval = 1.0f / e->emit_rate;
            while (e->emit_timer >= interval) {
                emitter_spawn_particle(e);
                e->emit_timer -= interval;
            }
        }

        /* Duration */
        if (e->duration > 0) {
            e->elapsed += delta;
            if (e->elapsed >= e->duration) {
                e->active = false;
                g_effects.emitter_count--;
            }
        }
    }

    /* Update skill effects */
    for (i = 0; i < MAX_SKILL_EFFECTS; i++) {
        SkillEffect* s = &g_effects.skills[i];
        if (!s->active) continue;

        s->elapsed += delta;
        if (s->elapsed >= s->duration) {
            s->active = false;
            continue;
        }

        /* Update frame */
        if (s->frame_count > 1) {
            float frame_dur = s->duration / s->frame_count;
            s->current_frame = (int)(s->elapsed / frame_dur);
            if (s->current_frame >= s->frame_count) {
                s->current_frame = s->frame_count - 1;
            }
        }
    }

    /* Update screen effects */
    for (i = 0; i < MAX_SCREEN_EFFECTS; i++) {
        ScreenEffect* s = &g_effects.screens[i];
        if (!s->active) continue;

        s->elapsed += delta;
        float progress = s->elapsed / s->duration;
        if (progress > 1.0f) progress = 1.0f;

        switch (s->type) {
            case SCREEN_SHAKE:
                {
                    float intensity = s->intensity * (1.0f - progress);
                    g_effects.screen_shake_x += (randf() * 2 - 1) * intensity * 10;
                    g_effects.screen_shake_y += (randf() * 2 - 1) * intensity * 10;
                }
                break;

            case SCREEN_FLASH:
                {
                    u8 alpha = (u8)(s->color.a * (1.0f - progress));
                    g_effects.screen_tint = s->color;
                    g_effects.screen_tint.a = alpha;
                }
                break;

            case SCREEN_FADE_IN:
                g_effects.screen_fade = (u8)(255 * (1.0f - progress));
                g_effects.screen_tint = s->color;
                break;

            case SCREEN_FADE_OUT:
                g_effects.screen_fade = (u8)(255 * progress);
                g_effects.screen_tint = s->color;
                break;

            case SCREEN_TINT:
                g_effects.screen_tint = s->color;
                g_effects.screen_tint.a = (u8)(s->color.a * (1.0f - progress));
                break;

            default:
                break;
        }

        if (s->elapsed >= s->duration) {
            s->active = false;
        }
    }
}

/*
 * ============================================================================
 * Draw
 * ============================================================================
 */

void effects_draw_particles(Renderer* renderer, float cam_x, float cam_y, float zoom)
{
    int i;

    for (i = 0; i < MAX_PARTICLES; i++) {
        Particle* p = &g_effects.particles[i];
        if (!p->active) continue;

        float t = 1.0f - (p->life / p->max_life);
        float size = p->size + (p->size_end - p->size) * t;
        Color color = lerp_color(p->color, p->color_end, t);

        float screen_x = (p->x - cam_x) * zoom + renderer->width / 2.0f;
        float screen_y = (p->y - cam_y) * zoom + renderer->height / 2.0f;
        float draw_size = size * zoom;

        Rect rect = {screen_x - draw_size/2, screen_y - draw_size/2,
                    draw_size, draw_size};
        renderer_draw_rect(renderer, rect, color, true);
    }
}

void effects_draw_skills(Renderer* renderer, float cam_x, float cam_y, float zoom)
{
    int i;

    for (i = 0; i < MAX_SKILL_EFFECTS; i++) {
        SkillEffect* s = &g_effects.skills[i];
        if (!s->active) continue;

        float screen_x = (s->x - cam_x) * zoom + renderer->width / 2.0f;
        float screen_y = (s->y - cam_y) * zoom + renderer->height / 2.0f;

        if (s->texture) {
            int tex_w, tex_h;
            SDL_QueryTexture(s->texture, NULL, NULL, &tex_w, &tex_h);

            int frame_w = tex_w / s->frame_count;
            float draw_w = frame_w * s->scale * zoom;
            float draw_h = tex_h * s->scale * zoom;

            Rect src = {(float)(s->current_frame * frame_w), 0,
                       (float)frame_w, (float)tex_h};
            Rect dst = {screen_x - draw_w/2, screen_y - draw_h/2, draw_w, draw_h};

            SDL_SetTextureAlphaMod(s->texture, s->alpha);
            renderer_draw_texture_ex(renderer, s->texture, src, dst,
                                    s->dir * 57.2958f, false, false);
            SDL_SetTextureAlphaMod(s->texture, 255);
        } else {
            /* Fallback: draw circle */
            float radius = 50 * s->scale * zoom;
            Rect rect = {screen_x - radius, screen_y - radius, radius * 2, radius * 2};
            Color color = {255, 200, 100, (u8)(s->alpha * (1.0f - s->elapsed / s->duration))};
            renderer_draw_rect(renderer, rect, color, true);
        }
    }
}

void effects_draw_screen(Renderer* renderer)
{
    /* Draw screen tint/flash/fade */
    if (g_effects.screen_tint.a > 0) {
        Rect full = {0, 0, (float)renderer->width, (float)renderer->height};
        renderer_draw_rect(renderer, full, g_effects.screen_tint, true);
    }

    if (g_effects.screen_fade > 0) {
        Color fade = g_effects.screen_tint;
        fade.a = g_effects.screen_fade;
        Rect full = {0, 0, (float)renderer->width, (float)renderer->height};
        renderer_draw_rect(renderer, full, fade, true);
    }
}

void effects_get_screen_shake(float* shake_x, float* shake_y)
{
    *shake_x = g_effects.screen_shake_x;
    *shake_y = g_effects.screen_shake_y;
}

/*
 * ============================================================================
 * Cleanup
 * ============================================================================
 */

void effects_clear(void)
{
    memset(&g_effects, 0, sizeof(g_effects));
}

void effects_cleanup(void)
{
    effects_clear();
}
