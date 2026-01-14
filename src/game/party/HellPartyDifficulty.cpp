#include <party/HellPartyDifficulty.h>
#include <game/party/HellPartyDifficulty.h>

int __cdecl QuickParty::HellPartyDifficulty::process(int id, CUser **a2, int value, _DWORD *a4)
{
  int result; // eax
  int id; // edx
  int id; // [esp+18h] [ebp-10h]
  int id; // [esp+1Ch] [ebp-Ch]

  v6 = std::vector<QuickParty::LevelDataHell>::size((_DWORD *)(*(_DWORD *)(a1 + 4) + 36));
  result = CParty::get_member_count(a2) - 2;
  v7 = result;
  if ( result >= 0 && result < v6 )
  {
    *a4 = *(_DWORD *)(std::vector<QuickParty::LevelDataHell>::operator[]((_DWORD *)(*(_DWORD *)(a1 + 4) + 36), result)
                    + 4);
    v5 = *(_DWORD *)std::vector<QuickParty::LevelDataHell>::operator[]((_DWORD *)(*(_DWORD *)(a1 + 4) + 36), v7);
    a4[1] = v5;
    return (int)a4;
  }
  return result;
}

QuickParty::IBasicReward *__cdecl QuickParty::HellPartyDifficulty::HellPartyDifficulty(
        QuickParty::IBasicReward *a1,

