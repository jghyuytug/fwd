/**
 * @file scene_dungeon.c
 * @brief Dungeon scene implementation - combat and monster battles
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

/* Dungeon constants */
#define DUNGEON_WIDTH       1600
#define DUNGEON_HEIGHT      480
#define ROOM_WIDTH          400
#define MONSTER_SPAWN_DELAY 1.0f
#define SKILL_COOLDOWN      0.5f

/* Dungeon state */
typedef enum {
    DUNGEON_STATE_LOADING,
    DUNGEON_STATE_PLAYING,
    DUNGEON_STATE_BOSS,
    DUNGEON_STATE_CLEAR,
    DUNGEON_STATE_FAILED
} DungeonState;

/* Monster data */
typedef struct {
    u32 entity_id;
    u32 template_id;
    char name[32];
    float x;
    float y;
    float dir;
    i32 hp;
    i32 max_hp;
    u8 level;
    u8 state;
    bool alive;
    bool is_boss;
    float attack_timer;
    float stun_timer;
} MonsterData;

/* Dungeon scene data */
typedef struct {
    /* Dungeon info */
    DungeonInfo info;
    DungeonState state;
    float state_timer;

    /* Player */
    CharacterData player;
    float player_x;
    float player_y;
    float player_dir;
    u8 player_state;

    /* Combat */
    float attack_cooldown;
    u32 combo_count;
    float combo_timer;
    i32 total_damage;
    u32 kill_count;

    /* Camera */
    float camera_x;
    float camera_y;
    float camera_shake;

    /* Monsters */
    MonsterData monsters[64];
    int monster_count;
    int monsters_alive;

    /* Drops */
    struct {
        u32 item_id;
        float x;
        float y;
        bool picked;
    } drops[32];
    int drop_count;

    /* Rooms */
    int current_room;
    int room_count;
    bool room_cleared;
    float room_transition_timer;

    /* UI */
    UIElement* hud_panel;
    UIElement* hp_bar;
    UIElement* mp_bar;
    UIElement* boss_hp_bar;
    UIElement* boss_name_label;
    UIElement* room_label;
    UIElement* time_label;
    UIElement* combo_label;
    UIElement* damage_label;

    /* Skill bar */
    UIElement* skill_slots[8];
    float skill_cooldowns[8];

    /* Result dialog */
    UIElement* result_dialog;
    UIElement* rank_label;
    UIElement* exp_label;
    UIElement* gold_label;
    UIElement* item_labels[8];
    UIElement* continue_btn;

    /* Pause menu */
    UIElement* pause_menu;
    UIElement* resume_btn;
    UIElement* quit_btn;

    /* State flags */
    bool show_result;
    bool show_pause;
    bool attacking;

    /* Timing */
    float elapsed_time;
    float time_limit;
} DungeonData;

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

static void spawn_monsters(DungeonData* data, int room)
{
    int count = 3 + room + (data->info.difficulty * 2);
    int i;
    float start_x = room * ROOM_WIDTH + 100;

    if (count > 10) count = 10;

    for (i = 0; i < count; i++) {
        MonsterData* m = &data->monsters[data->monster_count];
        m->entity_id = 2000 + data->monster_count;
        m->template_id = 100 + (rand() % 5);
        snprintf(m->name, sizeof(m->name), "Monster_%d", data->monster_count);
        m->x = start_x + (rand() % 300);
        m->y = 350 + (rand() % 50);
        m->dir = -1.0f;
        m->hp = 100 + room * 50 + data->info.difficulty * 30;
        m->max_hp = m->hp;
        m->level = data->info.difficulty + room;
        m->state = ENTITY_STATE_IDLE;
        m->alive = true;
        m->is_boss = false;
        m->attack_timer = 1.0f + (float)(rand() % 100) / 100.0f;
        m->stun_timer = 0;

        data->monster_count++;
        data->monsters_alive++;
    }

    /* Spawn boss on last room */
    if (room == data->room_count - 1) {
        MonsterData* boss = &data->monsters[data->monster_count];
        boss->entity_id = 9000;
        boss->template_id = 500;
        snprintf(boss->name, sizeof(boss->name), "Boss");
        boss->x = start_x + 200;
        boss->y = 350;
        boss->dir = -1.0f;
        boss->hp = 500 + data->info.difficulty * 200;
        boss->max_hp = boss->hp;
        boss->level = data->info.difficulty + 5;
        boss->state = ENTITY_STATE_IDLE;
        boss->alive = true;
        boss->is_boss = true;
        boss->attack_timer = 2.0f;
        boss->stun_timer = 0;

        data->monster_count++;
        data->monsters_alive++;
        data->state = DUNGEON_STATE_BOSS;
    }

    printf("Spawned %d monsters in room %d\n", count, room);
}

static void update_hud(DungeonData* data)
{
    char buf[64];

    if (data->room_label) {
        snprintf(buf, sizeof(buf), "Room %d/%d", data->current_room + 1, data->room_count);
        ui_set_text(data->room_label, buf);
    }

    if (data->time_label) {
        int seconds = (int)data->elapsed_time;
        int minutes = seconds / 60;
        seconds %= 60;
        snprintf(buf, sizeof(buf), "%02d:%02d", minutes, seconds);
        ui_set_text(data->time_label, buf);
    }

    if (data->combo_label) {
        if (data->combo_count > 1) {
            snprintf(buf, sizeof(buf), "%u Combo!", data->combo_count);
            ui_set_text(data->combo_label, buf);
            data->combo_label->visible = true;
        } else {
            data->combo_label->visible = false;
        }
    }

    if (data->damage_label) {
        snprintf(buf, sizeof(buf), "DMG: %d", data->total_damage);
        ui_set_text(data->damage_label, buf);
    }
}

static void show_result(DungeonData* data, DungeonResult* result)
{
    char buf[64];
    int i;

    data->show_result = true;
    data->state = DUNGEON_STATE_CLEAR;

    if (data->result_dialog) {
        data->result_dialog->visible = true;
    }

    if (data->rank_label) {
        const char* ranks = "SABCD";
        snprintf(buf, sizeof(buf), "Rank: %c", ranks[result->rank]);
        ui_set_text(data->rank_label, buf);
    }

    if (data->exp_label) {
        snprintf(buf, sizeof(buf), "EXP: +%u", result->exp_gained);
        ui_set_text(data->exp_label, buf);
    }

    if (data->gold_label) {
        snprintf(buf, sizeof(buf), "Gold: +%u", result->gold_gained);
        ui_set_text(data->gold_label, buf);
    }

    /* Show items */
    for (i = 0; i < 8 && i < result->item_count; i++) {
        if (data->item_labels[i]) {
            snprintf(buf, sizeof(buf), "Item: %u x%d",
                     result->items[i].item_id, result->items[i].count);
            ui_set_text(data->item_labels[i], buf);
            data->item_labels[i]->visible = true;
        }
    }
}

/*
 * ============================================================================
 * Combat
 * ============================================================================
 */

static void player_attack(DungeonData* data)
{
    int i;
    float attack_range = 80.0f;

    if (data->attack_cooldown > 0) return;

    data->attacking = true;
    data->attack_cooldown = SKILL_COOLDOWN;
    data->camera_shake = 0.1f;

    /* Check monster hits */
    for (i = 0; i < data->monster_count; i++) {
        MonsterData* m = &data->monsters[i];
        if (!m->alive) continue;

        float dist = distance(data->player_x, data->player_y, m->x, m->y);
        if (dist < attack_range) {
            /* Calculate damage */
            int damage = 50 + (data->player.phys_atk / 2);
            bool crit = (rand() % 100) < 15;
            if (crit) damage = (int)(damage * 1.5f);

            m->hp -= damage;
            m->stun_timer = 0.2f;
            data->total_damage += damage;
            data->combo_count++;
            data->combo_timer = 2.0f;

            /* Knockback */
            float kb_dir = (m->x > data->player_x) ? 1.0f : -1.0f;
            m->x += kb_dir * 20.0f;

            /* Send damage to server */
            if (g_network) {
                PacketBuilder* builder = network_begin_packet(g_network, MSG_USE_SKILL);
                packet_write_u32(builder, 1);  /* Basic attack */
                packet_write_u32(builder, m->entity_id);
                packet_write_i32(builder, damage);
                network_end_packet(g_network, builder);
            }

            printf("Hit monster %u for %d damage%s\n",
                   m->entity_id, damage, crit ? " (CRIT)" : "");

            /* Check death */
            if (m->hp <= 0) {
                m->alive = false;
                m->state = ENTITY_STATE_DEAD;
                data->kill_count++;
                data->monsters_alive--;

                /* Notify server */
                if (g_network) {
                    PacketBuilder* builder = network_begin_packet(g_network, MSG_DIE_MONSTER);
                    packet_write_u32(builder, m->entity_id);
                    network_end_packet(g_network, builder);
                }

                printf("Monster %u killed! (%d remaining)\n", m->entity_id, data->monsters_alive);

                /* Drop item */
                if (data->drop_count < 32 && (rand() % 100) < 30) {
                    data->drops[data->drop_count].item_id = 1000 + (rand() % 100);
                    data->drops[data->drop_count].x = m->x;
                    data->drops[data->drop_count].y = m->y;
                    data->drops[data->drop_count].picked = false;
                    data->drop_count++;
                }
            }
        }
    }
}

static void monster_ai_update(DungeonData* data, float delta)
{
    int i;

    for (i = 0; i < data->monster_count; i++) {
        MonsterData* m = &data->monsters[i];
        if (!m->alive) continue;

        /* Stun recovery */
        if (m->stun_timer > 0) {
            m->stun_timer -= delta;
            continue;
        }

        /* Chase player */
        float dx = data->player_x - m->x;
        float dist = fabsf(dx);

        if (dist > 60) {
            /* Move towards player */
            float speed = m->is_boss ? 80.0f : 60.0f;
            m->x += (dx > 0 ? 1 : -1) * speed * delta;
            m->dir = dx > 0 ? 1.0f : -1.0f;
            m->state = ENTITY_STATE_WALKING;
        } else {
            /* Attack */
            m->attack_timer -= delta;
            if (m->attack_timer <= 0) {
                m->attack_timer = m->is_boss ? 1.5f : 2.0f;
                m->state = ENTITY_STATE_ATTACKING;

                /* Deal damage to player */
                int damage = 10 + m->level * 2;
                if (m->is_boss) damage *= 2;

                data->player.hp -= damage;
                printf("Monster attacked player for %d damage\n", damage);

                /* Check player death */
                if (data->player.hp <= 0) {
                    data->state = DUNGEON_STATE_FAILED;
                    printf("Player died!\n");

                    if (g_network) {
                        PacketBuilder* builder = network_begin_packet(g_network, MSG_DIE_CHARACTER);
                        network_end_packet(g_network, builder);
                    }
                }
            } else {
                m->state = ENTITY_STATE_IDLE;
            }
        }
    }
}

/*
 * ============================================================================
 * UI Callbacks
 * ============================================================================
 */

static void on_continue_click(UIElement* element, void* user_data)
{
    DungeonData* data = (DungeonData*)user_data;
    (void)element;

    if (data) {
        change_scene(SCENE_TOWN);
    }
}

static void on_resume_click(UIElement* element, void* user_data)
{
    DungeonData* data = (DungeonData*)user_data;
    (void)element;

    if (data) {
        data->show_pause = false;
        if (data->pause_menu) {
            data->pause_menu->visible = false;
        }
    }
}

static void on_quit_click(UIElement* element, void* user_data)
{
    DungeonData* data = (DungeonData*)user_data;
    (void)element;

    if (data && g_network) {
        PacketBuilder* builder = network_begin_packet(g_network, MSG_GIVEUP_GAME);
        network_end_packet(g_network, builder);
    }

    change_scene(SCENE_TOWN);
}

/*
 * ============================================================================
 * Scene Lifecycle
 * ============================================================================
 */

static int dungeon_init(Scene* scene)
{
    DungeonData* data;
    int i;

    if (!scene) return CLIENT_ERROR;

    data = calloc(1, sizeof(DungeonData));
    if (!data) return CLIENT_ERROR_MEMORY;

    scene->data = data;

    g_ui = get_ui_manager();
    g_network = get_network_client();

    /* Initialize dungeon */
    data->state = DUNGEON_STATE_LOADING;
    data->info.difficulty = 1;
    data->room_count = 3 + data->info.difficulty;
    if (data->room_count > 7) data->room_count = 7;
    data->current_room = 0;
    data->time_limit = 300.0f;

    /* Player position */
    data->player_x = 100;
    data->player_y = 380;
    data->player.hp = 1000;
    data->player.max_hp = 1000;
    data->player.mp = 500;
    data->player.max_mp = 500;
    data->player.phys_atk = 100;

    /* Camera */
    data->camera_x = data->player_x;
    data->camera_y = DUNGEON_HEIGHT / 2;

    if (!g_ui) {
        fprintf(stderr, "UI manager not available\n");
        return CLIENT_ERROR;
    }

    /* HUD */
    data->hud_panel = ui_create_panel(g_ui, 10, 10, 250, 80);
    if (data->hud_panel) {
        data->hud_panel->bg_color = (Color){0, 0, 0, 180};
    }

    data->hp_bar = ui_create_label(g_ui, 20, 20, "HP: 1000/1000");
    if (data->hp_bar) {
        data->hp_bar->text_color = (Color){255, 100, 100, 255};
        ui_add_child(data->hud_panel, data->hp_bar);
    }

    data->mp_bar = ui_create_label(g_ui, 20, 40, "MP: 500/500");
    if (data->mp_bar) {
        data->mp_bar->text_color = (Color){100, 150, 255, 255};
        ui_add_child(data->hud_panel, data->mp_bar);
    }

    data->room_label = ui_create_label(g_ui, 20, 60, "Room 1/3");
    if (data->room_label) ui_add_child(data->hud_panel, data->room_label);

    data->time_label = ui_create_label(g_ui, 150, 60, "00:00");
    if (data->time_label) ui_add_child(data->hud_panel, data->time_label);

    /* Combo/Damage display */
    data->combo_label = ui_create_label(g_ui, WINDOW_WIDTH / 2 - 50, 100, "");
    if (data->combo_label) {
        data->combo_label->text_color = (Color){255, 200, 50, 255};
        data->combo_label->visible = false;
    }

    data->damage_label = ui_create_label(g_ui, WINDOW_WIDTH - 150, 20, "DMG: 0");

    /* Boss HP bar (top center) */
    data->boss_hp_bar = ui_create_panel(g_ui, WINDOW_WIDTH/2 - 150, 10, 300, 30);
    if (data->boss_hp_bar) {
        data->boss_hp_bar->bg_color = (Color){80, 0, 0, 200};
        data->boss_hp_bar->visible = false;
    }

    data->boss_name_label = ui_create_label(g_ui, WINDOW_WIDTH/2 - 30, 15, "BOSS");
    if (data->boss_name_label) {
        data->boss_name_label->text_color = (Color){255, 50, 50, 255};
        data->boss_name_label->visible = false;
    }

    /* Result dialog */
    data->result_dialog = ui_create_panel(g_ui, WINDOW_WIDTH/2 - 150, WINDOW_HEIGHT/2 - 150,
                                          300, 300);
    if (data->result_dialog) {
        data->result_dialog->bg_color = (Color){40, 40, 56, 240};
        data->result_dialog->visible = false;

        data->rank_label = ui_create_label(g_ui, WINDOW_WIDTH/2 - 40, WINDOW_HEIGHT/2 - 130, "Rank: S");
        if (data->rank_label) {
            data->rank_label->text_color = (Color){255, 200, 50, 255};
            ui_add_child(data->result_dialog, data->rank_label);
        }

        data->exp_label = ui_create_label(g_ui, WINDOW_WIDTH/2 - 80, WINDOW_HEIGHT/2 - 100, "EXP: +0");
        if (data->exp_label) ui_add_child(data->result_dialog, data->exp_label);

        data->gold_label = ui_create_label(g_ui, WINDOW_WIDTH/2 - 80, WINDOW_HEIGHT/2 - 75, "Gold: +0");
        if (data->gold_label) {
            data->gold_label->text_color = (Color){255, 215, 0, 255};
            ui_add_child(data->result_dialog, data->gold_label);
        }

        for (i = 0; i < 8; i++) {
            data->item_labels[i] = ui_create_label(g_ui,
                WINDOW_WIDTH/2 - 80, WINDOW_HEIGHT/2 - 40 + i * 20, "");
            if (data->item_labels[i]) {
                data->item_labels[i]->visible = false;
                ui_add_child(data->result_dialog, data->item_labels[i]);
            }
        }

        data->continue_btn = ui_create_button(g_ui, WINDOW_WIDTH/2 - 50, WINDOW_HEIGHT/2 + 110,
                                              100, 30, "Continue", on_continue_click, data);
        if (data->continue_btn) {
            data->continue_btn->bg_color = (Color){64, 128, 64, 255};
            ui_add_child(data->result_dialog, data->continue_btn);
        }
    }

    /* Pause menu */
    data->pause_menu = ui_create_panel(g_ui, WINDOW_WIDTH/2 - 100, WINDOW_HEIGHT/2 - 75,
                                       200, 150);
    if (data->pause_menu) {
        data->pause_menu->bg_color = (Color){40, 40, 56, 240};
        data->pause_menu->visible = false;

        data->resume_btn = ui_create_button(g_ui, WINDOW_WIDTH/2 - 80, WINDOW_HEIGHT/2 - 50,
                                            160, 35, "Resume", on_resume_click, data);
        if (data->resume_btn) ui_add_child(data->pause_menu, data->resume_btn);

        data->quit_btn = ui_create_button(g_ui, WINDOW_WIDTH/2 - 80, WINDOW_HEIGHT/2 + 10,
                                          160, 35, "Quit Dungeon", on_quit_click, data);
        if (data->quit_btn) {
            data->quit_btn->bg_color = (Color){128, 64, 64, 255};
            ui_add_child(data->pause_menu, data->quit_btn);
        }
    }

    /* Start first room */
    spawn_monsters(data, 0);
    data->state = DUNGEON_STATE_PLAYING;

    /* Notify server */
    if (g_network) {
        PacketBuilder* builder = network_begin_packet(g_network, MSG_DUNGEON_ENTER);
        packet_write_u32(builder, 1);  /* Dungeon ID */
        packet_write_u8(builder, data->info.difficulty);
        network_end_packet(g_network, builder);
    }

    printf("Dungeon scene initialized (rooms=%d, difficulty=%d)\n",
           data->room_count, data->info.difficulty);
    return CLIENT_OK;
}

static void dungeon_cleanup(Scene* scene)
{
    DungeonData* data;

    if (!scene) return;

    data = (DungeonData*)scene->data;
    if (data) {
        free(data);
        scene->data = NULL;
    }
}

static void dungeon_update(Scene* scene, float delta)
{
    DungeonData* data;
    int i;

    if (!scene) return;

    data = (DungeonData*)scene->data;
    if (!data) return;

    /* Pause check */
    if (data->show_pause || data->show_result) return;

    /* Update time */
    data->elapsed_time += delta;

    /* Cooldowns */
    if (data->attack_cooldown > 0) {
        data->attack_cooldown -= delta;
    }
    if (data->combo_timer > 0) {
        data->combo_timer -= delta;
        if (data->combo_timer <= 0) {
            data->combo_count = 0;
        }
    }
    if (data->camera_shake > 0) {
        data->camera_shake -= delta;
    }

    /* Attack animation */
    if (data->attacking && data->attack_cooldown <= SKILL_COOLDOWN - 0.1f) {
        data->attacking = false;
    }

    /* Monster AI */
    if (data->state == DUNGEON_STATE_PLAYING || data->state == DUNGEON_STATE_BOSS) {
        monster_ai_update(data, delta);
    }

    /* Check room clear */
    if (data->monsters_alive == 0 && !data->room_cleared) {
        data->room_cleared = true;
        data->room_transition_timer = 1.0f;
        printf("Room %d cleared!\n", data->current_room + 1);

        if (data->current_room >= data->room_count - 1) {
            /* Dungeon complete */
            DungeonResult result = {0};
            result.exp_gained = 1000 + data->kill_count * 50;
            result.gold_gained = 500 + data->kill_count * 10;
            result.clear_time = (u32)(data->elapsed_time * 1000);
            result.rank = (data->elapsed_time < 60) ? 0 :
                         (data->elapsed_time < 120) ? 1 :
                         (data->elapsed_time < 180) ? 2 : 3;
            result.item_count = 0;

            show_result(data, &result);

            /* Notify server */
            if (g_network) {
                PacketBuilder* builder = network_begin_packet(g_network, MSG_SET_PLAY_RESULT);
                packet_write_u8(builder, result.rank);
                packet_write_u32(builder, result.clear_time);
                network_end_packet(g_network, builder);
            }
        }
    }

    /* Room transition */
    if (data->room_cleared && !data->show_result) {
        data->room_transition_timer -= delta;
        if (data->room_transition_timer <= 0) {
            data->current_room++;
            data->room_cleared = false;
            spawn_monsters(data, data->current_room);
        }
    }

    /* Camera follow */
    float target_cam = data->player_x;
    if (target_cam < WINDOW_WIDTH / 2) target_cam = WINDOW_WIDTH / 2;
    if (target_cam > DUNGEON_WIDTH - WINDOW_WIDTH / 2) {
        target_cam = DUNGEON_WIDTH - WINDOW_WIDTH / 2;
    }
    data->camera_x += (target_cam - data->camera_x) * 5.0f * delta;

    /* Pick up drops */
    for (i = 0; i < data->drop_count; i++) {
        if (data->drops[i].picked) continue;
        float dist = distance(data->player_x, data->player_y,
                             data->drops[i].x, data->drops[i].y);
        if (dist < 30) {
            data->drops[i].picked = true;
            printf("Picked up item %u\n", data->drops[i].item_id);

            if (g_network) {
                PacketBuilder* builder = network_begin_packet(g_network, MSG_GET_ITEM);
                packet_write_u32(builder, data->drops[i].item_id);
                network_end_packet(g_network, builder);
            }
        }
    }

    /* Update HUD */
    update_hud(data);

    /* Update HP/MP display */
    if (data->hp_bar) {
        char buf[32];
        snprintf(buf, sizeof(buf), "HP: %d/%d", data->player.hp, data->player.max_hp);
        ui_set_text(data->hp_bar, buf);
    }
}

static void dungeon_draw(Scene* scene, Renderer* renderer)
{
    DungeonData* data;
    int i;
    float offset_x;

    if (!scene || !renderer) return;

    data = (DungeonData*)scene->data;
    if (!data) return;

    offset_x = WINDOW_WIDTH / 2.0f - data->camera_x;

    /* Camera shake */
    float shake_x = 0, shake_y = 0;
    if (data->camera_shake > 0) {
        shake_x = (float)(rand() % 10 - 5) * data->camera_shake;
        shake_y = (float)(rand() % 10 - 5) * data->camera_shake;
    }

    /* Background */
    renderer_clear(renderer, (Color){32, 32, 48, 255});

    /* Ground */
    Rect ground = {offset_x + shake_x, 420 + shake_y, DUNGEON_WIDTH, 80};
    renderer_draw_rect(renderer, ground, (Color){60, 50, 40, 255}, true);

    /* Room dividers */
    for (i = 1; i < data->room_count; i++) {
        Rect divider = {offset_x + i * ROOM_WIDTH + shake_x, 200 + shake_y, 5, 220};
        renderer_draw_rect(renderer, divider, (Color){80, 80, 80, 255}, true);
    }

    /* Drops */
    for (i = 0; i < data->drop_count; i++) {
        if (data->drops[i].picked) continue;
        Rect drop_rect = {
            offset_x + data->drops[i].x - 10 + shake_x,
            data->drops[i].y - 10 + shake_y,
            20, 20
        };
        renderer_draw_rect(renderer, drop_rect, (Color){255, 200, 50, 255}, true);
    }

    /* Monsters */
    for (i = 0; i < data->monster_count; i++) {
        MonsterData* m = &data->monsters[i];
        if (!m->alive) continue;

        float sx = offset_x + m->x + shake_x;
        float sy = m->y + shake_y;

        /* Body */
        int size = m->is_boss ? 60 : 35;
        Rect body = {sx - size/2, sy - size, (float)size, (float)size};
        Color body_color = m->is_boss ?
            (Color){150, 50, 50, 255} : (Color){150, 100, 100, 255};
        if (m->stun_timer > 0) {
            body_color = (Color){255, 255, 255, 255};
        }
        renderer_draw_rect(renderer, body, body_color, true);

        /* HP bar */
        float hp_pct = (float)m->hp / (float)m->max_hp;
        Rect hp_bg = {sx - 20, sy - size - 10, 40, 5};
        Rect hp_fill = {sx - 20, sy - size - 10, 40 * hp_pct, 5};
        renderer_draw_rect(renderer, hp_bg, (Color){50, 50, 50, 255}, true);
        renderer_draw_rect(renderer, hp_fill, (Color){255, 50, 50, 255}, true);
    }

    /* Player */
    float px = offset_x + data->player_x + shake_x;
    float py = data->player_y + shake_y;

    Rect player_body = {px - 20, py - 50, 40, 50};
    Color player_color = data->attacking ?
        (Color){255, 200, 100, 255} : (Color){100, 150, 200, 255};
    renderer_draw_rect(renderer, player_body, player_color, true);

    /* Attack effect */
    if (data->attacking) {
        Rect attack = {px + 20, py - 40, 60, 30};
        renderer_draw_rect(renderer, attack, (Color){255, 255, 100, 150}, true);
    }

    /* Player HP bar */
    float player_hp_pct = (float)data->player.hp / (float)data->player.max_hp;
    Rect player_hp_bg = {px - 25, py - 60, 50, 6};
    Rect player_hp_fill = {px - 25, py - 60, 50 * player_hp_pct, 6};
    renderer_draw_rect(renderer, player_hp_bg, (Color){50, 50, 50, 255}, true);
    renderer_draw_rect(renderer, player_hp_fill, (Color){50, 200, 50, 255}, true);
}

static void dungeon_handle_input(Scene* scene, SDL_Event* event)
{
    DungeonData* data;

    if (!scene || !event) return;

    data = (DungeonData*)scene->data;
    if (!data) return;

    /* Pause menu input */
    if (data->show_pause) {
        if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_ESCAPE) {
            on_resume_click(NULL, data);
        }
        return;
    }

    /* Result screen - only allow continue */
    if (data->show_result) return;

    switch (event->type) {
        case SDL_KEYDOWN:
            switch (event->key.keysym.sym) {
                case SDLK_LEFT:
                case SDLK_a:
                    data->player_x -= 10;
                    data->player_dir = -1;
                    break;

                case SDLK_RIGHT:
                case SDLK_d:
                    data->player_x += 10;
                    data->player_dir = 1;
                    break;

                case SDLK_x:
                case SDLK_SPACE:
                    player_attack(data);
                    break;

                case SDLK_ESCAPE:
                    data->show_pause = true;
                    if (data->pause_menu) {
                        data->pause_menu->visible = true;
                    }
                    break;
            }
            break;

        case SDL_MOUSEBUTTONDOWN:
            if (event->button.button == SDL_BUTTON_LEFT) {
                player_attack(data);
            }
            break;
    }

    /* Clamp player position */
    if (data->player_x < 50) data->player_x = 50;
    if (data->player_x > DUNGEON_WIDTH - 50) data->player_x = DUNGEON_WIDTH - 50;
}

static void dungeon_on_packet(Scene* scene, u16 msg_no, u8* pkt_data, int len)
{
    DungeonData* data;

    if (!scene) return;

    data = (DungeonData*)scene->data;
    if (!data) return;

    printf("Dungeon scene received packet: %u, len=%d\n", msg_no, len);

    switch (msg_no) {
        case MSG_RESP_MONSTER_SPAWN:
            printf("Monster spawn from server\n");
            break;

        case MSG_RESP_DAMAGE:
            printf("Damage event from server\n");
            break;

        case MSG_RESP_DUNGEON_CLEAR:
            printf("Dungeon clear response\n");
            break;

        case MSG_RESP_DUNGEON_REWARD:
            printf("Dungeon reward received\n");
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

Scene* scene_create_dungeon(void)
{
    Scene* scene = calloc(1, sizeof(Scene));
    if (!scene) return NULL;

    scene->type = SCENE_DUNGEON;
    scene->init = dungeon_init;
    scene->cleanup = dungeon_cleanup;
    scene->update = dungeon_update;
    scene->draw = dungeon_draw;
    scene->handle_input = dungeon_handle_input;
    scene->on_packet = dungeon_on_packet;

    return scene;
}
