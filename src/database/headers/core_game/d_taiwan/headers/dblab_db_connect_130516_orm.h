#ifndef DBLAB_DB_CONNECT_130516_ORM_H
#define DBLAB_DB_CONNECT_130516_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned int no;
    char host_name[51];
    unsigned char db_server_group;
    unsigned int db_type;
    char db_name[51];
    char db_ip[17];
    unsigned int db_port;
    char db_userid[21];
    char db_passwd[51];
    char comments[256];
} DblabDbConnect130516;

int DblabDbConnect130516_Add(DBConnectionManager* manager, const DblabDbConnect130516* record);
int DblabDbConnect130516_Get(DBConnectionManager* manager, unsigned int no, DblabDbConnect130516* record);
void DblabDbConnect130516_PrintInfo(const DblabDbConnect130516* record);

#endif
