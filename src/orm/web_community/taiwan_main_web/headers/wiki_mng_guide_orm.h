#ifndef WIKI_MNG_GUIDE_ORM_H
#define WIKI_MNG_GUIDE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int id;
    char url_go[21];
    char url_to[21];
    int reg_date;
    char subject[101];
    int hits;
    char content[1024];
    signed char flag;
    signed char p_type;
} WikiMngGuide;

/* CRUD Operations */
int WikiMngGuide_Add(DBConnectionManager* manager, const WikiMngGuide* record);
int WikiMngGuide_Get(DBConnectionManager* manager, int id, WikiMngGuide* record);
int WikiMngGuide_Update(DBConnectionManager* manager, const WikiMngGuide* record);
int WikiMngGuide_Delete(DBConnectionManager* manager, int id);
int WikiMngGuide_Exists(DBConnectionManager* manager, int id);
int WikiMngGuide_GetAll(DBConnectionManager* manager, WikiMngGuide* records, int max_count, int* actual_count);

#endif /* WIKI_MNG_GUIDE_ORM_H */