#ifndef GAME_GENRE_ORM_H
#define GAME_GENRE_ORM_H

#include "db_connection_manager.h"

typedef struct {
    signed char genre_code;
    char jenre_name[11];
} GameGenre;

/* CRUD Operations */
int GameGenre_Add(DBConnectionManager* manager, const GameGenre* record);
int GameGenre_Get(DBConnectionManager* manager, signed char genre_code, GameGenre* record);
int GameGenre_Update(DBConnectionManager* manager, const GameGenre* record);
int GameGenre_Delete(DBConnectionManager* manager, signed char genre_code);
int GameGenre_Exists(DBConnectionManager* manager, signed char genre_code);
int GameGenre_GetAll(DBConnectionManager* manager, GameGenre* records, int max_count, int* actual_count);

#endif /* GAME_GENRE_ORM_H */