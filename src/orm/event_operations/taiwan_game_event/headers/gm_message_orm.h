#ifndef GM_MESSAGE_ORM_H
#define GM_MESSAGE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    signed char event_id;
    signed char server_id;
    signed char msg_order;
    int send_time;
    signed char msg_type;
    char msg[256];
    char send_charac_name[31];
} GmMessage;

/* CRUD Operations */
int GmMessage_Add(DBConnectionManager* manager, const GmMessage* record);
int GmMessage_Get(DBConnectionManager* manager, signed char event_id, signed char server_id, signed char msg_order, int send_time, GmMessage* record);
int GmMessage_Update(DBConnectionManager* manager, const GmMessage* record);
int GmMessage_Delete(DBConnectionManager* manager, signed char event_id, signed char server_id, signed char msg_order, int send_time);
int GmMessage_Exists(DBConnectionManager* manager, signed char event_id, signed char server_id, signed char msg_order, int send_time);
int GmMessage_GetAll(DBConnectionManager* manager, GmMessage* records, int max_count, int* actual_count);

#endif /* GM_MESSAGE_ORM_H */