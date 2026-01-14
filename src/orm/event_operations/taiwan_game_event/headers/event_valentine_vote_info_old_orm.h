#ifndef EVENT_VALENTINE_VOTE_INFO_OLD_ORM_H
#define EVENT_VALENTINE_VOTE_INFO_OLD_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    signed char server_id;
    signed char vote_A;
    signed char vote_B;
    signed char vote_C;
} EventValentineVoteInfoOld;

/* CRUD Operations */
int EventValentineVoteInfoOld_Add(DBConnectionManager* manager, const EventValentineVoteInfoOld* record);
int EventValentineVoteInfoOld_Get(DBConnectionManager* manager, int m_id, signed char server_id, EventValentineVoteInfoOld* record);
int EventValentineVoteInfoOld_Update(DBConnectionManager* manager, const EventValentineVoteInfoOld* record);
int EventValentineVoteInfoOld_Delete(DBConnectionManager* manager, int m_id, signed char server_id);
int EventValentineVoteInfoOld_Exists(DBConnectionManager* manager, int m_id, signed char server_id);
int EventValentineVoteInfoOld_GetAll(DBConnectionManager* manager, EventValentineVoteInfoOld* records, int max_count, int* actual_count);

#endif /* EVENT_VALENTINE_VOTE_INFO_OLD_ORM_H */