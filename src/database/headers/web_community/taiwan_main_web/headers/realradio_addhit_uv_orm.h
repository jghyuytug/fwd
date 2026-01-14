#ifndef REALRADIO_ADDHIT_UV_ORM_H
#define REALRADIO_ADDHIT_UV_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int click_view;
} RealradioAddhitUv;

/* CRUD Operations */
int RealradioAddhitUv_Add(DBConnectionManager* manager, const RealradioAddhitUv* record);
int RealradioAddhitUv_Get(DBConnectionManager* manager, int no, RealradioAddhitUv* record);
int RealradioAddhitUv_Update(DBConnectionManager* manager, const RealradioAddhitUv* record);
int RealradioAddhitUv_Delete(DBConnectionManager* manager, int no);
int RealradioAddhitUv_Exists(DBConnectionManager* manager, int no);
int RealradioAddhitUv_GetAll(DBConnectionManager* manager, RealradioAddhitUv* records, int max_count, int* actual_count);

#endif /* REALRADIO_ADDHIT_UV_ORM_H */