#ifndef WARAREA_LAG_INDEX_DAILY_ORM_H
#define WARAREA_LAG_INDEX_DAILY_ORM_H

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
} WarareaLagIndexDaily;

/* CRUD Operations */
int WarareaLagIndexDaily_Add(DBConnectionManager* manager, const WarareaLagIndexDaily* record);
int WarareaLagIndexDaily_Get(DBConnectionManager* manager, const char* occ_date, int spec_id, WarareaLagIndexDaily* record);
int WarareaLagIndexDaily_Update(DBConnectionManager* manager, const WarareaLagIndexDaily* record);
int WarareaLagIndexDaily_Delete(DBConnectionManager* manager, const char* occ_date, int spec_id);
int WarareaLagIndexDaily_Exists(DBConnectionManager* manager, const char* occ_date, int spec_id);
int WarareaLagIndexDaily_GetAll(DBConnectionManager* manager, WarareaLagIndexDaily* records, int max_count, int* actual_count);

#endif /* WARAREA_LAG_INDEX_DAILY_ORM_H */