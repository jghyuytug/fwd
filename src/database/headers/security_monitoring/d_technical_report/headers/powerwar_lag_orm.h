#ifndef POWERWAR_LAG_ORM_H
#define POWERWAR_LAG_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    char occ_time[20];
    short round;
    signed char player;
    float lag_avg;
    float lag_cnt;
} PowerwarLag;

/* CRUD Operations */
int PowerwarLag_Add(DBConnectionManager* manager, const PowerwarLag* record);
int PowerwarLag_Get(DBConnectionManager* manager, int m_id, const char* occ_time, short round, PowerwarLag* record);
int PowerwarLag_Update(DBConnectionManager* manager, const PowerwarLag* record);
int PowerwarLag_Delete(DBConnectionManager* manager, int m_id, const char* occ_time, short round);
int PowerwarLag_Exists(DBConnectionManager* manager, int m_id, const char* occ_time, short round);
int PowerwarLag_GetAll(DBConnectionManager* manager, PowerwarLag* records, int max_count, int* actual_count);

#endif /* POWERWAR_LAG_ORM_H */