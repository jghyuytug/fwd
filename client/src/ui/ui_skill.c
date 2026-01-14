/**
 * @file ui_skill.c
 * @brief Skill tree UI with skill upgrades and quickbar management
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

#define MAX_SKILLS              50
#define SKILL_ICON_SIZE         48
#define SKILL_SPACING           16
#define SKILLS_PER_ROW          5
#define QUICKBAR_SLOTS          6
#define QUICKBAR_SLOT_SIZE      44
#define PANEL_PADDING           12
#define TOOLTIP_WIDTH           240

/*
 * ============================================================================
 * Skill UI Types
 * ============================================================================
 */

typedef enum {
    SKILL_TREE_ACTIVE,
    SKILL_TREE_PASSIVE,
    SKILL_TREE_BUFF,
    SKILL_TREE_AWAKENING
} SkillTreeTab;

typedef struct {
    uint32_t        skill_id;
    char            name[48];
    char            description[256];
    uint8_t         level;
    uint8_t         max_level;
    uint8_t         required_level;
    uint32_t        sp_cost;
    uint32_t        mp_cost;
    float           cooldown;
    uint32_t        icon_id;
    uint32_t        prerequisite_id;
    uint8_t         prerequisite_level;
    bool            learned;
    bool            can_learn;
    SkillTreeTab    tree;
    int             row;
    int             col;
} SkillUIData;

typedef struct {
    /* Panel state */
    bool            visible;
    float           x;
    float           y;
    float           width;
    float           height;

    /* Skills */
    SkillUIData     skills[MAX_SKILLS];
    int             skill_count;

    /* Quickbar */
    uint32_t        quickbar[QUICKBAR_SLOTS];

    /* Selection */
    int             selected_skill;
    int             hover_skill;

    /* Current tab */
    SkillTreeTab    current_tab;

    /* SP */
    uint32_t        sp_available;
    uint32_t        sp_used;

    /* Tooltip */
    bool            show_tooltip;
    float           tooltip_x;
    float           tooltip_y;
    SkillUIData*    tooltip_skill;

    /* Drag to quickbar */
    bool            dragging;
    int             drag_skill_idx;
    float           drag_x;
    float           drag_y;

    bool            initialized;
} SkillUI;

static SkillUI g_skill = {0};

/*
 * ============================================================================
 * External Functions
 * ============================================================================
 */

extern int network_send_packet(uint16_t msg_no, const void* data, size_t len);
extern void renderer_draw_rect(Renderer* r, Rect rect, Color color, bool filled);
extern void renderer_draw_text(Renderer* r, const char* text, int x, int y, Color color);

/*
 * ============================================================================
 * Helper Functions
 * ============================================================================
 */

static SkillUIData* find_skill_by_id(uint32_t skill_id)
{
    for (int i = 0; i < g_skill.skill_count; i++) {
        if (g_skill.skills[i].skill_id == skill_id) {
            return &g_skill.skills[i];
        }
    }
    return NULL;
}

static int get_skill_at_position(float mx, float my)
{
    float content_x = g_skill.x + PANEL_PADDING;
    float content_y = g_skill.y + 80;  /* After header and tabs */

    int count = 0;
    for (int i = 0; i < g_skill.skill_count; i++) {
        if (g_skill.skills[i].tree != g_skill.current_tab) continue;

        int row = count / SKILLS_PER_ROW;
        int col = count % SKILLS_PER_ROW;

        float sx = content_x + col * (SKILL_ICON_SIZE + SKILL_SPACING);
        float sy = content_y + row * (SKILL_ICON_SIZE + SKILL_SPACING);

        if (mx >= sx && mx <= sx + SKILL_ICON_SIZE &&
            my >= sy && my <= sy + SKILL_ICON_SIZE) {
            return i;
        }

        count++;
    }
    return -1;
}

static int get_quickbar_slot_at_position(float mx, float my)
{
    float qb_x = g_skill.x + PANEL_PADDING;
    float qb_y = g_skill.y + g_skill.height - QUICKBAR_SLOT_SIZE - PANEL_PADDING - 30;

    for (int i = 0; i < QUICKBAR_SLOTS; i++) {
        float sx = qb_x + i * (QUICKBAR_SLOT_SIZE + 4);
        if (mx >= sx && mx <= sx + QUICKBAR_SLOT_SIZE &&
            my >= qb_y && my <= qb_y + QUICKBAR_SLOT_SIZE) {
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

void ui_skill_init(void)
{
    memset(&g_skill, 0, sizeof(SkillUI));

    g_skill.width = PANEL_PADDING * 2 +
                    SKILLS_PER_ROW * SKILL_ICON_SIZE +
                    (SKILLS_PER_ROW - 1) * SKILL_SPACING;
    g_skill.height = 400;

    g_skill.x = 50;
    g_skill.y = 100;
    g_skill.selected_skill = -1;
    g_skill.hover_skill = -1;

    g_skill.initialized = true;
}

void ui_skill_shutdown(void)
{
    g_skill.initialized = false;
}

/*
 * ============================================================================
 * Visibility
 * ============================================================================
 */

void ui_skill_show(void)
{
    g_skill.visible = true;
}

void ui_skill_hide(void)
{
    g_skill.visible = false;
    g_skill.dragging = false;
}

void ui_skill_toggle(void)
{
    g_skill.visible = !g_skill.visible;
    if (!g_skill.visible) {
        g_skill.dragging = false;
    }
}

bool ui_skill_is_visible(void)
{
    return g_skill.visible;
}

/*
 * ============================================================================
 * Skill Management
 * ============================================================================
 */

void ui_skill_add(uint32_t skill_id, const char* name, const char* description,
                  uint8_t level, uint8_t max_level, uint8_t required_level,
                  uint32_t sp_cost, uint32_t mp_cost, float cooldown,
                  uint32_t prerequisite_id, uint8_t prerequisite_level,
                  SkillTreeTab tree)
{
    if (g_skill.skill_count >= MAX_SKILLS) return;

    SkillUIData* skill = &g_skill.skills[g_skill.skill_count++];
    skill->skill_id = skill_id;
    strncpy(skill->name, name, sizeof(skill->name) - 1);
    strncpy(skill->description, description, sizeof(skill->description) - 1);
    skill->level = level;
    skill->max_level = max_level;
    skill->required_level = required_level;
    skill->sp_cost = sp_cost;
    skill->mp_cost = mp_cost;
    skill->cooldown = cooldown;
    skill->prerequisite_id = prerequisite_id;
    skill->prerequisite_level = prerequisite_level;
    skill->tree = tree;
    skill->learned = level > 0;

    /* Check if can learn */
    skill->can_learn = !skill->learned && g_skill.sp_available >= sp_cost;
    if (prerequisite_id > 0) {
        SkillUIData* prereq = find_skill_by_id(prerequisite_id);
        if (!prereq || prereq->level < prerequisite_level) {
            skill->can_learn = false;
        }
    }
}

void ui_skill_update_level(uint32_t skill_id, uint8_t level)
{
    SkillUIData* skill = find_skill_by_id(skill_id);
    if (skill) {
        skill->level = level;
        skill->learned = level > 0;
    }
}

void ui_skill_set_sp(uint32_t available, uint32_t used)
{
    g_skill.sp_available = available;
    g_skill.sp_used = used;

    /* Update can_learn for all skills */
    for (int i = 0; i < g_skill.skill_count; i++) {
        SkillUIData* skill = &g_skill.skills[i];
        skill->can_learn = !skill->learned && available >= skill->sp_cost;

        if (skill->prerequisite_id > 0) {
            SkillUIData* prereq = find_skill_by_id(skill->prerequisite_id);
            if (!prereq || prereq->level < skill->prerequisite_level) {
                skill->can_learn = false;
            }
        }
    }
}

void ui_skill_set_quickbar(int slot, uint32_t skill_id)
{
    if (slot >= 0 && slot < QUICKBAR_SLOTS) {
        g_skill.quickbar[slot] = skill_id;
    }
}

void ui_skill_clear(void)
{
    g_skill.skill_count = 0;
    memset(g_skill.quickbar, 0, sizeof(g_skill.quickbar));
}

/*
 * ============================================================================
 * Input Handling
 * ============================================================================
 */

bool ui_skill_handle_input(int mouse_x, int mouse_y, bool left_click,
                           bool right_click, bool left_release)
{
    if (!g_skill.visible) return false;

    bool over_panel = mouse_x >= g_skill.x && mouse_x <= g_skill.x + g_skill.width &&
                      mouse_y >= g_skill.y && mouse_y <= g_skill.y + g_skill.height;

    if (!over_panel && !g_skill.dragging) {
        g_skill.hover_skill = -1;
        g_skill.show_tooltip = false;
        return false;
    }

    /* Check tab clicks */
    if (left_click) {
        float tab_y = g_skill.y + 45;
        if (mouse_y >= tab_y && mouse_y <= tab_y + 20) {
            float tab_x = g_skill.x + PANEL_PADDING;
            int tab = (int)((mouse_x - tab_x) / 70);
            if (tab >= 0 && tab < 4) {
                g_skill.current_tab = (SkillTreeTab)tab;
            }
        }
    }

    /* Get skill under cursor */
    int skill_idx = get_skill_at_position((float)mouse_x, (float)mouse_y);
    g_skill.hover_skill = skill_idx;

    /* Update tooltip */
    if (skill_idx >= 0) {
        g_skill.show_tooltip = true;
        g_skill.tooltip_skill = &g_skill.skills[skill_idx];
        g_skill.tooltip_x = mouse_x + 15;
        g_skill.tooltip_y = mouse_y + 15;
    } else {
        g_skill.show_tooltip = false;
    }

    /* Start drag */
    if (left_click && skill_idx >= 0 && !g_skill.dragging) {
        SkillUIData* skill = &g_skill.skills[skill_idx];
        if (skill->learned) {
            g_skill.dragging = true;
            g_skill.drag_skill_idx = skill_idx;
        }
    }

    /* End drag */
    if (left_release && g_skill.dragging) {
        g_skill.dragging = false;

        /* Check if dropped on quickbar */
        int qb_slot = get_quickbar_slot_at_position((float)mouse_x, (float)mouse_y);
        if (qb_slot >= 0) {
            uint32_t skill_id = g_skill.skills[g_skill.drag_skill_idx].skill_id;
            g_skill.quickbar[qb_slot] = skill_id;

            /* Send to server */
            struct {
                uint8_t slot;
                uint32_t skill_id;
            } packet = { qb_slot, skill_id };
            network_send_packet(MSG_SET_QUICKBAR_SKILL, &packet, sizeof(packet));
        }
    }

    /* Right click - learn skill */
    if (right_click && skill_idx >= 0) {
        SkillUIData* skill = &g_skill.skills[skill_idx];
        if (skill->can_learn || (skill->learned && skill->level < skill->max_level)) {
            struct {
                uint32_t skill_id;
            } packet = { skill->skill_id };
            network_send_packet(MSG_BUY_SKILL, &packet, sizeof(packet));
        }
    }

    /* Left click - select */
    if (left_click && skill_idx >= 0 && !g_skill.dragging) {
        g_skill.selected_skill = skill_idx;
    }

    return over_panel;
}

/*
 * ============================================================================
 * Rendering
 * ============================================================================
 */

void ui_skill_render(Renderer* renderer)
{
    if (!g_skill.visible || !renderer) return;

    Color bg_color = {30, 30, 40, 230};
    Color border_color = {80, 80, 100, 255};
    Color text_color = {255, 255, 255, 255};
    Color sp_color = {100, 200, 255, 255};
    Color locked_color = {80, 80, 80, 200};
    Color learned_color = {80, 150, 80, 255};
    Color can_learn_color = {150, 150, 80, 255};

    /* Draw panel */
    Rect panel = {g_skill.x, g_skill.y, g_skill.width, g_skill.height};
    renderer_draw_rect(renderer, panel, bg_color, true);
    renderer_draw_rect(renderer, panel, border_color, false);

    /* Title */
    renderer_draw_text(renderer, "Skills", (int)g_skill.x + 10, (int)g_skill.y + 15, text_color);

    /* SP display */
    char sp_str[32];
    snprintf(sp_str, sizeof(sp_str), "SP: %u", g_skill.sp_available);
    renderer_draw_text(renderer, sp_str, (int)g_skill.x + g_skill.width - 80,
                       (int)g_skill.y + 15, sp_color);

    /* Tabs */
    const char* tabs[] = {"Active", "Passive", "Buff", "Awaken"};
    float tab_x = g_skill.x + PANEL_PADDING;
    float tab_y = g_skill.y + 45;

    for (int i = 0; i < 4; i++) {
        Color tab_color = (i == g_skill.current_tab) ?
                          (Color){100, 100, 150, 255} : (Color){50, 50, 60, 255};
        Rect tab_rect = {tab_x, tab_y, 60, 20};
        renderer_draw_rect(renderer, tab_rect, tab_color, true);
        renderer_draw_text(renderer, tabs[i], (int)tab_x + 8, (int)tab_y + 4, text_color);
        tab_x += 65;
    }

    /* Skill icons */
    float content_x = g_skill.x + PANEL_PADDING;
    float content_y = g_skill.y + 80;
    int count = 0;

    for (int i = 0; i < g_skill.skill_count; i++) {
        SkillUIData* skill = &g_skill.skills[i];
        if (skill->tree != g_skill.current_tab) continue;

        int row = count / SKILLS_PER_ROW;
        int col = count % SKILLS_PER_ROW;
        float sx = content_x + col * (SKILL_ICON_SIZE + SKILL_SPACING);
        float sy = content_y + row * (SKILL_ICON_SIZE + SKILL_SPACING);

        /* Determine background color */
        Color slot_bg;
        if (skill->learned) {
            slot_bg = learned_color;
        } else if (skill->can_learn) {
            slot_bg = can_learn_color;
        } else {
            slot_bg = locked_color;
        }

        /* Highlight if selected or hovered */
        if (i == g_skill.selected_skill) {
            slot_bg.r = (uint8_t)(slot_bg.r * 1.3f > 255 ? 255 : slot_bg.r * 1.3f);
            slot_bg.g = (uint8_t)(slot_bg.g * 1.3f > 255 ? 255 : slot_bg.g * 1.3f);
            slot_bg.b = (uint8_t)(slot_bg.b * 1.3f > 255 ? 255 : slot_bg.b * 1.3f);
        } else if (i == g_skill.hover_skill) {
            slot_bg.r = (uint8_t)(slot_bg.r * 1.15f > 255 ? 255 : slot_bg.r * 1.15f);
            slot_bg.g = (uint8_t)(slot_bg.g * 1.15f > 255 ? 255 : slot_bg.g * 1.15f);
            slot_bg.b = (uint8_t)(slot_bg.b * 1.15f > 255 ? 255 : slot_bg.b * 1.15f);
        }

        /* Draw slot */
        Rect slot_rect = {sx, sy, SKILL_ICON_SIZE, SKILL_ICON_SIZE};
        renderer_draw_rect(renderer, slot_rect, slot_bg, true);
        renderer_draw_rect(renderer, slot_rect, border_color, false);

        /* Draw skill level */
        if (skill->level > 0) {
            char lvl[8];
            snprintf(lvl, sizeof(lvl), "%d/%d", skill->level, skill->max_level);
            renderer_draw_text(renderer, lvl, (int)sx + 2, (int)sy + SKILL_ICON_SIZE - 14, text_color);
        }

        count++;
    }

    /* Quickbar */
    float qb_x = g_skill.x + PANEL_PADDING;
    float qb_y = g_skill.y + g_skill.height - QUICKBAR_SLOT_SIZE - PANEL_PADDING - 30;

    renderer_draw_text(renderer, "Quickbar:", (int)qb_x, (int)qb_y - 18, text_color);

    for (int i = 0; i < QUICKBAR_SLOTS; i++) {
        float sx = qb_x + i * (QUICKBAR_SLOT_SIZE + 4);
        Rect slot = {sx, qb_y, QUICKBAR_SLOT_SIZE, QUICKBAR_SLOT_SIZE};

        Color slot_color = {50, 50, 60, 255};
        renderer_draw_rect(renderer, slot, slot_color, true);
        renderer_draw_rect(renderer, slot, border_color, false);

        /* Draw skill if assigned */
        if (g_skill.quickbar[i] > 0) {
            SkillUIData* skill = find_skill_by_id(g_skill.quickbar[i]);
            if (skill && skill->learned) {
                Color icon_color = {100, 150, 200, 255};
                Rect icon = {sx + 4, qb_y + 4, QUICKBAR_SLOT_SIZE - 8, QUICKBAR_SLOT_SIZE - 8};
                renderer_draw_rect(renderer, icon, icon_color, true);
            }
        }

        /* Slot number */
        char num[4];
        snprintf(num, sizeof(num), "%d", i + 1);
        renderer_draw_text(renderer, num, (int)sx + 2, (int)qb_y + 2, text_color);
    }

    /* Draw dragged skill */
    if (g_skill.dragging && g_skill.drag_skill_idx >= 0) {
        int mx, my;
        input_get_mouse_position(&mx, &my);

        Color drag_color = {100, 150, 200, 180};
        Rect drag = {mx - SKILL_ICON_SIZE / 2, my - SKILL_ICON_SIZE / 2,
                     SKILL_ICON_SIZE, SKILL_ICON_SIZE};
        renderer_draw_rect(renderer, drag, drag_color, true);
    }

    /* Tooltip */
    if (g_skill.show_tooltip && g_skill.tooltip_skill) {
        ui_skill_render_tooltip(renderer, g_skill.tooltip_skill,
                                g_skill.tooltip_x, g_skill.tooltip_y);
    }
}

void ui_skill_render_tooltip(Renderer* renderer, SkillUIData* skill,
                             float x, float y)
{
    Color bg = {20, 20, 30, 240};
    Color border = {100, 100, 120, 255};
    Color name_color = {255, 255, 100, 255};
    Color text_color = {200, 200, 200, 255};
    Color mp_color = {100, 150, 255, 255};
    Color sp_color = {150, 255, 150, 255};
    Color cooldown_color = {255, 200, 100, 255};

    float height = 140;

    Rect tooltip = {x, y, TOOLTIP_WIDTH, height};
    renderer_draw_rect(renderer, tooltip, bg, true);
    renderer_draw_rect(renderer, tooltip, border, false);

    int ty = (int)y + 8;

    /* Name */
    renderer_draw_text(renderer, skill->name, (int)x + 8, ty, name_color);
    ty += 18;

    /* Level */
    char lvl[32];
    snprintf(lvl, sizeof(lvl), "Level: %d / %d", skill->level, skill->max_level);
    renderer_draw_text(renderer, lvl, (int)x + 8, ty, text_color);
    ty += 16;

    /* MP Cost */
    char mp[32];
    snprintf(mp, sizeof(mp), "MP Cost: %u", skill->mp_cost);
    renderer_draw_text(renderer, mp, (int)x + 8, ty, mp_color);
    ty += 16;

    /* Cooldown */
    char cd[32];
    snprintf(cd, sizeof(cd), "Cooldown: %.1fs", skill->cooldown);
    renderer_draw_text(renderer, cd, (int)x + 8, ty, cooldown_color);
    ty += 16;

    /* SP Cost to learn/upgrade */
    if (!skill->learned || skill->level < skill->max_level) {
        char sp[32];
        snprintf(sp, sizeof(sp), "SP to upgrade: %u", skill->sp_cost);
        renderer_draw_text(renderer, sp, (int)x + 8, ty, sp_color);
        ty += 16;
    }

    /* Description */
    renderer_draw_text(renderer, skill->description, (int)x + 8, ty, text_color);
}

/*
 * ============================================================================
 * Skill Reset
 * ============================================================================
 */

void ui_skill_reset_all(void)
{
    network_send_packet(MSG_SKILL_RESET, NULL, 0);
}
