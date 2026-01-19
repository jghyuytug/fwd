/**
 * @file ani_loader.h
 * @brief DNF ANI Animation Loader
 *
 * ANI is DNF's animation definition format stored in PVF archives.
 * It defines frame sequences, timing, hitboxes, and linked resources.
 *
 * ANI file structure (reverse engineered):
 * - Header: magic, version, frame count
 * - Frame definitions: IMG references, timing, offsets
 * - Hitbox data: collision and attack boxes per frame
 * - Event triggers: sound, effect, etc.
 *
 * @date 2026-01-04
 */

#ifndef ANI_LOADER_H
#define ANI_LOADER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* Forward from PVF library */
struct PackSetInternal;
typedef struct PackSetInternal PackSetInternal;

/* ========================================================================== */
/* Constants                                                                   */
/* ========================================================================== */

/* ANI magic signatures */
#define ANI_MAGIC               "NEOPLE ANI"
#define ANI_MAGIC_V1            0x414E4931  /* "ANI1" */
#define ANI_MAGIC_V2            0x414E4932  /* "ANI2" */

/* ANI versions */
#define ANI_VERSION_1           1
#define ANI_VERSION_2           2
#define ANI_VERSION_3           3

/* Limits */
#define ANI_MAX_FRAMES          256
#define ANI_MAX_ACTIONS         64
#define ANI_MAX_HITBOXES        16
#define ANI_MAX_EVENTS          32
#define ANI_MAX_IMG_PATH        256
#define ANI_MAX_EFFECT_PATH     256

/* Return codes */
#define ANI_SUCCESS             0
#define ANI_ERROR_INVALID_MAGIC -1
#define ANI_ERROR_UNSUPPORTED   -2
#define ANI_ERROR_MEMORY        -3
#define ANI_ERROR_PARSE         -4
#define ANI_ERROR_NOT_FOUND     -5
#define ANI_ERROR_PVF           -6
#define ANI_ERROR_INVALID       -7

/* Hitbox types */
typedef enum ANIHitboxType {
    HITBOX_NONE = 0,
    HITBOX_COLLISION,       /* Body collision box */
    HITBOX_ATTACK,          /* Attack damage box */
    HITBOX_GRAB,            /* Grab/throw box */
    HITBOX_INVINCIBLE,      /* Invincibility frames */
    HITBOX_SUPER_ARMOR,     /* Super armor frames */
    HITBOX_COUNT
} ANIHitboxType;

/* Event types */
typedef enum ANIEventType {
    EVENT_NONE = 0,
    EVENT_SOUND,            /* Play sound effect */
    EVENT_EFFECT,           /* Spawn visual effect */
    EVENT_PROJECTILE,       /* Spawn projectile */
    EVENT_DAMAGE,           /* Damage tick */
    EVENT_GRAB,             /* Grab enemy */
    EVENT_MOVE,             /* Move character */
    EVENT_CANCEL,           /* Can cancel into other action */
    EVENT_LOOP,             /* Loop back to frame */
    EVENT_END,              /* End animation */
    EVENT_COUNT
} ANIEventType;

/* Blend modes */
typedef enum ANIBlendMode {
    BLEND_NORMAL = 0,
    BLEND_ADDITIVE,
    BLEND_MULTIPLY,
    BLEND_SCREEN,
    BLEND_COUNT
} ANIBlendMode;

/* ========================================================================== */
/* Data Structures                                                             */
/* ========================================================================== */

/**
 * @brief Rectangle (integer)
 */
typedef struct ANIRect {
    int16_t x;
    int16_t y;
    int16_t w;
    int16_t h;
} ANIRect;

/**
 * @brief Point (integer)
 */
typedef struct ANIPoint {
    int16_t x;
    int16_t y;
} ANIPoint;

/**
 * @brief Hitbox definition
 */
typedef struct ANIHitbox {
    ANIHitboxType   type;           /* Hitbox type */
    ANIRect         rect;           /* Hitbox rectangle */
    int             damage;         /* Damage (for attack boxes) */
    int             hitstun;        /* Hitstun frames */
    int             knockback_x;    /* Knockback X velocity */
    int             knockback_y;    /* Knockback Y velocity */
    int             element;        /* Damage element type */
} ANIHitbox;

/**
 * @brief Animation event
 */
typedef struct ANIEvent {
    ANIEventType    type;           /* Event type */
    int             frame;          /* Frame to trigger */
    union {
        /* Sound event */
        struct {
            char    path[64];       /* Sound file path */
            float   volume;         /* Volume (0.0-1.0) */
            bool    loop;           /* Loop sound */
        } sound;

        /* Effect event */
        struct {
            char    path[64];       /* Effect ANI path */
            int     offset_x;       /* X offset */
            int     offset_y;       /* Y offset */
            bool    follow;         /* Follow parent */
        } effect;

        /* Projectile event */
        struct {
            char    path[64];       /* Projectile ANI path */
            int     offset_x;       /* Spawn X offset */
            int     offset_y;       /* Spawn Y offset */
            int     velocity_x;     /* X velocity */
            int     velocity_y;     /* Y velocity */
            int     damage;         /* Projectile damage */
        } projectile;

        /* Move event */
        struct {
            int     delta_x;        /* X movement */
            int     delta_y;        /* Y movement */
        } move;

        /* Cancel event */
        struct {
            int     cancel_window;  /* Frames to cancel */
            int     allowed_actions;/* Bitmask of allowed actions */
        } cancel;

        /* Loop event */
        struct {
            int     target_frame;   /* Frame to loop to */
            int     loop_count;     /* -1 for infinite */
        } loop;
    } data;
} ANIEvent;

/**
 * @brief Single animation frame
 */
typedef struct ANIFrame {
    /* Image reference */
    char            img_path[ANI_MAX_IMG_PATH]; /* IMG file path in NPK */
    int             img_index;      /* Frame index in IMG */

    /* Timing */
    uint16_t        duration;       /* Frame duration (ms) */
    uint16_t        delay;          /* Delay before frame (ms) */

    /* Position offsets */
    ANIPoint        offset;         /* Render offset */
    ANIPoint        anchor;         /* Anchor point (pivot) */

    /* Transform */
    float           scale_x;        /* X scale */
    float           scale_y;        /* Y scale */
    float           rotation;       /* Rotation (degrees) */
    uint8_t         alpha;          /* Alpha (0-255) */
    ANIBlendMode    blend;          /* Blend mode */

    /* Hitboxes */
    ANIHitbox       hitboxes[ANI_MAX_HITBOXES];
    int             hitbox_count;

    /* Flags */
    bool            is_key_frame;   /* True if keyframe */
    bool            can_cancel;     /* Can cancel here */
    bool            is_loop_point;  /* Loop start point */
    bool            flip_x;         /* Flip horizontally */
    bool            flip_y;         /* Flip vertically */
} ANIFrame;

/**
 * @brief Animation action (sequence of frames)
 */
typedef struct ANIAction {
    char            name[64];       /* Action name (e.g., "idle", "attack1") */
    int             id;             /* Action ID */

    /* Frame data */
    ANIFrame*       frames;         /* Frame array */
    int             frame_count;    /* Number of frames */

    /* Properties */
    bool            loop;           /* Loop animation */
    int             loop_start;     /* Frame to loop from */
    float           speed;          /* Playback speed multiplier */

    /* Events */
    ANIEvent*       events;         /* Event array */
    int             event_count;    /* Number of events */

    /* Linked resources */
    char            npk_name[64];   /* Associated NPK file */
    char            img_path[ANI_MAX_IMG_PATH]; /* Default IMG path */
} ANIAction;

/**
 * @brief ANI file definition
 */
typedef struct ANIDefinition {
    /* Header info */
    uint32_t        version;        /* ANI version */
    char            name[64];       /* Animation set name */

    /* Actions */
    ANIAction*      actions;        /* Action array */
    int             action_count;   /* Number of actions */

    /* Global properties */
    char            default_npk[64];    /* Default NPK file */
    int             default_action;     /* Default action ID */
    ANIPoint        base_offset;        /* Base render offset */

    /* Shadow/ground */
    bool            has_shadow;     /* Draw shadow */
    ANIRect         shadow_rect;    /* Shadow rectangle */

    /* Reference counting */
    int             ref_count;      /* Reference count */
} ANIDefinition;

/**
 * @brief Animation instance (runtime state)
 */
typedef struct ANIInstance {
    ANIDefinition*  definition;     /* Animation definition */
    ANIAction*      current_action; /* Current action */

    /* Playback state */
    int             current_frame;  /* Current frame index */
    float           frame_timer;    /* Time in current frame (ms) */
    float           speed;          /* Playback speed multiplier */
    bool            playing;        /* Is playing */
    bool            paused;         /* Is paused */
    bool            finished;       /* Animation finished */
    int             loop_count;     /* Current loop count */

    /* Event tracking */
    int             last_event;     /* Last processed event index */

    /* Transform (instance override) */
    bool            flip_x;         /* Flip horizontally */
    bool            flip_y;         /* Flip vertically */
    uint8_t         alpha;          /* Alpha override (255 = use frame) */
    float           scale;          /* Scale override (1.0 = use frame) */

    /* Callbacks */
    void*           user_data;      /* User data for callbacks */
    void            (*on_event)(struct ANIInstance*, ANIEvent*);
    void            (*on_frame_change)(struct ANIInstance*, int old_frame, int new_frame);
    void            (*on_action_end)(struct ANIInstance*);
} ANIInstance;

/* ========================================================================== */
/* ANI Loading API                                                             */
/* ========================================================================== */

/**
 * @brief Load ANI definition from PVF
 *
 * @param pvf           PVF pack handle
 * @param ani_path      Path to ANI file in PVF
 * @param out_def       Output: ANI definition
 *
 * @return ANI_SUCCESS on success, error code otherwise
 */
int ANI_LoadFromPVF(PackSetInternal* pvf, const char* ani_path,
                    ANIDefinition** out_def);

/**
 * @brief Load ANI definition from memory
 *
 * @param data          ANI file data
 * @param size          Data size
 * @param out_def       Output: ANI definition
 *
 * @return ANI_SUCCESS on success, error code otherwise
 */
int ANI_LoadFromMemory(const void* data, size_t size,
                       ANIDefinition** out_def);

/**
 * @brief Free ANI definition
 *
 * @param def           ANI definition to free
 */
void ANI_FreeDefinition(ANIDefinition* def);

/**
 * @brief Add reference to ANI definition
 *
 * @param def           ANI definition
 */
void ANI_AddRef(ANIDefinition* def);

/**
 * @brief Release reference to ANI definition
 *
 * @param def           ANI definition
 * @return true if definition was freed
 */
bool ANI_Release(ANIDefinition* def);

/* ========================================================================== */
/* ANI Instance API                                                            */
/* ========================================================================== */

/**
 * @brief Create animation instance
 *
 * @param def           ANI definition
 *
 * @return ANIInstance* or NULL on error
 */
ANIInstance* ANI_CreateInstance(ANIDefinition* def);

/**
 * @brief Free animation instance
 *
 * @param inst          Instance to free
 */
void ANI_FreeInstance(ANIInstance* inst);

/**
 * @brief Update animation instance
 *
 * @param inst          Instance to update
 * @param delta_ms      Delta time in milliseconds
 */
void ANI_Update(ANIInstance* inst, float delta_ms);

/**
 * @brief Set current action by name
 *
 * @param inst          Instance
 * @param action_name   Action name
 *
 * @return ANI_SUCCESS on success
 */
int ANI_SetAction(ANIInstance* inst, const char* action_name);

/**
 * @brief Set current action by ID
 *
 * @param inst          Instance
 * @param action_id     Action ID
 *
 * @return ANI_SUCCESS on success
 */
int ANI_SetActionById(ANIInstance* inst, int action_id);

/**
 * @brief Play animation
 *
 * @param inst          Instance
 */
void ANI_Play(ANIInstance* inst);

/**
 * @brief Pause animation
 *
 * @param inst          Instance
 */
void ANI_Pause(ANIInstance* inst);

/**
 * @brief Stop animation (reset to frame 0)
 *
 * @param inst          Instance
 */
void ANI_Stop(ANIInstance* inst);

/**
 * @brief Reset animation
 *
 * @param inst          Instance
 */
void ANI_Reset(ANIInstance* inst);

/**
 * @brief Set playback speed
 *
 * @param inst          Instance
 * @param speed         Speed multiplier (1.0 = normal)
 */
void ANI_SetSpeed(ANIInstance* inst, float speed);

/**
 * @brief Go to specific frame
 *
 * @param inst          Instance
 * @param frame         Frame index
 */
void ANI_GotoFrame(ANIInstance* inst, int frame);

/* ========================================================================== */
/* ANI Query API                                                               */
/* ========================================================================== */

/**
 * @brief Get current frame
 *
 * @param inst          Instance
 * @return Current frame pointer or NULL
 */
ANIFrame* ANI_GetCurrentFrame(ANIInstance* inst);

/**
 * @brief Get frame by index
 *
 * @param inst          Instance
 * @param index         Frame index
 * @return Frame pointer or NULL
 */
ANIFrame* ANI_GetFrame(ANIInstance* inst, int index);

/**
 * @brief Get action by name
 *
 * @param def           Definition
 * @param name          Action name
 * @return Action pointer or NULL
 */
ANIAction* ANI_GetActionByName(ANIDefinition* def, const char* name);

/**
 * @brief Get action by ID
 *
 * @param def           Definition
 * @param id            Action ID
 * @return Action pointer or NULL
 */
ANIAction* ANI_GetActionById(ANIDefinition* def, int id);

/**
 * @brief Get total animation duration
 *
 * @param action        Action to measure
 * @return Duration in milliseconds
 */
int ANI_GetDuration(ANIAction* action);

/**
 * @brief Check if animation is playing
 *
 * @param inst          Instance
 * @return true if playing
 */
bool ANI_IsPlaying(ANIInstance* inst);

/**
 * @brief Check if animation finished
 *
 * @param inst          Instance
 * @return true if finished (non-looping only)
 */
bool ANI_IsFinished(ANIInstance* inst);

/**
 * @brief Get hitboxes for current frame
 *
 * @param inst          Instance
 * @param out_boxes     Output: hitbox array
 * @param max_boxes     Maximum boxes to return
 * @return Number of hitboxes
 */
int ANI_GetHitboxes(ANIInstance* inst, ANIHitbox* out_boxes, int max_boxes);

/**
 * @brief Check if current frame can cancel
 *
 * @param inst          Instance
 * @return true if cancellable
 */
bool ANI_CanCancel(ANIInstance* inst);

/* ========================================================================== */
/* Utility Functions                                                           */
/* ========================================================================== */

/**
 * @brief Get hitbox type name
 *
 * @param type          Hitbox type
 * @return String name
 */
const char* ANI_GetHitboxTypeName(ANIHitboxType type);

/**
 * @brief Get event type name
 *
 * @param type          Event type
 * @return String name
 */
const char* ANI_GetEventTypeName(ANIEventType type);

/**
 * @brief Print ANI info (for debugging)
 *
 * @param def           ANI definition
 */
void ANI_PrintInfo(ANIDefinition* def);

/**
 * @brief Print frame info (for debugging)
 *
 * @param frame         Frame to print
 */
void ANI_PrintFrame(ANIFrame* frame);

#ifdef __cplusplus
}
#endif

#endif /* ANI_LOADER_H */
