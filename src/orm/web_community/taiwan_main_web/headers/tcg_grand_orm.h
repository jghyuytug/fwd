#ifndef TCG_GRAND_ORM_H
#define TCG_GRAND_ORM_H

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
} TcgGrand;

/* CRUD Operations */
int TcgGrand_Add(DBConnectionManager* manager, const TcgGrand* record);
int TcgGrand_Get(DBConnectionManager* manager, int no, TcgGrand* record);
int TcgGrand_Update(DBConnectionManager* manager, const TcgGrand* record);
int TcgGrand_Delete(DBConnectionManager* manager, int no);
int TcgGrand_Exists(DBConnectionManager* manager, int no);
int TcgGrand_GetAll(DBConnectionManager* manager, TcgGrand* records, int max_count, int* actual_count);

#endif /* TCG_GRAND_ORM_H */