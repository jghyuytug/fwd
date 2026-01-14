/**
 * @file client.h
 * @brief Game Client main header
 *
 * 2D side-scrolling action game client using SDL2.
 */

#ifndef CLIENT_H
#define CLIENT_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Version */
#define CLIENT_VERSION_MAJOR 1
#define CLIENT_VERSION_MINOR 0
#define CLIENT_VERSION_PATCH 0
#define CLIENT_VERSION_STRING "1.0.0"

/* Window settings */
#define WINDOW_TITLE    "DNF Client"
#define WINDOW_WIDTH    1024
#define WINDOW_HEIGHT   768
#define TARGET_FPS      60
#define FRAME_DELAY     (1000 / TARGET_FPS)

/* Game settings */
#define TILE_SIZE       32
#define GRAVITY         0.5f
#define MAX_ENTITIES    256
#define MAX_EFFECTS     128
#define MAX_UI_ELEMENTS 64

/* Type definitions */
typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;
typedef int8_t      i8;
typedef int16_t     i16;
typedef int32_t     i32;
typedef int64_t     i64;

/* Error codes */
#define CLIENT_OK           0
#define CLIENT_ERROR       -1
#define CLIENT_ERROR_SDL   -2
#define CLIENT_ERROR_NET   -3
#define CLIENT_ERROR_IO    -4
#define CLIENT_ERROR_MEMORY -5

/* Forward declarations */
typedef struct GameClient GameClient;
typedef struct Renderer Renderer;
typedef struct NetworkClient NetworkClient;
typedef struct Scene Scene;
typedef struct Entity Entity;
typedef struct Sprite Sprite;
typedef struct Animation Animation;

/* Vector2 */
typedef struct {
    float x;
    float y;
} Vec2;

/* Rectangle */
typedef struct {
    float x;
    float y;
    float w;
    float h;
} Rect;

/* Color */
typedef struct {
    u8 r;
    u8 g;
    u8 b;
    u8 a;
} Color;

/* Input state (legacy) */
typedef struct {
    bool keys[SDL_NUM_SCANCODES];
    bool keys_pressed[SDL_NUM_SCANCODES];
    bool keys_released[SDL_NUM_SCANCODES];
    int mouse_x;
    int mouse_y;
    bool mouse_buttons[5];
    bool mouse_pressed[5];
} InputState;

/* Input actions */
typedef enum {
    /* Movement */
    ACTION_MOVE_UP = 0,
    ACTION_MOVE_DOWN,
    ACTION_MOVE_LEFT,
    ACTION_MOVE_RIGHT,
    ACTION_JUMP,

    /* Combat */
    ACTION_ATTACK,
    ACTION_SKILL_1,
    ACTION_SKILL_2,
    ACTION_SKILL_3,
    ACTION_SKILL_4,
    ACTION_DASH,

    /* UI */
    ACTION_INVENTORY,
    ACTION_SKILLS,
    ACTION_CHARACTER,
    ACTION_QUEST,
    ACTION_MAP,
    ACTION_MENU,
    ACTION_CHAT,

    /* Interaction */
    ACTION_INTERACT,
    ACTION_PICKUP,

    /* Quick items */
    ACTION_ITEM_1,
    ACTION_ITEM_2,
    ACTION_ITEM_3,
    ACTION_ITEM_4,

    ACTION_COUNT
} InputAction;

/* Game state */
typedef enum {
    GAME_STATE_INIT = 0,
    GAME_STATE_LOGIN,
    GAME_STATE_CHARSELECT,
    GAME_STATE_LOADING,
    GAME_STATE_TOWN,
    GAME_STATE_DUNGEON,
    GAME_STATE_MENU,
    GAME_STATE_QUIT
} GameState;

/**
 * Initialize game client
 * @return CLIENT_OK on success
 */
int client_init(void);

/**
 * Run game client main loop
 * @return Exit code
 */
int client_run(void);

/**
 * Shutdown game client
 */
void client_shutdown(void);

/**
 * Get game client instance
 * @return Client instance
 */
GameClient* client_get(void);

/**
 * Get current game state
 * @return Current state
 */
GameState client_get_state(void);

/**
 * Set game state
 * @param state New state
 */
void client_set_state(GameState state);

/**
 * Get delta time (seconds)
 * @return Delta time
 */
float client_get_delta(void);

/**
 * Get input state
 * @return Input state pointer
 */
InputState* client_get_input(void);

/*
 * ============================================================================
 * Input Manager API
 * ============================================================================
 */

/**
 * Initialize input system
 */
void input_init(void);

/**
 * Shutdown input system
 */
void input_shutdown(void);

/**
 * Update input state (call once per frame)
 */
void input_update(void);

/**
 * Handle SDL event
 */
void input_handle_event(SDL_Event* event);

/**
 * Check if action is currently pressed
 */
bool input_action_pressed(int action_id);

/**
 * Check if action was just pressed this frame
 */
bool input_action_just_pressed(int action_id);

/**
 * Check if action was just released this frame
 */
bool input_action_just_released(int action_id);

/**
 * Check if action is being held
 */
bool input_action_held(int action_id);

/**
 * Get movement vector from input (-1 to 1)
 */
void input_get_movement_vector(float* x, float* y);

/**
 * Get mouse position
 */
void input_get_mouse_position(int* x, int* y);

/**
 * Check if mouse button is pressed
 */
bool input_mouse_button_pressed(int button);

/**
 * Check if mouse button was just pressed
 */
bool input_mouse_button_just_pressed(int button);

/**
 * Get mouse wheel delta
 */
int input_get_mouse_wheel(void);

/**
 * Check if gamepad is connected
 */
bool input_has_gamepad(void);

/**
 * Get gamepad left stick values
 */
void input_get_left_stick(float* x, float* y);

/**
 * Start text input mode
 */
void input_start_text_input(void);

/**
 * Stop text input mode
 */
void input_stop_text_input(void);

/**
 * Get text input buffer
 */
const char* input_get_text_input(void);

/**
 * Clear text input buffer
 */
void input_clear_text_input(void);

/**
 * Normalize gamepad axis value with deadzone
 */
float input_normalize_axis(int16_t value);

/**
 * Trigger gamepad vibration
 */
void input_vibrate(float low_frequency, float high_frequency, uint32_t duration_ms);

#ifdef __cplusplus
}
#endif

#endif /* CLIENT_H */
