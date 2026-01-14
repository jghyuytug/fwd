#ifndef GUILD_MEMBER_INTRODUCE_ORM_H
#define GUILD_MEMBER_INTRODUCE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int guild_id;
    int charac_no;
    char introduce[201];  /* VARCHAR(200) -> 201 */
} GuildMemberIntroduce;

/* CRUD Operations */
int GuildMemberIntroduce_Add(DBConnectionManager* manager, const GuildMemberIntroduce* member);
int GuildMemberIntroduce_Get(DBConnectionManager* manager, int guild_id, int charac_no, GuildMemberIntroduce* member);
int GuildMemberIntroduce_Update(DBConnectionManager* manager, const GuildMemberIntroduce* member);
int GuildMemberIntroduce_Delete(DBConnectionManager* manager, int guild_id, int charac_no);
int GuildMemberIntroduce_Exists(DBConnectionManager* manager, int guild_id, int charac_no);

/* Business Operations */
int GuildMemberIntroduce_GetByGuild(DBConnectionManager* manager, int guild_id,
                                     GuildMemberIntroduce* members, int max_count, int* actual_count);
int GuildMemberIntroduce_DeleteByGuild(DBConnectionManager* manager, int guild_id);
int GuildMemberIntroduce_CountByGuild(DBConnectionManager* manager, int guild_id, int* count);

/* Utility Functions */
void GuildMemberIntroduce_PrintInfo(const GuildMemberIntroduce* member);

#endif /* GUILD_MEMBER_INTRODUCE_ORM_H */
