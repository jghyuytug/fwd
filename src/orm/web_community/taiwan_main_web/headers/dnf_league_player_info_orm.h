#ifndef DNF_LEAGUE_PLAYER_INFO_ORM_H
#define DNF_LEAGUE_PLAYER_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int pi_id;
    signed char league_id;
    signed char sub_cat;
    int bbs_id;
    char title[256];
    int m_id;
    char name[13];
    signed char server_id;
    int charac_no;
    char charac_name[21];
    signed char job;
    signed char grow_type;
    char picture[101];
} DnfLeaguePlayerInfo;

/* CRUD Operations */
int DnfLeaguePlayerInfo_Add(DBConnectionManager* manager, const DnfLeaguePlayerInfo* record);
int DnfLeaguePlayerInfo_Get(DBConnectionManager* manager, int pi_id, DnfLeaguePlayerInfo* record);
int DnfLeaguePlayerInfo_Update(DBConnectionManager* manager, const DnfLeaguePlayerInfo* record);
int DnfLeaguePlayerInfo_Delete(DBConnectionManager* manager, int pi_id);
int DnfLeaguePlayerInfo_Exists(DBConnectionManager* manager, int pi_id);
int DnfLeaguePlayerInfo_GetAll(DBConnectionManager* manager, DnfLeaguePlayerInfo* records, int max_count, int* actual_count);

#endif /* DNF_LEAGUE_PLAYER_INFO_ORM_H */