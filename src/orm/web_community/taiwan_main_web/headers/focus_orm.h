#ifndef FOCUS_ORM_H
#define FOCUS_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int reg_date;
    int open_date;
    char img_url[256];
    char img_link[256];
    char today_word[256];
    char hot_title[121];
    char hot_link[256];
    char hot_content[256];
    char hot_word[256];
    int img_view;
    int hot_view;
    int comment;
} Focus;

/* CRUD Operations */
int Focus_Add(DBConnectionManager* manager, const Focus* record);
int Focus_Get(DBConnectionManager* manager, int no, Focus* record);
int Focus_Update(DBConnectionManager* manager, const Focus* record);
int Focus_Delete(DBConnectionManager* manager, int no);
int Focus_Exists(DBConnectionManager* manager, int no);
int Focus_GetAll(DBConnectionManager* manager, Focus* records, int max_count, int* actual_count);

#endif /* FOCUS_ORM_H */