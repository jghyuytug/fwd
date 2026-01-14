/**
 * @file renderer.h
 * @brief SDL2 Renderer wrapper
 */

#ifndef CLIENT_RENDERER_H
#define CLIENT_RENDERER_H

#include "client.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Sprite sheet */
typedef struct Sprite {
    SDL_Texture*    texture;
    int             width;
    int             height;
    int             frame_w;
    int             frame_h;
    int             frames_x;
    int             frames_y;
} Sprite;

/* Animation */
typedef struct Animation {
    Sprite*         sprite;
    int*            frames;
    int             frame_count;
    float           frame_time;
    float           timer;
    int             current;
    bool            loop;
    bool            finished;
} Animation;

/* Renderer context */
typedef struct Renderer {
    SDL_Window*     window;
    SDL_Renderer*   sdl_renderer;
    int             width;
    int             height;
    float           scale_x;
    float           scale_y;
    Vec2            camera;
    bool            fullscreen;
} Renderer;

/**
 * Initialize renderer
 * @param renderer Renderer instance
 * @param title    Window title
 * @param width    Window width
 * @param height   Window height
 * @return CLIENT_OK on success
 */
int renderer_init(Renderer* renderer, const char* title, int width, int height);

/**
 * Cleanup renderer
 * @param renderer Renderer instance
 */
void renderer_cleanup(Renderer* renderer);

/**
 * Begin frame
 * @param renderer Renderer instance
 */
void renderer_begin(Renderer* renderer);

/**
 * End frame (present)
 * @param renderer Renderer instance
 */
void renderer_end(Renderer* renderer);

/**
 * Clear screen
 * @param renderer Renderer instance
 * @param color    Clear color
 */
void renderer_clear(Renderer* renderer, Color color);

/**
 * Set camera position
 * @param renderer Renderer instance
 * @param x        Camera X
 * @param y        Camera Y
 */
void renderer_set_camera(Renderer* renderer, float x, float y);

/**
 * Load texture from file
 * @param renderer Renderer instance
 * @param path     File path
 * @return SDL_Texture or NULL
 */
SDL_Texture* renderer_load_texture(Renderer* renderer, const char* path);

/**
 * Draw texture
 * @param renderer Renderer instance
 * @param texture  Texture
 * @param src      Source rect (NULL for whole)
 * @param dst      Destination rect
 * @param flip     Flip horizontally
 */
void renderer_draw_texture(Renderer* renderer, SDL_Texture* texture,
                          SDL_Rect* src, SDL_Rect* dst, bool flip);

/**
 * Draw rectangle
 * @param renderer Renderer instance
 * @param rect     Rectangle
 * @param color    Color
 * @param filled   Fill or outline
 */
void renderer_draw_rect(Renderer* renderer, Rect rect, Color color, bool filled);

/**
 * Draw line
 * @param renderer Renderer instance
 * @param x1       Start X
 * @param y1       Start Y
 * @param x2       End X
 * @param y2       End Y
 * @param color    Color
 */
void renderer_draw_line(Renderer* renderer, int x1, int y1, int x2, int y2, Color color);

/*
 * Sprite functions
 */

/**
 * Load sprite from file
 * @param renderer  Renderer instance
 * @param path      File path
 * @param frame_w   Frame width (0 for whole image)
 * @param frame_h   Frame height (0 for whole image)
 * @return Sprite or NULL
 */
Sprite* sprite_load(Renderer* renderer, const char* path, int frame_w, int frame_h);

/**
 * Free sprite
 * @param sprite Sprite instance
 */
void sprite_free(Sprite* sprite);

/**
 * Draw sprite frame
 * @param renderer Renderer instance
 * @param sprite   Sprite
 * @param frame    Frame index
 * @param x        X position
 * @param y        Y position
 * @param flip     Flip horizontally
 */
void sprite_draw(Renderer* renderer, Sprite* sprite, int frame,
                float x, float y, bool flip);

/**
 * Draw sprite frame scaled
 * @param renderer Renderer instance
 * @param sprite   Sprite
 * @param frame    Frame index
 * @param x        X position
 * @param y        Y position
 * @param scale    Scale factor
 * @param flip     Flip horizontally
 */
void sprite_draw_scaled(Renderer* renderer, Sprite* sprite, int frame,
                       float x, float y, float scale, bool flip);

/*
 * Animation functions
 */

/**
 * Create animation
 * @param sprite      Sprite sheet
 * @param frames      Frame indices
 * @param frame_count Number of frames
 * @param frame_time  Time per frame
 * @param loop        Loop animation
 * @return Animation or NULL
 */
Animation* animation_create(Sprite* sprite, int* frames, int frame_count,
                           float frame_time, bool loop);

/**
 * Free animation
 * @param anim Animation instance
 */
void animation_free(Animation* anim);

/**
 * Update animation
 * @param anim  Animation instance
 * @param delta Delta time
 */
void animation_update(Animation* anim, float delta);

/**
 * Reset animation
 * @param anim Animation instance
 */
void animation_reset(Animation* anim);

/**
 * Draw animation
 * @param renderer Renderer instance
 * @param anim     Animation
 * @param x        X position
 * @param y        Y position
 * @param flip     Flip horizontally
 */
void animation_draw(Renderer* renderer, Animation* anim,
                   float x, float y, bool flip);

#ifdef __cplusplus
}
#endif

#endif /* CLIENT_RENDERER_H */
