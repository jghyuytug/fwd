#ifndef BBS_DIV_ORM_H
#define BBS_DIV_ORM_H

#include "db_connection_manager.h"

typedef struct {
    signed char bbs_code;
    char bbs_name[31];
    char bbs_kind[21];
} BbsDiv;

/* CRUD Operations */
int BbsDiv_Add(DBConnectionManager* manager, const BbsDiv* record);
int BbsDiv_Get(DBConnectionManager* manager, signed char bbs_code, BbsDiv* record);
int BbsDiv_Update(DBConnectionManager* manager, const BbsDiv* record);
int BbsDiv_Delete(DBConnectionManager* manager, signed char bbs_code);
int BbsDiv_Exists(DBConnectionManager* manager, signed char bbs_code);
int BbsDiv_GetAll(DBConnectionManager* manager, BbsDiv* records, int max_count, int* actual_count);

#endif /* BBS_DIV_ORM_H */