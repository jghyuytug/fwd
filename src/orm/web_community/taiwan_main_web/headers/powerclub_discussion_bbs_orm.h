#ifndef POWERCLUB_DISCUSSION_BBS_ORM_H
#define POWERCLUB_DISCUSSION_BBS_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char notice;
    signed char category;
    char m_nickname[13];
    int m_id;
    char m_user_id[17];
    char m_sex;
    signed char server_id;
    int charac_no;
    char charac_name[21];
    signed char job;
    char title[121];
    int create_day;
    int comment;
    int view;
} PowerclubDiscussionBbs;

/* CRUD Operations */
int PowerclubDiscussionBbs_Add(DBConnectionManager* manager, const PowerclubDiscussionBbs* record);
int PowerclubDiscussionBbs_Get(DBConnectionManager* manager, int no, PowerclubDiscussionBbs* record);
int PowerclubDiscussionBbs_Update(DBConnectionManager* manager, const PowerclubDiscussionBbs* record);
int PowerclubDiscussionBbs_Delete(DBConnectionManager* manager, int no);
int PowerclubDiscussionBbs_Exists(DBConnectionManager* manager, int no);
int PowerclubDiscussionBbs_GetAll(DBConnectionManager* manager, PowerclubDiscussionBbs* records, int max_count, int* actual_count);

#endif /* POWERCLUB_DISCUSSION_BBS_ORM_H */