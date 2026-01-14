#ifndef DISCUSSION_MANAGER_ORM_H
#define DISCUSSION_MANAGER_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int content_no;
    int reg_date;
    char content_name[256];
    char content[1024];
    char positive_cmt[101];
    char negative_cmt[101];
} DiscussionManager;

/* CRUD Operations */
int DiscussionManager_Add(DBConnectionManager* manager, const DiscussionManager* record);
int DiscussionManager_Get(DBConnectionManager* manager, int no, DiscussionManager* record);
int DiscussionManager_Update(DBConnectionManager* manager, const DiscussionManager* record);
int DiscussionManager_Delete(DBConnectionManager* manager, int no);
int DiscussionManager_Exists(DBConnectionManager* manager, int no);
int DiscussionManager_GetAll(DBConnectionManager* manager, DiscussionManager* records, int max_count, int* actual_count);

#endif /* DISCUSSION_MANAGER_ORM_H */