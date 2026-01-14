#ifndef DNF_LEAGUE_BBS_SEARCH_ORM_H
#define DNF_LEAGUE_BBS_SEARCH_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int bbs_id;
    signed char main_cat;
    signed char sub_cat;
    char class;
    char word[33];
} DnfLeagueBbsSearch;

/* CRUD Operations */
int DnfLeagueBbsSearch_Add(DBConnectionManager* manager, const DnfLeagueBbsSearch* record);
int DnfLeagueBbsSearch_GetAll(DBConnectionManager* manager, DnfLeagueBbsSearch* records, int max_count, int* actual_count);

#endif /* DNF_LEAGUE_BBS_SEARCH_ORM_H */