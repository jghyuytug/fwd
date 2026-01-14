#ifndef MAX_COUNT_CHANNEL_ORM_H
#define MAX_COUNT_CHANNEL_ORM_H

#include "db_connection_manager.h"

typedef struct {
    signed char server_info;
    char gc_channeltype[26];
    int mc_max;
    char mc_date[20];
} MaxCountChannel;

/* CRUD Operations */
int MaxCountChannel_Add(DBConnectionManager* manager, const MaxCountChannel* record);
int MaxCountChannel_GetAll(DBConnectionManager* manager, MaxCountChannel* records, int max_count, int* actual_count);

#endif /* MAX_COUNT_CHANNEL_ORM_H */