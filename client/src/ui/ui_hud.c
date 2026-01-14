/**
 * @file ui_hud.c
 * @brief Heads-up display with HP/MP bars, skill quickbar, and status
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

#define HP_BAR_WIDTH            250
#define HP_BAR_HEIGHT           20
#define MP_BAR_HEIGHT           16
#define STAMINA_BAR_HEIGHT      8
#define EXP_BAR_HEIGHT          6
#define QUICKBAR_SLOTS          6
#define QUICKBAR_SLOT_SIZE      48
#define QUICKBAR_SPACING        4
#define ITEM_SLOT_SIZE          36
#define ITEM_SLOTS              4
#define BUFF_ICON_SIZE          28
#define MAX_BUFFS_DISPLAY       12
#define COMBO_DISPLAY_TIME      3.0f

/*
 * ============================================================================
 * HUD Data
 * ============================================================================
 */

typedef struct {
    uint32_t        skill_id;
    uint32_t        icon_id;
    float           cooldown;
    float           max_cooldown;
    uint8_t         key;            /* Keybind display */
} QuickbarSlot;

typedef struct {
    uint32_t        item_id;
    uint32_t        icon_id;
    uint16_t        quantity;
    uint8_t         key;
} ItemSlot;

typedef struct {
    uint32_t        buff_id;
    uint32_t        icon_id;
    float           remaining;
    float           duration;
    int             stacks;
    bool            is_debuff;
} BuffDisplay;

typedef struct {
    /* Visibility */
    bool            visible;

    /* Player stats */
    float           hp;
    float           max_hp;
    float           hp_display;
    float           mp;
    float           max_mp;
    float           mp_display;
    float           stamina;
    float           max_stamina;
    float           exp;
    float           exp_to_next;
    uint8_t         level;
    char            name[32];

    /* Quickbar */
    QuickbarSlot    quickbar[QUICKBAR_SLOTS];

    /* Item shortcuts */
    ItemSlot        item_slots[ITEM_SLOTS];

    /* Buffs */
    BuffDisplay     buffs[MAX_BUFFS_DISPLAY];
    int             buff_count;

    /* Combo */
    int             combo_count;
    float           combo_timer;
    int             combo_damage;

    /* Combat status */
    bool            in_combat;
    float           combat_timer;

    /* Target */
    bool            has_target;
    char            target_name[32];
    float           target_hp;
    float           target_max_hp;
    uint8_t         target_level;
    bool            target_is_boss;

    /* Notifications */
    char            notification[128];
    float           notification_timer;

    /* FPS/Ping display */
    bool            show_debug;
    int             fps;
    float           ping;

    bool            initialized;
} HUD;

static HUD g_hud = {0};

/*
 * ============================================================================
 * External Functions
 * ============================================================================
 */

extern void renderer_draw_rect(Renderer* r, Rect rect, Color color, bool filled);
extern void renderer_draw_text(Renderer* r, const char* text, int x, int y, Color color);

/*
 * ============================================================================
 * Helper Functions
 * ============================================================================
 */

static float lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}

static void draw_bar(Renderer* r, float x, float y, float width, float height,
                     float percent, Color fg, Color bg, Color border)
{
    /* Background */
    Rect bg_rect = {x, y, width, height};
    renderer_draw_rect(r, bg_rect, bg, true);

    /* Fill */
    if (percent > 0) {
        float fill_width = width * percent;
        if (fill_width < 1) fill_width = 1;
        Rect fill_rect = {x, y, fill_width, height};
        renderer_draw_rect(r, fill_rect, fg, true);
    }

    /* Border */
    renderer_draw_rect(r, bg_rect, border, false);
}

/*
 * ============================================================================
 * Initialization
 * ============================================================================
 */

void ui_hud_init(void)
{
    memset(&g_hud, 0, sizeof(HUD));

    g_hud.visible = true;
    g_hud.max_hp = 1000;
    g_hud.hp = 1000;
    g_hud.hp_display = 1000;
    g_hud.max_mp = 500;
    g_hud.mp = 500;
    g_hud.mp_display = 500;
    g_hud.max_stamina = 100;
    g_hud.stamina = 100;

    /* Default keybinds display */
    const uint8_t keys[] = {'Z', 'X', 'C', 'A', 'S', 'D'};
    for (int i = 0; i < QUICKBAR_SLOTS; i++) {
        g_hud.quickbar[i].key = keys[i];
    }

    const uint8_t item_keys[] = {'1', '2', '3', '4'};
    for (int i = 0; i < ITEM_SLOTS; i++) {
        g_hud.item_slots[i].key = item_keys[i];
    }

    g_hud.initialized = true;
}

void ui_hud_shutdown(void)
{
    g_hud.initialized = false;
}

/*
 * ============================================================================
 * Data Updates
 * ============================================================================
 */

void ui_hud_set_player_info(const char* name, uint8_t level)
{
    strncpy(g_hud.name, name, sizeof(g_hud.name) - 1);
    g_hud.level = level;
}

void ui_hud_set_hp(float hp, float max_hp)
{
    g_hud.hp = hp;
    g_hud.max_hp = max_hp;
}

void ui_hud_set_mp(float mp, float max_mp)
{
    g_hud.mp = mp;
    g_hud.max_mp = max_mp;
}

void ui_hud_set_stamina(float stamina, float max_stamina)
{
    g_hud.stamina = stamina;
    g_hud.max_stamina = max_stamina;
}

void ui_hud_set_exp(float exp, float exp_to_next)
{
    g_hud.exp = exp;
    g_hud.exp_to_next = exp_to_next;
}

void ui_hud_set_quickbar_skill(int slot, uint32_t skill_id, uint32_t icon_id,
                                float cooldown, float max_cooldown)
{
    if (slot < 0 || slot >= QUICKBAR_SLOTS) return;

    g_hud.quickbar[slot].skill_id = skill_id;
    g_hud.quickbar[slot].icon_id = icon_id;
    g_hud.quickbar[slot].cooldown = cooldown;
    g_hud.quickbar[slot].max_cooldown = max_cooldown;
}

void ui_hud_update_cooldown(int slot, float cooldown)
{
    if (slot >= 0 && slot < QUICKBAR_SLOTS) {
        g_hud.quickbar[slot].cooldown = cooldown;
    }
}

void ui_hud_set_item_slot(int slot, uint32_t item_id, uint32_t icon_id,
                           uint16_t quantity)
{
    if (slot < 0 || slot >= ITEM_SLOTS) return;

    g_hud.item_slots[slot].item_id = item_id;
    g_hud.item_slots[slot].icon_id = icon_id;
    g_hud.item_slots[slot].quantity = quantity;
}

/*
 * ============================================================================
 * Buff Display
 * ============================================================================
 */

void ui_hud_add_buff(uint32_t buff_id, uint32_t icon_id, float duration,
                      int stacks, bool is_debuff)
{
    /* Check if already exists */
    for (int i = 0; i < g_hud.buff_count; i++) {
        if (g_hud.buffs[i].buff_id == buff_id) {
            g_hud.buffs[i].remaining = duration;
            g_hud.buffs[i].duration = duration;
            g_hud.buffs[i].stacks = stacks;
            return;
        }
    }

    if (g_hud.buff_count >= MAX_BUFFS_DISPLAY) return;

    BuffDisplay* b = &g_hud.buffs[g_hud.buff_count++];
    b->buff_id = buff_id;
    b->icon_id = icon_id;
    b->duration = duration;
    b->remaining = duration;
    b->stacks = stacks;
    b->is_debuff = is_debuff;
}

void ui_hud_remove_buff(uint32_t buff_id)
{
    for (int i = 0; i < g_hud.buff_count; i++) {
        if (g_hud.buffs[i].buff_id == buff_id) {
            memmove(&g_hud.buffs[i], &g_hud.buffs[i + 1],
                    (g_hud.buff_count - i - 1) * sizeof(BuffDisplay));
            g_hud.buff_count--;
            return;
        }
    }
}

void ui_hud_clear_buffs(void)
{
    g_hud.buff_count = 0;
}

/*
 * ============================================================================
 * Target
 * ============================================================================
 */

void ui_hud_set_target(const char* name, uint8_t level, float hp, float max_hp,
                        bool is_boss)
{
    g_hud.has_target = true;
    strncpy(g_hud.target_name, name, sizeof(g_hud.target_name) - 1);
    g_hud.target_level = level;
    g_hud.target_hp = hp;
    g_hud.target_max_hp = max_hp;
    g_hud.target_is_boss = is_boss;
}

void ui_hud_update_target_hp(float hp)
{
    g_hud.target_hp = hp;
}

void ui_hud_clear_target(void)
{
    g_hud.has_target = false;
}

/*
 * ============================================================================
 * Combo
 * ============================================================================
 */

void ui_hud_set_combo(int count, int total_damage)
{
    g_hud.combo_count = count;
    g_hud.combo_damage = total_damage;
    g_hud.combo_timer = COMBO_DISPLAY_TIME;
}

void ui_hud_add_combo_hit(int damage)
{
    g_hud.combo_count++;
    g_hud.combo_damage += damage;
    g_hud.combo_timer = COMBO_DISPLAY_TIME;
}

void ui_hud_reset_combo(void)
{
    g_hud.combo_count = 0;
    g_hud.combo_damage = 0;
}

/*
 * ============================================================================
 * Notifications
 * ============================================================================
 */

void ui_hud_show_notification(const char* message, float duration)
{
    strncpy(g_hud.notification, message, sizeof(g_hud.notification) - 1);
    g_hud.notification_timer = duration;
}

/*
 * ============================================================================
 * Update
 * ============================================================================
 */

void ui_hud_update(float delta)
{
    if (!g_hud.initialized) return;

    /* Smooth HP/MP display */
    g_hud.hp_display = lerp(g_hud.hp_display, g_hud.hp, 8.0f * delta);
    g_hud.mp_display = lerp(g_hud.mp_display, g_hud.mp, 8.0f * delta);

    /* Update cooldowns */
    for (int i = 0; i < QUICKBAR_SLOTS; i++) {
        if (g_hud.quickbar[i].cooldown > 0) {
            g_hud.quickbar[i].cooldown -= delta;
            if (g_hud.quickbar[i].cooldown < 0) {
                g_hud.quickbar[i].cooldown = 0;
            }
        }
    }

    /* Update buff timers */
    for (int i = 0; i < g_hud.buff_count; i++) {
        g_hud.buffs[i].remaining -= delta;
        if (g_hud.buffs[i].remaining <= 0) {
            ui_hud_remove_buff(g_hud.buffs[i].buff_id);
            i--;
        }
    }

    /* Update combo timer */
    if (g_hud.combo_timer > 0) {
        g_hud.combo_timer -= delta;
        if (g_hud.combo_timer <= 0) {
            ui_hud_reset_combo();
        }
    }

    /* Update notification */
    if (g_hud.notification_timer > 0) {
        g_hud.notification_timer -= delta;
    }

    /* Combat timer */
    if (g_hud.in_combat) {
        g_hud.combat_timer += delta;
    }
}

/*
 * ============================================================================
 * Rendering
 * ============================================================================
 */

void ui_hud_render(Renderer* renderer)
{
    if (!g_hud.visible || !renderer) return;

    Color white = {255, 255, 255, 255};
    Color hp_fg = {200, 50, 50, 255};
    Color hp_bg = {60, 20, 20, 255};
    Color mp_fg = {50, 100, 200, 255};
    Color mp_bg = {20, 30, 60, 255};
    Color stamina_fg = {200, 200, 50, 255};
    Color stamina_bg = {60, 60, 20, 255};
    Color exp_fg = {100, 200, 100, 255};
    Color exp_bg = {30, 60, 30, 255};
    Color border = {80, 80, 80, 255};
    Color slot_bg = {40, 40, 50, 255};
    Color cooldown_overlay = {0, 0, 0, 180};

    float base_x = 20;
    float base_y = WINDOW_HEIGHT - 120;

    /* Player name and level */
    char info[64];
    snprintf(info, sizeof(info), "Lv.%d %s", g_hud.level, g_hud.name);
    renderer_draw_text(renderer, info, (int)base_x, (int)base_y - 25, white);

    /* HP Bar */
    float hp_percent = g_hud.max_hp > 0 ? g_hud.hp_display / g_hud.max_hp : 0;
    draw_bar(renderer, base_x, base_y, HP_BAR_WIDTH, HP_BAR_HEIGHT,
             hp_percent, hp_fg, hp_bg, border);

    char hp_text[32];
    snprintf(hp_text, sizeof(hp_text), "%.0f / %.0f", g_hud.hp, g_hud.max_hp);
    renderer_draw_text(renderer, hp_text, (int)base_x + 5, (int)base_y + 3, white);

    /* MP Bar */
    float mp_y = base_y + HP_BAR_HEIGHT + 2;
    float mp_percent = g_hud.max_mp > 0 ? g_hud.mp_display / g_hud.max_mp : 0;
    draw_bar(renderer, base_x, mp_y, HP_BAR_WIDTH, MP_BAR_HEIGHT,
             mp_percent, mp_fg, mp_bg, border);

    char mp_text[32];
    snprintf(mp_text, sizeof(mp_text), "%.0f / %.0f", g_hud.mp, g_hud.max_mp);
    renderer_draw_text(renderer, mp_text, (int)base_x + 5, (int)mp_y + 1, white);

    /* Stamina Bar */
    float stam_y = mp_y + MP_BAR_HEIGHT + 2;
    float stam_percent = g_hud.max_stamina > 0 ? g_hud.stamina / g_hud.max_stamina : 0;
    draw_bar(renderer, base_x, stam_y, HP_BAR_WIDTH, STAMINA_BAR_HEIGHT,
             stam_percent, stamina_fg, stamina_bg, border);

    /* EXP Bar */
    float exp_y = stam_y + STAMINA_BAR_HEIGHT + 2;
    float exp_percent = g_hud.exp_to_next > 0 ? g_hud.exp / g_hud.exp_to_next : 0;
    draw_bar(renderer, base_x, exp_y, HP_BAR_WIDTH, EXP_BAR_HEIGHT,
             exp_percent, exp_fg, exp_bg, border);

    /* Skill Quickbar */
    float qb_x = WINDOW_WIDTH / 2 - (QUICKBAR_SLOTS * (QUICKBAR_SLOT_SIZE + QUICKBAR_SPACING)) / 2;
    float qb_y = WINDOW_HEIGHT - QUICKBAR_SLOT_SIZE - 20;

    for (int i = 0; i < QUICKBAR_SLOTS; i++) {
        float sx = qb_x + i * (QUICKBAR_SLOT_SIZE + QUICKBAR_SPACING);

        /* Slot background */
        Rect slot = {sx, qb_y, QUICKBAR_SLOT_SIZE, QUICKBAR_SLOT_SIZE};
        renderer_draw_rect(renderer, slot, slot_bg, true);
        renderer_draw_rect(renderer, slot, border, false);

        /* Skill icon (placeholder) */
        if (g_hud.quickbar[i].skill_id > 0) {
            Color skill_color = {100, 150, 200, 255};
            Rect icon = {sx + 4, qb_y + 4, QUICKBAR_SLOT_SIZE - 8, QUICKBAR_SLOT_SIZE - 8};
            renderer_draw_rect(renderer, icon, skill_color, true);

            /* Cooldown overlay */
            if (g_hud.quickbar[i].cooldown > 0 && g_hud.quickbar[i].max_cooldown > 0) {
                float cd_percent = g_hud.quickbar[i].cooldown / g_hud.quickbar[i].max_cooldown;
                float cd_height = (QUICKBAR_SLOT_SIZE - 8) * cd_percent;
                Rect cd_rect = {sx + 4, qb_y + 4, QUICKBAR_SLOT_SIZE - 8, cd_height};
                renderer_draw_rect(renderer, cd_rect, cooldown_overlay, true);

                /* Cooldown text */
                char cd_text[8];
                snprintf(cd_text, sizeof(cd_text), "%.1f", g_hud.quickbar[i].cooldown);
                renderer_draw_text(renderer, cd_text, (int)sx + 12, (int)qb_y + 18, white);
            }
        }

        /* Keybind */
        char key[4];
        snprintf(key, sizeof(key), "%c", g_hud.quickbar[i].key);
        renderer_draw_text(renderer, key, (int)sx + 2, (int)qb_y + 2, white);
    }

    /* Item slots */
    float item_x = qb_x + QUICKBAR_SLOTS * (QUICKBAR_SLOT_SIZE + QUICKBAR_SPACING) + 20;
    for (int i = 0; i < ITEM_SLOTS; i++) {
        float ix = item_x + i * (ITEM_SLOT_SIZE + 4);
        float iy = qb_y + (QUICKBAR_SLOT_SIZE - ITEM_SLOT_SIZE) / 2;

        Rect slot = {ix, iy, ITEM_SLOT_SIZE, ITEM_SLOT_SIZE};
        renderer_draw_rect(renderer, slot, slot_bg, true);
        renderer_draw_rect(renderer, slot, border, false);

        if (g_hud.item_slots[i].item_id > 0) {
            Color item_color = {200, 150, 100, 255};
            Rect icon = {ix + 3, iy + 3, ITEM_SLOT_SIZE - 6, ITEM_SLOT_SIZE - 6};
            renderer_draw_rect(renderer, icon, item_color, true);

            /* Quantity */
            if (g_hud.item_slots[i].quantity > 1) {
                char qty[8];
                snprintf(qty, sizeof(qty), "%d", g_hud.item_slots[i].quantity);
                renderer_draw_text(renderer, qty, (int)ix + ITEM_SLOT_SIZE - 16,
                                   (int)iy + ITEM_SLOT_SIZE - 12, white);
            }
        }

        /* Keybind */
        char key[4];
        snprintf(key, sizeof(key), "%c", g_hud.item_slots[i].key);
        renderer_draw_text(renderer, key, (int)ix + 2, (int)iy + 2, white);
    }

    /* Buffs */
    float buff_x = base_x;
    float buff_y = base_y - 50;
    for (int i = 0; i < g_hud.buff_count; i++) {
        BuffDisplay* b = &g_hud.buffs[i];
        float bx = buff_x + i * (BUFF_ICON_SIZE + 4);

        Color buff_bg = b->is_debuff ? (Color){80, 40, 40, 255} : (Color){40, 60, 80, 255};
        Rect buff = {bx, buff_y, BUFF_ICON_SIZE, BUFF_ICON_SIZE};
        renderer_draw_rect(renderer, buff, buff_bg, true);
        renderer_draw_rect(renderer, buff, border, false);

        /* Timer */
        if (b->remaining > 0 && b->remaining < 10) {
            char timer[8];
            snprintf(timer, sizeof(timer), "%.0f", b->remaining);
            renderer_draw_text(renderer, timer, (int)bx + 8, (int)buff_y + 8, white);
        }

        /* Stacks */
        if (b->stacks > 1) {
            char stacks[4];
            snprintf(stacks, sizeof(stacks), "%d", b->stacks);
            renderer_draw_text(renderer, stacks, (int)bx + BUFF_ICON_SIZE - 10,
                               (int)buff_y + BUFF_ICON_SIZE - 12, white);
        }
    }

    /* Target frame */
    if (g_hud.has_target) {
        float target_x = WINDOW_WIDTH / 2 - 100;
        float target_y = 60;

        Color target_bg = {30, 30, 40, 220};
        Rect target_frame = {target_x, target_y, 200, 50};
        renderer_draw_rect(renderer, target_frame, target_bg, true);
        renderer_draw_rect(renderer, target_frame, border, false);

        /* Target name */
        char target_info[64];
        snprintf(target_info, sizeof(target_info), "Lv.%d %s%s",
                 g_hud.target_level, g_hud.target_name,
                 g_hud.target_is_boss ? " [BOSS]" : "");
        Color name_color = g_hud.target_is_boss ? (Color){255, 100, 100, 255} : white;
        renderer_draw_text(renderer, target_info, (int)target_x + 5, (int)target_y + 5, name_color);

        /* Target HP */
        float t_hp_percent = g_hud.target_max_hp > 0 ?
                             g_hud.target_hp / g_hud.target_max_hp : 0;
        draw_bar(renderer, target_x + 5, target_y + 25, 190, 16,
                 t_hp_percent, hp_fg, hp_bg, border);
    }

    /* Combo display */
    if (g_hud.combo_count > 1 && g_hud.combo_timer > 0) {
        float combo_alpha = g_hud.combo_timer / COMBO_DISPLAY_TIME;
        Color combo_color = {255, 200, 100, (uint8_t)(combo_alpha * 255)};

        char combo_text[32];
        snprintf(combo_text, sizeof(combo_text), "%d HITS!", g_hud.combo_count);
        renderer_draw_text(renderer, combo_text, WINDOW_WIDTH - 150, 150, combo_color);

        char damage_text[32];
        snprintf(damage_text, sizeof(damage_text), "%d DMG", g_hud.combo_damage);
        renderer_draw_text(renderer, damage_text, WINDOW_WIDTH - 150, 170, combo_color);
    }

    /* Notification */
    if (g_hud.notification_timer > 0 && g_hud.notification[0]) {
        float alpha = g_hud.notification_timer > 1.0f ? 1.0f : g_hud.notification_timer;
        Color notif_color = {255, 255, 100, (uint8_t)(alpha * 255)};
        int nx = WINDOW_WIDTH / 2 - strlen(g_hud.notification) * 4;
        renderer_draw_text(renderer, g_hud.notification, nx, 200, notif_color);
    }

    /* Debug info */
    if (g_hud.show_debug) {
        char debug[64];
        snprintf(debug, sizeof(debug), "FPS: %d  Ping: %.0fms", g_hud.fps, g_hud.ping);
        Color debug_color = {150, 150, 150, 255};
        renderer_draw_text(renderer, debug, 10, 10, debug_color);
    }
}

/*
 * ============================================================================
 * Settings
 * ============================================================================
 */

void ui_hud_toggle(void)
{
    g_hud.visible = !g_hud.visible;
}

void ui_hud_toggle_debug(void)
{
    g_hud.show_debug = !g_hud.show_debug;
}

void ui_hud_set_fps(int fps)
{
    g_hud.fps = fps;
}

void ui_hud_set_ping(float ping)
{
    g_hud.ping = ping;
}

void ui_hud_set_combat_state(bool in_combat)
{
    if (in_combat && !g_hud.in_combat) {
        g_hud.combat_timer = 0;
    }
    g_hud.in_combat = in_combat;
}

bool ui_hud_is_visible(void)
{
    return g_hud.visible;
}
