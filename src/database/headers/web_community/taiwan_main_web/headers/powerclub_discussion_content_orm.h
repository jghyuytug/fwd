#ifndef POWERCLUB_DISCUSSION_CONTENT_ORM_H
#define POWERCLUB_DISCUSSION_CONTENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char content[1024];
    char content_type;
    char ip[16];
    short ring;
    char sms;
} PowerclubDiscussionContent;

/* CRUD Operations */
int PowerclubDiscussionContent_Add(DBConnectionManager* manager, const PowerclubDiscussionContent* record);
int PowerclubDiscussionContent_Get(DBConnectionManager* manager, int no, PowerclubDiscussionContent* record);
int PowerclubDiscussionContent_Update(DBConnectionManager* manager, const PowerclubDiscussionContent* record);
int PowerclubDiscussionContent_Delete(DBConnectionManager* manager, int no);
int PowerclubDiscussionContent_Exists(DBConnectionManager* manager, int no);
int PowerclubDiscussionContent_GetAll(DBConnectionManager* manager, PowerclubDiscussionContent* records, int max_count, int* actual_count);

#endif /* POWERCLUB_DISCUSSION_CONTENT_ORM_H */