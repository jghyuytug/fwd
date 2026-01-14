#include "db_connect_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192

int DbConnect_Add(DBConnectionManager* manager, const DbConnect* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) return -1;

    snprintf(query, sizeof(query),
        "INSERT INTO db_connect (no, host_name, db_server_group, db_type, db_name, db_ip, db_port, db_userid, db_passwd, comments) "
        "VALUES (%d, '%s', %d, %d, '%s', '%s', %d, '%s', '%s', '%s')",
        record->no, record->host_name, record->db_server_group, record->db_type, record->db_name, record->db_ip, record->db_port, record->db_userid, record->db_passwd, record->comments);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DbConnect_GetAll(DBConnectionManager* manager, DbConnect* records, int max_count, int* actual_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* row[10];
    int count = 0;

    if (!manager || !records || !actual_count) return -1;

    snprintf(query, sizeof(query),
        "SELECT no, host_name, db_server_group, db_type, db_name, db_ip, db_port, db_userid, db_passwd, comments FROM db_connect");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TW, query, &result) < 0)
        return -1;

    while (count < max_count && DBQueryResult_FetchRow(&result, row) > 0) {
        memset(&records[count], 0, sizeof(DbConnect));
        records[count].no = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(records[count].host_name, row[1], sizeof(records[count].host_name) - 1);
        records[count].db_server_group = row[2] ? atoi(row[2]) : 0;
        records[count].db_type = row[3] ? atoi(row[3]) : 0;
        if (row[4]) strncpy(records[count].db_name, row[4], sizeof(records[count].db_name) - 1);
        if (row[5]) strncpy(records[count].db_ip, row[5], sizeof(records[count].db_ip) - 1);
        records[count].db_port = row[6] ? atoi(row[6]) : 0;
        if (row[7]) strncpy(records[count].db_userid, row[7], sizeof(records[count].db_userid) - 1);
        if (row[8]) strncpy(records[count].db_passwd, row[8], sizeof(records[count].db_passwd) - 1);
        if (row[9]) strncpy(records[count].comments, row[9], sizeof(records[count].comments) - 1);
        count++;
    }

    *actual_count = count;
    DBQueryResult_Free(&result);
    return 0;
}
