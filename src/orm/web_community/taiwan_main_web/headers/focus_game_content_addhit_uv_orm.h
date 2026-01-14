#ifndef FOCUS_GAME_CONTENT_ADDHIT_UV_ORM_H
#define FOCUS_GAME_CONTENT_ADDHIT_UV_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int view;
    int img_view;
} FocusGameContentAddhitUv;

/* CRUD Operations */
int FocusGameContentAddhitUv_Add(DBConnectionManager* manager, const FocusGameContentAddhitUv* record);
int FocusGameContentAddhitUv_Get(DBConnectionManager* manager, int no, FocusGameContentAddhitUv* record);
int FocusGameContentAddhitUv_Update(DBConnectionManager* manager, const FocusGameContentAddhitUv* record);
int FocusGameContentAddhitUv_Delete(DBConnectionManager* manager, int no);
int FocusGameContentAddhitUv_Exists(DBConnectionManager* manager, int no);
int FocusGameContentAddhitUv_GetAll(DBConnectionManager* manager, FocusGameContentAddhitUv* records, int max_count, int* actual_count);

#endif /* FOCUS_GAME_CONTENT_ADDHIT_UV_ORM_H */