#ifndef CRASH_DOWN_BY_PACKET_ORM_H
#define CRASH_DOWN_BY_PACKET_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_date[11];
    short packet_index;
    int down_count;
} CrashDownByPacket;

/* CRUD Operations */
int CrashDownByPacket_Add(DBConnectionManager* manager, const CrashDownByPacket* record);
int CrashDownByPacket_Get(DBConnectionManager* manager, const char* occ_date, short packet_index, CrashDownByPacket* record);
int CrashDownByPacket_Update(DBConnectionManager* manager, const CrashDownByPacket* record);
int CrashDownByPacket_Delete(DBConnectionManager* manager, const char* occ_date, short packet_index);
int CrashDownByPacket_Exists(DBConnectionManager* manager, const char* occ_date, short packet_index);
int CrashDownByPacket_GetAll(DBConnectionManager* manager, CrashDownByPacket* records, int max_count, int* actual_count);

#endif /* CRASH_DOWN_BY_PACKET_ORM_H */