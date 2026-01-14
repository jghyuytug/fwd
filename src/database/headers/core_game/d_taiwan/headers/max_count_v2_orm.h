#ifndef MAX_COUNT_V2_ORM_H
#define MAX_COUNT_V2_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned char server_info;              /* Server info */
    unsigned int num_occupations_charscreen; /* Occupations in char screen */
    unsigned int num_occupations_seriaroom;  /* Occupations in seria room */
    unsigned int num_login_per_min;         /* Logins per minute */
    unsigned int num_logout_per_min;        /* Logouts per minute */
    char mc_date[20];                       /* Date time (indexed) */
} MaxCountV2;

/* Insert Operations (no primary key) */
int MaxCountV2_Add(DBConnectionManager* manager, const MaxCountV2* record);

/* Query Operations */
int MaxCountV2_GetByServerInfo(DBConnectionManager* manager, unsigned char server_info,
                                MaxCountV2* records, int max_count, int* actual_count);
int MaxCountV2_GetByDateRange(DBConnectionManager* manager, const char* start_date, const char* end_date,
                               MaxCountV2* records, int max_count, int* actual_count);
int MaxCountV2_GetLatest(DBConnectionManager* manager, unsigned char server_info,
                          MaxCountV2* record);
int MaxCountV2_GetAll(DBConnectionManager* manager, MaxCountV2* records,
                       int max_count, int* actual_count);

/* Batch Delete Operations */
int MaxCountV2_DeleteByServerInfo(DBConnectionManager* manager, unsigned char server_info);
int MaxCountV2_DeleteByDateRange(DBConnectionManager* manager, const char* start_date, const char* end_date);

/* Utility Functions */
void MaxCountV2_PrintInfo(const MaxCountV2* record);

#endif
