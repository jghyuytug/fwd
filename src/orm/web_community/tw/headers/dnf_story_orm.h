#ifndef DNF_STORY_ORM_H
#define DNF_STORY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    signed char story_type;
    signed char notice_flag;
    int m_id;
    char reg_id[13];
    char title[51];
    char url[251];
    char img_name[31];
    signed char opt;
    char open_flag;
    char reg_date[20];
    int hits;
    int reserve_time;
    char content[1024];
} DnfStory;

/* CRUD Operations */
int DnfStory_Add(DBConnectionManager* manager, const DnfStory* record);
int DnfStory_Get(DBConnectionManager* manager, int no, DnfStory* record);
int DnfStory_Update(DBConnectionManager* manager, const DnfStory* record);
int DnfStory_Delete(DBConnectionManager* manager, int no);
int DnfStory_Exists(DBConnectionManager* manager, int no);
int DnfStory_GetAll(DBConnectionManager* manager, DnfStory* records, int max_count, int* actual_count);

#endif /* DNF_STORY_ORM_H */