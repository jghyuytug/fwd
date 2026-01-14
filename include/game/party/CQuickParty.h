#ifndef CQUICKPARTY_H
#define CQUICKPARTY_H

#include <common/defs.h>

// CQuickParty declarations
QuickParty::CQuickParty *__cdecl QuickParty::CQuickParty::CQuickParty(QuickParty::CQuickParty *this);
QuickParty::CQuickParty *__cdecl QuickParty::CQuickParty::init(QuickParty::CQuickParty *this);
int __cdecl QuickParty::CQuickParty::destroy(QuickParty::CQuickParty *this);
int __cdecl QuickParty::CQuickParty::get_timer_key(int id, int value);
int __cdecl QuickParty::CQuickParty::gen_timer_key(int id, int value);
bool __cdecl QuickParty::CQuickParty::check_timer_key(int id, int value, int count);
int __cdecl QuickParty::CQuickParty::get_party_object(QuickParty::CQuickParty *this, unsigned int id);
int __cdecl QuickParty::CQuickParty::get_party_object(QuickParty::CQuickParty *this);
char __cdecl QuickParty::CQuickParty::set_party_index(QuickParty::CQuickParty *this, int id);
QuickParty::CQuickParty *__cdecl QuickParty::CQuickParty::set_party_index(QuickParty::CQuickParty *this, unsigned int id, int value);
int __cdecl QuickParty::CQuickParty::get_party_index(QuickParty::CQuickParty *this, unsigned int id);
int __cdecl QuickParty::CQuickParty::delete_party_index(QuickParty::CQuickParty *this, int id);
bool __cdecl QuickParty::CQuickParty::delete_party_index(QuickParty::CQuickParty *this);
int __cdecl QuickParty::CQuickParty::is_empty_quick_party(QuickParty::CQuickParty *this);
int __cdecl QuickParty::CQuickParty::get_quick_party_count(QuickParty::CQuickParty *this);
int __cdecl QuickParty::CQuickParty::get_matching_quick_party_master_unique_id(QuickParty::CQuickParty *this);
bool __cdecl QuickParty::CQuickParty::send_matching_update_quick_party(QuickParty::CQuickParty *this, char flag, unsigned __int16 a3, char type);
int `global constructor keyed to'QuickParty::CQuickParty::CQuickParty();
int __cdecl QuickParty::CQuickParty::get_quick_party_index(QuickParty::CQuickParty *this);
int __cdecl QuickParty::CQuickParty::get_first_matching_cnt(QuickParty::CQuickParty *this);
QuickParty::CQuickParty *__cdecl QuickParty::CQuickParty::set_first_matching_cnt(QuickParty::CQuickParty *this, char flag);
QuickParty::CQuickParty *__cdecl QuickParty::CQuickParty::set_dungeon_index(QuickParty::CQuickParty *this, __int16 a2);
int __cdecl QuickParty::CQuickParty::get_dungeon_index(QuickParty::CQuickParty *this);
QuickParty::CQuickParty *__cdecl QuickParty::CQuickParty::set_dungeon_diff(QuickParty::CQuickParty *this, char flag);
int __cdecl QuickParty::CQuickParty::get_dungeon_diff(QuickParty::CQuickParty *this);
QuickParty::CQuickParty *__cdecl QuickParty::CQuickParty::set_pool_index(QuickParty::CQuickParty *this, char flag);
int __cdecl QuickParty::CQuickParty::get_pool_index(QuickParty::CQuickParty *this);
int __cdecl QuickParty::CQuickParty::is_change_quick_party_count(QuickParty::CQuickParty *this);
QuickParty::CQuickParty *__cdecl QuickParty::CQuickParty::set_change_quick_party_count(QuickParty::CQuickParty *this, bool a2);
QuickParty::CQuickParty *__cdecl QuickParty::CQuickParty::set_quick_party_index(QuickParty::CQuickParty *this, int id);

#endif // CQUICKPARTY_H
