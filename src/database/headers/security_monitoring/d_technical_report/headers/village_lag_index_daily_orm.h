#ifndef VILLAGE_LAG_INDEX_DAILY_ORM_H
#define VILLAGE_LAG_INDEX_DAILY_ORM_H

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
} VillageLagIndexDaily;

/* CRUD Operations */
int VillageLagIndexDaily_Add(DBConnectionManager* manager, const VillageLagIndexDaily* record);
int VillageLagIndexDaily_Get(DBConnectionManager* manager, const char* occ_date, int spec_id, VillageLagIndexDaily* record);
int VillageLagIndexDaily_Update(DBConnectionManager* manager, const VillageLagIndexDaily* record);
int VillageLagIndexDaily_Delete(DBConnectionManager* manager, const char* occ_date, int spec_id);
int VillageLagIndexDaily_Exists(DBConnectionManager* manager, const char* occ_date, int spec_id);
int VillageLagIndexDaily_GetAll(DBConnectionManager* manager, VillageLagIndexDaily* records, int max_count, int* actual_count);

#endif /* VILLAGE_LAG_INDEX_DAILY_ORM_H */