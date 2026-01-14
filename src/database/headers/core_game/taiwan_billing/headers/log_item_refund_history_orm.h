#ifndef LOG_ITEM_REFUND_HISTORY_ORM_H
#define LOG_ITEM_REFUND_HISTORY_ORM_H

#include "db_connection_manager.h"

/* Item refund history log record structure */
typedef struct {
    unsigned int pf_rel_id;           /* Primary key, auto_increment */
    unsigned long long purchase_tran_id;  /* Purchase transaction ID */
    unsigned long long recharge_tran_id;  /* Recharge transaction ID */
    char account_id[31];              /* Account ID (char 30) */
    char occ_date[20];                /* Occurrence datetime */
    char reason[256];                 /* Refund reason (varchar 255) */
    char admin_id[31];                /* Admin ID (varchar 30) */
    char query_user[46];              /* Query user (varchar 45) */
} LogItemRefundHistory;

/* CRUD Operations */
int LogItemRefundHistory_Add(DBConnectionManager* manager, const LogItemRefundHistory* log);
int LogItemRefundHistory_Get(DBConnectionManager* manager, unsigned int pf_rel_id,
                              LogItemRefundHistory* log);
int LogItemRefundHistory_Delete(DBConnectionManager* manager, unsigned int pf_rel_id);
int LogItemRefundHistory_Exists(DBConnectionManager* manager, unsigned int pf_rel_id);

/* Business Operations */
int LogItemRefundHistory_GetByAccountId(DBConnectionManager* manager, const char* account_id,
                                        LogItemRefundHistory* logs, int max_count,
                                        int* actual_count);
int LogItemRefundHistory_GetByPurchaseTranId(DBConnectionManager* manager,
                                              unsigned long long purchase_tran_id,
                                              LogItemRefundHistory* logs, int max_count,
                                              int* actual_count);
int LogItemRefundHistory_GetByRechargeTranId(DBConnectionManager* manager,
                                              unsigned long long recharge_tran_id,
                                              LogItemRefundHistory* logs, int max_count,
                                              int* actual_count);
int LogItemRefundHistory_GetByDateRange(DBConnectionManager* manager,
                                        const char* start_date, const char* end_date,
                                        LogItemRefundHistory* logs, int max_count,
                                        int* actual_count);
int LogItemRefundHistory_GetByAdmin(DBConnectionManager* manager, const char* admin_id,
                                    LogItemRefundHistory* logs, int max_count,
                                    int* actual_count);
int LogItemRefundHistory_GetRecent(DBConnectionManager* manager, int limit,
                                   LogItemRefundHistory* logs, int* actual_count);

/* Utility Functions */
void LogItemRefundHistory_PrintInfo(const LogItemRefundHistory* log);

#endif /* LOG_ITEM_REFUND_HISTORY_ORM_H */
