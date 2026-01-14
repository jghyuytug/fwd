#ifndef LOG_CHARAC_MONEY_ORM_H
#define LOG_CHARAC_MONEY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int charac_no;
    char occ_date[11];
    int money_plus;
    int money_minus;
    int m_id;
} LogCharacMoney;

/* CRUD Operations */
int LogCharacMoney_Add(DBConnectionManager* manager, const LogCharacMoney* record);
int LogCharacMoney_Get(DBConnectionManager* manager, int charac_no, const char* occ_date, LogCharacMoney* record);
int LogCharacMoney_Update(DBConnectionManager* manager, const LogCharacMoney* record);
int LogCharacMoney_Delete(DBConnectionManager* manager, int charac_no, const char* occ_date);
int LogCharacMoney_Exists(DBConnectionManager* manager, int charac_no, const char* occ_date);
int LogCharacMoney_GetAll(DBConnectionManager* manager, LogCharacMoney* records, int max_count, int* actual_count);

#endif /* LOG_CHARAC_MONEY_ORM_H */