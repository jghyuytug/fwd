#ifndef PROD_MOD_HISTORY_ORM_H
#define PROD_MOD_HISTORY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char mod_time[20];
    short prod_part_id;
    int ipg_no;
    short prod_step;
    int it_no;
    char prod_name[36];
    signed char master_type;
    short sub_type;
    char job[26];
    signed char class;
    signed char set_flag;
    signed char sell_type;
    signed char prod_stat;
    short prod_gold;
    short prod_winpoint;
    short prod_price;
    short prod_cnt;
    int prod_supply;
    signed char public_flag;
    signed char prod_limit_flag;
    short prod_period;
    char prod_icon[51];
    short bonus_weight;
    short bonus_rate;
    signed char bonus_flag;
    char prod_explain[251];
    signed char mod_type;
    char mod_user_id[26];
    char mod_explain[251];
} ProdModHistory;

/* CRUD Operations */
int ProdModHistory_Add(DBConnectionManager* manager, const ProdModHistory* record);
int ProdModHistory_Get(DBConnectionManager* manager, const char* mod_time, short prod_part_id, int ipg_no, ProdModHistory* record);
int ProdModHistory_Update(DBConnectionManager* manager, const ProdModHistory* record);
int ProdModHistory_Delete(DBConnectionManager* manager, const char* mod_time, short prod_part_id, int ipg_no);
int ProdModHistory_Exists(DBConnectionManager* manager, const char* mod_time, short prod_part_id, int ipg_no);
int ProdModHistory_GetAll(DBConnectionManager* manager, ProdModHistory* records, int max_count, int* actual_count);

#endif /* PROD_MOD_HISTORY_ORM_H */