#ifndef DNF_GAMEIN_NOTICE_ORM_H
#define DNF_GAMEIN_NOTICE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned int no;
    char img_name[251];
    unsigned char server_id;
    char reg_time[20];
    char open_flag;
} DnfGameinNotice;

/* CRUD Operations */
int DnfGameinNotice_Add(DBConnectionManager* manager, const DnfGameinNotice* notice);
int DnfGameinNotice_Get(DBConnectionManager* manager, unsigned int no, DnfGameinNotice* notice);
int DnfGameinNotice_Update(DBConnectionManager* manager, const DnfGameinNotice* notice);
int DnfGameinNotice_Delete(DBConnectionManager* manager, unsigned int no);
int DnfGameinNotice_Exists(DBConnectionManager* manager, unsigned int no);

/* Business Operations */
int DnfGameinNotice_GetByServerId(DBConnectionManager* manager, unsigned char server_id,
                                  DnfGameinNotice* notices, int max_count, int* actual_count);
int DnfGameinNotice_GetByOpenFlag(DBConnectionManager* manager, char open_flag,
                                  DnfGameinNotice* notices, int max_count, int* actual_count);
int DnfGameinNotice_GetAll(DBConnectionManager* manager, DnfGameinNotice* notices,
                           int max_count, int* actual_count);

/* Utility Functions */
void DnfGameinNotice_PrintInfo(const DnfGameinNotice* notice);

#endif
