#ifndef GUIDE_MENU_ORM_H
#define GUIDE_MENU_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int bbs_no;
    char url[256];
    char title[256];
    int sort_no;
    signed char view_type;
    int depth_1;
    int depth_2;
    int depth_3;
    int depth_4;
} GuideMenu;

/* CRUD Operations */
int GuideMenu_Add(DBConnectionManager* manager, const GuideMenu* record);
int GuideMenu_Get(DBConnectionManager* manager, int no, GuideMenu* record);
int GuideMenu_Update(DBConnectionManager* manager, const GuideMenu* record);
int GuideMenu_Delete(DBConnectionManager* manager, int no);
int GuideMenu_Exists(DBConnectionManager* manager, int no);
int GuideMenu_GetAll(DBConnectionManager* manager, GuideMenu* records, int max_count, int* actual_count);

#endif /* GUIDE_MENU_ORM_H */