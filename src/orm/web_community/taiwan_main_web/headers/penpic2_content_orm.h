#ifndef PENPIC2_CONTENT_ORM_H
#define PENPIC2_CONTENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char content[1024];
    char content_type;
    char ip[16];
    short ring;
    char sms;
} Penpic2Content;

/* CRUD Operations */
int Penpic2Content_Add(DBConnectionManager* manager, const Penpic2Content* record);
int Penpic2Content_Get(DBConnectionManager* manager, int no, Penpic2Content* record);
int Penpic2Content_Update(DBConnectionManager* manager, const Penpic2Content* record);
int Penpic2Content_Delete(DBConnectionManager* manager, int no);
int Penpic2Content_Exists(DBConnectionManager* manager, int no);
int Penpic2Content_GetAll(DBConnectionManager* manager, Penpic2Content* records, int max_count, int* actual_count);

#endif /* PENPIC2_CONTENT_ORM_H */