#ifndef HUMAN_CERTIFY_TRY_COUNT_ORM_H
#define HUMAN_CERTIFY_TRY_COUNT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    unsigned int count;
} HumanCertifyTryCount;

/* CRUD Operations */
int HumanCertifyTryCount_Add(DBConnectionManager* manager, const HumanCertifyTryCount* cert);
int HumanCertifyTryCount_Get(DBConnectionManager* manager, int m_id, HumanCertifyTryCount* cert);
int HumanCertifyTryCount_Update(DBConnectionManager* manager, const HumanCertifyTryCount* cert);
int HumanCertifyTryCount_Delete(DBConnectionManager* manager, int m_id);
int HumanCertifyTryCount_Exists(DBConnectionManager* manager, int m_id);

/* Business Operations */
int HumanCertifyTryCount_Increment(DBConnectionManager* manager, int m_id);
int HumanCertifyTryCount_Reset(DBConnectionManager* manager, int m_id);
int HumanCertifyTryCount_GetAll(DBConnectionManager* manager,
                                 HumanCertifyTryCount* certs,
                                 int max_count,
                                 int* actual_count);

/* Utility */
void HumanCertifyTryCount_PrintInfo(const HumanCertifyTryCount* cert);

#endif
