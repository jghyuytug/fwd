/**
 * @file player_controller.c
 * @brief Player movement and state control
 */

#include "../../include/client.h"
#include "../../include/renderer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/*
 * ============================================================================
 * Constants
 * ============================================================================
 */

#define GROUND_Y                500.0f
#define WALK_SPEED              200.0f
#define RUN_SPEED               350.0f
#define DASH_SPEED              600.0f
#define DASH_DURATION           0.2f
#define DASH_COOLDOWN           0.5f
#define JUMP_FORCE              -400.0f
#define DOUBLE_JUMP_FORCE       -350.0f
#define JUMP_HORIZONTAL_MULT    0.8f
#define FALL_SPEED_MAX          800.0f
#define AIR_CONTROL             0.6f
#define COYOTE_TIME             0.1f    /* Jump grace period */
#define JUMP_BUFFER_TIME        0.1f    /* Pre-landing jump buffer */
#define ATTACK_MOVEMENT_MULT    0.3f
#define HITSTUN_DURATION        0.3f
#define INVINCIBILITY_DURATION  1.0f

/*
 * ============================================================================
 * Player State
 * ============================================================================
 */

typedef enum {
    PLAYER_STATE_IDLE,
    PLAYER_STATE_WALK,
    PLAYER_STATE_RUN,
    PLAYER_STATE_JUMP,
    PLAYER_STATE_FALL,
    PLAYER_STATE_DASH,
    PLAYER_STATE_ATTACK,
    PLAYER_STATE_SKILL,
    PLAYER_STATE_HITSTUN,
    PLAYER_STATE_DOWN,
    PLAYER_STATE_GETUP,
    PLAYER_STATE_DEAD
} PlayerState;

typedef enum {
    FACING_RIGHT = 1,
    FACING_LEFT = -1
} FacingDirection;

typedef struct {
    /* Position and physics */
    float           x;
    float           y;
    float           velocity_x;
    float           velocity_y;
    float           gravity;

    /* State */
    PlayerState     state;
    PlayerState     prev_state;
    FacingDirection facing;
    float           state_timer;

    /* Movement flags */
    bool            on_ground;
    bool            can_double_jump;
    float           coyote_timer;
    float           jump_buffer_timer;

    /* Dash */
    float           dash_timer;
    float           dash_cooldown_timer;
    float           dash_dir_x;
    float           dash_dir_y;
    int             air_dash_count;
    int             max_air_dashes;

    /* Combat */
    int             combo_count;
    float           combo_timer;
    float           attack_timer;
    int             current_attack;
    bool            attack_connected;

    /* Damage */
    float           hitstun_timer;
    float           invincibility_timer;
    bool            is_invincible;
    float           knockback_x;
    float           knockback_y;

    /* Animation */
    int             anim_frame;
    float           anim_timer;

    /* Stats */
    float           hp;
    float           max_hp;
    float           mp;
    float           max_mp;
    float           stamina;
    float           max_stamina;

    /* Bounds */
    float           width;
    float           height;
    Rect            hitbox;

    /* Network sync */
    uint32_t        last_sync_time;
    float           target_x;
    float           target_y;

    bool            initialized;
} PlayerController;

static PlayerController g_player = {0};

/*
 * ============================================================================
 * Helper Functions
 * ============================================================================
 */

static void set_state(PlayerState new_state)
{
    if (g_player.state == new_state) return;

    g_player.prev_state = g_player.state;
    g_player.state = new_state;
    g_player.state_timer = 0;
    g_player.anim_frame = 0;
    g_player.anim_timer = 0;
}

static bool can_act(void)
{
    return g_player.state != PLAYER_STATE_HITSTUN &&
           g_player.state != PLAYER_STATE_DOWN &&
           g_player.state != PLAYER_STATE_GETUP &&
           g_player.state != PLAYER_STATE_DEAD;
}

static bool can_move(void)
{
    if (!can_act()) return false;
    if (g_player.state == PLAYER_STATE_DASH) return false;
    if (g_player.state == PLAYER_STATE_ATTACK && !g_player.on_ground) return true;
    if (g_player.state == PLAYER_STATE_ATTACK) return false;
    if (g_player.state == PLAYER_STATE_SKILL) return false;
    return true;
}

static bool can_jump(void)
{
    if (!can_act()) return false;
    if (g_player.state == PLAYER_STATE_DASH) return false;
    return g_player.on_ground || g_player.coyote_timer > 0 || g_player.can_double_jump;
}

static bool can_dash(void)
{
    if (!can_act()) return false;
    if (g_player.dash_cooldown_timer > 0) return false;
    if (!g_player.on_ground && g_player.air_dash_count >= g_player.max_air_dashes) return false;
    return true;
}

static bool can_attack(void)
{
    if (!can_act()) return false;
    if (g_player.state == PLAYER_STATE_DASH) return false;
    return true;
}

/*
 * ============================================================================
 * Initialization
 * ============================================================================
 */

void player_init(float x, float y)
{
    memset(&g_player, 0, sizeof(PlayerController));

    g_player.x = x;
    g_player.y = y;
    g_player.target_x = x;
    g_player.target_y = y;
    g_player.gravity = GRAVITY * 60.0f;  /* Convert to per-second */

    g_player.state = PLAYER_STATE_IDLE;
    g_player.facing = FACING_RIGHT;

    g_player.max_air_dashes = 1;

    /* Default stats */
    g_player.max_hp = 1000;
    g_player.hp = g_player.max_hp;
    g_player.max_mp = 500;
    g_player.mp = g_player.max_mp;
    g_player.max_stamina = 100;
    g_player.stamina = g_player.max_stamina;

    /* Collision box */
    g_player.width = 48;
    g_player.height = 80;
    g_player.hitbox.w = 32;
    g_player.hitbox.h = 64;

    g_player.initialized = true;
}

void player_shutdown(void)
{
    g_player.initialized = false;
}

/*
 * ============================================================================
 * Movement
 * ============================================================================
 */

static void handle_movement(float delta)
{
    if (!can_move()) return;

    float move_x, move_y;
    input_get_movement_vector(&move_x, &move_y);

    /* Determine speed */
    float speed = WALK_SPEED;
    if (input_action_held(ACTION_DASH) && g_player.on_ground) {
        speed = RUN_SPEED;
    }

    /* Air control */
    if (!g_player.on_ground) {
        speed *= AIR_CONTROL;
    }

    /* Apply horizontal movement */
    if (fabsf(move_x) > 0.1f) {
        g_player.velocity_x = move_x * speed;
        g_player.facing = move_x > 0 ? FACING_RIGHT : FACING_LEFT;

        if (g_player.on_ground && can_act()) {
            if (speed > WALK_SPEED) {
                set_state(PLAYER_STATE_RUN);
            } else {
                set_state(PLAYER_STATE_WALK);
            }
        }
    } else if (g_player.on_ground) {
        /* Friction */
        g_player.velocity_x *= 0.7f;
        if (fabsf(g_player.velocity_x) < 10.0f) {
            g_player.velocity_x = 0;
            if (g_player.state == PLAYER_STATE_WALK ||
                g_player.state == PLAYER_STATE_RUN) {
                set_state(PLAYER_STATE_IDLE);
            }
        }
    }
}

static void handle_jump(float delta)
{
    /* Buffer jump input */
    if (input_action_just_pressed(ACTION_JUMP)) {
        g_player.jump_buffer_timer = JUMP_BUFFER_TIME;
    }

    /* Check for buffered or immediate jump */
    bool want_jump = g_player.jump_buffer_timer > 0 || input_action_just_pressed(ACTION_JUMP);

    if (want_jump && can_jump()) {
        if (g_player.on_ground || g_player.coyote_timer > 0) {
            /* Ground jump */
            g_player.velocity_y = JUMP_FORCE;
            g_player.on_ground = false;
            g_player.coyote_timer = 0;
            g_player.can_double_jump = true;
            set_state(PLAYER_STATE_JUMP);
        } else if (g_player.can_double_jump) {
            /* Double jump */
            g_player.velocity_y = DOUBLE_JUMP_FORCE;
            g_player.can_double_jump = false;
            set_state(PLAYER_STATE_JUMP);
        }
        g_player.jump_buffer_timer = 0;
    }

    /* Variable jump height */
    if (!input_action_pressed(ACTION_JUMP) && g_player.velocity_y < 0) {
        g_player.velocity_y *= 0.5f;
    }
}

static void handle_dash(float delta)
{
    if (!can_dash()) return;

    if (input_action_just_pressed(ACTION_DASH) && !input_action_held(ACTION_DASH)) {
        float move_x, move_y;
        input_get_movement_vector(&move_x, &move_y);

        /* Default to facing direction if no input */
        if (fabsf(move_x) < 0.1f && fabsf(move_y) < 0.1f) {
            move_x = g_player.facing;
            move_y = 0;
        }

        /* Normalize */
        float len = sqrtf(move_x * move_x + move_y * move_y);
        if (len > 0) {
            g_player.dash_dir_x = move_x / len;
            g_player.dash_dir_y = move_y / len;
        }

        g_player.dash_timer = DASH_DURATION;
        g_player.dash_cooldown_timer = DASH_COOLDOWN;

        if (!g_player.on_ground) {
            g_player.air_dash_count++;
        }

        set_state(PLAYER_STATE_DASH);

        /* Cancel gravity during dash */
        g_player.velocity_y = 0;
    }
}

/*
 * ============================================================================
 * Combat
 * ============================================================================
 */

static void handle_attack(float delta)
{
    if (!can_attack()) return;

    /* Normal attack */
    if (input_action_just_pressed(ACTION_ATTACK)) {
        /* Combo system */
        if (g_player.state == PLAYER_STATE_ATTACK && g_player.combo_timer > 0) {
            g_player.combo_count++;
            if (g_player.combo_count > 5) g_player.combo_count = 1;
        } else {
            g_player.combo_count = 1;
        }

        g_player.current_attack = g_player.combo_count;
        g_player.attack_timer = 0.3f;  /* Attack duration */
        g_player.combo_timer = 0.5f;   /* Combo window */
        g_player.attack_connected = false;

        set_state(PLAYER_STATE_ATTACK);

        /* Apply small forward movement */
        g_player.velocity_x = g_player.facing * 50.0f;
    }
}

void player_start_skill(int skill_slot, float duration)
{
    if (!can_attack()) return;

    g_player.current_attack = skill_slot + 10;  /* Skills start at 10 */
    g_player.attack_timer = duration;
    g_player.combo_timer = 0;  /* Skills break combo */

    set_state(PLAYER_STATE_SKILL);
}

/*
 * ============================================================================
 * Damage
 * ============================================================================
 */

void player_take_damage(float damage, float knockback_x, float knockback_y)
{
    if (g_player.is_invincible) return;

    g_player.hp -= damage;
    if (g_player.hp < 0) g_player.hp = 0;

    /* Apply knockback */
    g_player.velocity_x = knockback_x;
    g_player.velocity_y = knockback_y;

    g_player.hitstun_timer = HITSTUN_DURATION;
    g_player.invincibility_timer = INVINCIBILITY_DURATION;
    g_player.is_invincible = true;

    set_state(PLAYER_STATE_HITSTUN);

    /* Death check */
    if (g_player.hp <= 0) {
        set_state(PLAYER_STATE_DEAD);
    }
}

void player_knockdown(float knockback_x, float knockback_y)
{
    g_player.velocity_x = knockback_x;
    g_player.velocity_y = knockback_y;
    set_state(PLAYER_STATE_DOWN);
}

void player_heal(float amount)
{
    g_player.hp += amount;
    if (g_player.hp > g_player.max_hp) {
        g_player.hp = g_player.max_hp;
    }
}

void player_restore_mp(float amount)
{
    g_player.mp += amount;
    if (g_player.mp > g_player.max_mp) {
        g_player.mp = g_player.max_mp;
    }
}

bool player_consume_mp(float amount)
{
    if (g_player.mp < amount) return false;
    g_player.mp -= amount;
    return true;
}

/*
 * ============================================================================
 * Physics
 * ============================================================================
 */

static void apply_physics(float delta)
{
    /* Gravity */
    if (!g_player.on_ground && g_player.state != PLAYER_STATE_DASH) {
        g_player.velocity_y += g_player.gravity * delta;
        if (g_player.velocity_y > FALL_SPEED_MAX) {
            g_player.velocity_y = FALL_SPEED_MAX;
        }
    }

    /* Dash movement */
    if (g_player.state == PLAYER_STATE_DASH) {
        g_player.velocity_x = g_player.dash_dir_x * DASH_SPEED;
        g_player.velocity_y = g_player.dash_dir_y * DASH_SPEED * 0.5f;  /* Reduced vertical */
    }

    /* Apply velocity */
    g_player.x += g_player.velocity_x * delta;
    g_player.y += g_player.velocity_y * delta;

    /* Ground collision */
    if (g_player.y >= GROUND_Y) {
        g_player.y = GROUND_Y;
        g_player.velocity_y = 0;

        if (!g_player.on_ground) {
            g_player.on_ground = true;
            g_player.can_double_jump = false;
            g_player.air_dash_count = 0;

            /* Land from air */
            if (g_player.state == PLAYER_STATE_JUMP ||
                g_player.state == PLAYER_STATE_FALL) {
                set_state(PLAYER_STATE_IDLE);
            }
        }
    } else {
        if (g_player.on_ground) {
            g_player.coyote_timer = COYOTE_TIME;
        }
        g_player.on_ground = false;
    }

    /* Update hitbox */
    g_player.hitbox.x = g_player.x - g_player.hitbox.w / 2;
    g_player.hitbox.y = g_player.y - g_player.hitbox.h;
}

/*
 * ============================================================================
 * Update
 * ============================================================================
 */

void player_update(float delta)
{
    if (!g_player.initialized) return;

    /* Update timers */
    g_player.state_timer += delta;

    if (g_player.coyote_timer > 0) {
        g_player.coyote_timer -= delta;
    }
    if (g_player.jump_buffer_timer > 0) {
        g_player.jump_buffer_timer -= delta;
    }
    if (g_player.dash_cooldown_timer > 0) {
        g_player.dash_cooldown_timer -= delta;
    }
    if (g_player.combo_timer > 0) {
        g_player.combo_timer -= delta;
        if (g_player.combo_timer <= 0) {
            g_player.combo_count = 0;
        }
    }

    /* Invincibility */
    if (g_player.is_invincible) {
        g_player.invincibility_timer -= delta;
        if (g_player.invincibility_timer <= 0) {
            g_player.is_invincible = false;
        }
    }

    /* State updates */
    switch (g_player.state) {
        case PLAYER_STATE_DASH:
            g_player.dash_timer -= delta;
            if (g_player.dash_timer <= 0) {
                set_state(g_player.on_ground ? PLAYER_STATE_IDLE : PLAYER_STATE_FALL);
            }
            break;

        case PLAYER_STATE_ATTACK:
        case PLAYER_STATE_SKILL:
            g_player.attack_timer -= delta;
            if (g_player.attack_timer <= 0) {
                set_state(g_player.on_ground ? PLAYER_STATE_IDLE : PLAYER_STATE_FALL);
            }
            break;

        case PLAYER_STATE_HITSTUN:
            g_player.hitstun_timer -= delta;
            if (g_player.hitstun_timer <= 0) {
                set_state(g_player.on_ground ? PLAYER_STATE_IDLE : PLAYER_STATE_FALL);
            }
            break;

        case PLAYER_STATE_DOWN:
            if (g_player.state_timer > 1.0f) {
                set_state(PLAYER_STATE_GETUP);
            }
            break;

        case PLAYER_STATE_GETUP:
            if (g_player.state_timer > 0.5f) {
                set_state(PLAYER_STATE_IDLE);
            }
            break;

        case PLAYER_STATE_JUMP:
            if (g_player.velocity_y > 0) {
                set_state(PLAYER_STATE_FALL);
            }
            break;

        default:
            break;
    }

    /* Input handling */
    handle_movement(delta);
    handle_jump(delta);
    handle_dash(delta);
    handle_attack(delta);

    /* Physics */
    apply_physics(delta);

    /* Stamina regeneration */
    if (g_player.stamina < g_player.max_stamina) {
        g_player.stamina += 10.0f * delta;
        if (g_player.stamina > g_player.max_stamina) {
            g_player.stamina = g_player.max_stamina;
        }
    }
}

/*
 * ============================================================================
 * Getters
 * ============================================================================
 */

void player_get_position(float* x, float* y)
{
    *x = g_player.x;
    *y = g_player.y;
}

void player_get_velocity(float* vx, float* vy)
{
    *vx = g_player.velocity_x;
    *vy = g_player.velocity_y;
}

int player_get_state(void)
{
    return g_player.state;
}

int player_get_facing(void)
{
    return g_player.facing;
}

float player_get_hp(void)
{
    return g_player.hp;
}

float player_get_max_hp(void)
{
    return g_player.max_hp;
}

float player_get_mp(void)
{
    return g_player.mp;
}

float player_get_max_mp(void)
{
    return g_player.max_mp;
}

float player_get_stamina(void)
{
    return g_player.stamina;
}

int player_get_combo_count(void)
{
    return g_player.combo_count;
}

bool player_is_invincible(void)
{
    return g_player.is_invincible;
}

bool player_is_attacking(void)
{
    return g_player.state == PLAYER_STATE_ATTACK ||
           g_player.state == PLAYER_STATE_SKILL;
}

bool player_is_on_ground(void)
{
    return g_player.on_ground;
}

Rect player_get_hitbox(void)
{
    return g_player.hitbox;
}

/*
 * ============================================================================
 * Setters
 * ============================================================================
 */

void player_set_position(float x, float y)
{
    g_player.x = x;
    g_player.y = y;
    g_player.target_x = x;
    g_player.target_y = y;
}

void player_set_velocity(float vx, float vy)
{
    g_player.velocity_x = vx;
    g_player.velocity_y = vy;
}

void player_set_stats(float max_hp, float max_mp, float max_stamina)
{
    g_player.max_hp = max_hp;
    g_player.hp = max_hp;
    g_player.max_mp = max_mp;
    g_player.mp = max_mp;
    g_player.max_stamina = max_stamina;
    g_player.stamina = max_stamina;
}

/*
 * ============================================================================
 * Network Sync
 * ============================================================================
 */

void player_sync_from_server(float x, float y, int state, int facing)
{
    g_player.target_x = x;
    g_player.target_y = y;

    /* Interpolate position */
    float dx = g_player.target_x - g_player.x;
    float dy = g_player.target_y - g_player.y;

    /* Snap if too far */
    if (fabsf(dx) > 100 || fabsf(dy) > 100) {
        g_player.x = x;
        g_player.y = y;
    }

    /* Sync state if different */
    if (state != g_player.state) {
        set_state(state);
    }
    g_player.facing = facing;
}

void player_get_sync_data(float* x, float* y, float* vx, float* vy, int* state, int* facing)
{
    *x = g_player.x;
    *y = g_player.y;
    *vx = g_player.velocity_x;
    *vy = g_player.velocity_y;
    *state = g_player.state;
    *facing = g_player.facing;
}

/*
 * ============================================================================
 * Animation Info
 * ============================================================================
 */

const char* player_get_animation_name(void)
{
    switch (g_player.state) {
        case PLAYER_STATE_IDLE:    return "idle";
        case PLAYER_STATE_WALK:    return "walk";
        case PLAYER_STATE_RUN:     return "run";
        case PLAYER_STATE_JUMP:    return "jump";
        case PLAYER_STATE_FALL:    return "fall";
        case PLAYER_STATE_DASH:    return "dash";
        case PLAYER_STATE_ATTACK:
            switch (g_player.combo_count) {
                case 1:  return "attack1";
                case 2:  return "attack2";
                case 3:  return "attack3";
                case 4:  return "attack4";
                case 5:  return "attack5";
                default: return "attack1";
            }
        case PLAYER_STATE_SKILL:   return "skill";
        case PLAYER_STATE_HITSTUN: return "hit";
        case PLAYER_STATE_DOWN:    return "down";
        case PLAYER_STATE_GETUP:   return "getup";
        case PLAYER_STATE_DEAD:    return "dead";
        default:                   return "idle";
    }
}

float player_get_state_timer(void)
{
    return g_player.state_timer;
}
