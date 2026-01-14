/**
 * @file client.c
 * @brief Game client main implementation
 */

#include "../../include/client.h"
#include "../../include/renderer.h"
#include "../../include/network.h"
#include "../../include/entity.h"
#include "../../include/scene.h"
#include "../../include/ui.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Global client instance */
static struct {
    Renderer        renderer;
    NetworkClient   network;
    EntityManager   entities;
    SceneManager    scenes;
    UIManager       ui;
    InputState      input;

    GameState       state;
    bool            running;
    u32             last_time;
    float           delta_time;
    u32             frame_count;
    float           fps;
    float           fps_timer;
} g_client;

/* Initialize SDL */
static int init_sdl(void)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return CLIENT_ERROR_SDL;
    }

    /* Initialize SDL_image */
    int img_flags = IMG_INIT_PNG | IMG_INIT_JPG;
    if ((IMG_Init(img_flags) & img_flags) != img_flags) {
        fprintf(stderr, "IMG_Init failed: %s\n", IMG_GetError());
        return CLIENT_ERROR_SDL;
    }

    /* Initialize SDL_ttf */
    if (TTF_Init() < 0) {
        fprintf(stderr, "TTF_Init failed: %s\n", TTF_GetError());
        return CLIENT_ERROR_SDL;
    }

    /* Initialize SDL_mixer */
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        fprintf(stderr, "Mix_OpenAudio failed: %s\n", Mix_GetError());
        /* Continue without audio */
    }

    return CLIENT_OK;
}

/* Cleanup SDL */
static void cleanup_sdl(void)
{
    Mix_CloseAudio();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

/* Update input state */
static void update_input(void)
{
    int i;

    /* Clear pressed/released states */
    memset(g_client.input.keys_pressed, 0, sizeof(g_client.input.keys_pressed));
    memset(g_client.input.keys_released, 0, sizeof(g_client.input.keys_released));
    memset(g_client.input.mouse_pressed, 0, sizeof(g_client.input.mouse_pressed));

    /* Get keyboard state */
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    for (i = 0; i < SDL_NUM_SCANCODES; i++) {
        bool was_down = g_client.input.keys[i];
        bool is_down = keys[i] != 0;

        if (is_down && !was_down) {
            g_client.input.keys_pressed[i] = true;
        } else if (!is_down && was_down) {
            g_client.input.keys_released[i] = true;
        }

        g_client.input.keys[i] = is_down;
    }

    /* Get mouse state */
    Uint32 buttons = SDL_GetMouseState(&g_client.input.mouse_x, &g_client.input.mouse_y);
    for (i = 0; i < 5; i++) {
        bool was_down = g_client.input.mouse_buttons[i];
        bool is_down = (buttons & SDL_BUTTON(i + 1)) != 0;

        if (is_down && !was_down) {
            g_client.input.mouse_pressed[i] = true;
        }

        g_client.input.mouse_buttons[i] = is_down;
    }
}

/* Handle events */
static void handle_events(void)
{
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                g_client.running = false;
                break;

            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    /* Toggle menu or quit */
                    if (g_client.state == GAME_STATE_MENU) {
                        g_client.state = GAME_STATE_TOWN;
                    } else if (g_client.state == GAME_STATE_TOWN ||
                               g_client.state == GAME_STATE_DUNGEON) {
                        g_client.state = GAME_STATE_MENU;
                    }
                }
                break;

            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    /* Handle window resize */
                }
                break;
        }

        /* Pass to UI */
        if (ui_manager_handle_event(&g_client.ui, &event)) {
            continue;
        }

        /* Pass to scene */
        scene_manager_handle_input(&g_client.scenes, &event);
    }
}

/* Network packet callback */
static void on_packet(NetworkClient* client, u16 msg_no, u8* data, int len)
{
    (void)client;

    /* Pass to scene */
    scene_manager_on_packet(&g_client.scenes, msg_no, data, len);
}

int client_init(void)
{
    int ret;

    printf("=== Game Client %s ===\n", CLIENT_VERSION_STRING);

    memset(&g_client, 0, sizeof(g_client));

    /* Initialize SDL */
    ret = init_sdl();
    if (ret != CLIENT_OK) {
        return ret;
    }

    /* Initialize renderer */
    ret = renderer_init(&g_client.renderer, WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT);
    if (ret != CLIENT_OK) {
        cleanup_sdl();
        return ret;
    }

    /* Initialize network */
    ret = network_init(&g_client.network);
    if (ret != CLIENT_OK) {
        renderer_cleanup(&g_client.renderer);
        cleanup_sdl();
        return ret;
    }
    g_client.network.on_packet = on_packet;

    /* Initialize entity manager */
    ret = entity_manager_init(&g_client.entities);
    if (ret != CLIENT_OK) {
        network_cleanup(&g_client.network);
        renderer_cleanup(&g_client.renderer);
        cleanup_sdl();
        return ret;
    }

    /* Initialize scene manager */
    ret = scene_manager_init(&g_client.scenes);
    if (ret != CLIENT_OK) {
        entity_manager_cleanup(&g_client.entities);
        network_cleanup(&g_client.network);
        renderer_cleanup(&g_client.renderer);
        cleanup_sdl();
        return ret;
    }

    /* Initialize UI manager */
    ret = ui_manager_init(&g_client.ui);
    if (ret != CLIENT_OK) {
        scene_manager_cleanup(&g_client.scenes);
        entity_manager_cleanup(&g_client.entities);
        network_cleanup(&g_client.network);
        renderer_cleanup(&g_client.renderer);
        cleanup_sdl();
        return ret;
    }

    /* Load default font */
    ui_load_font(&g_client.ui, "assets/fonts/default.ttf", 16);

    g_client.state = GAME_STATE_INIT;
    g_client.running = true;
    g_client.last_time = SDL_GetTicks();

    printf("Client initialized successfully\n");

    return CLIENT_OK;
}

void client_shutdown(void)
{
    printf("Shutting down client...\n");

    ui_manager_cleanup(&g_client.ui);
    scene_manager_cleanup(&g_client.scenes);
    entity_manager_cleanup(&g_client.entities);
    network_cleanup(&g_client.network);
    renderer_cleanup(&g_client.renderer);
    cleanup_sdl();

    printf("Client shutdown complete\n");
}

int client_run(void)
{
    u32 current_time;
    u32 frame_start;
    u32 frame_time;

    /* Start with login scene */
    scene_manager_change(&g_client.scenes, SCENE_LOGIN);
    g_client.state = GAME_STATE_LOGIN;

    /* Main loop */
    while (g_client.running) {
        frame_start = SDL_GetTicks();

        /* Calculate delta time */
        current_time = SDL_GetTicks();
        g_client.delta_time = (current_time - g_client.last_time) / 1000.0f;
        g_client.last_time = current_time;

        /* Cap delta time to prevent spiral of death */
        if (g_client.delta_time > 0.1f) {
            g_client.delta_time = 0.1f;
        }

        /* Update FPS counter */
        g_client.frame_count++;
        g_client.fps_timer += g_client.delta_time;
        if (g_client.fps_timer >= 1.0f) {
            g_client.fps = g_client.frame_count / g_client.fps_timer;
            g_client.frame_count = 0;
            g_client.fps_timer = 0;
        }

        /* Handle events */
        handle_events();

        /* Update input */
        update_input();

        /* Update network */
        network_update(&g_client.network);

        /* Update UI */
        ui_manager_update(&g_client.ui, &g_client.input);

        /* Update scene */
        scene_manager_update(&g_client.scenes, g_client.delta_time);

        /* Update entities */
        entity_manager_update(&g_client.entities, g_client.delta_time);

        /* Render */
        renderer_begin(&g_client.renderer);

        /* Clear screen */
        Color bg = {32, 32, 48, 255};
        renderer_clear(&g_client.renderer, bg);

        /* Draw scene */
        scene_manager_draw(&g_client.scenes, &g_client.renderer);

        /* Draw entities */
        entity_manager_draw(&g_client.entities, &g_client.renderer);

        /* Draw UI */
        ui_manager_draw(&g_client.ui, &g_client.renderer);

        renderer_end(&g_client.renderer);

        /* Frame rate limiting */
        frame_time = SDL_GetTicks() - frame_start;
        if (frame_time < FRAME_DELAY) {
            SDL_Delay(FRAME_DELAY - frame_time);
        }
    }

    return CLIENT_OK;
}

GameClient* client_get(void)
{
    return (GameClient*)&g_client;
}

GameState client_get_state(void)
{
    return g_client.state;
}

void client_set_state(GameState state)
{
    g_client.state = state;
}

float client_get_delta(void)
{
    return g_client.delta_time;
}

InputState* client_get_input(void)
{
    return &g_client.input;
}

/* Main entry point */
int main(int argc, char* argv[])
{
    int ret;

    (void)argc;
    (void)argv;

    ret = client_init();
    if (ret != CLIENT_OK) {
        fprintf(stderr, "Failed to initialize client\n");
        return 1;
    }

    ret = client_run();

    client_shutdown();

    return ret == CLIENT_OK ? 0 : 1;
}
