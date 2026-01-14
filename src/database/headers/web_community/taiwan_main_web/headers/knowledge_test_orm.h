#ifndef KNOWLEDGE_TEST_ORM_H
#define KNOWLEDGE_TEST_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    long long kn_id;
    signed char question_flag;
    int m_id;
    signed char server_id;
    int charac_no;
    char charac_name[21];
    signed char job;
    char title[121];
    signed char know_flag;
    int create_day;
    short comment;
    int view;
    int value;
    int file;
} KnowledgeTest;

/* CRUD Operations */
int KnowledgeTest_Add(DBConnectionManager* manager, const KnowledgeTest* record);
int KnowledgeTest_Get(DBConnectionManager* manager, int no, KnowledgeTest* record);
int KnowledgeTest_Update(DBConnectionManager* manager, const KnowledgeTest* record);
int KnowledgeTest_Delete(DBConnectionManager* manager, int no);
int KnowledgeTest_Exists(DBConnectionManager* manager, int no);
int KnowledgeTest_GetAll(DBConnectionManager* manager, KnowledgeTest* records, int max_count, int* actual_count);

#endif /* KNOWLEDGE_TEST_ORM_H */