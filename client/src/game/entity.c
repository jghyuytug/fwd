/**
 * @file entity.c
 * @brief Entity system implementation
 */

#include "../../include/entity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int entity_manager_init(EntityManager* manager)
{
    if (!manager) return CLIENT_ERROR;

    memset(manager, 0, sizeof(EntityManager));
    return CLIENT_OK;
}

void entity_manager_cleanup(EntityManager* manager)
{
    int i;

    if (!manager) return;

    for (i = 0; i < MAX_ENTITIES; i++) {
        if (manager->entities[i].active) {
            /* Free animations if owned */
            manager->entities[i].active = false;
        }
    }

    manager->count = 0;
}

Entity* entity_create(EntityManager* manager, u32 id, EntityType type)
{
    int i;
    Entity* entity;

    if (!manager) return NULL;

    /* Find free slot */
    for (i = 0; i < MAX_ENTITIES; i++) {
        if (!manager->entities[i].active) {
            entity = &manager->entities[i];
            break;
        }
    }

    if (i >= MAX_ENTITIES) {
        fprintf(stderr, "Entity limit reached\n");
        return NULL;
    }

    /* Initialize entity */
    memset(entity, 0, sizeof(Entity));
    entity->id = id;
    entity->type = type;
    entity->active = true;
    entity->visible = true;
    entity->state = ENTITY_STATE_IDLE;
    entity->direction = DIR_RIGHT;
    entity->gravity_enabled = true;
    entity->alpha = 1.0f;
    entity->tint = (Color){255, 255, 255, 255};

    /* Default hitbox */
    entity->hitbox.x = -16;
    entity->hitbox.y = -48;
    entity->hitbox.w = 32;
    entity->hitbox.h = 48;

    manager->count++;

    return entity;
}

void entity_destroy(EntityManager* manager, u32 id)
{
    int i;

    if (!manager) return;

    for (i = 0; i < MAX_ENTITIES; i++) {
        if (manager->entities[i].active && manager->entities[i].id == id) {
            manager->entities[i].active = false;
            manager->count--;
            break;
        }
    }
}

Entity* entity_get(EntityManager* manager, u32 id)
{
    int i;

    if (!manager) return NULL;

    for (i = 0; i < MAX_ENTITIES; i++) {
        if (manager->entities[i].active && manager->entities[i].id == id) {
            return &manager->entities[i];
        }
    }

    return NULL;
}

Entity* entity_get_local_player(EntityManager* manager)
{
    if (!manager) return NULL;
    return entity_get(manager, manager->local_player_id);
}

static void update_entity(Entity* entity, float delta)
{
    /* Update state timer */
    entity->state_timer += delta;

    /* Apply gravity */
    if (entity->gravity_enabled && !entity->on_ground) {
        entity->z_velocity += GRAVITY * delta * 60.0f;
    }

    /* Apply velocity */
    entity->position.x += entity->velocity.x * delta;
    entity->position.y += entity->velocity.y * delta;
    entity->z_position += entity->z_velocity * delta;

    /* Ground check (simple) */
    if (entity->z_position <= 0) {
        entity->z_position = 0;
        entity->z_velocity = 0;
        entity->on_ground = true;
    } else {
        entity->on_ground = false;
    }

    /* Update current animation */
    if (entity->current_anim) {
        animation_update(entity->current_anim, delta);
    }

    /* Update invincibility */
    if (entity->invincible) {
        entity->invincible_timer -= delta;
        if (entity->invincible_timer <= 0) {
            entity->invincible = false;
        }
    }

    /* Update flash effect */
    if (entity->flash) {
        entity->flash_timer -= delta;
        if (entity->flash_timer <= 0) {
            entity->flash = false;
        }
    }
}

void entity_manager_update(EntityManager* manager, float delta)
{
    int i;

    if (!manager) return;

    for (i = 0; i < MAX_ENTITIES; i++) {
        if (manager->entities[i].active) {
            update_entity(&manager->entities[i], delta);
        }
    }
}

/* Compare entities for depth sorting */
static int compare_entities(const void* a, const void* b)
{
    const Entity* ea = *(const Entity**)a;
    const Entity* eb = *(const Entity**)b;

    /* Sort by Y position (lower Y = further back) */
    float ya = ea->position.y;
    float yb = eb->position.y;

    if (ya < yb) return -1;
    if (ya > yb) return 1;
    return 0;
}

void entity_manager_draw(EntityManager* manager, Renderer* renderer)
{
    int i, count;
    Entity* sorted[MAX_ENTITIES];
    Entity* entity;
    float draw_x, draw_y;
    Color tint;

    if (!manager || !renderer) return;

    /* Collect active entities */
    count = 0;
    for (i = 0; i < MAX_ENTITIES; i++) {
        if (manager->entities[i].active && manager->entities[i].visible) {
            sorted[count++] = &manager->entities[i];
        }
    }

    /* Sort by depth */
    if (count > 1) {
        qsort(sorted, count, sizeof(Entity*), compare_entities);
    }

    /* Draw entities */
    for (i = 0; i < count; i++) {
        entity = sorted[i];

        /* Calculate screen position */
        draw_x = entity->position.x;
        draw_y = entity->position.y - entity->z_position;

        /* Apply flash effect */
        if (entity->flash) {
            tint = (Color){255, 128, 128, 255};
        } else {
            tint = entity->tint;
        }

        /* Draw shadow */
        if (entity->z_position > 0) {
            Rect shadow;
            shadow.x = entity->position.x - 16;
            shadow.y = entity->position.y - 4;
            shadow.w = 32;
            shadow.h = 8;
            Color shadow_color = {0, 0, 0, 64};
            renderer_draw_rect(renderer, shadow, shadow_color, true);
        }

        /* Draw animation or placeholder */
        if (entity->current_anim) {
            animation_draw(renderer, entity->current_anim,
                          draw_x - entity->current_anim->sprite->frame_w / 2,
                          draw_y - entity->current_anim->sprite->frame_h,
                          entity->direction == DIR_LEFT);
        } else {
            /* Draw placeholder rectangle */
            Rect rect;
            rect.x = draw_x + entity->hitbox.x;
            rect.y = draw_y + entity->hitbox.y;
            rect.w = entity->hitbox.w;
            rect.h = entity->hitbox.h;

            Color color;
            switch (entity->type) {
                case ENTITY_TYPE_PLAYER:
                    color = (Color){64, 128, 255, 200};
                    break;
                case ENTITY_TYPE_MONSTER:
                    color = (Color){255, 64, 64, 200};
                    break;
                case ENTITY_TYPE_NPC:
                    color = (Color){64, 255, 64, 200};
                    break;
                default:
                    color = (Color){200, 200, 200, 200};
                    break;
            }

            renderer_draw_rect(renderer, rect, color, true);
        }

        /* Draw HP bar for monsters */
        if (entity->type == ENTITY_TYPE_MONSTER && entity->max_hp > 0) {
            float hp_ratio = (float)entity->hp / entity->max_hp;
            Rect hp_bg = {draw_x - 20, draw_y + entity->hitbox.y - 8, 40, 4};
            Rect hp_bar = {draw_x - 20, draw_y + entity->hitbox.y - 8, 40 * hp_ratio, 4};

            renderer_draw_rect(renderer, hp_bg, (Color){64, 64, 64, 200}, true);
            renderer_draw_rect(renderer, hp_bar, (Color){255, 0, 0, 200}, true);
        }
    }
}

void entity_set_state(Entity* entity, EntityState state)
{
    if (!entity) return;

    if (entity->state != state) {
        entity->state = state;
        entity->state_timer = 0;

        /* Set appropriate animation */
        if (entity->animations) {
            switch (state) {
                case ENTITY_STATE_IDLE:
                    entity_play_animation(entity, entity->animations->idle);
                    break;
                case ENTITY_STATE_WALK:
                    entity_play_animation(entity, entity->animations->walk);
                    break;
                case ENTITY_STATE_RUN:
                    entity_play_animation(entity, entity->animations->run);
                    break;
                case ENTITY_STATE_JUMP:
                    entity_play_animation(entity, entity->animations->jump);
                    break;
                case ENTITY_STATE_FALL:
                    entity_play_animation(entity, entity->animations->fall);
                    break;
                case ENTITY_STATE_HIT:
                    entity_play_animation(entity, entity->animations->hit);
                    break;
                case ENTITY_STATE_DEAD:
                    entity_play_animation(entity, entity->animations->dead);
                    break;
                default:
                    break;
            }
        }
    }
}

void entity_play_animation(Entity* entity, Animation* anim)
{
    if (!entity || !anim) return;

    entity->current_anim = anim;
    animation_reset(anim);
}

void entity_move(Entity* entity, float dx, float dy)
{
    if (!entity) return;

    entity->position.x += dx;
    entity->position.y += dy;

    /* Update direction */
    if (dx > 0) entity->direction = DIR_RIGHT;
    else if (dx < 0) entity->direction = DIR_LEFT;
}

void entity_take_damage(Entity* entity, i32 damage)
{
    if (!entity) return;

    entity->hp -= damage;
    if (entity->hp < 0) entity->hp = 0;

    /* Flash effect */
    entity->flash = true;
    entity->flash_timer = 0.1f;

    /* Hit state */
    if (entity->hp <= 0) {
        entity_set_state(entity, ENTITY_STATE_DEAD);
    } else {
        entity_set_state(entity, ENTITY_STATE_HIT);
    }
}

void entity_attack(Entity* entity)
{
    if (!entity) return;

    if (entity->state != ENTITY_STATE_ATTACK) {
        entity_set_state(entity, ENTITY_STATE_ATTACK);
        entity->combo_count = 0;
    } else if (entity->combo_count < 3) {
        entity->combo_count++;
    }

    entity->attack_timer = 0.5f;
}

void entity_use_skill(Entity* entity, u16 skill_id)
{
    (void)skill_id;

    if (!entity) return;

    entity_set_state(entity, ENTITY_STATE_SKILL);
}
