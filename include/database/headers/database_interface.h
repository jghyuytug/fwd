#ifndef DATABASE_INTERFACE_H
#define DATABASE_INTERFACE_H

#include "common/defs.h"
#include "common/headers/error_codes.h"
#include "CDBConnector.h"
#include "CQueryCounter.h"
#include "DBMgr.h"
#include "db_types.h"
#include "db_enums.h"

// DatabaseType enum values
typedef enum {
    DB_ACCOUNT = 0,
    DB_CHARACTER = 1,
    DB_GAME = 2,
    DB_LOG = 3
} DatabaseType;

int Database_Initialize(const char *config_path);
void Database_Cleanup();
int Database_Query(const char *query, void **result);
void* Database_GetConnection(int connection_id);
int Database_ReleaseConnection(void *conn);
unsigned int Database_GetQueryCount();
void Database_ResetQueryCounter();
int Database_IsConnected();
void* Database_GetTypedConnection(DatabaseType db_type);

#endif // DATABASE_INTERFACE_H