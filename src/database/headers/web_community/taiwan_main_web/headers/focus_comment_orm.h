#ifndef FOCUS_COMMENT_ORM_H
#define FOCUS_COMMENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int content_no;
    char m_nickname[13];
    int m_id;
    char m_user_id[17];
    char m_sex;
    signed char server_id;
    int charac_no;
    char charac_name[21];
    signed char job;
    char comment_text[256];
    char ip[16];
    int create_day;
} FocusComment;

/* CRUD Operations */
int FocusComment_Add(DBConnectionManager* manager, const FocusComment* record);
int FocusComment_Get(DBConnectionManager* manager, int no, FocusComment* record);
int FocusComment_Update(DBConnectionManager* manager, const FocusComment* record);
int FocusComment_Delete(DBConnectionManager* manager, int no);
int FocusComment_Exists(DBConnectionManager* manager, int no);
int FocusComment_GetAll(DBConnectionManager* manager, FocusComment* records, int max_count, int* actual_count);

#endif /* FOCUS_COMMENT_ORM_H */