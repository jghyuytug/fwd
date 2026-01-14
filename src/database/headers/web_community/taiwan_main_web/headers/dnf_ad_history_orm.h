#ifndef DNF_AD_HISTORY_ORM_H
#define DNF_AD_HISTORY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    short ad_id;
    int occ_time;
    int open;
    int click;
    int login;
    int logout;
} DnfAdHistory;

/* CRUD Operations */
int DnfAdHistory_Add(DBConnectionManager* manager, const DnfAdHistory* record);
int DnfAdHistory_Get(DBConnectionManager* manager, short ad_id, int occ_time, DnfAdHistory* record);
int DnfAdHistory_Update(DBConnectionManager* manager, const DnfAdHistory* record);
int DnfAdHistory_Delete(DBConnectionManager* manager, short ad_id, int occ_time);
int DnfAdHistory_Exists(DBConnectionManager* manager, short ad_id, int occ_time);
int DnfAdHistory_GetAll(DBConnectionManager* manager, DnfAdHistory* records, int max_count, int* actual_count);

#endif /* DNF_AD_HISTORY_ORM_H */