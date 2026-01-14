#ifndef EVENT_PANDORA_ENTRY_200905_ORM_H
#define EVENT_PANDORA_ENTRY_200905_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned int m_id;
    char occ_date[11];           /* DATE format: YYYY-MM-DD */
    unsigned char server_id;
    unsigned int charac_no;
} EventPandoraEntry200905;

/* CRUD operations - composite primary key (m_id, occ_date, server_id) */
int EventPandoraEntry200905_Add(DBConnectionManager* manager, const EventPandoraEntry200905* entry);
int EventPandoraEntry200905_Get(DBConnectionManager* manager, unsigned int m_id, const char* occ_date,
                                 unsigned char server_id, EventPandoraEntry200905* entry);
int EventPandoraEntry200905_Update(DBConnectionManager* manager, const EventPandoraEntry200905* entry);
int EventPandoraEntry200905_Delete(DBConnectionManager* manager, unsigned int m_id, const char* occ_date,
                                    unsigned char server_id);
int EventPandoraEntry200905_Exists(DBConnectionManager* manager, unsigned int m_id, const char* occ_date,
                                    unsigned char server_id);

/* Business operations */
int EventPandoraEntry200905_GetByMember(DBConnectionManager* manager, unsigned int m_id,
                                         EventPandoraEntry200905* entries,
                                         int max_count, int* actual_count);
int EventPandoraEntry200905_GetByDate(DBConnectionManager* manager, const char* occ_date,
                                       EventPandoraEntry200905* entries,
                                       int max_count, int* actual_count);
int EventPandoraEntry200905_GetByCharac(DBConnectionManager* manager, unsigned char server_id,
                                         unsigned int charac_no, EventPandoraEntry200905* entries,
                                         int max_count, int* actual_count);
int EventPandoraEntry200905_DeleteByMember(DBConnectionManager* manager, unsigned int m_id);

/* Utility */
void EventPandoraEntry200905_PrintInfo(const EventPandoraEntry200905* entry);

#endif
