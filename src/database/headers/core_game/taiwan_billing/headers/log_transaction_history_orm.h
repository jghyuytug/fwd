#ifndef LOG_TRANSACTION_HISTORY_ORM_H
#define LOG_TRANSACTION_HISTORY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned long long tran_id;
    unsigned char tran_type;
    char occ_date[20];
} LogTransactionHistory;

/* CRUD Operations */
int LogTransactionHistory_Add(DBConnectionManager* manager, const LogTransactionHistory* log);
int LogTransactionHistory_Get(DBConnectionManager* manager, unsigned long long tran_id,
                                LogTransactionHistory* log);
int LogTransactionHistory_Update(DBConnectionManager* manager, const LogTransactionHistory* log);
int LogTransactionHistory_Delete(DBConnectionManager* manager, unsigned long long tran_id);
int LogTransactionHistory_Exists(DBConnectionManager* manager, unsigned long long tran_id);

/* Business Operations */
int LogTransactionHistory_GetAll(DBConnectionManager* manager, LogTransactionHistory* logs,
                                   int max_count, int* actual_count);
int LogTransactionHistory_GetByType(DBConnectionManager* manager, unsigned char tran_type,
                                     LogTransactionHistory* logs, int max_count, int* actual_count);
int LogTransactionHistory_GetByDateRange(DBConnectionManager* manager, const char* start_date,
                                           const char* end_date, LogTransactionHistory* logs,
                                           int max_count, int* actual_count);

/* Utility Functions */
void LogTransactionHistory_PrintInfo(const LogTransactionHistory* log);

#endif
