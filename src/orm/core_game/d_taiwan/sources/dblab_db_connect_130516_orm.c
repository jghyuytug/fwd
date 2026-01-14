#include "dblab_db_connect_130516_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096

int DblabDbConnect130516_Add(DBConnectionManager* manager, const DblabDbConnect130516* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO dblab_db_connect_130516 (no, host_name, db_server_group, db_type, db_name, db_ip, db_port, db_userid, db_passwd, comments) "
        "VALUES (%u, '%s', %u, %u, '%s', '%s', %u, '%s', '%s', '%s')",
        record->no, record->host_name, record->db_server_group, record->db_type, record->db_name,
        record->db_ip, record->db_port, record->db_userid, record->db_passwd, record->comments);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    DBQueryResult_Free(&result);
    return 0;
}

int DblabDbConnect130516_Get(DBConnectionManager* manager, unsigned int no, DblabDbConnect130516* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, host_name, db_server_group, db_type, db_name, db_ip, db_port, db_userid, db_passwd, comments "
        "FROM dblab_db_connect_130516 WHERE no = %u", no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0)
        return -1;

    if (DBQueryResult_FetchRow(&result, row) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(record, 0, sizeof(DblabDbConnect130516));
    record->no = row[0] ? (unsigned int)atoi(row[0]) : 0;
    if (row[1]) strncpy(record->host_name, row[1], sizeof(record->host_name) - 1);
    record->db_server_group = row[2] ? (unsigned char)atoi(row[2]) : 0;
    record->db_type = row[3] ? (unsigned int)atoi(row[3]) : 0;
    if (row[4]) strncpy(record->db_name, row[4], sizeof(record->db_name) - 1);
    if (row[5]) strncpy(record->db_ip, row[5], sizeof(record->db_ip) - 1);
    record->db_port = row[6] ? (unsigned int)atoi(row[6]) : 0;
    if (row[7]) strncpy(record->db_userid, row[7], sizeof(record->db_userid) - 1);
    if (row[8]) strncpy(record->db_passwd, row[8], sizeof(record->db_passwd) - 1);
    if (row[9]) strncpy(record->comments, row[9], sizeof(record->comments) - 1);

    DBQueryResult_Free(&result);
    return 0;
}

void DblabDbConnect130516_PrintInfo(const DblabDbConnect130516* record) {
    if (!record) return;
    printf("DblabDbConnect130516: no=%u, host=%s
", record->no, record->host_name);
}
