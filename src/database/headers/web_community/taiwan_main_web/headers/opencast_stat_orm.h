#ifndef OPENCAST_STAT_ORM_H
#define OPENCAST_STAT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    char cur_date[11];
    int cnt;
} OpencastStat;

/* CRUD Operations */
int OpencastStat_Add(DBConnectionManager* manager, const OpencastStat* record);
int OpencastStat_Get(DBConnectionManager* manager, int m_id, const char* cur_date, OpencastStat* record);
int OpencastStat_Update(DBConnectionManager* manager, const OpencastStat* record);
int OpencastStat_Delete(DBConnectionManager* manager, int m_id, const char* cur_date);
int OpencastStat_Exists(DBConnectionManager* manager, int m_id, const char* cur_date);
int OpencastStat_GetAll(DBConnectionManager* manager, OpencastStat* records, int max_count, int* actual_count);

#endif /* OPENCAST_STAT_ORM_H */