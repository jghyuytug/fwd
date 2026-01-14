/**
 * @file entity_renderer.c
 * @brief Entity rendering system for players, monsters, NPCs
 */

#include "../../include/renderer.h"
#include "../../include/entity.h"
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

#define MAX_ENTITY_SPRITES  128
#define MAX_RENDER_ENTITIES 256
#define SHADOW_ALPHA        100
#define NAMEPLATE_OFFSET    10
#define HP_BAR_WIDTH        50
#define HP_BAR_HEIGHT       6

/*
 * ============================================================================
 * Entity Sprite Data
 * ============================================================================
 */

typedef struct {
    u32         template_id;
    SDL_Texture* texture;
    int         frame_width;
    int         frame_height;
    int         frame_count;
    int         anim_idle_start;
    int         anim_idle_count;
    int         anim_walk_start;
    int         anim_walk_count;
    int         anim_attack_start;
    int         anim_attack_count;
    int         anim_hurt_start;
    int         anim_hurt_count;
    int         anim_die_start;
    int         anim_die_count;
    /* Anchor point */
    float       anchor_x;   /* 0.0 = left, 0.5 = center, 1.0 = right */
    float       anchor_y;   /* 0.0 = top, 0.5 = center, 1.0 = bottom */
} EntitySprite;

typedef struct {
    EntitySprite    sprites[MAX_ENTITY_SPRITES];
    int             count;
    /* Default colors for entities without sprites */
    Color           player_color;
    Color           monster_color;
    Color           npc_color;
    Color           pet_color;
} EntitySpriteManager;

static EntitySpriteManager g_entity_sprites = {0};

/*
 * ============================================================================
 * Sprite Loading
 * ============================================================================
 */

int entity_sprite_load(Renderer* renderer, u32 template_id, const char* path,
                       int frame_w, int frame_h, int frame_count)
{
    EntitySprite* sprite;
    int tex_w, tex_h;

    if (g_entity_sprites.count >= MAX_ENTITY_SPRITES) {
        fprintf(stderr, "Entity sprite limit reached\n");
        return -1;
    }

    sprite = &g_entity_sprites.sprites[g_entity_sprites.count];
    sprite->template_id = template_id;
    sprite->frame_width = frame_w;
    sprite->frame_height = frame_h;
    sprite->frame_count = frame_count;

    sprite->texture = renderer_load_texture(renderer, path);
    if (!sprite->texture) {
        fprintf(stderr, "Failed to load entity sprite: %s\n", path);
        return -1;
    }

    SDL_QueryTexture(sprite->texture, NULL, NULL, &tex_w, &tex_h);

    /* Default animation ranges */
    sprite->anim_idle_start = 0;
    sprite->anim_idle_count = 4;
    sprite->anim_walk_start = 4;
    sprite->anim_walk_count = 8;
    sprite->anim_attack_start = 12;
    sprite->anim_attack_count = 6;
    sprite->anim_hurt_start = 18;
    sprite->anim_hurt_count = 2;
    sprite->anim_die_start = 20;
    sprite->anim_die_count = 4;

    /* Default anchor at bottom center */
    sprite->anchor_x = 0.5f;
    sprite->anchor_y = 1.0f;

    printf("Loaded entity sprite %u: %s (%d frames)\n",
           template_id, path, frame_count);

    return g_entity_sprites.count++;
}

void entity_sprite_set_animation(u32 template_id, EntityState state,
                                  int start_frame, int frame_count)
{
    int i;
    for (i = 0; i < g_entity_sprites.count; i++) {
        if (g_entity_sprites.sprites[i].template_id == template_id) {
            EntitySprite* s = &g_entity_sprites.sprites[i];
            switch (state) {
                case ENTITY_STATE_IDLE:
                    s->anim_idle_start = start_frame;
                    s->anim_idle_count = frame_count;
                    break;
                case ENTITY_STATE_WALKING:
                    s->anim_walk_start = start_frame;
                    s->anim_walk_count = frame_count;
                    break;
                case ENTITY_STATE_ATTACKING:
                    s->anim_attack_start = start_frame;
                    s->anim_attack_count = frame_count;
                    break;
                case ENTITY_STATE_HURT:
                    s->anim_hurt_start = start_frame;
                    s->anim_hurt_count = frame_count;
                    break;
                case ENTITY_STATE_DEAD:
                    s->anim_die_start = start_frame;
                    s->anim_die_count = frame_count;
                    break;
                default:
                    break;
            }
            return;
        }
    }
}

EntitySprite* entity_sprite_get(u32 template_id)
{
    int i;
    for (i = 0; i < g_entity_sprites.count; i++) {
        if (g_entity_sprites.sprites[i].template_id == template_id) {
            return &g_entity_sprites.sprites[i];
        }
    }
    return NULL;
}

void entity_sprite_init(void)
{
    memset(&g_entity_sprites, 0, sizeof(EntitySpriteManager));
    g_entity_sprites.player_color = (Color){100, 150, 200, 255};
    g_entity_sprites.monster_color = (Color){200, 100, 100, 255};
    g_entity_sprites.npc_color = (Color){100, 200, 100, 255};
    g_entity_sprites.pet_color = (Color){200, 200, 100, 255};
}

void entity_sprite_cleanup(void)
{
    int i;
    for (i = 0; i < g_entity_sprites.count; i++) {
        if (g_entity_sprites.sprites[i].texture) {
            SDL_DestroyTexture(g_entity_sprites.sprites[i].texture);
        }
    }
    g_entity_sprites.count = 0;
}

/*
 * ============================================================================
 * Entity Renderer
 * ============================================================================
 */

typedef struct {
    Renderer*   renderer;
    float       camera_x;
    float       camera_y;
    float       zoom;
    bool        show_hitboxes;
    bool        show_names;
    bool        show_hp_bars;
} EntityRenderer;

static EntityRenderer g_entity_renderer = {0};

void entity_renderer_init(Renderer* renderer)
{
    memset(&g_entity_renderer, 0, sizeof(EntityRenderer));
    g_entity_renderer.renderer = renderer;
    g_entity_renderer.zoom = 1.0f;
    g_entity_renderer.show_names = true;
    g_entity_renderer.show_hp_bars = true;
    entity_sprite_init();
}

void entity_renderer_set_camera(float x, float y, float zoom)
{
    g_entity_renderer.camera_x = x;
    g_entity_renderer.camera_y = y;
    g_entity_renderer.zoom = zoom;
}

static void get_screen_pos(float world_x, float world_y, float* screen_x, float* screen_y)
{
    Renderer* r = g_entity_renderer.renderer;
    float zoom = g_entity_renderer.zoom;

    *screen_x = (world_x - g_entity_renderer.camera_x) * zoom + r->width / 2.0f;
    *screen_y = (world_y - g_entity_renderer.camera_y) * zoom + r->height / 2.0f;
}

static void get_entity_frame(Entity* entity, EntitySprite* sprite, int* frame)
{
    float anim_time = entity->anim_time;
    int start, count;

    switch (entity->state) {
        case ENTITY_STATE_WALKING:
        case ENTITY_STATE_RUNNING:
            start = sprite->anim_walk_start;
            count = sprite->anim_walk_count;
            break;
        case ENTITY_STATE_ATTACKING:
            start = sprite->anim_attack_start;
            count = sprite->anim_attack_count;
            break;
        case ENTITY_STATE_HURT:
            start = sprite->anim_hurt_start;
            count = sprite->anim_hurt_count;
            break;
        case ENTITY_STATE_DEAD:
            start = sprite->anim_die_start;
            count = sprite->anim_die_count;
            break;
        default:
            start = sprite->anim_idle_start;
            count = sprite->anim_idle_count;
            break;
    }

    if (count <= 0) count = 1;
    *frame = start + ((int)(anim_time * 10) % count);
}

void entity_renderer_draw(Entity* entity)
{
    Renderer* r;
    EntitySprite* sprite;
    float screen_x, screen_y;
    float zoom;
    bool flip;
    Color fallback_color;

    if (!entity || !g_entity_renderer.renderer) return;

    r = g_entity_renderer.renderer;
    zoom = g_entity_renderer.zoom;

    get_screen_pos(entity->x, entity->y, &screen_x, &screen_y);

    /* Check if on screen */
    if (screen_x < -100 || screen_x > r->width + 100 ||
        screen_y < -100 || screen_y > r->height + 100) {
        return;
    }

    flip = (entity->dir < 0);
    sprite = entity_sprite_get(entity->sprite_id);

    if (sprite && sprite->texture) {
        /* Draw sprite */
        int frame;
        get_entity_frame(entity, sprite, &frame);

        int columns = 1;
        int tex_w;
        SDL_QueryTexture(sprite->texture, NULL, NULL, &tex_w, NULL);
        columns = tex_w / sprite->frame_width;
        if (columns <= 0) columns = 1;

        int src_x = (frame % columns) * sprite->frame_width;
        int src_y = (frame / columns) * sprite->frame_height;

        float draw_w = sprite->frame_width * zoom;
        float draw_h = sprite->frame_height * zoom;
        float draw_x = screen_x - draw_w * sprite->anchor_x;
        float draw_y = screen_y - draw_h * sprite->anchor_y;

        /* Draw shadow */
        Rect shadow = {draw_x + draw_w * 0.1f, screen_y - 5,
                      draw_w * 0.8f, 10 * zoom};
        renderer_draw_rect(r, shadow, (Color){0, 0, 0, SHADOW_ALPHA}, true);

        /* Draw sprite */
        Rect src = {(float)src_x, (float)src_y,
                   (float)sprite->frame_width, (float)sprite->frame_height};
        Rect dst = {draw_x, draw_y, draw_w, draw_h};
        renderer_draw_texture_ex(r, sprite->texture, src, dst, 0, flip, false);
    } else {
        /* Fallback: draw colored rectangle */
        switch (entity->type) {
            case ENTITY_PLAYER:   fallback_color = g_entity_sprites.player_color; break;
            case ENTITY_MONSTER:  fallback_color = g_entity_sprites.monster_color; break;
            case ENTITY_NPC:      fallback_color = g_entity_sprites.npc_color; break;
            case ENTITY_PET:      fallback_color = g_entity_sprites.pet_color; break;
            default:              fallback_color = (Color){150, 150, 150, 255}; break;
        }

        float size_w = 40 * zoom;
        float size_h = 60 * zoom;
        Rect body = {screen_x - size_w / 2, screen_y - size_h,
                    size_w, size_h};

        /* Shadow */
        Rect shadow = {screen_x - size_w / 3, screen_y - 5,
                      size_w * 0.66f, 10 * zoom};
        renderer_draw_rect(r, shadow, (Color){0, 0, 0, SHADOW_ALPHA}, true);

        /* Body */
        renderer_draw_rect(r, body, fallback_color, true);

        /* Flash white when hurt */
        if (entity->state == ENTITY_STATE_HURT) {
            renderer_draw_rect(r, body, (Color){255, 255, 255, 128}, true);
        }
    }

    /* Draw HP bar */
    if (g_entity_renderer.show_hp_bars && entity->max_hp > 0) {
        float hp_pct = (float)entity->hp / (float)entity->max_hp;
        if (hp_pct < 0) hp_pct = 0;
        if (hp_pct > 1) hp_pct = 1;

        float bar_w = HP_BAR_WIDTH * zoom;
        float bar_h = HP_BAR_HEIGHT * zoom;
        float bar_x = screen_x - bar_w / 2;
        float bar_y = screen_y - 70 * zoom;

        /* Background */
        Rect bg = {bar_x - 1, bar_y - 1, bar_w + 2, bar_h + 2};
        renderer_draw_rect(r, bg, (Color){0, 0, 0, 200}, true);

        /* HP fill */
        Color hp_color;
        if (hp_pct > 0.5f) {
            hp_color = (Color){50, 200, 50, 255};
        } else if (hp_pct > 0.25f) {
            hp_color = (Color){200, 200, 50, 255};
        } else {
            hp_color = (Color){200, 50, 50, 255};
        }

        Rect fill = {bar_x, bar_y, bar_w * hp_pct, bar_h};
        renderer_draw_rect(r, fill, hp_color, true);
    }

    /* Draw hitbox (debug) */
    if (g_entity_renderer.show_hitboxes) {
        float hw = entity->width * zoom / 2;
        float hh = entity->height * zoom;
        Rect hitbox = {screen_x - hw, screen_y - hh, hw * 2, hh};
        renderer_draw_rect(r, hitbox, (Color){255, 0, 255, 100}, true);
    }
}

void entity_renderer_draw_batch(Entity** entities, int count)
{
    int i;

    /* Sort by Y position for depth ordering */
    /* Simple bubble sort for now - could optimize with qsort */
    for (i = 0; i < count - 1; i++) {
        int j;
        for (j = i + 1; j < count; j++) {
            if (entities[j]->y < entities[i]->y) {
                Entity* temp = entities[i];
                entities[i] = entities[j];
                entities[j] = temp;
            }
        }
    }

    /* Draw all entities */
    for (i = 0; i < count; i++) {
        if (entities[i]) {
            entity_renderer_draw(entities[i]);
        }
    }
}

void entity_renderer_toggle_hitboxes(void)
{
    g_entity_renderer.show_hitboxes = !g_entity_renderer.show_hitboxes;
}

void entity_renderer_toggle_names(void)
{
    g_entity_renderer.show_names = !g_entity_renderer.show_names;
}

void entity_renderer_toggle_hp_bars(void)
{
    g_entity_renderer.show_hp_bars = !g_entity_renderer.show_hp_bars;
}

void entity_renderer_cleanup(void)
{
    entity_sprite_cleanup();
    memset(&g_entity_renderer, 0, sizeof(EntityRenderer));
}

/*
 * ============================================================================
 * Damage Number Rendering
 * ============================================================================
 */

#define MAX_DAMAGE_NUMBERS  64

typedef struct {
    i32     value;
    float   x;
    float   y;
    float   vy;
    float   alpha;
    float   scale;
    bool    is_crit;
    bool    is_heal;
    float   lifetime;
} DamageNumber;

static struct {
    DamageNumber numbers[MAX_DAMAGE_NUMBERS];
    int count;
} g_damage_numbers = {0};

void damage_number_spawn(float x, float y, i32 value, bool is_crit, bool is_heal)
{
    DamageNumber* dn;

    if (g_damage_numbers.count >= MAX_DAMAGE_NUMBERS) {
        /* Remove oldest */
        memmove(&g_damage_numbers.numbers[0], &g_damage_numbers.numbers[1],
                (MAX_DAMAGE_NUMBERS - 1) * sizeof(DamageNumber));
        g_damage_numbers.count--;
    }

    dn = &g_damage_numbers.numbers[g_damage_numbers.count++];
    dn->value = value;
    dn->x = x + (rand() % 20 - 10);
    dn->y = y - 30;
    dn->vy = -100.0f;
    dn->alpha = 255;
    dn->scale = is_crit ? 1.5f : 1.0f;
    dn->is_crit = is_crit;
    dn->is_heal = is_heal;
    dn->lifetime = 0;
}

void damage_number_update(float delta)
{
    int i;

    for (i = g_damage_numbers.count - 1; i >= 0; i--) {
        DamageNumber* dn = &g_damage_numbers.numbers[i];

        dn->lifetime += delta;
        dn->y += dn->vy * delta;
        dn->vy += 200.0f * delta;  /* Gravity */

        if (dn->lifetime > 0.5f) {
            dn->alpha -= 500 * delta;
        }

        if (dn->alpha <= 0 || dn->lifetime > 1.5f) {
            /* Remove */
            memmove(&g_damage_numbers.numbers[i], &g_damage_numbers.numbers[i + 1],
                    (g_damage_numbers.count - i - 1) * sizeof(DamageNumber));
            g_damage_numbers.count--;
        }
    }
}

void damage_number_draw(Renderer* renderer, float cam_x, float cam_y, float zoom)
{
    int i;

    for (i = 0; i < g_damage_numbers.count; i++) {
        DamageNumber* dn = &g_damage_numbers.numbers[i];

        float screen_x = (dn->x - cam_x) * zoom + renderer->width / 2.0f;
        float screen_y = (dn->y - cam_y) * zoom + renderer->height / 2.0f;

        /* Draw number as colored rect (would be text in real implementation) */
        float size = 20 * dn->scale * zoom;
        Rect rect = {screen_x - size / 2, screen_y - size / 2, size, size};

        Color color;
        if (dn->is_heal) {
            color = (Color){50, 255, 50, (u8)dn->alpha};
        } else if (dn->is_crit) {
            color = (Color){255, 200, 50, (u8)dn->alpha};
        } else {
            color = (Color){255, 255, 255, (u8)dn->alpha};
        }

        renderer_draw_rect(renderer, rect, color, true);
    }
}
