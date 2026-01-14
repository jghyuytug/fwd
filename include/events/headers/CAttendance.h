#ifndef CATTENDANCE_H
#define CATTENDANCE_H

#include <common/defs.h>

// CAttendance declarations
_BOOL4 __cdecl EventClassify::CAttendance::is_eventing(EventClassify::CAttendance *this);
int __cdecl EventClassify::CAttendance::get_db_table_name(EventClassify::CAttendance *this);
int __cdecl EventClassify::CAttendance::get_mail_data(EventClassify::CAttendance *this);
int __cdecl EventClassify::CAttendance::get_fatigue_condition(EventClassify::CAttendance *this);
int __cdecl EventClassify::CAttendance::get_final_day_condition(EventClassify::CAttendance *this);
int __cdecl EventClassify::CAttendance::send_init_data(EventClassify::CAttendance *this, CUser *user, PacketBuf **a3, __int16 a4, unsigned __int8 a5);
char __cdecl EventClassify::CAttendance::send_attend_reward_item(EventClassify::CAttendance *this, CUser *user, int id);
int __cdecl EventClassify::CAttendance::send_attend_final_rward_item(EventClassify::CAttendance *this, CUser *user, unsigned int id, int value);
EventClassify::CAttendance *__cdecl EventClassify::CAttendance::set_event_data(EventClassify::CAttendance *this, int id, InGameEventScript *a3);
EventClassify::CAttendance *__cdecl EventClassify::CAttendance::CAttendance(EventClassify::CAttendance *this);

#endif // CATTENDANCE_H
