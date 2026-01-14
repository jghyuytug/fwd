#ifndef GEO_REJECT_ORM_H
#define GEO_REJECT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char rej_ip[21];
    char rej_c_code[5];
    int rej_ip_count;
    char rej_last_date[20];
    char rej_chk[2];
    char rej_src;
} GeoReject;

/* CRUD Operations */
int GeoReject_Add(DBConnectionManager* manager, const GeoReject* record);
int GeoReject_Get(DBConnectionManager* manager, const char* rej_ip, GeoReject* record);
int GeoReject_Update(DBConnectionManager* manager, const GeoReject* record);
int GeoReject_Delete(DBConnectionManager* manager, const char* rej_ip);
int GeoReject_Exists(DBConnectionManager* manager, const char* rej_ip);
int GeoReject_GetAll(DBConnectionManager* manager, GeoReject* records, int max_count, int* actual_count);

#endif /* GEO_REJECT_ORM_H */