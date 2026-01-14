#ifndef DISCUSSION_ENTRY_ORM_H
#define DISCUSSION_ENTRY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int content_no;
    int m_id;
} DiscussionEntry;

/* CRUD Operations */
int DiscussionEntry_Add(DBConnectionManager* manager, const DiscussionEntry* record);
int DiscussionEntry_Get(DBConnectionManager* manager, int content_no, int m_id, DiscussionEntry* record);
int DiscussionEntry_Update(DBConnectionManager* manager, const DiscussionEntry* record);
int DiscussionEntry_Delete(DBConnectionManager* manager, int content_no, int m_id);
int DiscussionEntry_Exists(DBConnectionManager* manager, int content_no, int m_id);
int DiscussionEntry_GetAll(DBConnectionManager* manager, DiscussionEntry* records, int max_count, int* actual_count);

#endif /* DISCUSSION_ENTRY_ORM_H */