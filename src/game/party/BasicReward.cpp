#include <party/BasicReward.h>
#include <game/party/BasicReward.h>

int __cdecl QuickParty::BasicReward::BasicReward(QuickParty::BasicReward *this)
{
  std::vector<QuickParty::LevelData>::vector(this);
  std::vector<QuickParty::LevelDataHell>::vector((_DWORD *)this + 3);
  *((_DWORD *)this + 6) = -1;
  *((_DWORD *)this + 7) = -1;
  std::vector<QuickParty::LevelData>::clear((int *)this);
  return std::vector<QuickParty::LevelDataHell>::clear((int *)this + 3);
}

