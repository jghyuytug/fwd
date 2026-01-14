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
#define ERR_BASE_DUNGEON      0x7000
#define ERR_BASE_PARTY        0x8000
#define ERR_BASE_GUILD        0x9000

// Common module error codes
#define ERR_SUCCESS                     (ERR_BASE_COMMON + 0)
#define ERR_UNKNOWN                     (ERR_BASE_COMMON + 1)
#define ERR_INVALID_PARAMETER           (ERR_BASE_COMMON + 2)
#define ERR_OUT_OF_MEMORY               (ERR_BASE_COMMON + 3)
#define ERR_MEMORY_ALLOCATION           (ERR_BASE_COMMON + 3)  // Alias for ERR_OUT_OF_MEMORY
#define ERR_INITIALIZATION_FAILED       (ERR_BASE_COMMON + 4)
#define ERR_ALREADY_INITIALIZED         (ERR_BASE_COMMON + 5)
#define ERR_NOT_INITIALIZED             (ERR_BASE_COMMON + 6)
#define ERR_INVALID_STATE               (ERR_BASE_COMMON + 7)  // Invalid state for operation
#define ERR_CONFIG_FILE_READ            (ERR_BASE_COMMON + 8)
#define ERR_CONFIG_INVALID              (ERR_BASE_COMMON + 9)
#define ERR_INTERNAL_ERROR              (ERR_BASE_COMMON + 10)
#define ERR_OPERATION_TIMEOUT           (ERR_BASE_COMMON + 11)
#define ERR_PERMISSION_DENIED           (ERR_BASE_COMMON + 12)
#define ERR_NOT_FOUND                   (ERR_BASE_COMMON + 13)
#define ERR_QUEUE_FULL                  (ERR_BASE_COMMON + 14)
#define ERR_MAX_LISTENERS_REACHED       (ERR_BASE_COMMON + 15)
#define ERR_MEMORY                      (ERR_BASE_COMMON + 16)
#define ERR_DUPLICATE                   (ERR_BASE_COMMON + 17)
#define ERR_LIMIT_EXCEEDED              (ERR_BASE_COMMON + 18)
#define ERR_BUFFER_OVERFLOW             (ERR_BASE_COMMON + 19)
#define ERR_INVALID_FORMAT              (ERR_BASE_COMMON + 20)

// Database and Network errors
#define ERR_DATABASE                    (ERR_BASE_DATABASE + 0)
#define ERR_DATABASE_CONNECTION_FAILED  (ERR_BASE_DATABASE + 1)
#define ERR_DATABASE_QUERY_FAILED       (ERR_BASE_DATABASE + 2)
#define ERR_DATABASE_NOT_FOUND          (ERR_BASE_DATABASE + 3)
#define ERR_NETWORK                     (ERR_BASE_NETWORK + 0)

// User module errors
#define ERR_USER                        (ERR_BASE_USER + 0)
#define ERR_ACCOUNT_NOT_FOUND           (ERR_BASE_USER + 1)
#define ERR_ACCOUNT_BLOCKED             (ERR_BASE_USER + 2)
#define ERR_AUTHENTICATION_FAILED       (ERR_BASE_USER + 3)
#define ERR_CHARACTER_NOT_FOUND         (ERR_BASE_USER + 4)
#define ERR_SESSION_NOT_FOUND           (ERR_BASE_USER + 5)
#define ERR_NOT_IMPLEMENTED             (ERR_BASE_USER + 99)

// Inventory errors
#define ERR_INVENTORY_FULL              (ERR_BASE_INVENTORY + 0)
#define ERR_INSUFFICIENT_FUNDS          (ERR_BASE_INVENTORY + 1)

// Dungeon errors
#define ERR_DUNGEON_NOT_FOUND           (ERR_BASE_DUNGEON + 0)
#define ERR_DUNGEON_FULL                (ERR_BASE_DUNGEON + 1)
#define ERR_DUNGEON_EXPIRED             (ERR_BASE_DUNGEON + 2)
#define ERR_DUNGEON_ALREADY_CLEARED     (ERR_BASE_DUNGEON + 3)
#define ERR_MONSTER_NOT_FOUND           (ERR_BASE_DUNGEON + 4)
#define ERR_MONSTER_ALREADY_DEAD        (ERR_BASE_DUNGEON + 5)

// Party errors
#define ERR_PARTY_NOT_FOUND             (ERR_BASE_PARTY + 0)
#define ERR_PARTY_FULL                  (ERR_BASE_PARTY + 1)
#define ERR_NOT_PARTY_LEADER            (ERR_BASE_PARTY + 2)
#define ERR_ALREADY_IN_PARTY            (ERR_BASE_PARTY + 3)

// Guild errors
#define ERR_GUILD_NOT_FOUND             (ERR_BASE_GUILD + 0)
#define ERR_GUILD_FULL                  (ERR_BASE_GUILD + 1)
#define ERR_NOT_GUILD_MASTER            (ERR_BASE_GUILD + 2)
#define ERR_ALREADY_IN_GUILD            (ERR_BASE_GUILD + 3)

// Error code utility macros
#define IS_SUCCESS(code)   ((code) == ERR_SUCCESS)
#define IS_FAILURE(code)   ((code) != ERR_SUCCESS)

#endif /* __COMMON_ERROR_CODES_H__ */
