#ifndef EVENT_QUEST_PARTY_MEMBER_ORM_H
#define EVENT_QUEST_PARTY_MEMBER_ORM_H

#include "db_connection_manager.h"

typedef struct {
    int charac_no;
    int quest_no;
    int member_1;
    char member_1_name[21];
    signed char member_1_lev;
    int member_2;
    char member_2_name[21];
    signed char member_2_lev;
    int member_3;
    char member_3_name[21];
    signed char member_3_lev;
    int m_id;
} EventQuestPartyMember;

/* CRUD Operations */
int EventQuestPartyMember_Add(DBConnectionManager* manager, const EventQuestPartyMember* record);
int EventQuestPartyMember_Get(DBConnectionManager* manager, int charac_no, int quest_no, EventQuestPartyMember* record);
int EventQuestPartyMember_Update(DBConnectionManager* manager, const EventQuestPartyMember* record);
int EventQuestPartyMember_Delete(DBConnectionManager* manager, int charac_no, int quest_no);
int EventQuestPartyMember_Exists(DBConnectionManager* manager, int charac_no, int quest_no);
int EventQuestPartyMember_GetAll(DBConnectionManager* manager, EventQuestPartyMember* records, int max_count, int* actual_count);

#endif /* EVENT_QUEST_PARTY_MEMBER_ORM_H */