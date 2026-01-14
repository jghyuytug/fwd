#ifndef PROD_INFO_ORM_H
#define PROD_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
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
    char regist_user_id[26];
    char regist_time[20];
    char mod_user_id[26];
    char mod_time[20];
} ProdInfo;

/* CRUD Operations */
int ProdInfo_Add(DBConnectionManager* manager, const ProdInfo* record);
int ProdInfo_Get(DBConnectionManager* manager, int ipg_no, ProdInfo* record);
int ProdInfo_Update(DBConnectionManager* manager, const ProdInfo* record);
int ProdInfo_Delete(DBConnectionManager* manager, int ipg_no);
int ProdInfo_Exists(DBConnectionManager* manager, int ipg_no);
int ProdInfo_GetAll(DBConnectionManager* manager, ProdInfo* records, int max_count, int* actual_count);

#endif /* PROD_INFO_ORM_H */