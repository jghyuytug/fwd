#ifndef MONITORING_SPEC_ORM_H
#define MONITORING_SPEC_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int unique_id;
    char modify_time[20];
    int spec_id;
    signed char cpu_vendor;
    signed char cpu_processor_num;
    int above_cpu_clock;
    int below_cpu_clock;
    short ram;
    int videocard_vendor;
    int videocard_device;
    short videocard_texture_mem;
    signed char os_version;
} MonitoringSpec;

/* CRUD Operations */
int MonitoringSpec_Add(DBConnectionManager* manager, const MonitoringSpec* record);
int MonitoringSpec_Get(DBConnectionManager* manager, int unique_id, MonitoringSpec* record);
int MonitoringSpec_Update(DBConnectionManager* manager, const MonitoringSpec* record);
int MonitoringSpec_Delete(DBConnectionManager* manager, int unique_id);
int MonitoringSpec_Exists(DBConnectionManager* manager, int unique_id);
int MonitoringSpec_GetAll(DBConnectionManager* manager, MonitoringSpec* records, int max_count, int* actual_count);

#endif /* MONITORING_SPEC_ORM_H */