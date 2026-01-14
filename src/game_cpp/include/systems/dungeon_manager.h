#ifndef SYSTEMS_DUNGEON_MANAGER_H
#define SYSTEMS_DUNGEON_MANAGER_H

#include <stdint.h>
#include "game_server_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int attacks_count;
    uint32_t total_damage;
    uint8_t  debuff_applied;
    uint16_t debuff_type;
    uint16_t debuff_duration;
} DungeonTickResult;

typedef enum {
    RESURRECT_OK = 0,
    RESURRECT_NO_COINS = 1,
    RESURRECT_DUNGEON_FAILED = 2,
    RESURRECT_ALREADY_ALIVE = 3
} ResurrectResult;

int player_is_dead(ClientSession* s);
void player_die(ClientSession* s);
ResurrectResult player_resurrect(ClientSession* s, DungeonInstance* d,
                                 uint8_t hp_percent);
int player_take_damage(ClientSession* s, DungeonInstance* d,
                       uint32_t damage, uint8_t* player_died);
int dungeon_room_cleared(DungeonInstance* d);
DungeonTickResult dungeon_tick_monsters_v2(DungeonInstance* d, ClientSession* s,
                                           uint16_t player_x, uint16_t player_y);

#ifdef __cplusplus
}
#endif

#endif /* SYSTEMS_DUNGEON_MANAGER_H */
