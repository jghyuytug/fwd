/**
 * @file scene_charselect.c
 * @brief Character selection scene implementation
 */

#include "../../include/scene.h"
#include "../../include/ui.h"
#include "../../include/network.h"
#include "../../include/protocol_types.h"
#include "../../include/client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Character select scene data */
typedef struct {
    /* UI Elements */
    UIElement* panel;
    UIElement* title_label;
    UIElement* char_slots[MAX_CHARACTERS];
    UIElement* create_btn;
    UIElement* delete_btn;
    UIElement* enter_btn;
    UIElement* back_btn;
    UIElement* status_label;

    /* Create character dialog */
    UIElement* create_dialog;
    UIElement* name_input;
    UIElement* class_buttons[CLASS_COUNT];
    UIElement* confirm_create_btn;
    UIElement* cancel_create_btn;

    /* Character data */
    CharacterInfo characters[MAX_CHARACTERS];
    int char_count;
    int selected_index;
    int selected_class;

    /* State */
    bool show_create_dialog;
    bool waiting_response;

    /* Server info */
    char server_name[64];
} CharSelectData;

/* External references */
extern NetworkClient* get_network_client(void);
extern UIManager* get_ui_manager(void);
extern void change_scene(SceneType type);

static NetworkClient* g_network = NULL;
static UIManager* g_ui = NULL;

/* Class names */
static const char* CLASS_NAMES[CLASS_COUNT] = {
    "Slayer",
    "Fighter",
    "Gunner",
    "Mage",
    "Priest",
    "Thief",
    "Knight",
    "Lancer",
    "Agent"
};

/*
 * ============================================================================
 * UI Callbacks
 * ============================================================================
 */

static void on_char_slot_click(UIElement* element, void* user_data)
{
    CharSelectData* data = (CharSelectData*)user_data;
    int i;

    if (!data) return;

    /* Find which slot was clicked */
    for (i = 0; i < MAX_CHARACTERS; i++) {
        if (data->char_slots[i] == element) {
            if (i < data->char_count) {
                data->selected_index = i;
                printf("Selected character %d: %s\n", i, data->characters[i].name);

                /* Update slot visuals */
                int j;
                for (j = 0; j < data->char_count; j++) {
                    if (data->char_slots[j]) {
                        if (j == data->selected_index) {
                            data->char_slots[j]->border_color = (Color){255, 200, 100, 255};
                        } else {
                            data->char_slots[j]->border_color = (Color){64, 64, 96, 255};
                        }
                    }
                }
            }
            break;
        }
    }
}

static void on_create_click(UIElement* element, void* user_data)
{
    CharSelectData* data = (CharSelectData*)user_data;
    (void)element;

    if (!data) return;

    if (data->char_count >= MAX_CHARACTERS) {
        ui_set_text(data->status_label, "Character slots full!");
        return;
    }

    /* Show create dialog */
    data->show_create_dialog = true;
    data->selected_class = 0;
    if (data->create_dialog) {
        data->create_dialog->visible = true;
    }
    if (data->name_input) {
        ui_set_text(data->name_input, "");
        ui_focus(g_ui, data->name_input);
    }
}

static void on_delete_click(UIElement* element, void* user_data)
{
    CharSelectData* data = (CharSelectData*)user_data;
    (void)element;

    if (!data || !g_network) return;

    if (data->selected_index < 0 || data->selected_index >= data->char_count) {
        ui_set_text(data->status_label, "Select a character first");
        return;
    }

    /* Send delete request */
    {
        PacketBuilder* builder = network_begin_packet(g_network, MSG_DELETE_CHARACTER);
        packet_write_u32(builder, data->characters[data->selected_index].charac_no);
        network_end_packet(g_network, builder);
    }

    data->waiting_response = true;
    ui_set_text(data->status_label, "Deleting character...");
    printf("Delete request sent for character %u\n",
           data->characters[data->selected_index].charac_no);
}

static void on_enter_click(UIElement* element, void* user_data)
{
    CharSelectData* data = (CharSelectData*)user_data;
    (void)element;

    if (!data || !g_network) return;

    if (data->selected_index < 0 || data->selected_index >= data->char_count) {
        ui_set_text(data->status_label, "Select a character first");
        return;
    }

    /* Check if character is deleted */
    if (data->characters[data->selected_index].delete_flag) {
        ui_set_text(data->status_label, "Cannot select deleted character");
        return;
    }

    /* Send select character request */
    {
        PacketBuilder* builder = network_begin_packet(g_network, MSG_SELECT_CHARACTER);
        packet_write_u32(builder, data->characters[data->selected_index].charac_no);
        network_end_packet(g_network, builder);
    }

    data->waiting_response = true;
    ui_set_text(data->status_label, "Entering game...");
    printf("Select request sent for character %u\n",
           data->characters[data->selected_index].charac_no);
}

static void on_back_click(UIElement* element, void* user_data)
{
    CharSelectData* data = (CharSelectData*)user_data;
    (void)element;

    if (!data || !g_network) return;

    /* Send return to select */
    {
        PacketBuilder* builder = network_begin_packet(g_network, MSG_RETURN_SELECT);
        network_end_packet(g_network, builder);
    }

    /* Go back to login */
    change_scene(SCENE_LOGIN);
}

static void on_class_select(UIElement* element, void* user_data)
{
    CharSelectData* data = (CharSelectData*)user_data;
    int i;

    if (!data) return;

    /* Find which class button was clicked */
    for (i = 0; i < CLASS_COUNT; i++) {
        if (data->class_buttons[i] == element) {
            data->selected_class = i;
            printf("Selected class: %s\n", CLASS_NAMES[i]);

            /* Update button visuals */
            int j;
            for (j = 0; j < CLASS_COUNT; j++) {
                if (data->class_buttons[j]) {
                    if (j == data->selected_class) {
                        data->class_buttons[j]->bg_color = (Color){100, 150, 200, 255};
                    } else {
                        data->class_buttons[j]->bg_color = (Color){64, 64, 80, 255};
                    }
                }
            }
            break;
        }
    }
}

static void on_confirm_create(UIElement* element, void* user_data)
{
    CharSelectData* data = (CharSelectData*)user_data;
    const char* name;
    (void)element;

    if (!data || !g_network) return;

    name = ui_get_text(data->name_input);
    if (!name || strlen(name) < 2) {
        ui_set_text(data->status_label, "Name too short (min 2 chars)");
        return;
    }
    if (strlen(name) > 16) {
        ui_set_text(data->status_label, "Name too long (max 16 chars)");
        return;
    }

    /* Send create character request */
    {
        PacketBuilder* builder = network_begin_packet(g_network, MSG_CREATE_CHARACTER);
        packet_write_string(builder, name, 32);
        packet_write_u8(builder, (u8)data->selected_class);
        packet_write_u8(builder, 0);  /* grow_type */
        packet_write_u8(builder, 0);  /* hair_style */
        packet_write_u8(builder, 0);  /* hair_color */
        packet_write_u8(builder, 0);  /* face_style */
        packet_write_u8(builder, 0);  /* skin_color */
        network_end_packet(g_network, builder);
    }

    data->waiting_response = true;
    data->show_create_dialog = false;
    if (data->create_dialog) {
        data->create_dialog->visible = false;
    }
    ui_set_text(data->status_label, "Creating character...");
    printf("Create request sent: name=%s, class=%d\n", name, data->selected_class);
}

static void on_cancel_create(UIElement* element, void* user_data)
{
    CharSelectData* data = (CharSelectData*)user_data;
    (void)element;

    if (!data) return;

    data->show_create_dialog = false;
    if (data->create_dialog) {
        data->create_dialog->visible = false;
    }
}

/*
 * ============================================================================
 * Character Slot Update
 * ============================================================================
 */

static void update_character_slots(CharSelectData* data)
{
    int i;
    float slot_x = WINDOW_WIDTH / 2.0f - 150;
    float slot_y = 120;
    float slot_height = 60;

    for (i = 0; i < MAX_CHARACTERS && i < 8; i++) {
        if (data->char_slots[i]) {
            if (i < data->char_count) {
                /* Show character info */
                char info[128];
                CharacterInfo* ch = &data->characters[i];

                if (ch->delete_flag) {
                    snprintf(info, sizeof(info), "[DELETED] %s", ch->name);
                    data->char_slots[i]->text_color = (Color){128, 128, 128, 255};
                } else {
                    snprintf(info, sizeof(info), "%s  Lv.%d %s",
                             ch->name, ch->level, CLASS_NAMES[ch->job]);
                    data->char_slots[i]->text_color = (Color){255, 255, 255, 255};
                }

                ui_set_text(data->char_slots[i], info);
                data->char_slots[i]->visible = true;
            } else {
                /* Empty slot */
                ui_set_text(data->char_slots[i], "(Empty)");
                data->char_slots[i]->text_color = (Color){100, 100, 100, 255};
                data->char_slots[i]->visible = true;
            }
        }
    }
}

/*
 * ============================================================================
 * Scene Lifecycle
 * ============================================================================
 */

static int charselect_init(Scene* scene)
{
    CharSelectData* data;
    float cx, cy;
    int i;

    if (!scene) return CLIENT_ERROR;

    data = calloc(1, sizeof(CharSelectData));
    if (!data) return CLIENT_ERROR_MEMORY;

    scene->data = data;
    data->selected_index = -1;
    data->selected_class = 0;
    strcpy(data->server_name, "Cain Server");

    g_ui = get_ui_manager();
    g_network = get_network_client();

    if (!g_ui) {
        fprintf(stderr, "UI manager not available\n");
        return CLIENT_ERROR;
    }

    cx = WINDOW_WIDTH / 2.0f;
    cy = WINDOW_HEIGHT / 2.0f;

    /* Main panel */
    data->panel = ui_create_panel(g_ui, cx - 200, 50, 400, 500);
    if (data->panel) {
        data->panel->bg_color = (Color){32, 32, 48, 220};
        data->panel->border_color = (Color){64, 64, 96, 255};
    }

    /* Title */
    data->title_label = ui_create_label(g_ui, cx - 80, 70, "Select Character");
    if (data->title_label) {
        data->title_label->text_color = (Color){255, 255, 255, 255};
        ui_add_child(data->panel, data->title_label);
    }

    /* Character slots (8 visible) */
    for (i = 0; i < 8; i++) {
        data->char_slots[i] = ui_create_button(g_ui,
            cx - 180, 110 + i * 50, 360, 45,
            "(Empty)", on_char_slot_click, data);
        if (data->char_slots[i]) {
            data->char_slots[i]->bg_color = (Color){48, 48, 64, 255};
            data->char_slots[i]->border_color = (Color){64, 64, 96, 255};
            data->char_slots[i]->text_color = (Color){100, 100, 100, 255};
            ui_add_child(data->panel, data->char_slots[i]);
        }
    }

    /* Action buttons */
    data->create_btn = ui_create_button(g_ui, cx - 180, 520, 85, 35,
                                        "Create", on_create_click, data);
    data->delete_btn = ui_create_button(g_ui, cx - 90, 520, 85, 35,
                                        "Delete", on_delete_click, data);
    data->enter_btn = ui_create_button(g_ui, cx + 5, 520, 85, 35,
                                       "Enter", on_enter_click, data);
    data->back_btn = ui_create_button(g_ui, cx + 95, 520, 85, 35,
                                      "Back", on_back_click, data);

    if (data->create_btn) {
        data->create_btn->bg_color = (Color){64, 128, 64, 255};
        ui_add_child(data->panel, data->create_btn);
    }
    if (data->delete_btn) {
        data->delete_btn->bg_color = (Color){128, 64, 64, 255};
        ui_add_child(data->panel, data->delete_btn);
    }
    if (data->enter_btn) {
        data->enter_btn->bg_color = (Color){64, 96, 160, 255};
        ui_add_child(data->panel, data->enter_btn);
    }
    if (data->back_btn) {
        data->back_btn->bg_color = (Color){80, 80, 80, 255};
        ui_add_child(data->panel, data->back_btn);
    }

    /* Status label */
    data->status_label = ui_create_label(g_ui, cx - 100, 560, "");
    if (data->status_label) {
        data->status_label->text_color = (Color){255, 200, 100, 255};
        ui_add_child(data->panel, data->status_label);
    }

    /* Create dialog (initially hidden) */
    data->create_dialog = ui_create_panel(g_ui, cx - 150, cy - 150, 300, 300);
    if (data->create_dialog) {
        data->create_dialog->bg_color = (Color){40, 40, 56, 240};
        data->create_dialog->border_color = (Color){100, 100, 140, 255};
        data->create_dialog->visible = false;

        /* Dialog title */
        UIElement* dlg_title = ui_create_label(g_ui, cx - 60, cy - 130, "Create Character");
        if (dlg_title) {
            dlg_title->text_color = (Color){255, 255, 255, 255};
            ui_add_child(data->create_dialog, dlg_title);
        }

        /* Name input */
        UIElement* name_label = ui_create_label(g_ui, cx - 130, cy - 90, "Name:");
        if (name_label) ui_add_child(data->create_dialog, name_label);

        data->name_input = ui_create_textbox(g_ui, cx - 130, cy - 70, 260, 30, 16);
        if (data->name_input) {
            ui_add_child(data->create_dialog, data->name_input);
        }

        /* Class buttons (3x3 grid) */
        int row, col;
        for (i = 0; i < CLASS_COUNT; i++) {
            row = i / 3;
            col = i % 3;
            data->class_buttons[i] = ui_create_button(g_ui,
                cx - 130 + col * 90, cy - 30 + row * 35,
                85, 30, CLASS_NAMES[i], on_class_select, data);
            if (data->class_buttons[i]) {
                data->class_buttons[i]->bg_color = (Color){64, 64, 80, 255};
                ui_add_child(data->create_dialog, data->class_buttons[i]);
            }
        }
        /* Highlight first class */
        if (data->class_buttons[0]) {
            data->class_buttons[0]->bg_color = (Color){100, 150, 200, 255};
        }

        /* Dialog buttons */
        data->confirm_create_btn = ui_create_button(g_ui, cx - 100, cy + 110, 90, 30,
                                                    "Create", on_confirm_create, data);
        data->cancel_create_btn = ui_create_button(g_ui, cx + 10, cy + 110, 90, 30,
                                                   "Cancel", on_cancel_create, data);
        if (data->confirm_create_btn) {
            data->confirm_create_btn->bg_color = (Color){64, 128, 64, 255};
            ui_add_child(data->create_dialog, data->confirm_create_btn);
        }
        if (data->cancel_create_btn) {
            data->cancel_create_btn->bg_color = (Color){128, 64, 64, 255};
            ui_add_child(data->create_dialog, data->cancel_create_btn);
        }
    }

    printf("Character select scene initialized\n");
    return CLIENT_OK;
}

static void charselect_cleanup(Scene* scene)
{
    CharSelectData* data;

    if (!scene) return;

    data = (CharSelectData*)scene->data;
    if (data) {
        /* UI elements are cleaned up by UI manager */
        free(data);
        scene->data = NULL;
    }
}

static void charselect_update(Scene* scene, float delta)
{
    (void)scene;
    (void)delta;
}

static void charselect_draw(Scene* scene, Renderer* renderer)
{
    (void)scene;
    (void)renderer;
    /* UI is drawn by UI manager */
}

static void charselect_handle_input(Scene* scene, SDL_Event* event)
{
    CharSelectData* data;

    if (!scene || !event) return;

    data = (CharSelectData*)scene->data;
    if (!data) return;

    /* Handle Enter key to enter game */
    if (event->type == SDL_KEYDOWN) {
        if (event->key.keysym.sym == SDLK_RETURN) {
            if (data->show_create_dialog) {
                on_confirm_create(NULL, data);
            } else if (data->selected_index >= 0) {
                on_enter_click(NULL, data);
            }
        } else if (event->key.keysym.sym == SDLK_ESCAPE) {
            if (data->show_create_dialog) {
                on_cancel_create(NULL, data);
            }
        }
    }
}

static void charselect_on_packet(Scene* scene, u16 msg_no, u8* pkt_data, int len)
{
    CharSelectData* data;

    if (!scene) return;

    data = (CharSelectData*)scene->data;
    if (!data) return;

    printf("CharSelect received packet: %u, len=%d\n", msg_no, len);

    switch (msg_no) {
        case MSG_RESP_CHARLIST:
            /* Parse character list */
            if (len >= 1) {
                int i;
                int count = pkt_data[0];
                int offset = 1;

                data->char_count = count;
                if (data->char_count > MAX_CHARACTERS) {
                    data->char_count = MAX_CHARACTERS;
                }

                printf("Received %d characters\n", data->char_count);

                /* Simple parsing (should use packet_parser) */
                for (i = 0; i < data->char_count && offset < len; i++) {
                    /* This is simplified - real parsing should use packet_parser.c */
                    CharacterInfo* ch = &data->characters[i];
                    memset(ch, 0, sizeof(CharacterInfo));

                    if (offset + 4 <= len) {
                        ch->charac_no = pkt_data[offset] | (pkt_data[offset+1] << 8) |
                                       (pkt_data[offset+2] << 16) | (pkt_data[offset+3] << 24);
                        offset += 4;
                    }
                    /* Skip rest for now - would need full parser */
                    snprintf(ch->name, MAX_NAME_LENGTH, "Character%d", i + 1);
                    ch->level = 1;
                    ch->job = i % CLASS_COUNT;
                }

                update_character_slots(data);
                data->waiting_response = false;
                ui_set_text(data->status_label, "");
            }
            break;

        case MSG_RESP_CREATE_CHAR:
            if (len >= 1) {
                u8 result = pkt_data[0];
                if (result == 0) {
                    ui_set_text(data->status_label, "Character created!");
                    /* Request updated character list */
                    if (g_network) {
                        PacketBuilder* builder = network_begin_packet(g_network, MSG_GET_USERINFO);
                        network_end_packet(g_network, builder);
                    }
                } else {
                    ui_set_text(data->status_label, "Failed to create character");
                }
                data->waiting_response = false;
            }
            break;

        case MSG_RESP_DELETE_CHAR:
            if (len >= 1) {
                u8 result = pkt_data[0];
                if (result == 0) {
                    ui_set_text(data->status_label, "Character deleted");
                    data->selected_index = -1;
                    /* Request updated character list */
                    if (g_network) {
                        PacketBuilder* builder = network_begin_packet(g_network, MSG_GET_USERINFO);
                        network_end_packet(g_network, builder);
                    }
                } else {
                    ui_set_text(data->status_label, "Failed to delete character");
                }
                data->waiting_response = false;
            }
            break;

        case MSG_RESP_SELECT_CHAR:
        case MSG_RESP_ENTERWORLD:
            if (len >= 1) {
                u8 result = pkt_data[0];
                if (result == 0) {
                    ui_set_text(data->status_label, "Entering world...");
                    /* Change to town scene */
                    change_scene(SCENE_TOWN);
                } else {
                    ui_set_text(data->status_label, "Failed to enter game");
                }
                data->waiting_response = false;
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

Scene* scene_create_charselect(void)
{
    Scene* scene = calloc(1, sizeof(Scene));
    if (!scene) return NULL;

    scene->type = SCENE_CHARSELECT;
    scene->init = charselect_init;
    scene->cleanup = charselect_cleanup;
    scene->update = charselect_update;
    scene->draw = charselect_draw;
    scene->handle_input = charselect_handle_input;
    scene->on_packet = charselect_on_packet;

    return scene;
}

/*
 * ============================================================================
 * External Character Data Update
 * ============================================================================
 */

void charselect_set_characters(Scene* scene, CharacterInfo* chars, int count)
{
    CharSelectData* data;

    if (!scene || scene->type != SCENE_CHARSELECT) return;

    data = (CharSelectData*)scene->data;
    if (!data) return;

    data->char_count = count;
    if (data->char_count > MAX_CHARACTERS) {
        data->char_count = MAX_CHARACTERS;
    }

    memcpy(data->characters, chars, data->char_count * sizeof(CharacterInfo));
    update_character_slots(data);
}
