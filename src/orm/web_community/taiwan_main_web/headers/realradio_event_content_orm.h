#ifndef REALRADIO_EVENT_CONTENT_ORM_H
#define REALRADIO_EVENT_CONTENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char content[1024];
    char content_type;
    char ip[16];
    short ring;
    char sms;
} RealradioEventContent;

/* CRUD Operations */
int RealradioEventContent_Add(DBConnectionManager* manager, const RealradioEventContent* record);
int RealradioEventContent_Get(DBConnectionManager* manager, int no, RealradioEventContent* record);
int RealradioEventContent_Update(DBConnectionManager* manager, const RealradioEventContent* record);
int RealradioEventContent_Delete(DBConnectionManager* manager, int no);
int RealradioEventContent_Exists(DBConnectionManager* manager, int no);
int RealradioEventContent_GetAll(DBConnectionManager* manager, RealradioEventContent* records, int max_count, int* actual_count);

#endif /* REALRADIO_EVENT_CONTENT_ORM_H */