#ifndef GEO_ALLOW_ORM_H
#define GEO_ALLOW_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char allow_ip[21];
    char allow_c_code[5];
    char allow_date[20];
} GeoAllow;

/* CRUD Operations */
int GeoAllow_Add(DBConnectionManager* manager, const GeoAllow* record);
int GeoAllow_Get(DBConnectionManager* manager, const char* allow_ip, GeoAllow* record);
int GeoAllow_Update(DBConnectionManager* manager, const GeoAllow* record);
int GeoAllow_Delete(DBConnectionManager* manager, const char* allow_ip);
int GeoAllow_Exists(DBConnectionManager* manager, const char* allow_ip);
int GeoAllow_GetAll(DBConnectionManager* manager, GeoAllow* records, int max_count, int* actual_count);

#endif /* GEO_ALLOW_ORM_H */