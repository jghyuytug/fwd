/**
 * @file ui_character.c
 * @brief Character info UI showing stats and equipment
 */

#include "../../include/client.h"
#include "../../include/renderer.h"
#include "../../include/network.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * ============================================================================
 * Constants
 * ============================================================================
 */

#define EQUIP_SLOT_SIZE         44
#define EQUIP_SLOTS             12
#define PANEL_PADDING           12
#define STAT_LINE_HEIGHT        18
#define AVATAR_SIZE             120

/*
 * ============================================================================
 * Equipment Slot Layout
 * ============================================================================
 */

typedef struct {
    const char*     name;
    int             x_offset;
    int             y_offset;
} EquipSlotLayout;

static const EquipSlotLayout EQUIP_LAYOUT[EQUIP_SLOTS] = {
    {"Weapon",    -60, 60},      /* 0: Weapon - left of avatar */
    {"Armor",     60, 0},        /* 1: Armor - right top */
    {"Pants",     60, 50},       /* 2: Pants - right mid */
    {"Shoulder",  -60, 0},       /* 3: Shoulder - left top */
    {"Boots",     60, 100},      /* 4: Boots - right bottom */
    {"Belt",      0, 130},       /* 5: Belt - below avatar */
    {"Necklace",  -60, -50},     /* 6: Necklace - top left */
    {"Bracelet",  60, -50},      /* 7: Bracelet - top right */
    {"Ring 1",    -60, 110},     /* 8: Ring1 - bottom left */
    {"Ring 2",    60, 160},      /* 9: Ring2 - far bottom right */
    {"Title",     0, -60},       /* 10: Title - above avatar */
    {"Avatar",    0, 180},       /* 11: Avatar slot */
};

/*
 * ============================================================================
 * Character Stats
 * ============================================================================
 */

typedef struct {
    /* Basic info */
    char            name[32];
    char            guild_name[32];
    char            class_name[32];
    uint8_t         level;
    uint64_t        exp;
    uint64_t        exp_to_next;

    /* Primary stats */
    int32_t         strength;
    int32_t         intelligence;
    int32_t         vitality;
    int32_t         spirit;

    /* Combat stats */
    int32_t         physical_attack;
    int32_t         magical_attack;
    int32_t         physical_defense;
    int32_t         magical_defense;
    float           attack_speed;
    float           move_speed;
    float           cast_speed;

    /* Secondary stats */
    float           critical_rate;
    float           critical_damage;
    int32_t         accuracy;
    int32_t         evasion;
    int32_t         pierce;

    /* Resources */
    int32_t         hp;
    int32_t         max_hp;
    int32_t         mp;
    int32_t         max_mp;

    /* Elemental */
    int32_t         fire_attack;
    int32_t         ice_attack;
    int32_t         light_attack;
    int32_t         dark_attack;
    int32_t         fire_resist;
    int32_t         ice_resist;
    int32_t         light_resist;
    int32_t         dark_resist;

    /* Status */
    int32_t         fame;
    int32_t         pk_points;
} CharacterStats;

typedef struct {
    uint32_t        item_id;
    uint32_t        unique_id;
    uint8_t         enhance_level;
    uint8_t         rarity;
    char            name[48];
    bool            equipped;
} EquipmentSlot;

typedef struct {
    /* Panel state */
    bool            visible;
    float           x;
    float           y;
    float           width;
    float           height;

    /* Character data */
    CharacterStats  stats;
    EquipmentSlot   equipment[EQUIP_SLOTS];

    /* Selection */
    int             hover_slot;
    int             selected_slot;

    /* Tabs */
    int             current_tab;    /* 0=Stats, 1=Details, 2=Titles */

    /* Tooltip */
    bool            show_tooltip;
    float           tooltip_x;
    float           tooltip_y;
    int             tooltip_slot;

    bool            initialized;
} CharacterUI;

static CharacterUI g_char = {0};

/*
 * ============================================================================
 * External Functions
 * ============================================================================
 */

extern void renderer_draw_rect(Renderer* r, Rect rect, Color color, bool filled);
extern void renderer_draw_text(Renderer* r, const char* text, int x, int y, Color color);
extern int network_send_packet(uint16_t msg_no, const void* data, size_t len);

/*
 * ============================================================================
 * Helper Functions
 * ============================================================================
 */

static Color get_rarity_color(uint8_t rarity)
{
    switch (rarity) {
        case 0: return (Color){200, 200, 200, 255};
        case 1: return (Color){255, 255, 255, 255};
        case 2: return (Color){100, 200, 255, 255};
        case 3: return (Color){200, 100, 255, 255};
        case 4: return (Color){255, 200, 100, 255};
        case 5: return (Color){255, 100, 100, 255};
        default: return (Color){255, 255, 255, 255};
    }
}

static int get_equip_slot_at(float mx, float my)
{
    float avatar_x = g_char.x + g_char.width / 2 - AVATAR_SIZE / 2;
    float avatar_y = g_char.y + 80;

    for (int i = 0; i < EQUIP_SLOTS; i++) {
        float sx = avatar_x + AVATAR_SIZE / 2 + EQUIP_LAYOUT[i].x_offset - EQUIP_SLOT_SIZE / 2;
        float sy = avatar_y + AVATAR_SIZE / 2 + EQUIP_LAYOUT[i].y_offset - EQUIP_SLOT_SIZE / 2;

        if (mx >= sx && mx <= sx + EQUIP_SLOT_SIZE &&
            my >= sy && my <= sy + EQUIP_SLOT_SIZE) {
            return i;
        }
    }
    return -1;
}

/*
 * ============================================================================
 * Initialization
 * ============================================================================
 */

void ui_character_init(void)
{
    memset(&g_char, 0, sizeof(CharacterUI));

    g_char.width = 350;
    g_char.height = 500;
    g_char.x = 50;
    g_char.y = 80;
    g_char.hover_slot = -1;
    g_char.selected_slot = -1;

    g_char.initialized = true;
}

void ui_character_shutdown(void)
{
    g_char.initialized = false;
}

/*
 * ============================================================================
 * Visibility
 * ============================================================================
 */

void ui_character_show(void)
{
    g_char.visible = true;
}

void ui_character_hide(void)
{
    g_char.visible = false;
}

void ui_character_toggle(void)
{
    g_char.visible = !g_char.visible;
}

bool ui_character_is_visible(void)
{
    return g_char.visible;
}

/*
 * ============================================================================
 * Data Updates
 * ============================================================================
 */

void ui_character_set_basic_info(const char* name, const char* guild,
                                  const char* class_name, uint8_t level)
{
    strncpy(g_char.stats.name, name, sizeof(g_char.stats.name) - 1);
    strncpy(g_char.stats.guild_name, guild, sizeof(g_char.stats.guild_name) - 1);
    strncpy(g_char.stats.class_name, class_name, sizeof(g_char.stats.class_name) - 1);
    g_char.stats.level = level;
}

void ui_character_set_exp(uint64_t exp, uint64_t exp_to_next)
{
    g_char.stats.exp = exp;
    g_char.stats.exp_to_next = exp_to_next;
}

void ui_character_set_primary_stats(int32_t str, int32_t intel,
                                     int32_t vit, int32_t spr)
{
    g_char.stats.strength = str;
    g_char.stats.intelligence = intel;
    g_char.stats.vitality = vit;
    g_char.stats.spirit = spr;
}

void ui_character_set_combat_stats(int32_t phys_atk, int32_t mag_atk,
                                    int32_t phys_def, int32_t mag_def,
                                    float atk_spd, float move_spd, float cast_spd)
{
    g_char.stats.physical_attack = phys_atk;
    g_char.stats.magical_attack = mag_atk;
    g_char.stats.physical_defense = phys_def;
    g_char.stats.magical_defense = mag_def;
    g_char.stats.attack_speed = atk_spd;
    g_char.stats.move_speed = move_spd;
    g_char.stats.cast_speed = cast_spd;
}

void ui_character_set_secondary_stats(float crit_rate, float crit_dmg,
                                       int32_t accuracy, int32_t evasion, int32_t pierce)
{
    g_char.stats.critical_rate = crit_rate;
    g_char.stats.critical_damage = crit_dmg;
    g_char.stats.accuracy = accuracy;
    g_char.stats.evasion = evasion;
    g_char.stats.pierce = pierce;
}

void ui_character_set_resources(int32_t hp, int32_t max_hp,
                                 int32_t mp, int32_t max_mp)
{
    g_char.stats.hp = hp;
    g_char.stats.max_hp = max_hp;
    g_char.stats.mp = mp;
    g_char.stats.max_mp = max_mp;
}

void ui_character_set_equipment(int slot, uint32_t item_id, uint32_t unique_id,
                                 uint8_t enhance, uint8_t rarity, const char* name)
{
    if (slot < 0 || slot >= EQUIP_SLOTS) return;

    g_char.equipment[slot].item_id = item_id;
    g_char.equipment[slot].unique_id = unique_id;
    g_char.equipment[slot].enhance_level = enhance;
    g_char.equipment[slot].rarity = rarity;
    g_char.equipment[slot].equipped = item_id > 0;
    if (name) {
        strncpy(g_char.equipment[slot].name, name,
                sizeof(g_char.equipment[slot].name) - 1);
    }
}

/*
 * ============================================================================
 * Input Handling
 * ============================================================================
 */

bool ui_character_handle_input(int mouse_x, int mouse_y, bool left_click,
                                bool right_click)
{
    if (!g_char.visible) return false;

    bool over_panel = mouse_x >= g_char.x && mouse_x <= g_char.x + g_char.width &&
                      mouse_y >= g_char.y && mouse_y <= g_char.y + g_char.height;

    if (!over_panel) {
        g_char.hover_slot = -1;
        g_char.show_tooltip = false;
        return false;
    }

    /* Check tab clicks */
    if (left_click) {
        float tab_y = g_char.y + 35;
        if (mouse_y >= tab_y && mouse_y <= tab_y + 20) {
            float tab_x = g_char.x + PANEL_PADDING;
            int tab = (int)((mouse_x - tab_x) / 80);
            if (tab >= 0 && tab < 3) {
                g_char.current_tab = tab;
            }
        }
    }

    /* Check equipment slots */
    int slot = get_equip_slot_at((float)mouse_x, (float)mouse_y);
    g_char.hover_slot = slot;

    if (slot >= 0 && g_char.equipment[slot].equipped) {
        g_char.show_tooltip = true;
        g_char.tooltip_slot = slot;
        g_char.tooltip_x = mouse_x + 15;
        g_char.tooltip_y = mouse_y + 15;
    } else {
        g_char.show_tooltip = false;
    }

    /* Right click to unequip */
    if (right_click && slot >= 0 && g_char.equipment[slot].equipped) {
        struct {
            uint8_t slot;
            uint32_t unique_id;
        } packet = { slot, g_char.equipment[slot].unique_id };
        network_send_packet(MSG_UNEQUIP_ITEM, &packet, sizeof(packet));
    }

    return over_panel;
}

/*
 * ============================================================================
 * Rendering
 * ============================================================================
 */

void ui_character_render(Renderer* renderer)
{
    if (!g_char.visible || !renderer) return;

    Color bg = {30, 30, 40, 230};
    Color border = {80, 80, 100, 255};
    Color text = {255, 255, 255, 255};
    Color label = {180, 180, 180, 255};
    Color value = {100, 200, 255, 255};
    Color slot_bg = {50, 50, 60, 255};
    Color slot_hover = {70, 70, 90, 255};
    Color avatar_bg = {40, 40, 50, 255};

    /* Panel */
    Rect panel = {g_char.x, g_char.y, g_char.width, g_char.height};
    renderer_draw_rect(renderer, panel, bg, true);
    renderer_draw_rect(renderer, panel, border, false);

    /* Title */
    renderer_draw_text(renderer, "Character", (int)g_char.x + 10, (int)g_char.y + 12, text);

    /* Tabs */
    const char* tabs[] = {"Stats", "Details", "Titles"};
    float tab_x = g_char.x + PANEL_PADDING;
    float tab_y = g_char.y + 35;
    for (int i = 0; i < 3; i++) {
        Color tc = (i == g_char.current_tab) ? (Color){100, 100, 150, 255} : slot_bg;
        Rect tr = {tab_x, tab_y, 70, 20};
        renderer_draw_rect(renderer, tr, tc, true);
        renderer_draw_text(renderer, tabs[i], (int)tab_x + 8, (int)tab_y + 4, text);
        tab_x += 75;
    }

    /* Character name and level */
    char name_line[64];
    snprintf(name_line, sizeof(name_line), "Lv.%d %s", g_char.stats.level, g_char.stats.name);
    renderer_draw_text(renderer, name_line, (int)g_char.x + PANEL_PADDING, (int)g_char.y + 60, text);

    if (g_char.stats.guild_name[0]) {
        char guild_line[48];
        snprintf(guild_line, sizeof(guild_line), "<%s>", g_char.stats.guild_name);
        renderer_draw_text(renderer, guild_line, (int)g_char.x + PANEL_PADDING + 150,
                           (int)g_char.y + 60, label);
    }

    /* Avatar area */
    float avatar_x = g_char.x + g_char.width / 2 - AVATAR_SIZE / 2;
    float avatar_y = g_char.y + 80;
    Rect avatar = {avatar_x, avatar_y, AVATAR_SIZE, AVATAR_SIZE};
    renderer_draw_rect(renderer, avatar, avatar_bg, true);
    renderer_draw_rect(renderer, avatar, border, false);

    /* Equipment slots */
    for (int i = 0; i < EQUIP_SLOTS; i++) {
        float sx = avatar_x + AVATAR_SIZE / 2 + EQUIP_LAYOUT[i].x_offset - EQUIP_SLOT_SIZE / 2;
        float sy = avatar_y + AVATAR_SIZE / 2 + EQUIP_LAYOUT[i].y_offset - EQUIP_SLOT_SIZE / 2;

        Color sc = (i == g_char.hover_slot) ? slot_hover : slot_bg;
        Rect slot = {sx, sy, EQUIP_SLOT_SIZE, EQUIP_SLOT_SIZE};
        renderer_draw_rect(renderer, slot, sc, true);
        renderer_draw_rect(renderer, slot, border, false);

        /* Draw equipped item */
        if (g_char.equipment[i].equipped) {
            Color ic = get_rarity_color(g_char.equipment[i].rarity);
            Rect ir = {sx + 4, sy + 4, EQUIP_SLOT_SIZE - 8, EQUIP_SLOT_SIZE - 8};
            renderer_draw_rect(renderer, ir, ic, true);

            if (g_char.equipment[i].enhance_level > 0) {
                char enh[8];
                snprintf(enh, sizeof(enh), "+%d", g_char.equipment[i].enhance_level);
                Color ec = {255, 255, 100, 255};
                renderer_draw_text(renderer, enh, (int)sx + 2, (int)sy + 2, ec);
            }
        }
    }

    /* Stats section */
    float stats_y = avatar_y + AVATAR_SIZE + 70;
    int sy = (int)stats_y;

    if (g_char.current_tab == 0) {
        /* Primary stats */
        char stat_line[64];

        snprintf(stat_line, sizeof(stat_line), "HP: %d / %d",
                 g_char.stats.hp, g_char.stats.max_hp);
        renderer_draw_text(renderer, stat_line, (int)g_char.x + PANEL_PADDING, sy, text);
        sy += STAT_LINE_HEIGHT;

        snprintf(stat_line, sizeof(stat_line), "MP: %d / %d",
                 g_char.stats.mp, g_char.stats.max_mp);
        renderer_draw_text(renderer, stat_line, (int)g_char.x + PANEL_PADDING, sy, text);
        sy += STAT_LINE_HEIGHT + 8;

        snprintf(stat_line, sizeof(stat_line), "STR: %d    INT: %d",
                 g_char.stats.strength, g_char.stats.intelligence);
        renderer_draw_text(renderer, stat_line, (int)g_char.x + PANEL_PADDING, sy, value);
        sy += STAT_LINE_HEIGHT;

        snprintf(stat_line, sizeof(stat_line), "VIT: %d    SPR: %d",
                 g_char.stats.vitality, g_char.stats.spirit);
        renderer_draw_text(renderer, stat_line, (int)g_char.x + PANEL_PADDING, sy, value);
        sy += STAT_LINE_HEIGHT + 8;

        snprintf(stat_line, sizeof(stat_line), "Phys Atk: %d", g_char.stats.physical_attack);
        renderer_draw_text(renderer, stat_line, (int)g_char.x + PANEL_PADDING, sy, text);
        sy += STAT_LINE_HEIGHT;

        snprintf(stat_line, sizeof(stat_line), "Mag Atk: %d", g_char.stats.magical_attack);
        renderer_draw_text(renderer, stat_line, (int)g_char.x + PANEL_PADDING, sy, text);
        sy += STAT_LINE_HEIGHT;

        snprintf(stat_line, sizeof(stat_line), "Phys Def: %d", g_char.stats.physical_defense);
        renderer_draw_text(renderer, stat_line, (int)g_char.x + PANEL_PADDING, sy, text);
        sy += STAT_LINE_HEIGHT;

        snprintf(stat_line, sizeof(stat_line), "Mag Def: %d", g_char.stats.magical_defense);
        renderer_draw_text(renderer, stat_line, (int)g_char.x + PANEL_PADDING, sy, text);
    } else if (g_char.current_tab == 1) {
        /* Detailed stats */
        char stat_line[64];

        snprintf(stat_line, sizeof(stat_line), "Crit Rate: %.1f%%", g_char.stats.critical_rate);
        renderer_draw_text(renderer, stat_line, (int)g_char.x + PANEL_PADDING, sy, text);
        sy += STAT_LINE_HEIGHT;

        snprintf(stat_line, sizeof(stat_line), "Crit Dmg: %.1f%%", g_char.stats.critical_damage);
        renderer_draw_text(renderer, stat_line, (int)g_char.x + PANEL_PADDING, sy, text);
        sy += STAT_LINE_HEIGHT;

        snprintf(stat_line, sizeof(stat_line), "Atk Speed: %.1f%%", g_char.stats.attack_speed);
        renderer_draw_text(renderer, stat_line, (int)g_char.x + PANEL_PADDING, sy, text);
        sy += STAT_LINE_HEIGHT;

        snprintf(stat_line, sizeof(stat_line), "Move Speed: %.1f%%", g_char.stats.move_speed);
        renderer_draw_text(renderer, stat_line, (int)g_char.x + PANEL_PADDING, sy, text);
    }

    /* Tooltip */
    if (g_char.show_tooltip && g_char.tooltip_slot >= 0) {
        EquipmentSlot* eq = &g_char.equipment[g_char.tooltip_slot];
        Color tbg = {20, 20, 30, 240};
        Color tname = get_rarity_color(eq->rarity);

        Rect tt = {g_char.tooltip_x, g_char.tooltip_y, 180, 60};
        renderer_draw_rect(renderer, tt, tbg, true);
        renderer_draw_rect(renderer, tt, border, false);

        char line[64];
        if (eq->enhance_level > 0) {
            snprintf(line, sizeof(line), "+%d %s", eq->enhance_level, eq->name);
        } else {
            snprintf(line, sizeof(line), "%s", eq->name);
        }
        renderer_draw_text(renderer, line, (int)g_char.tooltip_x + 8,
                           (int)g_char.tooltip_y + 8, tname);
        renderer_draw_text(renderer, EQUIP_LAYOUT[g_char.tooltip_slot].name,
                           (int)g_char.tooltip_x + 8, (int)g_char.tooltip_y + 28, label);
    }
}
