/**
 * @file entity.h
 * @brief Game entity system (players, monsters, NPCs)
 */

#ifndef CLIENT_ENTITY_H
#define CLIENT_ENTITY_H

#include "client.h"
#include "renderer.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ENTITY_NAME_MAX     32
#define ENTITY_MAX_ANIMS    16
#define ENTITY_HITBOX_COUNT 4

/* Entity type */
typedef enum {
    ENTITY_TYPE_NONE = 0,
    ENTITY_TYPE_PLAYER,
    ENTITY_TYPE_MONSTER,
    ENTITY_TYPE_NPC,
    ENTITY_TYPE_PROJECTILE,
    ENTITY_TYPE_EFFECT,
    ENTITY_TYPE_ITEM
} EntityType;

/* Entity state */
typedef enum {
    ENTITY_STATE_IDLE = 0,
    ENTITY_STATE_WALK,
    ENTITY_STATE_RUN,
    ENTITY_STATE_JUMP,
    ENTITY_STATE_FALL,
    ENTITY_STATE_ATTACK,
    ENTITY_STATE_SKILL,
    ENTITY_STATE_HIT,
    ENTITY_STATE_DOWN,
    ENTITY_STATE_GETUP,
    ENTITY_STATE_DEAD
} EntityState;

/* Direction */
typedef enum {
    DIR_RIGHT = 0,
    DIR_LEFT = 1
} Direction;

/* Animation set */
typedef struct {
    Animation*  idle;
    Animation*  walk;
    Animation*  run;
    Animation*  jump;
    Animation*  fall;
    Animation*  attack[4];      /* Basic attack combo */
    Animation*  skill[8];       /* Skill animations */
    Animation*  hit;
    Animation*  down;
    Animation*  getup;
    Animation*  dead;
} AnimationSet;

/* Entity */
typedef struct Entity {
    /* Identity */
    u32             id;
    EntityType      type;
    char            name[ENTITY_NAME_MAX];

    /* Transform */
    Vec2            position;
    Vec2            velocity;
    float           z_position;     /* Height for 2.5D */
    float           z_velocity;
    Direction       direction;

    /* Physics */
    Rect            hitbox;
    bool            on_ground;
    bool            gravity_enabled;

    /* State */
    EntityState     state;
    float           state_timer;

    /* Stats (client-side display) */
    i32             hp;
    i32             max_hp;
    i32             mp;
    i32             max_mp;
    u8              level;

    /* Rendering */
    AnimationSet*   animations;
    Animation*      current_anim;
    Color           tint;
    float           alpha;
    bool            visible;
    bool            flash;
    float           flash_timer;

    /* Combat */
    u32             target_id;
    float           attack_timer;
    int             combo_count;
    bool            invincible;
    float           invincible_timer;

    /* Flags */
    bool            active;
    bool            local_player;
    bool            selectable;
} Entity;

/* Entity manager */
typedef struct EntityManager {
    Entity          entities[MAX_ENTITIES];
    int             count;
    u32             local_player_id;
} EntityManager;

/**
 * Initialize entity manager
 * @param manager Entity manager instance
 * @return CLIENT_OK on success
 */
int entity_manager_init(EntityManager* manager);

/**
 * Cleanup entity manager
 * @param manager Entity manager instance
 */
void entity_manager_cleanup(EntityManager* manager);

/**
 * Create entity
 * @param manager Entity manager instance
 * @param id      Entity ID
 * @param type    Entity type
 * @return Entity pointer or NULL
 */
Entity* entity_create(EntityManager* manager, u32 id, EntityType type);

/**
 * Destroy entity
 * @param manager Entity manager instance
 * @param id      Entity ID
 */
void entity_destroy(EntityManager* manager, u32 id);

/**
 * Get entity by ID
 * @param manager Entity manager instance
 * @param id      Entity ID
 * @return Entity pointer or NULL
 */
Entity* entity_get(EntityManager* manager, u32 id);

/**
 * Get local player entity
 * @param manager Entity manager instance
 * @return Entity pointer or NULL
 */
Entity* entity_get_local_player(EntityManager* manager);

/**
 * Update all entities
 * @param manager Entity manager instance
 * @param delta   Delta time
 */
void entity_manager_update(EntityManager* manager, float delta);

/**
 * Draw all entities
 * @param manager  Entity manager instance
 * @param renderer Renderer instance
 */
void entity_manager_draw(EntityManager* manager, Renderer* renderer);

/**
 * Set entity state
 * @param entity Entity instance
 * @param state  New state
 */
void entity_set_state(Entity* entity, EntityState state);

/**
 * Set entity animation
 * @param entity Entity instance
 * @param anim   Animation to play
 */
void entity_play_animation(Entity* entity, Animation* anim);

/**
 * Move entity
 * @param entity Entity instance
 * @param dx     X movement
 * @param dy     Y movement
 */
void entity_move(Entity* entity, float dx, float dy);

/**
 * Apply damage to entity (visual only)
 * @param entity Entity instance
 * @param damage Damage amount
 */
void entity_take_damage(Entity* entity, i32 damage);

/**
 * Entity attack
 * @param entity Entity instance
 */
void entity_attack(Entity* entity);

/**
 * Entity use skill
 * @param entity   Entity instance
 * @param skill_id Skill ID
 */
void entity_use_skill(Entity* entity, u16 skill_id);

#ifdef __cplusplus
}
#endif

#endif /* CLIENT_ENTITY_H */
