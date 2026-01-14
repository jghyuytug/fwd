#ifndef BANNER_INFO2_ORM_H
#define BANNER_INFO2_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int id;
    signed char useyn;
    signed char priority;
    char img_url[101];
    char target_url[101];
    int reg_date;
    char intro[1024];
    int no;
} BannerInfo2;

/* CRUD Operations */
int BannerInfo2_Add(DBConnectionManager* manager, const BannerInfo2* record);
int BannerInfo2_Get(DBConnectionManager* manager, int id, BannerInfo2* record);
int BannerInfo2_Update(DBConnectionManager* manager, const BannerInfo2* record);
int BannerInfo2_Delete(DBConnectionManager* manager, int id);
int BannerInfo2_Exists(DBConnectionManager* manager, int id);
int BannerInfo2_GetAll(DBConnectionManager* manager, BannerInfo2* records, int max_count, int* actual_count);

#endif /* BANNER_INFO2_ORM_H */