#ifndef CHARAC_BLACK_LIST_ORM_H
#define CHARAC_BLACK_LIST_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int charac_no;
    char charac_name[21];
    char occ_time[20];
} CharacBlackList;

/* CRUD Operations */
int CharacBlackList_Add(DBConnectionManager* manager, const CharacBlackList* record);
int CharacBlackList_Get(DBConnectionManager* manager, int m_id, int charac_no, CharacBlackList* record);
int CharacBlackList_Update(DBConnectionManager* manager, const CharacBlackList* record);
int CharacBlackList_Delete(DBConnectionManager* manager, int m_id, int charac_no);
int CharacBlackList_Exists(DBConnectionManager* manager, int m_id, int charac_no);
int CharacBlackList_GetAll(DBConnectionManager* manager, CharacBlackList* records, int max_count, int* actual_count);

#endif /* CHARAC_BLACK_LIST_ORM_H */