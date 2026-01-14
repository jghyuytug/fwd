#ifndef TCG_FREE_ORM_H
#define TCG_FREE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char notice;
    signed char category;
    int m_id;
    char m_sex;
    int charac_no;
    char charac_name[21];
    signed char job;
    char title[121];
    int create_day;
    short comment;
    int view;
    int file;
} TcgFree;

/* CRUD Operations */
int TcgFree_Add(DBConnectionManager* manager, const TcgFree* record);
int TcgFree_Get(DBConnectionManager* manager, int no, TcgFree* record);
int TcgFree_Update(DBConnectionManager* manager, const TcgFree* record);
int TcgFree_Delete(DBConnectionManager* manager, int no);
int TcgFree_Exists(DBConnectionManager* manager, int no);
int TcgFree_GetAll(DBConnectionManager* manager, TcgFree* records, int max_count, int* actual_count);

#endif /* TCG_FREE_ORM_H */