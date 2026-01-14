#ifndef UPDATE_ACT_INFO_ORM_H
#define UPDATE_ACT_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int act_no;
    char subject[61];
    char MNG_user_id[31];
    char reg_date[20];
    char up_date[20];
    int hits;
    char open_flag;
    int open_date;
    char image_url[101];
} UpdateActInfo;

/* CRUD Operations */
int UpdateActInfo_Add(DBConnectionManager* manager, const UpdateActInfo* record);
int UpdateActInfo_Get(DBConnectionManager* manager, int act_no, UpdateActInfo* record);
int UpdateActInfo_Update(DBConnectionManager* manager, const UpdateActInfo* record);
int UpdateActInfo_Delete(DBConnectionManager* manager, int act_no);
int UpdateActInfo_Exists(DBConnectionManager* manager, int act_no);
int UpdateActInfo_GetAll(DBConnectionManager* manager, UpdateActInfo* records, int max_count, int* actual_count);

#endif /* UPDATE_ACT_INFO_ORM_H */