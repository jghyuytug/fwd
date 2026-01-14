#ifndef KNOWLEDGE_CONTENT_ORM_H
#define KNOWLEDGE_CONTENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char content[1024];
    char content_type;
    char ip[16];
} KnowledgeContent;

/* CRUD Operations */
int KnowledgeContent_Add(DBConnectionManager* manager, const KnowledgeContent* record);
int KnowledgeContent_Get(DBConnectionManager* manager, int no, KnowledgeContent* record);
int KnowledgeContent_Update(DBConnectionManager* manager, const KnowledgeContent* record);
int KnowledgeContent_Delete(DBConnectionManager* manager, int no);
int KnowledgeContent_Exists(DBConnectionManager* manager, int no);
int KnowledgeContent_GetAll(DBConnectionManager* manager, KnowledgeContent* records, int max_count, int* actual_count);

#endif /* KNOWLEDGE_CONTENT_ORM_H */