#ifndef WIKI_MNG_GUIDE_STATS_ORM_H
#define WIKI_MNG_GUIDE_STATS_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int id;
    char date_enroll[11];
    char url_go[21];
    char url_to[21];
    int hits;
} WikiMngGuideStats;

/* CRUD Operations */
int WikiMngGuideStats_Add(DBConnectionManager* manager, const WikiMngGuideStats* record);
int WikiMngGuideStats_GetAll(DBConnectionManager* manager, WikiMngGuideStats* records, int max_count, int* actual_count);

#endif /* WIKI_MNG_GUIDE_STATS_ORM_H */