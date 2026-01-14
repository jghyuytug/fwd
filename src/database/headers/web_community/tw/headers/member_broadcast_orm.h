#ifndef MEMBER_BROADCAST_ORM_H
#define MEMBER_BROADCAST_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int event_id;
    int m_id;
    signed char server_id;
    int charac_no;
    char charac_name[21];
    char start_time[20];
    char end_time[20];
} MemberBroadcast;

/* CRUD Operations */
int MemberBroadcast_Add(DBConnectionManager* manager, const MemberBroadcast* record);
int MemberBroadcast_Get(DBConnectionManager* manager, int event_id, int m_id, signed char server_id, int charac_no, const char* start_time, MemberBroadcast* record);
int MemberBroadcast_Update(DBConnectionManager* manager, const MemberBroadcast* record);
int MemberBroadcast_Delete(DBConnectionManager* manager, int event_id, int m_id, signed char server_id, int charac_no, const char* start_time);
int MemberBroadcast_Exists(DBConnectionManager* manager, int event_id, int m_id, signed char server_id, int charac_no, const char* start_time);
int MemberBroadcast_GetAll(DBConnectionManager* manager, MemberBroadcast* records, int max_count, int* actual_count);

#endif /* MEMBER_BROADCAST_ORM_H */