#ifndef POWERWAR_LOADING_ORM_H
#define POWERWAR_LOADING_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    char occ_time[20];
    short round;
    signed char player;
    short my_loading;
    short other_loading;
    short vs_loading;
} PowerwarLoading;

/* CRUD Operations */
int PowerwarLoading_Add(DBConnectionManager* manager, const PowerwarLoading* record);
int PowerwarLoading_Get(DBConnectionManager* manager, int m_id, const char* occ_time, short round, PowerwarLoading* record);
int PowerwarLoading_Update(DBConnectionManager* manager, const PowerwarLoading* record);
int PowerwarLoading_Delete(DBConnectionManager* manager, int m_id, const char* occ_time, short round);
int PowerwarLoading_Exists(DBConnectionManager* manager, int m_id, const char* occ_time, short round);
int PowerwarLoading_GetAll(DBConnectionManager* manager, PowerwarLoading* records, int max_count, int* actual_count);

#endif /* POWERWAR_LOADING_ORM_H */