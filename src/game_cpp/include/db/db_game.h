#ifndef DB_GAME_H
#define DB_GAME_H

#include <stdint.h>
#include "db/db_types.h"
// Removed: game_server_types.h - DBAccountInfo now in db_types.h

#ifdef __cplusplus
extern "C" {
#endif

int db_get_account_by_uid(uint32_t uid, DBAccountInfo* account);
int db_get_character(uint32_t charac_no, DBCharacterInfo* ch);
int db_get_equipment(uint32_t charac_no, DBEquipmentInfo* equip);
int db_soft_delete_character(uint32_t charac_no, uint32_t m_id);
int db_update_character_hp_mp(uint32_t charac_no, uint16_t hp, uint16_t mp);

int db_add_gold(uint32_t charac_no, int32_t amount);
int db_update_gold(uint32_t charac_no, int32_t amount);
int db_find_free_slot(uint32_t charac_no, int start_slot);
int db_add_item(uint32_t charac_no, uint32_t item_id, int slot, uint8_t stat);
int db_delete_item(uint32_t charac_no, uint32_t ui_id);
int db_delete_item_at_slot(uint32_t charac_no, int slot);
int db_get_item_at_slot(uint32_t charac_no, int slot, DBItemInfo* item);
int db_get_item_info_at_slot(uint32_t charac_no, int slot, DBItemInfo* item);
int db_get_item_by_ui_id(uint32_t charac_no, uint32_t ui_id, DBItemInfo* item);
int db_update_item_slot(uint32_t charac_no, uint32_t ui_id, int new_slot);
int db_repair_item(uint32_t charac_no, int slot);
int db_repair_all_items(uint32_t charac_no);
uint32_t calc_repair_cost(uint32_t item_id, uint8_t cur_dur);

int db_update_achievement(uint32_t charac_no, uint16_t ach_id, uint32_t progress,
                          uint8_t completed, uint8_t claimed);
int db_get_abyss_daily_count(uint32_t charac_no);
int db_get_abyss_ticket_count(uint32_t charac_no);

#ifdef __cplusplus
}
#endif

#endif /* DB_GAME_H */
