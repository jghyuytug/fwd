#ifndef DB_INVENTORY_H
#define DB_INVENTORY_H

#include <stdint.h>
#include "db/db_types.h"

#ifdef __cplusplus
extern "C" {
#endif

int db_get_inventory(uint32_t charac_no, DBInventoryInfo* inv);

/* Gold functions */
int db_add_gold(uint32_t charac_no, int32_t amount);
int db_update_gold(uint32_t charac_no, int32_t new_gold);

/* Item slot functions */
int db_find_free_slot(uint32_t charac_no, int start_slot);

/* Item CRUD functions */
int db_add_item(uint32_t charac_no, uint32_t item_id, int slot, uint8_t stat);
int db_delete_item(uint32_t charac_no, uint32_t ui_id);
int db_delete_item_at_slot(uint32_t charac_no, int slot);
int db_update_item_slot(uint32_t charac_no, uint32_t ui_id, int new_slot);

/* Item query functions */
int db_get_item_at_slot(uint32_t charac_no, int slot, DBItemInfo* item);
int db_get_item_info_at_slot(uint32_t charac_no, int slot, DBItemInfo* item);
int db_get_item_by_ui_id(uint32_t charac_no, uint32_t ui_id, DBItemInfo* item);

/* Item repair functions */
int db_repair_item(uint32_t charac_no, int slot);
int db_repair_all_items(uint32_t charac_no);

#ifdef __cplusplus
}
#endif

#endif /* DB_INVENTORY_H */
