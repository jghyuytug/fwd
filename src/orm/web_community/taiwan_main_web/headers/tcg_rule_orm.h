#ifndef TCG_RULE_ORM_H
#define TCG_RULE_ORM_H

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
} TcgRule;

/* CRUD Operations */
int TcgRule_Add(DBConnectionManager* manager, const TcgRule* record);
int TcgRule_Get(DBConnectionManager* manager, int no, TcgRule* record);
int TcgRule_Update(DBConnectionManager* manager, const TcgRule* record);
int TcgRule_Delete(DBConnectionManager* manager, int no);
int TcgRule_Exists(DBConnectionManager* manager, int no);
int TcgRule_GetAll(DBConnectionManager* manager, TcgRule* records, int max_count, int* actual_count);

#endif /* TCG_RULE_ORM_H */