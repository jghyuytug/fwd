#ifndef CHECK_MANAGER_ORM_H
#define CHECK_MANAGER_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char check_start_time[20];
    char check_end_time[20];
    signed char type;
    char checkup_content[1024];
    char dnf_title_1[121];
    char dnf_link_1[256];
    char dnf_source_1[121];
    char dnf_title_2[121];
    char dnf_link_2[256];
    char dnf_source_2[121];
    char dnf_title_3[121];
    char dnf_link_3[256];
    char dnf_source_3[121];
    char dnf_title_4[121];
    char dnf_link_4[256];
    char dnf_source_4[121];
    char dnf_title_5[121];
    char dnf_link_5[256];
    char dnf_source_5[121];
    char dnf_title_6[121];
    char dnf_link_6[256];
    char dnf_source_6[121];
} CheckManager;

/* CRUD Operations */
int CheckManager_Add(DBConnectionManager* manager, const CheckManager* record);
int CheckManager_GetAll(DBConnectionManager* manager, CheckManager* records, int max_count, int* actual_count);

#endif /* CHECK_MANAGER_ORM_H */