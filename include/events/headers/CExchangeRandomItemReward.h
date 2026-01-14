#ifndef CEXCHANGERANDOMITEMREWARD_H
#define CEXCHANGERANDOMITEMREWARD_H

#include <common/defs.h>

// CExchangeRandomItemReward declarations
EventClassify::CExchangeRandomItemReward *__cdecl EventClassify::CExchangeRandomItemReward::set_event_data(EventClassify::CExchangeRandomItemReward *this, int id, InGameEventScript *a3);
int __cdecl EventClassify::CExchangeRandomItemReward::verifyMaterialItemByRule(int id, CUserCharacInfo *charac_info, _DWORD *a3, _DWORD *a4);
int __cdecl EventClassify::CExchangeRandomItemReward::deleteUsedMaterialItem(int id, CUserCharacInfo *charac_info, _DWORD *a3, _DWORD *a4);
int __cdecl EventClassify::CExchangeRandomItemReward::sendRandomItemReward(int id, CUser *user, _DWORD *a3, int *a4, _BYTE *a5);
EventClassify::CExchangeRandomItemReward *__cdecl EventClassify::CExchangeRandomItemReward::CExchangeRandomItemReward(EventClassify::CExchangeRandomItemReward *this);

#endif // CEXCHANGERANDOMITEMREWARD_H
