#ifndef EVENT_MAGE_2YEARS_ORM_H
#define EVENT_MAGE_2YEARS_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    unsigned char server_info;
    int charac_no;
    char charac_name[101];
    char create_time[20];
    char delete_time[20];
    char delete_flag;
} EventMage2years;

/* CRUD Operations */
int EventMage2years_Add(DBConnectionManager* manager, const EventMage2years* record);
int EventMage2years_Get(DBConnectionManager* manager, int m_id, unsigned char server_info,
                         int charac_no, EventMage2years* record);
int EventMage2years_Update(DBConnectionManager* manager, const EventMage2years* record);
int EventMage2years_Delete(DBConnectionManager* manager, int m_id, unsigned char server_info,
                            int charac_no);
int EventMage2years_Exists(DBConnectionManager* manager, int m_id, unsigned char server_info,
                            int charac_no);

/* Business Operations */
int EventMage2years_GetByMemberId(DBConnectionManager* manager, int m_id,
                                   EventMage2years* records, int max_count, int* actual_count);
int EventMage2years_GetByServerInfo(DBConnectionManager* manager, unsigned char server_info,
                                     EventMage2years* records, int max_count, int* actual_count);
int EventMage2years_GetActiveCharacters(DBConnectionManager* manager,
                                         EventMage2years* records, int max_count, int* actual_count);
int EventMage2years_SoftDelete(DBConnectionManager* manager, int m_id, unsigned char server_info,
                                int charac_no, const char* delete_time);

/* Utility Functions */
void EventMage2years_PrintInfo(const EventMage2years* record);

#endif
