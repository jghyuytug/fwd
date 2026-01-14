#ifndef SPHINX_INFO_ORM_H
#define SPHINX_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char search_id[21];
    long long last_insert_id;
    int last_update_time;
} SphinxInfo;

/* CRUD Operations */
int SphinxInfo_Add(DBConnectionManager* manager, const SphinxInfo* record);
int SphinxInfo_Get(DBConnectionManager* manager, const char* search_id, SphinxInfo* record);
int SphinxInfo_Update(DBConnectionManager* manager, const SphinxInfo* record);
int SphinxInfo_Delete(DBConnectionManager* manager, const char* search_id);
int SphinxInfo_Exists(DBConnectionManager* manager, const char* search_id);
int SphinxInfo_GetAll(DBConnectionManager* manager, SphinxInfo* records, int max_count, int* actual_count);

#endif /* SPHINX_INFO_ORM_H */