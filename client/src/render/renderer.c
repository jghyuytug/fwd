/**
 * @file renderer.c
 * @brief SDL2 Renderer implementation
 */

#include "../../include/renderer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int renderer_init(Renderer* renderer, const char* title, int width, int height)
{
    if (!renderer || !title) {
        return CLIENT_ERROR;
    }

    memset(renderer, 0, sizeof(Renderer));

    /* Create window */
    renderer->window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );

    if (!renderer->window) {
        fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        return CLIENT_ERROR_SDL;
    }

    /* Create renderer */
    renderer->sdl_renderer = SDL_CreateRenderer(
        renderer->window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!renderer->sdl_renderer) {
        fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(renderer->window);
        return CLIENT_ERROR_SDL;
    }

    renderer->width = width;
    renderer->height = height;
    renderer->scale_x = 1.0f;
    renderer->scale_y = 1.0f;
    renderer->camera.x = 0;
    renderer->camera.y = 0;

    /* Set blend mode */
    SDL_SetRenderDrawBlendMode(renderer->sdl_renderer, SDL_BLENDMODE_BLEND);

    printf("Renderer initialized: %dx%d\n", width, height);

    return CLIENT_OK;
}

void renderer_cleanup(Renderer* renderer)
{
    if (!renderer) return;

    if (renderer->sdl_renderer) {
        SDL_DestroyRenderer(renderer->sdl_renderer);
        renderer->sdl_renderer = NULL;
    }

    if (renderer->window) {
        SDL_DestroyWindow(renderer->window);
        renderer->window = NULL;
    }
}

void renderer_begin(Renderer* renderer)
{
    (void)renderer;
    /* Nothing to do - clear is done separately */
}

void renderer_end(Renderer* renderer)
{
    if (!renderer || !renderer->sdl_renderer) return;
    SDL_RenderPresent(renderer->sdl_renderer);
}

void renderer_clear(Renderer* renderer, Color color)
{
    if (!renderer || !renderer->sdl_renderer) return;

    SDL_SetRenderDrawColor(renderer->sdl_renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(renderer->sdl_renderer);
}

void renderer_set_camera(Renderer* renderer, float x, float y)
{
    if (!renderer) return;
    renderer->camera.x = x;
    renderer->camera.y = y;
}

SDL_Texture* renderer_load_texture(Renderer* renderer, const char* path)
{
    SDL_Surface* surface;
    SDL_Texture* texture;

    if (!renderer || !path) return NULL;

    surface = IMG_Load(path);
    if (!surface) {
        fprintf(stderr, "Failed to load image: %s - %s\n", path, IMG_GetError());
        return NULL;
    }

    texture = SDL_CreateTextureFromSurface(renderer->sdl_renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        fprintf(stderr, "Failed to create texture: %s\n", SDL_GetError());
        return NULL;
    }

    return texture;
}

void renderer_draw_texture(Renderer* renderer, SDL_Texture* texture,
                          SDL_Rect* src, SDL_Rect* dst, bool flip)
{
    SDL_RendererFlip sdl_flip;

    if (!renderer || !renderer->sdl_renderer || !texture) return;

    sdl_flip = flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

    /* Apply camera offset */
    if (dst) {
        SDL_Rect adjusted = *dst;
        adjusted.x -= (int)renderer->camera.x;
        adjusted.y -= (int)renderer->camera.y;
        SDL_RenderCopyEx(renderer->sdl_renderer, texture, src, &adjusted, 0, NULL, sdl_flip);
    } else {
        SDL_RenderCopyEx(renderer->sdl_renderer, texture, src, NULL, 0, NULL, sdl_flip);
    }
}

void renderer_draw_rect(Renderer* renderer, Rect rect, Color color, bool filled)
{
    SDL_Rect sdl_rect;

    if (!renderer || !renderer->sdl_renderer) return;

    sdl_rect.x = (int)(rect.x - renderer->camera.x);
    sdl_rect.y = (int)(rect.y - renderer->camera.y);
    sdl_rect.w = (int)rect.w;
    sdl_rect.h = (int)rect.h;

    SDL_SetRenderDrawColor(renderer->sdl_renderer, color.r, color.g, color.b, color.a);

    if (filled) {
        SDL_RenderFillRect(renderer->sdl_renderer, &sdl_rect);
    } else {
        SDL_RenderDrawRect(renderer->sdl_renderer, &sdl_rect);
    }
}

void renderer_draw_line(Renderer* renderer, int x1, int y1, int x2, int y2, Color color)
{
    if (!renderer || !renderer->sdl_renderer) return;

    SDL_SetRenderDrawColor(renderer->sdl_renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawLine(renderer->sdl_renderer,
                      x1 - (int)renderer->camera.x,
                      y1 - (int)renderer->camera.y,
                      x2 - (int)renderer->camera.x,
                      y2 - (int)renderer->camera.y);
}

/*
 * Sprite functions
 */

Sprite* sprite_load(Renderer* renderer, const char* path, int frame_w, int frame_h)
{
    Sprite* sprite;
    int w, h;

    if (!renderer || !path) return NULL;

    sprite = calloc(1, sizeof(Sprite));
    if (!sprite) return NULL;

    sprite->texture = renderer_load_texture(renderer, path);
    if (!sprite->texture) {
        free(sprite);
        return NULL;
    }

    SDL_QueryTexture(sprite->texture, NULL, NULL, &w, &h);
    sprite->width = w;
    sprite->height = h;

    if (frame_w > 0 && frame_h > 0) {
        sprite->frame_w = frame_w;
        sprite->frame_h = frame_h;
        sprite->frames_x = w / frame_w;
        sprite->frames_y = h / frame_h;
    } else {
        sprite->frame_w = w;
        sprite->frame_h = h;
        sprite->frames_x = 1;
        sprite->frames_y = 1;
    }

    return sprite;
}

void sprite_free(Sprite* sprite)
{
    if (!sprite) return;

    if (sprite->texture) {
        SDL_DestroyTexture(sprite->texture);
    }

    free(sprite);
}

void sprite_draw(Renderer* renderer, Sprite* sprite, int frame,
                float x, float y, bool flip)
{
    SDL_Rect src, dst;
    int fx, fy;

    if (!renderer || !sprite) return;

    /* Calculate frame position */
    fx = frame % sprite->frames_x;
    fy = frame / sprite->frames_x;

    src.x = fx * sprite->frame_w;
    src.y = fy * sprite->frame_h;
    src.w = sprite->frame_w;
    src.h = sprite->frame_h;

    dst.x = (int)x;
    dst.y = (int)y;
    dst.w = sprite->frame_w;
    dst.h = sprite->frame_h;

    renderer_draw_texture(renderer, sprite->texture, &src, &dst, flip);
}

void sprite_draw_scaled(Renderer* renderer, Sprite* sprite, int frame,
                       float x, float y, float scale, bool flip)
{
    SDL_Rect src, dst;
    int fx, fy;

    if (!renderer || !sprite) return;

    fx = frame % sprite->frames_x;
    fy = frame / sprite->frames_x;

    src.x = fx * sprite->frame_w;
    src.y = fy * sprite->frame_h;
    src.w = sprite->frame_w;
    src.h = sprite->frame_h;

    dst.x = (int)x;
    dst.y = (int)y;
    dst.w = (int)(sprite->frame_w * scale);
    dst.h = (int)(sprite->frame_h * scale);

    renderer_draw_texture(renderer, sprite->texture, &src, &dst, flip);
}

/*
 * Animation functions
 */

Animation* animation_create(Sprite* sprite, int* frames, int frame_count,
                           float frame_time, bool loop)
{
    Animation* anim;

    if (!sprite || !frames || frame_count <= 0) return NULL;

    anim = calloc(1, sizeof(Animation));
    if (!anim) return NULL;

    anim->frames = malloc(sizeof(int) * frame_count);
    if (!anim->frames) {
        free(anim);
        return NULL;
    }

    memcpy(anim->frames, frames, sizeof(int) * frame_count);
    anim->sprite = sprite;
    anim->frame_count = frame_count;
    anim->frame_time = frame_time;
    anim->loop = loop;
    anim->timer = 0;
    anim->current = 0;
    anim->finished = false;

    return anim;
}

void animation_free(Animation* anim)
{
    if (!anim) return;

    if (anim->frames) {
        free(anim->frames);
    }

    free(anim);
}

void animation_update(Animation* anim, float delta)
{
    if (!anim || anim->finished) return;

    anim->timer += delta;

    while (anim->timer >= anim->frame_time) {
        anim->timer -= anim->frame_time;
        anim->current++;

        if (anim->current >= anim->frame_count) {
            if (anim->loop) {
                anim->current = 0;
            } else {
                anim->current = anim->frame_count - 1;
                anim->finished = true;
            }
        }
    }
}

void animation_reset(Animation* anim)
{
    if (!anim) return;

    anim->timer = 0;
    anim->current = 0;
    anim->finished = false;
}

void animation_draw(Renderer* renderer, Animation* anim,
                   float x, float y, bool flip)
{
    int frame;

    if (!renderer || !anim || !anim->sprite) return;

    frame = anim->frames[anim->current];
    sprite_draw(renderer, anim->sprite, frame, x, y, flip);
}
