/*
 * Common Module - Improved Constants
 * Defines standard constants used across all modules
 */

#ifndef __COMMON_CONSTANTS_IMPROVED_H__
#define __COMMON_CONSTANTS_IMPROVED_H__

#include <stdint.h>

// Time-related constants
#define SECONDS_PER_MINUTE      60
#define SECONDS_PER_HOUR        3600
#define SECONDS_PER_DAY         86400
#define MILLISECONDS_PER_SECOND 1000

// Networking constants
#define MAX_CONNECTIONS_PER_SERVER  4096
#define PACKET_HEADER_SIZE          16
#define MAX_PACKET_SIZE             65536
#define DEFAULT_SOCKET_TIMEOUT      30  // seconds

// Database constants
#define MAX_DB_CONNECTIONS          100
#define DEFAULT_POOL_SIZE           50
#define MAX_QUERY_LENGTH            4096

// User-related constants
#define MAX_USERNAME_LENGTH         32
#define MAX_PASSWORD_HASH_LENGTH    64
#define MAX_EMAIL_LENGTH            128
#define MAX_CHARACTER_SLOTS         5

// Inventory constants
#define MAX_INVENTORY_SLOTS         697
#define MAX_STACK_SIZE              999
#define MAX_CURRENCY_AMOUNT         2147483647  // Max 32-bit signed int

// Event system constants
#define MAX_EVENT_QUEUE_SIZE        1024
#define MAX_EVENT_LISTENERS         256
#define MAX_EVENT_QUEUE_WAIT_TIME   5000  // milliseconds

// Common system limits
#define MAX_THREAD_POOL_SIZE        32
#define MAX_LOGGING_BUFFER_SIZE     4096

// Type-safe constants
typedef const int32_t limit_t;

#endif /* __COMMON_CONSTANTS_IMPROVED_H__ */
