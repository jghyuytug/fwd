#ifndef MY_GUESTBOOK0_ORM_H
#define MY_GUESTBOOK0_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int master;
    char m_nickname[13];
    int m_id;
    char m_user_id[17];
    char m_sex;
    char comment_text[1024];
    signed char emoticon;
    char ip[16];
    int create_day;
} MyGuestbook0;

/* CRUD Operations */
int MyGuestbook0_Add(DBConnectionManager* manager, const MyGuestbook0* record);
int MyGuestbook0_Get(DBConnectionManager* manager, int master, int no, MyGuestbook0* record);
int MyGuestbook0_Update(DBConnectionManager* manager, const MyGuestbook0* record);
int MyGuestbook0_Delete(DBConnectionManager* manager, int master, int no);
int MyGuestbook0_Exists(DBConnectionManager* manager, int master, int no);
int MyGuestbook0_GetAll(DBConnectionManager* manager, MyGuestbook0* records, int max_count, int* actual_count);

#endif /* MY_GUESTBOOK0_ORM_H */