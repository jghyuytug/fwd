#ifndef EVENT_QUIZQUIZ_STAMP_ORM_H
#define EVENT_QUIZQUIZ_STAMP_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    char degree;
    int stamp;
    char occ_time[20];
} EventQuizquizStamp;

/* CRUD Operations */
int EventQuizquizStamp_Add(DBConnectionManager* manager, const EventQuizquizStamp* record);
int EventQuizquizStamp_Get(DBConnectionManager* manager, int m_id, char degree,
                            EventQuizquizStamp* record);
int EventQuizquizStamp_Update(DBConnectionManager* manager, const EventQuizquizStamp* record);
int EventQuizquizStamp_Delete(DBConnectionManager* manager, int m_id, char degree);
int EventQuizquizStamp_Exists(DBConnectionManager* manager, int m_id, char degree);

/* Business Operations */
int EventQuizquizStamp_GetByMemberId(DBConnectionManager* manager, int m_id,
                                      EventQuizquizStamp* records, int max_count, int* actual_count);
int EventQuizquizStamp_GetByDegree(DBConnectionManager* manager, char degree,
                                    EventQuizquizStamp* records, int max_count, int* actual_count);
int EventQuizquizStamp_GetAll(DBConnectionManager* manager, EventQuizquizStamp* records,
                               int max_count, int* actual_count);
int EventQuizquizStamp_UpdateStamp(DBConnectionManager* manager, int m_id, char degree,
                                    int stamp_increment);

/* Utility Functions */
void EventQuizquizStamp_PrintInfo(const EventQuizquizStamp* record);

#endif
