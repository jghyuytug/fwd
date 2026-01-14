#ifndef NOTICE_ORM_H
#define NOTICE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char bbs_name[11];
    unsigned int no;
    unsigned char category;
    char m_nickname[13];
    unsigned int m_id;
    char m_user_id[17];
    char m_sex;
    char title[121];
    unsigned int create_day;
    unsigned short comment;
    unsigned int view;
    unsigned int recom;
    unsigned char adorn;
    unsigned char adorn_color1;
    unsigned char adorn_color2;
    unsigned char depth;
    double sequence;
    char* content;
    char content_type[5];
    char ip[16];
    unsigned short ring;
    char sms;
} Notice;

/* CRUD Operations */
int Notice_Add(DBConnectionManager* manager, const Notice* notice);
int Notice_Get(DBConnectionManager* manager, unsigned int no, Notice* notice);
int Notice_Update(DBConnectionManager* manager, const Notice* notice);
int Notice_Delete(DBConnectionManager* manager, unsigned int no);
int Notice_Exists(DBConnectionManager* manager, unsigned int no);

/* Business Operations */
int Notice_GetByBbsName(DBConnectionManager* manager, const char* bbs_name,
                        Notice* notices, int max_count, int* actual_count);
int Notice_GetByCategory(DBConnectionManager* manager, unsigned char category,
                         Notice* notices, int max_count, int* actual_count);
int Notice_GetByUserId(DBConnectionManager* manager, unsigned int m_id,
                       Notice* notices, int max_count, int* actual_count);
int Notice_GetRecent(DBConnectionManager* manager, int limit,
                     Notice* notices, int max_count, int* actual_count);

/* Utility Functions */
void Notice_Free(Notice* notice);
void Notice_PrintInfo(const Notice* notice);

#endif
