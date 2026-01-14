#ifndef FUZZY_QUESTION_ANSWER_ORM_H
#define FUZZY_QUESTION_ANSWER_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int Id;
    char FuzzyQuestion[256];
    char FuzzyAnswer[1024];
    char CreatedAt[20];
} FuzzyQuestionAnswer;

/* CRUD Operations */
int FuzzyQuestionAnswer_Add(DBConnectionManager* manager, const FuzzyQuestionAnswer* record);
int FuzzyQuestionAnswer_Get(DBConnectionManager* manager, int Id, FuzzyQuestionAnswer* record);
int FuzzyQuestionAnswer_Update(DBConnectionManager* manager, const FuzzyQuestionAnswer* record);
int FuzzyQuestionAnswer_Delete(DBConnectionManager* manager, int Id);
int FuzzyQuestionAnswer_Exists(DBConnectionManager* manager, int Id);
int FuzzyQuestionAnswer_GetAll(DBConnectionManager* manager, FuzzyQuestionAnswer* records, int max_count, int* actual_count);

#endif /* FUZZY_QUESTION_ANSWER_ORM_H */