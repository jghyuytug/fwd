#ifndef COBJECTBRINGUP_H
#define COBJECTBRINGUP_H

#include <common/defs.h>

// CObjectBringUp declarations
EventClassify::CObjectBringUp *__cdecl EventClassify::CObjectBringUp::set_event_data(EventClassify::CObjectBringUp *this, int id, InGameEventScript *a3);
int __cdecl EventClassify::CObjectBringUp::get_db_table_name(EventClassify::CObjectBringUp *this);
_BOOL4 __cdecl EventClassify::CObjectBringUp::verifyMaterialItemByRule(EventClassify::CObjectBringUp *this, CUser *user, unsigned int id);
_BOOL4 __cdecl EventClassify::CObjectBringUp::verifyGiveBringUpReward(EventClassify::CObjectBringUp *this, ObjectBringUpScript::ObjectBringUpRule *a2, int id);
int __cdecl EventClassify::CObjectBringUp::sendBringUpReward(EventClassify::CObjectBringUp *a1, CUserCharacInfo *charac_info, int id, _BYTE *a4);
EventClassify::CObjectBringUp *__cdecl EventClassify::CObjectBringUp::CObjectBringUp(EventClassify::CObjectBringUp *this);

#endif // COBJECTBRINGUP_H
