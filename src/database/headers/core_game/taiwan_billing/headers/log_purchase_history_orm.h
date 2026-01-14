#ifndef LOG_PURCHASE_HISTORY_ORM_H
#define LOG_PURCHASE_HISTORY_ORM_H

#include "db_connection_manager.h"
#include <stdint.h>

typedef struct {
    uint64_t tran_id;           /* BIGINT UNSIGNED primary key */
    unsigned char tran_state;   /* TINYINT UNSIGNED */
    char account_id[31];        /* CHAR(30) -> 31 */
    char charac_id[31];         /* CHAR(30) -> 31 */
    unsigned int item_id;       /* INT UNSIGNED */
    unsigned int cera;          /* INT UNSIGNED - consumed cera */
    unsigned int befor_cera;    /* INT UNSIGNED - before purchase */
    unsigned int after_cera;    /* INT UNSIGNED - after purchase */
    char query_user[46];        /* VARCHAR(45) -> 46, default 'None' */
    char occ_date[20];          /* DATETIME */
} LogPurchaseHistory;

/* CRUD Operations */
int LogPurchaseHistory_Add(DBConnectionManager* manager, const LogPurchaseHistory* record);
int LogPurchaseHistory_Get(DBConnectionManager* manager, uint64_t tran_id, LogPurchaseHistory* record);
int LogPurchaseHistory_Delete(DBConnectionManager* manager, uint64_t tran_id);
int LogPurchaseHistory_Exists(DBConnectionManager* manager, uint64_t tran_id);

/* Business Operations */
int LogPurchaseHistory_GetRecent(DBConnectionManager* manager, LogPurchaseHistory* records,
                                  int max_count, int* actual_count);
int LogPurchaseHistory_GetByAccount(DBConnectionManager* manager, const char* account_id,
                                     LogPurchaseHistory* records, int max_count, int* actual_count);
int LogPurchaseHistory_GetByCharacter(DBConnectionManager* manager, const char* charac_id,
                                       LogPurchaseHistory* records, int max_count, int* actual_count);
int LogPurchaseHistory_GetByItem(DBConnectionManager* manager, unsigned int item_id,
                                  LogPurchaseHistory* records, int max_count, int* actual_count);
int LogPurchaseHistory_GetByDateRange(DBConnectionManager* manager,
                                        const char* start_date, const char* end_date,
                                        LogPurchaseHistory* records, int max_count, int* actual_count);
int LogPurchaseHistory_GetByState(DBConnectionManager* manager, unsigned char tran_state,
                                    LogPurchaseHistory* records, int max_count, int* actual_count);
int LogPurchaseHistory_GetTotalSpent(DBConnectionManager* manager, const char* account_id,
                                      unsigned int* total_cera);
int LogPurchaseHistory_GetCount(DBConnectionManager* manager, int* count);

/* Utility Functions */
void LogPurchaseHistory_PrintInfo(const LogPurchaseHistory* record);

#endif /* LOG_PURCHASE_HISTORY_ORM_H */
