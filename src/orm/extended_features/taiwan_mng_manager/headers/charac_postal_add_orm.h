#ifndef CHARAC_POSTAL_ADD_ORM_H
#define CHARAC_POSTAL_ADD_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int id;
    signed char server_info;
    int postal_id;
    int send_charac_no;
    char send_charac_name[21];
    int receive_charac_no;
    int item_id;
    int add_info;
    short endurance;
    int extend_info;
    int extend_info_expand;
    char random_option[15];
    signed char seperate_upgrade;
    signed char upgrade;
    signed char amplify_option;
    int amplify_value;
    int gold;
    signed char avata_flag;
    signed char unlimit_flag;
    signed char seal_flag;
    signed char creature_flag;
    int letter_id;
    char MNG_user_id[31];
    char work_time[20];
    signed char delete_flag;
    char memo[256];
    int group_id;
    signed char send_flag;
    int creature_stomach;
    int creature_exp;
    signed char creature_no_charge;
    char avatar_expire_date[20];
    signed char avatar_ability_no;
    unsigned char avatar_jewel_socket[30];
    char proc_flag;
} CharacPostalAdd;

/* CRUD Operations */
int CharacPostalAdd_Add(DBConnectionManager* manager, const CharacPostalAdd* record);
int CharacPostalAdd_Get(DBConnectionManager* manager, int id, CharacPostalAdd* record);
int CharacPostalAdd_Update(DBConnectionManager* manager, const CharacPostalAdd* record);
int CharacPostalAdd_Delete(DBConnectionManager* manager, int id);
int CharacPostalAdd_Exists(DBConnectionManager* manager, int id);
int CharacPostalAdd_GetAll(DBConnectionManager* manager, CharacPostalAdd* records, int max_count, int* actual_count);

#endif /* CHARAC_POSTAL_ADD_ORM_H */