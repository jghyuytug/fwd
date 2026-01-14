#ifndef LOG_GIFT_HISTORY_ORM_H
#define LOG_GIFT_HISTORY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned long long tran_id;
    unsigned char tran_state;
    char send_account_id[31];
    char send_charac_id[31];
    char recv_account_id[31];
    unsigned int item_id;
    unsigned int cera;
    unsigned int send_befor_cera;
    unsigned int send_after_cera;
    unsigned int recv_befor_cera;
    unsigned int recv_after_cera;
    char query_user[46];
    char occ_date[20];
} LogGiftHistory;

/* CRUD Operations */
int LogGiftHistory_Add(DBConnectionManager* manager, const LogGiftHistory* log);
int LogGiftHistory_Get(DBConnectionManager* manager, unsigned long long tran_id, LogGiftHistory* log);
int LogGiftHistory_Update(DBConnectionManager* manager, const LogGiftHistory* log);
int LogGiftHistory_Delete(DBConnectionManager* manager, unsigned long long tran_id);
int LogGiftHistory_Exists(DBConnectionManager* manager, unsigned long long tran_id);

/* Business Operations */
int LogGiftHistory_GetAll(DBConnectionManager* manager, LogGiftHistory* logs,
                           int max_count, int* actual_count);
int LogGiftHistory_GetByAccount(DBConnectionManager* manager, const char* account_id,
                                 LogGiftHistory* logs, int max_count, int* actual_count);
int LogGiftHistory_GetByState(DBConnectionManager* manager, unsigned char state,
                               LogGiftHistory* logs, int max_count, int* actual_count);
int LogGiftHistory_GetByDateRange(DBConnectionManager* manager, const char* start_date,
                                   const char* end_date, LogGiftHistory* logs,
                                   int max_count, int* actual_count);

/* Utility Functions */
void LogGiftHistory_PrintInfo(const LogGiftHistory* log);

#endif
