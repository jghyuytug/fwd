#ifndef DNF_TUNE_ORM_H
#define DNF_TUNE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    char link_tag[201];
    char img_name[201];
    int create_time;
    char apply_type[2];
} DnfTune;

/* CRUD Operations */
int DnfTune_Add(DBConnectionManager* manager, const DnfTune* record);
int DnfTune_Get(DBConnectionManager* manager, int no, DnfTune* record);
int DnfTune_Update(DBConnectionManager* manager, const DnfTune* record);
int DnfTune_Delete(DBConnectionManager* manager, int no);
int DnfTune_Exists(DBConnectionManager* manager, int no);
int DnfTune_GetAll(DBConnectionManager* manager, DnfTune* records, int max_count, int* actual_count);

#endif /* DNF_TUNE_ORM_H */