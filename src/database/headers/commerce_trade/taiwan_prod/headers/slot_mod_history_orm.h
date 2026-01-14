#ifndef SLOT_MOD_HISTORY_ORM_H
#define SLOT_MOD_HISTORY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char mod_time[20];
    short slot_part_code;
    signed char slot_num;
    signed char slot_position;
    int slot1_ipg_no;
    short slot1_rate;
    signed char mod_type;
    char mod_user_id[26];
    char mod_explain[251];
} SlotModHistory;

/* CRUD Operations */
int SlotModHistory_Add(DBConnectionManager* manager, const SlotModHistory* record);
int SlotModHistory_Get(DBConnectionManager* manager, const char* mod_time, short slot_part_code, signed char slot_num, signed char slot_position, SlotModHistory* record);
int SlotModHistory_Update(DBConnectionManager* manager, const SlotModHistory* record);
int SlotModHistory_Delete(DBConnectionManager* manager, const char* mod_time, short slot_part_code, signed char slot_num, signed char slot_position);
int SlotModHistory_Exists(DBConnectionManager* manager, const char* mod_time, short slot_part_code, signed char slot_num, signed char slot_position);
int SlotModHistory_GetAll(DBConnectionManager* manager, SlotModHistory* records, int max_count, int* actual_count);

#endif /* SLOT_MOD_HISTORY_ORM_H */