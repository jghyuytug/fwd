#ifndef OTHER_BBSINFO_ORM_H
#define OTHER_BBSINFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int kn_id;
    char url_go[21];
    char url_to[21];
    int hits;
    int reg_date;
    char subject[101];
    char ref_url[101];
    char content[1024];
} OtherBbsinfo;

/* CRUD Operations */
int OtherBbsinfo_Add(DBConnectionManager* manager, const OtherBbsinfo* record);
int OtherBbsinfo_Get(DBConnectionManager* manager, const char* url_go, const char* url_to, OtherBbsinfo* record);
int OtherBbsinfo_Update(DBConnectionManager* manager, const OtherBbsinfo* record);
int OtherBbsinfo_Delete(DBConnectionManager* manager, const char* url_go, const char* url_to);
int OtherBbsinfo_Exists(DBConnectionManager* manager, const char* url_go, const char* url_to);
int OtherBbsinfo_GetAll(DBConnectionManager* manager, OtherBbsinfo* records, int max_count, int* actual_count);

#endif /* OTHER_BBSINFO_ORM_H */