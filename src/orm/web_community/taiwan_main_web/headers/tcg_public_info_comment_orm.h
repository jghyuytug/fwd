#ifndef TCG_PUBLIC_INFO_COMMENT_ORM_H
#define TCG_PUBLIC_INFO_COMMENT_ORM_H

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
} TcgPublicInfoComment;

/* CRUD Operations */
int TcgPublicInfoComment_Add(DBConnectionManager* manager, const TcgPublicInfoComment* record);
int TcgPublicInfoComment_Get(DBConnectionManager* manager, int no, TcgPublicInfoComment* record);
int TcgPublicInfoComment_Update(DBConnectionManager* manager, const TcgPublicInfoComment* record);
int TcgPublicInfoComment_Delete(DBConnectionManager* manager, int no);
int TcgPublicInfoComment_Exists(DBConnectionManager* manager, int no);
int TcgPublicInfoComment_GetAll(DBConnectionManager* manager, TcgPublicInfoComment* records, int max_count, int* actual_count);

#endif /* TCG_PUBLIC_INFO_COMMENT_ORM_H */