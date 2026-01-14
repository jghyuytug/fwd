#ifndef LOG_POINT_HISTORY_ORM_H
#define LOG_POINT_HISTORY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned int no;
    char account_id[31];
    char charac_id[31];
    unsigned int cera_point;
    char command;
    char charge_type;
    char free_charge_type;
    unsigned int item_id;
    char query_user[46];
    char reg_date[20];
} LogPointHistory;

/* CRUD Operations */
int LogPointHistory_Add(DBConnectionManager* manager, const LogPointHistory* log);
int LogPointHistory_Get(DBConnectionManager* manager, unsigned int no, LogPointHistory* log);
int LogPointHistory_Update(DBConnectionManager* manager, const LogPointHistory* log);
int LogPointHistory_Delete(DBConnectionManager* manager, unsigned int no);
int LogPointHistory_Exists(DBConnectionManager* manager, unsigned int no);

/* Business Operations */
int LogPointHistory_GetAll(DBConnectionManager* manager, LogPointHistory* logs,
                            int max_count, int* actual_count);
int LogPointHistory_GetByAccount(DBConnectionManager* manager, const char* account_id,
                                  LogPointHistory* logs, int max_count, int* actual_count);
int LogPointHistory_GetByCommand(DBConnectionManager* manager, char command,
                                  LogPointHistory* logs, int max_count, int* actual_count);
int LogPointHistory_GetByDateRange(DBConnectionManager* manager, const char* start_date,
                                    const char* end_date, LogPointHistory* logs,
                                    int max_count, int* actual_count);

/* Utility Functions */
void LogPointHistory_PrintInfo(const LogPointHistory* log);

#endif
