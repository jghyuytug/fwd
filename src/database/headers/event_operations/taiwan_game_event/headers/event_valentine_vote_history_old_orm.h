#ifndef EVENT_VALENTINE_VOTE_HISTORY_OLD_ORM_H
#define EVENT_VALENTINE_VOTE_HISTORY_OLD_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char check_date[20];
    signed char server_id;
    signed char vote_A;
    signed char vote_B;
    signed char vote_C;
} EventValentineVoteHistoryOld;

/* CRUD Operations */
int EventValentineVoteHistoryOld_Add(DBConnectionManager* manager, const EventValentineVoteHistoryOld* record);
int EventValentineVoteHistoryOld_Get(DBConnectionManager* manager, const char* check_date, signed char server_id, EventValentineVoteHistoryOld* record);
int EventValentineVoteHistoryOld_Update(DBConnectionManager* manager, const EventValentineVoteHistoryOld* record);
int EventValentineVoteHistoryOld_Delete(DBConnectionManager* manager, const char* check_date, signed char server_id);
int EventValentineVoteHistoryOld_Exists(DBConnectionManager* manager, const char* check_date, signed char server_id);
int EventValentineVoteHistoryOld_GetAll(DBConnectionManager* manager, EventValentineVoteHistoryOld* records, int max_count, int* actual_count);

#endif /* EVENT_VALENTINE_VOTE_HISTORY_OLD_ORM_H */