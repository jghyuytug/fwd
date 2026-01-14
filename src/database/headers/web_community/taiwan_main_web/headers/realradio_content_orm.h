#ifndef REALRADIO_CONTENT_ORM_H
#define REALRADIO_CONTENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char content[1024];
    char content_type;
    char ip[16];
    short ring;
    char sms;
} RealradioContent;

/* CRUD Operations */
int RealradioContent_Add(DBConnectionManager* manager, const RealradioContent* record);
int RealradioContent_Get(DBConnectionManager* manager, int no, RealradioContent* record);
int RealradioContent_Update(DBConnectionManager* manager, const RealradioContent* record);
int RealradioContent_Delete(DBConnectionManager* manager, int no);
int RealradioContent_Exists(DBConnectionManager* manager, int no);
int RealradioContent_GetAll(DBConnectionManager* manager, RealradioContent* records, int max_count, int* actual_count);

#endif /* REALRADIO_CONTENT_ORM_H */