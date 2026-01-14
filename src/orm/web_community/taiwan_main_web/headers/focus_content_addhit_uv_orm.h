#ifndef FOCUS_CONTENT_ADDHIT_UV_ORM_H
#define FOCUS_CONTENT_ADDHIT_UV_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int view;
} FocusContentAddhitUv;

/* CRUD Operations */
int FocusContentAddhitUv_Add(DBConnectionManager* manager, const FocusContentAddhitUv* record);
int FocusContentAddhitUv_Get(DBConnectionManager* manager, int no, FocusContentAddhitUv* record);
int FocusContentAddhitUv_Update(DBConnectionManager* manager, const FocusContentAddhitUv* record);
int FocusContentAddhitUv_Delete(DBConnectionManager* manager, int no);
int FocusContentAddhitUv_Exists(DBConnectionManager* manager, int no);
int FocusContentAddhitUv_GetAll(DBConnectionManager* manager, FocusContentAddhitUv* records, int max_count, int* actual_count);

#endif /* FOCUS_CONTENT_ADDHIT_UV_ORM_H */