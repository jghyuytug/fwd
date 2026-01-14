#ifndef EVENT_QUIZQUIZ_STAMP_ORM_H
#define EVENT_QUIZQUIZ_STAMP_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    signed char degree;
    int stamp;
    char occ_time[20];
} EventQuizquizStamp;

/* CRUD Operations */
int EventQuizquizStamp_Add(DBConnectionManager* manager, const EventQuizquizStamp* record);
int EventQuizquizStamp_Get(DBConnectionManager* manager, int m_id, signed char degree, EventQuizquizStamp* record);
int EventQuizquizStamp_Update(DBConnectionManager* manager, const EventQuizquizStamp* record);
int EventQuizquizStamp_Delete(DBConnectionManager* manager, int m_id, signed char degree);
int EventQuizquizStamp_Exists(DBConnectionManager* manager, int m_id, signed char degree);
int EventQuizquizStamp_GetAll(DBConnectionManager* manager, EventQuizquizStamp* records, int max_count, int* actual_count);

#endif /* EVENT_QUIZQUIZ_STAMP_ORM_H */