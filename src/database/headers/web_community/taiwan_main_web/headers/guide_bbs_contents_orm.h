#ifndef GUIDE_BBS_CONTENTS_ORM_H
#define GUIDE_BBS_CONTENTS_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int bbs_no;
    char contents[1024];
} GuideBbsContents;

/* CRUD Operations */
int GuideBbsContents_Add(DBConnectionManager* manager, const GuideBbsContents* record);
int GuideBbsContents_Get(DBConnectionManager* manager, int no, GuideBbsContents* record);
int GuideBbsContents_Update(DBConnectionManager* manager, const GuideBbsContents* record);
int GuideBbsContents_Delete(DBConnectionManager* manager, int no);
int GuideBbsContents_Exists(DBConnectionManager* manager, int no);
int GuideBbsContents_GetAll(DBConnectionManager* manager, GuideBbsContents* records, int max_count, int* actual_count);

#endif /* GUIDE_BBS_CONTENTS_ORM_H */