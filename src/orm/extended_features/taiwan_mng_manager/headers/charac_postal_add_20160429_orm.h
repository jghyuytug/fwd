#ifndef CHARAC_POSTAL_ADD_20160429_ORM_H
#define CHARAC_POSTAL_ADD_20160429_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int id;
    signed char server_info;
    int postal_id;
    int send_charac_no;
    char send_charac_name[21];
    int receive_charac_no;
    short item_id;
    int add_info;
    short endurance;
    int extend_info;
    char random_option[15];
    signed char seperate_upgrade;
    signed char upgrade;
    int gold;
    signed char avata_flag;
    signed char unlimit_flag;
    signed char seal_flag;
    signed char creature_flag;
    int creature_stomach;
    int creature_exp;
    signed char creature_no_charge;
    char avatar_expire_date[20];
    signed char avatar_ability_no;
    unsigned char avatar_jewel_socket[30];
    char MNG_user_id[31];
    char work_time[20];
    char memo[256];
    int group_id;
    signed char send_flag;
} CharacPostalAdd20160429;

/* CRUD Operations */
int CharacPostalAdd20160429_Add(DBConnectionManager* manager, const CharacPostalAdd20160429* record);
int CharacPostalAdd20160429_Get(DBConnectionManager* manager, int id, CharacPostalAdd20160429* record);
int CharacPostalAdd20160429_Update(DBConnectionManager* manager, const CharacPostalAdd20160429* record);
int CharacPostalAdd20160429_Delete(DBConnectionManager* manager, int id);
int CharacPostalAdd20160429_Exists(DBConnectionManager* manager, int id);
int CharacPostalAdd20160429_GetAll(DBConnectionManager* manager, CharacPostalAdd20160429* records, int max_count, int* actual_count);

#endif /* CHARAC_POSTAL_ADD_20160429_ORM_H */