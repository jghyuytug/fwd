#ifndef LOG_PLAYTIME_ORM_H
#define LOG_PLAYTIME_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    int charac_no;
    int play_time;
    char lobby_intime[20];
    char lobby_outtime[20];
} LogPlaytime;

/* CRUD Operations */
int LogPlaytime_Add(DBConnectionManager* manager, const LogPlaytime* record);
int LogPlaytime_GetAll(DBConnectionManager* manager, LogPlaytime* records, int max_count, int* actual_count);

#endif /* LOG_PLAYTIME_ORM_H */