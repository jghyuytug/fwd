#ifndef SLOT_BONUS_INFO_ORM_H
#define SLOT_BONUS_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    short slot_part_code;
    int bonus_ipg_no;
    short bonus_rate;
} SlotBonusInfo;

/* CRUD Operations */
int SlotBonusInfo_Add(DBConnectionManager* manager, const SlotBonusInfo* record);
int SlotBonusInfo_Get(DBConnectionManager* manager, short slot_part_code, int bonus_ipg_no, SlotBonusInfo* record);
int SlotBonusInfo_Update(DBConnectionManager* manager, const SlotBonusInfo* record);
int SlotBonusInfo_Delete(DBConnectionManager* manager, short slot_part_code, int bonus_ipg_no);
int SlotBonusInfo_Exists(DBConnectionManager* manager, short slot_part_code, int bonus_ipg_no);
int SlotBonusInfo_GetAll(DBConnectionManager* manager, SlotBonusInfo* records, int max_count, int* actual_count);

#endif /* SLOT_BONUS_INFO_ORM_H */