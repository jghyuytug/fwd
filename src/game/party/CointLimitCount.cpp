#include <party/CointLimitCount.h>
#include <game/party/CointLimitCount.h>

char __cdecl QuickParty::CointLimitCount::process(int id, CUser **a2, int value, int *a4)
{
  int id; // eax
  int id; // eax
  int id; // esi
  int id; // ebx
  int id; // eax
  int id; // [esp+10h] [ebp-18h] BYREF
  int id; // [esp+14h] [ebp-14h] BYREF
  int id; // [esp+18h] [ebp-10h]
  int id; // [esp+1Ch] [ebp-Ch]

  std::map<QuickParty::RandomBuffType,QuickParty::LevelDataRandomReward>::find(
    &v10,
    *(_DWORD *)(a1 + 4) + 56,
    (_DWORD *)(a1 + 8));
  std::map<QuickParty::RandomBuffType,QuickParty::LevelDataRandomReward>::end(&v11, *(_DWORD *)(a1 + 4) + 56);
  LOBYTE(v4) = std::_Rb_tree_const_iterator<std::pair<QuickParty::RandomBuffType const,QuickParty::LevelDataRandomReward>>::operator==(
                 &v10,
                 &v11);
  if ( !(_BYTE)v4 )
  {
    v5 = std::_Rb_tree_const_iterator<std::pair<QuickParty::RandomBuffType const,QuickParty::LevelDataRandomReward>>::operator->(&v10);
    v12 = std::vector<QuickParty::LevelData>::size((_DWORD *)(v5 + 12));
    v4 = CParty::get_member_count(a2) - 2;
    v13 = v4;
    if ( v4 >= 0 )
    {
      LOBYTE(v4) = v13;
      if ( v13 < v12 )
      {
        v6 = *a4;
        v7 = v13;
        v8 = std::_Rb_tree_const_iterator<std::pair<QuickParty::RandomBuffType const,QuickParty::LevelDataRandomReward>>::operator->(&v10);
        *a4 = v6 - *(_DWORD *)(std::vector<QuickParty::LevelData>::operator[]((_DWORD *)(v8 + 12), v7) + 4);
        LOBYTE(v4) = (unsigned __int8)CParty::increase_dungeon_coin_count((CParty *)a2, *a4);
      }
    }
  }
  return v4;
}

QuickParty::IRandomwReward *__cdecl QuickParty::CointLimitCount::CointLimitCount(
        QuickParty::IRandomwReward *a1,

