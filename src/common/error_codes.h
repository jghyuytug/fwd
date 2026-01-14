/*
 * Common Module - Error Codes
 * Defines standard error codes for all modules
 */

#ifndef __COMMON_ERROR_CODES_H__
#define __COMMON_ERROR_CODES_H__

// Base error code ranges for different modules
#define ERR_BASE_COMMON       0x0000
#define ERR_BASE_SECURITY     0x1000
#define ERR_BASE_NETWORK      0x2000
#define ERR_BASE_DATABASE     0x3000
#define ERR_BASE_USER         0x4000
#define ERR_BASE_INVENTORY    0x5000
#define ERR_BASE_EVENTS       0x6000

// Common module error codes
#define ERR_SUCCESS                     (ERR_BASE_COMMON + 0)
#define ERR_UNKNOWN                     (ERR_BASE_COMMON + 1)
#define ERR_INVALID_PARAMETER           (ERR_BASE_COMMON + 2)
#define ERR_OUT_OF_MEMORY               (ERR_BASE_COMMON + 3)
#define ERR_MEMORY_ALLOCATION           (ERR_BASE_COMMON + 3)  // Alias for ERR_OUT_OF_MEMORY
#define ERR_INITIALIZATION_FAILED       (ERR_BASE_COMMON + 4)
#define ERR_ALREADY_INITIALIZED         (ERR_BASE_COMMON + 5)
#define ERR_NOT_INITIALIZED             (ERR_BASE_COMMON + 6)
#define ERR_CONFIG_FILE_READ            (ERR_BASE_COMMON + 7)
#define ERR_CONFIG_INVALID              (ERR_BASE_COMMON + 8)
#define ERR_INTERNAL_ERROR              (ERR_BASE_COMMON + 9)
#define ERR_OPERATION_TIMEOUT           (ERR_BASE_COMMON + 10)
#define ERR_PERMISSION_DENIED           (ERR_BASE_COMMON + 11)

// Database and Network errors
#define ERR_DATABASE                    (ERR_BASE_DATABASE + 0)
#define ERR_NETWORK                     (ERR_BASE_NETWORK + 0)

// Error code utility macros
#define IS_SUCCESS(code)   ((code) == ERR_SUCCESS)
#define IS_FAILURE(code)   ((code) != ERR_SUCCESS)

#endif /* __COMMON_ERROR_CODES_H__ */
