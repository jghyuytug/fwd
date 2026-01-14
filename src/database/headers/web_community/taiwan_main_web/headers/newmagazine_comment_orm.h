#ifndef NEWMAGAZINE_COMMENT_ORM_H
#define NEWMAGAZINE_COMMENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int mz_id;
    int m_id;
    char m_sex;
    signed char server_id;
    int charac_no;
    char charac_name[21];
    signed char job;
    char comment_text[201];
    int create_day;
} NewmagazineComment;

/* CRUD Operations */
int NewmagazineComment_Add(DBConnectionManager* manager, const NewmagazineComment* record);
int NewmagazineComment_Get(DBConnectionManager* manager, int no, NewmagazineComment* record);
int NewmagazineComment_Update(DBConnectionManager* manager, const NewmagazineComment* record);
int NewmagazineComment_Delete(DBConnectionManager* manager, int no);
int NewmagazineComment_Exists(DBConnectionManager* manager, int no);
int NewmagazineComment_GetAll(DBConnectionManager* manager, NewmagazineComment* records, int max_count, int* actual_count);

#endif /* NEWMAGAZINE_COMMENT_ORM_H */