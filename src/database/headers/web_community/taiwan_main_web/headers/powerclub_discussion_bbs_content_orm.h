#ifndef POWERCLUB_DISCUSSION_BBS_CONTENT_ORM_H
#define POWERCLUB_DISCUSSION_BBS_CONTENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char content[1024];
    char content_type;
    char ip[16];
    short ring;
    char sms;
} PowerclubDiscussionBbsContent;

/* CRUD Operations */
int PowerclubDiscussionBbsContent_Add(DBConnectionManager* manager, const PowerclubDiscussionBbsContent* record);
int PowerclubDiscussionBbsContent_Get(DBConnectionManager* manager, int no, PowerclubDiscussionBbsContent* record);
int PowerclubDiscussionBbsContent_Update(DBConnectionManager* manager, const PowerclubDiscussionBbsContent* record);
int PowerclubDiscussionBbsContent_Delete(DBConnectionManager* manager, int no);
int PowerclubDiscussionBbsContent_Exists(DBConnectionManager* manager, int no);
int PowerclubDiscussionBbsContent_GetAll(DBConnectionManager* manager, PowerclubDiscussionBbsContent* records, int max_count, int* actual_count);

#endif /* POWERCLUB_DISCUSSION_BBS_CONTENT_ORM_H */