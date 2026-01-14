#ifndef PENPIC_CONTENT_ORM_H
#define PENPIC_CONTENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char content[1024];
    char content_type;
    char ip[16];
    short ring;
    char sms;
} PenpicContent;

/* CRUD Operations */
int PenpicContent_Add(DBConnectionManager* manager, const PenpicContent* record);
int PenpicContent_Get(DBConnectionManager* manager, int no, PenpicContent* record);
int PenpicContent_Update(DBConnectionManager* manager, const PenpicContent* record);
int PenpicContent_Delete(DBConnectionManager* manager, int no);
int PenpicContent_Exists(DBConnectionManager* manager, int no);
int PenpicContent_GetAll(DBConnectionManager* manager, PenpicContent* records, int max_count, int* actual_count);

#endif /* PENPIC_CONTENT_ORM_H */