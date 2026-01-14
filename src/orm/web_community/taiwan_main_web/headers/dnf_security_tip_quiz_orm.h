#ifndef DNF_SECURITY_TIP_QUIZ_ORM_H
#define DNF_SECURITY_TIP_QUIZ_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int magazine_id;
    int m_id;
    char occ_time[20];
} DnfSecurityTipQuiz;

/* CRUD Operations */
int DnfSecurityTipQuiz_Add(DBConnectionManager* manager, const DnfSecurityTipQuiz* record);
int DnfSecurityTipQuiz_Get(DBConnectionManager* manager, int magazine_id, int m_id, DnfSecurityTipQuiz* record);
int DnfSecurityTipQuiz_Update(DBConnectionManager* manager, const DnfSecurityTipQuiz* record);
int DnfSecurityTipQuiz_Delete(DBConnectionManager* manager, int magazine_id, int m_id);
int DnfSecurityTipQuiz_Exists(DBConnectionManager* manager, int magazine_id, int m_id);
int DnfSecurityTipQuiz_GetAll(DBConnectionManager* manager, DnfSecurityTipQuiz* records, int max_count, int* actual_count);

#endif /* DNF_SECURITY_TIP_QUIZ_ORM_H */