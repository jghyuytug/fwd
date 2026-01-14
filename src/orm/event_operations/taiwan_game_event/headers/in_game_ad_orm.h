#ifndef IN_GAME_AD_ORM_H
#define IN_GAME_AD_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int banner_id;
    char title[51];
    signed char gender;
    signed char age;
    signed char job;
    char level[256];
    char banner_url[256];
    int item_no;
    char event_url[256];
    char ctrl_banner_url[256];
    char ctrl_event_url[256];
    int ctrl_item_no;
    char ctrl_description[256];
    int min_sera;
    int max_sera;
    signed char type_code;
    signed char visible;
    int limit_m_id;
    int view_code;
    char image_url[256];
    char description[256];
    signed char log_flag;
    char ex_property[256];
} InGameAd;

/* CRUD Operations */
int InGameAd_Add(DBConnectionManager* manager, const InGameAd* record);
int InGameAd_Get(DBConnectionManager* manager, int banner_id, InGameAd* record);
int InGameAd_Update(DBConnectionManager* manager, const InGameAd* record);
int InGameAd_Delete(DBConnectionManager* manager, int banner_id);
int InGameAd_Exists(DBConnectionManager* manager, int banner_id);
int InGameAd_GetAll(DBConnectionManager* manager, InGameAd* records, int max_count, int* actual_count);

#endif /* IN_GAME_AD_ORM_H */