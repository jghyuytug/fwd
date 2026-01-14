#ifndef GUILD_RECOMMEND_ORM_H
#define GUILD_RECOMMEND_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int guild_id;
    signed char server_id;
    int charac_no;
    char charac_name[21];
    char comment[101];
    char recommend_time[20];
} GuildRecommend;

/* CRUD Operations */
int GuildRecommend_Add(DBConnectionManager* manager, const GuildRecommend* record);
int GuildRecommend_Get(DBConnectionManager* manager, int no, GuildRecommend* record);
int GuildRecommend_Update(DBConnectionManager* manager, const GuildRecommend* record);
int GuildRecommend_Delete(DBConnectionManager* manager, int no);
int GuildRecommend_Exists(DBConnectionManager* manager, int no);
int GuildRecommend_GetAll(DBConnectionManager* manager, GuildRecommend* records, int max_count, int* actual_count);

#endif /* GUILD_RECOMMEND_ORM_H */