#ifndef FOCUS_GAME_ORM_H
#define FOCUS_GAME_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int reg_date;
    int open_date;
    int comment;
} FocusGame;

/* CRUD Operations */
int FocusGame_Add(DBConnectionManager* manager, const FocusGame* record);
int FocusGame_Get(DBConnectionManager* manager, int no, FocusGame* record);
int FocusGame_Update(DBConnectionManager* manager, const FocusGame* record);
int FocusGame_Delete(DBConnectionManager* manager, int no);
int FocusGame_Exists(DBConnectionManager* manager, int no);
int FocusGame_GetAll(DBConnectionManager* manager, FocusGame* records, int max_count, int* actual_count);

#endif /* FOCUS_GAME_ORM_H */