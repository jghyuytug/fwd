#ifndef CUSEDFATIGUEGIVEITEM_H
#define CUSEDFATIGUEGIVEITEM_H

#include <common/defs.h>

// CUsedFatigueGiveItem declarations
_BOOL4 __cdecl EventClassify::CUsedFatigueGiveItem::is_eventing(EventClassify::CUsedFatigueGiveItem *this);
EventClassify::CUsedFatigueGiveItem *__cdecl EventClassify::CUsedFatigueGiveItem::set_event_data(EventClassify::CUsedFatigueGiveItem *this, int id, InGameEventScript *a3);
int __cdecl EventClassify::CUsedFatigueGiveItem::get_db_table_name(EventClassify::CUsedFatigueGiveItem *this);
_BOOL4 __cdecl EventClassify::CUsedFatigueGiveItem::verifyApplyUser(EventClassify::CUsedFatigueGiveItem *this, CUser *user);
int __cdecl EventClassify::CUsedFatigueGiveItem::getIsCharacEvent(EventClassify::CUsedFatigueGiveItem *this, bool *a2);
int __cdecl EventClassify::CUsedFatigueGiveItem::get_user_type(EventClassify::CUsedFatigueGiveItem *this);
EventClassify::CUsedFatigueGiveItem *__cdecl EventClassify::CUsedFatigueGiveItem::CUsedFatigueGiveItem(EventClassify::CUsedFatigueGiveItem *this);

#endif // CUSEDFATIGUEGIVEITEM_H
