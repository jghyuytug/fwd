#ifndef EQUIP_MAPPING_INFO_ORM_H
#define EQUIP_MAPPING_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int equip_idx;
    int mapping_idx;
} EquipMappingInfo;

/* CRUD Operations */
int EquipMappingInfo_Add(DBConnectionManager* manager, const EquipMappingInfo* record);
int EquipMappingInfo_Get(DBConnectionManager* manager, int equip_idx, EquipMappingInfo* record);
int EquipMappingInfo_Update(DBConnectionManager* manager, const EquipMappingInfo* record);
int EquipMappingInfo_Delete(DBConnectionManager* manager, int equip_idx);
int EquipMappingInfo_Exists(DBConnectionManager* manager, int equip_idx);
int EquipMappingInfo_GetAll(DBConnectionManager* manager, EquipMappingInfo* records, int max_count, int* actual_count);

#endif /* EQUIP_MAPPING_INFO_ORM_H */