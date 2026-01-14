#ifndef RDAR_BETA_TESTER_ORM_H
#define RDAR_BETA_TESTER_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    char m_user_id[17];
    char email[51];
    int req_date;
    char pc_spec[1024];
    signed char line_code;
    signed char job_code;
    signed char genre_code;
    char fav_game[31];
    char req_reason[256];
    signed char winning_yn;
    signed char times;
} RdarBetaTester;

/* CRUD Operations */
int RdarBetaTester_Add(DBConnectionManager* manager, const RdarBetaTester* record);
int RdarBetaTester_Get(DBConnectionManager* manager, int m_id, RdarBetaTester* record);
int RdarBetaTester_Update(DBConnectionManager* manager, const RdarBetaTester* record);
int RdarBetaTester_Delete(DBConnectionManager* manager, int m_id);
int RdarBetaTester_Exists(DBConnectionManager* manager, int m_id);
int RdarBetaTester_GetAll(DBConnectionManager* manager, RdarBetaTester* records, int max_count, int* actual_count);

#endif /* RDAR_BETA_TESTER_ORM_H */