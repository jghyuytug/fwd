#ifndef KNOWLEDGE_TEST_CONTENT_ORM_H
#define KNOWLEDGE_TEST_CONTENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char content[1024];
    char content_type;
    char ip[16];
} KnowledgeTestContent;

/* CRUD Operations */
int KnowledgeTestContent_Add(DBConnectionManager* manager, const KnowledgeTestContent* record);
int KnowledgeTestContent_Get(DBConnectionManager* manager, int no, KnowledgeTestContent* record);
int KnowledgeTestContent_Update(DBConnectionManager* manager, const KnowledgeTestContent* record);
int KnowledgeTestContent_Delete(DBConnectionManager* manager, int no);
int KnowledgeTestContent_Exists(DBConnectionManager* manager, int no);
int KnowledgeTestContent_GetAll(DBConnectionManager* manager, KnowledgeTestContent* records, int max_count, int* actual_count);

#endif /* KNOWLEDGE_TEST_CONTENT_ORM_H */