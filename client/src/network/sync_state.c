/**
 * @file sync_state.c
 * @brief State synchronization for buffs, HP/MP, and entity states
 */

#include "../../include/client.h"
#include "../../include/network.h"
#include "../../include/protocol_types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/*
 * ============================================================================
 * Constants
 * ============================================================================
 */

#define MAX_SYNC_ENTITIES       128
#define MAX_BUFFS_PER_ENTITY    32
#define BUFF_ICON_SIZE          32
#define STATE_SYNC_RATE_MS      100
#define HP_BAR_LERP_SPEED       5.0f

/*
 * ============================================================================
 * Buff Types
 * ============================================================================
 */

typedef enum {
    BUFF_TYPE_NONE = 0,
    BUFF_TYPE_STAT_BOOST,       /* Increase stats */
    BUFF_TYPE_STAT_REDUCTION,   /* Decrease stats */
    BUFF_TYPE_HOT,              /* Heal over time */
    BUFF_TYPE_DOT,              /* Damage over time */
    BUFF_TYPE_SHIELD,           /* Absorb damage */
    BUFF_TYPE_IMMUNITY,         /* Immune to damage */
    BUFF_TYPE_STUN,             /* Cannot act */
    BUFF_TYPE_SLOW,             /* Reduced movement */
    BUFF_TYPE_HASTE,            /* Increased movement */
    BUFF_TYPE_INVINCIBLE,       /* Cannot be targeted */
    BUFF_TYPE_STEALTH,          /* Invisible */
    BUFF_TYPE_SUPER_ARMOR,      /* Cannot be interrupted */
    BUFF_TYPE_CUSTOM
} BuffType;

typedef struct {
    uint32_t        buff_id;
    BuffType        type;
    uint32_t        icon_id;
    char            name[32];

    /* Timing */
    float           duration;
    float           remaining;
    int             stacks;
    int             max_stacks;

    /* Effects */
    float           value;              /* Effect magnitude */
    float           tick_interval;      /* For DOT/HOT */
    float           tick_timer;

    bool            is_debuff;
    bool            is_dispellable;
    bool            show_timer;
    bool            active;
} Buff;

typedef struct {
    uint32_t        entity_id;
    bool            active;

    /* Vital stats */
    float           hp;
    float           max_hp;
    float           hp_display;         /* For smooth bar animation */
    float           mp;
    float           max_mp;
    float           mp_display;

    /* Secondary resources */
    float           stamina;
    float           max_stamina;
    float           shield;             /* Absorb shield */

    /* Status */
    bool            is_alive;
    bool            is_stunned;
    bool            is_silenced;
    bool            is_rooted;
    bool            is_invincible;
    bool            is_stealth;

    /* Buffs */
    Buff            buffs[MAX_BUFFS_PER_ENTITY];
    int             buff_count;

    /* Targeting */
    uint32_t        target_id;
    bool            is_hostile;

    /* Level and name */
    uint8_t         level;
    char            name[32];

} EntityState;

typedef struct {
    /* Entity states */
    EntityState     entities[MAX_SYNC_ENTITIES];
    int             entity_count;

    /* Local player reference */
    uint32_t        local_entity_id;
    EntityState*    local_state;

    /* Sync timing */
    uint32_t        last_sync_time;

    bool            initialized;
} StateSyncSystem;

static StateSyncSystem g_state = {0};

/*
 * ============================================================================
 * Helper Functions
 * ============================================================================
 */

static EntityState* find_entity(uint32_t entity_id)
{
    for (int i = 0; i < MAX_SYNC_ENTITIES; i++) {
        if (g_state.entities[i].active &&
            g_state.entities[i].entity_id == entity_id) {
            return &g_state.entities[i];
        }
    }
    return NULL;
}

static EntityState* add_entity(uint32_t entity_id)
{
    EntityState* existing = find_entity(entity_id);
    if (existing) return existing;

    for (int i = 0; i < MAX_SYNC_ENTITIES; i++) {
        if (!g_state.entities[i].active) {
            EntityState* ent = &g_state.entities[i];
            memset(ent, 0, sizeof(EntityState));
            ent->entity_id = entity_id;
            ent->active = true;
            ent->is_alive = true;
            g_state.entity_count++;
            return ent;
        }
    }
    return NULL;
}

static Buff* find_buff(EntityState* ent, uint32_t buff_id)
{
    for (int i = 0; i < MAX_BUFFS_PER_ENTITY; i++) {
        if (ent->buffs[i].active && ent->buffs[i].buff_id == buff_id) {
            return &ent->buffs[i];
        }
    }
    return NULL;
}

static Buff* alloc_buff(EntityState* ent)
{
    for (int i = 0; i < MAX_BUFFS_PER_ENTITY; i++) {
        if (!ent->buffs[i].active) {
            memset(&ent->buffs[i], 0, sizeof(Buff));
            ent->buffs[i].active = true;
            ent->buff_count++;
            return &ent->buffs[i];
        }
    }
    return NULL;
}

static float lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}

/*
 * ============================================================================
 * Initialization
 * ============================================================================
 */

void sync_state_init(void)
{
    memset(&g_state, 0, sizeof(StateSyncSystem));
    g_state.initialized = true;
}

void sync_state_shutdown(void)
{
    g_state.initialized = false;
}

void sync_state_set_local_entity(uint32_t entity_id)
{
    g_state.local_entity_id = entity_id;
    g_state.local_state = find_entity(entity_id);
}

/*
 * ============================================================================
 * Entity Management
 * ============================================================================
 */

void sync_state_add_entity(uint32_t entity_id, const char* name, uint8_t level,
                           float max_hp, float max_mp, bool is_hostile)
{
    EntityState* ent = add_entity(entity_id);
    if (!ent) return;

    strncpy(ent->name, name, sizeof(ent->name) - 1);
    ent->level = level;
    ent->max_hp = max_hp;
    ent->hp = max_hp;
    ent->hp_display = max_hp;
    ent->max_mp = max_mp;
    ent->mp = max_mp;
    ent->mp_display = max_mp;
    ent->is_hostile = is_hostile;
    ent->is_alive = true;

    if (entity_id == g_state.local_entity_id) {
        g_state.local_state = ent;
    }
}

void sync_state_remove_entity(uint32_t entity_id)
{
    EntityState* ent = find_entity(entity_id);
    if (ent) {
        ent->active = false;
        g_state.entity_count--;

        if (ent == g_state.local_state) {
            g_state.local_state = NULL;
        }
    }
}

void sync_state_clear_all(void)
{
    for (int i = 0; i < MAX_SYNC_ENTITIES; i++) {
        g_state.entities[i].active = false;
    }
    g_state.entity_count = 0;
    g_state.local_state = NULL;
}

/*
 * ============================================================================
 * HP/MP Updates
 * ============================================================================
 */

void sync_state_receive_hp(uint32_t entity_id, float hp, float max_hp)
{
    EntityState* ent = find_entity(entity_id);
    if (!ent) {
        ent = add_entity(entity_id);
        if (!ent) return;
    }

    ent->hp = hp;
    ent->max_hp = max_hp;

    /* Check death */
    if (hp <= 0 && ent->is_alive) {
        ent->is_alive = false;
        printf("[State] Entity %u died\n", entity_id);
    } else if (hp > 0 && !ent->is_alive) {
        ent->is_alive = true;
        printf("[State] Entity %u revived\n", entity_id);
    }
}

void sync_state_receive_mp(uint32_t entity_id, float mp, float max_mp)
{
    EntityState* ent = find_entity(entity_id);
    if (!ent) return;

    ent->mp = mp;
    ent->max_mp = max_mp;
}

void sync_state_receive_stamina(uint32_t entity_id, float stamina, float max_stamina)
{
    EntityState* ent = find_entity(entity_id);
    if (!ent) return;

    ent->stamina = stamina;
    ent->max_stamina = max_stamina;
}

void sync_state_receive_shield(uint32_t entity_id, float shield)
{
    EntityState* ent = find_entity(entity_id);
    if (!ent) return;

    ent->shield = shield;
}

/*
 * ============================================================================
 * Buff Management
 * ============================================================================
 */

void sync_state_add_buff(uint32_t entity_id, uint32_t buff_id, BuffType type,
                         const char* name, uint32_t icon_id, float duration,
                         float value, int stacks, bool is_debuff)
{
    EntityState* ent = find_entity(entity_id);
    if (!ent) return;

    /* Check for existing buff (refresh or stack) */
    Buff* existing = find_buff(ent, buff_id);
    if (existing) {
        /* Refresh duration */
        existing->remaining = duration;
        existing->duration = duration;

        /* Stack if allowed */
        if (stacks > 0 && existing->stacks < existing->max_stacks) {
            existing->stacks = stacks;
        }
        return;
    }

    /* Create new buff */
    Buff* buff = alloc_buff(ent);
    if (!buff) return;

    buff->buff_id = buff_id;
    buff->type = type;
    buff->icon_id = icon_id;
    strncpy(buff->name, name, sizeof(buff->name) - 1);
    buff->duration = duration;
    buff->remaining = duration;
    buff->stacks = stacks;
    buff->max_stacks = stacks > 0 ? 99 : 1;
    buff->value = value;
    buff->is_debuff = is_debuff;
    buff->is_dispellable = true;
    buff->show_timer = duration > 0;

    /* Set tick interval for DOT/HOT */
    if (type == BUFF_TYPE_DOT || type == BUFF_TYPE_HOT) {
        buff->tick_interval = 1.0f;  /* 1 second ticks */
    }

    /* Apply status effects */
    switch (type) {
        case BUFF_TYPE_STUN:
            ent->is_stunned = true;
            break;
        case BUFF_TYPE_INVINCIBLE:
        case BUFF_TYPE_IMMUNITY:
            ent->is_invincible = true;
            break;
        case BUFF_TYPE_STEALTH:
            ent->is_stealth = true;
            break;
        default:
            break;
    }

    printf("[State] Entity %u gained buff: %s (%.1fs)\n",
           entity_id, name, duration);
}

void sync_state_remove_buff(uint32_t entity_id, uint32_t buff_id)
{
    EntityState* ent = find_entity(entity_id);
    if (!ent) return;

    Buff* buff = find_buff(ent, buff_id);
    if (!buff) return;

    /* Remove status effects */
    switch (buff->type) {
        case BUFF_TYPE_STUN:
            ent->is_stunned = false;
            break;
        case BUFF_TYPE_INVINCIBLE:
        case BUFF_TYPE_IMMUNITY:
            ent->is_invincible = false;
            break;
        case BUFF_TYPE_STEALTH:
            ent->is_stealth = false;
            break;
        default:
            break;
    }

    printf("[State] Entity %u lost buff: %s\n", entity_id, buff->name);

    buff->active = false;
    ent->buff_count--;
}

void sync_state_update_buff_stacks(uint32_t entity_id, uint32_t buff_id, int stacks)
{
    EntityState* ent = find_entity(entity_id);
    if (!ent) return;

    Buff* buff = find_buff(ent, buff_id);
    if (buff) {
        buff->stacks = stacks;
        if (stacks <= 0) {
            sync_state_remove_buff(entity_id, buff_id);
        }
    }
}

/*
 * ============================================================================
 * Status Updates
 * ============================================================================
 */

void sync_state_receive_status(uint32_t entity_id, bool is_alive,
                               bool is_stunned, bool is_silenced,
                               bool is_rooted, bool is_invincible)
{
    EntityState* ent = find_entity(entity_id);
    if (!ent) return;

    ent->is_alive = is_alive;
    ent->is_stunned = is_stunned;
    ent->is_silenced = is_silenced;
    ent->is_rooted = is_rooted;
    ent->is_invincible = is_invincible;
}

void sync_state_receive_target(uint32_t entity_id, uint32_t target_id)
{
    EntityState* ent = find_entity(entity_id);
    if (ent) {
        ent->target_id = target_id;
    }
}

/*
 * ============================================================================
 * Update
 * ============================================================================
 */

void sync_state_update(float delta)
{
    if (!g_state.initialized) return;

    for (int i = 0; i < MAX_SYNC_ENTITIES; i++) {
        EntityState* ent = &g_state.entities[i];
        if (!ent->active) continue;

        /* Smooth HP/MP display */
        ent->hp_display = lerp(ent->hp_display, ent->hp, HP_BAR_LERP_SPEED * delta);
        ent->mp_display = lerp(ent->mp_display, ent->mp, HP_BAR_LERP_SPEED * delta);

        /* Snap if close enough */
        if (fabsf(ent->hp_display - ent->hp) < 1.0f) {
            ent->hp_display = ent->hp;
        }
        if (fabsf(ent->mp_display - ent->mp) < 1.0f) {
            ent->mp_display = ent->mp;
        }

        /* Update buffs */
        for (int j = 0; j < MAX_BUFFS_PER_ENTITY; j++) {
            Buff* buff = &ent->buffs[j];
            if (!buff->active) continue;

            /* Update duration */
            if (buff->duration > 0) {
                buff->remaining -= delta;
                if (buff->remaining <= 0) {
                    sync_state_remove_buff(ent->entity_id, buff->buff_id);
                    continue;
                }
            }

            /* DOT/HOT ticks */
            if (buff->type == BUFF_TYPE_DOT || buff->type == BUFF_TYPE_HOT) {
                buff->tick_timer += delta;
                if (buff->tick_timer >= buff->tick_interval) {
                    buff->tick_timer = 0;

                    if (buff->type == BUFF_TYPE_HOT) {
                        ent->hp += buff->value * buff->stacks;
                        if (ent->hp > ent->max_hp) {
                            ent->hp = ent->max_hp;
                        }
                    } else {
                        ent->hp -= buff->value * buff->stacks;
                        if (ent->hp < 0) {
                            ent->hp = 0;
                            ent->is_alive = false;
                        }
                    }
                }
            }
        }
    }
}

/*
 * ============================================================================
 * Query Functions
 * ============================================================================
 */

bool sync_state_get_hp(uint32_t entity_id, float* hp, float* max_hp, float* display_hp)
{
    EntityState* ent = find_entity(entity_id);
    if (!ent) return false;

    *hp = ent->hp;
    *max_hp = ent->max_hp;
    *display_hp = ent->hp_display;
    return true;
}

bool sync_state_get_mp(uint32_t entity_id, float* mp, float* max_mp)
{
    EntityState* ent = find_entity(entity_id);
    if (!ent) return false;

    *mp = ent->mp;
    *max_mp = ent->max_mp;
    return true;
}

float sync_state_get_hp_percent(uint32_t entity_id)
{
    EntityState* ent = find_entity(entity_id);
    if (!ent || ent->max_hp <= 0) return 0;
    return ent->hp_display / ent->max_hp;
}

float sync_state_get_mp_percent(uint32_t entity_id)
{
    EntityState* ent = find_entity(entity_id);
    if (!ent || ent->max_mp <= 0) return 0;
    return ent->mp / ent->max_mp;
}

bool sync_state_is_alive(uint32_t entity_id)
{
    EntityState* ent = find_entity(entity_id);
    return ent && ent->is_alive;
}

bool sync_state_is_stunned(uint32_t entity_id)
{
    EntityState* ent = find_entity(entity_id);
    return ent && ent->is_stunned;
}

bool sync_state_is_invincible(uint32_t entity_id)
{
    EntityState* ent = find_entity(entity_id);
    return ent && ent->is_invincible;
}

const char* sync_state_get_name(uint32_t entity_id)
{
    EntityState* ent = find_entity(entity_id);
    return ent ? ent->name : "Unknown";
}

uint8_t sync_state_get_level(uint32_t entity_id)
{
    EntityState* ent = find_entity(entity_id);
    return ent ? ent->level : 0;
}

/*
 * ============================================================================
 * Buff Query
 * ============================================================================
 */

int sync_state_get_buffs(uint32_t entity_id, Buff** out_buffs, int max_count)
{
    EntityState* ent = find_entity(entity_id);
    if (!ent) return 0;

    int count = 0;
    for (int i = 0; i < MAX_BUFFS_PER_ENTITY && count < max_count; i++) {
        if (ent->buffs[i].active) {
            out_buffs[count++] = &ent->buffs[i];
        }
    }
    return count;
}

int sync_state_get_buff_count(uint32_t entity_id)
{
    EntityState* ent = find_entity(entity_id);
    return ent ? ent->buff_count : 0;
}

bool sync_state_has_buff(uint32_t entity_id, uint32_t buff_id)
{
    EntityState* ent = find_entity(entity_id);
    if (!ent) return false;
    return find_buff(ent, buff_id) != NULL;
}

bool sync_state_has_buff_type(uint32_t entity_id, BuffType type)
{
    EntityState* ent = find_entity(entity_id);
    if (!ent) return false;

    for (int i = 0; i < MAX_BUFFS_PER_ENTITY; i++) {
        if (ent->buffs[i].active && ent->buffs[i].type == type) {
            return true;
        }
    }
    return false;
}

/*
 * ============================================================================
 * Local Player Helpers
 * ============================================================================
 */

float sync_state_get_local_hp_percent(void)
{
    if (!g_state.local_state) return 0;
    if (g_state.local_state->max_hp <= 0) return 0;
    return g_state.local_state->hp_display / g_state.local_state->max_hp;
}

float sync_state_get_local_mp_percent(void)
{
    if (!g_state.local_state) return 0;
    if (g_state.local_state->max_mp <= 0) return 0;
    return g_state.local_state->mp / g_state.local_state->max_mp;
}

int sync_state_get_local_buff_count(void)
{
    return g_state.local_state ? g_state.local_state->buff_count : 0;
}
