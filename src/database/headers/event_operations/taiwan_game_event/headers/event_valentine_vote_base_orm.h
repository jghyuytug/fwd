#ifndef EVENT_VALENTINE_VOTE_BASE_ORM_H
#define EVENT_VALENTINE_VOTE_BASE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    signed char server_id;
    int stage1;
    int stage2;
    int stage3;
    int stage4;
    int stage5;
    int cur_event;
} EventValentineVoteBase;

/* CRUD Operations */
int EventValentineVoteBase_Add(DBConnectionManager* manager, const EventValentineVoteBase* record);
int EventValentineVoteBase_Get(DBConnectionManager* manager, signed char server_id, EventValentineVoteBase* record);
int EventValentineVoteBase_Update(DBConnectionManager* manager, const EventValentineVoteBase* record);
int EventValentineVoteBase_Delete(DBConnectionManager* manager, signed char server_id);
int EventValentineVoteBase_Exists(DBConnectionManager* manager, signed char server_id);
int EventValentineVoteBase_GetAll(DBConnectionManager* manager, EventValentineVoteBase* records, int max_count, int* actual_count);

#endif /* EVENT_VALENTINE_VOTE_BASE_ORM_H */