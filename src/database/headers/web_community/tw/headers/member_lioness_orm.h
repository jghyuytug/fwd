#ifndef MEMBER_LIONESS_ORM_H
#define MEMBER_LIONESS_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char user_id[31];
} MemberLioness;

/* CRUD Operations */
int MemberLioness_Add(DBConnectionManager* manager, const MemberLioness* record);
int MemberLioness_GetAll(DBConnectionManager* manager, MemberLioness* records, int max_count, int* actual_count);

#endif /* MEMBER_LIONESS_ORM_H */