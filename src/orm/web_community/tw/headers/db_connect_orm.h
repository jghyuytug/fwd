#ifndef DB_CONNECT_ORM_H
#define DB_CONNECT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char host_name[51];
    signed char db_server_group;
    int db_type;
    char db_name[51];
    char db_ip[17];
    int db_port;
    char db_userid[21];
    char db_passwd[51];
    char comments[256];
} DbConnect;

/* CRUD Operations */
int DbConnect_Add(DBConnectionManager* manager, const DbConnect* record);
int DbConnect_GetAll(DBConnectionManager* manager, DbConnect* records, int max_count, int* actual_count);

#endif /* DB_CONNECT_ORM_H */