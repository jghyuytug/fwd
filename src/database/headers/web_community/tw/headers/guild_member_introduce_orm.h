#ifndef GUILD_MEMBER_INTRODUCE_ORM_H
#define GUILD_MEMBER_INTRODUCE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int guild_id;
    int charac_no;
    char introduce[201];
} GuildMemberIntroduce;

/* CRUD Operations */
int GuildMemberIntroduce_Add(DBConnectionManager* manager, const GuildMemberIntroduce* record);
int GuildMemberIntroduce_Get(DBConnectionManager* manager, int guild_id, int charac_no, GuildMemberIntroduce* record);
int GuildMemberIntroduce_Update(DBConnectionManager* manager, const GuildMemberIntroduce* record);
int GuildMemberIntroduce_Delete(DBConnectionManager* manager, int guild_id, int charac_no);
int GuildMemberIntroduce_Exists(DBConnectionManager* manager, int guild_id, int charac_no);
int GuildMemberIntroduce_GetAll(DBConnectionManager* manager, GuildMemberIntroduce* records, int max_count, int* actual_count);

#endif /* GUILD_MEMBER_INTRODUCE_ORM_H */