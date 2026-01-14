#include <dungeon/CBattle_Field.h>
#include <game/dungeon/CBattle_Field.h>

_UNKNOWN CBattle_Field::GetEnableHellDungeon(void)::enable_hell_dungeon; // weak
char *GetInterMessageName(ENUM_INTERNALPACKET)::NAMES[183] =

                dungeon_diff = CBattle_Field::get_dungeon_diff((CBattle_Field *)(v15 + 713)),
                Party = !WongWork::CHackAnalyzer::CheckHackRateByDifficult((int)a1, a3, v16, dungeon_diff),

  dungeon_index = (char *)CBattle_Field::get_dungeon_index((CParty *)((char *)Party + 2852));
  v21 = (CDungeonList **)G_CDataManager();

  if ( v4 >= CBattle_Field::getMapHeight((CBattle_Field *)(Party + 2852)) )
    return -5;

  if ( v5 >= CBattle_Field::getMapWidth((CBattle_Field *)(Party + 2852)) )
    return -5;

    && (CBattle_Field::get_cur_map((CBattle_Field *)(Party + 2852)) == 61001
     || CBattle_Field::get_cur_map((CBattle_Field *)(Party + 2852)) == 61009

     || CBattle_Field::get_cur_map((CBattle_Field *)(Party + 2852)) == 61016
      ? (v4 = 0)

      CBattle_Field::SetDamagedPacketCnt((CBattle_Field *)(Party + 2852), *(_DWORD *)((char *)v13 + 458));
      CBattle_Field::SetDamageValue((CBattle_Field *)(Party + 2852), *(_DWORD *)((char *)v13 + 458), v20);

      CBattle_Field::SetDamagedPacketCnt((CBattle_Field *)(Party + 2852), *(_DWORD *)(v27 + 37));
      CBattle_Field::SetDamageValue((CBattle_Field *)(Party + 2852), *(_DWORD *)(v27 + 37), v37);

                *(_DWORD *)v39 = CBattle_Field::get_cur_map((CBattle_Field *)(Party + 2852));
                v11 = (CMapList **)G_CDataManager();

                  && (unsigned __int8)CBattle_Field::IsAssignNPCByPotion((CBattle_Field *)(Party + 2852)) != 1 )
                {
                  v13 = CUser::getHackAnalyzer(a2);
                  WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)v13, a2, 0x323u, 1, 0, 0);
                }

    else if ( CBattle_Field::getMaze((CParty *)((char *)Party + 2852)) )
    {
      CurrentMapIndex = CBattle_Field::GetCurrentMapIndex((CMap **)Party + 713);
      PlayingDungeonIndex = CUser::getPlayingDungeonIndex(a2);
      if ( (unsigned __int8)UserQuest::CheckDungeonEventStoryPause(
                              CurCharacQuestR,
                              PlayingDungeonIndex,
                              CurrentMapIndex) )
        v12 = 1;
    }

      if ( *(char *)(v5 + 2207) > 0 && *(_BYTE *)(CBattle_Field::GetMapInfo((CBattle_Field *)(Party + 2852)) + 1) != 1 )
        CBattle_Field::BloodSpawnTimerManager((CBattle_Field *)(Party + 2852), 0, 0);

  if ( CBattle_Field::getBloodState((CParty *)((char *)Party + 2852)) > 2 )
    return 0;

    else if ( (unsigned __int8)CBattle_Field::getUltimateDifficylty((CParty *)((char *)Party + 2852)) )
    {
      return 0;
    }

      CBattle_Field::setUltimateDifficulty((CParty *)((char *)Party + 2852), v9);
      PacketGuard::PacketGuard((PacketGuard *)v8);

int __cdecl CBattle_Field::GetMapInfoKeyValue(CBattle_Field *this, int id, int id2)
{
  return a3 + 100 * a2;
}

int __cdecl CBattle_Field::GetCurrentMapInfo(CBattle_Field *this)
{
  int id; // [esp+18h] [ebp-10h] BYREF
  int id[3]; // [esp+1Ch] [ebp-Ch] BYREF

  v3[0] = -1;
  v2 = -1;
  CBattle_Field::getCurPosXY(this, v3, &v2);
  return CBattle_Field::GetMapInfoFromPos((MazeScript **)this, v3[0], v2, 0);
}

int __cdecl CBattle_Field::GetMapInfoFromPos(MazeScript **this, int id, int id2, bool a4)
{
  int id; // ebx
  char flag; // al
  int id; // [esp+18h] [ebp-300h] BYREF
  bool v8; // [esp+1Ch] [ebp-2FCh]
  void *v9[59]; // [esp+28h] [ebp-2F0h] BYREF
  int id; // [esp+114h] [ebp-204h] BYREF
  int id; // [esp+118h] [ebp-200h] BYREF
  int MapInfoKeyValue; // [esp+11Ch] [ebp-1FCh] BYREF
  int id; // [esp+120h] [ebp-1F8h] BYREF
  _BYTE v14[8]; // [esp+124h] [ebp-1F4h] BYREF
  _DWORD value[60]; // [esp+12Ch] [ebp-1ECh] BYREF
  _DWORD value[60]; // [esp+21Ch] [ebp-FCh] BYREF
  int GridR; // [esp+30Ch] [ebp-Ch]

  v8 = a4;
  MapInfoKeyValue = CBattle_Field::GetMapInfoKeyValue((CBattle_Field *)this, a2, a3);
  std::map<int,MapInfo>::find(&v11, (int)(this + 30), &MapInfoKeyValue);
  std::map<int,MapInfo>::end(&v13, (int)(this + 30));
  if ( std::_Rb_tree_iterator<std::pair<int const,MapInfo>>::operator!=(&v11, &v13) )
  {
    if ( !v8
      && *(this + 29)
      && ((GridR = MazeScript::getGridR(*(this + 29), a2, a3)) == 0 || !std::vector<int>::size((_DWORD *)(GridR + 28))
        ? (v5 = 0)
        : (v5 = 1),
          v5 && *(_BYTE *)(CBattle_Field::getCurrentLayeredMapInfo((CBattle_Field *)this) + 232)) )
    {
      return CBattle_Field::getCurrentLayeredMapInfo((CBattle_Field *)this);
    }
    else
    {
      return std::_Rb_tree_iterator<std::pair<int const,MapInfo>>::operator->(&v11) + 4;
    }
  }
  else
  {
    MapInfo::MapInfo((MapInfo *)v9);
    v10 = CBattle_Field::GetMapInfoKeyValue((CBattle_Field *)this, a2, a3);
    std::make_pair<int &,MapInfo &>(v16, (int)&v10, (int)v9);
    std::pair<int const,MapInfo>::pair<int,MapInfo>(v15, (int)v16);
    std::map<int,MapInfo>::insert((int)v14, (int)(this + 30), (int)v15);
    std::pair<int const,MapInfo>::~pair((int)v15);
    std::pair<int,MapInfo>::~pair((int)v16);
    std::map<int,MapInfo>::find(&v7, (int)(this + 30), &v10);
    v11 = v7;
    v4 = std::_Rb_tree_iterator<std::pair<int const,MapInfo>>::operator->(&v11) + 4;
    MapInfo::~MapInfo(v9);
  }
  return v4;
}

int __cdecl CBattle_Field::IsAssignNPCByPotion(CBattle_Field *this)
{
  return *(unsigned __int8 *)(CBattle_Field::GetCurrentMapInfo(this) + 164);
}

int __cdecl CBattle_Field::getMapWidth(CBattle_Field *this)
{
  return *((_DWORD *)this + 99);
}

int __cdecl CBattle_Field::getMapHeight(CBattle_Field *this)
{
  return *((_DWORD *)this + 100);
}

int __cdecl CBattle_Field::getBloodState(CBattle_Field *this)
{
  return *((_DWORD *)this + 173);
}

void *CBattle_Field::GetEnableHellDungeon()
{
  return &CBattle_Field::GetEnableHellDungeon(void)::enable_hell_dungeon;
}

_BYTE *__cdecl CBattle_Field::SetEnableHellDungeon(CBattle_Field *this)
{
  _BYTE *result; // eax

  result = CBattle_Field::GetEnableHellDungeon();
  *result = (_BYTE)this;
  return result;
}

_BOOL4 __cdecl CBattle_Field::check_start_point(CBattle_Field *this)
{
  int id; // [esp+18h] [ebp-10h] BYREF
  int id[3]; // [esp+1Ch] [ebp-Ch] BYREF

  v3[0] = -1;
  v2 = -1;
  CBattle_Field::getCurPosXY(this, v3, &v2);
  return *((_DWORD *)this + 1) == v3[0] && *((_DWORD *)this + 2) == v2;
}

_BOOL4 __cdecl CBattle_Field::check_end_point(CBattle_Field *this)
{
  int id; // [esp+18h] [ebp-10h] BYREF
  int id[3]; // [esp+1Ch] [ebp-Ch] BYREF

  v3[0] = -1;
  v2 = -1;
  CBattle_Field::getCurPosXY(this, v3, &v2);
  return *((_DWORD *)this + 3) == v3[0] && *((_DWORD *)this + 4) == v2;
}

int __cdecl CBattle_Field::get_cur_map(CBattle_Field *this)
{
  return *(_DWORD *)(CBattle_Field::GetCurrentMapInfo(this) + 4);
}

CBattle_Field *__cdecl CBattle_Field::setUltimateDifficulty(CBattle_Field *this, char flag)
{
  *((_BYTE *)this + 683) = a2;
  return this;
}

int __cdecl CBattle_Field::getUltimateDifficylty(CBattle_Field *this)
{
  return *((unsigned __int8 *)this + 683);
}

int __cdecl CBattle_Field::GetMapInfo(CBattle_Field *this)
{
  return CBattle_Field::GetCurrentMapInfo(this);
}

int __cdecl CBattle_Field::getMaze(CBattle_Field *this)
{
  return *((_DWORD *)this + 29);
}

int __cdecl CBattle_Field::GetCurrentMapIndex(CMap **this)
{
  if ( *(this + 66) )
    return CMap::get_index(*(this + 66));
  else
    return 0;
}

_BOOL4 __cdecl CBattle_Field::IsClearDungeon(CBattle_Field *this)
{
  if ( !CClearCondition::Empty((CBattle_Field *)((char *)this + 240)) )
    return CClearCondition::IsClearCondition((CBattle_Field *)((char *)this + 240));
  else
    return CBattle_Field::check_end_point(this);
}

int __cdecl CBattle_Field::get_dungeon_index(CBattle_Field *this)
{
  return *((_DWORD *)this + 101);
}

  dungeon_index = CBattle_Field::get_dungeon_index((CBattle_Field *)(Party + 2852));
  if ( TowerOfDespairMgr::IsTowerOfDespairDungeon_ByIndex(GlobalData::s_pTowerOfDespairMgr, dungeon_index) )

  v4 = (GlobalScriptTutorial *)CBattle_Field::get_dungeon_index((CBattle_Field *)(Party + 2852));
  return !GlobalScriptTutorial::IsGlobalTutorialDungeon(v4);

  dungeon_index = (GlobalScriptTutorial *)CBattle_Field::get_dungeon_index((CBattle_Field *)(Party + 2852));
  return GlobalScriptTutorial::IsGlobalTutorialDungeon(dungeon_index);

        v11 = _S_KILL_MONSTER_OVER_DAMAGE_DUNGEON_RATIO[CBattle_Field::get_dungeon_diff((CBattle_Field *)(v9 + 2852))];
      }
    }
    else
    {
      if ( index != 100 )
        goto LABEL_18;
      v8 = CUser::GetParty(a1);
      v11 = _S_KILL_MONSTER_OVER_DAMAGE_DUNGEON_RATIO[CBattle_Field::get_dungeon_diff((CBattle_Field *)(v8 + 2852))];
    }
LABEL_21:
    *a2 = (__int64)((long double)(unsigned int)_S_KILL_MONSTER_OVER_DAMAGE_CONST[v10] * v11 * v13);
    *a3 = _S_KILL_MONSTER_OVER_DAMAGE_HACK_TYPE[v10];
    return (int)a3;
  }
  return result;
}
// 8C10C68: using guessed type int _S_KILL_MONSTER_OVER_DAMAGE_CONST[5];
// 8C10CA0: using guessed type double _S_KILL_MONSTER_OVER_DAMAGE_DUNGEON_RATIO[5];
// 8C10CC8: using guessed type int _S_KILL_MONSTER_OVER_DAMAGE_HACK_TYPE[14];
// 8C10D00: using guessed type int LEVEL_SECTION_TABLE[89];

//----- (08286984) --------------------------------------------------------
int CHackLog_OverDamage::GetDeathTowerLimitDamage()
{
  return 700000;
}

//----- (0828698E) --------------------------------------------------------
int CHackLog_OverDamage::GetDeathTowerOverDamageHackType()
{
  return 107;
}

//----- (08286998) --------------------------------------------------------
int CHackLog_OverDamage::GetDungeonLimitMaxDamage()
{
  return 700000;
}

//----- (082869A2) --------------------------------------------------------
int CHackLog_OverDamage::GetHackTypeOverLimitMaxDamage()
{
  return 107;
}

//----- (082869AC) --------------------------------------------------------
int CHackLog_OverDamage::GetBossTowerLimitDamage()
{
  return 70000;
}

//----- (082869B6) --------------------------------------------------------
int CHackLog_OverDamage::GetBossTowerOverDamageHackType()
{
  return 133;
}

//----- (082869C0) --------------------------------------------------------
int __cdecl __static_initialization_and_destruction_0(int id, int value)
{
  int result; // eax

  if ( a1 == 1 && a2 == INVALID_ID_U16 )
  {
    std::ios_base::Init::Init(&std::__ioinit);
    return __cxa_atexit((void (*)(void *))std::ios_base::Init::~Init, &std::__ioinit, &_dso_handle);
  }
  return result;
}

//----- (08286A00) --------------------------------------------------------
int `global constructor keyed to'CHackLog_OverDamage::GetDungeonLimitDamageAndHackType()
{
  return __static_initialization_and_destruction_0(1, INVALID_ID_U16);
}

//----- (08286A1C) --------------------------------------------------------
bool __cdecl comp_by_time(_DWORD *a1, _DWORD *a2)
{
  return *a1 > *a2;
}

//----- (08286A30) --------------------------------------------------------
char __cdecl Secu_DataControl::SetControlData(
        Secu_DataControl *this,
        CUser *user,
        unsigned __int8 a3,
        unsigned __int16 a4,
        unsigned __int16 a5)
{
  int id; // eax
  unsigned int id; // eax
  char flag; // bl
  unsigned int id; // ebx
  char *HackAnalyzer; // eax
  char *v10; // eax
  _DWORD *v11; // eax
  unsigned int acc_id; // eax
  char flag; // bl
  unsigned int id; // eax
  char flag; // bl
  _DWORD *v16; // eax
  unsigned int id; // eax
  char flag; // bl
  size_t Point; // [esp+2Ch] [ebp-8Ch]
  int id[4]; // [esp+44h] [ebp-74h] BYREF
  int id; // [esp+54h] [ebp-64h] BYREF
  int id; // [esp+58h] [ebp-60h] BYREF
  int id; // [esp+5Ch] [ebp-5Ch] BYREF
  int id[4]; // [esp+60h] [ebp-58h] BYREF
  int id[4]; // [esp+70h] [ebp-48h] BYREF
  int id; // [esp+80h] [ebp-38h] BYREF
  int id; // [esp+84h] [ebp-34h] BYREF
  int id; // [esp+88h] [ebp-30h] BYREF
  int id[4]; // [esp+8Ch] [ebp-2Ch] BYREF
  int id; // [esp+9Ch] [ebp-1Ch]
  int id; // [esp+A0h] [ebp-18h]
  int i; // [esp+A4h] [ebp-14h]
  int id; // [esp+A8h] [ebp-10h]
  int id; // [esp+ACh] [ebp-Ch]

  LOBYTE(v5) = a5;
  if ( a2 )
  {
    LOBYTE(v5) = a3;
    if ( a3 == 1 )
    {
      v5 = CUser::isGMUser(a2) ^ 1;
      if ( !(_BYTE)v5 )
      {

CBattle_Field *__cdecl CBattle_Field::SetParty(CBattle_Field *this, CParty *party)
{
  *(_DWORD *)this = a2;
  return this;
}

CBattle_Field *__cdecl CBattle_Field::SetCurSeatNo(CBattle_Field *this, int value)
{
  *((_DWORD *)this + 69) = a2;
  return this;
}

void __cdecl CBattle_Field::CBattle_Field(CBattle_Field *this)
{
  _DWORD *v1; // ebx
  int i; // esi
  _DWORD *v3; // ebx
  int j; // esi
  int k; // [esp+1Ch] [ebp-Ch]

  v1 = (_DWORD *)((char *)this + 28);
  for ( i = 3; i != -1; --i )
  {
    std::pair<int,int>::pair(v1);
    v1 += 2;
  }
  v3 = (_DWORD *)((char *)this + 60);
  for ( j = 3; j != -1; --j )
  {
    std::pair<int,int>::pair(v3);
    v3 += 2;
  }
  std::map<int,MapInfo>::map((_DWORD *)this + 30);
  Mutex::Mutex((pthread_mutex_t *)this + 6);
  Mutex::Mutex((pthread_mutex_t *)this + 7);
  std::vector<std::pair<int,int>>::vector((_DWORD *)this + 48);
  CRidable::CRidable((CBattle_Field *)((char *)this + 208));
  CClearCondition::CClearCondition((CBattle_Field *)((char *)this + 240));
  std::vector<int>::vector((_DWORD *)this + 70);
  std::map<short,std::vector<CBattle_Field::TournamentInfo>>::map((_DWORD *)this + 79);
  std::vector<CBattle_Field::stConditionEnterBossRoomStatus_t>::vector((_DWORD *)this + 92);
  std::vector<CBattle_Field::stEventMonsterStatus_t>::vector((_DWORD *)this + 95);
  std::vector<STAssignBloodSystem>::vector((_DWORD *)this + 144);
  std::vector<STAssignBloodPhaseTime>::vector((_DWORD *)this + 147);
  std::vector<short>::vector((_DWORD *)this + 150);
  std::vector<bool>::vector((CBattle_Field *)((char *)this + 612));
  CBattle_Field::CBloodClearRewardData::CBloodClearRewardData((CBattle_Field *)((char *)this + 696));
  CBattle_Field::CTowerOfDespairRewardData::CTowerOfDespairRewardData((CBattle_Field *)((char *)this + 3204));
  CTournamentDungeonReward::CTournamentDungeonReward((CBattle_Field *)((char *)this + 3224));
  *(_DWORD *)this = 0;
  *((_DWORD *)this + 27) = -1;
  *((_DWORD *)this + 26) = *((_DWORD *)this + 27);
  *((_DWORD *)this + 25) = *((_DWORD *)this + 26);
  *((_DWORD *)this + 24) = *((_DWORD *)this + 25);
  *((_DWORD *)this + 6) = *((_DWORD *)this + 24);
  *((_DWORD *)this + 5) = *((_DWORD *)this + 6);
  *((_DWORD *)this + 4) = *((_DWORD *)this + 5);
  *((_DWORD *)this + 3) = *((_DWORD *)this + 4);
  *((_DWORD *)this + 2) = *((_DWORD *)this + 3);
  *((_DWORD *)this + 1) = *((_DWORD *)this + 2);
  for ( k = 0; k <= 3; ++k )
  {
    *((_DWORD *)this + 2 * k + 7) = 0;
    *((_DWORD *)this + 2 * k + 8) = 0;
    *((_DWORD *)this + 2 * k + 15) = 0;
    *((_DWORD *)this + 2 * k + 16) = 0;
  }
  *((_BYTE *)this + 92) = 1;
  *((_DWORD *)this + 100) = 0;
  *((_DWORD *)this + 99) = *((_DWORD *)this + 100);
  *((_BYTE *)this + 683) = 0;
  *((_WORD *)this + 148) = 0;
  *((_DWORD *)this + 142) = 0;
  *((_DWORD *)this + 109) = 0;
  *((_BYTE *)this + 572) = 0;
  std::map<short,std::vector<CBattle_Field::TournamentInfo>>::clear((int)this + 316);
  CTournamentDungeonReward::ResetReward((CBattle_Field *)((char *)this + 3224));
  *((_DWORD *)this + 85) = 0;
  *((_DWORD *)this + 67) = -1;
  *((_BYTE *)this + 272) = 0;
  CBattle_Field::SetCurSeatNo(this, 255);
}

CBattle_Field *__cdecl CBattle_Field::SetTournamentCurRound(CBattle_Field *this, int value)
{
  *((_DWORD *)this + 85) = a2;
  return this;
}

int __cdecl CBattle_Field::GetTournamentCurRound(CBattle_Field *this)
{
  return *((_DWORD *)this + 85);
}

int __cdecl CBattle_Field::DestroyPassiveObject(CBattle_Field *this, int id)
{
  unsigned int i; // [esp+1Ch] [ebp-Ch]

  if ( *((_DWORD *)this + 66) )
  {
    if ( std::vector<int>::empty((_DWORD *)(*((_DWORD *)this + 66) + 168)) )
      return 0;
    for ( i = 0; std::vector<int>::size((_DWORD *)(*((_DWORD *)this + 66) + 168)) > i; ++i )
    {
      if ( *(_DWORD *)std::vector<int>::operator[]((_DWORD *)(*((_DWORD *)this + 66) + 168), i) == a2 )
        return 1;
    }
  }
  return 0;
}

  CBattle_Field::set_dungeon_diff(a2, a5);
  *((_DWORD *)a2 + 102) = a7;

  if ( (unsigned __int8)CBattle_Field::IsEnableHellDungeon() && a8 && (unsigned __int8)CWorldMap::IsInHellDungeon(a8) )
  {
    if ( a6 == 1 )
    {
      CParty::sub_hellparty_value(*(CParty **)a2);
      *((_BYTE *)a2 + 460) = CBattle_Field::GetHellPartyDifficulty((CUserCharacInfo ***)a2, 1);
      if ( !*((_BYTE *)a2 + 460) )
      {
        LogManager::logFormat(
          (LogManager *)1,
          (int)"battle_field.cpp",
          "bool CBattle_Field::SelectDungeon(int, const CDungeon*, int, ENUM_DUNGEON_TYPE, int, CWorldMap*, bool, int)",
          (const char *)0x1FC,
          "HELLPARTY_LOG : HELLPARTY DIFFICULTY ERROR!!\n",
          v13);
        return 0;
      }
      *((_BYTE *)a2 + 461) = CBattle_Field::GetHellPartyEventType(a2, *((_BYTE *)a2 + 460));
      *((_BYTE *)a2 + 440) = 1;
    }
    else if ( a9 )
    {
      if ( CParty::get_member_count(*(CUser ***)a2) != 1 && CBattle_Field::CheckHellPartyCondition(a2) )
      {
        CParty::sub_hellparty_value(*(CParty **)a2);
        *((_BYTE *)a2 + 440) = 1;
        *((_BYTE *)a2 + 462) = a9;
        *((_BYTE *)a2 + 460) = CBattle_Field::GetHellPartyDifficulty((CUserCharacInfo ***)a2, a6);
        if ( !*((_BYTE *)a2 + 460) )
        {
          LogManager::logFormat(
            (LogManager *)1,
            (int)"battle_field.cpp",
            "bool CBattle_Field::SelectDungeon(int, const CDungeon*, int, ENUM_DUNGEON_TYPE, int, CWorldMap*, bool, int)",
            (const char *)0x22B,
            "HELLPARTY_LOG : HELLPARTY DIFFICULTY ERROR!!\n",
            v13);
          return 0;
        }
        *((_BYTE *)a2 + 461) = CBattle_Field::GetHellPartyEventType(a2, *((_BYTE *)a2 + 460));
      }
    }
  }

  AppropriateMaze = (MazeScript *)CBattle_Field::GetAppropriateMaze(a1, (int)a2, a5);
  *((_DWORD *)a2 + 107) = *((_DWORD *)AppropriateMaze + 25);

  CBattle_Field::setQuestMazeInfo(a2);
  CClearCondition::SetDungeonClearConditionVector((CBattle_Field *)((char *)a2 + 240), (_DWORD *)AppropriateMaze + 22);

  CBattle_Field::getCurPosXY(a2, &v18, &v17);
  GridR = (char **)MazeScript::getGridR(*((MazeScript **)a2 + 29), v18, v17);

    MapInfoKeyValue = CBattle_Field::GetMapInfoKeyValue(a2, v18, v17);
    std::make_pair<int &,MapInfo &>(v21, (int)&MapInfoKeyValue, (int)v14);

    CBattle_Field::ConsistMap(a2);
    v10 = 1;

      "bool CBattle_Field::SelectDungeon(int, const CDungeon*, int, ENUM_DUNGEON_TYPE, int, CWorldMap*, bool, int)",
      (const char *)0x27C,

_BOOL4 __cdecl CBattle_Field::CheckHellPartyCondition(CBattle_Field *this)
{
  int id; // ebx
  int id; // ebx
  _BOOL4 result; // eax

  result = 0;
  if ( *(_DWORD *)this )
  {
    v1 = (int)*(float *)(*(_DWORD *)this + 2828);
    if ( v1 >= (int)get_rand_int(100) )
    {
      v2 = (int)*(float *)(*(_DWORD *)this + 2828);
      G_CDataManager();
      if ( v2 >= *((_DWORD *)CDataManager::get_hellparty_script_values() + 36) )
        return 1;
    }
  }
  return result;
}

int __cdecl CBattle_Field::GetHellPartyDifficulty(CUserCharacInfo ***a1, int id)
{
  char *hellparty_script_values; // eax
  char *v3; // eax
  int dest; // [esp+1Eh] [ebp-1Ah] BYREF
  __int16 v6; // [esp+22h] [ebp-16h]
  signed int id; // [esp+24h] [ebp-14h]
  signed int rand_int; // [esp+28h] [ebp-10h]
  int i; // [esp+2Ch] [ebp-Ch]

  dest = 0;
  v6 = 0;
  if ( a2 == 1 )
  {
    G_CDataManager();
    hellparty_script_values = (char *)CDataManager::get_hellparty_script_values();
    memcpy(&dest, hellparty_script_values + 132, 6u);
  }
  else
  {
    G_CDataManager();
    v3 = (char *)CDataManager::get_hellparty_script_values();
    memcpy(&dest, v3 + 126, 6u);
    if ( *a1 )
      CParty::increase_hell_party_ratio(*a1, (char *)&dest);
  }
  v7 = 0;
  rand_int = get_rand_int(100);
  for ( i = 1; i <= 5; ++i )
  {
    v7 += *((char *)&dest + i);
    if ( v7 >= rand_int )
      return i;
  }
  return 0;
}

int __cdecl CBattle_Field::GetHellPartyEventType(CBattle_Field *this, char flag)
{
  G_CDataManager();
  return *((char *)CDataManager::get_hellparty_script_values() + a2 + 138);
}

void __cdecl CBattle_Field::SetGridPath(CBattle_Field *this, MazeScript *a2, int value)
{
  unsigned int value; // ebx
  int *v4; // eax
  int value; // edx
  unsigned int value; // ebx
  int *v7; // eax
  int value; // edx
  unsigned int value; // ebx
  int *v10; // eax
  int value; // edx
  int *v12; // eax
  int value; // edx
  int value; // ebx
  int *v15; // eax
  int value; // edx
  int value; // ebx
  CMapList **v19; // eax
  bool v20; // bl
  int value; // eax
  unsigned int rand_int; // eax
  int *v23; // eax
  int value; // edx
  int RepeatEvent; // eax
  unsigned int TryGenEventMonsterProb; // ebx
  char *EventMonsters; // eax
  char *v29; // eax
  int EventMonsterTotalProb; // eax
  int value; // eax
  int value; // eax
  unsigned int value; // eax
  _DWORD *v34; // eax
  int value; // edx
  char flag; // bl
  char flag; // bl
  unsigned int TryGenEventMonsterCnt; // eax
  unsigned int TryGenSecondEventMonsterProb; // ebx
  char *SecondEventMonsters; // eax
  char *v41; // eax
  int SecondEventMonsterTotalProb; // eax
  int value; // eax
  int value; // eax
  unsigned int value; // eax
  _DWORD *v46; // eax
  int value; // edx
  unsigned int TryGenSecondEventMonsterCnt; // eax
  unsigned int value; // [esp+2Ch] [ebp-1BCh]
  _DWORD value[3]; // [esp+3Ch] [ebp-1ACh] BYREF
  _DWORD value[6]; // [esp+48h] [ebp-1A0h] BYREF
  int value; // [esp+60h] [ebp-188h] BYREF
  int value; // [esp+64h] [ebp-184h] BYREF
  int nn; // [esp+68h] [ebp-180h] BYREF
  int mm; // [esp+6Ch] [ebp-17Ch] BYREF
  void *v56[3]; // [esp+70h] [ebp-178h] BYREF
  _BYTE v57[2]; // [esp+7Eh] [ebp-16Ah] BYREF
  int value; // [esp+80h] [ebp-168h] BYREF
  char flag[4]; // [esp+84h] [ebp-164h]
  int value; // [esp+88h] [ebp-160h]
  int value; // [esp+8Ch] [ebp-15Ch]
  int value; // [esp+90h] [ebp-158h]
  int value; // [esp+94h] [ebp-154h]
  int value; // [esp+98h] [ebp-150h] BYREF
  int value; // [esp+9Ch] [ebp-14Ch] BYREF
  int jj; // [esp+A0h] [ebp-148h] BYREF
  int ii; // [esp+A4h] [ebp-144h] BYREF
  int value; // [esp+A8h] [ebp-140h] BYREF
  int value[2]; // [esp+ACh] [ebp-13Ch] BYREF
  void *v70[3]; // [esp+B4h] [ebp-134h] BYREF
  int value[2]; // [esp+C0h] [ebp-128h] BYREF
  void *v72; // [esp+C8h] [ebp-120h] BYREF
  _DWORD value[5]; // [esp+CCh] [ebp-11Ch] BYREF
  int value; // [esp+E0h] [ebp-108h] BYREF
  int value; // [esp+E4h] [ebp-104h] BYREF
  int m; // [esp+E8h] [ebp-100h] BYREF
  int k; // [esp+ECh] [ebp-FCh] BYREF
  int value; // [esp+F0h] [ebp-F8h] BYREF
  int value; // [esp+F4h] [ebp-F4h] BYREF
  void *v80[3]; // [esp+F8h] [ebp-F0h] BYREF
  int value[2]; // [esp+104h] [ebp-E4h] BYREF
  int value; // [esp+10Ch] [ebp-DCh] BYREF
  int value; // [esp+110h] [ebp-D8h]
  int value; // [esp+114h] [ebp-D4h] BYREF
  int value; // [esp+118h] [ebp-D0h]
  int value; // [esp+11Ch] [ebp-CCh] BYREF
  int value; // [esp+120h] [ebp-C8h] BYREF
  _DWORD value[2]; // [esp+124h] [ebp-C4h] BYREF
  _DWORD value[2]; // [esp+12Ch] [ebp-BCh] BYREF
  _DWORD value[2]; // [esp+134h] [ebp-B4h] BYREF
  _DWORD value[2]; // [esp+13Ch] [ebp-ACh] BYREF
  _DWORD value[2]; // [esp+144h] [ebp-A4h] BYREF
  int value; // [esp+14Ch] [ebp-9Ch] BYREF
  _DWORD *v94; // [esp+150h] [ebp-98h] BYREF
  int value; // [esp+154h] [ebp-94h] BYREF
  int value[4]; // [esp+158h] [ebp-90h] BYREF
  int value[4]; // [esp+168h] [ebp-80h] BYREF
  _DWORD value[2]; // [esp+178h] [ebp-70h] BYREF
  unsigned int value; // [esp+180h] [ebp-68h]
  signed int value; // [esp+184h] [ebp-64h]
  int value; // [esp+188h] [ebp-60h]
  int i; // [esp+18Ch] [ebp-5Ch]
  int j; // [esp+190h] [ebp-58h]
  int GridR; // [esp+194h] [ebp-54h]
  char flag[4]; // [esp+198h] [ebp-50h]
  int map; // [esp+19Ch] [ebp-4Ch]
  void *v107; // [esp+1A0h] [ebp-48h]
  int value; // [esp+1A4h] [ebp-44h]
  int value; // [esp+1A8h] [ebp-40h]
  int n; // [esp+1ACh] [ebp-3Ch]
  _DWORD *v111; // [esp+1B0h] [ebp-38h]
  unsigned int kk; // [esp+1B4h] [ebp-34h]
  unsigned int value; // [esp+1B8h] [ebp-30h]
  unsigned int value; // [esp+1BCh] [ebp-2Ch]
  _DWORD *v115; // [esp+1C0h] [ebp-28h]
  unsigned int i1; // [esp+1C4h] [ebp-24h]
  unsigned int value; // [esp+1C8h] [ebp-20h]
  unsigned int value; // [esp+1CCh] [ebp-1Ch]

  *((_DWORD *)this + 29) = a2;
  *((_DWORD *)this + 99) = **((_DWORD **)this + 29);
  *((_DWORD *)this + 100) = *(_DWORD *)(*((_DWORD *)this + 29) + 4);
  if ( *((_DWORD *)this + 99) + *((_DWORD *)this + 100) <= 3 )
  {
    *((_BYTE *)this + 440) = 0;
    *((_BYTE *)this + 459) = 1;
  }
  std::pair<int,int>::pair(&v86);
  std::pair<int,int>::pair(&v84);
  std::pair<int,int>::pair(&v82);
  if ( std::vector<std::pair<int,int>>::size((_DWORD *)(*((_DWORD *)this + 29) + 20)) )
  {
    v3 = rand();
    v49 = std::vector<std::pair<int,int>>::size((_DWORD *)(*((_DWORD *)this + 29) + 20));
    v4 = (int *)std::vector<std::pair<int,int>>::operator[]((_DWORD *)(*((_DWORD *)this + 29) + 20), v3 % v49);
    v5 = v4[1];
    v86 = *v4;
    v87 = v5;
    if ( std::vector<std::pair<int,int>>::size((_DWORD *)(*((_DWORD *)this + 29) + 32)) )
    {
      v6 = rand();
      v99 = v6 % std::vector<std::pair<int,int>>::size((_DWORD *)(*((_DWORD *)this + 29) + 32));
      v7 = (int *)std::vector<std::pair<int,int>>::operator[]((_DWORD *)(*((_DWORD *)this + 29) + 32), v99);
      v8 = v7[1];
      v84 = *v7;
      v85 = v8;
      if ( a3 > 0 )
      {
        v100 = v99;
        v101 = 0;
        *((_DWORD *)this + 51) = 0;
        std::vector<std::pair<int,int>>::clear((int *)this + 48);
        for ( i = 0; i < a3; ++i )
        {
          if ( (unsigned int)std::vector<std::pair<int,int>>::size((_DWORD *)(*((_DWORD *)this + 29) + 32)) > 1 )
          {
            v9 = rand();
            v101 = v9 % (std::vector<std::pair<int,int>>::size((_DWORD *)(*((_DWORD *)this + 29) + 32)) - 1);
            if ( v101 >= v100 )
              ++v101;
            v10 = (int *)std::vector<std::pair<int,int>>::operator[]((_DWORD *)(*((_DWORD *)this + 29) + 32), v101);
            v11 = v10[1];
            v81[0] = *v10;
            v81[1] = v11;
            std::vector<std::pair<int,int>>::push_back((int)this + 192, v81);
            v100 = v101;
          }
        }
      }
      if ( (int)(v99 - 1) >= 0 )
      {
        v12 = (int *)std::vector<std::pair<int,int>>::operator[]((_DWORD *)(*((_DWORD *)this + 29) + 32), v99 - 1);
        v13 = v12[1];
        v82 = *v12;
        v83 = v13;
      }
      v14 = v99 + 1;
      if ( v14 <= std::vector<std::pair<int,int>>::size((_DWORD *)(*((_DWORD *)this + 29) + 32)) - 1 )
      {
        v15 = (int *)std::vector<std::pair<int,int>>::operator[]((_DWORD *)(*((_DWORD *)this + 29) + 32), v99 + 1);
        v16 = v15[1];
        v82 = *v15;
        v83 = v16;
      }
      *((_DWORD *)this + 1) = v86;
      *((_DWORD *)this + 24) = *((_DWORD *)this + 1);
      *((_DWORD *)this + 2) = v87;
      *((_DWORD *)this + 25) = *((_DWORD *)this + 2);
      *((_DWORD *)this + 26) = *((_DWORD *)this + 24);
      *((_DWORD *)this + 27) = *((_DWORD *)this + 25);
      if ( *((_DWORD *)this + 98) && (unsigned __int8)CDungeon::IsEnterEachMap(*((CDungeon **)this + 98)) )
      {
        for ( j = 0; j <= 3; ++j )
        {
          std::make_pair<int &,int &>(v88, (int)&v86, (int)&v87);
          std::pair<int,int>::operator=((_DWORD *)this + 2 * j + 7, (int)v88);
          std::make_pair<int &,int &>(v89, (int)&v86, (int)&v87);
          std::pair<int,int>::operator=((_DWORD *)this + 2 * j + 15, (int)v89);
        }
      }
      *((_DWORD *)this + 3) = v84;
      *((_DWORD *)this + 4) = v85;
      *((_DWORD *)this + 5) = v82;
      *((_DWORD *)this + 6) = v83;
      *((_DWORD *)this + 112) = *(_DWORD *)(*((_DWORD *)this + 29) + 48);
      *((_DWORD *)this + 113) = *(_DWORD *)(*((_DWORD *)this + 29) + 52);
      *((_DWORD *)this + 111) = *(_DWORD *)(*((_DWORD *)this + 29) + 44);
      CRidable::SetRidableObject((CBattle_Field *)((char *)this + 208), (RidableScript *)(*((_DWORD *)this + 29) + 56));
      if ( *(_DWORD *)(*((_DWORD *)this + 98) + 2316) == 1 )
      {
        std::vector<std::pair<int,int>>::vector(v80);
        for ( k = 0; *(_DWORD *)(*((_DWORD *)this + 29) + 4) > k; ++k )
        {
          for ( m = 0; **((_DWORD **)this + 29) > m; ++m )
          {
            if ( (*((_DWORD *)this + 3) != m || *((_DWORD *)this + 4) != k)
              && (*((_DWORD *)this + 1) != m || *((_DWORD *)this + 2) != k) )
            {
              GridR = MazeScript::getGridR(*((MazeScript **)this + 29), m, k);
              if ( !std::vector<int>::empty((_DWORD *)(GridR + 4)) )
              {
                std::vector<int>::begin(&v75, (_DWORD *)(GridR + 4));
                std::vector<int>::end(&v74, GridR + 4);
                while ( __gnu_cxx::operator!=<int const*,std::vector<int>>((int)&v75, (int)&v74) )
                {
                  *(_DWORD *)v105 = *(_DWORD *)__gnu_cxx::__normal_iterator<int const*,std::vector<int>>::operator*((int)&v75);
                  v18 = *(_DWORD *)v105;
                  v19 = (CMapList **)G_CDataManager();
                  map = CDataManager::find_map(v19, v18);
                  if ( !map )
                    break;
                  CMap::get_condition_monster_list((CMap *)v90, map);
                  v20 = std::list<_mapMonster>::empty(v90);
                  std::list<_mapMonster>::~list(v90);
                  if ( v20 )
                    break;
                  __gnu_cxx::__normal_iterator<int const*,std::vector<int>>::operator++(&v75);
                }
                if ( __gnu_cxx::operator==<int const*,std::vector<int>>((int)&v75, (int)&v74) )
                {
                  std::make_pair<int &,int &>(v91, (int)&m, (int)&k);
                  std::vector<std::pair<int,int>>::push_back((int)v80, (int)v91);
                }
              }
            }
          }
        }
        std::vector<int>::begin(&v79, (_DWORD *)(*((_DWORD *)this + 98) + 2320));
        std::vector<int>::end(&v78, *((_DWORD *)this + 98) + 2320);
        while ( __gnu_cxx::operator!=<int const*,std::vector<int>>((int)&v79, (int)&v78) )
        {
          v107 = *(void **)__gnu_cxx::__normal_iterator<int const*,std::vector<int>>::operator*((int)&v79);
          __gnu_cxx::__normal_iterator<int const*,std::vector<int>>::operator++(&v79);
          v108 = *(_DWORD *)__gnu_cxx::__normal_iterator<int const*,std::vector<int>>::operator*((int)&v79);
          __gnu_cxx::__normal_iterator<int const*,std::vector<int>>::operator++(&v79);
          v109 = *(_DWORD *)__gnu_cxx::__normal_iterator<int const*,std::vector<int>>::operator*((int)&v79);
          __gnu_cxx::__normal_iterator<int const*,std::vector<int>>::operator++(&v79);
          CBattle_Field::stConditionEnterBossRoomStatus_t::stConditionEnterBossRoomStatus_t((CBattle_Field::stConditionEnterBossRoomStatus_t *)&v72);
          v72 = v107;
          v73[3] = 0;
          v73[4] = v109;
          for ( n = 0; n < v108; ++n )
          {
            v21 = std::vector<std::pair<int,int>>::size(v80);
            rand_int = get_rand_int(v21);
            v23 = (int *)std::vector<std::pair<int,int>>::at(v80, rand_int);
            v24 = v23[1];
            v71[0] = *v23;
            v71[1] = v24;
            std::vector<std::pair<int,int>>::push_back((int)v73, v71);
          }
          std::vector<CBattle_Field::stConditionEnterBossRoomStatus_t>::push_back(
            (int)this + 368,
            (CBattle_Field::stConditionEnterBossRoomStatus_t *)&v72);
          CBattle_Field::stConditionEnterBossRoomStatus_t::~stConditionEnterBossRoomStatus_t(&v72);
        }
        std::vector<std::pair<int,int>>::~vector(v80);
      }
      if ( CDungeon::getTryGenEventMonsterCnt(*((CDungeon **)this + 98)) )
      {
        RepeatEvent = CEventManager::GetRepeatEvent(GlobalData::s_event_manager, 36);
        if ( (*(unsigned __int8 (__cdecl **)(int, _DWORD))(*(_DWORD *)RepeatEvent + 52))(RepeatEvent, 0) )
        {
          std::vector<std::pair<int,int>>::vector(v70);
          for ( ii = 0; *(_DWORD *)(*((_DWORD *)this + 29) + 4) > ii; ++ii )
          {
            for ( jj = 0; **((_DWORD **)this + 29) > jj; ++jj )
            {
              v111 = (_DWORD *)MazeScript::getGridR(*((MazeScript **)this + 29), jj, ii);
              if ( *v111 )
              {
                std::make_pair<int &,int &>(v92, (int)&jj, (int)&ii);
                std::vector<std::pair<int,int>>::push_back((int)v70, (int)v92);
              }
            }
          }
          std::list<MONSTER_POS>::list(v69);
          std::_List_iterator<MONSTER_POS>::_List_iterator(&v68);
          for ( kk = 0; ; ++kk )
          {
            TryGenEventMonsterCnt = CDungeon::getTryGenEventMonsterCnt(*((CDungeon **)this + 98));
            if ( TryGenEventMonsterCnt <= kk )
              break;
            TryGenEventMonsterProb = CDungeon::getTryGenEventMonsterProb(*((CDungeon **)this + 98));
            if ( TryGenEventMonsterProb >= get_rand_int(MAX_LIMIT_10000) )
            {
              EventMonsters = CDungeon::getEventMonsters(*((CDungeon **)this + 98));
              std::vector<stEventMonster_t>::begin(&v65, EventMonsters);
              v29 = CDungeon::getEventMonsters(*((CDungeon **)this + 98));
              std::vector<stEventMonster_t>::end(&v64, (int)v29);
              v113 = 0;
              EventMonsterTotalProb = CDungeon::getEventMonsterTotalProb(*((CDungeon **)this + 98));
              v114 = get_rand_int(EventMonsterTotalProb);
              while ( __gnu_cxx::operator!=<stEventMonster_t const*,std::vector<stEventMonster_t>>((int)&v65, (int)&v64) )
              {
                v31 = __gnu_cxx::__normal_iterator<stEventMonster_t const*,std::vector<stEventMonster_t>>::operator->((int)&v65);
                v113 += *(_DWORD *)(v31 + 8);
                if ( v114 < v113 )
                {
                  CBattle_Field::stEventMonsterStatus_t::stEventMonsterStatus_t((CBattle_Field::stEventMonsterStatus_t *)&v58);
                  v58 = *(_DWORD *)__gnu_cxx::__normal_iterator<stEventMonster_t const*,std::vector<stEventMonster_t>>::operator->((int)&v65);
                  v61 = *(_DWORD *)(__gnu_cxx::__normal_iterator<stEventMonster_t const*,std::vector<stEventMonster_t>>::operator->((int)&v65)
                                  + 4);
                  v32 = std::vector<std::pair<int,int>>::size(v70);
                  v33 = get_rand_int(v32);
                  v34 = (_DWORD *)std::vector<std::pair<int,int>>::at(v70, v33);
                  v35 = v34[1];
                  *(_DWORD *)v59 = *v34;
                  v60 = v35;
                  v62 = *(_DWORD *)(__gnu_cxx::__normal_iterator<stEventMonster_t const*,std::vector<stEventMonster_t>>::operator->((int)&v65)
                                  + 12);
                  v63 = *(_DWORD *)(__gnu_cxx::__normal_iterator<stEventMonster_t const*,std::vector<stEventMonster_t>>::operator->((int)&v65)
                                  + 16);
                  v57[0] = v59[0];
                  v57[1] = v60;
                  std::list<MONSTER_POS>::end(&v93, (int)v69);
                  std::list<MONSTER_POS>::begin(&v94, v69);
                  std::find<std::_List_iterator<MONSTER_POS>,MONSTER_POS>(v50, v94, v93, v57);
                  v68 = v50[0];
                  std::list<MONSTER_POS>::end(&v95, (int)v69);
                  if ( std::_List_iterator<MONSTER_POS>::operator==(&v68, &v95) )
                  {
                    v36 = v59[0];
                    cMyTrace::cMyTrace((cMyTrace *)v96, "void CBattle_Field::SetGridPath(MazeScript*, int)", 995, 0);
                    cMyTrace::operator()((int)v96, "Event monster generated (%d,%d ->%d(%d)", v36);
                    std::list<MONSTER_POS>::push_back((int)v69, (int)v57);
                    std::vector<CBattle_Field::stEventMonsterStatus_t>::push_back((int)this + 380, &v58);
                  }
                  else
                  {
                    v37 = v59[0];
                    cMyTrace::cMyTrace((cMyTrace *)v97, "void CBattle_Field::SetGridPath(MazeScript*, int)", STATIC_POOL_SIZE_1000, 0);
                    cMyTrace::operator()((int)v97, "Event monster SameMap (not generated) (%d,%d ->%d(%d)", v37);
                  }
                  break;
                }
                __gnu_cxx::__normal_iterator<stEventMonster_t const*,std::vector<stEventMonster_t>>::operator++(&v65);
              }
            }
          }
          std::list<MONSTER_POS>::clear(v69);
          std::list<MONSTER_POS>::~list(v69);
          std::vector<std::pair<int,int>>::~vector(v70);
        }
      }
      if ( CDungeon::getTryGenSecondEventMonsterCnt(*((CDungeon **)this + 98)) )
      {
        std::vector<std::pair<int,int>>::vector(v56);
        for ( mm = 0; *(_DWORD *)(*((_DWORD *)this + 29) + 4) > mm; ++mm )
        {
          for ( nn = 0; **((_DWORD **)this + 29) > nn; ++nn )
          {
            v115 = (_DWORD *)MazeScript::getGridR(*((MazeScript **)this + 29), nn, mm);
            if ( *v115 )
            {
              std::make_pair<int &,int &>(v98, (int)&nn, (int)&mm);
              std::vector<std::pair<int,int>>::push_back((int)v56, (int)v98);
            }
          }
        }
        for ( i1 = 0; ; ++i1 )
        {
          TryGenSecondEventMonsterCnt = CDungeon::getTryGenSecondEventMonsterCnt(*((CDungeon **)this + 98));
          if ( TryGenSecondEventMonsterCnt <= i1 )
            break;
          TryGenSecondEventMonsterProb = CDungeon::getTryGenSecondEventMonsterProb(*((CDungeon **)this + 98));
          if ( TryGenSecondEventMonsterProb >= get_rand_int(MAX_LIMIT_10000) )
          {
            SecondEventMonsters = CDungeon::getSecondEventMonsters(*((CDungeon **)this + 98));
            std::vector<stSecondEventMonster_t>::begin(&v53, SecondEventMonsters);
            v41 = CDungeon::getSecondEventMonsters(*((CDungeon **)this + 98));
            std::vector<stSecondEventMonster_t>::end(&v52, (int)v41);
            v117 = 0;
            SecondEventMonsterTotalProb = CDungeon::getSecondEventMonsterTotalProb(*((CDungeon **)this + 98));
            v118 = get_rand_int(SecondEventMonsterTotalProb);
            while ( __gnu_cxx::operator!=<stSecondEventMonster_t const*,std::vector<stSecondEventMonster_t>>(
                      (int)&v53,
                      (int)&v52) )
            {
              v43 = __gnu_cxx::__normal_iterator<stSecondEventMonster_t const*,std::vector<stSecondEventMonster_t>>::operator->((int)&v53);
              v117 += *(_DWORD *)(v43 + 8);
              if ( v118 < v117 )
              {
                CBattle_Field::stEventMonsterStatus_t::stEventMonsterStatus_t((CBattle_Field::stEventMonsterStatus_t *)v51);
                v51[0] = *(_DWORD *)__gnu_cxx::__normal_iterator<stSecondEventMonster_t const*,std::vector<stSecondEventMonster_t>>::operator->((int)&v53);
                v51[3] = *(_DWORD *)(__gnu_cxx::__normal_iterator<stSecondEventMonster_t const*,std::vector<stSecondEventMonster_t>>::operator->((int)&v53)
                                   + 4);
                v44 = std::vector<std::pair<int,int>>::size(v56);
                v45 = get_rand_int(v44);
                v46 = (_DWORD *)std::vector<std::pair<int,int>>::at(v56, v45);
                v47 = v46[1];
                v51[1] = *v46;
                v51[2] = v47;
                v51[4] = *(_DWORD *)(__gnu_cxx::__normal_iterator<stSecondEventMonster_t const*,std::vector<stSecondEventMonster_t>>::operator->((int)&v53)
                                   + 12);
                v51[5] = *(_DWORD *)(__gnu_cxx::__normal_iterator<stSecondEventMonster_t const*,std::vector<stSecondEventMonster_t>>::operator->((int)&v53)
                                   + 16);
                std::vector<CBattle_Field::stEventMonsterStatus_t>::push_back((int)this + 380, v51);
                break;
              }
              __gnu_cxx::__normal_iterator<stSecondEventMonster_t const*,std::vector<stSecondEventMonster_t>>::operator++(&v53);
            }
          }
        }
        std::vector<std::pair<int,int>>::~vector(v56);
      }
    }
  }
}

  if ( !*(_BYTE *)(CBattle_Field::GetMapInfoFromPos((MazeScript **)this, v7, v8, 0) + 1) )
    return 0;

int *__cdecl CBattle_Field::getCurPosXY(CBattle_Field *this, int *a2, int *a3)
{
  int id; // edx

  if ( *((_DWORD *)this + 98)
    && (unsigned __int8)CDungeon::IsEnterEachMap(*((CDungeon **)this + 98))
    && *((_DWORD *)this + 69) < 4u )
  {
    *a2 = *((_DWORD *)this + 2 * *((_DWORD *)this + 69) + 7);
    v4 = *((_DWORD *)this + 2 * *((_DWORD *)this + 69) + 8);
  }
  else
  {
    *a2 = *((_DWORD *)this + 24);
    v4 = *((_DWORD *)this + 25);
  }
  *a3 = v4;
  return a3;
}

int *__cdecl CBattle_Field::getLastPosXY(CBattle_Field *this, int *a2, int *a3)
{
  if ( (unsigned __int8)CDungeon::IsEnterEachMap(*((CDungeon **)this + 98)) && *((_DWORD *)this + 69) < 4u )
  {
    *a2 = *((_DWORD *)this + 2 * *((_DWORD *)this + 69) + 15);
    *a3 = *((_DWORD *)this + 2 * *((_DWORD *)this + 69) + 16);
    return a3;
  }
  else
  {
    *a2 = *((_DWORD *)this + 26);
    *a2 = *((_DWORD *)this + 27);
    return a2;
  }
}

int __cdecl CBattle_Field::getMap(CBattle_Field *this, int id)
{
  CMapList **v2; // eax
  const char *v3; // eax
  const char *v4; // edi
  int id; // esi
  int id; // ebx
  int id; // eax
  int id; // ebx
  CMapList **v11; // eax
  const char *v12; // eax
  const char *v13; // edi
  int id; // esi
  int id; // ebx
  int CurrentMapInfo; // eax
  int map; // [esp+3Ch] [ebp-1Ch]

  v2 = (CMapList **)G_CDataManager();
  map = CDataManager::find_map(v2, a2);
  if ( map )
  {
    if ( CBattle_Field::get_dungeon_diff(this) != 4 || !*(_DWORD *)(map + 180) )
      return map;
    *(_DWORD *)(CBattle_Field::GetCurrentMapInfo(this) + 4) = *(_DWORD *)(map + 180);
    v10 = *(_DWORD *)(map + 180);
    v11 = (CMapList **)G_CDataManager();
    map = CDataManager::find_map(v11, v10);
    if ( map )
    {
      return map;
    }
    else
    {
      if ( *(_BYTE *)(CBattle_Field::GetCurrentMapInfo(this) + 232) )
        v12 = "layeredMap";
      else
        v12 = "generalMap";
      v13 = v12;
      v14 = *((_DWORD *)this + 25);
      v15 = *((_DWORD *)this + 24);
      CurrentMapInfo = CBattle_Field::GetCurrentMapInfo(this);
      LogManager::logFormat(
        (LogManager *)1,
        (int)"battle_field.cpp",
        "const CMap* CBattle_Field::getMap(int)",
        (const char *)0x4C4,
        "Find Map[%d] Error(In Map Maker)(%d/%d) [Map Kind : %s]",
        *(const char **)(CurrentMapInfo + 4),
        v15,
        v14,
        v13);
      return 0;
    }
  }
  else
  {
    if ( *(_BYTE *)(CBattle_Field::GetCurrentMapInfo(this) + 232) )
      v3 = "layeredMap";
    else
      v3 = "generalMap";
    v4 = v3;
    v5 = *((_DWORD *)this + 25);
    v6 = *((_DWORD *)this + 24);
    v7 = CBattle_Field::GetCurrentMapInfo(this);
    LogManager::logFormat(
      (LogManager *)1,
      (int)"battle_field.cpp",
      "const CMap* CBattle_Field::getMap(int)",
      (const char *)0x4A7,
      "Find Map[%d] Error(In Map Maker)(%d/%d) [Map Kind : %s]",
      *(const char **)(v7 + 4),
      v6,
      v5,
      v4);
    return 0;
  }
}

CDungeonClearTracer *__cdecl CBattle_Field::setDungeonClearTrace(CBattle_Field *this, int value, int value2, int value3, int value4)
{
  CDungeonClearTracer::SetMapId((CBattle_Field *)((char *)this + 344), a2);
  CDungeonClearTracer::SetMaxBossCount((CBattle_Field *)((char *)this + 344), a3);
  CDungeonClearTracer::SetKilledBossCount((CBattle_Field *)((char *)this + 344), a4);
  return CDungeonClearTracer::SetKilledMonsterCount((CBattle_Field *)((char *)this + 344), a5);
}

int __cdecl CBattle_Field::getAddChampionMobCnt(CBattle_Field *this)
{
  int id; // ebx
  int id; // [esp+38h] [ebp-10h]
  int id; // [esp+3Ch] [ebp-Ch]

  if ( !*((_DWORD *)this + 66) )
    return -1;
  v3 = *(_DWORD *)(*((_DWORD *)this + 66) + 128);
  if ( !*((_DWORD *)this + 99) && !*((_DWORD *)this + 100) )
    LogManager::logFormat(
      (LogManager *)1,
      (int)"battle_field.cpp",
      "int CBattle_Field::getAddChampionMobCnt()",
      (const char *)0x4ED,
      "[FPE] [CBattle_Field::ConsistMap] Dungeon[%d] Width[%d] Height[%d]",
      *((const char **)this + 101),
      *((_DWORD *)this + 99),
      *((_DWORD *)this + 100));
  v4 = *((_DWORD *)this + 28);
  switch ( CBattle_Field::get_dungeon_diff(this) )
  {
    case 0:
      v4 = *((_DWORD *)this + 28);
      break;
    case 1:
      v4 = 150 * *((_DWORD *)this + 28) / 100;
      break;
    case 2:
      v4 = 250 * *((_DWORD *)this + 28) / 100;
      break;
    case 3:
      v4 = 500 * *((_DWORD *)this + 28) / 100;
      break;
    case 4:
      v4 = *((_DWORD *)this + 28);
      break;
    default:
      break;
  }
  if ( *(_BYTE *)(*((_DWORD *)this + 66) + 300) != 1 )
  {
    v2 = 100 * v4 / (*((_DWORD *)this + 99) * *((_DWORD *)this + 100));
    if ( v2 > (int)get_rand_int(100) )
      ++v3;
  }
  return v3;
}

int __cdecl CBattle_Field::get_monster_list(CBattle_Field *battlefield, int *a2)
{
  int member_count; // eax
  _DWORD value[2]; // [esp+14h] [ebp-14h] BYREF
  char flag; // [esp+1Fh] [ebp-9h]

  if ( !*((_DWORD *)a1 + 98) || !*((_DWORD *)a1 + 66) || !*(_DWORD *)a1 )
    return 0;
  if ( CDungeon::isTournamentDungeon(*((CDungeon **)a1 + 98)) )
  {
    std::map<short,std::vector<CBattle_Field::TournamentInfo>>::clear((int)a1 + 316);
    v5 = CBattle_Field::ConsistTournament((CUser ***)a1, *((_DWORD *)a1 + 66), a2, (int)a1 + 316);
    if ( v5 != 1 )
      return 0;
    member_count = CParty::get_member_count(*(CUser ***)a1);
    CDungeonClearTracer::SetMaxBossCount((CBattle_Field *)((char *)a1 + 344), member_count);
    CBattle_Field::SetTournamentCurRound(a1, 1);
  }
  else
  {
    CMap::get_monster_list((CMap *)v4, *((_DWORD *)a1 + 66));
    std::list<_mapMonster>::operator=((std::_List_node_base *)a2, (std::_List_node_base *)v4);
    std::list<_mapMonster>::~list(v4);
  }
  return 1;
}

char __cdecl CBattle_Field::add_monster_list_bossroom_condition(int *a1, int id, int id2, int id3)
{
  int id; // eax
  int id; // eax
  int id; // eax
  int id; // ebx
  int id; // eax
  _DWORD *v11; // [esp+14h] [ebp-34h] BYREF
  int id; // [esp+18h] [ebp-30h] BYREF
  int id; // [esp+1Ch] [ebp-2Ch] BYREF
  int id; // [esp+20h] [ebp-28h] BYREF
  int id; // [esp+24h] [ebp-24h] BYREF
  int id; // [esp+28h] [ebp-20h] BYREF
  int id; // [esp+2Ch] [ebp-1Ch] BYREF
  int id[2]; // [esp+30h] [ebp-18h] BYREF
  int id; // [esp+38h] [ebp-10h] BYREF
  int *v20; // [esp+3Ch] [ebp-Ch]

  v4 = a1[98];
  if ( v4 )
  {
    v4 = a1[66];
    if ( v4 )
    {
      v4 = *(_DWORD *)(a1[98] + 2316);
      if ( v4 == 1 )
      {
        std::vector<CBattle_Field::stConditionEnterBossRoomStatus_t>::begin(&v16, a1 + 92);
        __gnu_cxx::__normal_iterator<CBattle_Field::stConditionEnterBossRoomStatus_t const*,std::vector<CBattle_Field::stConditionEnterBossRoomStatus_t>>::__normal_iterator<CBattle_Field::stConditionEnterBossRoomStatus_t*>(
          &v15,
          (int)&v16);
        std::vector<CBattle_Field::stConditionEnterBossRoomStatus_t>::end(&v17, (int)(a1 + 92));
        __gnu_cxx::__normal_iterator<CBattle_Field::stConditionEnterBossRoomStatus_t const*,std::vector<CBattle_Field::stConditionEnterBossRoomStatus_t>>::__normal_iterator<CBattle_Field::stConditionEnterBossRoomStatus_t*>(
          &v14,
          (int)&v17);
        while ( 1 )
        {
          LOBYTE(v4) = __gnu_cxx::operator!=<CBattle_Field::stConditionEnterBossRoomStatus_t const*,std::vector<CBattle_Field::stConditionEnterBossRoomStatus_t>>(
                         (int)&v15,
                         (int)&v14);
          if ( !(_BYTE)v4 )
            break;
          v5 = __gnu_cxx::__normal_iterator<CBattle_Field::stConditionEnterBossRoomStatus_t const*,std::vector<CBattle_Field::stConditionEnterBossRoomStatus_t>>::operator->((int)&v15);
          std::vector<std::pair<int,int>>::begin(&v13, (_DWORD *)(v5 + 4));
          v6 = __gnu_cxx::__normal_iterator<CBattle_Field::stConditionEnterBossRoomStatus_t const*,std::vector<CBattle_Field::stConditionEnterBossRoomStatus_t>>::operator->((int)&v15);
          std::vector<std::pair<int,int>>::end(&v12, v6 + 4);
          while ( __gnu_cxx::operator!=<std::pair<int,int> const*,std::vector<std::pair<int,int>>>((int)&v13, (int)&v12) )
          {
            if ( *(_DWORD *)__gnu_cxx::__normal_iterator<std::pair<int,int> const*,std::vector<std::pair<int,int>>>::operator->((int)&v13) == a3
              && *(_DWORD *)(__gnu_cxx::__normal_iterator<std::pair<int,int> const*,std::vector<std::pair<int,int>>>::operator->((int)&v13)
                           + 4) == a4 )
            {
              CMap::get_condition_monster_list((CMap *)v18, a1[66]);
              v20 = v18;
              std::list<_mapMonster>::begin(&v11, v18);
              while ( 1 )
              {
                std::list<_mapMonster>::end(&v19, (int)v20);
                if ( !std::_List_const_iterator<_mapMonster>::operator!=(&v11, &v19) )
                  break;
                v8 = *(_DWORD *)std::_List_const_iterator<_mapMonster>::operator->(&v11);
                if ( v8 == *(_DWORD *)__gnu_cxx::__normal_iterator<CBattle_Field::stConditionEnterBossRoomStatus_t const*,std::vector<CBattle_Field::stConditionEnterBossRoomStatus_t>>::operator->((int)&v15) )
                {
                  v9 = std::_List_const_iterator<_mapMonster>::operator*(&v11);
                  std::list<_mapMonster>::push_back(a2, v9);
                }
                std::_List_const_iterator<_mapMonster>::operator++(&v11);
              }
              std::list<_mapMonster>::~list(v18);
            }
            __gnu_cxx::__normal_iterator<std::pair<int,int> const*,std::vector<std::pair<int,int>>>::operator++(&v13);
          }
          __gnu_cxx::__normal_iterator<CBattle_Field::stConditionEnterBossRoomStatus_t const*,std::vector<CBattle_Field::stConditionEnterBossRoomStatus_t>>::operator++(&v15);
        }
      }
    }
  }
  return v4;
}

    CurrentMapInfo = CBattle_Field::GetCurrentMapInfo(a2);
    std::list<_mapMonster>::begin(&v20, a3);

        && (unsigned __int8)CBattle_Field::checkClearedNamedMonsterMap(a1, a2, *(_DWORD *)(v28 + 32), *(void **)v28) )
      {
        std::_List_const_iterator<_mapMonster>::operator++(dest, &v19);
      }

              "void CBattle_Field::add_monsters_current_map(MOB_REF_LIST&, std::vector<int, std::allocator<int> >&, map_m"
              "onster&, int&, int&, int&)",

char __cdecl CBattle_Field::add_monster_hell_party(CBattle_Field *this, map_monster *a2, bool *a3, int *a4, int *a5)
{
  int hellparty_groupOrder_map; // eax
  int id; // ebx
  CDataManager *v7; // eax

  LOBYTE(hellparty_groupOrder_map) = *((_BYTE *)this + 440);
  if ( (_BYTE)hellparty_groupOrder_map )
  {
    v6 = *(_DWORD *)(CBattle_Field::GetCurrentMapInfo(this) + 4);
    v7 = G_CDataManager();
    hellparty_groupOrder_map = CDataManager::get_hellparty_groupOrder_map(v7, v6);
    if ( hellparty_groupOrder_map )
      LOBYTE(hellparty_groupOrder_map) = CBattle_Field::add_hell_party_monster(
                                           v6,
                                           (CDungeon **)this,
                                           a2,
                                           a4,
                                           a5,
                                           hellparty_groupOrder_map);
  }
  return hellparty_groupOrder_map;
}

int __cdecl CBattle_Field::add_monster_blood(CBattle_Field *this, map_monster *a2, int *a3, int *a4)
{
  int id; // ebx
  const char *v6; // [esp+14h] [ebp-34h]
  char flag[4]; // [esp+28h] [ebp-20h] BYREF
  int id[5]; // [esp+34h] [ebp-14h] BYREF

  if ( !*((_DWORD *)this + 66) )
    return 0;
  if ( !std::vector<STAssignBloodPhaseTime>::size((_DWORD *)(*((_DWORD *)this + 66) + 196)) )
    return 1;
  std::vector<STAssignBloodSystem>::operator=((int *)this + 144, (int *)(*((_DWORD *)this + 66) + 184));
  std::vector<STAssignBloodPhaseTime>::operator=((int *)this + 147, (int *)(*((_DWORD *)this + 66) + 196));
  std::vector<STAssignBloodPhaseTime>::vector((int)v7, (_DWORD *)(*((_DWORD *)this + 66) + 196));
  std::vector<STAssignBloodSystem>::vector((int)v8, (_DWORD *)(*((_DWORD *)this + 66) + 184));
  v5 = CBattle_Field::add_blood_monster((CMap **)this, a2, a3, a4, v8, v7) ^ 1;
  std::vector<STAssignBloodSystem>::~vector((void **)v8);
  std::vector<STAssignBloodPhaseTime>::~vector((void **)v7);
  if ( !(_BYTE)v5 )
    return 1;
  LogManager::logFormat(
    (LogManager *)1,
    (int)"battle_field.cpp",
    "bool CBattle_Field::add_monster_blood(map_monster&, int&, int&)",
    (const char *)0x62B,
    "BLOOD_LOG : MONSTERS SETTING ERROR!!\n",
    v6);
  return 0;
}

void __cdecl CBattle_Field::add_monster_APC_AI(CMap **this, int *a2)
{
  const char *v2; // ebx
  CDataManager *v3; // eax
  const char *v4; // eax
  const char *v5; // esi
  int index; // ebx
  int id; // eax
  int CurrentMapInfo; // eax
  void *s; // [esp+30h] [ebp-6B8h] BYREF
  int id; // [esp+34h] [ebp-6B4h] BYREF
  char flag; // [esp+38h] [ebp-6B0h]
  const char *v13; // [esp+3Ch] [ebp-6ACh]
  unsigned __int8 Level; // [esp+40h] [ebp-6A8h]
  char flag; // [esp+41h] [ebp-6A7h]
  char flag; // [esp+42h] [ebp-6A6h]
  int id; // [esp+5Ch] [ebp-68Ch]
  int id; // [esp+37Ch] [ebp-36Ch] BYREF
  int id; // [esp+380h] [ebp-368h] BYREF
  void *v20[211]; // [esp+384h] [ebp-364h] BYREF
  char *v21; // [esp+6D0h] [ebp-18h]
  char *AICharacters; // [esp+6D4h] [ebp-14h]
  int id; // [esp+6D8h] [ebp-10h]
  CAICharacter *v24; // [esp+6DCh] [ebp-Ch]

  if ( *(this + 66) )
  {
    if ( *(this + 98) )
    {
      v21 = 0;
      AICharacters = CMap::getAICharacters(*(this + 66));
      if ( !std::vector<STAICharacterMapArrangeData>::empty(AICharacters) )
      {
        map_monster::map_monster((map_monster *)&s);
        v21 = 0;
        std::vector<STAICharacterMapArrangeData>::begin(&v19, AICharacters);
        std::vector<STAICharacterMapArrangeData>::end(&v18, (int)AICharacters);
        while ( __gnu_cxx::operator!=<STAICharacterMapArrangeData const*,std::vector<STAICharacterMapArrangeData>>(
                  (int)&v19,
                  (int)&v18) )
        {
          v23 = __gnu_cxx::__normal_iterator<STAICharacterMapArrangeData const*,std::vector<STAICharacterMapArrangeData>>::operator*((int)&v19);
          v2 = *(const char **)v23;
          v3 = G_CDataManager();
          v24 = (CAICharacter *)CAICharacterList::get(*((CAICharacterList **)v3 + 8615), (unsigned int)v2);
          if ( v24 )
          {
            memset(&s, 0, 0x34Cu);
            s = v21;
            v13 = *(const char **)v23;
            v11 = *a2;
            *a2 = v11 + 1;
            v12 = *(_DWORD *)(v23 + 20);
            if ( v12 == 8 && *(_BYTE *)(CBattle_Field::GetCurrentMapInfo((CBattle_Field *)this) + 69) != 1 )
              std::vector<int>::push_back((int)(this + 70), &v11);
            if ( (char)CDungeon::get_dimension_possible(*(this + 98)) <= 0 )
              Level = CAICharacter::getLevel(v24);
            else
              Level = (unsigned __int8)*(this + 142);
            v17 = *(_DWORD *)(v23 + 16);
            v15 = *(_BYTE *)(v23 + 25);
            v16 = *(_BYTE *)(v23 + 24);
            map_monster::map_monster((map_monster *)v20, (const map_monster *)&s);
            CurrentMapInfo = CBattle_Field::GetCurrentMapInfo((CBattle_Field *)this);
            MapInfo::Add_Mob(CurrentMapInfo, (int)v20);
            map_monster::~map_monster(v20);
            ++v21;
          }
          else
          {
            if ( *(_BYTE *)(CBattle_Field::GetCurrentMapInfo((CBattle_Field *)this) + 232) )
              v4 = "layeredMap";
            else
              v4 = "generalMap";
            v5 = v4;
            index = CMap::get_index(*(this + 66));
            v7 = CDungeon::get_index(*(this + 98));
            LogManager::logFormat(
              (LogManager *)1,
              (int)"battle_field.cpp",
              "void CBattle_Field::add_monster_APC_AI(int&)",
              (const char *)0x649,
              "Unknown APC Type(%d). In Dungeon(%d), Map(%d) [Map Kind : %s]",
              *(const char **)v23,
              v7,
              index,
              v5);
          }
          __gnu_cxx::__normal_iterator<STAICharacterMapArrangeData const*,std::vector<STAICharacterMapArrangeData>>::operator++(&v19);
        }
        map_monster::~map_monster(&s);
      }
    }
  }
}

void __cdecl CBattle_Field::add_monster_APC_AI_by_rate(
        CDungeon **this,

bool __cdecl CBattle_Field::add_monster_additional_champion(CBattle_Field *battlefield, _DWORD *a2, int id)
{
  bool result; // al
  unsigned int id; // ebx
  int CurrentMapInfo; // eax
  char *v6; // [esp+34h] [ebp-24h] BYREF
  unsigned int id; // [esp+38h] [ebp-20h] BYREF
  int id; // [esp+3Ch] [ebp-1Ch] BYREF
  int id; // [esp+40h] [ebp-18h] BYREF
  int id; // [esp+44h] [ebp-14h] BYREF
  int i; // [esp+48h] [ebp-10h]
  int id; // [esp+4Ch] [ebp-Ch]

  for ( i = 0; ; ++i )
  {
    result = i < a3;
    if ( i >= a3 )
      break;
    result = std::vector<int>::size(a2) == 0;
    if ( result )
      break;
    v4 = rand();
    v7 = v4 % std::vector<int>::size(a2);
    v6 = *(char **)std::vector<int>::operator[](a2, v7);
    std::vector<int>::begin(&v10, a2);
    __gnu_cxx::__normal_iterator<int *,std::vector<int>>::operator+(&v9, &v10, &v7);
    std::vector<int>::erase(&v8, (int)a2, v9);
    CurrentMapInfo = CBattle_Field::GetCurrentMapInfo(a1);
    v12 = std::map<int,map_monster>::operator[](CurrentMapInfo + 12, &v6);
    if ( *(_BYTE *)(v12 + 19) )
    {
      if ( *(_BYTE *)(v12 + 8) == 1 )
        LogManager::logFormat(
          (LogManager *)1,
          (int)"battle_field.cpp",
          "void CBattle_Field::add_monster_additional_champion(std::vector<int, std::allocator<int> >&, int)",
          (const char *)0x6BF,
          "Box Monster is Champion! id(%d)",
          v6);
    }
    else
    {
      *(_BYTE *)(v12 + 8) = 1;
    }
  }
  return result;
}

char __cdecl CBattle_Field::add_monster_event(CBattle_Field *this, int *a2, int id, int value)
{
  int id; // eax
  int CurrentMapInfo; // eax
  int id; // eax
  int id; // eax
  unsigned int id; // [esp+14h] [ebp-6C4h] BYREF
  int id; // [esp+18h] [ebp-6C0h]
  char flag; // [esp+1Ch] [ebp-6BCh]
  int id; // [esp+20h] [ebp-6B8h]
  char standard_level; // [esp+24h] [ebp-6B4h]
  char flag; // [esp+25h] [ebp-6B3h]
  char flag; // [esp+26h] [ebp-6B2h]
  char flag; // [esp+27h] [ebp-6B1h]
  char flag; // [esp+28h] [ebp-6B0h]
  int id; // [esp+40h] [ebp-698h]
  char flag; // [esp+48h] [ebp-690h]
  char flag; // [esp+35Eh] [ebp-37Ah]
  int id; // [esp+360h] [ebp-378h] BYREF
  int id; // [esp+364h] [ebp-374h] BYREF
  int id; // [esp+368h] [ebp-370h] BYREF
  int id; // [esp+36Ch] [ebp-36Ch] BYREF
  void *v25[211]; // [esp+370h] [ebp-368h] BYREF
  int id; // [esp+6BCh] [ebp-1Ch]
  int id; // [esp+6C0h] [ebp-18h]
  unsigned int id; // [esp+6C4h] [ebp-14h]
  int *v29; // [esp+6C8h] [ebp-10h]
  int i; // [esp+6CCh] [ebp-Ch]

  v4 = *((_DWORD *)this + 98);
  if ( v4 )
  {
    v4 = *((_DWORD *)this + 66);
    if ( v4 )
    {
      LOBYTE(v4) = !std::vector<CBattle_Field::stEventMonsterStatus_t>::empty((_DWORD *)this + 95);
      if ( (_BYTE)v4 )
      {
        v26 = 0;
        CurrentMapInfo = CBattle_Field::GetCurrentMapInfo(this);
        v27 = std::map<int,map_monster>::size(CurrentMapInfo + 12);
        v28 = MAX_LIMIT_10000;
        std::vector<CBattle_Field::stEventMonsterStatus_t>::begin(&v23, (_DWORD *)this + 95);
        __gnu_cxx::__normal_iterator<CBattle_Field::stEventMonsterStatus_t const*,std::vector<CBattle_Field::stEventMonsterStatus_t>>::__normal_iterator<CBattle_Field::stEventMonsterStatus_t*>(
          &v22,
          (int)&v23);
        std::vector<CBattle_Field::stEventMonsterStatus_t>::end(&v24, (int)this + 380);
        __gnu_cxx::__normal_iterator<CBattle_Field::stEventMonsterStatus_t const*,std::vector<CBattle_Field::stEventMonsterStatus_t>>::__normal_iterator<CBattle_Field::stEventMonsterStatus_t*>(
          &v21,
          (int)&v24);
        while ( 1 )
        {
          LOBYTE(v4) = __gnu_cxx::operator!=<CBattle_Field::stEventMonsterStatus_t const*,std::vector<CBattle_Field::stEventMonsterStatus_t>>(
                         (int)&v22,
                         (int)&v21);
          if ( !(_BYTE)v4 )
            break;
          v29 = (int *)__gnu_cxx::__normal_iterator<CBattle_Field::stEventMonsterStatus_t const*,std::vector<CBattle_Field::stEventMonsterStatus_t>>::operator*((int)&v22);
          if ( v29[1] == a3 && v29[2] == a4 )
          {
            LOBYTE(v4) = std::vector<STRivalMapData>::empty((_DWORD *)(*((_DWORD *)this + 66) + 156));
            if ( (_BYTE)v4 )
              return v4;
            for ( i = 0; v29[3] > i; ++i )
            {
              map_monster::map_monster((map_monster *)&v9);
              v12 = *v29;
              standard_level = CDungeon::get_standard_level(*((CDungeon **)this + 98));
              v9 = v28;
              v14 = v29[5];
              v15 = v29[4];
              v11 = 0;
              v16 = 0;
              v17 = 0;
              v18 = 100;
              v19 = 0;
              v20 = 1;
              if ( v26 < v27 )
              {
                v10 = *a2;
                *a2 = v10 + 1;
                map_monster::map_monster((map_monster *)v25, (const map_monster *)&v9);
                v6 = CBattle_Field::GetCurrentMapInfo(this);
                MapInfo::Add_Mob(v6, (int)v25);
                map_monster::~map_monster(v25);
                ++v28;
                v7 = std::vector<STRivalMapData>::size((_DWORD *)(*((_DWORD *)this + 66) + 156));
                if ( v7 + MAX_LIMIT_10000 <= v28 )
                  v28 = MAX_LIMIT_10000;
              }
              ++v26;
              map_monster::~map_monster((void **)&v9);
            }
          }
          __gnu_cxx::__normal_iterator<CBattle_Field::stEventMonsterStatus_t const*,std::vector<CBattle_Field::stEventMonsterStatus_t>>::operator++(&v22);
        }
      }
    }
  }
  return v4;
}

char __cdecl CBattle_Field::set_monster_ability(CBattle_Field *this)
{
  int value; // eax
  int value; // eax
  int value; // eax
  int value; // eax
  int value; // eax
  int value; // ebx
  CMonsterManager **v7; // eax
  int value; // edi
  int value; // esi
  int value; // ebx
  char *MonsterScript; // eax
  int *v12; // esi
  char *v13; // eax
  _DWORD *v14; // edi
  unsigned int value; // ebx
  ActiveStaticInfo *v16; // esi
  int dungeon_diff; // eax
  unsigned int value; // ebx
  char *v19; // eax
  int value; // ebx
  int value; // eax
  ActiveStaticInfo *v23; // [esp+2Ch] [ebp-4Ch]
  int value[3]; // [esp+3Ch] [ebp-3Ch] BYREF
  int value; // [esp+48h] [ebp-30h] BYREF
  std::_Rb_tree_node_base::_Base_ptr v26; // [esp+4Ch] [ebp-2Ch] BYREF
  int CurrentMapInfo; // [esp+50h] [ebp-28h]
  int value; // [esp+54h] [ebp-24h]
  CMonster *monster; // [esp+58h] [ebp-20h]
  int RandomChampionElement; // [esp+5Ch] [ebp-1Ch]

  v1 = *((_DWORD *)this + 98);
  if ( v1 )
  {
    CurrentMapInfo = CBattle_Field::GetCurrentMapInfo(this);
    *(_DWORD *)(CurrentMapInfo + 72) = rand();
    v2 = CBattle_Field::GetCurrentMapInfo(this);
    srand(*(_DWORD *)(v2 + 72));
    v3 = CBattle_Field::GetCurrentMapInfo(this);
    std::map<int,map_monster>::begin(&v26, v3 + 12);
    v4 = CBattle_Field::GetCurrentMapInfo(this);
    std::map<int,map_monster>::end(&v25, v4 + 12);
    while ( 1 )
    {
      LOBYTE(v1) = std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator!=(&v26, &v25);
      if ( !(_BYTE)v1 )
        break;
      v5 = std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator->(&v26);
      v28 = v5 + 4;
      if ( *(char *)(v5 + 12) <= 4 )
      {
        v6 = *(_DWORD *)(v28 + 12);
        v7 = (CMonsterManager **)G_CDataManager();
        monster = (CMonster *)CDataManager::find_monster(v7, v6);
        if ( monster )
        {
          v8 = *(char *)(v28 + 8);
          v9 = *((_DWORD *)this + 101);
          v10 = *(unsigned __int8 *)(v28 + 16);
          v23 = (ActiveStaticInfo *)(v28 + 56);
          MonsterScript = CMonster::getMonsterScript(monster);
          setBasicAbilityByLevel((int)MonsterScript, v23, v10, v9, v8, 0, 0);
          LOBYTE(v10) = *(_BYTE *)(v28 + 8) == 3;
          v12 = (int *)(v28 + 56);
          v13 = CMonster::getMonsterScript(monster);
          upgradeAbilityByMonsterParameterCategory((_DWORD *)v13 + 31, v12, v10);
          v14 = (_DWORD *)(*((_DWORD *)this + 98) + 2304);
          v15 = *(unsigned __int8 *)(v28 + 16);
          v16 = (ActiveStaticInfo *)(v28 + 56);
          dungeon_diff = CBattle_Field::get_dungeon_diff(this);
          upgradeAbilityByDifficultyOfDungeon(dungeon_diff, v16, v15, v14);
          if ( *(_BYTE *)(v28 + 8) == 3 )
          {
            upgradeAbilityByBossFeature();
          }
          else if ( *(_BYTE *)(v28 + 8) == 1 || *(_BYTE *)(v28 + 8) == 2 )
          {
            v18 = *(_DWORD *)(CBattle_Field::GetCurrentMapInfo(this) + 72);
            v19 = CMonster::getMonsterScript(monster);
            RandomChampionElement = getRandomChampionElement((int)v19, v18);
            std::vector<int>::vector(v24);
            v20 = CBattle_Field::get_dungeon_diff(this);
            v21 = CBattle_Field::GetCurrentMapInfo(this);
            makeChampionFeatureList(
              (_DWORD *)(v28 + 808),
              *(unsigned __int8 *)(v28 + 16),
              *(_DWORD *)(v21 + 72),
              v24,
              v20);
            upgradeAbilityByChampionFeature(RandomChampionElement, *(char *)(v28 + 8), v24, (_DWORD *)(v28 + 56));
            std::vector<int>::~vector((void **)v24);
          }
        }
      }
      std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator++(&v26);
    }
  }
  return v1;
}

char __cdecl CBattle_Field::insert_tournamentRoundMonster(CBattle_Field *this)
{
  int id; // eax
  int id; // eax
  int i; // [esp+1Ch] [ebp-6CCh] BYREF
  std::_Rb_tree_node_base *v6; // [esp+20h] [ebp-6C8h] BYREF
  int id; // [esp+24h] [ebp-6C4h] BYREF
  int id; // [esp+28h] [ebp-6C0h] BYREF
  _DWORD value[212]; // [esp+2Ch] [ebp-6BCh] BYREF
  _DWORD value[212]; // [esp+37Ch] [ebp-36Ch] BYREF
  _BYTE dest[4]; // [esp+6CCh] [ebp-1Ch] BYREF
  int id; // [esp+6D0h] [ebp-18h]
  int id; // [esp+6D4h] [ebp-14h]
  int member_count; // [esp+6D8h] [ebp-10h]
  int id; // [esp+6DCh] [ebp-Ch]

  v1 = *((_DWORD *)this + 98);
  if ( v1 )
  {
    v1 = *(_DWORD *)this;
    if ( *(_DWORD *)this )
    {
      LOBYTE(v1) = CDungeon::isTournamentDungeon(*((CDungeon **)this + 98));
      if ( (_BYTE)v1 )
      {
        v12 = CBattle_Field::GetCurrentMapInfo(this) + 208;
        v13 = CBattle_Field::GetCurrentMapInfo(this) + 12;
        std::map<int,map_monster>::begin(&v6, v13);
        member_count = CParty::get_member_count(*(CUser ***)this);
        for ( i = 1; ; ++i )
        {
          LOBYTE(v1) = i <= 4;
          if ( i > 4 )
            break;
          v15 = 0;
          while ( v15 < member_count )
          {
            std::map<int,map_monster>::end(&v7, v13);
            if ( !std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator!=(&v6, &v7) )
              break;
            v2 = std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator->(&v6);
            std::make_pair<int &,map_monster &>(v10, (int)&i, v2 + 4);
            std::pair<int const,map_monster>::pair<int,map_monster>(v9, (int)v10);
            std::multimap<int,map_monster>::insert(&v8, v12, (int)v9);
            std::pair<int const,map_monster>::~pair((int)v9);
            std::pair<int,map_monster>::~pair((int)v10);
            ++v15;
            std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator++(dest, &v6);
          }
        }
      }
    }
  }
  return v1;
}

void __cdecl CBattle_Field::add_item_curr_map(CBattle_Field *this, int *a2, int *a3)
{
  signed int *v3; // eax
  unsigned int item_id; // eax
  signed int item_id; // esi
  int item_id; // ebx
  int CurrentMapInfo; // eax
  CDataManager *v10; // eax
  int ItemGenerator; // eax
  int Generator; // eax
  CDataManager *v13; // eax
  int item_id; // eax
  int item_id; // eax
  CDataManager *v16; // eax
  int item_id; // eax
  int item_id; // eax
  int item_id; // eax
  char *v21[21]; // [esp+4h] [ebp-214h] BYREF
  int item_id; // [esp+6Ch] [ebp-1ACh]
  _DWORD value[21]; // [esp+70h] [ebp-1A8h] BYREF
  void *v24[18]; // [esp+C4h] [ebp-154h] BYREF
  void *v25[14]; // [esp+10Ch] [ebp-10Ch] BYREF
  char quality; // [esp+144h] [ebp-D4h]
  char quality; // [esp+145h] [ebp-D3h]
  void *v28[13]; // [esp+154h] [ebp-C4h] BYREF
  char standard_level; // [esp+188h] [ebp-90h]
  char dungeon_diff; // [esp+189h] [ebp-8Fh]
  char quality; // [esp+18Ah] [ebp-8Eh]
  char quality; // [esp+18Bh] [ebp-8Dh]
  void *v33[12]; // [esp+19Ch] [ebp-7Ch] BYREF
  int item_id; // [esp+1CCh] [ebp-4Ch] BYREF
  int item_id; // [esp+1D0h] [ebp-48h] BYREF
  signed int item_id; // [esp+1D4h] [ebp-44h] BYREF
  signed int item_id; // [esp+1D8h] [ebp-40h]
  signed int item_id; // [esp+1DCh] [ebp-3Ch]
  signed int item_id; // [esp+1E0h] [ebp-38h]
  _DWORD *v40; // [esp+1E4h] [ebp-34h] BYREF
  int item_id[2]; // [esp+1E8h] [ebp-30h] BYREF
  _DWORD value[2]; // [esp+1F0h] [ebp-28h] BYREF
  char *SpecialPassiveObjectItems; // [esp+1F8h] [ebp-20h]
  _DWORD *v44; // [esp+1FCh] [ebp-1Ch]

  if ( *((_DWORD *)this + 98) && *((_DWORD *)this + 66) )
  {
    *(_DWORD *)(CBattle_Field::GetCurrentMapInfo(this) + 60) = *a3;
    CMap::get_item_list((CMap *)v41, *((_DWORD *)this + 66));
    std::list<_mapItem>::begin(&v40, v41);
    v42[1] = *(_DWORD *)(CBattle_Field::GetCurrentMapInfo(this) + 4);
    _mapItem::_mapItem((_mapItem *)&v36);
    SpecialPassiveObjectItems = CDungeon::getSpecialPassiveObjectItems(*((CDungeon **)this + 98));
    WongWork::stGenerateRefData_t::stGenerateRefData_t((WongWork::stGenerateRefData_t *)v28);
    WongWork::stGenerateRefData_t::stGenerateRefData_t((WongWork::stGenerateRefData_t *)v25);
    WongWork::stGenerateRefData_t::stGenerateRefData_t((WongWork::stGenerateRefData_t *)v24);
    dungeon_diff = CBattle_Field::get_dungeon_diff(this);
    v31 = 1;
    v24[13] = CDungeon::getObjectDropItems(*((CDungeon **)this + 98));
    while ( 1 )
    {
      std::list<_mapItem>::end(v42, (int)v41);
      if ( !std::_List_iterator<_mapItem>::operator!=(&v40, v42) || *((_DWORD *)this + 109) == 1 )
      {
LABEL_29:
        WongWork::stGenerateRefData_t::~stGenerateRefData_t(v24);
        WongWork::stGenerateRefData_t::~stGenerateRefData_t(v25);
        WongWork::stGenerateRefData_t::~stGenerateRefData_t(v28);
        std::list<_mapItem>::~list(v41);
        return;
      }
      v3 = (signed int *)std::_List_iterator<_mapItem>::operator*(&v40);
      v36 = *v3;
      v37 = v3[1];
      v38 = v3[2];
      v39 = v3[3];
      v4 = std::vector<stDungeonAssignItem_t>::size(SpecialPassiveObjectItems);
      if ( v4 <= v36 || v36 < 0 )
      {
        v6 = v36;
        v7 = std::vector<stDungeonAssignItem_t>::size(SpecialPassiveObjectItems);
        CurrentMapInfo = CBattle_Field::GetCurrentMapInfo(this);
        LogManager::logFormat(
          (LogManager *)1,
          (int)"battle_field.cpp",
          "void CBattle_Field::add_item_curr_map(int&, int&)",
          (const char *)0x7A5,
          "Dungeon passive object index error! Map id:%d, DungeonAssignItem size:%d, Passive object id:%d",
          *(const char **)(CurrentMapInfo + 4),
          v7,
          v6);
        goto LABEL_29;
      }
      v44 = (_DWORD *)std::vector<stDungeonAssignItem_t>::at(SpecialPassiveObjectItems, v36);
      if ( *v44 != -1 )
        break;
      if ( CDungeon::get_standard_level(*((CDungeon **)this + 98)) > 0
        && CDungeon::get_standard_level(*((CDungeon **)this + 98)) <= 200 )
      {
        if ( (char)CDungeon::get_dimension_possible(*((CDungeon **)this + 98)) <= 0 )
        {
          standard_level = CDungeon::get_standard_level(*((CDungeon **)this + 98));
          v26 = CDungeon::get_standard_level(*((CDungeon **)this + 98));
        }
        else
        {
          standard_level = *((_DWORD *)this + 142);
          v26 = *((_DWORD *)this + 142);
        }
LABEL_20:
        WongWork::stGenerateResult_t::stGenerateResult_t((WongWork::stGenerateResult_t *)v33);
        v32 = v38;
        v27 = v37;
        v25[13] = v44 + 1;
        v10 = G_CDataManager();
        ItemGenerator = CDataManager::getItemGenerator(v10);
        Generator = WongWork::CItemGeneratorMgr::getGenerator(ItemGenerator, 1);
        (*(void (__cdecl **)(int, void **, void **))(*(_DWORD *)Generator + 8))(Generator, v28, v33);
        v13 = G_CDataManager();
        v14 = CDataManager::getItemGenerator(v13);
        v15 = WongWork::CItemGeneratorMgr::getGenerator(v14, 1);
        (*(void (__cdecl **)(int, void **, void **))(*(_DWORD *)v15 + 12))(v15, v25, v33);
        v16 = G_CDataManager();
        v17 = CDataManager::getItemGenerator(v16);
        v18 = WongWork::CItemGeneratorMgr::getGenerator(v17, 4);
        (*(void (__cdecl **)(int, void **, void **))(*(_DWORD *)v18 + 12))(v18, v24, v33);
        std::vector<Inven_Item>::begin(&v35, v33);
        std::vector<Inven_Item>::end(&v34, (int)v33);
        while ( __gnu_cxx::operator!=<Inven_Item *,std::vector<Inven_Item>>((int)&v35, (int)&v34) )
        {
          map_item::map_item((map_item *)v23);
          LOBYTE(v23[0]) = 1;
          v23[1] = *a2;
          *a2 = v23[1] + 1;
          BYTE1(v23[0]) = v39;
          v19 = __gnu_cxx::__normal_iterator<Inven_Item *,std::vector<Inven_Item>>::operator*((int)&v35);
          v23[4] = *(_DWORD *)v19;
          v23[5] = *(_DWORD *)(v19 + 4);
          v23[6] = *(_DWORD *)(v19 + 8);
          v23[7] = *(_DWORD *)(v19 + 12);
          v23[8] = *(_DWORD *)(v19 + 16);
          v23[9] = *(_DWORD *)(v19 + 20);
          v23[10] = *(_DWORD *)(v19 + 24);
          v23[11] = *(_DWORD *)(v19 + 28);
          v23[12] = *(_DWORD *)(v19 + 32);
          v23[13] = *(_DWORD *)(v19 + 36);
          v23[14] = *(_DWORD *)(v19 + 40);
          v23[15] = *(_DWORD *)(v19 + 44);
          v23[16] = *(_DWORD *)(v19 + 48);
          v23[17] = *(_DWORD *)(v19 + 52);
          v23[18] = *(_DWORD *)(v19 + 56);
          LOBYTE(v23[19]) = *(_BYTE *)(v19 + 60);
          v22 = CBattle_Field::GetCurrentMapInfo(this);
          qmemcpy(v21, v23, sizeof(v21));
          MapInfo::Add_Item(v22, (char)v21[0], v21[1]);
          __gnu_cxx::__normal_iterator<Inven_Item *,std::vector<Inven_Item>>::operator++(&v35);
        }
        std::_List_iterator<_mapItem>::operator++(&v40);
        WongWork::stGenerateResult_t::~stGenerateResult_t(v33);
      }
    }
    standard_level = *v44;
    v26 = *v44;
    goto LABEL_20;
  }
}

void __cdecl CBattle_Field::ConsistMap(CBattle_Field *this)
{
  int BossCount; // eax
  int id; // ebx
  CDungeonList **v3; // eax
  unsigned int id; // eax
  void *v5[211]; // [esp+28h] [ebp-390h] BYREF
  int id; // [esp+374h] [ebp-44h] BYREF
  bool v7; // [esp+37Bh] [ebp-3Dh] BYREF
  int rand_int; // [esp+37Ch] [ebp-3Ch] BYREF
  int id; // [esp+380h] [ebp-38h] BYREF
  int id[3]; // [esp+384h] [ebp-34h] BYREF
  int id; // [esp+390h] [ebp-28h] BYREF
  int id; // [esp+394h] [ebp-24h] BYREF
  int id; // [esp+398h] [ebp-20h] BYREF
  int id[2]; // [esp+39Ch] [ebp-1Ch] BYREF
  MapInfo *CurrentMapInfo; // [esp+3A4h] [ebp-14h]
  int AddChampionMobCnt; // [esp+3ACh] [ebp-Ch]

  if ( *(_DWORD *)this )
  {
    CurrentMapInfo = (MapInfo *)CBattle_Field::GetCurrentMapInfo(this);
    *((_DWORD *)this + 66) = CBattle_Field::getMap(this, *((_DWORD *)CurrentMapInfo + 1));
    if ( *((_DWORD *)this + 66) )
    {
      BossCount = CMap::GetBossCount(*((CMap **)this + 66));
      CBattle_Field::setDungeonClearTrace(this, *((_DWORD *)CurrentMapInfo + 1), BossCount, 0, 0);
      v2 = *((_DWORD *)this + 101);
      v3 = (CDungeonList **)G_CDataManager();
      if ( CDataManager::find_dungeon(v3, v2) )
      {
        AddChampionMobCnt = CBattle_Field::getAddChampionMobCnt(this);
        if ( AddChampionMobCnt >= 0 )
        {
          std::list<_mapMonster>::list(v14);
          if ( (unsigned __int8)CBattle_Field::get_monster_list(this, v14) == 1 )
          {
            v13 = -1;
            v12 = -1;
            CBattle_Field::getCurPosXY(this, &v13, &v12);
            CBattle_Field::add_monster_list_bossroom_condition((int *)this, (int)v14, v13, v12);
            v11 = 0;
            map_monster::map_monster((map_monster *)v5);
            std::vector<int>::vector(v10);
            v9 = 0;
            rand_int = get_rand_int(60000);
            CBattle_Field::add_monsters_current_map(v2, this, v14, (int)v10, (map_monster *)v5, &v11, &rand_int, &v9);
            v7 = 0;
            CBattle_Field::add_monster_hell_party(this, (map_monster *)v5, &v7, &rand_int, &v9);
            if ( (unsigned __int8)CBattle_Field::add_monster_blood(this, (map_monster *)v5, &rand_int, &v9) == 1 )
            {
              CDungeonClearTracer::SetMaxMonsterCount((CBattle_Field *)((char *)this + 344), v11);
              CBattle_Field::add_monster_APC_AI((CMap **)this, &rand_int);
              CBattle_Field::add_monster_APC_AI_by_rate(
                (CDungeon **)this,
                (map_monster *)v5,
                &rand_int,
                (CDungeon *)v13,
                (CDungeon *)v12);
              CBattle_Field::add_monster_additional_champion(this, v10, AddChampionMobCnt);
              CBattle_Field::add_monster_event(this, &rand_int, v13, v12);
              CBattle_Field::set_monster_ability(this);
              CBattle_Field::insert_tournamentRoundMonster(this);
              v6 = get_rand_int(60000);
              CBattle_Field::add_item_curr_map(this, &v6, &rand_int);
              *((_DWORD *)CurrentMapInfo + 16) = v6;
              if ( *(_DWORD *)(*((_DWORD *)this + 66) + 236) )
                v4 = *(_DWORD *)(*((_DWORD *)this + 66) + 236);
              else
                v4 = *((_DWORD *)G_CDataManager() + 4724);
              MapInfo::SelectDonsterItemDropLimit(CurrentMapInfo, v4);
            }
            std::vector<int>::~vector((void **)v10);
            map_monster::~map_monster(v5);
          }
          std::list<_mapMonster>::~list(v14);
        }
      }
      else
      {
        LogManager::logFormat(
          (LogManager *)1,
          (int)"battle_field.cpp",
          "void CBattle_Field::ConsistMap()",
          (const char *)0x7F2,
          "G_CDataManager()->find_dungeon( %d) fail",
          *((const char **)this + 101));
      }
    }
  }
}

int __cdecl CBattle_Field::ConsistTournament(CUser ***a1, int id, int *a3, int id2)
{
  int id; // ebx
  char flag; // bl
  int id; // eax
  char flag; // [esp+8h] [ebp-90h]
  char flag; // [esp+8h] [ebp-90h]
  void *v10[3]; // [esp+20h] [ebp-78h] BYREF
  void *v11[3]; // [esp+2Ch] [ebp-6Ch] BYREF
  int id; // [esp+38h] [ebp-60h] BYREF
  void *v13[3]; // [esp+3Ch] [ebp-5Ch] BYREF
  int id[4]; // [esp+48h] [ebp-50h] BYREF
  int id[4]; // [esp+58h] [ebp-40h] BYREF
  char flag; // [esp+6Bh] [ebp-2Dh] BYREF
  int id; // [esp+6Ch] [ebp-2Ch] BYREF
  int id[4]; // [esp+70h] [ebp-28h] BYREF
  int id; // [esp+80h] [ebp-18h] BYREF
  int id; // [esp+84h] [ebp-14h] BYREF
  int member_count; // [esp+88h] [ebp-10h]

  if ( a2 )
  {
    if ( std::map<char,std::vector<TournamentMonster>>::empty(a2 + 276) )
    {
      cMyTrace::cMyTrace(
        (cMyTrace *)v15,
        "bool CBattle_Field::ConsistTournament(const CMap*, MOB_REF_LIST&, CBattle_Field::TournamentRoundInfoMap&)",
        2913,
        5);
      cMyTrace::operator()((int)v15, "CBattle_Field::ConsistTournament ERROR!! : tournamentMonster is empty", v9);
      return 0;
    }
    else
    {
      std::vector<TournamentMonster>::vector(v13);
      member_count = CParty::get_member_count(*a1);
      v16 = member_count;
      std::map<char,std::vector<TournamentMonster>>::find(&v12, a2 + 276, &v16);
      std::map<char,std::vector<TournamentMonster>>::end(&v17, a2 + 276);
      if ( std::_Rb_tree_const_iterator<std::pair<char const,std::vector<TournamentMonster>>>::operator==(&v12, &v17) )
      {
        v5 = CParty::get_member_count(*a1);
        cMyTrace::cMyTrace(
          (cMyTrace *)v18,
          "bool CBattle_Field::ConsistTournament(const CMap*, MOB_REF_LIST&, CBattle_Field::TournamentRoundInfoMap&)",
          2924,
          5);
        cMyTrace::operator()(
          (int)v18,
          "CBattle_Field::ConsistTournament ERROR!! : party member count is invalid(%d)",
          v5);
        v4 = 0;
      }
      else
      {
        v6 = std::_Rb_tree_const_iterator<std::pair<char const,std::vector<TournamentMonster>>>::operator->(&v12);
        std::vector<TournamentMonster>::operator=((int *)v13, (int *)(v6 + 4));
        std::vector<CBattle_Field::TournamentInfo>::vector(v11);
        if ( !CBattle_Field::MakeBasicRoundInfo((int)a1, member_count, v13, (int)v11) )
        {
          v4 = 0;
        }
        else
        {
          std::vector<CBattle_Field::TournamentInfo>::end(&v19, (int)v11);
          std::vector<CBattle_Field::TournamentInfo>::begin(&v20, v11);
          std::sort<__gnu_cxx::__normal_iterator<CBattle_Field::TournamentInfo *,std::vector<CBattle_Field::TournamentInfo>>>(
            v20,
            v19);
          std::vector<CBattle_Field::TournamentInfo>::vector(v10);
          CBattle_Field::ChoiceFirstGroup((int)a1, v11, (int)v10);
          CBattle_Field::ConsistTournamentRound((int)a1, v10, v11, a4, a3);
          v4 = 1;
          std::vector<CBattle_Field::TournamentInfo>::~vector(v10);
        }
        std::vector<CBattle_Field::TournamentInfo>::~vector(v11);
      }
      std::vector<TournamentMonster>::~vector(v13);
    }
  }
  else
  {
    cMyTrace::cMyTrace(
      (cMyTrace *)v14,
      "bool CBattle_Field::ConsistTournament(const CMap*, MOB_REF_LIST&, CBattle_Field::TournamentRoundInfoMap&)",
      2906,
      5);
    cMyTrace::operator()((int)v14, "CBattle_Field::ConsistTournament ERROR!! : pMap is NULL pointer", v8);
    return 0;
  }
  return v4;
}

_BOOL4 __cdecl CBattle_Field::MakeBasicRoundInfo(int count, int count2, _DWORD *a3, int count3)
{
  int count; // ebx
  _DWORD *v6; // eax
  int count; // edx
  int count; // ebx
  _DWORD value[5]; // [esp+18h] [ebp-40h] BYREF
  int count; // [esp+2Ch] [ebp-2Ch]
  int count; // [esp+30h] [ebp-28h] BYREF
  int count; // [esp+34h] [ebp-24h] BYREF
  int count; // [esp+38h] [ebp-20h] BYREF
  int count; // [esp+3Ch] [ebp-1Ch] BYREF
  int count; // [esp+40h] [ebp-18h] BYREF
  int count; // [esp+44h] [ebp-14h] BYREF
  int i; // [esp+48h] [ebp-10h]
  int j; // [esp+4Ch] [ebp-Ch]

  std::vector<TournamentMonster>::end(&v13, (int)a3);
  std::vector<TournamentMonster>::begin(&v14, a3);
  std::random_shuffle<__gnu_cxx::__normal_iterator<TournamentMonster *,std::vector<TournamentMonster>>>(v14, v13);
  std::vector<TournamentMonster>::begin(&v12, a3);
  std::vector<TournamentMonster>::end(&v15, (int)a3);
  if ( __gnu_cxx::operator==<TournamentMonster *,std::vector<TournamentMonster>>((int)&v12, (int)&v15) )
    return 0;
  for ( i = 0; i <= 14; ++i )
  {
    std::vector<TournamentMonster>::end(&v16, (int)a3);
    if ( !__gnu_cxx::operator!=<TournamentMonster *,std::vector<TournamentMonster>>((int)&v12, (int)&v16) )
      break;
    CBattle_Field::TournamentInfo::TournamentInfo((CBattle_Field::TournamentInfo *)v10);
    for ( j = 0; j < a2; ++j )
    {
      v5 = j;
      v6 = (_DWORD *)__gnu_cxx::__normal_iterator<TournamentMonster *,std::vector<TournamentMonster>>::operator*((int)&v12);
      v7 = v6[1];
      v10[2 * v5 + 1] = *v6;
      v10[2 * v5 + 2] = v7;
      v8 = v11;
      v11 = v8
          + *(_DWORD *)(__gnu_cxx::__normal_iterator<TournamentMonster *,std::vector<TournamentMonster>>::operator->((int)&v12)
                      + 4);
      __gnu_cxx::__normal_iterator<TournamentMonster *,std::vector<TournamentMonster>>::operator++(&v17, &v12);
    }
    std::vector<CBattle_Field::TournamentInfo>::push_back(a4, v10);
  }
  return i == 15;
}

bool __cdecl CBattle_Field::ChoiceFirstGroup(int id, _DWORD *a2, int value)
{
  int id; // eax
  bool result; // al
  _DWORD *v5; // eax
  int id; // [esp+1Ch] [ebp-1Ch] BYREF
  int id; // [esp+24h] [ebp-14h] BYREF
  int id; // [esp+28h] [ebp-10h] BYREF
  int id; // [esp+2Ch] [ebp-Ch]

  std::vector<CBattle_Field::TournamentInfo>::begin(&v7, a2);
  v9 = *(_DWORD *)(__gnu_cxx::__normal_iterator<CBattle_Field::TournamentInfo *,std::vector<CBattle_Field::TournamentInfo>>::operator->((int)&v7)
                 + 20);
  while ( 1 )
  {
    std::vector<CBattle_Field::TournamentInfo>::end(&v8, (int)a2);
    result = __gnu_cxx::operator!=<CBattle_Field::TournamentInfo *,std::vector<CBattle_Field::TournamentInfo>>(
               (int)&v7,
               (int)&v8);
    if ( !result )
      break;
    v3 = __gnu_cxx::__normal_iterator<CBattle_Field::TournamentInfo *,std::vector<CBattle_Field::TournamentInfo>>::operator->((int)&v7);
    result = *(_DWORD *)(v3 + 20) == v9;
    if ( !result )
      break;
    v5 = (_DWORD *)__gnu_cxx::__normal_iterator<CBattle_Field::TournamentInfo *,std::vector<CBattle_Field::TournamentInfo>>::operator*((int)&v7);
    std::vector<CBattle_Field::TournamentInfo>::push_back(a3, v5);
    std::vector<CBattle_Field::TournamentInfo>::erase(&v6, (int)a2, v7);
    v7 = v6;
  }
  return result;
}

void __cdecl CBattle_Field::ConsistTournamentRound(int id, _DWORD *a2, _DWORD *a3, int id2, int *a5)
{
  _WORD *v5; // eax
  int id; // ebx
  int id; // eax
  int i; // [esp+10h] [ebp-E8h] BYREF
  void *v9[3]; // [esp+14h] [ebp-E4h] BYREF
  int id; // [esp+20h] [ebp-D8h] BYREF
  _WORD v11[12]; // [esp+24h] [ebp-D4h] BYREF
  _DWORD value[6]; // [esp+3Ch] [ebp-BCh] BYREF
  _BYTE v13[8]; // [esp+54h] [ebp-A4h] BYREF
  _BYTE v14[28]; // [esp+5Ch] [ebp-9Ch] BYREF
  _BYTE v15[28]; // [esp+78h] [ebp-80h] BYREF
  int id; // [esp+94h] [ebp-64h] BYREF
  int id; // [esp+98h] [ebp-60h] BYREF
  _BYTE v18[8]; // [esp+9Ch] [ebp-5Ch] BYREF
  _BYTE v19[28]; // [esp+A4h] [ebp-54h] BYREF
  _BYTE v20[28]; // [esp+C0h] [ebp-38h] BYREF
  int id; // [esp+DCh] [ebp-1Ch] BYREF
  int id; // [esp+E0h] [ebp-18h] BYREF
  int id; // [esp+E4h] [ebp-14h] BYREF
  unsigned int rand_int; // [esp+E8h] [ebp-10h]
  int id; // [esp+ECh] [ebp-Ch]

  rand_int = get_rand_int(16);
  std::map<short,CBattle_Field::TournamentInfo>::map(v12);
  CBattle_Field::TournamentInfo::TournamentInfo((CBattle_Field::TournamentInfo *)v11);
  v11[0] = rand_int;
  std::make_pair<short &,CBattle_Field::TournamentInfo &>((int)v15, (int)v11, (int)v11);
  std::pair<short const,CBattle_Field::TournamentInfo>::pair<short,CBattle_Field::TournamentInfo>((int)v14, (int)v15);
  std::map<short,CBattle_Field::TournamentInfo>::insert((int)v13, (int)v12, (int)v14);
  v25 = -1;
  if ( (rand_int & 1) != 0 )
    v25 = rand_int - 1;
  else
    v25 = rand_int + 1;
  std::vector<CBattle_Field::TournamentInfo>::end(&v16, (int)a2);
  std::vector<CBattle_Field::TournamentInfo>::begin(&v17, a2);
  std::random_shuffle<__gnu_cxx::__normal_iterator<CBattle_Field::TournamentInfo *,std::vector<CBattle_Field::TournamentInfo>>>(
    v17,
    v16);
  std::vector<CBattle_Field::TournamentInfo>::begin(&v10, a2);
  v5 = (_WORD *)__gnu_cxx::__normal_iterator<CBattle_Field::TournamentInfo *,std::vector<CBattle_Field::TournamentInfo>>::operator->((int)&v10);
  *v5 = v25;
  v6 = __gnu_cxx::__normal_iterator<CBattle_Field::TournamentInfo *,std::vector<CBattle_Field::TournamentInfo>>::operator*((int)&v10);
  v7 = __gnu_cxx::__normal_iterator<CBattle_Field::TournamentInfo *,std::vector<CBattle_Field::TournamentInfo>>::operator->((int)&v10);
  std::make_pair<short &,CBattle_Field::TournamentInfo &>((int)v20, v7, v6);
  std::pair<short const,CBattle_Field::TournamentInfo>::pair<short,CBattle_Field::TournamentInfo>((int)v19, (int)v20);
  std::map<short,CBattle_Field::TournamentInfo>::insert((int)v18, (int)v12, (int)v19);
  std::vector<CBattle_Field::TournamentInfo>::erase(&v21, (int)a2, v10);
  std::vector<int>::vector(v9);
  for ( i = 0; i <= 15; ++i )
  {
    if ( i != rand_int && i != v25 )
      std::vector<int>::push_back((int)v9, &i);
  }
  std::vector<int>::end(&v22, (int)v9);
  std::vector<int>::begin(&v23, v9);
  std::random_shuffle<__gnu_cxx::__normal_iterator<int *,std::vector<int>>>(v23, v22);
  CBattle_Field::InsertTournamentInfo(a1, v9, a2, (int)v12);
  CBattle_Field::InsertTournamentInfo(a1, v9, a3, (int)v12);
  CBattle_Field::MakeRoundInfo(a1, (int)v12, a4, a5);
  std::vector<int>::~vector(v9);
  std::map<short,CBattle_Field::TournamentInfo>::~map((int)v12);
}

_BOOL4 __cdecl CBattle_Field::InsertTournamentInfo(int id, _DWORD *a2, _DWORD *a3, int value)
{
  _WORD *v4; // ebx
  int id; // ebx
  int id; // eax
  _BOOL4 result; // eax
  int id; // [esp+1Ch] [ebp-5Ch] BYREF
  int id; // [esp+20h] [ebp-58h] BYREF
  int id; // [esp+24h] [ebp-54h] BYREF
  int id; // [esp+28h] [ebp-50h] BYREF
  int id; // [esp+2Ch] [ebp-4Ch] BYREF
  _BYTE v13[8]; // [esp+30h] [ebp-48h] BYREF
  _BYTE v14[28]; // [esp+38h] [ebp-40h] BYREF
  _BYTE v15[32]; // [esp+54h] [ebp-24h] BYREF

  std::vector<CBattle_Field::TournamentInfo>::begin(&v10, a3);
  std::vector<int>::begin(&v9, a2);
  while ( 1 )
  {
    std::vector<CBattle_Field::TournamentInfo>::end(&v11, (int)a3);
    result = 0;
    if ( __gnu_cxx::operator!=<CBattle_Field::TournamentInfo *,std::vector<CBattle_Field::TournamentInfo>>(
           (int)&v10,
           (int)&v11) )
    {
      std::vector<int>::end(&v12, (int)a2);
      if ( __gnu_cxx::operator!=<int *,std::vector<int>>((int)&v9, (int)&v12) )
        result = 1;
    }
    if ( !result )
      break;
    v4 = (_WORD *)__gnu_cxx::__normal_iterator<CBattle_Field::TournamentInfo *,std::vector<CBattle_Field::TournamentInfo>>::operator->((int)&v10);
    *v4 = *(_DWORD *)__gnu_cxx::__normal_iterator<int *,std::vector<int>>::operator*((int)&v9);
    v5 = __gnu_cxx::__normal_iterator<CBattle_Field::TournamentInfo *,std::vector<CBattle_Field::TournamentInfo>>::operator*((int)&v10);
    v6 = __gnu_cxx::__normal_iterator<CBattle_Field::TournamentInfo *,std::vector<CBattle_Field::TournamentInfo>>::operator->((int)&v10);
    std::make_pair<short &,CBattle_Field::TournamentInfo &>((int)v15, v6, v5);
    std::pair<short const,CBattle_Field::TournamentInfo>::pair<short,CBattle_Field::TournamentInfo>((int)v14, (int)v15);
    std::map<short,CBattle_Field::TournamentInfo>::insert((int)v13, a4, (int)v14);
    std::vector<CBattle_Field::TournamentInfo>::erase(&v8, (int)a3, v10);
    v10 = v8;
    std::vector<int>::erase(&v8, (int)a2, v9);
    v9 = v8;
  }
  return result;
}

bool __cdecl CBattle_Field::MakeRoundInfo(int count, int count2, int count3, int *a4)
{
  int count; // eax
  void *v6[3]; // [esp+1Ch] [ebp-4Ch] BYREF
  std::_Rb_tree_node_base *v7; // [esp+28h] [ebp-40h] BYREF
  int i; // [esp+2Ch] [ebp-3Ch] BYREF
  int count; // [esp+30h] [ebp-38h] BYREF
  _BYTE dest[4]; // [esp+34h] [ebp-34h] BYREF
  _BYTE v11[8]; // [esp+38h] [ebp-30h] BYREF
  _BYTE v12[16]; // [esp+40h] [ebp-28h] BYREF
  _DWORD value[6]; // [esp+50h] [ebp-18h] BYREF

  for ( i = 1; i <= 4; ++i )
  {
    std::map<short,CBattle_Field::TournamentInfo>::begin(&v7, a2);
    std::vector<CBattle_Field::TournamentInfo>::vector(v6);
    while ( 1 )
    {
      std::map<short,CBattle_Field::TournamentInfo>::end(&v9, a2);
      if ( !std::_Rb_tree_iterator<std::pair<short const,CBattle_Field::TournamentInfo>>::operator!=(&v7, &v9) )
        break;
      v4 = std::_Rb_tree_iterator<std::pair<short const,CBattle_Field::TournamentInfo>>::operator->(&v7);
      std::vector<CBattle_Field::TournamentInfo>::push_back((int)v6, (_DWORD *)(v4 + 4));
      std::_Rb_tree_iterator<std::pair<short const,CBattle_Field::TournamentInfo>>::operator++(dest, &v7);
    }
    std::make_pair<int &,std::vector<CBattle_Field::TournamentInfo> &>(v13, (int)&i, (int)v6);
    std::pair<short const,std::vector<CBattle_Field::TournamentInfo>>::pair<int,std::vector<CBattle_Field::TournamentInfo>>(
      (int)v12,
      (int)v13);
    std::map<short,std::vector<CBattle_Field::TournamentInfo>>::insert((int)v11, a3, (int)v12);
    std::pair<short const,std::vector<CBattle_Field::TournamentInfo>>::~pair((int)v12);
    std::pair<int,std::vector<CBattle_Field::TournamentInfo>>::~pair((int)v13);
    CBattle_Field::MakeNextRoundInfo(a1, i, a2);
    std::vector<CBattle_Field::TournamentInfo>::~vector(v6);
  }
  return CBattle_Field::MakeTournamentMonsterList(a1, a3, a4);
}

void __cdecl CBattle_Field::MakeNextRoundInfo(int count, int count2, int count3)
{
  _WORD *v3; // eax
  int count; // ebx
  int count; // eax
  _WORD *v6; // eax
  int count; // ebx
  int count; // eax
  _WORD *v9; // eax
  int count; // ebx
  int count; // eax
  _WORD *v12; // eax
  int count; // ebx
  int count; // eax
  _DWORD value[6]; // [esp+18h] [ebp-160h] BYREF
  __int16 v16; // [esp+30h] [ebp-148h] BYREF
  __int16 v17; // [esp+32h] [ebp-146h] BYREF
  __int16 v18; // [esp+34h] [ebp-144h] BYREF
  __int16 v19; // [esp+36h] [ebp-142h] BYREF
  _BYTE v20[8]; // [esp+38h] [ebp-140h] BYREF
  _BYTE v21[28]; // [esp+40h] [ebp-138h] BYREF
  _BYTE v22[28]; // [esp+5Ch] [ebp-11Ch] BYREF
  __int16 v23; // [esp+78h] [ebp-100h] BYREF
  __int16 v24; // [esp+7Ah] [ebp-FEh] BYREF
  __int16 v25; // [esp+7Ch] [ebp-FCh] BYREF
  __int16 v26; // [esp+7Eh] [ebp-FAh] BYREF
  _BYTE v27[8]; // [esp+80h] [ebp-F8h] BYREF
  _BYTE v28[28]; // [esp+88h] [ebp-F0h] BYREF
  _BYTE v29[30]; // [esp+A4h] [ebp-D4h] BYREF
  __int16 v30; // [esp+C2h] [ebp-B6h] BYREF
  __int16 v31; // [esp+C4h] [ebp-B4h] BYREF
  __int16 v32; // [esp+C6h] [ebp-B2h] BYREF
  _BYTE v33[8]; // [esp+C8h] [ebp-B0h] BYREF
  _BYTE v34[28]; // [esp+D0h] [ebp-A8h] BYREF
  _BYTE v35[30]; // [esp+ECh] [ebp-8Ch] BYREF
  __int16 v36; // [esp+10Ah] [ebp-6Eh] BYREF
  __int16 v37; // [esp+10Ch] [ebp-6Ch] BYREF
  __int16 v38; // [esp+10Eh] [ebp-6Ah] BYREF
  _BYTE v39[8]; // [esp+110h] [ebp-68h] BYREF
  _BYTE v40[28]; // [esp+118h] [ebp-60h] BYREF
  _BYTE v41[28]; // [esp+134h] [ebp-44h] BYREF
  __int16 v42; // [esp+150h] [ebp-28h] BYREF
  __int16 v43; // [esp+152h] [ebp-26h] BYREF
  int i; // [esp+154h] [ebp-24h]
  int count; // [esp+158h] [ebp-20h]
  int count; // [esp+15Ch] [ebp-1Ch]
  int count; // [esp+160h] [ebp-18h]
  int count; // [esp+164h] [ebp-14h]
  signed int rand_int; // [esp+168h] [ebp-10h]
  signed int count; // [esp+16Ch] [ebp-Ch]

  std::map<short,CBattle_Field::TournamentInfo>::map(v15);
  for ( i = 0; 16 / a2 - 1 > i; i += 2 )
  {
    v45 = i;
    v46 = i + 1;
    v16 = i;
    v47 = *(_DWORD *)(std::map<short,CBattle_Field::TournamentInfo>::operator[](a3, &v16) + 20);
    v17 = v46;
    v48 = *(_DWORD *)(std::map<short,CBattle_Field::TournamentInfo>::operator[](a3, &v17) + 20);
    v18 = v45;
    if ( *(_DWORD *)(std::map<short,CBattle_Field::TournamentInfo>::operator[](a3, &v18) + 4) )
    {
      v25 = v46;
      if ( *(_DWORD *)(std::map<short,CBattle_Field::TournamentInfo>::operator[](a3, &v25) + 4) )
      {
        rand_int = 0;
        v50 = 0;
        rand_int = get_rand_int(v47);
        v50 = get_rand_int(v48);
        if ( rand_int <= v50 )
        {
          v38 = v46;
          v12 = (_WORD *)std::map<short,CBattle_Field::TournamentInfo>::operator[](a3, &v38);
          *v12 = v46 / 2;
          v42 = v46;
          v13 = std::map<short,CBattle_Field::TournamentInfo>::operator[](a3, &v42);
          v43 = v46;
          v14 = std::map<short,CBattle_Field::TournamentInfo>::operator[](a3, &v43);
          std::make_pair<short &,CBattle_Field::TournamentInfo &>((int)v41, v14, v13);
          std::pair<short const,CBattle_Field::TournamentInfo>::pair<short,CBattle_Field::TournamentInfo>(
            (int)v40,
            (int)v41);
          std::map<short,CBattle_Field::TournamentInfo>::insert((int)v39, (int)v15, (int)v40);
        }
        else
        {
          v32 = v45;
          v9 = (_WORD *)std::map<short,CBattle_Field::TournamentInfo>::operator[](a3, &v32);
          *v9 = v45 / 2;
          v36 = v45;
          v10 = std::map<short,CBattle_Field::TournamentInfo>::operator[](a3, &v36);
          v37 = v45;
          v11 = std::map<short,CBattle_Field::TournamentInfo>::operator[](a3, &v37);
          std::make_pair<short &,CBattle_Field::TournamentInfo &>((int)v35, v11, v10);
          std::pair<short const,CBattle_Field::TournamentInfo>::pair<short,CBattle_Field::TournamentInfo>(
            (int)v34,
            (int)v35);
          std::map<short,CBattle_Field::TournamentInfo>::insert((int)v33, (int)v15, (int)v34);
        }
      }
      else
      {
        v26 = v46;
        v6 = (_WORD *)std::map<short,CBattle_Field::TournamentInfo>::operator[](a3, &v26);
        *v6 = v46 / 2;
        v30 = v46;
        v7 = std::map<short,CBattle_Field::TournamentInfo>::operator[](a3, &v30);
        v31 = v46;
        v8 = std::map<short,CBattle_Field::TournamentInfo>::operator[](a3, &v31);
        std::make_pair<short &,CBattle_Field::TournamentInfo &>((int)v29, v8, v7);
        std::pair<short const,CBattle_Field::TournamentInfo>::pair<short,CBattle_Field::TournamentInfo>(
          (int)v28,
          (int)v29);
        std::map<short,CBattle_Field::TournamentInfo>::insert((int)v27, (int)v15, (int)v28);
      }
    }
    else
    {
      v19 = v45;
      v3 = (_WORD *)std::map<short,CBattle_Field::TournamentInfo>::operator[](a3, &v19);
      *v3 = v45 / 2;
      v23 = v45;
      v4 = std::map<short,CBattle_Field::TournamentInfo>::operator[](a3, &v23);
      v24 = v45;
      v5 = std::map<short,CBattle_Field::TournamentInfo>::operator[](a3, &v24);
      std::make_pair<short &,CBattle_Field::TournamentInfo &>((int)v22, v5, v4);
      std::pair<short const,CBattle_Field::TournamentInfo>::pair<short,CBattle_Field::TournamentInfo>(
        (int)v21,
        (int)v22);
      std::map<short,CBattle_Field::TournamentInfo>::insert((int)v20, (int)v15, (int)v21);
    }
  }
  std::map<short,CBattle_Field::TournamentInfo>::operator=(a3, (int)v15);
  std::map<short,CBattle_Field::TournamentInfo>::~map((int)v15);
}

bool __cdecl CBattle_Field::MakeTournamentMonsterList(int id, int id2, int *a3)
{
  int id; // eax
  int id; // eax
  int id; // eax
  bool result; // al
  _DWORD *v7; // [esp+1Ch] [ebp-2Ch] BYREF
  __int16 v8; // [esp+22h] [ebp-26h] BYREF
  int id; // [esp+24h] [ebp-24h] BYREF
  _BYTE dest[4]; // [esp+28h] [ebp-20h] BYREF
  int id; // [esp+2Ch] [ebp-1Ch]
  int i; // [esp+30h] [ebp-18h]
  _DWORD *v13; // [esp+34h] [ebp-14h]
  int id; // [esp+38h] [ebp-10h]
  int j; // [esp+3Ch] [ebp-Ch]

  for ( i = 1; i <= 4; ++i )
  {
    v8 = i;
    v13 = (_DWORD *)std::map<short,std::vector<CBattle_Field::TournamentInfo>>::operator[](a2, &v8);
    v14 = -1;
    for ( j = 0; j <= 15; ++j )
    {
      if ( !*(_DWORD *)(std::vector<CBattle_Field::TournamentInfo>::operator[](v13, j) + 4) )
      {
        if ( (j & 1) != 0 )
          v14 = j - 1;
        else
          v14 = j + 1;
        break;
      }
    }
    if ( i == 4 )
    {
      v3 = std::vector<CBattle_Field::TournamentInfo>::operator[](v13, v14);
      CBattle_Field::AddTournamentMonster(a1, v3, (int)a3, 1);
    }
    else
    {
      v4 = std::vector<CBattle_Field::TournamentInfo>::operator[](v13, v14);
      CBattle_Field::AddTournamentMonster(a1, v4, (int)a3, 0);
    }
  }
  std::list<_mapMonster>::begin(&v7, a3);
  v11 = 0;
  while ( 1 )
  {
    std::list<_mapMonster>::end(&v9, (int)a3);
    result = std::_List_iterator<_mapMonster>::operator!=(&v7, &v9);
    if ( !result )
      break;
    v5 = std::_List_iterator<_mapMonster>::operator->(&v7);
    *(_DWORD *)(v5 + 44) = v11++;
    std::_List_iterator<_mapMonster>::operator++(dest, &v7);
  }
  return result;
}

bool __cdecl CBattle_Field::AddTournamentMonster(int id, int value, int count, char flag)
{
  bool result; // al
  void *v5; // [esp+2Ch] [ebp-4Ch] BYREF
  char flag; // [esp+30h] [ebp-48h]
  int id; // [esp+34h] [ebp-44h]
  int id; // [esp+38h] [ebp-40h]
  int id; // [esp+3Ch] [ebp-3Ch]
  int id; // [esp+40h] [ebp-38h]
  int id; // [esp+44h] [ebp-34h]
  char flag; // [esp+48h] [ebp-30h]
  int id; // [esp+4Ch] [ebp-2Ch]
  int id; // [esp+54h] [ebp-24h]
  int id; // [esp+58h] [ebp-20h]
  char flag; // [esp+5Ch] [ebp-1Ch]
  int i; // [esp+6Ch] [ebp-Ch]

  for ( i = 0; ; ++i )
  {
    result = i <= 1;
    if ( i > 1 )
      break;
    _mapMonster::_mapMonster((_mapMonster *)&v5);
    if ( *(_DWORD *)(a2 + 8 * i + 4) )
    {
      v5 = *(void **)(a2 + 8 * i + 4);
      v6 = 1;
      v7 = 0;
      v9 = 1;
      v10 = 1;
      v8 = 0;
      if ( a4 )
        v11 = 3;
      else
        v11 = 0;
      v12 = 0;
      v13 = 0;
      v15 = 0;
      v14 = 100;
      v16 = 0;
      std::list<_mapMonster>::push_back(a3, (int)&v5);
    }
    _mapMonster::~_mapMonster(&v5);
  }
  return result;
}

int __cdecl CBattle_Field::add_blood_monster(CMap **a1, map_monster *a2, int *a3, int *a4, _DWORD *a5, _DWORD *a6)
{
  int id; // eax
  int id; // eax
  int id; // eax
  int id; // eax
  int CurrentMapInfo; // eax
  int id; // ebx
  CMonsterManager **v14; // eax
  int id; // [esp+2Ch] [ebp-39Ch] BYREF
  int id; // [esp+30h] [ebp-398h] BYREF
  __int16 v17; // [esp+36h] [ebp-392h] BYREF
  void *v18[211]; // [esp+38h] [ebp-390h] BYREF
  __int16 v19; // [esp+384h] [ebp-44h]
  __int16 v20; // [esp+386h] [ebp-42h]
  int id; // [esp+388h] [ebp-40h]
  __int16 v22; // [esp+38Eh] [ebp-3Ah]
  int id; // [esp+390h] [ebp-38h]
  int id; // [esp+394h] [ebp-34h]
  unsigned int id; // [esp+398h] [ebp-30h]
  unsigned int id; // [esp+39Ch] [ebp-2Ch]
  int id; // [esp+3A0h] [ebp-28h]
  __int16 rand_int; // [esp+3A4h] [ebp-24h]
  char flag; // [esp+3A7h] [ebp-21h]
  int id; // [esp+3A8h] [ebp-20h]
  int id; // [esp+3ACh] [ebp-1Ch]
  int i; // [esp+3B4h] [ebp-14h]
  signed int id; // [esp+3B8h] [ebp-10h]
  int j; // [esp+3BCh] [ebp-Ch]

  if ( std::vector<STAssignBloodPhaseTime>::empty(a6) || std::vector<STAssignBloodSystem>::empty(a5) )
    return 0;
  v19 = -1;
  v20 = 0;
  v21 = 0;
  v22 = 0;
  v23 = 0;
  v24 = 0;
  v25 = 0;
  v26 = 0;
  v27 = std::vector<STAssignBloodSystem>::size(a5);
  rand_int = 0;
  if ( !v27 )
    return 0;
  v29 = 19;
  if ( a1[66] )
  {
    if ( CMap::get_map_type(a1[66]) == 1 )
      rand_int = get_rand_int(4);
    else
      rand_int = get_rand_int(5);
  }
  if ( *((_BYTE *)a1[98] + 2207) == 2 )
    rand_int = -1;
  std::vector<STAssignBloodPhaseTime>::begin(&v15, a6);
  while ( 1 )
  {
    std::vector<STAssignBloodPhaseTime>::end(&v16, (int)a6);
    if ( !__gnu_cxx::operator!=<STAssignBloodPhaseTime *,std::vector<STAssignBloodPhaseTime>>((int)&v15, (int)&v16) )
      break;
    v23 = *(_DWORD *)__gnu_cxx::__normal_iterator<STAssignBloodPhaseTime *,std::vector<STAssignBloodPhaseTime>>::operator->((int)&v15);
    v25 = *(_DWORD *)(__gnu_cxx::__normal_iterator<STAssignBloodPhaseTime *,std::vector<STAssignBloodPhaseTime>>::operator->((int)&v15)
                    + 4);
    if ( *(_DWORD *)(__gnu_cxx::__normal_iterator<STAssignBloodPhaseTime *,std::vector<STAssignBloodPhaseTime>>::operator->((int)&v15)
                   + 4) == -1 )
    {
      v25 = get_rand_int(v27 - 1);
      v26 = v25;
      v17 = v25;
      std::vector<short>::push_back((int)(a1 + 150), (int)&v17);
    }
    v30 = 0;
    v31 = 0;
    v24 = *(_DWORD *)(std::vector<STAssignBloodSystem>::operator[](a5, v25) + 28);
    for ( i = 0; ; ++i )
    {
      v9 = std::vector<STAssignBloodSystem>::operator[](a5, v25);
      if ( *(_DWORD *)(v9 + 36) <= i )
        break;
      v8 = std::vector<STAssignBloodSystem>::operator[](a5, v25);
      v30 = v24 + i * *(_DWORD *)(v8 + 32);
      v31 += v30;
    }
    v24 = v31;
    std::vector<bool>::push_back(a1 + 153, 0);
    if ( v19 != v23 )
    {
      if ( rand_int == v23 )
      {
        v34 = get_rand_int(100);
        if ( v34 > 39 )
        {
          if ( v34 > 69 )
          {
            if ( v34 > 84 )
            {
              if ( v34 > 94 )
                v29 = 18;
              else
                v29 = get_rand_int(3) + 15;
            }
            else
            {
              v29 = get_rand_int(4) + 11;
            }
          }
          else
          {
            v29 = get_rand_int(6) + 5;
          }
        }
        else
        {
          v29 = get_rand_int(5);
        }
        *((_WORD *)a1 + 340) = v22;
        a1[169] = *(CMap **)std::vector<STAssignBloodSystem>::operator[](a5, v25);
        *((_BYTE *)a1 + 682) = v29;
      }
      v19 = v23;
    }
    for ( j = 0; j < v24; ++j )
    {
      v10 = *a4;
      *(_DWORD *)a2 = *a4;
      *a4 = v10 + 1;
      *((_DWORD *)a2 + 3) = *(_DWORD *)std::vector<STAssignBloodSystem>::operator[](a5, v25);
      if ( *((_BYTE *)a1[98] + 2207) == 2 )
        *((_BYTE *)a2 + 16) = 70;
      else
        *((_BYTE *)a2 + 16) = 60;
      *((_BYTE *)a2 + 8) = 0;
      *((_BYTE *)a2 + 837) = v29;
      v11 = *a3;
      *((_DWORD *)a2 + 1) = *a3;
      *a3 = v11 + 1;
      *((_DWORD *)a2 + 11) = 100;
      *((_BYTE *)a2 + 17) = 0;
      *((_BYTE *)a2 + 18) = 0;
      *((_BYTE *)a2 + 19) = 0;
      *((_BYTE *)a2 + 20) = 0;
      *((_BYTE *)a2 + 52) = 0;
      *((_WORD *)a2 + 413) = v22;
      *((_WORD *)a2 + 414) = (__int16)(*(float *)(__gnu_cxx::__normal_iterator<STAssignBloodPhaseTime *,std::vector<STAssignBloodPhaseTime>>::operator->((int)&v15)
                                                + 12)
                                     * STATIC_POOL_SIZE_1000.0);
      *((_WORD *)a2 + 415) = *(_DWORD *)(std::vector<STAssignBloodSystem>::operator[](a5, v25) + 8);
      *((_WORD *)a2 + 416) = *(_DWORD *)(std::vector<STAssignBloodSystem>::operator[](a5, v25) + 12);
      *((_WORD *)a2 + 417) = *(_DWORD *)(std::vector<STAssignBloodSystem>::operator[](a5, v25) + 16);
      *((_BYTE *)a2 + 836) = 0;
      *((_WORD *)a2 + 419) = *(_DWORD *)__gnu_cxx::__normal_iterator<STAssignBloodPhaseTime *,std::vector<STAssignBloodPhaseTime>>::operator->((int)&v15);
      *((_BYTE *)a2 + 840) = *(_BYTE *)(__gnu_cxx::__normal_iterator<STAssignBloodPhaseTime *,std::vector<STAssignBloodPhaseTime>>::operator->((int)&v15)
                                      + 24);
      map_monster::map_monster((map_monster *)v18, a2);
      CurrentMapInfo = CBattle_Field::GetCurrentMapInfo((CBattle_Field *)a1);
      MapInfo::Add_Mob(CurrentMapInfo, (int)v18);
      map_monster::~map_monster(v18);
      v29 = 19;
    }
    v13 = *((_DWORD *)a2 + 3);
    v14 = (CMonsterManager **)G_CDataManager();
    if ( !CDataManager::find_monster(v14, v13) )
      return 0;
    __gnu_cxx::__normal_iterator<STAssignBloodPhaseTime *,std::vector<STAssignBloodPhaseTime>>::operator++(&v15);
    ++v22;
  }
  CBattle_Field::CBloodRound::setBloodLastSmallRoundOrder((CBattle_Field::CBloodRound *)(a1 + 171), v22 - 1);
  return 1;
}

void __cdecl CBattle_Field::BloodSpawnTimerManager(CBattle_Field *this, bool a2, bool a3)
{
  __int16 BloodRoundOrder; // [esp+34h] [ebp-14h]
  __int16 BloodSmallRoundOrder; // [esp+36h] [ebp-12h]
  int blood_spawn_small_round_interval; // [esp+3Ch] [ebp-Ch]

  BloodRoundOrder = CBattle_Field::CBloodRound::getBloodRoundOrder((CBattle_Field *)((char *)this + 684));
  BloodSmallRoundOrder = CBattle_Field::CBloodRound::getBloodSmallRoundOrder((CBattle_Field *)((char *)this + 684));
  if ( BloodSmallRoundOrder <= (__int16)CBattle_Field::CBloodRound::getBloodLastSmallRoundOrder((CBattle_Field *)((char *)this + 684)) )
  {
    if ( (unsigned __int8)CBattle_Field::checkBloodSpawnOrder(this, BloodSmallRoundOrder) != 1 )
    {
      LogManager::logFormat(
        (LogManager *)1,
        (int)"battle_field.cpp",
        "void CBattle_Field::BloodSpawnTimerManager(bool, bool)",
        (const char *)0xD0C,
        "BLOOD_LOG : round_order_ (%d), small_round_order_ (%d) : SET ORDER ERROR!!\n",
        (const char *)BloodRoundOrder,
        BloodSmallRoundOrder);
    }
    else
    {
      blood_spawn_small_round_interval = CBattle_Field::get_blood_spawn_small_round_interval(this, BloodSmallRoundOrder);
      if ( blood_spawn_small_round_interval > 0 )
      {
        if ( a2 && !a3 || !BloodSmallRoundOrder )
        {
          CBattle_Field::CBloodClearRewardData::onStartBloodRound((CBattle_Field::CBloodClearRewardData *)(*(_DWORD *)this + 3548));
          CBattle_Field::BloodRoundIntervalPacket((CUser ***)this, BloodRoundOrder, blood_spawn_small_round_interval);
        }
        if ( CBattle_Field::BloodPhaseSpawnTimerResgister(this, BloodSmallRoundOrder) == -1 )
          LogManager::logFormat(
            (LogManager *)1,
            (int)"battle_field.cpp",
            "void CBattle_Field::BloodSpawnTimerManager(bool, bool)",
            (const char *)0xD2B,
            "BLOOD_LOG : small_round_order_ (%d) : ABNOMAL ORDER!!\n",
            (const char *)BloodSmallRoundOrder);
      }
      else
      {
        LogManager::logFormat(
          (LogManager *)1,
          (int)"battle_field.cpp",
          "void CBattle_Field::BloodSpawnTimerManager(bool, bool)",
          (const char *)0xD19,
          "BLOOD_LOG : small_round_order_ (%d) : ABNOMAL ORDER!!\n",
          (const char *)BloodSmallRoundOrder);
      }
    }
  }
}

void __cdecl CBattle_Field::checkKilledSpawnBloodMonsters(CBattle_Field *this, int id)
{
  int CurrentMapInfo; // eax
  int id; // eax
  int NextRoundDifficulty; // eax
  int id; // ebx
  int id; // esi
  int id; // ebx
  TimerQueue *v9; // eax
  int id; // esi
  int PartyIndex; // ebx
  TimerQueue *v12; // eax
  int id; // eax
  void *v14[211]; // [esp+2Ch] [ebp-37Ch] BYREF
  std::_Rb_tree_node_base::_Base_ptr v15; // [esp+378h] [ebp-30h] BYREF
  int id; // [esp+37Ch] [ebp-2Ch] BYREF
  __int16 BloodRoundOrder; // [esp+382h] [ebp-26h]
  __int16 BloodSmallRoundOrder; // [esp+384h] [ebp-24h]
  __int16 BloodLastSmallRoundOrder; // [esp+386h] [ebp-22h]
  int id; // [esp+388h] [ebp-20h]
  int blood_round_of_small_round; // [esp+38Ch] [ebp-1Ch]
  int id; // [esp+390h] [ebp-18h]
  int blood_round_difficylty_of_small_round; // [esp+394h] [ebp-14h]
  int id; // [esp+398h] [ebp-10h]
  bool v25; // [esp+39Fh] [ebp-9h]

  BloodRoundOrder = CBattle_Field::CBloodRound::getBloodRoundOrder((CBattle_Field *)((char *)this + 684));
  BloodSmallRoundOrder = CBattle_Field::CBloodRound::getBloodSmallRoundOrder((CBattle_Field *)((char *)this + 684));
  BloodLastSmallRoundOrder = CBattle_Field::CBloodRound::getBloodLastSmallRoundOrder((CBattle_Field *)((char *)this + 684));
  if ( BloodLastSmallRoundOrder >= BloodSmallRoundOrder )
  {
    v20 = 0;
    map_monster::map_monster((map_monster *)v14);
    CurrentMapInfo = CBattle_Field::GetCurrentMapInfo(this);
    std::map<int,map_monster>::begin(&v15, CurrentMapInfo + 12);
    while ( 1 )
    {
      v13 = CBattle_Field::GetCurrentMapInfo(this);
      std::map<int,map_monster>::end(&v16, v13 + 12);
      if ( !std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator!=(&v15, &v16) )
        break;
      v4 = std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator->(&v15);
      if ( *(__int16 *)(v4 + 830) < BloodSmallRoundOrder )
        break;
      blood_round_of_small_round = CBattle_Field::get_blood_round_of_small_round(this, BloodSmallRoundOrder);
      v22 = CBattle_Field::get_blood_round_of_small_round(this, BloodSmallRoundOrder - 1);
      blood_round_difficylty_of_small_round = CBattle_Field::get_blood_round_difficylty_of_small_round(
                                                this,
                                                BloodSmallRoundOrder);
      v24 = CBattle_Field::get_blood_round_difficylty_of_small_round(this, BloodSmallRoundOrder - 1);
      if ( blood_round_of_small_round < 0 || v22 < 0 )
      {
        LogManager::logFormat(
          (LogManager *)1,
          (int)"battle_field.cpp",
          "void CBattle_Field::checkKilledSpawnBloodMonsters(int)",
          (const char *)0xD59,
          "BLOOD_LOG : small_round_order_ (%d) : ABNOMAL ROUND!!\n",
          (const char *)BloodSmallRoundOrder);
        break;
      }
      v25 = 0;
      v25 = v22 != blood_round_of_small_round || blood_round_difficylty_of_small_round != v24;
      if ( v25 )
      {
        CBattle_Field::CBloodClearRewardData::onFinishBloodRound(
          (CBattle_Field::CBloodClearRewardData *)(*(_DWORD *)this + 3548),
          0,
          BloodRoundOrder,
          *(CUserCharacInfo ***)this,
          *((std::string **)this + 98));
        if ( *(_BYTE *)(*((_DWORD *)this + 98) + 2207) == 2 )
        {
          NextRoundDifficulty = CBattle_Field::getNextRoundDifficulty(this, BloodRoundOrder - 1);
          CBattle_Field::CBloodClearRewardData::incUltimateRoundClearPoint(
            (CBattle_Field *)((char *)this + 696),
            NextRoundDifficulty);
          if ( (unsigned __int8)CBattle_Field::forceValuesToChangeRound(this, BloodRoundOrder, BloodSmallRoundOrder, 1) == 1 )
          {
            CBattle_Field::setUltimateDifficulty(this, 0);
            v6 = BloodRoundOrder;
            if ( v6 == CBattle_Field::getBloodMaxRound(this) - 1 )
            {
              v10 = CParty::gen_timer_key(*(_DWORD *)this, 38);
              PartyIndex = CParty::GetPartyIndex(*(CParty **)this);
              v12 = G_TimerQueue();
              TimerQueue::InsertTimer(v10, v12, 1, PartyIndex, 38, 1, v10, 38);
            }
            else
            {
              v7 = CParty::gen_timer_key(*(_DWORD *)this, 38);
              v8 = CParty::GetPartyIndex(*(CParty **)this);
              v9 = G_TimerQueue();
              TimerQueue::InsertTimer(v7, v9, 1, v8, 38, 10, v7, 38);
              CBattle_Field::sendUltimateRandomDifficulty((CUser ***)this, 0, BloodRoundOrder, 10);
            }
          }
        }
        else
        {
          CBattle_Field::BloodSpawnTimerManager(this, 1, 0);
        }
        break;
      }
      std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator++(&v15);
    }
    map_monster::~map_monster(v14);
  }
  else if ( !a2 && CMap::get_map_type(*((CMap **)this + 66)) != 1 )
  {
    CBattle_Field::CBloodClearRewardData::onFinishBloodRound(
      (CBattle_Field *)((char *)this + 696),
      0,
      BloodRoundOrder,
      *(CUserCharacInfo ***)this,
      *((std::string **)this + 98));
  }
}

int __cdecl CBattle_Field::BloodPhaseSpawnTimerResgister(CBattle_Field *this, __int16 a2)
{
  int PartyIndex; // edi
  TimerQueue *v4; // eax
  int id; // [esp+24h] [ebp-34h]
  __int16 v6; // [esp+2Ch] [ebp-2Ch]
  int blood_spawn_cnt_of_small_round; // [esp+34h] [ebp-24h]
  int i; // [esp+38h] [ebp-20h]
  int blood_spawn_small_round_interval; // [esp+3Ch] [ebp-1Ch]

  v6 = a2;
  blood_spawn_cnt_of_small_round = CBattle_Field::get_blood_spawn_cnt_of_small_round(this, a2);
  if ( (unsigned int)blood_spawn_cnt_of_small_round > 0xA )
    return -1;
  if ( !blood_spawn_cnt_of_small_round )
    blood_spawn_cnt_of_small_round = 1;
  for ( i = 0; i < blood_spawn_cnt_of_small_round; ++i )
  {
    blood_spawn_small_round_interval = CBattle_Field::get_blood_spawn_small_round_interval(this, v6);
    if ( blood_spawn_small_round_interval <= 0 )
      return -1;
    if ( *((_WORD *)this + 340) == v6 )
      CBattle_Field::BloodCrazyMonsterSpawnPacket(this);
    *((_WORD *)this + i + 316) = v6;
    *((_WORD *)this + i + 327) = 0;
    v5 = CParty::gen_timer_key(*(_DWORD *)this, i + 23);
    PartyIndex = CParty::GetPartyIndex(*(CParty **)this);
    v4 = G_TimerQueue();
    TimerQueue::InsertTimerInMilisecond(
      blood_spawn_small_round_interval >> 31,
      v4,
      1,
      PartyIndex,
      i + 23,
      blood_spawn_small_round_interval,
      v5,
      0);
    ++v6;
    CBattle_Field::CBloodRound::incBloodSmallRoundOrder((CBattle_Field *)((char *)this + 684));
  }
  return 0;
}

void __cdecl CBattle_Field::BloodPhaseSpawn(CMap **this, __int16 a2, __int16 *a3, int id, int id2)
{
  int id; // edi
  TimerQueue *v6; // eax
  int id; // ebx
  int id; // esi
  int PartyIndex; // edi
  TimerQueue *v10; // eax
  int id; // [esp+20h] [ebp-58h]
  int id; // [esp+28h] [ebp-50h]
  _BYTE v13[10]; // [esp+30h] [ebp-48h] BYREF
  __int16 blood_spawn_cnt_of_phase; // [esp+3Ah] [ebp-3Eh]
  int blood_spawn_time_of_phase; // [esp+3Ch] [ebp-3Ch]
  __int16 BloodRoundOrder; // [esp+42h] [ebp-36h]
  __int16 BloodSmallRoundOrder; // [esp+44h] [ebp-34h]
  __int16 BloodLastSmallRoundOrder; // [esp+46h] [ebp-32h]
  int blood_spawn_phase_total_cnt_of_small_round; // [esp+48h] [ebp-30h]
  int blood_round_of_small_round; // [esp+4Ch] [ebp-2Ch]
  int id; // [esp+50h] [ebp-28h]
  int blood_round_difficylty_of_small_round; // [esp+54h] [ebp-24h]
  int id; // [esp+58h] [ebp-20h]
  int blood_spawn_time_of_next_small_round; // [esp+5Ch] [ebp-1Ch]

  blood_spawn_cnt_of_phase = CBattle_Field::get_blood_spawn_cnt_of_phase((CBattle_Field *)this, a2, *a3);
  blood_spawn_time_of_phase = CBattle_Field::get_blood_spawn_time_of_phase((CBattle_Field *)this, a2);
  BloodRoundOrder = CBattle_Field::CBloodRound::getBloodRoundOrder((CBattle_Field::CBloodRound *)(this + 171));
  BloodSmallRoundOrder = CBattle_Field::CBloodRound::getBloodSmallRoundOrder((CBattle_Field::CBloodRound *)(this + 171));
  BloodLastSmallRoundOrder = CBattle_Field::CBloodRound::getBloodLastSmallRoundOrder((CBattle_Field::CBloodRound *)(this + 171));
  if ( blood_spawn_cnt_of_phase < 0 || blood_spawn_time_of_phase < 0 )
  {
    LogManager::logFormat(
      (LogManager *)1,
      (int)"battle_field.cpp",
      "void CBattle_Field::BloodPhaseSpawn(short int, short int&, int, int)",
      (const char *)0xDB8,
      "BLOOD_LOG : small_round_order_ (%d) : ABNOMAL ORDER!!\n",
      (const char *)a2);
    return;
  }
  if ( *(this + 66) && BloodLastSmallRoundOrder == a2 )
  {
    if ( CMap::get_map_type(*(this + 66)) == 1 )
      blood_spawn_time_of_phase = -1;
    else
      blood_spawn_time_of_phase = 0;
  }
  CBattle_Field::BloodMonsterSpawnPacket((CBattle_Field *)this, blood_spawn_cnt_of_phase, blood_spawn_time_of_phase, a2);
  blood_spawn_phase_total_cnt_of_small_round = CBattle_Field::get_blood_spawn_phase_total_cnt_of_small_round(
                                                 (CBattle_Field *)this,
                                                 a2);
  if ( blood_spawn_phase_total_cnt_of_small_round < 0 )
  {
    LogManager::logFormat(
      (LogManager *)1,
      (int)"battle_field.cpp",
      "void CBattle_Field::BloodPhaseSpawn(short int, short int&, int, int)",
      (const char *)0xDCC,
      "BLOOD_LOG : small_round_order_ (%d) : ABNOMAL ORDER!!\n",
      (const char *)a2);
    return;
  }
  if ( blood_spawn_phase_total_cnt_of_small_round - 1 <= *a3 )
  {
    *((_WORD *)this + a5 + 327) = 0;
    std::vector<bool>::operator[]((std::_Bit_reference *)v13, this + 153, a2);
    std::_Bit_reference::operator=((int)v13, 1);
    if ( (unsigned __int8)CBattle_Field::checkBloodSpawnOrder((CBattle_Field *)this, BloodSmallRoundOrder) )
    {
      if ( BloodLastSmallRoundOrder < BloodSmallRoundOrder )
      {
        CBattle_Field::CBloodRound::incBloodRoundOrder((CBattle_Field::CBloodRound *)(this + 171));
        CBattle_Field::CBloodRound::setPreMapBloodRoundOrder((CBattle_Field::CBloodRound *)(this + 171));
        return;
      }
      blood_round_of_small_round = CBattle_Field::get_blood_round_of_small_round(
                                     (CBattle_Field *)this,
                                     BloodSmallRoundOrder);
      v21 = CBattle_Field::get_blood_round_of_small_round((CBattle_Field *)this, BloodSmallRoundOrder - 1);
      blood_round_difficylty_of_small_round = CBattle_Field::get_blood_round_difficylty_of_small_round(
                                                (CBattle_Field *)this,
                                                BloodSmallRoundOrder);
      v23 = CBattle_Field::get_blood_round_difficylty_of_small_round((CBattle_Field *)this, BloodSmallRoundOrder - 1);
      if ( blood_round_of_small_round < 0 || v21 < 0 )
      {
        LogManager::logFormat(
          (LogManager *)1,
          (int)"battle_field.cpp",
          "void CBattle_Field::BloodPhaseSpawn(short int, short int&, int, int)",
          (const char *)0xDEE,
          "BLOOD_LOG : small_round_order_ (%d) : ABNOMAL ROUND!!\n",
          (const char *)BloodSmallRoundOrder);
        return;
      }
      blood_spawn_time_of_next_small_round = CBattle_Field::get_blood_spawn_time_of_next_small_round(
                                               (CBattle_Field *)this,
                                               a2);
      if ( blood_spawn_time_of_next_small_round <= 0 )
      {
        LogManager::logFormat(
          (LogManager *)1,
          (int)"battle_field.cpp",
          "void CBattle_Field::BloodPhaseSpawn(short int, short int&, int, int)",
          (const char *)0xDF5,
          "BLOOD_LOG : small_round_order_ (%d) : ABNOMAL ORDER!!\n",
          (const char *)BloodSmallRoundOrder);
        return;
      }
      if ( v21 == blood_round_of_small_round && blood_round_difficylty_of_small_round == v23 )
      {
        v12 = CParty::gen_timer_key((int)*this, 22);
        v7 = blood_spawn_time_of_next_small_round;
        v8 = blood_spawn_time_of_next_small_round >> 31;
        PartyIndex = CParty::GetPartyIndex(*this);
        v10 = G_TimerQueue();
        TimerQueue::InsertTimerInMilisecond(v8, v10, 1, PartyIndex, 22, __SPAIR64__(v8, v7), v12, 0);
      }
      else
      {
        CBattle_Field::CBloodRound::incBloodRoundOrder((CBattle_Field::CBloodRound *)(this + 171));
        CBattle_Field::CBloodRound::setPreMapBloodRoundOrder((CBattle_Field::CBloodRound *)(this + 171));
      }
    }
  }
  else
  {
    v11 = CParty::gen_timer_key((int)*this, a4);
    v5 = CParty::GetPartyIndex(*this);
    v6 = G_TimerQueue();
    TimerQueue::InsertTimerInMilisecond(
      blood_spawn_time_of_phase >> 31,
      v6,
      1,
      v5,
      a4,
      blood_spawn_time_of_phase,
      v11,
      0);
  }
  ++*a3;
}

void __cdecl CBattle_Field::BloodMonsterSpawnPacket(CBattle_Field *this, int id, int value, __int16 a4)
{
  int CurrentMapInfo; // eax
  int id; // eax
  int id; // eax
  int id; // eax
  int id; // eax
  int id; // eax
  int id; // eax
  int id; // eax
  int id; // eax
  const char *v14; // [esp+14h] [ebp-394h]
  void *v15[211]; // [esp+38h] [ebp-370h] BYREF
  std::_Rb_tree_node_base *v16; // [esp+384h] [ebp-24h] BYREF
  PacketBuf *v17[3]; // [esp+388h] [ebp-20h] BYREF
  int id; // [esp+394h] [ebp-14h] BYREF
  _BYTE dest[4]; // [esp+398h] [ebp-10h] BYREF
  int id; // [esp+39Ch] [ebp-Ch]

  v20 = 0;
  PacketGuard::PacketGuard((PacketGuard *)v17);
  InterfacePacketBuf::put_header(v17, 0, 210);
  InterfacePacketBuf::put_short(v17, a2);
  map_monster::map_monster((map_monster *)v15);
  CurrentMapInfo = CBattle_Field::GetCurrentMapInfo(this);
  std::map<int,map_monster>::begin(&v16, CurrentMapInfo + 12);
  while ( 1 )
  {
    v13 = CBattle_Field::GetCurrentMapInfo(this);
    std::map<int,map_monster>::end(&v18, v13 + 12);
    if ( !std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator!=(&v16, &v18) || v20 == a2 )
      break;
    if ( *(_WORD *)(std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator->(&v16) + 830) == a4
      && *(_BYTE *)(std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator->(&v16) + 840) != 1 )
    {
      v6 = std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator->(&v16);
      InterfacePacketBuf::put_byte(v17, *(_BYTE *)(v6 + 841));
      v7 = std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator->(&v16);
      InterfacePacketBuf::put_short(v17, *(_DWORD *)(v7 + 8));
      v8 = std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator->(&v16);
      InterfacePacketBuf::put_int(v17, *(_DWORD *)(v8 + 16));
      if ( a3 == -1 )
        InterfacePacketBuf::put_byte(v17, 3);
      else
        InterfacePacketBuf::put_byte(v17, 0);
      if ( *(_BYTE *)(*((_DWORD *)this + 98) + 2207) == 2 )
        InterfacePacketBuf::put_byte(v17, 70);
      else
        InterfacePacketBuf::put_byte(v17, 60);
      v9 = std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator->(&v16);
      InterfacePacketBuf::put_short(v17, *(_WORD *)(v9 + 832));
      v10 = std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator->(&v16);
      InterfacePacketBuf::put_short(v17, *(_WORD *)(v10 + 834));
      v11 = std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator->(&v16);
      InterfacePacketBuf::put_short(v17, *(_WORD *)(v11 + 836));
      v12 = std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator->(&v16);
      InterfacePacketBuf::put_short(v17, *(_WORD *)(v12 + 838));
      *(_BYTE *)(std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator->(&v16) + 840) = 1;
      ++v20;
    }
    std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator++(dest, &v16);
  }
  if ( v20 == a2 )
  {
    InterfacePacketBuf::put_short(v17, a3);
    InterfacePacketBuf::finalize(v17, 1);
    CParty::send_to_party(*(CUser ***)this, (PacketGuard *)v17);
  }
  else
  {
    LogManager::logFormat(
      (LogManager *)1,
      (int)"battle_field.cpp",
      "void CBattle_Field::BloodMonsterSpawnPacket(int, int, short int, short int)",
      (const char *)0xE43,
      "BLOOD_LOG : EROOR BLOOD SPAWN MONSTER COUNT!!\n",
      v14);
  }
  map_monster::~map_monster(v15);
  PacketGuard::~PacketGuard(v17);
}

void __cdecl CBattle_Field::BloodCrazyMonsterSpawnPacket(CBattle_Field *this)
{
  PacketBuf *v1[5]; // [esp+14h] [ebp-14h] BYREF

  PacketGuard::PacketGuard((PacketGuard *)v1);
  InterfacePacketBuf::put_header(v1, 0, 213);
  InterfacePacketBuf::put_int(v1, *((_DWORD *)this + 169));
  InterfacePacketBuf::put_byte(v1, *((_BYTE *)this + 682));
  InterfacePacketBuf::finalize(v1, 1);
  CParty::send_to_party(*(CUser ***)this, (PacketGuard *)v1);
  PacketGuard::~PacketGuard(v1);
}

void __cdecl CBattle_Field::BloodRoundMaxTimePacket(CUser ***this, __int16 a2, int id, int value)
{
  PacketBuf *v4[5]; // [esp+24h] [ebp-14h] BYREF

  PacketGuard::PacketGuard((PacketGuard *)v4);
  InterfacePacketBuf::put_header(v4, 0, 214);
  InterfacePacketBuf::put_byte(v4, 0);
  InterfacePacketBuf::put_byte(v4, a2 + 1);
  InterfacePacketBuf::put_int(v4, a3);
  InterfacePacketBuf::put_int(v4, a4);
  InterfacePacketBuf::finalize(v4, 1);
  CParty::send_to_party(*this, (PacketGuard *)v4);
  PacketGuard::~PacketGuard(v4);
}

void __cdecl CBattle_Field::BloodRoundIntervalPacket(CUser ***this, __int16 a2, int id)
{
  PacketBuf *v3[5]; // [esp+24h] [ebp-14h] BYREF

  PacketGuard::PacketGuard((PacketGuard *)v3);
  InterfacePacketBuf::put_header(v3, 0, 215);
  InterfacePacketBuf::put_byte(v3, a2 + 1);
  InterfacePacketBuf::put_int(v3, a3);
  InterfacePacketBuf::finalize(v3, 1);
  CParty::send_to_party(*this, (PacketGuard *)v3);
  PacketGuard::~PacketGuard(v3);
}

int __cdecl CBattle_Field::forceValuesToChangeRound(CBattle_Field *this, __int16 a2, __int16 a3, int id)
{
  int id; // eax
  int id; // eax
  int id; // eax
  int id; // eax
  int id; // eax
  int id; // [esp+24h] [ebp-34h] BYREF
  __int16 v12; // [esp+28h] [ebp-30h]
  __int16 v13; // [esp+2Ch] [ebp-2Ch]
  std::_Rb_tree_node_base *v14; // [esp+30h] [ebp-28h] BYREF
  int id; // [esp+34h] [ebp-24h] BYREF
  _BYTE v16[8]; // [esp+38h] [ebp-20h] BYREF
  int id; // [esp+40h] [ebp-18h]
  __int16 NextSamllRoundOrder; // [esp+44h] [ebp-14h]
  __int16 v19; // [esp+46h] [ebp-12h]
  int CurrentMapInfo; // [esp+48h] [ebp-10h]
  signed int i; // [esp+4Ch] [ebp-Ch]

  v13 = a2;
  v12 = a3;
  v17 = a4;
  NextSamllRoundOrder = CBattle_Field::getNextSamllRoundOrder(this, a2, a4);
  v19 = 0;
  if ( NextSamllRoundOrder )
  {
    if ( NextSamllRoundOrder > v12 )
    {
      for ( i = 0; NextSamllRoundOrder > i; ++i )
      {
        std::vector<bool>::operator[]((std::_Bit_reference *)v16, (_DWORD *)this + 153, i);
        std::_Bit_reference::operator=((int)v16, 1);
      }
      CurrentMapInfo = CBattle_Field::GetCurrentMapInfo(this);
      v5 = CBattle_Field::GetCurrentMapInfo(this);
      std::map<int,map_monster>::begin(&v15, v5 + 12);
      while ( 1 )
      {
        v6 = std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator->(&v15);
        if ( *(__int16 *)(v6 + 842) > v13 )
          break;
        v7 = std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator->(&v15);
        if ( *(_WORD *)(v7 + 842) == v13
          && *(char *)(std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator->(&v15) + 844) == a4 )
        {
          break;
        }
        v9 = std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator->(&v15);
        std::map<int,map_monster>::find(&v14, CurrentMapInfo + 12, (_DWORD *)(v9 + 8));
        std::map<int,map_monster>::erase(CurrentMapInfo + 12, v14);
        v10 = CBattle_Field::GetCurrentMapInfo(this);
        std::map<int,map_monster>::begin(&v11, v10 + 12);
        v15 = v11;
        v19 = 0;
      }
    }
    CBattle_Field::CBloodRound::setBloodSmallRoundOrder((CBattle_Field *)((char *)this + 684), NextSamllRoundOrder);
    return 1;
  }
  else
  {
    LogManager::logFormat(
      (LogManager *)1,
      (int)"battle_field.cpp",
      "bool CBattle_Field::forceValuesToChangeRound(short int, short int, int)",
      (const char *)0xE7E,
      "ULTIMATE_LOG : blood_round_order (%d) forceValuesToChangeRound ERROR!!\n",
      (const char *)v13);
    return 0;
  }
}

int __cdecl CBattle_Field::getNextRoundRandomDifficulty(CBattle_Field *this)
{
  int BloodRoundOrder; // ebx

  BloodRoundOrder = (__int16)CBattle_Field::CBloodRound::getBloodRoundOrder((CBattle_Field *)((char *)this + 684));
  if ( BloodRoundOrder == CBattle_Field::getBloodMaxRound(this) - 1 )
    return 1;
  if ( (int)get_rand_int(100) <= 49 )
    return 2;
  return 1;
}

int __cdecl CBattle_Field::getNextRoundDifficulty(CBattle_Field *this, __int16 a2)
{
  int UltimateDifficylty; // [esp+2Ch] [ebp-Ch]

  UltimateDifficylty = (char)CBattle_Field::getUltimateDifficylty(this);
  if ( a2 == CBattle_Field::getBloodMaxRound(this) - 1 || !a2 )
    return 1;
  else
    return UltimateDifficylty;
}

int __cdecl CBattle_Field::getNextSamllRoundOrder(CBattle_Field *this, __int16 a2, int id)
{
  int CurrentMapInfo; // eax
  int id; // eax
  std::_Rb_tree_node_base *v7; // [esp+20h] [ebp-18h] BYREF
  int id; // [esp+24h] [ebp-14h] BYREF
  _BYTE dest[6]; // [esp+28h] [ebp-10h] BYREF
  unsigned __int16 v10; // [esp+2Eh] [ebp-Ah]

  v10 = 0;
  CurrentMapInfo = CBattle_Field::GetCurrentMapInfo(this);
  std::map<int,map_monster>::begin(&v7, CurrentMapInfo + 12);
  while ( 1 )
  {
    v5 = CBattle_Field::GetCurrentMapInfo(this);
    std::map<int,map_monster>::end(&v8, v5 + 12);
    if ( !std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator!=(&v7, &v8) )
      break;
    if ( *(_WORD *)(std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator->(&v7) + 842) == a2
      && *(char *)(std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator->(&v7) + 844) == a3 )
    {
      return *(unsigned __int16 *)(std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator->(&v7) + 830);
    }
    std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator++(dest, &v7);
  }
  return v10;
}

int __cdecl CBattle_Field::get_blood_round_difficylty_of_small_round(CBattle_Field *this, __int16 a2)
{
  if ( std::vector<STAssignBloodPhaseTime>::empty((_DWORD *)this + 147)
    || (__int16)std::vector<STAssignBloodPhaseTime>::size((_DWORD *)this + 147) <= a2
    || a2 < 0 )
  {
    return -1;
  }
  else
  {
    return *(char *)(std::vector<STAssignBloodPhaseTime>::operator[]((_DWORD *)this + 147, a2) + 24);
  }
}

void __cdecl CBattle_Field::sendUltimateRandomDifficulty(CUser ***this, char flag, char type, __int16 a4)
{
  PacketBuf *v4[5]; // [esp+14h] [ebp-14h] BYREF

  PacketGuard::PacketGuard((PacketGuard *)v4);
  InterfacePacketBuf::put_header(v4, 0, 294);
  InterfacePacketBuf::put_byte(v4, a2);
  InterfacePacketBuf::put_byte(v4, a3);
  InterfacePacketBuf::put_short(v4, a4);
  InterfacePacketBuf::finalize(v4, 1);
  CParty::send_to_party(*this, (PacketGuard *)v4);
  PacketGuard::~PacketGuard(v4);
}

void __cdecl CBattle_Field::sendUltimateRandomDifficulty(CUser ***this, char flag, char type)
{
  PacketBuf *v3[5]; // [esp+14h] [ebp-14h] BYREF

  PacketGuard::PacketGuard((PacketGuard *)v3);
  InterfacePacketBuf::put_header(v3, 0, 294);
  InterfacePacketBuf::put_byte(v3, a2);
  InterfacePacketBuf::put_byte(v3, a3);
  InterfacePacketBuf::finalize(v3, 1);
  CParty::send_to_party(*this, (PacketGuard *)v3);
  PacketGuard::~PacketGuard(v3);
}

void __cdecl CBattle_Field::startUltimateRound(CBattle_Field *this)
{
  __int16 BloodRoundOrder; // [esp+26h] [ebp-12h]
  __int16 BloodSmallRoundOrder; // [esp+28h] [ebp-10h]
  int NextRoundDifficulty; // [esp+2Ch] [ebp-Ch]

  BloodRoundOrder = CBattle_Field::CBloodRound::getBloodRoundOrder((CBattle_Field *)((char *)this + 684));
  BloodSmallRoundOrder = CBattle_Field::CBloodRound::getBloodSmallRoundOrder((CBattle_Field *)((char *)this + 684));
  CBattle_Field::CBloodRound::getBloodLastSmallRoundOrder((CBattle_Field *)((char *)this + 684));
  NextRoundDifficulty = CBattle_Field::getNextRoundDifficulty(this, BloodRoundOrder);
  if ( NextRoundDifficulty == 1 )
    goto LABEL_6;
  if ( !NextRoundDifficulty )
  {
    LogManager::logFormat(
      (LogManager *)1,
      (int)"battle_field.cpp",
      "void CBattle_Field::startUltimateRound()",
      (const char *)0xF04,
      "ULTIMATE_LOG : ULITMATE ROUND (%d) startUltimateRound() ERROR!!\n",
      (const char *)BloodRoundOrder);
    return;
  }
  if ( (unsigned __int8)CBattle_Field::forceValuesToChangeRound(
                          this,
                          BloodRoundOrder,
                          BloodSmallRoundOrder,
                          NextRoundDifficulty) == 1 )
LABEL_6:
    CBattle_Field::BloodSpawnTimerManager(this, 1, 0);
  else
    LogManager::logFormat(
      (LogManager *)1,
      (int)"battle_field.cpp",
      "void CBattle_Field::startUltimateRound()",
      (const char *)0xF0A,
      "ULTIMATE_LOG : startUltimateRound() ERROR!!\n",
      (const char *)BloodRoundOrder);
}

int __cdecl CBattle_Field::get_blood_round_of_small_round(CBattle_Field *this, __int16 a2)
{
  if ( std::vector<STAssignBloodPhaseTime>::empty((_DWORD *)this + 147)
    || (__int16)std::vector<STAssignBloodPhaseTime>::size((_DWORD *)this + 147) <= a2
    || a2 < 0 )
  {
    return -1;
  }
  else
  {
    return *(_DWORD *)std::vector<STAssignBloodPhaseTime>::operator[]((_DWORD *)this + 147, a2);
  }
}

int __cdecl CBattle_Field::get_blood_spawn_cnt_of_small_round(CBattle_Field *this, __int16 a2)
{
  if ( std::vector<STAssignBloodPhaseTime>::empty((_DWORD *)this + 147)
    || (__int16)std::vector<STAssignBloodPhaseTime>::size((_DWORD *)this + 147) <= a2
    || a2 < 0 )
  {
    return -1;
  }
  else
  {
    return *(_DWORD *)(std::vector<STAssignBloodPhaseTime>::operator[]((_DWORD *)this + 147, a2) + 20);
  }
}

int __cdecl CBattle_Field::get_blood_spawn_small_round_interval(CBattle_Field *this, __int16 a2)
{
  if ( std::vector<STAssignBloodPhaseTime>::empty((_DWORD *)this + 147) )
    return -1;
  if ( (__int16)std::vector<STAssignBloodPhaseTime>::size((_DWORD *)this + 147) <= a2 || a2 < 0 )
    return -1;
  else
    return *(_DWORD *)(std::vector<STAssignBloodPhaseTime>::operator[]((_DWORD *)this + 147, a2) + 8);
}

int __cdecl CBattle_Field::get_blood_spawn_time_of_next_small_round(CBattle_Field *this, __int16 a2)
{
  int id; // ebx
  int id; // esi
  int id; // [esp+2Ch] [ebp-Ch]

  if ( std::vector<STAssignBloodPhaseTime>::empty((_DWORD *)this + 147)
    || std::vector<STAssignBloodSystem>::empty((_DWORD *)this + 144) )
  {
    return -1;
  }
  if ( (__int16)std::vector<STAssignBloodPhaseTime>::size((_DWORD *)this + 147) <= a2 || a2 < 0 )
    return -1;
  v8 = *(_DWORD *)(std::vector<STAssignBloodPhaseTime>::operator[]((_DWORD *)this + 147, a2) + 4);
  if ( v8 == -1 )
  {
    if ( std::vector<short>::empty((_DWORD *)this + 150)
      || (__int16)std::vector<short>::size((_DWORD *)this + 150) <= a2 )
    {
      return -1;
    }
    v8 = *(__int16 *)std::vector<short>::operator[]((_DWORD *)this + 150, a2);
  }
  if ( std::vector<STAssignBloodSystem>::size((_DWORD *)this + 144) <= v8 )
    return -1;
  v6 = *(_DWORD *)(std::vector<STAssignBloodSystem>::operator[]((_DWORD *)this + 144, v8) + 20);
  v7 = *(_DWORD *)(std::vector<STAssignBloodSystem>::operator[]((_DWORD *)this + 144, v8) + 24);
  return v6 - v7 * *(_DWORD *)(std::vector<STAssignBloodSystem>::operator[]((_DWORD *)this + 144, v8) + 36);
}

int __cdecl CBattle_Field::get_blood_spawn_time_of_phase(CBattle_Field *this, __int16 a2)
{
  int id; // [esp+2Ch] [ebp-Ch]

  if ( std::vector<STAssignBloodPhaseTime>::empty((_DWORD *)this + 147)
    || std::vector<STAssignBloodSystem>::empty((_DWORD *)this + 144) )
  {
    return -1;
  }
  if ( (__int16)std::vector<STAssignBloodPhaseTime>::size((_DWORD *)this + 147) <= a2 || a2 < 0 )
    return -1;
  v6 = *(_DWORD *)(std::vector<STAssignBloodPhaseTime>::operator[]((_DWORD *)this + 147, a2) + 4);
  if ( v6 == -1 )
  {
    if ( std::vector<short>::empty((_DWORD *)this + 150)
      || (__int16)std::vector<short>::size((_DWORD *)this + 150) <= a2 )
    {
      return -1;
    }
    v6 = *(__int16 *)std::vector<short>::operator[]((_DWORD *)this + 150, a2);
  }
  if ( std::vector<STAssignBloodSystem>::size((_DWORD *)this + 144) > v6 )
    return *(_DWORD *)(std::vector<STAssignBloodSystem>::operator[]((_DWORD *)this + 144, v6) + 24);
  else
    return -1;
}

int __cdecl CBattle_Field::get_blood_spawn_cnt_of_phase(CBattle_Field *this, __int16 a2, __int16 a3)
{
  int id; // [esp+20h] [ebp-18h]
  int id; // [esp+24h] [ebp-14h]
  int id; // [esp+28h] [ebp-10h]
  int i; // [esp+2Ch] [ebp-Ch]

  if ( std::vector<STAssignBloodPhaseTime>::empty((_DWORD *)this + 147)
    || std::vector<STAssignBloodSystem>::empty((_DWORD *)this + 144) )
  {
    return -1;
  }
  if ( (__int16)std::vector<STAssignBloodPhaseTime>::size((_DWORD *)this + 147) <= a2 || a2 < 0 )
    return -1;
  v7 = 0;
  v9 = *(_DWORD *)(std::vector<STAssignBloodPhaseTime>::operator[]((_DWORD *)this + 147, a2) + 4);
  if ( v9 == -1 )
  {
    if ( std::vector<short>::empty((_DWORD *)this + 150)
      || (__int16)std::vector<short>::size((_DWORD *)this + 150) <= a2 )
    {
      return -1;
    }
    v9 = *(__int16 *)std::vector<short>::operator[]((_DWORD *)this + 150, a2);
  }
  if ( std::vector<STAssignBloodSystem>::size((_DWORD *)this + 144) <= v9 )
    return -1;
  v8 = *(_DWORD *)(std::vector<STAssignBloodSystem>::operator[]((_DWORD *)this + 144, v9) + 28);
  for ( i = 0; a3 + 1 > i; ++i )
    v7 = v8 + i * *(_DWORD *)(std::vector<STAssignBloodSystem>::operator[]((_DWORD *)this + 144, v9) + 32);
  return v7;
}

int __cdecl CBattle_Field::get_blood_spawn_phase_total_cnt_of_small_round(CBattle_Field *this, __int16 a2)
{
  int id; // [esp+2Ch] [ebp-Ch]

  if ( std::vector<STAssignBloodPhaseTime>::empty((_DWORD *)this + 147)
    || std::vector<STAssignBloodSystem>::empty((_DWORD *)this + 144) )
  {
    return -1;
  }
  if ( (__int16)std::vector<STAssignBloodPhaseTime>::size((_DWORD *)this + 147) <= a2 || a2 < 0 )
    return -1;
  v6 = *(_DWORD *)(std::vector<STAssignBloodPhaseTime>::operator[]((_DWORD *)this + 147, a2) + 4);
  if ( v6 == -1 )
  {
    if ( std::vector<short>::empty((_DWORD *)this + 150)
      || (__int16)std::vector<short>::size((_DWORD *)this + 150) <= a2 )
    {
      return -1;
    }
    v6 = *(__int16 *)std::vector<short>::operator[]((_DWORD *)this + 150, a2);
  }
  if ( std::vector<STAssignBloodSystem>::size((_DWORD *)this + 144) > v6 )
    return *(_DWORD *)(std::vector<STAssignBloodSystem>::operator[]((_DWORD *)this + 144, v6) + 36);
  else
    return -1;
}

CBattle_Field *__cdecl CBattle_Field::reset_blood_values(CBattle_Field *this)
{
  std::vector<STAssignBloodSystem>::clear((int *)this + 144);
  std::vector<STAssignBloodPhaseTime>::clear((int *)this + 147);
  std::vector<short>::clear((int *)this + 150);
  std::vector<bool>::clear((_DWORD *)this + 153);
  memset((char *)this + 632, 0, 0x16u);
  memset((char *)this + 654, 0, 0x16u);
  *((_WORD *)this + 340) = -1;
  *((_DWORD *)this + 169) = 0;
  *((_BYTE *)this + 682) = 0;
  return this;
}

int __cdecl CBattle_Field::checkBloodSpawnOrder(CBattle_Field *this, __int16 a2)
{
  _BYTE v3[8]; // [esp+20h] [ebp-18h] BYREF
  signed int id; // [esp+28h] [ebp-10h]
  signed int i; // [esp+2Ch] [ebp-Ch]

  if ( (__int16)std::vector<bool>::size((CBattle_Field *)((char *)this + 612)) < a2 )
    return 0;
  if ( std::vector<bool>::empty((CBattle_Field *)((char *)this + 612)) )
    return 0;
  v4 = a2;
  for ( i = 0; i < v4; ++i )
  {
    std::vector<bool>::operator[]((std::_Bit_reference *)v3, (_DWORD *)this + 153, i);
    if ( !std::_Bit_reference::operator bool((int)v3) )
      return 0;
  }
  return 1;
}

  CurrentMapInfo = CBattle_Field::GetCurrentMapInfo((CBattle_Field *)a2);
  std::map<int,int>::clear(CurrentMapInfo + 140);

              v13 = CBattle_Field::GetCurrentMapInfo((CBattle_Field *)a2);
              MapInfo::Add_Mob(v13, (int)v24);

            v15 = CBattle_Field::GetCurrentMapInfo((CBattle_Field *)a2);
            std::map<int,int>::insert((int)v25, v15 + 140, (int)v26);

            a1 = *(_DWORD *)(CBattle_Field::GetCurrentMapInfo((CBattle_Field *)a2) + 4);
            v10 = (const char **)std::_List_iterator<STHellPartyGroupRate>::operator->(&v17);

              "void CBattle_Field::add_hell_party_monster(map_monster&, int&, int&, MAP_HELL_PARTY_GROUP_ORDER*)",
              (const char *)0x1076,

int __cdecl CBattle_Field::IsFirstEnterDungeon(CBattle_Field *this)
{
  return *((unsigned __int8 *)this + 92);
}

CBattle_Field *__cdecl CBattle_Field::ResetFirstEnterDungeon(CBattle_Field *this)
{
  *((_BYTE *)this + 92) = 0;
  return this;
}

void __cdecl CBattle_Field::moveMap_1_Log(CBattle_Field *this, int id, int value)
{
  const char *v3; // eax
  const char *v4; // edi
  int id; // esi
  int id; // ebx
  int MapInfoFromPos; // eax

  if ( *(_BYTE *)(CBattle_Field::GetMapInfoFromPos((MazeScript **)this, a2, a3, 0) + 232) )
    v3 = "layeredMap";
  else
    v3 = "generalMap";
  v4 = v3;
  v5 = *((_DWORD *)this + 25);
  v6 = *((_DWORD *)this + 24);
  MapInfoFromPos = CBattle_Field::GetMapInfoFromPos((MazeScript **)this, a2, a3, 0);
  LogManager::logFormat(
    (LogManager *)1,
    (int)"battle_field.cpp",
    "void CBattle_Field::moveMap_1_Log(int, int)",
    (const char *)0x1126,
    "TRY MoveMap [1][Dungeon Type : %d][Map Index : %d](%d,%d) -> (%d,%d) [Map Kind : %s]",
    *((const char **)this + 101),
    *(_DWORD *)(MapInfoFromPos + 4),
    v6,
    v5,
    a2,
    a3,
    v4);
}

void __cdecl CBattle_Field::moveMap_2_Log(CBattle_Field *this, int id, int value)
{
  const char *v3; // eax
  const char *v4; // edi
  int id; // esi
  int id; // ebx
  int MapInfoFromPos; // eax

  if ( *(_BYTE *)(CBattle_Field::GetMapInfoFromPos((MazeScript **)this, a2, a3, 0) + 232) )
    v3 = "layeredMap";
  else
    v3 = "generalMap";
  v4 = v3;
  v5 = *((_DWORD *)this + 25);
  v6 = *((_DWORD *)this + 24);
  MapInfoFromPos = CBattle_Field::GetMapInfoFromPos((MazeScript **)this, a2, a3, 0);
  LogManager::logFormat(
    (LogManager *)1,
    (int)"battle_field.cpp",
    "void CBattle_Field::moveMap_2_Log(int, int)",
    (const char *)0x1138,
    "TRY MoveMap [2][Dungeon Type : %d][Map Index : %d](%d,%d) -> (%d,%d) [Map Kind : %s]",
    *((const char **)this + 101),
    *(_DWORD *)(MapInfoFromPos + 4),
    v6,
    v5,
    a2,
    a3,
    v4);
}

void __cdecl CBattle_Field::moveMap_3_Log(CBattle_Field *this, int id, int value)
{
  const char *v3; // eax
  const char *v4; // edi
  int id; // esi
  int id; // ebx
  int MapInfoFromPos; // eax

  if ( *(_BYTE *)(CBattle_Field::GetMapInfoFromPos((MazeScript **)this, a2, a3, 0) + 232) )
    v3 = "layeredMap";
  else
    v3 = "generalMap";
  v4 = v3;
  v5 = *((_DWORD *)this + 25);
  v6 = *((_DWORD *)this + 24);
  MapInfoFromPos = CBattle_Field::GetMapInfoFromPos((MazeScript **)this, a2, a3, 0);
  LogManager::logFormat(
    (LogManager *)1,
    (int)"battle_field.cpp",
    "void CBattle_Field::moveMap_3_Log(int, int)",
    (const char *)0x114A,
    "TRY MoveMap [3][Dungeon Type : %d][Map Index : %d](%d,%d) -> (%d,%d) [Map Kind : %s]",
    *((const char **)this + 101),
    *(_DWORD *)(MapInfoFromPos + 4),
    v6,
    v5,
    a2,
    a3,
    v4);
}

void __cdecl CBattle_Field::moveMap_4_Log(CBattle_Field *this, int id, int value, const GridScript *a4)
{
  int id; // ebx
  const char *v5; // eax
  const char *v6; // edi
  int id; // eax

  if ( a4 )
  {
    v4 = std::vector<int>::size((_DWORD *)a4 + 7);
    if ( *(_BYTE *)(CBattle_Field::GetMapInfoFromPos(
                      (MazeScript **)this,
                      *((_DWORD *)this + 24),
                      *((_DWORD *)this + 25),
                      0)
                  + 232) )
      v5 = "layeredMap";
    else
      v5 = "generalMap";
    v6 = v5;
    v7 = std::vector<int>::size((_DWORD *)a4 + 1);
    LogManager::logFormat(
      (LogManager *)1,
      (int)"battle_field.cpp",
      "void CBattle_Field::moveMap_4_Log(int, int, const GridScript*)",
      (const char *)0x115B,
      "TRY MoveMap [4] Error(G_CDataManager()->find_dungeon( m_Dungeon )) Dungeon[%d] Pos(%d,%d), gs->mapIndex(%d) [Map K"
      "ind : %s], gs->layeredMapIndex(%d)\n",
      *((const char **)this + 101),
      *((_DWORD *)this + 24),
      *((_DWORD *)this + 25),
      v7,
      v6,
      v4);
  }
}

int __cdecl CBattle_Field::check_boss_map(CBattle_Field *this, int id, int id2)
{
  int id; // ebx
  int id; // esi
  int id; // ebx
  const char **v6; // eax
  int id; // [esp+20h] [ebp-18h] BYREF
  int id; // [esp+24h] [ebp-14h] BYREF
  int id; // [esp+28h] [ebp-10h] BYREF
  _DWORD value[3]; // [esp+2Ch] [ebp-Ch] BYREF

  if ( *((_DWORD *)this + 3) == a2
    && *((_DWORD *)this + 4) == a3
    && *((_DWORD *)this + 98)
    && *(_DWORD *)(*((_DWORD *)this + 98) + 2316) == 1 )
  {
    std::vector<CBattle_Field::stConditionEnterBossRoomStatus_t>::begin(&v10, (_DWORD *)this + 92);
    __gnu_cxx::__normal_iterator<CBattle_Field::stConditionEnterBossRoomStatus_t const*,std::vector<CBattle_Field::stConditionEnterBossRoomStatus_t>>::__normal_iterator<CBattle_Field::stConditionEnterBossRoomStatus_t*>(
      &v9,
      (int)&v10);
    std::vector<CBattle_Field::stConditionEnterBossRoomStatus_t>::end(v11, (int)this + 368);
    __gnu_cxx::__normal_iterator<CBattle_Field::stConditionEnterBossRoomStatus_t const*,std::vector<CBattle_Field::stConditionEnterBossRoomStatus_t>>::__normal_iterator<CBattle_Field::stConditionEnterBossRoomStatus_t*>(
      &v8,
      (int)v11);
    while ( __gnu_cxx::operator!=<CBattle_Field::stConditionEnterBossRoomStatus_t const*,std::vector<CBattle_Field::stConditionEnterBossRoomStatus_t>>(
              (int)&v9,
              (int)&v8) )
    {
      v3 = *(_DWORD *)(__gnu_cxx::__normal_iterator<CBattle_Field::stConditionEnterBossRoomStatus_t const*,std::vector<CBattle_Field::stConditionEnterBossRoomStatus_t>>::operator->((int)&v9)
                     + 16);
      if ( v3 < *(_DWORD *)(__gnu_cxx::__normal_iterator<CBattle_Field::stConditionEnterBossRoomStatus_t const*,std::vector<CBattle_Field::stConditionEnterBossRoomStatus_t>>::operator->((int)&v9)
                          + 20) )
      {
        v4 = *(_DWORD *)(__gnu_cxx::__normal_iterator<CBattle_Field::stConditionEnterBossRoomStatus_t const*,std::vector<CBattle_Field::stConditionEnterBossRoomStatus_t>>::operator->((int)&v9)
                       + 20);
        v5 = *(_DWORD *)(__gnu_cxx::__normal_iterator<CBattle_Field::stConditionEnterBossRoomStatus_t const*,std::vector<CBattle_Field::stConditionEnterBossRoomStatus_t>>::operator->((int)&v9)
                       + 16);
        v6 = (const char **)__gnu_cxx::__normal_iterator<CBattle_Field::stConditionEnterBossRoomStatus_t const*,std::vector<CBattle_Field::stConditionEnterBossRoomStatus_t>>::operator->((int)&v9);
        LogManager::logFormat(
          (LogManager *)1,
          (int)"battle_field.cpp",
          "int CBattle_Field::check_boss_map(int, int)",
          (const char *)0x1181,
          "TRY MoveMap error condition [%d][%d]/[%d]",
          *v6,
          v5,
          v4);
        return 7;
      }
      __gnu_cxx::__normal_iterator<CBattle_Field::stConditionEnterBossRoomStatus_t const*,std::vector<CBattle_Field::stConditionEnterBossRoomStatus_t>>::operator++(&v9);
    }
  }
  return 0;
}

int __cdecl CBattle_Field::check_movemap_range(const char **this, int id, int id2)
{
  const char *v3; // eax
  const char *v4; // edi
  int id; // esi
  int id; // ebx
  int MapInfoFromPos; // eax
  int id; // [esp+30h] [ebp-28h] BYREF
  int id; // [esp+34h] [ebp-24h] BYREF
  int id; // [esp+38h] [ebp-20h]
  int id; // [esp+3Ch] [ebp-1Ch]

  v10 = -1;
  v9 = -1;
  CBattle_Field::getCurPosXY((CBattle_Field *)this, &v10, &v9);
  v11 = abs32(a2 - v10);
  v12 = abs32(a3 - v9);
  if ( v11 <= 1 && v12 <= 1 && (v11 != 1 || v12 != 1) )
    return 0;
  if ( *(_BYTE *)(CBattle_Field::GetMapInfoFromPos((MazeScript **)this, a2, a3, 0) + 232) )
    v3 = "layeredMap";
  else
    v3 = "generalMap";
  v4 = v3;
  v5 = (int)*(this + 25);
  v6 = (int)*(this + 24);
  MapInfoFromPos = CBattle_Field::GetMapInfoFromPos((MazeScript **)this, a2, a3, 0);
  LogManager::logFormat(
    (LogManager *)1,
    (int)"battle_field.cpp",
    "int CBattle_Field::check_movemap_range(int, int)",
    (const char *)0x119F,
    "TRY MoveMap [3][Dungeon Type : %d][Map Index : %d](%d,%d) -> (%d,%d) [Map Kind : %s]",
    *(this + 101),
    *(_DWORD *)(MapInfoFromPos + 4),
    v6,
    v5,
    a2,
    a3,
    v4);
  return 21;
}

_BOOL4 __cdecl CBattle_Field::is_visited_map(MazeScript **this, int id, int id2)
{
  return *(_BYTE *)(CBattle_Field::GetMapInfoFromPos(this, a2, a3, 0) + 1) != 0;
}

_BOOL4 __cdecl CBattle_Field::is_visited_map_goto(MazeScript **this, int id, int id2)
{
  int MapInfoFromPos; // [esp+1Ch] [ebp-Ch]

  MapInfoFromPos = CBattle_Field::GetMapInfoFromPos(this, a2, a3, 0);
  return *(_BYTE *)(MapInfoFromPos + 1) || *(_BYTE *)(MapInfoFromPos + 2);
}

CBattle_Field *__cdecl CBattle_Field::on_not_visited_map(CBattle_Field *this, int id, int id2, bool *a4)
{
  int id; // ebx
  CMapList **v5; // eax
  CBattle_Field *result; // eax
  CBattle_Field *battlefield; // [esp+1Ch] [ebp-Ch]

  *a4 = 1;
  *(_BYTE *)(CBattle_Field::GetCurrentMapInfo(this) + 1) = 1;
  CBattle_Field::swap_curpos_xy(this, a2, a3);
  v4 = *(_DWORD *)(CBattle_Field::GetCurrentMapInfo(this) + 4);
  v5 = (CMapList **)G_CDataManager();
  result = (CBattle_Field *)CDataManager::find_map(v5, v4);
  v7 = result;
  if ( result )
  {
    *((_DWORD *)this + 66) = v7;
    return this;
  }
  return result;
}

int __cdecl CBattle_Field::check_visited_map(CDungeon **this, int id, int id2, bool *a4)
{
  if ( *(this + 98) && (unsigned __int8)CDungeon::IsEnterEachMap(*(this + 98)) )
  {
    if ( CBattle_Field::is_visited_map_goto(this, a2, a3) )
    {
LABEL_7:
      CBattle_Field::on_not_visited_map((CBattle_Field *)this, a2, a3, a4);
      return 1;
    }
  }
  else if ( CBattle_Field::is_visited_map(this, a2, a3) )
  {
    goto LABEL_7;
  }
  return 0;
}

int __cdecl CBattle_Field::get_invade_direction(CBattle_Field *this, int id, int id2)
{
  int id; // [esp+14h] [ebp-14h] BYREF
  int id; // [esp+18h] [ebp-10h] BYREF
  int id; // [esp+1Ch] [ebp-Ch]

  v5 = -1;
  v4 = -1;
  CBattle_Field::getCurPosXY(this, &v5, &v4);
  v6 = 0;
  if ( v5 == a2 )
  {
    if ( v4 > a3 )
      v6 = 8;
    if ( v4 < a3 )
      v6 = 2;
  }
  if ( v4 == a3 )
  {
    if ( v5 < a2 )
      v6 = 4;
    if ( v5 > a2 )
      return 1;
  }
  return v6;
}

void __cdecl CBattle_Field::choose_random_bossmap_from_dummymaps(
        CBattle_Field *this,

int __cdecl CBattle_Field::get_candidator_map(
        CBattle_Field *battlefield,

        if ( *(_BYTE *)(CBattle_Field::GetMapInfoFromPos(
                          (MazeScript **)a1,

          "int CBattle_Field::get_candidator_map(const CDungeon*, const GridScript*, int&, INT_VECTOR&, int, int, int)",
          (const char *)0x12BB,

      if ( *(_BYTE *)(CBattle_Field::GetCurrentMapInfo(a1) + 232) != 1 && !std::vector<int>::size(a5) )
      {
        for ( k = 0; std::vector<int>::size(a3 + 1) > (unsigned int)k; ++k )
        {
          v20 = *(_DWORD *)std::vector<int>::operator[](a3 + 1, (int)k);
          v21 = (CMapList **)G_CDataManager();
          CDataManager::find_map(v21, v20);
          LogManager::logFormat(
            (LogManager *)1,
            (int)"battle_field.cpp",
            "int CBattle_Field::get_candidator_map(const CDungeon*, const GridScript*, int&, INT_VECTOR&, int, int, int)",
            (const char *)0x12E4,
            "[CBattle_Field::MoveMap] Error(< map_candidator.size()==0) Dungeon[%d] Pos(%d,%d), (Direction: %d) \n",
            *((const char **)a1 + 101),
            *((_DWORD *)a1 + 24),
            *((_DWORD *)a1 + 25),
            a8);
          v22 = (_DWORD *)std::vector<int>::operator[](a3 + 1, (int)k);
          LogManager::logFormat(
            (LogManager *)1,
            (int)"battle_field.cpp",
            "int CBattle_Field::get_candidator_map(const CDungeon*, const GridScript*, int&, INT_VECTOR&, int, int, int)",
            (const char *)0x12E5,
            "[CBattle_Field::MoveMap] Error(< map_candidator.size()==0) gs->mapIndexes_[%d]=%d \n",
            k,
            *v22);
        }
      }

      LayeredMapIndex = CBattle_Field::getLayeredMapIndex(this);
      result = CBattle_Field::getLayeredMapInfoIndex(this, LayeredMapIndex - 1);

      result = CBattle_Field::GetMapInfoFromPos((MazeScript **)this, v7[0], v6, 1);
    }
    *(_BYTE *)(result + 1) = a4;
  }
  else
  {
    result = CBattle_Field::GetCurrentMapInfo(this);
    *(_BYTE *)(result + 1) = a4;
  }
  return result;
}

//----- (0830878E) --------------------------------------------------------
CBattle_Field *__cdecl CBattle_Field::swap_curpos_xy(CBattle_Field *this, int id, int value)
{
  int id; // ebx
  int id; // edx
  _DWORD value[3]; // [esp+18h] [ebp-10h] BYREF

  if ( *((_DWORD *)this + 98)
    && (unsigned __int8)CDungeon::IsEnterEachMap(*((CDungeon **)this + 98))
    && *((_DWORD *)this + 69) < 4u )
  {
    v4 = *((_DWORD *)this + 69) + 6;
    v5 = *((_DWORD *)this + 2 * *((_DWORD *)this + 69) + 8);
    *((_DWORD *)this + 2 * v4 + 3) = *((_DWORD *)this + 2 * *((_DWORD *)this + 69) + 7);
    *((_DWORD *)this + 2 * v4 + 4) = v5;
    std::make_pair<int &,int &>(v7, (int)&a2, (int)&a3);
    return (CBattle_Field *)std::pair<int,int>::operator=((_DWORD *)this + 2 * *((_DWORD *)this + 69) + 7, (int)v7);
  }
  else
  {
    *((_DWORD *)this + 26) = *((_DWORD *)this + 24);
    *((_DWORD *)this + 27) = *((_DWORD *)this + 25);
    *((_DWORD *)this + 24) = a2;
    *((_DWORD *)this + 25) = a3;
    return this;
  }
}

//----- (08308882) --------------------------------------------------------
int __cdecl CBattle_Field::set_map_id(CBattle_Field *battlefield, int value, int *a3, _DWORD *a4)
{
  int value; // eax
  int LayeredMapIndex; // eax
  int value; // esi
  const char *v8; // eax
  int value; // ebx
  int value; // eax
  const char *v11; // [esp+3Ch] [ebp-2Ch]
  unsigned int rand_int; // [esp+4Ch] [ebp-1Ch]

  if ( !a2 )
    return 21;
  if ( *a3 < 0 )
  {
    if ( std::vector<int>::size(a4) )
    {

    if ( *(_BYTE *)(CBattle_Field::GetCurrentMapInfo(a1) + 232) )
    {
      LayeredMapIndex = CBattle_Field::getLayeredMapIndex(a1);
      *a3 = *(_DWORD *)std::vector<int>::operator[]((_DWORD *)(a2 + 28), LayeredMapIndex);
    }

  *(_DWORD *)(CBattle_Field::GetCurrentMapInfo(a1) + 4) = *a3;
  if ( *a3 > 0 )

  if ( *(_BYTE *)(CBattle_Field::GetMapInfoFromPos((MazeScript **)a1, *((_DWORD *)a1 + 24), *((_DWORD *)a1 + 25), 0)
                + 232) )

    "int CBattle_Field::set_map_id(const GridScript*, int&, INT_VECTOR&)",
    (const char *)0x1363,

int __cdecl CBattle_Field::set_visited_map_goto(CBattle_Field *this, int value, int value2)
{
  int result; // eax
  int LayeredMapIndex; // eax
  int value; // [esp+18h] [ebp-10h] BYREF
  int value[3]; // [esp+1Ch] [ebp-Ch] BYREF

  v6[0] = -1;
  v5 = -1;
  CBattle_Field::getLastPosXY(this, v6, &v5);
  if ( v6[0] == a2 && v5 == a3 )
  {
    if ( CBattle_Field::getLayeredMapIndex(this) )
    {
      LayeredMapIndex = CBattle_Field::getLayeredMapIndex(this);
      result = CBattle_Field::getLayeredMapInfoIndex(this, LayeredMapIndex - 1);
    }
    else
    {
      result = CBattle_Field::GetMapInfoFromPos((MazeScript **)this, a2, a3, 1);
    }
    *(_BYTE *)(result + 2) = 1;
  }
  else
  {
    result = CBattle_Field::GetMapInfoFromPos((MazeScript **)this, a2, a3, 0);
    *(_BYTE *)(result + 2) = 1;
  }
  return result;
}

int __cdecl CBattle_Field::MoveMap(MazeScript **this, unsigned int id, unsigned int value, bool *a4)
{
  int id; // ebx
  int id; // ebx
  CDungeonList **v6; // eax
  int CurrentMapInfo; // eax
  int id; // [esp+20h] [ebp-38h] BYREF
  int id; // [esp+24h] [ebp-34h] BYREF
  int id[3]; // [esp+28h] [ebp-30h] BYREF
  bool v12; // [esp+37h] [ebp-21h] BYREF
  int id[2]; // [esp+38h] [ebp-20h] BYREF
  int candidator_map; // [esp+40h] [ebp-18h]
  GridScript *GridR; // [esp+44h] [ebp-14h]
  int invade_direction; // [esp+48h] [ebp-10h]
  CDungeon *dungeon; // [esp+4Ch] [ebp-Ch]

  v13[1] = CBattle_Field::GetCurrentMapInfo((CBattle_Field *)this);
  if ( (unsigned __int8)CDungeon::IsEnterEachMap(*(this + 98)) != 1
    && (unsigned __int8)CBattle_Field::check_grid_clear((CBattle_Field *)this) != 1 )
  {
    CBattle_Field::moveMap_1_Log((CBattle_Field *)this, a2, a3);
    return 20;
  }
  else
  {
    if ( *(this + 98) && *((char *)*(this + 98) + 2207) > 0 )
    {
      CBattle_Field::reset_blood_values((CBattle_Field *)this);
      CBattle_Field::CBloodRound::reset_blood_values((CBattle_Field::CBloodRound *)(this + 171));
      CBattle_Field::CBloodRound::setBloodRoundOrder((CBattle_Field::CBloodRound *)(this + 171));
    }
    if ( a2 <= 0x31 && a3 < 0x32 )
    {
      if ( *(_BYTE *)(CBattle_Field::GetCurrentMapInfo((CBattle_Field *)this) + 232) != 1
        && (v10 = -1, v9 = -1, CBattle_Field::getCurPosXY((CBattle_Field *)this, &v10, &v9), a2 == v10)
        && a3 == v9 )
      {
        return 18;
      }
      else if ( *(this + 29) )
      {
        if ( *(_DWORD *)*(this + 29) > (signed int)a2 && *((_DWORD *)*(this + 29) + 1) > (signed int)a3 )
        {
          candidator_map = 0;
          candidator_map = CBattle_Field::check_boss_map((CBattle_Field *)this, a2, a3);
          if ( candidator_map )
          {
            return candidator_map;
          }
          else
          {
            candidator_map = CBattle_Field::check_movemap_range((const char **)this, a2, a3);
            if ( candidator_map )
            {
              return candidator_map;
            }
            else
            {
              GridR = (GridScript *)MazeScript::getGridR(*(this + 29), a2, a3);
              if ( GridR )
              {
                if ( (unsigned __int8)CBattle_Field::check_visited_map(this, a2, a3, a4) )
                {
                  return 0;
                }
                else
                {
                  invade_direction = CBattle_Field::get_invade_direction((CBattle_Field *)this, a2, a3);
                  v5 = (int)*(this + 101);
                  v6 = (CDungeonList **)G_CDataManager();
                  dungeon = (CDungeon *)CDataManager::find_dungeon(v6, v5);
                  if ( dungeon )
                  {
                    v13[0] = -1;
                    v12 = 1;
                    CBattle_Field::choose_random_bossmap_from_dummymaps(
                      (CBattle_Field *)this,
                      (const char **)dungeon,
                      GridR,
                      v13,
                      &v12,
                      a2,
                      a3,
                      invade_direction);
                    std::vector<int>::vector(v11);
                    *((_BYTE *)this + 456) = 0;
                    candidator_map = CBattle_Field::get_candidator_map(
                                       (CBattle_Field *)this,
                                       (const char **)dungeon,
                                       (int *)GridR,
                                       v13,
                                       v11,
                                       a2,
                                       a3,
                                       invade_direction);
                    if ( candidator_map )
                    {
                      v4 = candidator_map;
                    }
                    else
                    {
                      CBattle_Field::set_visited_map((CBattle_Field *)this, a2, a3, v12);
                      CBattle_Field::swap_curpos_xy((CBattle_Field *)this, a2, a3);
                      CBattle_Field::set_visited_map_goto((CBattle_Field *)this, a2, a3);
                      CurrentMapInfo = CBattle_Field::GetCurrentMapInfo((CBattle_Field *)this);
                      *(_BYTE *)(CurrentMapInfo + 8) = *(_DWORD *)GridR;
                      candidator_map = CBattle_Field::set_map_id((CBattle_Field *)this, (int)GridR, v13, v11);
                      if ( candidator_map )
                      {
                        v4 = candidator_map;
                      }
                      else
                      {
                        CBattle_Field::ConsistMap((CBattle_Field *)this);
                        if ( *this )
                          std::map<int,int>::clear((int)*this + 6212);
                        v4 = 0;
                      }
                    }
                    std::vector<int>::~vector((void **)v11);
                  }
                  else
                  {
                    CBattle_Field::moveMap_4_Log((CBattle_Field *)this, a2, a3, GridR);
                    return 1;
                  }
                }
              }
              else
              {
                CBattle_Field::moveMap_3_Log((CBattle_Field *)this, a2, a3);
                return 21;
              }
            }
          }
        }
        else
        {
          return 21;
        }
      }
      else
      {
        CBattle_Field::moveMap_2_Log((CBattle_Field *)this, a2, a3);
        return 21;
      }
    }
    else
    {
      return 1;
    }
  }
  return v4;
}

int CBattle_Field::SetDamagedPacketCnt(CBattle_Field *this, ...)
{
  int CurrentMapInfo; // eax
  int value; // eax
  int value; // eax
  int value; // [esp+1Ch] [ebp-1Ch] BYREF
  int value; // [esp+24h] [ebp-14h] BYREF
  int value; // [esp+28h] [ebp-10h] BYREF
  int value; // [esp+2Ch] [ebp-Ch]
  va_list va; // [esp+44h] [ebp+Ch] BYREF

  va_start(va, this);
  v8 = 0;
  std::_Rb_tree_iterator<std::pair<int const,map_monster>>::_Rb_tree_iterator(&v6);
  CurrentMapInfo = CBattle_Field::GetCurrentMapInfo(this);
  std::map<int,map_monster>::find(&v5, CurrentMapInfo + 12, (int *)va);
  v6 = v5;
  v2 = CBattle_Field::GetCurrentMapInfo(this);
  std::map<int,map_monster>::end(&v7, v2 + 12);
  if ( std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator==(&v6, &v7) )
    return 0;
  v4 = std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator->(&v6);
  v8 = v4 + 4;
  ++*(_DWORD *)(v4 + 36);
  return 1;
}

int __cdecl CBattle_Field::SetDamageValue(CBattle_Field *this, int value, int value2)
{
  int CurrentMapInfo; // eax
  int value; // eax
  int value; // eax
  int value; // [esp+1Ch] [ebp-1Ch] BYREF
  int value; // [esp+24h] [ebp-14h] BYREF
  int value; // [esp+28h] [ebp-10h] BYREF
  int value; // [esp+2Ch] [ebp-Ch]

  v10 = 0;
  std::_Rb_tree_iterator<std::pair<int const,map_monster>>::_Rb_tree_iterator(&v8);
  CurrentMapInfo = CBattle_Field::GetCurrentMapInfo(this);
  std::map<int,map_monster>::find(&v7, CurrentMapInfo + 12, &a2);
  v8 = v7;
  v4 = CBattle_Field::GetCurrentMapInfo(this);
  std::map<int,map_monster>::end(&v9, v4 + 12);
  if ( std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator==(&v8, &v9) )
    return 0;
  v6 = std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator->(&v8);
  v10 = v6 + 4;
  *(_DWORD *)(v6 + 40) += a3;
  return 1;
}

void __cdecl CBattle_Field::MakePacket(CBattle_Field *this, int count, InterfacePacketBuf *a3)
{
  int count; // eax
  int count; // eax
  int count; // eax
  int count; // eax
  char flag; // al
  int count; // eax
  int count; // eax
  int count; // eax
  int count; // eax
  char flag; // al
  int count; // eax
  int add_info; // eax
  char AbilityType; // al
  __int16 AbilityValue; // ax
  int count; // eax
  char ObjectCnt; // al
  _DWORD *v21; // eax
  int count; // eax
  int count; // eax
  char flag; // al
  int CurrentMapInfo; // eax
  int count; // eax
  int count; // eax
  int count; // ebx
  CMapList **v29; // eax
  int count; // esi
  int count; // ebx
  int count; // eax
  char random_buff_type; // al
  char flag; // al
  _WORD *v35; // eax
  int count; // eax
  int count; // eax
  unsigned int count; // eax
  int count; // eax
  int count; // eax
  int count; // eax
  int count; // eax
  int count; // ebx
  CMapList **v45; // eax
  int count; // esi
  int count; // ebx
  int count; // eax
  int count; // [esp+20h] [ebp-448h] BYREF
  int count; // [esp+24h] [ebp-444h]
  char flag; // [esp+28h] [ebp-440h]
  int count; // [esp+2Ch] [ebp-43Ch]
  char flag; // [esp+30h] [ebp-438h]
  char flag; // [esp+33h] [ebp-435h]
  char flag; // [esp+34h] [ebp-434h]
  char flag; // [esp+54h] [ebp-414h]
  __int16 v57; // [esp+354h] [ebp-114h]
  int count[21]; // [esp+36Ch] [ebp-FCh] BYREF
  int count; // [esp+3C0h] [ebp-A8h] BYREF
  int count; // [esp+3C4h] [ebp-A4h] BYREF
  int count; // [esp+3C8h] [ebp-A0h] BYREF
  std::_Rb_tree_node_base *v62; // [esp+3CCh] [ebp-9Ch] BYREF
  int count; // [esp+3D0h] [ebp-98h] BYREF
  int count; // [esp+3D4h] [ebp-94h] BYREF
  void *v65[3]; // [esp+3D8h] [ebp-90h] BYREF
  std::_Rb_tree_node_base *v66; // [esp+3E4h] [ebp-84h] BYREF
  std::_Rb_tree_node_base *v67; // [esp+3E8h] [ebp-80h] BYREF
  int count; // [esp+3ECh] [ebp-7Ch] BYREF
  int count; // [esp+3F0h] [ebp-78h] BYREF
  int count; // [esp+3F4h] [ebp-74h] BYREF
  _BYTE dest[4]; // [esp+3F8h] [ebp-70h] BYREF
  int count; // [esp+3FCh] [ebp-6Ch] BYREF
  _BYTE v73[4]; // [esp+400h] [ebp-68h] BYREF
  _BYTE v74[2]; // [esp+404h] [ebp-64h] BYREF
  __int16 v75; // [esp+406h] [ebp-62h] BYREF
  int count; // [esp+408h] [ebp-60h] BYREF
  _BYTE v77[4]; // [esp+40Ch] [ebp-5Ch] BYREF
  InterfacePacketBuf *v78; // [esp+410h] [ebp-58h]
  int n; // [esp+414h] [ebp-54h]
  int Manager; // [esp+418h] [ebp-50h]
  int CharacExpandData; // [esp+41Ch] [ebp-4Ch]
  __int16 Layer; // [esp+422h] [ebp-46h]
  char member_count; // [esp+42Bh] [ebp-3Dh]
  int count; // [esp+42Ch] [ebp-3Ch]
  char i; // [esp+433h] [ebp-35h]
  _DWORD *v87; // [esp+434h] [ebp-34h]
  unsigned int j; // [esp+438h] [ebp-30h]
  int k; // [esp+43Ch] [ebp-2Ch]
  char m; // [esp+443h] [ebp-25h]
  int count; // [esp+444h] [ebp-24h]
  int count; // [esp+448h] [ebp-20h]

  v78 = a3;
  if ( a2 == 211 )
  {
    InterfacePacketBuf::put_header((PacketBuf **)v78, 0, 211);
    v64 = -1;
    v63 = -1;
    CBattle_Field::getCurPosXY(this, &v64, &v63);
    InterfacePacketBuf::put_byte((PacketBuf **)v78, v64);
    InterfacePacketBuf::put_byte((PacketBuf **)v78, v63);
    CurrentMapInfo = CBattle_Field::GetCurrentMapInfo(this);
    InterfacePacketBuf::put_int((PacketBuf **)v78, *(_DWORD *)(CurrentMapInfo + 72));
    InterfacePacketBuf::put_byte((PacketBuf **)v78, 0);
    if ( *(_BYTE *)(CBattle_Field::GetCurrentMapInfo(this) + 1) )
    {
      InterfacePacketBuf::put_byte((PacketBuf **)v78, 0);
      InterfacePacketBuf::finalize((PacketBuf **)v78, 1);
    }
    else
    {
      v26 = CBattle_Field::GetCurrentMapInfo(this);
      InterfacePacketBuf::put_byte((PacketBuf **)v78, *(_BYTE *)(v26 + 68));
      v27 = CBattle_Field::GetCurrentMapInfo(this);
      InterfacePacketBuf::put_short((PacketBuf **)v78, *(_DWORD *)(v27 + 4));
      v28 = *(_DWORD *)(CBattle_Field::GetCurrentMapInfo(this) + 4);
      v29 = (CMapList **)G_CDataManager();
      if ( CDataManager::find_map(v29, v28) )
      {
        InterfacePacketBuf::put_byte((PacketBuf **)v78, 0);
        InterfacePacketBuf::put_byte((PacketBuf **)v78, 0);
        InterfacePacketBuf::put_byte((PacketBuf **)v78, 0);
        InterfacePacketBuf::finalize((PacketBuf **)v78, 1);
      }
      else
      {
        v30 = *((_DWORD *)this + 25);
        v31 = *((_DWORD *)this + 24);
        v32 = CBattle_Field::GetCurrentMapInfo(this);
        LogManager::logFormat(
          (LogManager *)1,
          (int)"battle_field.cpp",
          "void CBattle_Field::MakePacket(int, void*)",
          (const char *)0x1746,
          "Find Map[%d] Error(In Map Maker)(%d/%d)",
          *(const char **)(v32 + 4),
          v31,
          v30);
      }
    }
  }
  else if ( a2 > 211 )
  {
    switch ( a2 )
    {
      case 386:
        member_count = CParty::get_member_count(*(CUser ***)this);
        InterfacePacketBuf::put_byte((PacketBuf **)v78, member_count);
        v85 = CBattle_Field::GetCurrentMapInfo(this) + 12;
        std::map<int,map_monster>::begin(&v62, v85);
        for ( i = 1; i <= 4; ++i )
        {
          InterfacePacketBuf::put_byte((PacketBuf **)v78, i);
          v75 = i;
          std::map<short,std::vector<CBattle_Field::TournamentInfo>>::find(&v61, (int)this + 316, &v75);
          v87 = (_DWORD *)(std::_Rb_tree_iterator<std::pair<short const,std::vector<CBattle_Field::TournamentInfo>>>::operator->(&v61)
                         + 4);
          v34 = std::vector<CBattle_Field::TournamentInfo>::size(v87);
          InterfacePacketBuf::put_byte((PacketBuf **)v78, v34);
          for ( j = 0; ; ++j )
          {
            v38 = std::vector<CBattle_Field::TournamentInfo>::size(v87);
            if ( v38 <= j )
              break;
            v35 = (_WORD *)std::vector<CBattle_Field::TournamentInfo>::operator[](v87, j);
            InterfacePacketBuf::put_byte((PacketBuf **)v78, *v35);
            for ( k = 0; member_count > k; ++k )
            {
              v36 = std::vector<CBattle_Field::TournamentInfo>::operator[](v87, j);
              InterfacePacketBuf::put_int((PacketBuf **)v78, *(_DWORD *)(v36 + 8 * k + 4));
              v37 = std::vector<CBattle_Field::TournamentInfo>::operator[](v87, j);
              InterfacePacketBuf::put_short((PacketBuf **)v78, *(_DWORD *)(v37 + 8 * k + 8));
            }
          }
        }
        for ( m = 1; m <= 4; ++m )
        {
          InterfacePacketBuf::put_byte((PacketBuf **)v78, m);
          v91 = 0;
          while ( member_count > v91 )
          {
            std::map<int,map_monster>::end(&v76, v85);
            if ( !std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator!=(&v62, &v76) )
              break;
            v39 = std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator->(&v62);
            v92 = v39 + 4;
            InterfacePacketBuf::put_short((PacketBuf **)v78, *(_DWORD *)(v39 + 8));
            InterfacePacketBuf::put_int((PacketBuf **)v78, *(_DWORD *)(v92 + 12));
            InterfacePacketBuf::put_byte((PacketBuf **)v78, *(_BYTE *)(v92 + 16));
            InterfacePacketBuf::put_byte((PacketBuf **)v78, *(_BYTE *)(v92 + 8));
            ++v91;
            std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator++(v77, &v62);
          }
        }
        InterfacePacketBuf::finalize((PacketBuf **)v78, 1);
        break;
      case 387:
        InterfacePacketBuf::put_header((PacketBuf **)v78, 0, 387);
        v60 = -1;
        v59 = -1;
        CBattle_Field::getCurPosXY(this, &v60, &v59);
        InterfacePacketBuf::put_byte((PacketBuf **)v78, v60);
        InterfacePacketBuf::put_byte((PacketBuf **)v78, v59);
        v41 = CBattle_Field::GetCurrentMapInfo(this);
        InterfacePacketBuf::put_int((PacketBuf **)v78, *(_DWORD *)(v41 + 72));
        InterfacePacketBuf::put_byte((PacketBuf **)v78, 0);
        if ( *(_BYTE *)(CBattle_Field::GetCurrentMapInfo(this) + 1) )
        {
          InterfacePacketBuf::put_byte((PacketBuf **)v78, 0);
          InterfacePacketBuf::finalize((PacketBuf **)v78, 1);
        }
        else
        {
          v42 = CBattle_Field::GetCurrentMapInfo(this);
          InterfacePacketBuf::put_byte((PacketBuf **)v78, *(_BYTE *)(v42 + 68));
          v43 = CBattle_Field::GetCurrentMapInfo(this);
          InterfacePacketBuf::put_short((PacketBuf **)v78, *(_DWORD *)(v43 + 4));
          v44 = *(_DWORD *)(CBattle_Field::GetCurrentMapInfo(this) + 4);
          v45 = (CMapList **)G_CDataManager();
          if ( CDataManager::find_map(v45, v44) )
          {
            InterfacePacketBuf::put_byte((PacketBuf **)v78, 0);
            InterfacePacketBuf::put_byte((PacketBuf **)v78, 0);
            InterfacePacketBuf::put_byte((PacketBuf **)v78, 0);
            InterfacePacketBuf::finalize((PacketBuf **)v78, 1);
          }
          else
          {
            v46 = *((_DWORD *)this + 25);
            v47 = *((_DWORD *)this + 24);
            v48 = CBattle_Field::GetCurrentMapInfo(this);
            LogManager::logFormat(
              (LogManager *)1,
              (int)"battle_field.cpp",
              "void CBattle_Field::MakePacket(int, void*)",
              (const char *)0x17D2,
              "Find Map[%d] Error(In Map Maker)(%d/%d)",
              *(const char **)(v48 + 4),
              v47,
              v46);
          }
        }
        break;
      case 212:
        InterfacePacketBuf::put_byte((PacketBuf **)v78, *((_DWORD *)this + 107));
        InterfacePacketBuf::put_byte((PacketBuf **)v78, *((_DWORD *)this + 3));
        InterfacePacketBuf::put_byte((PacketBuf **)v78, *((_DWORD *)this + 4));
        InterfacePacketBuf::put_byte((PacketBuf **)v78, 0);
        InterfacePacketBuf::put_byte((PacketBuf **)v78, 0);
        InterfacePacketBuf::put_byte((PacketBuf **)v78, *((_DWORD *)this + 109));
        if ( *(_DWORD *)this )
        {
          random_buff_type = CParty::get_random_buff_type(*(CParty **)this);
          InterfacePacketBuf::put_byte((PacketBuf **)v78, random_buff_type);
        }
        else
        {
          InterfacePacketBuf::put_byte((PacketBuf **)v78, 11);
        }
        InterfacePacketBuf::finalize((PacketBuf **)v78, 1);
        break;
    }
  }
  else if ( a2 == 28 )
  {
    InterfacePacketBuf::put_byte((PacketBuf **)v78, *((_DWORD *)this + 107));
    InterfacePacketBuf::put_byte((PacketBuf **)v78, *((_DWORD *)this + 3));
    InterfacePacketBuf::put_byte((PacketBuf **)v78, *((_DWORD *)this + 4));
    InterfacePacketBuf::put_byte((PacketBuf **)v78, *((_DWORD *)this + 112));
    InterfacePacketBuf::put_byte((PacketBuf **)v78, *((_DWORD *)this + 113));
    InterfacePacketBuf::put_byte((PacketBuf **)v78, *((_DWORD *)this + 109));
    ObjectCnt = CRidable::GetObjectCnt((CBattle_Field *)((char *)this + 208));
    InterfacePacketBuf::put_byte((PacketBuf **)v78, ObjectCnt);
    std::vector<std::pair<int,int>>::vector(v65);
    CRidable::GetObjectPos((int)this + 208, (int)v65);
    for ( n = 0; ; ++n )
    {
      v23 = std::vector<std::pair<int,int>>::size(v65);
      if ( v23 <= n )
        break;
      v21 = (_DWORD *)std::vector<std::pair<int,int>>::operator[](v65, n);
      InterfacePacketBuf::put_byte((PacketBuf **)v78, *v21);
      v22 = std::vector<std::pair<int,int>>::operator[](v65, n);
      InterfacePacketBuf::put_byte((PacketBuf **)v78, *(_DWORD *)(v22 + 4));
    }
    Manager = 0;
    CharacExpandData = 0;
    Layer = 0;
    if ( *(_DWORD *)this )
    {
      Manager = CParty::getManager(*(CParty **)this);
      if ( Manager )
      {
        CharacExpandData = CUser::GetCharacExpandData(Manager, 0xDu);
        TOD_UserState::getTodayEnterLayer((TOD_UserState *)v74, CharacExpandData);
        Layer = TOD_Layer::GetLayer((TOD_Layer *)v74);
      }
    }
    InterfacePacketBuf::put_short((PacketBuf **)v78, Layer);
    if ( *(_DWORD *)this )
    {
      v24 = CParty::get_random_buff_type(*(CParty **)this);
      InterfacePacketBuf::put_byte((PacketBuf **)v78, v24);
    }
    else
    {
      InterfacePacketBuf::put_byte((PacketBuf **)v78, 11);
    }
    InterfacePacketBuf::finalize((PacketBuf **)v78, 1);
    std::vector<std::pair<int,int>>::~vector(v65);
  }
  else if ( a2 == 29 )
  {
    InterfacePacketBuf::put_header((PacketBuf **)v78, 0, 29);
    v69 = -1;
    v68 = -1;
    CBattle_Field::getCurPosXY(this, &v69, &v68);
    InterfacePacketBuf::put_byte((PacketBuf **)v78, v69);
    InterfacePacketBuf::put_byte((PacketBuf **)v78, v68);
    if ( *(_BYTE *)(CBattle_Field::GetCurrentMapInfo(this) + 232) )
      InterfacePacketBuf::put_byte((PacketBuf **)v78, 1);
    else
      InterfacePacketBuf::put_byte((PacketBuf **)v78, 0);
    v3 = CBattle_Field::GetCurrentMapInfo(this);
    InterfacePacketBuf::put_int((PacketBuf **)v78, *(_DWORD *)(v3 + 72));
    InterfacePacketBuf::put_byte((PacketBuf **)v78, *((_BYTE *)this + 460));
    InterfacePacketBuf::put_byte((PacketBuf **)v78, *((_BYTE *)this + 461));
    if ( *(_BYTE *)(CBattle_Field::GetCurrentMapInfo(this) + 1) )
    {
      InterfacePacketBuf::put_byte((PacketBuf **)v78, 0);
      CBattle_Field::CheckAppearRidableObject(this, (PacketBuf **)v78);
      if ( *((_DWORD *)this + 98) && (unsigned __int8)CDungeon::IsEnterEachMap(*((CDungeon **)this + 98)) )
        InterfacePacketBuf::put_byte((PacketBuf **)v78, *((_DWORD *)this + 69));
      else
        InterfacePacketBuf::put_byte((PacketBuf **)v78, 255);
      InterfacePacketBuf::finalize((PacketBuf **)v78, 1);
    }
    else
    {
      v5 = CBattle_Field::GetCurrentMapInfo(this);
      InterfacePacketBuf::put_byte((PacketBuf **)v78, *(_BYTE *)(v5 + 68));
      v6 = CBattle_Field::GetCurrentMapInfo(this);
      InterfacePacketBuf::put_short((PacketBuf **)v78, *(_DWORD *)(v6 + 4));
      v7 = CBattle_Field::GetCurrentMapInfo(this);
      v8 = std::map<int,map_monster>::size(v7 + 12);
      InterfacePacketBuf::put_byte((PacketBuf **)v78, v8);
      map_monster::map_monster((map_monster *)&v49);
      v9 = CBattle_Field::GetCurrentMapInfo(this);
      std::map<int,map_monster>::begin(&v67, v9 + 12);
      while ( 1 )
      {
        v11 = CBattle_Field::GetCurrentMapInfo(this);
        std::map<int,map_monster>::end(&v70, v11 + 12);
        if ( !std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator!=(&v67, &v70) )
          break;
        v10 = std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator->(&v67);
        map_monster::operator=((int)&v49, v10 + 4);
        InterfacePacketBuf::put_short((PacketBuf **)v78, v57);
        InterfacePacketBuf::put_int((PacketBuf **)v78, v49);
        InterfacePacketBuf::put_short((PacketBuf **)v78, v50);
        InterfacePacketBuf::put_int((PacketBuf **)v78, v52);
        InterfacePacketBuf::put_byte((PacketBuf **)v78, v53);
        if ( v56 )
          InterfacePacketBuf::put_byte((PacketBuf **)v78, 3);
        else
          InterfacePacketBuf::put_byte((PacketBuf **)v78, v51);
        InterfacePacketBuf::put_byte((PacketBuf **)v78, v54);
        InterfacePacketBuf::put_byte((PacketBuf **)v78, v55);
        std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator++(dest, &v67);
      }
      v12 = CBattle_Field::GetCurrentMapInfo(this);
      v13 = std::map<int,map_item>::size(v12 + 36);
      InterfacePacketBuf::put_byte((PacketBuf **)v78, v13);
      map_item::map_item((map_item *)v58);
      v14 = CBattle_Field::GetCurrentMapInfo(this);
      std::map<int,map_item>::begin(&v66, v14 + 36);
      while ( 1 )
      {
        v18 = CBattle_Field::GetCurrentMapInfo(this);
        std::map<int,map_item>::end(&v72, v18 + 36);
        if ( !std::_Rb_tree_iterator<std::pair<int const,map_item>>::operator!=(&v66, &v72) )
          break;
        qmemcpy(
          v58,
          (const void *)(std::_Rb_tree_iterator<std::pair<int const,map_item>>::operator->(&v66) + 4),
          sizeof(v58));
        InterfacePacketBuf::put_byte((PacketBuf **)v78, SBYTE1(v58[0]));
        InterfacePacketBuf::put_short((PacketBuf **)v78, v58[1]);
        InterfacePacketBuf::put_int((PacketBuf **)v78, *(int *)((char *)&v58[4] + 2));
        add_info = Inven_Item::get_add_info((Inven_Item *)&v58[4]);
        InterfacePacketBuf::put_int((PacketBuf **)v78, add_info);
        InterfacePacketBuf::put_short((PacketBuf **)v78, *(__int16 *)((char *)&v58[6] + 3));
        AbilityType = stAmplifyOption_t::getAbilityType((stAmplifyOption_t *)((char *)&v58[8] + 1));
        InterfacePacketBuf::put_byte((PacketBuf **)v78, AbilityType);
        AbilityValue = stAmplifyOption_t::getAbilityValue((stAmplifyOption_t *)((char *)&v58[8] + 1));
        InterfacePacketBuf::put_short((PacketBuf **)v78, AbilityValue);
        std::_Rb_tree_iterator<std::pair<int const,map_item>>::operator++(v73, &v66);
      }
      InterfacePacketBuf::put_byte((PacketBuf **)v78, *((_BYTE *)this + 440));
      CBattle_Field::CheckAppearRidableObject(this, (PacketBuf **)v78);
      if ( *((_DWORD *)this + 98) && (unsigned __int8)CDungeon::IsEnterEachMap(*((CDungeon **)this + 98)) )
        InterfacePacketBuf::put_byte((PacketBuf **)v78, *((_DWORD *)this + 69));
      else
        InterfacePacketBuf::put_byte((PacketBuf **)v78, 255);
      InterfacePacketBuf::finalize((PacketBuf **)v78, 1);
      map_monster::~map_monster((void **)&v49);
    }
  }
}

int __cdecl CBattle_Field::check_grid_clear(CBattle_Field *this)
{
  int CurrentMapInfo; // eax
  int id; // eax
  int id; // ebx
  int id; // eax
  void *v6[4]; // [esp+18h] [ebp-360h] BYREF
  char flag; // [esp+2Bh] [ebp-34Dh]
  int id; // [esp+44h] [ebp-334h]
  std::_Rb_tree_node_base *v9; // [esp+364h] [ebp-14h] BYREF
  int id; // [esp+368h] [ebp-10h] BYREF
  _BYTE dest[12]; // [esp+36Ch] [ebp-Ch] BYREF

  map_monster::map_monster((map_monster *)v6);
  CurrentMapInfo = CBattle_Field::GetCurrentMapInfo(this);
  std::map<int,map_monster>::begin(&v9, CurrentMapInfo + 12);
  while ( 1 )
  {
    v4 = CBattle_Field::GetCurrentMapInfo(this);
    std::map<int,map_monster>::end(&v10, v4 + 12);
    if ( !std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator!=(&v9, &v10) )
      break;
    v2 = std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator->(&v9);
    map_monster::operator=((int)v6, v2 + 4);
    if ( v8 == 100 && !v7 )
    {
      v3 = 0;
      goto LABEL_8;
    }
    std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator++(dest, &v9);
  }
  v3 = 1;
LABEL_8:
  map_monster::~map_monster(v6);
  return v3;
}

  CBattle_Field::reset_blood_values(this);
  CBattle_Field::CBloodRound::reset_blood_values((CBattle_Field *)((char *)this + 684));

  return CBattle_Field::SetCurSeatNo(this, 255);
}

//----- (0830A57C) --------------------------------------------------------
bool __cdecl CBattle_Field::reset_hell_party_value(CBattle_Field *this)
{

int __cdecl CBattle_Field::GetFieldMob(CBattle_Field *this, int id, map_monster *a3)
{
  int CurrentMapInfo; // eax
  int id; // eax
  int id; // eax
  _DWORD value[4]; // [esp+1Ch] [ebp-36Ch] BYREF
  int id; // [esp+2Ch] [ebp-35Ch] BYREF
  int id; // [esp+30h] [ebp-358h] BYREF
  void *v10[213]; // [esp+34h] [ebp-354h] BYREF

  std::_Rb_tree_iterator<std::pair<int const,map_monster>>::_Rb_tree_iterator(&v8);
  CurrentMapInfo = CBattle_Field::GetCurrentMapInfo(this);
  std::map<int,map_monster>::find(v7, CurrentMapInfo + 12, &a2);
  v8 = v7[0];
  v4 = CBattle_Field::GetCurrentMapInfo(this);
  std::map<int,map_monster>::end(&v9, v4 + 12);
  if ( std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator==(&v8, &v9) )
    return 0;
  v6 = std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator->(&v8);
  map_monster::map_monster((map_monster *)v10, (const map_monster *)(v6 + 4));
  map_monster::operator=((int)a3, (int)v10);
  map_monster::~map_monster(v10);
  return 1;
}

int __cdecl CBattle_Field::convertAPCRoleType2MonsterRoleType(CBattle_Field *this, unsigned __int8 a2)
{
  if ( a2 == 6 )
    return 1;
  if ( (char)a2 > 6 )
  {
    if ( a2 == 7 )
      return 2;
    if ( a2 == 8 )
      return 3;
  }
  else if ( a2 == 5 )
  {
    return 0;
  }
  return a2;
}

CParty *__cdecl CBattle_Field::check_random_appear_hell_dungeon(CParty **a1, CDungeon *dungeon, int id, int id2, _BYTE *a5)
{
  CParty *result; // eax
  CUserCharacInfo *user; // eax
  CUserCharacInfo *charac_info; // eax
  int id; // eax
  int id; // eax
  int id; // [esp+1Ch] [ebp-1Ch]
  int id; // [esp+20h] [ebp-18h]
  int id; // [esp+24h] [ebp-14h]
  int id; // [esp+28h] [ebp-10h]
  int i; // [esp+2Ch] [ebp-Ch]

  result = (CParty *)*((_DWORD *)*a1 + 1562);
  if ( result )
  {
    result = (CParty *)CBattle_Field::IsEnableHellDungeon();
    if ( (_BYTE)result )
    {
      result = (CParty *)*((_DWORD *)*a1 + 1562);
      if ( result )
      {
        result = (CParty *)CWorldMap::IsInHellDungeon(*((CWorldMap **)*a1 + 1562));
        if ( (_BYTE)result )
        {
          v10 = 1;
          v11 = 200;
          v13 = 0;
          for ( i = 0; i <= 3; ++i )
          {
            if ( (unsigned __int8)CParty::checkValidUser((CUser **)*a1, i) == 1 )
            {
              user = (CUserCharacInfo *)CParty::get_user(*a1, i);
              if ( CUserCharacInfo::getCurCharacR(user) )
              {
                v7 = (CUserCharacInfo *)CParty::get_user(*a1, i);
                v12 = *(__int16 *)(CUserCharacInfo::getCurCharacR(v7) + 39);
              }
              else
              {
                v12 = 1;
              }
              if ( v10 <= v12 )
                v8 = v12;
              else
                v8 = v10;
              v10 = v8;
              if ( v11 >= v12 )
                v9 = v12;
              else
                v9 = v11;
              v11 = v9;
              ++v13;
            }
          }
          if ( v10 - CDungeon::get_standard_level(a2) <= 5 )
          {
            if ( v13 <= 1 )
            {
              *a5 = 1;
            }
            else if ( v10 - v11 <= 8 )
            {
              *a5 = 1;
            }
          }
          if ( v10 - v11 > 8 && v13 > 1
            || *((_BYTE *)*a1 + 6244) != 1 && a4 == 1
            || (result = (CParty *)(*((unsigned __int8 *)*a1 + 6245) ^ 1), (_BYTE)result) && a4 == 1 )
          {
            result = *a1;
            *((_DWORD *)*a1 + 1562) = 0;
          }
        }
      }
    }
    if ( a3 == 1 )
    {
      *((_DWORD *)*a1 + 1562) = 0;
      *a5 = 0;
      return (CParty *)a5;
    }
  }
  return result;
}

int __cdecl CBattle_Field::getItemDropDungeonType(CBattle_Field *this)
{
  if ( !*((_DWORD *)this + 98) )
    return 3;
  if ( (char)CDungeon::get_dimension_possible(*((CDungeon **)this + 98)) <= 0 )
    return *(_BYTE *)(*((_DWORD *)this + 98) + 2204) != 0;
  return 2;
}

int __cdecl CBattle_Field::getRandomBuffDungeonType(CBattle_Field *this, int index)
{
  CDungeonList **v3; // eax
  CGameManager *v4; // eax
  QuickParty::CQuickPartyRewardManager *QuickPartyRewardManager; // eax
  char *dungeon_idx_list; // eax
  CGameManager *v7; // eax
  QuickParty::CQuickPartyRewardManager *v8; // eax
  char *v9; // eax
  int id; // [esp+14h] [ebp-14h] BYREF
  int id; // [esp+18h] [ebp-10h] BYREF
  CDungeon *dungeon; // [esp+1Ch] [ebp-Ch]

  if ( !index )
  {
    if ( !*((_DWORD *)this + 98) )
      return 7;
    index = CDungeon::get_index(*((CDungeon **)this + 98));
  }
  v3 = (CDungeonList **)G_CDataManager();
  dungeon = (CDungeon *)CDataManager::find_dungeon(v3, index);
  if ( !dungeon )
    return 7;
  v4 = G_CGameManager();
  QuickPartyRewardManager = (QuickParty::CQuickPartyRewardManager *)CGameManager::GetQuickPartyRewardManager(v4);
  dungeon_idx_list = QuickParty::CQuickPartyRewardManager::get_dungeon_idx_list(QuickPartyRewardManager);
  std::vector<int>::begin(&v13, dungeon_idx_list);
  v7 = G_CGameManager();
  v8 = (QuickParty::CQuickPartyRewardManager *)CGameManager::GetQuickPartyRewardManager(v7);
  v9 = QuickParty::CQuickPartyRewardManager::get_dungeon_idx_list(v8);
  std::vector<int>::end(&v12, (int)v9);
  while ( __gnu_cxx::operator!=<int const*,std::vector<int>>((int)&v13, (int)&v12) )
  {
    if ( *(_DWORD *)__gnu_cxx::__normal_iterator<int const*,std::vector<int>>::operator*((int)&v13) == index )
      return 7;
    __gnu_cxx::__normal_iterator<int const*,std::vector<int>>::operator++(&v13);
  }
  if ( (char)CDungeon::get_dimension_possible(dungeon) > 0 )
    return 2;
  if ( *((_BYTE *)dungeon + 2204) )
    return 1;
  if ( (unsigned __int8)CDungeon::get_blood_dungeon_type(dungeon) == 1 )
    return 4;
  if ( (unsigned __int8)CDungeon::get_blood_dungeon_type(dungeon) == 2 )
    return 5;
  if ( CDungeon::getDungeonKind(dungeon) == 1 && (unsigned __int8)CDungeon::limitOfStackableItemInTower(dungeon) )
    return 3;
  if ( CDungeon::getDungeonKind(dungeon) == 1 && (unsigned __int8)CDungeon::limitOfStackableItemInTower(dungeon) != 1 )
    return 6;
  else
    return 0;
}

int __cdecl CBattle_Field::setEplpWaitingTimer(int *a1, int value)
{
  int result; // eax
  CUser *Manager; // eax
  int value; // esi
  int Idx; // ebx
  TimerQueue *v6; // eax
  int value; // esi
  int value; // ebx
  TimerQueue *v9; // eax
  int value; // esi
  int PartyIndex; // ebx
  TimerQueue *v12; // eax
  WongWork::CDeathTower *v13; // [esp+2Ch] [ebp-Ch]

  result = *a1;
  if ( *a1 )
  {
    if ( a2 > 5 )
    {
      if ( a2 != 6 )
      {
LABEL_10:
        v10 = CParty::gen_timer_key(*a1, 21);
        PartyIndex = CParty::GetPartyIndex((CParty *)*a1);
        v12 = G_TimerQueue();
        return TimerQueue::InsertTimer(v10, v12, 1, PartyIndex, 21, 30, v10, 21);
      }
    }
    else
    {
      if ( a2 >= 4 )
      {
        v7 = CParty::gen_timer_key(*a1, 37);
        v8 = CParty::GetPartyIndex((CParty *)*a1);
        v9 = G_TimerQueue();
        return TimerQueue::InsertTimer(v7, v9, 1, v8, 37, 30, v7, 37);
      }
      if ( a2 != 3 )
        goto LABEL_10;
    }
    Manager = (CUser *)CParty::getManager((CParty *)*a1);
    result = CUser::getDeathTower(Manager);
    v13 = (WongWork::CDeathTower *)result;
    if ( result )
    {
      v4 = WongWork::CDeathTower::genTimerKey(result, 77);
      Idx = WongWork::CDeathTower::getIdx(v13);
      v6 = G_TimerQueue();
      return TimerQueue::InsertTimer(v4, v6, 6, Idx, 77, 30, v4, 77);
    }
  }
  return result;
}

void __cdecl CBattle_Field::_MakeDropItems(
        CDungeon **a1,

  v81 = CBattle_Field::convertAPCRoleType2MonsterRoleType((CBattle_Field *)a1, *(_BYTE *)(a4 + 8));
  WongWork::stGenerateRefData_t::stGenerateRefData_t((WongWork::stGenerateRefData_t *)v60);

  HIBYTE(DropItems) = CBattle_Field::get_dungeon_diff((CBattle_Field *)a1);
  if ( (char)CDungeon::get_dimension_possible(a1[98]) <= 0 )

  ItemDropDungeonType = CBattle_Field::getItemDropDungeonType((CBattle_Field *)a1);
  std::vector<bool>::operator=((int)v61, a10);

  CBattle_Field::setPremiumDropRate(a1, (int)v60, a12);
  v74 = *((_BYTE *)a1[98] + 2207) > 0;

  BYTE2(DropItems) = CBattle_Field::get_dungeon_diff((CBattle_Field *)a1);
  v68 = *(_DWORD *)(a4 + 12);

int __cdecl CBattle_Field::setPremiumDropRate(CDungeon **a1, int value, int value2)
{
  if ( (char)CDungeon::get_blood_dungeon_type(a1[98]) > 0
    || (unsigned __int8)CBattle_Field::getItemDropDungeonType((CBattle_Field *)a1)
    || CDungeon::isTowerOfDespairDungeon(a1[98])
    || *(_BYTE *)(a2 + 64) )
  {
    *(_DWORD *)(a2 + 32) = 0;
    return a2;
  }
  else
  {
    if ( a3 )
      *(_DWORD *)(a2 + 32) = a3;
    else
      *(_DWORD *)(a2 + 32) = -1;
    return a2;
  }
}

void __cdecl CBattle_Field::_MakeNotiPacketDieMonster(
        CBattle_Field *battlefield,

  if ( (unsigned __int8)CBattle_Field::convertAPCRoleType2MonsterRoleType(a1, *(_BYTE *)(a6 + 8)) )
    InterfacePacketBuf::put_byte(a2, *((_DWORD *)a1 + 106));

int __cdecl CBattle_Field::kill_monster(
        CBattle_Field *this,

  CurrentMapInfo = (MapInfo *)CBattle_Field::GetCurrentMapInfo(this);
  std::map<int,map_monster>::find(&v82, (int)CurrentMapInfo + 12, &v84);

      v7 = CBattle_Field::GetCurrentMapInfo(this);
      LogManager::logFormat(

        "bool CBattle_Field::kill_monster(PacketGuard&, map_monster&, uint&, const STKillMonsterData&)",
        (const char *)0x1BA7,

    CBattle_Field::KillHellPartyGroupMonsterCnt(this, CurrentMapInfo, (map_monster *)(v14 + 4), &v81);
  }
  if ( *(char *)(v102 + 8) > 3 )
  {

        "bool CBattle_Field::kill_monster(PacketGuard&, map_monster&, uint&, const STKillMonsterData&)",
        (const char *)0x1C03,

        "bool CBattle_Field::kill_monster(PacketGuard&, map_monster&, uint&, const STKillMonsterData&)",
        (const char *)0x1BFD,

          "bool CBattle_Field::kill_monster(PacketGuard&, map_monster&, uint&, const STKillMonsterData&)",
          (const char *)0x1BF3,

          "bool CBattle_Field::kill_monster(PacketGuard&, map_monster&, uint&, const STKillMonsterData&)",
          (const char *)0x1BED,

    v29 = CBattle_Field::get_dungeon_diff(this);
    if ( !CKillMonsterInfo::CheckBossKill(v115, (int)a3[3], v29) )

      v30 = CBattle_Field::get_dungeon_diff(this);
      if ( !(unsigned __int8)CKillMonsterInfo::RegisterBossKill(v115, a3[3], v30) )

    v34 = CBattle_Field::get_dungeon_diff(this);
    if ( !CKillMonsterInfo::CheckNamedKill(v115, (int)a3[3], v34) )

      v35 = CBattle_Field::get_dungeon_diff(this);
      if ( (unsigned __int8)CKillMonsterInfo::RegisterNamedKill(v115, a3[3], v35) )

        v36 = CBattle_Field::get_dungeon_diff(this);
        CParty::MemberRegisterKillMonster(*(CParty **)this, a3, v36);

  v32 = CBattle_Field::get_dungeon_diff(this);
  if ( CKillMonsterInfo::CheckAPCBossKill(v115, (int)a3[3], v32) )

    v37 = CBattle_Field::get_dungeon_diff(this);
    CParty::MemberRegisterKillMonster(*(CParty **)this, a3, v37);

  v33 = CBattle_Field::get_dungeon_diff(this);
  if ( (unsigned __int8)CKillMonsterInfo::RegisterAPCBossKill(v115, a3[3], v33) )

    v31 = CBattle_Field::get_dungeon_diff(this);
    CParty::MemberRegisterKillMonster(*(CParty **)this, a3, v31);

    CBattle_Field::get_dungeon_diff(this);
    CBattle_Field::get_dungeon_index(this);

      "bool CBattle_Field::kill_monster(PacketGuard&, map_monster&, uint&, const STKillMonsterData&)",
      7271,

    CBattle_Field::_MakeDropItems(
      (CDungeon **)this,

  if ( (char)CBattle_Field::convertAPCRoleType2MonsterRoleType(this, *(_BYTE *)(v102 + 8)) == 1 && rand() % 100 > 0 )
    ++*((_DWORD *)this + 106);

  CBattle_Field::_MakeNotiPacketDieMonster(this, a2, v80, (__int16)v84, v96, v102, (int)a3);
  v47 = *(unsigned __int8 *)(v102 + 16);

          v52 = CBattle_Field::get_dungeon_diff(this);
          index = CDungeon::get_index(*((CDungeon **)this + 98));

              "bool CBattle_Field::kill_monster(PacketGuard&, map_monster&, uint&, const STKillMonsterData&)",
              (const char *)0x1D04,

    CBattle_Field::send_elevator_time_packet(this, *((_BYTE *)this + 312));
  }
  std::map<int,map_monster>::erase((int)CurrentMapInfo + 12, v82);
  if ( *(char *)(*((_DWORD *)this + 98) + 2207) > 0 )
  {

    CBattle_Field::checkKilledSpawnBloodMonsters(this, v60);
  }
  if ( !stMapMonsterKillChecker_t::getUseSkillMaterialTime((MapInfo *)((char *)CurrentMapInfo + 76))
    && v97 == 1
    && !CBattle_Field::IsClearDungeon(this) )
  {

    && !CBattle_Field::IsClearDungeon(this) )
  {
    v65 = CSystemTime::getCurTickCount((CSystemTime *)GlobalData::s_systemTime_);
    if ( stMapMonsterKillChecker_t::dieMonster((MapInfo *)((char *)CurrentMapInfo + 100), v65, 0xBB8u) == 3 )
    {
      stMapMonsterKillChecker_t::initMonsterInfo((MapInfo *)((char *)CurrentMapInfo + 100));
      for ( ii = 0; ii <= 3; ++ii )
      {
        if ( (unsigned __int8)CParty::checkValidUser(*(CUser ***)this, ii) )
        {
          v130 = (CUser *)CParty::get_user(*(CParty **)this, ii);
          if ( v130 )
          {
            v66 = CUser::getHackAnalyzer(v130);
            WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)v66, v130, 0xD7u, 1, 0, 0);
            break;
          }
        }
      }
    }
  }

int __cdecl CBattle_Field::kill_blood_monster(CBattle_Field *this, CUser *user, int id, unsigned __int16 *a4)
{
  int id; // eax
  int id; // eax
  void *v7[209]; // [esp+1Ch] [ebp-36Ch] BYREF
  char flag; // [esp+360h] [ebp-28h]
  std::_Rb_tree_node_base *v9; // [esp+368h] [ebp-20h] BYREF
  int id; // [esp+36Ch] [ebp-1Ch] BYREF
  int id; // [esp+370h] [ebp-18h] BYREF
  char flag; // [esp+377h] [ebp-11h]
  int CurrentMapInfo; // [esp+378h] [ebp-10h]
  int i; // [esp+37Ch] [ebp-Ch]

  if ( !*((_DWORD *)this + 98) )
    return 0;
  if ( !*(_BYTE *)(*((_DWORD *)this + 98) + 2207) )
    return 0;
  v12 = 0;
  CurrentMapInfo = CBattle_Field::GetCurrentMapInfo(this);
  for ( i = 0; i < a3; ++i )
  {
    v10 = a4[i];
    std::map<int,map_monster>::find(&v9, CurrentMapInfo + 12, &v10);
    std::map<int,map_monster>::end(&v11, CurrentMapInfo + 12);
    if ( !std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator==(&v9, &v11) )
    {
      v5 = std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator->(&v9);
      map_monster::map_monster((map_monster *)v7, (const map_monster *)(v5 + 4));
      if ( v8 == 1 )
      {
        std::map<int,map_monster>::erase(CurrentMapInfo + 12, v9);
        v12 = 1;
      }
      map_monster::~map_monster(v7);
    }
  }
  if ( v12 )
  {
    v6 = std::map<int,map_monster>::size(CurrentMapInfo + 12);
    CBattle_Field::checkKilledSpawnBloodMonsters(this, v6);
  }
  return 1;
}

void __cdecl CBattle_Field::KillHellPartyGroupMonsterCnt(CBattle_Field *this, MapInfo *a2, map_monster *a3, bool *a4)
{
  int id; // eax
  std::_Rb_tree_node_base *v5; // [esp+24h] [ebp-14h] BYREF
  int id; // [esp+28h] [ebp-10h] BYREF
  _DWORD value[3]; // [esp+2Ch] [ebp-Ch] BYREF

  if ( !*((_BYTE *)this + 458) )
  {
    v6 = *((unsigned __int16 *)a3 + 411);
    std::map<int,int>::find(&v5, (int)a2 + 140, &v6);
    std::map<int,int>::end(v7, (int)a2 + 140);
    if ( std::_Rb_tree_iterator<std::pair<int const,int>>::operator==(&v5, v7) )
    {
      LogManager::logFormat(
        (LogManager *)1,
        (int)"battle_field.cpp",
        "void CBattle_Field::KillHellPartyGroupMonsterCnt(MapInfo&, map_monster&, bool&)",
        (const char *)0x1DD9,
        "HELLPARTY_LOG : HELLPARTY GROUP INDEX (%d), MAP INDEX (%d) : KILL HELL GROUP ERROR!!\n",
        (const char *)*((unsigned __int16 *)a3 + 411),
        *((_DWORD *)a2 + 1));
    }
    else
    {
      *((_BYTE *)a3 + 8) = 0;
      if ( *(_DWORD *)(std::_Rb_tree_iterator<std::pair<int const,int>>::operator->(&v5) + 4) )
      {
        v4 = std::_Rb_tree_iterator<std::pair<int const,int>>::operator->(&v5);
        --*(_DWORD *)(v4 + 4);
        *a4 = 0;
      }
      else
      {
        *a4 = 1;
        std::map<int,int>::erase((int)a2 + 140, v5);
        if ( !std::map<int,int>::size((int)a2 + 140) )
          *((_BYTE *)this + 458) = 1;
      }
    }
  }
}

void __cdecl CBattle_Field::send_elevator_time_packet(CBattle_Field *this, char flag)
{
  PacketBuf *v2[5]; // [esp+24h] [ebp-14h] BYREF

  PacketGuard::PacketGuard((PacketGuard *)v2);
  InterfacePacketBuf::put_header(v2, 0, 253);
  InterfacePacketBuf::put_byte(v2, *((_BYTE *)this + 313));
  InterfacePacketBuf::put_byte(v2, a2);
  InterfacePacketBuf::finalize(v2, 1);
  CParty::send_to_party(*(CUser ***)this, (PacketGuard *)v2);
  PacketGuard::~PacketGuard(v2);
}

void __cdecl CBattle_Field::check_elevator_timer(CBattle_Field *this)
{
  int id; // esi
  int PartyIndex; // ebx
  TimerQueue *v3; // eax

  if ( (char)++*((_BYTE *)this + 313) <= 3 && *((_BYTE *)this + 312) == 2 )
  {
    v1 = CParty::gen_timer_key(*(_DWORD *)this, 40);
    PartyIndex = CParty::GetPartyIndex(*(CParty **)this);
    v3 = G_TimerQueue();
    TimerQueue::InsertTimer(v1, v3, 1, PartyIndex, 40, 15, v1, 0);
  }
  if ( *((_BYTE *)this + 312) == 2 )
    CBattle_Field::send_elevator_time_packet(this, 0);
}

int __cdecl CBattle_Field::get_field_item(CBattle_Field *this, int id, map_item *a3)
{
  int CurrentMapInfo; // eax
  int id; // eax
  int id; // eax
  _DWORD value[3]; // [esp+1Ch] [ebp-2Ch] BYREF
  int id; // [esp+28h] [ebp-20h] BYREF
  _DWORD value[7]; // [esp+2Ch] [ebp-1Ch] BYREF

  std::_Rb_tree_iterator<std::pair<int const,map_item>>::_Rb_tree_iterator(&v8);
  CurrentMapInfo = CBattle_Field::GetCurrentMapInfo(this);
  std::map<int,map_item>::find(v7, CurrentMapInfo + 36, &a2);
  v8 = v7[0];
  v4 = CBattle_Field::GetCurrentMapInfo(this);
  std::map<int,map_item>::end(v9, v4 + 36);
  if ( std::_Rb_tree_iterator<std::pair<int const,map_item>>::operator==(&v8, v9) )
    return 0;
  v6 = std::_Rb_tree_iterator<std::pair<int const,map_item>>::operator->(&v8);
  qmemcpy(a3, (const void *)(v6 + 4), 0x54u);
  return 1;
}

_BOOL4 CBattle_Field::is_last_field_item(CBattle_Field *this, ...)
{
  int LastMapInfo; // eax
  int id; // eax
  _DWORD value[3]; // [esp+1Ch] [ebp-1Ch] BYREF
  int id; // [esp+28h] [ebp-10h] BYREF
  _DWORD value[3]; // [esp+2Ch] [ebp-Ch] BYREF
  va_list va; // [esp+44h] [ebp+Ch] BYREF

  va_start(va, this);
  std::_Rb_tree_iterator<std::pair<int const,map_item>>::_Rb_tree_iterator(&v5);
  LastMapInfo = CBattle_Field::GetLastMapInfo(this);
  std::map<int,map_item>::find(v4, LastMapInfo + 36, (int *)va);
  v5 = v4[0];
  v2 = CBattle_Field::GetLastMapInfo(this);
  std::map<int,map_item>::end(v6, v2 + 36);
  return !std::_Rb_tree_iterator<std::pair<int const,map_item>>::operator==(&v5, v6);
}

int CBattle_Field::pickup_item(CBattle_Field *this, ...)
{
  std::_Rb_tree_node_base *v2; // [esp+1Ch] [ebp-1Ch] BYREF
  std::_Rb_tree_node_base *v3; // [esp+24h] [ebp-14h] BYREF
  int id; // [esp+28h] [ebp-10h] BYREF
  int CurrentMapInfo; // [esp+2Ch] [ebp-Ch]
  va_list va; // [esp+44h] [ebp+Ch] BYREF

  va_start(va, this);
  CurrentMapInfo = CBattle_Field::GetCurrentMapInfo(this);
  std::_Rb_tree_iterator<std::pair<int const,map_item>>::_Rb_tree_iterator(&v3);
  std::map<int,map_item>::find(&v2, CurrentMapInfo + 36, (int *)va);
  v3 = v2;
  std::map<int,map_item>::end(&v4, CurrentMapInfo + 36);
  if ( std::_Rb_tree_iterator<std::pair<int const,map_item>>::operator==(&v3, &v4) )
    return 21;
  std::map<int,map_item>::erase(CurrentMapInfo + 36, v3);
  return 0;
}

int __cdecl CBattle_Field::drop_item(CBattle_Field *battlefield, int id, int value)
{
  char *v4[21]; // [esp+4h] [ebp-84h] BYREF
  int CurrentMapInfo; // [esp+68h] [ebp-20h]
  int id; // [esp+6Ch] [ebp-1Ch]

  CurrentMapInfo = CBattle_Field::GetCurrentMapInfo(a1);
  v6 = *(_DWORD *)(CurrentMapInfo + 64);
  *(_DWORD *)(CurrentMapInfo + 64) = v6 + 1;
  a3 = v6;
  qmemcpy(v4, &a2, sizeof(v4));
  MapInfo::Add_Item(CurrentMapInfo, (char)v4[0], v4[1]);
  return v6;
}

_DWORD *__cdecl CBattle_Field::onUseItem(CBattle_Field *this, CUser *user, const Inven_Item *item)
{
  unsigned int CurTickCount; // eax
  unsigned int count; // eax
  _DWORD *result; // eax
  _DWORD *CurrentMapInfo; // [esp+1Ch] [ebp-Ch]

  CurrentMapInfo = (_DWORD *)CBattle_Field::GetCurrentMapInfo(this);
  if ( *(_DWORD *)((char *)a3 + 2) == 3037 )
  {
    CurTickCount = CSystemTime::getCurTickCount((CSystemTime *)&GlobalData::s_systemTime_);
    stMapMonsterKillChecker_t::setUseSkillMaterial((stMapMonsterKillChecker_t *)(CurrentMapInfo + 19), CurTickCount);
    v4 = CSystemTime::getCurTickCount((CSystemTime *)&GlobalData::s_systemTime_);
    stMapMonsterKillChecker_t::setUseSkillMaterial((stMapMonsterKillChecker_t *)(CurrentMapInfo + 25), v4);
  }
  result = CurrentMapInfo;
  if ( *((_BYTE *)a3 + 1) == 2 )
    ++CurrentMapInfo[33];
  else
    ++CurrentMapInfo[34];
  return result;
}

int __cdecl CBattle_Field::onStartMap(CBattle_Field *this)
{
  int CurrentMapInfo; // ebx
  int result; // eax

  CurrentMapInfo = CBattle_Field::GetCurrentMapInfo(this);
  result = CSystemTime::getCurTickCount((CSystemTime *)GlobalData::s_systemTime_);
  *(_DWORD *)(CurrentMapInfo + 124) = result;
  return result;
}

char __cdecl CBattle_Field::onClearMap(CBattle_Field *this, bool a2)
{
  int CurTickCount; // eax
  _DWORD *CurrentMapInfo; // [esp+28h] [ebp-10h]
  unsigned int id; // [esp+2Ch] [ebp-Ch]

  CurrentMapInfo = (_DWORD *)CBattle_Field::GetCurrentMapInfo(this);
  CurTickCount = CSystemTime::getCurTickCount((CSystemTime *)GlobalData::s_systemTime_);
  CurrentMapInfo[32] = CurTickCount;
  if ( a2 )
  {
    CurTickCount = *((_DWORD *)this + 101);
    if ( CurTickCount == 41 )
    {
      v5 = 120000;
      if ( CParty::get_member_count(*(CUser ***)this) > 1 )
        v5 = 120001;
      if ( CParty::checkBossRoom(*(CParty **)this)
        && stMapPlayInfo_t::getPlayTick((stMapPlayInfo_t *)(CurrentMapInfo + 31)) <= v5 )
      {
        CParty::enumPartyMember(*(CParty **)this, (bool (*)(CUser *))addUserHackCount149);
      }
      LOBYTE(CurTickCount) = CParty::checkBossRoom(*(CParty **)this);
      if ( (_BYTE)CurTickCount )
      {
        CurTickCount = CurrentMapInfo[33];
        if ( !CurTickCount )
        {
          CurTickCount = CurrentMapInfo[34];
          if ( !CurTickCount )
            LOBYTE(CurTickCount) = CParty::enumPartyMember(*(CParty **)this, (bool (*)(CUser *))addUserHackCount150);
        }
      }
    }
  }
  return CurTickCount;
}

int __cdecl CBattle_Field::GetTotalSkillMaterial(CBattle_Field *this)
{
  int id; // eax
  int id; // ebx
  std::_Rb_tree_node_base *v4; // [esp+1Ch] [ebp-10Ch] BYREF
  void *v5[17]; // [esp+28h] [ebp-100h] BYREF
  char flag; // [esp+6Dh] [ebp-BBh]
  int id; // [esp+74h] [ebp-B4h]
  std::_Rb_tree_node_base::_Base_ptr i; // [esp+114h] [ebp-14h] BYREF
  int id; // [esp+118h] [ebp-10h] BYREF
  int id; // [esp+11Ch] [ebp-Ch]

  MapInfo::MapInfo((MapInfo *)v5);
  v10 = 0;
  std::_Rb_tree_iterator<std::pair<int const,MapInfo>>::_Rb_tree_iterator(&i);
  std::map<int,MapInfo>::begin(&v4, (int)this + 120);
  for ( i = v4; ; std::_Rb_tree_iterator<std::pair<int const,MapInfo>>::operator++(&i) )
  {
    std::map<int,MapInfo>::end(&v9, (int)this + 120);
    if ( !std::_Rb_tree_iterator<std::pair<int const,MapInfo>>::operator!=(&i, &v9) )
      break;
    v1 = std::_Rb_tree_iterator<std::pair<int const,MapInfo>>::operator->(&i);
    MapInfo::operator=((int)v5, v1 + 4);
    if ( v6 != 1 && BYTE1(v5[0]) )
      v10 += v7;
  }
  v2 = v10;
  MapInfo::~MapInfo(v5);
  return v2;
}

int __cdecl CBattle_Field::getLayeredMapIndex(CBattle_Field *this)
{
  return *((_DWORD *)this + 67);
}

CBattle_Field *__cdecl CBattle_Field::setLayeredMapIndex(CBattle_Field *this)
{
  ++*((_DWORD *)this + 67);
  return this;
}

int __cdecl CBattle_Field::getLayeredMapKeyValue(CBattle_Field *this, int id)
{
  return 100 * a2 + MAX_LIMIT_10000;
}

int __cdecl CBattle_Field::getCurrentLayeredMapInfo(CBattle_Field *this)
{
  int LayeredMapIndex; // eax

  LayeredMapIndex = CBattle_Field::getLayeredMapIndex(this);
  return CBattle_Field::getLayeredMapInfoIndex(this, LayeredMapIndex);
}

int __cdecl CBattle_Field::getLayeredMapInfoIndex(CBattle_Field *this, int id)
{
  int id; // ebx
  _DWORD value[4]; // [esp+1Ch] [ebp-2FCh] BYREF
  void *v5[59]; // [esp+2Ch] [ebp-2ECh] BYREF
  int id; // [esp+118h] [ebp-200h] BYREF
  int id; // [esp+11Ch] [ebp-1FCh] BYREF
  int LayeredMapKeyValue; // [esp+120h] [ebp-1F8h] BYREF
  int id; // [esp+124h] [ebp-1F4h] BYREF
  _BYTE v10[8]; // [esp+128h] [ebp-1F0h] BYREF
  _DWORD value[60]; // [esp+130h] [ebp-1E8h] BYREF
  _DWORD value[62]; // [esp+220h] [ebp-F8h] BYREF

  LayeredMapKeyValue = CBattle_Field::getLayeredMapKeyValue(this, a2);
  std::map<int,MapInfo>::find(&v7, (int)this + 120, &LayeredMapKeyValue);
  std::map<int,MapInfo>::end(&v9, (int)this + 120);
  if ( std::_Rb_tree_iterator<std::pair<int const,MapInfo>>::operator!=(&v7, &v9) )
    return std::_Rb_tree_iterator<std::pair<int const,MapInfo>>::operator->(&v7) + 4;
  MapInfo::MapInfo((MapInfo *)v5);
  v6 = CBattle_Field::getLayeredMapKeyValue(this, a2);
  std::make_pair<int &,MapInfo &>(v12, (int)&v6, (int)v5);
  std::pair<int const,MapInfo>::pair<int,MapInfo>(v11, (int)v12);
  std::map<int,MapInfo>::insert((int)v10, (int)this + 120, (int)v11);
  std::pair<int const,MapInfo>::~pair((int)v11);
  std::pair<int,MapInfo>::~pair((int)v12);
  std::map<int,MapInfo>::find(v4, (int)this + 120, &v6);
  v7 = v4[0];
  v2 = std::_Rb_tree_iterator<std::pair<int const,MapInfo>>::operator->(&v7) + 4;
  MapInfo::~MapInfo(v5);
  return v2;
}

bool __cdecl CBattle_Field::setQuestMazeInfo(CBattle_Field *this)
{
  int value; // ebx
  QuestList **v4; // eax
  bool result; // al
  int value; // [esp+18h] [ebp-20h] BYREF
  int value; // [esp+1Ch] [ebp-1Ch] BYREF
  int i; // [esp+20h] [ebp-18h]
  _DWORD *QuestMazeScriptVector; // [esp+24h] [ebp-14h]
  int value; // [esp+28h] [ebp-10h]
  int quest; // [esp+2Ch] [ebp-Ch]

  for ( i = 0; ; ++i )
  {
    result = i <= 1;
    if ( i > 1 )
      break;
    QuestMazeScriptVector = (_DWORD *)CDungeon::GetQuestMazeScriptVector(*((_DWORD *)this + 98), i);
    if ( QuestMazeScriptVector && !std::vector<MazeScript>::empty(QuestMazeScriptVector) )
    {
      std::vector<MazeScript>::begin(&v7, QuestMazeScriptVector);
      std::vector<MazeScript>::end(&v6, (int)QuestMazeScriptVector);
      while ( __gnu_cxx::operator!=<MazeScript *,std::vector<MazeScript>>((int)&v7, (int)&v6) )
      {
        v10 = __gnu_cxx::__normal_iterator<MazeScript *,std::vector<MazeScript>>::operator*((int)&v7);
        if ( *((_DWORD *)this + 109) != 1
          && *(_DWORD *)this
          && (unsigned __int8)CParty::CheckQuestConnection(*(CUser ***)this, *(_DWORD *)(v10 + 80), i) )
        {
          v3 = *(_DWORD *)(v10 + 80);
          v4 = (QuestList **)G_CDataManager();
          quest = CDataManager::find_quest(v4, v3);
          if ( quest )
          {
            if ( *(_BYTE *)(quest + 288) != 1 || CParty::get_member_count(*(CUser ***)this) == 1 )
              CBattle_Field::setQuestMaze(this, 1);
          }
        }
        __gnu_cxx::__normal_iterator<MazeScript *,std::vector<MazeScript>>::operator++(&v7);
      }
    }
  }
  return result;
}

CBattle_Field *__cdecl CBattle_Field::setQuestMaze(CBattle_Field *this, bool a2)
{
  *((_BYTE *)this + 272) = a2;
  return this;
}

int __cdecl CBattle_Field::isQuestMaze(CBattle_Field *this)
{
  return *((unsigned __int8 *)this + 272);
}

int __cdecl CBattle_Field::checkLayeredMap(CBattle_Field *this, int id, int id2)
{
  int result; // eax
  int id; // ebx
  int GridR; // [esp+1Ch] [ebp-Ch]

  result = CBattle_Field::getMaze(this);
  if ( result )
  {
    GridR = MazeScript::getGridR((MazeScript *)result, a2, a3);
    result = GridR && std::vector<int>::size((_DWORD *)(GridR + 28));
    if ( (_BYTE)result )
    {
      v4 = std::vector<int>::size((_DWORD *)(GridR + 28));
      if ( v4 > CBattle_Field::getLayeredMapIndex(this) + 1 )
        CBattle_Field::setLayeredMapIndex(this);
      result = CBattle_Field::getCurrentLayeredMapInfo(this);
      *(_BYTE *)(result + 232) = 1;
    }
  }
  return result;
}

int `global constructor keyed to'CBattle_Field::SetTournamentCurRound()
{
  return __static_initialization_and_destruction_0(1, INVALID_ID_U16);
}

int __cdecl CBattle_Field::GetLastMapInfo(CBattle_Field *this)
{
  if ( *((_DWORD *)this + 98) && (unsigned __int8)CDungeon::IsEnterEachMap(*((CDungeon **)this + 98)) )
    return CBattle_Field::GetMapInfoFromPos(
             (MazeScript **)this,
             *((_DWORD *)this + 2 * *((_DWORD *)this + 69) + 15),
             *((_DWORD *)this + 2 * *((_DWORD *)this + 69) + 16),
             0);
  else
    return CBattle_Field::GetMapInfoFromPos((MazeScript **)this, *((_DWORD *)this + 26), *((_DWORD *)this + 27), 0);
}

int __cdecl CBattle_Field::getBloodMaxRound(CBattle_Field *this)
{
  if ( *((_DWORD *)this + 98) )
    return *(_DWORD *)(*((_DWORD *)this + 98) + 2208);
  else
    return 0;
}

int __cdecl CBattle_Field::CheckAppearRidableObject(CBattle_Field *this, PacketBuf **a2)
{
  int id; // [esp+28h] [ebp-10h] BYREF
  int id[3]; // [esp+2Ch] [ebp-Ch] BYREF

  v4[0] = -1;
  v3 = -1;
  CBattle_Field::getCurPosXY(this, v4, &v3);
  return CRidable::CheckAppearRidableObject((CBattle_Field *)((char *)this + 208), *(CParty **)this, v4[0], v3, a2);
}

int CBattle_Field::IsEnableHellDungeon()
{
  return *(unsigned __int8 *)CBattle_Field::GetEnableHellDungeon();
}

CBattle_Field *__cdecl CBattle_Field::set_dungeon_diff(CBattle_Field *this, char flag)
{
  *((_DWORD *)this + 104) = a2;
  return this;
}

  RandomBuffDungeonType = CBattle_Field::getRandomBuffDungeonType((CBattle_Field *)(*a1 + 713), 0);
  CParty::choose_random_buff((CParty *)*a1, RandomBuffDungeonType);

      MapInfo = CBattle_Field::GetMapInfo((CBattle_Field *)(Party + 2852));
      std::map<int,map_monster>::begin(&v16, MapInfo + 12);

        CurrentMapInfo = (MapInfo *)CBattle_Field::GetCurrentMapInfo((CParty *)((char *)Party + 2852));
        v5 = CParty::getPassedMapInfoList(Party);

  dungeon_index = CBattle_Field::get_dungeon_index((CParty *)((char *)a2 + 2852));
  v4 = (CDungeonList **)G_CDataManager();

  CurrentMapInfo = (MapInfo *)CBattle_Field::GetCurrentMapInfo((CParty *)((char *)a2 + 2852));
  for ( i = 0; *(_DWORD *)(a1 + 12) > i; ++i )

    CBattle_Field::KillHellPartyGroupMonsterCnt((CParty *)((char *)a2 + 2852), a3, a4, &v18);
  v14 = AddIndependentDropRateFromPremium;

          CurrentMapInfo = CBattle_Field::GetCurrentMapInfo((CParty *)((char *)Party + 2852));
          std::map<int,map_monster>::begin(&v39, CurrentMapInfo + 12);

            dungeon_diff = CBattle_Field::get_dungeon_diff((CParty *)((char *)Party + 2852));
            standard_level = 0;

  dungeon_index = (char *)CBattle_Field::get_dungeon_index((CParty *)((char *)this + 2852));
  dungeon_diff = CBattle_Field::get_dungeon_diff((CParty *)((char *)this + 2852));

  CBattle_Field::reset_field(a1, (CParty *)((char *)this + 2852));
  return CParty::dungeon_start(this, dungeon_index, dungeon_diff, 0);

      if ( !CBattle_Field::check_start_point((CBattle_Field *)(this + 713))
        || (unsigned __int8)CBattle_Field::IsVisitedCurrentMap((CBattle_Field *)(this + 713)) )

      if ( v150 != 1 || !CBattle_Field::is_last_field_item((CBattle_Field *)(this + 713), a3) )
      {
        v15 = CUser::getHackAnalyzer(a2);
        WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)v15, a2, 0x12Du, 1, 0, 0);
      }

      CBattle_Field::get_dungeon_diff((CBattle_Field *)(this + 713));
      CParty::get_member_count(this);

        v157 = CBattle_Field::pickup_item((CBattle_Field *)(this + 713), a3);
        if ( v157 <= 0 )

        v170 = CBattle_Field::pickup_item((CBattle_Field *)(this + 713), a3);
        if ( v170 <= 0 )

          v149 = CBattle_Field::pickup_item((CBattle_Field *)(this + 713), a3);
          if ( v149 <= 0 )

     && (unsigned __int8)CBattle_Field::IsEnableHellDungeon()
     && *((_DWORD *)this + 821) == 1;

  dungeon_index = CBattle_Field::get_dungeon_index((CBattle_Field *)(this + 713));
  dungeon = (CDungeon *)CDataManager::find_dungeon((CDungeonList **)v26, dungeon_index);

    v5 = (const char *)CBattle_Field::get_dungeon_index((CBattle_Field *)(this + 713));
    LogManager::logFormat(

  if ( CBattle_Field::GetDungeon((CParty *)((char *)this + 2852))
    && (Dungeon = (const CDungeon *)CBattle_Field::GetDungeon((CParty *)((char *)this + 2852)),

  CBattle_Field::onClearMap((CParty *)((char *)this + 2852), 0);
  if ( a3

  v4 = CBattle_Field::DestroyPassiveObject((CParty *)((char *)this + 2852), a3);
  if ( v4 && CBattle_Field::ClearCondition((int)this + 2852, 0, a3) )

  if ( !CBattle_Field::getBloodState((CBattle_Field *)(a2 + 713)) )
    return 0;

      if ( CBattle_Field::getBloodState((CBattle_Field *)(a2 + 713)) <= 2 )
        return 0;

      CBattle_Field::setBloodState((int)(a2 + 713), 4);
      v9 = CParty::gen_timer_key((int)a2, 36);

      if ( CBattle_Field::getBloodState((CBattle_Field *)(a2 + 713)) <= 3 )
        return 0;

      CBattle_Field::setBloodState((int)(a2 + 713), 5);
      CParty::SetEPLPState((CParty *)a2, 2);

      if ( CBattle_Field::getBloodState((CBattle_Field *)(a2 + 713)) <= 1 )
        return 0;

      else if ( !(unsigned __int8)CBattle_Field::check_grid_clear((CBattle_Field *)(a2 + 713))
             || !CBattle_Field::check_end_point((CBattle_Field *)(a2 + 713))

  LOBYTE(v3) = CBattle_Field::getBloodState((CParty *)((char *)this + 2852)) > 2;
  if ( !(_BYTE)v3 )

      BloodMaxRound = CBattle_Field::getBloodMaxRound((CParty *)((char *)this + 2852));
      CBattle_Field::CBloodClearRewardData::setCurrentBloodRound((CParty *)((char *)this + 3548), BloodMaxRound);

    v3 = CBattle_Field::setBloodState((int)this + 2852, 3) ^ 1;
    if ( !(_BYTE)v3 )

        v4 = CBattle_Field::getBloodMaxRound((CParty *)((char *)this + 2852));
        NextRoundDifficulty = CBattle_Field::getNextRoundDifficulty((CParty *)((char *)this + 2852), v4 - 1);

  BloodMaxRound = CBattle_Field::getBloodMaxRound((CBattle_Field *)(this + 713));
  InterfacePacketBuf::put_byte(v16, BloodMaxRound);

  CBattle_Field::MakePacket((CBattle_Field *)(this + 713), 28, (InterfacePacketBuf *)v14);
  CParty::send_to_party(this, (PacketGuard *)v14);

  CBattle_Field::MakePacket((CBattle_Field *)(this + 713), 29, (InterfacePacketBuf *)v14);
  CParty::send_to_party(this, (PacketGuard *)v14);

  if ( (unsigned __int8)CBattle_Field::check_grid_clear((CParty *)((char *)this + 2852)) )
    v18 = 1;

int __cdecl CBattle_Field::GetMonsterMap(CBattle_Field *this)
{
  return CBattle_Field::GetCurrentMapInfo(this) + 12;
}

int __cdecl CBattle_Field::setBloodState(int value, int value2)
{
  if ( a2 <= *(_DWORD *)(a1 + 692) )
    return 0;
  *(_DWORD *)(a1 + 692) = a2;
  return 1;
}

int __cdecl CBattle_Field::SetRidableObjectState(int value, int value2, int value3)
{
  return CRidable::SetRidableObjectState(a1 + 208, a2, a3);
}

int __cdecl CBattle_Field::IsKilledAllHellGruoups(CBattle_Field *this)
{
  return *((unsigned __int8 *)this + 458);
}

int __cdecl CBattle_Field::get_map_index(MazeScript **this, int id, int id2)
{
  return *(_DWORD *)(CBattle_Field::GetMapInfoFromPos(this, a2, a3, 0) + 4);
}

int __cdecl CBattle_Field::GetCurrentMap(CBattle_Field *this)
{
  return *((_DWORD *)this + 66);
}

_BOOL4 __cdecl CBattle_Field::ClearCondition(int id, unsigned int value, int count)
{
  return !CClearCondition::Empty((CClearCondition *)(a1 + 240))
      && CClearCondition::ClearCondition((_DWORD *)(a1 + 240), a2, a3);
}

int __cdecl CBattle_Field::IsVisitedCurrentMap(CBattle_Field *this)
{
  return *(unsigned __int8 *)(CBattle_Field::GetCurrentMapInfo(this) + 1);
}

CBattle_Field *__cdecl CBattle_Field::setWorldMapTowerArea(CBattle_Field *this, bool a2)
{
  *((_BYTE *)this + 572) = a2;
  return this;
}

int __cdecl CBattle_Field::isWorldMapTowerArea(CBattle_Field *this)
{
  return *((unsigned __int8 *)this + 572);
}

int __cdecl CBattle_Field::GetCurPosX(CBattle_Field *this)
{
  if ( *((_DWORD *)this + 98) && (unsigned __int8)CDungeon::IsEnterEachMap(*((CDungeon **)this + 98)) )
    return *((_DWORD *)this + 2 * *((_DWORD *)this + 69) + 7);
  else
    return *((_DWORD *)this + 24);
}

int __cdecl CBattle_Field::GetCurPosY(CBattle_Field *this)
{
  if ( *((_DWORD *)this + 98) && (unsigned __int8)CDungeon::IsEnterEachMap(*((CDungeon **)this + 98)) )
    return *((_DWORD *)this + 2 * *((_DWORD *)this + 69) + 8);
  else
    return *((_DWORD *)this + 25);
}

    return CBattle_Field::SetAssignNPCByPotion((CBattle_Field *)(result + 2852), 1);
  return result;

int __cdecl CBattle_Field::SetAssignNPCByPotion(CBattle_Field *this, bool a2)
{
  int result; // eax

  result = CBattle_Field::GetCurrentMapInfo(this);
  *(_BYTE *)(result + 164) = a2;
  return result;
}

  CBattle_Field::BloodSpawnTimerManager((CBattle_Field *)(Party + 2852), 0, 1);
  return 1;

  CBattle_Field::BloodPhaseSpawn((CMap **)(Party + 2852), *(_WORD *)(Party + 3484), (__int16 *)(Party + 3506), 23, 0);
  return 1;

  CBattle_Field::BloodPhaseSpawn((CMap **)(Party + 2852), *(_WORD *)(Party + 3486), (__int16 *)(Party + 3508), 24, 1);
  return 1;

  CBattle_Field::BloodPhaseSpawn((CMap **)(Party + 2852), *(_WORD *)(Party + 3488), (__int16 *)(Party + 3510), 25, 2);
  return 1;

  CBattle_Field::BloodPhaseSpawn((CMap **)(Party + 2852), *(_WORD *)(Party + 3490), (__int16 *)(Party + 3512), 26, 3);
  return 1;

  CBattle_Field::BloodPhaseSpawn((CMap **)(Party + 2852), *(_WORD *)(Party + 3492), (__int16 *)(Party + 3514), 27, 4);
  return 1;

  CBattle_Field::BloodPhaseSpawn((CMap **)(Party + 2852), *(_WORD *)(Party + 3494), (__int16 *)(Party + 3516), 28, 5);
  return 1;

  CBattle_Field::BloodPhaseSpawn((CMap **)(Party + 2852), *(_WORD *)(Party + 3496), (__int16 *)(Party + 3518), 29, 6);
  return 1;

  CBattle_Field::BloodPhaseSpawn((CMap **)(Party + 2852), *(_WORD *)(Party + 3498), (__int16 *)(Party + 3520), 30, 7);
  return 1;

  CBattle_Field::BloodPhaseSpawn((CMap **)(Party + 2852), *(_WORD *)(Party + 3500), (__int16 *)(Party + 3522), 31, 8);
  return 1;

  CBattle_Field::BloodPhaseSpawn((CMap **)(Party + 2852), *(_WORD *)(Party + 3502), (__int16 *)(Party + 3524), 32, 9);
  return 1;

  CBattle_Field::BloodCrazyMonsterSpawnPacket((CBattle_Field *)(Party + 2852));
  return 1;

  CBattle_Field::check_elevator_timer((CBattle_Field *)(Party + 2852));
  return 1;

  if ( (unsigned __int8)CBattle_Field::getUltimateDifficylty((CBattle_Field *)(Party + 713)) )
    return 0;

  NextRoundRandomDifficulty = CBattle_Field::getNextRoundRandomDifficulty((CBattle_Field *)(Party + 713));
  CBattle_Field::setUltimateDifficulty((CBattle_Field *)(Party + 713), NextRoundRandomDifficulty);

  if ( BloodRoundOrder != CBattle_Field::getBloodMaxRound((CBattle_Field *)(Party + 713)) - 1 )
  {
    v7 = CBattle_Field::CBloodRound::getBloodRoundOrder((CBattle_Field::CBloodRound *)(Party + 884));
    CBattle_Field::sendUltimateRandomDifficulty(Party + 713, NextRoundRandomDifficulty, v7);
  }

  CBattle_Field::startUltimateRound((CBattle_Field *)(Party + 713));
  return 1;

    return CBattle_Field::get_dungeon_index((CBattle_Field *)(Party + 2852));
  else

          Maze = CBattle_Field::getMaze((CBattle_Field *)(Party + 2852));
          if ( Maze )

