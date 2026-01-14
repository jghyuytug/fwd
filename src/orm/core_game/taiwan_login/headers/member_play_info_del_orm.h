#ifndef MEMBER_PLAY_INFO_DEL_ORM_H
#define MEMBER_PLAY_INFO_DEL_ORM_H

#include "db_connection_manager.h"

typedef struct {
    char sdate[11];                 /* Delete date (YYYY-MM-DD) */
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
} MemberPlayInfoDel;

/* CRUD Operations */
int MemberPlayInfoDel_Add(DBConnectionManager* manager, const MemberPlayInfoDel* info);
int MemberPlayInfoDel_Get(DBConnectionManager* manager, const char* sdate,
                           const char* occ_date, int m_id, MemberPlayInfoDel* info);
int MemberPlayInfoDel_Update(DBConnectionManager* manager, const MemberPlayInfoDel* info);
int MemberPlayInfoDel_Delete(DBConnectionManager* manager, const char* sdate,
                              const char* occ_date, int m_id);
int MemberPlayInfoDel_Exists(DBConnectionManager* manager, const char* sdate,
                              const char* occ_date, int m_id);

/* Business Operations */
int MemberPlayInfoDel_GetByMemberId(DBConnectionManager* manager, int m_id,
                                     MemberPlayInfoDel* infos, int max_count, int* actual_count);
int MemberPlayInfoDel_GetByDeleteDate(DBConnectionManager* manager, const char* sdate,
                                       MemberPlayInfoDel* infos, int max_count, int* actual_count);
int MemberPlayInfoDel_GetByDateRange(DBConnectionManager* manager, const char* start_sdate,
                                      const char* end_sdate, MemberPlayInfoDel* infos,
                                      int max_count, int* actual_count);

/* Utility Functions */
void MemberPlayInfoDel_PrintInfo(const MemberPlayInfoDel* info);

#endif /* MEMBER_PLAY_INFO_DEL_ORM_H */
