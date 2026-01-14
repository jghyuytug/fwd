#ifndef REALRADIO_MAIN_ORM_H
#define REALRADIO_MAIN_ORM_H

#include "db_connection_manager.h"

typedef struct {
    signed char type;
    char text1[1024];
    char text2[1024];
} RealradioMain;

/* CRUD Operations */
int RealradioMain_Add(DBConnectionManager* manager, const RealradioMain* record);
int RealradioMain_Get(DBConnectionManager* manager, signed char type, RealradioMain* record);
int RealradioMain_Update(DBConnectionManager* manager, const RealradioMain* record);
int RealradioMain_Delete(DBConnectionManager* manager, signed char type);
int RealradioMain_Exists(DBConnectionManager* manager, signed char type);
int RealradioMain_GetAll(DBConnectionManager* manager, RealradioMain* records, int max_count, int* actual_count);

#endif /* REALRADIO_MAIN_ORM_H */