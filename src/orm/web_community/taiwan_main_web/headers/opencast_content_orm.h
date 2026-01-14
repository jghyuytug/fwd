#ifndef OPENCAST_CONTENT_ORM_H
#define OPENCAST_CONTENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int bbs_no;
    int m_id;
    int cate_no;
    signed char cc_type;
    signed char list_type;
    char title[256];
    char popular_word[256];
    char link[256];
    int view;
    char img_url[256];
    int img_view;
    char occ_time[20];
    signed char content_cnt;
    char comment[1024];
} OpencastContent;

/* CRUD Operations */
int OpencastContent_Add(DBConnectionManager* manager, const OpencastContent* record);
int OpencastContent_Get(DBConnectionManager* manager, int bbs_no, OpencastContent* record);
int OpencastContent_Update(DBConnectionManager* manager, const OpencastContent* record);
int OpencastContent_Delete(DBConnectionManager* manager, int bbs_no);
int OpencastContent_Exists(DBConnectionManager* manager, int bbs_no);
int OpencastContent_GetAll(DBConnectionManager* manager, OpencastContent* records, int max_count, int* actual_count);

#endif /* OPENCAST_CONTENT_ORM_H */