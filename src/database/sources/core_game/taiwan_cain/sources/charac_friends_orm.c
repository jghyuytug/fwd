#include "charac_friends_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 4096
#define MAX_FIELDS 16

/*******************************************************************************
 * CRUD 操作实现
 ******************************************************************************/

int CharacFriends_Add(DBConnectionManager* manager, const CharacFriends* record) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !record) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "REPLACE INTO charac_friends (charac_no, friend_no) VALUES (%u, %u)",
        record->charac_no, record->friend_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacFriends_Delete(DBConnectionManager* manager, unsigned int charac_no,
                         unsigned int friend_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM charac_friends WHERE charac_no = %u AND friend_no = %u",
        charac_no, friend_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int CharacFriends_Exists(DBConnectionManager* manager, unsigned int charac_no,
                         unsigned int friend_no) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int exists = 0;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT 1 FROM charac_friends WHERE charac_no = %u AND friend_no = %u LIMIT 1",
        charac_no, friend_no);

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
 * 业务查询操作实现
 ******************************************************************************/

int CharacFriends_GetFriendsByCharacNo(DBConnectionManager* manager,
                                       unsigned int charac_no,
                                       unsigned int* friend_nos,
                                       int max_friends) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager || !friend_nos || max_friends <= 0) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT friend_no FROM charac_friends WHERE charac_no = %u ORDER BY friend_no",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    while (count < max_friends && DBQueryResult_FetchRow(&result, values) > 0) {
        friend_nos[count] = (unsigned int)atoi(values[0]);
        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacFriends_GetFriendsCount(DBConnectionManager* manager,
                                  unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int count = 0;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_friends WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacFriends_IsMutualFriend(DBConnectionManager* manager,
                                 unsigned int charac_no1,
                                 unsigned int charac_no2) {
    char query[MAX_QUERY_LEN];
    char* values[MAX_FIELDS];
    DBQueryResult result;
    int is_mutual = 0;

    if (!manager) {
        return -1;
    }

    /* 检查双向好友关系 */
    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM charac_friends "
        "WHERE (charac_no = %u AND friend_no = %u) "
        "OR (charac_no = %u AND friend_no = %u)",
        charac_no1, charac_no2, charac_no2, charac_no1);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        /* 如果双向都存在，计数应该是2 */
        is_mutual = (atoi(values[0]) == 2) ? 1 : 0;
    }

    DBQueryResult_Free(&result);
    return is_mutual;
}

/*******************************************************************************
 * 批量操作实现
 ******************************************************************************/

int CharacFriends_BulkAdd(DBConnectionManager* manager,
                          const CharacFriends* records, int count) {
    char query[MAX_QUERY_LEN];
    char values_str[MAX_QUERY_LEN];
    DBQueryResult result;
    int i;

    if (!manager || !records || count <= 0) {
        return -1;
    }

    /* 构建批量插入语句 */
    snprintf(query, sizeof(query), "REPLACE INTO charac_friends (charac_no, friend_no) VALUES ");

    for (i = 0; i < count; i++) {
        if (i > 0) {
            strncat(query, ", ", sizeof(query) - strlen(query) - 1);
        }
        snprintf(values_str, sizeof(values_str), "(%u, %u)",
                 records[i].charac_no, records[i].friend_no);
        strncat(query, values_str, sizeof(query) - strlen(query) - 1);
    }

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacFriends_DeleteAllByCharacNo(DBConnectionManager* manager,
                                      unsigned int charac_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "DELETE FROM charac_friends WHERE charac_no = %u",
        charac_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/*******************************************************************************
 * 工具函数实现
 ******************************************************************************/

void CharacFriends_PrintInfo(const CharacFriends* record) {
    if (!record) {
        printf("CharacFriends record is NULL\n");
        return;
    }

    printf("=== CharacFriends Record ===\n");
    printf("  charac_no: %u\n", record->charac_no);
    printf("  friend_no: %u\n", record->friend_no);
    printf("============================\n");
}
