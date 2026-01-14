#ifndef WIKI_SYNC_ORM_H
#define WIKI_SYNC_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int gno;
    int b2t_time;
    int t2r_time;
    int r2s_time;
    int b2t_admin;
    int t2r_admin;
    int r2s_admin;
    int state;
} WikiSync;

/* CRUD Operations */
int WikiSync_Add(DBConnectionManager* manager, const WikiSync* record);
int WikiSync_GetAll(DBConnectionManager* manager, WikiSync* records, int max_count, int* actual_count);

#endif /* WIKI_SYNC_ORM_H */