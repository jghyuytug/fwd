#ifndef LOG_NAT_DAILY_ORM_H
#define LOG_NAT_DAILY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_time[20];
    signed char nat_type;
    signed char nat_type_other;
    int success_cnt;
    int fail_cnt;
} LogNatDaily;

/* CRUD Operations */
int LogNatDaily_Add(DBConnectionManager* manager, const LogNatDaily* record);
int LogNatDaily_Get(DBConnectionManager* manager, const char* occ_time, signed char nat_type, signed char nat_type_other, LogNatDaily* record);
int LogNatDaily_Update(DBConnectionManager* manager, const LogNatDaily* record);
int LogNatDaily_Delete(DBConnectionManager* manager, const char* occ_time, signed char nat_type, signed char nat_type_other);
int LogNatDaily_Exists(DBConnectionManager* manager, const char* occ_time, signed char nat_type, signed char nat_type_other);
int LogNatDaily_GetAll(DBConnectionManager* manager, LogNatDaily* records, int max_count, int* actual_count);

#endif /* LOG_NAT_DAILY_ORM_H */