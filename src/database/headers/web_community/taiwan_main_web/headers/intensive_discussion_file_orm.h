#ifndef INTENSIVE_DISCUSSION_FILE_ORM_H
#define INTENSIVE_DISCUSSION_FILE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int content_no;
    char file_url[129];
} IntensiveDiscussionFile;

/* CRUD Operations */
int IntensiveDiscussionFile_Add(DBConnectionManager* manager, const IntensiveDiscussionFile* record);
int IntensiveDiscussionFile_Get(DBConnectionManager* manager, int no, IntensiveDiscussionFile* record);
int IntensiveDiscussionFile_Update(DBConnectionManager* manager, const IntensiveDiscussionFile* record);
int IntensiveDiscussionFile_Delete(DBConnectionManager* manager, int no);
int IntensiveDiscussionFile_Exists(DBConnectionManager* manager, int no);
int IntensiveDiscussionFile_GetAll(DBConnectionManager* manager, IntensiveDiscussionFile* records, int max_count, int* actual_count);

#endif /* INTENSIVE_DISCUSSION_FILE_ORM_H */