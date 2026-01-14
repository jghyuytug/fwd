#ifndef CHARAC_POSTAL_ADD_LARGE_ORM_H
#define CHARAC_POSTAL_ADD_LARGE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int group_id;
    char MNG_user_id[31];
    char work_time[20];
    char memo[256];
    signed char end_flag;
} CharacPostalAddLarge;

/* CRUD Operations */
int CharacPostalAddLarge_Add(DBConnectionManager* manager, const CharacPostalAddLarge* record);
int CharacPostalAddLarge_Get(DBConnectionManager* manager, int group_id, CharacPostalAddLarge* record);
int CharacPostalAddLarge_Update(DBConnectionManager* manager, const CharacPostalAddLarge* record);
int CharacPostalAddLarge_Delete(DBConnectionManager* manager, int group_id);
int CharacPostalAddLarge_Exists(DBConnectionManager* manager, int group_id);
int CharacPostalAddLarge_GetAll(DBConnectionManager* manager, CharacPostalAddLarge* records, int max_count, int* actual_count);

#endif /* CHARAC_POSTAL_ADD_LARGE_ORM_H */