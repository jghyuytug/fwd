#include <party/DimensionInoutCount.h>
#include <game/party/DimensionInoutCount.h>

char __cdecl QuickParty::DimensionInoutCount::process(int id, CUser **a2, int value, int *a4)
{
  int id; // eax
  int id; // eax
  int id; // ebx
  int id; // eax
  int id; // [esp+10h] [ebp-18h] BYREF
  int id; // [esp+14h] [ebp-14h] BYREF
  int id; // [esp+18h] [ebp-10h]
  int id; // [esp+1Ch] [ebp-Ch]

  std::map<QuickParty::RandomBuffType,QuickParty::LevelDataRandomReward>::find(
    &v9,
    *(_DWORD *)(a1 + 4) + 56,
    (_DWORD *)(a1 + 8));
  std::map<QuickParty::RandomBuffType,QuickParty::LevelDataRandomReward>::end(&v10, *(_DWORD *)(a1 + 4) + 56);
  LOBYTE(v4) = std::_Rb_tree_const_iterator<std::pair<QuickParty::RandomBuffType const,QuickParty::LevelDataRandomReward>>::operator==(
                 &v9,
                 &v10);
  if ( !(_BYTE)v4 )
  {
    v5 = std::_Rb_tree_const_iterator<std::pair<QuickParty::RandomBuffType const,QuickParty::LevelDataRandomReward>>::operator->(&v9);
    v11 = std::vector<QuickParty::LevelData>::size((_DWORD *)(v5 + 12));
    v4 = CParty::get_member_count(a2) - 2;
    v12 = v4;
    if ( v4 >= 0 )
    {
      LOBYTE(v4) = v12;
      if ( v12 < v11 )
      {
        v6 = v12;
        v7 = std::_Rb_tree_const_iterator<std::pair<QuickParty::RandomBuffType const,QuickParty::LevelDataRandomReward>>::operator->(&v9);
        *a4 = *(_DWORD *)(std::vector<QuickParty::LevelData>::operator[]((_DWORD *)(v7 + 12), v6) + 4);
        LOBYTE(v4) = CParty::increase_dimension_inout_count((CParty *)a2, *a4);
      }
    }
  }
  return v4;
}

