#ifndef CHALLENGE_LAG_INDEX_DAILY_ORM_H
#define CHALLENGE_LAG_INDEX_DAILY_ORM_H

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
} ChallengeLagIndexDaily;

/* CRUD Operations */
int ChallengeLagIndexDaily_Add(DBConnectionManager* manager, const ChallengeLagIndexDaily* record);
int ChallengeLagIndexDaily_Get(DBConnectionManager* manager, const char* occ_date, int spec_id, ChallengeLagIndexDaily* record);
int ChallengeLagIndexDaily_Update(DBConnectionManager* manager, const ChallengeLagIndexDaily* record);
int ChallengeLagIndexDaily_Delete(DBConnectionManager* manager, const char* occ_date, int spec_id);
int ChallengeLagIndexDaily_Exists(DBConnectionManager* manager, const char* occ_date, int spec_id);
int ChallengeLagIndexDaily_GetAll(DBConnectionManager* manager, ChallengeLagIndexDaily* records, int max_count, int* actual_count);

#endif /* CHALLENGE_LAG_INDEX_DAILY_ORM_H */