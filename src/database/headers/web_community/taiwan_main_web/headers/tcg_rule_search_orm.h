#ifndef TCG_RULE_SEARCH_ORM_H
#define TCG_RULE_SEARCH_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    signed char category;
    char class;
    char word[33];
} TcgRuleSearch;

/* CRUD Operations */
int TcgRuleSearch_Add(DBConnectionManager* manager, const TcgRuleSearch* record);
int TcgRuleSearch_GetAll(DBConnectionManager* manager, TcgRuleSearch* records, int max_count, int* actual_count);

#endif /* TCG_RULE_SEARCH_ORM_H */