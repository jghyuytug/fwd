#ifndef ATTENDANCEEVENT_H
#define ATTENDANCEEVENT_H

#include <common/defs.h>

// AttendanceEvent declarations
void __cdecl AttendanceEvent::AttendanceEvent(AttendanceEvent *this);
CEventBase *__cdecl AttendanceEvent::StartEvent(AttendanceEvent *this);
void __cdecl AttendanceEvent::StartEvent(int id, int value);
int __cdecl AttendanceEvent::EndEvent(AttendanceEvent *this);
int __cdecl AttendanceEvent::getRewordItem(AttendanceEvent *this);
int `global constructor keyed to'AttendanceEvent::AttendanceEvent();

#endif // ATTENDANCEEVENT_H
