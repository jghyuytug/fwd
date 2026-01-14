/**
 * @file ui_chat.c
 * @brief Chat UI with channels, whisper, and history
 */

#include "../../include/client.h"
#include "../../include/renderer.h"
#include "../../include/network.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
 * ============================================================================
 * Constants
 * ============================================================================
 */

#define MAX_CHAT_MESSAGES       100
#define MAX_MESSAGE_LENGTH      256
#define MAX_NAME_LENGTH         32
#define CHAT_LINE_HEIGHT        16
#define VISIBLE_LINES           10
#define INPUT_HEIGHT            24
#define PANEL_PADDING           8
#define FADE_TIME               10.0f   /* Seconds before fade starts */
#define FADE_DURATION           3.0f

/*
 * ============================================================================
 * Chat Types
 * ============================================================================
 */

typedef enum {
    CHAT_CHANNEL_NORMAL = 0,
    CHAT_CHANNEL_PARTY,
    CHAT_CHANNEL_GUILD,
    CHAT_CHANNEL_WORLD,
    CHAT_CHANNEL_WHISPER,
    CHAT_CHANNEL_SYSTEM,
    CHAT_CHANNEL_MEGAPHONE,
    CHAT_CHANNEL_COUNT
} ChatChannel;

typedef struct {
    char            sender[MAX_NAME_LENGTH];
    char            message[MAX_MESSAGE_LENGTH];
    ChatChannel     channel;
    uint32_t        timestamp;
    float           age;            /* For fading */
    bool            is_self;
} ChatMessage;

typedef struct {
    /* Messages */
    ChatMessage     messages[MAX_CHAT_MESSAGES];
    int             message_count;
    int             message_head;   /* Circular buffer head */

    /* UI state */
    bool            visible;
    bool            focused;
    float           x;
    float           y;
    float           width;
    float           height;

    /* Input */
    char            input_buffer[MAX_MESSAGE_LENGTH];
    int             input_cursor;
    bool            input_active;

    /* Scroll */
    int             scroll_offset;
    float           scroll_target;

    /* Channel */
    ChatChannel     current_channel;
    bool            channel_filters[CHAT_CHANNEL_COUNT];

    /* Whisper */
    char            whisper_target[MAX_NAME_LENGTH];

    /* Fade */
    float           idle_timer;
    float           fade_alpha;

    /* History */
    char            history[20][MAX_MESSAGE_LENGTH];
    int             history_count;
    int             history_index;

    bool            initialized;
} ChatUI;

static ChatUI g_chat = {0};

/*
 * ============================================================================
 * Channel Colors
 * ============================================================================
 */

static Color get_channel_color(ChatChannel channel)
{
    switch (channel) {
        case CHAT_CHANNEL_NORMAL:    return (Color){255, 255, 255, 255};
        case CHAT_CHANNEL_PARTY:     return (Color){100, 200, 255, 255};
        case CHAT_CHANNEL_GUILD:     return (Color){100, 255, 100, 255};
        case CHAT_CHANNEL_WORLD:     return (Color){255, 200, 100, 255};
        case CHAT_CHANNEL_WHISPER:   return (Color){255, 150, 255, 255};
        case CHAT_CHANNEL_SYSTEM:    return (Color){255, 255, 100, 255};
        case CHAT_CHANNEL_MEGAPHONE: return (Color){255, 100, 100, 255};
        default:                     return (Color){255, 255, 255, 255};
    }
}

static const char* get_channel_prefix(ChatChannel channel)
{
    switch (channel) {
        case CHAT_CHANNEL_NORMAL:    return "";
        case CHAT_CHANNEL_PARTY:     return "[Party] ";
        case CHAT_CHANNEL_GUILD:     return "[Guild] ";
        case CHAT_CHANNEL_WORLD:     return "[World] ";
        case CHAT_CHANNEL_WHISPER:   return "[Whisper] ";
        case CHAT_CHANNEL_SYSTEM:    return "[System] ";
        case CHAT_CHANNEL_MEGAPHONE: return "[Megaphone] ";
        default:                     return "";
    }
}

/*
 * ============================================================================
 * External Functions
 * ============================================================================
 */

extern void renderer_draw_rect(Renderer* r, Rect rect, Color color, bool filled);
extern void renderer_draw_text(Renderer* r, const char* text, int x, int y, Color color);
extern int network_send_packet(uint16_t msg_no, const void* data, size_t len);

/*
 * ============================================================================
 * Initialization
 * ============================================================================
 */

void ui_chat_init(void)
{
    memset(&g_chat, 0, sizeof(ChatUI));

    g_chat.width = 400;
    g_chat.height = VISIBLE_LINES * CHAT_LINE_HEIGHT + INPUT_HEIGHT + PANEL_PADDING * 3;
    g_chat.x = 10;
    g_chat.y = WINDOW_HEIGHT - g_chat.height - 10;

    g_chat.visible = true;
    g_chat.fade_alpha = 1.0f;
    g_chat.current_channel = CHAT_CHANNEL_NORMAL;

    /* Enable all channel filters by default */
    for (int i = 0; i < CHAT_CHANNEL_COUNT; i++) {
        g_chat.channel_filters[i] = true;
    }

    g_chat.initialized = true;
}

void ui_chat_shutdown(void)
{
    g_chat.initialized = false;
}

/*
 * ============================================================================
 * Message Management
 * ============================================================================
 */

void ui_chat_add_message(const char* sender, const char* message,
                          ChatChannel channel, bool is_self)
{
    int idx = (g_chat.message_head + g_chat.message_count) % MAX_CHAT_MESSAGES;

    if (g_chat.message_count >= MAX_CHAT_MESSAGES) {
        g_chat.message_head = (g_chat.message_head + 1) % MAX_CHAT_MESSAGES;
    } else {
        g_chat.message_count++;
    }

    ChatMessage* msg = &g_chat.messages[idx];
    strncpy(msg->sender, sender, MAX_NAME_LENGTH - 1);
    strncpy(msg->message, message, MAX_MESSAGE_LENGTH - 1);
    msg->channel = channel;
    msg->timestamp = (uint32_t)time(NULL);
    msg->age = 0;
    msg->is_self = is_self;

    /* Reset fade */
    g_chat.idle_timer = 0;
    g_chat.fade_alpha = 1.0f;

    /* Auto-scroll to bottom */
    g_chat.scroll_offset = 0;
}

void ui_chat_add_system_message(const char* message)
{
    ui_chat_add_message("System", message, CHAT_CHANNEL_SYSTEM, false);
}

void ui_chat_add_whisper(const char* sender, const char* message, bool outgoing)
{
    if (outgoing) {
        char formatted[MAX_MESSAGE_LENGTH];
        snprintf(formatted, sizeof(formatted), "To %s: %s", g_chat.whisper_target, message);
        ui_chat_add_message("", formatted, CHAT_CHANNEL_WHISPER, true);
    } else {
        char formatted[MAX_MESSAGE_LENGTH];
        snprintf(formatted, sizeof(formatted), "From %s: %s", sender, message);
        ui_chat_add_message("", formatted, CHAT_CHANNEL_WHISPER, false);

        /* Set reply target */
        strncpy(g_chat.whisper_target, sender, MAX_NAME_LENGTH - 1);
    }
}

/*
 * ============================================================================
 * Input Handling
 * ============================================================================
 */

void ui_chat_start_input(void)
{
    g_chat.input_active = true;
    g_chat.focused = true;
    g_chat.fade_alpha = 1.0f;
    g_chat.idle_timer = 0;
    input_start_text_input();
}

void ui_chat_stop_input(void)
{
    g_chat.input_active = false;
    input_stop_text_input();
}

void ui_chat_send_message(void)
{
    if (strlen(g_chat.input_buffer) == 0) {
        ui_chat_stop_input();
        return;
    }

    /* Check for commands */
    if (g_chat.input_buffer[0] == '/') {
        /* Parse command */
        if (strncmp(g_chat.input_buffer, "/w ", 3) == 0 ||
            strncmp(g_chat.input_buffer, "/whisper ", 9) == 0) {
            /* Whisper command */
            char* start = strchr(g_chat.input_buffer, ' ') + 1;
            char* space = strchr(start, ' ');
            if (space) {
                *space = '\0';
                strncpy(g_chat.whisper_target, start, MAX_NAME_LENGTH - 1);
                g_chat.current_channel = CHAT_CHANNEL_WHISPER;

                char* msg = space + 1;
                if (strlen(msg) > 0) {
                    struct {
                        char target[MAX_NAME_LENGTH];
                        char message[MAX_MESSAGE_LENGTH];
                    } packet;
                    strncpy(packet.target, g_chat.whisper_target, MAX_NAME_LENGTH);
                    strncpy(packet.message, msg, MAX_MESSAGE_LENGTH);
                    network_send_packet(MSG_CHAT_WHISPER, &packet, sizeof(packet));
                    ui_chat_add_whisper(g_chat.whisper_target, msg, true);
                }
            }
        } else if (strncmp(g_chat.input_buffer, "/p ", 3) == 0 ||
                   strncmp(g_chat.input_buffer, "/party ", 7) == 0) {
            g_chat.current_channel = CHAT_CHANNEL_PARTY;
        } else if (strncmp(g_chat.input_buffer, "/g ", 3) == 0 ||
                   strncmp(g_chat.input_buffer, "/guild ", 7) == 0) {
            g_chat.current_channel = CHAT_CHANNEL_GUILD;
        } else if (strncmp(g_chat.input_buffer, "/s ", 3) == 0 ||
                   strncmp(g_chat.input_buffer, "/say ", 5) == 0) {
            g_chat.current_channel = CHAT_CHANNEL_NORMAL;
        }
    } else {
        /* Send message to current channel */
        struct {
            uint8_t channel;
            char message[MAX_MESSAGE_LENGTH];
        } packet;
        packet.channel = g_chat.current_channel;
        strncpy(packet.message, g_chat.input_buffer, MAX_MESSAGE_LENGTH);
        network_send_packet(MSG_CHAT, &packet, sizeof(packet));

        /* Add to local display immediately */
        ui_chat_add_message("You", g_chat.input_buffer, g_chat.current_channel, true);
    }

    /* Add to history */
    if (g_chat.history_count < 20) {
        strncpy(g_chat.history[g_chat.history_count++],
                g_chat.input_buffer, MAX_MESSAGE_LENGTH);
    } else {
        memmove(&g_chat.history[0], &g_chat.history[1],
                19 * MAX_MESSAGE_LENGTH);
        strncpy(g_chat.history[19], g_chat.input_buffer, MAX_MESSAGE_LENGTH);
    }
    g_chat.history_index = g_chat.history_count;

    /* Clear input */
    g_chat.input_buffer[0] = '\0';
    g_chat.input_cursor = 0;
    ui_chat_stop_input();
}

bool ui_chat_handle_input(int mouse_x, int mouse_y, bool left_click)
{
    if (!g_chat.visible) return false;

    bool over_panel = mouse_x >= g_chat.x && mouse_x <= g_chat.x + g_chat.width &&
                      mouse_y >= g_chat.y && mouse_y <= g_chat.y + g_chat.height;

    if (left_click && over_panel) {
        /* Check if clicked on input area */
        float input_y = g_chat.y + g_chat.height - INPUT_HEIGHT - PANEL_PADDING;
        if (mouse_y >= input_y) {
            ui_chat_start_input();
        }
        return true;
    }

    /* Handle keyboard when input is active */
    if (g_chat.input_active) {
        /* Get text from input system */
        const char* text = input_get_text_input();
        if (text && strlen(text) > 0) {
            strncpy(g_chat.input_buffer, text, MAX_MESSAGE_LENGTH - 1);
            g_chat.input_cursor = strlen(g_chat.input_buffer);
        }

        /* Enter to send */
        if (input_key_just_pressed(SDL_SCANCODE_RETURN)) {
            ui_chat_send_message();
            input_clear_text_input();
            return true;
        }

        /* Escape to cancel */
        if (input_key_just_pressed(SDL_SCANCODE_ESCAPE)) {
            g_chat.input_buffer[0] = '\0';
            input_clear_text_input();
            ui_chat_stop_input();
            return true;
        }

        /* History navigation */
        if (input_key_just_pressed(SDL_SCANCODE_UP)) {
            if (g_chat.history_index > 0) {
                g_chat.history_index--;
                strncpy(g_chat.input_buffer, g_chat.history[g_chat.history_index],
                        MAX_MESSAGE_LENGTH);
                input_set_text_input(g_chat.input_buffer);
            }
            return true;
        }
        if (input_key_just_pressed(SDL_SCANCODE_DOWN)) {
            if (g_chat.history_index < g_chat.history_count - 1) {
                g_chat.history_index++;
                strncpy(g_chat.input_buffer, g_chat.history[g_chat.history_index],
                        MAX_MESSAGE_LENGTH);
                input_set_text_input(g_chat.input_buffer);
            }
            return true;
        }

        return true;
    }

    /* Mouse wheel for scrolling */
    int wheel = input_get_mouse_wheel();
    if (wheel != 0 && over_panel) {
        g_chat.scroll_offset += wheel * 2;
        if (g_chat.scroll_offset < 0) g_chat.scroll_offset = 0;
        int max_scroll = g_chat.message_count - VISIBLE_LINES;
        if (max_scroll < 0) max_scroll = 0;
        if (g_chat.scroll_offset > max_scroll) g_chat.scroll_offset = max_scroll;
        return true;
    }

    return over_panel;
}

/*
 * ============================================================================
 * Update
 * ============================================================================
 */

void ui_chat_update(float delta)
{
    if (!g_chat.initialized) return;

    /* Update message ages */
    for (int i = 0; i < g_chat.message_count; i++) {
        int idx = (g_chat.message_head + i) % MAX_CHAT_MESSAGES;
        g_chat.messages[idx].age += delta;
    }

    /* Update fade */
    if (!g_chat.input_active && !g_chat.focused) {
        g_chat.idle_timer += delta;
        if (g_chat.idle_timer > FADE_TIME) {
            float fade_progress = (g_chat.idle_timer - FADE_TIME) / FADE_DURATION;
            g_chat.fade_alpha = 1.0f - fade_progress;
            if (g_chat.fade_alpha < 0.3f) g_chat.fade_alpha = 0.3f;
        }
    }
}

/*
 * ============================================================================
 * Rendering
 * ============================================================================
 */

void ui_chat_render(Renderer* renderer)
{
    if (!g_chat.visible || !renderer) return;

    uint8_t alpha = (uint8_t)(g_chat.fade_alpha * 255);
    if (g_chat.input_active) alpha = 255;

    Color bg = {20, 20, 30, (uint8_t)(alpha * 0.8f)};
    Color border = {60, 60, 80, alpha};
    Color input_bg = {30, 30, 40, alpha};
    Color text = {255, 255, 255, alpha};

    /* Background */
    Rect panel = {g_chat.x, g_chat.y, g_chat.width, g_chat.height};
    renderer_draw_rect(renderer, panel, bg, true);
    renderer_draw_rect(renderer, panel, border, false);

    /* Channel tabs */
    const char* tabs[] = {"All", "Party", "Guild", "World"};
    float tab_x = g_chat.x + PANEL_PADDING;
    for (int i = 0; i < 4; i++) {
        bool is_current = (i == 0 && g_chat.current_channel == CHAT_CHANNEL_NORMAL) ||
                          (i == 1 && g_chat.current_channel == CHAT_CHANNEL_PARTY) ||
                          (i == 2 && g_chat.current_channel == CHAT_CHANNEL_GUILD) ||
                          (i == 3 && g_chat.current_channel == CHAT_CHANNEL_WORLD);

        Color tc = is_current ? (Color){100, 100, 150, alpha} : input_bg;
        Rect tab = {tab_x, g_chat.y + 2, 45, 14};
        renderer_draw_rect(renderer, tab, tc, true);

        Color tab_text = {255, 255, 255, alpha};
        renderer_draw_text(renderer, tabs[i], (int)tab_x + 4, (int)g_chat.y + 4, tab_text);
        tab_x += 50;
    }

    /* Messages */
    float msg_y = g_chat.y + 20;
    int visible_start = g_chat.message_count - VISIBLE_LINES - g_chat.scroll_offset;
    if (visible_start < 0) visible_start = 0;

    for (int i = 0; i < VISIBLE_LINES && (visible_start + i) < g_chat.message_count; i++) {
        int idx = (g_chat.message_head + visible_start + i) % MAX_CHAT_MESSAGES;
        ChatMessage* msg = &g_chat.messages[idx];

        if (!g_chat.channel_filters[msg->channel]) continue;

        Color msg_color = get_channel_color(msg->channel);
        msg_color.a = alpha;

        char line[MAX_MESSAGE_LENGTH + MAX_NAME_LENGTH + 20];
        if (msg->sender[0]) {
            snprintf(line, sizeof(line), "%s%s: %s",
                     get_channel_prefix(msg->channel), msg->sender, msg->message);
        } else {
            snprintf(line, sizeof(line), "%s%s",
                     get_channel_prefix(msg->channel), msg->message);
        }

        renderer_draw_text(renderer, line, (int)g_chat.x + PANEL_PADDING,
                           (int)msg_y, msg_color);
        msg_y += CHAT_LINE_HEIGHT;
    }

    /* Input area */
    float input_y = g_chat.y + g_chat.height - INPUT_HEIGHT - PANEL_PADDING;
    Rect input_rect = {g_chat.x + PANEL_PADDING, input_y,
                       g_chat.width - PANEL_PADDING * 2, INPUT_HEIGHT};
    renderer_draw_rect(renderer, input_rect, input_bg, true);
    renderer_draw_rect(renderer, input_rect, border, false);

    /* Channel indicator */
    Color channel_color = get_channel_color(g_chat.current_channel);
    channel_color.a = alpha;
    const char* channel_name = "";
    switch (g_chat.current_channel) {
        case CHAT_CHANNEL_NORMAL: channel_name = "[Say]"; break;
        case CHAT_CHANNEL_PARTY: channel_name = "[Party]"; break;
        case CHAT_CHANNEL_GUILD: channel_name = "[Guild]"; break;
        case CHAT_CHANNEL_WORLD: channel_name = "[World]"; break;
        case CHAT_CHANNEL_WHISPER:
            {
                static char whisper_label[48];
                snprintf(whisper_label, sizeof(whisper_label), "[To: %s]", g_chat.whisper_target);
                channel_name = whisper_label;
            }
            break;
        default: break;
    }
    renderer_draw_text(renderer, channel_name, (int)g_chat.x + PANEL_PADDING + 4,
                       (int)input_y + 5, channel_color);

    /* Input text */
    if (g_chat.input_buffer[0] || g_chat.input_active) {
        renderer_draw_text(renderer, g_chat.input_buffer,
                           (int)g_chat.x + PANEL_PADDING + 60, (int)input_y + 5, text);

        /* Cursor */
        if (g_chat.input_active && ((int)(SDL_GetTicks() / 500) % 2)) {
            int cursor_x = (int)g_chat.x + PANEL_PADDING + 60 +
                           strlen(g_chat.input_buffer) * 8;
            Rect cursor = {cursor_x, input_y + 4, 2, INPUT_HEIGHT - 8};
            renderer_draw_rect(renderer, cursor, text, true);
        }
    }
}

/*
 * ============================================================================
 * Channel Management
 * ============================================================================
 */

void ui_chat_set_channel(ChatChannel channel)
{
    g_chat.current_channel = channel;
}

void ui_chat_set_whisper_target(const char* name)
{
    strncpy(g_chat.whisper_target, name, MAX_NAME_LENGTH - 1);
    g_chat.current_channel = CHAT_CHANNEL_WHISPER;
}

void ui_chat_toggle_channel_filter(ChatChannel channel)
{
    if (channel < CHAT_CHANNEL_COUNT) {
        g_chat.channel_filters[channel] = !g_chat.channel_filters[channel];
    }
}

bool ui_chat_is_input_active(void)
{
    return g_chat.input_active;
}
