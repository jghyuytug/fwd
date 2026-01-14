#ifndef RANDOM_OPTION_REF_ORM_H
#define RANDOM_OPTION_REF_ORM_H

#include "db_connection_manager.h"

typedef struct {
    signed char random_option_index;
    signed char random_option_value;
    char random_option_name[256];
} RandomOptionRef;

/* CRUD Operations */
int RandomOptionRef_Add(DBConnectionManager* manager, const RandomOptionRef* record);
int RandomOptionRef_Get(DBConnectionManager* manager, signed char random_option_index, signed char random_option_value, RandomOptionRef* record);
int RandomOptionRef_Update(DBConnectionManager* manager, const RandomOptionRef* record);
int RandomOptionRef_Delete(DBConnectionManager* manager, signed char random_option_index, signed char random_option_value);
int RandomOptionRef_Exists(DBConnectionManager* manager, signed char random_option_index, signed char random_option_value);
int RandomOptionRef_GetAll(DBConnectionManager* manager, RandomOptionRef* records, int max_count, int* actual_count);

#endif /* RANDOM_OPTION_REF_ORM_H */