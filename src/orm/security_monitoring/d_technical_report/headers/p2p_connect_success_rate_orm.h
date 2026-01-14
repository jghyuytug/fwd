#ifndef P2P_CONNECT_SUCCESS_RATE_ORM_H
#define P2P_CONNECT_SUCCESS_RATE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    signed char server_group;
    signed char connected_type;
    int required_time;
    int check_time;
    char nation_code[16];
    char peer_address[16];
    char occ_date[20];
} P2PConnectSuccessRate;

/* CRUD Operations */
int P2PConnectSuccessRate_Add(DBConnectionManager* manager, const P2PConnectSuccessRate* record);
int P2PConnectSuccessRate_Get(DBConnectionManager* manager, int no, P2PConnectSuccessRate* record);
int P2PConnectSuccessRate_Update(DBConnectionManager* manager, const P2PConnectSuccessRate* record);
int P2PConnectSuccessRate_Delete(DBConnectionManager* manager, int no);
int P2PConnectSuccessRate_Exists(DBConnectionManager* manager, int no);
int P2PConnectSuccessRate_GetAll(DBConnectionManager* manager, P2PConnectSuccessRate* records, int max_count, int* actual_count);

#endif /* P2P_CONNECT_SUCCESS_RATE_ORM_H */