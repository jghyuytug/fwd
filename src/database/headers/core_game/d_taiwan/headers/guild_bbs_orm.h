#ifndef GUILD_BBS_ORM_H
#define GUILD_BBS_ORM_H

#include "db_connection_manager.h"

typedef struct {
    unsigned int gno;           /* Auto-increment primary key */
    unsigned char bd_id;        /* Board ID */
    unsigned char empyn;        /* Empty flag */
    int mgno;                   /* Message group number */
    unsigned char open;         /* Open flag (1=open, 0=closed) */
    unsigned char main;         /* Main topic flag */
    unsigned int reg_date;      /* Registration date (Unix timestamp) */
    unsigned int mod_date;      /* Modification date (Unix timestamp) */
    unsigned int hits;          /* View count */
    char body_type;             /* Body type */
    int m_id;                   /* Member ID */
    char reg_id[21];            /* Registration ID (varchar 20 + 1) */
    char subject[51];           /* Subject (varchar 50 + 1) */
} GuildBbs;

/* CRUD Operations */
int GuildBbs_Add(DBConnectionManager* manager, const GuildBbs* bbs);
int GuildBbs_Get(DBConnectionManager* manager, unsigned int gno, GuildBbs* bbs);
int GuildBbs_Update(DBConnectionManager* manager, const GuildBbs* bbs);
int GuildBbs_Delete(DBConnectionManager* manager, unsigned int gno);
int GuildBbs_Exists(DBConnectionManager* manager, unsigned int gno);

/* Business Operations */
int GuildBbs_GetByBoard(DBConnectionManager* manager, unsigned char bd_id,
                        GuildBbs* bbs_list, int max_count, int* actual_count);
int GuildBbs_GetByBoardAndEmpyn(DBConnectionManager* manager, unsigned char bd_id,
                                 unsigned char empyn, GuildBbs* bbs_list,
                                 int max_count, int* actual_count);
int GuildBbs_GetByMember(DBConnectionManager* manager, int m_id,
                         GuildBbs* bbs_list, int max_count, int* actual_count);
int GuildBbs_IncrementHits(DBConnectionManager* manager, unsigned int gno);
int GuildBbs_UpdateSubject(DBConnectionManager* manager, unsigned int gno,
                            const char* new_subject);

/* Utility Functions */
void GuildBbs_PrintInfo(const GuildBbs* bbs);

#endif