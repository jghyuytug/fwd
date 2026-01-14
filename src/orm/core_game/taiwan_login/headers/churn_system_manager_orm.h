#ifndef CHURN_SYSTEM_MANAGER_ORM_H
#define CHURN_SYSTEM_MANAGER_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned int no;             /* ID (AUTO_INCREMENT PK) */
    int weekday_var_a;           /* Weekday variable A */
    int weekday_var_b;           /* Weekday variable B */
    int weekday_var_c;           /* Weekday variable C */
    int weekend_var_x;           /* Weekend variable X */
    int weekend_var_y;           /* Weekend variable Y */
    int weekend_var_z;           /* Weekend variable Z */
    int next_reward_day;         /* Next reward day */
    unsigned int admin_id;       /* Admin ID */
    char reg_time[20];           /* Registration time */
    unsigned char state_flag;    /* State flag */
} ChurnSystemManager;

/* CRUD Operations - Add does NOT include no (AUTO_INCREMENT) */
int ChurnSystemManager_Add(DBConnectionManager* manager, const ChurnSystemManager* config);
int ChurnSystemManager_Get(DBConnectionManager* manager, unsigned int no, ChurnSystemManager* config);
int ChurnSystemManager_Update(DBConnectionManager* manager, const ChurnSystemManager* config);
int ChurnSystemManager_Delete(DBConnectionManager* manager, unsigned int no);
int ChurnSystemManager_Exists(DBConnectionManager* manager, unsigned int no);

/* Business Operations */
int ChurnSystemManager_GetActive(DBConnectionManager* manager, ChurnSystemManager* configs,
                                  int max_count, int* actual_count);
int ChurnSystemManager_GetByAdmin(DBConnectionManager* manager, unsigned int admin_id,
                                   ChurnSystemManager* configs, int max_count, int* actual_count);
int ChurnSystemManager_GetLatest(DBConnectionManager* manager, ChurnSystemManager* config);

/* Utility Functions */
void ChurnSystemManager_PrintInfo(const ChurnSystemManager* config);

#endif /* CHURN_SYSTEM_MANAGER_ORM_H */
