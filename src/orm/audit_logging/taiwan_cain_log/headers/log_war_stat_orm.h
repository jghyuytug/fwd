#ifndef LOG_WAR_STAT_ORM_H
#define LOG_WAR_STAT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_date[11];
    int channel_no;
    int war_zone_cnt;
    int peace_zone_cnt;
    int rare_item_cnt;
    int unique_item_cnt;
} LogWarStat;

/* CRUD Operations */
int LogWarStat_Add(DBConnectionManager* manager, const LogWarStat* record);
int LogWarStat_Get(DBConnectionManager* manager, const char* occ_date, int channel_no, LogWarStat* record);
int LogWarStat_Update(DBConnectionManager* manager, const LogWarStat* record);
int LogWarStat_Delete(DBConnectionManager* manager, const char* occ_date, int channel_no);
int LogWarStat_Exists(DBConnectionManager* manager, const char* occ_date, int channel_no);
int LogWarStat_GetAll(DBConnectionManager* manager, LogWarStat* records, int max_count, int* actual_count);

#endif /* LOG_WAR_STAT_ORM_H */