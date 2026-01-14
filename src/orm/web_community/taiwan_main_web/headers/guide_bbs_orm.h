#ifndef GUIDE_BBS_ORM_H
#define GUIDE_BBS_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char title[256];
    signed char bbs_cd;
    int occ_time;
    char img_src[256];
    char url[256];
    int view_cnt;
    int click_cnt;
    signed char open_flg;
} GuideBbs;

/* CRUD Operations */
int GuideBbs_Add(DBConnectionManager* manager, const GuideBbs* record);
int GuideBbs_Get(DBConnectionManager* manager, int no, GuideBbs* record);
int GuideBbs_Update(DBConnectionManager* manager, const GuideBbs* record);
int GuideBbs_Delete(DBConnectionManager* manager, int no);
int GuideBbs_Exists(DBConnectionManager* manager, int no);
int GuideBbs_GetAll(DBConnectionManager* manager, GuideBbs* records, int max_count, int* actual_count);

#endif /* GUIDE_BBS_ORM_H */