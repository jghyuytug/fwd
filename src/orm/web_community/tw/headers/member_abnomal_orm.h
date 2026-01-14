#ifndef MEMBER_ABNOMAL_ORM_H
#define MEMBER_ABNOMAL_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char user_id[13];
    short overlab_count;
} MemberAbnomal;

/* CRUD Operations */
int MemberAbnomal_Add(DBConnectionManager* manager, const MemberAbnomal* record);
int MemberAbnomal_Get(DBConnectionManager* manager, const char* user_id, MemberAbnomal* record);
int MemberAbnomal_Update(DBConnectionManager* manager, const MemberAbnomal* record);
int MemberAbnomal_Delete(DBConnectionManager* manager, const char* user_id);
int MemberAbnomal_Exists(DBConnectionManager* manager, const char* user_id);
int MemberAbnomal_GetAll(DBConnectionManager* manager, MemberAbnomal* records, int max_count, int* actual_count);

#endif /* MEMBER_ABNOMAL_ORM_H */