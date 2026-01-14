#ifndef GUILD_MEMO_ORM_H
#define GUILD_MEMO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int guild_id;
    int m_id;
    int charac_no;
    char charac_name[21];
    char nick_name[13];
    char memo[121];
    char create_time[20];
    signed char job;
    signed char grow_type;
} GuildMemo;

/* CRUD Operations */
int GuildMemo_Add(DBConnectionManager* manager, const GuildMemo* record);
int GuildMemo_Get(DBConnectionManager* manager, int no, GuildMemo* record);
int GuildMemo_Update(DBConnectionManager* manager, const GuildMemo* record);
int GuildMemo_Delete(DBConnectionManager* manager, int no);
int GuildMemo_Exists(DBConnectionManager* manager, int no);
int GuildMemo_GetAll(DBConnectionManager* manager, GuildMemo* records, int max_count, int* actual_count);

#endif /* GUILD_MEMO_ORM_H */