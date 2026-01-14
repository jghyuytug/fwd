#ifndef REPL_BBS_ORM_H
#define REPL_BBS_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int rno;
    signed char bd_id;
    double thread_id;
    double rev_thread;
    signed char depth;
    int reg_date;
    int mod_date;
    short hits;
    short recomm;
    short de_recom;
    short comment;
    char body_type[2];
    int m_id;
    char reg_id[21];
    char subject[51];
} ReplBbs;

/* CRUD Operations */
int ReplBbs_Add(DBConnectionManager* manager, const ReplBbs* record);
int ReplBbs_Get(DBConnectionManager* manager, int rno, ReplBbs* record);
int ReplBbs_Update(DBConnectionManager* manager, const ReplBbs* record);
int ReplBbs_Delete(DBConnectionManager* manager, int rno);
int ReplBbs_Exists(DBConnectionManager* manager, int rno);
int ReplBbs_GetAll(DBConnectionManager* manager, ReplBbs* records, int max_count, int* actual_count);

#endif /* REPL_BBS_ORM_H */