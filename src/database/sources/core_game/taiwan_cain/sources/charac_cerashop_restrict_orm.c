#include "charac_cerashop_restrict_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 10

/*******************************************************************************
 * CRUD 操作实现
 ******************************************************************************/

int CharacCerashopRestrict_Add(DBConnectionManager* manager, const CharacCerashopRestrict* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "REPLACE INTO charac_cerashop_restrict (charac_no, ipg_no, count, next_date, end_date, last_access_date) "
        "VALUES (%u, %u, %u, %u, %u, %u)",
        record->charac_no, record->ipg_no, record->count,
        record->next_date, record->end_date, record->last_access_date);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacCerashopRestrict_Get(DBConnectionManager* manager, unsigned int charac_no, unsigned int ipg_no, CharacCerashopRestrict* record) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int ret = -1;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));
    memset(record, 0, sizeof(CharacCerashopRestrict));

    snprintf(query, sizeof(query),
        "SELECT charac_no, ipg_no, count, next_date, end_date, last_access_date "
        "FROM charac_cerashop_restrict WHERE charac_no = %u AND ipg_no = %u",
        charac_no, ipg_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->charac_no = (unsigned int)strtoul(values[0], NULL, 10);
        record->ipg_no = (unsigned int)strtoul(values[1], NULL, 10);
        record->count = (unsigned int)strtoul(values[2], NULL, 10);
        record->next_date = (unsigned int)strtoul(values[3], NULL, 10);
        record->end_date = (unsigned int)strtoul(values[4], NULL, 10);
        record->last_access_date = (unsigned int)strtoul(values[5], NULL, 10);
        ret = 0;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int CharacCerashopRestrict_Update(DBConnectionManager* manager, const CharacCerashopRestrict* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "UPDATE charac_cerashop_restrict SET count = %u, next_date = %u, end_date = %u, last_access_date = %u "
        "WHERE charac_no = %u AND ipg_no = %u",
        record->count, record->next_date, record->end_date, record->last_access_date,
        record->charac_no, record->ipg_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacCerashopRestrict_Delete(DBConnectionManager* manager, unsigned int charac_no, unsigned int ipg_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "DELETE FROM charac_cerashop_restrict WHERE charac_no = %u AND ipg_no = %u",
        charac_no, ipg_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacCerashopRestrict_Exists(DBConnectionManager* manager, unsigned int charac_no, unsigned int ipg_no) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_cerashop_restrict WHERE charac_no = %u AND ipg_no = %u",
        charac_no, ipg_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return (count > 0) ? 1 : 0;
}

/*******************************************************************************
 * 查询操作实现
 ******************************************************************************/

int CharacCerashopRestrict_Count(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = -1;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM charac_cerashop_restrict");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacCerashopRestrict_GetAll(DBConnectionManager* manager, CharacCerashopRestrict* records, int max_records) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "SELECT charac_no, ipg_no, count, next_date, end_date, last_access_date "
        "FROM charac_cerashop_restrict ORDER BY charac_no, ipg_no LIMIT %d",
        max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].charac_no = (unsigned int)strtoul(values[0], NULL, 10);
        records[count].ipg_no = (unsigned int)strtoul(values[1], NULL, 10);
        records[count].count = (unsigned int)strtoul(values[2], NULL, 10);
        records[count].next_date = (unsigned int)strtoul(values[3], NULL, 10);
        records[count].end_date = (unsigned int)strtoul(values[4], NULL, 10);
        records[count].last_access_date = (unsigned int)strtoul(values[5], NULL, 10);
        count++;
        memset(values, 0, sizeof(values));
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacCerashopRestrict_GetByCharacter(DBConnectionManager* manager, unsigned int charac_no, CharacCerashopRestrict* records, int max_records) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "SELECT charac_no, ipg_no, count, next_date, end_date, last_access_date "
        "FROM charac_cerashop_restrict WHERE charac_no = %u ORDER BY ipg_no LIMIT %d",
        charac_no, max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].charac_no = (unsigned int)strtoul(values[0], NULL, 10);
        records[count].ipg_no = (unsigned int)strtoul(values[1], NULL, 10);
        records[count].count = (unsigned int)strtoul(values[2], NULL, 10);
        records[count].next_date = (unsigned int)strtoul(values[3], NULL, 10);
        records[count].end_date = (unsigned int)strtoul(values[4], NULL, 10);
        records[count].last_access_date = (unsigned int)strtoul(values[5], NULL, 10);
        count++;
        memset(values, 0, sizeof(values));
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacCerashopRestrict_DeleteByCharacter(DBConnectionManager* manager, unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int deleted_count = 0;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));

    /* First get count */
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_cerashop_restrict WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        deleted_count = atoi(values[0]);
    }
    DBQueryResult_Free(&result);

    /* Then delete */
    memset(&result, 0, sizeof(DBQueryResult));
    snprintf(query, sizeof(query),
        "DELETE FROM charac_cerashop_restrict WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return deleted_count;
}

/*******************************************************************************
 * 业务操作实现
 ******************************************************************************/

int CharacCerashopRestrict_IncreaseCount(DBConnectionManager* manager, unsigned int charac_no, unsigned int ipg_no, unsigned int amount) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || amount == 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "UPDATE charac_cerashop_restrict SET count = count + %u WHERE charac_no = %u AND ipg_no = %u",
        amount, charac_no, ipg_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/*******************************************************************************
 * 工具函数实现
 ******************************************************************************/

void CharacCerashopRestrict_PrintInfo(const CharacCerashopRestrict* record) {
    if (!record) {
        printf("CharacCerashopRestrict: NULL\n");
        return;
    }

    printf("=== Charac Cerashop Restrict ===\n");
    printf("  charac_no: %u\n", record->charac_no);
    printf("  ipg_no: %u\n", record->ipg_no);
    printf("  count: %u\n", record->count);
    printf("  next_date: %u\n", record->next_date);
    printf("  end_date: %u\n", record->end_date);
    printf("  last_access_date: %u\n", record->last_access_date);
    printf("=================================\n");
}
