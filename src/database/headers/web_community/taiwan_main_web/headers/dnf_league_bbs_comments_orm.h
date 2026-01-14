#ifndef DNF_LEAGUE_BBS_COMMENTS_ORM_H
#define DNF_LEAGUE_BBS_COMMENTS_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int comment_id;
    int bbs_id;
    int m_id;
    signed char server_id;
    int charac_no;
    char charac_name[21];
    signed char job;
    char comments[201];
    int reg_time;
    signed char delete_flag;
} DnfLeagueBbsComments;

/* CRUD Operations */
int DnfLeagueBbsComments_Add(DBConnectionManager* manager, const DnfLeagueBbsComments* record);
int DnfLeagueBbsComments_Get(DBConnectionManager* manager, int comment_id, DnfLeagueBbsComments* record);
int DnfLeagueBbsComments_Update(DBConnectionManager* manager, const DnfLeagueBbsComments* record);
int DnfLeagueBbsComments_Delete(DBConnectionManager* manager, int comment_id);
int DnfLeagueBbsComments_Exists(DBConnectionManager* manager, int comment_id);
int DnfLeagueBbsComments_GetAll(DBConnectionManager* manager, DnfLeagueBbsComments* records, int max_count, int* actual_count);

#endif /* DNF_LEAGUE_BBS_COMMENTS_ORM_H */