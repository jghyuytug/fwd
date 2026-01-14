#ifndef LOG_ASSAULT_ORM_H
#define LOG_ASSAULT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_date[11];
    short channel_no;
    int assault_count;
    int load_fail_count;
    int p2p_fail_count;
} LogAssault;

/* CRUD Operations */
int LogAssault_Add(DBConnectionManager* manager, const LogAssault* record);
int LogAssault_Get(DBConnectionManager* manager, const char* occ_date, short channel_no, LogAssault* record);
int LogAssault_Update(DBConnectionManager* manager, const LogAssault* record);
int LogAssault_Delete(DBConnectionManager* manager, const char* occ_date, short channel_no);
int LogAssault_Exists(DBConnectionManager* manager, const char* occ_date, short channel_no);
int LogAssault_GetAll(DBConnectionManager* manager, LogAssault* records, int max_count, int* actual_count);

#endif /* LOG_ASSAULT_ORM_H */