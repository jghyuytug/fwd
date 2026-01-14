#ifndef KNOWLEDGE_BEST_ORM_H
#define KNOWLEDGE_BEST_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int kn_id;
    signed char rank;
    int content_no;
} KnowledgeBest;

/* CRUD Operations */
int KnowledgeBest_Add(DBConnectionManager* manager, const KnowledgeBest* record);
int KnowledgeBest_Get(DBConnectionManager* manager, int kn_id, signed char rank, KnowledgeBest* record);
int KnowledgeBest_Update(DBConnectionManager* manager, const KnowledgeBest* record);
int KnowledgeBest_Delete(DBConnectionManager* manager, int kn_id, signed char rank);
int KnowledgeBest_Exists(DBConnectionManager* manager, int kn_id, signed char rank);
int KnowledgeBest_GetAll(DBConnectionManager* manager, KnowledgeBest* records, int max_count, int* actual_count);

#endif /* KNOWLEDGE_BEST_ORM_H */