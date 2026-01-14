#include "limited_shop_manager_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192
#define MAX_FIELDS 32

/* ==================== CRUD 操作实现 ==================== */

int LimitedShopManager_Add(DBConnectionManager* manager, const LimitedShopManager* shop) {
    char query[MAX_QUERY_LEN];
    char escaped_title[102];
    char escaped_reason_etc[402];
    char escaped_reason_stop[402];
    DBQueryResult result;

    if (!manager || !shop) {
        return -1;
    }

    /* 转义字符串 */
    /* 简化处理：直接复制，生产环境应使用 mysql_real_escape_string */
    snprintf(escaped_title, sizeof(escaped_title), "%s", shop->title);
    snprintf(escaped_reason_etc, sizeof(escaped_reason_etc), "%s", shop->reason_etc);
    snprintf(escaped_reason_stop, sizeof(escaped_reason_stop), "%s", shop->reason_stop);

    snprintf(query, sizeof(query),
        "INSERT INTO limited_shop_manager "
        "(occ_time, server_id, ipg_no, item_no, item_cnt, cera_price, gold_price, "
        "avatar_period_type, total_cnt, sell_cnt, restrict_no, start_time, end_time, "
        "real_end_time, npc_idx, cond_charac_job, cond_lev_begin, cond_lev_end, "
        "cond_acc_create_time_begin, cond_acc_create_time_end, "
        "cond_cha_create_time_begin, cond_cha_create_time_end, "
        "status_flag, title, range_section, reason_etc, reason_stop, pos_flag) "
        "VALUES (%u, %u, %u, %u, %u, %u, %u, %d, %d, %u, %u, %u, %u, %u, %u, "
        "%u, %u, %u, %u, %u, %u, %u, %u, '%s', %u, '%s', '%s', '%c')",
        shop->occ_time, (unsigned int)shop->server_id, shop->ipg_no,
        shop->item_no, shop->item_cnt, shop->cera_price, shop->gold_price,
        (int)shop->avatar_period_type, shop->total_cnt, shop->sell_cnt,
        shop->restrict_no, shop->start_time, shop->end_time, shop->real_end_time,
        shop->npc_idx, (unsigned int)shop->cond_charac_job,
        (unsigned int)shop->cond_lev_begin, (unsigned int)shop->cond_lev_end,
        shop->cond_acc_create_time_begin, shop->cond_acc_create_time_end,
        shop->cond_cha_create_time_begin, shop->cond_cha_create_time_end,
        (unsigned int)shop->status_flag, escaped_title,
        (unsigned int)shop->range_section, escaped_reason_etc, escaped_reason_stop,
        shop->pos_flag);

    memset(&result, 0, sizeof(DBQueryResult));
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);

    /* 获取自增ID */
    memset(&result, 0, sizeof(DBQueryResult));
    snprintf(query, sizeof(query), "SELECT LAST_INSERT_ID()");
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[1];
    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int new_id = atoi(values[0]);
    DBQueryResult_Free(&result);
    return new_id;
}

int LimitedShopManager_Get(DBConnectionManager* manager, unsigned int no, unsigned char server_id, unsigned int ipg_no, LimitedShopManager* shop) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !shop) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT no, occ_time, server_id, ipg_no, item_no, item_cnt, cera_price, gold_price, "
        "avatar_period_type, total_cnt, sell_cnt, restrict_no, start_time, end_time, "
        "real_end_time, npc_idx, cond_charac_job, cond_lev_begin, cond_lev_end, "
        "cond_acc_create_time_begin, cond_acc_create_time_end, "
        "cond_cha_create_time_begin, cond_cha_create_time_end, "
        "status_flag, title, range_section, reason_etc, reason_stop, pos_flag "
        "FROM limited_shop_manager WHERE no = %u AND server_id = %u AND ipg_no = %u",
        no, (unsigned int)server_id, ipg_no);

    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    /* 获取行数据 */
    char* values[29];
    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    /* 解析结果 */
    shop->no = (unsigned int)atoi(values[0]);
    shop->occ_time = (unsigned int)atoi(values[1]);
    shop->server_id = (unsigned char)atoi(values[2]);
    shop->ipg_no = (unsigned int)atoi(values[3]);
    shop->item_no = (unsigned int)atoi(values[4]);
    shop->item_cnt = (unsigned int)atoi(values[5]);
    shop->cera_price = (unsigned int)atoi(values[6]);
    shop->gold_price = (unsigned int)atoi(values[7]);
    shop->avatar_period_type = (char)atoi(values[8]);
    shop->total_cnt = atoi(values[9]);
    shop->sell_cnt = (unsigned int)atoi(values[10]);
    shop->restrict_no = (unsigned int)atoi(values[11]);
    shop->start_time = (unsigned int)atoi(values[12]);
    shop->end_time = (unsigned int)atoi(values[13]);
    shop->real_end_time = (unsigned int)atoi(values[14]);
    shop->npc_idx = (unsigned int)atoi(values[15]);
    shop->cond_charac_job = (unsigned char)atoi(values[16]);
    shop->cond_lev_begin = (unsigned char)atoi(values[17]);
    shop->cond_lev_end = (unsigned char)atoi(values[18]);
    shop->cond_acc_create_time_begin = (unsigned int)atoi(values[19]);
    shop->cond_acc_create_time_end = (unsigned int)atoi(values[20]);
    shop->cond_cha_create_time_begin = (unsigned int)atoi(values[21]);
    shop->cond_cha_create_time_end = (unsigned int)atoi(values[22]);
    shop->status_flag = (unsigned char)atoi(values[23]);
    snprintf(shop->title, sizeof(shop->title), "%s", values[24] ? values[24] : "");
    shop->range_section = (unsigned char)atoi(values[25]);
    snprintf(shop->reason_etc, sizeof(shop->reason_etc), "%s", values[26] ? values[26] : "");
    snprintf(shop->reason_stop, sizeof(shop->reason_stop), "%s", values[27] ? values[27] : "");
    shop->pos_flag = values[28] ? values[28][0] : '0';

    DBQueryResult_Free(&result);
    return 0;
}

int LimitedShopManager_Update(DBConnectionManager* manager, const LimitedShopManager* shop) {
    char query[MAX_QUERY_LEN];
    char escaped_title[102];
    char escaped_reason_etc[402];
    char escaped_reason_stop[402];
    DBQueryResult result;

    if (!manager || !shop) {
        return -1;
    }

    /* 转义字符串 */
    snprintf(escaped_title, sizeof(escaped_title), "%s", shop->title);
    snprintf(escaped_reason_etc, sizeof(escaped_reason_etc), "%s", shop->reason_etc);
    snprintf(escaped_reason_stop, sizeof(escaped_reason_stop), "%s", shop->reason_stop);

    snprintf(query, sizeof(query),
        "UPDATE limited_shop_manager SET "
        "occ_time=%u, item_no=%u, item_cnt=%u, cera_price=%u, gold_price=%u, "
        "avatar_period_type=%d, total_cnt=%d, sell_cnt=%u, restrict_no=%u, "
        "start_time=%u, end_time=%u, real_end_time=%u, npc_idx=%u, "
        "cond_charac_job=%u, cond_lev_begin=%u, cond_lev_end=%u, "
        "cond_acc_create_time_begin=%u, cond_acc_create_time_end=%u, "
        "cond_cha_create_time_begin=%u, cond_cha_create_time_end=%u, "
        "status_flag=%u, title='%s', range_section=%u, reason_etc='%s', "
        "reason_stop='%s', pos_flag='%c' "
        "WHERE no=%u AND server_id=%u AND ipg_no=%u",
        shop->occ_time, shop->item_no, shop->item_cnt, shop->cera_price, shop->gold_price,
        (int)shop->avatar_period_type, shop->total_cnt, shop->sell_cnt, shop->restrict_no,
        shop->start_time, shop->end_time, shop->real_end_time, shop->npc_idx,
        (unsigned int)shop->cond_charac_job, (unsigned int)shop->cond_lev_begin,
        (unsigned int)shop->cond_lev_end, shop->cond_acc_create_time_begin,
        shop->cond_acc_create_time_end, shop->cond_cha_create_time_begin,
        shop->cond_cha_create_time_end, (unsigned int)shop->status_flag, escaped_title,
        (unsigned int)shop->range_section, escaped_reason_etc, escaped_reason_stop,
        shop->pos_flag, shop->no, (unsigned int)shop->server_id, shop->ipg_no);

    memset(&result, 0, sizeof(DBQueryResult));
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LimitedShopManager_Delete(DBConnectionManager* manager, unsigned int no, unsigned char server_id, unsigned int ipg_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM limited_shop_manager WHERE no = %u AND server_id = %u AND ipg_no = %u",
        no, (unsigned int)server_id, ipg_no);

    memset(&result, 0, sizeof(DBQueryResult));
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LimitedShopManager_Exists(DBConnectionManager* manager, unsigned int no, unsigned char server_id, unsigned int ipg_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT 1 FROM limited_shop_manager WHERE no = %u AND server_id = %u AND ipg_no = %u LIMIT 1",
        no, (unsigned int)server_id, ipg_no);

    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    int exists = (ret > 0) ? 1 : 0;
    DBQueryResult_Free(&result);
    return exists;
}

/* ==================== 专项操作实现 ==================== */

int LimitedShopManager_IncrementSellCount(DBConnectionManager* manager, unsigned int no, unsigned char server_id, unsigned int ipg_no, unsigned int count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE limited_shop_manager SET sell_cnt = sell_cnt + %u "
        "WHERE no = %u AND server_id = %u AND ipg_no = %u",
        count, no, (unsigned int)server_id, ipg_no);

    memset(&result, 0, sizeof(DBQueryResult));
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LimitedShopManager_UpdateStatus(DBConnectionManager* manager, unsigned int no, unsigned char server_id, unsigned int ipg_no, unsigned char status_flag) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE limited_shop_manager SET status_flag = %u "
        "WHERE no = %u AND server_id = %u AND ipg_no = %u",
        (unsigned int)status_flag, no, (unsigned int)server_id, ipg_no);

    memset(&result, 0, sizeof(DBQueryResult));
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LimitedShopManager_SetStopReason(DBConnectionManager* manager, unsigned int no, unsigned char server_id, unsigned int ipg_no, const char* reason_stop) {
    char query[MAX_QUERY_LEN];
    char escaped_reason[402];
    DBQueryResult result;

    if (!manager || !reason_stop) {
        return -1;
    }

    snprintf(escaped_reason, sizeof(escaped_reason), "%s", reason_stop);

    snprintf(query, sizeof(query),
        "UPDATE limited_shop_manager SET reason_stop = '%s' "
        "WHERE no = %u AND server_id = %u AND ipg_no = %u",
        escaped_reason, no, (unsigned int)server_id, ipg_no);

    memset(&result, 0, sizeof(DBQueryResult));
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int LimitedShopManager_CheckStock(DBConnectionManager* manager, unsigned int no, unsigned char server_id, unsigned int ipg_no, unsigned int required_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT (total_cnt - sell_cnt) >= %u FROM limited_shop_manager "
        "WHERE no = %u AND server_id = %u AND ipg_no = %u",
        required_count, no, (unsigned int)server_id, ipg_no);

    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[1];
    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int sufficient = atoi(values[0]);
    DBQueryResult_Free(&result);
    return sufficient;
}

int LimitedShopManager_GetRemainingStock(DBConnectionManager* manager, unsigned int no, unsigned char server_id, unsigned int ipg_no, int* remaining) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !remaining) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT (total_cnt - sell_cnt) FROM limited_shop_manager "
        "WHERE no = %u AND server_id = %u AND ipg_no = %u",
        no, (unsigned int)server_id, ipg_no);

    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[1];
    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    *remaining = atoi(values[0]);
    DBQueryResult_Free(&result);
    return 0;
}

/* ==================== 查询操作实现 ==================== */

int LimitedShopManager_CountByServer(DBConnectionManager* manager, unsigned char server_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM limited_shop_manager WHERE server_id = %u",
        (unsigned int)server_id);

    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[1];
    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = atoi(values[0]);
    DBQueryResult_Free(&result);
    return count;
}

int LimitedShopManager_CountByStatus(DBConnectionManager* manager, unsigned char status_flag) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM limited_shop_manager WHERE status_flag = %u",
        (unsigned int)status_flag);

    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[1];
    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = atoi(values[0]);
    DBQueryResult_Free(&result);
    return count;
}

int LimitedShopManager_CountByServerAndStatus(DBConnectionManager* manager, unsigned char server_id, unsigned char status_flag) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM limited_shop_manager WHERE server_id = %u AND status_flag = %u",
        (unsigned int)server_id, (unsigned int)status_flag);

    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[1];
    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = atoi(values[0]);
    DBQueryResult_Free(&result);
    return count;
}

int LimitedShopManager_CountExpired(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM limited_shop_manager WHERE end_time < UNIX_TIMESTAMP()");

    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[1];
    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = atoi(values[0]);
    DBQueryResult_Free(&result);
    return count;
}

int LimitedShopManager_CountTotal(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM limited_shop_manager");

    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    char* values[1];
    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = atoi(values[0]);
    DBQueryResult_Free(&result);
    return count;
}

/* ==================== 批量操作实现 ==================== */

int LimitedShopManager_DeleteExpired(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM limited_shop_manager WHERE end_time < UNIX_TIMESTAMP()");

    memset(&result, 0, sizeof(DBQueryResult));
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;  /* 成功返回0（无法获取受影响行数，API未实现） */
}

int LimitedShopManager_UpdateExpiredStatus(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE limited_shop_manager SET status_flag = %d "
        "WHERE end_time < UNIX_TIMESTAMP() AND status_flag != %d",
        SHOP_STATUS_ENDED, SHOP_STATUS_ENDED);

    memset(&result, 0, sizeof(DBQueryResult));
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;  /* 成功返回0（无法获取受影响行数，API未实现） */
}

/* ==================== 工具函数实现 ==================== */

void LimitedShopManager_PrintInfo(const LimitedShopManager* shop) {
    if (!shop) {
        printf("LimitedShopManager: NULL\n");
        return;
    }

    printf("LimitedShopManager:\n");
    printf("  no: %u, server_id: %u, ipg_no: %u\n", shop->no, (unsigned int)shop->server_id, shop->ipg_no);
    printf("  title: %s\n", shop->title);
    printf("  item_no: %u, item_cnt: %u\n", shop->item_no, shop->item_cnt);
    printf("  cera_price: %u, gold_price: %u\n", shop->cera_price, shop->gold_price);
    printf("  库存: total=%d, sold=%u, remaining=%d\n", shop->total_cnt, shop->sell_cnt, shop->total_cnt - (int)shop->sell_cnt);
    printf("  时间: start=%u, end=%u, real_end=%u\n", shop->start_time, shop->end_time, shop->real_end_time);
    printf("  条件: job=%u, lev=%u~%u\n", (unsigned int)shop->cond_charac_job,
        (unsigned int)shop->cond_lev_begin, (unsigned int)shop->cond_lev_end);
    printf("  状态: status_flag=%u", (unsigned int)shop->status_flag);

    const char* status_str = "未知";
    switch (shop->status_flag) {
        case SHOP_STATUS_INACTIVE: status_str = "未激活"; break;
        case SHOP_STATUS_ACTIVE: status_str = "激活中"; break;
        case SHOP_STATUS_PAUSED: status_str = "暂停"; break;
        case SHOP_STATUS_ENDED: status_str = "已结束"; break;
    }
    printf(" (%s)\n", status_str);

    if (shop->reason_stop[0] != '\0') {
        printf("  停止原因: %s\n", shop->reason_stop);
    }
}

void LimitedShopManager_Free(LimitedShopManager* shop) {
    /* 当前结构体没有动态分配的字段，此函数保留供将来扩展 */
    (void)shop;
}
