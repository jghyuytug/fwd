/**
 * Common Module - Core Implementation
 *
 * This file implements:
 * - Module initialization
 * - Logging infrastructure
 * - Error message mapping
 * - Global state management
 */

#include "common_interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>

/* Global server state */
static GlobalServerState g_server_state = {
    .server_id = 0,
    .channel_id = 0,
    .start_time = 0,
    .is_running = 0
};

/* Module state */
static struct {
    int initialized;
    FILE *log_file;
    int log_to_console;
    int log_to_file;
} g_common_state = {
    .initialized = 0,
    .log_file = NULL,
    .log_to_console = 1,
    .log_to_file = 0
};

/**
 * Initialize Common Module
 */
int Common_Initialize()
{
    if (g_common_state.initialized) {
        fprintf(stderr, "Common module already initialized\n");
        return 0;
    }

    printf("[Common] Initializing common module...\n");

    /* Initialize global state */
    g_server_state.start_time = (unsigned int)time(NULL);
    g_server_state.is_running = 1;

    /* Initialize logging */
    g_common_state.log_to_console = 1;
    g_common_state.log_to_file = 1;

    /* Open log file */
    g_common_state.log_file = fopen("server.log", "a");
    if (g_common_state.log_file) {
        setvbuf(g_common_state.log_file, NULL, _IOLBF, 256);
    }

    printf("[Common] Common module initialized (uptime: 0s)\n");
    g_common_state.initialized = 1;

    return 0;
}

/**
 * Cleanup Common Module
 */
void Common_Cleanup()
{
    if (!g_common_state.initialized) {
        return;
    }

    printf("[Common] Cleaning up common module...\n");

    /* Close log file */
    if (g_common_state.log_file) {
        fclose(g_common_state.log_file);
        g_common_state.log_file = NULL;
    }

    g_common_state.initialized = 0;
    printf("[Common] Common module cleanup complete\n");
}

/**
 * Get error message
 */
const char* Common_GetErrorString(int error_code)
{
    /* Return descriptive error messages */
    /* Maps from error_codes.h */
    switch (error_code) {
        case ERR_SUCCESS:
            return "Success";
        case ERR_INVALID_PARAMETER:
            return "Invalid parameter";
        case ERR_MEMORY_ALLOCATION:
            return "Memory allocation failed";
        case ERR_DATABASE:
            return "Database error";
        case ERR_NETWORK:
            return "Network error";
        case ERR_PERMISSION_DENIED:
            return "Permission denied";
        default:
            return "Unknown error";
    }
}

/**
 * Internal logging function
 */
static void _common_log_internal(const char *level, const char *format, va_list args)
{
    if (!g_common_state.initialized) {
        return;
    }

    char timestamp[32];
    time_t now = time(NULL);
    struct tm *timeinfo = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);

    if (g_common_state.log_to_console) {
        printf("[%s] [%s] ", timestamp, level);
        vprintf(format, args);
        printf("\n");
    }

    if (g_common_state.log_to_file && g_common_state.log_file) {
        fprintf(g_common_state.log_file, "[%s] [%s] ", timestamp, level);
        vfprintf(g_common_state.log_file, format, args);
        fprintf(g_common_state.log_file, "\n");
    }
}

/**
 * Debug logging
 */
void Common_Log(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    _common_log_internal("INFO", format, args);
    va_end(args);
}

/**
 * Warning logging
 */
void Common_LogWarning(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    _common_log_internal("WARN", format, args);
    va_end(args);
}

/**
 * Error logging
 */
void Common_LogError(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    _common_log_internal("ERROR", format, args);
    va_end(args);
}

/**
 * Get server uptime
 */
unsigned int Common_GetServerUptime()
{
    if (!g_server_state.start_time) {
        return 0;
    }

    return (unsigned int)(time(NULL) - g_server_state.start_time);
}

/**
 * Get current server time
 */
time_t Common_GetServerTime()
{
    return time(NULL);
}

/**
 * Get global state
 */
GlobalServerState* Common_GetGlobalState()
{
    return &g_server_state;
}
