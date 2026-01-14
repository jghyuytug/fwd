#ifndef COMMUNITY_DISCUSSION_FILE_ORM_H
#define COMMUNITY_DISCUSSION_FILE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int content_no;
    char file_url[129];
} CommunityDiscussionFile;

/* CRUD Operations */
int CommunityDiscussionFile_Add(DBConnectionManager* manager, const CommunityDiscussionFile* record);
int CommunityDiscussionFile_Get(DBConnectionManager* manager, int no, CommunityDiscussionFile* record);
int CommunityDiscussionFile_Update(DBConnectionManager* manager, const CommunityDiscussionFile* record);
int CommunityDiscussionFile_Delete(DBConnectionManager* manager, int no);
int CommunityDiscussionFile_Exists(DBConnectionManager* manager, int no);
int CommunityDiscussionFile_GetAll(DBConnectionManager* manager, CommunityDiscussionFile* records, int max_count, int* actual_count);

#endif /* COMMUNITY_DISCUSSION_FILE_ORM_H */