#ifndef KNOWCAFE_QA_INFO_ORM_H
#define KNOWCAFE_QA_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char notice;
    signed char category;
    signed char depth;
    double sequence;
} KnowcafeQaInfo;

/* CRUD Operations */
int KnowcafeQaInfo_Add(DBConnectionManager* manager, const KnowcafeQaInfo* record);
int KnowcafeQaInfo_Get(DBConnectionManager* manager, int no, KnowcafeQaInfo* record);
int KnowcafeQaInfo_Update(DBConnectionManager* manager, const KnowcafeQaInfo* record);
int KnowcafeQaInfo_Delete(DBConnectionManager* manager, int no);
int KnowcafeQaInfo_Exists(DBConnectionManager* manager, int no);
int KnowcafeQaInfo_GetAll(DBConnectionManager* manager, KnowcafeQaInfo* records, int max_count, int* actual_count);

#endif /* KNOWCAFE_QA_INFO_ORM_H */