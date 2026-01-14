#ifndef SLOT_PROD_ABILITY_INFO_ORM_H
#define SLOT_PROD_ABILITY_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    short slot_part_code;
    signed char slot_num;
    signed char slot_position;
    int slot_ipg_no;
    signed char job;
    signed char grow_type;
    int ability_no;
    short rate;
} SlotProdAbilityInfo;

/* CRUD Operations */
int SlotProdAbilityInfo_Add(DBConnectionManager* manager, const SlotProdAbilityInfo* record);
int SlotProdAbilityInfo_Get(DBConnectionManager* manager, short slot_part_code, signed char slot_num, signed char slot_position, int slot_ipg_no, signed char job, signed char grow_type, int ability_no, SlotProdAbilityInfo* record);
int SlotProdAbilityInfo_Update(DBConnectionManager* manager, const SlotProdAbilityInfo* record);
int SlotProdAbilityInfo_Delete(DBConnectionManager* manager, short slot_part_code, signed char slot_num, signed char slot_position, int slot_ipg_no, signed char job, signed char grow_type, int ability_no);
int SlotProdAbilityInfo_Exists(DBConnectionManager* manager, short slot_part_code, signed char slot_num, signed char slot_position, int slot_ipg_no, signed char job, signed char grow_type, int ability_no);
int SlotProdAbilityInfo_GetAll(DBConnectionManager* manager, SlotProdAbilityInfo* records, int max_count, int* actual_count);

#endif /* SLOT_PROD_ABILITY_INFO_ORM_H */