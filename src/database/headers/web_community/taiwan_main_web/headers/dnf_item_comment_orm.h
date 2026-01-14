#ifndef DNF_ITEM_COMMENT_ORM_H
#define DNF_ITEM_COMMENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int item_no;
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
    signed char recomm_score;
} DnfItemComment;

/* CRUD Operations */
int DnfItemComment_Add(DBConnectionManager* manager, const DnfItemComment* record);
int DnfItemComment_Get(DBConnectionManager* manager, int no, DnfItemComment* record);
int DnfItemComment_Update(DBConnectionManager* manager, const DnfItemComment* record);
int DnfItemComment_Delete(DBConnectionManager* manager, int no);
int DnfItemComment_Exists(DBConnectionManager* manager, int no);
int DnfItemComment_GetAll(DBConnectionManager* manager, DnfItemComment* records, int max_count, int* actual_count);

#endif /* DNF_ITEM_COMMENT_ORM_H */