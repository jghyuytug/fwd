#include <party/LevelData.h>
#include <game/party/LevelData.h>

  QuickParty::LevelData::LevelData((QuickParty::LevelData *)&v12);
LABEL_4:

QuickParty::LevelData *__cdecl QuickParty::LevelData::LevelData(QuickParty::LevelData *this)
{
  *(_DWORD *)this = 0;
  *((_DWORD *)this + 1) = -1;
  return this;
}

