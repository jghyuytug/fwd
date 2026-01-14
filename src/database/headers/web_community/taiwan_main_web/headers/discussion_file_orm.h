#ifndef DISCUSSION_FILE_ORM_H
#define DISCUSSION_FILE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int content_no;
    char file_url[129];
} DiscussionFile;

/* CRUD Operations */
int DiscussionFile_Add(DBConnectionManager* manager, const DiscussionFile* record);
int DiscussionFile_Get(DBConnectionManager* manager, int no, DiscussionFile* record);
int DiscussionFile_Update(DBConnectionManager* manager, const DiscussionFile* record);
int DiscussionFile_Delete(DBConnectionManager* manager, int no);
int DiscussionFile_Exists(DBConnectionManager* manager, int no);
int DiscussionFile_GetAll(DBConnectionManager* manager, DiscussionFile* records, int max_count, int* actual_count);

#endif /* DISCUSSION_FILE_ORM_H */