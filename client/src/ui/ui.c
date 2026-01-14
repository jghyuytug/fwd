/**
 * @file ui.c
 * @brief UI system implementation
 */

#include "../../include/ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int ui_manager_init(UIManager* manager)
{
    if (!manager) return CLIENT_ERROR;

    memset(manager, 0, sizeof(UIManager));
    manager->next_id = 1;

    return CLIENT_OK;
}

void ui_manager_cleanup(UIManager* manager)
{
    int i;

    if (!manager) return;

    for (i = 0; i < manager->element_count; i++) {
        if (manager->elements[i]) {
            free(manager->elements[i]);
        }
    }

    if (manager->default_font) {
        TTF_CloseFont(manager->default_font);
    }

    memset(manager, 0, sizeof(UIManager));
}

int ui_load_font(UIManager* manager, const char* path, int size)
{
    if (!manager || !path) return CLIENT_ERROR;

    manager->default_font = TTF_OpenFont(path, size);
    if (!manager->default_font) {
        fprintf(stderr, "Failed to load font: %s - %s\n", path, TTF_GetError());
        return CLIENT_ERROR_IO;
    }

    return CLIENT_OK;
}

static bool point_in_rect(float px, float py, Rect* rect)
{
    return px >= rect->x && px < rect->x + rect->w &&
           py >= rect->y && py < rect->y + rect->h;
}

void ui_manager_update(UIManager* manager, InputState* input)
{
    int i;
    UIElement* element;

    if (!manager || !input) return;

    manager->hovered = NULL;

    /* Update element states */
    for (i = 0; i < manager->element_count; i++) {
        element = manager->elements[i];
        if (!element || !element->visible || !element->enabled) continue;

        /* Check hover */
        if (point_in_rect((float)input->mouse_x, (float)input->mouse_y, &element->bounds)) {
            manager->hovered = element;

            if (element->state != UI_STATE_PRESSED) {
                element->state = UI_STATE_HOVER;
            }

            /* Check click */
            if (input->mouse_pressed[0]) {
                element->state = UI_STATE_PRESSED;

                /* Focus textboxes */
                if (element->type == UI_TYPE_TEXTBOX) {
                    manager->focused = element;
                    element->focused = true;
                }
            } else if (!input->mouse_buttons[0] && element->state == UI_STATE_PRESSED) {
                /* Mouse released - trigger click */
                if (element->on_click) {
                    element->on_click(element, element->user_data);
                }
                element->state = UI_STATE_HOVER;
            }
        } else {
            if (element->state != UI_STATE_DISABLED) {
                element->state = UI_STATE_NORMAL;
            }
        }
    }

    /* Unfocus if clicked outside */
    if (input->mouse_pressed[0] && manager->focused && manager->hovered != manager->focused) {
        manager->focused->focused = false;
        manager->focused = NULL;
    }
}

static void draw_element(UIManager* manager, Renderer* renderer, UIElement* element)
{
    SDL_Rect rect;
    Color color;

    if (!element->visible) return;

    rect.x = (int)element->bounds.x;
    rect.y = (int)element->bounds.y;
    rect.w = (int)element->bounds.w;
    rect.h = (int)element->bounds.h;

    switch (element->type) {
        case UI_TYPE_PANEL:
            renderer_draw_rect(renderer, element->bounds, element->bg_color, true);
            renderer_draw_rect(renderer, element->bounds, element->border_color, false);
            break;

        case UI_TYPE_BUTTON:
            /* Button background */
            color = element->bg_color;
            if (element->state == UI_STATE_HOVER) {
                color.r = (u8)(color.r * 1.2f > 255 ? 255 : color.r * 1.2f);
                color.g = (u8)(color.g * 1.2f > 255 ? 255 : color.g * 1.2f);
                color.b = (u8)(color.b * 1.2f > 255 ? 255 : color.b * 1.2f);
            } else if (element->state == UI_STATE_PRESSED) {
                color.r = (u8)(color.r * 0.8f);
                color.g = (u8)(color.g * 0.8f);
                color.b = (u8)(color.b * 0.8f);
            }
            renderer_draw_rect(renderer, element->bounds, color, true);
            renderer_draw_rect(renderer, element->bounds, element->border_color, false);

            /* Button text */
            if (manager->default_font && strlen(element->text) > 0) {
                SDL_Surface* surface = TTF_RenderText_Blended(manager->default_font,
                    element->text,
                    (SDL_Color){element->text_color.r, element->text_color.g,
                               element->text_color.b, element->text_color.a});
                if (surface) {
                    SDL_Texture* texture = SDL_CreateTextureFromSurface(
                        renderer->sdl_renderer, surface);
                    if (texture) {
                        SDL_Rect text_rect;
                        text_rect.w = surface->w;
                        text_rect.h = surface->h;
                        text_rect.x = rect.x + (rect.w - text_rect.w) / 2;
                        text_rect.y = rect.y + (rect.h - text_rect.h) / 2;
                        SDL_RenderCopy(renderer->sdl_renderer, texture, NULL, &text_rect);
                        SDL_DestroyTexture(texture);
                    }
                    SDL_FreeSurface(surface);
                }
            }
            break;

        case UI_TYPE_LABEL:
            if (manager->default_font && strlen(element->text) > 0) {
                SDL_Surface* surface = TTF_RenderText_Blended(manager->default_font,
                    element->text,
                    (SDL_Color){element->text_color.r, element->text_color.g,
                               element->text_color.b, element->text_color.a});
                if (surface) {
                    SDL_Texture* texture = SDL_CreateTextureFromSurface(
                        renderer->sdl_renderer, surface);
                    if (texture) {
                        SDL_Rect text_rect = {rect.x, rect.y, surface->w, surface->h};
                        SDL_RenderCopy(renderer->sdl_renderer, texture, NULL, &text_rect);
                        SDL_DestroyTexture(texture);
                    }
                    SDL_FreeSurface(surface);
                }
            }
            break;

        case UI_TYPE_TEXTBOX:
            /* Background */
            color = element->focused ? (Color){48, 48, 64, 255} : element->bg_color;
            renderer_draw_rect(renderer, element->bounds, color, true);
            renderer_draw_rect(renderer, element->bounds, element->border_color, false);

            /* Text */
            if (manager->default_font && strlen(element->text) > 0) {
                char display[UI_MAX_TEXT];
                if (element->password) {
                    int i, len = (int)strlen(element->text);
                    for (i = 0; i < len && i < UI_MAX_TEXT - 1; i++) {
                        display[i] = '*';
                    }
                    display[i] = '\0';
                } else {
                    strncpy(display, element->text, UI_MAX_TEXT - 1);
                }

                SDL_Surface* surface = TTF_RenderText_Blended(manager->default_font,
                    display,
                    (SDL_Color){element->text_color.r, element->text_color.g,
                               element->text_color.b, element->text_color.a});
                if (surface) {
                    SDL_Texture* texture = SDL_CreateTextureFromSurface(
                        renderer->sdl_renderer, surface);
                    if (texture) {
                        SDL_Rect text_rect;
                        text_rect.x = rect.x + 5;
                        text_rect.y = rect.y + (rect.h - surface->h) / 2;
                        text_rect.w = surface->w;
                        text_rect.h = surface->h;
                        SDL_RenderCopy(renderer->sdl_renderer, texture, NULL, &text_rect);
                        SDL_DestroyTexture(texture);
                    }
                    SDL_FreeSurface(surface);
                }
            }

            /* Cursor */
            if (element->focused) {
                Rect cursor;
                int text_width = 0;
                if (manager->default_font && strlen(element->text) > 0) {
                    TTF_SizeText(manager->default_font, element->text, &text_width, NULL);
                }
                cursor.x = element->bounds.x + 5 + text_width;
                cursor.y = element->bounds.y + 5;
                cursor.w = 2;
                cursor.h = element->bounds.h - 10;
                Color cursor_color = {255, 255, 255, 255};
                renderer_draw_rect(renderer, cursor, cursor_color, true);
            }
            break;

        case UI_TYPE_PROGRESSBAR:
            /* Background */
            renderer_draw_rect(renderer, element->bounds, element->bg_color, true);

            /* Progress */
            if (element->max_value > 0) {
                Rect progress = element->bounds;
                progress.w = progress.w * (element->value / element->max_value);
                Color progress_color = {64, 200, 64, 255};
                renderer_draw_rect(renderer, progress, progress_color, true);
            }

            renderer_draw_rect(renderer, element->bounds, element->border_color, false);
            break;

        default:
            break;
    }

    /* Draw children */
    {
        int i;
        for (i = 0; i < element->child_count; i++) {
            if (element->children[i]) {
                draw_element(manager, renderer, element->children[i]);
            }
        }
    }
}

void ui_manager_draw(UIManager* manager, Renderer* renderer)
{
    int i;

    if (!manager || !renderer) return;

    for (i = 0; i < manager->element_count; i++) {
        if (manager->elements[i] && !manager->elements[i]->parent) {
            draw_element(manager, renderer, manager->elements[i]);
        }
    }
}

bool ui_manager_handle_event(UIManager* manager, SDL_Event* event)
{
    if (!manager || !event) return false;

    /* Handle text input for focused textbox */
    if (manager->focused && manager->focused->type == UI_TYPE_TEXTBOX) {
        if (event->type == SDL_TEXTINPUT) {
            int len = (int)strlen(manager->focused->text);
            int input_len = (int)strlen(event->text.text);
            if (len + input_len < manager->focused->max_length) {
                strcat(manager->focused->text, event->text.text);
            }
            return true;
        } else if (event->type == SDL_KEYDOWN) {
            if (event->key.keysym.sym == SDLK_BACKSPACE) {
                int len = (int)strlen(manager->focused->text);
                if (len > 0) {
                    manager->focused->text[len - 1] = '\0';
                }
                return true;
            }
        }
    }

    return false;
}

static UIElement* create_element(UIManager* manager, UIType type)
{
    UIElement* element;

    if (!manager || manager->element_count >= MAX_UI_ELEMENTS) {
        return NULL;
    }

    element = calloc(1, sizeof(UIElement));
    if (!element) return NULL;

    element->id = manager->next_id++;
    element->type = type;
    element->state = UI_STATE_NORMAL;
    element->visible = true;
    element->enabled = true;
    element->bg_color = (Color){48, 48, 48, 255};
    element->border_color = (Color){96, 96, 96, 255};
    element->text_color = (Color){255, 255, 255, 255};
    element->font = manager->default_font;

    manager->elements[manager->element_count++] = element;

    return element;
}

UIElement* ui_create_panel(UIManager* manager, float x, float y, float w, float h)
{
    UIElement* element = create_element(manager, UI_TYPE_PANEL);
    if (!element) return NULL;

    element->bounds.x = x;
    element->bounds.y = y;
    element->bounds.w = w;
    element->bounds.h = h;

    return element;
}

UIElement* ui_create_button(UIManager* manager, float x, float y, float w, float h,
                           const char* text, UICallback on_click, void* user_data)
{
    UIElement* element = create_element(manager, UI_TYPE_BUTTON);
    if (!element) return NULL;

    element->bounds.x = x;
    element->bounds.y = y;
    element->bounds.w = w;
    element->bounds.h = h;
    element->on_click = on_click;
    element->user_data = user_data;

    if (text) {
        strncpy(element->text, text, UI_MAX_TEXT - 1);
    }

    return element;
}

UIElement* ui_create_label(UIManager* manager, float x, float y, const char* text)
{
    UIElement* element = create_element(manager, UI_TYPE_LABEL);
    if (!element) return NULL;

    element->bounds.x = x;
    element->bounds.y = y;
    element->bounds.w = 200;
    element->bounds.h = 20;

    if (text) {
        strncpy(element->text, text, UI_MAX_TEXT - 1);
    }

    return element;
}

UIElement* ui_create_image(UIManager* manager, float x, float y, SDL_Texture* texture)
{
    UIElement* element = create_element(manager, UI_TYPE_IMAGE);
    if (!element) return NULL;

    element->bounds.x = x;
    element->bounds.y = y;
    element->texture = texture;

    if (texture) {
        int w, h;
        SDL_QueryTexture(texture, NULL, NULL, &w, &h);
        element->bounds.w = (float)w;
        element->bounds.h = (float)h;
    }

    return element;
}

UIElement* ui_create_textbox(UIManager* manager, float x, float y, float w, float h,
                            int max_length)
{
    UIElement* element = create_element(manager, UI_TYPE_TEXTBOX);
    if (!element) return NULL;

    element->bounds.x = x;
    element->bounds.y = y;
    element->bounds.w = w;
    element->bounds.h = h;
    element->max_length = max_length > 0 ? max_length : UI_MAX_TEXT;
    element->bg_color = (Color){32, 32, 32, 255};

    return element;
}

UIElement* ui_create_progressbar(UIManager* manager, float x, float y, float w, float h,
                                float max_val)
{
    UIElement* element = create_element(manager, UI_TYPE_PROGRESSBAR);
    if (!element) return NULL;

    element->bounds.x = x;
    element->bounds.y = y;
    element->bounds.w = w;
    element->bounds.h = h;
    element->max_value = max_val;
    element->value = max_val;

    return element;
}

void ui_destroy_element(UIManager* manager, UIElement* element)
{
    int i;

    if (!manager || !element) return;

    for (i = 0; i < manager->element_count; i++) {
        if (manager->elements[i] == element) {
            free(element);
            manager->elements[i] = manager->elements[--manager->element_count];
            break;
        }
    }
}

void ui_set_text(UIElement* element, const char* text)
{
    if (!element) return;
    if (text) {
        strncpy(element->text, text, UI_MAX_TEXT - 1);
        element->text[UI_MAX_TEXT - 1] = '\0';
    } else {
        element->text[0] = '\0';
    }
}

const char* ui_get_text(UIElement* element)
{
    if (!element) return NULL;
    return element->text;
}

void ui_set_value(UIElement* element, float value)
{
    if (!element) return;
    element->value = value;
    if (element->value < 0) element->value = 0;
    if (element->value > element->max_value) element->value = element->max_value;
}

void ui_add_child(UIElement* parent, UIElement* child)
{
    if (!parent || !child) return;
    if (parent->child_count >= UI_MAX_CHILDREN) return;

    parent->children[parent->child_count++] = child;
    child->parent = parent;
}

void ui_set_visible(UIElement* element, bool visible)
{
    if (!element) return;
    element->visible = visible;
}

void ui_set_enabled(UIElement* element, bool enabled)
{
    if (!element) return;
    element->enabled = enabled;
    element->state = enabled ? UI_STATE_NORMAL : UI_STATE_DISABLED;
}

/* Common layouts */

UIElement* ui_create_status_bars(UIManager* manager, float x, float y)
{
    UIElement* panel = ui_create_panel(manager, x, y, 200, 50);
    if (!panel) return NULL;

    /* HP bar */
    UIElement* hp_bar = ui_create_progressbar(manager, x + 10, y + 10, 180, 15, 100);
    if (hp_bar) {
        hp_bar->bg_color = (Color){64, 32, 32, 255};
        ui_add_child(panel, hp_bar);
    }

    /* MP bar */
    UIElement* mp_bar = ui_create_progressbar(manager, x + 10, y + 30, 180, 15, 100);
    if (mp_bar) {
        mp_bar->bg_color = (Color){32, 32, 64, 255};
        ui_add_child(panel, mp_bar);
    }

    return panel;
}

UIElement* ui_create_skill_bar(UIManager* manager, float x, float y)
{
    int i;
    UIElement* panel = ui_create_panel(manager, x, y, 400, 50);
    if (!panel) return NULL;

    for (i = 0; i < 8; i++) {
        UIElement* slot = ui_create_button(manager, x + 10 + i * 48, y + 5, 40, 40,
                                          NULL, NULL, NULL);
        if (slot) {
            slot->bg_color = (Color){48, 48, 64, 255};
            ui_add_child(panel, slot);
        }
    }

    return panel;
}
