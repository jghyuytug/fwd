#ifndef EVENT_SERVER_MESSAGE_ORM_H
#define EVENT_SERVER_MESSAGE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned char server_info;
    unsigned char channel_no;
    char kind;
    char message_index;
    char charac_name[65];
    char message[129];
    unsigned int update_time;
} EventServerMessage;

/* CRUD Operations */
int EventServerMessage_Add(DBConnectionManager* manager, const EventServerMessage* msg);
int EventServerMessage_Get(DBConnectionManager* manager,
                           unsigned char server_info,
                           unsigned char channel_no,
                           char message_index,
                           EventServerMessage* msg);
int EventServerMessage_Update(DBConnectionManager* manager, const EventServerMessage* msg);
int EventServerMessage_Delete(DBConnectionManager* manager,
                               unsigned char server_info,
                               unsigned char channel_no,
                               char message_index);
int EventServerMessage_Exists(DBConnectionManager* manager,
                               unsigned char server_info,
                               unsigned char channel_no,
                               char message_index);

/* Business Operations */
int EventServerMessage_GetAll(DBConnectionManager* manager,
                               EventServerMessage* messages,
                               int max_count,
                               int* actual_count);
int EventServerMessage_GetByServer(DBConnectionManager* manager,
                                    unsigned char server_info,
                                    EventServerMessage* messages,
                                    int max_count,
                                    int* actual_count);
int EventServerMessage_GetByChannel(DBConnectionManager* manager,
                                     unsigned char server_info,
                                     unsigned char channel_no,
                                     EventServerMessage* messages,
                                     int max_count,
                                     int* actual_count);
int EventServerMessage_DeleteAll(DBConnectionManager* manager);

/* Utility */
void EventServerMessage_PrintInfo(const EventServerMessage* msg);

#endif
