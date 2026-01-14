#ifndef BBS_SEARCH_HD_ORM_H
#define BBS_SEARCH_HD_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int bbs_class;
    int f_key;
    int s_key;
    char subject[51];
    char body[256];
    int reg_date;
} BbsSearchHd;

/* CRUD Operations */
int BbsSearchHd_Add(DBConnectionManager* manager, const BbsSearchHd* record);
int BbsSearchHd_GetAll(DBConnectionManager* manager, BbsSearchHd* records, int max_count, int* actual_count);

#endif /* BBS_SEARCH_HD_ORM_H */