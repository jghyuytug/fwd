#ifndef KNOWCAFE_TIP_CONTENT_ORM_H
#define KNOWCAFE_TIP_CONTENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char content[1024];
    char content_type;
    char ip[16];
    short ring;
    char sms;
} KnowcafeTipContent;

/* CRUD Operations */
int KnowcafeTipContent_Add(DBConnectionManager* manager, const KnowcafeTipContent* record);
int KnowcafeTipContent_Get(DBConnectionManager* manager, int no, KnowcafeTipContent* record);
int KnowcafeTipContent_Update(DBConnectionManager* manager, const KnowcafeTipContent* record);
int KnowcafeTipContent_Delete(DBConnectionManager* manager, int no);
int KnowcafeTipContent_Exists(DBConnectionManager* manager, int no);
int KnowcafeTipContent_GetAll(DBConnectionManager* manager, KnowcafeTipContent* records, int max_count, int* actual_count);

#endif /* KNOWCAFE_TIP_CONTENT_ORM_H */