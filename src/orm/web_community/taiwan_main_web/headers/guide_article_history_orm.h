#ifndef GUIDE_ARTICLE_HISTORY_ORM_H
#define GUIDE_ARTICLE_HISTORY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    signed char bbs_cd;
    int bbs_no;
    int occ_time;
    char contents[256];
} GuideArticleHistory;

/* CRUD Operations */
int GuideArticleHistory_Add(DBConnectionManager* manager, const GuideArticleHistory* record);
int GuideArticleHistory_Get(DBConnectionManager* manager, int no, GuideArticleHistory* record);
int GuideArticleHistory_Update(DBConnectionManager* manager, const GuideArticleHistory* record);
int GuideArticleHistory_Delete(DBConnectionManager* manager, int no);
int GuideArticleHistory_Exists(DBConnectionManager* manager, int no);
int GuideArticleHistory_GetAll(DBConnectionManager* manager, GuideArticleHistory* records, int max_count, int* actual_count);

#endif /* GUIDE_ARTICLE_HISTORY_ORM_H */