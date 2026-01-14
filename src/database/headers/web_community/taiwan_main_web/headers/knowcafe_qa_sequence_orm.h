#ifndef KNOWCAFE_QA_SEQUENCE_ORM_H
#define KNOWCAFE_QA_SEQUENCE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int sequence;
} KnowcafeQaSequence;

/* CRUD Operations */
int KnowcafeQaSequence_Add(DBConnectionManager* manager, const KnowcafeQaSequence* record);
int KnowcafeQaSequence_GetAll(DBConnectionManager* manager, KnowcafeQaSequence* records, int max_count, int* actual_count);

#endif /* KNOWCAFE_QA_SEQUENCE_ORM_H */