#ifndef EVENT_REWARD_ITEM_ARAD_ORM_H
#define EVENT_REWARD_ITEM_ARAD_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int event_id;
    int m_id;
    int charac_no;
    int item_id;
    char end_date[20];
    char occ_date[20];
} EventRewardItemArad;

/* CRUD Operations */
int EventRewardItemArad_Add(DBConnectionManager* manager, const EventRewardItemArad* record);
int EventRewardItemArad_Get(DBConnectionManager* manager, int event_id, int m_id, int charac_no, EventRewardItemArad* record);
int EventRewardItemArad_Update(DBConnectionManager* manager, const EventRewardItemArad* record);
int EventRewardItemArad_Delete(DBConnectionManager* manager, int event_id, int m_id, int charac_no);
int EventRewardItemArad_Exists(DBConnectionManager* manager, int event_id, int m_id, int charac_no);
int EventRewardItemArad_GetAll(DBConnectionManager* manager, EventRewardItemArad* records, int max_count, int* actual_count);

#endif /* EVENT_REWARD_ITEM_ARAD_ORM_H */