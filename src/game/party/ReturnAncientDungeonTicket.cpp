#include <party/ReturnAncientDungeonTicket.h>
#include <game/party/ReturnAncientDungeonTicket.h>

char __cdecl QuickParty::ReturnAncientDungeonTicket::process(int id, CParty *party)
{
  return CParty::increase_ancient_dungeon_ticket(a2);
}

QuickParty::IRandomwReward *__cdecl QuickParty::ReturnAncientDungeonTicket::ReturnAncientDungeonTicket(
        QuickParty::IRandomwReward *a1,

