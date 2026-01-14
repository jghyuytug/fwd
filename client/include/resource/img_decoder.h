/**
 * @file img_decoder.h
 * @brief DNF IMG Image Decoder
 *
 * IMG is DNF's sprite image format, stored inside NPK archives.
 * Multiple versions exist:
 * - IMGV1: Early version, rarely used
 * - IMGV2: Most common, used for UI/icons/titles (indexed color)
 * - IMGV4: Compressed sprites
 * - IMGV5: DDS-based, used for characters/effects
 * - IMGV6: Latest version with additional features
 *
 * @date 2026-01-04
 */

#ifndef IMG_DECODER_H
#define IMG_DECODER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* Forward declarations */
struct SDL_Renderer;
struct SDL_Texture;
struct SDL_Surface;
typedef struct SDL_Renderer SDL_Renderer;
typedef struct SDL_Texture SDL_Texture;
typedef struct SDL_Surface SDL_Surface;

/* ========================================================================== */
/* Constants                                                                   */
/* ========================================================================== */

/* IMG magic signature */
#define IMG_MAGIC               "Neople Image File"
#define IMG_MAGIC_ALT           "Neople Img File"

/* IMG versions */
#define IMG_VERSION_1           1
#define IMG_VERSION_2           2
#define IMG_VERSION_4           4
#define IMG_VERSION_5           5
#define IMG_VERSION_6           6

/* Color formats */
#define IMG_COLOR_ARGB1555      0x0E    /* 16-bit ARGB 1555 */
#define IMG_COLOR_ARGB4444      0x0F    /* 16-bit ARGB 4444 */
#define IMG_COLOR_ARGB8888      0x10    /* 32-bit ARGB 8888 */
#define IMG_COLOR_INDEX8        0x11    /* 8-bit indexed (palette) */
#define IMG_COLOR_DDS_DXT1      0x12    /* DDS DXT1 compressed */
#define IMG_COLOR_DDS_DXT3      0x13    /* DDS DXT3 compressed */
#define IMG_COLOR_DDS_DXT5      0x14    /* DDS DXT5 compressed */

/* Compression flags (based on Taiwan DNF analysis) */
#define IMG_COMPRESS_NONE       0x05    /* Raw/uncompressed */
#define IMG_COMPRESS_ZLIB       0x06    /* zlib compressed (78 9C header) */
#define IMG_COMPRESS_LINK       0x00    /* Link frame (no data) */

/* IMG header size */
#define IMG_HEADER_SIZE         32
#define IMG_ENTRY_SIZE          36

/* Return codes */
#define IMG_SUCCESS              0
#define IMG_ERROR_INVALID_MAGIC  -1
#define IMG_ERROR_UNSUPPORTED    -2
#define IMG_ERROR_MEMORY         -3
#define IMG_ERROR_DECOMPRESS     -4
#define IMG_ERROR_INVALID_FRAME  -5
#define IMG_ERROR_INVALID_PARAM  -6

/* Limits */
#define IMG_MAX_FRAMES          4096
#define IMG_MAX_SIZE            8192

/* ========================================================================== */
/* Data Structures                                                             */
/* ========================================================================== */

/**
 * @brief Color palette (for 8-bit indexed images)
 */
typedef struct IMGPalette {
    uint8_t     colors[256][4]; /* RGBA for each index */
    int         count;          /* Number of colors */
} IMGPalette;

/**
 * @brief Single frame information
 */
typedef struct IMGFrame {
    uint32_t    color_format;   /* Color format (IMG_COLOR_*) */
    uint32_t    compress;       /* Compression type */
    int         width;          /* Frame width */
    int         height;         /* Frame height */
    int         offset_x;       /* X offset (for positioning) */
    int         offset_y;       /* Y offset (for positioning) */
    int         max_width;      /* Maximum/canvas width */
    int         max_height;     /* Maximum/canvas height */
    uint32_t    data_offset;    /* Offset in IMG data */
    uint32_t    data_size;      /* Compressed/raw data size */
    uint32_t    raw_size;       /* Decompressed size */

    /* Link frames (for reference frames) */
    int         link_index;     /* Index of linked frame, -1 if none */
    bool        is_link;        /* True if this is a link frame */
} IMGFrame;

/**
 * @brief IMG file handle
 */
typedef struct IMGFile {
    uint32_t    version;        /* IMG version (1,2,4,5,6) */
    uint32_t    frame_count;    /* Number of frames */
    int         max_width;      /* Maximum frame width */
    int         max_height;     /* Maximum frame height */

    /* Frames */
    IMGFrame*   frames;         /* Frame array */

    /* Palette (for indexed images) */
    IMGPalette  palette;        /* Global palette */
    bool        has_palette;    /* True if has global palette */

    /* Raw data reference */
    const uint8_t*  raw_data;   /* Original IMG data */
    size_t          raw_size;   /* Original data size */
} IMGFile;

/**
 * @brief Decoded frame pixels
 */
typedef struct IMGPixels {
    uint32_t*   data;           /* RGBA8888 pixel data */
    int         width;          /* Pixel width */
    int         height;         /* Pixel height */
    int         offset_x;       /* X offset */
    int         offset_y;       /* Y offset */
    size_t      data_size;      /* data array size in bytes */
} IMGPixels;

/* ========================================================================== */
/* Core API                                                                    */
/* ========================================================================== */

/**
 * @brief Parse IMG data
 *
 * @param data      IMG file data (from NPK)
 * @param size      Data size
 * @param img       Output: parsed IMG structure
 *
 * @return IMG_SUCCESS on success, error code otherwise
 */
int IMG_Parse(const void* data, size_t size, IMGFile* img);

/**
 * @brief Free IMG resources
 *
 * @param img       IMG handle to free
 */
void IMG_Free(IMGFile* img);

/**
 * @brief Get frame count
 *
 * @param img       IMG handle
 * @return Number of frames, 0 if invalid
 */
uint32_t IMG_GetFrameCount(IMGFile* img);

/**
 * @brief Get frame info
 *
 * @param img       IMG handle
 * @param index     Frame index (0-based)
 *
 * @return Pointer to frame info, NULL if invalid
 */
IMGFrame* IMG_GetFrame(IMGFile* img, uint32_t index);

/* ========================================================================== */
/* Decoding API                                                                */
/* ========================================================================== */

/**
 * @brief Decode a frame to RGBA pixels
 *
 * @param img       IMG handle
 * @param index     Frame index
 * @param pixels    Output: decoded pixels (caller frees with IMG_FreePixels)
 *
 * @return IMG_SUCCESS on success, error code otherwise
 */
int IMG_DecodeFrame(IMGFile* img, uint32_t index, IMGPixels* pixels);

/**
 * @brief Free decoded pixels
 *
 * @param pixels    Pixels to free
 */
void IMG_FreePixels(IMGPixels* pixels);

/**
 * @brief Decode frame to SDL_Surface
 *
 * @param img       IMG handle
 * @param index     Frame index
 *
 * @return SDL_Surface* or NULL on error (caller must SDL_FreeSurface)
 */
SDL_Surface* IMG_DecodeToSurface(IMGFile* img, uint32_t index);

/**
 * @brief Create SDL_Texture from frame
 *
 * @param renderer  SDL renderer
 * @param img       IMG handle
 * @param index     Frame index
 *
 * @return SDL_Texture* or NULL on error (caller must SDL_DestroyTexture)
 */
SDL_Texture* IMG_CreateTexture(SDL_Renderer* renderer, IMGFile* img, uint32_t index);

/* ========================================================================== */
/* Sprite Sheet API                                                            */
/* ========================================================================== */

/**
 * @brief Sprite sheet (all frames packed into one texture)
 */
typedef struct IMGSpriteSheet {
    SDL_Texture*    texture;        /* Combined texture */
    int             texture_width;  /* Texture width */
    int             texture_height; /* Texture height */
    uint32_t        frame_count;    /* Number of frames */

    /* Per-frame rectangles */
    struct {
        int x, y, w, h;             /* Position and size in texture */
        int offset_x, offset_y;     /* Offset for rendering */
    }*              frame_rects;
} IMGSpriteSheet;

/**
 * @brief Create sprite sheet from IMG
 *
 * @param renderer  SDL renderer
 * @param img       IMG handle
 *
 * @return IMGSpriteSheet* or NULL on error (caller frees with IMG_FreeSpriteSheet)
 */
IMGSpriteSheet* IMG_CreateSpriteSheet(SDL_Renderer* renderer, IMGFile* img);

/**
 * @brief Free sprite sheet
 *
 * @param sheet     Sprite sheet to free
 */
void IMG_FreeSpriteSheet(IMGSpriteSheet* sheet);

/* ========================================================================== */
/* Utility Functions                                                           */
/* ========================================================================== */

/**
 * @brief Get color format name
 *
 * @param format    Color format code
 * @return String name (e.g., "ARGB8888")
 */
const char* IMG_GetColorFormatName(uint32_t format);

/**
 * @brief Check if IMG version is supported
 *
 * @param version   Version number
 * @return true if supported
 */
bool IMG_IsVersionSupported(uint32_t version);

/**
 * @brief Print IMG info (for debugging)
 *
 * @param img       IMG handle
 */
void IMG_PrintInfo(IMGFile* img);

#ifdef __cplusplus
}
#endif

#endif /* IMG_DECODER_H */
