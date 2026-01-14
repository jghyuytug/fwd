#ifndef TCG_TUNE_SEARCH_ORM_H
#define TCG_TUNE_SEARCH_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    signed char category;
    char class;
    char word[33];
} TcgTuneSearch;

/* CRUD Operations */
int TcgTuneSearch_Add(DBConnectionManager* manager, const TcgTuneSearch* record);
int TcgTuneSearch_GetAll(DBConnectionManager* manager, TcgTuneSearch* records, int max_count, int* actual_count);

#endif /* TCG_TUNE_SEARCH_ORM_H */