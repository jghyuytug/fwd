#ifndef GUILD_FILES_ORM_H
#define GUILD_FILES_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int gno;
    signed char gf_no;
    char file_server[51];
    char file_location[101];
} GuildFiles;

/* CRUD Operations */
int GuildFiles_Add(DBConnectionManager* manager, const GuildFiles* record);
int GuildFiles_Get(DBConnectionManager* manager, int gno, signed char gf_no, GuildFiles* record);
int GuildFiles_Update(DBConnectionManager* manager, const GuildFiles* record);
int GuildFiles_Delete(DBConnectionManager* manager, int gno, signed char gf_no);
int GuildFiles_Exists(DBConnectionManager* manager, int gno, signed char gf_no);
int GuildFiles_GetAll(DBConnectionManager* manager, GuildFiles* records, int max_count, int* actual_count);

#endif /* GUILD_FILES_ORM_H */