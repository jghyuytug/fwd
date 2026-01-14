#ifndef LOG_PACKET_DISPATCHER_ERROR_LINE_ORM_H
#define LOG_PACKET_DISPATCHER_ERROR_LINE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_time[20];
    signed char channel_no;
    int error_line;
    int cnt;
} LogPacketDispatcherErrorLine;

/* CRUD Operations */
int LogPacketDispatcherErrorLine_Add(DBConnectionManager* manager, const LogPacketDispatcherErrorLine* record);
int LogPacketDispatcherErrorLine_Get(DBConnectionManager* manager, const char* occ_time, signed char channel_no, int error_line, LogPacketDispatcherErrorLine* record);
int LogPacketDispatcherErrorLine_Update(DBConnectionManager* manager, const LogPacketDispatcherErrorLine* record);
int LogPacketDispatcherErrorLine_Delete(DBConnectionManager* manager, const char* occ_time, signed char channel_no, int error_line);
int LogPacketDispatcherErrorLine_Exists(DBConnectionManager* manager, const char* occ_time, signed char channel_no, int error_line);
int LogPacketDispatcherErrorLine_GetAll(DBConnectionManager* manager, LogPacketDispatcherErrorLine* records, int max_count, int* actual_count);

#endif /* LOG_PACKET_DISPATCHER_ERROR_LINE_ORM_H */