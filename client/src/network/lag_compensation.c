/**
 * @file lag_compensation.c
 * @brief Client-side prediction and lag compensation
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

#define INPUT_BUFFER_SIZE       128
#define SNAPSHOT_BUFFER_SIZE    32
#define PING_HISTORY_SIZE       20
#define MAX_PREDICTION_TIME     200     /* ms */
#define RECONCILIATION_THRESHOLD 5.0f   /* Position error threshold */
#define SMOOTHING_FACTOR        0.2f
#define PING_UPDATE_INTERVAL    1000    /* ms */

/*
 * ============================================================================
 * Data Structures
 * ============================================================================
 */

typedef struct {
    uint32_t        sequence;
    uint32_t        timestamp;

    /* Input state */
    float           move_x;
    float           move_y;
    bool            jump;
    bool            dash;
    bool            attack;
    int             skill_slot;

    /* Predicted result */
    float           predicted_x;
    float           predicted_y;
    float           predicted_vx;
    float           predicted_vy;
    uint8_t         predicted_state;
} InputRecord;

typedef struct {
    uint32_t        timestamp;
    float           x;
    float           y;
    float           vx;
    float           vy;
    uint8_t         state;
    int8_t          facing;
} StateSnapshot;

typedef struct {
    uint32_t        send_time;
    uint32_t        recv_time;
    float           rtt;
} PingRecord;

typedef struct {
    /* Input prediction */
    InputRecord     input_buffer[INPUT_BUFFER_SIZE];
    int             input_head;
    int             input_count;
    uint32_t        last_acked_input;

    /* State snapshots */
    StateSnapshot   snapshots[SNAPSHOT_BUFFER_SIZE];
    int             snapshot_head;
    int             snapshot_count;

    /* Latency measurement */
    PingRecord      ping_history[PING_HISTORY_SIZE];
    int             ping_head;
    int             ping_count;
    float           current_ping;
    float           average_ping;
    float           jitter;
    uint32_t        last_ping_time;
    uint32_t        ping_sequence;

    /* Server time sync */
    int32_t         server_time_offset;
    uint32_t        last_server_time;
    bool            time_synced;

    /* Reconciliation */
    float           correction_x;
    float           correction_y;
    float           correction_smoothing;
    uint32_t        last_correction_time;
    int             corrections_count;

    /* Current predicted state */
    float           predicted_x;
    float           predicted_y;
    float           predicted_vx;
    float           predicted_vy;

    bool            initialized;
} LagCompensation;

static LagCompensation g_lag = {0};

/*
 * ============================================================================
 * Helper Functions
 * ============================================================================
 */

static float lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}

static uint32_t get_server_time(void)
{
    return SDL_GetTicks() + g_lag.server_time_offset;
}

/*
 * ============================================================================
 * Initialization
 * ============================================================================
 */

void lag_init(void)
{
    memset(&g_lag, 0, sizeof(LagCompensation));
    g_lag.correction_smoothing = 1.0f;
    g_lag.initialized = true;
}

void lag_shutdown(void)
{
    g_lag.initialized = false;
}

void lag_reset(void)
{
    g_lag.input_count = 0;
    g_lag.input_head = 0;
    g_lag.snapshot_count = 0;
    g_lag.snapshot_head = 0;
    g_lag.last_acked_input = 0;
    g_lag.corrections_count = 0;
    g_lag.correction_x = 0;
    g_lag.correction_y = 0;
}

/*
 * ============================================================================
 * Input Recording
 * ============================================================================
 */

uint32_t lag_record_input(float move_x, float move_y, bool jump, bool dash,
                          bool attack, int skill_slot,
                          float predicted_x, float predicted_y,
                          float predicted_vx, float predicted_vy,
                          uint8_t predicted_state)
{
    /* Store in circular buffer */
    int idx = (g_lag.input_head + g_lag.input_count) % INPUT_BUFFER_SIZE;

    InputRecord* record = &g_lag.input_buffer[idx];
    record->sequence = g_lag.input_count;
    record->timestamp = SDL_GetTicks();
    record->move_x = move_x;
    record->move_y = move_y;
    record->jump = jump;
    record->dash = dash;
    record->attack = attack;
    record->skill_slot = skill_slot;
    record->predicted_x = predicted_x;
    record->predicted_y = predicted_y;
    record->predicted_vx = predicted_vx;
    record->predicted_vy = predicted_vy;
    record->predicted_state = predicted_state;

    if (g_lag.input_count < INPUT_BUFFER_SIZE) {
        g_lag.input_count++;
    } else {
        g_lag.input_head = (g_lag.input_head + 1) % INPUT_BUFFER_SIZE;
    }

    return record->sequence;
}

/*
 * ============================================================================
 * Server Acknowledgment
 * ============================================================================
 */

void lag_receive_ack(uint32_t acked_sequence, float server_x, float server_y,
                     float server_vx, float server_vy, uint8_t server_state,
                     uint32_t server_timestamp)
{
    /* Update server time */
    if (!g_lag.time_synced) {
        g_lag.server_time_offset = server_timestamp - SDL_GetTicks();
        g_lag.time_synced = true;
    }
    g_lag.last_server_time = server_timestamp;

    /* Find the acknowledged input */
    InputRecord* acked_input = NULL;
    int acked_idx = -1;

    for (int i = 0; i < g_lag.input_count; i++) {
        int idx = (g_lag.input_head + i) % INPUT_BUFFER_SIZE;
        if (g_lag.input_buffer[idx].sequence == acked_sequence) {
            acked_input = &g_lag.input_buffer[idx];
            acked_idx = i;
            break;
        }
    }

    if (!acked_input) {
        return;  /* Old or invalid ack */
    }

    /* Check prediction error */
    float error_x = fabsf(acked_input->predicted_x - server_x);
    float error_y = fabsf(acked_input->predicted_y - server_y);

    if (error_x > RECONCILIATION_THRESHOLD ||
        error_y > RECONCILIATION_THRESHOLD) {
        /* Prediction was wrong, need to correct */
        g_lag.corrections_count++;

        /* Calculate correction */
        g_lag.correction_x = server_x - acked_input->predicted_x;
        g_lag.correction_y = server_y - acked_input->predicted_y;
        g_lag.correction_smoothing = 0;
        g_lag.last_correction_time = SDL_GetTicks();

        /* Replay unacknowledged inputs from server state */
        g_lag.predicted_x = server_x;
        g_lag.predicted_y = server_y;
        g_lag.predicted_vx = server_vx;
        g_lag.predicted_vy = server_vy;

        /* Replay all inputs after the acknowledged one */
        for (int i = acked_idx + 1; i < g_lag.input_count; i++) {
            int idx = (g_lag.input_head + i) % INPUT_BUFFER_SIZE;
            InputRecord* input = &g_lag.input_buffer[idx];

            /* Simulate one frame with this input */
            lag_simulate_input(input, 0.016f);  /* Assume 60fps */
        }
    }

    /* Remove acknowledged and older inputs */
    g_lag.last_acked_input = acked_sequence;
    g_lag.input_head = (g_lag.input_head + acked_idx + 1) % INPUT_BUFFER_SIZE;
    g_lag.input_count -= (acked_idx + 1);
    if (g_lag.input_count < 0) g_lag.input_count = 0;
}

/*
 * ============================================================================
 * Input Simulation
 * ============================================================================
 */

void lag_simulate_input(InputRecord* input, float delta)
{
    /* Simple physics simulation */
    float speed = 200.0f;
    float gravity = 500.0f;

    /* Apply movement */
    g_lag.predicted_vx = input->move_x * speed;

    /* Apply gravity if not on ground */
    /* (Simplified - real implementation would check collision) */
    g_lag.predicted_vy += gravity * delta;

    /* Update position */
    g_lag.predicted_x += g_lag.predicted_vx * delta;
    g_lag.predicted_y += g_lag.predicted_vy * delta;

    /* Ground collision (simplified) */
    if (g_lag.predicted_y > 500.0f) {
        g_lag.predicted_y = 500.0f;
        g_lag.predicted_vy = 0;
    }

    /* Update input's predicted values */
    input->predicted_x = g_lag.predicted_x;
    input->predicted_y = g_lag.predicted_y;
    input->predicted_vx = g_lag.predicted_vx;
    input->predicted_vy = g_lag.predicted_vy;
}

/*
 * ============================================================================
 * Ping Measurement
 * ============================================================================
 */

void lag_send_ping(void)
{
    uint32_t now = SDL_GetTicks();

    if (now - g_lag.last_ping_time < PING_UPDATE_INTERVAL) {
        return;
    }

    g_lag.last_ping_time = now;
    g_lag.ping_sequence++;

    /* Record send time */
    int idx = g_lag.ping_head;
    g_lag.ping_history[idx].send_time = now;
    g_lag.ping_history[idx].recv_time = 0;

    /* Send ping packet to server */
    struct {
        uint32_t sequence;
        uint32_t client_time;
    } packet = { g_lag.ping_sequence, now };

    network_send_packet(MSG_PING, &packet, sizeof(packet));
}

void lag_receive_pong(uint32_t sequence, uint32_t server_time)
{
    uint32_t now = SDL_GetTicks();

    /* Find matching ping */
    for (int i = 0; i < PING_HISTORY_SIZE; i++) {
        int idx = (g_lag.ping_head - i + PING_HISTORY_SIZE) % PING_HISTORY_SIZE;
        if (g_lag.ping_history[idx].send_time > 0 &&
            g_lag.ping_history[idx].recv_time == 0) {
            /* This is likely our ping */
            g_lag.ping_history[idx].recv_time = now;
            g_lag.ping_history[idx].rtt = (float)(now - g_lag.ping_history[idx].send_time);

            /* Update current ping */
            g_lag.current_ping = g_lag.ping_history[idx].rtt;

            /* Update server time offset */
            g_lag.server_time_offset = server_time - (now - (uint32_t)(g_lag.current_ping / 2));

            break;
        }
    }

    g_lag.ping_head = (g_lag.ping_head + 1) % PING_HISTORY_SIZE;
    if (g_lag.ping_count < PING_HISTORY_SIZE) {
        g_lag.ping_count++;
    }

    /* Calculate average and jitter */
    float sum = 0;
    float min_ping = 9999;
    float max_ping = 0;
    int valid = 0;

    for (int i = 0; i < g_lag.ping_count; i++) {
        if (g_lag.ping_history[i].rtt > 0) {
            sum += g_lag.ping_history[i].rtt;
            if (g_lag.ping_history[i].rtt < min_ping) min_ping = g_lag.ping_history[i].rtt;
            if (g_lag.ping_history[i].rtt > max_ping) max_ping = g_lag.ping_history[i].rtt;
            valid++;
        }
    }

    if (valid > 0) {
        g_lag.average_ping = sum / valid;
        g_lag.jitter = max_ping - min_ping;
    }
}

/*
 * ============================================================================
 * Update
 * ============================================================================
 */

void lag_update(float delta)
{
    if (!g_lag.initialized) return;

    /* Smooth correction */
    if (g_lag.correction_smoothing < 1.0f) {
        g_lag.correction_smoothing += SMOOTHING_FACTOR * delta * 10.0f;
        if (g_lag.correction_smoothing > 1.0f) {
            g_lag.correction_smoothing = 1.0f;
        }
    }

    /* Send periodic pings */
    lag_send_ping();
}

/*
 * ============================================================================
 * State Snapshots
 * ============================================================================
 */

void lag_save_snapshot(float x, float y, float vx, float vy,
                       uint8_t state, int8_t facing)
{
    int idx = (g_lag.snapshot_head + g_lag.snapshot_count) % SNAPSHOT_BUFFER_SIZE;

    StateSnapshot* snap = &g_lag.snapshots[idx];
    snap->timestamp = SDL_GetTicks();
    snap->x = x;
    snap->y = y;
    snap->vx = vx;
    snap->vy = vy;
    snap->state = state;
    snap->facing = facing;

    if (g_lag.snapshot_count < SNAPSHOT_BUFFER_SIZE) {
        g_lag.snapshot_count++;
    } else {
        g_lag.snapshot_head = (g_lag.snapshot_head + 1) % SNAPSHOT_BUFFER_SIZE;
    }
}

bool lag_get_snapshot_at_time(uint32_t timestamp, float* x, float* y,
                              float* vx, float* vy)
{
    if (g_lag.snapshot_count == 0) return false;

    /* Find closest snapshot */
    StateSnapshot* before = NULL;
    StateSnapshot* after = NULL;

    for (int i = 0; i < g_lag.snapshot_count; i++) {
        int idx = (g_lag.snapshot_head + i) % SNAPSHOT_BUFFER_SIZE;
        StateSnapshot* snap = &g_lag.snapshots[idx];

        if (snap->timestamp <= timestamp) {
            before = snap;
        } else if (snap->timestamp > timestamp && !after) {
            after = snap;
            break;
        }
    }

    if (!before) return false;

    if (!after) {
        /* Extrapolate */
        *x = before->x;
        *y = before->y;
        *vx = before->vx;
        *vy = before->vy;
    } else {
        /* Interpolate */
        float t = (float)(timestamp - before->timestamp) /
                  (float)(after->timestamp - before->timestamp);
        *x = lerp(before->x, after->x, t);
        *y = lerp(before->y, after->y, t);
        *vx = lerp(before->vx, after->vx, t);
        *vy = lerp(before->vy, after->vy, t);
    }

    return true;
}

/*
 * ============================================================================
 * Query Functions
 * ============================================================================
 */

void lag_get_correction(float* offset_x, float* offset_y)
{
    *offset_x = g_lag.correction_x * (1.0f - g_lag.correction_smoothing);
    *offset_y = g_lag.correction_y * (1.0f - g_lag.correction_smoothing);
}

float lag_get_ping(void)
{
    return g_lag.current_ping;
}

float lag_get_average_ping(void)
{
    return g_lag.average_ping;
}

float lag_get_jitter(void)
{
    return g_lag.jitter;
}

int lag_get_pending_inputs(void)
{
    return g_lag.input_count;
}

int lag_get_correction_count(void)
{
    return g_lag.corrections_count;
}

uint32_t lag_get_server_time(void)
{
    return get_server_time();
}

bool lag_is_time_synced(void)
{
    return g_lag.time_synced;
}

/*
 * ============================================================================
 * Prediction Quality
 * ============================================================================
 */

typedef enum {
    PREDICTION_QUALITY_EXCELLENT,   /* < 5 corrections per minute */
    PREDICTION_QUALITY_GOOD,        /* < 15 corrections per minute */
    PREDICTION_QUALITY_FAIR,        /* < 30 corrections per minute */
    PREDICTION_QUALITY_POOR         /* > 30 corrections per minute */
} PredictionQuality;

PredictionQuality lag_get_prediction_quality(void)
{
    /* Calculate corrections per minute based on recent history */
    if (g_lag.corrections_count < 5) {
        return PREDICTION_QUALITY_EXCELLENT;
    } else if (g_lag.corrections_count < 15) {
        return PREDICTION_QUALITY_GOOD;
    } else if (g_lag.corrections_count < 30) {
        return PREDICTION_QUALITY_FAIR;
    }
    return PREDICTION_QUALITY_POOR;
}

/*
 * ============================================================================
 * Debug
 * ============================================================================
 */

void lag_get_debug_info(int* pending_inputs, int* snapshot_count,
                        float* correction_magnitude)
{
    *pending_inputs = g_lag.input_count;
    *snapshot_count = g_lag.snapshot_count;
    *correction_magnitude = sqrtf(g_lag.correction_x * g_lag.correction_x +
                                   g_lag.correction_y * g_lag.correction_y);
}
