#ifndef CPURCHASECASHITEMBONUS_H
#define CPURCHASECASHITEMBONUS_H

#include <common/defs.h>

// CPurchaseCashItemBonus declarations
EventClassify::CPurchaseCashItemBonus *__cdecl EventClassify::CPurchaseCashItemBonus::set_event_data(EventClassify::CPurchaseCashItemBonus *this, int id, InGameEventScript *a3);
int __cdecl EventClassify::CPurchaseCashItemBonus::get_db_table_name(EventClassify::CPurchaseCashItemBonus *this);
char __cdecl EventClassify::CPurchaseCashItemBonus::updateTargetGoodsCount(int id, CUser *user, char flag, int value);
_BOOL4 __cdecl EventClassify::CPurchaseCashItemBonus::verifyGiveBonusReward(EventClassify::CPurchaseCashItemBonus *this, PurchaseCashItemBonusScript::PurchaseBonusRule *a2, int id);
EventClassify::CPurchaseCashItemBonus *__cdecl EventClassify::CPurchaseCashItemBonus::CPurchaseCashItemBonus(EventClassify::CPurchaseCashItemBonus *this);

#endif // CPURCHASECASHITEMBONUS_H
