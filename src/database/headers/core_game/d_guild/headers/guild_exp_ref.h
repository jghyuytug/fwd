#ifndef GUILD_EXP_REF_H
#define GUILD_EXP_REF_H

#include "db_connection_manager.h"

typedef struct {
    int grade;
    unsigned int exp;
} GuildExpRef;

// CRUD Operations
int GuildExpRef_Add(DBConnectionManager* manager, const GuildExpRef* ref);
int GuildExpRef_Get(DBConnectionManager* manager, int grade, GuildExpRef* ref);
int GuildExpRef_Update(DBConnectionManager* manager, const GuildExpRef* ref);
int GuildExpRef_Delete(DBConnectionManager* manager, int grade);
int GuildExpRef_Exists(DBConnectionManager* manager, int grade);

// Business Operations
int GuildExpRef_GetAll(DBConnectionManager* manager, GuildExpRef** refs, int* count);
int GuildExpRef_GetByExpRange(DBConnectionManager* manager, unsigned int min_exp, unsigned int max_exp, GuildExpRef** refs, int* count);
int GuildExpRef_GetMaxGrade(DBConnectionManager* manager, int* max_grade);

#endif // GUILD_EXP_REF_H
