/**
 * @file scene_town.c
 * @brief Town scene implementation - main hub for player activities
 */

#include "../../include/scene.h"
#include "../../include/ui.h"
#include "../../include/network.h"
#include "../../include/protocol_types.h"
#include "../../include/entity.h"
#include "../../include/client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Town scene constants */
#define TOWN_MAP_WIDTH      2000
#define TOWN_MAP_HEIGHT     600
#define NPC_INTERACT_DIST   50.0f
#define MOVE_SPEED          200.0f

/* NPC types */
typedef enum {
    NPC_TYPE_SHOP = 0,
    NPC_TYPE_BLACKSMITH,
    NPC_TYPE_STORAGE,
    NPC_TYPE_QUEST,
    NPC_TYPE_TELEPORT,
    NPC_TYPE_GUILD,
    NPC_TYPE_AUCTION,
    NPC_TYPE_SKILL,
    NPC_TYPE_DISASSEMBLE
} NPCType;

/* NPC data */
typedef struct {
    u32 npc_id;
    u8 type;
    char name[32];
    float x;
    float y;
    bool interacting;
} NPCData;

/* Town scene data */
typedef struct {
    /* Player */
    CharacterData player;
    float player_x;
    float player_y;
    float player_dir;
    bool player_moving;
    float target_x;
    float target_y;

    /* Camera */
    float camera_x;
    float camera_y;

    /* Entities */
    Entity* entities[MAX_ENTITIES];
    int entity_count;

    /* NPCs */
    NPCData npcs[32];
    int npc_count;
    int interacting_npc;

    /* UI Elements */
    UIElement* hud_panel;
    UIElement* hp_bar;
    UIElement* mp_bar;
    UIElement* exp_bar;
    UIElement* gold_label;
    UIElement* fatigue_label;
    UIElement* level_label;
    UIElement* name_label;

    /* Quick bar */
    UIElement* skill_slots[8];

    /* Menu buttons */
    UIElement* inventory_btn;
    UIElement* skill_btn;
    UIElement* quest_btn;
    UIElement* guild_btn;
    UIElement* menu_btn;

    /* Chat */
    UIElement* chat_panel;
    UIElement* chat_input;
    UIElement* chat_history[10];
    int chat_count;

    /* NPC dialog */
    UIElement* npc_dialog;
    UIElement* npc_name_label;
    UIElement* npc_options[8];
    int npc_option_count;

    /* State */
    bool show_inventory;
    bool show_skills;
    bool show_npc_dialog;
    bool chat_focused;

    /* Map */
    GameMap* map;
} TownData;

/* External references */
extern NetworkClient* get_network_client(void);
extern UIManager* get_ui_manager(void);
extern void change_scene(SceneType type);

static NetworkClient* g_network = NULL;
static UIManager* g_ui = NULL;

/*
 * ============================================================================
 * Helper Functions
 * ============================================================================
 */

static float distance(float x1, float y1, float x2, float y2)
{
    float dx = x2 - x1;
    float dy = y2 - y1;
    return sqrtf(dx * dx + dy * dy);
}

static void clamp_camera(TownData* data)
{
    float half_w = WINDOW_WIDTH / 2.0f;
    float half_h = WINDOW_HEIGHT / 2.0f;

    if (data->camera_x < half_w) data->camera_x = half_w;
    if (data->camera_y < half_h) data->camera_y = half_h;
    if (data->camera_x > TOWN_MAP_WIDTH - half_w) data->camera_x = TOWN_MAP_WIDTH - half_w;
    if (data->camera_y > TOWN_MAP_HEIGHT - half_h) data->camera_y = TOWN_MAP_HEIGHT - half_h;
}

static void update_hud(TownData* data)
{
    char buf[64];

    if (data->hp_bar) {
        float hp_pct = (float)data->player.hp / (float)data->player.max_hp;
        if (hp_pct < 0) hp_pct = 0;
        if (hp_pct > 1) hp_pct = 1;
        /* Would update progress bar width here */
    }

    if (data->mp_bar) {
        float mp_pct = (float)data->player.mp / (float)data->player.max_mp;
        if (mp_pct < 0) mp_pct = 0;
        if (mp_pct > 1) mp_pct = 1;
    }

    if (data->gold_label) {
        snprintf(buf, sizeof(buf), "Gold: %u", data->player.info.gold);
        ui_set_text(data->gold_label, buf);
    }

    if (data->fatigue_label) {
        snprintf(buf, sizeof(buf), "FP: %u/%u",
                 data->player.info.fatigue, data->player.info.max_fatigue);
        ui_set_text(data->fatigue_label, buf);
    }

    if (data->level_label) {
        snprintf(buf, sizeof(buf), "Lv.%u", data->player.info.level);
        ui_set_text(data->level_label, buf);
    }

    if (data->name_label) {
        ui_set_text(data->name_label, data->player.info.name);
    }
}

/*
 * ============================================================================
 * NPC Interaction
 * ============================================================================
 */

static void spawn_default_npcs(TownData* data)
{
    /* Shop NPC */
    data->npcs[0].npc_id = 1001;
    data->npcs[0].type = NPC_TYPE_SHOP;
    strcpy(data->npcs[0].name, "Seria");
    data->npcs[0].x = 200;
    data->npcs[0].y = 400;

    /* Blacksmith */
    data->npcs[1].npc_id = 1002;
    data->npcs[1].type = NPC_TYPE_BLACKSMITH;
    strcpy(data->npcs[1].name, "Kiri");
    data->npcs[1].x = 400;
    data->npcs[1].y = 400;

    /* Storage */
    data->npcs[2].npc_id = 1003;
    data->npcs[2].type = NPC_TYPE_STORAGE;
    strcpy(data->npcs[2].name, "Delilah");
    data->npcs[2].x = 600;
    data->npcs[2].y = 400;

    /* Dungeon teleporter */
    data->npcs[3].npc_id = 1004;
    data->npcs[3].type = NPC_TYPE_TELEPORT;
    strcpy(data->npcs[3].name, "Dungeon Gate");
    data->npcs[3].x = 800;
    data->npcs[3].y = 400;

    /* Guild manager */
    data->npcs[4].npc_id = 1005;
    data->npcs[4].type = NPC_TYPE_GUILD;
    strcpy(data->npcs[4].name, "Kargon");
    data->npcs[4].x = 1000;
    data->npcs[4].y = 400;

    data->npc_count = 5;
}

static void show_npc_dialog(TownData* data, int npc_index)
{
    NPCData* npc = &data->npcs[npc_index];
    int i;

    data->interacting_npc = npc_index;
    data->show_npc_dialog = true;

    if (data->npc_dialog) {
        data->npc_dialog->visible = true;
    }

    if (data->npc_name_label) {
        ui_set_text(data->npc_name_label, npc->name);
    }

    /* Set options based on NPC type */
    data->npc_option_count = 0;

    switch (npc->type) {
        case NPC_TYPE_SHOP:
            if (data->npc_options[0]) ui_set_text(data->npc_options[0], "Buy Items");
            if (data->npc_options[1]) ui_set_text(data->npc_options[1], "Sell Items");
            data->npc_option_count = 2;
            break;

        case NPC_TYPE_BLACKSMITH:
            if (data->npc_options[0]) ui_set_text(data->npc_options[0], "Repair All");
            if (data->npc_options[1]) ui_set_text(data->npc_options[1], "Enhance Item");
            if (data->npc_options[2]) ui_set_text(data->npc_options[2], "Disassemble");
            data->npc_option_count = 3;
            break;

        case NPC_TYPE_STORAGE:
            if (data->npc_options[0]) ui_set_text(data->npc_options[0], "Open Storage");
            if (data->npc_options[1]) ui_set_text(data->npc_options[1], "Expand Storage");
            data->npc_option_count = 2;
            break;

        case NPC_TYPE_TELEPORT:
            if (data->npc_options[0]) ui_set_text(data->npc_options[0], "Enter Dungeon");
            if (data->npc_options[1]) ui_set_text(data->npc_options[1], "Abyss Mode");
            data->npc_option_count = 2;
            break;

        case NPC_TYPE_GUILD:
            if (data->npc_options[0]) ui_set_text(data->npc_options[0], "Guild Info");
            if (data->npc_options[1]) ui_set_text(data->npc_options[1], "Create Guild");
            if (data->npc_options[2]) ui_set_text(data->npc_options[2], "Join Guild");
            data->npc_option_count = 3;
            break;

        default:
            if (data->npc_options[0]) ui_set_text(data->npc_options[0], "Talk");
            data->npc_option_count = 1;
            break;
    }

    /* Show/hide option buttons */
    for (i = 0; i < 8; i++) {
        if (data->npc_options[i]) {
            data->npc_options[i]->visible = (i < data->npc_option_count);
        }
    }

    printf("Interacting with NPC: %s (type=%d)\n", npc->name, npc->type);
}

static void close_npc_dialog(TownData* data)
{
    data->show_npc_dialog = false;
    data->interacting_npc = -1;

    if (data->npc_dialog) {
        data->npc_dialog->visible = false;
    }
}

static void on_npc_option_click(UIElement* element, void* user_data)
{
    TownData* data = (TownData*)user_data;
    NPCData* npc;
    int option = -1;
    int i;

    if (!data || data->interacting_npc < 0) return;

    npc = &data->npcs[data->interacting_npc];

    /* Find which option was clicked */
    for (i = 0; i < 8; i++) {
        if (data->npc_options[i] == element) {
            option = i;
            break;
        }
    }

    if (option < 0) return;

    printf("NPC option clicked: %s -> option %d\n", npc->name, option);

    /* Handle NPC actions */
    switch (npc->type) {
        case NPC_TYPE_TELEPORT:
            if (option == 0) {
                /* Enter dungeon */
                close_npc_dialog(data);
                change_scene(SCENE_DUNGEON);
            }
            break;

        case NPC_TYPE_SHOP:
            if (g_network) {
                /* Open shop */
                PacketBuilder* builder = network_begin_packet(g_network, MSG_NPC_SHOP_BUY);
                packet_write_u32(builder, npc->npc_id);
                network_end_packet(g_network, builder);
            }
            break;

        case NPC_TYPE_STORAGE:
            if (g_network && option == 0) {
                PacketBuilder* builder = network_begin_packet(g_network, MSG_STORAGE_OPEN);
                network_end_packet(g_network, builder);
            }
            break;

        default:
            break;
    }
}

static void on_close_dialog_click(UIElement* element, void* user_data)
{
    TownData* data = (TownData*)user_data;
    (void)element;
    close_npc_dialog(data);
}

/*
 * ============================================================================
 * UI Callbacks
 * ============================================================================
 */

static void on_inventory_click(UIElement* element, void* user_data)
{
    TownData* data = (TownData*)user_data;
    (void)element;
    if (data) {
        data->show_inventory = !data->show_inventory;
        printf("Toggle inventory: %d\n", data->show_inventory);
    }
}

static void on_skill_click(UIElement* element, void* user_data)
{
    TownData* data = (TownData*)user_data;
    (void)element;
    if (data) {
        data->show_skills = !data->show_skills;
        printf("Toggle skills: %d\n", data->show_skills);
    }
}

static void on_menu_click(UIElement* element, void* user_data)
{
    TownData* data = (TownData*)user_data;
    (void)element;
    (void)data;
    /* Return to character select */
    change_scene(SCENE_CHARSELECT);
}

static void on_chat_submit(UIElement* element, void* user_data)
{
    TownData* data = (TownData*)user_data;
    const char* msg;
    (void)element;

    if (!data || !g_network) return;

    msg = ui_get_text(data->chat_input);
    if (msg && strlen(msg) > 0) {
        /* Send chat message */
        PacketBuilder* builder = network_begin_packet(g_network, MSG_CHAT);
        packet_write_u8(builder, CHAT_CHANNEL_NORMAL);
        packet_write_string(builder, msg, MAX_CHAT_LENGTH);
        network_end_packet(g_network, builder);

        ui_set_text(data->chat_input, "");
        printf("Chat sent: %s\n", msg);
    }
}

/*
 * ============================================================================
 * Scene Lifecycle
 * ============================================================================
 */

static int town_init(Scene* scene)
{
    TownData* data;
    int i;

    if (!scene) return CLIENT_ERROR;

    data = calloc(1, sizeof(TownData));
    if (!data) return CLIENT_ERROR_MEMORY;

    scene->data = data;

    g_ui = get_ui_manager();
    g_network = get_network_client();

    /* Initialize player position */
    data->player_x = 300;
    data->player_y = 400;
    data->camera_x = data->player_x;
    data->camera_y = data->player_y;
    data->interacting_npc = -1;

    /* Spawn NPCs */
    spawn_default_npcs(data);

    if (!g_ui) {
        fprintf(stderr, "UI manager not available\n");
        return CLIENT_ERROR;
    }

    /* HUD Panel (top) */
    data->hud_panel = ui_create_panel(g_ui, 10, 10, 300, 80);
    if (data->hud_panel) {
        data->hud_panel->bg_color = (Color){0, 0, 0, 180};
    }

    data->name_label = ui_create_label(g_ui, 20, 15, "Player");
    if (data->name_label) ui_add_child(data->hud_panel, data->name_label);

    data->level_label = ui_create_label(g_ui, 200, 15, "Lv.1");
    if (data->level_label) ui_add_child(data->hud_panel, data->level_label);

    /* HP/MP bars would be progress bars - using labels for now */
    data->hp_bar = ui_create_label(g_ui, 20, 35, "HP: 1000/1000");
    if (data->hp_bar) {
        data->hp_bar->text_color = (Color){255, 100, 100, 255};
        ui_add_child(data->hud_panel, data->hp_bar);
    }

    data->mp_bar = ui_create_label(g_ui, 20, 50, "MP: 500/500");
    if (data->mp_bar) {
        data->mp_bar->text_color = (Color){100, 150, 255, 255};
        ui_add_child(data->hud_panel, data->mp_bar);
    }

    data->gold_label = ui_create_label(g_ui, 20, 70, "Gold: 0");
    if (data->gold_label) {
        data->gold_label->text_color = (Color){255, 215, 0, 255};
        ui_add_child(data->hud_panel, data->gold_label);
    }

    data->fatigue_label = ui_create_label(g_ui, 150, 70, "FP: 156/156");
    if (data->fatigue_label) ui_add_child(data->hud_panel, data->fatigue_label);

    /* Menu buttons (right side) */
    data->inventory_btn = ui_create_button(g_ui, WINDOW_WIDTH - 110, 10, 100, 30,
                                           "Inventory", on_inventory_click, data);
    data->skill_btn = ui_create_button(g_ui, WINDOW_WIDTH - 110, 45, 100, 30,
                                       "Skills", on_skill_click, data);
    data->menu_btn = ui_create_button(g_ui, WINDOW_WIDTH - 110, 80, 100, 30,
                                      "Menu", on_menu_click, data);

    /* Chat panel (bottom) */
    data->chat_panel = ui_create_panel(g_ui, 10, WINDOW_HEIGHT - 150, 400, 140);
    if (data->chat_panel) {
        data->chat_panel->bg_color = (Color){0, 0, 0, 150};
    }

    data->chat_input = ui_create_textbox(g_ui, 15, WINDOW_HEIGHT - 35, 350, 25, 128);
    if (data->chat_input) {
        ui_add_child(data->chat_panel, data->chat_input);
    }

    /* NPC Dialog (center, hidden initially) */
    data->npc_dialog = ui_create_panel(g_ui, WINDOW_WIDTH/2 - 150, WINDOW_HEIGHT/2 - 150,
                                       300, 300);
    if (data->npc_dialog) {
        data->npc_dialog->bg_color = (Color){40, 40, 56, 240};
        data->npc_dialog->visible = false;

        data->npc_name_label = ui_create_label(g_ui, WINDOW_WIDTH/2 - 50,
                                               WINDOW_HEIGHT/2 - 130, "NPC");
        if (data->npc_name_label) {
            data->npc_name_label->text_color = (Color){255, 255, 100, 255};
            ui_add_child(data->npc_dialog, data->npc_name_label);
        }

        /* NPC option buttons */
        for (i = 0; i < 8; i++) {
            data->npc_options[i] = ui_create_button(g_ui,
                WINDOW_WIDTH/2 - 120, WINDOW_HEIGHT/2 - 90 + i * 35,
                240, 30, "", on_npc_option_click, data);
            if (data->npc_options[i]) {
                data->npc_options[i]->bg_color = (Color){60, 60, 80, 255};
                data->npc_options[i]->visible = false;
                ui_add_child(data->npc_dialog, data->npc_options[i]);
            }
        }

        /* Close button */
        UIElement* close_btn = ui_create_button(g_ui,
            WINDOW_WIDTH/2 - 50, WINDOW_HEIGHT/2 + 120, 100, 30,
            "Close", on_close_dialog_click, data);
        if (close_btn) {
            close_btn->bg_color = (Color){100, 60, 60, 255};
            ui_add_child(data->npc_dialog, close_btn);
        }
    }

    printf("Town scene initialized\n");
    return CLIENT_OK;
}

static void town_cleanup(Scene* scene)
{
    TownData* data;
    int i;

    if (!scene) return;

    data = (TownData*)scene->data;
    if (data) {
        /* Clean up entities */
        for (i = 0; i < data->entity_count; i++) {
            if (data->entities[i]) {
                entity_destroy(data->entities[i]);
            }
        }

        if (data->map) {
            map_free(data->map);
        }

        free(data);
        scene->data = NULL;
    }
}

static void town_update(Scene* scene, float delta)
{
    TownData* data;
    int i;

    if (!scene) return;

    data = (TownData*)scene->data;
    if (!data) return;

    /* Player movement */
    if (data->player_moving) {
        float dx = data->target_x - data->player_x;
        float dy = data->target_y - data->player_y;
        float dist = sqrtf(dx * dx + dy * dy);

        if (dist > 5.0f) {
            float move = MOVE_SPEED * delta;
            if (move > dist) move = dist;

            data->player_x += (dx / dist) * move;
            data->player_y += (dy / dist) * move;
            data->player_dir = atan2f(dy, dx);

            /* Send position update */
            if (g_network && (int)(data->player_x) % 50 == 0) {
                PacketBuilder* builder = network_begin_packet(g_network, MSG_SET_USER_POSITION);
                packet_write_u32(builder, *(u32*)&data->player_x);
                packet_write_u32(builder, *(u32*)&data->player_y);
                network_end_packet(g_network, builder);
            }
        } else {
            data->player_moving = false;
        }
    }

    /* Camera follow player */
    data->camera_x += (data->player_x - data->camera_x) * 5.0f * delta;
    data->camera_y += (data->player_y - data->camera_y) * 5.0f * delta;
    clamp_camera(data);

    /* Check NPC proximity */
    for (i = 0; i < data->npc_count; i++) {
        float dist = distance(data->player_x, data->player_y,
                             data->npcs[i].x, data->npcs[i].y);
        data->npcs[i].interacting = (dist < NPC_INTERACT_DIST);
    }

    /* Update HUD */
    update_hud(data);
}

static void town_draw(Scene* scene, Renderer* renderer)
{
    TownData* data;
    int i;
    float screen_x, screen_y;

    if (!scene || !renderer) return;

    data = (TownData*)scene->data;
    if (!data) return;

    /* Calculate screen offset */
    float offset_x = WINDOW_WIDTH / 2.0f - data->camera_x;
    float offset_y = WINDOW_HEIGHT / 2.0f - data->camera_y;

    /* Draw background */
    renderer_clear(renderer, (Color){48, 64, 80, 255});

    /* Draw ground */
    Rect ground = {offset_x, offset_y + 450, TOWN_MAP_WIDTH, 150};
    renderer_draw_rect(renderer, ground, (Color){80, 60, 40, 255}, true);

    /* Draw NPCs */
    for (i = 0; i < data->npc_count; i++) {
        screen_x = data->npcs[i].x + offset_x;
        screen_y = data->npcs[i].y + offset_y;

        /* NPC body */
        Rect npc_rect = {screen_x - 20, screen_y - 50, 40, 50};
        Color npc_color = data->npcs[i].interacting ?
            (Color){100, 200, 100, 255} : (Color){100, 100, 200, 255};
        renderer_draw_rect(renderer, npc_rect, npc_color, true);

        /* NPC name */
        /* Would draw text here */
    }

    /* Draw player */
    screen_x = data->player_x + offset_x;
    screen_y = data->player_y + offset_y;

    Rect player_rect = {screen_x - 20, screen_y - 60, 40, 60};
    renderer_draw_rect(renderer, player_rect, (Color){200, 150, 100, 255}, true);

    /* Draw other entities */
    for (i = 0; i < data->entity_count; i++) {
        if (data->entities[i]) {
            Entity* e = data->entities[i];
            screen_x = e->x + offset_x;
            screen_y = e->y + offset_y;

            Rect ent_rect = {screen_x - 15, screen_y - 40, 30, 40};
            Color ent_color = (e->type == ENTITY_PLAYER) ?
                (Color){150, 150, 200, 255} : (Color){200, 100, 100, 255};
            renderer_draw_rect(renderer, ent_rect, ent_color, true);
        }
    }

    /* UI is drawn by UI manager */
}

static void town_handle_input(Scene* scene, SDL_Event* event)
{
    TownData* data;

    if (!scene || !event) return;

    data = (TownData*)scene->data;
    if (!data) return;

    /* Don't process movement if dialog is open */
    if (data->show_npc_dialog) {
        if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_ESCAPE) {
            close_npc_dialog(data);
        }
        return;
    }

    switch (event->type) {
        case SDL_MOUSEBUTTONDOWN:
            if (event->button.button == SDL_BUTTON_LEFT) {
                /* Move to clicked position */
                float offset_x = WINDOW_WIDTH / 2.0f - data->camera_x;
                float offset_y = WINDOW_HEIGHT / 2.0f - data->camera_y;

                data->target_x = event->button.x - offset_x;
                data->target_y = event->button.y - offset_y;
                data->player_moving = true;
            }
            break;

        case SDL_KEYDOWN:
            switch (event->key.keysym.sym) {
                case SDLK_SPACE:
                    /* Interact with nearby NPC */
                    {
                        int i;
                        for (i = 0; i < data->npc_count; i++) {
                            if (data->npcs[i].interacting) {
                                show_npc_dialog(data, i);
                                break;
                            }
                        }
                    }
                    break;

                case SDLK_i:
                    on_inventory_click(NULL, data);
                    break;

                case SDLK_k:
                    on_skill_click(NULL, data);
                    break;

                case SDLK_RETURN:
                    if (data->chat_input) {
                        on_chat_submit(data->chat_input, data);
                    }
                    break;

                case SDLK_ESCAPE:
                    on_menu_click(NULL, data);
                    break;
            }
            break;
    }
}

static void town_on_packet(Scene* scene, u16 msg_no, u8* pkt_data, int len)
{
    TownData* data;

    if (!scene) return;

    data = (TownData*)scene->data;
    if (!data) return;

    printf("Town scene received packet: %u, len=%d\n", msg_no, len);

    switch (msg_no) {
        case MSG_RESP_PLAYER_SPAWN:
            /* Another player spawned */
            printf("Player spawn received\n");
            break;

        case MSG_RESP_ENTITY_MOVE:
            /* Entity moved */
            break;

        case MSG_RESP_CHAT:
            /* Chat message received */
            printf("Chat message received\n");
            break;

        case MSG_RESP_INVENTORY_UPDATE:
            printf("Inventory update received\n");
            break;

        case MSG_RESP_GOLD_UPDATE:
            if (len >= 4) {
                data->player.info.gold = pkt_data[0] | (pkt_data[1] << 8) |
                                         (pkt_data[2] << 16) | (pkt_data[3] << 24);
                update_hud(data);
            }
            break;

        case MSG_RESP_HP_UPDATE:
            if (len >= 8) {
                data->player.hp = pkt_data[0] | (pkt_data[1] << 8) |
                                  (pkt_data[2] << 16) | (pkt_data[3] << 24);
                data->player.max_hp = pkt_data[4] | (pkt_data[5] << 8) |
                                      (pkt_data[6] << 16) | (pkt_data[7] << 24);
                update_hud(data);
            }
            break;

        default:
            break;
    }
}

/*
 * ============================================================================
 * Scene Creation
 * ============================================================================
 */

Scene* scene_create_town(void)
{
    Scene* scene = calloc(1, sizeof(Scene));
    if (!scene) return NULL;

    scene->type = SCENE_TOWN;
    scene->init = town_init;
    scene->cleanup = town_cleanup;
    scene->update = town_update;
    scene->draw = town_draw;
    scene->handle_input = town_handle_input;
    scene->on_packet = town_on_packet;

    return scene;
}

/*
 * ============================================================================
 * External Data Update
 * ============================================================================
 */

void town_set_player_data(Scene* scene, CharacterData* player)
{
    TownData* data;

    if (!scene || scene->type != SCENE_TOWN || !player) return;

    data = (TownData*)scene->data;
    if (!data) return;

    memcpy(&data->player, player, sizeof(CharacterData));
    data->player_x = player->x;
    data->player_y = player->y;

    update_hud(data);
}
