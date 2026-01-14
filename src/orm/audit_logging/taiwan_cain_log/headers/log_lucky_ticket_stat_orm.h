#ifndef LOG_LUCKY_TICKET_STAT_ORM_H
#define LOG_LUCKY_TICKET_STAT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int idx;
    int occ_time;
    int m_id;
    int charac_no;
    signed char type_flag;
    signed char ticket_flag;
    int item_idx;
    signed char upgrade;
    signed char succ_flag;
} LogLuckyTicketStat;

/* CRUD Operations */
int LogLuckyTicketStat_Add(DBConnectionManager* manager, const LogLuckyTicketStat* record);
int LogLuckyTicketStat_Get(DBConnectionManager* manager, int idx, LogLuckyTicketStat* record);
int LogLuckyTicketStat_Update(DBConnectionManager* manager, const LogLuckyTicketStat* record);
int LogLuckyTicketStat_Delete(DBConnectionManager* manager, int idx);
int LogLuckyTicketStat_Exists(DBConnectionManager* manager, int idx);
int LogLuckyTicketStat_GetAll(DBConnectionManager* manager, LogLuckyTicketStat* records, int max_count, int* actual_count);

#endif /* LOG_LUCKY_TICKET_STAT_ORM_H */