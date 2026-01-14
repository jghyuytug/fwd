#ifndef KNOWCAFE_JOB_CONTENT_ORM_H
#define KNOWCAFE_JOB_CONTENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char content[1024];
    char content_type;
    char ip[16];
    short ring;
    char sms;
} KnowcafeJobContent;

/* CRUD Operations */
int KnowcafeJobContent_Add(DBConnectionManager* manager, const KnowcafeJobContent* record);
int KnowcafeJobContent_Get(DBConnectionManager* manager, int no, KnowcafeJobContent* record);
int KnowcafeJobContent_Update(DBConnectionManager* manager, const KnowcafeJobContent* record);
int KnowcafeJobContent_Delete(DBConnectionManager* manager, int no);
int KnowcafeJobContent_Exists(DBConnectionManager* manager, int no);
int KnowcafeJobContent_GetAll(DBConnectionManager* manager, KnowcafeJobContent* records, int max_count, int* actual_count);

#endif /* KNOWCAFE_JOB_CONTENT_ORM_H */