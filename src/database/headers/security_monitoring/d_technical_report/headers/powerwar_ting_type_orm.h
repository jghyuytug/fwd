#ifndef POWERWAR_TING_TYPE_ORM_H
#define POWERWAR_TING_TYPE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_time[20];
    signed char server_id;
    signed char ting_type;
    int ting_cnt;
} PowerwarTingType;

/* CRUD Operations */
int PowerwarTingType_Add(DBConnectionManager* manager, const PowerwarTingType* record);
int PowerwarTingType_Get(DBConnectionManager* manager, const char* occ_time, signed char server_id, signed char ting_type, PowerwarTingType* record);
int PowerwarTingType_Update(DBConnectionManager* manager, const PowerwarTingType* record);
int PowerwarTingType_Delete(DBConnectionManager* manager, const char* occ_time, signed char server_id, signed char ting_type);
int PowerwarTingType_Exists(DBConnectionManager* manager, const char* occ_time, signed char server_id, signed char ting_type);
int PowerwarTingType_GetAll(DBConnectionManager* manager, PowerwarTingType* records, int max_count, int* actual_count);

#endif /* POWERWAR_TING_TYPE_ORM_H */