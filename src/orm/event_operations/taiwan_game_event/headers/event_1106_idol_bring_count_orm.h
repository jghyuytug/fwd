#ifndef EVENT_1106_IDOL_BRING_COUNT_ORM_H
#define EVENT_1106_IDOL_BRING_COUNT_ORM_H

#include "db_connection_manager.h"

typedef struct {
    signed char server_id;
    signed char pot_type;
    int r_count;
    int adjust_value;
} Event1106IdolBringCount;

/* CRUD Operations */
int Event1106IdolBringCount_Add(DBConnectionManager* manager, const Event1106IdolBringCount* record);
int Event1106IdolBringCount_Get(DBConnectionManager* manager, signed char server_id, signed char pot_type, Event1106IdolBringCount* record);
int Event1106IdolBringCount_Update(DBConnectionManager* manager, const Event1106IdolBringCount* record);
int Event1106IdolBringCount_Delete(DBConnectionManager* manager, signed char server_id, signed char pot_type);
int Event1106IdolBringCount_Exists(DBConnectionManager* manager, signed char server_id, signed char pot_type);
int Event1106IdolBringCount_GetAll(DBConnectionManager* manager, Event1106IdolBringCount* records, int max_count, int* actual_count);

#endif /* EVENT_1106_IDOL_BRING_COUNT_ORM_H */