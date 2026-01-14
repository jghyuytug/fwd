#ifndef GUIDE_BBS_SEARCH_ORM_H
#define GUIDE_BBS_SEARCH_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int bbs_no;
    char search_text[256];
} GuideBbsSearch;

/* CRUD Operations */
int GuideBbsSearch_Add(DBConnectionManager* manager, const GuideBbsSearch* record);
int GuideBbsSearch_Get(DBConnectionManager* manager, int no, GuideBbsSearch* record);
int GuideBbsSearch_Update(DBConnectionManager* manager, const GuideBbsSearch* record);
int GuideBbsSearch_Delete(DBConnectionManager* manager, int no);
int GuideBbsSearch_Exists(DBConnectionManager* manager, int no);
int GuideBbsSearch_GetAll(DBConnectionManager* manager, GuideBbsSearch* records, int max_count, int* actual_count);

#endif /* GUIDE_BBS_SEARCH_ORM_H */