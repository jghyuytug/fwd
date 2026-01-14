#ifndef REALRADIO_AGAIN_ORM_H
#define REALRADIO_AGAIN_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char title[121];
    int create_day;
    int broadcast_day;
    char file_url[129];
    signed char useyn;
    int view;
} RealradioAgain;

/* CRUD Operations */
int RealradioAgain_Add(DBConnectionManager* manager, const RealradioAgain* record);
int RealradioAgain_Get(DBConnectionManager* manager, int no, RealradioAgain* record);
int RealradioAgain_Update(DBConnectionManager* manager, const RealradioAgain* record);
int RealradioAgain_Delete(DBConnectionManager* manager, int no);
int RealradioAgain_Exists(DBConnectionManager* manager, int no);
int RealradioAgain_GetAll(DBConnectionManager* manager, RealradioAgain* records, int max_count, int* actual_count);

#endif /* REALRADIO_AGAIN_ORM_H */