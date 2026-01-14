#ifndef GUILD_RECOMMEND_ORM_H
#define GUILD_RECOMMEND_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;                     /* Recommendation number (primary key) */
    int guild_id;               /* Guild ID */
    unsigned char server_id;    /* Server ID */
    int charac_no;              /* Character number */
    char charac_name[21];       /* Character name (varchar 20 + 1) */
    char comment[101];          /* Recommendation comment (varchar 100 + 1) */
    char recommend_time[20];    /* Recommendation time (datetime string) */
} GuildRecommend;

/* CRUD Operations */
int GuildRecommend_Add(DBConnectionManager* manager, const GuildRecommend* recommend);
int GuildRecommend_Get(DBConnectionManager* manager, int no, GuildRecommend* recommend);
int GuildRecommend_Update(DBConnectionManager* manager, const GuildRecommend* recommend);
int GuildRecommend_Delete(DBConnectionManager* manager, int no);
int GuildRecommend_Exists(DBConnectionManager* manager, int no);

/* Business Operations */
int GuildRecommend_GetByGuild(DBConnectionManager* manager, int guild_id,
                               GuildRecommend* recommends, int max_count, int* actual_count);
int GuildRecommend_GetByCharacter(DBConnectionManager* manager, int charac_no,
                                   GuildRecommend* recommends, int max_count, int* actual_count);
int GuildRecommend_DeleteByGuild(DBConnectionManager* manager, int guild_id);
int GuildRecommend_CountByGuild(DBConnectionManager* manager, int guild_id, int* count);
int GuildRecommend_UpdateComment(DBConnectionManager* manager, int no, const char* new_comment);

/* Utility Functions */
void GuildRecommend_PrintInfo(const GuildRecommend* recommend);

#endif
