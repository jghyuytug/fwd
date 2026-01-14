#include "member_info_detail_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 10

/* ======================================== CRUD Operations ======================================== */

int MemberInfoDetail_Add(DBConnectionManager* manager, const MemberInfoDetail* detail) {
    if (!manager || !detail) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "REPLACE INTO member_info_detail "
        "(m_id, zipcode, address, address_detail, occ_date) "
        "VALUES (%d, '%s', '%s', '%s', '%s')",
        detail->m_id, detail->zipcode, detail->address,
        detail->address_detail, detail->occ_date);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberInfoDetail_Get(DBConnectionManager* manager, int m_id, MemberInfoDetail* detail) {
    if (!manager || !detail) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, zipcode, address, address_detail, occ_date "
        "FROM member_info_detail WHERE m_id = %d",
        m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[MAX_FIELDS];
    int ret = DBQueryResult_FetchRow(&result, values);

    if (ret > 0) {
        detail->m_id = atoi(values[0]);
        strncpy(detail->zipcode, values[1], sizeof(detail->zipcode) - 1);
        strncpy(detail->address, values[2], sizeof(detail->address) - 1);
        strncpy(detail->address_detail, values[3], sizeof(detail->address_detail) - 1);
        strncpy(detail->occ_date, values[4], sizeof(detail->occ_date) - 1);
        detail->zipcode[sizeof(detail->zipcode) - 1] = '\0';
        detail->address[sizeof(detail->address) - 1] = '\0';
        detail->address_detail[sizeof(detail->address_detail) - 1] = '\0';
        detail->occ_date[sizeof(detail->occ_date) - 1] = '\0';
    }

    DBQueryResult_Free(&result);
    return (ret > 0) ? 0 : -1;
}

int MemberInfoDetail_Update(DBConnectionManager* manager, const MemberInfoDetail* detail) {
    if (!manager || !detail) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "UPDATE member_info_detail SET "
        "zipcode = '%s', address = '%s', "
        "address_detail = '%s', occ_date = '%s' "
        "WHERE m_id = %d",
        detail->zipcode, detail->address, detail->address_detail,
        detail->occ_date, detail->m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberInfoDetail_Delete(DBConnectionManager* manager, int m_id) {
    if (!manager) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "DELETE FROM member_info_detail WHERE m_id = %d", m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int MemberInfoDetail_Exists(DBConnectionManager* manager, int m_id) {
    if (!manager) {
        return 0;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM member_info_detail WHERE m_id = %d", m_id);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return 0;
    }

    char* values[MAX_FIELDS];
    int exists = 0;

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        exists = (atoi(values[0]) > 0) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return exists;
}

/* ======================================== Business Queries ======================================== */

int MemberInfoDetail_GetByZipcode(DBConnectionManager* manager, const char* zipcode,
                                  MemberInfoDetail* details, int max_count) {
    if (!manager || !zipcode || !details || max_count <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, zipcode, address, address_detail, occ_date "
        "FROM member_info_detail WHERE zipcode = '%s' LIMIT %d",
        zipcode, max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_count && DBQueryResult_FetchRow(&result, values) > 0) {
        details[count].m_id = atoi(values[0]);
        strncpy(details[count].zipcode, values[1], sizeof(details[count].zipcode) - 1);
        strncpy(details[count].address, values[2], sizeof(details[count].address) - 1);
        strncpy(details[count].address_detail, values[3], sizeof(details[count].address_detail) - 1);
        strncpy(details[count].occ_date, values[4], sizeof(details[count].occ_date) - 1);
        details[count].zipcode[sizeof(details[count].zipcode) - 1] = '\0';
        details[count].address[sizeof(details[count].address) - 1] = '\0';
        details[count].address_detail[sizeof(details[count].address_detail) - 1] = '\0';
        details[count].occ_date[sizeof(details[count].occ_date) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberInfoDetail_GetByAddressPattern(DBConnectionManager* manager, const char* address_pattern,
                                         MemberInfoDetail* details, int max_count) {
    if (!manager || !address_pattern || !details || max_count <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, zipcode, address, address_detail, occ_date "
        "FROM member_info_detail WHERE address LIKE '%%%s%%' LIMIT %d",
        address_pattern, max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_count && DBQueryResult_FetchRow(&result, values) > 0) {
        details[count].m_id = atoi(values[0]);
        strncpy(details[count].zipcode, values[1], sizeof(details[count].zipcode) - 1);
        strncpy(details[count].address, values[2], sizeof(details[count].address) - 1);
        strncpy(details[count].address_detail, values[3], sizeof(details[count].address_detail) - 1);
        strncpy(details[count].occ_date, values[4], sizeof(details[count].occ_date) - 1);
        details[count].zipcode[sizeof(details[count].zipcode) - 1] = '\0';
        details[count].address[sizeof(details[count].address) - 1] = '\0';
        details[count].address_detail[sizeof(details[count].address_detail) - 1] = '\0';
        details[count].occ_date[sizeof(details[count].occ_date) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberInfoDetail_GetByDateRange(DBConnectionManager* manager, const char* start_date,
                                    const char* end_date, MemberInfoDetail* details, int max_count) {
    if (!manager || !start_date || !end_date || !details || max_count <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, zipcode, address, address_detail, occ_date "
        "FROM member_info_detail WHERE occ_date BETWEEN '%s' AND '%s' LIMIT %d",
        start_date, end_date, max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_count && DBQueryResult_FetchRow(&result, values) > 0) {
        details[count].m_id = atoi(values[0]);
        strncpy(details[count].zipcode, values[1], sizeof(details[count].zipcode) - 1);
        strncpy(details[count].address, values[2], sizeof(details[count].address) - 1);
        strncpy(details[count].address_detail, values[3], sizeof(details[count].address_detail) - 1);
        strncpy(details[count].occ_date, values[4], sizeof(details[count].occ_date) - 1);
        details[count].zipcode[sizeof(details[count].zipcode) - 1] = '\0';
        details[count].address[sizeof(details[count].address) - 1] = '\0';
        details[count].address_detail[sizeof(details[count].address_detail) - 1] = '\0';
        details[count].occ_date[sizeof(details[count].occ_date) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberInfoDetail_GetAll(DBConnectionManager* manager, MemberInfoDetail* details, int offset, int limit) {
    if (!manager || !details || limit <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, zipcode, address, address_detail, occ_date "
        "FROM member_info_detail LIMIT %d, %d",
        offset, limit);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < limit && DBQueryResult_FetchRow(&result, values) > 0) {
        details[count].m_id = atoi(values[0]);
        strncpy(details[count].zipcode, values[1], sizeof(details[count].zipcode) - 1);
        strncpy(details[count].address, values[2], sizeof(details[count].address) - 1);
        strncpy(details[count].address_detail, values[3], sizeof(details[count].address_detail) - 1);
        strncpy(details[count].occ_date, values[4], sizeof(details[count].occ_date) - 1);
        details[count].zipcode[sizeof(details[count].zipcode) - 1] = '\0';
        details[count].address[sizeof(details[count].address) - 1] = '\0';
        details[count].address_detail[sizeof(details[count].address_detail) - 1] = '\0';
        details[count].occ_date[sizeof(details[count].occ_date) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberInfoDetail_GetCount(DBConnectionManager* manager) {
    if (!manager) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM member_info_detail");

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberInfoDetail_GetRecent(DBConnectionManager* manager, MemberInfoDetail* details, int limit) {
    if (!manager || !details || limit <= 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, zipcode, address, address_detail, occ_date "
        "FROM member_info_detail ORDER BY occ_date DESC LIMIT %d",
        limit);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < limit && DBQueryResult_FetchRow(&result, values) > 0) {
        details[count].m_id = atoi(values[0]);
        strncpy(details[count].zipcode, values[1], sizeof(details[count].zipcode) - 1);
        strncpy(details[count].address, values[2], sizeof(details[count].address) - 1);
        strncpy(details[count].address_detail, values[3], sizeof(details[count].address_detail) - 1);
        strncpy(details[count].occ_date, values[4], sizeof(details[count].occ_date) - 1);
        details[count].zipcode[sizeof(details[count].zipcode) - 1] = '\0';
        details[count].address[sizeof(details[count].address) - 1] = '\0';
        details[count].address_detail[sizeof(details[count].address_detail) - 1] = '\0';
        details[count].occ_date[sizeof(details[count].occ_date) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int MemberInfoDetail_GetOldRecords(DBConnectionManager* manager, int days_ago,
                                   MemberInfoDetail* details, int max_count) {
    if (!manager || !details || max_count <= 0 || days_ago < 0) {
        return -1;
    }

    char query[MAX_QUERY_LEN];
    snprintf(query, sizeof(query),
        "SELECT m_id, zipcode, address, address_detail, occ_date "
        "FROM member_info_detail WHERE occ_date < DATE_SUB(NOW(), INTERVAL %d DAY) LIMIT %d",
        days_ago, max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[MAX_FIELDS];

    while (count < max_count && DBQueryResult_FetchRow(&result, values) > 0) {
        details[count].m_id = atoi(values[0]);
        strncpy(details[count].zipcode, values[1], sizeof(details[count].zipcode) - 1);
        strncpy(details[count].address, values[2], sizeof(details[count].address) - 1);
        strncpy(details[count].address_detail, values[3], sizeof(details[count].address_detail) - 1);
        strncpy(details[count].occ_date, values[4], sizeof(details[count].occ_date) - 1);
        details[count].zipcode[sizeof(details[count].zipcode) - 1] = '\0';
        details[count].address[sizeof(details[count].address) - 1] = '\0';
        details[count].address_detail[sizeof(details[count].address_detail) - 1] = '\0';
        details[count].occ_date[sizeof(details[count].occ_date) - 1] = '\0';
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

/* ======================================== Bulk Operations ======================================== */

int MemberInfoDetail_BulkAdd(DBConnectionManager* manager, const MemberInfoDetail* details, int count) {
    if (!manager || !details || count <= 0) {
        return -1;
    }

    int success_count = 0;
    int i;

    for (i = 0; i < count; i++) {
        if (MemberInfoDetail_Add(manager, &details[i]) == 0) {
            success_count++;
        }
    }

    return success_count;
}

int MemberInfoDetail_BulkDelete(DBConnectionManager* manager, const int* m_ids, int count) {
    if (!manager || !m_ids || count <= 0) {
        return -1;
    }

    int i;
    for (i = 0; i < count; i++) {
        if (MemberInfoDetail_Delete(manager, m_ids[i]) != 0) {
            return -1;
        }
    }

    return 0;
}

/* ======================================== Utility Functions ======================================== */

void MemberInfoDetail_PrintInfo(const MemberInfoDetail* detail) {
    if (!detail) {
        printf("MemberInfoDetail is NULL\n");
        return;
    }

    printf("=== Member Info Detail ===\n");
    printf("  m_id:           %d\n", detail->m_id);
    printf("  zipcode:        %s\n", detail->zipcode);
    printf("  address:        %s\n", detail->address);
    printf("  address_detail: %s\n", detail->address_detail);
    printf("  occ_date:       %s\n", detail->occ_date);
    printf("==========================\n");
}
