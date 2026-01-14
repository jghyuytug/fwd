#ifndef KNOWL_CLASS_TREE_ORM_H
#define KNOWL_CLASS_TREE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int kn_id;
    int up_id;
    int depth;
} KnowlClassTree;

/* CRUD Operations */
int KnowlClassTree_Add(DBConnectionManager* manager, const KnowlClassTree* record);
int KnowlClassTree_GetAll(DBConnectionManager* manager, KnowlClassTree* records, int max_count, int* actual_count);

#endif /* KNOWL_CLASS_TREE_ORM_H */