#ifndef KNOWCAFE_QA_CONTENT_ORM_H
#define KNOWCAFE_QA_CONTENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char content[1024];
    char content_type;
    char ip[16];
    short ring;
    char sms;
} KnowcafeQaContent;

/* CRUD Operations */
int KnowcafeQaContent_Add(DBConnectionManager* manager, const KnowcafeQaContent* record);
int KnowcafeQaContent_Get(DBConnectionManager* manager, int no, KnowcafeQaContent* record);
int KnowcafeQaContent_Update(DBConnectionManager* manager, const KnowcafeQaContent* record);
int KnowcafeQaContent_Delete(DBConnectionManager* manager, int no);
int KnowcafeQaContent_Exists(DBConnectionManager* manager, int no);
int KnowcafeQaContent_GetAll(DBConnectionManager* manager, KnowcafeQaContent* records, int max_count, int* actual_count);

#endif /* KNOWCAFE_QA_CONTENT_ORM_H */