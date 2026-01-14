#ifndef GEN_BODY_ORM_H
#define GEN_BODY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int gno;
    char body[4096];
} GenBody;

/* CRUD Operations */
int GenBody_Add(DBConnectionManager* manager, const GenBody* record);
int GenBody_Get(DBConnectionManager* manager, int gno, GenBody* record);
int GenBody_Update(DBConnectionManager* manager, const GenBody* record);
int GenBody_Delete(DBConnectionManager* manager, int gno);
int GenBody_Exists(DBConnectionManager* manager, int gno);
int GenBody_GetAll(DBConnectionManager* manager, GenBody* records, int max_count, int* actual_count);

#endif /* GEN_BODY_ORM_H */