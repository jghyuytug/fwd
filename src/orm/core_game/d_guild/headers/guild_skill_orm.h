#ifndef GUILD_SKILL_ORM_H
#define GUILD_SKILL_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int guild_id;
    int remain_sp;
    int used_sp;
} GuildSkill;

/* CRUD Operations */
int GuildSkill_Add(DBConnectionManager* manager, const GuildSkill* record);
int GuildSkill_Get(DBConnectionManager* manager, int guild_id, GuildSkill* record);
int GuildSkill_Update(DBConnectionManager* manager, const GuildSkill* record);
int GuildSkill_Delete(DBConnectionManager* manager, int guild_id);
int GuildSkill_Exists(DBConnectionManager* manager, int guild_id);
int GuildSkill_GetAll(DBConnectionManager* manager, GuildSkill* records, int max_count, int* actual_count);

#endif /* GUILD_SKILL_ORM_H */