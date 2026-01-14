#ifndef CASH_CERA_POINT_ORM_H
#define CASH_CERA_POINT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char account[31];           /* VARCHAR(30) -> 31 */
    unsigned int cera_point;    /* INT UNSIGNED */
    char reg_date[20];          /* DATETIME */
    char mod_date[20];          /* DATETIME */
} CashCeraPoint;

/* CRUD Operations */
int CashCeraPoint_Add(DBConnectionManager* manager, const CashCeraPoint* record);
int CashCeraPoint_Get(DBConnectionManager* manager, const char* account, CashCeraPoint* record);
int CashCeraPoint_Update(DBConnectionManager* manager, const CashCeraPoint* record);
int CashCeraPoint_Delete(DBConnectionManager* manager, const char* account);
int CashCeraPoint_Exists(DBConnectionManager* manager, const char* account);

/* Business Operations */
int CashCeraPoint_AddPoints(DBConnectionManager* manager, const char* account, unsigned int amount);
int CashCeraPoint_DeductPoints(DBConnectionManager* manager, const char* account, unsigned int amount);
int CashCeraPoint_GetBalance(DBConnectionManager* manager, const char* account, unsigned int* balance);
int CashCeraPoint_GetAll(DBConnectionManager* manager, CashCeraPoint* records,
                         int max_count, int* actual_count);
int CashCeraPoint_GetByPointsRange(DBConnectionManager* manager,
                                    unsigned int min_points, unsigned int max_points,
                                    CashCeraPoint* records, int max_count, int* actual_count);

/* Utility Functions */
void CashCeraPoint_PrintInfo(const CashCeraPoint* record);

#endif /* CASH_CERA_POINT_ORM_H */
