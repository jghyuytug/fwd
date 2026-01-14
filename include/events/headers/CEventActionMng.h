#ifndef CEVENTACTIONMNG_H
#define CEVENTACTIONMNG_H

#include <common/defs.h>

// CEventActionMng declarations
int __cdecl EventClassify::CEventActionMng::process_coin_send(EventClassify::CEventActionMng *this, CUser **a2, Action_SendMail *a3, Inven_Item *item);
int __cdecl EventClassify::CEventActionMng::process_avatar_send(EventClassify::CEventActionMng *this, CUser *user, Inven_Item *item, const CItem *a4);
int __cdecl EventClassify::CEventActionMng::process_gold_mail_send(EventClassify::CEventActionMng *this, CUser *user, Action_SendMail *a3);
int __cdecl EventClassify::CEventActionMng::process_mail_send(EventClassify::CEventActionMng *this, CUser *user, Action_SendMail *a3, Inven_Item *item, const CItem *a5);

#endif // CEVENTACTIONMNG_H
