#ifndef LOG_3MONTH_STOP_WEB_ORM_H
#define LOG_3MONTH_STOP_WEB_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_time[20];
    int m_id;
    signed char is_complete;
    signed char is_confirm;
    signed char server_no;
} Log3MonthStopWeb;

/* CRUD Operations */
int Log3MonthStopWeb_Add(DBConnectionManager* manager, const Log3MonthStopWeb* record);
int Log3MonthStopWeb_Get(DBConnectionManager* manager, const char* occ_time, int m_id, Log3MonthStopWeb* record);
int Log3MonthStopWeb_Update(DBConnectionManager* manager, const Log3MonthStopWeb* record);
int Log3MonthStopWeb_Delete(DBConnectionManager* manager, const char* occ_time, int m_id);
int Log3MonthStopWeb_Exists(DBConnectionManager* manager, const char* occ_time, int m_id);
int Log3MonthStopWeb_GetAll(DBConnectionManager* manager, Log3MonthStopWeb* records, int max_count, int* actual_count);

#endif /* LOG_3MONTH_STOP_WEB_ORM_H */