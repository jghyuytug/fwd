#include <dungeon/CDungeon.h>
#include <game/dungeon/CDungeon.h>

                 charac_level - CDungeon::get_standard_level(v15[811]) < 0)
              ? (v12 = 0)

              : (v11 = CUserCharacInfo::get_charac_level(a2), v12 = v11 - CDungeon::get_standard_level(v15[811])),
                v16 = v12,

    return CDungeon::get_index(*(this + 28));
  else

        DungeonName = CDungeon::GetDungeonName(*((std::string **)this + 636));
        cUserHistoryLog::LeaveDungeon((CUser **)(user + CUSER_OFFSET_HISTORY_LOG), DungeonName, 0, v8, v6);

        if ( charac_level < CDungeon::get_min_level(*((CDungeon **)this + 636)) )
        {
          CDungeon::get_min_level(*((CDungeon **)this + 636));
          v9 = CUserCharacInfo::get_charac_level(user);
          cMyTrace::cMyTrace(
            (cMyTrace *)v19,
            "bool WongWork::CBossTower::checkStartGameCondition(unsigned char&, bool)",
            240,
            5);
          cMyTrace::operator()((int)v19, "dungeon level check charac_level : %d, dungeon_min_level : %d", v9);
          PacketGuard::PacketGuard((PacketGuard *)v15);
          InterfacePacketBuf::clear(v15);
          InterfacePacketBuf::put_header(v15, 1, 16);
          InterfacePacketBuf::put_byte(v15, 0);
          InterfacePacketBuf::put_int(v15, 14);
          InterfacePacketBuf::finalize(v15, 1);
          CParty::send_to_party(*((CUser ***)this + 1), (PacketGuard *)v15);
          v6 = 0;
          PacketGuard::~PacketGuard(v15);
          return v6;
        }

      DungeonName = CDungeon::GetDungeonName(*((std::string **)a2 + 636));
      cUserHistoryLog::EnterDungeon((CUser **)user + CUSER_OFFSET_HISTORY_LOG_ALT, DungeonName, v10, s, a1);

  DungeonName = CDungeon::GetDungeonName(*((std::string **)this + 636));
  cUserHistoryLog::LeaveDungeon(a3 + CUSER_OFFSET_HISTORY_LOG_ALT, DungeonName, v9, &v15, v8);

      index = CDungeon::get_index(*(this + 636));
      v3 = G_CDataManager();

      TowerMaxClearRewardItemNum = CDungeon::getTowerMaxClearRewardItemNum(*(this + 636));
      v9 = *std::min<int>(&TowerMaxClearRewardItemNum, &v10);

int __cdecl CDungeon::get_min_level(CDungeon *this)
{
  return *((_DWORD *)this + 3);
}

char *__cdecl CDungeon::GetDungeonName(std::string *this)
{
  return std::string::c_str(this + 6);
}

int __cdecl CDungeon::getRandomTowerMaxStage(CDungeon *this)
{
  return *((_DWORD *)this + 405);
}

int __cdecl CDungeon::getTowerMaxClearRewardItemNum(CDungeon *this)
{
  return *((_DWORD *)this + 430);
}

  index = CDungeon::get_index(a5);
  v7 = G_CDataManager();

  v8 = CDungeon::get_index(a5);
  v9 = G_CEnvironment();

        standard_level = CDungeon::get_standard_level(*((CDungeon **)this + 636));
        Level = standard_level + *(_DWORD *)(v98 + 8);

    SpecialPassiveObjectItems = CDungeon::getSpecialPassiveObjectItems(*((CDungeon **)this + 636));
    WongWork::stGenerateRefData_t::stGenerateRefData_t((WongWork::stGenerateRefData_t *)v43);

    v42[13] = CDungeon::getObjectDropItems(*((CDungeon **)this + 636));
    CMap::get_item_list((CMap *)v56, (int)a2);

      if ( CDungeon::get_standard_level(*((CDungeon **)this + 636)) > 0
        && CDungeon::get_standard_level(*((CDungeon **)this + 636)) <= 200 )

        v44 = CDungeon::get_standard_level(*((CDungeon **)this + 636));
        goto LABEL_52;

        index = CDungeon::get_index(*((CDungeon **)this + 636));
        v7 = G_CDataManager();

      v8 = CDungeon::get_index(*((CDungeon **)this + 636));
      v9 = G_CDataManager();

      v11 = CDungeon::get_index(*((CDungeon **)this + 636));
      CUser::CheckQuestMonster(user, v11, v10, v21);

      if ( (unsigned __int8)CDungeon::hasTowerItemDrop(*((CDungeon **)this + 636)) )
        CAICharacter::deathTowerSpecifyItemDrop(v44, (int)v38);

  RandomTowerMapIndexes = CDungeon::getRandomTowerMapIndexes(*((CDungeon **)this + 636));
  std::map<int,std::vector<RandomList>>::find(v9, (int)RandomTowerMapIndexes, &v12);

  v3 = CDungeon::getRandomTowerMapIndexes(*((CDungeon **)this + 636));
  std::map<int,std::vector<RandomList>>::end(&v13, (int)v3);

char *__cdecl CDungeon::getObjectDropItems(CDungeon *this)
{
  return (char *)this + 76;
}

char *__cdecl CDungeon::getSpecialPassiveObjectItems(CDungeon *this)
{
  return (char *)this + 88;
}

char *__cdecl CDungeon::getRandomTowerMapIndexes(CDungeon *this)
{
  return (char *)this + 1624;
}

int __cdecl CDungeon::hasTowerItemDrop(CDungeon *this)
{
  return *((unsigned __int8 *)this + 1717);
}

    v18 = (unsigned __int8)CDungeon::limitOfStackableItemInTower(Dungeon) != 1;
    v8 = WongWork::CDeathTower::getCDungeonMGr(DeathTower);

    if ( (unsigned __int8)CDungeon::limitOfStackableItemInTower(v9) )
    {
      for ( i = 0; *((char *)a3 + 14) > i; ++i )
      {
        v10 = *(_DWORD *)((char *)a3 + 4 * i + 415);
        v11 = (CItemList **)G_CDataManager();
        item = (CItem *)CDataManager::find_item(v11, v10);
        if ( item && CItem::get_index(item) == 6515 )
        {
          CurTickCount = CSystemTime::getCurTickCount((CSystemTime *)&GlobalData::s_systemTime_);
          WongWork::CDeathTower::stMapMonsterKillChecker_t::setUseSkillMaterial(
            (WongWork::CDeathTower *)((char *)DeathTower + 2904),
            CurTickCount);
        }
      }
    }

    if ( CDungeon::getDungeonKind(v46) == 1 )
    {
      if ( Party )
      {
        v27 = G_CGameManager();
        DeathTower = CGameManager::getDeathTower(v27);
        if ( DeathTower )
        {
          min_level = CDungeon::get_min_level(v46);
          for ( n = 0; n <= 3; ++n )
          {
            v67 = (CUserCharacInfo *)CParty::get_user(Party, n);
            if ( v67 )
            {
              charac_level = CUserCharacInfo::get_charac_level(v67);
              if ( charac_level < min_level )
                return 0;
            }
          }
          WongWork::CDeathTower::attachParty(DeathTower, Party);
          WongWork::CDeathTower::handleStartGame(
            (CUserCharacInfo ***)DeathTower,
            *(__int16 *)((char *)v41 + 13),
            *((char *)v41 + 15),
            *((char *)v41 + 16));
          v40[1] = MAX_INT32;
          return 0;
        }
        else
        {
          v40[1] = MAX_INT32;
          return 0;
        }
      }
      else
      {
        v40[1] = MAX_INT32;
        return 0;
      }
    }

    else if ( CDungeon::getDungeonKind(v46) == 3 )
    {
      member_count = CParty::get_member_count((CUser **)Party);
      limit_party_count = CDungeon::get_limit_party_count(v46);
      if ( limit_party_count >= member_count )
      {
        Global::CBossTowerFunc::SelectDungeon(a1, (CUser **)Party, *(_WORD *)((char *)v41 + 13), *((char *)v41 + 16));
        return 0;
      }
      else
      {
        PacketGuard::PacketGuard((PacketGuard *)v38);
        InterfacePacketBuf::clear(v38);
        InterfacePacketBuf::put_header(v38, 1, 16);
        InterfacePacketBuf::put_byte(v38, 0);
        InterfacePacketBuf::put_int(v38, 243);
        InterfacePacketBuf::finalize(v38, 1);
        CParty::send_to_party((CUser **)Party, (PacketGuard *)v38);
        v5 = 0;
        PacketGuard::~PacketGuard(v38);
      }
    }

            && (unsigned __int8)CDungeon::isNoFatigueDungeon(v46) != 1 )
          {
            v75 = CParty::CheckMemberFatigue((CUser **)Party);
            if ( v75 > 0 )
            {
              v40[3] = 22;
              v40[2] = 22;
              v40[5] = v75 - 1;
              return 0;
            }
          }

          if ( *((_DWORD *)v105 + 811) && CDungeon::get_index(*((CDungeon **)v105 + 811)) == 11006 )
          {
            HackAnalyzer = CUser::getHackAnalyzer(a3);
            WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)HackAnalyzer, a3, 0xCDu, 1, 2u, 0);
          }

            if ( *((_DWORD *)v105 + 811) && CDungeon::get_index(*((CDungeon **)v105 + 811)) == 11006 )
            {
              v18 = CUser::getHackAnalyzer(a3);
              WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)v18, a3, 0xCEu, 1, 2u, 0);
            }

              if ( CDungeon::get_index(*(CDungeon **)(v25 + 3244)) == 11006 )
              {
                v27 = v71;
                v28 = CUser::getHackAnalyzer(v106);
                WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)v28, v106, MASK_7BIT, 1, v27, 0);
              }

            if ( v107 > v89 && (!*((_DWORD *)v105 + 811) || CDungeon::get_index(*((CDungeon **)v105 + 811)) != 11006) )
            {
              v32 = v88;
              v33 = CUser::getHackAnalyzer(v106);
              WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)v33, v106, v32, 1, 0, 0);
              if ( CHackLog_OverDamage::GetDungeonLimitMaxDamage() < v107 )
              {
                HackTypeOverLimitMaxDamage = CHackLog_OverDamage::GetHackTypeOverLimitMaxDamage();
                v35 = CUser::getHackAnalyzer(v106);
                WongWork::CHackAnalyzer::addServerHackCnt(
                  (WongWork::CHackAnalyzer *)v35,
                  v106,
                  HackTypeOverLimitMaxDamage,
                  1,
                  0,
                  0);
              }
            }

  if ( *((_DWORD *)Party + 811) && CDungeon::isTowerOfDespairDungeon(*((CDungeon **)Party + 811)) )
    return 0;

  if ( *((_DWORD *)Party + 811) && CDungeon::isTowerOfDespairDungeon(*((CDungeon **)Party + 811)) )
    return 0;

    if ( !*((_DWORD *)Party + 811) || CDungeon::get_index(*((CDungeon **)Party + 811)) != 11007 )
    {
      LogManager::logFormat(
        (LogManager *)1,
        (int)"PacketDispatcher_Impl_1.cpp",
        "virtual int Dispatcher_SelectUltimateDifficulty::dispatch_sig(CUser*, PacketBuf&)",
        (const char *)0xE2E3,
        "ULTIMATE_LOG : Dispatcher_SelectUltimateDifficulty ERROR!!\n",
        v7);
      return 0;
    }

int __cdecl CDungeon::get_limit_party_count(CDungeon *this)
{
  return *((unsigned __int8 *)this + 59);
}

int __cdecl CDungeon::getDungeonKind(CDungeon *this)
{
  return *((_DWORD *)this + 412);
}

int __cdecl CDungeon::limitOfStackableItemInTower(CDungeon *this)
{
  return *((unsigned __int8 *)this + 1716);
}

int __cdecl CDungeon::isNoFatigueDungeon(CDungeon *this)
{
  return *((unsigned __int8 *)this + 1736);
}

int __cdecl CDungeon::GetType_DungeonInHeritance(CDungeon *this)
{
  return *((_DWORD *)this + 1);
}

bool __cdecl CDungeon::isTowerOfDespairDungeon(CDungeon *this)
{
  return CDungeon::GetType_DungeonInHeritance(this) == 1;
}

      index = CDungeon::get_index(a2);
      if ( index == *(_DWORD *)__gnu_cxx::__normal_iterator<int const*,std::vector<int>>::operator*((int)&v14) )

    if ( !CDungeon::isTowerOfDespairDungeon(a2) )
    {
      QuickParty::STQuickPartyPoolMap_Key::STQuickPartyPoolMap_Key((QuickParty::STQuickPartyPoolMap_Key *)&v11);
      QuickParty::STQuickPartyPoolMap_Data::STQuickPartyPoolMap_Data((QuickParty::STQuickPartyPoolMap_Data *)v10);
      v11 = CDungeon::get_index(a2);
      for ( i = 0; i <= 3; ++i )
      {
        v9 = CDungeon::getDungeonKind(a2) == 1
          || (char)CDungeon::get_blood_dungeon_type(a2) > 0
          || (unsigned __int8)CDungeon::getQuestNpcDungeon(a2);
        if ( !v9 || !i )
        {
          v12 = i;
          std::make_pair<QuickParty::STQuickPartyPoolMap_Key &,QuickParty::STQuickPartyPoolMap_Data &>(
            v17,
            (int)&v11,
            (int)v10);
          std::pair<QuickParty::STQuickPartyPoolMap_Key const,QuickParty::STQuickPartyPoolMap_Data>::pair<QuickParty::STQuickPartyPoolMap_Key,QuickParty::STQuickPartyPoolMap_Data>(
            v16,
            (int)v17);
          std::map<QuickParty::STQuickPartyPoolMap_Key,QuickParty::STQuickPartyPoolMap_Data>::insert(
            (int)v15,
            (int)this,
            (int)v16);
          std::pair<QuickParty::STQuickPartyPoolMap_Key const,QuickParty::STQuickPartyPoolMap_Data>::~pair((int)v16);
          std::pair<QuickParty::STQuickPartyPoolMap_Key,QuickParty::STQuickPartyPoolMap_Data>::~pair((int)v17);
        }
      }
      QuickParty::STQuickPartyPoolMap_Data::~STQuickPartyPoolMap_Data((QuickParty::STQuickPartyPoolMap_Data *)v10);
    }

  if ( (unsigned __int8)CDungeon::get_blood_dungeon_type(dungeon) == 2
    && (char)CDungeon::get_limit_party_count(dungeon) <= member_count )

        if ( (unsigned __int8)CDungeon::get_blood_dungeon_type(dungeon) == 2 )
          limit_party_count = (char)CDungeon::get_limit_party_count(dungeon);

  if ( (unsigned __int8)CDungeon::get_blood_dungeon_type(dungeon) == 2 )
  {
    if ( (char)CDungeon::get_limit_party_count(dungeon) != a3 )
      return 0;
  }

  else if ( (char)CDungeon::get_dimension_possible(dungeon) <= 0 )
  {
    RequireQuickPartyMinCount = -1;
    v12 = *((_BYTE *)dungeon + 2204) != 0;
    v5 = G_CGameManager();
    if ( CGameManager::GetQuickPartyRewardManager(v5) )
    {
      v6 = G_CGameManager();
      QuickPartyRewardManager = (QuickParty::CQuickPartyRewardManager *)CGameManager::GetQuickPartyRewardManager(v6);
      RequireQuickPartyMinCount = QuickParty::CQuickPartyRewardManager::getRequireQuickPartyMinCount(
                                    QuickPartyRewardManager,
                                    v12,
                                    v9);
    }
    if ( RequireQuickPartyMinCount != -1 && RequireQuickPartyMinCount > a3 )
      return 0;
    if ( a3 <= 1 )
      return 0;
  }

  else if ( (char)CDungeon::get_dimension_min_partymem(dungeon) + 1 > a3 )
  {
    return 0;
  }

int __cdecl CDungeon::get_blood_dungeon_type(CDungeon *this)
{
  return *((unsigned __int8 *)this + 2207);
}

int __cdecl CDungeon::get_dimension_possible(CDungeon *this)
{
  return *((unsigned __int8 *)this + 56);
}

int __cdecl CDungeon::get_dimension_min_partymem(CDungeon *this)
{
  return *((unsigned __int8 *)this + 58);
}

int __cdecl CDungeon::getQuestNpcDungeon(CDungeon *this)
{
  return *((unsigned __int8 *)this + 2300);
}

    v12 = charac_level - CDungeon::get_min_level(*(CDungeon **)(Party + 3244));
    if ( v12 < 0 )

    index = CDungeon::get_index(*(CDungeon **)(v6 + 3244));
    if ( index == 11000 )

        if ( CDungeon::isTowerOfDespairDungeon(v8) )
          v7 = 1;

        if ( (char)CDungeon::get_dimension_possible(v8) > 0
          || *((_BYTE *)v8 + 2204)

          || CDungeon::isTournamentDungeon(v8) )
        {
          v7 = 1;
        }

    QuestMazeScriptVector = (_DWORD *)CDungeon::GetQuestMazeScriptVector(*(_DWORD *)(a2 + 392), i);
    if ( QuestMazeScriptVector && !std::vector<MazeScript>::empty(QuestMazeScriptVector) )

  DummyAppearCount = CDungeon::GetDummyAppearCount(a4);
  CBattle_Field::SetGridPath(a2, AppropriateMaze, DummyAppearCount);

  StartMap = (void *)CDungeon::GetStartMap(a4, *GridR);
  if ( (int)StartMap > 0 )

            if ( (char)CDungeon::get_dimension_possible(*((CDungeon **)a2 + 98)) <= 0 )
            {
              standard_level = CDungeon::get_standard_level(*((CDungeon **)a2 + 98));
              *((_BYTE *)a5 + 16) = *(_DWORD *)(v28 + 8) + standard_level;
            }

            if ( (char)CDungeon::get_dimension_possible(*((CDungeon **)a2 + 98)) <= 0 )
              v12 = *(_DWORD *)(v28 + 8);

            && !CDungeon::isTournamentDungeon(*((CDungeon **)a2 + 98)) )
          {
            std::vector<int>::push_back(a4, (_DWORD *)a5 + 1);
          }

      if ( CDungeon::getAICharacterAppearRate(*(this + 98)) )
      {
        AICharacterAppearRate = CDungeon::getAICharacterAppearRate(*(this + 98));
        if ( AICharacterAppearRate > (int)get_rand_int(100) )
        {
          standard_level = CDungeon::get_standard_level(*(this + 98));
          v7 = G_CDataManager();
          v12 = (CAICharacter *)CAICharacterList::getByLevel(*((CAICharacterList **)v7 + 8615), standard_level);
          if ( v12 )
          {
            memset(s, 0, 0x34Cu);
            *(_DWORD *)s = v11;
            *((_DWORD *)s + 3) = CAICharacter::getIdx(v12);
            v8 = *a3;
            *((_DWORD *)s + 1) = *a3;
            *a3 = v8 + 1;
            *((_BYTE *)s + 8) = 5;
            if ( (char)CDungeon::get_dimension_possible(*(this + 98)) <= 0 )
              *((_BYTE *)s + 16) = CAICharacter::getLevel(v12);
            else
              *((_BYTE *)s + 16) = (unsigned __int8)*(this + 142);
            *((_DWORD *)s + 11) = 100;
            *((_BYTE *)s + 17) = 0;
            *((_BYTE *)s + 18) = 0;
            map_monster::map_monster((map_monster *)v10, s);
            CurrentMapInfo = CBattle_Field::GetCurrentMapInfo((CBattle_Field *)this);
            MapInfo::Add_Mob(CurrentMapInfo, (int)v10);
            map_monster::~map_monster(v10);
          }
        }
      }

  DungeonName = CDungeon::GetDungeonName(a5);
  v7 = CBattle_Field::CBloodClearRewardData::getPlayTime(this) / 0x3E8u;

              *((_BYTE *)a3 + 16) = CDungeon::get_standard_level(a2[98]);
              *((_DWORD *)a3 + 3) = *(_DWORD *)(hellparty_group + 4 * v34);

    if ( (unsigned __int8)CDungeon::IsHaveDummyMap((CDungeon *)a2) )
    {
      if ( *((_DWORD *)this + 3) == a6 && *((_DWORD *)this + 4) == a7 )
      {
        v8 = *((_DWORD *)this + 51);
        if ( v8 >= std::vector<std::pair<int,int>>::size((_DWORD *)this + 48) )
        {
          if ( *(_BYTE *)(CBattle_Field::GetMapInfoFromPos((MazeScript **)this, a6, a7, 0) + 69) )
            *(_BYTE *)(CBattle_Field::GetMapInfoFromPos((MazeScript **)this, a6, a7, 0) + 69) = 0;
        }
        else
        {
          v9 = (int *)std::vector<std::pair<int,int>>::operator[]((_DWORD *)this + 48, *((_DWORD *)this + 51));
          v10 = v9[1];
          v14 = *v9;
          v15 = v10;
          *((_DWORD *)this + 3) = v14;
          *((_DWORD *)this + 4) = v15;
          if ( *(_BYTE *)(CBattle_Field::GetMapInfoFromPos(
                            (MazeScript **)this,
                            *((_DWORD *)this + 3),
                            *((_DWORD *)this + 4),
                            0)
                        + 1) )
          {
            *(_BYTE *)(CBattle_Field::GetMapInfoFromPos(
                         (MazeScript **)this,
                         *((_DWORD *)this + 3),
                         *((_DWORD *)this + 4),
                         0)
                     + 1) = 0;
            *(_BYTE *)(CBattle_Field::GetMapInfoFromPos(
                         (MazeScript **)this,
                         *((_DWORD *)this + 3),
                         *((_DWORD *)this + 4),
                         0)
                     + 68) = 2;
          }
          v13 = -1;
          v12 = -1;
          CBattle_Field::getCurPosXY(this, &v13, &v12);
          if ( *((_DWORD *)this + 3) == v13 && *((_DWORD *)this + 4) == v12 )
          {
            *a5 = 0;
            *(_BYTE *)(CBattle_Field::GetMapInfoFromPos(
                         (MazeScript **)this,
                         *((_DWORD *)this + 3),
                         *((_DWORD *)this + 4),
                         0)
                     + 68) = 2;
          }
          *a4 = CDungeon::ChooseRandomMap(a2, 2, *(_DWORD *)a3, a8);
          *(_BYTE *)(CBattle_Field::GetMapInfoFromPos((MazeScript **)this, a6, a7, 0) + 69) = 1;
          ++*((_DWORD *)this + 51);
        }
      }
    }

        *a4 = CDungeon::ChooseRandomMap(a2, 1, *a3, a8);
      }
    }
    else if ( *((_DWORD *)a1 + 5) == a6 && *((_DWORD *)a1 + 6) == a7 && *((_DWORD *)a1 + 103) )
    {
      *a4 = *((_DWORD *)a1 + 103);
      if ( *a4 <= 0 )
      {

      *a4 = CDungeon::ChooseRandomMap(a2, 0, *a3, a8);
    }
  }
  return 0;
}

//----- (083086D2) --------------------------------------------------------
int __cdecl CBattle_Field::set_visited_map(CBattle_Field *this, int value, int value2, bool a4)
{
  int result; // eax
  int LayeredMapIndex; // eax
  int value; // [esp+28h] [ebp-10h] BYREF
  int value[3]; // [esp+2Ch] [ebp-Ch] BYREF

  v7[0] = -1;
  v6 = -1;
  CBattle_Field::getCurPosXY(this, v7, &v6);
  if ( v7[0] == a2 && v6 == a3 )
  {
    if ( CBattle_Field::getLayeredMapIndex(this) )
    {

    BYTE2(DropItems) = CDungeon::get_standard_level(a1[98]);
  else

  if ( (char)CDungeon::get_dimension_possible(a1[98]) <= 0 )
    BYTE1(DropItems) = CDungeon::get_standard_level(a1[98]);

  DropItems = CDungeon::getDropItems(a1[98]);
  v25 = G_CDataManager();

  if ( *((_DWORD *)this + 98) && CDungeon::isTournamentDungeon(*((CDungeon **)this + 98)) )
  {
    TournamentCurRound = CBattle_Field::GetTournamentCurRound(this);
    v106 = CBattle_Field::GetCurrentMapInfo(this) + 208;
    std::multimap<int,map_monster>::lower_bound(&v76, v106, &TournamentCurRound);
    std::multimap<int,map_monster>::upper_bound(&v75, v106, &TournamentCurRound);
    for ( i = v76;
          std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator!=(&i, &v75);
          std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator++(v87, &i) )
    {
      v10 = std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator->(&i);
      if ( *(_DWORD *)(v10 + 8) == *(_DWORD *)(v102 + 4) )
      {
        std::multimap<int,map_monster>::erase(v106, i);
        break;
      }
    }
    std::multimap<int,map_monster>::find(&v74, v106, &TournamentCurRound);
    std::multimap<int,map_monster>::end(&v88, v106);
    if ( std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator==(&v74, &v88) )
    {
      TournamentRoundFatigue = CDungeon::getTournamentRoundFatigue(*((CDungeon **)this + 98));
      if ( TournamentCurRound != 4 )
      {
        for ( j = 0; j <= 3; ++j )
        {
          if ( (unsigned __int8)CParty::checkValidUser(*(CUser ***)this, j) == 1 )
          {
            v11 = TournamentRoundFatigue;
            Member = CParty::GetMember(*(CParty **)this);
            CUser::FatigueUp(*(CUser **)&Member[24 * j], v11);
            v13 = CParty::GetMember(*(CParty **)this);
            CUser::SendFatigue(*(CUser **)&v13[24 * j]);
          }
        }
        CBattle_Field::SetTournamentCurRound(this, TournamentCurRound + 1);
      }
    }
  }

            v56 = (const char *)CDungeon::get_index(*((CDungeon **)this + 98));
            LogManager::logFormat(

int __cdecl CDungeon::getAICharacterAppearRate(CDungeon *this)
{
  return *((_DWORD *)this + 404);
}

char *__cdecl CDungeon::getDropItems(CDungeon *this)
{
  return (char *)this + 64;
}

int __cdecl CDungeon::IsHaveDummyMap(CDungeon *this)
{
  return *((unsigned __int8 *)this + 1572);
}

int __cdecl CDungeon::GetDummyAppearCount(CDungeon *this)
{
  return *((_DWORD *)this + 394);
}

int __cdecl CDungeon::getTryGenEventMonsterCnt(CDungeon *this)
{
  return *((_DWORD *)this + 417);
}

int __cdecl CDungeon::getTryGenEventMonsterProb(CDungeon *this)
{
  return *((_DWORD *)this + 418);
}

int __cdecl CDungeon::getEventMonsterTotalProb(CDungeon *this)
{
  return *((_DWORD *)this + 419);
}

char *__cdecl CDungeon::getEventMonsters(CDungeon *this)
{
  return (char *)this + 1680;
}

int __cdecl CDungeon::getTryGenSecondEventMonsterCnt(CDungeon *this)
{
  return *((_DWORD *)this + 423);
}

int __cdecl CDungeon::getTryGenSecondEventMonsterProb(CDungeon *this)
{
  return *((_DWORD *)this + 424);
}

int __cdecl CDungeon::getSecondEventMonsterTotalProb(CDungeon *this)
{
  return *((_DWORD *)this + 425);
}

char *__cdecl CDungeon::getSecondEventMonsters(CDungeon *this)
{
  return (char *)this + 1704;
}

int __cdecl CDungeon::GetQuestMazeScriptVector(int id, int id2)
{
  return a1 + 12 * a2 + 1584 + 8;
}

CDungeon *__cdecl CDungeon::set_dungeon(CDungeon *this, STDungeonScript *a2)
{
  char *v2; // eax
  char *v3; // eax
  char flag; // bl
  unsigned int value; // ebx
  unsigned int value; // esi
  CDataManager *v7; // eax
  int value; // ebx
  int value; // esi
  CDataManager *v10; // eax
  char flag; // bl
  CDataManager *v12; // eax
  char flag; // bl
  CDataManager *v14; // eax
  int value; // eax
  _DWORD *v16; // eax
  int value; // edx
  int *v18; // eax
  _DWORD *v19; // ebx
  unsigned int value; // eax
  int value; // ebx
  int value; // eax
  char dest[500]; // [esp+2Ch] [ebp-29Ch] BYREF
  int value; // [esp+220h] [ebp-A8h] BYREF
  const std::_Rb_tree_node_base *v26; // [esp+224h] [ebp-A4h] BYREF
  _DWORD value[2]; // [esp+228h] [ebp-A0h] BYREF
  int value; // [esp+230h] [ebp-98h] BYREF
  char *v29; // [esp+234h] [ebp-94h] BYREF
  int value; // [esp+238h] [ebp-90h] BYREF
  int value; // [esp+23Ch] [ebp-8Ch] BYREF
  int value[4]; // [esp+240h] [ebp-88h] BYREF
  int value; // [esp+250h] [ebp-78h] BYREF
  _DWORD value[3]; // [esp+254h] [ebp-74h] BYREF
  _BYTE v35[8]; // [esp+260h] [ebp-68h] BYREF
  _DWORD value[2]; // [esp+268h] [ebp-60h] BYREF
  int value; // [esp+270h] [ebp-58h] BYREF
  int value; // [esp+274h] [ebp-54h] BYREF
  int value; // [esp+278h] [ebp-50h] BYREF
  _BYTE v40[8]; // [esp+27Ch] [ebp-4Ch] BYREF
  _DWORD value[4]; // [esp+284h] [ebp-44h] BYREF
  _DWORD value[4]; // [esp+294h] [ebp-34h] BYREF
  int i; // [esp+2A4h] [ebp-24h]
  MazeScript *v44; // [esp+2A8h] [ebp-20h]
  unsigned int j; // [esp+2ACh] [ebp-1Ch]

  *((_DWORD *)this + 2) = *((_DWORD *)a2 + 6);
  *((_DWORD *)this + 4) = *((_DWORD *)a2 + 43);
  *((_DWORD *)this + 3) = *((_DWORD *)a2 + 30);
  *((_DWORD *)this + 5) = *((_DWORD *)a2 + 44);
  memset(dest, 0, sizeof(dest));
  v2 = std::string::c_str((std::string *)a2 + 7);
  strcpy(dest, v2);
  v3 = std::string::c_str((std::string *)a2 + 7);
  if ( (unsigned __int8)CodePage::script2Database((CodePage *)v3, dest) != 1 )
  {
    std::string::c_str((std::string *)a2 + 7);
    CodePage::database();
    v4 = (unsigned __int8)CodePage::script();
    cMyTrace::cMyTrace((cMyTrace *)v32, "void CDungeon::set_dungeon(STDungeonScript&)", 1418, 5);
    cMyTrace::operator()((int)v32, "Error!!! %s->%s [%s][%s][%d]", v4);
  }
  std::string::operator=((std::string *)this + 6, dest);
  for ( i = 0; i <= 4; ++i )
    *((_DWORD *)this + i + 440) = *((_DWORD *)a2 + i + 45);
  *((_DWORD *)this + 13) = *((_DWORD *)a2 + 5);
  *((_BYTE *)this + 56) = *((_BYTE *)a2 + 772);
  *((_BYTE *)this + 57) = *((_BYTE *)a2 + 773);
  *((_BYTE *)this + 58) = *((_BYTE *)a2 + 774);
  if ( *((_BYTE *)this + 56) )
  {
    v5 = *((char *)this + 57);
    v6 = *((char *)this + 56) - 1;
    v7 = G_CDataManager();
    CDataManager::set_dimensionInout(v7, v6, v5);
    v8 = *((char *)this + 57);
    v9 = *((char *)this + 56) - 1;
    v10 = G_CDataManager();
    CDataManager::set_original_dimensionInout(v10, v9, v8);
  }
  *((_BYTE *)this + 59) = *((_BYTE *)a2 + 804);
  *((_BYTE *)this + 60) = *((_BYTE *)a2 + 805);
  if ( *((_DWORD *)this + 2) == 11007 )
  {
    v11 = *((_BYTE *)this + 60);
    v12 = G_CDataManager();
    CDataManager::set_limit_inout_count((int)v12, v11, 2);
  }
  if ( *((_DWORD *)this + 2) == 11006 )
  {
    v13 = *((_BYTE *)this + 60);
    v14 = G_CDataManager();
    CDataManager::set_limit_inout_count((int)v14, v13, 1);
  }
  std::vector<MazeScript>::begin(&v31, (_DWORD *)a2 + 92);
  std::vector<MazeScript>::end(&v30, (int)a2 + 368);
  while ( __gnu_cxx::operator!=<MazeScript *,std::vector<MazeScript>>((int)&v31, (int)&v30) )
  {
    v44 = (MazeScript *)__gnu_cxx::__normal_iterator<MazeScript *,std::vector<MazeScript>>::operator*((int)&v31);
    v15 = *((_DWORD *)v44 + 19);
    if ( v15 )
    {
      if ( v15 == 1 )
      {
        std::vector<MazeScript>::push_back((int)this + 1604, v44);
      }
      else if ( v15 == -1 )
      {
        std::vector<MazeScript>::push_back((int)this + 1580, v44);
      }
    }
    else
    {
      std::vector<MazeScript>::push_back((int)this + 1592, v44);
    }
    __gnu_cxx::__normal_iterator<MazeScript *,std::vector<MazeScript>>::operator++(&v31);
  }
  v29 = 0;
  std::map<int,int>::clear((int)this + 28);
  std::vector<std::pair<int,int>>::begin(&v28, (_DWORD *)a2 + 50);
  std::pair<int,int>::pair(v27);
  while ( 1 )
  {
    std::vector<std::pair<int,int>>::end(&v33, (int)a2 + 200);
    if ( !__gnu_cxx::operator!=<std::pair<int,int> *,std::vector<std::pair<int,int>>>((int)&v28, (int)&v33) )
      break;
    v16 = (_DWORD *)__gnu_cxx::__normal_iterator<std::pair<int,int> *,std::vector<std::pair<int,int>>>::operator*((int)&v28);
    v17 = v16[1];
    v34[0] = *v16;
    v34[1] = v17;
    std::pair<int,int>::operator=(v27, (int)v34);
    if ( v27[0] )
    {
      v29 += v27[1];
      std::pair<int const,int>::pair<int &,int &>(v36, (int)&v29, (int)v27);
      std::map<int,int>::insert((int)v35, (int)this + 28, (int)v36);
      if ( (int)v29 > 100 )
      {
        LogManager::logFormat(
          (LogManager *)1,
          (int)"data_manager.cpp",
          "void CDungeon::set_dungeon(STDungeonScript&)",
          (const char *)0x5E1,
          "total_rate(%d) > 100",
          v29);
        break;
      }
      __gnu_cxx::__normal_iterator<std::pair<int,int> *,std::vector<std::pair<int,int>>>::operator++(&v37, &v28);
    }
  }
  std::vector<std::pair<int,int>>::operator=((int *)this + 16, (int *)a2 + 53);
  std::vector<std::pair<int,int>>::operator=((int *)this + 19, (int *)a2 + 56);
  std::vector<stDungeonAssignItem_t>::operator=((int)this + 88, (int *)a2 + 59);
  STWarRoomData::copy((STDungeonScript *)((char *)a2 + 396), (CDungeon *)((char *)this + 1780));
  if ( (char *)this + 2036 != (char *)a2 + 124 )
    memcpy((char *)this + 2036, (char *)a2 + 124, 9u);
  *((_BYTE *)this + 2140) = *((_BYTE *)a2 + 136);
  if ( (char *)this + 2144 != (char *)a2 + 140 )
    memcpy((char *)this + 2144, (char *)a2 + 140, 9u);
  std::vector<std::pair<int,int>>::operator=((int *)this + 539, (int *)a2 + 38);
  *((_BYTE *)this + 2168) = *((_BYTE *)a2 + 788);
  *((_BYTE *)this + 2169) = *((_BYTE *)a2 + 789);
  *((_BYTE *)this + 2171) = *((_BYTE *)a2 + 791);
  if ( *((_BYTE *)a2 + 790) )
    *((_BYTE *)this + 2170) = 1;
  else
    *((_BYTE *)this + 2170) = 0;
  *((_DWORD *)this + 543) = *((_DWORD *)a2 + 41);
  *((_DWORD *)this + 579) = *((_DWORD *)a2 + 202);
  std::vector<int>::operator=((int *)this + 580, (int *)a2 + 203);
  std::copy<int *,int *>((int)a2 + 296, (int)a2 + 316, (int)this + 2176);
  *((_DWORD *)this + 549) = *((_DWORD *)a2 + 79);
  *((_DWORD *)this + 550) = *((_DWORD *)a2 + 90);
  *((_BYTE *)this + 2204) = *((_BYTE *)a2 + 652);
  *((_BYTE *)this + 2205) = *((_BYTE *)a2 + 320);
  *((_BYTE *)this + 2206) = *((_BYTE *)a2 + 653);
  *((_BYTE *)this + 2207) = *((_BYTE *)a2 + 654);
  *((_BYTE *)this + 2048) = *((_BYTE *)a2 + 683);
  *((_DWORD *)this + 513) = *((_DWORD *)a2 + 171);
  memcpy((char *)this + 2056, (char *)a2 + 688, 0xCu);
  memcpy((char *)this + 2080, (char *)a2 + 700, 0xCu);
  std::vector<int>::operator=((int *)this + 517, (int *)a2 + 178);
  advancealtar::ClearRewardData::operator=((int *)this + 523, (int *)a2 + 181);
  advancealtar::SurvivalRewardData::operator=((int)this + 2128, (int *)a2 + 190);
  *((_BYTE *)this + 1572) = 0;
  *((_DWORD *)this + 394) = *((_DWORD *)a2 + 91);
  *((_DWORD *)this + 404) = *((_DWORD *)a2 + 206);
  *((_DWORD *)this + 412) = *((_DWORD *)a2 + 214);
  *((_DWORD *)this + 416) = *((_DWORD *)a2 + 218);
  if ( *((_DWORD *)this + 416) )
  {
    std::vector<int>::reserve((int *)this + 413, *((_DWORD *)this + 416) + 1);
    for ( j = 0; ; ++j )
    {
      v20 = std::vector<std::pair<int,int>>::size((_DWORD *)a2 + 215);
      if ( v20 <= j )
        break;
      v18 = (int *)std::vector<std::pair<int,int>>::at((_DWORD *)a2 + 215, j);
      v19 = (_DWORD *)std::vector<int>::operator[]((_DWORD *)this + 413, *v18);
      *v19 = *(_DWORD *)(std::vector<std::pair<int,int>>::at((_DWORD *)a2 + 215, j) + 4);
    }
  }
  *((_DWORD *)this + 405) = *((_DWORD *)a2 + 207);
  if ( *((_DWORD *)this + 405) )
  {
    std::map<int,std::vector<RandomList>>::clear((int)this + 1624);
    std::map<int,std::vector<RandomList>>::begin(&v38, (int)a2 + 832);
    std::_Rb_tree_const_iterator<std::pair<int const,std::vector<RandomList>>>::_Rb_tree_const_iterator(&v26, &v38);
    std::map<int,std::vector<RandomList>>::end(&v39, (int)a2 + 832);
    std::_Rb_tree_const_iterator<std::pair<int const,std::vector<RandomList>>>::_Rb_tree_const_iterator(&v25, &v39);
    while ( std::_Rb_tree_const_iterator<std::pair<int const,std::vector<RandomList>>>::operator!=(&v26, &v25) )
    {
      v21 = std::_Rb_tree_const_iterator<std::pair<int const,std::vector<RandomList>>>::operator->(&v26) + 4;
      v22 = std::_Rb_tree_const_iterator<std::pair<int const,std::vector<RandomList>>>::operator->(&v26);
      std::make_pair<int const&,std::vector<RandomList> const&>(v42, v22, v21);
      std::pair<int const,std::vector<RandomList>>::pair<int,std::vector<RandomList>>(v41, (int)v42);
      std::map<int,std::vector<RandomList>>::insert((int)v40, (int)this + 1624, (int)v41);
      std::pair<int const,std::vector<RandomList>>::~pair((int)v41);
      std::pair<int,std::vector<RandomList>>::~pair((int)v42);
      std::_Rb_tree_const_iterator<std::pair<int const,std::vector<RandomList>>>::operator++(&v26);
    }
  }
  *((_DWORD *)this + 552) = *((_DWORD *)a2 + 241);
  std::vector<STNamedMonsterMapPos>::operator=((int *)this + 572, (int *)a2 + 242);
  *((_DWORD *)this + 417) = *((_DWORD *)a2 + 245);
  *((_DWORD *)this + 418) = *((_DWORD *)a2 + 246);
  *((_DWORD *)this + 419) = *((_DWORD *)a2 + 247);
  std::vector<stEventMonster_t>::operator=((int *)this + 420, (int *)a2 + 248);
  *((_DWORD *)this + 423) = *((_DWORD *)a2 + 251);
  *((_DWORD *)this + 424) = *((_DWORD *)a2 + 252);
  *((_DWORD *)this + 425) = *((_DWORD *)a2 + 253);
  std::vector<stSecondEventMonster_t>::operator=((int *)this + 426, (int *)a2 + 254);
  *((_BYTE *)this + 1716) = *((_DWORD *)a2 + 224) == 1;
  *((_BYTE *)this + 1717) = *((_DWORD *)a2 + 237) == 1;
  *((_DWORD *)this + 430) = *((_DWORD *)a2 + 238);
  std::vector<std::vector<float>>::operator=((int)this + 2304, (int *)a2 + 257);
  *((_BYTE *)this + 1736) = *((_BYTE *)a2 + 1164);
  if ( *((_BYTE *)a2 + 681) )
    *((_DWORD *)this + 1) = 1;
  *((_BYTE *)this + 2300) = *((_BYTE *)a2 + 682);
  *((_BYTE *)this + 2216) = *((_BYTE *)a2 + 1172);
  *((_DWORD *)this + 553) = *((_DWORD *)a2 + 294);
  TournamentRewardItemRate::operator=((int *)this + 561, (int *)a2 + 301);
  std::map<int,TournamentRewardRate>::operator=((int)this + 2220, (int)a2 + 1180);
  *((_DWORD *)this + 565) = *((_DWORD *)a2 + 305);
  std::map<unsigned char,unsigned int>::operator=((int)this + 2264, (int)a2 + 1224);
  std::map<int,UseCoinDungeonDiff>::operator=((int)this + 2332, (int)a2 + 1252);
  std::vector<DimensionPartyCount>::swap((_DWORD *)this + 431, (_DWORD *)a2 + 194);
  *((_BYTE *)this + 1737) = *((_BYTE *)a2 + 1248);
  *((_DWORD *)this + 435) = *((_DWORD *)a2 + 319);
  *((_BYTE *)this + 1744) = *((_BYTE *)a2 + 1284);
  *((_BYTE *)this + 1745) = *((_BYTE *)a2 + 1285);
  return this;
}

CDungeon *__cdecl CDungeon::InsertMap(CDungeon *this, int id, char flag, CMap *a4)
{
  CDungeon *result; // eax
  int id; // [esp+10h] [ebp-C8h] BYREF
  _DWORD value[2]; // [esp+14h] [ebp-C4h] BYREF
  int id; // [esp+1Ch] [ebp-BCh] BYREF
  int id; // [esp+20h] [ebp-B8h] BYREF
  _DWORD value[2]; // [esp+24h] [ebp-B4h] BYREF
  int id; // [esp+2Ch] [ebp-ACh] BYREF
  int id; // [esp+30h] [ebp-A8h] BYREF
  _DWORD value[2]; // [esp+34h] [ebp-A4h] BYREF
  int id; // [esp+3Ch] [ebp-9Ch] BYREF
  int id; // [esp+40h] [ebp-98h] BYREF
  _DWORD value[2]; // [esp+44h] [ebp-94h] BYREF
  int id; // [esp+4Ch] [ebp-8Ch] BYREF
  int id; // [esp+50h] [ebp-88h] BYREF
  _DWORD value[2]; // [esp+54h] [ebp-84h] BYREF
  int id; // [esp+5Ch] [ebp-7Ch] BYREF
  int id; // [esp+60h] [ebp-78h] BYREF
  _DWORD value[2]; // [esp+64h] [ebp-74h] BYREF
  int id; // [esp+6Ch] [ebp-6Ch] BYREF
  int id; // [esp+70h] [ebp-68h] BYREF
  _DWORD value[2]; // [esp+74h] [ebp-64h] BYREF
  int id; // [esp+7Ch] [ebp-5Ch] BYREF
  int id; // [esp+80h] [ebp-58h] BYREF
  _DWORD value[2]; // [esp+84h] [ebp-54h] BYREF
  int id; // [esp+8Ch] [ebp-4Ch] BYREF
  int id; // [esp+90h] [ebp-48h] BYREF
  _DWORD value[2]; // [esp+94h] [ebp-44h] BYREF
  int id; // [esp+9Ch] [ebp-3Ch] BYREF
  int id; // [esp+A0h] [ebp-38h] BYREF
  _DWORD value[2]; // [esp+A4h] [ebp-34h] BYREF
  int id; // [esp+ACh] [ebp-2Ch] BYREF
  int id; // [esp+B0h] [ebp-28h] BYREF
  _DWORD value[2]; // [esp+B4h] [ebp-24h] BYREF
  int id; // [esp+BCh] [ebp-1Ch] BYREF
  int id; // [esp+C0h] [ebp-18h] BYREF
  _DWORD value[2]; // [esp+C4h] [ebp-14h] BYREF
  int id[3]; // [esp+CCh] [ebp-Ch] BYREF

  if ( CMap::get_map_type(a4) )
  {
    if ( CMap::get_map_type(a4) == 2 )
    {
      if ( (a3 & 1) != 0 )
      {
        v19 = 1;
        std::pair<int const,CMap *>::pair<PATH_DIRECTION,CMap *&>(v18, (int)&v19, (int)&a4);
        std::multimap<int,CMap *>::insert(&v17, (int)this + 24 * a2 + 1188, (int)v18);
      }
      if ( (a3 & 2) != 0 )
      {
        v22 = 2;
        std::pair<int const,CMap *>::pair<PATH_DIRECTION,CMap *&>(v21, (int)&v22, (int)&a4);
        std::multimap<int,CMap *>::insert(&v20, (int)this + 24 * a2 + 1188, (int)v21);
      }
      if ( (a3 & 4) != 0 )
      {
        v25 = 4;
        std::pair<int const,CMap *>::pair<PATH_DIRECTION,CMap *&>(v24, (int)&v25, (int)&a4);
        std::multimap<int,CMap *>::insert(&v23, (int)this + 24 * a2 + 1188, (int)v24);
      }
      if ( (a3 & 8) != 0 )
      {
        v28 = 8;
        std::pair<int const,CMap *>::pair<PATH_DIRECTION,CMap *&>(v27, (int)&v28, (int)&a4);
        std::multimap<int,CMap *>::insert(&v26, (int)this + 24 * a2 + 1188, (int)v27);
      }
      *((_BYTE *)this + 1572) = 1;
      return this;
    }
    else
    {
      if ( (a3 & 1) != 0 )
      {
        v31 = 1;
        std::pair<int const,CMap *>::pair<PATH_DIRECTION,CMap *&>(v30, (int)&v31, (int)&a4);
        std::multimap<int,CMap *>::insert(&v29, (int)this + 24 * a2 + 804, (int)v30);
      }
      if ( (a3 & 2) != 0 )
      {
        v34 = 2;
        std::pair<int const,CMap *>::pair<PATH_DIRECTION,CMap *&>(v33, (int)&v34, (int)&a4);
        std::multimap<int,CMap *>::insert(&v32, (int)this + 24 * a2 + 804, (int)v33);
      }
      if ( (a3 & 4) != 0 )
      {
        v37 = 4;
        std::pair<int const,CMap *>::pair<PATH_DIRECTION,CMap *&>(v36, (int)&v37, (int)&a4);
        std::multimap<int,CMap *>::insert(&v35, (int)this + 24 * a2 + 804, (int)v36);
      }
      result = (CDungeon *)(a3 & 8);
      if ( (a3 & 8) != 0 )
      {
        v40[0] = 8;
        std::pair<int const,CMap *>::pair<PATH_DIRECTION,CMap *&>(v39, (int)v40, (int)&a4);
        return (CDungeon *)std::multimap<int,CMap *>::insert(&v38, (int)this + 24 * a2 + 804, (int)v39);
      }
    }
  }
  else
  {
    if ( (a3 & 1) != 0 )
    {
      v7 = 1;
      std::pair<int const,CMap *>::pair<PATH_DIRECTION,CMap *&>(v6, (int)&v7, (int)&a4);
      std::multimap<int,CMap *>::insert(&v5, (int)this + 24 * a2 + 420, (int)v6);
    }
    if ( (a3 & 2) != 0 )
    {
      v10 = 2;
      std::pair<int const,CMap *>::pair<PATH_DIRECTION,CMap *&>(v9, (int)&v10, (int)&a4);
      std::multimap<int,CMap *>::insert(&v8, (int)this + 24 * a2 + 420, (int)v9);
    }
    if ( (a3 & 4) != 0 )
    {
      v13 = 4;
      std::pair<int const,CMap *>::pair<PATH_DIRECTION,CMap *&>(v12, (int)&v13, (int)&a4);
      std::multimap<int,CMap *>::insert(&v11, (int)this + 24 * a2 + 420, (int)v12);
    }
    result = (CDungeon *)(a3 & 8);
    if ( (a3 & 8) != 0 )
    {
      v16 = 8;
      std::pair<int const,CMap *>::pair<PATH_DIRECTION,CMap *&>(v15, (int)&v16, (int)&a4);
      return (CDungeon *)std::multimap<int,CMap *>::insert(&v14, (int)this + 24 * a2 + 420, (int)v15);
    }
  }
  return result;
}

int __cdecl CDungeon::insertStart(CDungeon *this, int id, CMap *a3)
{
  _BYTE v4[12]; // [esp+18h] [ebp-20h] BYREF
  int id[2]; // [esp+24h] [ebp-14h] BYREF
  int id[3]; // [esp+2Ch] [ebp-Ch] BYREF

  v6[0] = CMap::get_index(a3);
  std::pair<int const,CMap *>::pair<int,CMap *&>(v5, (int)v6, (int)&a3);
  return __gnu_cxx::hash_map<int,CMap *,__gnu_cxx::hash<int>,std::equal_to<int>,std::allocator<CMap *>>::insert(
           (int)v4,
           (int)this + 20 * a2 + 100,
           v5);
}

int __cdecl CDungeon::GetRandMaze(CDungeon *this, int *a2)
{
  unsigned int id; // ebx

  v2 = rand();
  *a2 = v2 % std::vector<MazeScript>::size((_DWORD *)this + 395);
  return std::vector<MazeScript>::operator[]((_DWORD *)this + 395, *a2);
}

int __cdecl CDungeon::GetMaze(CDungeon *this, unsigned int id)
{
  if ( a2 < std::vector<MazeScript>::size((_DWORD *)this + 395) )
    return std::vector<MazeScript>::operator[]((_DWORD *)this + 395, a2);
  else
    return 0;
}

int __cdecl CDungeon::ChooseRandomMap(const char **this, int id, int value, int count)
{
  unsigned int id; // ebx
  unsigned int id; // ebx
  unsigned int id; // ebx
  unsigned int id; // [esp+3Ch] [ebp-6Ch]
  unsigned int id; // [esp+3Ch] [ebp-6Ch]
  unsigned int id; // [esp+3Ch] [ebp-6Ch]
  const std::_Rb_tree_node_base *v11; // [esp+4Ch] [ebp-5Ch] BYREF
  const std::_Rb_tree_node_base *v12; // [esp+50h] [ebp-58h] BYREF
  const std::_Rb_tree_node_base *v13; // [esp+54h] [ebp-54h] BYREF
  _DWORD value[2]; // [esp+58h] [ebp-50h] BYREF
  _BYTE dest[4]; // [esp+60h] [ebp-48h] BYREF
  _DWORD value[2]; // [esp+64h] [ebp-44h] BYREF
  _BYTE v17[4]; // [esp+6Ch] [ebp-3Ch] BYREF
  _DWORD value[2]; // [esp+70h] [ebp-38h] BYREF
  _BYTE v19[4]; // [esp+78h] [ebp-30h] BYREF
  _DWORD value[2]; // [esp+7Ch] [ebp-2Ch] BYREF
  _BYTE v21[4]; // [esp+84h] [ebp-24h] BYREF
  CMap *v22; // [esp+88h] [ebp-20h]
  int id; // [esp+8Ch] [ebp-1Ch]
  unsigned int i; // [esp+90h] [ebp-18h]
  unsigned int j; // [esp+94h] [ebp-14h]
  unsigned int k; // [esp+98h] [ebp-10h]
  int m; // [esp+9Ch] [ebp-Ch]

  if ( a2 < 0 || a3 < 0 || a4 < 0 )
  {
    LogManager::logFormat(
      (LogManager *)1,
      (int)"data_manager.cpp",
      "int CDungeon::ChooseRandomMap(int, int, int) const",
      (const char *)0x71B,
      "CDungeon::ChooseRandomMap [1](Dungeon Index(%d))(map_type(%d) < 0 || direction(%d) < 0 || entrance(%d) < 0)==false",
      *(this + 2),
      a2,
      a3,
      a4);
    return -1;
  }
  v22 = 0;
  std::pair<std::_Rb_tree_const_iterator<std::pair<int const,CMap *>>,std::_Rb_tree_const_iterator<std::pair<int const,CMap *>>>::pair(v14);
  std::_Rb_tree_const_iterator<std::pair<int const,CMap *>>::_Rb_tree_const_iterator(&v13);
  v23 = 0;
  if ( a2 == 1 )
  {
    if ( !a4 )
    {
      if ( !std::multimap<int,CMap *>::size((int)(this + 6 * a3 + 201)) )
      {
        LogManager::logFormat(
          (LogManager *)1,
          (int)"data_manager.cpp",
          "int CDungeon::ChooseRandomMap(int, int, int) const",
          (const char *)0x72C,
          "CDungeon::ChooseRandomMap (boss_maplist_[direction].size()==0)(Dungeon Index(%d))(map_type(%d),direction(%d),entrance(%d)",
          *(this + 2),
          1,
          a3,
          a4);
        return -1;
      }
      std::multimap<int,CMap *>::begin(&v11, (int)(this + 6 * a3 + 201));
      v13 = v11;
      for ( i = 0; ; ++i )
      {
        v5 = rand();
        v8 = std::multimap<int,CMap *>::size((int)(this + 6 * a3 + 201));
        if ( v5 % v8 <= i )
          break;
        std::_Rb_tree_const_iterator<std::pair<int const,CMap *>>::operator++(dest, &v13);
      }
      v22 = *(CMap **)(std::_Rb_tree_const_iterator<std::pair<int const,CMap *>>::operator->(&v13) + 4);
      if ( CMap::get_index(v22) == -1 )
        LogManager::logFormat(
          (LogManager *)1,
          (int)"data_manager.cpp",
          "int CDungeon::ChooseRandomMap(int, int, int) const",
          (const char *)0x736,
          "CDungeon::ChooseRandomMap (p_map->get_index()==-1)[1](Dungeon Index(%d))(map_type(%d),direction(%d),entrance(%d)",
          *(this + 2),
          1,
          a3,
          a4);
      return CMap::get_index(v22);
    }
    std::multimap<int,CMap *>::equal_range(v16, (int)(this + 6 * a3 + 201), &a4);
    std::pair<std::_Rb_tree_const_iterator<std::pair<int const,CMap *>>,std::_Rb_tree_const_iterator<std::pair<int const,CMap *>>>::operator=(
      v14,
      (int)v16);
    v23 = std::multimap<int,CMap *>::count((int)(this + 6 * a3 + 201), &a4);
  }
  else if ( a2 == 2 )
  {
    if ( !a4 )
    {
      if ( !std::multimap<int,CMap *>::size((int)(this + 6 * a3 + 297)) )
      {
        LogManager::logFormat(
          (LogManager *)1,
          (int)"data_manager.cpp",
          "int CDungeon::ChooseRandomMap(int, int, int) const",
          (const char *)0x745,
          "CDungeon::ChooseRandomMap (dummy_maplist_[direction].size()==0)(Dungeon Index(%d))(map_type(%d),direction(%d),entrance(%d)",
          *(this + 2),
          2,
          a3,
          a4);
        return -1;
      }
      std::multimap<int,CMap *>::begin(&v11, (int)(this + 6 * a3 + 297));
      v13 = v11;
      for ( j = 0; ; ++j )
      {
        v6 = rand();
        v9 = std::multimap<int,CMap *>::size((int)(this + 6 * a3 + 297));
        if ( v6 % v9 <= j )
          break;
        std::_Rb_tree_const_iterator<std::pair<int const,CMap *>>::operator++(v17, &v13);
      }
      v22 = *(CMap **)(std::_Rb_tree_const_iterator<std::pair<int const,CMap *>>::operator->(&v13) + 4);
      if ( CMap::get_index(v22) == -1 )
        LogManager::logFormat(
          (LogManager *)1,
          (int)"data_manager.cpp",
          "int CDungeon::ChooseRandomMap(int, int, int) const",
          (const char *)0x74F,
          "CDungeon::ChooseRandomMap (p_map->get_index()==-1)[1](Dungeon Index(%d))(map_type(%d),direction(%d),entrance(%d)",
          *(this + 2),
          2,
          a3,
          a4);
      return CMap::get_index(v22);
    }
    std::multimap<int,CMap *>::equal_range(v18, (int)(this + 6 * a3 + 297), &a4);
    std::pair<std::_Rb_tree_const_iterator<std::pair<int const,CMap *>>,std::_Rb_tree_const_iterator<std::pair<int const,CMap *>>>::operator=(
      v14,
      (int)v18);
    v23 = std::multimap<int,CMap *>::count((int)(this + 6 * a3 + 297), &a4);
  }
  else
  {
    if ( !a4 )
    {
      if ( !std::multimap<int,CMap *>::size((int)(this + 6 * a3 + 105)) )
      {
        LogManager::logFormat(
          (LogManager *)1,
          (int)"data_manager.cpp",
          "int CDungeon::ChooseRandomMap(int, int, int) const",
          (const char *)0x75E,
          "CDungeon::ChooseRandomMap (maplist_[direction].size()==0)(Dungeon Index(%d))(map_type(%d),direction(%d),entrance(%d)",
          *(this + 2),
          a2,
          a3,
          a4);
        return -1;
      }
      std::multimap<int,CMap *>::begin(&v11, (int)(this + 6 * a3 + 105));
      v13 = v11;
      for ( k = 0; ; ++k )
      {
        v7 = rand();
        v10 = std::multimap<int,CMap *>::size((int)(this + 6 * a3 + 105));
        if ( v7 % v10 <= k )
          break;
        std::_Rb_tree_const_iterator<std::pair<int const,CMap *>>::operator++(v19, &v13);
      }
      v22 = *(CMap **)(std::_Rb_tree_const_iterator<std::pair<int const,CMap *>>::operator->(&v13) + 4);
      if ( CMap::get_index(v22) == -1 )
        LogManager::logFormat(
          (LogManager *)1,
          (int)"data_manager.cpp",
          "int CDungeon::ChooseRandomMap(int, int, int) const",
          (const char *)0x768,
          "CDungeon::ChooseRandomMap (p_map->get_index()==-1)[2](Dungeon Index(%d))(map_type(%d),direction(%d),entrance(%d)",
          *(this + 2),
          a2,
          a3,
          a4);
      return CMap::get_index(v22);
    }
    std::multimap<int,CMap *>::equal_range(v20, (int)(this + 6 * a3 + 105), &a4);
    std::pair<std::_Rb_tree_const_iterator<std::pair<int const,CMap *>>,std::_Rb_tree_const_iterator<std::pair<int const,CMap *>>>::operator=(
      v14,
      (int)v20);
    v23 = std::multimap<int,CMap *>::count((int)(this + 6 * a3 + 105), &a4);
  }
  if ( v23 )
  {
    v12 = (const std::_Rb_tree_node_base *)v14[0];
    for ( m = 0; rand() % v23 > m; ++m )
      std::_Rb_tree_const_iterator<std::pair<int const,CMap *>>::operator++(v21, &v12);
    v22 = *(CMap **)(std::_Rb_tree_const_iterator<std::pair<int const,CMap *>>::operator->(&v12) + 4);
    if ( CMap::get_index(v22) == -1 )
      LogManager::logFormat(
        (LogManager *)1,
        (int)"data_manager.cpp",
        "int CDungeon::ChooseRandomMap(int, int, int) const",
        (const char *)0x786,
        "CDungeon::ChooseRandomMap (p_map->get_index()==-1)[3](Dungeon Index(%d))(map_type(%d),direction(%d),entrance(%d)",
        *(this + 2),
        a2,
        a3,
        a4);
    return CMap::get_index(v22);
  }
  else
  {
    LogManager::logFormat(
      (LogManager *)1,
      (int)"data_manager.cpp",
      "int CDungeon::ChooseRandomMap(int, int, int) const",
      (const char *)0x77B,
      "CDungeon::ChooseRandomMap (Key count is zero! : plz check map script)(Dungeon Index(%d))(map_type(%d),direction(%d),entrance(%d)",
      *(this + 2),
      a2,
      a3,
      a4);
    return -1;
  }
}

int __cdecl CDungeon::GetStartMap(CDungeon *this, char *a2)
{
  int id; // eax
  _DWORD value[2]; // [esp+20h] [ebp-28h] BYREF
  _BYTE dest[8]; // [esp+28h] [ebp-20h] BYREF
  _DWORD value[2]; // [esp+30h] [ebp-18h] BYREF
  signed int rand_int; // [esp+38h] [ebp-10h]
  signed int i; // [esp+3Ch] [ebp-Ch]

  if ( (unsigned int)a2 >= SIZE_16_BYTES )
    return -1;
  if ( __gnu_cxx::hash_map<int,CMap *,__gnu_cxx::hash<int>,std::equal_to<int>,std::allocator<CMap *>>::size((int)this + 20 * (_DWORD)a2 + 100) )
  {
    v3 = __gnu_cxx::hash_map<int,CMap *,__gnu_cxx::hash<int>,std::equal_to<int>,std::allocator<CMap *>>::size((int)this + 20 * (_DWORD)a2 + 100);
    rand_int = get_rand_int(v3);
    __gnu_cxx::hash_map<int,CMap *,__gnu_cxx::hash<int>,std::equal_to<int>,std::allocator<CMap *>>::begin(
      v4,
      (int)this + 20 * (_DWORD)a2 + 100);
    for ( i = 0; i < rand_int; ++i )
    {
      __gnu_cxx::_Hashtable_const_iterator<std::pair<int const,CMap *>,int,__gnu_cxx::hash<int>,std::_Select1st<std::pair<int const,CMap *>>,std::equal_to<int>,std::allocator<CMap *>>::operator++(
        dest,
        v4);
      __gnu_cxx::hash_map<int,CMap *,__gnu_cxx::hash<int>,std::equal_to<int>,std::allocator<CMap *>>::end(
        v6,
        (int)this + 20 * (_DWORD)a2 + 100);
      if ( __gnu_cxx::_Hashtable_const_iterator<std::pair<int const,CMap *>,int,__gnu_cxx::hash<int>,std::_Select1st<std::pair<int const,CMap *>>,std::equal_to<int>,std::allocator<CMap *>>::operator==(
             v4,
             v6) )
      {
        return -1;
      }
    }
    return *(_DWORD *)__gnu_cxx::_Hashtable_const_iterator<std::pair<int const,CMap *>,int,__gnu_cxx::hash<int>,std::_Select1st<std::pair<int const,CMap *>>,std::equal_to<int>,std::allocator<CMap *>>::operator->(v4);
  }
  else
  {
    LogManager::logFormat(
      (LogManager *)1,
      (int)"data_manager.cpp",
      "int CDungeon::GetStartMap(int) const",
      (const char *)0x79A,
      "CDungeon::GetStartMap() : direction(%d), dungeon index(%d), start_maplist_[direction].size() == 0, May be Script Error!",
      a2,
      *((_DWORD *)this + 2));
    return -1;
  }
}

int __cdecl CDungeon::get_clear_item(CDungeon *this)
{
  int id; // [esp+1Ch] [ebp-1Ch] BYREF
  int id; // [esp+20h] [ebp-18h] BYREF
  unsigned int id; // [esp+24h] [ebp-14h] BYREF
  int id; // [esp+28h] [ebp-10h] BYREF
  int id; // [esp+2Ch] [ebp-Ch]

  v6 = 0;
  std::_Rb_tree_const_iterator<std::pair<int const,int>>::_Rb_tree_const_iterator(&v3);
  v4 = get_rand_int(100) + 1;
  std::map<int,int>::lower_bound(&v2, (int)this + 28, &v4);
  v3 = v2;
  std::map<int,int>::end(&v5, (int)this + 28);
  if ( std::_Rb_tree_const_iterator<std::pair<int const,int>>::operator==(&v3, &v5) )
    return -2;
  else
    return *(_DWORD *)(std::_Rb_tree_const_iterator<std::pair<int const,int>>::operator->(&v3) + 4);
}

int __cdecl CDungeon::get_clear_item(CDungeon *this, int id)
{
  int id; // [esp+1Ch] [ebp-1Ch] BYREF
  int id; // [esp+20h] [ebp-18h] BYREF
  int id; // [esp+24h] [ebp-14h] BYREF
  int id; // [esp+28h] [ebp-10h] BYREF
  int id; // [esp+2Ch] [ebp-Ch]

  v7 = 0;
  std::_Rb_tree_const_iterator<std::pair<int const,int>>::_Rb_tree_const_iterator(&v4);
  v5 = a2 + 1;
  std::map<int,int>::lower_bound(&v3, (int)this + 28, &v5);
  v4 = v3;
  std::map<int,int>::end(&v6, (int)this + 28);
  if ( std::_Rb_tree_const_iterator<std::pair<int const,int>>::operator==(&v4, &v6) )
    return -2;
  else
    return *(_DWORD *)(std::_Rb_tree_const_iterator<std::pair<int const,int>>::operator->(&v4) + 4);
}

    if ( (unsigned __int8)CDungeon::VerifyMaze((const char **)this, v8) != 1 )
      return 0;

      if ( (unsigned __int8)CDungeon::VerifyMaze((const char **)this, (const MazeScript *)v6) != 1 )
      {
        a1 = 0;
        v3 = 0;
      }

int __cdecl CDungeon::VerifyMaze(const char **this, const MazeScript *a2)
{
  int *v3; // eax
  int id; // edx
  int *v5; // eax
  int id; // edx
  int m; // [esp+34h] [ebp-64h] BYREF
  int k; // [esp+38h] [ebp-60h] BYREF
  int id; // [esp+3Ch] [ebp-5Ch]
  int id; // [esp+40h] [ebp-58h]
  int id; // [esp+44h] [ebp-54h]
  int id; // [esp+48h] [ebp-50h]
  int id; // [esp+4Ch] [ebp-4Ch] BYREF
  int id; // [esp+50h] [ebp-48h] BYREF
  _DWORD value[2]; // [esp+54h] [ebp-44h] BYREF
  int id; // [esp+5Ch] [ebp-3Ch] BYREF
  int id; // [esp+60h] [ebp-38h] BYREF
  int id; // [esp+64h] [ebp-34h] BYREF
  int id; // [esp+68h] [ebp-30h] BYREF
  _DWORD value[2]; // [esp+6Ch] [ebp-2Ch] BYREF
  int id; // [esp+74h] [ebp-24h] BYREF
  int id; // [esp+78h] [ebp-20h] BYREF
  unsigned int i; // [esp+7Ch] [ebp-1Ch]
  _DWORD *GridR; // [esp+80h] [ebp-18h]
  unsigned int j; // [esp+84h] [ebp-14h]
  _DWORD *v26; // [esp+88h] [ebp-10h]
  _DWORD *v27; // [esp+8Ch] [ebp-Ch]

  if ( *(_DWORD *)a2 == 1 && *((_DWORD *)a2 + 1) == 1 )
    return 1;
  if ( std::vector<std::pair<int,int>>::size((_DWORD *)a2 + 5) )
  {
    for ( i = 0; std::vector<std::pair<int,int>>::size((_DWORD *)a2 + 5) > i; ++i )
    {
      v3 = (int *)std::vector<std::pair<int,int>>::operator[]((_DWORD *)a2 + 5, i);
      v4 = v3[1];
      v11 = *v3;
      v12 = v4;
      GridR = (_DWORD *)MazeScript::getGridR(a2, v11, v4);
      if ( !__gnu_cxx::hash_map<int,CMap *,__gnu_cxx::hash<int>,std::equal_to<int>,std::allocator<CMap *>>::size((int)(this + 5 * *GridR + 25)) )
        LogManager::logFormat(
          (LogManager *)1,
          (int)"data_manager.cpp",
          "bool CDungeon::VerifyMaze(const MazeScript&) const",
          (const char *)0x847,
          "dungeon index(%d) maze(%d) has error. (Can't found start map(grid: %d,%d)!)",
          *(this + 2),
          *((_DWORD *)a2 + 25),
          v11,
          v12);
    }
    if ( std::vector<std::pair<int,int>>::size((_DWORD *)a2 + 8) )
    {
      for ( j = 0; std::vector<std::pair<int,int>>::size((_DWORD *)a2 + 8) > j; ++j )
      {
        v5 = (int *)std::vector<std::pair<int,int>>::operator[]((_DWORD *)a2 + 8, j);
        v6 = v5[1];
        v9 = *v5;
        v10 = v6;
        v26 = (_DWORD *)MazeScript::getGridR(a2, v9, v6);
        if ( !std::multimap<int,CMap *>::size((int)(this + 6 * *v26 + 201)) )
          LogManager::logFormat(
            (LogManager *)1,
            (int)"data_manager.cpp",
            "bool CDungeon::VerifyMaze(const MazeScript&) const",
            (const char *)0x857,
            "dungeon index(%d) maze(%d) has error. (Can't found boss map(grid: %d,%d)!",
            *(this + 2),
            *((_DWORD *)a2 + 25),
            v9,
            v10);
      }
      for ( k = 0; *(_DWORD *)a2 > k; ++k )
      {
        for ( m = 0; *((_DWORD *)a2 + 1) > m; ++m )
        {
          v27 = (_DWORD *)MazeScript::getGridR(a2, k, m);
          if ( *v27 )
          {
            std::vector<std::pair<int,int>>::end(&v13, (int)a2 + 20);
            std::pair<int,int>::pair<int &,int &>(v15, (int)&k, (int)&m);
            std::vector<std::pair<int,int>>::end(&v16, (int)a2 + 20);
            std::vector<std::pair<int,int>>::begin(&v17, (_DWORD *)a2 + 5);
            std::find<__gnu_cxx::__normal_iterator<std::pair<int,int> const*,std::vector<std::pair<int,int>>>,std::pair<int,int>>(
              &v14,
              v17,
              v16,
              v15);
            if ( !__gnu_cxx::operator!=<std::pair<int,int> const*,std::vector<std::pair<int,int>>>((int)&v14, (int)&v13) )
            {
              std::vector<std::pair<int,int>>::end(&v18, (int)a2 + 32);
              std::pair<int,int>::pair<int &,int &>(v20, (int)&k, (int)&m);
              std::vector<std::pair<int,int>>::end(&v21, (int)a2 + 32);
              std::vector<std::pair<int,int>>::begin(&v22, (_DWORD *)a2 + 8);
              std::find<__gnu_cxx::__normal_iterator<std::pair<int,int> const*,std::vector<std::pair<int,int>>>,std::pair<int,int>>(
                &v19,
                v22,
                v21,
                v20);
              if ( !__gnu_cxx::operator!=<std::pair<int,int> const*,std::vector<std::pair<int,int>>>(
                      (int)&v19,
                      (int)&v18)
                && !std::multimap<int,CMap *>::size((int)(this + 6 * *v27 + 105)) )
              {
                LogManager::logFormat(
                  (LogManager *)1,
                  (int)"data_manager.cpp",
                  "bool CDungeon::VerifyMaze(const MazeScript&) const",
                  (const char *)0x86B,
                  "dungeon index(%d) maze(%d) has error. (Can't found grid map(grid: %d,%d)!)",
                  *(this + 2),
                  *((_DWORD *)a2 + 25),
                  k,
                  m);
              }
            }
          }
        }
      }
      return 1;
    }
    else
    {
      LogManager::logFormat(
        (LogManager *)1,
        (int)"data_manager.cpp",
        "bool CDungeon::VerifyMaze(const MazeScript&) const",
        (const char *)0x84E,
        "dungeon index(%d) maze(%d) has error. (End positions were not declared!)",
        *(this + 2),
        *((_DWORD *)a2 + 25));
      return 0;
    }
  }
  else
  {
    LogManager::logFormat(
      (LogManager *)1,
      (int)"data_manager.cpp",
      "bool CDungeon::VerifyMaze(const MazeScript&) const",
      (const char *)0x83E,
      "dungeon index(%d) maze(%d) has error. (Start positions were not declared!)",
      *(this + 2),
      *((_DWORD *)a2 + 25));
    return 0;
  }
}

int __cdecl CDungeon::GetRandMazebyPartyCount(CDungeon *this, int id)
{
  int id; // ebx
  unsigned int id; // eax
  unsigned int id; // ebx
  unsigned int id; // ebx
  void *v7[3]; // [esp+24h] [ebp-24h] BYREF
  int id; // [esp+30h] [ebp-18h] BYREF
  unsigned int id; // [esp+34h] [ebp-14h]
  unsigned int i; // [esp+38h] [ebp-10h]
  int id; // [esp+3Ch] [ebp-Ch]

  if ( std::vector<MazeScript>::size((_DWORD *)this + 395) == 1 )
    return std::vector<MazeScript>::operator[]((_DWORD *)this + 395, 0);
  std::vector<MazeScript *>::vector(v7);
  for ( i = 0; ; ++i )
  {
    v3 = std::vector<MazeScript>::size((_DWORD *)this + 395);
    if ( v3 <= i )
      break;
    v11 = std::vector<MazeScript>::operator[]((_DWORD *)this + 395, i);
    if ( *(_DWORD *)(v11 + 104) <= a2 && *(_DWORD *)(v11 + 108) >= a2 )
    {
      v8 = v11;
      std::vector<MazeScript *>::push_back((int)v7, (int)&v8);
    }
  }
  v9 = 0;
  if ( std::vector<MazeScript *>::empty(v7) )
  {
    v4 = rand();
    v9 = v4 % std::vector<MazeScript>::size((_DWORD *)this + 395);
    v2 = std::vector<MazeScript>::operator[]((_DWORD *)this + 395, v9);
  }
  else if ( std::vector<MazeScript *>::size(v7) == 1 )
  {
    v2 = *(_DWORD *)std::vector<MazeScript *>::operator[](v7, 0);
  }
  else
  {
    v5 = rand();
    v9 = v5 % std::vector<MazeScript *>::size(v7);
    v2 = *(_DWORD *)std::vector<MazeScript *>::operator[](v7, v9);
  }
  std::vector<MazeScript *>::~vector(v7);
  return v2;
}

int __cdecl CDungeon::get_dimension_member_count(CDungeon *this, DimensionPartyCount *a2)
{
  int id; // [esp+10h] [ebp-18h] BYREF
  int id; // [esp+14h] [ebp-14h] BYREF
  int id; // [esp+18h] [ebp-10h] BYREF
  _DWORD value[3]; // [esp+1Ch] [ebp-Ch] BYREF

  std::vector<DimensionPartyCount>::end(&v4, (int)this + 1724);
  std::vector<DimensionPartyCount>::begin(&v5, (_DWORD *)this + 431);
  std::find<__gnu_cxx::__normal_iterator<DimensionPartyCount const*,std::vector<DimensionPartyCount>>,DimensionPartyCount>(
    &v3,
    v5,
    v4,
    a2);
  std::vector<DimensionPartyCount>::end(v6, (int)this + 1724);
  if ( !__gnu_cxx::operator!=<DimensionPartyCount const*,std::vector<DimensionPartyCount>>((int)&v3, (int)v6) )
    return 0;
  *((_BYTE *)a2 + 2) = *(_BYTE *)(__gnu_cxx::__normal_iterator<DimensionPartyCount const*,std::vector<DimensionPartyCount>>::operator->((int)&v3)
                                + 2);
  return 1;
}

_BOOL4 __cdecl CDungeon::IsOpenScheduleDungeon(CDungeon *this)
{
  int ScheduleState; // [esp+1Ch] [ebp-Ch]

  ScheduleState = CDailyScheduleManager::GetScheduleState(
                    (int)GlobalData::s_DailyScheduleManager,
                    0,
                    *((_DWORD *)this + 2));
  return ScheduleState == 1 || ScheduleState == 2;
}

      CDungeon::CDungeon(a1);
    }
    v16 = a1;
    CDungeon::set_dungeon(a1, (STDungeonScript *)v9);
    CDungeonList::insert_dungeon(this, v16);
    v4 = G_CGameManager();
    QuickPartySystemManager = (QuickParty::CQuickPartySystemManager *)CGameManager::GetQuickPartySystemManager(v4);
    QuickParty::CQuickPartySystemManager::init_quick_party_pool(QuickPartySystemManager, v16);
    if ( CDungeonList::size(this) > STATIC_POOL_SIZE_1000 )
    {

  index = CDungeon::get_index(a2);
  __gnu_cxx::hash_map<int,CDungeon *,__gnu_cxx::hash<int>,std::equal_to<int>,std::allocator<CDungeon *>>::find(

    v10[0] = CDungeon::get_index(a2);
    std::pair<int const,CDungeon *>::pair<int,CDungeon *&>(v9, (int)v10, (int)&a2);

    v3 = (const char *)CDungeon::get_index(a2);
    LogManager::logFormat(

          CDungeon::InsertMap(dungeon, 15, 15, v28);
          CMapList::insert_map(a3[1], 15, 15, v28, a4);

            CDungeon::insertStart(dungeon, 15, v28);
        }
        for ( j = 0; ; ++j )
        {

          CDungeon::InsertMap(dungeon, v15, v7, v28);
          a1 = v28;

            CDungeon::insertStart(dungeon, v15, v28);
        }
      }
      std::_Rb_tree_iterator<std::pair<int const,std::string>>::operator++(v25, &v18);
    }
    else
    {
      CMapList::insert_map(a3[1], 15, 15, v28, a4);
      std::_Rb_tree_iterator<std::pair<int const,std::string>>::operator++(v21, &v18);
    }
  }
  v5 = 1;
LABEL_25:
  STMapScript::~STMapScript((STMapScript *)v11);
  return v5;
}

//----- (08360306) --------------------------------------------------------
int __cdecl CDataManager::InitLoginState(int id, int value, int count)
{
  CEnvironment *v3; // eax
  CEnvironment *v4; // eax
  CEnvironment *v6; // eax
  unsigned __int8 v8; // [esp+1Bh] [ebp-Dh]
  MySQL *DBHandle; // [esp+1Ch] [ebp-Ch]

  DBHandle = (MySQL *)DBMgr::GetDBHandle((int *)GlobalData::s_db_mgr, 6, 0);
  v3 = G_CEnvironment();
  if ( !CEnvironment::get_operation_type(v3) || (v4 = G_CEnvironment(), CEnvironment::get_operation_type(v4) == 2) )
  {
    MySQL::set_query(DBHandle, "upDate login_account_%d set login_status=0 where m_channel_no=%d", a2, a3);
    return (unsigned __int8)MySQL::exec(DBHandle, 1);
  }
  else
  {
    v6 = G_CEnvironment();
    if ( CEnvironment::get_operation_type(v6) == 1 )
    {
      if ( *((_DWORD *)G_CEnvironment() + 108) == 1 )
      {
        MySQL::set_query(DBHandle, "upDate login_account_%d set login_status=0", a2);
        return (unsigned __int8)MySQL::exec(DBHandle, 1);
      }
      else
      {
        return 1;
      }
    }
  }
  return v8;
}
// 83603FA: variable 'v8' is possibly undefined

//----- (08360400) --------------------------------------------------------
int __cdecl CDataManager::set_level_exp(CDataManager *this)
{
  int i; // [esp+Ch] [ebp-4h]

  for ( i = 0; i <= 199; ++i )
    *((_DWORD *)this + i + 10913) = STCharacterScript::naExpTable[i];
  return 1;
}
// 94FE960: using guessed type int STCharacterScript::naExpTable[200];

//----- (08360442) --------------------------------------------------------
int __cdecl CDataManager::get_level_exp(CDataManager *this, int id)
{
  if ( a2 > 1 && a2 <= 200 )
    return *((_DWORD *)this + a2 + 10911);
  else
    return 0;
}

//----- (08360470) --------------------------------------------------------
int __cdecl CDataManager::get_exp_level(CDataManager *this, int id)
{
  int i; // [esp+14h] [ebp-4h]

  for ( i = 1; i <= 99 && CDataManager::get_level_exp(this, i) <= a2; ++i )
    ;
  return i - 1;
}

//----- (083604B6) --------------------------------------------------------
int __cdecl CDataManager::get_level_section_exp(CDataManager *this, int id)
{
  if ( a2 > 1 && a2 <= 200 )
    return *((_DWORD *)this + a2 + 10912) - *((_DWORD *)this + a2 + 10911);
  else
    return 0;
}

//----- (083604FE) --------------------------------------------------------
int __cdecl CDataManager::get_level_exp_percent(CDataManager *this, int id, int id2)
{
  return a3 * CDataManager::get_level_section_exp(this, a2) / 100;
}

//----- (0836053C) --------------------------------------------------------
int __cdecl CDataManager::get_creature_level_exp(CDataManager *this, int id)
{
  if ( a2 > 1 && a2 <= 99 )
    return CreatureScript::ExpTable_[a2 - 2];
  else
    return 0;
}
// 94FEFA0: using guessed type int CreatureScript::ExpTable_[99];

//----- (08360562) --------------------------------------------------------
int __cdecl CDataManager::get_creature_exp_level(CDataManager *this, int id)
{
  int i; // [esp+14h] [ebp-4h]

  for ( i = 1; i <= 98 && CDataManager::get_creature_level_exp(this, i) <= a2; ++i )
    ;
  return i - 1;
}

//----- (083605A8) --------------------------------------------------------
int __cdecl CDataManager::set_mob_reward(CDataManager *this)
{
  __int16 v2; // [esp+10h] [ebp-18h] BYREF
  char flag; // [esp+13h] [ebp-15h]
  MySQL *DBHandle; // [esp+14h] [ebp-14h]
  int n_rows; // [esp+18h] [ebp-10h]
  int i; // [esp+1Ch] [ebp-Ch]

  DBHandle = (MySQL *)DBMgr::GetDBHandle((int *)GlobalData::s_db_mgr, 2, 0);
  MySQL::set_query(DBHandle, "seLect level, exp from monster_reward_ref");
  v3 = MySQL::exec(DBHandle, 1);
  if ( v3 != 1 )
    return 0;
  n_rows = MySQL::get_n_rows(DBHandle);
  if ( n_rows <= 0 )
    return 0;
  for ( i = 0; i < n_rows; ++i )
  {
    v3 = MySQL::fetch(DBHandle);
    if ( v3 != 1 )
      break;
    v3 = MySQL::get_short(DBHandle, 0, &v2);
    if ( v3 != 1 )
      return 0;
    v3 = MySQL::get_short(DBHandle, 1, (int *)this + v2 + 11113);
    if ( v3 != 1 )
      return 0;
  }
  *((_DWORD *)this + 11113) = 0;
  memcpy((char *)this + 45256, (char *)this + 44452, 0x324u);
  return 1;
}

//----- (08360716) --------------------------------------------------------
void __cdecl CDataManager::SetExpRate(CDataManager *this, float a2)
{
  int i; // [esp+2Ch] [ebp-Ch]

  if ( *((float *)this + 11515) != a2 )
  {
    memcpy((char *)this + 44452, (char *)this + 45256, 0x324u);
    *((float *)this + 11515) = a2;
    for ( i = 0; i <= 200; ++i )
      *((_DWORD *)this + i + 11113) = (__int64)((long double)*((unsigned int *)this + i + 11113) * a2);
  }
}

//----- (083607D8) --------------------------------------------------------
void __cdecl CDataManager::SetItemDropRate(CDataManager *this, float a2)
{
  float v2; // [esp+2Ch] [ebp-1Ch]
  int i; // [esp+30h] [ebp-18h]
  int value; // [esp+34h] [ebp-14h]
  int j; // [esp+38h] [ebp-10h]
  int value; // [esp+3Ch] [ebp-Ch]

  if ( *((float *)this + 11516) != a2 )
  {
    v2 = a2 / *((float *)this + 11516);
    *((float *)this + 11516) = a2;
    WongWork::CItemGeneratorMgr::setDropRatio(*((WongWork::CItemGeneratorMgr **)this + 9008), a2);
    for ( i = 0; std::vector<sItemGenRef>::size((_DWORD *)this + 282) > i; ++i )
    {
      v4 = std::vector<sItemGenRef>::operator[]((_DWORD *)this + 282, i);
      *(_DWORD *)(v4 + 16) = (int)((long double)*(int *)(v4 + 16) * v2);
    }
    for ( j = 0; std::vector<sItemGenRef>::size((_DWORD *)this + 266) > j; ++j )
    {
      v6 = std::vector<sItemGenRef>::operator[]((_DWORD *)this + 266, j);
      *(_DWORD *)(v6 + 16) = (int)((long double)*(int *)(v6 + 16) * v2);
    }
  }
}

//----- (08360914) --------------------------------------------------------
long double __cdecl CDataManager::BaseExpPenalty(CDataManager *this, int id, int value)
{
  int id; // [esp+Ch] [ebp-8h]

  v4 = a3 - a2;
  if ( a3 - a2 >= -6 )
  {
    if ( v4 >= -5 )
    {
      switch ( v4 )
      {
        case -5:
          return 0.5;
        case -4:
          return 0.75;
        case -3:
          return 1.0;
        case -2:
          return 1.0;
        case -1:
          return 1.0;
        default:
          if ( v4 )
          {

            v27 = CDungeon::get_index(v120);
            WongWork::CDungeonClear::addClearedDungeon((WongWork::CDungeonClear *)v56, v27, 4);

      SpecialPassiveObjectItems = CDungeon::getSpecialPassiveObjectItems(dungeon);
      std::vector<stDungeonAssignItem_t>::size(SpecialPassiveObjectItems);

      if ( CDungeon::getDungeonKind(v70) == 1 )
      {
        for ( i = 1; *((_DWORD *)v70 + 416) >= i && v69 != 1; ++i )
        {
          v11 = *(_DWORD *)std::vector<int>::operator[]((_DWORD *)v70 + 413, i);
          if ( v11 == CMap::get_index(a3) )
          {
            v69 = 1;
            break;
          }
        }
      }

  CDungeon::VerifyMaze(a1, a3);
  return 1;

      if ( CDungeon::getDungeonKind(dungeon) == 1 )
      {
        min_level = CDungeon::get_min_level(dungeon);
        *((_DWORD *)this + 2) = *std::min<int>((_DWORD *)this + 2, &min_level);
        *((_BYTE *)this + 12) = 1;
      }

      else if ( CDungeon::getDungeonKind(dungeon) == 2 )
      {
        v18 = CDungeon::get_min_level(dungeon);
        *((_DWORD *)this + 2) = *std::min<int>((_DWORD *)this + 2, &v18);
      }

        if ( CDungeon::get_index(dungeon) == 100 )
        {
          v19 = CDungeon::get_min_level(dungeon);
          *((_DWORD *)this + 2) = *std::min<int>((_DWORD *)this + 2, &v19);
        }

        standard_level = CDungeon::get_standard_level(dungeon);
        *((_DWORD *)this + 2) = *std::min<int>((_DWORD *)this + 2, &standard_level);

_BOOL4 __cdecl CDungeon::isTournamentDungeon(CDungeon *this)
{
  return *((_BYTE *)this + 2216) && *((char *)this + 2216) <= 1;
}

int __cdecl CDungeon::getTournamentRoundFatigue(CDungeon *this)
{
  if ( *((_DWORD *)this + 553) )
    return *((_DWORD *)this + 553);
  else
    return 1;
}

int __cdecl CDungeon::isRiskDungeon(CDungeon *this)
{
  return *((unsigned __int8 *)this + 1737);
}

int __cdecl CDungeon::GetPartyMemberCoinLimit(CDungeon *this)
{
  return *((_DWORD *)this + 435);
}

int __cdecl CDungeon::IsEnterEachMap(CDungeon *this)
{
  return *((unsigned __int8 *)this + 1744);
}

int __cdecl CDungeon::IsOpenEvenEnemy(CDungeon *this)
{
  return *((unsigned __int8 *)this + 1745);
}

_BOOL4 CDungeon::checkFreeRevivalCondition(CDungeon *this, ...)
{
  int id; // [esp+18h] [ebp-10h] BYREF
  _DWORD value[3]; // [esp+1Ch] [ebp-Ch] BYREF
  va_list va; // [esp+34h] [ebp+Ch] BYREF

  va_start(va, this);
  std::map<int,UseCoinDungeonDiff>::find(&v2, (int)this + 2332, (int *)va);
  std::map<int,UseCoinDungeonDiff>::end(v3, (int)this + 2332);
  return std::_Rb_tree_const_iterator<std::pair<int const,UseCoinDungeonDiff>>::operator!=(&v2, v3)
      && *(_BYTE *)(std::_Rb_tree_const_iterator<std::pair<int const,UseCoinDungeonDiff>>::operator->(&v2) + 8) != 1;
}

int __cdecl CDungeon::getLimitCoinDiff(CDungeon *this, int id, int *a3)
{
  int id; // eax
  int id; // [esp+18h] [ebp-10h] BYREF
  _DWORD value[3]; // [esp+1Ch] [ebp-Ch] BYREF

  if ( !std::map<int,UseCoinDungeonDiff>::size((int)this + 2332) )
    return 0;
  std::map<int,UseCoinDungeonDiff>::find(&v5, (int)this + 2332, &a2);
  std::map<int,UseCoinDungeonDiff>::end(v6, (int)this + 2332);
  if ( !std::_Rb_tree_const_iterator<std::pair<int const,UseCoinDungeonDiff>>::operator!=(&v5, v6) )
    return 0;
  v4 = std::_Rb_tree_const_iterator<std::pair<int const,UseCoinDungeonDiff>>::operator->(&v5);
  *a3 = *(_DWORD *)(v4 + 4);
  return 1;
}

void __cdecl CDungeon::CDungeon(CDungeon *this)
{
  char *v1; // edi
  int i; // ebx
  _DWORD *v3; // edi
  int j; // ebx
  _DWORD *v5; // edi
  int k; // ebx
  _DWORD *v7; // edi
  int m; // ebx
  _DWORD *v9; // edi
  int n; // ebx

  *(_DWORD *)this = &off_8C34A08;
  *((_DWORD *)this + 1) = 0;
  std::string::string((std::string *)this + 6);
  std::map<int,int>::map((_DWORD *)this + 7);
  std::vector<std::pair<int,int>>::vector((_DWORD *)this + 16);
  std::vector<std::pair<int,int>>::vector((_DWORD *)this + 19);
  std::vector<stDungeonAssignItem_t>::vector((_DWORD *)this + 22);
  v1 = (char *)this + 100;
  for ( i = 15; i != -1; --i )
  {
    __gnu_cxx::hash_map<int,CMap *,__gnu_cxx::hash<int>,std::equal_to<int>,std::allocator<CMap *>>::hash_map((int)v1);
    v1 += 20;
  }
  v3 = (_DWORD *)((char *)this + 420);
  for ( j = 15; j != -1; --j )
  {
    std::multimap<int,CMap *>::multimap(v3);
    v3 += 6;
  }
  v5 = (_DWORD *)((char *)this + 804);
  for ( k = 15; k != -1; --k )
  {
    std::multimap<int,CMap *>::multimap(v5);
    v5 += 6;
  }
  v7 = (_DWORD *)((char *)this + 1188);
  for ( m = 15; m != -1; --m )
  {
    std::multimap<int,CMap *>::multimap(v7);
    v7 += 6;
  }
  std::vector<MazeScript>::vector((_DWORD *)this + 395);
  v9 = (_DWORD *)((char *)this + 1592);
  for ( n = 1; n != -1; --n )
  {
    std::vector<MazeScript>::vector(v9);
    v9 += 3;
  }
  std::map<int,std::vector<RandomList>>::map((_DWORD *)this + 406);
  std::vector<int>::vector((_DWORD *)this + 413);
  std::vector<stEventMonster_t>::vector((_DWORD *)this + 420);
  std::vector<stSecondEventMonster_t>::vector((_DWORD *)this + 426);
  std::vector<DimensionPartyCount>::vector((_DWORD *)this + 431);
  *((_BYTE *)this + 1737) = 0;
  *((_DWORD *)this + 435) = 0;
  *((_BYTE *)this + 1744) = 0;
  *((_BYTE *)this + 1745) = 0;
  std::vector<int>::vector((_DWORD *)this + 437);
  STWarRoomData::STWarRoomData((CDungeon *)((char *)this + 1780));
  RequiredItem::RequiredItem((CDungeon *)((char *)this + 2036));
  std::vector<int>::vector((_DWORD *)this + 517);
  advancealtar::ClearRewardData::ClearRewardData((CDungeon *)((char *)this + 2092));
  advancealtar::SurvivalRewardData::SurvivalRewardData((CDungeon *)((char *)this + 2128));
  RequiredItem::RequiredItem((CDungeon *)((char *)this + 2144));
  std::vector<std::pair<int,int>>::vector((_DWORD *)this + 539);
  std::map<int,TournamentRewardRate>::map((_DWORD *)this + 555);
  TournamentRewardItemRate::TournamentRewardItemRate((CDungeon *)((char *)this + 2244));
  std::map<unsigned char,unsigned int>::map((_DWORD *)this + 566);
  std::vector<STNamedMonsterMapPos>::vector((_DWORD *)this + 572);
  std::vector<std::vector<float>>::vector((_DWORD *)this + 576);
  stConditionEnterBossRoom_t::stConditionEnterBossRoom_t((CDungeon *)((char *)this + 2316));
  std::map<int,UseCoinDungeonDiff>::map((_DWORD *)this + 583);
}

int CDungeon::_do_after_dungeon_start()
{
  return 1;
}

  CDungeon::CDungeon(this);
  *(_DWORD *)this = &off_8C349F8;

  *((_DWORD *)this + 1) = CDungeon::getDeathTowerMaxStage(*(CDungeon **)this);
  return 1;

  DeathTowerMapIndex = CDungeon::getDeathTowerMapIndex(*this, a2);
  v2 = (CMapList **)G_CDataManager();

        index = CDungeon::get_index(v10);
        QuickParty::RewardData_Result::RewardData_Result((QuickParty::RewardData_Result *)v52);

  if ( (unsigned __int8)CDungeon::limitOfStackableItemInTower(Dungeon) )
  {
    v22 = (float)(a4 * a3);
    v7 = *((float *)G_CDataManager() + a3 + 5223) * v22;
  }

  index = CDungeon::get_index(v14);
  v16 = G_CEnvironment();

                if ( CDungeon::get_index(Dungeon) == 11000 )
                {
                  v25 = v69;
                  InstanceCirculationStatistic = GetInstanceCirculationStatistic();
                  CCirculationStatistic::AddCirculationStatistic((int)InstanceCirculationStatistic, 4u, v65, v25);
                }

    DungeonName = CDungeon::GetDungeonName(Dungeon);
    cUserHistoryLog::LeaveDungeon(a2 + CUSER_OFFSET_HISTORY_LOG_ALT, DungeonName, 0, MemberNames, v7);

    index = CDungeon::get_index(v12);
    InstanceDungeonEntranceLog = (CDungeonEntranceLog *)GetInstanceDungeonEntranceLog();

      min_level = CDungeon::get_min_level(Dungeon);
      charac_level = CUserCharacInfo::get_charac_level(user);

          if ( (unsigned __int8)CDungeon::hasTowerItemDrop(Dungeon) )
            v80 = CMonster::deathTowerSpecifyItemDrop(monster);

          if ( (unsigned __int8)CDungeon::hasTowerItemDrop(v18) )
            CAICharacter::deathTowerSpecifyItemDrop((int)v103, (int)v90);

          index = CDungeon::get_index(v32);
          CUser::CheckQuestMonster(user, index, v31, v111);

  if ( (unsigned __int8)CDungeon::limitOfStackableItemInTower(Dungeon)
    && (*(_DWORD *)(InvenRef + 2) <= 0x1963u || *(_DWORD *)(InvenRef + 2) > 0x1B57u) )

          index = (WongWork::CDeathTowerRanking *)CDungeon::get_index(Dungeon);
          v85 = WongWork::CDeathTower::_checkRenewMyRecord(

            v15 = (WongWork::CDeathTowerRanking *)CDungeon::get_index(v14);
            if ( WongWork::CDeathTower::_checkRenewTopRecord(

          v18 = (WongWork::CDeathTowerRanking *)CDungeon::get_index(v17);
          DungeonIdx2TowerIdx = WongWork::CDeathTowerRanking::makeDungeonIdx2TowerIdx(v18);

          v87 = CDungeon::get_index(v20) != 11000;
          v21 = WongWork::CDeathTower::CPlayData::getStartMemberCnt((WongWork::CDeathTower::CPlayData *)(this + 68));

            v34 = (WongWork::CDeathTowerRanking *)CDungeon::get_index(v33);
            v35 = WongWork::CDeathTowerRanking::makeDungeonIdx2TowerIdx(v34);

              v38 = (WongWork::CDeathTowerRanking *)CDungeon::get_index(v37);
              v39 = WongWork::CDeathTowerRanking::makeDungeonIdx2TowerIdx(v38);

              v44 = (WongWork::CDeathTowerRanking *)CDungeon::get_index(v43);
              v45 = WongWork::CDeathTowerRanking::makeDungeonIdx2TowerIdx(v44);

            v50 = (WongWork::CDeathTowerRanking *)CDungeon::get_index(v49);
            v51 = WongWork::CDeathTowerRanking::makeDungeonIdx2TowerIdx(v50);

          v53 = (WongWork::CDeathTowerRanking *)CDungeon::get_index(v52);
          v54 = v85 == 1;

          if ( (unsigned __int8)CDungeon::limitOfStackableItemInTower(v57) )
            v88 = *((_DWORD *)G_CDataManager() + CurrentStage + 5423);

          TowerMaxClearRewardItemNum = CDungeon::getTowerMaxClearRewardItemNum(v58);
          v69 = *std::min<unsigned int>(&TowerMaxClearRewardItemNum, &v75);

      DungeonName = CDungeon::GetDungeonName(Dungeon);
      cUserHistoryLog::EnterDungeon((CUser **)user + CUSER_OFFSET_HISTORY_LOG_ALT, DungeonName, 0, v10, v2);

      index = CDungeon::get_index(v6);
      v8 = CUser::getCurCharacQuestW(user);

  v29 = CDungeon::get_index(Dungeon) != 11000;
  charac_level = CUserCharacInfo::get_charac_level(a3);

int __cdecl CDungeon::getDeathTowerMapIndex(CDungeon *this, unsigned int id)
{
  return *(_DWORD *)std::vector<int>::operator[]((_DWORD *)this + 413, a2);
}

int __cdecl CDungeon::getDeathTowerMaxStage(CDungeon *this)
{
  return *((_DWORD *)this + 416);
}

  return CDungeon::get_standard_level(*this);
}

//----- (08469AD6) --------------------------------------------------------
char *__cdecl WongWork::CDeathTower::CDungeonMgr::getObjectDropItems(CDungeon **this)
{

  return CDungeon::getObjectDropItems(*this);
}

//----- (08469AEC) --------------------------------------------------------
char *__cdecl WongWork::CDeathTower::CDungeonMgr::getSpecialPassiveObjectItems(CDungeon **this)
{

  return CDungeon::getSpecialPassiveObjectItems(*this);
}

//----- (08469B02) --------------------------------------------------------
int __cdecl WongWork::CDeathTower::CStage::getCurrentStage(WongWork::CDeathTower::CStage *this)
{

      HeroesRequiredQuest = CDungeon::getHeroesRequiredQuest(dungeon);
    if ( HeroesRequiredQuest )

      HeroesRequiredQuest = CDungeon::getHeroesRequiredQuest(dungeon);
    if ( HeroesRequiredQuest )

int __cdecl CDungeon::getHeroesRequiredQuest(CDungeon *this)
{
  return *((_DWORD *)this + 13);
}

            index = CDungeon::get_index(dungeon);
            std::list<int>::end(&v50, (int)DungeonLists);

          DungeonName = CDungeon::GetDungeonName((std::string *)dungeon);
          sprintf((char *)&v42, "item_indepedent_drop_%s.txt", DungeonName);

          v26 = CDungeon::GetDungeonName((std::string *)dungeon);
          sprintf((char *)&v42, "item_indepedent_drop_all_%s.txt", v26);

            if ( (unsigned __int8)CDungeon::get_blood_dungeon_type(v13) != 1
              && (unsigned __int8)CDungeon::get_blood_dungeon_type(v13) != 2

              && !CDungeon::isTowerOfDespairDungeon(v13)
              && !CDungeon::isTournamentDungeon(v13)

              && CDungeon::getDungeonKind(v13) != 1 )
            {
              v4 = *((_BYTE *)this + 8);
              index = CDungeon::get_index(v13);
              DungeonClearRefW = CUserCharacInfo::getDungeonClearRefW(v11);
              WongWork::CDungeonClear::addClearedDungeon((WongWork::CDungeonClear *)DungeonClearRefW, index, v4);
            }

            if ( (char)CDungeon::get_dimension_possible(v63) <= 0 )
              standard_level = CDungeon::get_standard_level(v63);

        DungeonName = CDungeon::GetDungeonName((std::string *)v21);
        String = (char *)RDARScriptStringManager::findString(

  result = CDungeon::get_clear_item(*(CDungeon **)(a2 + 12), Number);
  v8 = result;

      index = CDungeon::get_index(*(CDungeon **)(Party + 3244));
      v9[0] = 1;

          if ( charac_level <= CDungeon::get_standard_level(*(CDungeon **)(v7 + 3244)) + 10 )
          {
            *((_DWORD *)this + 3 * j + 20) = a3[j];
            *((_DWORD *)this + 3 * j + 21) += CurSec - *(_DWORD *)this;
            ++*((_DWORD *)this + 3 * j + 22);
          }

  if ( CDungeon::isTowerOfDespairDungeon(a2) )
    return 1;

    if ( CParty::get_user(this, i) && (char)CDungeon::get_blood_dungeon_type(*((CDungeon **)this + 811)) <= 0 )
    {
      user = (CUser *)CParty::get_user(this, i);
      CurCharacTotalFatigue = CUser::getCurCharacTotalFatigue(user);
      v5 = (CUser *)CParty::get_user(this, i);
      if ( CurCharacTotalFatigue == (unsigned __int16)CUser::getCurCharacTotalMaxFatigue(v5) )
        return 1;
    }

       && (unsigned __int8)CDungeon::isNoFatigueDungeon(*((CDungeon **)this + 811)) != 1
       && *(_BYTE *)(*((_DWORD *)this + 811) + 2048) != 1;

        index = CDungeon::get_index(dungeon);
        v19 = G_CEnvironment();

    v5 = *((_DWORD *)this + 811) && (char)CDungeon::get_dimension_possible(*((CDungeon **)this + 811)) > 0;
    if ( v5

       || (v6 = CDungeon::get_standard_level(*((CDungeon **)this + 811)) - 5,
           v6 > CUserCharacInfo::get_charac_level((CUserCharacInfo *)a3))

       || (v7 = CDungeon::get_standard_level(*((CDungeon **)this + 811)) + 3,
           v7 < CUserCharacInfo::get_charac_level((CUserCharacInfo *)a3))

  index = CDungeon::get_index(*(this + 811));
  v15 = G_CEnvironment();

int __cdecl CDungeon::get_limit_Inout_count(CDungeon *this)
{
  return *((unsigned __int8 *)this + 60);
}

long double __cdecl CDungeon::get_exp_weight(CDungeon *this)
{
  return *((float *)this + 5);
}

        standard_level = CDungeon::get_standard_level(*(CDungeon **)(v2 + 4));
        v4 = __gnu_cxx::_Hashtable_iterator<std::pair<int const,CDungeon *>,int,__gnu_cxx::hash<int>,std::_Select1st<std::pair<int const,CDungeon *>>,std::equal_to<int>,std::allocator<CDungeon *>>::operator->(v25);

        index = CDungeon::get_index(*(CDungeon **)(v4 + 4));
        secretshop::CSecretShop::LotteryItems((int)this, v27, 1002, index, standard_level);

      v10 = CDungeon::get_standard_level(*(CDungeon **)(v9 + 4));
      v11 = __gnu_cxx::_Hashtable_iterator<std::pair<int const,CDungeon *>,int,__gnu_cxx::hash<int>,std::_Select1st<std::pair<int const,CDungeon *>>,std::equal_to<int>,std::allocator<CDungeon *>>::operator->(v25);

      v12 = CDungeon::get_index(*(CDungeon **)(v11 + 4));
      v13 = __gnu_cxx::_Hashtable_iterator<std::pair<int const,CDungeon *>,int,__gnu_cxx::hash<int>,std::_Select1st<std::pair<int const,CDungeon *>>,std::equal_to<int>,std::allocator<CDungeon *>>::operator->(v25);

      DungeonName = CDungeon::GetDungeonName(*(std::string **)(v13 + 4));
      fprintf(stream, "Dungeon : %s(idx:%d, lev:%d)\n", DungeonName, v12, v10);

        v141 = CDungeon::get_index(Dungeon) != 11000;
        v27 = EquipmentRepairPrice;

        v149 = CDungeon::get_index(v66) != 11000;
        v81 = EquipmentRepairPrice;

      *((_DWORD *)this + 214) = CDungeon::get_index(CDungeonByLayer);
  }
  return *((_DWORD *)this + 214);
}

//----- (086440EE) --------------------------------------------------------
int __cdecl TowerOfDespairMgr::CheckEnterable(TowerOfDespairMgr *this, CUser *user, const CDungeon *dungeon)
{
  time_t CurSec; // esi
  time_t LastClearTime; // ebx
  CEnvironment *v6; // eax
  char *v7; // ebx
  CInventory *CurCharacInvenR; // eax
  int *v9; // eax
  _BYTE v10[7]; // [esp+17h] [ebp-51h] BYREF
  int id; // [esp+1Eh] [ebp-4Ah]
  TOD_UserState *CharacExpandData; // [esp+54h] [ebp-14h]
  CParty *Party; // [esp+58h] [ebp-10h]
  int id; // [esp+5Ch] [ebp-Ch]

  if ( !a2 )
    return 21;
  if ( CUserCharacInfo::get_charac_level(a2) < *((unsigned __int16 *)this + 423) )
    return 14;
  CharacExpandData = (TOD_UserState *)CUser::GetCharacExpandData((int)a2, 0xDu);
  if ( TOD_UserState::getEnterCount(CharacExpandData) <= 0 )
    goto LABEL_13;
  if ( (unsigned __int8)CUser::isGMUser(a2) != 1 )
  {

    DropItems = CDungeon::getDropItems(a6);
    v21 = G_CDataManager();

      DungeonName = CDungeon::GetDungeonName(*(this + 116));
      cUserHistoryLog::EnterDungeon(a3 + CUSER_OFFSET_HISTORY_LOG_ALT, DungeonName, v13);

      index = CDungeon::get_index(*((CDungeon **)this + 116));
      v9 = G_CEnvironment();

      DungeonName = CDungeon::GetDungeonName(*((std::string **)a1 + 116));
      cUserHistoryLog::LeaveDungeon(a2 + CUSER_OFFSET_HISTORY_LOG_ALT, DungeonName, v9);

      DungeonName = CDungeon::GetDungeonName(*((std::string **)this + 116));
      cUserHistoryLog::LeaveDungeon((CUser **)(*((_DWORD *)this + (_DWORD)(a2 + 72) + 3) + CUSER_OFFSET_HISTORY_LOG), DungeonName, v7);

        index = CDungeon::get_index(*(this + 116));
        InstanceDungeonEntranceLog = (CDungeonEntranceLog *)GetInstanceDungeonEntranceLog();

            index = CDungeon::get_index(*((CDungeon **)this + 116));
            CUser::CheckQuestMonster(a5, index, v7, v37);

    return CDungeon::get_min_level(dungeon);
  else

