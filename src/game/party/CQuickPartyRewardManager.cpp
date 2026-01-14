#include <party/CQuickPartyRewardManager.h>
#include <game/party/CQuickPartyRewardManager.h>

    dungeon_idx_list = QuickParty::CQuickPartyRewardManager::get_dungeon_idx_list(QuickPartyRewardManager);
    std::vector<int>::begin(&v14, dungeon_idx_list);

    v7 = QuickParty::CQuickPartyRewardManager::get_dungeon_idx_list(v6);
    std::vector<int>::end(&v13, (int)v7);

bool __cdecl QuickParty::CQuickPartyRewardManager::CQuickPartyRewardManager(QuickParty::CQuickPartyRewardManager *this)
{
  return QuickParty::QuickPartyData::QuickPartyData(this);
}

int __cdecl QuickParty::CQuickPartyRewardManager::init(QuickParty::CQuickPartyRewardManager *this)
{
  QuickParty::IBasicReward *v2; // ebx
  QuickParty::IBasicReward *v3; // ebx
  QuickParty::IRandomwReward *v4; // ebx
  QuickParty::IRandomwReward *v5; // ebx
  QuickParty::IRandomwReward *v6; // ebx
  QuickParty::IRandomwReward *v7; // ebx
  char flag; // [esp+8h] [ebp-20h]
  int id[6]; // [esp+10h] [ebp-18h] BYREF

  if ( !QuickParty::importQuickPartyScript((TCHAR *)"Etc/QuickParty.etc", (const char *)this) )
  {
    cMyTrace::cMyTrace((cMyTrace *)v9, "bool QuickParty::CQuickPartyRewardManager::init()", 146, 0);
    cMyTrace::operator()((int)v9, "QUICK_PARTY_LOG : QuickParty.etc FILE LOAD FAIL!!", v8);
    return 0;
  }
  else
  {
    v2 = (QuickParty::IBasicReward *)operator new(8u);
    QuickParty::RewardCardGold::RewardCardGold(v2, 11, this);
    *((_DWORD *)this + 123) = v2;
    v3 = (QuickParty::IBasicReward *)operator new(8u);
    QuickParty::HellPartyDifficulty::HellPartyDifficulty(v3, 12, this);
    *((_DWORD *)this + 124) = v3;
    v4 = (QuickParty::IRandomwReward *)operator new(0xCu);
    QuickParty::KillMonsterExp::KillMonsterExp(v4, 0, this);
    *((_DWORD *)this + 125) = v4;
    v5 = (QuickParty::IRandomwReward *)operator new(0xCu);
    QuickParty::EquipItemDropRate::EquipItemDropRate(v5, 3, this);
    *((_DWORD *)this + 128) = v5;
    v6 = (QuickParty::IRandomwReward *)operator new(0xCu);
    QuickParty::CointLimitCount::CointLimitCount(v6, 9, this);
    *((_DWORD *)this + 134) = v6;
    v7 = (QuickParty::IRandomwReward *)operator new(0xCu);
    QuickParty::ReturnAncientDungeonTicket::ReturnAncientDungeonTicket(v7, 10, this);
    *((_DWORD *)this + 135) = v7;
    QuickParty::CQuickPartyRewardManager::set_rand_buff_total_prob(this);
    return 1;
  }
}

int __cdecl QuickParty::CQuickPartyRewardManager::get_random_buff_prob(int id, int id2, int id3)
{
  if ( a2 <= 11 && a3 <= 6 )
    return *(_DWORD *)(a1 + 4 * (a3 + 7 * a2 + 20));
  else
    return 0;
}

char *__cdecl QuickParty::CQuickPartyRewardManager::get_dungeon_idx_list(QuickParty::CQuickPartyRewardManager *this)
{
  return (char *)this + 12;
}

int __cdecl QuickParty::CQuickPartyRewardManager::getRequireQuickPartyMinCount(
        QuickParty::CQuickPartyRewardManager *this,

int __cdecl QuickParty::CQuickPartyRewardManager::get_reward_type_object(int id, int id2, unsigned int id3)
{
  if ( a2 <= 1 && a3 <= 0xC )
    return *(_DWORD *)(a1 + 4 * (a3 + 13 * a2 + 112));
  else
    return 0;
}

bool __cdecl QuickParty::CQuickPartyRewardManager::set_rand_buff_total_prob(QuickParty::CQuickPartyRewardManager *this)
{
  bool result; // al
  int i; // [esp+4h] [ebp-10h]
  int value; // [esp+8h] [ebp-Ch]
  int j; // [esp+Ch] [ebp-8h]

  for ( i = 0; ; ++i )
  {
    result = i <= 6;
    if ( i > 6 )
      break;
    v3 = 0;
    for ( j = 0; j <= 10; ++j )
      v3 += *((_DWORD *)this + 7 * j + i + 20);
    *((_DWORD *)this + 7 * j + i + 20) = v3;
  }
  return result;
}

    QuickParty::CQuickPartyRewardManager::CQuickPartyRewardManager(v1);
    *((_DWORD *)this + 192) = v1;

                                                                              if ( (unsigned __int8)QuickParty::CQuickPartyRewardManager::init(QuickPartyRewardManager) != 1 )
                                                                              {
                                                                                v10 = 0;
                                                                              }

