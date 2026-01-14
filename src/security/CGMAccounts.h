#ifndef CGMACCOUNTS_H
#define CGMACCOUNTS_H

#include "defs.h"

/* Forward declarations for C compatibility */
typedef void CGMAccounts;
typedef unsigned long _BOOL4;
typedef unsigned long _DWORD;

/* CGMAccounts function declarations (pure C interface) */
int __cdecl CGMAccounts_loadGMAccounts(void);
_BOOL4 __cdecl CGMAccounts_isGM(void *this, unsigned int id);
int __cdecl CGMAccounts_appendGM(void);
int __cdecl CGMAccounts_removeGM(void);
void* __stdcall CGMAccounts_getGMInfo(void *this, int *a2, int id);
void* __cdecl CGMAccounts_LoadGmList(void *this, unsigned int id, int value);
void* __cdecl CGMAccounts_clearGmList(void *this);
int __cdecl CGMAccounts_AppendGM_Sys(void *this, unsigned int id, char flag);
void* __cdecl CGMAccounts_GetGMList(void *this);
void* __cdecl CGMAccounts_init(void *this);

#endif // CGMACCOUNTS_H
