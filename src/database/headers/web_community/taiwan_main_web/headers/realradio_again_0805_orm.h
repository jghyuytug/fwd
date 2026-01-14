#ifndef REALRADIO_AGAIN_0805_ORM_H
#define REALRADIO_AGAIN_0805_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char title[121];
    int create_day;
    int broadcast_day;
    char file_url[129];
    signed char useyn;
    int view;
    int play_num;
    char scenario[1024];
    int read_cnt;
    char search[31];
    short comment_cnt;
} RealradioAgain0805;

/* CRUD Operations */
int RealradioAgain0805_Add(DBConnectionManager* manager, const RealradioAgain0805* record);
int RealradioAgain0805_Get(DBConnectionManager* manager, int no, RealradioAgain0805* record);
int RealradioAgain0805_Update(DBConnectionManager* manager, const RealradioAgain0805* record);
int RealradioAgain0805_Delete(DBConnectionManager* manager, int no);
int RealradioAgain0805_Exists(DBConnectionManager* manager, int no);
int RealradioAgain0805_GetAll(DBConnectionManager* manager, RealradioAgain0805* records, int max_count, int* actual_count);

#endif /* REALRADIO_AGAIN_0805_ORM_H */