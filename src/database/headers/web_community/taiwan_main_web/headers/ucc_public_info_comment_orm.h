#ifndef UCC_PUBLIC_INFO_COMMENT_ORM_H
#define UCC_PUBLIC_INFO_COMMENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int content_no;
    int m_id;
    int charac_no;
    char charac_name[21];
    char comment_text[105];
    char ip[16];
    int create_day;
} UccPublicInfoComment;

/* CRUD Operations */
int UccPublicInfoComment_Add(DBConnectionManager* manager, const UccPublicInfoComment* record);
int UccPublicInfoComment_Get(DBConnectionManager* manager, int no, UccPublicInfoComment* record);
int UccPublicInfoComment_Update(DBConnectionManager* manager, const UccPublicInfoComment* record);
int UccPublicInfoComment_Delete(DBConnectionManager* manager, int no);
int UccPublicInfoComment_Exists(DBConnectionManager* manager, int no);
int UccPublicInfoComment_GetAll(DBConnectionManager* manager, UccPublicInfoComment* records, int max_count, int* actual_count);

#endif /* UCC_PUBLIC_INFO_COMMENT_ORM_H */