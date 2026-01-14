/**
 * @file sync_combat.c
 * @brief Combat synchronization for attacks, damage, and deaths
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

#define MAX_PENDING_ATTACKS     16
#define MAX_DAMAGE_EVENTS       64
#define MAX_KILL_EVENTS         16
#define ATTACK_TIMEOUT_MS       500
#define DAMAGE_DISPLAY_TIME     1.5f
#define KILL_DISPLAY_TIME       3.0f
#define HIT_CONFIRM_WINDOW      200     /* ms for hit confirmation */

/*
 * ============================================================================
 * Combat Types
 * ============================================================================
 */

typedef enum {
    ATTACK_TYPE_NORMAL,
    ATTACK_TYPE_SKILL,
    ATTACK_TYPE_PROJECTILE,
    ATTACK_TYPE_AOE
} AttackType;

typedef enum {
    DAMAGE_NORMAL,
    DAMAGE_CRITICAL,
    DAMAGE_MISS,
    DAMAGE_BLOCKED,
    DAMAGE_HEAL
} DamageDisplayType;

typedef struct {
    uint32_t        attack_id;
    uint32_t        attacker_id;
    uint32_t        skill_id;
    AttackType      type;

    /* Position and hitbox */
    float           x;
    float           y;
    float           width;
    float           height;
    int8_t          facing;

    /* Timing */
    uint32_t        start_time;
    uint32_t        duration_ms;
    uint32_t        hit_time;           /* When hit is active */
    uint32_t        hit_duration;

    /* State */
    bool            active;
    bool            hit_confirmed;
    uint32_t        targets_hit[8];     /* Entities already hit */
    int             target_count;
} PendingAttack;

typedef struct {
    uint32_t            target_id;
    int32_t             damage;
    DamageDisplayType   type;
    float               x;
    float               y;
    float               display_timer;
    float               velocity_y;
    bool                active;
} DamageEvent;

typedef struct {
    uint32_t        victim_id;
    uint32_t        killer_id;
    char            victim_name[32];
    char            killer_name[32];
    float           display_timer;
    bool            active;
} KillEvent;

typedef struct {
    /* Pending attacks */
    PendingAttack   attacks[MAX_PENDING_ATTACKS];
    int             attack_count;
    uint32_t        next_attack_id;

    /* Damage display */
    DamageEvent     damage_events[MAX_DAMAGE_EVENTS];
    int             damage_event_count;

    /* Kill feed */
    KillEvent       kill_events[MAX_KILL_EVENTS];
    int             kill_event_count;

    /* Local player ID */
    uint32_t        local_player_id;

    /* Combat stats */
    uint32_t        total_damage_dealt;
    uint32_t        total_damage_taken;
    uint32_t        total_kills;
    uint32_t        total_deaths;
    uint32_t        hits_landed;
    uint32_t        hits_missed;

    /* Combo tracking */
    int             current_combo;
    float           combo_timer;
    int             max_combo;

    bool            initialized;
} CombatSyncSystem;

static CombatSyncSystem g_combat = {0};

/*
 * ============================================================================
 * External Functions
 * ============================================================================
 */

extern int network_send_packet(uint16_t msg_no, const void* data, size_t len);
extern void player_take_damage(float damage, float knockback_x, float knockback_y);
extern bool sync_position_get_render_position(uint32_t entity_id, float* x, float* y);

/*
 * ============================================================================
 * Helper Functions
 * ============================================================================
 */

static PendingAttack* find_attack(uint32_t attack_id)
{
    for (int i = 0; i < MAX_PENDING_ATTACKS; i++) {
        if (g_combat.attacks[i].active &&
            g_combat.attacks[i].attack_id == attack_id) {
            return &g_combat.attacks[i];
        }
    }
    return NULL;
}

static PendingAttack* alloc_attack(void)
{
    for (int i = 0; i < MAX_PENDING_ATTACKS; i++) {
        if (!g_combat.attacks[i].active) {
            memset(&g_combat.attacks[i], 0, sizeof(PendingAttack));
            g_combat.attacks[i].active = true;
            g_combat.attack_count++;
            return &g_combat.attacks[i];
        }
    }
    return NULL;
}

static DamageEvent* alloc_damage_event(void)
{
    /* Find inactive slot */
    for (int i = 0; i < MAX_DAMAGE_EVENTS; i++) {
        if (!g_combat.damage_events[i].active) {
            g_combat.damage_events[i].active = true;
            g_combat.damage_event_count++;
            return &g_combat.damage_events[i];
        }
    }

    /* Recycle oldest */
    float oldest_time = 0;
    int oldest_idx = 0;
    for (int i = 0; i < MAX_DAMAGE_EVENTS; i++) {
        if (g_combat.damage_events[i].display_timer > oldest_time) {
            oldest_time = g_combat.damage_events[i].display_timer;
            oldest_idx = i;
        }
    }
    return &g_combat.damage_events[oldest_idx];
}

static bool check_hitbox_collision(float ax, float ay, float aw, float ah,
                                   float bx, float by, float bw, float bh)
{
    return !(ax + aw < bx || ax > bx + bw ||
             ay + ah < by || ay > by + bh);
}

/*
 * ============================================================================
 * Initialization
 * ============================================================================
 */

void sync_combat_init(void)
{
    memset(&g_combat, 0, sizeof(CombatSyncSystem));
    g_combat.next_attack_id = 1;
    g_combat.initialized = true;
}

void sync_combat_shutdown(void)
{
    g_combat.initialized = false;
}

void sync_combat_set_local_player(uint32_t entity_id)
{
    g_combat.local_player_id = entity_id;
}

void sync_combat_reset_stats(void)
{
    g_combat.total_damage_dealt = 0;
    g_combat.total_damage_taken = 0;
    g_combat.total_kills = 0;
    g_combat.total_deaths = 0;
    g_combat.hits_landed = 0;
    g_combat.hits_missed = 0;
    g_combat.max_combo = 0;
}

/*
 * ============================================================================
 * Attack Handling
 * ============================================================================
 */

uint32_t sync_combat_start_attack(uint32_t skill_id, float x, float y,
                                  float width, float height, int8_t facing,
                                  uint32_t duration_ms, uint32_t hit_time,
                                  uint32_t hit_duration)
{
    PendingAttack* attack = alloc_attack();
    if (!attack) return 0;

    attack->attack_id = g_combat.next_attack_id++;
    attack->attacker_id = g_combat.local_player_id;
    attack->skill_id = skill_id;
    attack->type = skill_id > 0 ? ATTACK_TYPE_SKILL : ATTACK_TYPE_NORMAL;

    attack->x = x;
    attack->y = y;
    attack->width = width;
    attack->height = height;
    attack->facing = facing;

    attack->start_time = SDL_GetTicks();
    attack->duration_ms = duration_ms;
    attack->hit_time = hit_time;
    attack->hit_duration = hit_duration;

    /* Send attack start to server */
    struct {
        uint32_t    attack_id;
        uint32_t    skill_id;
        float       x;
        float       y;
        float       width;
        float       height;
        int8_t      facing;
    } packet = {
        attack->attack_id,
        skill_id,
        x, y, width, height, facing
    };

    network_send_packet(MSG_USE_SKILL, &packet, sizeof(packet));

    return attack->attack_id;
}

void sync_combat_cancel_attack(uint32_t attack_id)
{
    PendingAttack* attack = find_attack(attack_id);
    if (!attack) return;

    attack->active = false;
    g_combat.attack_count--;

    /* Notify server */
    struct { uint32_t attack_id; } packet = { attack_id };
    network_send_packet(MSG_SKILL_CANCEL, &packet, sizeof(packet));
}

/*
 * ============================================================================
 * Hit Detection (Client-side prediction)
 * ============================================================================
 */

void sync_combat_check_hit(uint32_t attack_id, uint32_t target_id,
                           float target_x, float target_y,
                           float target_width, float target_height)
{
    PendingAttack* attack = find_attack(attack_id);
    if (!attack || attack->hit_confirmed) return;

    /* Check if already hit this target */
    for (int i = 0; i < attack->target_count; i++) {
        if (attack->targets_hit[i] == target_id) return;
    }

    /* Check timing */
    uint32_t now = SDL_GetTicks();
    uint32_t elapsed = now - attack->start_time;
    if (elapsed < attack->hit_time ||
        elapsed > attack->hit_time + attack->hit_duration) {
        return;
    }

    /* Adjust hitbox position based on facing */
    float hitbox_x = attack->x;
    if (attack->facing < 0) {
        hitbox_x = attack->x - attack->width;
    }

    /* Check collision */
    if (check_hitbox_collision(hitbox_x, attack->y - attack->height,
                               attack->width, attack->height,
                               target_x - target_width / 2,
                               target_y - target_height,
                               target_width, target_height)) {
        /* Hit detected! */
        if (attack->target_count < 8) {
            attack->targets_hit[attack->target_count++] = target_id;
        }

        /* Send hit to server */
        struct {
            uint32_t    attack_id;
            uint32_t    target_id;
            float       hit_x;
            float       hit_y;
        } packet = {
            attack_id,
            target_id,
            target_x,
            target_y
        };

        network_send_packet(MSG_ATTACK_HIT, &packet, sizeof(packet));
    }
}

/*
 * ============================================================================
 * Receive from Server
 * ============================================================================
 */

void sync_combat_receive_damage(uint32_t target_id, uint32_t attacker_id,
                                int32_t damage, DamageDisplayType type,
                                float knockback_x, float knockback_y)
{
    /* Get target position for display */
    float x, y;
    if (!sync_position_get_render_position(target_id, &x, &y)) {
        x = 0;
        y = 0;
    }

    /* Create damage display */
    DamageEvent* evt = alloc_damage_event();
    evt->target_id = target_id;
    evt->damage = damage;
    evt->type = type;
    evt->x = x + (rand() % 20 - 10);
    evt->y = y - 40;
    evt->display_timer = 0;
    evt->velocity_y = -100.0f;

    /* Track stats */
    if (target_id == g_combat.local_player_id) {
        g_combat.total_damage_taken += damage;

        /* Apply damage to local player */
        player_take_damage((float)damage, knockback_x, knockback_y);
    } else if (attacker_id == g_combat.local_player_id) {
        g_combat.total_damage_dealt += damage;
        g_combat.hits_landed++;

        /* Update combo */
        g_combat.current_combo++;
        g_combat.combo_timer = 3.0f;
        if (g_combat.current_combo > g_combat.max_combo) {
            g_combat.max_combo = g_combat.current_combo;
        }
    }
}

void sync_combat_receive_hit_confirm(uint32_t attack_id, bool hit)
{
    PendingAttack* attack = find_attack(attack_id);
    if (attack) {
        attack->hit_confirmed = true;

        if (!hit) {
            g_combat.hits_missed++;
        }
    }
}

void sync_combat_receive_death(uint32_t victim_id, uint32_t killer_id,
                               const char* victim_name, const char* killer_name)
{
    /* Add to kill feed */
    if (g_combat.kill_event_count < MAX_KILL_EVENTS) {
        KillEvent* evt = &g_combat.kill_events[g_combat.kill_event_count++];
        evt->victim_id = victim_id;
        evt->killer_id = killer_id;
        strncpy(evt->victim_name, victim_name, sizeof(evt->victim_name) - 1);
        strncpy(evt->killer_name, killer_name, sizeof(evt->killer_name) - 1);
        evt->display_timer = 0;
        evt->active = true;
    }

    /* Track stats */
    if (victim_id == g_combat.local_player_id) {
        g_combat.total_deaths++;
        g_combat.current_combo = 0;
    } else if (killer_id == g_combat.local_player_id) {
        g_combat.total_kills++;
    }
}

void sync_combat_receive_respawn(uint32_t entity_id, float x, float y)
{
    /* Entity respawned */
    printf("[Combat] Entity %u respawned at (%.1f, %.1f)\n", entity_id, x, y);
}

/*
 * ============================================================================
 * Update
 * ============================================================================
 */

void sync_combat_update(float delta)
{
    if (!g_combat.initialized) return;

    uint32_t now = SDL_GetTicks();

    /* Update pending attacks */
    for (int i = 0; i < MAX_PENDING_ATTACKS; i++) {
        PendingAttack* attack = &g_combat.attacks[i];
        if (!attack->active) continue;

        /* Check timeout */
        if (now - attack->start_time > attack->duration_ms + ATTACK_TIMEOUT_MS) {
            attack->active = false;
            g_combat.attack_count--;
        }
    }

    /* Update damage displays */
    for (int i = 0; i < MAX_DAMAGE_EVENTS; i++) {
        DamageEvent* evt = &g_combat.damage_events[i];
        if (!evt->active) continue;

        evt->display_timer += delta;
        evt->y += evt->velocity_y * delta;
        evt->velocity_y += 200.0f * delta;  /* Gravity */

        if (evt->display_timer >= DAMAGE_DISPLAY_TIME) {
            evt->active = false;
            g_combat.damage_event_count--;
        }
    }

    /* Update kill feed */
    for (int i = 0; i < MAX_KILL_EVENTS; i++) {
        KillEvent* evt = &g_combat.kill_events[i];
        if (!evt->active) continue;

        evt->display_timer += delta;
        if (evt->display_timer >= KILL_DISPLAY_TIME) {
            evt->active = false;
            g_combat.kill_event_count--;
        }
    }

    /* Update combo timer */
    if (g_combat.combo_timer > 0) {
        g_combat.combo_timer -= delta;
        if (g_combat.combo_timer <= 0) {
            g_combat.current_combo = 0;
        }
    }
}

/*
 * ============================================================================
 * Query Functions
 * ============================================================================
 */

int sync_combat_get_damage_events(DamageEvent** out_events, int max_count)
{
    int count = 0;
    for (int i = 0; i < MAX_DAMAGE_EVENTS && count < max_count; i++) {
        if (g_combat.damage_events[i].active) {
            out_events[count++] = &g_combat.damage_events[i];
        }
    }
    return count;
}

int sync_combat_get_kill_events(KillEvent** out_events, int max_count)
{
    int count = 0;
    for (int i = 0; i < MAX_KILL_EVENTS && count < max_count; i++) {
        if (g_combat.kill_events[i].active) {
            out_events[count++] = &g_combat.kill_events[i];
        }
    }
    return count;
}

int sync_combat_get_combo(void)
{
    return g_combat.current_combo;
}

float sync_combat_get_combo_timer(void)
{
    return g_combat.combo_timer;
}

int sync_combat_get_max_combo(void)
{
    return g_combat.max_combo;
}

void sync_combat_get_stats(uint32_t* damage_dealt, uint32_t* damage_taken,
                           uint32_t* kills, uint32_t* deaths,
                           uint32_t* hits, uint32_t* misses)
{
    *damage_dealt = g_combat.total_damage_dealt;
    *damage_taken = g_combat.total_damage_taken;
    *kills = g_combat.total_kills;
    *deaths = g_combat.total_deaths;
    *hits = g_combat.hits_landed;
    *misses = g_combat.hits_missed;
}

/*
 * ============================================================================
 * Monster Damage (from dungeon scene)
 * ============================================================================
 */

void sync_combat_send_monster_damage(uint32_t monster_id, int32_t damage,
                                     uint32_t skill_id)
{
    struct {
        uint32_t    monster_id;
        int32_t     damage;
        uint32_t    skill_id;
    } packet = { monster_id, damage, skill_id };

    network_send_packet(MSG_DIE_MONSTER, &packet, sizeof(packet));

    /* Optimistic update */
    float x, y;
    if (sync_position_get_render_position(monster_id, &x, &y)) {
        DamageEvent* evt = alloc_damage_event();
        evt->target_id = monster_id;
        evt->damage = damage;
        evt->type = DAMAGE_NORMAL;
        evt->x = x + (rand() % 20 - 10);
        evt->y = y - 40;
        evt->display_timer = 0;
        evt->velocity_y = -100.0f;
    }

    g_combat.total_damage_dealt += damage;
    g_combat.hits_landed++;

    /* Combo */
    g_combat.current_combo++;
    g_combat.combo_timer = 3.0f;
    if (g_combat.current_combo > g_combat.max_combo) {
        g_combat.max_combo = g_combat.current_combo;
    }
}

void sync_combat_receive_monster_death(uint32_t monster_id, uint32_t exp,
                                       uint32_t gold)
{
    printf("[Combat] Monster %u killed! +%u exp, +%u gold\n",
           monster_id, exp, gold);
}
