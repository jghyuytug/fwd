#ifndef NEWMAGAZINE_CONTENT_ORM_H
#define NEWMAGAZINE_CONTENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int mz_id;
    char content[4096];
} NewmagazineContent;

/* CRUD Operations */
int NewmagazineContent_Add(DBConnectionManager* manager, const NewmagazineContent* record);
int NewmagazineContent_Get(DBConnectionManager* manager, int mz_id, NewmagazineContent* record);
int NewmagazineContent_Update(DBConnectionManager* manager, const NewmagazineContent* record);
int NewmagazineContent_Delete(DBConnectionManager* manager, int mz_id);
int NewmagazineContent_Exists(DBConnectionManager* manager, int mz_id);
int NewmagazineContent_GetAll(DBConnectionManager* manager, NewmagazineContent* records, int max_count, int* actual_count);

#endif /* NEWMAGAZINE_CONTENT_ORM_H */