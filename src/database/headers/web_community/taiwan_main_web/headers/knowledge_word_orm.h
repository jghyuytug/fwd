#ifndef KNOWLEDGE_WORD_ORM_H
#define KNOWLEDGE_WORD_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char word[33];
    int word_point;
} KnowledgeWord;

/* CRUD Operations */
int KnowledgeWord_Add(DBConnectionManager* manager, const KnowledgeWord* record);
int KnowledgeWord_Get(DBConnectionManager* manager, const char* word, KnowledgeWord* record);
int KnowledgeWord_Update(DBConnectionManager* manager, const KnowledgeWord* record);
int KnowledgeWord_Delete(DBConnectionManager* manager, const char* word);
int KnowledgeWord_Exists(DBConnectionManager* manager, const char* word);
int KnowledgeWord_GetAll(DBConnectionManager* manager, KnowledgeWord* records, int max_count, int* actual_count);

#endif /* KNOWLEDGE_WORD_ORM_H */