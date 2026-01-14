#ifndef FOCUS_GAME_CONTENT_ORM_H
#define FOCUS_GAME_CONTENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int focus_no;
    signed char category;
    char title[256];
    char popular_word[256];
    char link[256];
    int view;
    int view_uv;
    signed char best;
    char review[121];
    char img_url[256];
    int img_view;
} FocusGameContent;

/* CRUD Operations */
int FocusGameContent_Add(DBConnectionManager* manager, const FocusGameContent* record);
int FocusGameContent_Get(DBConnectionManager* manager, int no, FocusGameContent* record);
int FocusGameContent_Update(DBConnectionManager* manager, const FocusGameContent* record);
int FocusGameContent_Delete(DBConnectionManager* manager, int no);
int FocusGameContent_Exists(DBConnectionManager* manager, int no);
int FocusGameContent_GetAll(DBConnectionManager* manager, FocusGameContent* records, int max_count, int* actual_count);

#endif /* FOCUS_GAME_CONTENT_ORM_H */