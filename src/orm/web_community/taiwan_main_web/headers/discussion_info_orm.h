#ifndef DISCUSSION_INFO_ORM_H
#define DISCUSSION_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int content_no;
    short id;
    char name[101];
    int cnt;
} DiscussionInfo;

/* CRUD Operations */
int DiscussionInfo_Add(DBConnectionManager* manager, const DiscussionInfo* record);
int DiscussionInfo_Get(DBConnectionManager* manager, int content_no, short id, DiscussionInfo* record);
int DiscussionInfo_Update(DBConnectionManager* manager, const DiscussionInfo* record);
int DiscussionInfo_Delete(DBConnectionManager* manager, int content_no, short id);
int DiscussionInfo_Exists(DBConnectionManager* manager, int content_no, short id);
int DiscussionInfo_GetAll(DBConnectionManager* manager, DiscussionInfo* records, int max_count, int* actual_count);

#endif /* DISCUSSION_INFO_ORM_H */