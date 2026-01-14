#ifndef BBS_SEARCH_ORM_H
#define BBS_SEARCH_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int bbs_class;
    int f_key;
    int s_key;
    char subject[51];
    char body[256];
    int reg_date;
} BbsSearch;

/* CRUD Operations */
int BbsSearch_Add(DBConnectionManager* manager, const BbsSearch* record);
int BbsSearch_GetAll(DBConnectionManager* manager, BbsSearch* records, int max_count, int* actual_count);

#endif /* BBS_SEARCH_ORM_H */