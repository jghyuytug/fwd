#ifndef KNOWLEDGE_TEST_FILE_ORM_H
#define KNOWLEDGE_TEST_FILE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int content_no;
    char file_url[129];
} KnowledgeTestFile;

/* CRUD Operations */
int KnowledgeTestFile_Add(DBConnectionManager* manager, const KnowledgeTestFile* record);
int KnowledgeTestFile_Get(DBConnectionManager* manager, int no, KnowledgeTestFile* record);
int KnowledgeTestFile_Update(DBConnectionManager* manager, const KnowledgeTestFile* record);
int KnowledgeTestFile_Delete(DBConnectionManager* manager, int no);
int KnowledgeTestFile_Exists(DBConnectionManager* manager, int no);
int KnowledgeTestFile_GetAll(DBConnectionManager* manager, KnowledgeTestFile* records, int max_count, int* actual_count);

#endif /* KNOWLEDGE_TEST_FILE_ORM_H */