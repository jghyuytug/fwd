#ifndef LOG_RANDOM_OPTION_ORM_H
#define LOG_RANDOM_OPTION_ORM_H

#include "db_connection_manager.h"

typedef struct {
    signed char level;
    char occ_date[11];
    int unseal_cnt;
    long long unseal_gold;
    int regen_cnt;
    long long regen_gold;
    int change_cnt;
    long long change_gold;
    int pick_cnt;
    int reset_cnt;
} LogRandomOption;

/* CRUD Operations */
int LogRandomOption_Add(DBConnectionManager* manager, const LogRandomOption* record);
int LogRandomOption_Get(DBConnectionManager* manager, signed char level, const char* occ_date, LogRandomOption* record);
int LogRandomOption_Update(DBConnectionManager* manager, const LogRandomOption* record);
int LogRandomOption_Delete(DBConnectionManager* manager, signed char level, const char* occ_date);
int LogRandomOption_Exists(DBConnectionManager* manager, signed char level, const char* occ_date);
int LogRandomOption_GetAll(DBConnectionManager* manager, LogRandomOption* records, int max_count, int* actual_count);

#endif /* LOG_RANDOM_OPTION_ORM_H */