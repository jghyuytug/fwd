/**
 * @file skill_system.c
 * @brief Skill management, cooldowns, and combo system
 */

#include "../../include/client.h"
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

#define MAX_SKILLS              100
#define MAX_EQUIPPED_SKILLS     12
#define MAX_SKILL_SLOTS         6       /* Quickbar slots */
#define MAX_COMBO_INPUTS        10
#define COMBO_INPUT_TIMEOUT     0.5f
#define SKILL_QUEUE_SIZE        3
#define CANCEL_WINDOW           0.15f   /* Skill cancel window */

/*
 * ============================================================================
 * Skill Types
 * ============================================================================
 */

typedef enum {
    SKILL_TYPE_ACTIVE,
    SKILL_TYPE_PASSIVE,
    SKILL_TYPE_BUFF,
    SKILL_TYPE_TOGGLE
} SkillType;

typedef enum {
    SKILL_TARGET_SELF,
    SKILL_TARGET_ENEMY,
    SKILL_TARGET_ALLY,
    SKILL_TARGET_GROUND,
    SKILL_TARGET_DIRECTION
} SkillTargetType;

typedef enum {
    DAMAGE_TYPE_PHYSICAL,
    DAMAGE_TYPE_MAGICAL,
    DAMAGE_TYPE_FIXED,
    DAMAGE_TYPE_PERCENT
} DamageType;

typedef enum {
    SKILL_STATE_READY,
    SKILL_STATE_CASTING,
    SKILL_STATE_ACTIVE,
    SKILL_STATE_COOLDOWN
} SkillState;

/*
 * ============================================================================
 * Skill Definition
 * ============================================================================
 */

typedef struct {
    uint32_t        id;
    char            name[64];
    char            description[256];
    uint32_t        icon_id;

    SkillType       type;
    SkillTargetType target_type;
    DamageType      damage_type;

    /* Requirements */
    uint8_t         required_level;
    uint32_t        required_sp;        /* SP to learn */
    uint32_t        prerequisite_id;    /* Required skill */
    uint8_t         prerequisite_level;

    /* Costs */
    float           mp_cost;
    float           hp_cost;
    float           stamina_cost;

    /* Timing */
    float           cast_time;          /* Pre-cast delay */
    float           duration;           /* Active time */
    float           cooldown;
    float           animation_time;

    /* Combat */
    float           base_damage;
    float           damage_multiplier;
    int             hit_count;          /* Number of damage instances */
    float           hit_interval;

    /* Range */
    float           range;
    float           width;
    float           height;

    /* Canceling */
    bool            can_cancel;         /* Can be canceled into other skills */
    bool            can_be_canceled;    /* Can cancel from this skill */
    uint32_t        cancel_skills[8];   /* Skills that can cancel into this */
    int             cancel_skill_count;

    /* Properties */
    bool            is_super_armor;     /* Immune to hitstun */
    bool            is_invincible;
    bool            can_move;
    bool            is_air_skill;

    /* Current state */
    uint8_t         level;
    bool            learned;
} SkillDefinition;

typedef struct {
    uint32_t        skill_id;
    SkillState      state;
    float           cooldown_remaining;
    float           cast_timer;
    float           active_timer;
    float           total_duration;
    int             hits_done;
    float           hit_timer;
    bool            is_channeling;
} ActiveSkill;

typedef struct {
    int             action;             /* Input action ID */
    uint32_t        timestamp;
} ComboInput;

typedef struct {
    /* Learned skills */
    SkillDefinition skills[MAX_SKILLS];
    int             skill_count;

    /* Equipped skills */
    uint32_t        equipped[MAX_EQUIPPED_SKILLS];
    int             equipped_count;

    /* Quickbar */
    uint32_t        quickbar[MAX_SKILL_SLOTS];

    /* Active skills */
    ActiveSkill     active;
    uint32_t        queued_skills[SKILL_QUEUE_SIZE];
    int             queue_head;
    int             queue_tail;

    /* Combo system */
    ComboInput      combo_inputs[MAX_COMBO_INPUTS];
    int             combo_input_count;
    float           combo_timer;

    /* Global cooldown */
    float           gcd_remaining;

    /* Skill points */
    uint32_t        sp_available;
    uint32_t        sp_used;

    /* Cooldown reduction */
    float           cdr_percent;        /* 0-1 */

    bool            initialized;
} SkillSystem;

static SkillSystem g_skills = {0};

/*
 * ============================================================================
 * External Functions (from player_controller.c)
 * ============================================================================
 */

extern void player_start_skill(int skill_slot, float duration);
extern bool player_consume_mp(float amount);
extern float player_get_mp(void);
extern bool player_is_attacking(void);
extern int player_get_state(void);

/*
 * ============================================================================
 * Helper Functions
 * ============================================================================
 */

static SkillDefinition* find_skill(uint32_t skill_id)
{
    for (int i = 0; i < g_skills.skill_count; i++) {
        if (g_skills.skills[i].id == skill_id) {
            return &g_skills.skills[i];
        }
    }
    return NULL;
}

static float get_effective_cooldown(SkillDefinition* skill)
{
    float cd = skill->cooldown;
    cd *= (1.0f - g_skills.cdr_percent);
    if (cd < 0.5f) cd = 0.5f;  /* Minimum cooldown */
    return cd;
}

static bool can_use_skill(SkillDefinition* skill)
{
    if (!skill || !skill->learned) return false;
    if (skill->level == 0) return false;
    if (skill->type == SKILL_TYPE_PASSIVE) return false;

    /* Check cooldown */
    for (int i = 0; i < MAX_SKILL_SLOTS; i++) {
        if (g_skills.quickbar[i] == skill->id) {
            /* Find active state */
        }
    }

    /* Check MP */
    if (player_get_mp() < skill->mp_cost) return false;

    /* Check GCD */
    if (g_skills.gcd_remaining > 0) return false;

    return true;
}

/*
 * ============================================================================
 * Initialization
 * ============================================================================
 */

void skill_init(void)
{
    memset(&g_skills, 0, sizeof(SkillSystem));
    g_skills.sp_available = 0;
    g_skills.cdr_percent = 0;
    g_skills.initialized = true;
}

void skill_shutdown(void)
{
    g_skills.initialized = false;
}

/*
 * ============================================================================
 * Skill Management
 * ============================================================================
 */

void skill_register(const SkillDefinition* def)
{
    if (g_skills.skill_count >= MAX_SKILLS) {
        printf("[Skill] Max skills reached\n");
        return;
    }

    memcpy(&g_skills.skills[g_skills.skill_count], def, sizeof(SkillDefinition));
    g_skills.skill_count++;
}

bool skill_learn(uint32_t skill_id)
{
    SkillDefinition* skill = find_skill(skill_id);
    if (!skill) return false;

    /* Check prerequisites */
    if (skill->prerequisite_id > 0) {
        SkillDefinition* prereq = find_skill(skill->prerequisite_id);
        if (!prereq || prereq->level < skill->prerequisite_level) {
            return false;
        }
    }

    /* Check SP */
    if (g_skills.sp_available < skill->required_sp) {
        return false;
    }

    /* Learn skill */
    g_skills.sp_available -= skill->required_sp;
    g_skills.sp_used += skill->required_sp;
    skill->learned = true;
    skill->level = 1;

    return true;
}

bool skill_level_up(uint32_t skill_id)
{
    SkillDefinition* skill = find_skill(skill_id);
    if (!skill || !skill->learned) return false;

    /* Check max level (10) */
    if (skill->level >= 10) return false;

    /* SP cost scales with level */
    uint32_t sp_cost = skill->required_sp + (skill->level * 50);
    if (g_skills.sp_available < sp_cost) return false;

    g_skills.sp_available -= sp_cost;
    g_skills.sp_used += sp_cost;
    skill->level++;

    return true;
}

void skill_reset_all(void)
{
    g_skills.sp_available += g_skills.sp_used;
    g_skills.sp_used = 0;

    for (int i = 0; i < g_skills.skill_count; i++) {
        g_skills.skills[i].learned = false;
        g_skills.skills[i].level = 0;
    }
}

/*
 * ============================================================================
 * Quickbar
 * ============================================================================
 */

void skill_set_quickbar(int slot, uint32_t skill_id)
{
    if (slot < 0 || slot >= MAX_SKILL_SLOTS) return;

    /* Verify skill exists and is learned */
    SkillDefinition* skill = find_skill(skill_id);
    if (skill && skill->learned) {
        g_skills.quickbar[slot] = skill_id;
    }
}

uint32_t skill_get_quickbar(int slot)
{
    if (slot < 0 || slot >= MAX_SKILL_SLOTS) return 0;
    return g_skills.quickbar[slot];
}

float skill_get_cooldown_remaining(int slot)
{
    if (slot < 0 || slot >= MAX_SKILL_SLOTS) return 0;

    uint32_t skill_id = g_skills.quickbar[slot];
    if (skill_id == 0) return 0;

    /* Check if this skill is the active one */
    if (g_skills.active.skill_id == skill_id &&
        g_skills.active.state == SKILL_STATE_COOLDOWN) {
        return g_skills.active.cooldown_remaining;
    }

    return 0;
}

float skill_get_cooldown_percent(int slot)
{
    if (slot < 0 || slot >= MAX_SKILL_SLOTS) return 0;

    uint32_t skill_id = g_skills.quickbar[slot];
    SkillDefinition* skill = find_skill(skill_id);
    if (!skill) return 0;

    float remaining = skill_get_cooldown_remaining(slot);
    float total = get_effective_cooldown(skill);

    if (total <= 0) return 0;
    return remaining / total;
}

/*
 * ============================================================================
 * Skill Usage
 * ============================================================================
 */

bool skill_use(int slot)
{
    if (slot < 0 || slot >= MAX_SKILL_SLOTS) return false;

    uint32_t skill_id = g_skills.quickbar[slot];
    if (skill_id == 0) return false;

    SkillDefinition* skill = find_skill(skill_id);
    if (!skill || !can_use_skill(skill)) return false;

    /* Check if can cancel current skill */
    if (g_skills.active.state == SKILL_STATE_ACTIVE) {
        SkillDefinition* active_skill = find_skill(g_skills.active.skill_id);
        if (active_skill && !active_skill->can_be_canceled) {
            /* Check cancel window */
            float elapsed = g_skills.active.active_timer;
            float duration = g_skills.active.total_duration;
            if (elapsed < duration - CANCEL_WINDOW) {
                /* Queue skill instead */
                skill_queue(skill_id);
                return false;
            }
        }
    }

    /* Consume MP */
    if (!player_consume_mp(skill->mp_cost)) {
        return false;
    }

    /* Start skill */
    g_skills.active.skill_id = skill_id;
    g_skills.active.state = SKILL_STATE_CASTING;
    g_skills.active.cast_timer = skill->cast_time;
    g_skills.active.active_timer = 0;
    g_skills.active.total_duration = skill->animation_time;
    g_skills.active.hits_done = 0;
    g_skills.active.hit_timer = 0;
    g_skills.active.is_channeling = false;

    /* Set GCD */
    g_skills.gcd_remaining = 0.2f;

    /* Notify player controller */
    player_start_skill(slot, skill->animation_time);

    return true;
}

void skill_queue(uint32_t skill_id)
{
    int next_tail = (g_skills.queue_tail + 1) % SKILL_QUEUE_SIZE;
    if (next_tail == g_skills.queue_head) {
        /* Queue full, drop oldest */
        g_skills.queue_head = (g_skills.queue_head + 1) % SKILL_QUEUE_SIZE;
    }

    g_skills.queued_skills[g_skills.queue_tail] = skill_id;
    g_skills.queue_tail = next_tail;
}

void skill_cancel(void)
{
    if (g_skills.active.state != SKILL_STATE_ACTIVE) return;

    SkillDefinition* skill = find_skill(g_skills.active.skill_id);
    if (skill && skill->can_be_canceled) {
        /* Start cooldown */
        g_skills.active.state = SKILL_STATE_COOLDOWN;
        g_skills.active.cooldown_remaining = get_effective_cooldown(skill);
    }
}

/*
 * ============================================================================
 * Combo System
 * ============================================================================
 */

void skill_record_input(int action)
{
    uint32_t now = SDL_GetTicks();

    /* Add to combo buffer */
    if (g_skills.combo_input_count < MAX_COMBO_INPUTS) {
        g_skills.combo_inputs[g_skills.combo_input_count].action = action;
        g_skills.combo_inputs[g_skills.combo_input_count].timestamp = now;
        g_skills.combo_input_count++;
    } else {
        /* Shift buffer */
        memmove(&g_skills.combo_inputs[0], &g_skills.combo_inputs[1],
                sizeof(ComboInput) * (MAX_COMBO_INPUTS - 1));
        g_skills.combo_inputs[MAX_COMBO_INPUTS - 1].action = action;
        g_skills.combo_inputs[MAX_COMBO_INPUTS - 1].timestamp = now;
    }

    g_skills.combo_timer = COMBO_INPUT_TIMEOUT;
}

bool skill_check_combo(const int* pattern, int pattern_length)
{
    if (g_skills.combo_input_count < pattern_length) return false;

    /* Check last N inputs */
    int start = g_skills.combo_input_count - pattern_length;
    for (int i = 0; i < pattern_length; i++) {
        if (g_skills.combo_inputs[start + i].action != pattern[i]) {
            return false;
        }
    }

    return true;
}

void skill_clear_combo(void)
{
    g_skills.combo_input_count = 0;
    g_skills.combo_timer = 0;
}

/*
 * ============================================================================
 * Update
 * ============================================================================
 */

void skill_update(float delta)
{
    if (!g_skills.initialized) return;

    /* Update GCD */
    if (g_skills.gcd_remaining > 0) {
        g_skills.gcd_remaining -= delta;
    }

    /* Update combo timer */
    if (g_skills.combo_timer > 0) {
        g_skills.combo_timer -= delta;
        if (g_skills.combo_timer <= 0) {
            skill_clear_combo();
        }
    }

    /* Update active skill */
    switch (g_skills.active.state) {
        case SKILL_STATE_CASTING:
            g_skills.active.cast_timer -= delta;
            if (g_skills.active.cast_timer <= 0) {
                g_skills.active.state = SKILL_STATE_ACTIVE;
            }
            break;

        case SKILL_STATE_ACTIVE:
            g_skills.active.active_timer += delta;

            /* Handle multi-hit skills */
            {
                SkillDefinition* skill = find_skill(g_skills.active.skill_id);
                if (skill && skill->hit_count > 1) {
                    g_skills.active.hit_timer += delta;
                    if (g_skills.active.hit_timer >= skill->hit_interval &&
                        g_skills.active.hits_done < skill->hit_count) {
                        g_skills.active.hits_done++;
                        g_skills.active.hit_timer = 0;
                        /* Trigger hit callback */
                        skill_on_hit(g_skills.active.skill_id, g_skills.active.hits_done);
                    }
                }
            }

            /* Check duration */
            if (g_skills.active.active_timer >= g_skills.active.total_duration) {
                SkillDefinition* skill = find_skill(g_skills.active.skill_id);
                g_skills.active.state = SKILL_STATE_COOLDOWN;
                g_skills.active.cooldown_remaining = get_effective_cooldown(skill);
            }
            break;

        case SKILL_STATE_COOLDOWN:
            g_skills.active.cooldown_remaining -= delta;
            if (g_skills.active.cooldown_remaining <= 0) {
                g_skills.active.state = SKILL_STATE_READY;
                g_skills.active.skill_id = 0;

                /* Check queued skills */
                if (g_skills.queue_head != g_skills.queue_tail) {
                    uint32_t queued = g_skills.queued_skills[g_skills.queue_head];
                    g_skills.queue_head = (g_skills.queue_head + 1) % SKILL_QUEUE_SIZE;

                    /* Find slot for queued skill */
                    for (int i = 0; i < MAX_SKILL_SLOTS; i++) {
                        if (g_skills.quickbar[i] == queued) {
                            skill_use(i);
                            break;
                        }
                    }
                }
            }
            break;

        default:
            break;
    }

    /* Handle skill input */
    if (input_action_just_pressed(ACTION_SKILL_1)) {
        skill_record_input(ACTION_SKILL_1);
        skill_use(0);
    }
    if (input_action_just_pressed(ACTION_SKILL_2)) {
        skill_record_input(ACTION_SKILL_2);
        skill_use(1);
    }
    if (input_action_just_pressed(ACTION_SKILL_3)) {
        skill_record_input(ACTION_SKILL_3);
        skill_use(2);
    }
    if (input_action_just_pressed(ACTION_SKILL_4)) {
        skill_record_input(ACTION_SKILL_4);
        skill_use(3);
    }
}

/*
 * ============================================================================
 * Callbacks (weak symbols, can be overridden)
 * ============================================================================
 */

__attribute__((weak))
void skill_on_hit(uint32_t skill_id, int hit_number)
{
    /* Default: do nothing */
    (void)skill_id;
    (void)hit_number;
}

__attribute__((weak))
void skill_on_complete(uint32_t skill_id)
{
    /* Default: do nothing */
    (void)skill_id;
}

/*
 * ============================================================================
 * Query Functions
 * ============================================================================
 */

bool skill_is_learned(uint32_t skill_id)
{
    SkillDefinition* skill = find_skill(skill_id);
    return skill && skill->learned;
}

int skill_get_level(uint32_t skill_id)
{
    SkillDefinition* skill = find_skill(skill_id);
    return skill ? skill->level : 0;
}

const char* skill_get_name(uint32_t skill_id)
{
    SkillDefinition* skill = find_skill(skill_id);
    return skill ? skill->name : "Unknown";
}

const char* skill_get_description(uint32_t skill_id)
{
    SkillDefinition* skill = find_skill(skill_id);
    return skill ? skill->description : "";
}

float skill_get_damage(uint32_t skill_id)
{
    SkillDefinition* skill = find_skill(skill_id);
    if (!skill) return 0;

    /* Base damage scales with level */
    float damage = skill->base_damage * (1.0f + (skill->level - 1) * 0.15f);
    damage *= skill->damage_multiplier;
    return damage;
}

float skill_get_mp_cost(uint32_t skill_id)
{
    SkillDefinition* skill = find_skill(skill_id);
    return skill ? skill->mp_cost : 0;
}

uint32_t skill_get_sp_available(void)
{
    return g_skills.sp_available;
}

void skill_add_sp(uint32_t amount)
{
    g_skills.sp_available += amount;
}

void skill_set_cdr(float percent)
{
    g_skills.cdr_percent = percent;
    if (g_skills.cdr_percent > 0.5f) {
        g_skills.cdr_percent = 0.5f;  /* Cap at 50% */
    }
}

/*
 * ============================================================================
 * Active Skill Info
 * ============================================================================
 */

uint32_t skill_get_active_id(void)
{
    if (g_skills.active.state == SKILL_STATE_ACTIVE ||
        g_skills.active.state == SKILL_STATE_CASTING) {
        return g_skills.active.skill_id;
    }
    return 0;
}

bool skill_is_casting(void)
{
    return g_skills.active.state == SKILL_STATE_CASTING;
}

bool skill_is_active(void)
{
    return g_skills.active.state == SKILL_STATE_ACTIVE;
}

float skill_get_cast_progress(void)
{
    if (g_skills.active.state != SKILL_STATE_CASTING) return 1.0f;

    SkillDefinition* skill = find_skill(g_skills.active.skill_id);
    if (!skill || skill->cast_time <= 0) return 1.0f;

    return 1.0f - (g_skills.active.cast_timer / skill->cast_time);
}

/*
 * ============================================================================
 * Data Loading
 * ============================================================================
 */

void skill_load_from_server(const SkillData* data, int count)
{
    for (int i = 0; i < count && i < MAX_SKILLS; i++) {
        SkillDefinition def = {0};
        def.id = data[i].skill_id;
        def.level = data[i].level;
        def.learned = data[i].level > 0;
        /* Other properties would come from skill table */
        skill_register(&def);
    }
}

void skill_load_quickbar_from_server(const uint32_t* slots, int count)
{
    for (int i = 0; i < count && i < MAX_SKILL_SLOTS; i++) {
        g_skills.quickbar[i] = slots[i];
    }
}
