/**
 * @file interaction.c
 * @brief Interaction system for NPCs, items, and portals
 */

#include "../../include/client.h"
#include "../../include/network.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/*
 * ============================================================================
 * Constants
 * ============================================================================
 */

#define MAX_INTERACTABLES       64
#define INTERACTION_RANGE       80.0f
#define ITEM_PICKUP_RANGE       60.0f
#define PORTAL_RANGE            50.0f
#define HIGHLIGHT_PULSE_SPEED   3.0f
#define AUTO_PICKUP_DELAY       0.5f

/*
 * ============================================================================
 * Interaction Types
 * ============================================================================
 */

typedef enum {
    INTERACT_NONE,
    INTERACT_NPC,
    INTERACT_ITEM,
    INTERACT_PORTAL,
    INTERACT_OBJECT,
    INTERACT_PLAYER
} InteractableType;

typedef enum {
    NPC_TYPE_NORMAL,
    NPC_TYPE_SHOP,
    NPC_TYPE_BLACKSMITH,
    NPC_TYPE_STORAGE,
    NPC_TYPE_QUEST,
    NPC_TYPE_TELEPORT,
    NPC_TYPE_GUILD,
    NPC_TYPE_AUCTION,
    NPC_TYPE_SKILL_TRAINER
} NPCType;

typedef enum {
    PORTAL_TYPE_DUNGEON,
    PORTAL_TYPE_TOWN,
    PORTAL_TYPE_ZONE,
    PORTAL_TYPE_INSTANCE
} PortalType;

typedef enum {
    OBJECT_TYPE_CHEST,
    OBJECT_TYPE_SWITCH,
    OBJECT_TYPE_DOOR,
    OBJECT_TYPE_DESTRUCTIBLE
} ObjectType;

/*
 * ============================================================================
 * Data Structures
 * ============================================================================
 */

typedef struct {
    uint32_t            id;
    InteractableType    type;

    /* Position */
    float               x;
    float               y;
    float               width;
    float               height;

    /* State */
    bool                active;
    bool                highlighted;
    bool                in_range;
    float               distance;

    /* Type-specific data */
    union {
        struct {
            NPCType     npc_type;
            char        name[64];
            uint32_t    quest_id;       /* Active quest icon */
            bool        has_quest;
        } npc;

        struct {
            uint32_t    item_id;
            uint32_t    quantity;
            uint8_t     rarity;
            float       spawn_time;
            float       expire_time;
            uint32_t    owner_id;       /* Who can pick up (0 = anyone) */
        } item;

        struct {
            PortalType  portal_type;
            uint32_t    target_map;
            char        target_name[64];
            bool        requires_key;
            uint32_t    key_item_id;
        } portal;

        struct {
            ObjectType  object_type;
            bool        is_open;
            uint32_t    loot_table;
        } object;
    } data;
} Interactable;

typedef struct {
    /* Interactables */
    Interactable        list[MAX_INTERACTABLES];
    int                 count;

    /* Current target */
    Interactable*       current_target;
    float               target_timer;

    /* Player position reference */
    float               player_x;
    float               player_y;

    /* Auto-pickup */
    bool                auto_pickup_enabled;
    float               auto_pickup_timer;
    uint8_t             auto_pickup_rarity;     /* Min rarity to auto-pickup */

    /* Highlight animation */
    float               highlight_phase;

    /* Dialog state */
    bool                in_dialog;
    uint32_t            dialog_npc_id;
    int                 dialog_option;

    bool                initialized;
} InteractionSystem;

static InteractionSystem g_interact = {0};

/*
 * ============================================================================
 * External Functions
 * ============================================================================
 */

extern void player_get_position(float* x, float* y);
extern int network_send_packet(uint16_t msg_no, const void* data, size_t len);

/*
 * ============================================================================
 * Helper Functions
 * ============================================================================
 */

static float get_distance(float x1, float y1, float x2, float y2)
{
    float dx = x2 - x1;
    float dy = y2 - y1;
    return sqrtf(dx * dx + dy * dy);
}

static Interactable* find_by_id(uint32_t id)
{
    for (int i = 0; i < g_interact.count; i++) {
        if (g_interact.list[i].id == id && g_interact.list[i].active) {
            return &g_interact.list[i];
        }
    }
    return NULL;
}

static float get_interaction_range(InteractableType type)
{
    switch (type) {
        case INTERACT_NPC:    return INTERACTION_RANGE;
        case INTERACT_ITEM:   return ITEM_PICKUP_RANGE;
        case INTERACT_PORTAL: return PORTAL_RANGE;
        case INTERACT_OBJECT: return INTERACTION_RANGE;
        default:              return INTERACTION_RANGE;
    }
}

static int compare_by_distance(const void* a, const void* b)
{
    const Interactable* ia = *(const Interactable**)a;
    const Interactable* ib = *(const Interactable**)b;
    if (ia->distance < ib->distance) return -1;
    if (ia->distance > ib->distance) return 1;
    return 0;
}

/*
 * ============================================================================
 * Initialization
 * ============================================================================
 */

void interaction_init(void)
{
    memset(&g_interact, 0, sizeof(InteractionSystem));
    g_interact.auto_pickup_enabled = true;
    g_interact.auto_pickup_rarity = 0;  /* Pick up everything */
    g_interact.initialized = true;
}

void interaction_shutdown(void)
{
    g_interact.initialized = false;
}

/*
 * ============================================================================
 * Interactable Management
 * ============================================================================
 */

void interaction_add_npc(uint32_t id, float x, float y, NPCType type, const char* name)
{
    if (g_interact.count >= MAX_INTERACTABLES) return;

    Interactable* obj = &g_interact.list[g_interact.count++];
    memset(obj, 0, sizeof(Interactable));

    obj->id = id;
    obj->type = INTERACT_NPC;
    obj->x = x;
    obj->y = y;
    obj->width = 48;
    obj->height = 80;
    obj->active = true;

    obj->data.npc.npc_type = type;
    strncpy(obj->data.npc.name, name, sizeof(obj->data.npc.name) - 1);
}

void interaction_add_item(uint32_t id, float x, float y, uint32_t item_id,
                          uint32_t quantity, uint8_t rarity, uint32_t owner_id)
{
    if (g_interact.count >= MAX_INTERACTABLES) return;

    Interactable* obj = &g_interact.list[g_interact.count++];
    memset(obj, 0, sizeof(Interactable));

    obj->id = id;
    obj->type = INTERACT_ITEM;
    obj->x = x;
    obj->y = y;
    obj->width = 32;
    obj->height = 32;
    obj->active = true;

    obj->data.item.item_id = item_id;
    obj->data.item.quantity = quantity;
    obj->data.item.rarity = rarity;
    obj->data.item.spawn_time = 0;
    obj->data.item.expire_time = 60.0f;  /* 60 seconds */
    obj->data.item.owner_id = owner_id;
}

void interaction_add_portal(uint32_t id, float x, float y, PortalType type,
                            uint32_t target_map, const char* target_name)
{
    if (g_interact.count >= MAX_INTERACTABLES) return;

    Interactable* obj = &g_interact.list[g_interact.count++];
    memset(obj, 0, sizeof(Interactable));

    obj->id = id;
    obj->type = INTERACT_PORTAL;
    obj->x = x;
    obj->y = y;
    obj->width = 64;
    obj->height = 96;
    obj->active = true;

    obj->data.portal.portal_type = type;
    obj->data.portal.target_map = target_map;
    strncpy(obj->data.portal.target_name, target_name,
            sizeof(obj->data.portal.target_name) - 1);
}

void interaction_add_object(uint32_t id, float x, float y, ObjectType type)
{
    if (g_interact.count >= MAX_INTERACTABLES) return;

    Interactable* obj = &g_interact.list[g_interact.count++];
    memset(obj, 0, sizeof(Interactable));

    obj->id = id;
    obj->type = INTERACT_OBJECT;
    obj->x = x;
    obj->y = y;
    obj->width = 48;
    obj->height = 48;
    obj->active = true;

    obj->data.object.object_type = type;
    obj->data.object.is_open = false;
}

void interaction_remove(uint32_t id)
{
    for (int i = 0; i < g_interact.count; i++) {
        if (g_interact.list[i].id == id) {
            /* Swap with last and decrement count */
            g_interact.list[i] = g_interact.list[g_interact.count - 1];
            g_interact.count--;

            /* Clear target if it was removed */
            if (g_interact.current_target &&
                g_interact.current_target->id == id) {
                g_interact.current_target = NULL;
            }
            return;
        }
    }
}

void interaction_clear_all(void)
{
    g_interact.count = 0;
    g_interact.current_target = NULL;
}

/*
 * ============================================================================
 * Update
 * ============================================================================
 */

void interaction_update(float delta)
{
    if (!g_interact.initialized) return;

    /* Get player position */
    player_get_position(&g_interact.player_x, &g_interact.player_y);

    /* Update highlight animation */
    g_interact.highlight_phase += HIGHLIGHT_PULSE_SPEED * delta;
    if (g_interact.highlight_phase > 6.28318f) {
        g_interact.highlight_phase -= 6.28318f;
    }

    /* Update distances and in-range status */
    Interactable* nearest = NULL;
    float nearest_dist = 9999999.0f;

    for (int i = 0; i < g_interact.count; i++) {
        Interactable* obj = &g_interact.list[i];
        if (!obj->active) continue;

        /* Calculate distance */
        obj->distance = get_distance(g_interact.player_x, g_interact.player_y,
                                     obj->x, obj->y);

        /* Check range */
        float range = get_interaction_range(obj->type);
        obj->in_range = obj->distance <= range;
        obj->highlighted = false;

        /* Find nearest in-range interactable */
        if (obj->in_range && obj->distance < nearest_dist) {
            nearest_dist = obj->distance;
            nearest = obj;
        }

        /* Update item timers */
        if (obj->type == INTERACT_ITEM) {
            obj->data.item.spawn_time += delta;
            if (obj->data.item.spawn_time >= obj->data.item.expire_time) {
                interaction_remove(obj->id);
                i--;  /* Adjust index after removal */
            }
        }
    }

    /* Update current target */
    if (nearest) {
        nearest->highlighted = true;
        g_interact.current_target = nearest;
    } else {
        g_interact.current_target = NULL;
    }

    /* Auto-pickup items */
    if (g_interact.auto_pickup_enabled) {
        g_interact.auto_pickup_timer += delta;
        if (g_interact.auto_pickup_timer >= AUTO_PICKUP_DELAY) {
            g_interact.auto_pickup_timer = 0;

            for (int i = 0; i < g_interact.count; i++) {
                Interactable* obj = &g_interact.list[i];
                if (!obj->active || obj->type != INTERACT_ITEM) continue;
                if (!obj->in_range) continue;

                /* Check rarity filter */
                if (obj->data.item.rarity >= g_interact.auto_pickup_rarity) {
                    interaction_pickup_item(obj->id);
                    break;  /* One item per frame */
                }
            }
        }
    }

    /* Handle interaction input */
    if (input_action_just_pressed(ACTION_INTERACT)) {
        if (g_interact.current_target) {
            interaction_interact_with(g_interact.current_target->id);
        }
    }

    /* Handle pickup input (separate from interact) */
    if (input_action_just_pressed(ACTION_PICKUP)) {
        /* Find nearest item specifically */
        for (int i = 0; i < g_interact.count; i++) {
            Interactable* obj = &g_interact.list[i];
            if (obj->active && obj->type == INTERACT_ITEM && obj->in_range) {
                interaction_pickup_item(obj->id);
                break;
            }
        }
    }
}

/*
 * ============================================================================
 * Interaction Actions
 * ============================================================================
 */

bool interaction_interact_with(uint32_t id)
{
    Interactable* obj = find_by_id(id);
    if (!obj || !obj->in_range) return false;

    switch (obj->type) {
        case INTERACT_NPC:
            return interaction_talk_to_npc(id);

        case INTERACT_ITEM:
            return interaction_pickup_item(id);

        case INTERACT_PORTAL:
            return interaction_use_portal(id);

        case INTERACT_OBJECT:
            return interaction_use_object(id);

        default:
            return false;
    }
}

bool interaction_talk_to_npc(uint32_t npc_id)
{
    Interactable* obj = find_by_id(npc_id);
    if (!obj || obj->type != INTERACT_NPC) return false;

    /* Open dialog based on NPC type */
    switch (obj->data.npc.npc_type) {
        case NPC_TYPE_SHOP:
            printf("[Interact] Opening shop: %s\n", obj->data.npc.name);
            /* Send shop request to server */
            {
                struct { uint32_t npc_id; } packet = { npc_id };
                network_send_packet(MSG_NPC_SHOP_OPEN, &packet, sizeof(packet));
            }
            break;

        case NPC_TYPE_BLACKSMITH:
            printf("[Interact] Opening blacksmith: %s\n", obj->data.npc.name);
            {
                struct { uint32_t npc_id; } packet = { npc_id };
                network_send_packet(MSG_NPC_BLACKSMITH_OPEN, &packet, sizeof(packet));
            }
            break;

        case NPC_TYPE_STORAGE:
            printf("[Interact] Opening storage: %s\n", obj->data.npc.name);
            {
                struct { uint32_t npc_id; } packet = { npc_id };
                network_send_packet(MSG_STORAGE_OPEN, &packet, sizeof(packet));
            }
            break;

        case NPC_TYPE_QUEST:
            printf("[Interact] Quest NPC: %s\n", obj->data.npc.name);
            {
                struct { uint32_t npc_id; } packet = { npc_id };
                network_send_packet(MSG_QUEST_DIALOG, &packet, sizeof(packet));
            }
            break;

        case NPC_TYPE_TELEPORT:
            printf("[Interact] Teleport NPC: %s\n", obj->data.npc.name);
            /* Show teleport menu */
            break;

        case NPC_TYPE_GUILD:
            printf("[Interact] Guild NPC: %s\n", obj->data.npc.name);
            {
                struct { uint32_t npc_id; } packet = { npc_id };
                network_send_packet(MSG_GUILD_NPC_TALK, &packet, sizeof(packet));
            }
            break;

        case NPC_TYPE_AUCTION:
            printf("[Interact] Auction NPC: %s\n", obj->data.npc.name);
            {
                struct { uint32_t npc_id; } packet = { npc_id };
                network_send_packet(MSG_AUCTION_OPEN, &packet, sizeof(packet));
            }
            break;

        case NPC_TYPE_SKILL_TRAINER:
            printf("[Interact] Skill trainer: %s\n", obj->data.npc.name);
            {
                struct { uint32_t npc_id; } packet = { npc_id };
                network_send_packet(MSG_SKILL_TRAINER_OPEN, &packet, sizeof(packet));
            }
            break;

        default:
            printf("[Interact] Talking to: %s\n", obj->data.npc.name);
            g_interact.in_dialog = true;
            g_interact.dialog_npc_id = npc_id;
            break;
    }

    return true;
}

bool interaction_pickup_item(uint32_t item_drop_id)
{
    Interactable* obj = find_by_id(item_drop_id);
    if (!obj || obj->type != INTERACT_ITEM) return false;

    printf("[Interact] Picking up item (drop_id=%u, item=%u, qty=%u)\n",
           item_drop_id, obj->data.item.item_id, obj->data.item.quantity);

    /* Send pickup request */
    struct {
        uint32_t drop_id;
    } packet = { item_drop_id };

    network_send_packet(MSG_GET_ITEM, &packet, sizeof(packet));

    /* Optimistically remove (server will confirm) */
    interaction_remove(item_drop_id);

    return true;
}

bool interaction_use_portal(uint32_t portal_id)
{
    Interactable* obj = find_by_id(portal_id);
    if (!obj || obj->type != INTERACT_PORTAL) return false;

    printf("[Interact] Using portal to: %s (map=%u)\n",
           obj->data.portal.target_name, obj->data.portal.target_map);

    /* Check for key requirement */
    if (obj->data.portal.requires_key) {
        /* Check inventory for key */
        printf("[Interact] Portal requires key item: %u\n",
               obj->data.portal.key_item_id);
        /* TODO: Check inventory */
    }

    /* Send teleport request */
    struct {
        uint32_t portal_id;
        uint32_t target_map;
    } packet = { portal_id, obj->data.portal.target_map };

    if (obj->data.portal.portal_type == PORTAL_TYPE_DUNGEON) {
        network_send_packet(MSG_DUNGEON_ENTER, &packet, sizeof(packet));
    } else {
        network_send_packet(MSG_PORTAL_USE, &packet, sizeof(packet));
    }

    return true;
}

bool interaction_use_object(uint32_t object_id)
{
    Interactable* obj = find_by_id(object_id);
    if (!obj || obj->type != INTERACT_OBJECT) return false;

    printf("[Interact] Using object: %u (type=%d)\n",
           object_id, obj->data.object.object_type);

    switch (obj->data.object.object_type) {
        case OBJECT_TYPE_CHEST:
            if (!obj->data.object.is_open) {
                obj->data.object.is_open = true;
                /* Request loot from server */
                struct { uint32_t object_id; } packet = { object_id };
                network_send_packet(MSG_OBJECT_INTERACT, &packet, sizeof(packet));
            }
            break;

        case OBJECT_TYPE_SWITCH:
            /* Toggle switch state */
            {
                struct { uint32_t object_id; } packet = { object_id };
                network_send_packet(MSG_OBJECT_INTERACT, &packet, sizeof(packet));
            }
            break;

        case OBJECT_TYPE_DOOR:
            {
                struct { uint32_t object_id; } packet = { object_id };
                network_send_packet(MSG_OBJECT_INTERACT, &packet, sizeof(packet));
            }
            break;

        case OBJECT_TYPE_DESTRUCTIBLE:
            /* Send attack to destroy */
            break;
    }

    return true;
}

/*
 * ============================================================================
 * Dialog System
 * ============================================================================
 */

void interaction_close_dialog(void)
{
    g_interact.in_dialog = false;
    g_interact.dialog_npc_id = 0;
}

void interaction_select_dialog_option(int option)
{
    if (!g_interact.in_dialog) return;

    g_interact.dialog_option = option;

    /* Send option to server */
    struct {
        uint32_t npc_id;
        uint32_t option;
    } packet = { g_interact.dialog_npc_id, option };

    network_send_packet(MSG_NPC_DIALOG_OPTION, &packet, sizeof(packet));
}

bool interaction_in_dialog(void)
{
    return g_interact.in_dialog;
}

/*
 * ============================================================================
 * Query Functions
 * ============================================================================
 */

Interactable* interaction_get_target(void)
{
    return g_interact.current_target;
}

int interaction_get_count(void)
{
    return g_interact.count;
}

int interaction_get_items_in_range(uint32_t* out_ids, int max_count)
{
    int count = 0;
    for (int i = 0; i < g_interact.count && count < max_count; i++) {
        Interactable* obj = &g_interact.list[i];
        if (obj->active && obj->type == INTERACT_ITEM && obj->in_range) {
            out_ids[count++] = obj->id;
        }
    }
    return count;
}

int interaction_get_npcs_in_range(uint32_t* out_ids, int max_count)
{
    int count = 0;
    for (int i = 0; i < g_interact.count && count < max_count; i++) {
        Interactable* obj = &g_interact.list[i];
        if (obj->active && obj->type == INTERACT_NPC && obj->in_range) {
            out_ids[count++] = obj->id;
        }
    }
    return count;
}

bool interaction_is_highlighted(uint32_t id)
{
    Interactable* obj = find_by_id(id);
    return obj && obj->highlighted;
}

float interaction_get_highlight_alpha(void)
{
    /* Pulsing effect */
    return 0.5f + 0.5f * sinf(g_interact.highlight_phase);
}

/*
 * ============================================================================
 * Settings
 * ============================================================================
 */

void interaction_set_auto_pickup(bool enabled)
{
    g_interact.auto_pickup_enabled = enabled;
}

void interaction_set_auto_pickup_rarity(uint8_t min_rarity)
{
    g_interact.auto_pickup_rarity = min_rarity;
}

/*
 * ============================================================================
 * NPC Query
 * ============================================================================
 */

void interaction_npc_set_quest(uint32_t npc_id, uint32_t quest_id, bool has_quest)
{
    Interactable* obj = find_by_id(npc_id);
    if (obj && obj->type == INTERACT_NPC) {
        obj->data.npc.quest_id = quest_id;
        obj->data.npc.has_quest = has_quest;
    }
}

bool interaction_npc_has_quest(uint32_t npc_id)
{
    Interactable* obj = find_by_id(npc_id);
    return obj && obj->type == INTERACT_NPC && obj->data.npc.has_quest;
}

const char* interaction_npc_get_name(uint32_t npc_id)
{
    Interactable* obj = find_by_id(npc_id);
    if (obj && obj->type == INTERACT_NPC) {
        return obj->data.npc.name;
    }
    return "Unknown";
}

NPCType interaction_npc_get_type(uint32_t npc_id)
{
    Interactable* obj = find_by_id(npc_id);
    if (obj && obj->type == INTERACT_NPC) {
        return obj->data.npc.npc_type;
    }
    return NPC_TYPE_NORMAL;
}

/*
 * ============================================================================
 * Item Query
 * ============================================================================
 */

uint32_t interaction_item_get_id(uint32_t drop_id)
{
    Interactable* obj = find_by_id(drop_id);
    if (obj && obj->type == INTERACT_ITEM) {
        return obj->data.item.item_id;
    }
    return 0;
}

uint8_t interaction_item_get_rarity(uint32_t drop_id)
{
    Interactable* obj = find_by_id(drop_id);
    if (obj && obj->type == INTERACT_ITEM) {
        return obj->data.item.rarity;
    }
    return 0;
}
