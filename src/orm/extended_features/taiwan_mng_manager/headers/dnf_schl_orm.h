#ifndef DNF_SCHL_ORM_H
#define DNF_SCHL_ORM_H

#include "db_connection_manager.h"

typedef struct {
    signed char server_group;
    char end_date[20];
    char st_date[20];
    char schl_cd[5];
    char period_month[3];
    char period_week[3];
    char period_day[3];
    char period_hour[3];
    char period_minute[3];
    char type[5];
    char wave_hour[3];
    char wave_minute[3];
    char level[5];
    char stat[5];
    char description[256];
    char occ_date[20];
} DnfSchl;

/* CRUD Operations */
int DnfSchl_Add(DBConnectionManager* manager, const DnfSchl* record);
int DnfSchl_Get(DBConnectionManager* manager, signed char server_group, const char* end_date, const char* st_date, const char* schl_cd, DnfSchl* record);
int DnfSchl_Update(DBConnectionManager* manager, const DnfSchl* record);
int DnfSchl_Delete(DBConnectionManager* manager, signed char server_group, const char* end_date, const char* st_date, const char* schl_cd);
int DnfSchl_Exists(DBConnectionManager* manager, signed char server_group, const char* end_date, const char* st_date, const char* schl_cd);
int DnfSchl_GetAll(DBConnectionManager* manager, DnfSchl* records, int max_count, int* actual_count);

#endif /* DNF_SCHL_ORM_H */