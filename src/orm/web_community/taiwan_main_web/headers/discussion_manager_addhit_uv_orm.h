#ifndef DISCUSSION_MANAGER_ADDHIT_UV_ORM_H
#define DISCUSSION_MANAGER_ADDHIT_UV_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int click_view;
} DiscussionManagerAddhitUv;

/* CRUD Operations */
int DiscussionManagerAddhitUv_Add(DBConnectionManager* manager, const DiscussionManagerAddhitUv* record);
int DiscussionManagerAddhitUv_Get(DBConnectionManager* manager, int no, DiscussionManagerAddhitUv* record);
int DiscussionManagerAddhitUv_Update(DBConnectionManager* manager, const DiscussionManagerAddhitUv* record);
int DiscussionManagerAddhitUv_Delete(DBConnectionManager* manager, int no);
int DiscussionManagerAddhitUv_Exists(DBConnectionManager* manager, int no);
int DiscussionManagerAddhitUv_GetAll(DBConnectionManager* manager, DiscussionManagerAddhitUv* records, int max_count, int* actual_count);

#endif /* DISCUSSION_MANAGER_ADDHIT_UV_ORM_H */