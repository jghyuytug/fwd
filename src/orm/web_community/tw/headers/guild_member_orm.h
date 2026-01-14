#ifndef GUILD_MEMBER_ORM_H
#define GUILD_MEMBER_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int guild_id;
    int m_id;
    signed char server_id;
    int charac_no;
    char charac_name[21];
    char nick_name[13];
    signed char grade;
    signed char job;
    signed char grow_type;
    signed char lev;
    signed char age;
    char born_year[3];
    char sex[2];
    char apply_time[20];
    char member_time[20];
    signed char member_flag;
    short bbs_cnt;
    char last_visit_time[20];
    signed char secede_type;
    char secede_time[20];
    int member_point;
    int member_point_prev;
    char last_play_time[20];
} GuildMember;

/* CRUD Operations */
int GuildMember_Add(DBConnectionManager* manager, const GuildMember* record);
int GuildMember_Get(DBConnectionManager* manager, int guild_id, int charac_no, GuildMember* record);
int GuildMember_Update(DBConnectionManager* manager, const GuildMember* record);
int GuildMember_Delete(DBConnectionManager* manager, int guild_id, int charac_no);
int GuildMember_Exists(DBConnectionManager* manager, int guild_id, int charac_no);
int GuildMember_GetAll(DBConnectionManager* manager, GuildMember* records, int max_count, int* actual_count);

#endif /* GUILD_MEMBER_ORM_H */