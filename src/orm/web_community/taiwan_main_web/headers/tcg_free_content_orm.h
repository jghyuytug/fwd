#ifndef TCG_FREE_CONTENT_ORM_H
#define TCG_FREE_CONTENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char content[1024];
    char content_type;
    char ip[16];
    short ring;
    char sms;
} TcgFreeContent;

/* CRUD Operations */
int TcgFreeContent_Add(DBConnectionManager* manager, const TcgFreeContent* record);
int TcgFreeContent_Get(DBConnectionManager* manager, int no, TcgFreeContent* record);
int TcgFreeContent_Update(DBConnectionManager* manager, const TcgFreeContent* record);
int TcgFreeContent_Delete(DBConnectionManager* manager, int no);
int TcgFreeContent_Exists(DBConnectionManager* manager, int no);
int TcgFreeContent_GetAll(DBConnectionManager* manager, TcgFreeContent* records, int max_count, int* actual_count);

#endif /* TCG_FREE_CONTENT_ORM_H */