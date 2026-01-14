#ifndef GEO_ALLOW_COUNTRY_ORM_H
#define GEO_ALLOW_COUNTRY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    signed char server_group;
    char country_code[11];
    char reg_date[20];
} GeoAllowCountry;

/* CRUD Operations */
int GeoAllowCountry_Add(DBConnectionManager* manager, const GeoAllowCountry* record);
int GeoAllowCountry_Get(DBConnectionManager* manager, const char* country_code, signed char server_group, GeoAllowCountry* record);
int GeoAllowCountry_Update(DBConnectionManager* manager, const GeoAllowCountry* record);
int GeoAllowCountry_Delete(DBConnectionManager* manager, const char* country_code, signed char server_group);
int GeoAllowCountry_Exists(DBConnectionManager* manager, const char* country_code, signed char server_group);
int GeoAllowCountry_GetAll(DBConnectionManager* manager, GeoAllowCountry* records, int max_count, int* actual_count);

#endif /* GEO_ALLOW_COUNTRY_ORM_H */