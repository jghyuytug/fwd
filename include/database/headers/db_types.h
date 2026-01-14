#ifndef DATABASE_DB_TYPES_H
#define DATABASE_DB_TYPES_H

#include <time.h>
#include "db_enums.h"

// Database connection information
// Confidence: 70% - Inferred from usage pattern
// Connection parameters for MySQL database
typedef struct {
    char host[256];     // Hostname or IP (e.g., "192.168.200.131")
    int port;           // Port number (default: 3306)
    char username[64];  // MySQL username
    char password[64];  // MySQL password
    char database[64];  // Database name
    int poolSize;       // Connection pool size
    int timeout;        // Connection timeout (seconds)
    /* Used by DBConnectionManager for each database handle */
} STDBConnInfo;

// Bonus item table structure
// Confidence: 75% - Common pattern in DNF
// Error count: 996
// Bonus/reward item definition
struct AradBonusItem {
    int itemId;         // Item template ID
    int count;          // Quantity to award
    int bonusType;      // 0=immediate, 1=mail, 2=pickup
    int duration;       // Item duration (0=permanent)
    int condition;      // Condition for award (level, quest, etc.)
    /* Used by event and promotion reward systems */
};

#endif // DATABASE_DB_TYPES_H
