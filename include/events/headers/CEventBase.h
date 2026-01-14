#ifndef CEVENTBASE_H
#define CEVENTBASE_H

#include <common/defs.h>

// CEventBase declarations
void CEventBase::StartAction();
void CEventBase::AppendInfo();
CEventBase *__stdcall CEventBase::GetAddInfo(CEventBase *this, int id);
CEventBase *__cdecl CEventBase::SetEventFlag(CEventBase *this, bool a2);
int __cdecl CEventBase::IsEventing(CEventBase *this);
void __cdecl CEventBase::CEventBase(CEventBase *this);
void CEventBase::changeCharacName();
void CEventBase::dailyresetData();
void CEventBase::deliveryStart();
void CEventBase::deliveryEnd();
void CEventBase::delivery();
int `global constructor keyed to'CEventBase::CEventBase();
void CEventBase::StartEvent();

#endif // CEVENTBASE_H
