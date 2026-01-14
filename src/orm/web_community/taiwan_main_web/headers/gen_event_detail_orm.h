#ifndef GEN_EVENT_DETAIL_ORM_H
#define GEN_EVENT_DETAIL_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int gno;
    signed char type_flag1;
    signed char type_flag2;
    signed char type_flag3;
    signed char type_flag4;
    signed char type_flag5;
    signed char type_flag6;
    char content1[1024];
    char file_location1[101];
    char content2[1024];
    char file_location2[101];
    char content3[1024];
    char file_location3[101];
    char content4[1024];
    char file_location4[101];
    char content5[1024];
    char file_location5[101];
    char content6[1024];
    char file_location6[101];
} GenEventDetail;

/* CRUD Operations */
int GenEventDetail_Add(DBConnectionManager* manager, const GenEventDetail* record);
int GenEventDetail_Get(DBConnectionManager* manager, int gno, GenEventDetail* record);
int GenEventDetail_Update(DBConnectionManager* manager, const GenEventDetail* record);
int GenEventDetail_Delete(DBConnectionManager* manager, int gno);
int GenEventDetail_Exists(DBConnectionManager* manager, int gno);
int GenEventDetail_GetAll(DBConnectionManager* manager, GenEventDetail* records, int max_count, int* actual_count);

#endif /* GEN_EVENT_DETAIL_ORM_H */