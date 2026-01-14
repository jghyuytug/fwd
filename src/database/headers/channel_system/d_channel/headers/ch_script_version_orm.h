#ifndef CH_SCRIPT_VERSION_ORM_H
#define CH_SCRIPT_VERSION_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char script_version[11];
} ChScriptVersion;

/* CRUD Operations */
int ChScriptVersion_Add(DBConnectionManager* manager, const ChScriptVersion* record);
int ChScriptVersion_GetAll(DBConnectionManager* manager, ChScriptVersion* records, int max_count, int* actual_count);

#endif /* CH_SCRIPT_VERSION_ORM_H */