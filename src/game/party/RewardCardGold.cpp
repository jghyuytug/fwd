#include <party/RewardCardGold.h>
#include <game/party/RewardCardGold.h>

int __cdecl QuickParty::RewardCardGold::process(int id, CUser **a2, int value, int *a4)
{
  int result; // eax
  int id; // [esp+24h] [ebp-14h]
  float v6; // [esp+2Ch] [ebp-Ch]

  v5 = std::vector<QuickParty::LevelData>::size((_DWORD *)(*(_DWORD *)(a1 + 4) + 24));
  result = CParty::get_member_count(a2) - 2;
  if ( result >= 0 && result < v5 )
  {
    v6 = (long double)*(int *)(std::vector<QuickParty::LevelData>::operator[](
                                 (_DWORD *)(*(_DWORD *)(a1 + 4) + 24),
                                 result)
                             + 4)
       / 100.0
       + 1.0;
    *a4 = (int)((long double)*a4 * v6);
    return (int)a4;
  }
  return result;
}

int `global constructor keyed to'QuickParty::RewardCardGold::process()
{
  return __static_initialization_and_destruction_0(1, INVALID_ID_U16);
}

QuickParty::IBasicReward *__cdecl QuickParty::RewardCardGold::RewardCardGold(
        QuickParty::IBasicReward *a1,

