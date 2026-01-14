#ifndef __INVEN_ITEM_H__
#define __INVEN_ITEM_H__

#include <common/defs.h>
#include <user/CUser.h>

// Forward declaration
struct Inven_Item;

// Function declarations (23 functions)

ReservedCapacity *__cdecl Inven_Item::reset(Inven_Item *this);
void __cdecl Inven_Item::Inven_Item(Inven_Item *this);
Inven_Item *__cdecl Inven_Item::set_add_info(Inven_Item *this, int id);
Inven_Item *__cdecl Inven_Item::ResetItemAttr(Inven_Item *this);
Inven_Item *__cdecl Inven_Item::SetUpgrade(Inven_Item *this, unsigned __int8 a2);
int __cdecl Inven_Item::GetUpgrade(Inven_Item *this);
int __cdecl Inven_Item::get_add_info(Inven_Item *this);
_BOOL4 __cdecl Inven_Item::isEmpty(Inven_Item *this);
Inven_Item *__cdecl Inven_Item::SetTradeLimitCount(Inven_Item *this, unsigned __int8 a2);
char __cdecl Inven_Item::GetTradeLimitCount(Inven_Item *this);
Inven_Item *__cdecl Inven_Item::setCopy(Inven_Item *this, const Inven_Item *item);
int __cdecl Inven_Item::GetItemAttr(Inven_Item *this);
Inven_Item *__cdecl Inven_Item::SetItemAttr(Inven_Item *this, unsigned __int8 a2);
bool __cdecl Inven_Item::isAvatarItemType(Inven_Item *this);
_BOOL4 __cdecl Inven_Item::isEquipableItemType(Inven_Item *this);
char __cdecl Inven_Item::GetReSealCount(Inven_Item *this);
Inven_Item *__cdecl Inven_Item::SetReSealCount(Inven_Item *this, unsigned __int8 a2);
_BOOL4 __cdecl Inven_Item::IsCreatureItemType(Inven_Item *this);
int __cdecl Inven_Item::GetInventoryType(Inven_Item *this);
int __cdecl Inven_Item::GetItemSpace(Inven_Item *this);
unsigned int __cdecl Inven_Item::getPackedCode1(Inven_Item *this);
int __cdecl Inven_Item::getKey(Inven_Item *this);
int __cdecl Inven_Item::IncUpgrade(Inven_Item *this);

#endif // __INVEN_ITEM_H__
