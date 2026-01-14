#ifndef DNF_LEAGUE_MAINPAGE_ORM_H
#define DNF_LEAGUE_MAINPAGE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int mp_id;
    char top_img[101];
    char broadcast_time_img[101];
    char vod_link[256];
    char contents[1024];
    char notice[1024];
} DnfLeagueMainpage;

/* CRUD Operations */
int DnfLeagueMainpage_Add(DBConnectionManager* manager, const DnfLeagueMainpage* record);
int DnfLeagueMainpage_Get(DBConnectionManager* manager, int mp_id, DnfLeagueMainpage* record);
int DnfLeagueMainpage_Update(DBConnectionManager* manager, const DnfLeagueMainpage* record);
int DnfLeagueMainpage_Delete(DBConnectionManager* manager, int mp_id);
int DnfLeagueMainpage_Exists(DBConnectionManager* manager, int mp_id);
int DnfLeagueMainpage_GetAll(DBConnectionManager* manager, DnfLeagueMainpage* records, int max_count, int* actual_count);

#endif /* DNF_LEAGUE_MAINPAGE_ORM_H */