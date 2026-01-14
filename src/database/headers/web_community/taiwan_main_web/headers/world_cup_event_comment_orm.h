#ifndef WORLD_CUP_EVENT_COMMENT_ORM_H
#define WORLD_CUP_EVENT_COMMENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    signed char game_type;
    signed char score1;
    signed char score2;
    int m_id;
    signed char server_id;
    int charac_no;
    char charac_name[21];
    char comment_text[105];
    int create_day;
} WorldCupEventComment;

/* CRUD Operations */
int WorldCupEventComment_Add(DBConnectionManager* manager, const WorldCupEventComment* record);
int WorldCupEventComment_Get(DBConnectionManager* manager, int no, WorldCupEventComment* record);
int WorldCupEventComment_Update(DBConnectionManager* manager, const WorldCupEventComment* record);
int WorldCupEventComment_Delete(DBConnectionManager* manager, int no);
int WorldCupEventComment_Exists(DBConnectionManager* manager, int no);
int WorldCupEventComment_GetAll(DBConnectionManager* manager, WorldCupEventComment* records, int max_count, int* actual_count);

#endif /* WORLD_CUP_EVENT_COMMENT_ORM_H */