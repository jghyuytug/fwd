#ifndef CPCROOMPLAYTIME_H
#define CPCROOMPLAYTIME_H

#include <common/defs.h>

// CPcRoomPlayTime declarations
_BOOL4 __cdecl EventClassify::CPcRoomPlayTime::is_eventing(EventClassify::CPcRoomPlayTime *this);
int __cdecl EventClassify::CPcRoomPlayTime::get_db_table_name(EventClassify::CPcRoomPlayTime *this);
EventClassify::CPcRoomPlayTime *__cdecl EventClassify::CPcRoomPlayTime::set_event_data(EventClassify::CPcRoomPlayTime *this, int id, InGameEventScript *a3);
int __cdecl EventClassify::CPcRoomPlayTime::processEventPcRoomPlayTime(EventClassify::CPcRoomPlayTime *this, CUser *user);
int __cdecl EventClassify::CPcRoomPlayTime::verifyUserPcRoomPlayEvent(EventClassify::CPcRoomPlayTime *this, CUser *user);
EventClassify::CPcRoomPlayTime *__cdecl EventClassify::CPcRoomPlayTime::CPcRoomPlayTime(EventClassify::CPcRoomPlayTime *this);

#endif // CPCROOMPLAYTIME_H
