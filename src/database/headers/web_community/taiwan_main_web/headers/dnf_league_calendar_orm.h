#ifndef DNF_LEAGUE_CALENDAR_ORM_H
#define DNF_LEAGUE_CALENDAR_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int cal_id;
    int schedule_time;
    char contents[256];
} DnfLeagueCalendar;

/* CRUD Operations */
int DnfLeagueCalendar_Add(DBConnectionManager* manager, const DnfLeagueCalendar* record);
int DnfLeagueCalendar_Get(DBConnectionManager* manager, int cal_id, DnfLeagueCalendar* record);
int DnfLeagueCalendar_Update(DBConnectionManager* manager, const DnfLeagueCalendar* record);
int DnfLeagueCalendar_Delete(DBConnectionManager* manager, int cal_id);
int DnfLeagueCalendar_Exists(DBConnectionManager* manager, int cal_id);
int DnfLeagueCalendar_GetAll(DBConnectionManager* manager, DnfLeagueCalendar* records, int max_count, int* actual_count);

#endif /* DNF_LEAGUE_CALENDAR_ORM_H */