#ifndef EVENT_1112_ONTIME_REWARD_USER_ORM_H
#define EVENT_1112_ONTIME_REWARD_USER_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int recv_no;
} Event1112OntimeRewardUser;

/* CRUD Operations */
int Event1112OntimeRewardUser_Add(DBConnectionManager* manager, const Event1112OntimeRewardUser* record);
int Event1112OntimeRewardUser_GetAll(DBConnectionManager* manager, Event1112OntimeRewardUser* records, int max_count, int* actual_count);

#endif /* EVENT_1112_ONTIME_REWARD_USER_ORM_H */