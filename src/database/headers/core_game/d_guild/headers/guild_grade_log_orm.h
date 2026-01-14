#ifndef GUILD_GRADE_LOG_ORM_H
#define GUILD_GRADE_LOG_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int id;
    int guild_id;
    int m_id;
    signed char server_id;
    int charac_no;
    char charac_name[21];
    char occ_time[20];
    signed char grade_prev;
    signed char grade_next;
    char reason[41];
    int admin_no;
    char admin_name[21];
} GuildGradeLog;

/* CRUD Operations */
int GuildGradeLog_Add(DBConnectionManager* manager, const GuildGradeLog* record);
int GuildGradeLog_Get(DBConnectionManager* manager, int id, GuildGradeLog* record);
int GuildGradeLog_Update(DBConnectionManager* manager, const GuildGradeLog* record);
int GuildGradeLog_Delete(DBConnectionManager* manager, int id);
int GuildGradeLog_Exists(DBConnectionManager* manager, int id);
int GuildGradeLog_GetAll(DBConnectionManager* manager, GuildGradeLog* records, int max_count, int* actual_count);

#endif /* GUILD_GRADE_LOG_ORM_H */