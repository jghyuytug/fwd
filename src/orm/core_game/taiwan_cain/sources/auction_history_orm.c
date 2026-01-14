#include "auction_history_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 32

/*******************************************************************************
 * CRUD 操作实现
 ******************************************************************************/

int AuctionHistory_Add(DBConnectionManager* manager, const AuctionHistory* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "REPLACE INTO auction_history (auction_id, start_time, occ_time, event_type, owner_id, buyer_id, price, "
        "seal_flag, item_id, add_info, upgrade, amplify_option, amplify_value, seal_cnt, endurance, "
        "extend_info, owner_postal_id, buyer_postal_id, unit_price) "
        "VALUES (%llu, '%s', '%s', %d, %d, %d, %d, %d, %u, %d, %u, %u, %u, %u, %u, %u, %u, %u, %u)",
        record->auction_id, record->start_time, record->occ_time, record->event_type,
        record->owner_id, record->buyer_id, record->price, record->seal_flag,
        record->item_id, record->add_info, record->upgrade, record->amplify_option,
        record->amplify_value, record->seal_cnt, record->endurance, record->extend_info,
        record->owner_postal_id, record->buyer_postal_id, record->unit_price);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AuctionHistory_Get(DBConnectionManager* manager, unsigned long long auction_id, AuctionHistory* record) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int ret = -1;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));
    memset(record, 0, sizeof(AuctionHistory));

    snprintf(query, sizeof(query),
        "SELECT auction_id, start_time, occ_time, event_type, owner_id, buyer_id, price, seal_flag, "
        "item_id, add_info, upgrade, amplify_option, amplify_value, seal_cnt, endurance, "
        "extend_info, owner_postal_id, buyer_postal_id, unit_price "
        "FROM auction_history WHERE auction_id = %llu", auction_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        record->auction_id = strtoull(values[0], NULL, 10);
        if (values[1]) strncpy(record->start_time, values[1], sizeof(record->start_time) - 1);
        if (values[2]) strncpy(record->occ_time, values[2], sizeof(record->occ_time) - 1);
        record->event_type = (char)atoi(values[3]);
        record->owner_id = atoi(values[4]);
        record->buyer_id = atoi(values[5]);
        record->price = atoi(values[6]);
        record->seal_flag = (char)atoi(values[7]);
        record->item_id = (unsigned int)strtoul(values[8], NULL, 10);
        record->add_info = atoi(values[9]);
        record->upgrade = (unsigned char)atoi(values[10]);
        record->amplify_option = (unsigned char)atoi(values[11]);
        record->amplify_value = (unsigned int)strtoul(values[12], NULL, 10);
        record->seal_cnt = (unsigned char)atoi(values[13]);
        record->endurance = (unsigned short)atoi(values[14]);
        record->extend_info = (unsigned int)strtoul(values[15], NULL, 10);
        record->owner_postal_id = (unsigned int)strtoul(values[16], NULL, 10);
        record->buyer_postal_id = (unsigned int)strtoul(values[17], NULL, 10);
        record->unit_price = (unsigned int)strtoul(values[18], NULL, 10);
        ret = 0;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int AuctionHistory_Update(DBConnectionManager* manager, const AuctionHistory* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "UPDATE auction_history SET start_time = '%s', occ_time = '%s', event_type = %d, "
        "owner_id = %d, buyer_id = %d, price = %d, seal_flag = %d, item_id = %u, "
        "add_info = %d, upgrade = %u, amplify_option = %u, amplify_value = %u, "
        "seal_cnt = %u, endurance = %u, extend_info = %u, owner_postal_id = %u, "
        "buyer_postal_id = %u, unit_price = %u WHERE auction_id = %llu",
        record->start_time, record->occ_time, record->event_type,
        record->owner_id, record->buyer_id, record->price, record->seal_flag,
        record->item_id, record->add_info, record->upgrade, record->amplify_option,
        record->amplify_value, record->seal_cnt, record->endurance, record->extend_info,
        record->owner_postal_id, record->buyer_postal_id, record->unit_price,
        record->auction_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AuctionHistory_Delete(DBConnectionManager* manager, unsigned long long auction_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "DELETE FROM auction_history WHERE auction_id = %llu", auction_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int AuctionHistory_Exists(DBConnectionManager* manager, unsigned long long auction_id) {
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
        "SELECT COUNT(*) FROM auction_history WHERE auction_id = %llu", auction_id);

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

int AuctionHistory_Count(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = -1;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM auction_history");

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

int AuctionHistory_GetAll(DBConnectionManager* manager, AuctionHistory* records, int max_records) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "SELECT auction_id, start_time, occ_time, event_type, owner_id, buyer_id, price, seal_flag, "
        "item_id, add_info, upgrade, amplify_option, amplify_value, seal_cnt, endurance, "
        "extend_info, owner_postal_id, buyer_postal_id, unit_price "
        "FROM auction_history ORDER BY auction_id DESC LIMIT %d", max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].auction_id = strtoull(values[0], NULL, 10);
        if (values[1]) strncpy(records[count].start_time, values[1], sizeof(records[count].start_time) - 1);
        if (values[2]) strncpy(records[count].occ_time, values[2], sizeof(records[count].occ_time) - 1);
        records[count].event_type = (char)atoi(values[3]);
        records[count].owner_id = atoi(values[4]);
        records[count].buyer_id = atoi(values[5]);
        records[count].price = atoi(values[6]);
        records[count].seal_flag = (char)atoi(values[7]);
        records[count].item_id = (unsigned int)strtoul(values[8], NULL, 10);
        records[count].add_info = atoi(values[9]);
        records[count].upgrade = (unsigned char)atoi(values[10]);
        records[count].amplify_option = (unsigned char)atoi(values[11]);
        records[count].amplify_value = (unsigned int)strtoul(values[12], NULL, 10);
        records[count].seal_cnt = (unsigned char)atoi(values[13]);
        records[count].endurance = (unsigned short)atoi(values[14]);
        records[count].extend_info = (unsigned int)strtoul(values[15], NULL, 10);
        records[count].owner_postal_id = (unsigned int)strtoul(values[16], NULL, 10);
        records[count].buyer_postal_id = (unsigned int)strtoul(values[17], NULL, 10);
        records[count].unit_price = (unsigned int)strtoul(values[18], NULL, 10);
        count++;
        memset(values, 0, sizeof(values));
    }

    DBQueryResult_Free(&result);
    return count;
}

int AuctionHistory_GetByOwner(DBConnectionManager* manager, int owner_id, AuctionHistory* records, int max_records) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "SELECT auction_id, start_time, occ_time, event_type, owner_id, buyer_id, price, seal_flag, "
        "item_id, add_info, upgrade, amplify_option, amplify_value, seal_cnt, endurance, "
        "extend_info, owner_postal_id, buyer_postal_id, unit_price "
        "FROM auction_history WHERE owner_id = %d ORDER BY auction_id DESC LIMIT %d",
        owner_id, max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].auction_id = strtoull(values[0], NULL, 10);
        if (values[1]) strncpy(records[count].start_time, values[1], sizeof(records[count].start_time) - 1);
        if (values[2]) strncpy(records[count].occ_time, values[2], sizeof(records[count].occ_time) - 1);
        records[count].event_type = (char)atoi(values[3]);
        records[count].owner_id = atoi(values[4]);
        records[count].buyer_id = atoi(values[5]);
        records[count].price = atoi(values[6]);
        records[count].seal_flag = (char)atoi(values[7]);
        records[count].item_id = (unsigned int)strtoul(values[8], NULL, 10);
        records[count].add_info = atoi(values[9]);
        records[count].upgrade = (unsigned char)atoi(values[10]);
        records[count].amplify_option = (unsigned char)atoi(values[11]);
        records[count].amplify_value = (unsigned int)strtoul(values[12], NULL, 10);
        records[count].seal_cnt = (unsigned char)atoi(values[13]);
        records[count].endurance = (unsigned short)atoi(values[14]);
        records[count].extend_info = (unsigned int)strtoul(values[15], NULL, 10);
        records[count].owner_postal_id = (unsigned int)strtoul(values[16], NULL, 10);
        records[count].buyer_postal_id = (unsigned int)strtoul(values[17], NULL, 10);
        records[count].unit_price = (unsigned int)strtoul(values[18], NULL, 10);
        count++;
        memset(values, 0, sizeof(values));
    }

    DBQueryResult_Free(&result);
    return count;
}

int AuctionHistory_GetByBuyer(DBConnectionManager* manager, int buyer_id, AuctionHistory* records, int max_records) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "SELECT auction_id, start_time, occ_time, event_type, owner_id, buyer_id, price, seal_flag, "
        "item_id, add_info, upgrade, amplify_option, amplify_value, seal_cnt, endurance, "
        "extend_info, owner_postal_id, buyer_postal_id, unit_price "
        "FROM auction_history WHERE buyer_id = %d ORDER BY auction_id DESC LIMIT %d",
        buyer_id, max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].auction_id = strtoull(values[0], NULL, 10);
        if (values[1]) strncpy(records[count].start_time, values[1], sizeof(records[count].start_time) - 1);
        if (values[2]) strncpy(records[count].occ_time, values[2], sizeof(records[count].occ_time) - 1);
        records[count].event_type = (char)atoi(values[3]);
        records[count].owner_id = atoi(values[4]);
        records[count].buyer_id = atoi(values[5]);
        records[count].price = atoi(values[6]);
        records[count].seal_flag = (char)atoi(values[7]);
        records[count].item_id = (unsigned int)strtoul(values[8], NULL, 10);
        records[count].add_info = atoi(values[9]);
        records[count].upgrade = (unsigned char)atoi(values[10]);
        records[count].amplify_option = (unsigned char)atoi(values[11]);
        records[count].amplify_value = (unsigned int)strtoul(values[12], NULL, 10);
        records[count].seal_cnt = (unsigned char)atoi(values[13]);
        records[count].endurance = (unsigned short)atoi(values[14]);
        records[count].extend_info = (unsigned int)strtoul(values[15], NULL, 10);
        records[count].owner_postal_id = (unsigned int)strtoul(values[16], NULL, 10);
        records[count].buyer_postal_id = (unsigned int)strtoul(values[17], NULL, 10);
        records[count].unit_price = (unsigned int)strtoul(values[18], NULL, 10);
        count++;
        memset(values, 0, sizeof(values));
    }

    DBQueryResult_Free(&result);
    return count;
}

int AuctionHistory_GetByItem(DBConnectionManager* manager, unsigned int item_id, AuctionHistory* records, int max_records) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "SELECT auction_id, start_time, occ_time, event_type, owner_id, buyer_id, price, seal_flag, "
        "item_id, add_info, upgrade, amplify_option, amplify_value, seal_cnt, endurance, "
        "extend_info, owner_postal_id, buyer_postal_id, unit_price "
        "FROM auction_history WHERE item_id = %u ORDER BY auction_id DESC LIMIT %d",
        item_id, max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].auction_id = strtoull(values[0], NULL, 10);
        if (values[1]) strncpy(records[count].start_time, values[1], sizeof(records[count].start_time) - 1);
        if (values[2]) strncpy(records[count].occ_time, values[2], sizeof(records[count].occ_time) - 1);
        records[count].event_type = (char)atoi(values[3]);
        records[count].owner_id = atoi(values[4]);
        records[count].buyer_id = atoi(values[5]);
        records[count].price = atoi(values[6]);
        records[count].seal_flag = (char)atoi(values[7]);
        records[count].item_id = (unsigned int)strtoul(values[8], NULL, 10);
        records[count].add_info = atoi(values[9]);
        records[count].upgrade = (unsigned char)atoi(values[10]);
        records[count].amplify_option = (unsigned char)atoi(values[11]);
        records[count].amplify_value = (unsigned int)strtoul(values[12], NULL, 10);
        records[count].seal_cnt = (unsigned char)atoi(values[13]);
        records[count].endurance = (unsigned short)atoi(values[14]);
        records[count].extend_info = (unsigned int)strtoul(values[15], NULL, 10);
        records[count].owner_postal_id = (unsigned int)strtoul(values[16], NULL, 10);
        records[count].buyer_postal_id = (unsigned int)strtoul(values[17], NULL, 10);
        records[count].unit_price = (unsigned int)strtoul(values[18], NULL, 10);
        count++;
        memset(values, 0, sizeof(values));
    }

    DBQueryResult_Free(&result);
    return count;
}

int AuctionHistory_GetByEventType(DBConnectionManager* manager, char event_type, AuctionHistory* records, int max_records) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager || !records || max_records <= 0) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));

    snprintf(query, sizeof(query),
        "SELECT auction_id, start_time, occ_time, event_type, owner_id, buyer_id, price, seal_flag, "
        "item_id, add_info, upgrade, amplify_option, amplify_value, seal_cnt, endurance, "
        "extend_info, owner_postal_id, buyer_postal_id, unit_price "
        "FROM auction_history WHERE event_type = %d ORDER BY auction_id DESC LIMIT %d",
        event_type, max_records);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (count < max_records && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].auction_id = strtoull(values[0], NULL, 10);
        if (values[1]) strncpy(records[count].start_time, values[1], sizeof(records[count].start_time) - 1);
        if (values[2]) strncpy(records[count].occ_time, values[2], sizeof(records[count].occ_time) - 1);
        records[count].event_type = (char)atoi(values[3]);
        records[count].owner_id = atoi(values[4]);
        records[count].buyer_id = atoi(values[5]);
        records[count].price = atoi(values[6]);
        records[count].seal_flag = (char)atoi(values[7]);
        records[count].item_id = (unsigned int)strtoul(values[8], NULL, 10);
        records[count].add_info = atoi(values[9]);
        records[count].upgrade = (unsigned char)atoi(values[10]);
        records[count].amplify_option = (unsigned char)atoi(values[11]);
        records[count].amplify_value = (unsigned int)strtoul(values[12], NULL, 10);
        records[count].seal_cnt = (unsigned char)atoi(values[13]);
        records[count].endurance = (unsigned short)atoi(values[14]);
        records[count].extend_info = (unsigned int)strtoul(values[15], NULL, 10);
        records[count].owner_postal_id = (unsigned int)strtoul(values[16], NULL, 10);
        records[count].buyer_postal_id = (unsigned int)strtoul(values[17], NULL, 10);
        records[count].unit_price = (unsigned int)strtoul(values[18], NULL, 10);
        count++;
        memset(values, 0, sizeof(values));
    }

    DBQueryResult_Free(&result);
    return count;
}

/*******************************************************************************
 * 统计操作实现
 ******************************************************************************/

int AuctionHistory_GetAveragePrice(DBConnectionManager* manager, unsigned int item_id) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int avg_price = -1;

    if (!manager) {
        return -1;
    }

    memset(&result, 0, sizeof(DBQueryResult));
    memset(values, 0, sizeof(values));

    snprintf(query, sizeof(query),
        "SELECT AVG(price) FROM auction_history WHERE item_id = %u AND price > 0", item_id);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        if (values[0]) {
            avg_price = atoi(values[0]);
        } else {
            avg_price = 0;
        }
    }

    DBQueryResult_Free(&result);
    return avg_price;
}

/*******************************************************************************
 * 工具函数实现
 ******************************************************************************/

void AuctionHistory_PrintInfo(const AuctionHistory* record) {
    if (!record) {
        printf("AuctionHistory: NULL\n");
        return;
    }

    printf("=== Auction History ===\n");
    printf("  auction_id: %llu\n", record->auction_id);
    printf("  start_time: %s\n", record->start_time);
    printf("  occ_time: %s\n", record->occ_time);
    printf("  event_type: %d\n", record->event_type);
    printf("  owner_id: %d\n", record->owner_id);
    printf("  buyer_id: %d\n", record->buyer_id);
    printf("  price: %d\n", record->price);
    printf("  item_id: %u\n", record->item_id);
    printf("  upgrade: %u\n", record->upgrade);
    printf("  amplify_option: %u\n", record->amplify_option);
    printf("  amplify_value: %u\n", record->amplify_value);
    printf("  unit_price: %u\n", record->unit_price);
    printf("========================\n");
}
