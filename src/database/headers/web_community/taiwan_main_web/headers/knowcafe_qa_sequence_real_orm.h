#ifndef KNOWCAFE_QA_SEQUENCE_REAL_ORM_H
#define KNOWCAFE_QA_SEQUENCE_REAL_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int sequence;
    long long cnt;
} KnowcafeQaSequenceReal;

/* CRUD Operations */
int KnowcafeQaSequenceReal_Add(DBConnectionManager* manager, const KnowcafeQaSequenceReal* record);
int KnowcafeQaSequenceReal_GetAll(DBConnectionManager* manager, KnowcafeQaSequenceReal* records, int max_count, int* actual_count);

#endif /* KNOWCAFE_QA_SEQUENCE_REAL_ORM_H */