#ifndef NEWMAGAZINE_UCC_COMMENT_ORM_H
#define NEWMAGAZINE_UCC_COMMENT_ORM_H

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
} NewmagazineUccComment;

/* CRUD Operations */
int NewmagazineUccComment_Add(DBConnectionManager* manager, const NewmagazineUccComment* record);
int NewmagazineUccComment_Get(DBConnectionManager* manager, int no, NewmagazineUccComment* record);
int NewmagazineUccComment_Update(DBConnectionManager* manager, const NewmagazineUccComment* record);
int NewmagazineUccComment_Delete(DBConnectionManager* manager, int no);
int NewmagazineUccComment_Exists(DBConnectionManager* manager, int no);
int NewmagazineUccComment_GetAll(DBConnectionManager* manager, NewmagazineUccComment* records, int max_count, int* actual_count);

#endif /* NEWMAGAZINE_UCC_COMMENT_ORM_H */