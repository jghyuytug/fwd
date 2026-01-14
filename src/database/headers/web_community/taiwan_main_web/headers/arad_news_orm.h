#ifndef ARAD_NEWS_ORM_H
#define ARAD_NEWS_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char notice;
    signed char category;
    int m_id;
    signed char server_id;
    int charac_no;
    char charac_name[21];
    signed char job;
    signed char lev;
    char title[121];
    int create_day;
    short comment;
    int view;
    int recom;
    char source_url[129];
    signed char copyright;
    char tag[129];
    int no_recom;
    signed char is_main;
} AradNews;

/* CRUD Operations */
int AradNews_Add(DBConnectionManager* manager, const AradNews* record);
int AradNews_Get(DBConnectionManager* manager, int no, AradNews* record);
int AradNews_Update(DBConnectionManager* manager, const AradNews* record);
int AradNews_Delete(DBConnectionManager* manager, int no);
int AradNews_Exists(DBConnectionManager* manager, int no);
int AradNews_GetAll(DBConnectionManager* manager, AradNews* records, int max_count, int* actual_count);

#endif /* ARAD_NEWS_ORM_H */