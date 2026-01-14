#ifndef MEMBER_PLAY_INFO_ORM_H
#define MEMBER_PLAY_INFO_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char occ_date[11];              /* Occurrence date (YYYY-MM-DD) */
    int m_id;                       /* Member ID */
    unsigned int play_time;         /* Total play time in seconds */
    unsigned int play_count;        /* Number of logins */
    int trade_cnt;                  /* Trade count */
    unsigned int exp;               /* Experience gained */
    unsigned short used_fatigue;    /* Fatigue points used */
    char ip[16];                    /* Login IP address */
    unsigned int last_play_time;    /* Last play timestamp */
    unsigned char pcbang_flag;      /* PC bang flag (0=home, 1=pcbang) */
    char end_ip[4];                 /* End IP segment */
    unsigned short ting_count;      /* Ting count */
    char mac_addr[65];              /* MAC address */
    unsigned char server_id;        /* Server ID */
} MemberPlayInfo;

/* CRUD Operations */
int MemberPlayInfo_Add(DBConnectionManager* manager, const MemberPlayInfo* info);
int MemberPlayInfo_Get(DBConnectionManager* manager, const char* occ_date, int m_id,
                        MemberPlayInfo* info);
int MemberPlayInfo_Update(DBConnectionManager* manager, const MemberPlayInfo* info);
int MemberPlayInfo_Delete(DBConnectionManager* manager, const char* occ_date, int m_id);
int MemberPlayInfo_Exists(DBConnectionManager* manager, const char* occ_date, int m_id);

/* Business Operations */
int MemberPlayInfo_GetByMemberId(DBConnectionManager* manager, int m_id,
                                  MemberPlayInfo* infos, int max_count, int* actual_count);
int MemberPlayInfo_GetByDateRange(DBConnectionManager* manager, const char* start_date,
                                   const char* end_date, MemberPlayInfo* infos,
                                   int max_count, int* actual_count);
int MemberPlayInfo_GetByServer(DBConnectionManager* manager, unsigned char server_id,
                                MemberPlayInfo* infos, int max_count, int* actual_count);
int MemberPlayInfo_GetPCBangUsers(DBConnectionManager* manager, const char* occ_date,
                                   MemberPlayInfo* infos, int max_count, int* actual_count);

/* Utility Functions */
void MemberPlayInfo_PrintInfo(const MemberPlayInfo* info);

#endif /* MEMBER_PLAY_INFO_ORM_H */
