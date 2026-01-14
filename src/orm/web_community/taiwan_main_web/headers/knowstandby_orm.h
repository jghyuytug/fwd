#ifndef KNOWSTANDBY_ORM_H
#define KNOWSTANDBY_ORM_H

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
} Knowstandby;

/* CRUD Operations */
int Knowstandby_Add(DBConnectionManager* manager, const Knowstandby* record);
int Knowstandby_Get(DBConnectionManager* manager, int no, Knowstandby* record);
int Knowstandby_Update(DBConnectionManager* manager, const Knowstandby* record);
int Knowstandby_Delete(DBConnectionManager* manager, int no);
int Knowstandby_Exists(DBConnectionManager* manager, int no);
int Knowstandby_GetAll(DBConnectionManager* manager, Knowstandby* records, int max_count, int* actual_count);

#endif /* KNOWSTANDBY_ORM_H */