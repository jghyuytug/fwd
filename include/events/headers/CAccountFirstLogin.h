#ifndef CACCOUNTFIRSTLOGIN_H
#define CACCOUNTFIRSTLOGIN_H

#include <common/defs.h>

// CAccountFirstLogin declarations
int __cdecl EventClassify::CAccountFirstLogin::get_db_table_name(EventClassify::CAccountFirstLogin *this);
int __cdecl EventClassify::CAccountFirstLogin::get_mail_data(EventClassify::CAccountFirstLogin *this);
EventClassify::CAccountFirstLogin *__cdecl EventClassify::CAccountFirstLogin::set_event_data(EventClassify::CAccountFirstLogin *this, int id, InGameEventScript *a3);
EventClassify::CAccountFirstLogin *__cdecl EventClassify::CAccountFirstLogin::CAccountFirstLogin(EventClassify::CAccountFirstLogin *this);

#endif // CACCOUNTFIRSTLOGIN_H
