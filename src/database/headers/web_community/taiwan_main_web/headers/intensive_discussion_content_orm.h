#ifndef INTENSIVE_DISCUSSION_CONTENT_ORM_H
#define INTENSIVE_DISCUSSION_CONTENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char content[1024];
    char content_type;
    char ip[16];
    short ring;
    char sms;
} IntensiveDiscussionContent;

/* CRUD Operations */
int IntensiveDiscussionContent_Add(DBConnectionManager* manager, const IntensiveDiscussionContent* record);
int IntensiveDiscussionContent_Get(DBConnectionManager* manager, int no, IntensiveDiscussionContent* record);
int IntensiveDiscussionContent_Update(DBConnectionManager* manager, const IntensiveDiscussionContent* record);
int IntensiveDiscussionContent_Delete(DBConnectionManager* manager, int no);
int IntensiveDiscussionContent_Exists(DBConnectionManager* manager, int no);
int IntensiveDiscussionContent_GetAll(DBConnectionManager* manager, IntensiveDiscussionContent* records, int max_count, int* actual_count);

#endif /* INTENSIVE_DISCUSSION_CONTENT_ORM_H */