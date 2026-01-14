#ifndef SERVER_STATE_CONTROL_ORM_H
#define SERVER_STATE_CONTROL_ORM_H

#include "db_connection_manager.h"

typedef struct {
    signed char server_id;
    char title[121];
    signed char flag;
    char link_url[121];
    signed char pop_up;
} ServerStateControl;

/* CRUD Operations */
int ServerStateControl_Add(DBConnectionManager* manager, const ServerStateControl* record);
int ServerStateControl_Get(DBConnectionManager* manager, signed char server_id, ServerStateControl* record);
int ServerStateControl_Update(DBConnectionManager* manager, const ServerStateControl* record);
int ServerStateControl_Delete(DBConnectionManager* manager, signed char server_id);
int ServerStateControl_Exists(DBConnectionManager* manager, signed char server_id);
int ServerStateControl_GetAll(DBConnectionManager* manager, ServerStateControl* records, int max_count, int* actual_count);

#endif /* SERVER_STATE_CONTROL_ORM_H */