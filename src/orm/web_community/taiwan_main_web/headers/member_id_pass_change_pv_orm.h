#ifndef MEMBER_ID_PASS_CHANGE_PV_ORM_H
#define MEMBER_ID_PASS_CHANGE_PV_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int id;
    char occ_date[11];
    short type;
    int count;
} MemberIdPassChangePv;

/* CRUD Operations */
int MemberIdPassChangePv_Add(DBConnectionManager* manager, const MemberIdPassChangePv* record);
int MemberIdPassChangePv_Get(DBConnectionManager* manager, int id, MemberIdPassChangePv* record);
int MemberIdPassChangePv_Update(DBConnectionManager* manager, const MemberIdPassChangePv* record);
int MemberIdPassChangePv_Delete(DBConnectionManager* manager, int id);
int MemberIdPassChangePv_Exists(DBConnectionManager* manager, int id);
int MemberIdPassChangePv_GetAll(DBConnectionManager* manager, MemberIdPassChangePv* records, int max_count, int* actual_count);

#endif /* MEMBER_ID_PASS_CHANGE_PV_ORM_H */