#ifndef DNF_GAMEIN_NOTICE_ORM_H
#define DNF_GAMEIN_NOTICE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char img_name[251];
    signed char server_id;
    char reg_time[20];
    char open_flag;
} DnfGameinNotice;

/* CRUD Operations */
int DnfGameinNotice_Add(DBConnectionManager* manager, const DnfGameinNotice* record);
int DnfGameinNotice_Get(DBConnectionManager* manager, int no, DnfGameinNotice* record);
int DnfGameinNotice_Update(DBConnectionManager* manager, const DnfGameinNotice* record);
int DnfGameinNotice_Delete(DBConnectionManager* manager, int no);
int DnfGameinNotice_Exists(DBConnectionManager* manager, int no);
int DnfGameinNotice_GetAll(DBConnectionManager* manager, DnfGameinNotice* records, int max_count, int* actual_count);

#endif /* DNF_GAMEIN_NOTICE_ORM_H */