#ifndef PENPIC4_CONTENT_ORM_H
#define PENPIC4_CONTENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char content[1024];
    char content_type;
    char ip[16];
    short ring;
    char sms;
} Penpic4Content;

/* CRUD Operations */
int Penpic4Content_Add(DBConnectionManager* manager, const Penpic4Content* record);
int Penpic4Content_Get(DBConnectionManager* manager, int no, Penpic4Content* record);
int Penpic4Content_Update(DBConnectionManager* manager, const Penpic4Content* record);
int Penpic4Content_Delete(DBConnectionManager* manager, int no);
int Penpic4Content_Exists(DBConnectionManager* manager, int no);
int Penpic4Content_GetAll(DBConnectionManager* manager, Penpic4Content* records, int max_count, int* actual_count);

#endif /* PENPIC4_CONTENT_ORM_H */