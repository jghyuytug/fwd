#ifndef DIRECTX_VERSION_ORM_H
#define DIRECTX_VERSION_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_time[20];
    signed char server_group;
    int ver_etc;
    int ver_8_x;
    int ver_9_0;
    int ver_9_0_a;
    int ver_9_0_b;
    int ver_9_0_c;
    int ver_10_x;
    int ver_11_x;
} DirectxVersion;

/* CRUD Operations */
int DirectxVersion_Add(DBConnectionManager* manager, const DirectxVersion* record);
int DirectxVersion_Get(DBConnectionManager* manager, const char* occ_time, signed char server_group, DirectxVersion* record);
int DirectxVersion_Update(DBConnectionManager* manager, const DirectxVersion* record);
int DirectxVersion_Delete(DBConnectionManager* manager, const char* occ_time, signed char server_group);
int DirectxVersion_Exists(DBConnectionManager* manager, const char* occ_time, signed char server_group);
int DirectxVersion_GetAll(DBConnectionManager* manager, DirectxVersion* records, int max_count, int* actual_count);

#endif /* DIRECTX_VERSION_ORM_H */