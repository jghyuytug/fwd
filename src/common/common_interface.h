/**
 * Common Module - Public Interface
 * Purpose: Shared definitions, constants, and utilities for all modules
 *
 * This module provides:
 * - Core type definitions
 * - Error codes
 * - Common constants (time units, limits, etc.)
 * - Utility functions used across modules
 */

#ifndef SRC_COMMON_COMMON_INTERFACE_H_
#define SRC_COMMON_COMMON_INTERFACE_H_

#include "defs.h"
#include "error_codes.h"
#include "constants_improved.h"

/**
 * Common Module Initialization
 *
 * Should be called first before any other modules
 * Initializes:
 * - Global state structures
 * - Logging systems
 * - Memory pools
 *
 * Returns: 0 on success, error code on failure
 */
int Common_Initialize();

/**
 * Common Module Cleanup
 *
 * Should be called last, after all other modules are shut down
 * Frees:
 * - Memory pools
 * - Logging resources
 * - Global state
 */
void Common_Cleanup();

/**
 * Get human-readable error message for error code
 *
 * Returns: Static string describing the error (never NULL)
 */
const char* Common_GetErrorString(int error_code);

/**
 * Logging function for debug output
 *
 * Thread-safe, formats similar to printf
 * Can be redirected to file, console, or both
 */
void Common_Log(const char *format, ...);

/**
 * Logging for warnings
 */
void Common_LogWarning(const char *format, ...);

/**
 * Logging for errors
 */
void Common_LogError(const char *format, ...);

/**
 * Get current server uptime in seconds
 */
unsigned int Common_GetServerUptime();

/**
 * Get current server time (Unix timestamp)
 */
time_t Common_GetServerTime();

/**
 * Global state accessor - provides access to global server state
 * This is used by other modules to access:
 * - Configuration data
 * - Server parameters
 * - Shared resources
 */
typedef struct {
    int server_id;           /* Unique server identifier */
    int channel_id;          /* Channel this server is serving */
    unsigned int start_time; /* Server start time (Unix timestamp) */
    int is_running;          /* Server running status */
} GlobalServerState;

GlobalServerState* Common_GetGlobalState();

#endif // SRC_COMMON_COMMON_INTERFACE_H_
