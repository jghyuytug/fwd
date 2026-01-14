#ifndef REPL_COMMENT_ORM_H
#define REPL_COMMENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int rno;
    short comm_no;
    signed char imtc_no;
    int m_id;
    char reg_id[21];
    char pawd[17];
    int reg_date;
    char comment[201];
} ReplComment;

/* CRUD Operations */
int ReplComment_Add(DBConnectionManager* manager, const ReplComment* record);
int ReplComment_Get(DBConnectionManager* manager, int rno, short comm_no, ReplComment* record);
int ReplComment_Update(DBConnectionManager* manager, const ReplComment* record);
int ReplComment_Delete(DBConnectionManager* manager, int rno, short comm_no);
int ReplComment_Exists(DBConnectionManager* manager, int rno, short comm_no);
int ReplComment_GetAll(DBConnectionManager* manager, ReplComment* records, int max_count, int* actual_count);

#endif /* REPL_COMMENT_ORM_H */