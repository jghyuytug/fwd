#ifndef NEWMAGAZINE_UCC_CONTENT_ORM_H
#define NEWMAGAZINE_UCC_CONTENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int mz_id;
    char content[4096];
} NewmagazineUccContent;

/* CRUD Operations */
int NewmagazineUccContent_Add(DBConnectionManager* manager, const NewmagazineUccContent* record);
int NewmagazineUccContent_Get(DBConnectionManager* manager, int mz_id, NewmagazineUccContent* record);
int NewmagazineUccContent_Update(DBConnectionManager* manager, const NewmagazineUccContent* record);
int NewmagazineUccContent_Delete(DBConnectionManager* manager, int mz_id);
int NewmagazineUccContent_Exists(DBConnectionManager* manager, int mz_id);
int NewmagazineUccContent_GetAll(DBConnectionManager* manager, NewmagazineUccContent* records, int max_count, int* actual_count);

#endif /* NEWMAGAZINE_UCC_CONTENT_ORM_H */