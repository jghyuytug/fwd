#ifndef KNOWCAFE_MEMBER_POINT_ORM_H
#define KNOWCAFE_MEMBER_POINT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int occ_time;
    int total_point;
    int now_point;
} KnowcafeMemberPoint;

/* CRUD Operations */
int KnowcafeMemberPoint_Add(DBConnectionManager* manager, const KnowcafeMemberPoint* record);
int KnowcafeMemberPoint_Get(DBConnectionManager* manager, int m_id, KnowcafeMemberPoint* record);
int KnowcafeMemberPoint_Update(DBConnectionManager* manager, const KnowcafeMemberPoint* record);
int KnowcafeMemberPoint_Delete(DBConnectionManager* manager, int m_id);
int KnowcafeMemberPoint_Exists(DBConnectionManager* manager, int m_id);
int KnowcafeMemberPoint_GetAll(DBConnectionManager* manager, KnowcafeMemberPoint* records, int max_count, int* actual_count);

#endif /* KNOWCAFE_MEMBER_POINT_ORM_H */