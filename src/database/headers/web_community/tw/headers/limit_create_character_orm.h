#ifndef LIMIT_CREATE_CHARACTER_ORM_H
#define LIMIT_CREATE_CHARACTER_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int count;
    char last_access_time[20];
} LimitCreateCharacter;

/* CRUD Operations */
int LimitCreateCharacter_Add(DBConnectionManager* manager, const LimitCreateCharacter* record);
int LimitCreateCharacter_Get(DBConnectionManager* manager, int m_id, LimitCreateCharacter* record);
int LimitCreateCharacter_Update(DBConnectionManager* manager, const LimitCreateCharacter* record);
int LimitCreateCharacter_Delete(DBConnectionManager* manager, int m_id);
int LimitCreateCharacter_Exists(DBConnectionManager* manager, int m_id);
int LimitCreateCharacter_GetAll(DBConnectionManager* manager, LimitCreateCharacter* records, int max_count, int* actual_count);

#endif /* LIMIT_CREATE_CHARACTER_ORM_H */