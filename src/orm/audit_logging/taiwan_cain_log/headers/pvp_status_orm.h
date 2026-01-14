#ifndef PVP_STATUS_ORM_H
#define PVP_STATUS_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_date[11];
    signed char room_number;
    signed char play_type;
    signed char no_participants;
    int occ_count;
} PvpStatus;

/* CRUD Operations */
int PvpStatus_Add(DBConnectionManager* manager, const PvpStatus* record);
int PvpStatus_Get(DBConnectionManager* manager, const char* occ_date, signed char room_number, signed char play_type, PvpStatus* record);
int PvpStatus_Update(DBConnectionManager* manager, const PvpStatus* record);
int PvpStatus_Delete(DBConnectionManager* manager, const char* occ_date, signed char room_number, signed char play_type);
int PvpStatus_Exists(DBConnectionManager* manager, const char* occ_date, signed char room_number, signed char play_type);
int PvpStatus_GetAll(DBConnectionManager* manager, PvpStatus* records, int max_count, int* actual_count);

#endif /* PVP_STATUS_ORM_H */