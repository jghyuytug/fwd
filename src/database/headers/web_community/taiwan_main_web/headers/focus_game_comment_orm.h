#ifndef FOCUS_GAME_COMMENT_ORM_H
#define FOCUS_GAME_COMMENT_ORM_H

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
} FocusGameComment;

/* CRUD Operations */
int FocusGameComment_Add(DBConnectionManager* manager, const FocusGameComment* record);
int FocusGameComment_Get(DBConnectionManager* manager, int no, FocusGameComment* record);
int FocusGameComment_Update(DBConnectionManager* manager, const FocusGameComment* record);
int FocusGameComment_Delete(DBConnectionManager* manager, int no);
int FocusGameComment_Exists(DBConnectionManager* manager, int no);
int FocusGameComment_GetAll(DBConnectionManager* manager, FocusGameComment* records, int max_count, int* actual_count);

#endif /* FOCUS_GAME_COMMENT_ORM_H */