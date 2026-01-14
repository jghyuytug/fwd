#ifndef KNOWLEDGE_SEARCH_ORM_H
#define KNOWLEDGE_SEARCH_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    long long kn_id;
    char word[33];
} KnowledgeSearch;

/* CRUD Operations */
int KnowledgeSearch_Add(DBConnectionManager* manager, const KnowledgeSearch* record);
int KnowledgeSearch_GetAll(DBConnectionManager* manager, KnowledgeSearch* records, int max_count, int* actual_count);

#endif /* KNOWLEDGE_SEARCH_ORM_H */