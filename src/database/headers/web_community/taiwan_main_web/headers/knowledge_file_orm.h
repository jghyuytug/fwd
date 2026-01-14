#ifndef KNOWLEDGE_FILE_ORM_H
#define KNOWLEDGE_FILE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int content_no;
    char file_url[129];
} KnowledgeFile;

/* CRUD Operations */
int KnowledgeFile_Add(DBConnectionManager* manager, const KnowledgeFile* record);
int KnowledgeFile_Get(DBConnectionManager* manager, int no, KnowledgeFile* record);
int KnowledgeFile_Update(DBConnectionManager* manager, const KnowledgeFile* record);
int KnowledgeFile_Delete(DBConnectionManager* manager, int no);
int KnowledgeFile_Exists(DBConnectionManager* manager, int no);
int KnowledgeFile_GetAll(DBConnectionManager* manager, KnowledgeFile* records, int max_count, int* actual_count);

#endif /* KNOWLEDGE_FILE_ORM_H */