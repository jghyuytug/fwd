#ifndef KNOWLEDGE_ORM_H
#define KNOWLEDGE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    long long kn_id;
    signed char question_flag;
    int m_id;
    signed char server_id;
    int charac_no;
    char charac_name[21];
    signed char job;
    char title[121];
    signed char know_flag;
    int create_day;
    short comment;
    int view;
    int value;
    int file;
} Knowledge;

/* CRUD Operations */
int Knowledge_Add(DBConnectionManager* manager, const Knowledge* record);
int Knowledge_Get(DBConnectionManager* manager, int no, Knowledge* record);
int Knowledge_Update(DBConnectionManager* manager, const Knowledge* record);
int Knowledge_Delete(DBConnectionManager* manager, int no);
int Knowledge_Exists(DBConnectionManager* manager, int no);
int Knowledge_GetAll(DBConnectionManager* manager, Knowledge* records, int max_count, int* actual_count);

#endif /* KNOWLEDGE_ORM_H */