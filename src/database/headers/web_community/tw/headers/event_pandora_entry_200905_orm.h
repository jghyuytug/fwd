#ifndef EVENT_PANDORA_ENTRY_200905_ORM_H
#define EVENT_PANDORA_ENTRY_200905_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    char occ_date[11];
    signed char server_id;
    int charac_no;
} EventPandoraEntry200905;

/* CRUD Operations */
int EventPandoraEntry200905_Add(DBConnectionManager* manager, const EventPandoraEntry200905* record);
int EventPandoraEntry200905_Get(DBConnectionManager* manager, int m_id, const char* occ_date, signed char server_id, EventPandoraEntry200905* record);
int EventPandoraEntry200905_Update(DBConnectionManager* manager, const EventPandoraEntry200905* record);
int EventPandoraEntry200905_Delete(DBConnectionManager* manager, int m_id, const char* occ_date, signed char server_id);
int EventPandoraEntry200905_Exists(DBConnectionManager* manager, int m_id, const char* occ_date, signed char server_id);
int EventPandoraEntry200905_GetAll(DBConnectionManager* manager, EventPandoraEntry200905* records, int max_count, int* actual_count);

#endif /* EVENT_PANDORA_ENTRY_200905_ORM_H */