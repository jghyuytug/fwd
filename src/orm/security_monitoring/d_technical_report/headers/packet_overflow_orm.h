#ifndef PACKET_OVERFLOW_ORM_H
#define PACKET_OVERFLOW_ORM_H

#include "db_connection_manager.h"

typedef struct {
    short packet_type;
    char packet_kind[256];
    int cnt;
} PacketOverflow;

/* CRUD Operations */
int PacketOverflow_Add(DBConnectionManager* manager, const PacketOverflow* record);
int PacketOverflow_Get(DBConnectionManager* manager, short packet_type, const char* packet_kind, PacketOverflow* record);
int PacketOverflow_Update(DBConnectionManager* manager, const PacketOverflow* record);
int PacketOverflow_Delete(DBConnectionManager* manager, short packet_type, const char* packet_kind);
int PacketOverflow_Exists(DBConnectionManager* manager, short packet_type, const char* packet_kind);
int PacketOverflow_GetAll(DBConnectionManager* manager, PacketOverflow* records, int max_count, int* actual_count);

#endif /* PACKET_OVERFLOW_ORM_H */