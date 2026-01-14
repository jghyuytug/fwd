#ifndef RE_STAMINA_SHUSIA_ORM_H
#define RE_STAMINA_SHUSIA_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_date[11];
    signed char lev;
    int occ_count;
} ReStaminaShusia;

/* CRUD Operations */
int ReStaminaShusia_Add(DBConnectionManager* manager, const ReStaminaShusia* record);
int ReStaminaShusia_Get(DBConnectionManager* manager, const char* occ_date, signed char lev, ReStaminaShusia* record);
int ReStaminaShusia_Update(DBConnectionManager* manager, const ReStaminaShusia* record);
int ReStaminaShusia_Delete(DBConnectionManager* manager, const char* occ_date, signed char lev);
int ReStaminaShusia_Exists(DBConnectionManager* manager, const char* occ_date, signed char lev);
int ReStaminaShusia_GetAll(DBConnectionManager* manager, ReStaminaShusia* records, int max_count, int* actual_count);

#endif /* RE_STAMINA_SHUSIA_ORM_H */