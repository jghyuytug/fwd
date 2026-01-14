#include "db/db_inventory.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "database/headers/db_connection_manager.h"
#include "game_server_context.h"
#include "inventory_orm.h"
#include "user_items_orm.h"

static int execute_query_and_fetch(const char* query, char** values, int expected_columns) {
    DBQueryResult result;
    std::memset(&result, 0, sizeof(result));

    if (DBConnectionManager_ExecuteQuery(&g_db_manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    int fetched = DBQueryResult_FetchRow(&result, values);
    DBQueryResult_Free(&result);
    return fetched == expected_columns ? 0 : -1;
}

static int execute_non_query(const char* query) {
    DBQueryResult result;
    std::memset(&result, 0, sizeof(result));

    if (DBConnectionManager_ExecuteQuery(&g_db_manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    int affected = result.row_count;
    DBQueryResult_Free(&result);
    return affected;
}

static void fill_db_item_from_row(DBItemInfo* item, char** row, int slot_hint) {
    std::memset(item, 0, sizeof(DBItemInfo));
    item->ui_id = row[0] ? static_cast<uint32_t>(strtoul(row[0], NULL, 10)) : 0;
    item->slot = row[1] ? static_cast<uint32_t>(strtoul(row[1], NULL, 10))
                        : static_cast<uint32_t>(slot_hint);
    item->it_id = row[2] ? static_cast<uint32_t>(strtoul(row[2], NULL, 10)) : 0;
    item->stat = row[3] ? static_cast<uint8_t>(strtoul(row[3], NULL, 10)) : 0;
    item->hidden_option = row[4] ? static_cast<uint16_t>(strtoul(row[4], NULL, 10)) : 0;
    item->endurance = row[5] ? static_cast<uint8_t>(strtoul(row[5], NULL, 10)) : 0;
}

static int query_item_at_slot(uint32_t charac_no, int slot, DBItemInfo* item) {
    if (!item) return -1;

    char query[256];
    std::snprintf(query, sizeof(query),
                  "SELECT ui_id, slot, it_id, stat, hidden_option, emblem_endurance "
                  "FROM user_items WHERE charac_no=%u AND slot=%d LIMIT 1",
                  charac_no, slot);

    char* row[6] = {0};
    if (execute_query_and_fetch(query, row, 6) < 0) {
        return -1;
    }

    fill_db_item_from_row(item, row, slot);
    return 0;
}

static int query_item_by_ui_id(uint32_t charac_no, uint32_t ui_id, DBItemInfo* item) {
    if (!item) return -1;

    char query[256];
    std::snprintf(query, sizeof(query),
                  "SELECT ui_id, slot, it_id, stat, hidden_option, emblem_endurance "
                  "FROM user_items WHERE charac_no=%u AND ui_id=%u LIMIT 1",
                  charac_no, ui_id);

    char* row[6] = {0};
    if (execute_query_and_fetch(query, row, 6) < 0) {
        return -1;
    }

    fill_db_item_from_row(item, row, 0);
    return 0;
}

static void load_bag_items(uint32_t charac_no, DBInventoryInfo* inv) {
    if (!inv) return;

    char query[512];
    std::snprintf(query, sizeof(query),
                  "SELECT ui_id, slot, it_id, stat FROM user_items "
                  "WHERE charac_no=%u AND slot >= 11 ORDER BY slot LIMIT %d",
                  charac_no, MAX_BAG_ITEMS);

    DBQueryResult result;
    std::memset(&result, 0, sizeof(result));

    if (DBConnectionManager_ExecuteQuery(&g_db_manager, DB_TYPE_CAIN, query, &result) < 0) {
        std::printf("[DB][INVENTORY] Failed to load bag items for charac_no=%u\n", charac_no);
        return;
    }

    char* values[4] = {0};
    while (inv->item_count < MAX_BAG_ITEMS &&
           DBQueryResult_FetchRow(&result, values) == 4) {
        DBBagItem* item = &inv->items[inv->item_count];
        item->ui_id = values[0] ? static_cast<uint32_t>(strtoul(values[0], NULL, 10)) : 0;
        item->slot = values[1] ? static_cast<uint16_t>(strtoul(values[1], NULL, 10)) : 0;
        item->it_id = values[2] ? static_cast<uint32_t>(strtoul(values[2], NULL, 10)) : 0;
        item->upgrade = values[3] ? static_cast<uint8_t>(strtoul(values[3], NULL, 10)) : 0;
        item->count = 1;
        item->seal = 0;
        item->flags = 0;
        inv->item_count++;
    }

    DBQueryResult_Free(&result);
}

int db_get_inventory(uint32_t charac_no, DBInventoryInfo* inv) {
    if (!inv) {
        return -1;
    }

    std::memset(inv, 0, sizeof(DBInventoryInfo));
    inv->charac_no = charac_no;

    Inventory orm_inv;
    std::memset(&orm_inv, 0, sizeof(orm_inv));
    if (Inventory_Get(&g_db_manager, static_cast<int>(charac_no), &orm_inv) == 0) {
        inv->money = static_cast<uint32_t>(orm_inv.money);
        inv->coin = static_cast<uint32_t>(orm_inv.coin);
        inv->pay_coin = static_cast<uint32_t>(orm_inv.pay_coin);
        inv->event_coin = static_cast<uint32_t>(orm_inv.event_coin);
        inv->inventory_capacity = static_cast<uint32_t>(orm_inv.inventory_capacity);
    } else {
        std::printf("[DB][INVENTORY] Inventory_Get failed for charac_no=%u\n", charac_no);
    }

    load_bag_items(charac_no, inv);

    inv->inventory_loaded = 1;
    std::printf("[DB] Loaded inventory for charac_no=%u: money=%u, items=%u\n",
                charac_no, inv->money, inv->item_count);
    return 0;
}

int db_add_gold(uint32_t charac_no, int32_t amount) {
    if (amount == 0) return -1;

    char query[256];
    std::snprintf(query, sizeof(query),
                  "UPDATE inventory SET money=money+%d WHERE charac_no=%u",
                  amount, charac_no);

    int affected = execute_non_query(query);
    std::printf("[DB] Added %u gold for charac_no=%u (affected=%d)\n",
                amount, charac_no, affected);
    return affected > 0 ? 0 : -1;
}

int db_update_gold(uint32_t charac_no, int32_t new_gold) {
    char query[256];
    std::snprintf(query, sizeof(query),
                  "UPDATE inventory SET money=%d WHERE charac_no=%u",
                  new_gold, charac_no);

    int affected = execute_non_query(query);
    std::printf("[DB] Updated gold for charac_no=%u: new_gold=%u (affected=%d)\n",
                charac_no, new_gold, affected);
    return affected > 0 ? 0 : -1;
}

int db_find_free_slot(uint32_t charac_no, int start_slot) {
    char query[256];
    std::snprintf(query, sizeof(query),
                  "SELECT slot FROM user_items WHERE charac_no=%u AND slot >= %d ORDER BY slot",
                  charac_no, start_slot);

    DBQueryResult result;
    std::memset(&result, 0, sizeof(result));
    if (DBConnectionManager_ExecuteQuery(&g_db_manager, DB_TYPE_CAIN, query, &result) < 0) {
        return start_slot;
    }

    char* row[1] = {0};
    int expected_slot = start_slot;
    while (DBQueryResult_FetchRow(&result, row) == 1) {
        int used_slot = row[0] ? std::atoi(row[0]) : 0;
        if (used_slot > expected_slot) {
            break;
        }
        expected_slot = used_slot + 1;
    }

    DBQueryResult_Free(&result);
    return expected_slot;
}

int db_add_item(uint32_t charac_no, uint32_t item_id, int slot, uint8_t stat) {
    UserItems record;
    std::memset(&record, 0, sizeof(record));
    record.charac_no = static_cast<int>(charac_no);
    record.slot = slot;
    record.it_id = static_cast<int>(item_id);
    record.stat = static_cast<signed char>(stat);
    record.hidden_option = 0;
    record.emblem_endurance = 0;

    if (UserItems_Add(&g_db_manager, &record) < 0) {
        std::printf("[DB] Add item failed for charac_no=%u slot=%d\n", charac_no, slot);
        return -1;
    }

    DBItemInfo inserted;
    if (query_item_at_slot(charac_no, slot, &inserted) < 0 || inserted.ui_id == 0) {
        std::printf("[DB] Failed to read inserted item ui_id for charac_no=%u slot=%d\n",
                    charac_no, slot);
        return -1;
    }

    std::printf("[DB] Added item for charac_no=%u: item_id=%u slot=%d ui_id=%u\n",
                charac_no, item_id, slot, inserted.ui_id);
    return static_cast<int>(inserted.ui_id);
}

int db_delete_item(uint32_t charac_no, uint32_t ui_id) {
    (void)charac_no;
    if (UserItems_Delete(&g_db_manager, static_cast<int>(ui_id)) < 0) {
        std::printf("[DB] Delete item failed for ui_id=%u\n", ui_id);
        return -1;
    }

    std::printf("[DB] Deleted item ui_id=%u\n", ui_id);
    return 0;
}

int db_delete_item_at_slot(uint32_t charac_no, int slot) {
    DBItemInfo item;
    if (query_item_at_slot(charac_no, slot, &item) < 0 || item.ui_id == 0) {
        std::printf("[DB] Delete item failed: no item at slot %d (char=%u)\n", slot, charac_no);
        return -1;
    }

    return db_delete_item(charac_no, item.ui_id);
}

int db_update_item_slot(uint32_t charac_no, uint32_t ui_id, int new_slot) {
    char query[256];
    std::snprintf(query, sizeof(query),
                  "UPDATE user_items SET slot=%d WHERE charac_no=%u AND ui_id=%u LIMIT 1",
                  new_slot, charac_no, ui_id);

    int affected = execute_non_query(query);
    std::printf("[DB] Moved item for charac_no=%u ui_id=%u -> slot %d (affected=%d)\n",
                charac_no, ui_id, new_slot, affected);
    return affected > 0 ? 0 : -1;
}

int db_get_item_at_slot(uint32_t charac_no, int slot, DBItemInfo* item) {
    return query_item_at_slot(charac_no, slot, item);
}

int db_get_item_info_at_slot(uint32_t charac_no, int slot, DBItemInfo* item) {
    return query_item_at_slot(charac_no, slot, item);
}

int db_get_item_by_ui_id(uint32_t charac_no, uint32_t ui_id, DBItemInfo* item) {
    return query_item_by_ui_id(charac_no, ui_id, item);
}

int db_repair_item(uint32_t charac_no, int slot) {
    char query[256];
    std::snprintf(query, sizeof(query),
                  "UPDATE user_items SET stat=100 WHERE charac_no=%u AND slot=%d LIMIT 1",
                  charac_no, slot);
    return execute_non_query(query) > 0 ? 0 : -1;
}

int db_repair_all_items(uint32_t charac_no) {
    char query[256];
    std::snprintf(query, sizeof(query),
                  "UPDATE user_items SET stat=100 WHERE charac_no=%u AND slot BETWEEN 0 AND 12",
                  charac_no);
    return execute_non_query(query);
}
