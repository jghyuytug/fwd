#ifndef CQUICKPARTYSYSTEMMANAGER_H
#define CQUICKPARTYSYSTEMMANAGER_H

#include <common/defs.h>

// CQuickPartySystemManager declarations
int __cdecl QuickParty::CQuickPartySystemManager::CQuickPartySystemManager(QuickParty::CQuickPartySystemManager *this);
void __cdecl QuickParty::CQuickPartySystemManager::init_quick_party_pool(QuickParty::CQuickPartySystemManager *this, const CDungeon *dungeon);
int __cdecl QuickParty::CQuickPartySystemManager::check_match_proper_party(QuickParty::CQuickPartySystemManager *this, const QuickParty::STQuickPartyPoolMap_Key *a2, CUser **a3);
int __cdecl QuickParty::CQuickPartySystemManager::cancel_quick_party(QuickParty::CQuickPartySystemManager *this, CParty *party, bool a3, CUser *user);
int __cdecl QuickParty::CQuickPartySystemManager::find_quick_party_in_pool(int id, int *a2);
int __cdecl QuickParty::CQuickPartySystemManager::remove_quick_party_in_pool(QuickParty::CQuickPartySystemManager *this, QuickParty::CQuickParty *a2, const QuickParty::STQuickPartyPoolMap_Key *a3);
int __cdecl QuickParty::CQuickPartySystemManager::check_quick_party_min_count(QuickParty::CQuickPartySystemManager *this, const QuickParty::STQuickPartyPoolMap_Key *a2, int id);
int __cdecl QuickParty::CQuickPartySystemManager::insert_timer_waiting_party_matching(QuickParty::CQuickPartySystemManager *this, QuickParty::CQuickParty *a2);
int __cdecl QuickParty::CQuickPartySystemManager::insert_timer_waiting_start_game(QuickParty::CQuickPartySystemManager *this, CParty *party);
_BOOL4 __cdecl QuickParty::CQuickPartySystemManager::regist_quick_party(QuickParty::CQuickPartySystemManager *this, const QuickParty::STQuickPartyPoolMap_Key *a2, QuickParty::STQuickPartyPoolMap_Data *a3, unsigned int id, CParty *party, int value);
QuickParty::CQuickParty *__cdecl QuickParty::CQuickPartySystemManager::pop_quick_party_in_pool(QuickParty::CQuickPartySystemManager *this, int *a2, QuickParty::STQuickPartyPoolMap_Data *a3, unsigned int id, int value, int count);
int __cdecl QuickParty::CQuickPartySystemManager::push_quick_party_in_pool(QuickParty::CQuickPartySystemManager *this, const QuickParty::STQuickPartyPoolMap_Key *a2, QuickParty::STQuickPartyPoolMap_Data *a3, CParty *party, char *a5, QuickParty::CQuickParty *a6, bool a7);
CParty *__cdecl QuickParty::CQuickPartySystemManager::make_quick_party(QuickParty::CQuickPartySystemManager *this, QuickParty::CQuickParty *a2);

#endif // CQUICKPARTYSYSTEMMANAGER_H
