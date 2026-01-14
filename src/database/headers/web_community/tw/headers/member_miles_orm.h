#ifndef MEMBER_MILES_ORM_H
#define MEMBER_MILES_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int miles;
    short daily_miles;
} MemberMiles;

/* CRUD Operations */
int MemberMiles_Add(DBConnectionManager* manager, const MemberMiles* record);
int MemberMiles_Get(DBConnectionManager* manager, int m_id, MemberMiles* record);
int MemberMiles_Update(DBConnectionManager* manager, const MemberMiles* record);
int MemberMiles_Delete(DBConnectionManager* manager, int m_id);
int MemberMiles_Exists(DBConnectionManager* manager, int m_id);
int MemberMiles_GetAll(DBConnectionManager* manager, MemberMiles* records, int max_count, int* actual_count);

#endif /* MEMBER_MILES_ORM_H */