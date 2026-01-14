#ifndef POWERCLUB_DISCUSSION_FILE_ORM_H
#define POWERCLUB_DISCUSSION_FILE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int content_no;
    char file_url[129];
} PowerclubDiscussionFile;

/* CRUD Operations */
int PowerclubDiscussionFile_Add(DBConnectionManager* manager, const PowerclubDiscussionFile* record);
int PowerclubDiscussionFile_Get(DBConnectionManager* manager, int no, PowerclubDiscussionFile* record);
int PowerclubDiscussionFile_Update(DBConnectionManager* manager, const PowerclubDiscussionFile* record);
int PowerclubDiscussionFile_Delete(DBConnectionManager* manager, int no);
int PowerclubDiscussionFile_Exists(DBConnectionManager* manager, int no);
int PowerclubDiscussionFile_GetAll(DBConnectionManager* manager, PowerclubDiscussionFile* records, int max_count, int* actual_count);

#endif /* POWERCLUB_DISCUSSION_FILE_ORM_H */