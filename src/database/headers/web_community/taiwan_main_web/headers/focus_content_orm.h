#ifndef FOCUS_CONTENT_ORM_H
#define FOCUS_CONTENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int focus_no;
    signed char category;
    char title[256];
    char popular_word[256];
    char link[256];
    int view;
    signed char best;
    char review[121];
} FocusContent;

/* CRUD Operations */
int FocusContent_Add(DBConnectionManager* manager, const FocusContent* record);
int FocusContent_Get(DBConnectionManager* manager, int no, FocusContent* record);
int FocusContent_Update(DBConnectionManager* manager, const FocusContent* record);
int FocusContent_Delete(DBConnectionManager* manager, int no);
int FocusContent_Exists(DBConnectionManager* manager, int no);
int FocusContent_GetAll(DBConnectionManager* manager, FocusContent* records, int max_count, int* actual_count);

#endif /* FOCUS_CONTENT_ORM_H */