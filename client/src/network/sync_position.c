/**
 * @file sync_position.c
 * @brief Position synchronization with interpolation and prediction
 */

#include "../../include/client.h"
#include "../../include/network.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/*
 * ============================================================================
 * Constants
 * ============================================================================
 */

#define MAX_SYNC_ENTITIES       128
#define POSITION_HISTORY_SIZE   20
#define INTERPOLATION_DELAY     100     /* ms - how far behind we render */
#define EXTRAPOLATION_LIMIT     250     /* ms - max prediction time */
#define SNAP_THRESHOLD          200.0f  /* Distance to snap instead of lerp */
#define SEND_RATE_MS            50      /* Position update send rate */
#define VELOCITY_SMOOTHING      0.3f
#define POSITION_TOLERANCE      2.0f    /* Min movement to send update */

/*
 * ============================================================================
 * Data Structures
 * ============================================================================
 */

typedef struct {
    float       x;
    float       y;
    float       vx;
    float       vy;
    uint32_t    timestamp;
    uint8_t     state;
    int8_t      facing;
} PositionSnapshot;

typedef struct {
    uint32_t            entity_id;
    bool                active;
    bool                is_local;       /* Is this the local player? */

    /* Current interpolated position */
    float               render_x;
    float               render_y;

    /* Velocity for extrapolation */
    float               velocity_x;
    float               velocity_y;

    /* Position history for interpolation */
    PositionSnapshot    history[POSITION_HISTORY_SIZE];
    int                 history_head;
    int                 history_count;

    /* Last known server state */
    float               server_x;
    float               server_y;
    uint32_t            last_server_time;

    /* Animation state */
    uint8_t             state;
    int8_t              facing;

    /* Prediction state (for local player) */
    float               predicted_x;
    float               predicted_y;
    uint32_t            last_input_seq;
    bool                needs_reconciliation;
} SyncEntity;

typedef struct {
    float       x;
    float       y;
    float       vx;
    float       vy;
    uint32_t    input_seq;
    uint32_t    timestamp;
} PendingInput;

typedef struct {
    /* Synchronized entities */
    SyncEntity          entities[MAX_SYNC_ENTITIES];
    int                 entity_count;

    /* Local player reference */
    uint32_t            local_entity_id;
    SyncEntity*         local_entity;

    /* Pending inputs for reconciliation */
    PendingInput        pending_inputs[64];
    int                 pending_head;
    int                 pending_count;
    uint32_t            input_sequence;

    /* Timing */
    uint32_t            server_time_offset;
    uint32_t            last_send_time;
    float               latency_ms;
    float               jitter_ms;

    /* Stats */
    uint32_t            packets_sent;
    uint32_t            packets_received;
    uint32_t            corrections_applied;

    bool                initialized;
} PositionSyncSystem;

static PositionSyncSystem g_pos_sync = {0};

/*
 * ============================================================================
 * Helper Functions
 * ============================================================================
 */

static uint32_t get_render_time(void)
{
    /* Render time is current time minus interpolation delay */
    uint32_t now = SDL_GetTicks() + g_pos_sync.server_time_offset;
    if (now < INTERPOLATION_DELAY) return 0;
    return now - INTERPOLATION_DELAY;
}

static SyncEntity* find_entity(uint32_t entity_id)
{
    for (int i = 0; i < g_pos_sync.entity_count; i++) {
        if (g_pos_sync.entities[i].entity_id == entity_id &&
            g_pos_sync.entities[i].active) {
            return &g_pos_sync.entities[i];
        }
    }
    return NULL;
}

static SyncEntity* add_entity(uint32_t entity_id)
{
    /* Check if already exists */
    SyncEntity* existing = find_entity(entity_id);
    if (existing) return existing;

    /* Find free slot */
    for (int i = 0; i < MAX_SYNC_ENTITIES; i++) {
        if (!g_pos_sync.entities[i].active) {
            SyncEntity* ent = &g_pos_sync.entities[i];
            memset(ent, 0, sizeof(SyncEntity));
            ent->entity_id = entity_id;
            ent->active = true;
            ent->facing = 1;
            g_pos_sync.entity_count++;
            return ent;
        }
    }
    return NULL;
}

static void add_to_history(SyncEntity* ent, const PositionSnapshot* snap)
{
    ent->history[ent->history_head] = *snap;
    ent->history_head = (ent->history_head + 1) % POSITION_HISTORY_SIZE;
    if (ent->history_count < POSITION_HISTORY_SIZE) {
        ent->history_count++;
    }
}

static float lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}

/*
 * ============================================================================
 * Initialization
 * ============================================================================
 */

void sync_position_init(void)
{
    memset(&g_pos_sync, 0, sizeof(PositionSyncSystem));
    g_pos_sync.initialized = true;
}

void sync_position_shutdown(void)
{
    g_pos_sync.initialized = false;
}

void sync_position_set_local_entity(uint32_t entity_id)
{
    g_pos_sync.local_entity_id = entity_id;
    g_pos_sync.local_entity = find_entity(entity_id);

    if (g_pos_sync.local_entity) {
        g_pos_sync.local_entity->is_local = true;
    }
}

/*
 * ============================================================================
 * Entity Management
 * ============================================================================
 */

void sync_position_add_entity(uint32_t entity_id, float x, float y)
{
    SyncEntity* ent = add_entity(entity_id);
    if (!ent) return;

    ent->render_x = x;
    ent->render_y = y;
    ent->server_x = x;
    ent->server_y = y;
    ent->predicted_x = x;
    ent->predicted_y = y;

    /* Initialize history */
    PositionSnapshot snap = {
        .x = x,
        .y = y,
        .vx = 0,
        .vy = 0,
        .timestamp = SDL_GetTicks() + g_pos_sync.server_time_offset,
        .state = 0,
        .facing = 1
    };
    add_to_history(ent, &snap);

    if (entity_id == g_pos_sync.local_entity_id) {
        g_pos_sync.local_entity = ent;
        ent->is_local = true;
    }
}

void sync_position_remove_entity(uint32_t entity_id)
{
    SyncEntity* ent = find_entity(entity_id);
    if (ent) {
        ent->active = false;
        g_pos_sync.entity_count--;

        if (ent == g_pos_sync.local_entity) {
            g_pos_sync.local_entity = NULL;
        }
    }
}

void sync_position_clear_all(void)
{
    for (int i = 0; i < MAX_SYNC_ENTITIES; i++) {
        g_pos_sync.entities[i].active = false;
    }
    g_pos_sync.entity_count = 0;
    g_pos_sync.local_entity = NULL;
}

/*
 * ============================================================================
 * Receive Updates from Server
 * ============================================================================
 */

void sync_position_receive(uint32_t entity_id, float x, float y,
                           float vx, float vy, uint8_t state, int8_t facing,
                           uint32_t server_timestamp)
{
    SyncEntity* ent = find_entity(entity_id);
    if (!ent) {
        ent = add_entity(entity_id);
        if (!ent) return;
    }

    g_pos_sync.packets_received++;

    /* Update server time offset */
    uint32_t now = SDL_GetTicks();
    g_pos_sync.server_time_offset = server_timestamp - now;

    /* Store snapshot */
    PositionSnapshot snap = {
        .x = x,
        .y = y,
        .vx = vx,
        .vy = vy,
        .timestamp = server_timestamp,
        .state = state,
        .facing = facing
    };
    add_to_history(ent, &snap);

    /* Update server state */
    ent->server_x = x;
    ent->server_y = y;
    ent->velocity_x = vx;
    ent->velocity_y = vy;
    ent->state = state;
    ent->facing = facing;
    ent->last_server_time = server_timestamp;

    /* For local player, check if we need reconciliation */
    if (ent->is_local) {
        float dx = fabsf(x - ent->predicted_x);
        float dy = fabsf(y - ent->predicted_y);

        if (dx > POSITION_TOLERANCE || dy > POSITION_TOLERANCE) {
            ent->needs_reconciliation = true;
            g_pos_sync.corrections_applied++;
        }
    }
}

void sync_position_receive_ack(uint32_t input_seq, float x, float y)
{
    if (!g_pos_sync.local_entity) return;

    /* Remove acknowledged inputs from pending list */
    while (g_pos_sync.pending_count > 0) {
        int idx = g_pos_sync.pending_head;
        if (g_pos_sync.pending_inputs[idx].input_seq <= input_seq) {
            g_pos_sync.pending_head = (g_pos_sync.pending_head + 1) % 64;
            g_pos_sync.pending_count--;
        } else {
            break;
        }
    }

    /* Server reconciliation */
    SyncEntity* local = g_pos_sync.local_entity;
    float dx = fabsf(x - local->predicted_x);
    float dy = fabsf(y - local->predicted_y);

    if (dx > POSITION_TOLERANCE || dy > POSITION_TOLERANCE) {
        /* Snap to server position and replay pending inputs */
        local->predicted_x = x;
        local->predicted_y = y;

        /* Replay unacknowledged inputs */
        for (int i = 0; i < g_pos_sync.pending_count; i++) {
            int idx = (g_pos_sync.pending_head + i) % 64;
            PendingInput* input = &g_pos_sync.pending_inputs[idx];

            /* Apply input delta */
            local->predicted_x += input->vx * 0.016f;  /* Assuming 60fps */
            local->predicted_y += input->vy * 0.016f;
        }
    }
}

/*
 * ============================================================================
 * Send Updates to Server
 * ============================================================================
 */

void sync_position_send_update(float x, float y, float vx, float vy,
                               uint8_t state, int8_t facing)
{
    uint32_t now = SDL_GetTicks();

    /* Rate limit sends */
    if (now - g_pos_sync.last_send_time < SEND_RATE_MS) {
        return;
    }

    g_pos_sync.last_send_time = now;
    g_pos_sync.input_sequence++;
    g_pos_sync.packets_sent++;

    /* Store pending input for reconciliation */
    if (g_pos_sync.pending_count < 64) {
        int idx = (g_pos_sync.pending_head + g_pos_sync.pending_count) % 64;
        g_pos_sync.pending_inputs[idx].x = x;
        g_pos_sync.pending_inputs[idx].y = y;
        g_pos_sync.pending_inputs[idx].vx = vx;
        g_pos_sync.pending_inputs[idx].vy = vy;
        g_pos_sync.pending_inputs[idx].input_seq = g_pos_sync.input_sequence;
        g_pos_sync.pending_inputs[idx].timestamp = now;
        g_pos_sync.pending_count++;
    }

    /* Send to server */
    struct {
        uint32_t    seq;
        float       x;
        float       y;
        float       vx;
        float       vy;
        uint8_t     state;
        int8_t      facing;
    } packet = {
        g_pos_sync.input_sequence,
        x, y, vx, vy, state, facing
    };

    network_send_packet(MSG_SET_USER_POSITION, &packet, sizeof(packet));
}

/*
 * ============================================================================
 * Interpolation
 * ============================================================================
 */

static void interpolate_entity(SyncEntity* ent, uint32_t render_time)
{
    if (ent->history_count < 2) {
        /* Not enough data, use last known position */
        ent->render_x = ent->server_x;
        ent->render_y = ent->server_y;
        return;
    }

    /* Find two snapshots to interpolate between */
    PositionSnapshot* before = NULL;
    PositionSnapshot* after = NULL;

    for (int i = 0; i < ent->history_count; i++) {
        int idx = (ent->history_head - 1 - i + POSITION_HISTORY_SIZE) % POSITION_HISTORY_SIZE;
        PositionSnapshot* snap = &ent->history[idx];

        if (snap->timestamp <= render_time) {
            before = snap;
            /* Look for after */
            if (i > 0) {
                int after_idx = (ent->history_head - i + POSITION_HISTORY_SIZE) % POSITION_HISTORY_SIZE;
                after = &ent->history[after_idx];
            }
            break;
        }
    }

    if (!before) {
        /* All snapshots are in the future, use oldest */
        int oldest_idx = (ent->history_head - ent->history_count + POSITION_HISTORY_SIZE) % POSITION_HISTORY_SIZE;
        before = &ent->history[oldest_idx];
    }

    if (!after) {
        /* No future snapshot, extrapolate */
        uint32_t elapsed = render_time - before->timestamp;
        if (elapsed > EXTRAPOLATION_LIMIT) {
            elapsed = EXTRAPOLATION_LIMIT;
        }

        float t = elapsed / 1000.0f;
        ent->render_x = before->x + before->vx * t;
        ent->render_y = before->y + before->vy * t;
        ent->state = before->state;
        ent->facing = before->facing;
    } else {
        /* Interpolate between snapshots */
        uint32_t total = after->timestamp - before->timestamp;
        uint32_t elapsed = render_time - before->timestamp;

        if (total == 0) {
            ent->render_x = after->x;
            ent->render_y = after->y;
        } else {
            float t = (float)elapsed / (float)total;
            if (t > 1.0f) t = 1.0f;

            ent->render_x = lerp(before->x, after->x, t);
            ent->render_y = lerp(before->y, after->y, t);
        }

        ent->state = after->state;
        ent->facing = after->facing;
    }

    /* Check for snap threshold */
    float dx = fabsf(ent->render_x - ent->server_x);
    float dy = fabsf(ent->render_y - ent->server_y);
    if (dx > SNAP_THRESHOLD || dy > SNAP_THRESHOLD) {
        ent->render_x = ent->server_x;
        ent->render_y = ent->server_y;
    }
}

/*
 * ============================================================================
 * Update
 * ============================================================================
 */

void sync_position_update(float delta)
{
    if (!g_pos_sync.initialized) return;

    uint32_t render_time = get_render_time();

    for (int i = 0; i < MAX_SYNC_ENTITIES; i++) {
        SyncEntity* ent = &g_pos_sync.entities[i];
        if (!ent->active) continue;

        if (ent->is_local) {
            /* Local player uses prediction, not interpolation */
            /* Render position is set by player controller */
        } else {
            /* Remote entities use interpolation */
            interpolate_entity(ent, render_time);
        }
    }
}

/*
 * ============================================================================
 * Query Functions
 * ============================================================================
 */

bool sync_position_get_render_position(uint32_t entity_id, float* x, float* y)
{
    SyncEntity* ent = find_entity(entity_id);
    if (!ent) return false;

    *x = ent->render_x;
    *y = ent->render_y;
    return true;
}

bool sync_position_get_state(uint32_t entity_id, uint8_t* state, int8_t* facing)
{
    SyncEntity* ent = find_entity(entity_id);
    if (!ent) return false;

    *state = ent->state;
    *facing = ent->facing;
    return true;
}

bool sync_position_get_velocity(uint32_t entity_id, float* vx, float* vy)
{
    SyncEntity* ent = find_entity(entity_id);
    if (!ent) return false;

    *vx = ent->velocity_x;
    *vy = ent->velocity_y;
    return true;
}

float sync_position_get_latency(void)
{
    return g_pos_sync.latency_ms;
}

void sync_position_set_latency(float latency_ms)
{
    /* Smooth latency updates */
    g_pos_sync.latency_ms = lerp(g_pos_sync.latency_ms, latency_ms, 0.1f);
}

/*
 * ============================================================================
 * Debug
 * ============================================================================
 */

void sync_position_get_stats(uint32_t* packets_sent, uint32_t* packets_received,
                             uint32_t* corrections)
{
    *packets_sent = g_pos_sync.packets_sent;
    *packets_received = g_pos_sync.packets_received;
    *corrections = g_pos_sync.corrections_applied;
}

int sync_position_get_entity_count(void)
{
    return g_pos_sync.entity_count;
}

void sync_position_debug_draw(void* renderer)
{
    /* Draw ghost positions for debugging */
    /* This would show interpolation vs server position */
    (void)renderer;
}
