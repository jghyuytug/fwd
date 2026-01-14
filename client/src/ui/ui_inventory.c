/**
 * @file ui_inventory.c
 * @brief Inventory UI with item display and drag-drop
 */

#include "../../include/client.h"
#include "../../include/renderer.h"
#include "../../include/network.h"
#include "../../include/protocol_types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * ============================================================================
 * Constants
 * ============================================================================
 */

#define INVENTORY_COLS          8
#define INVENTORY_ROWS          6
#define INVENTORY_SIZE          (INVENTORY_COLS * INVENTORY_ROWS)
#define SLOT_SIZE               40
#define SLOT_PADDING            4
#define PANEL_PADDING           10
#define TOOLTIP_WIDTH           200
#define TOOLTIP_PADDING         8

#define EQUIP_SLOTS             12
#define EQUIP_WEAPON            0
#define EQUIP_ARMOR             1
#define EQUIP_PANTS             2
#define EQUIP_SHOULDER          3
#define EQUIP_BOOTS             4
#define EQUIP_BELT              5
#define EQUIP_NECKLACE          6
#define EQUIP_BRACELET          7
#define EQUIP_RING1             8
#define EQUIP_RING2             9
#define EQUIP_TITLE             10
#define EQUIP_AVATAR            11

/*
 * ============================================================================
 * Item Data
 * ============================================================================
 */

typedef struct {
    uint32_t        item_id;
    uint32_t        unique_id;
    uint16_t        quantity;
    uint8_t         slot;
    uint8_t         enhance_level;
    uint8_t         rarity;
    uint8_t         bind_type;
    int16_t         durability;
    int16_t         max_durability;
    char            name[64];
    char            description[256];
    uint32_t        icon_id;
    uint32_t        sell_price;
    bool            is_equipped;
} InventoryItem;

typedef struct {
    /* Panel state */
    bool            visible;
    float           x;
    float           y;
    float           width;
    float           height;

    /* Items */
    InventoryItem   items[INVENTORY_SIZE];
    int             item_count;

    /* Equipment */
    InventoryItem   equipment[EQUIP_SLOTS];

    /* Drag state */
    bool            dragging;
    int             drag_from_slot;
    bool            drag_from_equip;
    float           drag_offset_x;
    float           drag_offset_y;

    /* Selection */
    int             selected_slot;
    int             hover_slot;
    bool            hover_is_equip;

    /* Tooltip */
    bool            show_tooltip;
    float           tooltip_x;
    float           tooltip_y;
    InventoryItem*  tooltip_item;

    /* Currency */
    uint64_t        gold;

    /* Sort */
    bool            auto_sort;

    /* Tabs */
    int             current_tab;    /* 0=All, 1=Equip, 2=Use, 3=Etc */

    bool            initialized;
} InventoryUI;

static InventoryUI g_inv = {0};

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

static Color get_rarity_color(uint8_t rarity)
{
    switch (rarity) {
        case 0: return (Color){200, 200, 200, 255};  /* Common - Gray */
        case 1: return (Color){255, 255, 255, 255};  /* Uncommon - White */
        case 2: return (Color){100, 200, 255, 255};  /* Magic - Blue */
        case 3: return (Color){200, 100, 255, 255};  /* Rare - Purple */
        case 4: return (Color){255, 200, 100, 255};  /* Epic - Orange */
        case 5: return (Color){255, 100, 100, 255};  /* Legendary - Red */
        default: return (Color){255, 255, 255, 255};
    }
}

static int get_slot_at_position(float mx, float my)
{
    float start_x = g_inv.x + PANEL_PADDING;
    float start_y = g_inv.y + 50;  /* After header */

    float rel_x = mx - start_x;
    float rel_y = my - start_y;

    if (rel_x < 0 || rel_y < 0) return -1;

    int col = (int)(rel_x / (SLOT_SIZE + SLOT_PADDING));
    int row = (int)(rel_y / (SLOT_SIZE + SLOT_PADDING));

    if (col >= INVENTORY_COLS || row >= INVENTORY_ROWS) return -1;

    return row * INVENTORY_COLS + col;
}

static InventoryItem* get_item_at_slot(int slot)
{
    for (int i = 0; i < g_inv.item_count; i++) {
        if (g_inv.items[i].slot == slot && g_inv.items[i].item_id > 0) {
            return &g_inv.items[i];
        }
    }
    return NULL;
}

/*
 * ============================================================================
 * Initialization
 * ============================================================================
 */

void ui_inventory_init(void)
{
    memset(&g_inv, 0, sizeof(InventoryUI));

    g_inv.width = PANEL_PADDING * 2 +
                  INVENTORY_COLS * SLOT_SIZE +
                  (INVENTORY_COLS - 1) * SLOT_PADDING;
    g_inv.height = 50 + PANEL_PADDING * 2 +
                   INVENTORY_ROWS * SLOT_SIZE +
                   (INVENTORY_ROWS - 1) * SLOT_PADDING +
                   40;  /* Footer for gold */

    g_inv.x = WINDOW_WIDTH - g_inv.width - 20;
    g_inv.y = 100;
    g_inv.selected_slot = -1;
    g_inv.hover_slot = -1;

    g_inv.initialized = true;
}

void ui_inventory_shutdown(void)
{
    g_inv.initialized = false;
}

/*
 * ============================================================================
 * Visibility
 * ============================================================================
 */

void ui_inventory_show(void)
{
    g_inv.visible = true;
}

void ui_inventory_hide(void)
{
    g_inv.visible = false;
    g_inv.dragging = false;
}

void ui_inventory_toggle(void)
{
    if (g_inv.visible) {
        ui_inventory_hide();
    } else {
        ui_inventory_show();
    }
}

bool ui_inventory_is_visible(void)
{
    return g_inv.visible;
}

/*
 * ============================================================================
 * Item Management
 * ============================================================================
 */

void ui_inventory_set_items(const ItemData* items, int count)
{
    g_inv.item_count = 0;

    for (int i = 0; i < count && i < INVENTORY_SIZE; i++) {
        if (items[i].item_id == 0) continue;

        InventoryItem* inv_item = &g_inv.items[g_inv.item_count++];
        inv_item->item_id = items[i].item_id;
        inv_item->unique_id = items[i].unique_id;
        inv_item->quantity = items[i].quantity;
        inv_item->slot = items[i].slot;
        inv_item->enhance_level = items[i].enhance_level;
        inv_item->rarity = items[i].rarity;
        inv_item->durability = items[i].durability;
        inv_item->max_durability = items[i].max_durability;
        /* Name and description would come from item database */
        snprintf(inv_item->name, sizeof(inv_item->name), "Item #%u", items[i].item_id);
    }
}

void ui_inventory_add_item(const ItemData* item)
{
    if (g_inv.item_count >= INVENTORY_SIZE) return;

    InventoryItem* inv_item = &g_inv.items[g_inv.item_count++];
    inv_item->item_id = item->item_id;
    inv_item->unique_id = item->unique_id;
    inv_item->quantity = item->quantity;
    inv_item->slot = item->slot;
    inv_item->enhance_level = item->enhance_level;
    inv_item->rarity = item->rarity;
}

void ui_inventory_remove_item(uint32_t unique_id)
{
    for (int i = 0; i < g_inv.item_count; i++) {
        if (g_inv.items[i].unique_id == unique_id) {
            /* Shift remaining items */
            memmove(&g_inv.items[i], &g_inv.items[i + 1],
                    (g_inv.item_count - i - 1) * sizeof(InventoryItem));
            g_inv.item_count--;
            return;
        }
    }
}

void ui_inventory_update_item(uint32_t unique_id, uint16_t quantity)
{
    for (int i = 0; i < g_inv.item_count; i++) {
        if (g_inv.items[i].unique_id == unique_id) {
            g_inv.items[i].quantity = quantity;
            if (quantity == 0) {
                ui_inventory_remove_item(unique_id);
            }
            return;
        }
    }
}

void ui_inventory_set_gold(uint64_t gold)
{
    g_inv.gold = gold;
}

/*
 * ============================================================================
 * Equipment
 * ============================================================================
 */

void ui_inventory_set_equipment(int slot, const ItemData* item)
{
    if (slot < 0 || slot >= EQUIP_SLOTS) return;

    if (item && item->item_id > 0) {
        g_inv.equipment[slot].item_id = item->item_id;
        g_inv.equipment[slot].unique_id = item->unique_id;
        g_inv.equipment[slot].enhance_level = item->enhance_level;
        g_inv.equipment[slot].rarity = item->rarity;
        g_inv.equipment[slot].is_equipped = true;
    } else {
        memset(&g_inv.equipment[slot], 0, sizeof(InventoryItem));
    }
}

/*
 * ============================================================================
 * Input Handling
 * ============================================================================
 */

bool ui_inventory_handle_input(int mouse_x, int mouse_y, bool left_click,
                               bool right_click, bool left_release)
{
    if (!g_inv.visible) return false;

    /* Check if mouse is over panel */
    bool over_panel = mouse_x >= g_inv.x && mouse_x <= g_inv.x + g_inv.width &&
                      mouse_y >= g_inv.y && mouse_y <= g_inv.y + g_inv.height;

    if (!over_panel && !g_inv.dragging) {
        g_inv.hover_slot = -1;
        g_inv.show_tooltip = false;
        return false;
    }

    /* Get slot under cursor */
    int slot = get_slot_at_position((float)mouse_x, (float)mouse_y);
    g_inv.hover_slot = slot;

    /* Update tooltip */
    if (slot >= 0) {
        InventoryItem* item = get_item_at_slot(slot);
        if (item) {
            g_inv.show_tooltip = true;
            g_inv.tooltip_item = item;
            g_inv.tooltip_x = mouse_x + 15;
            g_inv.tooltip_y = mouse_y + 15;
        } else {
            g_inv.show_tooltip = false;
        }
    } else {
        g_inv.show_tooltip = false;
    }

    /* Start drag */
    if (left_click && slot >= 0 && !g_inv.dragging) {
        InventoryItem* item = get_item_at_slot(slot);
        if (item) {
            g_inv.dragging = true;
            g_inv.drag_from_slot = slot;
            g_inv.drag_from_equip = false;
            g_inv.drag_offset_x = mouse_x - (g_inv.x + PANEL_PADDING +
                (slot % INVENTORY_COLS) * (SLOT_SIZE + SLOT_PADDING));
            g_inv.drag_offset_y = mouse_y - (g_inv.y + 50 +
                (slot / INVENTORY_COLS) * (SLOT_SIZE + SLOT_PADDING));
        }
    }

    /* End drag */
    if (left_release && g_inv.dragging) {
        g_inv.dragging = false;

        if (slot >= 0 && slot != g_inv.drag_from_slot) {
            /* Move item */
            struct {
                uint8_t from_slot;
                uint8_t to_slot;
            } packet = { g_inv.drag_from_slot, slot };
            network_send_packet(MSG_MOVE_ITEM, &packet, sizeof(packet));
        } else if (!over_panel) {
            /* Drop item (outside inventory) */
            InventoryItem* item = get_item_at_slot(g_inv.drag_from_slot);
            if (item) {
                struct {
                    uint32_t unique_id;
                    uint8_t slot;
                } packet = { item->unique_id, g_inv.drag_from_slot };
                network_send_packet(MSG_DROP_ITEM, &packet, sizeof(packet));
            }
        }
    }

    /* Right click - use item */
    if (right_click && slot >= 0) {
        InventoryItem* item = get_item_at_slot(slot);
        if (item) {
            struct {
                uint32_t unique_id;
                uint8_t slot;
            } packet = { item->unique_id, slot };
            network_send_packet(MSG_USE_ITEM, &packet, sizeof(packet));
        }
    }

    /* Select slot */
    if (left_click && slot >= 0) {
        g_inv.selected_slot = slot;
    }

    return over_panel;
}

/*
 * ============================================================================
 * Rendering
 * ============================================================================
 */

void ui_inventory_render(Renderer* renderer)
{
    if (!g_inv.visible || !renderer) return;

    Color bg_color = {30, 30, 40, 230};
    Color border_color = {80, 80, 100, 255};
    Color slot_color = {50, 50, 60, 255};
    Color slot_hover = {70, 70, 90, 255};
    Color slot_selected = {100, 100, 150, 255};
    Color text_color = {255, 255, 255, 255};
    Color gold_color = {255, 215, 0, 255};

    /* Draw panel background */
    Rect panel = {g_inv.x, g_inv.y, g_inv.width, g_inv.height};
    renderer_draw_rect(renderer, panel, bg_color, true);
    renderer_draw_rect(renderer, panel, border_color, false);

    /* Draw title */
    renderer_draw_text(renderer, "Inventory", (int)g_inv.x + 10, (int)g_inv.y + 15, text_color);

    /* Draw tabs */
    const char* tabs[] = {"All", "Equip", "Use", "Etc"};
    float tab_x = g_inv.x + 80;
    for (int i = 0; i < 4; i++) {
        Color tab_color = (i == g_inv.current_tab) ? slot_selected : slot_color;
        Rect tab_rect = {tab_x, g_inv.y + 30, 40, 16};
        renderer_draw_rect(renderer, tab_rect, tab_color, true);
        renderer_draw_text(renderer, tabs[i], (int)tab_x + 5, (int)g_inv.y + 32, text_color);
        tab_x += 45;
    }

    /* Draw inventory slots */
    float start_x = g_inv.x + PANEL_PADDING;
    float start_y = g_inv.y + 50;

    for (int row = 0; row < INVENTORY_ROWS; row++) {
        for (int col = 0; col < INVENTORY_COLS; col++) {
            int slot = row * INVENTORY_COLS + col;
            float sx = start_x + col * (SLOT_SIZE + SLOT_PADDING);
            float sy = start_y + row * (SLOT_SIZE + SLOT_PADDING);

            /* Determine slot color */
            Color sc = slot_color;
            if (slot == g_inv.selected_slot) {
                sc = slot_selected;
            } else if (slot == g_inv.hover_slot) {
                sc = slot_hover;
            }

            /* Draw slot background */
            Rect slot_rect = {sx, sy, SLOT_SIZE, SLOT_SIZE};
            renderer_draw_rect(renderer, slot_rect, sc, true);
            renderer_draw_rect(renderer, slot_rect, border_color, false);

            /* Draw item if present */
            InventoryItem* item = get_item_at_slot(slot);
            if (item && !(g_inv.dragging && slot == g_inv.drag_from_slot)) {
                /* Draw item icon (placeholder - colored square based on rarity) */
                Color item_color = get_rarity_color(item->rarity);
                Rect item_rect = {sx + 4, sy + 4, SLOT_SIZE - 8, SLOT_SIZE - 8};
                renderer_draw_rect(renderer, item_rect, item_color, true);

                /* Draw enhancement level */
                if (item->enhance_level > 0) {
                    char enhance[8];
                    snprintf(enhance, sizeof(enhance), "+%d", item->enhance_level);
                    Color enhance_color = {255, 255, 100, 255};
                    renderer_draw_text(renderer, enhance, (int)sx + 2, (int)sy + 2, enhance_color);
                }

                /* Draw quantity */
                if (item->quantity > 1) {
                    char qty[16];
                    snprintf(qty, sizeof(qty), "%d", item->quantity);
                    renderer_draw_text(renderer, qty, (int)sx + SLOT_SIZE - 20,
                                       (int)sy + SLOT_SIZE - 14, text_color);
                }
            }
        }
    }

    /* Draw gold */
    float gold_y = start_y + INVENTORY_ROWS * (SLOT_SIZE + SLOT_PADDING) + 10;
    char gold_str[32];
    snprintf(gold_str, sizeof(gold_str), "Gold: %llu", (unsigned long long)g_inv.gold);
    renderer_draw_text(renderer, gold_str, (int)g_inv.x + 10, (int)gold_y, gold_color);

    /* Draw dragged item */
    if (g_inv.dragging) {
        InventoryItem* item = get_item_at_slot(g_inv.drag_from_slot);
        if (item) {
            int mx, my;
            input_get_mouse_position(&mx, &my);

            Color item_color = get_rarity_color(item->rarity);
            Rect drag_rect = {mx - g_inv.drag_offset_x, my - g_inv.drag_offset_y,
                              SLOT_SIZE, SLOT_SIZE};
            item_color.a = 180;
            renderer_draw_rect(renderer, drag_rect, item_color, true);
        }
    }

    /* Draw tooltip */
    if (g_inv.show_tooltip && g_inv.tooltip_item) {
        ui_inventory_render_tooltip(renderer, g_inv.tooltip_item,
                                    g_inv.tooltip_x, g_inv.tooltip_y);
    }
}

void ui_inventory_render_tooltip(Renderer* renderer, InventoryItem* item,
                                 float x, float y)
{
    if (!item) return;

    Color bg = {20, 20, 30, 240};
    Color border = {100, 100, 120, 255};
    Color name_color = get_rarity_color(item->rarity);
    Color text_color = {200, 200, 200, 255};
    Color stat_color = {150, 255, 150, 255};

    float height = 100;  /* Dynamic based on content */

    Rect tooltip = {x, y, TOOLTIP_WIDTH, height};
    renderer_draw_rect(renderer, tooltip, bg, true);
    renderer_draw_rect(renderer, tooltip, border, false);

    int ty = (int)y + TOOLTIP_PADDING;

    /* Item name with enhancement */
    char name_line[128];
    if (item->enhance_level > 0) {
        snprintf(name_line, sizeof(name_line), "+%d %s",
                 item->enhance_level, item->name);
    } else {
        snprintf(name_line, sizeof(name_line), "%s", item->name);
    }
    renderer_draw_text(renderer, name_line, (int)x + TOOLTIP_PADDING, ty, name_color);
    ty += 18;

    /* Rarity */
    const char* rarity_names[] = {"Common", "Uncommon", "Magic", "Rare", "Epic", "Legendary"};
    if (item->rarity < 6) {
        renderer_draw_text(renderer, rarity_names[item->rarity],
                           (int)x + TOOLTIP_PADDING, ty, text_color);
        ty += 16;
    }

    /* Durability */
    if (item->max_durability > 0) {
        char dur_str[32];
        snprintf(dur_str, sizeof(dur_str), "Durability: %d/%d",
                 item->durability, item->max_durability);
        renderer_draw_text(renderer, dur_str, (int)x + TOOLTIP_PADDING, ty, text_color);
        ty += 16;
    }

    /* Sell price */
    if (item->sell_price > 0) {
        char price_str[32];
        snprintf(price_str, sizeof(price_str), "Sell: %u gold", item->sell_price);
        Color gold = {255, 215, 0, 255};
        renderer_draw_text(renderer, price_str, (int)x + TOOLTIP_PADDING, ty, gold);
    }
}

/*
 * ============================================================================
 * Sorting
 * ============================================================================
 */

void ui_inventory_sort(void)
{
    /* Send sort request to server */
    network_send_packet(MSG_SORT_ITEM, NULL, 0);
}

void ui_inventory_set_tab(int tab)
{
    if (tab >= 0 && tab < 4) {
        g_inv.current_tab = tab;
    }
}
