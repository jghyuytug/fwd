#ifndef CAUTOPUNISHRULEBLACKIPMGR_H
#define CAUTOPUNISHRULEBLACKIPMGR_H

#include "defs.h"

/* Forward declarations for C compatibility */
typedef void CAutoPunishRuleBlackIPMgr;
typedef unsigned long _DWORD;
typedef unsigned char __int8;

/* CAutoPunishRuleBlackIPMgr function declarations (pure C interface) */
int __cdecl CAutoPunishRuleBlackIPMgr_reset(void *this);
void* __cdecl CAutoPunishRuleBlackIPMgr_init(void *this);
int __cdecl CAutoPunishRuleBlackIPMgr_addBlackIP(void *this, unsigned int id, unsigned __int8 a3, unsigned __int8 a4);
int __cdecl CAutoPunishRuleBlackIPMgr_isBlackIP(void *this, char *src);
int __cdecl CAutoPunishRuleBlackIPMgr_getBlackIPCount(void *this);

#endif // CAUTOPUNISHRULEBLACKIPMGR_H
