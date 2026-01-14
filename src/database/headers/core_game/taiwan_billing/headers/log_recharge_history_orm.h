#ifndef LOG_RECHARGE_HISTORY_ORM_H
#define LOG_RECHARGE_HISTORY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned long long tran_id;
    char order_tran_id[36];
    unsigned char tran_state;
    char account_id[31];
    char charac_id[31];
    unsigned int cera;
    unsigned int befor_cera;
    unsigned int after_cera;
    unsigned char charge_type;
    char query_user[46];
    char occ_date[20];
} LogRechargeHistory;

/* CRUD Operations */
int LogRechargeHistory_Add(DBConnectionManager* manager, const LogRechargeHistory* log);
int LogRechargeHistory_Get(DBConnectionManager* manager, unsigned long long tran_id, LogRechargeHistory* log);
int LogRechargeHistory_Update(DBConnectionManager* manager, const LogRechargeHistory* log);
int LogRechargeHistory_Delete(DBConnectionManager* manager, unsigned long long tran_id);
int LogRechargeHistory_Exists(DBConnectionManager* manager, unsigned long long tran_id);

/* Business Operations */
int LogRechargeHistory_GetAll(DBConnectionManager* manager, LogRechargeHistory* logs,
                               int max_count, int* actual_count);
int LogRechargeHistory_GetByAccount(DBConnectionManager* manager, const char* account_id,
                                     LogRechargeHistory* logs, int max_count, int* actual_count);
int LogRechargeHistory_GetByCharac(DBConnectionManager* manager, const char* charac_id,
                                    LogRechargeHistory* logs, int max_count, int* actual_count);
int LogRechargeHistory_GetByDateRange(DBConnectionManager* manager, const char* start_date,
                                       const char* end_date, LogRechargeHistory* logs,
                                       int max_count, int* actual_count);

/* Utility Functions */
void LogRechargeHistory_PrintInfo(const LogRechargeHistory* log);

#endif
