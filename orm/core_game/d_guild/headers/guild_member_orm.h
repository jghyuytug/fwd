#ifndef GUILD_MEMBER_ORM_H
#define GUILD_MEMBER_ORM_H

#include "database/headers/db_connection_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int guild_id;
    int m_id;
    int server_id;
    int charac_no;
    char charac_name[21];
    char memo[256];
    signed char grade;
    signed char job;
    signed char grow_type;
    signed char lev;
    int age;
    char born_year[5];
    char sex[2];
    char apply_time[20];
    char member_time[20];
    signed char member_flag;
    int bbs_cnt;
    char last_visit_time[20];
    signed char secede_type;
    char secede_time[20];
    int member_point;
    int member_point_prev;
    char last_play_time[20];
    char nick_name[21];
} GuildMember;

/* Basic CRUD operations */
int GuildMember_Get(DBConnectionManager* manager, int guild_id, int charac_no, GuildMember* member);
int GuildMember_Add(DBConnectionManager* manager, const GuildMember* member);
int GuildMember_Delete(DBConnectionManager* manager, int guild_id, int charac_no);

/* Extended query operations */
int GuildMember_GetByCharacNo(DBConnectionManager* manager, int charac_no, GuildMember* member);
int GuildMember_GetByGuild(DBConnectionManager* manager, int guild_id, GuildMember* members, int max_count, int* actual_count);

/* Update operations */
int GuildMember_UpdateFlag(DBConnectionManager* manager, int guild_id, int charac_no, int flag);
int GuildMember_ClearGuild(DBConnectionManager* manager, int guild_id, int flag);

#ifdef __cplusplus
}
#endif

#endif /* GUILD_MEMBER_ORM_H */
