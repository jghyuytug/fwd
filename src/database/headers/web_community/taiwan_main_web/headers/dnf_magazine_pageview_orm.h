#ifndef DNF_MAGAZINE_PAGEVIEW_ORM_H
#define DNF_MAGAZINE_PAGEVIEW_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int magazine_id;
    int pageview;
} DnfMagazinePageview;

/* CRUD Operations */
int DnfMagazinePageview_Add(DBConnectionManager* manager, const DnfMagazinePageview* record);
int DnfMagazinePageview_Get(DBConnectionManager* manager, int magazine_id, DnfMagazinePageview* record);
int DnfMagazinePageview_Update(DBConnectionManager* manager, const DnfMagazinePageview* record);
int DnfMagazinePageview_Delete(DBConnectionManager* manager, int magazine_id);
int DnfMagazinePageview_Exists(DBConnectionManager* manager, int magazine_id);
int DnfMagazinePageview_GetAll(DBConnectionManager* manager, DnfMagazinePageview* records, int max_count, int* actual_count);

#endif /* DNF_MAGAZINE_PAGEVIEW_ORM_H */