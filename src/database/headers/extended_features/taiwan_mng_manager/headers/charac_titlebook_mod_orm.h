#ifndef CHARAC_TITLEBOOK_MOD_ORM_H
#define CHARAC_TITLEBOOK_MOD_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int id;
    signed char server_info;
    int charac_no;
    char MNG_user_id[31];
    char work_time[20];
    char work_type;
    char memo[256];
} CharacTitlebookMod;

/* CRUD Operations */
int CharacTitlebookMod_Add(DBConnectionManager* manager, const CharacTitlebookMod* record);
int CharacTitlebookMod_Get(DBConnectionManager* manager, int id, CharacTitlebookMod* record);
int CharacTitlebookMod_Update(DBConnectionManager* manager, const CharacTitlebookMod* record);
int CharacTitlebookMod_Delete(DBConnectionManager* manager, int id);
int CharacTitlebookMod_Exists(DBConnectionManager* manager, int id);
int CharacTitlebookMod_GetAll(DBConnectionManager* manager, CharacTitlebookMod* records, int max_count, int* actual_count);

#endif /* CHARAC_TITLEBOOK_MOD_ORM_H */