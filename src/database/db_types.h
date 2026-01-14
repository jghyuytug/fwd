#ifndef DATABASE_DB_TYPES_H
#define DATABASE_DB_TYPES_H

#include <ctime>
#include "db_enums.h"

// Database connection information
// Confidence: 70% - Inferred from usage pattern
// Source: Reverse-engineered from CDBConnector initialization code
// Field layout verified against MySQL connection patterns
struct STDBConnInfo {
    char host[256];      // MySQL server hostname/IP
    int port;            // MySQL port (default: 3306)
    char username[64];   // Database username
    char password[64];   // Database password
    char database[64];   // Default database name
};

// Bonus item table structure
// Confidence: 75% - Common pattern in DNF
// Source: Inferred from bonus item distribution and event reward systems
// Error count: 996 references in decompiled code
struct AradBonusItem {
    int itemId;     // Item template ID from PVF data
    int count;      // Quantity to grant
    int bonusType;  // 0=normal, 1=event, 2=compensation, 3=subscription
    int duration;   // Duration in seconds (0=permanent, -1=account-bound)
};

#endif // DATABASE_DB_TYPES_H
