#ifndef LOG_ADVANCE_ALTER_LAYER_STAT_ORM_H
#define LOG_ADVANCE_ALTER_LAYER_STAT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_date[11];
    short item_type;
    int item_id;
    short item_level;
    short use_count;
    short use_star;
} LogAdvanceAlterLayerStat;

/* CRUD Operations */
int LogAdvanceAlterLayerStat_Add(DBConnectionManager* manager, const LogAdvanceAlterLayerStat* record);
int LogAdvanceAlterLayerStat_Get(DBConnectionManager* manager, const char* occ_date, short item_type, int item_id, short item_level, LogAdvanceAlterLayerStat* record);
int LogAdvanceAlterLayerStat_Update(DBConnectionManager* manager, const LogAdvanceAlterLayerStat* record);
int LogAdvanceAlterLayerStat_Delete(DBConnectionManager* manager, const char* occ_date, short item_type, int item_id, short item_level);
int LogAdvanceAlterLayerStat_Exists(DBConnectionManager* manager, const char* occ_date, short item_type, int item_id, short item_level);
int LogAdvanceAlterLayerStat_GetAll(DBConnectionManager* manager, LogAdvanceAlterLayerStat* records, int max_count, int* actual_count);

#endif /* LOG_ADVANCE_ALTER_LAYER_STAT_ORM_H */