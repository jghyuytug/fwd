#ifndef INTENSIVE_DISCUSSION_ORM_H
#define INTENSIVE_DISCUSSION_ORM_H

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
} IntensiveDiscussion;

/* CRUD Operations */
int IntensiveDiscussion_Add(DBConnectionManager* manager, const IntensiveDiscussion* record);
int IntensiveDiscussion_Get(DBConnectionManager* manager, int no, IntensiveDiscussion* record);
int IntensiveDiscussion_Update(DBConnectionManager* manager, const IntensiveDiscussion* record);
int IntensiveDiscussion_Delete(DBConnectionManager* manager, int no);
int IntensiveDiscussion_Exists(DBConnectionManager* manager, int no);
int IntensiveDiscussion_GetAll(DBConnectionManager* manager, IntensiveDiscussion* records, int max_count, int* actual_count);

#endif /* INTENSIVE_DISCUSSION_ORM_H */