#ifndef DNF_FAQ_CONTENT_ORM_H
#define DNF_FAQ_CONTENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char contents[1024];
} DnfFaqContent;

/* CRUD Operations */
int DnfFaqContent_Add(DBConnectionManager* manager, const DnfFaqContent* record);
int DnfFaqContent_Get(DBConnectionManager* manager, int no, DnfFaqContent* record);
int DnfFaqContent_Update(DBConnectionManager* manager, const DnfFaqContent* record);
int DnfFaqContent_Delete(DBConnectionManager* manager, int no);
int DnfFaqContent_Exists(DBConnectionManager* manager, int no);
int DnfFaqContent_GetAll(DBConnectionManager* manager, DnfFaqContent* records, int max_count, int* actual_count);

#endif /* DNF_FAQ_CONTENT_ORM_H */