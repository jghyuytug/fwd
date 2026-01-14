#ifndef KNOWL_TEST_ORM_H
#define KNOWL_TEST_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int gno;
} KnowlTest;

/* CRUD Operations */
int KnowlTest_Add(DBConnectionManager* manager, const KnowlTest* record);
int KnowlTest_GetAll(DBConnectionManager* manager, KnowlTest* records, int max_count, int* actual_count);

#endif /* KNOWL_TEST_ORM_H */