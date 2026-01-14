#ifndef BINGOEVENT_H
#define BINGOEVENT_H

#include <common/defs.h>

// BingoEvent declarations
void __cdecl BingoEvent::BingoEvent(BingoEvent *this);
int __cdecl BingoEvent::StartEvent(BingoEvent *this);
int __cdecl BingoEvent::StartEvent(int id);
int BingoEvent::StartAction();
int __cdecl BingoEvent::EndEvent(BingoEvent *this);
int __cdecl BingoEvent::dailyresetData(BingoEvent *this);
char *__cdecl BingoEvent::getBingoSystem(BingoEvent *this);
int __cdecl BingoEvent::reqBingoMark(BingoEvent *a1, int id);
int __cdecl BingoEvent::reqBingoQuizMark(BingoEvent *this, CUser *user, int id);
void __cdecl BingoEvent::sendOneMatchLineReward(BingoEvent *this, CUser *user);
void __cdecl BingoEvent::sendMaxMatchLineReward(BingoEvent *this, CUser *user);
int `global constructor keyed to'BingoEvent::BingoEvent();

#endif // BINGOEVENT_H
