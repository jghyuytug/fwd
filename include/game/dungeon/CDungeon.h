#ifndef CDUNGEON_H
#define CDUNGEON_H

#include <common/defs.h>

// CDungeon declarations
int __cdecl CDungeon::get_standard_level(CDungeon *this);
int __cdecl CDungeon::get_index(CDungeon *this);
int __cdecl CDungeon::get_min_level(CDungeon *this);
char *__cdecl CDungeon::GetDungeonName(std::string *this);
int __cdecl CDungeon::getRandomTowerMaxStage(CDungeon *this);
int __cdecl CDungeon::getTowerMaxClearRewardItemNum(CDungeon *this);
char *__cdecl CDungeon::getObjectDropItems(CDungeon *this);
char *__cdecl CDungeon::getSpecialPassiveObjectItems(CDungeon *this);
char *__cdecl CDungeon::getRandomTowerMapIndexes(CDungeon *this);
int __cdecl CDungeon::hasTowerItemDrop(CDungeon *this);
int __cdecl CDungeon::get_limit_party_count(CDungeon *this);
int __cdecl CDungeon::getDungeonKind(CDungeon *this);
int __cdecl CDungeon::limitOfStackableItemInTower(CDungeon *this);
int __cdecl CDungeon::isNoFatigueDungeon(CDungeon *this);
int __cdecl CDungeon::GetType_DungeonInHeritance(CDungeon *this);
bool __cdecl CDungeon::isTowerOfDespairDungeon(CDungeon *this);
int __cdecl CDungeon::get_blood_dungeon_type(CDungeon *this);
int __cdecl CDungeon::get_dimension_possible(CDungeon *this);
int __cdecl CDungeon::get_dimension_min_partymem(CDungeon *this);
int __cdecl CDungeon::getQuestNpcDungeon(CDungeon *this);
int __cdecl CDungeon::getAICharacterAppearRate(CDungeon *this);
char *__cdecl CDungeon::getDropItems(CDungeon *this);
int __cdecl CDungeon::IsHaveDummyMap(CDungeon *this);
int __cdecl CDungeon::GetDummyAppearCount(CDungeon *this);
int __cdecl CDungeon::getTryGenEventMonsterCnt(CDungeon *this);
int __cdecl CDungeon::getTryGenEventMonsterProb(CDungeon *this);
int __cdecl CDungeon::getEventMonsterTotalProb(CDungeon *this);
char *__cdecl CDungeon::getEventMonsters(CDungeon *this);
int __cdecl CDungeon::getTryGenSecondEventMonsterCnt(CDungeon *this);
int __cdecl CDungeon::getTryGenSecondEventMonsterProb(CDungeon *this);
int __cdecl CDungeon::getSecondEventMonsterTotalProb(CDungeon *this);
char *__cdecl CDungeon::getSecondEventMonsters(CDungeon *this);
int __cdecl CDungeon::GetQuestMazeScriptVector(int id, int value);
CDungeon *__cdecl CDungeon::set_dungeon(CDungeon *this, STDungeonScript *a2);
CDungeon *__cdecl CDungeon::InsertMap(CDungeon *this, int id, char flag, CMap *a4);
int __cdecl CDungeon::insertStart(CDungeon *this, int id, CMap *a3);
int __cdecl CDungeon::GetRandMaze(CDungeon *this, int *a2);
int __cdecl CDungeon::GetMaze(CDungeon *this, unsigned int id);
int __cdecl CDungeon::ChooseRandomMap(const char **this, int id, int value, int count);
int __cdecl CDungeon::GetStartMap(CDungeon *this, char *a2);
int __cdecl CDungeon::get_clear_item(CDungeon *this);
int __cdecl CDungeon::get_clear_item(CDungeon *this, int id);
int __cdecl CDungeon::VerifyMaze(const char **this, const MazeScript *a2);
int __cdecl CDungeon::GetRandMazebyPartyCount(CDungeon *this, int id);
int __cdecl CDungeon::get_dimension_member_count(CDungeon *this, DimensionPartyCount *a2);
_BOOL4 __cdecl CDungeon::IsOpenScheduleDungeon(CDungeon *this);
_BOOL4 __cdecl CDungeon::isTournamentDungeon(CDungeon *this);
int __cdecl CDungeon::getTournamentRoundFatigue(CDungeon *this);
int __cdecl CDungeon::isRiskDungeon(CDungeon *this);
int __cdecl CDungeon::GetPartyMemberCoinLimit(CDungeon *this);
int __cdecl CDungeon::IsEnterEachMap(CDungeon *this);
int __cdecl CDungeon::IsOpenEvenEnemy(CDungeon *this);
_BOOL4 CDungeon::checkFreeRevivalCondition(CDungeon *this, ...);
int __cdecl CDungeon::getLimitCoinDiff(CDungeon *this, int id, int *a3);
void __cdecl CDungeon::CDungeon(CDungeon *this);
int CDungeon::_do_after_dungeon_start();
int __cdecl CDungeon::getDeathTowerMapIndex(CDungeon *this, unsigned int id);
int __cdecl CDungeon::getDeathTowerMaxStage(CDungeon *this);
int __cdecl CDungeon::getHeroesRequiredQuest(CDungeon *this);
int __cdecl CDungeon::get_limit_Inout_count(CDungeon *this);
long double __cdecl CDungeon::get_exp_weight(CDungeon *this);

#endif // CDUNGEON_H
