#include "dnf_story_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192
#define MAX_CONTENT_LEN 65535

/* ==================== CRUD 操作实现 ==================== */

int DnfStory_Add(DBConnectionManager* manager, const DnfStory* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    int base_len;

    if (!manager || !record) {
        return -1;
    }

    /* 构建 INSERT 查询 - 先不包含 content */
    base_len = snprintf(query, sizeof(query),
        "INSERT INTO dnf_story (story_type, notice_flag, m_id, reg_id, title, url, img_name, opt, open_flag, reg_date, hits, reserve_time, content) "
        "VALUES (%d, %d, %d, '%s', '%s', '%s', '%s', %u, '%c', '%s', %u, %u, ",
        record->story_type, record->notice_flag, record->m_id,
        record->reg_id, record->title, record->url, record->img_name,
        record->opt, record->open_flag, record->reg_date,
        record->hits, record->reserve_time);

    /* 添加 content 字段 */
    if (record->content) {
        base_len += snprintf(query + base_len, sizeof(query) - base_len, "'%s')", record->content);
    } else {
        base_len += snprintf(query + base_len, sizeof(query) - base_len, "NULL)");
    }

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

int DnfStory_Get(DBConnectionManager* manager, int no, DnfStory* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT no, story_type, notice_flag, m_id, reg_id, title, url, img_name, opt, open_flag, reg_date, hits, reserve_time, content "
        "FROM dnf_story WHERE no = %d",
        no);

    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    /* 获取行数据 */
    char* values[14];
    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    /* 解析结果 */
    record->no = atoi(values[0]);
    record->story_type = atoi(values[1]);
    record->notice_flag = atoi(values[2]);
    record->m_id = atoi(values[3]);
    snprintf(record->reg_id, sizeof(record->reg_id), "%s", values[4]);
    snprintf(record->title, sizeof(record->title), "%s", values[5]);
    snprintf(record->url, sizeof(record->url), "%s", values[6]);
    snprintf(record->img_name, sizeof(record->img_name), "%s", values[7]);
    record->opt = (unsigned char)atoi(values[8]);
    record->open_flag = values[9][0];
    snprintf(record->reg_date, sizeof(record->reg_date), "%s", values[10]);
    record->hits = (unsigned int)atoi(values[11]);
    record->reserve_time = (unsigned int)atoi(values[12]);

    /* 动态分配content内存 */
    if (values[13] && strlen(values[13]) > 0) {
        record->content = (char*)malloc(strlen(values[13]) + 1);
        if (record->content) {
            strcpy(record->content, values[13]);
        }
    } else {
        record->content = NULL;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfStory_Update(DBConnectionManager* manager, const DnfStory* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    int base_len;

    if (!manager || !record) {
        return -1;
    }

    /* 构建 UPDATE 查询 - 先不包含 content */
    base_len = snprintf(query, sizeof(query),
        "UPDATE dnf_story SET story_type=%d, notice_flag=%d, m_id=%d, reg_id='%s', title='%s', url='%s', img_name='%s', "
        "opt=%u, open_flag='%c', reg_date='%s', hits=%u, reserve_time=%u, content=",
        record->story_type, record->notice_flag, record->m_id, record->reg_id, record->title, record->url, record->img_name,
        record->opt, record->open_flag, record->reg_date, record->hits, record->reserve_time);

    /* 添加 content 字段和 WHERE 子句 */
    if (record->content) {
        base_len += snprintf(query + base_len, sizeof(query) - base_len, "'%s' WHERE no=%d", record->content, record->no);
    } else {
        base_len += snprintf(query + base_len, sizeof(query) - base_len, "NULL WHERE no=%d", record->no);
    }

    memset(&result, 0, sizeof(DBQueryResult));
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfStory_Delete(DBConnectionManager* manager, int no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM dnf_story WHERE no = %d", no);

    memset(&result, 0, sizeof(DBQueryResult));
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfStory_Exists(DBConnectionManager* manager, int no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT 1 FROM dnf_story WHERE no = %d LIMIT 1", no);

    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    int exists = (ret > 0) ? 1 : 0;
    DBQueryResult_Free(&result);
    return exists;
}

/* ==================== 专项操作实现 ==================== */

int DnfStory_GetByMid(DBConnectionManager* manager, int m_id, DnfStory* records, int max_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !records || max_count <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT no, story_type, notice_flag, m_id, reg_id, title, url, img_name, opt, open_flag, reg_date, hits, reserve_time, content "
        "FROM dnf_story WHERE m_id = %d ORDER BY reg_date DESC LIMIT %d",
        m_id, max_count);

    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[14];
    while (count < max_count && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].no = atoi(values[0]);
        records[count].story_type = atoi(values[1]);
        records[count].notice_flag = atoi(values[2]);
        records[count].m_id = atoi(values[3]);
        snprintf(records[count].reg_id, sizeof(records[count].reg_id), "%s", values[4]);
        snprintf(records[count].title, sizeof(records[count].title), "%s", values[5]);
        snprintf(records[count].url, sizeof(records[count].url), "%s", values[6]);
        snprintf(records[count].img_name, sizeof(records[count].img_name), "%s", values[7]);
        records[count].opt = (unsigned char)atoi(values[8]);
        records[count].open_flag = values[9][0];
        snprintf(records[count].reg_date, sizeof(records[count].reg_date), "%s", values[10]);
        records[count].hits = (unsigned int)atoi(values[11]);
        records[count].reserve_time = (unsigned int)atoi(values[12]);

        /* 动态分配content内存 */
        if (values[13] && strlen(values[13]) > 0) {
            records[count].content = (char*)malloc(strlen(values[13]) + 1);
            if (records[count].content) {
                strcpy(records[count].content, values[13]);
            }
        } else {
            records[count].content = NULL;
        }

        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int DnfStory_GetByStoryType(DBConnectionManager* manager, int story_type, DnfStory* records, int max_count) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !records || max_count <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT no, story_type, notice_flag, m_id, reg_id, title, url, img_name, opt, open_flag, reg_date, hits, reserve_time, content "
        "FROM dnf_story WHERE story_type = %d ORDER BY reg_date DESC LIMIT %d",
        story_type, max_count);

    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result);

    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    int count = 0;
    char* values[14];
    while (count < max_count && DBQueryResult_FetchRow(&result, values) > 0) {
        records[count].no = atoi(values[0]);
        records[count].story_type = atoi(values[1]);
        records[count].notice_flag = atoi(values[2]);
        records[count].m_id = atoi(values[3]);
        snprintf(records[count].reg_id, sizeof(records[count].reg_id), "%s", values[4]);
        snprintf(records[count].title, sizeof(records[count].title), "%s", values[5]);
        snprintf(records[count].url, sizeof(records[count].url), "%s", values[6]);
        snprintf(records[count].img_name, sizeof(records[count].img_name), "%s", values[7]);
        records[count].opt = (unsigned char)atoi(values[8]);
        records[count].open_flag = values[9][0];
        snprintf(records[count].reg_date, sizeof(records[count].reg_date), "%s", values[10]);
        records[count].hits = (unsigned int)atoi(values[11]);
        records[count].reserve_time = (unsigned int)atoi(values[12]);

        /* 动态分配content内存 */
        if (values[13] && strlen(values[13]) > 0) {
            records[count].content = (char*)malloc(strlen(values[13]) + 1);
            if (records[count].content) {
                strcpy(records[count].content, values[13]);
            }
        } else {
            records[count].content = NULL;
        }

        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int DnfStory_UpdateHits(DBConnectionManager* manager, int no, unsigned int hits) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE dnf_story SET hits=%u WHERE no=%d",
        hits, no);

    memset(&result, 0, sizeof(DBQueryResult));
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfStory_IncrementHits(DBConnectionManager* manager, int no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE dnf_story SET hits=hits+1 WHERE no=%d",
        no);

    memset(&result, 0, sizeof(DBQueryResult));
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int DnfStory_UpdateOpenFlag(DBConnectionManager* manager, int no, char open_flag) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    if (open_flag != 'y' && open_flag != 'n') {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE dnf_story SET open_flag='%c' WHERE no=%d",
        open_flag, no);

    memset(&result, 0, sizeof(DBQueryResult));
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* ==================== 查询操作实现 ==================== */

int DnfStory_CountByMid(DBConnectionManager* manager, int m_id) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM dnf_story WHERE m_id = %d", m_id);

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

int DnfStory_CountByStoryType(DBConnectionManager* manager, int story_type) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM dnf_story WHERE story_type = %d", story_type);

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

int DnfStory_CountOpen(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM dnf_story WHERE open_flag='y'");

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

int DnfStory_CountTotal(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM dnf_story");

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

int DnfStory_BatchDelete(DBConnectionManager* manager, const int* nos, int count) {
    char query[MAX_QUERY_LEN];
    char id_list[1024];
    DBQueryResult result;

    if (!manager || !nos || count <= 0) {
        return -1;
    }

    /* 构建 no 列表 */
    id_list[0] = '\0';
    for (int i = 0; i < count; i++) {
        char buf[32];
        snprintf(buf, sizeof(buf), "%d", nos[i]);
        if (i > 0) {
            strcat(id_list, ",");
        }
        strcat(id_list, buf);
    }

    snprintf(query, sizeof(query),
        "DELETE FROM dnf_story WHERE no IN (%s)", id_list);

    memset(&result, 0, sizeof(DBQueryResult));
    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_TAIWAN, query, &result) < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* ==================== 工具函数实现 ==================== */

void DnfStory_PrintInfo(const DnfStory* record) {
    if (!record) {
        printf("DnfStory: NULL\n");
        return;
    }

    printf("DnfStory:\n");
    printf("  no: %d\n", record->no);
    printf("  story_type: %d\n", record->story_type);
    printf("  notice_flag: %d\n", record->notice_flag);
    printf("  m_id: %d\n", record->m_id);
    printf("  reg_id: %s\n", record->reg_id);
    printf("  title: %s\n", record->title);
    printf("  url: %s\n", record->url);
    printf("  img_name: %s\n", record->img_name);
    printf("  opt: %u\n", record->opt);
    printf("  open_flag: %c\n", record->open_flag);
    printf("  reg_date: %s\n", record->reg_date);
    printf("  hits: %u\n", record->hits);
    printf("  reserve_time: %u\n", record->reserve_time);
    printf("  content: %s\n", record->content ? record->content : "(NULL)");
}

void DnfStory_FreeContent(DnfStory* record) {
    if (record && record->content) {
        free(record->content);
        record->content = NULL;
    }
}
