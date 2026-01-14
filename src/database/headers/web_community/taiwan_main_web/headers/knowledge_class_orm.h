#ifndef KNOWLEDGE_CLASS_ORM_H
#define KNOWLEDGE_CLASS_ORM_H

#include "db_connection_manager.h"

typedef struct {
    long long kn_id;
    long long sup_id;
    char kn_name[51];
    int cur_depth;
} KnowledgeClass;

/* CRUD Operations */
int KnowledgeClass_Add(DBConnectionManager* manager, const KnowledgeClass* record);
int KnowledgeClass_Get(DBConnectionManager* manager, long long kn_id, KnowledgeClass* record);
int KnowledgeClass_Update(DBConnectionManager* manager, const KnowledgeClass* record);
int KnowledgeClass_Delete(DBConnectionManager* manager, long long kn_id);
int KnowledgeClass_Exists(DBConnectionManager* manager, long long kn_id);
int KnowledgeClass_GetAll(DBConnectionManager* manager, KnowledgeClass* records, int max_count, int* actual_count);

#endif /* KNOWLEDGE_CLASS_ORM_H */