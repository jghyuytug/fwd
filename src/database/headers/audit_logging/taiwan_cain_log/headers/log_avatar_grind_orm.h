#ifndef LOG_AVATAR_GRIND_ORM_H
#define LOG_AVATAR_GRIND_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char cur_date[11];
    char grade;
    char body_part;
    int avatar_emblem_grind;
    int avatar_bindcube_grind;
    int avatar_rechargestone_grind;
} LogAvatarGrind;

/* CRUD Operations */
int LogAvatarGrind_Add(DBConnectionManager* manager, const LogAvatarGrind* record);
int LogAvatarGrind_Get(DBConnectionManager* manager, const char* cur_date, char grade, char body_part, LogAvatarGrind* record);
int LogAvatarGrind_Update(DBConnectionManager* manager, const LogAvatarGrind* record);
int LogAvatarGrind_Delete(DBConnectionManager* manager, const char* cur_date, char grade, char body_part);
int LogAvatarGrind_Exists(DBConnectionManager* manager, const char* cur_date, char grade, char body_part);
int LogAvatarGrind_GetAll(DBConnectionManager* manager, LogAvatarGrind* records, int max_count, int* actual_count);

#endif /* LOG_AVATAR_GRIND_ORM_H */