#ifndef MONTH_ITEM_ORM_H
#define MONTH_ITEM_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char state;
    char pre_img[101];
    char now_img[101];
    char next_img[101];
    int admin_no;
    char reg_date[20];
    char pre_pop[101];
    char now_pop[101];
    char next_pop[101];
    char pre_map[17];
    char now_map[17];
    char next_map[17];
} MonthItem;

/* CRUD Operations */
int MonthItem_Add(DBConnectionManager* manager, const MonthItem* record);
int MonthItem_Get(DBConnectionManager* manager, int no, MonthItem* record);
int MonthItem_Update(DBConnectionManager* manager, const MonthItem* record);
int MonthItem_Delete(DBConnectionManager* manager, int no);
int MonthItem_Exists(DBConnectionManager* manager, int no);
int MonthItem_GetAll(DBConnectionManager* manager, MonthItem* records, int max_count, int* actual_count);

#endif /* MONTH_ITEM_ORM_H */