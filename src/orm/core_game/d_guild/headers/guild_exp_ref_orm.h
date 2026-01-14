#ifndef GUILD_EXP_REF_ORM_H
#define GUILD_EXP_REF_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int grade;
    int exp;
} GuildExpRef;

/* CRUD Operations */
int GuildExpRef_Add(DBConnectionManager* manager, const GuildExpRef* record);
int GuildExpRef_Get(DBConnectionManager* manager, int grade, GuildExpRef* record);
int GuildExpRef_Update(DBConnectionManager* manager, const GuildExpRef* record);
int GuildExpRef_Delete(DBConnectionManager* manager, int grade);
int GuildExpRef_Exists(DBConnectionManager* manager, int grade);
int GuildExpRef_GetAll(DBConnectionManager* manager, GuildExpRef* records, int max_count, int* actual_count);

#endif /* GUILD_EXP_REF_ORM_H */