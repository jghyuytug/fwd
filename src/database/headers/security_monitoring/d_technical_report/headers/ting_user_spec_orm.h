#ifndef TING_USER_SPEC_ORM_H
#define TING_USER_SPEC_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    char reg_datetime[20];
    char cpu_vendor[2];
    char cpu_num[2];
    int cpu_clock;
    short ram;
    short video_vendor;
    short video_device;
    short video_ram;
    char os[2];
    char os_bit[2];
} TingUserSpec;

/* CRUD Operations */
int TingUserSpec_Add(DBConnectionManager* manager, const TingUserSpec* record);
int TingUserSpec_Get(DBConnectionManager* manager, int m_id, TingUserSpec* record);
int TingUserSpec_Update(DBConnectionManager* manager, const TingUserSpec* record);
int TingUserSpec_Delete(DBConnectionManager* manager, int m_id);
int TingUserSpec_Exists(DBConnectionManager* manager, int m_id);
int TingUserSpec_GetAll(DBConnectionManager* manager, TingUserSpec* records, int max_count, int* actual_count);

#endif /* TING_USER_SPEC_ORM_H */