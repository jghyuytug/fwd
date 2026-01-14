#ifndef MAX_COUNT_V2_ORM_H
#define MAX_COUNT_V2_ORM_H

#include "db_connection_manager.h"

typedef struct {
    signed char server_info;
    int num_occupations_charscreen;
    int num_occupations_seriaroom;
    int num_login_per_min;
    int num_logout_per_min;
    char mc_date[20];
} MaxCountV2;

/* CRUD Operations */
int MaxCountV2_Add(DBConnectionManager* manager, const MaxCountV2* record);
int MaxCountV2_GetAll(DBConnectionManager* manager, MaxCountV2* records, int max_count, int* actual_count);

#endif /* MAX_COUNT_V2_ORM_H */