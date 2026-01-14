#include "ip_info_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 16

/* ==================== CRUD 操作 ==================== */

/**
 * 添加IP信息记录（使用 INSERT）
 */
int IpInfo_Add(DBConnectionManager* manager, const IpInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];

    if (!manager || !record) {
        return -1;
    }

    /* INSERT INTO（自增主键no自动生成） */
    snprintf(query, sizeof(query),
        "INSERT INTO ip_info "
        "(m_id, ip, start_ip, end_ip, occ_time, ip_check, vendor_no, speed_no, "
        "start_time, end_time, charge_flag, settle_no) "
        "VALUES (%u, '%s', %u, %u, '%s', %u, %u, %u, '%s', '%s', %d, %u)",
        record->m_id, record->ip, record->start_ip, record->end_ip, record->occ_time,
        record->ip_check, record->vendor_no, record->speed_no,
        record->start_time, record->end_time, record->charge_flag, record->settle_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    /* 获取自增ID */
    snprintf(query, sizeof(query), "SELECT LAST_INSERT_ID()");
    DBQueryResult_Free(&result);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    int insert_id = -1;
    if (DBQueryResult_FetchRow(&result, values) > 0) {
        insert_id = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return insert_id;
}

/**
 * 获取IP信息记录（按主键no）
 */
int IpInfo_Get(DBConnectionManager* manager, unsigned int no, IpInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT no, m_id, ip, start_ip, end_ip, occ_time, ip_check, vendor_no, speed_no, "
        "start_time, end_time, charge_flag, settle_no "
        "FROM ip_info WHERE no = %u",
        no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->no = (unsigned int)atoi(values[0]);
        record->m_id = (unsigned int)atoi(values[1]);
        strncpy(record->ip, values[2], sizeof(record->ip) - 1);
        record->ip[sizeof(record->ip) - 1] = '\0';
        record->start_ip = (unsigned char)atoi(values[3]);
        record->end_ip = (unsigned char)atoi(values[4]);
        strncpy(record->occ_time, values[5], sizeof(record->occ_time) - 1);
        record->occ_time[sizeof(record->occ_time) - 1] = '\0';
        record->ip_check = (unsigned char)atoi(values[6]);
        record->vendor_no = (unsigned int)atoi(values[7]);
        record->speed_no = (unsigned int)atoi(values[8]);
        strncpy(record->start_time, values[9], sizeof(record->start_time) - 1);
        record->start_time[sizeof(record->start_time) - 1] = '\0';
        strncpy(record->end_time, values[10], sizeof(record->end_time) - 1);
        record->end_time[sizeof(record->end_time) - 1] = '\0';
        record->charge_flag = (char)atoi(values[11]);
        record->settle_no = (unsigned int)atoi(values[12]);
        DBQueryResult_Free(&result);
        return 0;
    }

    DBQueryResult_Free(&result);
    return -1;
}

/**
 * 更新IP信息记录（按主键no）
 */
int IpInfo_Update(DBConnectionManager* manager, const IpInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE ip_info SET "
        "m_id = %u, ip = '%s', start_ip = %u, end_ip = %u, occ_time = '%s', "
        "ip_check = %u, vendor_no = %u, speed_no = %u, start_time = '%s', "
        "end_time = '%s', charge_flag = %d, settle_no = %u "
        "WHERE no = %u",
        record->m_id, record->ip, record->start_ip, record->end_ip, record->occ_time,
        record->ip_check, record->vendor_no, record->speed_no, record->start_time,
        record->end_time, record->charge_flag, record->settle_no, record->no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/**
 * 删除IP信息记录（按主键no）
 */
int IpInfo_Delete(DBConnectionManager* manager, unsigned int no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query), "DELETE FROM ip_info WHERE no = %u", no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/**
 * 检查IP信息记录是否存在（按主键no）
 */
int IpInfo_Exists(DBConnectionManager* manager, unsigned int no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int exists = 0;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query), "SELECT 1 FROM ip_info WHERE no = %u LIMIT 1", no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        exists = 1;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/* ==================== 专项操作 ==================== */

/**
 * 检查IP是否在记录范围内（ip前缀 + 最后一段匹配）
 */
int IpInfo_CheckIpInRange(DBConnectionManager* manager, const char* ip_prefix, unsigned char last_segment, IpInfo* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];

    if (!manager || !ip_prefix || !record) {
        return -1;
    }

    /* 查询：ip前缀匹配 AND last_segment在start_ip~end_ip范围内 */
    snprintf(query, sizeof(query),
        "SELECT no, m_id, ip, start_ip, end_ip, occ_time, ip_check, vendor_no, speed_no, "
        "start_time, end_time, charge_flag, settle_no "
        "FROM ip_info WHERE ip = '%s' AND %u BETWEEN start_ip AND end_ip "
        "LIMIT 1",
        ip_prefix, last_segment);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->no = (unsigned int)atoi(values[0]);
        record->m_id = (unsigned int)atoi(values[1]);
        strncpy(record->ip, values[2], sizeof(record->ip) - 1);
        record->ip[sizeof(record->ip) - 1] = '\0';
        record->start_ip = (unsigned char)atoi(values[3]);
        record->end_ip = (unsigned char)atoi(values[4]);
        strncpy(record->occ_time, values[5], sizeof(record->occ_time) - 1);
        record->occ_time[sizeof(record->occ_time) - 1] = '\0';
        record->ip_check = (unsigned char)atoi(values[6]);
        record->vendor_no = (unsigned int)atoi(values[7]);
        record->speed_no = (unsigned int)atoi(values[8]);
        strncpy(record->start_time, values[9], sizeof(record->start_time) - 1);
        record->start_time[sizeof(record->start_time) - 1] = '\0';
        strncpy(record->end_time, values[10], sizeof(record->end_time) - 1);
        record->end_time[sizeof(record->end_time) - 1] = '\0';
        record->charge_flag = (char)atoi(values[11]);
        record->settle_no = (unsigned int)atoi(values[12]);
        DBQueryResult_Free(&result);
        return 0;
    }

    DBQueryResult_Free(&result);
    return -1;
}

/**
 * 更新IP检查状态
 */
int IpInfo_UpdateCheckStatus(DBConnectionManager* manager, unsigned int no, unsigned char ip_check) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE ip_info SET ip_check = %u WHERE no = %u",
        ip_check, no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/**
 * 更新计费状态
 */
int IpInfo_UpdateChargeStatus(DBConnectionManager* manager, unsigned int no, char charge_flag, unsigned int settle_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE ip_info SET charge_flag = %d, settle_no = %u WHERE no = %u",
        charge_flag, settle_no, no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/**
 * 延长结束时间（用于网吧续费）
 */
int IpInfo_ExtendEndTime(DBConnectionManager* manager, unsigned int no, int hours) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    /* 使用MySQL DATE_ADD函数延长end_time */
    snprintf(query, sizeof(query),
        "UPDATE ip_info SET end_time = DATE_ADD(end_time, INTERVAL %d HOUR) WHERE no = %u",
        hours, no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/**
 * 批量删除过期记录（end_time < 当前时间）
 */
int IpInfo_DeleteExpired(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM ip_info WHERE end_time < NOW()");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    int affected = result.row_count;
    DBQueryResult_Free(&result);
    return affected;
}

/* ==================== 查询操作 ==================== */

/**
 * 按会员ID查询IP记录数量
 */
int IpInfo_CountByMemberId(DBConnectionManager* manager, unsigned int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM ip_info WHERE m_id = %u",
        m_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    int count = 0;
    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

/**
 * 按供应商编号查询记录数量
 */
int IpInfo_CountByVendor(DBConnectionManager* manager, unsigned int vendor_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM ip_info WHERE vendor_no = %u",
        vendor_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    int count = 0;
    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

/**
 * 按IP前缀查询记录数量
 */
int IpInfo_CountByIpPrefix(DBConnectionManager* manager, const char* ip_prefix) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];

    if (!manager || !ip_prefix) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM ip_info WHERE ip = '%s'",
        ip_prefix);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    int count = 0;
    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

/**
 * 统计有效记录数（end_time >= 当前时间）
 */
int IpInfo_CountActive(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM ip_info WHERE end_time >= NOW()");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    int count = 0;
    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

/**
 * 按检查状态统计
 */
int IpInfo_CountByCheckStatus(DBConnectionManager* manager, unsigned char ip_check) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM ip_info WHERE ip_check = %u",
        ip_check);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    int count = 0;
    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

/* ==================== 工具函数 ==================== */

/**
 * 打印IP信息记录（调试用）
 */
void IpInfo_PrintInfo(const IpInfo* record) {
    if (!record) {
        printf("IpInfo: NULL\n");
        return;
    }

    printf("========== IpInfo ==========\n");
    printf("no: %u\n", record->no);
    printf("m_id: %u\n", record->m_id);
    printf("ip: %s\n", record->ip);
    printf("IP范围: %u - %u (完整IP: %s%u ~ %s%u)\n",
        record->start_ip, record->end_ip,
        record->ip, record->start_ip, record->ip, record->end_ip);
    printf("occ_time: %s\n", record->occ_time);
    printf("ip_check: %u (0=未检查, 1=已检查, 2=异常)\n", record->ip_check);
    printf("vendor_no: %u\n", record->vendor_no);
    printf("speed_no: %u (1=慢速, 2=中速, 3=高速)\n", record->speed_no);
    printf("时间范围: %s ~ %s\n", record->start_time, record->end_time);
    printf("charge_flag: %d (0=免费, 1=计费)\n", record->charge_flag);
    printf("settle_no: %u\n", record->settle_no);
    printf("============================\n");
}
