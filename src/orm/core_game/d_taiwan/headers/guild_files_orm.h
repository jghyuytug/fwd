#ifndef GUILD_FILES_ORM_H
#define GUILD_FILES_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int gno;                    /* Guild number (part of composite primary key) */
    unsigned char gf_no;        /* File number (auto_increment, part of composite primary key) */
    char file_server[51];       /* File server (varchar 50 + 1) */
    char file_location[101];    /* File location path (varchar 100 + 1) */
} GuildFiles;

/* CRUD Operations */
int GuildFiles_Add(DBConnectionManager* manager, const GuildFiles* file);
int GuildFiles_Get(DBConnectionManager* manager, int gno, unsigned char gf_no, GuildFiles* file);
int GuildFiles_Update(DBConnectionManager* manager, const GuildFiles* file);
int GuildFiles_Delete(DBConnectionManager* manager, int gno, unsigned char gf_no);
int GuildFiles_Exists(DBConnectionManager* manager, int gno, unsigned char gf_no);

/* Business Operations */
int GuildFiles_GetByGuild(DBConnectionManager* manager, int gno,
                          GuildFiles* files, int max_count, int* actual_count);
int GuildFiles_DeleteByGuild(DBConnectionManager* manager, int gno);
int GuildFiles_CountByGuild(DBConnectionManager* manager, int gno, int* count);
int GuildFiles_UpdateLocation(DBConnectionManager* manager, int gno,
                               unsigned char gf_no, const char* new_location);

/* Utility Functions */
void GuildFiles_PrintInfo(const GuildFiles* file);

#endif
