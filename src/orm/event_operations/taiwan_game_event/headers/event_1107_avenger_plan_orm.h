#ifndef EVENT_1107_AVENGER_PLAN_ORM_H
#define EVENT_1107_AVENGER_PLAN_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int charac_no;
    char occ_date[20];
} Event1107AvengerPlan;

/* CRUD Operations */
int Event1107AvengerPlan_Add(DBConnectionManager* manager, const Event1107AvengerPlan* record);
int Event1107AvengerPlan_Get(DBConnectionManager* manager, int m_id, Event1107AvengerPlan* record);
int Event1107AvengerPlan_Update(DBConnectionManager* manager, const Event1107AvengerPlan* record);
int Event1107AvengerPlan_Delete(DBConnectionManager* manager, int m_id);
int Event1107AvengerPlan_Exists(DBConnectionManager* manager, int m_id);
int Event1107AvengerPlan_GetAll(DBConnectionManager* manager, Event1107AvengerPlan* records, int max_count, int* actual_count);

#endif /* EVENT_1107_AVENGER_PLAN_ORM_H */