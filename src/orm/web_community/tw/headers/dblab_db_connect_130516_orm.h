#ifndef DBLAB_DB_CONNECT_130516_ORM_H
#define DBLAB_DB_CONNECT_130516_ORM_H

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
} DblabDbConnect130516;

/* CRUD Operations */
int DblabDbConnect130516_Add(DBConnectionManager* manager, const DblabDbConnect130516* record);
int DblabDbConnect130516_GetAll(DBConnectionManager* manager, DblabDbConnect130516* records, int max_count, int* actual_count);

#endif /* DBLAB_DB_CONNECT_130516_ORM_H */