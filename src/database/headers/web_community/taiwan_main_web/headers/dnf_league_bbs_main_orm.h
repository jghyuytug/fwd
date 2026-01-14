#ifndef DNF_LEAGUE_BBS_MAIN_ORM_H
#define DNF_LEAGUE_BBS_MAIN_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int bbs_id;
    signed char league_id;
    signed char main_cat;
    signed char sub_cat;
    signed char open_flag;
    char title[121];
    int reg_time;
    int view;
} DnfLeagueBbsMain;

/* CRUD Operations */
int DnfLeagueBbsMain_Add(DBConnectionManager* manager, const DnfLeagueBbsMain* record);
int DnfLeagueBbsMain_Get(DBConnectionManager* manager, int bbs_id, DnfLeagueBbsMain* record);
int DnfLeagueBbsMain_Update(DBConnectionManager* manager, const DnfLeagueBbsMain* record);
int DnfLeagueBbsMain_Delete(DBConnectionManager* manager, int bbs_id);
int DnfLeagueBbsMain_Exists(DBConnectionManager* manager, int bbs_id);
int DnfLeagueBbsMain_GetAll(DBConnectionManager* manager, DnfLeagueBbsMain* records, int max_count, int* actual_count);

#endif /* DNF_LEAGUE_BBS_MAIN_ORM_H */