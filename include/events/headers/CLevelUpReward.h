#ifndef CLEVELUPREWARD_H
#define CLEVELUPREWARD_H

#include <common/defs.h>

// CLevelUpReward declarations
EventClassify::CLevelUpReward *__cdecl EventClassify::CLevelUpReward::set_event_data(EventClassify::CLevelUpReward *this, int id, InGameEventScript *a3);
int __cdecl EventClassify::CLevelUpReward::check_recv_level(EventClassify::CLevelUpReward *this, int id, int value, int count, bool a5);
int __cdecl EventClassify::CLevelUpReward::get_db_table_name(EventClassify::CLevelUpReward *this);
int __cdecl EventClassify::CLevelUpReward::get_mail_data(EventClassify::CLevelUpReward *this);
int __cdecl EventClassify::CLevelUpReward::is_recv_check_item(EventClassify::CLevelUpReward *this);
int __cdecl EventClassify::CLevelUpReward::set_reward_item(EventClassify::CLevelUpReward *this, int id, int value, int count);
int __cdecl EventClassify::CLevelUpReward::send_reward_from_recv_level(EventClassify::CLevelUpReward *this, CUser *user, int id, int value);
EventClassify::CLevelUpReward *__cdecl EventClassify::CLevelUpReward::CLevelUpReward(EventClassify::CLevelUpReward *this);

#endif // CLEVELUPREWARD_H
