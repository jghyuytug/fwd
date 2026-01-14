#ifndef TCG_GRAND_CONTENT_ORM_H
#define TCG_GRAND_CONTENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char content[1024];
    char content_type;
    char ip[16];
    short ring;
    char sms;
} TcgGrandContent;

/* CRUD Operations */
int TcgGrandContent_Add(DBConnectionManager* manager, const TcgGrandContent* record);
int TcgGrandContent_Get(DBConnectionManager* manager, int no, TcgGrandContent* record);
int TcgGrandContent_Update(DBConnectionManager* manager, const TcgGrandContent* record);
int TcgGrandContent_Delete(DBConnectionManager* manager, int no);
int TcgGrandContent_Exists(DBConnectionManager* manager, int no);
int TcgGrandContent_GetAll(DBConnectionManager* manager, TcgGrandContent* records, int max_count, int* actual_count);

#endif /* TCG_GRAND_CONTENT_ORM_H */