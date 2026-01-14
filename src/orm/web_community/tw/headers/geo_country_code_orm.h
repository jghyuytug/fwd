#ifndef GEO_COUNTRY_CODE_ORM_H
#define GEO_COUNTRY_CODE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int code_no;
    char country_code_a2[11];
    char country_code_a3[11];
    char country[256];
} GeoCountryCode;

/* CRUD Operations */
int GeoCountryCode_Add(DBConnectionManager* manager, const GeoCountryCode* record);
int GeoCountryCode_Get(DBConnectionManager* manager, int code_no, GeoCountryCode* record);
int GeoCountryCode_Update(DBConnectionManager* manager, const GeoCountryCode* record);
int GeoCountryCode_Delete(DBConnectionManager* manager, int code_no);
int GeoCountryCode_Exists(DBConnectionManager* manager, int code_no);
int GeoCountryCode_GetAll(DBConnectionManager* manager, GeoCountryCode* records, int max_count, int* actual_count);

#endif /* GEO_COUNTRY_CODE_ORM_H */