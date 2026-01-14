#ifndef HANDLER_INVENTORY_H
#define HANDLER_INVENTORY_H

#include <stdint.h>
#include "game_server_types.h"

#ifdef __cplusplus
extern "C" {
#endif

int handle_delete_item(ClientSession* s, const uint8_t* data, int len);
int handle_move_item(ClientSession* s, const uint8_t* data, int len);
int handle_sort_item(ClientSession* s, const uint8_t* data, int len);
int handle_npc_shop_buy(ClientSession* s, const uint8_t* data, int len);
int handle_npc_shop_sell(ClientSession* s, const uint8_t* data, int len);
int handle_repair_equipment(ClientSession* s, const uint8_t* data, int len);
int handle_trade_request(ClientSession* s, const uint8_t* data, int len);
int handle_compound_item(ClientSession* s, const uint8_t* data, int len);
int handle_disjoint_item(ClientSession* s, const uint8_t* data, int len);

#ifdef __cplusplus
}
#endif

#endif /* HANDLER_INVENTORY_H */
