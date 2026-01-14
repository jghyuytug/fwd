#ifndef EVENT_VISIT_ROOM_INFO_ORM_H
#define EVENT_VISIT_ROOM_INFO_ORM_H

#include "db_connection_manager.h"

#define MAX_VISIT_CHARAC_BLOB_SIZE 256

typedef struct {
    unsigned int charac_no;
    unsigned char visit_cnt;
    unsigned char visit_charac_no[MAX_VISIT_CHARAC_BLOB_SIZE];
    unsigned int visit_charac_no_len;
    char update_time[20];
} EventVisitRoomInfo;

/* CRUD Operations */
int EventVisitRoomInfo_Add(DBConnectionManager* manager, const EventVisitRoomInfo* info);
int EventVisitRoomInfo_Get(DBConnectionManager* manager, unsigned int charac_no,
                           EventVisitRoomInfo* info);
int EventVisitRoomInfo_Update(DBConnectionManager* manager, const EventVisitRoomInfo* info);
int EventVisitRoomInfo_Delete(DBConnectionManager* manager, unsigned int charac_no);
int EventVisitRoomInfo_Exists(DBConnectionManager* manager, unsigned int charac_no);

/* Business Operations */
int EventVisitRoomInfo_GetAll(DBConnectionManager* manager, EventVisitRoomInfo* infos,
                              int max_count, int* actual_count);
int EventVisitRoomInfo_GetByVisitCount(DBConnectionManager* manager, unsigned char min_cnt,
                                        EventVisitRoomInfo* infos, int max_count, int* actual_count);
int EventVisitRoomInfo_IncrementVisitCount(DBConnectionManager* manager, unsigned int charac_no);
int EventVisitRoomInfo_GetRecentUpdates(DBConnectionManager* manager, const char* since_time,
                                         EventVisitRoomInfo* infos, int max_count, int* actual_count);

/* Utility Functions */
void EventVisitRoomInfo_PrintInfo(const EventVisitRoomInfo* info);

#endif
