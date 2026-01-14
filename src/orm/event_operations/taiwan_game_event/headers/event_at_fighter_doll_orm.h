#ifndef EVENT_AT_FIGHTER_DOLL_ORM_H
#define EVENT_AT_FIGHTER_DOLL_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    char occ_date[20];
} EventAtFighterDoll;

/* CRUD Operations */
int EventAtFighterDoll_Add(DBConnectionManager* manager, const EventAtFighterDoll* record);
int EventAtFighterDoll_Get(DBConnectionManager* manager, int m_id, EventAtFighterDoll* record);
int EventAtFighterDoll_Update(DBConnectionManager* manager, const EventAtFighterDoll* record);
int EventAtFighterDoll_Delete(DBConnectionManager* manager, int m_id);
int EventAtFighterDoll_Exists(DBConnectionManager* manager, int m_id);
int EventAtFighterDoll_GetAll(DBConnectionManager* manager, EventAtFighterDoll* records, int max_count, int* actual_count);

#endif /* EVENT_AT_FIGHTER_DOLL_ORM_H */