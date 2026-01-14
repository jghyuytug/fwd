#ifndef RDAR6_CONTENT_ORM_H
#define RDAR6_CONTENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char content[1024];
    char content_type;
    char ip[16];
    short ring;
    char sms;
} Rdar6Content;

/* CRUD Operations */
int Rdar6Content_Add(DBConnectionManager* manager, const Rdar6Content* record);
int Rdar6Content_Get(DBConnectionManager* manager, int no, Rdar6Content* record);
int Rdar6Content_Update(DBConnectionManager* manager, const Rdar6Content* record);
int Rdar6Content_Delete(DBConnectionManager* manager, int no);
int Rdar6Content_Exists(DBConnectionManager* manager, int no);
int Rdar6Content_GetAll(DBConnectionManager* manager, Rdar6Content* records, int max_count, int* actual_count);

#endif /* RDAR6_CONTENT_ORM_H */