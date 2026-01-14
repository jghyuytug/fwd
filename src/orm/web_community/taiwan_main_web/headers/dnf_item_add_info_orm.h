#ifndef DNF_ITEM_ADD_INFO_ORM_H
#define DNF_ITEM_ADD_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int it_no;
    char dungeon[101];
    char shop[256];
    signed char pot;
    int mix_item_no;
    char link_it_no[51];
    char ir_info[1024];
    char item_pic[256];
    char add_item_memo[1024];
    char quest_name[256];
    signed char view_flag;
    float recomm_avg;
    int recomm_cnt;
    signed char recomm_score;
} DnfItemAddInfo;

/* CRUD Operations */
int DnfItemAddInfo_Add(DBConnectionManager* manager, const DnfItemAddInfo* record);
int DnfItemAddInfo_Get(DBConnectionManager* manager, int it_no, DnfItemAddInfo* record);
int DnfItemAddInfo_Update(DBConnectionManager* manager, const DnfItemAddInfo* record);
int DnfItemAddInfo_Delete(DBConnectionManager* manager, int it_no);
int DnfItemAddInfo_Exists(DBConnectionManager* manager, int it_no);
int DnfItemAddInfo_GetAll(DBConnectionManager* manager, DnfItemAddInfo* records, int max_count, int* actual_count);

#endif /* DNF_ITEM_ADD_INFO_ORM_H */