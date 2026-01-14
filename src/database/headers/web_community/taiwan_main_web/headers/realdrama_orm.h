#ifndef REALDRAMA_ORM_H
#define REALDRAMA_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char title[121];
    int create_day;
    int broadcast_day;
    char appear[31];
    char plan[16];
    char play[16];
    char summary[201];
    char content[1024];
    int view;
    char banner1[129];
    char banner2[129];
    char file_url[129];
    signed char open_flag;
    signed char main_flag;
} Realdrama;

/* CRUD Operations */
int Realdrama_Add(DBConnectionManager* manager, const Realdrama* record);
int Realdrama_Get(DBConnectionManager* manager, int no, Realdrama* record);
int Realdrama_Update(DBConnectionManager* manager, const Realdrama* record);
int Realdrama_Delete(DBConnectionManager* manager, int no);
int Realdrama_Exists(DBConnectionManager* manager, int no);
int Realdrama_GetAll(DBConnectionManager* manager, Realdrama* records, int max_count, int* actual_count);

#endif /* REALDRAMA_ORM_H */