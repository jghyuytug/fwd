#ifndef ARAD_NEWS_TAG_ORM_H
#define ARAD_NEWS_TAG_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_date[11];
    char tag[21];
    int tag_cnt;
    int rank;
} AradNewsTag;

/* CRUD Operations */
int AradNewsTag_Add(DBConnectionManager* manager, const AradNewsTag* record);
int AradNewsTag_Get(DBConnectionManager* manager, const char* occ_date, const char* tag, int rank, AradNewsTag* record);
int AradNewsTag_Update(DBConnectionManager* manager, const AradNewsTag* record);
int AradNewsTag_Delete(DBConnectionManager* manager, const char* occ_date, const char* tag, int rank);
int AradNewsTag_Exists(DBConnectionManager* manager, const char* occ_date, const char* tag, int rank);
int AradNewsTag_GetAll(DBConnectionManager* manager, AradNewsTag* records, int max_count, int* actual_count);

#endif /* ARAD_NEWS_TAG_ORM_H */