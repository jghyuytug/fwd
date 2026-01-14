#ifndef DISCUSSION_CONTENT_ORM_H
#define DISCUSSION_CONTENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char content[1024];
    char content_type;
    char ip[16];
    short ring;
    char sms;
} DiscussionContent;

/* CRUD Operations */
int DiscussionContent_Add(DBConnectionManager* manager, const DiscussionContent* record);
int DiscussionContent_Get(DBConnectionManager* manager, int no, DiscussionContent* record);
int DiscussionContent_Update(DBConnectionManager* manager, const DiscussionContent* record);
int DiscussionContent_Delete(DBConnectionManager* manager, int no);
int DiscussionContent_Exists(DBConnectionManager* manager, int no);
int DiscussionContent_GetAll(DBConnectionManager* manager, DiscussionContent* records, int max_count, int* actual_count);

#endif /* DISCUSSION_CONTENT_ORM_H */