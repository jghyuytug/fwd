#ifndef WIKI_MNG_ENCYCLOPEDIA_ORM_H
#define WIKI_MNG_ENCYCLOPEDIA_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int id;
    signed char category;
    char subject[121];
    char content[1024];
    int reg_date;
    int hit;
    signed char today_info;
    signed char today_char;
    signed char pupular_flag;
    char img_url[256];
    char word[1024];
} WikiMngEncyclopedia;

/* CRUD Operations */
int WikiMngEncyclopedia_Add(DBConnectionManager* manager, const WikiMngEncyclopedia* record);
int WikiMngEncyclopedia_Get(DBConnectionManager* manager, int id, WikiMngEncyclopedia* record);
int WikiMngEncyclopedia_Update(DBConnectionManager* manager, const WikiMngEncyclopedia* record);
int WikiMngEncyclopedia_Delete(DBConnectionManager* manager, int id);
int WikiMngEncyclopedia_Exists(DBConnectionManager* manager, int id);
int WikiMngEncyclopedia_GetAll(DBConnectionManager* manager, WikiMngEncyclopedia* records, int max_count, int* actual_count);

#endif /* WIKI_MNG_ENCYCLOPEDIA_ORM_H */