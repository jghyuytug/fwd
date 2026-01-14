#ifndef TCG_RELAY_CONFIG_ORM_H
#define TCG_RELAY_CONFIG_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int seq;
    int occ_date;
    int event_seq;
    char event_name[51];
    int prize_count;
    char item_nick[51];
    char item_name[51];
    int item_quantity;
    int prize_date;
    int prize_user_m_id;
    signed char prize_user_server_id;
    int prize_user_charac_no;
    int item_id;
} TcgRelayConfig;

/* CRUD Operations */
int TcgRelayConfig_Add(DBConnectionManager* manager, const TcgRelayConfig* record);
int TcgRelayConfig_Get(DBConnectionManager* manager, int seq, TcgRelayConfig* record);
int TcgRelayConfig_Update(DBConnectionManager* manager, const TcgRelayConfig* record);
int TcgRelayConfig_Delete(DBConnectionManager* manager, int seq);
int TcgRelayConfig_Exists(DBConnectionManager* manager, int seq);
int TcgRelayConfig_GetAll(DBConnectionManager* manager, TcgRelayConfig* records, int max_count, int* actual_count);

#endif /* TCG_RELAY_CONFIG_ORM_H */