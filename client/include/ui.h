/**
 * @file ui.h
 * @brief UI system (buttons, panels, text)
 */

#ifndef CLIENT_UI_H
#define CLIENT_UI_H

#include "client.h"
#include "renderer.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UI_MAX_TEXT         256
#define UI_MAX_CHILDREN     32

/* UI element types */
typedef enum {
    UI_TYPE_NONE = 0,
    UI_TYPE_PANEL,
    UI_TYPE_BUTTON,
    UI_TYPE_LABEL,
    UI_TYPE_IMAGE,
    UI_TYPE_TEXTBOX,
    UI_TYPE_PROGRESSBAR,
    UI_TYPE_CHECKBOX,
    UI_TYPE_SLIDER,
    UI_TYPE_LIST
} UIType;

/* UI state */
typedef enum {
    UI_STATE_NORMAL = 0,
    UI_STATE_HOVER,
    UI_STATE_PRESSED,
    UI_STATE_DISABLED
} UIState;

/* Forward declaration */
typedef struct UIElement UIElement;

/* UI callback */
typedef void (*UICallback)(UIElement* element, void* user_data);

/* UI element base */
struct UIElement {
    u32             id;
    UIType          type;
    UIState         state;

    /* Position and size */
    Rect            bounds;
    bool            visible;
    bool            enabled;

    /* Hierarchy */
    UIElement*      parent;
    UIElement*      children[UI_MAX_CHILDREN];
    int             child_count;

    /* Appearance */
    Color           bg_color;
    Color           border_color;
    Color           text_color;
    SDL_Texture*    texture;
    TTF_Font*       font;
    char            text[UI_MAX_TEXT];

    /* Button specific */
    UICallback      on_click;
    void*           user_data;

    /* Progress bar specific */
    float           value;
    float           max_value;

    /* Textbox specific */
    bool            focused;
    int             cursor_pos;
    int             max_length;
    bool            password;

    /* Slider specific */
    float           min_value;
    float           step;
};

/* UI manager */
typedef struct UIManager {
    UIElement*      elements[MAX_UI_ELEMENTS];
    int             element_count;
    UIElement*      focused;
    UIElement*      hovered;
    TTF_Font*       default_font;
    u32             next_id;
} UIManager;

/**
 * Initialize UI manager
 * @param manager UI manager instance
 * @return CLIENT_OK on success
 */
int ui_manager_init(UIManager* manager);

/**
 * Cleanup UI manager
 * @param manager UI manager instance
 */
void ui_manager_cleanup(UIManager* manager);

/**
 * Load default font
 * @param manager   UI manager instance
 * @param path      Font file path
 * @param size      Font size
 * @return CLIENT_OK on success
 */
int ui_load_font(UIManager* manager, const char* path, int size);

/**
 * Update UI
 * @param manager UI manager instance
 * @param input   Input state
 */
void ui_manager_update(UIManager* manager, InputState* input);

/**
 * Draw UI
 * @param manager  UI manager instance
 * @param renderer Renderer instance
 */
void ui_manager_draw(UIManager* manager, Renderer* renderer);

/**
 * Handle event
 * @param manager UI manager instance
 * @param event   SDL event
 * @return true if event was consumed
 */
bool ui_manager_handle_event(UIManager* manager, SDL_Event* event);

/*
 * Element creation functions
 */

/**
 * Create panel
 * @param manager UI manager instance
 * @param x       X position
 * @param y       Y position
 * @param w       Width
 * @param h       Height
 * @return UIElement pointer
 */
UIElement* ui_create_panel(UIManager* manager, float x, float y, float w, float h);

/**
 * Create button
 * @param manager   UI manager instance
 * @param x         X position
 * @param y         Y position
 * @param w         Width
 * @param h         Height
 * @param text      Button text
 * @param on_click  Click callback
 * @param user_data Callback data
 * @return UIElement pointer
 */
UIElement* ui_create_button(UIManager* manager, float x, float y, float w, float h,
                           const char* text, UICallback on_click, void* user_data);

/**
 * Create label
 * @param manager UI manager instance
 * @param x       X position
 * @param y       Y position
 * @param text    Label text
 * @return UIElement pointer
 */
UIElement* ui_create_label(UIManager* manager, float x, float y, const char* text);

/**
 * Create image
 * @param manager UI manager instance
 * @param x       X position
 * @param y       Y position
 * @param texture Texture
 * @return UIElement pointer
 */
UIElement* ui_create_image(UIManager* manager, float x, float y, SDL_Texture* texture);

/**
 * Create textbox
 * @param manager    UI manager instance
 * @param x          X position
 * @param y          Y position
 * @param w          Width
 * @param h          Height
 * @param max_length Maximum text length
 * @return UIElement pointer
 */
UIElement* ui_create_textbox(UIManager* manager, float x, float y, float w, float h,
                            int max_length);

/**
 * Create progress bar
 * @param manager UI manager instance
 * @param x       X position
 * @param y       Y position
 * @param w       Width
 * @param h       Height
 * @param max_val Maximum value
 * @return UIElement pointer
 */
UIElement* ui_create_progressbar(UIManager* manager, float x, float y, float w, float h,
                                float max_val);

/**
 * Destroy element
 * @param manager UI manager instance
 * @param element Element to destroy
 */
void ui_destroy_element(UIManager* manager, UIElement* element);

/**
 * Set element text
 * @param element UI element
 * @param text    New text
 */
void ui_set_text(UIElement* element, const char* text);

/**
 * Get element text
 * @param element UI element
 * @return Text string
 */
const char* ui_get_text(UIElement* element);

/**
 * Set progress bar value
 * @param element UI element
 * @param value   New value
 */
void ui_set_value(UIElement* element, float value);

/**
 * Add child element
 * @param parent Parent element
 * @param child  Child element
 */
void ui_add_child(UIElement* parent, UIElement* child);

/**
 * Set element visibility
 * @param element UI element
 * @param visible Visibility
 */
void ui_set_visible(UIElement* element, bool visible);

/**
 * Set element enabled state
 * @param element UI element
 * @param enabled Enabled state
 */
void ui_set_enabled(UIElement* element, bool enabled);

/*
 * Common UI layouts
 */

/**
 * Create HP/MP bar panel
 * @param manager UI manager
 * @param x       X position
 * @param y       Y position
 * @return Panel element
 */
UIElement* ui_create_status_bars(UIManager* manager, float x, float y);

/**
 * Create skill bar
 * @param manager UI manager
 * @param x       X position
 * @param y       Y position
 * @return Panel element
 */
UIElement* ui_create_skill_bar(UIManager* manager, float x, float y);

/**
 * Create chat box
 * @param manager UI manager
 * @param x       X position
 * @param y       Y position
 * @param w       Width
 * @param h       Height
 * @return Panel element
 */
UIElement* ui_create_chat_box(UIManager* manager, float x, float y, float w, float h);

#ifdef __cplusplus
}
#endif

#endif /* CLIENT_UI_H */
