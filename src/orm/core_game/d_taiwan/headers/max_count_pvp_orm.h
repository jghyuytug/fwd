#ifndef MAX_COUNT_PVP_ORM_H
#define MAX_COUNT_PVP_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned char server_info;
    unsigned int mc_max;
    char mc_date[20];
} MaxCountPvp;

/* CRUD Operations */
int MaxCountPvp_Add(DBConnectionManager* manager, const MaxCountPvp* record);
int MaxCountPvp_GetByServerInfo(DBConnectionManager* manager, unsigned char server_info,
                                 MaxCountPvp* records, int max_count, int* actual_count);
int MaxCountPvp_UpdateByServerInfo(DBConnectionManager* manager, unsigned char server_info,
                                    const MaxCountPvp* record);
int MaxCountPvp_DeleteByServerInfo(DBConnectionManager* manager, unsigned char server_info);

/* Business Operations */
int MaxCountPvp_GetAll(DBConnectionManager* manager, MaxCountPvp* records,
                        int max_count, int* actual_count);
int MaxCountPvp_GetByDateRange(DBConnectionManager* manager, const char* start_date,
                                const char* end_date, MaxCountPvp* records,
                                int max_count, int* actual_count);
int MaxCountPvp_GetLatest(DBConnectionManager* manager, MaxCountPvp* records,
                           int max_count, int* actual_count);

/* Utility Functions */
void MaxCountPvp_PrintInfo(const MaxCountPvp* record);

#endif
