#ifndef CHACKANALYZER_H
#define CHACKANALYZER_H

#include "defs.h"

/* Forward declarations for C compatibility */
typedef void CHackAnalyzer;
typedef void CUser;
typedef void CUserCharacInfo;
typedef unsigned long _BOOL4;
typedef unsigned short __int16;
typedef struct PacketBuf {
    void *data;
    int size;
} PacketBuf;

/* CHackAnalyzer function declarations (pure C interface) */
void* __cdecl CHackAnalyzer_setUserP(void *this, CUser *user);
int __cdecl CHackAnalyzer_getUserP(void *this);
int __cdecl CHackAnalyzer_beginCollectHackInfo(void *this, CUser *user);
int __cdecl CHackAnalyzer_analyzeHackReport(void *this, CUser *user, PacketBuf *a3);
char __cdecl CHackAnalyzer_analyzeMonsterDieHack(void *this);
char __cdecl CHackAnalyzer_analyzeSpeedHack(void *this, unsigned int id);
char __cdecl CHackAnalyzer_analyzeDifficultyHack(void *this, char flag);
char __cdecl CHackAnalyzer_analyzeMemoryHack(void *this, unsigned int id);
char __cdecl CHackAnalyzer_analyzeEquipmentUpgradeHack(CUserCharacInfo **this, const unsigned __int16 *a2);
int __cdecl CHackAnalyzer_addHackInfo(int id, char flag, __int16 a3, int value);
char __cdecl CHackAnalyzer_reportIncurrectReport(CUser **this);
int __cdecl CHackAnalyzer_reportHackInfo(void *this);
void* __cdecl CHackAnalyzer_resetHackInfo(void *this);
_BOOL4 __cdecl CHackAnalyzer_CheckExceptionCnt(int id, int value, int count);
_BOOL4 __cdecl CHackAnalyzer_CheckHackRateByPartyCount(int id, int value, int count);
_BOOL4 __cdecl CHackAnalyzer_CheckHackRateByDifficult(int id, int value, int count, unsigned int index);
int __cdecl CHackAnalyzer_AddCheckRateHackTypeList(void *this);
int __cdecl CHackAnalyzer_onUpdateHackCount(int id, CUser *user, unsigned int value, unsigned int count, unsigned int index, unsigned int type);
char __cdecl CHackAnalyzer_addServerHackCnt(void *a1, CUserCharacInfo *charac_info, unsigned int id, int value, unsigned int count, unsigned int index);
void __cdecl CHackAnalyzer_ReqDBSaveHackInfo(void *this, unsigned int id, __int16 a3, int value, int count);
void __cdecl CHackAnalyzer_ReqDBSaveHackUserPunish(void *this, unsigned int id);
char __cdecl CHackAnalyzer_ValidateFixMobDiePos(void *this, CUser *user, unsigned __int16 a3, unsigned __int16 a4);
int __cdecl CHackAnalyzer_setBaseHackType(int id, int value);
int __cdecl CHackAnalyzer_getBaseHackType(void *this);
int __cdecl CHackAnalyzer_checkCollectedHack(int id, int value);
int __cdecl CHackAnalyzer_getServerHackAccumulatedCnt(int id, int value);
int __cdecl CHackAnalyzer_setServerHackAccumulatedCnt(int id, int value, int count);
void* __cdecl CHackAnalyzer_setLastMonsterDeadTime(void *this, int id);
int __cdecl CHackAnalyzer_getServerHackCnt(int id, int value);
int __cdecl CHackAnalyzer_resetServerHackCnt(int id, int value);
int __cdecl CHackAnalyzer_resetServerHackAccumulatedCnt(int id, int value);
int __cdecl CHackAnalyzer_init(void *this);

#endif // CHACKANALYZER_H
