#ifndef LOG_ERROR_HISTORY_ORM_H
#define LOG_ERROR_HISTORY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned int no;            /* AUTO_INCREMENT primary key */
    int error_id;               /* Error ID */
    char error_msg[256];        /* VARCHAR(255) -> 256 */
    char error_query[513];      /* VARCHAR(512) -> 513 */
    char proc_name[46];         /* VARCHAR(45) -> 46 */
    int proc_line;              /* Procedure line number */
    char query_user[46];        /* VARCHAR(45) -> 46, default 'None' */
    char occ_date[20];          /* DATETIME */
} LogErrorHistory;

/* CRUD Operations */
int LogErrorHistory_Add(DBConnectionManager* manager, const LogErrorHistory* record);
int LogErrorHistory_Get(DBConnectionManager* manager, unsigned int no, LogErrorHistory* record);
int LogErrorHistory_Delete(DBConnectionManager* manager, unsigned int no);
int LogErrorHistory_Exists(DBConnectionManager* manager, unsigned int no);

/* Business Operations */
int LogErrorHistory_GetRecent(DBConnectionManager* manager, LogErrorHistory* records,
                               int max_count, int* actual_count);
int LogErrorHistory_GetByErrorId(DBConnectionManager* manager, int error_id,
                                  LogErrorHistory* records, int max_count, int* actual_count);
int LogErrorHistory_GetByProcName(DBConnectionManager* manager, const char* proc_name,
                                   LogErrorHistory* records, int max_count, int* actual_count);
int LogErrorHistory_GetByDateRange(DBConnectionManager* manager,
                                    const char* start_date, const char* end_date,
                                    LogErrorHistory* records, int max_count, int* actual_count);
int LogErrorHistory_DeleteOlderThan(DBConnectionManager* manager, const char* date);
int LogErrorHistory_GetCount(DBConnectionManager* manager, int* count);

/* Utility Functions */
void LogErrorHistory_PrintInfo(const LogErrorHistory* record);

#endif /* LOG_ERROR_HISTORY_ORM_H */
