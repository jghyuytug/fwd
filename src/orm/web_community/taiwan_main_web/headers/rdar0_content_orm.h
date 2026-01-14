#ifndef RDAR0_CONTENT_ORM_H
#define RDAR0_CONTENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char content[1024];
    char content_type;
    char ip[16];
    short ring;
    char sms;
} Rdar0Content;

/* CRUD Operations */
int Rdar0Content_Add(DBConnectionManager* manager, const Rdar0Content* record);
int Rdar0Content_Get(DBConnectionManager* manager, int no, Rdar0Content* record);
int Rdar0Content_Update(DBConnectionManager* manager, const Rdar0Content* record);
int Rdar0Content_Delete(DBConnectionManager* manager, int no);
int Rdar0Content_Exists(DBConnectionManager* manager, int no);
int Rdar0Content_GetAll(DBConnectionManager* manager, Rdar0Content* records, int max_count, int* actual_count);

#endif /* RDAR0_CONTENT_ORM_H */