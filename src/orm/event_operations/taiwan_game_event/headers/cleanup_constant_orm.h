#ifndef CLEANUP_CONSTANT_ORM_H
#define CLEANUP_CONSTANT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    signed char limit_penalty;
    signed char base1_penalty;
    signed char base2_penalty;
    signed char base3_penalty;
    signed char base4_penalty;
    signed char person_trade;
    signed char person_shop_trade;
    signed char auction_trade;
    signed char mail_trade;
    int mail_min_gold;
    signed char abnormal_npc_trade;
    signed char abnormal_user_trade;
    signed char dungeon_clear;
    signed char removal_dungeon_clear;
    signed char penalty_user_trade;
    signed char penalty_ghost_clear_n;
    signed char penalty_ghost_clear_m;
    signed char penalty_ghost_clear_l;
    short pc_room_weight;
    short hps_ip_weight;
    short otm_weight_n;
    short otm_weight_m;
    short hack_weight;
    signed char pvp_penalty;
    signed char dungeon_clear_penalty;
    signed char quest_clear_penalty;
    signed char reduce_time_date;
    signed char reduce_time_penalty;
} CleanupConstant;

/* CRUD Operations */
int CleanupConstant_Add(DBConnectionManager* manager, const CleanupConstant* record);
int CleanupConstant_GetAll(DBConnectionManager* manager, CleanupConstant* records, int max_count, int* actual_count);

#endif /* CLEANUP_CONSTANT_ORM_H */