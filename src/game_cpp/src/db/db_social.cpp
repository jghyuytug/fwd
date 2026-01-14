#include "db/db_social.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <vector>
#include "database/headers/db_connection_manager.h"
#include "game_server_context.h"
#include "charac_friends_orm.h"
#include "guild_info_orm.h"
#include "guild_member_orm.h"

static int run_query(int db_type, const char* query, DBQueryResult* result) {
    if (DBConnectionManager_ExecuteQuery(&g_db_manager, db_type, query, result) < 0) {
        return -1;
    }
    return 0;
}

static int run_non_query(int db_type, const char* query) {
    DBQueryResult result;
    memset(&result, 0, sizeof(result));
    if (run_query(db_type, query, &result) < 0) {
        return -1;
    }
    int affected = result.row_count;
    DBQueryResult_Free(&result);
    return affected;
}

static int run_insert(int db_type, const char* query, unsigned long long* insert_id) {
    if (DBConnectionManager_ExecuteInsert(&g_db_manager, db_type, query, insert_id) < 0) {
        return -1;
    }
    return 0;
}

static void sanitize_name(char* dest, const char* src, size_t max_len) {
    memset(dest, 0, max_len);
    if (src) {
        strncpy(dest, src, max_len - 1);
    }
    for (size_t i = 0; i < max_len - 1 && dest[i]; i++) {
        if (dest[i] == '\'' || dest[i] == '\\') {
            dest[i] = '_';
        }
    }
}

static void format_time_now(char* dest, size_t max_len) {
    if (!dest || max_len == 0) return;
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    if (!tm_info) {
        dest[0] = '\0';
        return;
    }
    strftime(dest, max_len, "%Y-%m-%d %H:%M:%S", tm_info);
}

int db_get_mail_list(uint32_t charac_no, DBMailInfo* mails, int max_count) {
    if (!mails || max_count <= 0) return 0;

    char query[256];
    snprintf(query, sizeof(query),
                  "SELECT postal_id, send_charac_no, send_charac_name, item_id, gold, "
                  "UNIX_TIMESTAMP(occ_time), delete_flag "
                  "FROM postal WHERE receive_charac_no=%u AND delete_flag=0 "
                  "ORDER BY occ_time DESC LIMIT %d",
                  charac_no, max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(result));
    if (run_query(DB_TYPE_CAIN, query, &result) < 0) {
        printf("[DB] Mail query failed for charac_no=%u\n", charac_no);
        return 0;
    }

    char* row[7] = {0};
    int count = 0;
    while (count < max_count && DBQueryResult_FetchRow(&result, row) == 7) {
        DBMailInfo* m = &mails[count];
        memset(m, 0, sizeof(DBMailInfo));
        m->postal_id = row[0] ? atoi(row[0]) : 0;
        m->send_charac_no = row[1] ? atoi(row[1]) : 0;
        if (row[2]) strncpy(m->send_name, row[2], sizeof(m->send_name) - 1);
        m->item_id = row[3] ? atoi(row[3]) : 0;
        m->gold = row[4] ? atoi(row[4]) : 0;
        m->occ_time = row[5] ? atoi(row[5]) : 0;
        m->read_flag = 0;
        count++;
    }

    DBQueryResult_Free(&result);
    printf("[DB] Loaded %d mails for charac_no=%u\n", count, charac_no);
    return count;
}

int db_get_unread_mail_count(uint32_t charac_no) {
    char query[256];
    snprintf(query, sizeof(query),
                  "SELECT COUNT(*) FROM postal WHERE receive_charac_no=%u AND delete_flag=0",
                  charac_no);

    DBQueryResult result;
    memset(&result, 0, sizeof(result));
    if (run_query(DB_TYPE_CAIN, query, &result) < 0) {
        return 0;
    }

    char* row[1] = {0};
    int count = 0;
    if (DBQueryResult_FetchRow(&result, row) == 1 && row[0]) {
        count = atoi(row[0]);
    }
    DBQueryResult_Free(&result);
    return count;
}

int db_get_mail_attachment(uint32_t postal_id, uint32_t charac_no, DBMailAttachment* att) {
    if (!att) return -1;

    char query[256];
    snprintf(query, sizeof(query),
                  "SELECT postal_id, item_id, gold, delete_flag FROM postal "
                  "WHERE postal_id=%u AND receive_charac_no=%u",
                  postal_id, charac_no);

    DBQueryResult result;
    memset(&result, 0, sizeof(result));
    if (run_query(DB_TYPE_CAIN, query, &result) < 0) {
        printf("[DB] Get mail attachment failed for postal_id=%u\n", postal_id);
        return -1;
    }

    char* row[4] = {0};
    if (DBQueryResult_FetchRow(&result, row) != 4) {
        DBQueryResult_Free(&result);
        return -1;
    }

    att->postal_id = row[0] ? atoi(row[0]) : 0;
    att->item_id = row[1] ? atoi(row[1]) : 0;
    att->gold = row[2] ? strtoul(row[2], NULL, 10) : 0;
    att->claimed = row[3] ? atoi(row[3]) : 0;

    DBQueryResult_Free(&result);
    return 0;
}

int db_claim_mail_attachment(uint32_t postal_id) {
    char query[256];
    snprintf(query, sizeof(query),
                  "UPDATE postal SET delete_flag=1 WHERE postal_id=%u",
                  postal_id);

    int affected = run_non_query(DB_TYPE_CAIN, query);
    if (affected <= 0) {
        printf("[DB] Claim mail attachment failed for postal_id=%u\n", postal_id);
        return -1;
    }

    printf("[DB] Mail %u marked as claimed\n", postal_id);
    return 0;
}

int db_get_mail_by_id(uint32_t postal_id, uint32_t charac_no, DBMailInfo* mail) {
    if (!mail) return -1;

    char query[256];
    snprintf(query, sizeof(query),
                  "SELECT postal_id, send_charac_no, send_charac_name, item_id, gold, "
                  "UNIX_TIMESTAMP(occ_time) "
                  "FROM postal WHERE postal_id=%u AND receive_charac_no=%u AND delete_flag=0",
                  postal_id, charac_no);

    DBQueryResult result;
    memset(&result, 0, sizeof(result));
    if (run_query(DB_TYPE_CAIN, query, &result) < 0) {
        return -1;
    }

    char* row[6] = {0};
    if (DBQueryResult_FetchRow(&result, row) != 6) {
        DBQueryResult_Free(&result);
        return -1;
    }

    memset(mail, 0, sizeof(DBMailInfo));
    mail->postal_id = row[0] ? atoi(row[0]) : 0;
    mail->send_charac_no = row[1] ? atoi(row[1]) : 0;
    if (row[2]) strncpy(mail->send_name, row[2], sizeof(mail->send_name) - 1);
    mail->item_id = row[3] ? atoi(row[3]) : 0;
    mail->gold = row[4] ? atoi(row[4]) : 0;
    mail->occ_time = row[5] ? atoi(row[5]) : 0;
    mail->read_flag = 0;

    DBQueryResult_Free(&result);
    return 0;
}

int db_delete_mail(uint32_t postal_id, uint32_t charac_no) {
    char query[256];
    snprintf(query, sizeof(query),
                  "UPDATE postal SET delete_flag=1 WHERE postal_id=%u AND receive_charac_no=%u",
                  postal_id, charac_no);

    int affected = run_non_query(DB_TYPE_CAIN, query);
    if (affected <= 0) {
        printf("[DB] Delete mail failed postal_id=%u charac_no=%u\n", postal_id, charac_no);
        return -1;
    }
    printf("[DB] Mail %u deleted for charac_no=%u\n", postal_id, charac_no);
    return 0;
}

int db_send_mail(uint32_t send_charac_no, const char* send_name,
                 uint32_t receive_charac_no, uint32_t item_id, uint32_t gold) {
    char safe_name[41];
    sanitize_name(safe_name, send_name, sizeof(safe_name));

    char query[512];
    snprintf(query, sizeof(query),
                  "INSERT INTO postal (occ_time, send_charac_no, send_charac_name, "
                  "receive_charac_no, item_id, gold, delete_flag) "
                  "VALUES (NOW(), %u, '%s', %u, %u, %u, 0)",
                  send_charac_no, safe_name, receive_charac_no, item_id, gold);

    unsigned long long insert_id = 0;
    if (run_insert(DB_TYPE_CAIN, query, &insert_id) < 0) {
        printf("[DB] Send mail failed: from=%u to=%u\n", send_charac_no, receive_charac_no);
        return -1;
    }

    printf("[DB] Mail sent: from=%u to=%u gold=%u item=%u postal_id=%llu\n",
                send_charac_no, receive_charac_no, gold, item_id, insert_id);
    return (int)insert_id;
}

int db_add_friend(uint32_t charac_no, uint32_t friend_no) {
    CharacFriends relation;
    relation.charac_no = charac_no;
    relation.friend_no = friend_no;

    if (CharacFriends_Add(&g_db_manager, &relation) != 0) {
        printf("[DB] Add friend failed: %u -> %u\n", charac_no, friend_no);
        return -1;
    }

    printf("[DB] Added friend: %u -> %u\n", charac_no, friend_no);
    return 0;
}

int db_remove_friend(uint32_t charac_no, uint32_t friend_no) {
    int removed = 0;
    if (CharacFriends_Delete(&g_db_manager, charac_no, friend_no) == 0) {
        removed++;
    }
    if (CharacFriends_Delete(&g_db_manager, friend_no, charac_no) == 0) {
        removed++;
    }

    if (removed == 0) {
        printf("[DB] Remove friend failed: %u <-> %u\n", charac_no, friend_no);
        return -1;
    }

    printf("[DB] Removed friend: %u <-> %u (affected=%d)\n", charac_no, friend_no, removed);
    return 0;
}

int db_get_friend_list(uint32_t charac_no, DBFriendInfo* friends, int max_count) {
    if (!friends || max_count <= 0) return 0;

    char query[512];
    snprintf(query, sizeof(query),
                  "SELECT f.friend_no, c.charac_name "
                  "FROM charac_friends f "
                  "LEFT JOIN charac_info c ON f.friend_no = c.charac_no "
                  "WHERE f.charac_no=%u LIMIT %d",
                  charac_no, max_count);

    DBQueryResult result;
    memset(&result, 0, sizeof(result));
    if (run_query(DB_TYPE_CAIN, query, &result) < 0) {
        printf("[DB] Friend query failed for charac_no=%u\n", charac_no);
        return 0;
    }

    int count = 0;
    char* row[2] = {0};
    while (count < max_count && DBQueryResult_FetchRow(&result, row) == 2) {
        DBFriendInfo* f = &friends[count];
        memset(f, 0, sizeof(DBFriendInfo));
        f->friend_charac_no = row[0] ? atoi(row[0]) : 0;
        if (row[1]) strncpy(f->friend_name, row[1], sizeof(f->friend_name) - 1);
        f->online = (uint8_t)is_character_online(f->friend_charac_no);
        count++;
    }

    DBQueryResult_Free(&result);
    printf("[DB] Loaded %d friends for charac_no=%u\n", count, charac_no);
    return count;
}

int db_create_guild(const char* guild_name, uint32_t master_charac_no, const char* master_name) {
    GuildInfo info;
    memset(&info, 0, sizeof(info));

    char safe_name[41];
    sanitize_name(safe_name, guild_name, sizeof(safe_name));
    strncpy(info.guild_name, safe_name, sizeof(info.guild_name) - 1);

    char safe_master[21];
    sanitize_name(safe_master, master_name, sizeof(safe_master));
    strncpy(info.master_name, safe_master, sizeof(info.master_name) - 1);

    info.server_id = 1;
    info.master_id = (int)master_charac_no;
    info.master_no = (int)master_charac_no;
    info.member_count = 1;
    info.lev = 1;
    info.guild_exp = 0;
    info.guild_point = 0;
    info.guild_point_acc = 0;
    info.guild_point_prev = 0;
    info.guild_rank = 0;
    info.guild_war_point = 0;
    info.final_entry = 0;
    info.final_win = 0;
    info.guild_icon_auth = 0;
    info.power_side = 0;
    info.guild_agit_flag = 0;
    info.power_war_point = 0;
    info.power_join_count = 0;
    info.guild_fund = 0;

    format_time_now(info.create_time, sizeof(info.create_time));
    format_time_now(info.lev_up_time, sizeof(info.lev_up_time));

    unsigned long long guild_id = 0;
    if (GuildInfo_AddEx(&g_db_manager, &info, &guild_id) != 0) {
        printf("[DB] Create guild failed: name=%s\n", safe_name);
        return -1;
    }

    printf("[DB] Guild created: id=%llu name=%s master=%s\n", guild_id, safe_name, safe_master);
    return (int)guild_id;
}

int db_add_guild_member(uint32_t guild_id, uint32_t charac_no, const char* charac_name, uint8_t grade) {
    GuildMember member;
    memset(&member, 0, sizeof(member));

    member.guild_id = (int)guild_id;
    member.charac_no = (int)charac_no;
    member.server_id = 1;
    member.grade = (signed char)grade;
    member.member_flag = 1;
    member.lev = 1;

    char safe_name[21];
    sanitize_name(safe_name, charac_name, sizeof(safe_name));
    strncpy(member.charac_name, safe_name, sizeof(member.charac_name) - 1);

    format_time_now(member.apply_time, sizeof(member.apply_time));
    strncpy(member.member_time, member.apply_time, sizeof(member.member_time) - 1);

    if (GuildMember_Add(&g_db_manager, &member) != 0) {
        printf("[DB] Add guild member failed: guild=%u charac=%u\n", guild_id, charac_no);
        return -1;
    }

    printf("[DB] Added guild member: guild=%u charac=%u name=%s grade=%u\n",
           guild_id, charac_no, safe_name, grade);
    return 0;
}

int db_update_guild_member_count(uint32_t guild_id, int delta) {
    if (GuildInfo_UpdateMemberCount(&g_db_manager, (int)guild_id, delta) != 0) {
        printf("[DB] Update guild member count failed guild=%u\n", guild_id);
        return -1;
    }
    return 0;
}

int db_update_character_guild(uint32_t charac_no, uint32_t guild_id) {
    char query[256];
    snprintf(query, sizeof(query),
                  "UPDATE charac_info SET guild_id = %u WHERE charac_no = %u",
                  guild_id, charac_no);

    if (run_non_query(DB_TYPE_CAIN, query) < 0) {
        printf("[DB] Update character guild failed charac_no=%u guild_id=%u\n",
                    charac_no, guild_id);
        return -1;
    }
    return 0;
}

int db_remove_guild_member(uint32_t guild_id, uint32_t charac_no) {
    if (GuildMember_UpdateFlag(&g_db_manager, (int)guild_id, (int)charac_no, 0) != 0) {
        printf("[DB] Remove guild member failed guild=%u charac=%u\n", guild_id, charac_no);
        return -1;
    }

    printf("[DB] Removed guild member: guild=%u charac=%u\n",
           guild_id, charac_no);
    return 0;
}

int db_disband_guild(uint32_t guild_id) {
    if (GuildInfo_SetExpireFlag(&g_db_manager, (int)guild_id, 1) != 0) {
        printf("[DB] Disband guild failed at info table guild=%u\n", guild_id);
        return -1;
    }

    GuildMember_ClearGuild(&g_db_manager, (int)guild_id, 0);
    printf("[DB] Guild %u disbanded\n", guild_id);
    return 0;
}

int db_get_guild_by_charac(uint32_t charac_no, DBGuildInfo* guild) {
    if (!guild) return -1;

    GuildMember member;
    if (GuildMember_GetByCharacNo(&g_db_manager, (int)charac_no, &member) != 0) {
        return -1;
    }

    GuildInfo info;
    if (GuildInfo_Get(&g_db_manager, member.guild_id, &info) != 0) {
        return -1;
    }
    if (info.expire_flag) {
        return -1;
    }

    memset(guild, 0, sizeof(DBGuildInfo));
    guild->guild_id = (uint32_t)info.guild_id;
    strncpy(guild->guild_name, info.guild_name, sizeof(guild->guild_name) - 1);
    strncpy(guild->master_name, info.master_name, sizeof(guild->master_name) - 1);
    guild->level = info.lev > 0 ? (uint8_t)info.lev : 1;
    guild->member_count = info.member_count > 0 ? (uint32_t)info.member_count : 0;
    guild->guild_point = info.guild_point > 0 ? (uint32_t)info.guild_point : 0;

    printf("[DB] Loaded guild %s (id=%u) for charac_no=%u\n",
           guild->guild_name, guild->guild_id, charac_no);
    return 0;
}

int db_get_guild_members(uint32_t guild_id, DBGuildMemberInfo* members, int max_count) {
    if (!members || max_count <= 0) return 0;

    std::vector<GuildMember> orm_members(max_count);
    int actual = 0;
    if (GuildMember_GetByGuild(&g_db_manager, (int)guild_id, orm_members.data(), max_count, &actual) != 0) {
        printf("[DB] Guild member query failed guild_id=%u\n", guild_id);
        return 0;
    }

    for (int i = 0; i < actual; ++i) {
        const GuildMember& src = orm_members[i];
        DBGuildMemberInfo* dst = &members[i];
        memset(dst, 0, sizeof(DBGuildMemberInfo));
        dst->charac_no = src.charac_no;
        strncpy(dst->charac_name, src.charac_name, sizeof(dst->charac_name) - 1);
        dst->grade = src.grade;
        dst->job = src.job;
        dst->level = src.lev > 0 ? (uint8_t)src.lev : 1;
        dst->online = (uint8_t)is_character_online(dst->charac_no);
    }

    printf("[DB] Loaded %d guild members for guild_id=%u\n", actual, guild_id);
    return actual;
}
