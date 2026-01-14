#ifndef EVENT_HERO_MISSION_DATA_ORM_H
#define EVENT_HERO_MISSION_DATA_ORM_H

#include "db_connection_manager.h"

typedef struct {
    signed char server_group;
    int charac_no;
    char mission[257];
    char mod_date[20];
} EventHeroMissionData;

/* CRUD Operations */
int EventHeroMissionData_Add(DBConnectionManager* manager, const EventHeroMissionData* record);
int EventHeroMissionData_Get(DBConnectionManager* manager, signed char server_group, int charac_no, EventHeroMissionData* record);
int EventHeroMissionData_Update(DBConnectionManager* manager, const EventHeroMissionData* record);
int EventHeroMissionData_Delete(DBConnectionManager* manager, signed char server_group, int charac_no);
int EventHeroMissionData_Exists(DBConnectionManager* manager, signed char server_group, int charac_no);
int EventHeroMissionData_GetAll(DBConnectionManager* manager, EventHeroMissionData* records, int max_count, int* actual_count);

#endif /* EVENT_HERO_MISSION_DATA_ORM_H */