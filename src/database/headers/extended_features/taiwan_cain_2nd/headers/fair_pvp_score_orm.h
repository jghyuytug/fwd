#ifndef FAIR_PVP_SCORE_ORM_H
#define FAIR_PVP_SCORE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int charac_no;
    int private_win;
    int private_lose;
    int private_draw;
    int relay_battle_win;
    int relay_battle_lose;
    int relay_battle_draw;
    int relay_battle_2kill;
    int successive_win;
    int relay_battle_3kill;
    int max_successive_win;
    int daily_play_count;
    char last_play_time[20];
    signed char give_item;
} FairPvpScore;

/* CRUD Operations */
int FairPvpScore_Add(DBConnectionManager* manager, const FairPvpScore* record);
int FairPvpScore_Get(DBConnectionManager* manager, int charac_no, FairPvpScore* record);
int FairPvpScore_Update(DBConnectionManager* manager, const FairPvpScore* record);
int FairPvpScore_Delete(DBConnectionManager* manager, int charac_no);
int FairPvpScore_Exists(DBConnectionManager* manager, int charac_no);
int FairPvpScore_GetAll(DBConnectionManager* manager, FairPvpScore* records, int max_count, int* actual_count);

#endif /* FAIR_PVP_SCORE_ORM_H */