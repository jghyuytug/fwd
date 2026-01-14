#ifndef CQUICKPARTYREWARDMANAGER_H
#define CQUICKPARTYREWARDMANAGER_H

#include <common/defs.h>

// CQuickPartyRewardManager declarations
bool __cdecl QuickParty::CQuickPartyRewardManager::CQuickPartyRewardManager(QuickParty::CQuickPartyRewardManager *this);
int __cdecl QuickParty::CQuickPartyRewardManager::init(QuickParty::CQuickPartyRewardManager *this);
int __cdecl QuickParty::CQuickPartyRewardManager::get_random_buff_prob(int id, int value, int count);
char *__cdecl QuickParty::CQuickPartyRewardManager::get_dungeon_idx_list(QuickParty::CQuickPartyRewardManager *this);
int __cdecl QuickParty::CQuickPartyRewardManager::getRequireQuickPartyMinCount(QuickParty::CQuickPartyRewardManager *this, unsigned int id, unsigned int value);
int __cdecl QuickParty::CQuickPartyRewardManager::get_reward_type_object(int id, int value, unsigned int count);
bool __cdecl QuickParty::CQuickPartyRewardManager::set_rand_buff_total_prob(QuickParty::CQuickPartyRewardManager *this);

#endif // CQUICKPARTYREWARDMANAGER_H
