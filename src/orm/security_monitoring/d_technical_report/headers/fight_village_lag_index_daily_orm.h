#ifndef FIGHT_VILLAGE_LAG_INDEX_DAILY_ORM_H
#define FIGHT_VILLAGE_LAG_INDEX_DAILY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_date[11];
    int spec_id;
    int win_fps;
    int win_fps_cnt;
    int full_fps;
    int full_fps_cnt;
    int full_win_fps;
    int full_win_fps_cnt;
    int full_win_nosync_fps;
    int full_win_nosync_fps_cnt;
} FightVillageLagIndexDaily;

/* CRUD Operations */
int FightVillageLagIndexDaily_Add(DBConnectionManager* manager, const FightVillageLagIndexDaily* record);
int FightVillageLagIndexDaily_Get(DBConnectionManager* manager, const char* occ_date, int spec_id, FightVillageLagIndexDaily* record);
int FightVillageLagIndexDaily_Update(DBConnectionManager* manager, const FightVillageLagIndexDaily* record);
int FightVillageLagIndexDaily_Delete(DBConnectionManager* manager, const char* occ_date, int spec_id);
int FightVillageLagIndexDaily_Exists(DBConnectionManager* manager, const char* occ_date, int spec_id);
int FightVillageLagIndexDaily_GetAll(DBConnectionManager* manager, FightVillageLagIndexDaily* records, int max_count, int* actual_count);

#endif /* FIGHT_VILLAGE_LAG_INDEX_DAILY_ORM_H */