#ifndef EVENT_QUEST_PARTY_MEMBER_WEB_ORM_H
#define EVENT_QUEST_PARTY_MEMBER_WEB_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    unsigned char server_id;
    int charac_no;
    int quest_no;
    char occ_time[20];
    int send_charac_no;
} EventQuestPartyMemberWeb;

/* CRUD operations - composite primary key (m_id, server_id, charac_no, quest_no) */
int EventQuestPartyMemberWeb_Add(DBConnectionManager* manager, const EventQuestPartyMemberWeb* member);
int EventQuestPartyMemberWeb_Get(DBConnectionManager* manager, int m_id, unsigned char server_id,
                                  int charac_no, int quest_no, EventQuestPartyMemberWeb* member);
int EventQuestPartyMemberWeb_Update(DBConnectionManager* manager, const EventQuestPartyMemberWeb* member);
int EventQuestPartyMemberWeb_Delete(DBConnectionManager* manager, int m_id, unsigned char server_id,
                                     int charac_no, int quest_no);
int EventQuestPartyMemberWeb_Exists(DBConnectionManager* manager, int m_id, unsigned char server_id,
                                     int charac_no, int quest_no);

/* Business operations */
int EventQuestPartyMemberWeb_GetByMember(DBConnectionManager* manager, int m_id, unsigned char server_id,
                                          int charac_no, EventQuestPartyMemberWeb* members,
                                          int max_count, int* actual_count);
int EventQuestPartyMemberWeb_GetByQuest(DBConnectionManager* manager, int quest_no,
                                         EventQuestPartyMemberWeb* members,
                                         int max_count, int* actual_count);
int EventQuestPartyMemberWeb_DeleteByMember(DBConnectionManager* manager, int m_id,
                                             unsigned char server_id, int charac_no);

/* Utility */
void EventQuestPartyMemberWeb_PrintInfo(const EventQuestPartyMemberWeb* member);

#endif
