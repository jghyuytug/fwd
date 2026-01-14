#ifndef POWERCLUB_DISCUSSION_BBS_FILE_ORM_H
#define POWERCLUB_DISCUSSION_BBS_FILE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int content_no;
    char file_url[129];
} PowerclubDiscussionBbsFile;

/* CRUD Operations */
int PowerclubDiscussionBbsFile_Add(DBConnectionManager* manager, const PowerclubDiscussionBbsFile* record);
int PowerclubDiscussionBbsFile_Get(DBConnectionManager* manager, int no, PowerclubDiscussionBbsFile* record);
int PowerclubDiscussionBbsFile_Update(DBConnectionManager* manager, const PowerclubDiscussionBbsFile* record);
int PowerclubDiscussionBbsFile_Delete(DBConnectionManager* manager, int no);
int PowerclubDiscussionBbsFile_Exists(DBConnectionManager* manager, int no);
int PowerclubDiscussionBbsFile_GetAll(DBConnectionManager* manager, PowerclubDiscussionBbsFile* records, int max_count, int* actual_count);

#endif /* POWERCLUB_DISCUSSION_BBS_FILE_ORM_H */