#ifndef DNF_AD_STAT_ORM_H
#define DNF_AD_STAT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    short ad_id;
    int open;
    int click;
    int login;
    int logout;
} DnfAdStat;

/* CRUD Operations */
int DnfAdStat_Add(DBConnectionManager* manager, const DnfAdStat* record);
int DnfAdStat_Get(DBConnectionManager* manager, short ad_id, DnfAdStat* record);
int DnfAdStat_Update(DBConnectionManager* manager, const DnfAdStat* record);
int DnfAdStat_Delete(DBConnectionManager* manager, short ad_id);
int DnfAdStat_Exists(DBConnectionManager* manager, short ad_id);
int DnfAdStat_GetAll(DBConnectionManager* manager, DnfAdStat* records, int max_count, int* actual_count);

#endif /* DNF_AD_STAT_ORM_H */