#include "handlers/handler_inventory.h"

#include <cstdio>
#include <cstring>

#include "db/db_game.h"
#include "handlers/gameplay_helpers.h"
#include "game_packet_utils.h"
#include "game_server_context.h"

int handle_delete_item(ClientSession* s, const uint8_t* data, int len) {
    std::printf("[GAME] DELETE_ITEM from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 2) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 18, error, 4);
    }

    uint16_t slot;
    std::memcpy(&slot, data, 2);
    std::printf("[GAME] DELETE_ITEM: slot=%u\n", slot);

    db_delete_item_at_slot(s->character_id, slot);

    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 18, response, 4);
}

int handle_move_item(ClientSession* s, const uint8_t* data, int len) {
    std::printf("[GAME] MOVE_ITEM from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 19, error, 4);
    }

    uint16_t from_slot, to_slot;
    std::memcpy(&from_slot, data, 2);
    std::memcpy(&to_slot, data + 2, 2);

    std::printf("[GAME] MOVE_ITEM: from=%u to=%u\n", from_slot, to_slot);

    if (from_slot != to_slot) {
        DBItemInfo from_item;
        if (db_get_item_at_slot(s->character_id, from_slot, &from_item) < 0 || from_item.ui_id == 0) {
            std::printf("[MOVE] No item found at slot %u for character %u\n",
                        from_slot, s->character_id);
        } else {
            DBItemInfo to_item;
            bool has_target = (db_get_item_at_slot(s->character_id, to_slot, &to_item) == 0 &&
                               to_item.ui_id != 0);
            if (has_target) {
                int temp_slot = 9999;
                db_update_item_slot(s->character_id, to_item.ui_id, temp_slot);
                db_update_item_slot(s->character_id, from_item.ui_id, to_slot);
                db_update_item_slot(s->character_id, to_item.ui_id, from_slot);
                std::printf("[MOVE] Swapped items between slots %u and %u\n", from_slot, to_slot);
            } else {
                db_update_item_slot(s->character_id, from_item.ui_id, to_slot);
                std::printf("[MOVE] Moved item from slot %u to %u\n", from_slot, to_slot);
            }
        }
    }

    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 19, response, 4);
}

int handle_sort_item(ClientSession* s, const uint8_t* data, int len) {
    (void)data;
    (void)len;
    std::printf("[GAME] SORT_ITEM from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 20, error, 4);
    }

    std::printf("[GAME] Sorting inventory for character %u\n", s->character_id);

    uint8_t response[4] = {0, 0, 0, 0};
    return send_response(s, 20, response, 4);
}

int handle_npc_shop_buy(ClientSession* s, const uint8_t* data, int len) {
    std::printf("[GAME] NPC_SHOP_BUY from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 8) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 36, error, 4);
    }

    uint32_t npc_id, item_id;
    uint16_t count;
    std::memcpy(&npc_id, data, 4);
    std::memcpy(&item_id, data + 4, 4);
    if (len >= 10) {
        std::memcpy(&count, data + 8, 2);
    } else {
        count = 1;
    }

    std::printf("[GAME] NPC_SHOP_BUY: npc=%u item=%u count=%u\n", npc_id, item_id, count);

    uint32_t price_per_item = (item_id % 10000) * 10;
    if (price_per_item == 0) price_per_item = 100;
    uint32_t total_price = price_per_item * count;

    uint32_t current_gold = s->inventory.money;
    if (current_gold < total_price) {
        std::printf("[GAME] NPC_SHOP_BUY: Not enough gold! have=%u need=%u\n", current_gold, total_price);
        uint8_t error[4] = {3, 0, 0, 0};
        return send_response(s, 36, error, 4);
    }

    int free_slot = db_find_free_slot(s->character_id, MAX_EQUIP_SLOTS);
    if (free_slot < 0 || free_slot >= 200) {
        std::printf("[GAME] NPC_SHOP_BUY: No free slot!\n");
        uint8_t error[4] = {4, 0, 0, 0};
        return send_response(s, 36, error, 4);
    }

    uint32_t new_gold = current_gold - total_price;
    if (db_update_gold(s->character_id, new_gold) < 0) {
        std::printf("[GAME] NPC_SHOP_BUY: Failed to update gold in DB!\n");
        uint8_t error[4] = {5, 0, 0, 0};
        return send_response(s, 36, error, 4);
    }
    s->inventory.money = new_gold;

    int new_ui_id = db_add_item(s->character_id, item_id, free_slot, 0);
    if (new_ui_id < 0) {
        db_update_gold(s->character_id, current_gold);
        s->inventory.money = current_gold;
        std::printf("[GAME] NPC_SHOP_BUY: Failed to add item, gold rolled back!\n");
        uint8_t error[4] = {5, 0, 0, 0};
        return send_response(s, 36, error, 4);
    }

    std::printf("[GAME] NPC_SHOP_BUY: SUCCESS! item=%u, slot=%d, price=%u, new_gold=%u\n",
               item_id, free_slot, total_price, new_gold);

    uint8_t response[16];
    std::memset(response, 0, sizeof(response));
    uint32_t result = 0;
    std::memcpy(response, &result, 4);
    std::memcpy(response + 4, &new_gold, 4);
    uint16_t slot16 = (uint16_t)free_slot;
    std::memcpy(response + 8, &slot16, 2);
    std::memcpy(response + 10, &item_id, 4);

    return send_response(s, 14, response, 14);
}

int handle_npc_shop_sell(ClientSession* s, const uint8_t* data, int len) {
    std::printf("[GAME] NPC_SHOP_SELL from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 37, error, 4);
    }

    uint16_t slot, count;
    std::memcpy(&slot, data, 2);
    if (len >= 4) {
        std::memcpy(&count, data + 2, 2);
    } else {
        count = 1;
    }

    std::printf("[GAME] NPC_SHOP_SELL: slot=%u count=%u\n", slot, count);

    uint32_t item_id = 0;
    for (uint16_t i = 0; i < s->inventory.item_count; i++) {
        if (s->inventory.items[i].slot == slot) {
            item_id = s->inventory.items[i].it_id;
            break;
        }
    }

    if (item_id == 0) {
        std::printf("[GAME] NPC_SHOP_SELL: Item not found in slot %u\n", slot);
        uint8_t error[4] = {2, 0, 0, 0};
        return send_response(s, 37, error, 4);
    }

    uint32_t sell_price = ((item_id % 10000) * 10) / 2;
    if (sell_price == 0) sell_price = 50;
    uint32_t total_price = sell_price * count;

    if (db_delete_item_at_slot(s->character_id, slot) < 0) {
        std::printf("[GAME] NPC_SHOP_SELL: Failed to delete item from DB!\n");
        uint8_t error[4] = {5, 0, 0, 0};
        return send_response(s, 37, error, 4);
    }

    uint32_t current_gold = s->inventory.money;
    uint32_t new_gold = current_gold + total_price;
    if (db_update_gold(s->character_id, new_gold) < 0) {
        std::printf("[GAME] NPC_SHOP_SELL: Failed to update gold!\n");
    }
    s->inventory.money = new_gold;

    for (uint16_t i = 0; i < s->inventory.item_count; i++) {
        if (s->inventory.items[i].slot == slot) {
            for (uint16_t j = i; j < s->inventory.item_count - 1; j++) {
                s->inventory.items[j] = s->inventory.items[j + 1];
            }
            s->inventory.item_count--;
            break;
        }
    }

    std::printf("[GAME] NPC_SHOP_SELL: SUCCESS! item=%u sold for %u, new_gold=%u\n",
               item_id, total_price, new_gold);

    uint8_t response[12];
    std::memset(response, 0, sizeof(response));
    uint32_t result = 0;
    std::memcpy(response, &result, 4);
    std::memcpy(response + 4, &new_gold, 4);

    return send_response(s, 14, response, 12);
}

int handle_repair_equipment(ClientSession* s, const uint8_t* data, int len) {
    std::printf("[GAME] REPAIR_EQUIPMENT from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 3) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 25, error, 4);
    }

    uint16_t slot;
    std::memcpy(&slot, data, 2);
    uint8_t repair_all = data[2];

    std::printf("[GAME] REPAIR_EQUIPMENT: slot=%u repair_all=%u\n", slot, repair_all);

    uint32_t total_cost = 0;
    int repaired_count = 0;

    if (repair_all) {
        for (int i = 0; i <= 12; i++) {
            DBItemInfo item;
            if (db_get_item_info_at_slot(s->character_id, i, &item) == 0) {
                if (item.it_id > 0 && item.stat < 100) {
                    total_cost += calc_repair_cost(item.it_id, item.stat);
                }
            }
        }

        if (s->inventory.money < total_cost) {
            uint8_t response[4] = {2, 0, 0, 0};
            return send_response(s, 25, response, 4);
        }

        if (total_cost > 0) {
            s->inventory.money -= total_cost;
            db_update_gold(s->character_id, s->inventory.money);
        }
        repaired_count = db_repair_all_items(s->character_id);
    } else {
        DBItemInfo item;
        if (db_get_item_info_at_slot(s->character_id, slot, &item) == 0) {
            if (item.it_id > 0 && item.stat < 100) {
                total_cost = calc_repair_cost(item.it_id, item.stat);

                if (s->inventory.money < total_cost) {
                    uint8_t response[4] = {2, 0, 0, 0};
                    return send_response(s, 25, response, 4);
                }

                s->inventory.money -= total_cost;
                db_update_gold(s->character_id, s->inventory.money);
                if (db_repair_item(s->character_id, slot) == 0) {
                    repaired_count = 1;
                }
            }
        }
    }

    uint8_t response[12];
    std::memset(response, 0, 12);
    std::memcpy(response + 4, &repaired_count, 4);
    std::memcpy(response + 8, &total_cost, 4);

    std::printf("[GAME] REPAIR_EQUIPMENT: repaired=%d cost=%u gold_left=%u\n",
               repaired_count, total_cost, s->inventory.money);

    return send_response(s, 25, response, 12);
}

int handle_compound_item(ClientSession* s, const uint8_t* data, int len) {
    std::printf("[GAME] COMPOUND_ITEM from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 8) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 27, error, 4);
    }

    uint32_t recipe_id;
    uint32_t material_count;
    std::memcpy(&recipe_id, data, 4);
    std::memcpy(&material_count, data + 4, 4);

    std::printf("[GAME] COMPOUND_ITEM: recipe=%u, materials=%u\n", recipe_id, material_count);

    uint8_t response[24];
    std::memset(response, 0, sizeof(response));

    if (material_count > 8 || len < (int)(8 + material_count * 8)) {
        uint32_t result = 2;
        std::memcpy(response, &result, 4);
        return send_response(s, 27, response, 4);
    }

    uint32_t output_item_id = 0;
    uint32_t gold_cost = 0;

    switch (recipe_id) {
        case 1:
            output_item_id = 2001;
            gold_cost = 100;
            break;
        case 2:
            output_item_id = 2002;
            gold_cost = 100;
            break;
        case 3:
            output_item_id = 3001 + (recipe_id % 100);
            gold_cost = 500;
            break;
        default:
            output_item_id = recipe_id + 10000;
            gold_cost = 200;
            break;
    }

    if (s->inventory.money < gold_cost) {
        uint32_t result = 3;
        std::memcpy(response, &result, 4);
        return send_response(s, 27, response, 4);
    }

    for (uint32_t i = 0; i < material_count; i++) {
        uint32_t slot;
        std::memcpy(&slot, data + 8 + i * 8, 4);
        if (db_delete_item_at_slot(s->character_id, slot) < 0) {
            std::printf("[GAME] COMPOUND_ITEM: failed to consume material at slot %u\n", slot);
        }
    }

    s->inventory.money -= gold_cost;
    db_update_gold(s->character_id, s->inventory.money);

    int free_slot = db_find_free_slot(s->character_id, 100);
    if (free_slot < 0) {
        uint32_t result = 4;
        std::memcpy(response, &result, 4);
        return send_response(s, 27, response, 4);
    }

    if (db_add_item(s->character_id, output_item_id, free_slot, 0) < 0) {
        uint32_t result = 1;
        std::memcpy(response, &result, 4);
        return send_response(s, 27, response, 4);
    }

    uint32_t result = 0;
    std::memcpy(response, &result, 4);
    std::memcpy(response + 4, &output_item_id, 4);
    std::memcpy(response + 8, &free_slot, 4);
    std::memcpy(response + 12, &gold_cost, 4);

    std::printf("[GAME] COMPOUND_ITEM success: output=%u at slot=%d, cost=%u gold\n",
               output_item_id, free_slot, gold_cost);
    return send_response(s, 27, response, 16);
}

int handle_disjoint_item(ClientSession* s, const uint8_t* data, int len) {
    std::printf("[GAME] DISJOINT_ITEM from fd=%d\n", s->fd);

    if (s->state < STATE_IN_GAME || len < 4) {
        uint8_t error[4] = {1, 0, 0, 0};
        return send_response(s, 28, error, 4);
    }

    uint32_t slot;
    std::memcpy(&slot, data, 4);
    std::printf("[GAME] DISJOINT_ITEM: slot=%u\n", slot);

    uint8_t response[32];
    std::memset(response, 0, sizeof(response));

    DBItemInfo item;
    if (db_get_item_at_slot(s->character_id, slot, &item) < 0 || item.it_id == 0) {
        uint32_t result = 2;
        std::memcpy(response, &result, 4);
        return send_response(s, 28, response, 4);
    }

    uint32_t gold_cost = 50;
    uint32_t material_id = 0;
    uint32_t material_count = 1;

    uint32_t item_id = item.it_id;
    if (slot < 11) {
        material_id = 4001;
        material_count = 1;
    } else if (item_id >= 2000 && item_id < 3000) {
        uint32_t result = 3;
        std::memcpy(response, &result, 4);
        return send_response(s, 28, response, 4);
    } else {
        material_id = 4000 + (item_id % 10);
        material_count = (item_id % 3) + 1;
    }

    if (s->inventory.money < gold_cost) {
        uint32_t result = 4;
        std::memcpy(response, &result, 4);
        return send_response(s, 28, response, 4);
    }

    if (db_delete_item(s->character_id, item.ui_id) < 0) {
        uint32_t result = 1;
        std::memcpy(response, &result, 4);
        return send_response(s, 28, response, 4);
    }

    s->inventory.money -= gold_cost;
    db_update_gold(s->character_id, s->inventory.money);

    int32_t reward_slot = -1;
    uint16_t remaining = static_cast<uint16_t>(material_count);
    int stacked = db_try_stack_item(s->character_id, material_id,
                                    remaining, &reward_slot);
    if (stacked < 0) {
        uint32_t result = 5;
        std::memcpy(response, &result, 4);
        return send_response(s, 28, response, 4);
    }

    if (stacked > 0) {
        if (stacked > remaining) stacked = remaining;
        remaining = static_cast<uint16_t>(remaining - stacked);
    }

    if (remaining > 0) {
        int free_slot = db_find_free_slot(s->character_id, MAX_EQUIP_SLOTS);
        if (free_slot < 0 ||
            db_add_item(s->character_id, material_id, free_slot, remaining) < 0) {
            uint32_t result = 6;
            std::memcpy(response, &result, 4);
            return send_response(s, 28, response, 4);
        }
        reward_slot = free_slot;
    }

    uint32_t result = 0;
    std::memcpy(response, &result, 4);
    std::memcpy(response + 4, &material_id, 4);
    std::memcpy(response + 8, &material_count, 4);
    std::memcpy(response + 12, &gold_cost, 4);
    std::memcpy(response + 16, &reward_slot, 4);

    std::printf("[GAME] DISJOINT_ITEM success: item %u -> material %u x%u, cost=%u gold\n",
               item_id, material_id, material_count, gold_cost);
    return send_response(s, 28, response, 20);
}
