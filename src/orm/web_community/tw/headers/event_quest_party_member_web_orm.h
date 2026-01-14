#ifndef EVENT_QUEST_PARTY_MEMBER_WEB_ORM_H
#define EVENT_QUEST_PARTY_MEMBER_WEB_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int m_id;
    signed char server_id;
    int charac_no;
    int quest_no;
    char occ_time[20];
    int send_charac_no;
} EventQuestPartyMemberWeb;

/* CRUD Operations */
int EventQuestPartyMemberWeb_Add(DBConnectionManager* manager, const EventQuestPartyMemberWeb* record);
int EventQuestPartyMemberWeb_Get(DBConnectionManager* manager, int m_id, signed char server_id, int charac_no, int quest_no, EventQuestPartyMemberWeb* record);
int EventQuestPartyMemberWeb_Update(DBConnectionManager* manager, const EventQuestPartyMemberWeb* record);
int EventQuestPartyMemberWeb_Delete(DBConnectionManager* manager, int m_id, signed char server_id, int charac_no, int quest_no);
int EventQuestPartyMemberWeb_Exists(DBConnectionManager* manager, int m_id, signed char server_id, int charac_no, int quest_no);
int EventQuestPartyMemberWeb_GetAll(DBConnectionManager* manager, EventQuestPartyMemberWeb* records, int max_count, int* actual_count);

#endif /* EVENT_QUEST_PARTY_MEMBER_WEB_ORM_H */