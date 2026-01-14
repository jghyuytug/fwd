#ifndef DNF_GAMEIN_SERASHOP_ORM_H
#define DNF_GAMEIN_SERASHOP_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char img_name[251];
    signed char server_id;
    char reg_time[20];
    char open_flag;
    signed char banner_type;
} DnfGameinSerashop;

/* CRUD Operations */
int DnfGameinSerashop_Add(DBConnectionManager* manager, const DnfGameinSerashop* record);
int DnfGameinSerashop_Get(DBConnectionManager* manager, int no, DnfGameinSerashop* record);
int DnfGameinSerashop_Update(DBConnectionManager* manager, const DnfGameinSerashop* record);
int DnfGameinSerashop_Delete(DBConnectionManager* manager, int no);
int DnfGameinSerashop_Exists(DBConnectionManager* manager, int no);
int DnfGameinSerashop_GetAll(DBConnectionManager* manager, DnfGameinSerashop* records, int max_count, int* actual_count);

#endif /* DNF_GAMEIN_SERASHOP_ORM_H */