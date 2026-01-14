#ifndef EVENT_VALENTINE_VOTE_HISTORY_ORM_H
#define EVENT_VALENTINE_VOTE_HISTORY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char check_date[20];
    signed char server_id;
    int vote_A;
    int vote_B;
    int vote_C;
} EventValentineVoteHistory;

/* CRUD Operations */
int EventValentineVoteHistory_Add(DBConnectionManager* manager, const EventValentineVoteHistory* record);
int EventValentineVoteHistory_Get(DBConnectionManager* manager, const char* check_date, signed char server_id, EventValentineVoteHistory* record);
int EventValentineVoteHistory_Update(DBConnectionManager* manager, const EventValentineVoteHistory* record);
int EventValentineVoteHistory_Delete(DBConnectionManager* manager, const char* check_date, signed char server_id);
int EventValentineVoteHistory_Exists(DBConnectionManager* manager, const char* check_date, signed char server_id);
int EventValentineVoteHistory_GetAll(DBConnectionManager* manager, EventValentineVoteHistory* records, int max_count, int* actual_count);

#endif /* EVENT_VALENTINE_VOTE_HISTORY_ORM_H */