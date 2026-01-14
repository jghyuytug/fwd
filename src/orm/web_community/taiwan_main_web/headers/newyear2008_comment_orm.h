#ifndef NEWYEAR2008_COMMENT_ORM_H
#define NEWYEAR2008_COMMENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int m_id;
    signed char server_id;
    int charac_no;
    char charac_name[21];
    char comment[101];
} Newyear2008Comment;

/* CRUD Operations */
int Newyear2008Comment_Add(DBConnectionManager* manager, const Newyear2008Comment* record);
int Newyear2008Comment_Get(DBConnectionManager* manager, int no, Newyear2008Comment* record);
int Newyear2008Comment_Update(DBConnectionManager* manager, const Newyear2008Comment* record);
int Newyear2008Comment_Delete(DBConnectionManager* manager, int no);
int Newyear2008Comment_Exists(DBConnectionManager* manager, int no);
int Newyear2008Comment_GetAll(DBConnectionManager* manager, Newyear2008Comment* records, int max_count, int* actual_count);

#endif /* NEWYEAR2008_COMMENT_ORM_H */