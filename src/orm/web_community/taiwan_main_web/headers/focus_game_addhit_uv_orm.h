#ifndef FOCUS_GAME_ADDHIT_UV_ORM_H
#define FOCUS_GAME_ADDHIT_UV_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int group_view;
} FocusGameAddhitUv;

/* CRUD Operations */
int FocusGameAddhitUv_Add(DBConnectionManager* manager, const FocusGameAddhitUv* record);
int FocusGameAddhitUv_Get(DBConnectionManager* manager, int no, FocusGameAddhitUv* record);
int FocusGameAddhitUv_Update(DBConnectionManager* manager, const FocusGameAddhitUv* record);
int FocusGameAddhitUv_Delete(DBConnectionManager* manager, int no);
int FocusGameAddhitUv_Exists(DBConnectionManager* manager, int no);
int FocusGameAddhitUv_GetAll(DBConnectionManager* manager, FocusGameAddhitUv* records, int max_count, int* actual_count);

#endif /* FOCUS_GAME_ADDHIT_UV_ORM_H */