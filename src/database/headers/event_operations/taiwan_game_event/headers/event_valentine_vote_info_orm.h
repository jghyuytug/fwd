#ifndef EVENT_VALENTINE_VOTE_INFO_ORM_H
#define EVENT_VALENTINE_VOTE_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    signed char server_id;
    signed char vote_A;
    signed char vote_B;
    signed char vote_C;
} EventValentineVoteInfo;

/* CRUD Operations */
int EventValentineVoteInfo_Add(DBConnectionManager* manager, const EventValentineVoteInfo* record);
int EventValentineVoteInfo_Get(DBConnectionManager* manager, int m_id, signed char server_id, EventValentineVoteInfo* record);
int EventValentineVoteInfo_Update(DBConnectionManager* manager, const EventValentineVoteInfo* record);
int EventValentineVoteInfo_Delete(DBConnectionManager* manager, int m_id, signed char server_id);
int EventValentineVoteInfo_Exists(DBConnectionManager* manager, int m_id, signed char server_id);
int EventValentineVoteInfo_GetAll(DBConnectionManager* manager, EventValentineVoteInfo* records, int max_count, int* actual_count);

#endif /* EVENT_VALENTINE_VOTE_INFO_ORM_H */