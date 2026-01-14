#ifndef REALRADIO_1YEAR_COMMENT_ORM_H
#define REALRADIO_1YEAR_COMMENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int m_id;
    signed char server_id;
    int charac_no;
    char charac_name[21];
    char comment[101];
} Realradio1YearComment;

/* CRUD Operations */
int Realradio1YearComment_Add(DBConnectionManager* manager, const Realradio1YearComment* record);
int Realradio1YearComment_Get(DBConnectionManager* manager, int no, Realradio1YearComment* record);
int Realradio1YearComment_Update(DBConnectionManager* manager, const Realradio1YearComment* record);
int Realradio1YearComment_Delete(DBConnectionManager* manager, int no);
int Realradio1YearComment_Exists(DBConnectionManager* manager, int no);
int Realradio1YearComment_GetAll(DBConnectionManager* manager, Realradio1YearComment* records, int max_count, int* actual_count);

#endif /* REALRADIO_1YEAR_COMMENT_ORM_H */