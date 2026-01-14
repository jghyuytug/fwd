#ifndef GUIDE_BBS_LINK_ORM_H
#define GUIDE_BBS_LINK_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int bbs_no;
    signed char link_kind;
    char title[256];
    char link_url[256];
} GuideBbsLink;

/* CRUD Operations */
int GuideBbsLink_Add(DBConnectionManager* manager, const GuideBbsLink* record);
int GuideBbsLink_Get(DBConnectionManager* manager, int no, GuideBbsLink* record);
int GuideBbsLink_Update(DBConnectionManager* manager, const GuideBbsLink* record);
int GuideBbsLink_Delete(DBConnectionManager* manager, int no);
int GuideBbsLink_Exists(DBConnectionManager* manager, int no);
int GuideBbsLink_GetAll(DBConnectionManager* manager, GuideBbsLink* records, int max_count, int* actual_count);

#endif /* GUIDE_BBS_LINK_ORM_H */