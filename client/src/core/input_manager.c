/**
 * @file input_manager.c
 * @brief Unified input management for keyboard, mouse, and gamepad
 */

#include "../../include/client.h"
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/*
 * ============================================================================
 * Constants
 * ============================================================================
 */

#define MAX_KEY_BINDINGS        64
#define MAX_ACTIONS             32
#define MAX_ACTION_NAME         32
#define DOUBLE_CLICK_TIME       300     /* ms */
#define HOLD_THRESHOLD          200     /* ms */
#define AXIS_DEADZONE           8000    /* Gamepad axis deadzone */

/*
 * ============================================================================
 * Input Types
 * ============================================================================
 */

typedef enum {
    INPUT_SOURCE_KEYBOARD,
    INPUT_SOURCE_MOUSE,
    INPUT_SOURCE_GAMEPAD
} InputSource;

typedef enum {
    INPUT_STATE_RELEASED,
    INPUT_STATE_PRESSED,
    INPUT_STATE_HELD,
    INPUT_STATE_JUST_PRESSED,
    INPUT_STATE_JUST_RELEASED
} InputState;

typedef struct {
    char            name[MAX_ACTION_NAME];
    int             action_id;

    /* Primary binding */
    InputSource     source;
    int             key_code;           /* SDL_Scancode or mouse button */

    /* Secondary binding */
    InputSource     alt_source;
    int             alt_key_code;

    /* Gamepad binding */
    int             gamepad_button;     /* SDL_GameControllerButton or -1 */
    int             gamepad_axis;       /* Axis + direction or -1 */
    int             axis_positive;      /* true for positive axis direction */

    bool            active;
} KeyBinding;

typedef struct {
    InputState      state;
    InputState      prev_state;
    uint32_t        press_time;
    uint32_t        release_time;
    int             click_count;        /* For double-click detection */
} ActionState;

typedef struct {
    /* Keyboard state */
    const uint8_t*  keyboard_state;
    uint8_t         prev_keyboard[SDL_NUM_SCANCODES];

    /* Mouse state */
    int             mouse_x;
    int             mouse_y;
    int             mouse_dx;
    int             mouse_dy;
    int             mouse_wheel;
    uint32_t        mouse_buttons;
    uint32_t        prev_mouse_buttons;
    uint32_t        mouse_click_time[5];
    int             mouse_click_count[5];

    /* Gamepad state */
    SDL_GameController* gamepad;
    int             gamepad_index;
    int16_t         axis_values[SDL_CONTROLLER_AXIS_MAX];
    uint32_t        gamepad_buttons;
    uint32_t        prev_gamepad_buttons;
    float           left_stick_x;
    float           left_stick_y;
    float           right_stick_x;
    float           right_stick_y;
    float           left_trigger;
    float           right_trigger;

    /* Key bindings */
    KeyBinding      bindings[MAX_KEY_BINDINGS];
    int             binding_count;

    /* Action states */
    ActionState     actions[MAX_ACTIONS];

    /* Input capture */
    bool            text_input_active;
    char            text_buffer[256];
    int             text_cursor;

    /* Current frame timestamp */
    uint32_t        current_time;

    bool            initialized;
} InputManager;

static InputManager g_input = {0};

/*
 * ============================================================================
 * Default Key Bindings
 * ============================================================================
 */

typedef struct {
    const char*     name;
    int             action_id;
    SDL_Scancode    key;
    SDL_Scancode    alt_key;
    int             gamepad_button;
} DefaultBinding;

static const DefaultBinding DEFAULT_BINDINGS[] = {
    /* Movement */
    {"move_up",     ACTION_MOVE_UP,     SDL_SCANCODE_W,      SDL_SCANCODE_UP,    SDL_CONTROLLER_BUTTON_DPAD_UP},
    {"move_down",   ACTION_MOVE_DOWN,   SDL_SCANCODE_S,      SDL_SCANCODE_DOWN,  SDL_CONTROLLER_BUTTON_DPAD_DOWN},
    {"move_left",   ACTION_MOVE_LEFT,   SDL_SCANCODE_A,      SDL_SCANCODE_LEFT,  SDL_CONTROLLER_BUTTON_DPAD_LEFT},
    {"move_right",  ACTION_MOVE_RIGHT,  SDL_SCANCODE_D,      SDL_SCANCODE_RIGHT, SDL_CONTROLLER_BUTTON_DPAD_RIGHT},
    {"jump",        ACTION_JUMP,        SDL_SCANCODE_SPACE,  SDL_SCANCODE_C,     SDL_CONTROLLER_BUTTON_A},

    /* Combat */
    {"attack",      ACTION_ATTACK,      SDL_SCANCODE_X,      SDL_SCANCODE_J,     SDL_CONTROLLER_BUTTON_X},
    {"skill_1",     ACTION_SKILL_1,     SDL_SCANCODE_Z,      SDL_SCANCODE_U,     SDL_CONTROLLER_BUTTON_Y},
    {"skill_2",     ACTION_SKILL_2,     SDL_SCANCODE_A,      SDL_SCANCODE_I,     SDL_CONTROLLER_BUTTON_B},
    {"skill_3",     ACTION_SKILL_3,     SDL_SCANCODE_S,      SDL_SCANCODE_O,     SDL_CONTROLLER_BUTTON_RIGHTSHOULDER},
    {"skill_4",     ACTION_SKILL_4,     SDL_SCANCODE_D,      SDL_SCANCODE_P,     SDL_CONTROLLER_BUTTON_LEFTSHOULDER},
    {"dash",        ACTION_DASH,        SDL_SCANCODE_LSHIFT, SDL_SCANCODE_K,     SDL_CONTROLLER_BUTTON_LEFTSTICK},

    /* UI */
    {"inventory",   ACTION_INVENTORY,   SDL_SCANCODE_I,      SDL_SCANCODE_B,     SDL_CONTROLLER_BUTTON_BACK},
    {"skills",      ACTION_SKILLS,      SDL_SCANCODE_K,      0,                  -1},
    {"character",   ACTION_CHARACTER,   SDL_SCANCODE_P,      0,                  -1},
    {"quest",       ACTION_QUEST,       SDL_SCANCODE_Q,      SDL_SCANCODE_L,     -1},
    {"map",         ACTION_MAP,         SDL_SCANCODE_M,      SDL_SCANCODE_TAB,   -1},
    {"menu",        ACTION_MENU,        SDL_SCANCODE_ESCAPE, 0,                  SDL_CONTROLLER_BUTTON_START},
    {"chat",        ACTION_CHAT,        SDL_SCANCODE_RETURN, 0,                  -1},

    /* Interaction */
    {"interact",    ACTION_INTERACT,    SDL_SCANCODE_F,      SDL_SCANCODE_E,     SDL_CONTROLLER_BUTTON_A},
    {"pickup",      ACTION_PICKUP,      SDL_SCANCODE_G,      0,                  SDL_CONTROLLER_BUTTON_X},

    /* Quick items */
    {"item_1",      ACTION_ITEM_1,      SDL_SCANCODE_1,      0,                  -1},
    {"item_2",      ACTION_ITEM_2,      SDL_SCANCODE_2,      0,                  -1},
    {"item_3",      ACTION_ITEM_3,      SDL_SCANCODE_3,      0,                  -1},
    {"item_4",      ACTION_ITEM_4,      SDL_SCANCODE_4,      0,                  -1},

    {NULL, 0, 0, 0, -1}
};

/*
 * ============================================================================
 * Initialization
 * ============================================================================
 */

void input_init(void)
{
    memset(&g_input, 0, sizeof(InputManager));

    g_input.keyboard_state = SDL_GetKeyboardState(NULL);
    g_input.gamepad_index = -1;

    /* Initialize default bindings */
    for (int i = 0; DEFAULT_BINDINGS[i].name != NULL; i++) {
        const DefaultBinding* def = &DEFAULT_BINDINGS[i];
        KeyBinding* bind = &g_input.bindings[g_input.binding_count++];

        strncpy(bind->name, def->name, MAX_ACTION_NAME - 1);
        bind->action_id = def->action_id;
        bind->source = INPUT_SOURCE_KEYBOARD;
        bind->key_code = def->key;
        bind->alt_source = INPUT_SOURCE_KEYBOARD;
        bind->alt_key_code = def->alt_key;
        bind->gamepad_button = def->gamepad_button;
        bind->gamepad_axis = -1;
        bind->active = true;
    }

    /* Try to open first available gamepad */
    for (int i = 0; i < SDL_NumJoysticks(); i++) {
        if (SDL_IsGameController(i)) {
            g_input.gamepad = SDL_GameControllerOpen(i);
            if (g_input.gamepad) {
                g_input.gamepad_index = i;
                printf("[Input] Gamepad connected: %s\n",
                       SDL_GameControllerName(g_input.gamepad));
                break;
            }
        }
    }

    g_input.initialized = true;
}

void input_shutdown(void)
{
    if (g_input.gamepad) {
        SDL_GameControllerClose(g_input.gamepad);
        g_input.gamepad = NULL;
    }
    g_input.initialized = false;
}

/*
 * ============================================================================
 * Gamepad Management
 * ============================================================================
 */

void input_on_gamepad_added(int device_index)
{
    if (g_input.gamepad) return;  /* Already have one */

    if (SDL_IsGameController(device_index)) {
        g_input.gamepad = SDL_GameControllerOpen(device_index);
        if (g_input.gamepad) {
            g_input.gamepad_index = device_index;
            printf("[Input] Gamepad connected: %s\n",
                   SDL_GameControllerName(g_input.gamepad));
        }
    }
}

void input_on_gamepad_removed(int instance_id)
{
    if (g_input.gamepad &&
        SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(g_input.gamepad)) == instance_id) {
        SDL_GameControllerClose(g_input.gamepad);
        g_input.gamepad = NULL;
        g_input.gamepad_index = -1;
        printf("[Input] Gamepad disconnected\n");
    }
}

bool input_has_gamepad(void)
{
    return g_input.gamepad != NULL;
}

/*
 * ============================================================================
 * Update
 * ============================================================================
 */

void input_update(void)
{
    if (!g_input.initialized) return;

    g_input.current_time = SDL_GetTicks();

    /* Store previous keyboard state */
    memcpy(g_input.prev_keyboard, g_input.keyboard_state, SDL_NUM_SCANCODES);

    /* Update mouse */
    g_input.prev_mouse_buttons = g_input.mouse_buttons;
    g_input.mouse_buttons = SDL_GetMouseState(&g_input.mouse_x, &g_input.mouse_y);
    SDL_GetRelativeMouseState(&g_input.mouse_dx, &g_input.mouse_dy);

    /* Update gamepad */
    if (g_input.gamepad) {
        g_input.prev_gamepad_buttons = g_input.gamepad_buttons;
        g_input.gamepad_buttons = 0;

        for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; i++) {
            if (SDL_GameControllerGetButton(g_input.gamepad, i)) {
                g_input.gamepad_buttons |= (1 << i);
            }
        }

        /* Read axes */
        for (int i = 0; i < SDL_CONTROLLER_AXIS_MAX; i++) {
            g_input.axis_values[i] = SDL_GameControllerGetAxis(g_input.gamepad, i);
        }

        /* Normalize stick values with deadzone */
        g_input.left_stick_x = input_normalize_axis(
            g_input.axis_values[SDL_CONTROLLER_AXIS_LEFTX]);
        g_input.left_stick_y = input_normalize_axis(
            g_input.axis_values[SDL_CONTROLLER_AXIS_LEFTY]);
        g_input.right_stick_x = input_normalize_axis(
            g_input.axis_values[SDL_CONTROLLER_AXIS_RIGHTX]);
        g_input.right_stick_y = input_normalize_axis(
            g_input.axis_values[SDL_CONTROLLER_AXIS_RIGHTY]);

        /* Triggers (0-1 range) */
        g_input.left_trigger = g_input.axis_values[SDL_CONTROLLER_AXIS_TRIGGERLEFT] / 32767.0f;
        g_input.right_trigger = g_input.axis_values[SDL_CONTROLLER_AXIS_TRIGGERRIGHT] / 32767.0f;
    }

    /* Update action states */
    for (int i = 0; i < g_input.binding_count; i++) {
        KeyBinding* bind = &g_input.bindings[i];
        if (!bind->active) continue;

        ActionState* action = &g_input.actions[bind->action_id];
        action->prev_state = action->state;

        bool pressed = false;

        /* Check keyboard */
        if (bind->key_code > 0 && g_input.keyboard_state[bind->key_code]) {
            pressed = true;
        }
        if (bind->alt_key_code > 0 && g_input.keyboard_state[bind->alt_key_code]) {
            pressed = true;
        }

        /* Check gamepad button */
        if (g_input.gamepad && bind->gamepad_button >= 0) {
            if (g_input.gamepad_buttons & (1 << bind->gamepad_button)) {
                pressed = true;
            }
        }

        /* Check gamepad axis */
        if (g_input.gamepad && bind->gamepad_axis >= 0) {
            int16_t value = g_input.axis_values[bind->gamepad_axis];
            if (bind->axis_positive && value > AXIS_DEADZONE) {
                pressed = true;
            } else if (!bind->axis_positive && value < -AXIS_DEADZONE) {
                pressed = true;
            }
        }

        /* Update state */
        if (pressed) {
            if (action->prev_state == INPUT_STATE_RELEASED ||
                action->prev_state == INPUT_STATE_JUST_RELEASED) {
                action->state = INPUT_STATE_JUST_PRESSED;
                action->press_time = g_input.current_time;
            } else if (g_input.current_time - action->press_time > HOLD_THRESHOLD) {
                action->state = INPUT_STATE_HELD;
            } else {
                action->state = INPUT_STATE_PRESSED;
            }
        } else {
            if (action->prev_state == INPUT_STATE_PRESSED ||
                action->prev_state == INPUT_STATE_HELD ||
                action->prev_state == INPUT_STATE_JUST_PRESSED) {
                action->state = INPUT_STATE_JUST_RELEASED;
                action->release_time = g_input.current_time;
            } else {
                action->state = INPUT_STATE_RELEASED;
            }
        }
    }

    /* Reset mouse wheel (only valid for one frame) */
    g_input.mouse_wheel = 0;
}

void input_handle_event(SDL_Event* event)
{
    switch (event->type) {
        case SDL_MOUSEWHEEL:
            g_input.mouse_wheel = event->wheel.y;
            break;

        case SDL_MOUSEBUTTONDOWN:
            {
                int btn = event->button.button - 1;
                if (btn >= 0 && btn < 5) {
                    uint32_t now = SDL_GetTicks();
                    if (now - g_input.mouse_click_time[btn] < DOUBLE_CLICK_TIME) {
                        g_input.mouse_click_count[btn]++;
                    } else {
                        g_input.mouse_click_count[btn] = 1;
                    }
                    g_input.mouse_click_time[btn] = now;
                }
            }
            break;

        case SDL_CONTROLLERDEVICEADDED:
            input_on_gamepad_added(event->cdevice.which);
            break;

        case SDL_CONTROLLERDEVICEREMOVED:
            input_on_gamepad_removed(event->cdevice.which);
            break;

        case SDL_TEXTINPUT:
            if (g_input.text_input_active) {
                int len = strlen(g_input.text_buffer);
                int input_len = strlen(event->text.text);
                if (len + input_len < sizeof(g_input.text_buffer) - 1) {
                    strcat(g_input.text_buffer, event->text.text);
                }
            }
            break;

        case SDL_KEYDOWN:
            if (g_input.text_input_active) {
                if (event->key.keysym.sym == SDLK_BACKSPACE) {
                    int len = strlen(g_input.text_buffer);
                    if (len > 0) {
                        g_input.text_buffer[len - 1] = '\0';
                    }
                }
            }
            break;
    }
}

float input_normalize_axis(int16_t value)
{
    if (abs(value) < AXIS_DEADZONE) return 0.0f;

    float normalized;
    if (value > 0) {
        normalized = (value - AXIS_DEADZONE) / (float)(32767 - AXIS_DEADZONE);
    } else {
        normalized = (value + AXIS_DEADZONE) / (float)(32768 - AXIS_DEADZONE);
    }

    return normalized;
}

/*
 * ============================================================================
 * Action Queries
 * ============================================================================
 */

bool input_action_pressed(int action_id)
{
    if (action_id < 0 || action_id >= MAX_ACTIONS) return false;
    InputState state = g_input.actions[action_id].state;
    return state == INPUT_STATE_PRESSED ||
           state == INPUT_STATE_HELD ||
           state == INPUT_STATE_JUST_PRESSED;
}

bool input_action_just_pressed(int action_id)
{
    if (action_id < 0 || action_id >= MAX_ACTIONS) return false;
    return g_input.actions[action_id].state == INPUT_STATE_JUST_PRESSED;
}

bool input_action_just_released(int action_id)
{
    if (action_id < 0 || action_id >= MAX_ACTIONS) return false;
    return g_input.actions[action_id].state == INPUT_STATE_JUST_RELEASED;
}

bool input_action_held(int action_id)
{
    if (action_id < 0 || action_id >= MAX_ACTIONS) return false;
    return g_input.actions[action_id].state == INPUT_STATE_HELD;
}

uint32_t input_action_hold_time(int action_id)
{
    if (action_id < 0 || action_id >= MAX_ACTIONS) return 0;
    ActionState* action = &g_input.actions[action_id];
    if (action->state == INPUT_STATE_PRESSED ||
        action->state == INPUT_STATE_HELD ||
        action->state == INPUT_STATE_JUST_PRESSED) {
        return g_input.current_time - action->press_time;
    }
    return 0;
}

/*
 * ============================================================================
 * Key Queries (Raw)
 * ============================================================================
 */

bool input_key_pressed(SDL_Scancode key)
{
    return g_input.keyboard_state[key] != 0;
}

bool input_key_just_pressed(SDL_Scancode key)
{
    return g_input.keyboard_state[key] && !g_input.prev_keyboard[key];
}

bool input_key_just_released(SDL_Scancode key)
{
    return !g_input.keyboard_state[key] && g_input.prev_keyboard[key];
}

/*
 * ============================================================================
 * Mouse Queries
 * ============================================================================
 */

void input_get_mouse_position(int* x, int* y)
{
    *x = g_input.mouse_x;
    *y = g_input.mouse_y;
}

void input_get_mouse_delta(int* dx, int* dy)
{
    *dx = g_input.mouse_dx;
    *dy = g_input.mouse_dy;
}

int input_get_mouse_wheel(void)
{
    return g_input.mouse_wheel;
}

bool input_mouse_button_pressed(int button)
{
    return (g_input.mouse_buttons & SDL_BUTTON(button)) != 0;
}

bool input_mouse_button_just_pressed(int button)
{
    return (g_input.mouse_buttons & SDL_BUTTON(button)) &&
           !(g_input.prev_mouse_buttons & SDL_BUTTON(button));
}

bool input_mouse_button_just_released(int button)
{
    return !(g_input.mouse_buttons & SDL_BUTTON(button)) &&
           (g_input.prev_mouse_buttons & SDL_BUTTON(button));
}

bool input_mouse_double_clicked(int button)
{
    int btn = button - 1;
    if (btn < 0 || btn >= 5) return false;
    return g_input.mouse_click_count[btn] >= 2;
}

/*
 * ============================================================================
 * Gamepad Queries
 * ============================================================================
 */

void input_get_left_stick(float* x, float* y)
{
    *x = g_input.left_stick_x;
    *y = g_input.left_stick_y;
}

void input_get_right_stick(float* x, float* y)
{
    *x = g_input.right_stick_x;
    *y = g_input.right_stick_y;
}

float input_get_left_trigger(void)
{
    return g_input.left_trigger;
}

float input_get_right_trigger(void)
{
    return g_input.right_trigger;
}

bool input_gamepad_button_pressed(int button)
{
    return (g_input.gamepad_buttons & (1 << button)) != 0;
}

bool input_gamepad_button_just_pressed(int button)
{
    return (g_input.gamepad_buttons & (1 << button)) &&
           !(g_input.prev_gamepad_buttons & (1 << button));
}

/*
 * ============================================================================
 * Key Binding Management
 * ============================================================================
 */

void input_set_binding(int action_id, InputSource source, int key_code)
{
    for (int i = 0; i < g_input.binding_count; i++) {
        if (g_input.bindings[i].action_id == action_id) {
            g_input.bindings[i].source = source;
            g_input.bindings[i].key_code = key_code;
            return;
        }
    }
}

void input_set_alt_binding(int action_id, InputSource source, int key_code)
{
    for (int i = 0; i < g_input.binding_count; i++) {
        if (g_input.bindings[i].action_id == action_id) {
            g_input.bindings[i].alt_source = source;
            g_input.bindings[i].alt_key_code = key_code;
            return;
        }
    }
}

void input_set_gamepad_binding(int action_id, int button)
{
    for (int i = 0; i < g_input.binding_count; i++) {
        if (g_input.bindings[i].action_id == action_id) {
            g_input.bindings[i].gamepad_button = button;
            return;
        }
    }
}

void input_reset_bindings(void)
{
    g_input.binding_count = 0;
    for (int i = 0; DEFAULT_BINDINGS[i].name != NULL; i++) {
        const DefaultBinding* def = &DEFAULT_BINDINGS[i];
        KeyBinding* bind = &g_input.bindings[g_input.binding_count++];

        strncpy(bind->name, def->name, MAX_ACTION_NAME - 1);
        bind->action_id = def->action_id;
        bind->source = INPUT_SOURCE_KEYBOARD;
        bind->key_code = def->key;
        bind->alt_source = INPUT_SOURCE_KEYBOARD;
        bind->alt_key_code = def->alt_key;
        bind->gamepad_button = def->gamepad_button;
        bind->gamepad_axis = -1;
        bind->active = true;
    }
}

/*
 * ============================================================================
 * Text Input
 * ============================================================================
 */

void input_start_text_input(void)
{
    g_input.text_input_active = true;
    g_input.text_buffer[0] = '\0';
    SDL_StartTextInput();
}

void input_stop_text_input(void)
{
    g_input.text_input_active = false;
    SDL_StopTextInput();
}

bool input_is_text_input_active(void)
{
    return g_input.text_input_active;
}

const char* input_get_text_input(void)
{
    return g_input.text_buffer;
}

void input_clear_text_input(void)
{
    g_input.text_buffer[0] = '\0';
}

void input_set_text_input(const char* text)
{
    strncpy(g_input.text_buffer, text, sizeof(g_input.text_buffer) - 1);
    g_input.text_buffer[sizeof(g_input.text_buffer) - 1] = '\0';
}

/*
 * ============================================================================
 * Movement Helper
 * ============================================================================
 */

void input_get_movement_vector(float* x, float* y)
{
    *x = 0;
    *y = 0;

    /* Keyboard/action input */
    if (input_action_pressed(ACTION_MOVE_LEFT))  *x -= 1.0f;
    if (input_action_pressed(ACTION_MOVE_RIGHT)) *x += 1.0f;
    if (input_action_pressed(ACTION_MOVE_UP))    *y -= 1.0f;
    if (input_action_pressed(ACTION_MOVE_DOWN))  *y += 1.0f;

    /* Override with gamepad stick if significant */
    if (g_input.gamepad) {
        float stick_x = g_input.left_stick_x;
        float stick_y = g_input.left_stick_y;
        float mag = sqrtf(stick_x * stick_x + stick_y * stick_y);

        if (mag > 0.3f) {
            *x = stick_x;
            *y = stick_y;
        }
    }

    /* Normalize if diagonal */
    float len = sqrtf((*x) * (*x) + (*y) * (*y));
    if (len > 1.0f) {
        *x /= len;
        *y /= len;
    }
}

/*
 * ============================================================================
 * Vibration
 * ============================================================================
 */

void input_vibrate(float low_frequency, float high_frequency, uint32_t duration_ms)
{
    if (!g_input.gamepad) return;

#if SDL_VERSION_ATLEAST(2, 0, 9)
    uint16_t low = (uint16_t)(low_frequency * 65535);
    uint16_t high = (uint16_t)(high_frequency * 65535);
    SDL_GameControllerRumble(g_input.gamepad, low, high, duration_ms);
#endif
}

void input_stop_vibration(void)
{
    if (!g_input.gamepad) return;

#if SDL_VERSION_ATLEAST(2, 0, 9)
    SDL_GameControllerRumble(g_input.gamepad, 0, 0, 0);
#endif
}
