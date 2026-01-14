#ifndef DNF_MOBILE_BOARD_ORM_H
#define DNF_MOBILE_BOARD_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int dmb_idx;
    signed char cate;
    signed char open_flag;
    char title[51];
    char summary[101];
    char thumbnail_img[101];
    int view;
    int comment;
    char occ_time[20];
} DnfMobileBoard;

/* CRUD Operations */
int DnfMobileBoard_Add(DBConnectionManager* manager, const DnfMobileBoard* record);
int DnfMobileBoard_Get(DBConnectionManager* manager, int dmb_idx, DnfMobileBoard* record);
int DnfMobileBoard_Update(DBConnectionManager* manager, const DnfMobileBoard* record);
int DnfMobileBoard_Delete(DBConnectionManager* manager, int dmb_idx);
int DnfMobileBoard_Exists(DBConnectionManager* manager, int dmb_idx);
int DnfMobileBoard_GetAll(DBConnectionManager* manager, DnfMobileBoard* records, int max_count, int* actual_count);

#endif /* DNF_MOBILE_BOARD_ORM_H */