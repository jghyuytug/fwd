#ifndef GUIDE_TAG_LIST_ORM_H
#define GUIDE_TAG_LIST_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char tag_name[256];
} GuideTagList;

/* CRUD Operations */
int GuideTagList_Add(DBConnectionManager* manager, const GuideTagList* record);
int GuideTagList_Get(DBConnectionManager* manager, int no, GuideTagList* record);
int GuideTagList_Update(DBConnectionManager* manager, const GuideTagList* record);
int GuideTagList_Delete(DBConnectionManager* manager, int no);
int GuideTagList_Exists(DBConnectionManager* manager, int no);
int GuideTagList_GetAll(DBConnectionManager* manager, GuideTagList* records, int max_count, int* actual_count);

#endif /* GUIDE_TAG_LIST_ORM_H */