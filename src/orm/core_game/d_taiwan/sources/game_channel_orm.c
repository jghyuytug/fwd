#include "game_channel_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_QUERY_LEN 8192
#define MAX_FIELDS 32

/* ========== CRUD 操作 ========== */

int GameChannel_Add(DBConnectionManager* manager, const GameChannel* channel) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];

    if (!manager || !channel) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "INSERT INTO game_channel (gc_now, gc_ip, gc_port, gc_max, gc_game, gc_channel, "
        "gc_ch_group, gc_channeltype, gc_up_time, gc_swordman_cnt, gc_fighter_cnt, gc_gunner_cnt, "
        "gc_mage_cnt, gc_priest_cnt, gc_at_gunner_cnt, gc_thief_cnt, gc_hangame, gc_nexon, gc_type) "
        "VALUES (%hu, '%s', %hu, %hu, %hhu, '%s', %hd, '%s', '%s', %hu, %hu, %hu, %hu, %hu, %hu, %hu, %hu, %hu, %hhd)",
        channel->gc_now, channel->gc_ip, channel->gc_port, channel->gc_max, channel->gc_game,
        channel->gc_channel, channel->gc_ch_group, channel->gc_channeltype, channel->gc_up_time,
        channel->gc_swordman_cnt, channel->gc_fighter_cnt, channel->gc_gunner_cnt, channel->gc_mage_cnt,
        channel->gc_priest_cnt, channel->gc_at_gunner_cnt, channel->gc_thief_cnt,
        channel->gc_hangame, channel->gc_nexon, channel->gc_type);

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

int GameChannel_Get(DBConnectionManager* manager, unsigned int gc_no, GameChannel* channel) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];

    if (!manager || !channel) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT gc_no, gc_now, gc_ip, gc_port, gc_max, gc_game, gc_channel, gc_ch_group, "
        "gc_channeltype, gc_up_time, gc_swordman_cnt, gc_fighter_cnt, gc_gunner_cnt, gc_mage_cnt, "
        "gc_priest_cnt, gc_at_gunner_cnt, gc_thief_cnt, gc_hangame, gc_nexon, gc_type "
        "FROM game_channel WHERE gc_no=%u", gc_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        channel->gc_no = (unsigned int)atoi(values[0]);
        channel->gc_now = (unsigned short)atoi(values[1]);
        strncpy(channel->gc_ip, values[2], sizeof(channel->gc_ip) - 1);
        channel->gc_port = (unsigned short)atoi(values[3]);
        channel->gc_max = (unsigned short)atoi(values[4]);
        channel->gc_game = (unsigned char)atoi(values[5]);
        strncpy(channel->gc_channel, values[6], sizeof(channel->gc_channel) - 1);
        channel->gc_ch_group = (short)atoi(values[7]);
        strncpy(channel->gc_channeltype, values[8], sizeof(channel->gc_channeltype) - 1);
        strncpy(channel->gc_up_time, values[9], sizeof(channel->gc_up_time) - 1);
        channel->gc_swordman_cnt = (unsigned short)atoi(values[10]);
        channel->gc_fighter_cnt = (unsigned short)atoi(values[11]);
        channel->gc_gunner_cnt = (unsigned short)atoi(values[12]);
        channel->gc_mage_cnt = (unsigned short)atoi(values[13]);
        channel->gc_priest_cnt = (unsigned short)atoi(values[14]);
        channel->gc_at_gunner_cnt = (unsigned short)atoi(values[15]);
        channel->gc_thief_cnt = (unsigned short)atoi(values[16]);
        channel->gc_hangame = (unsigned short)atoi(values[17]);
        channel->gc_nexon = (unsigned short)atoi(values[18]);
        channel->gc_type = (char)atoi(values[19]);
        DBQueryResult_Free(&result);
        return 0;
    }

    DBQueryResult_Free(&result);
    fprintf(stderr, "GameChannel_Get: Record not found (gc_no=%u)\n", gc_no);
    return -1;
}

int GameChannel_Update(DBConnectionManager* manager, const GameChannel* channel) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager || !channel) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE game_channel SET gc_now=%hu, gc_ip='%s', gc_port=%hu, gc_max=%hu, gc_game=%hhu, "
        "gc_channel='%s', gc_ch_group=%hd, gc_channeltype='%s', gc_up_time='%s', "
        "gc_swordman_cnt=%hu, gc_fighter_cnt=%hu, gc_gunner_cnt=%hu, gc_mage_cnt=%hu, "
        "gc_priest_cnt=%hu, gc_at_gunner_cnt=%hu, gc_thief_cnt=%hu, gc_hangame=%hu, "
        "gc_nexon=%hu, gc_type=%hhd WHERE gc_no=%u",
        channel->gc_now, channel->gc_ip, channel->gc_port, channel->gc_max, channel->gc_game,
        channel->gc_channel, channel->gc_ch_group, channel->gc_channeltype, channel->gc_up_time,
        channel->gc_swordman_cnt, channel->gc_fighter_cnt, channel->gc_gunner_cnt, channel->gc_mage_cnt,
        channel->gc_priest_cnt, channel->gc_at_gunner_cnt, channel->gc_thief_cnt,
        channel->gc_hangame, channel->gc_nexon, channel->gc_type, channel->gc_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GameChannel_Delete(DBConnectionManager* manager, unsigned int gc_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query), "DELETE FROM game_channel WHERE gc_no=%u", gc_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GameChannel_Exists(DBConnectionManager* manager, unsigned int gc_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int count = 0;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM game_channel WHERE gc_no=%u", gc_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count > 0 ? 1 : 0;
}

/* ========== 专项操作 ========== */

int GameChannel_UpdateOnlineCount(DBConnectionManager* manager, unsigned int gc_no, unsigned short gc_now) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE game_channel SET gc_now=%hu WHERE gc_no=%u", gc_now, gc_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GameChannel_UpdateJobCounts(DBConnectionManager* manager, unsigned int gc_no,
                                unsigned short swordman, unsigned short fighter, unsigned short gunner,
                                unsigned short mage, unsigned short priest, unsigned short at_gunner,
                                unsigned short thief) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE game_channel SET gc_swordman_cnt=%hu, gc_fighter_cnt=%hu, gc_gunner_cnt=%hu, "
        "gc_mage_cnt=%hu, gc_priest_cnt=%hu, gc_at_gunner_cnt=%hu, gc_thief_cnt=%hu "
        "WHERE gc_no=%u",
        swordman, fighter, gunner, mage, priest, at_gunner, thief, gc_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GameChannel_UpdateTimestamp(DBConnectionManager* manager, unsigned int gc_no) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "UPDATE game_channel SET gc_up_time=NOW() WHERE gc_no=%u", gc_no);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* ========== 查询操作 ========== */

int GameChannel_Count(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int count = 0;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM game_channel");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

int GameChannel_CountByGroup(DBConnectionManager* manager, short gc_ch_group) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int count = 0;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query),
        "SELECT COUNT(*) FROM game_channel WHERE gc_ch_group=%hd", gc_ch_group);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

int GameChannel_GetTotalOnline(DBConnectionManager* manager) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int total = 0;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query), "SELECT SUM(gc_now) FROM game_channel");

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0 && values[0] != NULL) {
        total = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return total;
}

int GameChannel_GetTotalJobCount(DBConnectionManager* manager, int job_type) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;
    char* values[MAX_FIELDS];
    int total = 0;
    const char* job_field;

    if (!manager) {
        return -1;
    }

    /* 根据职业类型选择对应字段 */
    switch (job_type) {
        case 0: job_field = "gc_swordman_cnt"; break;
        case 1: job_field = "gc_fighter_cnt"; break;
        case 2: job_field = "gc_gunner_cnt"; break;
        case 3: job_field = "gc_mage_cnt"; break;
        case 4: job_field = "gc_priest_cnt"; break;
        case 5: job_field = "gc_at_gunner_cnt"; break;
        case 6: job_field = "gc_thief_cnt"; break;
        default: return -1;
    }

    snprintf(query, sizeof(query), "SELECT SUM(%s) FROM game_channel", job_field);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    if (DBQueryResult_FetchRow(&result, values) > 0 && values[0] != NULL) {
        total = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return total;
}

int GameChannel_DeleteByGroup(DBConnectionManager* manager, short gc_ch_group) {
    char query[MAX_QUERY_LEN];
    DBQueryResult result;

    if (!manager) {
        return -1;
    }

    snprintf(query, sizeof(query), "DELETE FROM game_channel WHERE gc_ch_group=%hd", gc_ch_group);

    if (DBConnectionManager_ExecuteQuery(manager, DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    DBQueryResult_Free(&result);
    return 0;
}

/* ========== 工具函数 ========== */

void GameChannel_PrintInfo(const GameChannel* channel) {
    if (!channel) {
        printf("GameChannel: NULL\n");
        return;
    }

    printf("========================================\n");
    printf("Game Channel Information\n");
    printf("========================================\n");
    printf("Channel No    : %u\n", channel->gc_no);
    printf("Online Now    : %hu\n", channel->gc_now);
    printf("IP:Port       : %s:%hu\n", channel->gc_ip, channel->gc_port);
    printf("Max Capacity  : %hu\n", channel->gc_max);
    printf("Game Type     : %hhu\n", channel->gc_game);
    printf("Channel Name  : %s\n", channel->gc_channel);
    printf("Channel Group : %hd\n", channel->gc_ch_group);
    printf("Channel Type  : %s\n", channel->gc_channeltype);
    printf("Update Time   : %s\n", channel->gc_up_time);
    printf("----------------------------------------\n");
    printf("Job Counts:\n");
    printf("  Swordman    : %hu\n", channel->gc_swordman_cnt);
    printf("  Fighter     : %hu\n", channel->gc_fighter_cnt);
    printf("  Gunner      : %hu\n", channel->gc_gunner_cnt);
    printf("  Mage        : %hu\n", channel->gc_mage_cnt);
    printf("  Priest      : %hu\n", channel->gc_priest_cnt);
    printf("  At_Gunner   : %hu\n", channel->gc_at_gunner_cnt);
    printf("  Thief       : %hu\n", channel->gc_thief_cnt);
    printf("----------------------------------------\n");
    printf("Platform:\n");
    printf("  Hangame     : %hu\n", channel->gc_hangame);
    printf("  Nexon       : %hu\n", channel->gc_nexon);
    printf("Type          : %hhd\n", channel->gc_type);
    printf("========================================\n");
}
