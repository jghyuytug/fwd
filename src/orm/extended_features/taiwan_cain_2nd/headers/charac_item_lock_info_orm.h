#ifndef CHARAC_ITEM_LOCK_INFO_ORM_H
#define CHARAC_ITEM_LOCK_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int charac_no;
} CharacItemLockInfo;

/* CRUD Operations */
int CharacItemLockInfo_Add(DBConnectionManager* manager, const CharacItemLockInfo* record);
int CharacItemLockInfo_Get(DBConnectionManager* manager, int charac_no, CharacItemLockInfo* record);
int CharacItemLockInfo_Update(DBConnectionManager* manager, const CharacItemLockInfo* record);
int CharacItemLockInfo_Delete(DBConnectionManager* manager, int charac_no);
int CharacItemLockInfo_Exists(DBConnectionManager* manager, int charac_no);
int CharacItemLockInfo_GetAll(DBConnectionManager* manager, CharacItemLockInfo* records, int max_count, int* actual_count);

#endif /* CHARAC_ITEM_LOCK_INFO_ORM_H */