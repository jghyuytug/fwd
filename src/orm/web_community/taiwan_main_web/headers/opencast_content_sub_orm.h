#ifndef OPENCAST_CONTENT_SUB_ORM_H
#define OPENCAST_CONTENT_SUB_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int m_id;
    int bbs_no;
    char title[256];
    char popular_word[256];
    char link[256];
    int view;
    char img_url[256];
    int img_view;
} OpencastContentSub;

/* CRUD Operations */
int OpencastContentSub_Add(DBConnectionManager* manager, const OpencastContentSub* record);
int OpencastContentSub_Get(DBConnectionManager* manager, int no, OpencastContentSub* record);
int OpencastContentSub_Update(DBConnectionManager* manager, const OpencastContentSub* record);
int OpencastContentSub_Delete(DBConnectionManager* manager, int no);
int OpencastContentSub_Exists(DBConnectionManager* manager, int no);
int OpencastContentSub_GetAll(DBConnectionManager* manager, OpencastContentSub* records, int max_count, int* actual_count);

#endif /* OPENCAST_CONTENT_SUB_ORM_H */