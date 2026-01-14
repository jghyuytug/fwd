#ifndef LIMITED_SHOP_MANAGER_ORM_H
#define LIMITED_SHOP_MANAGER_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int occ_time;
    signed char server_id;
    int ipg_no;
    int item_no;
    int item_cnt;
    int cera_price;
    int gold_price;
    signed char avatar_period_type;
    int total_cnt;
    int sell_cnt;
    int restrict_no;
    int start_time;
    int end_time;
    int real_end_time;
    int npc_idx;
    signed char cond_charac_job;
    signed char cond_lev_begin;
    signed char cond_lev_end;
    int cond_acc_create_time_begin;
    int cond_acc_create_time_end;
    int cond_cha_create_time_begin;
    int cond_cha_create_time_end;
    signed char status_flag;
    char title[51];
    signed char range_section;
    char reason_etc[201];
    char reason_stop[201];
    char pos_flag[2];
} LimitedShopManager;

/* CRUD Operations */
int LimitedShopManager_Add(DBConnectionManager* manager, const LimitedShopManager* record);
int LimitedShopManager_Get(DBConnectionManager* manager, int no, signed char server_id, int ipg_no, LimitedShopManager* record);
int LimitedShopManager_Update(DBConnectionManager* manager, const LimitedShopManager* record);
int LimitedShopManager_Delete(DBConnectionManager* manager, int no, signed char server_id, int ipg_no);
int LimitedShopManager_Exists(DBConnectionManager* manager, int no, signed char server_id, int ipg_no);
int LimitedShopManager_GetAll(DBConnectionManager* manager, LimitedShopManager* records, int max_count, int* actual_count);

#endif /* LIMITED_SHOP_MANAGER_ORM_H */