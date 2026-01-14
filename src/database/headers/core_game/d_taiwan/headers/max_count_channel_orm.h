#ifndef MAX_COUNT_CHANNEL_ORM_H
#define MAX_COUNT_CHANNEL_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned char server_info;
    char gc_channeltype[26];
    unsigned int mc_max;
    char mc_date[20];
} MaxCountChannel;

/* CRUD Operations - using composite key (server_info, gc_channeltype) */
int MaxCountChannel_Add(DBConnectionManager* manager, const MaxCountChannel* record);
int MaxCountChannel_Get(DBConnectionManager* manager, unsigned char server_info,
                        const char* gc_channeltype, MaxCountChannel* record);
int MaxCountChannel_Update(DBConnectionManager* manager, const MaxCountChannel* record);
int MaxCountChannel_Delete(DBConnectionManager* manager, unsigned char server_info,
                           const char* gc_channeltype);
int MaxCountChannel_Exists(DBConnectionManager* manager, unsigned char server_info,
                           const char* gc_channeltype);

/* Business Operations */
int MaxCountChannel_GetAll(DBConnectionManager* manager, MaxCountChannel* records,
                           int max_count, int* actual_count);
int MaxCountChannel_GetByServer(DBConnectionManager* manager, unsigned char server_info,
                                 MaxCountChannel* records, int max_count, int* actual_count);
int MaxCountChannel_GetByChannelType(DBConnectionManager* manager, const char* gc_channeltype,
                                      MaxCountChannel* records, int max_count, int* actual_count);
int MaxCountChannel_GetByDateRange(DBConnectionManager* manager, const char* start_date,
                                    const char* end_date, MaxCountChannel* records,
                                    int max_count, int* actual_count);
int MaxCountChannel_GetMaxRecord(DBConnectionManager* manager, unsigned char server_info,
                                  MaxCountChannel* record);

/* Utility Functions */
void MaxCountChannel_PrintInfo(const MaxCountChannel* record);

#endif
