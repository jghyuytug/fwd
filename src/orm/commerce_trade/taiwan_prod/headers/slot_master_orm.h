#ifndef SLOT_MASTER_ORM_H
#define SLOT_MASTER_ORM_H

#include "db_connection_manager.h"

typedef struct {
    signed char slot_part_type;
    short slot_part_code;
    char slot_part_name[36];
    signed char slot_part_step;
    char slot_part_script[51];
    short slot_npc_no;
    char slot_sell_item_type[36];
    signed char slot_set_flag;
    short slot_price;
    short slot_bonus_rate;
    int slot_it_no;
    signed char slot_it_cnt;
    signed char slot_stat;
} SlotMaster;

/* CRUD Operations */
int SlotMaster_Add(DBConnectionManager* manager, const SlotMaster* record);
int SlotMaster_Get(DBConnectionManager* manager, short slot_part_code, SlotMaster* record);
int SlotMaster_Update(DBConnectionManager* manager, const SlotMaster* record);
int SlotMaster_Delete(DBConnectionManager* manager, short slot_part_code);
int SlotMaster_Exists(DBConnectionManager* manager, short slot_part_code);
int SlotMaster_GetAll(DBConnectionManager* manager, SlotMaster* records, int max_count, int* actual_count);

#endif /* SLOT_MASTER_ORM_H */