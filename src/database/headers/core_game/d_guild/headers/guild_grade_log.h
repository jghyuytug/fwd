#ifndef GUILD_GRADE_LOG_H
#define GUILD_GRADE_LOG_H

#include "db_connection_manager.h"

typedef struct {
    int id;
    int guild_id;
    int m_id;
    char server_id;
    int charac_no;
    char charac_name[21];
    char occ_time[20];
    char grade_prev;
    char grade_next;
    char reason[41];
    int admin_no;
    char admin_name[21];
} GuildGradeLog;

// CRUD Operations (Note: Add does not accept id parameter - auto_increment)
int GuildGradeLog_Add(DBConnectionManager* manager, const GuildGradeLog* log, int* new_id);
int GuildGradeLog_Get(DBConnectionManager* manager, int id, GuildGradeLog* log);
int GuildGradeLog_Delete(DBConnectionManager* manager, int id);

// Business Operations
int GuildGradeLog_GetByGuild(DBConnectionManager* manager, int guild_id, GuildGradeLog** logs, int* count);
int GuildGradeLog_GetByCharacter(DBConnectionManager* manager, int server_id, int charac_no, GuildGradeLog** logs, int* count);
int GuildGradeLog_GetByDateRange(DBConnectionManager* manager, const char* start_date, const char* end_date, GuildGradeLog** logs, int* count);

#endif // GUILD_GRADE_LOG_H
