#ifndef CAUTOPUNISHRULEBLACKIPMGR_H
#define CAUTOPUNISHRULEBLACKIPMGR_H

#include "common/defs.h"

/* Forward declarations for C compatibility */
typedef void CAutoPunishRuleBlackIPMgr;

/* CAutoPunishRuleBlackIPMgr function declarations (pure C interface) */
int __cdecl CAutoPunishRuleBlackIPMgr_reset(void *this);
void* __cdecl CAutoPunishRuleBlackIPMgr_init(void *this);
int __cdecl CAutoPunishRuleBlackIPMgr_addBlackIP(void *this, unsigned int id, uint8_t a3, uint8_t a4);
int __cdecl CAutoPunishRuleBlackIPMgr_isBlackIP(void *this, char *src);
int __cdecl CAutoPunishRuleBlackIPMgr_getBlackIPCount(void *this);

#endif // CAUTOPUNISHRULEBLACKIPMGR_H
