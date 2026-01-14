#ifndef CASH_TRANSACTION_ORM_H
#define CASH_TRANSACTION_ORM_H

#include "db_connection_manager.h"

/* Cash transaction record structure */
typedef struct {
    long long tran_id;    /* Transaction ID, auto_increment */
    char dummy;           /* Dummy field (placeholder) */
} CashTransaction;

/* CRUD Operations */
int CashTransaction_Add(DBConnectionManager* manager, const CashTransaction* transaction);
int CashTransaction_Get(DBConnectionManager* manager, long long tran_id,
                        CashTransaction* transaction);
int CashTransaction_Delete(DBConnectionManager* manager, long long tran_id);
int CashTransaction_Exists(DBConnectionManager* manager, long long tran_id);

/* Business Operations */
int CashTransaction_GetNextTranId(DBConnectionManager* manager, long long* next_id);
int CashTransaction_GetLatestTranId(DBConnectionManager* manager, long long* latest_id);
int CashTransaction_GetRange(DBConnectionManager* manager, long long start_id,
                             long long end_id, CashTransaction* transactions,
                             int max_count, int* actual_count);
int CashTransaction_GetAll(DBConnectionManager* manager, CashTransaction* transactions,
                           int max_count, int* actual_count);

/* Utility Functions */
void CashTransaction_PrintInfo(const CashTransaction* transaction);

#endif /* CASH_TRANSACTION_ORM_H */
