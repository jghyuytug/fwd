#ifndef EVENT_STAMP_REWARD_ORM_H
#define EVENT_STAMP_REWARD_ORM_H

#include "db_connection_manager.h"

typedef struct {
    short check_step;
    int item_id;
    short item_cnt;
} EventStampReward;

/* CRUD Operations */
int EventStampReward_Add(DBConnectionManager* manager, const EventStampReward* record);
int EventStampReward_Get(DBConnectionManager* manager, short check_step, EventStampReward* record);
int EventStampReward_Update(DBConnectionManager* manager, const EventStampReward* record);
int EventStampReward_Delete(DBConnectionManager* manager, short check_step);
int EventStampReward_Exists(DBConnectionManager* manager, short check_step);
int EventStampReward_GetAll(DBConnectionManager* manager, EventStampReward* records, int max_count, int* actual_count);

#endif /* EVENT_STAMP_REWARD_ORM_H */