#ifndef MY_GUESTBOOK0_REPLY_ORM_H
#define MY_GUESTBOOK0_REPLY_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int r_id;
    int no;
    char comment_reply[1024];
} MyGuestbook0Reply;

/* CRUD Operations */
int MyGuestbook0Reply_Add(DBConnectionManager* manager, const MyGuestbook0Reply* record);
int MyGuestbook0Reply_Get(DBConnectionManager* manager, int r_id, MyGuestbook0Reply* record);
int MyGuestbook0Reply_Update(DBConnectionManager* manager, const MyGuestbook0Reply* record);
int MyGuestbook0Reply_Delete(DBConnectionManager* manager, int r_id);
int MyGuestbook0Reply_Exists(DBConnectionManager* manager, int r_id);
int MyGuestbook0Reply_GetAll(DBConnectionManager* manager, MyGuestbook0Reply* records, int max_count, int* actual_count);

#endif /* MY_GUESTBOOK0_REPLY_ORM_H */