#ifndef TCG_FREE_COMMENT_ORM_H
#define TCG_FREE_COMMENT_ORM_H

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
} TcgFreeComment;

/* CRUD Operations */
int TcgFreeComment_Add(DBConnectionManager* manager, const TcgFreeComment* record);
int TcgFreeComment_Get(DBConnectionManager* manager, int no, TcgFreeComment* record);
int TcgFreeComment_Update(DBConnectionManager* manager, const TcgFreeComment* record);
int TcgFreeComment_Delete(DBConnectionManager* manager, int no);
int TcgFreeComment_Exists(DBConnectionManager* manager, int no);
int TcgFreeComment_GetAll(DBConnectionManager* manager, TcgFreeComment* records, int max_count, int* actual_count);

#endif /* TCG_FREE_COMMENT_ORM_H */