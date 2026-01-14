#ifndef LOG_REFUND_HISTORY_ORM_H
#define LOG_REFUND_HISTORY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned long long tran_id;
    char account_id[31];
    char order_tran_id[36];
    unsigned int amount;
    unsigned char tran_state;
    char query_user[46];
    char occ_date[20];
} LogRefundHistory;

/* CRUD Operations */
int LogRefundHistory_Add(DBConnectionManager* manager, const LogRefundHistory* refund);
int LogRefundHistory_Get(DBConnectionManager* manager, unsigned long long tran_id,
                          const char* account_id, LogRefundHistory* refund);
int LogRefundHistory_Update(DBConnectionManager* manager, const LogRefundHistory* refund);
int LogRefundHistory_Delete(DBConnectionManager* manager, unsigned long long tran_id,
                             const char* account_id);
int LogRefundHistory_Exists(DBConnectionManager* manager, unsigned long long tran_id,
                             const char* account_id);

/* Business Operations */
int LogRefundHistory_GetAll(DBConnectionManager* manager, LogRefundHistory* refunds,
                             int max_count, int* actual_count);
int LogRefundHistory_GetByAccount(DBConnectionManager* manager, const char* account_id,
                                   LogRefundHistory* refunds, int max_count, int* actual_count);
int LogRefundHistory_GetByState(DBConnectionManager* manager, unsigned char tran_state,
                                 LogRefundHistory* refunds, int max_count, int* actual_count);
int LogRefundHistory_GetByDateRange(DBConnectionManager* manager, const char* start_date,
                                     const char* end_date, LogRefundHistory* refunds,
                                     int max_count, int* actual_count);

/* Utility Functions */
void LogRefundHistory_PrintInfo(const LogRefundHistory* refund);

#endif
