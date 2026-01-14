/**
 * DNF游戏服务器 - CharacFriends表ORM实现
 */

#define _GNU_SOURCE
#include "charac_friends_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ========================================================================
// 内部辅助函数
// ========================================================================

/**
 * 从查询结果行填充好友关系结构
 */
static void FillFriendFromRow(char** row, CharacFriend* friend) {
    friend->charac_no = atoi(row[0]);
    friend->friend_no = atoi(row[1]);
}

// ========================================================================
// CRUD操作实现
// ========================================================================

int CharacFriends_Add(DBConnectionManager* manager, CharacFriend* friend) {
    char query[256];

    /* 使用INSERT IGNORE实现幂等性 - 重复添加不报错 */
    snprintf(query, sizeof(query),
             "INSERT IGNORE INTO charac_friends (charac_no, friend_no) "
             "VALUES (%d, %d)",
             friend->charac_no, friend->friend_no);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);

    /* 查询执行错误 */
    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    /* 添加成功（无论是新增还是已存在） */
    if (ret > 0) {
        printf("[CharacFriends ORM] Added friend: charac_no=%d, friend_no=%d\n",
               friend->charac_no, friend->friend_no);
    }

    DBQueryResult_Free(&result);
    return 0;  /* 成功（幂等） */
}

int CharacFriends_Remove(DBConnectionManager* manager, int charac_no, int friend_no) {
    char query[256];
    snprintf(query, sizeof(query),
             "DELETE FROM charac_friends WHERE charac_no=%d AND friend_no=%d",
             charac_no, friend_no);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);

    /* 查询执行错误 */
    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    /* 删除成功（无论是否真的删除了记录） */
    if (ret > 0) {
        printf("[CharacFriends ORM] Removed friend: charac_no=%d, friend_no=%d\n",
               charac_no, friend_no);
    }

    DBQueryResult_Free(&result);
    return 0;  /* 成功（幂等 - 0行受影响也是成功） */
}

int CharacFriends_GetList(DBConnectionManager* manager, int charac_no,
                          CharacFriend* friends, int max_count) {
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT charac_no, friend_no FROM charac_friends "
             "WHERE charac_no=%d ORDER BY friend_no LIMIT %d",
             charac_no, max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);

    /* 查询执行错误 */
    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    /* 无记录 - 返回0不是错误 */
    if (ret == 0) {
        DBQueryResult_Free(&result);
        return 0;
    }

    /* 填充好友列表 */
    int count = (result.row_count < max_count) ? result.row_count : max_count;
    char* values[2];
    for (int i = 0; i < count; i++) {
        if (DBQueryResult_FetchRow(&result, values) > 0) {
            FillFriendFromRow(values, &friends[i]);
        }
    }

    DBQueryResult_Free(&result);
    return count;
}

int CharacFriends_RemoveAll(DBConnectionManager* manager, int charac_no) {
    char query[256];
    snprintf(query, sizeof(query),
             "DELETE FROM charac_friends WHERE charac_no=%d",
             charac_no);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);

    /* 查询执行错误 */
    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    /* 删除成功 */
    if (ret > 0) {
        printf("[CharacFriends ORM] Removed all friends for charac_no=%d, count=%d\n",
               charac_no, ret);
    }

    DBQueryResult_Free(&result);
    return 0;  /* 成功（幂等） */
}

// ========================================================================
// 查询辅助函数实现
// ========================================================================

bool CharacFriends_IsFriend(DBConnectionManager* manager, int charac_no, int friend_no) {
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM charac_friends "
             "WHERE charac_no=%d AND friend_no=%d",
             charac_no, friend_no);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);

    if (ret <= 0) {
        DBQueryResult_Free(&result);
        return false;
    }

    char* values[1];
    if (DBQueryResult_FetchRow(&result, values) <= 0) {
        DBQueryResult_Free(&result);
        return false;
    }

    int count = atoi(values[0]);
    DBQueryResult_Free(&result);

    return (count > 0);
}

int CharacFriends_GetCount(DBConnectionManager* manager, int charac_no) {
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM charac_friends WHERE charac_no=%d",
             charac_no);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);

    if (ret <= 0) {
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

bool CharacFriends_IsMutual(DBConnectionManager* manager, int charac_no1, int charac_no2) {
    /* 检查双向好友关系 */
    bool forward = CharacFriends_IsFriend(manager, charac_no1, charac_no2);
    if (!forward) {
        return false;
    }

    bool backward = CharacFriends_IsFriend(manager, charac_no2, charac_no1);
    return backward;
}

int CharacFriends_GetFans(DBConnectionManager* manager, int charac_no,
                          CharacFriend* fans, int max_count) {
    char query[256];
    /* 查询把该角色加为好友的其他角色 */
    snprintf(query, sizeof(query),
             "SELECT charac_no, friend_no FROM charac_friends "
             "WHERE friend_no=%d ORDER BY charac_no LIMIT %d",
             charac_no, max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(DBQueryResult));
    int ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result);

    /* 查询执行错误 */
    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    /* 无记录 - 返回0不是错误 */
    if (ret == 0) {
        DBQueryResult_Free(&result);
        return 0;
    }

    /* 填充粉丝列表 */
    int count = (result.row_count < max_count) ? result.row_count : max_count;
    char* values[2];
    for (int i = 0; i < count; i++) {
        if (DBQueryResult_FetchRow(&result, values) > 0) {
            FillFriendFromRow(values, &fans[i]);
        }
    }

    DBQueryResult_Free(&result);
    return count;
}

// ========================================================================
// 批量操作实现
// ========================================================================

int CharacFriends_AddBatch(DBConnectionManager* manager, CharacFriend* friends, int count) {
    if (count <= 0 || friends == NULL) {
        return 0;
    }

    int success_count = 0;
    for (int i = 0; i < count; i++) {
        if (CharacFriends_Add(manager, &friends[i]) == 0) {
            success_count++;
        }
    }

    return success_count;
}

// ========================================================================
// 工具函数实现
// ========================================================================

void CharacFriend_Init(CharacFriend* friend) {
    memset(friend, 0, sizeof(CharacFriend));
    friend->charac_no = 0;
    friend->friend_no = 0;
}

void CharacFriend_Print(const CharacFriend* friend) {
    printf("================================================================================\n");
    printf("CharacFriend Information:\n");
    printf("================================================================================\n");
    printf("  角色编号:      %d\n", friend->charac_no);
    printf("  好友编号:      %d\n", friend->friend_no);
    printf("================================================================================\n");
}
