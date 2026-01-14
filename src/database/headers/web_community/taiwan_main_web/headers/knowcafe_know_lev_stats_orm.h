#ifndef KNOWCAFE_KNOW_LEV_STATS_ORM_H
#define KNOWCAFE_KNOW_LEV_STATS_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char date_enroll[11];
    signed char lev;
    int m_cnt;
} KnowcafeKnowLevStats;

/* CRUD Operations */
int KnowcafeKnowLevStats_Add(DBConnectionManager* manager, const KnowcafeKnowLevStats* record);
int KnowcafeKnowLevStats_Get(DBConnectionManager* manager, const char* date_enroll, signed char lev, KnowcafeKnowLevStats* record);
int KnowcafeKnowLevStats_Update(DBConnectionManager* manager, const KnowcafeKnowLevStats* record);
int KnowcafeKnowLevStats_Delete(DBConnectionManager* manager, const char* date_enroll, signed char lev);
int KnowcafeKnowLevStats_Exists(DBConnectionManager* manager, const char* date_enroll, signed char lev);
int KnowcafeKnowLevStats_GetAll(DBConnectionManager* manager, KnowcafeKnowLevStats* records, int max_count, int* actual_count);

#endif /* KNOWCAFE_KNOW_LEV_STATS_ORM_H */