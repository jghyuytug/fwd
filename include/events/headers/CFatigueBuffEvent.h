#ifndef CFATIGUEBUFFEVENT_H
#define CFATIGUEBUFFEVENT_H

#include <common/defs.h>

// CFatigueBuffEvent declarations
void __cdecl CFatigueBuffEvent::CFatigueBuffEvent(CFatigueBuffEvent *this);
int __cdecl CFatigueBuffEvent::StartEvent(CFatigueBuffEvent *this);
int __cdecl CFatigueBuffEvent::StartEvent(int id);
int __cdecl CFatigueBuffEvent::EndEvent(CFatigueBuffEvent *this);

#endif // CFATIGUEBUFFEVENT_H
