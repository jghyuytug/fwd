#ifndef POWERCLUB_DISCUSSION_ORM_H
#define POWERCLUB_DISCUSSION_ORM_H

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
    int recom;
    int file;
    signed char depth;
    double sequence;
    int positive;
    int negative;
    int bbs_no;
    signed char positive_mark;
    signed char negative_mark;
} PowerclubDiscussion;

/* CRUD Operations */
int PowerclubDiscussion_Add(DBConnectionManager* manager, const PowerclubDiscussion* record);
int PowerclubDiscussion_Get(DBConnectionManager* manager, int no, PowerclubDiscussion* record);
int PowerclubDiscussion_Update(DBConnectionManager* manager, const PowerclubDiscussion* record);
int PowerclubDiscussion_Delete(DBConnectionManager* manager, int no);
int PowerclubDiscussion_Exists(DBConnectionManager* manager, int no);
int PowerclubDiscussion_GetAll(DBConnectionManager* manager, PowerclubDiscussion* records, int max_count, int* actual_count);

#endif /* POWERCLUB_DISCUSSION_ORM_H */