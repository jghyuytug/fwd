#ifndef TEST_ORM_H
#define TEST_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int a;
    char b[20];  /* DATETIME as string 'YYYY-MM-DD HH:MM:SS' */
    int a_is_null;
    int b_is_null;
} TestRecord;

/* Basic Operations */
int Test_Add(DBConnectionManager* manager, const TestRecord* record);
int Test_GetAll(DBConnectionManager* manager, TestRecord* records,
                int max_count, int* actual_count);
int Test_Count(DBConnectionManager* manager, int* count);
int Test_Clear(DBConnectionManager* manager);

/* Query Operations */
int Test_GetByA(DBConnectionManager* manager, int a, TestRecord* records,
                int max_count, int* actual_count);
int Test_DeleteByA(DBConnectionManager* manager, int a);

/* Utility Functions */
void Test_PrintInfo(const TestRecord* record);

#endif
