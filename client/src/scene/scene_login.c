/**
 * @file scene_login.c
 * @brief Login scene implementation
 */

#include "../../include/scene.h"
#include "../../include/ui.h"
#include "../../include/network.h"
#include "../../include/client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Login scene data */
typedef struct {
    UIElement* panel;
    UIElement* username_box;
    UIElement* password_box;
    UIElement* login_btn;
    UIElement* status_label;
    UIElement* server_label;
    bool connecting;
    char server_host[64];
    u16 server_port;
} LoginSceneData;

/* External references */
extern NetworkClient* get_network_client(void);
extern UIManager* get_ui_manager(void);
extern void change_scene(SceneType type);

static NetworkClient* g_network = NULL;
static UIManager* g_ui = NULL;

static void on_login_click(UIElement* element, void* user_data)
{
    LoginSceneData* data = (LoginSceneData*)user_data;
    const char* username;
    const char* password;

    (void)element;

    if (!data || !g_network) return;

    username = ui_get_text(data->username_box);
    password = ui_get_text(data->password_box);

    if (!username || strlen(username) == 0) {
        ui_set_text(data->status_label, "Please enter username");
        return;
    }

    if (!password || strlen(password) == 0) {
        ui_set_text(data->status_label, "Please enter password");
        return;
    }

    /* Connect to server if not connected */
    if (!network_is_connected(g_network)) {
        ui_set_text(data->status_label, "Connecting to server...");
        data->connecting = true;

        if (network_connect(g_network, data->server_host, data->server_port, 5000) != CLIENT_OK) {
            ui_set_text(data->status_label, "Connection failed!");
            data->connecting = false;
            return;
        }
    }

    /* Send login packet */
    ui_set_text(data->status_label, "Logging in...");

    {
        PacketBuilder* builder = network_begin_packet(g_network, MSG_LOGIN);
        packet_write_string(builder, username, 32);
        packet_write_string(builder, password, 32);
        packet_write_u32(builder, 0);  /* Version */
        network_end_packet(g_network, builder);
    }

    printf("Login sent: %s\n", username);
}

static int login_init(Scene* scene)
{
    LoginSceneData* data;
    float cx, cy;

    if (!scene) return CLIENT_ERROR;

    data = calloc(1, sizeof(LoginSceneData));
    if (!data) return CLIENT_ERROR_MEMORY;

    scene->data = data;

    /* Default server */
    strcpy(data->server_host, "127.0.0.1");
    data->server_port = 7000;  /* Bridge port */

    /* Get UI manager */
    /* In real implementation, this would come from client context */
    g_ui = get_ui_manager();
    g_network = get_network_client();

    if (!g_ui) {
        fprintf(stderr, "UI manager not available\n");
        return CLIENT_ERROR;
    }

    cx = WINDOW_WIDTH / 2.0f;
    cy = WINDOW_HEIGHT / 2.0f;

    /* Create login panel */
    data->panel = ui_create_panel(g_ui, cx - 150, cy - 120, 300, 240);
    if (data->panel) {
        data->panel->bg_color = (Color){32, 32, 48, 220};
        data->panel->border_color = (Color){64, 64, 96, 255};
    }

    /* Title label */
    {
        UIElement* title = ui_create_label(g_ui, cx - 60, cy - 100, "DNF Login");
        if (title) {
            title->text_color = (Color){255, 255, 255, 255};
            ui_add_child(data->panel, title);
        }
    }

    /* Server label */
    {
        char server_str[128];
        snprintf(server_str, sizeof(server_str), "Server: %s:%u",
                data->server_host, data->server_port);
        data->server_label = ui_create_label(g_ui, cx - 80, cy - 70, server_str);
        if (data->server_label) {
            data->server_label->text_color = (Color){160, 160, 160, 255};
            ui_add_child(data->panel, data->server_label);
        }
    }

    /* Username label and textbox */
    {
        UIElement* label = ui_create_label(g_ui, cx - 120, cy - 40, "Username:");
        if (label) ui_add_child(data->panel, label);

        data->username_box = ui_create_textbox(g_ui, cx - 120, cy - 20, 240, 30, 32);
        if (data->username_box) {
            ui_add_child(data->panel, data->username_box);
        }
    }

    /* Password label and textbox */
    {
        UIElement* label = ui_create_label(g_ui, cx - 120, cy + 20, "Password:");
        if (label) ui_add_child(data->panel, label);

        data->password_box = ui_create_textbox(g_ui, cx - 120, cy + 40, 240, 30, 32);
        if (data->password_box) {
            data->password_box->password = true;
            ui_add_child(data->panel, data->password_box);
        }
    }

    /* Login button */
    data->login_btn = ui_create_button(g_ui, cx - 60, cy + 80, 120, 35,
                                       "Login", on_login_click, data);
    if (data->login_btn) {
        data->login_btn->bg_color = (Color){64, 96, 160, 255};
        ui_add_child(data->panel, data->login_btn);
    }

    /* Status label */
    data->status_label = ui_create_label(g_ui, cx - 100, cy + 130, "");
    if (data->status_label) {
        data->status_label->text_color = (Color){255, 200, 100, 255};
        ui_add_child(data->panel, data->status_label);
    }

    printf("Login scene initialized\n");

    return CLIENT_OK;
}

static void login_cleanup(Scene* scene)
{
    LoginSceneData* data;

    if (!scene) return;

    data = (LoginSceneData*)scene->data;
    if (data) {
        /* UI elements are cleaned up by UI manager */
        free(data);
        scene->data = NULL;
    }
}

static void login_update(Scene* scene, float delta)
{
    (void)scene;
    (void)delta;
    /* Nothing to update */
}

static void login_draw(Scene* scene, Renderer* renderer)
{
    (void)scene;
    (void)renderer;
    /* UI is drawn by UI manager */
}

static void login_handle_input(Scene* scene, SDL_Event* event)
{
    LoginSceneData* data;

    if (!scene || !event) return;

    data = (LoginSceneData*)scene->data;
    if (!data) return;

    /* Handle Enter key for login */
    if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_RETURN) {
        on_login_click(NULL, data);
    }
}

static void login_on_packet(Scene* scene, u16 msg_no, u8* pkt_data, int len)
{
    LoginSceneData* data;

    if (!scene) return;

    data = (LoginSceneData*)scene->data;
    if (!data) return;

    printf("Login scene received packet: %u, len=%d\n", msg_no, len);

    if (msg_no == MSG_RESP_LOGIN) {
        /* Parse login response */
        if (len >= 4) {
            u32 result = pkt_data[0] | (pkt_data[1] << 8) |
                        (pkt_data[2] << 16) | (pkt_data[3] << 24);

            if (result == 0) {
                ui_set_text(data->status_label, "Login successful!");
                /* Change to character select scene */
                /* change_scene(SCENE_CHARSELECT); */
            } else {
                char msg[64];
                snprintf(msg, sizeof(msg), "Login failed: %u", result);
                ui_set_text(data->status_label, msg);
            }
        }
    }
}

Scene* scene_create_login(void)
{
    Scene* scene = calloc(1, sizeof(Scene));
    if (!scene) return NULL;

    scene->type = SCENE_LOGIN;
    scene->init = login_init;
    scene->cleanup = login_cleanup;
    scene->update = login_update;
    scene->draw = login_draw;
    scene->handle_input = login_handle_input;
    scene->on_packet = login_on_packet;

    return scene;
}

/* Scene creation functions are now in their respective files:
 * - scene_charselect.c
 * - scene_town.c
 * - scene_dungeon.c
 */

/* Helper functions that need to be implemented in client.c */
NetworkClient* get_network_client(void)
{
    /* This should return the actual network client from game client */
    return NULL;
}

UIManager* get_ui_manager(void)
{
    /* This should return the actual UI manager from game client */
    return NULL;
}
