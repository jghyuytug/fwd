#ifndef OTHER_BBSINFO2_ORM_H
#define OTHER_BBSINFO2_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int kn_id;
    char url_go[21];
    char url_to[21];
    int hits;
    int reg_date;
    char subject[101];
    char ref_url[101];
    char content[8192];
} OtherBbsinfo2;

/* CRUD Operations */
int OtherBbsinfo2_Add(DBConnectionManager* manager, const OtherBbsinfo2* record);
int OtherBbsinfo2_Get(DBConnectionManager* manager, const char* url_go, const char* url_to, OtherBbsinfo2* record);
int OtherBbsinfo2_Update(DBConnectionManager* manager, const OtherBbsinfo2* record);
int OtherBbsinfo2_Delete(DBConnectionManager* manager, const char* url_go, const char* url_to);
int OtherBbsinfo2_Exists(DBConnectionManager* manager, const char* url_go, const char* url_to);
int OtherBbsinfo2_GetAll(DBConnectionManager* manager, OtherBbsinfo2* records, int max_count, int* actual_count);

#endif /* OTHER_BBSINFO2_ORM_H */