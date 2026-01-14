#ifndef LOG_NAT_STAT_ORM_H
#define LOG_NAT_STAT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_time[20];
    signed char nat_type;
    int nat_cnt;
} LogNatStat;

/* CRUD Operations */
int LogNatStat_Add(DBConnectionManager* manager, const LogNatStat* record);
int LogNatStat_Get(DBConnectionManager* manager, const char* occ_time, signed char nat_type, LogNatStat* record);
int LogNatStat_Update(DBConnectionManager* manager, const LogNatStat* record);
int LogNatStat_Delete(DBConnectionManager* manager, const char* occ_time, signed char nat_type);
int LogNatStat_Exists(DBConnectionManager* manager, const char* occ_time, signed char nat_type);
int LogNatStat_GetAll(DBConnectionManager* manager, LogNatStat* records, int max_count, int* actual_count);

#endif /* LOG_NAT_STAT_ORM_H */