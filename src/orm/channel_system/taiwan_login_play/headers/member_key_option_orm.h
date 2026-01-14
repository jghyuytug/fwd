#ifndef MEMBER_KEY_OPTION_ORM_H
#define MEMBER_KEY_OPTION_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    signed char key_type;
} MemberKeyOption;

/* CRUD Operations */
int MemberKeyOption_Add(DBConnectionManager* manager, const MemberKeyOption* record);
int MemberKeyOption_Get(DBConnectionManager* manager, int m_id, signed char key_type, MemberKeyOption* record);
int MemberKeyOption_Update(DBConnectionManager* manager, const MemberKeyOption* record);
int MemberKeyOption_Delete(DBConnectionManager* manager, int m_id, signed char key_type);
int MemberKeyOption_Exists(DBConnectionManager* manager, int m_id, signed char key_type);
int MemberKeyOption_GetAll(DBConnectionManager* manager, MemberKeyOption* records, int max_count, int* actual_count);

#endif /* MEMBER_KEY_OPTION_ORM_H */