#ifndef POSTAL_DEL_ORM_H
#define POSTAL_DEL_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char sdate[11];
    int postal_id;
    char occ_time[20];
    int send_charac_no;
    char send_charac_name[21];
    int receive_charac_no;
    int item_id;
    int add_info;
    short endurance;
    signed char upgrade;
    signed char amplify_option;
    int amplify_value;
    int gold;
    char receive_time[20];
    signed char delete_flag;
    signed char avata_flag;
    signed char unlimit_flag;
    signed char seal_flag;
    signed char creature_flag;
    int postal;
    int letter_id;
    int extend_info;
    signed char ipg_db_id;
    int ipg_transaction_id;
    char ipg_nexon_id[33];
    long long auction_id;
    unsigned char random_option[14];
    signed char seperate_upgrade;
    signed char type;
    unsigned char item_guid[10];
} PostalDel;

/* CRUD Operations */
int PostalDel_Add(DBConnectionManager* manager, const PostalDel* record);
int PostalDel_Get(DBConnectionManager* manager, const char* sdate, int postal_id, PostalDel* record);
int PostalDel_Update(DBConnectionManager* manager, const PostalDel* record);
int PostalDel_Delete(DBConnectionManager* manager, const char* sdate, int postal_id);
int PostalDel_Exists(DBConnectionManager* manager, const char* sdate, int postal_id);
int PostalDel_GetAll(DBConnectionManager* manager, PostalDel* records, int max_count, int* actual_count);

#endif /* POSTAL_DEL_ORM_H */