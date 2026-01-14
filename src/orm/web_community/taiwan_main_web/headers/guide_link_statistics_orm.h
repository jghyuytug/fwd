#ifndef GUIDE_LINK_STATISTICS_ORM_H
#define GUIDE_LINK_STATISTICS_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    signed char bbs_cd;
    int bbs_no;
    int link_bbs_no;
    int view_cnt;
} GuideLinkStatistics;

/* CRUD Operations */
int GuideLinkStatistics_Add(DBConnectionManager* manager, const GuideLinkStatistics* record);
int GuideLinkStatistics_Get(DBConnectionManager* manager, int no, GuideLinkStatistics* record);
int GuideLinkStatistics_Update(DBConnectionManager* manager, const GuideLinkStatistics* record);
int GuideLinkStatistics_Delete(DBConnectionManager* manager, int no);
int GuideLinkStatistics_Exists(DBConnectionManager* manager, int no);
int GuideLinkStatistics_GetAll(DBConnectionManager* manager, GuideLinkStatistics* records, int max_count, int* actual_count);

#endif /* GUIDE_LINK_STATISTICS_ORM_H */