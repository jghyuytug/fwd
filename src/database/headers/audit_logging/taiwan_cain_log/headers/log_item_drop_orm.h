#ifndef LOG_ITEM_DROP_ORM_H
#define LOG_ITEM_DROP_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_date[11];
    short channel_no;
    signed char drop_type;
    int stackable_common;
    int stackable_uncommon;
    int stackable_rare;
    int stackable_unique;
    int stackable_epic;
    int equip_common;
    int equip_uncommon;
    int equip_rare;
    int equip_unique;
    int equip_epic;
    int recipe_common;
    int recipe_uncommon;
    int recipe_rare;
    int recipe_unique;
    int recipe_epic;
    int artifact_common;
    int artifact_uncommon;
    int artifact_rare;
    int artifact_unique;
    int artifact_epic;
    int equip_chronicle;
    int recipe_chronicle;
    int artifact_chronicle;
    int Stackable_chronicle;
} LogItemDrop;

/* CRUD Operations */
int LogItemDrop_Add(DBConnectionManager* manager, const LogItemDrop* record);
int LogItemDrop_Get(DBConnectionManager* manager, const char* occ_date, short channel_no, signed char drop_type, LogItemDrop* record);
int LogItemDrop_Update(DBConnectionManager* manager, const LogItemDrop* record);
int LogItemDrop_Delete(DBConnectionManager* manager, const char* occ_date, short channel_no, signed char drop_type);
int LogItemDrop_Exists(DBConnectionManager* manager, const char* occ_date, short channel_no, signed char drop_type);
int LogItemDrop_GetAll(DBConnectionManager* manager, LogItemDrop* records, int max_count, int* actual_count);

#endif /* LOG_ITEM_DROP_ORM_H */