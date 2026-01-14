#ifndef CAVATARDISJOINTRANDOMREWARD_H
#define CAVATARDISJOINTRANDOMREWARD_H

#include <common/defs.h>

// CAvatarDisjointRandomReward declarations
EventClassify::CAvatarDisjointRandomReward *__cdecl EventClassify::CAvatarDisjointRandomReward::set_event_data(EventClassify::CAvatarDisjointRandomReward *this, int id, InGameEventScript *a3);
int __cdecl EventClassify::CAvatarDisjointRandomReward::checkAvatarDisjointError(EventClassify::CAvatarDisjointRandomReward *this, CUser *user, unsigned __int16 a3, Inven_Item *item, const CItem *a5);
int __cdecl EventClassify::CAvatarDisjointRandomReward::deleteAvatarItem(EventClassify::CAvatarDisjointRandomReward *this, CUser *user, unsigned __int16 a3, Inven_Item *item, const CItem *a5);
int __cdecl EventClassify::CAvatarDisjointRandomReward::SendDisjointAvatarStatistic(EventClassify::CAvatarDisjointRandomReward *this, const CItem *a2);
char __cdecl EventClassify::CAvatarDisjointRandomReward::SendCreateEmblemStatistic(int id, _DWORD *a2);
int __cdecl EventClassify::CAvatarDisjointRandomReward::sendRandomItemReward(int id, CUser *user, _DWORD *a3);
EventClassify::CAvatarDisjointRandomReward *__cdecl EventClassify::CAvatarDisjointRandomReward::CAvatarDisjointRandomReward(EventClassify::CAvatarDisjointRandomReward *this);

#endif // CAVATARDISJOINTRANDOMREWARD_H
