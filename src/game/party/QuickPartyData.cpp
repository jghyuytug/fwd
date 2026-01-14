#include <party/QuickPartyData.h>
#include <game/party/QuickPartyData.h>

bool __cdecl QuickParty::QuickPartyData::QuickPartyData(QuickParty::QuickPartyData *this)
{
  bool result; // al
  int i; // [esp+18h] [ebp-10h]
  int j; // [esp+1Ch] [ebp-Ch]

  std::vector<int>::vector(this);
  std::vector<int>::vector((_DWORD *)this + 3);
  QuickParty::BasicReward::BasicReward((QuickParty::QuickPartyData *)((char *)this + 24));
  std::map<QuickParty::RandomBuffType,QuickParty::LevelDataRandomReward>::map((_DWORD *)this + 14);
  std::vector<int>::clear((int *)this);
  std::vector<int>::clear((int *)this + 3);
  std::map<QuickParty::RandomBuffType,QuickParty::LevelDataRandomReward>::clear((int)this + 56);
  for ( i = 0; ; ++i )
  {
    result = i <= 1;
    if ( i > 1 )
      break;
    for ( j = 0; j <= 1; ++j )
      *((_DWORD *)this + 4 * i + j + 104) = -1;
  }
  return result;
}

