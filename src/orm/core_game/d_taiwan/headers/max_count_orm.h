#ifndef MAX_COUNT_ORM_H
#define MAX_COUNT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned char server_info;    /* Server info (indexed) */
    unsigned int mc_max;          /* Max count value */
    char mc_date[20];             /* Date time (indexed) */
} MaxCount;

/* Insert Operations (no primary key, so no unique Get/Update/Delete) */
int MaxCount_Add(DBConnectionManager* manager, const MaxCount* record);

/* Query Operations - by indexed fields */
int MaxCount_GetByServerInfo(DBConnectionManager* manager, unsigned char server_info,
                               MaxCount* records, int max_count, int* actual_count);
int MaxCount_GetByDateRange(DBConnectionManager* manager, const char* start_date, const char* end_date,
                             MaxCount* records, int max_count, int* actual_count);
int MaxCount_GetLatest(DBConnectionManager* manager, unsigned char server_info,
                        MaxCount* record);
int MaxCount_GetAll(DBConnectionManager* manager, MaxCount* records,
                     int max_count, int* actual_count);

/* Batch Delete Operations */
int MaxCount_DeleteByServerInfo(DBConnectionManager* manager, unsigned char server_info);
int MaxCount_DeleteByDateRange(DBConnectionManager* manager, const char* start_date, const char* end_date);

/* Utility Functions */
void MaxCount_PrintInfo(const MaxCount* record);

#endif
