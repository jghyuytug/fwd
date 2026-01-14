#ifndef LOG_MERCENARY_ORM_H
#define LOG_MERCENARY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int charac_no;
    char start_time[20];
    char finish_time[20];
    signed char type;
    signed char period;
    signed char area;
    int gold;
    int item_id;
    int rarity;
} LogMercenary;

/* CRUD Operations */
int LogMercenary_Add(DBConnectionManager* manager, const LogMercenary* record);
int LogMercenary_GetAll(DBConnectionManager* manager, LogMercenary* records, int max_count, int* actual_count);

#endif /* LOG_MERCENARY_ORM_H */