#ifndef MAGAZINE_CONTENT_ORM_H
#define MAGAZINE_CONTENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int mz_id;
    char content[4096];
} MagazineContent;

/* CRUD Operations */
int MagazineContent_Add(DBConnectionManager* manager, const MagazineContent* record);
int MagazineContent_Get(DBConnectionManager* manager, int mz_id, MagazineContent* record);
int MagazineContent_Update(DBConnectionManager* manager, const MagazineContent* record);
int MagazineContent_Delete(DBConnectionManager* manager, int mz_id);
int MagazineContent_Exists(DBConnectionManager* manager, int mz_id);
int MagazineContent_GetAll(DBConnectionManager* manager, MagazineContent* records, int max_count, int* actual_count);

#endif /* MAGAZINE_CONTENT_ORM_H */