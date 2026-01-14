#ifndef KNOWSTANDBY_COMMENT_ORM_H
#define KNOWSTANDBY_COMMENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int content_no;
    int m_id;
    signed char server_id;
    int charac_no;
    char charac_name[21];
    signed char job;
    char comment_text[1024];
    char ip[16];
    int create_day;
    short value;
} KnowstandbyComment;

/* CRUD Operations */
int KnowstandbyComment_Add(DBConnectionManager* manager, const KnowstandbyComment* record);
int KnowstandbyComment_Get(DBConnectionManager* manager, int no, KnowstandbyComment* record);
int KnowstandbyComment_Update(DBConnectionManager* manager, const KnowstandbyComment* record);
int KnowstandbyComment_Delete(DBConnectionManager* manager, int no);
int KnowstandbyComment_Exists(DBConnectionManager* manager, int no);
int KnowstandbyComment_GetAll(DBConnectionManager* manager, KnowstandbyComment* records, int max_count, int* actual_count);

#endif /* KNOWSTANDBY_COMMENT_ORM_H */