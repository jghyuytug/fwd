#ifndef CH_SERVER_DATA_ORM_H
#define CH_SERVER_DATA_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int id;
    signed char server_id;
    signed char is_use;
    short channel_number;
    char channel_name[31];
    signed char channel_kind;
    char kind_name[21];
    float exp_rate;
    float exp_1;
    float exp_2;
    float exp_3;
    float exp_4;
    float exp_5;
    float exp_6;
    float exp_7;
    float exp_8;
    float exp_9;
    float exp_10;
} ChServerData;

/* CRUD Operations */
int ChServerData_Add(DBConnectionManager* manager, const ChServerData* record);
int ChServerData_Get(DBConnectionManager* manager, int id, ChServerData* record);
int ChServerData_Update(DBConnectionManager* manager, const ChServerData* record);
int ChServerData_Delete(DBConnectionManager* manager, int id);
int ChServerData_Exists(DBConnectionManager* manager, int id);
int ChServerData_GetAll(DBConnectionManager* manager, ChServerData* records, int max_count, int* actual_count);

#endif /* CH_SERVER_DATA_ORM_H */