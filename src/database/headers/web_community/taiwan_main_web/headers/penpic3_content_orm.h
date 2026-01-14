#ifndef PENPIC3_CONTENT_ORM_H
#define PENPIC3_CONTENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char content[1024];
    char content_type;
    char ip[16];
    short ring;
    char sms;
} Penpic3Content;

/* CRUD Operations */
int Penpic3Content_Add(DBConnectionManager* manager, const Penpic3Content* record);
int Penpic3Content_Get(DBConnectionManager* manager, int no, Penpic3Content* record);
int Penpic3Content_Update(DBConnectionManager* manager, const Penpic3Content* record);
int Penpic3Content_Delete(DBConnectionManager* manager, int no);
int Penpic3Content_Exists(DBConnectionManager* manager, int no);
int Penpic3Content_GetAll(DBConnectionManager* manager, Penpic3Content* records, int max_count, int* actual_count);

#endif /* PENPIC3_CONTENT_ORM_H */