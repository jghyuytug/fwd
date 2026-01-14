#ifndef PROD_SALE_ENTRY_132_ORM_H
#define PROD_SALE_ENTRY_132_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char agency_no[21];
    int m_id;
    int charac_no;
    signed char job;
    signed char grow_type;
    signed char level;
    int ipg_no;
    int price;
    signed char buy_type;
    signed char gift_server_info;
    int gift_get_charac_no;
    char sale_time[20];
    int charge_price;
    int free_price;
    short count_no;
    int each_price;
    signed char price_type;
    signed char seal_flag;
    signed char slot_part_type;
    short slot_part_code;
    signed char buy_method_type;
    short prod_part_id;
    int bonus_ipg_no;
    char user_id[26];
    signed char server_id;
    signed char prize_flag;
    signed char new_flag;
    char result_code[5];
    char result_message[101];
    char user_ip[17];
    long long tran_id;
} ProdSaleEntry132;

/* CRUD Operations */
int ProdSaleEntry132_Add(DBConnectionManager* manager, const ProdSaleEntry132* record);
int ProdSaleEntry132_Get(DBConnectionManager* manager, const char* agency_no, ProdSaleEntry132* record);
int ProdSaleEntry132_Update(DBConnectionManager* manager, const ProdSaleEntry132* record);
int ProdSaleEntry132_Delete(DBConnectionManager* manager, const char* agency_no);
int ProdSaleEntry132_Exists(DBConnectionManager* manager, const char* agency_no);
int ProdSaleEntry132_GetAll(DBConnectionManager* manager, ProdSaleEntry132* records, int max_count, int* actual_count);

#endif /* PROD_SALE_ENTRY_132_ORM_H */