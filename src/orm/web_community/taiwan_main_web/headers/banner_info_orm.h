#ifndef BANNER_INFO_ORM_H
#define BANNER_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int id;
    signed char useyn;
    signed char priority;
    char img_url[101];
    char target_url[101];
    char title[51];
    int reg_date;
    char intro[1024];
    int no;
    signed char banner_type;
    signed char banner_type_sub;
    int start_date;
    int end_date;
} BannerInfo;

/* CRUD Operations */
int BannerInfo_Add(DBConnectionManager* manager, const BannerInfo* record);
int BannerInfo_Get(DBConnectionManager* manager, int id, BannerInfo* record);
int BannerInfo_Update(DBConnectionManager* manager, const BannerInfo* record);
int BannerInfo_Delete(DBConnectionManager* manager, int id);
int BannerInfo_Exists(DBConnectionManager* manager, int id);
int BannerInfo_GetAll(DBConnectionManager* manager, BannerInfo* records, int max_count, int* actual_count);

#endif /* BANNER_INFO_ORM_H */