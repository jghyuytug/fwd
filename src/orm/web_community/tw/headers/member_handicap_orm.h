#ifndef MEMBER_HANDICAP_ORM_H
#define MEMBER_HANDICAP_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int event_id;
    signed char cap_type;
    signed char server_id;
    int m_id;
    char start_time[20];
    char end_time[20];
    int handicap_value;
} MemberHandicap;

/* CRUD Operations */
int MemberHandicap_Add(DBConnectionManager* manager, const MemberHandicap* record);
int MemberHandicap_Get(DBConnectionManager* manager, int event_id, signed char cap_type, signed char server_id, int m_id, const char* start_time, MemberHandicap* record);
int MemberHandicap_Update(DBConnectionManager* manager, const MemberHandicap* record);
int MemberHandicap_Delete(DBConnectionManager* manager, int event_id, signed char cap_type, signed char server_id, int m_id, const char* start_time);
int MemberHandicap_Exists(DBConnectionManager* manager, int event_id, signed char cap_type, signed char server_id, int m_id, const char* start_time);
int MemberHandicap_GetAll(DBConnectionManager* manager, MemberHandicap* records, int max_count, int* actual_count);

#endif /* MEMBER_HANDICAP_ORM_H */