#ifndef LOG_ADVANCE_ALTER_ENTRANCE_ORM_H
#define LOG_ADVANCE_ALTER_ENTRANCE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_date[11];
    int stage_id;
    int entrance_count;
    int success_count;
    int obtain_star;
} LogAdvanceAlterEntrance;

/* CRUD Operations */
int LogAdvanceAlterEntrance_Add(DBConnectionManager* manager, const LogAdvanceAlterEntrance* record);
int LogAdvanceAlterEntrance_Get(DBConnectionManager* manager, const char* occ_date, int stage_id, LogAdvanceAlterEntrance* record);
int LogAdvanceAlterEntrance_Update(DBConnectionManager* manager, const LogAdvanceAlterEntrance* record);
int LogAdvanceAlterEntrance_Delete(DBConnectionManager* manager, const char* occ_date, int stage_id);
int LogAdvanceAlterEntrance_Exists(DBConnectionManager* manager, const char* occ_date, int stage_id);
int LogAdvanceAlterEntrance_GetAll(DBConnectionManager* manager, LogAdvanceAlterEntrance* records, int max_count, int* actual_count);

#endif /* LOG_ADVANCE_ALTER_ENTRANCE_ORM_H */