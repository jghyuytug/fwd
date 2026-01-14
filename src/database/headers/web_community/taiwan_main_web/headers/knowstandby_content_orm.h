#ifndef KNOWSTANDBY_CONTENT_ORM_H
#define KNOWSTANDBY_CONTENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char content[1024];
    char content_type;
    char ip[16];
} KnowstandbyContent;

/* CRUD Operations */
int KnowstandbyContent_Add(DBConnectionManager* manager, const KnowstandbyContent* record);
int KnowstandbyContent_Get(DBConnectionManager* manager, int no, KnowstandbyContent* record);
int KnowstandbyContent_Update(DBConnectionManager* manager, const KnowstandbyContent* record);
int KnowstandbyContent_Delete(DBConnectionManager* manager, int no);
int KnowstandbyContent_Exists(DBConnectionManager* manager, int no);
int KnowstandbyContent_GetAll(DBConnectionManager* manager, KnowstandbyContent* records, int max_count, int* actual_count);

#endif /* KNOWSTANDBY_CONTENT_ORM_H */