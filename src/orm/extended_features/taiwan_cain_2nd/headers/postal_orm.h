#ifndef POSTAL_ORM_H
#define POSTAL_ORM_H

#include "db_connection_manager.h"

typedef struct {
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
} Postal;

/* CRUD Operations */
int Postal_Add(DBConnectionManager* manager, const Postal* record);
int Postal_Get(DBConnectionManager* manager, int postal_id, Postal* record);
int Postal_Update(DBConnectionManager* manager, const Postal* record);
int Postal_Delete(DBConnectionManager* manager, int postal_id);
int Postal_Exists(DBConnectionManager* manager, int postal_id);
int Postal_GetAll(DBConnectionManager* manager, Postal* records, int max_count, int* actual_count);

#endif /* POSTAL_ORM_H */