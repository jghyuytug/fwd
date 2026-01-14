#ifndef REPL_BODY_ORM_H
#define REPL_BODY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int rno;
    char body[1024];
} ReplBody;

/* CRUD Operations */
int ReplBody_Add(DBConnectionManager* manager, const ReplBody* record);
int ReplBody_Get(DBConnectionManager* manager, int rno, ReplBody* record);
int ReplBody_Update(DBConnectionManager* manager, const ReplBody* record);
int ReplBody_Delete(DBConnectionManager* manager, int rno);
int ReplBody_Exists(DBConnectionManager* manager, int rno);
int ReplBody_GetAll(DBConnectionManager* manager, ReplBody* records, int max_count, int* actual_count);

#endif /* REPL_BODY_ORM_H */