#ifndef FOCUS_ADDHIT_UV_ORM_H
#define FOCUS_ADDHIT_UV_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int img_view;
    int hot_view;
    int group_view;
} FocusAddhitUv;

/* CRUD Operations */
int FocusAddhitUv_Add(DBConnectionManager* manager, const FocusAddhitUv* record);
int FocusAddhitUv_Get(DBConnectionManager* manager, int no, FocusAddhitUv* record);
int FocusAddhitUv_Update(DBConnectionManager* manager, const FocusAddhitUv* record);
int FocusAddhitUv_Delete(DBConnectionManager* manager, int no);
int FocusAddhitUv_Exists(DBConnectionManager* manager, int no);
int FocusAddhitUv_GetAll(DBConnectionManager* manager, FocusAddhitUv* records, int max_count, int* actual_count);

#endif /* FOCUS_ADDHIT_UV_ORM_H */