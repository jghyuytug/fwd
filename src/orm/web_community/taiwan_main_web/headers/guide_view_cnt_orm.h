#ifndef GUIDE_VIEW_CNT_ORM_H
#define GUIDE_VIEW_CNT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    signed char bbs_cd;
    int bbs_no;
    int occ_time;
    int click_cnt;
    int day_cnt;
} GuideViewCnt;

/* CRUD Operations */
int GuideViewCnt_Add(DBConnectionManager* manager, const GuideViewCnt* record);
int GuideViewCnt_Get(DBConnectionManager* manager, int no, GuideViewCnt* record);
int GuideViewCnt_Update(DBConnectionManager* manager, const GuideViewCnt* record);
int GuideViewCnt_Delete(DBConnectionManager* manager, int no);
int GuideViewCnt_Exists(DBConnectionManager* manager, int no);
int GuideViewCnt_GetAll(DBConnectionManager* manager, GuideViewCnt* records, int max_count, int* actual_count);

#endif /* GUIDE_VIEW_CNT_ORM_H */