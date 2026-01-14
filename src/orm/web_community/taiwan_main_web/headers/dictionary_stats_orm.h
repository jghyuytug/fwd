#ifndef DICTIONARY_STATS_ORM_H
#define DICTIONARY_STATS_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char date_enroll[11];
    char dic_name[101];
    int every_view_cnt;
    int detail_view_cnt;
    int comment_cnt;
} DictionaryStats;

/* CRUD Operations */
int DictionaryStats_Add(DBConnectionManager* manager, const DictionaryStats* record);
int DictionaryStats_Get(DBConnectionManager* manager, const char* date_enroll, const char* dic_name, DictionaryStats* record);
int DictionaryStats_Update(DBConnectionManager* manager, const DictionaryStats* record);
int DictionaryStats_Delete(DBConnectionManager* manager, const char* date_enroll, const char* dic_name);
int DictionaryStats_Exists(DBConnectionManager* manager, const char* date_enroll, const char* dic_name);
int DictionaryStats_GetAll(DBConnectionManager* manager, DictionaryStats* records, int max_count, int* actual_count);

#endif /* DICTIONARY_STATS_ORM_H */