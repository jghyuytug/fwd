#ifndef CRASH_DOWN_BY_PLAYSTATUS_ORM_H
#define CRASH_DOWN_BY_PLAYSTATUS_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_date[11];
    signed char play_status;
    int down_count;
} CrashDownByPlaystatus;

/* CRUD Operations */
int CrashDownByPlaystatus_Add(DBConnectionManager* manager, const CrashDownByPlaystatus* record);
int CrashDownByPlaystatus_Get(DBConnectionManager* manager, const char* occ_date, signed char play_status, CrashDownByPlaystatus* record);
int CrashDownByPlaystatus_Update(DBConnectionManager* manager, const CrashDownByPlaystatus* record);
int CrashDownByPlaystatus_Delete(DBConnectionManager* manager, const char* occ_date, signed char play_status);
int CrashDownByPlaystatus_Exists(DBConnectionManager* manager, const char* occ_date, signed char play_status);
int CrashDownByPlaystatus_GetAll(DBConnectionManager* manager, CrashDownByPlaystatus* records, int max_count, int* actual_count);

#endif /* CRASH_DOWN_BY_PLAYSTATUS_ORM_H */