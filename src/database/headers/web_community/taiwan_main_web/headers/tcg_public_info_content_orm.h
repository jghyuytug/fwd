#ifndef TCG_PUBLIC_INFO_CONTENT_ORM_H
#define TCG_PUBLIC_INFO_CONTENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char content[1024];
    char content_type;
    char ip[16];
    short ring;
    char sms;
} TcgPublicInfoContent;

/* CRUD Operations */
int TcgPublicInfoContent_Add(DBConnectionManager* manager, const TcgPublicInfoContent* record);
int TcgPublicInfoContent_Get(DBConnectionManager* manager, int no, TcgPublicInfoContent* record);
int TcgPublicInfoContent_Update(DBConnectionManager* manager, const TcgPublicInfoContent* record);
int TcgPublicInfoContent_Delete(DBConnectionManager* manager, int no);
int TcgPublicInfoContent_Exists(DBConnectionManager* manager, int no);
int TcgPublicInfoContent_GetAll(DBConnectionManager* manager, TcgPublicInfoContent* records, int max_count, int* actual_count);

#endif /* TCG_PUBLIC_INFO_CONTENT_ORM_H */