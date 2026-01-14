#include "dnf_pcroom_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 16

/*******************************************************************************
 * CRUD 操作实现
 ******************************************************************************/

int DnfPcroom_Add(DBConnectionManager* manager, const DnfPcroom* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int new_id = -1;

    if (!manager || !record) {
        return -1;
    }

    /* 验证IP范围 */
    if (!DnfPcroom_ValidateIpRange(record->start_ip, record->end_ip)) {
        fprintf(stderr, "DnfPcroom_Add: Invalid IP range (%u - %u)\n",
                record->start_ip, record->end_ip);
        return -1;
    }

    snprintf(query, sizeof(query),
        "INSERT INTO dnf_pcroom (district, firm_name, telephone, address, leader, "
        "ip, start_ip, end_ip) "
        "VALUES ('%s', '%s', '%s', '%s', '%s', '%s', %u, %u)",
        record->district, record->firm_name, record->telephone, record->address,
        record->leader, record->ip, record->start_ip, record->end_ip);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);

    /* 获取自动生成的ID */
    snprintf(query, sizeof(query), "SELECT LAST_INSERT_ID()");
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        new_id = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return new_id;
}

int DnfPcroom_Get(DBConnectionManager* manager, int ip_no, DnfPcroom* record) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int ret = -1;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT ip_no, district, firm_name, telephone, address, leader, "
        "ip, start_ip, end_ip FROM dnf_pcroom WHERE ip_no = %d",
        ip_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->ip_no = atoi(values[0]);
        strncpy(record->district, values[1] ? values[1] : "", sizeof(record->district) - 1);
        record->district[sizeof(record->district) - 1] = '\0';
        strncpy(record->firm_name, values[2] ? values[2] : "", sizeof(record->firm_name) - 1);
        record->firm_name[sizeof(record->firm_name) - 1] = '\0';
        strncpy(record->telephone, values[3] ? values[3] : "", sizeof(record->telephone) - 1);
        record->telephone[sizeof(record->telephone) - 1] = '\0';
        strncpy(record->address, values[4] ? values[4] : "", sizeof(record->address) - 1);
        record->address[sizeof(record->address) - 1] = '\0';
        strncpy(record->leader, values[5] ? values[5] : "", sizeof(record->leader) - 1);
        record->leader[sizeof(record->leader) - 1] = '\0';
        strncpy(record->ip, values[6] ? values[6] : "", sizeof(record->ip) - 1);
        record->ip[sizeof(record->ip) - 1] = '\0';
        record->start_ip = (unsigned char)atoi(values[7]);
        record->end_ip = (unsigned char)atoi(values[8]);

        ret = 0;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int DnfPcroom_Update(DBConnectionManager* manager, const DnfPcroom* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    /* 验证IP范围 */
    if (!DnfPcroom_ValidateIpRange(record->start_ip, record->end_ip)) {
        fprintf(stderr, "DnfPcroom_Update: Invalid IP range (%u - %u)\n",
                record->start_ip, record->end_ip);
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE dnf_pcroom SET district = '%s', firm_name = '%s', telephone = '%s', "
        "address = '%s', leader = '%s', ip = '%s', start_ip = %u, end_ip = %u "
        "WHERE ip_no = %d",
        record->district, record->firm_name, record->telephone, record->address,
        record->leader, record->ip, record->start_ip, record->end_ip, record->ip_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfPcroom_Delete(DBConnectionManager* manager, int ip_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query), "DELETE FROM dnf_pcroom WHERE ip_no = %d", ip_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfPcroom_Exists(DBConnectionManager* manager, int ip_no) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int exists = 0;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT 1 FROM dnf_pcroom WHERE ip_no = %d LIMIT 1", ip_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        exists = 1;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/*******************************************************************************
 * 业务操作实现
 ******************************************************************************/

int DnfPcroom_GetByDistrict(DBConnectionManager* manager, const char* district,
                            DnfPcroom* records, int max_count) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager || !district || !records || max_count <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT ip_no, district, firm_name, telephone, address, leader, "
        "ip, start_ip, end_ip FROM dnf_pcroom WHERE district = '%s' "
        "ORDER BY ip_no LIMIT %d",
        district, max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].ip_no = atoi(values[0]);
        strncpy(records[count].district, values[1] ? values[1] : "",
                sizeof(records[count].district) - 1);
        records[count].district[sizeof(records[count].district) - 1] = '\0';
        strncpy(records[count].firm_name, values[2] ? values[2] : "",
                sizeof(records[count].firm_name) - 1);
        records[count].firm_name[sizeof(records[count].firm_name) - 1] = '\0';
        strncpy(records[count].telephone, values[3] ? values[3] : "",
                sizeof(records[count].telephone) - 1);
        records[count].telephone[sizeof(records[count].telephone) - 1] = '\0';
        strncpy(records[count].address, values[4] ? values[4] : "",
                sizeof(records[count].address) - 1);
        records[count].address[sizeof(records[count].address) - 1] = '\0';
        strncpy(records[count].leader, values[5] ? values[5] : "",
                sizeof(records[count].leader) - 1);
        records[count].leader[sizeof(records[count].leader) - 1] = '\0';
        strncpy(records[count].ip, values[6] ? values[6] : "",
                sizeof(records[count].ip) - 1);
        records[count].ip[sizeof(records[count].ip) - 1] = '\0';
        records[count].start_ip = (unsigned char)atoi(values[7]);
        records[count].end_ip = (unsigned char)atoi(values[8]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int DnfPcroom_SearchByFirmName(DBConnectionManager* manager, const char* firm_name,
                               DnfPcroom* records, int max_count) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager || !firm_name || !records || max_count <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT ip_no, district, firm_name, telephone, address, leader, "
        "ip, start_ip, end_ip FROM dnf_pcroom WHERE firm_name LIKE '%%%s%%' "
        "ORDER BY ip_no LIMIT %d",
        firm_name, max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].ip_no = atoi(values[0]);
        strncpy(records[count].district, values[1] ? values[1] : "",
                sizeof(records[count].district) - 1);
        records[count].district[sizeof(records[count].district) - 1] = '\0';
        strncpy(records[count].firm_name, values[2] ? values[2] : "",
                sizeof(records[count].firm_name) - 1);
        records[count].firm_name[sizeof(records[count].firm_name) - 1] = '\0';
        strncpy(records[count].telephone, values[3] ? values[3] : "",
                sizeof(records[count].telephone) - 1);
        records[count].telephone[sizeof(records[count].telephone) - 1] = '\0';
        strncpy(records[count].address, values[4] ? values[4] : "",
                sizeof(records[count].address) - 1);
        records[count].address[sizeof(records[count].address) - 1] = '\0';
        strncpy(records[count].leader, values[5] ? values[5] : "",
                sizeof(records[count].leader) - 1);
        records[count].leader[sizeof(records[count].leader) - 1] = '\0';
        strncpy(records[count].ip, values[6] ? values[6] : "",
                sizeof(records[count].ip) - 1);
        records[count].ip[sizeof(records[count].ip) - 1] = '\0';
        records[count].start_ip = (unsigned char)atoi(values[7]);
        records[count].end_ip = (unsigned char)atoi(values[8]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int DnfPcroom_CheckIpInRange(DBConnectionManager* manager, const char* full_ip,
                             DnfPcroom* record) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    char ip_prefix[12];
    unsigned int o1, o2, o3, o4;
    int ret = 0;

    if (!manager || !full_ip || !record) {
        return -1;
    }

    /* 解析IP地址：192.168.1.100 -> ip_prefix="192.168.1", last_octet=100 */
    if (sscanf(full_ip, "%u.%u.%u.%u", &o1, &o2, &o3, &o4) != 4) {
        fprintf(stderr, "DnfPcroom_CheckIpInRange: Invalid IP format: %s\n", full_ip);
        return -1;
    }

    /* 组合IP前缀和最后一个八位组 */
    snprintf(ip_prefix, sizeof(ip_prefix), "%u.%u.%u", o1, o2, o3);

    /* 查询匹配的网吧 */
    snprintf(query, sizeof(query),
        "SELECT ip_no, district, firm_name, telephone, address, leader, "
        "ip, start_ip, end_ip FROM dnf_pcroom "
        "WHERE ip = '%s' AND %u >= start_ip AND %u <= end_ip "
        "LIMIT 1",
        ip_prefix, o4, o4);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->ip_no = atoi(values[0]);
        strncpy(record->district, values[1] ? values[1] : "", sizeof(record->district) - 1);
        record->district[sizeof(record->district) - 1] = '\0';
        strncpy(record->firm_name, values[2] ? values[2] : "", sizeof(record->firm_name) - 1);
        record->firm_name[sizeof(record->firm_name) - 1] = '\0';
        strncpy(record->telephone, values[3] ? values[3] : "", sizeof(record->telephone) - 1);
        record->telephone[sizeof(record->telephone) - 1] = '\0';
        strncpy(record->address, values[4] ? values[4] : "", sizeof(record->address) - 1);
        record->address[sizeof(record->address) - 1] = '\0';
        strncpy(record->leader, values[5] ? values[5] : "", sizeof(record->leader) - 1);
        record->leader[sizeof(record->leader) - 1] = '\0';
        strncpy(record->ip, values[6] ? values[6] : "", sizeof(record->ip) - 1);
        record->ip[sizeof(record->ip) - 1] = '\0';
        record->start_ip = (unsigned char)atoi(values[7]);
        record->end_ip = (unsigned char)atoi(values[8]);
        ret = 1;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int DnfPcroom_GetByLeader(DBConnectionManager* manager, const char* leader,
                          DnfPcroom* records, int max_count) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager || !leader || !records || max_count <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT ip_no, district, firm_name, telephone, address, leader, "
        "ip, start_ip, end_ip FROM dnf_pcroom WHERE leader = '%s' "
        "ORDER BY ip_no LIMIT %d",
        leader, max_count);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].ip_no = atoi(values[0]);
        strncpy(records[count].district, values[1] ? values[1] : "",
                sizeof(records[count].district) - 1);
        records[count].district[sizeof(records[count].district) - 1] = '\0';
        strncpy(records[count].firm_name, values[2] ? values[2] : "",
                sizeof(records[count].firm_name) - 1);
        records[count].firm_name[sizeof(records[count].firm_name) - 1] = '\0';
        strncpy(records[count].telephone, values[3] ? values[3] : "",
                sizeof(records[count].telephone) - 1);
        records[count].telephone[sizeof(records[count].telephone) - 1] = '\0';
        strncpy(records[count].address, values[4] ? values[4] : "",
                sizeof(records[count].address) - 1);
        records[count].address[sizeof(records[count].address) - 1] = '\0';
        strncpy(records[count].leader, values[5] ? values[5] : "",
                sizeof(records[count].leader) - 1);
        records[count].leader[sizeof(records[count].leader) - 1] = '\0';
        strncpy(records[count].ip, values[6] ? values[6] : "",
                sizeof(records[count].ip) - 1);
        records[count].ip[sizeof(records[count].ip) - 1] = '\0';
        records[count].start_ip = (unsigned char)atoi(values[7]);
        records[count].end_ip = (unsigned char)atoi(values[8]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int DnfPcroom_Count(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM dnf_pcroom");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

int DnfPcroom_CountByDistrict(DBConnectionManager* manager, const char* district) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager || !district) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM dnf_pcroom WHERE district = '%s'", district);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

/*******************************************************************************
 * 工具函数实现
 ******************************************************************************/

void DnfPcroom_PrintInfo(const DnfPcroom* record) {
    if (!record) {
        printf("DnfPcroom: (null)\n");
        return;
    }

    printf("============ DNF PC Room Info ============\n");
    printf("  IP No:       %d\n", record->ip_no);
    printf("  District:    %s\n", record->district);
    printf("  Firm Name:   %s\n", record->firm_name);
    printf("  Telephone:   %s\n", record->telephone);
    printf("  Address:     %s\n", record->address);
    printf("  Leader:      %s\n", record->leader);
    printf("  IP Range:    %s.%u - %s.%u\n",
           record->ip, record->start_ip, record->ip, record->end_ip);
    printf("=========================================\n");
}

int DnfPcroom_ValidateIpRange(unsigned char start_ip, unsigned char end_ip) {
    /* 起始IP必须小于等于结束IP */
    if (start_ip > end_ip) {
        return 0;
    }
    return 1;
}
