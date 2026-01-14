#ifndef SLOT_INFO_ORM_H
#define SLOT_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    short slot_part_code;
    signed char slot_num;
    signed char slot_position;
    int slot1_ipg_no;
    short slot1_rate;
    char regist_user_id[26];
    char regist_time[20];
    char mod_user_id[26];
    char mod_time[20];
} SlotInfo;

/* CRUD Operations */
int SlotInfo_Add(DBConnectionManager* manager, const SlotInfo* record);
int SlotInfo_Get(DBConnectionManager* manager, short slot_part_code, signed char slot_num, signed char slot_position, SlotInfo* record);
int SlotInfo_Update(DBConnectionManager* manager, const SlotInfo* record);
int SlotInfo_Delete(DBConnectionManager* manager, short slot_part_code, signed char slot_num, signed char slot_position);
int SlotInfo_Exists(DBConnectionManager* manager, short slot_part_code, signed char slot_num, signed char slot_position);
int SlotInfo_GetAll(DBConnectionManager* manager, SlotInfo* records, int max_count, int* actual_count);

#endif /* SLOT_INFO_ORM_H */