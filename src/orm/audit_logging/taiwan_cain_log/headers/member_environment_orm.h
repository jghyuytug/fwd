#ifndef MEMBER_ENVIRONMENT_ORM_H
#define MEMBER_ENVIRONMENT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int os_type;
    int hard_wholesize;
    int hard_remainsize;
    int memory_wholesize;
    int memory_remainsize;
    int virtualmem_wholesize;
    int virtualmem_remainsize;
    int cpu_speed;
    int cpu_count;
    char cpu_vendor[21];
    char graphic_name[129];
    int graphic_memory;
    int average_fps;
    int reg_date;
} MemberEnvironment;

/* CRUD Operations */
int MemberEnvironment_Add(DBConnectionManager* manager, const MemberEnvironment* record);
int MemberEnvironment_GetAll(DBConnectionManager* manager, MemberEnvironment* records, int max_count, int* actual_count);

#endif /* MEMBER_ENVIRONMENT_ORM_H */