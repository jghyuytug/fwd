#ifndef GUARDER_INFO_ORM_H
#define GUARDER_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    char g_name[11];
    char g_birth[9];
    char g_phone[16];
    signed char g_confirm;
} GuarderInfo;

/* CRUD Operations */
int GuarderInfo_Add(DBConnectionManager* manager, const GuarderInfo* record);
int GuarderInfo_Get(DBConnectionManager* manager, int m_id, GuarderInfo* record);
int GuarderInfo_Update(DBConnectionManager* manager, const GuarderInfo* record);
int GuarderInfo_Delete(DBConnectionManager* manager, int m_id);
int GuarderInfo_Exists(DBConnectionManager* manager, int m_id);
int GuarderInfo_GetAll(DBConnectionManager* manager, GuarderInfo* records, int max_count, int* actual_count);

#endif /* GUARDER_INFO_ORM_H */