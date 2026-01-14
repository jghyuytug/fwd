#ifndef DNF_LEAGUE_BBS_CONTENTS_ORM_H
#define DNF_LEAGUE_BBS_CONTENTS_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int bbs_id;
    int m_id;
    int broadcast_date;
    char thumbnail_img[101];
    char attach_file[101];
    char contents[1024];
} DnfLeagueBbsContents;

/* CRUD Operations */
int DnfLeagueBbsContents_Add(DBConnectionManager* manager, const DnfLeagueBbsContents* record);
int DnfLeagueBbsContents_Get(DBConnectionManager* manager, int bbs_id, DnfLeagueBbsContents* record);
int DnfLeagueBbsContents_Update(DBConnectionManager* manager, const DnfLeagueBbsContents* record);
int DnfLeagueBbsContents_Delete(DBConnectionManager* manager, int bbs_id);
int DnfLeagueBbsContents_Exists(DBConnectionManager* manager, int bbs_id);
int DnfLeagueBbsContents_GetAll(DBConnectionManager* manager, DnfLeagueBbsContents* records, int max_count, int* actual_count);

#endif /* DNF_LEAGUE_BBS_CONTENTS_ORM_H */