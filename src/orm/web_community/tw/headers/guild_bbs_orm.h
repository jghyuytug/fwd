#ifndef GUILD_BBS_ORM_H
#define GUILD_BBS_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int gno;
    signed char bd_id;
    signed char empyn;
    int mgno;
    signed char open;
    signed char main;
    int reg_date;
    int mod_date;
    int hits;
    char body_type[2];
    int m_id;
    char reg_id[21];
    char subject[51];
} GuildBbs;

/* CRUD Operations */
int GuildBbs_Add(DBConnectionManager* manager, const GuildBbs* record);
int GuildBbs_Get(DBConnectionManager* manager, int gno, GuildBbs* record);
int GuildBbs_Update(DBConnectionManager* manager, const GuildBbs* record);
int GuildBbs_Delete(DBConnectionManager* manager, int gno);
int GuildBbs_Exists(DBConnectionManager* manager, int gno);
int GuildBbs_GetAll(DBConnectionManager* manager, GuildBbs* records, int max_count, int* actual_count);

#endif /* GUILD_BBS_ORM_H */