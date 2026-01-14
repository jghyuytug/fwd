#ifndef DBLAB_AVATAR_SOCKET_ORM_H
#define DBLAB_AVATAR_SOCKET_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int it_id;
    char jewel_socket[601];
} DblabAvatarSocket;

/* CRUD Operations */
int DblabAvatarSocket_Add(DBConnectionManager* manager, const DblabAvatarSocket* record);
int DblabAvatarSocket_Get(DBConnectionManager* manager, int it_id, DblabAvatarSocket* record);
int DblabAvatarSocket_Update(DBConnectionManager* manager, const DblabAvatarSocket* record);
int DblabAvatarSocket_Delete(DBConnectionManager* manager, int it_id);
int DblabAvatarSocket_Exists(DBConnectionManager* manager, int it_id);
int DblabAvatarSocket_GetAll(DBConnectionManager* manager, DblabAvatarSocket* records, int max_count, int* actual_count);

#endif /* DBLAB_AVATAR_SOCKET_ORM_H */