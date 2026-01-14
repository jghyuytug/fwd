#ifndef LOG_POLICE_CHATTING_ORM_H
#define LOG_POLICE_CHATTING_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int no;
    int charac_no;
    signed char msg_type;
    int occ_time;
    char message[256];
} LogPoliceChatting;

/* CRUD Operations */
int LogPoliceChatting_Add(DBConnectionManager* manager, const LogPoliceChatting* record);
int LogPoliceChatting_Get(DBConnectionManager* manager, int no, LogPoliceChatting* record);
int LogPoliceChatting_Update(DBConnectionManager* manager, const LogPoliceChatting* record);
int LogPoliceChatting_Delete(DBConnectionManager* manager, int no);
int LogPoliceChatting_Exists(DBConnectionManager* manager, int no);
int LogPoliceChatting_GetAll(DBConnectionManager* manager, LogPoliceChatting* records, int max_count, int* actual_count);

#endif /* LOG_POLICE_CHATTING_ORM_H */