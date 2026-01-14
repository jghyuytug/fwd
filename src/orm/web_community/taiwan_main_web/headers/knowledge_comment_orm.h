#ifndef KNOWLEDGE_COMMENT_ORM_H
#define KNOWLEDGE_COMMENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int content_no;
    int m_id;
    signed char server_id;
    int charac_no;
    char charac_name[21];
    signed char job;
    char comment_text[1024];
    char ip[16];
    int create_day;
    short value;
} KnowledgeComment;

/* CRUD Operations */
int KnowledgeComment_Add(DBConnectionManager* manager, const KnowledgeComment* record);
int KnowledgeComment_Get(DBConnectionManager* manager, int no, KnowledgeComment* record);
int KnowledgeComment_Update(DBConnectionManager* manager, const KnowledgeComment* record);
int KnowledgeComment_Delete(DBConnectionManager* manager, int no);
int KnowledgeComment_Exists(DBConnectionManager* manager, int no);
int KnowledgeComment_GetAll(DBConnectionManager* manager, KnowledgeComment* records, int max_count, int* actual_count);

#endif /* KNOWLEDGE_COMMENT_ORM_H */