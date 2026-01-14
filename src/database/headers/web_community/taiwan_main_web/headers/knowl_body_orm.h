#ifndef KNOWL_BODY_ORM_H
#define KNOWL_BODY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int gno;
    char body[1024];
} KnowlBody;

/* CRUD Operations */
int KnowlBody_Add(DBConnectionManager* manager, const KnowlBody* record);
int KnowlBody_Get(DBConnectionManager* manager, int gno, KnowlBody* record);
int KnowlBody_Update(DBConnectionManager* manager, const KnowlBody* record);
int KnowlBody_Delete(DBConnectionManager* manager, int gno);
int KnowlBody_Exists(DBConnectionManager* manager, int gno);
int KnowlBody_GetAll(DBConnectionManager* manager, KnowlBody* records, int max_count, int* actual_count);

#endif /* KNOWL_BODY_ORM_H */