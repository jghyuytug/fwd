#ifndef GROUP_BLACKLIST_ORM_H
#define GROUP_BLACKLIST_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int Id;
    int QQGroupNumber;
    int Uin;
} GroupBlacklist;

/* CRUD Operations */
int GroupBlacklist_Add(DBConnectionManager* manager, const GroupBlacklist* record);
int GroupBlacklist_Get(DBConnectionManager* manager, int Id, GroupBlacklist* record);
int GroupBlacklist_Update(DBConnectionManager* manager, const GroupBlacklist* record);
int GroupBlacklist_Delete(DBConnectionManager* manager, int Id);
int GroupBlacklist_Exists(DBConnectionManager* manager, int Id);
int GroupBlacklist_GetAll(DBConnectionManager* manager, GroupBlacklist* records, int max_count, int* actual_count);

#endif /* GROUP_BLACKLIST_ORM_H */