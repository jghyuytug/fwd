#ifndef UCC_PUBLIC_INFO_CONTENT_ORM_H
#define UCC_PUBLIC_INFO_CONTENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char content[1024];
    char content_type;
    char ip[16];
    short ring;
    char sms;
} UccPublicInfoContent;

/* CRUD Operations */
int UccPublicInfoContent_Add(DBConnectionManager* manager, const UccPublicInfoContent* record);
int UccPublicInfoContent_Get(DBConnectionManager* manager, int no, UccPublicInfoContent* record);
int UccPublicInfoContent_Update(DBConnectionManager* manager, const UccPublicInfoContent* record);
int UccPublicInfoContent_Delete(DBConnectionManager* manager, int no);
int UccPublicInfoContent_Exists(DBConnectionManager* manager, int no);
int UccPublicInfoContent_GetAll(DBConnectionManager* manager, UccPublicInfoContent* records, int max_count, int* actual_count);

#endif /* UCC_PUBLIC_INFO_CONTENT_ORM_H */