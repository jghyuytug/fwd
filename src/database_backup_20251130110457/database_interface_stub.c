#include "../../include/database/headers/database_interface.h"

#include <stdbool.h>
#include <stdio.h>

static bool g_database_initialized = false;
static unsigned int g_database_query_count = 0;

int Database_Initialize(const char *config_path) {
    if (g_database_initialized) {
        return ERR_ALREADY_INITIALIZED;
    }
    if (!config_path || config_path[0] == '\0') {
        return ERR_INVALID_PARAMETER;
    }

    g_database_initialized = true;
    g_database_query_count = 0;
    fprintf(stderr, "[Database] Stub initialize using config: %s\n", config_path);
    return ERR_SUCCESS;
}

void Database_Cleanup() {
    g_database_initialized = false;
    g_database_query_count = 0;
}

int Database_Query(const char *query, void **result) {
    if (!g_database_initialized) {
        return ERR_NOT_INITIALIZED;
    }
    if (!query) {
        return ERR_INVALID_PARAMETER;
    }

    g_database_query_count++;
    if (result) {
        *result = NULL;
    }
    fprintf(stderr, "[Database] Stub query ignored: %s\n", query);
    return ERR_NOT_IMPLEMENTED;
}

void* Database_GetConnection(int connection_id) {
    (void)connection_id;
    return NULL;
}

int Database_ReleaseConnection(void *conn) {
    (void)conn;
    return ERR_NOT_IMPLEMENTED;
}

unsigned int Database_GetQueryCount() {
    return g_database_query_count;
}

void Database_ResetQueryCounter() {
    g_database_query_count = 0;
}

int Database_IsConnected() {
    return g_database_initialized ? 1 : 0;
}

void* Database_GetTypedConnection(DatabaseType db_type) {
    return Database_GetConnection((int)db_type);
}
