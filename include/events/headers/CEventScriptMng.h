#ifndef CEVENTSCRIPTMNG_H
#define CEVENTSCRIPTMNG_H

#include <common/defs.h>

// CEventScriptMng declarations
int __cdecl EventClassify::CEventScriptMng::get_convert_to_user_confirmflag(EventClassify::CEventScriptMng *this, int id);
int __cdecl EventClassify::CEventScriptMng::get_db_table_name(EventClassify::CEventScriptMng *this, int id);
int __cdecl EventClassify::CEventScriptMng::CEventScriptMng(EventClassify::CEventScriptMng *this);
EventClassify::CEventEntity *__cdecl EventClassify::CEventScriptMng::create_event_entity(EventClassify::CEventScriptMng *this, int id, bool a3);
int EventClassify::CEventScriptMng::get_event_entity(EventClassify::CEventScriptMng *this, ...);
_BOOL4 EventClassify::CEventScriptMng::is_event_entity(EventClassify::CEventScriptMng *this, ...);
int __cdecl EventClassify::CEventScriptMng::is_eventing(EventClassify::CEventScriptMng *this, unsigned int id);
int __cdecl EventClassify::CEventScriptMng::initScriptlist(EventClassify::CEventScriptMng *this, InGameEventInfoList *a2);
bool __cdecl EventClassify::CEventScriptMng::init_evnet_entity(EventClassify::CEventScriptMng *this);
void __cdecl EventClassify::CEventScriptMng::process_account_first_login(EventClassify::CEventScriptMng *a1, unsigned int id, int value, int count);
void __cdecl EventClassify::CEventScriptMng::process_add_item_count_db(EventClassify::CEventScriptMng *this, CUser *user, int id, unsigned __int16 a4);
void __cdecl EventClassify::CEventScriptMng::process_add_quest_count_db(EventClassify::CEventScriptMng *this, CUser *user, int id);
void __cdecl EventClassify::CEventScriptMng::process_level_up_reward(EventClassify::CEventScriptMng *this, CUser *user, bool a3);
void __cdecl EventClassify::CEventScriptMng::process_level_up_reward_recv_level(EventClassify::CEventScriptMng *this, CUser *user, int id, int value);
void __cdecl EventClassify::CEventScriptMng::process_add_fatigue_count(EventClassify::CEventScriptMng *this, CUser *user, __int16 a3);
char __cdecl EventClassify::CEventScriptMng::inc_user_fatigue(EventClassify::CEventScriptMng *this, CUser *user, unsigned int id);
char __cdecl EventClassify::CEventScriptMng::update_purchase_target_item_count(EventClassify::CEventScriptMng *a1, CUser *user, char flag, int id);
void __cdecl EventClassify::CEventScriptMng::send_pc_room_play_init_time(EventClassify::CEventScriptMng *this, CUser *user);
void __cdecl EventClassify::CEventScriptMng::send_event_init_data(EventClassify::CEventScriptMng *this, CUser *user, bool a3);
EventClassify::CEventScriptMng *__cdecl EventClassify::CEventScriptMng::update_time_values(EventClassify::CEventScriptMng *this, int id);
int __cdecl EventClassify::CEventScriptMng::get_today_time_day(EventClassify::CEventScriptMng *this);
int __cdecl EventClassify::CEventScriptMng::get_today_time(EventClassify::CEventScriptMng *this);
bool __cdecl EventClassify::CEventScriptMng::get_ingame_event_record_type_list(int id, int *a2);
_BOOL4 EventClassify::CEventScriptMng::is_current_ingame_event_record_type(EventClassify::CEventScriptMng *this, ...);
int EventClassify::CEventScriptMng::get_record_type(EventClassify::CEventScriptMng *this, ...);
void __cdecl EventClassify::CEventScriptMng::process_update_ingame_history(EventClassify::CEventScriptMng *this, CUser *user);
int __cdecl EventClassify::CEventScriptMng::get_pass_day(EventClassify::CEventScriptMng *this, int id, int value);
void __cdecl EventClassify::CEventScriptMng::send_event_init_record_data(EventClassify::CEventScriptMng *this, CUser *user);
void __cdecl EventClassify::CEventScriptMng::test_process_change_day(EventClassify::CEventScriptMng *this, int id);
int __cdecl EventClassify::CEventScriptMng::init_event_nums(EventClassify::CEventScriptMng *this);

#endif // CEVENTSCRIPTMNG_H
