#include <party/EquipItemDropRate.h>
#include <game/party/EquipItemDropRate.h>

char __cdecl QuickParty::EquipItemDropRate::process(int id, CUser **a2, int value, int count)
{
  int id; // eax
  int id; // eax
  int id; // ebx
  int id; // eax
  int id; // [esp+2Ch] [ebp-1Ch] BYREF
  int id; // [esp+30h] [ebp-18h] BYREF
  int id; // [esp+34h] [ebp-14h]
  int id; // [esp+38h] [ebp-10h]
  float v13; // [esp+3Ch] [ebp-Ch]

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
        v13 = 1.0;
        v6 = v12;
        v7 = std::_Rb_tree_const_iterator<std::pair<QuickParty::RandomBuffType const,QuickParty::LevelDataRandomReward>>::operator->(&v9);
        v4 = std::vector<QuickParty::LevelData>::operator[]((_DWORD *)(v7 + 12), v6);
        v13 = (long double)*(int *)(v4 + 4) / 100.0 + v13;
        *(float *)(a4 + 20) = v13;
        LOBYTE(v4) = a4;
      }
    }
  }
  return v4;
}

QuickParty::IRandomwReward *__cdecl QuickParty::EquipItemDropRate::EquipItemDropRate(
        QuickParty::IRandomwReward *a1,

