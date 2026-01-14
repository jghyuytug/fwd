#ifndef EVENT_1207_GROWTHWEAPON_ORM_H
#define EVENT_1207_GROWTHWEAPON_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    signed char server_id;
    int charac_no;
    int growthweapon_id;
    signed char timepiece;
    char occ_date[20];
    int infinityweapon_id;
    char reward_occ_date[20];
} Event1207Growthweapon;

/* CRUD Operations */
int Event1207Growthweapon_Add(DBConnectionManager* manager, const Event1207Growthweapon* record);
int Event1207Growthweapon_Get(DBConnectionManager* manager, int m_id, Event1207Growthweapon* record);
int Event1207Growthweapon_Update(DBConnectionManager* manager, const Event1207Growthweapon* record);
int Event1207Growthweapon_Delete(DBConnectionManager* manager, int m_id);
int Event1207Growthweapon_Exists(DBConnectionManager* manager, int m_id);
int Event1207Growthweapon_GetAll(DBConnectionManager* manager, Event1207Growthweapon* records, int max_count, int* actual_count);

#endif /* EVENT_1207_GROWTHWEAPON_ORM_H */