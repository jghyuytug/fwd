#ifndef DNF_MAGAZINE_QUIZ_ORM_H
#define DNF_MAGAZINE_QUIZ_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int magazine_id;
    int m_id;
    char occ_time[20];
} DnfMagazineQuiz;

/* CRUD Operations */
int DnfMagazineQuiz_Add(DBConnectionManager* manager, const DnfMagazineQuiz* record);
int DnfMagazineQuiz_Get(DBConnectionManager* manager, int magazine_id, int m_id, DnfMagazineQuiz* record);
int DnfMagazineQuiz_Update(DBConnectionManager* manager, const DnfMagazineQuiz* record);
int DnfMagazineQuiz_Delete(DBConnectionManager* manager, int magazine_id, int m_id);
int DnfMagazineQuiz_Exists(DBConnectionManager* manager, int magazine_id, int m_id);
int DnfMagazineQuiz_GetAll(DBConnectionManager* manager, DnfMagazineQuiz* records, int max_count, int* actual_count);

#endif /* DNF_MAGAZINE_QUIZ_ORM_H */