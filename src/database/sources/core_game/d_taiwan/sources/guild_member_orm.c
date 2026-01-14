/**
 * DNF Game Server - GuildMember ORM Implementation
 *
 * Database: test_d_guild
 * Table: guild_member (24 fields, composite key)
 */

#include "guild_member_orm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ========================================================================
 * Utility Functions
 * ======================================================================== */

void GuildMember_Init(GuildMember* member) {
    if (!member) return;

    member->guild_id = 0;
    member->m_id = 0;
    member->server_id = 0;
    member->charac_no = 0;
    memset(member->charac_name, 0, sizeof(member->charac_name));
    memset(member->memo, 0, sizeof(member->memo));
    member->grade = 0;
    member->job = 0;
    member->grow_type = 0;
    member->lev = 0;
    member->age = 0;
    memset(member->born_year, 0, sizeof(member->born_year));
    memset(member->sex, 0, sizeof(member->sex));
    memset(member->apply_time, 0, sizeof(member->apply_time));
    memset(member->member_time, 0, sizeof(member->member_time));
    member->member_flag = 0;
    member->bbs_cnt = 0;
    memset(member->last_visit_time, 0, sizeof(member->last_visit_time));
    member->secede_type = 0;
    memset(member->secede_time, 0, sizeof(member->secede_time));
    member->member_point = 0;
    member->member_point_prev = 0;
    memset(member->last_play_time, 0, sizeof(member->last_play_time));
    memset(member->nick_name, 0, sizeof(member->nick_name));
}

void GuildMember_Print(const GuildMember* member) {
    if (!member) return;

    printf("GuildMember Information:\n");
    printf("  Guild ID: %d\n", member->guild_id);
    printf("  Member ID: %d\n", member->m_id);
    printf("  Server ID: %d\n", (int)member->server_id);
    printf("  Character No: %d\n", member->charac_no);
    printf("  Character Name: %s\n", member->charac_name);
    printf("  Nickname: %s\n", member->nick_name);
    printf("  Grade: %d\n", (int)member->grade);
    printf("  Level: %d\n", (int)member->lev);
    printf("  Job: %d\n", (int)member->job);
    printf("  Member Flag: %d\n", (int)member->member_flag);
    printf("  Member Points: %u\n", member->member_point);
    printf("  BBS Count: %u\n", member->bbs_cnt);
    printf("  Memo: %s\n", member->memo);
}

int GuildMember_Copy(GuildMember* dest, const GuildMember* src) {
    if (!dest || !src) return -1;
    memcpy(dest, src, sizeof(GuildMember));
    return 0;
}

/* ========================================================================
 * CRUD Operations
 * ======================================================================== */

int GuildMember_Create(DBConnectionManager* manager, GuildMember* member) {
    char query[2048];
    DBQueryResult result;
    int ret;

    if (!manager || !member) return -1;

    snprintf(query, sizeof(query),
             "INSERT INTO guild_member (guild_id, m_id, server_id, charac_no, charac_name, memo, "
             "grade, job, grow_type, lev, age, born_year, sex, apply_time, member_time, "
             "member_flag, bbs_cnt, last_visit_time, secede_type, secede_time, member_point, "
             "member_point_prev, last_play_time, nick_name) VALUES "
             "(%d, %d, %d, %d, '%s', '%s', %d, %d, %d, %d, %d, '%s', '%s', '%s', '%s', "
             "%d, %u, '%s', %d, '%s', %u, %u, '%s', '%s')",
             member->guild_id, member->m_id, (int)member->server_id, member->charac_no,
             member->charac_name, member->memo, (int)member->grade, (int)member->job,
             (int)member->grow_type, (int)member->lev, (int)member->age, member->born_year,
             member->sex, member->apply_time, member->member_time, (int)member->member_flag,
             member->bbs_cnt, member->last_visit_time, (int)member->secede_type,
             member->secede_time, member->member_point, member->member_point_prev,
             member->last_play_time, member->nick_name);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);

    if (ret > 0) {
        printf("[GuildMember ORM] Created member for guild_id=%d, charac_no=%d\n",
               member->guild_id, member->charac_no);
        ret = 0;
    } else {
        printf("[GuildMember ORM] Failed to create member for guild_id=%d, charac_no=%d\n",
               member->guild_id, member->charac_no);
        ret = -1;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int GuildMember_GetByKey(DBConnectionManager* manager, int guild_id, int charac_no, GuildMember* member) {
    char query[1024];
    char* values[24];
    DBQueryResult result;
    int ret;

    if (!manager || !member) return -1;

    snprintf(query, sizeof(query),
             "SELECT guild_id, m_id, server_id, charac_no, charac_name, memo, grade, job, "
             "grow_type, lev, age, born_year, sex, apply_time, member_time, member_flag, "
             "bbs_cnt, last_visit_time, secede_type, secede_time, member_point, member_point_prev, "
             "last_play_time, nick_name FROM guild_member WHERE guild_id=%d AND charac_no=%d",
             guild_id, charac_no);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);

    if (ret > 0 && DBQueryResult_FetchRow(&result, values) > 0) {
        member->guild_id = atoi(values[0]);
        member->m_id = atoi(values[1]);
        member->server_id = (char)atoi(values[2]);
        member->charac_no = atoi(values[3]);
        strncpy(member->charac_name, values[4] ? values[4] : "", sizeof(member->charac_name) - 1);
        strncpy(member->memo, values[5] ? values[5] : "", sizeof(member->memo) - 1);
        member->grade = (char)atoi(values[6]);
        member->job = (char)atoi(values[7]);
        member->grow_type = (char)atoi(values[8]);
        member->lev = (char)atoi(values[9]);
        member->age = (char)atoi(values[10]);
        strncpy(member->born_year, values[11] ? values[11] : "", sizeof(member->born_year) - 1);
        strncpy(member->sex, values[12] ? values[12] : "", sizeof(member->sex) - 1);
        strncpy(member->apply_time, values[13] ? values[13] : "", sizeof(member->apply_time) - 1);
        strncpy(member->member_time, values[14] ? values[14] : "", sizeof(member->member_time) - 1);
        member->member_flag = (char)atoi(values[15]);
        member->bbs_cnt = (unsigned short)atoi(values[16]);
        strncpy(member->last_visit_time, values[17] ? values[17] : "", sizeof(member->last_visit_time) - 1);
        member->secede_type = (char)atoi(values[18]);
        strncpy(member->secede_time, values[19] ? values[19] : "", sizeof(member->secede_time) - 1);
        member->member_point = (unsigned int)atoi(values[20]);
        member->member_point_prev = (unsigned int)atoi(values[21]);
        strncpy(member->last_play_time, values[22] ? values[22] : "", sizeof(member->last_play_time) - 1);
        strncpy(member->nick_name, values[23] ? values[23] : "", sizeof(member->nick_name) - 1);
        ret = 0;
    } else {
        ret = -1;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int GuildMember_Update(DBConnectionManager* manager, GuildMember* member) {
    char query[2048];
    DBQueryResult result;
    int ret;

    if (!manager || !member) return -1;

    snprintf(query, sizeof(query),
             "UPDATE guild_member SET m_id=%d, server_id=%d, charac_name='%s', memo='%s', "
             "grade=%d, job=%d, grow_type=%d, lev=%d, age=%d, born_year='%s', sex='%s', "
             "apply_time='%s', member_time='%s', member_flag=%d, bbs_cnt=%u, "
             "last_visit_time='%s', secede_type=%d, secede_time='%s', member_point=%u, "
             "member_point_prev=%u, last_play_time='%s', nick_name='%s' "
             "WHERE guild_id=%d AND charac_no=%d",
             member->m_id, (int)member->server_id, member->charac_name, member->memo,
             (int)member->grade, (int)member->job, (int)member->grow_type, (int)member->lev,
             (int)member->age, member->born_year, member->sex, member->apply_time,
             member->member_time, (int)member->member_flag, member->bbs_cnt,
             member->last_visit_time, (int)member->secede_type, member->secede_time,
             member->member_point, member->member_point_prev, member->last_play_time,
             member->nick_name, member->guild_id, member->charac_no);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);

    /* Query execution error */
    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    /* Update succeeded (even if 0 rows affected - idempotent) */
    if (ret > 0) {
        printf("[GuildMember ORM] Updated member guild_id=%d, charac_no=%d\n",
               member->guild_id, member->charac_no);
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GuildMember_Delete(DBConnectionManager* manager, int guild_id, int charac_no) {
    char query[256];
    DBQueryResult result;
    int ret;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
             "DELETE FROM guild_member WHERE guild_id=%d AND charac_no=%d",
             guild_id, charac_no);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);

    if (ret > 0) {
        printf("[GuildMember ORM] Deleted member guild_id=%d, charac_no=%d\n",
               guild_id, charac_no);
        ret = 0;
    } else {
        ret = -1;
    }

    DBQueryResult_Free(&result);
    return ret;
}

/* ========================================================================
 * Auxiliary Query Functions
 * ======================================================================== */

bool GuildMember_Exists(DBConnectionManager* manager, int guild_id, int charac_no) {
    char query[256];
    char* values[1];
    DBQueryResult result;
    int ret;
    bool exists = false;

    if (!manager) return false;

    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM guild_member WHERE guild_id=%d AND charac_no=%d",
             guild_id, charac_no);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);

    if (ret > 0 && DBQueryResult_FetchRow(&result, values) > 0) {
        exists = (atoi(values[0]) > 0);
    }

    DBQueryResult_Free(&result);
    return exists;
}

int GuildMember_GetByGuildID(DBConnectionManager* manager, int guild_id, GuildMember* members, int max_count) {
    char query[1024];
    char* values[24];
    DBQueryResult result;
    int ret;
    int count = 0;

    if (!manager || !members || max_count <= 0) return -1;

    snprintf(query, sizeof(query),
             "SELECT guild_id, m_id, server_id, charac_no, charac_name, memo, grade, job, "
             "grow_type, lev, age, born_year, sex, apply_time, member_time, member_flag, "
             "bbs_cnt, last_visit_time, secede_type, secede_time, member_point, member_point_prev, "
             "last_play_time, nick_name FROM guild_member WHERE guild_id=%d LIMIT %d",
             guild_id, max_count);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);

    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, values) > 0) {
        GuildMember* member = &members[count];

        member->guild_id = atoi(values[0]);
        member->m_id = atoi(values[1]);
        member->server_id = (char)atoi(values[2]);
        member->charac_no = atoi(values[3]);
        strncpy(member->charac_name, values[4] ? values[4] : "", sizeof(member->charac_name) - 1);
        strncpy(member->memo, values[5] ? values[5] : "", sizeof(member->memo) - 1);
        member->grade = (char)atoi(values[6]);
        member->job = (char)atoi(values[7]);
        member->grow_type = (char)atoi(values[8]);
        member->lev = (char)atoi(values[9]);
        member->age = (char)atoi(values[10]);
        strncpy(member->born_year, values[11] ? values[11] : "", sizeof(member->born_year) - 1);
        strncpy(member->sex, values[12] ? values[12] : "", sizeof(member->sex) - 1);
        strncpy(member->apply_time, values[13] ? values[13] : "", sizeof(member->apply_time) - 1);
        strncpy(member->member_time, values[14] ? values[14] : "", sizeof(member->member_time) - 1);
        member->member_flag = (char)atoi(values[15]);
        member->bbs_cnt = (unsigned short)atoi(values[16]);
        strncpy(member->last_visit_time, values[17] ? values[17] : "", sizeof(member->last_visit_time) - 1);
        member->secede_type = (char)atoi(values[18]);
        strncpy(member->secede_time, values[19] ? values[19] : "", sizeof(member->secede_time) - 1);
        member->member_point = (unsigned int)atoi(values[20]);
        member->member_point_prev = (unsigned int)atoi(values[21]);
        strncpy(member->last_play_time, values[22] ? values[22] : "", sizeof(member->last_play_time) - 1);
        strncpy(member->nick_name, values[23] ? values[23] : "", sizeof(member->nick_name) - 1);

        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int GuildMember_GetByCharacNo(DBConnectionManager* manager, int charac_no, GuildMember* member) {
    char query[1024];
    char* values[24];
    DBQueryResult result;
    int ret;

    if (!manager || !member) return -1;

    snprintf(query, sizeof(query),
             "SELECT guild_id, m_id, server_id, charac_no, charac_name, memo, grade, job, "
             "grow_type, lev, age, born_year, sex, apply_time, member_time, member_flag, "
             "bbs_cnt, last_visit_time, secede_type, secede_time, member_point, member_point_prev, "
             "last_play_time, nick_name FROM guild_member WHERE charac_no=%d",
             charac_no);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);

    if (ret > 0 && DBQueryResult_FetchRow(&result, values) > 0) {
        member->guild_id = atoi(values[0]);
        member->m_id = atoi(values[1]);
        member->server_id = (char)atoi(values[2]);
        member->charac_no = atoi(values[3]);
        strncpy(member->charac_name, values[4] ? values[4] : "", sizeof(member->charac_name) - 1);
        strncpy(member->memo, values[5] ? values[5] : "", sizeof(member->memo) - 1);
        member->grade = (char)atoi(values[6]);
        member->job = (char)atoi(values[7]);
        member->grow_type = (char)atoi(values[8]);
        member->lev = (char)atoi(values[9]);
        member->age = (char)atoi(values[10]);
        strncpy(member->born_year, values[11] ? values[11] : "", sizeof(member->born_year) - 1);
        strncpy(member->sex, values[12] ? values[12] : "", sizeof(member->sex) - 1);
        strncpy(member->apply_time, values[13] ? values[13] : "", sizeof(member->apply_time) - 1);
        strncpy(member->member_time, values[14] ? values[14] : "", sizeof(member->member_time) - 1);
        member->member_flag = (char)atoi(values[15]);
        member->bbs_cnt = (unsigned short)atoi(values[16]);
        strncpy(member->last_visit_time, values[17] ? values[17] : "", sizeof(member->last_visit_time) - 1);
        member->secede_type = (char)atoi(values[18]);
        strncpy(member->secede_time, values[19] ? values[19] : "", sizeof(member->secede_time) - 1);
        member->member_point = (unsigned int)atoi(values[20]);
        member->member_point_prev = (unsigned int)atoi(values[21]);
        strncpy(member->last_play_time, values[22] ? values[22] : "", sizeof(member->last_play_time) - 1);
        strncpy(member->nick_name, values[23] ? values[23] : "", sizeof(member->nick_name) - 1);
        ret = 0;
    } else {
        ret = -1;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int GuildMember_GetByCharacName(DBConnectionManager* manager, const char* charac_name, GuildMember* member) {
    char query[1024];
    char* values[24];
    DBQueryResult result;
    int ret;

    if (!manager || !charac_name || !member) return -1;

    snprintf(query, sizeof(query),
             "SELECT guild_id, m_id, server_id, charac_no, charac_name, memo, grade, job, "
             "grow_type, lev, age, born_year, sex, apply_time, member_time, member_flag, "
             "bbs_cnt, last_visit_time, secede_type, secede_time, member_point, member_point_prev, "
             "last_play_time, nick_name FROM guild_member WHERE charac_name='%s'",
             charac_name);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);

    if (ret > 0 && DBQueryResult_FetchRow(&result, values) > 0) {
        member->guild_id = atoi(values[0]);
        member->m_id = atoi(values[1]);
        member->server_id = (char)atoi(values[2]);
        member->charac_no = atoi(values[3]);
        strncpy(member->charac_name, values[4] ? values[4] : "", sizeof(member->charac_name) - 1);
        strncpy(member->memo, values[5] ? values[5] : "", sizeof(member->memo) - 1);
        member->grade = (char)atoi(values[6]);
        member->job = (char)atoi(values[7]);
        member->grow_type = (char)atoi(values[8]);
        member->lev = (char)atoi(values[9]);
        member->age = (char)atoi(values[10]);
        strncpy(member->born_year, values[11] ? values[11] : "", sizeof(member->born_year) - 1);
        strncpy(member->sex, values[12] ? values[12] : "", sizeof(member->sex) - 1);
        strncpy(member->apply_time, values[13] ? values[13] : "", sizeof(member->apply_time) - 1);
        strncpy(member->member_time, values[14] ? values[14] : "", sizeof(member->member_time) - 1);
        member->member_flag = (char)atoi(values[15]);
        member->bbs_cnt = (unsigned short)atoi(values[16]);
        strncpy(member->last_visit_time, values[17] ? values[17] : "", sizeof(member->last_visit_time) - 1);
        member->secede_type = (char)atoi(values[18]);
        strncpy(member->secede_time, values[19] ? values[19] : "", sizeof(member->secede_time) - 1);
        member->member_point = (unsigned int)atoi(values[20]);
        member->member_point_prev = (unsigned int)atoi(values[21]);
        strncpy(member->last_play_time, values[22] ? values[22] : "", sizeof(member->last_play_time) - 1);
        strncpy(member->nick_name, values[23] ? values[23] : "", sizeof(member->nick_name) - 1);
        ret = 0;
    } else {
        ret = -1;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int GuildMember_UpdateGrade(DBConnectionManager* manager, int guild_id, int charac_no, char grade) {
    char query[256];
    DBQueryResult result;
    int ret;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
             "UPDATE guild_member SET grade=%d WHERE guild_id=%d AND charac_no=%d",
             (int)grade, guild_id, charac_no);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);

    if (ret > 0) {
        printf("[GuildMember ORM] Updated grade for guild_id=%d, charac_no=%d\n",
               guild_id, charac_no);
        ret = 0;
    } else {
        ret = -1;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int GuildMember_UpdateMemo(DBConnectionManager* manager, int guild_id, int charac_no, const char* memo) {
    char query[512];
    DBQueryResult result;
    int ret;

    if (!manager || !memo) return -1;

    snprintf(query, sizeof(query),
             "UPDATE guild_member SET memo='%s' WHERE guild_id=%d AND charac_no=%d",
             memo, guild_id, charac_no);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);

    /* Query execution error */
    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    /* Update succeeded (even if 0 rows affected - idempotent) */
    if (ret > 0) {
        printf("[GuildMember ORM] Updated memo for guild_id=%d, charac_no=%d\n",
               guild_id, charac_no);
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GuildMember_UpdateFlag(DBConnectionManager* manager, int guild_id, int charac_no, char flag) {
    char query[256];
    DBQueryResult result;
    int ret;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
             "UPDATE guild_member SET member_flag=%d WHERE guild_id=%d AND charac_no=%d",
             (int)flag, guild_id, charac_no);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);

    if (ret > 0) {
        printf("[GuildMember ORM] Updated flag for guild_id=%d, charac_no=%d\n",
               guild_id, charac_no);
        ret = 0;
    } else {
        ret = -1;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int GuildMember_UpdateLastVisit(DBConnectionManager* manager, int guild_id, int charac_no) {
    char query[256];
    DBQueryResult result;
    int ret;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
             "UPDATE guild_member SET last_visit_time=NOW() WHERE guild_id=%d AND charac_no=%d",
             guild_id, charac_no);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);

    if (ret > 0) {
        printf("[GuildMember ORM] Updated last visit for guild_id=%d, charac_no=%d\n",
               guild_id, charac_no);
        ret = 0;
    } else {
        ret = -1;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int GuildMember_IncrementBBSCount(DBConnectionManager* manager, int guild_id, int charac_no) {
    char query[256];
    DBQueryResult result;
    int ret;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
             "UPDATE guild_member SET bbs_cnt=bbs_cnt+1 WHERE guild_id=%d AND charac_no=%d",
             guild_id, charac_no);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);

    if (ret > 0) {
        printf("[GuildMember ORM] Incremented BBS count for guild_id=%d, charac_no=%d\n",
               guild_id, charac_no);
        ret = 0;
    } else {
        ret = -1;
    }

    DBQueryResult_Free(&result);
    return ret;
}

int GuildMember_UpdatePoints(DBConnectionManager* manager, int guild_id, int charac_no, unsigned int points) {
    char query[256];
    DBQueryResult result;
    int ret;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
             "UPDATE guild_member SET member_point_prev=member_point, member_point=%u "
             "WHERE guild_id=%d AND charac_no=%d",
             points, guild_id, charac_no);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);

    /* Query execution error */
    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    /* Update succeeded (even if 0 rows affected - idempotent) */
    if (ret > 0) {
        printf("[GuildMember ORM] Updated points for guild_id=%d, charac_no=%d\n",
               guild_id, charac_no);
    }

    DBQueryResult_Free(&result);
    return 0;
}

int GuildMember_GetCount(DBConnectionManager* manager, int guild_id) {
    char query[256];
    char* values[1];
    DBQueryResult result;
    int ret;
    int count = 0;

    if (!manager) return -1;

    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM guild_member WHERE guild_id=%d",
             guild_id);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);

    if (ret > 0 && DBQueryResult_FetchRow(&result, values) > 0) {
        count = atoi(values[0]);
    }

    DBQueryResult_Free(&result);
    return count;
}

int GuildMember_GetActiveMembers(DBConnectionManager* manager, int guild_id, GuildMember* members, int max_count) {
    char query[1024];
    char* values[24];
    DBQueryResult result;
    int ret;
    int count = 0;

    if (!manager || !members || max_count <= 0) return -1;

    snprintf(query, sizeof(query),
             "SELECT guild_id, m_id, server_id, charac_no, charac_name, memo, grade, job, "
             "grow_type, lev, age, born_year, sex, apply_time, member_time, member_flag, "
             "bbs_cnt, last_visit_time, secede_type, secede_time, member_point, member_point_prev, "
             "last_play_time, nick_name FROM guild_member WHERE guild_id=%d AND member_flag!=0 LIMIT %d",
             guild_id, max_count);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);

    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, values) > 0) {
        GuildMember* member = &members[count];

        member->guild_id = atoi(values[0]);
        member->m_id = atoi(values[1]);
        member->server_id = (char)atoi(values[2]);
        member->charac_no = atoi(values[3]);
        strncpy(member->charac_name, values[4] ? values[4] : "", sizeof(member->charac_name) - 1);
        strncpy(member->memo, values[5] ? values[5] : "", sizeof(member->memo) - 1);
        member->grade = (char)atoi(values[6]);
        member->job = (char)atoi(values[7]);
        member->grow_type = (char)atoi(values[8]);
        member->lev = (char)atoi(values[9]);
        member->age = (char)atoi(values[10]);
        strncpy(member->born_year, values[11] ? values[11] : "", sizeof(member->born_year) - 1);
        strncpy(member->sex, values[12] ? values[12] : "", sizeof(member->sex) - 1);
        strncpy(member->apply_time, values[13] ? values[13] : "", sizeof(member->apply_time) - 1);
        strncpy(member->member_time, values[14] ? values[14] : "", sizeof(member->member_time) - 1);
        member->member_flag = (char)atoi(values[15]);
        member->bbs_cnt = (unsigned short)atoi(values[16]);
        strncpy(member->last_visit_time, values[17] ? values[17] : "", sizeof(member->last_visit_time) - 1);
        member->secede_type = (char)atoi(values[18]);
        strncpy(member->secede_time, values[19] ? values[19] : "", sizeof(member->secede_time) - 1);
        member->member_point = (unsigned int)atoi(values[20]);
        member->member_point_prev = (unsigned int)atoi(values[21]);
        strncpy(member->last_play_time, values[22] ? values[22] : "", sizeof(member->last_play_time) - 1);
        strncpy(member->nick_name, values[23] ? values[23] : "", sizeof(member->nick_name) - 1);

        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int GuildMember_GetByGrade(DBConnectionManager* manager, int guild_id, char grade, GuildMember* members, int max_count) {
    char query[1024];
    char* values[24];
    DBQueryResult result;
    int ret;
    int count = 0;

    if (!manager || !members || max_count <= 0) return -1;

    snprintf(query, sizeof(query),
             "SELECT guild_id, m_id, server_id, charac_no, charac_name, memo, grade, job, "
             "grow_type, lev, age, born_year, sex, apply_time, member_time, member_flag, "
             "bbs_cnt, last_visit_time, secede_type, secede_time, member_point, member_point_prev, "
             "last_play_time, nick_name FROM guild_member WHERE guild_id=%d AND grade=%d LIMIT %d",
             guild_id, (int)grade, max_count);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);

    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, values) > 0) {
        GuildMember* member = &members[count];

        member->guild_id = atoi(values[0]);
        member->m_id = atoi(values[1]);
        member->server_id = (char)atoi(values[2]);
        member->charac_no = atoi(values[3]);
        strncpy(member->charac_name, values[4] ? values[4] : "", sizeof(member->charac_name) - 1);
        strncpy(member->memo, values[5] ? values[5] : "", sizeof(member->memo) - 1);
        member->grade = (char)atoi(values[6]);
        member->job = (char)atoi(values[7]);
        member->grow_type = (char)atoi(values[8]);
        member->lev = (char)atoi(values[9]);
        member->age = (char)atoi(values[10]);
        strncpy(member->born_year, values[11] ? values[11] : "", sizeof(member->born_year) - 1);
        strncpy(member->sex, values[12] ? values[12] : "", sizeof(member->sex) - 1);
        strncpy(member->apply_time, values[13] ? values[13] : "", sizeof(member->apply_time) - 1);
        strncpy(member->member_time, values[14] ? values[14] : "", sizeof(member->member_time) - 1);
        member->member_flag = (char)atoi(values[15]);
        member->bbs_cnt = (unsigned short)atoi(values[16]);
        strncpy(member->last_visit_time, values[17] ? values[17] : "", sizeof(member->last_visit_time) - 1);
        member->secede_type = (char)atoi(values[18]);
        strncpy(member->secede_time, values[19] ? values[19] : "", sizeof(member->secede_time) - 1);
        member->member_point = (unsigned int)atoi(values[20]);
        member->member_point_prev = (unsigned int)atoi(values[21]);
        strncpy(member->last_play_time, values[22] ? values[22] : "", sizeof(member->last_play_time) - 1);
        strncpy(member->nick_name, values[23] ? values[23] : "", sizeof(member->nick_name) - 1);

        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}

int GuildMember_GetTopByPoints(DBConnectionManager* manager, int guild_id, GuildMember* members, int max_count) {
    char query[1024];
    char* values[24];
    DBQueryResult result;
    int ret;
    int count = 0;

    if (!manager || !members || max_count <= 0) return -1;

    snprintf(query, sizeof(query),
             "SELECT guild_id, m_id, server_id, charac_no, charac_name, memo, grade, job, "
             "grow_type, lev, age, born_year, sex, apply_time, member_time, member_flag, "
             "bbs_cnt, last_visit_time, secede_type, secede_time, member_point, member_point_prev, "
             "last_play_time, nick_name FROM guild_member WHERE guild_id=%d "
             "ORDER BY member_point DESC LIMIT %d",
             guild_id, max_count);

    memset(&result, 0, sizeof(DBQueryResult));
    ret = DBConnectionManager_ExecuteQuery(manager, DB_TYPE_GUILD, query, &result);

    if (ret < 0) {
        DBQueryResult_Free(&result);
        return -1;
    }

    while (count < max_count && DBQueryResult_FetchRow(&result, values) > 0) {
        GuildMember* member = &members[count];

        member->guild_id = atoi(values[0]);
        member->m_id = atoi(values[1]);
        member->server_id = (char)atoi(values[2]);
        member->charac_no = atoi(values[3]);
        strncpy(member->charac_name, values[4] ? values[4] : "", sizeof(member->charac_name) - 1);
        strncpy(member->memo, values[5] ? values[5] : "", sizeof(member->memo) - 1);
        member->grade = (char)atoi(values[6]);
        member->job = (char)atoi(values[7]);
        member->grow_type = (char)atoi(values[8]);
        member->lev = (char)atoi(values[9]);
        member->age = (char)atoi(values[10]);
        strncpy(member->born_year, values[11] ? values[11] : "", sizeof(member->born_year) - 1);
        strncpy(member->sex, values[12] ? values[12] : "", sizeof(member->sex) - 1);
        strncpy(member->apply_time, values[13] ? values[13] : "", sizeof(member->apply_time) - 1);
        strncpy(member->member_time, values[14] ? values[14] : "", sizeof(member->member_time) - 1);
        member->member_flag = (char)atoi(values[15]);
        member->bbs_cnt = (unsigned short)atoi(values[16]);
        strncpy(member->last_visit_time, values[17] ? values[17] : "", sizeof(member->last_visit_time) - 1);
        member->secede_type = (char)atoi(values[18]);
        strncpy(member->secede_time, values[19] ? values[19] : "", sizeof(member->secede_time) - 1);
        member->member_point = (unsigned int)atoi(values[20]);
        member->member_point_prev = (unsigned int)atoi(values[21]);
        strncpy(member->last_play_time, values[22] ? values[22] : "", sizeof(member->last_play_time) - 1);
        strncpy(member->nick_name, values[23] ? values[23] : "", sizeof(member->nick_name) - 1);

        count++;
    }

    DBQueryResult_Free(&result);
    return count;
}
