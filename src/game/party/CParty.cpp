#include <party/CParty.h>
#include <game/party/CParty.h>

                CParty::send_invite_bluemarble(Party, unique_id);
                v7 = G_CGameManager();

        member_count = CParty::get_member_count(v15);
        Party = !WongWork::CHackAnalyzer::CheckHackRateByPartyCount((int)a1, a3, member_count);

  if ( !*((_DWORD *)this + 1) || (unsigned __int8)CParty::GetEPLPState(*((CParty **)this + 1)) != 2 )
    return 0;

    CParty::getMemberNames(*((CUserCharacInfo ***)this + 1), v8);
    for ( i = 0; i <= 3; ++i )

      user = CParty::get_user(*((CParty **)this + 1), i);
      if ( user && (unsigned __int8)CParty::checkValidUser(*((CUser ***)this + 1), i) == 1 )

    CParty::ReturnToVillage(a1, *((CParty **)this + 1));
  }
  WongWork::CBossTower::_destroy(this);
  return 1;
}

//----- (08142CDE) --------------------------------------------------------
int __usercall WongWork::CBossTower::_onTimerProcess@<eax>(char flag@<sil>, WongWork::CBossTower *a2, int id)
{
  int result; // eax

  result = a3;
  switch ( a3 )
  {

  CParty::getMemberNames(*((CUserCharacInfo ***)this + 1), &v12);
  for ( i = 0; i <= 3; ++i )

    user = (CUser *)CParty::get_user(*((CParty **)this + 1), i);
    if ( user && (unsigned __int8)CParty::checkValidUser(*((CUser ***)this + 1), i) == 1 )

      v20 = CParty::CheckEnterAncientDungeon(
              *((CUserCharacInfo ***)this + 1),

        CParty::send_to_party(*((CUser ***)this + 1), (PacketGuard *)v14);
        v6 = 0;

             && (v20 = CParty::UseAncientDungeonItems(
                         *((CParty **)this + 1),

        CParty::send_to_party(*((CUser ***)this + 1), (PacketGuard *)v13);
        v6 = 0;

  Manager = (CUser *)CParty::getManager(a3);
  if ( (unsigned __int8)CUser::isGMUser(Manager) && *((_BYTE *)Manager + 497356) )

  CParty::_getMemberNames(*((CUserCharacInfo ***)a2 + 1), s);
  for ( i = 0; i <= 3; ++i )

    user = (CUser *)CParty::get_user(*((CParty **)a2 + 1), i);
    if ( user && (unsigned __int8)CParty::checkValidUser(*((CUser ***)a2 + 1), i) == 1 )

  CParty::SetEPLPState(*((CParty **)a2 + 1), 1);
  CParty::gen_timer_key(*((_DWORD *)a2 + 1), 45);

  CParty::SetSelectedEPLPCmd(*((CParty **)a2 + 1), -1);
  PacketGuard::PacketGuard((PacketGuard *)v17);

  CParty::send_to_party(*((CUser ***)a2 + 1), (PacketGuard *)v17);
  WongWork::CBossStage::SendMapInfoPacket(a2);

  if ( CParty::get_member_count(*((CUser ***)a2 + 1)) == 1 )
    v20 = 90;

    user = (CUser *)CParty::get_user(*(this + 1), i);
    if ( user && (unsigned __int8)WongWork::CBossPlay::isMemberAlive((WongWork::CBossPlay *)this, i) )

  CParty::getMemberNames(*((CUserCharacInfo ***)this + 1), &v15);
  v8 = v17[1];

  member_count = CParty::get_member_count(*((CUser ***)this + 1));
  cHistoryTrace::operator()((int)v17, "member_count : %d", member_count);

  if ( !*((_DWORD *)this + 1) || CParty::get_member_count(*((CUser ***)this + 1)) <= 0 )
  {
    WongWork::CBossTower::_destroy(this);
    return 0;
  }

      || CParty::get_member_count(*((CUser ***)this + 1)) > 0
      && (*(unsigned __int8 (__cdecl **)(WongWork::CBossTower *))(*(_DWORD *)this + 8))(this) )

      v14 = CParty::get_member_count(*((CUser ***)this + 1));
      *(_DWORD *)&v17[2] = v14;

  if ( !*((_DWORD *)this + 1) || CParty::IsRoutingState(*((CParty **)this + 1)) )
  {
    CUser::SendCmdErrorPacket(a2, 46, 216);
    return 0;
  }

  MemberSlotNo = CParty::GetMemberSlotNo(*(this + 1), a2);
  if ( MemberSlotNo == -1 || !(unsigned __int8)CParty::checkValidUser(*(this + 1), MemberSlotNo) )

  member_count = CParty::get_member_count(*(this + 1));
  if ( ((unsigned __int8 (__cdecl *)(const CUser ***, int))v3)(this, member_count) )

    CParty::send_to_party(*(this + 1), (PacketGuard *)v6);
    PacketGuard::~PacketGuard(v6);

    user = (CUser *)CParty::get_user(*((CParty **)this + 1), i);
    v2 = !user || (unsigned __int8)CParty::checkValidUser(*((CUser ***)this + 1), i) != 1;

      user = CParty::get_user(*(this + 1), i);
      if ( user && (unsigned __int8)CParty::checkValidUser((CUser **)*(this + 1), i) == 1 )

  CParty::send_to_party((CUser **)*(this + 1), (PacketGuard *)v4);
  PacketGuard::~PacketGuard(v4);

    user = (CUser *)CParty::get_user(*(this + 1), i);
    if ( user && (unsigned __int8)CParty::checkValidUser((CUser **)*(this + 1), i) == 1 )

    CParty::send_to_party(*(this + 1), (PacketGuard *)v5);
    PacketGuard::~PacketGuard(v5);

  CParty::SetEPLPState((CParty *)*(this + 1), 2);
  v3 = (int)*(this + 700);

    user = (CUserCharacInfo *)CParty::get_user(*(this + 1), i);
    if ( user && (unsigned __int8)CParty::checkValidUser((CUser **)*(this + 1), i) == 1 )

    user = (CUserCharacInfo *)CParty::get_user(*(this + 1), i);
    if ( user && (unsigned __int8)CParty::checkValidUser((CUser **)*(this + 1), i) == 1 )

    user = (CUserCharacInfo *)CParty::get_user(*((CParty **)this + 1), i);
    if ( user && (unsigned __int8)CParty::checkValidUser(*((CUser ***)this + 1), i) == 1 )

    user = (CUserCharacInfo *)CParty::get_user(*(this + 1), i);
    if ( user && (unsigned __int8)CParty::checkValidUser((CUser **)*(this + 1), i) == 1 )

  if ( *((_DWORD *)this + 1) && (unsigned __int8)CParty::GetEPLPState(*((CParty **)this + 1)) == 2 )
  {
    memset(v7, 0, sizeof(v7));
    v8 = 0;
    v9 = 0;
    v10 = *(_DWORD *)(*((_DWORD *)this + 1) + 3288);
    CParty::getMemberNames(*((CUserCharacInfo ***)this + 1), v7);
    for ( i = 0; i <= 3; ++i )
    {
      user = CParty::get_user(*((CParty **)this + 1), i);
      if ( user && (unsigned __int8)CParty::checkValidUser(*((CUser ***)this + 1), i) == 1 )
      {
        a1 = v10;
        v4 = *((_DWORD *)this + 698);
        DungeonName = CDungeon::GetDungeonName(*((std::string **)this + 636));
        cUserHistoryLog::LeaveDungeon((CUser **)(user + CUSER_OFFSET_HISTORY_LOG), DungeonName, v4, v7, a1);
      }
    }
    CParty::ReturnToVillage(a1, *((CParty **)this + 1));
    WongWork::CBossTower::_destroy(this);
  }

  member_count = CParty::get_member_count(*(this + 1));
  if ( ((unsigned __int8 (__cdecl *)(CUser ***, int))v1)(this, member_count) )

  if ( !*(this + 1) || CParty::get_member_count(*(this + 1)) <= 0 )
  {
    WongWork::CBossTower::_destroy((WongWork::CBossTower *)this);
    return 0;
  }

      user = (CUser *)CParty::get_user((CParty *)*(this + 1), i);
      v5 = !user || (unsigned __int8)CParty::checkValidUser(*(this + 1), i) != 1;

    if ( CParty::get_member_count(*(this + 1)) <= 0 )
    {
      WongWork::CBossTower::_destroy((WongWork::CBossTower *)this);
    }

      CParty::send_to_party(*(this + 1), (PacketGuard *)v6);
      PacketGuard::~PacketGuard(v6);

  MemberSlotNo = CParty::GetMemberSlotNo(*(this + 1), a2);
  if ( MemberSlotNo == -1 )

int __cdecl CParty::get_user(CParty *this, int id)
{
  return *((_DWORD *)this + 6 * a2 + 30);
}

int __cdecl CParty::getManager(CParty *this)
{
  return *((_DWORD *)this + 29);
}

char __cdecl CParty::SetEPLPState(CParty *this, char flag)
{
  int value; // eax
  int index; // ebx
  CDungeonEntranceLog *InstanceDungeonEntranceLog; // eax
  int i; // [esp+2Ch] [ebp-Ch]

  LOBYTE(v2) = (_BYTE)this;
  *((_BYTE *)this + 107) = a2;
  if ( a2 == 2 )
  {
    v2 = *((_DWORD *)this + 811);
    if ( v2 )
    {
      for ( i = 0; ; ++i )
      {
        LOBYTE(v2) = i <= 3;
        if ( i > 3 )
          break;
        if ( (unsigned __int8)CParty::_checkValidUser((CUser **)this, i) == 1 )
        {
          index = CDungeon::get_index(*((CDungeon **)this + 811));
          InstanceDungeonEntranceLog = (CDungeonEntranceLog *)GetInstanceDungeonEntranceLog();
          CDungeonEntranceLog::DecrementDungeonEntrance(InstanceDungeonEntranceLog, index);
        }
      }
    }
  }
  return v2;
}

int __cdecl CParty::GetEPLPState(CParty *this)
{
  return *((unsigned __int8 *)this + 107);
}

CParty *__cdecl CParty::SetSelectedEPLPCmd(CParty *this, char flag)
{
  *((_BYTE *)this + 108) = a2;
  return this;
}

_BOOL4 __cdecl CParty::IsRoutingState(CParty *this)
{
  return (unsigned __int8)CParty::CItemRoutingData::GetRoutingState(this) != 0;
}

int __cdecl CParty::checkValidUser(CUser **this, int id)
{
  return CParty::_checkValidUser(this, a2);
}

char *__cdecl CParty::getMemberNames(CUserCharacInfo **this, char *s)
{
  return CParty::_getMemberNames(this, s);
}

    member_count = CParty::get_member_count(a2);
    v12 = member_count;

        CParty::giveup_game(a1, Party, this, 1, 0, 0);
      break;

      CParty::useItem(Party, this, a2);
    return 1;

    user = (CUser *)CParty::get_user(*(this + 1), i);
    if ( user && (unsigned __int8)CParty::checkValidUser((CUser **)*(this + 1), i) == 1 )

      v4 = (CUser *)CParty::get_user(*(this + 1), i);
      CUser::setBossTowerIndex(v4, a3);

    if ( (unsigned __int8)CParty::checkValidUser(*(this + 1), i)
      && (unsigned __int8)WongWork::CBossPlay::isMemberAlive((WongWork::CBossPlay *)this, i) )

    if ( (unsigned __int8)CParty::checkValidUser(*(this + 1), i)
      && (CUser *)CParty::get_user((CParty *)*(this + 1), i) == a2 )

      CParty::send_to_party(*(this + 1), (PacketGuard *)v7);
      RevivalTime = CUser::GetRevivalTime(a2);

        CParty::send_to_party(*(this + 1), (PacketGuard *)v6);
        PacketGuard::~PacketGuard(v6);

    if ( (unsigned __int8)CParty::checkValidUser(*((CUser ***)this + 1), i)
      && (CUser *)CParty::get_user(*((CParty **)this + 1), i) == a3 )

            user = (CLuckPoint *)CParty::get_user(*(CParty **)(a1 + 4), a2);
            CLuckPoint::UseLuckPoint(user, v27, rarity, grade);

      member_count = CParty::get_member_count(*(CUser ***)(a1 + 4));
      v34 = 1;

  user = (CUserCharacInfo *)CParty::get_user(*(this + 1), a2);
  if ( !user )

  AvatarBonus = CParty::getAvatarBonus((CUser **)*(this + 1));
  CreatureBonus = CParty::getCreatureBonus(*(this + 1), user);

  ClearRewardBonusExp = CParty::getClearRewardBonusExp(
                          *(this + 1),

long double __cdecl CParty::getAvatarBonus(CUser **this)
{
  return (float)CParty::CalcAvatarBonus(this);
}

long double __cdecl CParty::getCreatureBonus(CParty *this, CUser *user)
{
  return (float)CParty::CalcCreatureBonus(this, a2);
}

  CParty::send_to_party(*((CUser ***)this + 1), (PacketGuard *)v18);
  PacketGuard::~PacketGuard(v18);

  v17[2] = CParty::getMemberLevelGap(*((CUserCharacInfo ***)this + 1));
  for ( i = 0; i <= 3; ++i )

    user = (CUserCharacInfo *)CParty::get_user(*((CParty **)this + 1), i);
    v3 = !user || (unsigned __int8)CParty::checkValidUser(*((CUser ***)this + 1), i) != 1;

      *a3 = CParty::getMonsterTotalExp(
              *((CParty **)this + 1),

      *a3 = CParty::sumGainedEachExp(*((CParty **)this + 1), user, (const STGainedEachExpData *)v17);
      CUserCharacInfo::calcHelpAbuseRatio(user, *a3, 0);

      member_count = CParty::get_member_count(*((CUser ***)this + 1));
      CMonster::generateIndependentItem(monster, (WongWork::stGenerateRefData_t *)v21, (int)v29);

      v28 = CParty::get_member_count(*((CUser ***)this + 1));
      CAICharacter::generateIndependentItem(v44, (WongWork::stGenerateRefData_t *)v25, (int)v29);

  CParty::send_to_party(*((CUser ***)this + 1), (PacketGuard *)v33);
  PacketGuard::~PacketGuard(v33);

    user = (CUserCharacInfo *)CParty::get_user(*(this + 1), i);
    if ( user && (unsigned __int8)CParty::checkValidUser((CUser **)*(this + 1), i) == 1 )

        CParty::send_to_party((CUser **)*(this + 1), (PacketGuard *)v18);
        v11 = 1;

    CParty::send_to_party(*(this + 1), (PacketGuard *)v12);
    v6 = 1;

      user = (CUserCharacInfo *)CParty::get_user(*((CParty **)this + 1), j);
      if ( user && (unsigned __int8)CParty::checkValidUser(*((CUser ***)this + 1), j) )

    CParty::send_to_party(*((CUser ***)this + 1), (PacketGuard *)v10);
    v4 = 1;

    user = (CUser *)CParty::get_user(*((CParty **)this + 1), i);
    if ( user && (unsigned __int8)CParty::checkValidUser(*((CUser ***)this + 1), i) == 1 )

      if ( CParty::get_member_count(*(this + 1)) > 1
        && WongWork::CBossPlay::getLiveMemberCount((WongWork::CBossPlay *)this) > 1

        if ( CParty::get_member_count(*(this + 1)) > 1
          && WongWork::CBossPlay::getLiveMemberCount((WongWork::CBossPlay *)this) > 1

    CParty::send_to_party(*((CUser ***)this + 1), (PacketGuard *)v17);
    InterfacePacketBuf::clear(v17);

    CParty::send_to_party(a1[1], (PacketGuard *)v12);
    v5 = 1;

int __cdecl CParty::getMemberLevelGap(CUserCharacInfo **this)
{
  return CParty::_getMemberLevelGap(this);
}

      if ( CParty::IsUseFatigueDungeon(Party) )
      {
        v4 = (CParty *)CUser::GetParty(a2);
        if ( !(unsigned __int8)CParty::CheckMapClear(v4) )
        {
          std::vector<int>::vector(v5);
          if ( a3 <= 9 )
            AvatarRechargeServer::CalcDurabilityDetail((int)this, a2, a3, (int)v5);
          std::vector<int>::~vector((void **)v5);
        }
      }

        CParty::BloodHandleDieUser(v12, (CUser *)a2);
        *((_DWORD *)a4 + 1) = MAX_INT32;

          CParty::die_user(Party, (CUser *)a2);
        return 0;

          v11 = CParty::drop_item(
                  Party,

  if ( !(unsigned __int8)CParty::get_item(
                           (const CUser **)Party,

  if ( (unsigned __int8)CParty::IsAutoCreated(Party) || CParty::get_member_count((CUser **)Party) == 1 )
  {
    v17 = *(_WORD *)((char *)v32 + 19);
    v18 = *(_WORD *)((char *)v32 + 17);
    BackupSeed = CUser::getBackupSeed(a2);
    if ( !checkGetItemPacket(BackupSeed, v18, v17) )
    {
      v20 = CUser::getHackAnalyzer(a2);
      WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)v20, a2, 0x12Eu, 1, 0, 0);
    }
  }

        CParty::giveup_game(a1, Party, a3, 1, 0, 0);
    }
    return v5;
  }
  else
  {
    acc_id = CUser::get_acc_id(a3);
    return LineFunc(4087, "virtual int DisPatcher_GiveUpGame::process(CUser*, MSG_BASE&, ParamBase&)", 0, acc_id);
  }
}

//----- (081C41E8) --------------------------------------------------------
int __cdecl DisPatcher_GiveUpGame::check_error(DisPatcher_GiveUpGame *this, CUser *user)
{
  if ( CUser::get_state(a2) != 5
    && CUser::get_state(a2) != 10
    && CUser::get_state(a2) != 12
    && CUser::get_state(a2) != 13 )
  {
    return 19;
  }
  else
  {
    return 0;
  }
}

//----- (081C424C) --------------------------------------------------------
int DisPatcher_LeaveParty::read()
{
  return 0;
}

//----- (081C4256) --------------------------------------------------------
int __cdecl DisPatcher_LeaveParty::process(DisPatcher_LeaveParty *this, CUser *user, MSG_BASE *a3, ParamBase *a4)
{
  unsigned int acc_id; // eax
  pvp_assault::CAssaultMgr *InstanceAssaultMgr; // eax
  std::string **Party; // [esp+18h] [ebp-10h]

  *((_DWORD *)a4 + 1) = DisPatcher_LeaveParty::check_error(this, a2);
  if ( *((int *)a4 + 1) > 0 )
    return 0;
  if ( *((int *)a4 + 1) >= 0 )
  {
    Party = (std::string **)CUser::GetParty(a2);
    if ( CParty::leave_user(Party, (std::string *)a2, 2) == 1 )
      *((_DWORD *)a4 + 1) = 1;
    InstanceAssaultMgr = (pvp_assault::CAssaultMgr *)pvp_assault::GetInstanceAssaultMgr();
    pvp_assault::CAssaultMgr::ForcedMoveAtPowerWarPlace(InstanceAssaultMgr, a2);
    return 0;
  }
  else
  {
    acc_id = CUser::get_acc_id(a2);
    return LineFunc(
             4255,
             "virtual int DisPatcher_LeaveParty::process(CUser*, MSG_BASE&, ParamBase&)",
             *((_DWORD *)a4 + 1),
             acc_id);
  }
}

//----- (081C4326) --------------------------------------------------------
int __cdecl DisPatcher_LeaveParty::send(DisPatcher_LeaveParty *this, CUser *user, ParamBase *a3)
{
  int result; // eax

  result = *((_DWORD *)a3 + 1);
  if ( result != MAX_INT32 )
  {
    result = *((_DWORD *)a3 + 1);
    if ( result )
      return CUser::SendCmdErrorPacket(a2, 13, *((_DWORD *)a3 + 1));
  }
  return result;
}

//----- (081C436E) --------------------------------------------------------
int __cdecl DisPatcher_LeaveParty::check_error(DisPatcher_LeaveParty *this, CUser *user)
{
  if ( CUser::GetParty(a2) )
    return 0;
  else
    return 18;
}

//----- (081C4396) --------------------------------------------------------
int __cdecl DisPatcher_SetPartyInfo::read(DisPatcher_SetPartyInfo *this, PacketBuf *a2, MSG_BASE *a3)
{
  if ( (unsigned __int8)PacketBuf::get_byte(a2, (unsigned __int8 *)a3 + 13) != 1 )
    return LineFunc(4313, "virtual int DisPatcher_SetPartyInfo::read(PacketBuf&, MSG_BASE&)", 0, 0);
  if ( *((_BYTE *)a3 + 13) )
    goto LABEL_17;
  if ( (unsigned __int8)PacketBuf::get_int(a2, (int *)((char *)a3 + 14)) != 1 )
    return LineFunc(4317, "virtual int DisPatcher_SetPartyInfo::read(PacketBuf&, MSG_BASE&)", 0, 0);
  if ( (unsigned __int8)PacketBuf::get_str(a2, (char *)a3 + 18, 32, *(_DWORD *)((char *)a3 + 14)) != 1 )
    return LineFunc(4320, "virtual int DisPatcher_SetPartyInfo::read(PacketBuf&, MSG_BASE&)", 0, 0);
LABEL_17:
  if ( (unsigned __int8)PacketBuf::get_byte(a2, (unsigned __int8 *)a3 + 50) != 1 )
    return LineFunc(4322, "virtual int DisPatcher_SetPartyInfo::read(PacketBuf&, MSG_BASE&)", 0, 0);
  if ( (unsigned __int8)PacketBuf::get_short(a2, (unsigned __int16 *)((char *)a3 + 51)) != 1 )
    return LineFunc(4325, "virtual int DisPatcher_SetPartyInfo::read(PacketBuf&, MSG_BASE&)", 0, 0);
  if ( (unsigned __int8)PacketBuf::get_byte(a2, (unsigned __int8 *)a3 + 53) != 1 )
    return LineFunc(4326, "virtual int DisPatcher_SetPartyInfo::read(PacketBuf&, MSG_BASE&)", 0, 0);
  return 0;
}

//----- (081C4562) --------------------------------------------------------
int __usercall DisPatcher_SetPartyInfo::process@<eax>(
        char flag@<sil>,
        DisPatcher_SetPartyInfo *this,
        CUser *user,
        MSG_BASE *a4,
        ParamBase *a5)
{
  unsigned int acc_id; // eax
  CGameManager *v7; // eax
  GameWorld *v8; // eax
  GameWorld *v9; // eax
  CParty *Party; // [esp+18h] [ebp-10h]
  CParty *party; // [esp+1Ch] [ebp-Ch]

  *((_DWORD *)a5 + 1) = DisPatcher_SetPartyInfo::check_error(a1, this, a3, a4);
  if ( *((int *)a5 + 1) > 0 )
    return 0;
  if ( *((int *)a5 + 1) < 0 )
  {
    acc_id = CUser::get_acc_id(a3);
    return LineFunc(
             4342,
             "virtual int DisPatcher_SetPartyInfo::process(CUser*, MSG_BASE&, ParamBase&)",
             *((_DWORD *)a5 + 1),
             acc_id);
  }
  Party = (CParty *)CUser::GetParty(a3);
  if ( Party )
  {
    if ( (CUser *)CParty::getManager(Party) != a3 )
    {

    CParty::SetPartyInfoUI(Party, (int)a4);
    v9 = G_GameWorld();

    CParty::create_party(v11, a3);
    CParty::SetPartyInfoUI(v11, (int)a4);

          if ( (unsigned __int8)CParty::checkValidUser((CUser **)v11, i) == 1 )
          {
            user = (CUser *)CParty::get_user(v11, i);
            if ( user != a2 )
            {
              v7 = CUser::getHackAnalyzer(user);
              WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)v7, user, 0x1FAu, 1, 0, 0);
            }
            str_ip = (char *)CNetwork<NETWORK_BUFFER_SIZE_4K,MAX_NETWORK_CONNECTIONS_450K>::get_str_ip((int)user + 224);
            acc_id = (DB_ReqSaveHackUserPunish *)CUser::get_acc_id(user);
            DB_ReqSaveHackUserPunish::makeRequest(acc_id, 0, str_ip);
            CUser::DisConnSig((pthread_mutex_t *)user, 24, 1, 0);
          }

      v21 = (unsigned int)CParty::GetMapPlayingTime(v22) / 0x3E8;
    }
    for ( i = 0; ; ++i )
    {

      if ( (unsigned __int8)CParty::checkValidUser((CUser **)v22, i) == 1 )
      {
        user = (CUser *)CParty::get_user(v22, i);
        if ( user )
        {
          if ( *(_DWORD *)(a3 + 4 * (i + 8) + 3) > 0x2710u )
          {
            v26 = *(_DWORD *)(a3 + 4 * (i + 8) + 3) / 0x2710u;
            v27 = *(_DWORD *)(a3 + 4 * (i + 8) + 3) % 0x2710u;
            if ( *(_DWORD *)(CUser::GetParty(user) + 3244)
              && (v5 = CUser::GetParty(user), CDungeon::get_index(*(CDungeon **)(v5 + 3244)) == 11006) )
            {
              HackAnalyzer = CUser::getHackAnalyzer(user);
              WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)HackAnalyzer, user, 0x82u, 1, v26, 0);
            }
            else
            {
              v7 = CUser::getHackAnalyzer(user);
              WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)v7, user, 0x77u, 1, v26, 0);
            }
            *(_DWORD *)(a3 + 4 * (i + 8) + 3) = v27;
          }
          if ( CUserCharacInfo::get_charac_job(user) == 3
            || CUserCharacInfo::get_charac_job(user) == 4 && CUserCharacInfo::getCurCharFirstGrowType(user) == 2 )
          {
            v9 = (long double)*(unsigned int *)(a3 + 4 * (i + 8) + 3) * 0.5;
          }
          else
          {
            v9 = (long double)*(unsigned int *)(a3 + 4 * (i + 8) + 3) * 0.8;
          }
          *(_DWORD *)(a3 + 4 * (i + 8) + 3) = (__int64)v9;
          if ( !CBattle_Field::check_start_point((CParty *)((char *)v22 + 2852))
            && *(_DWORD *)(a3 + 4 * (i + 8) + 3) > v21 )
          {
            if ( (long double)*(unsigned int *)(a3 + 4 * (i + 8) + 3) <= (long double)v21 * 1.5 )
            {
              if ( *(_DWORD *)(CUser::GetParty(user) + 3244)
                && (v15 = CUser::GetParty(user), CDungeon::get_index(*(CDungeon **)(v15 + 3244)) == 11006) )
              {
                v16 = *(_DWORD *)(a3 + 4 * (i + 8) + 3);
                v17 = CUser::getHackAnalyzer(user);
                WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)v17, user, 0x81u, 1, v16, 0);
              }
              else
              {
                v18 = *(_DWORD *)(a3 + 4 * (i + 8) + 3);
                v19 = CUser::getHackAnalyzer(user);
                WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)v19, user, 0x74u, 1, v18, 0);
              }
            }
            else
            {
              for ( j = 0; j <= 9; ++j )
              {
                if ( *(_DWORD *)(CUser::GetParty(user) + 3244)
                  && (v10 = CUser::GetParty(user), CDungeon::get_index(*(CDungeon **)(v10 + 3244)) == 11006) )
                {
                  v11 = *(_DWORD *)(a3 + 4 * (i + 8) + 3);
                  v12 = CUser::getHackAnalyzer(user);
                  WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)v12, user, 0x81u, 1, v11, 0);
                }
                else
                {
                  v13 = *(_DWORD *)(a3 + 4 * (i + 8) + 3);
                  v14 = CUser::getHackAnalyzer(user);
                  WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)v14, user, 0x74u, 1, v13, 0);
                }
              }
            }
          }
        }
      }

      if ( CParty::IsRoutingState(Party) )
      {
        v8 = G_CGameManager();
        SpecialItemRoutingManager = (CSpecialItemRoutingManager *)CGameManager::GetSpecialItemRoutingManager(v8);
        if ( SpecialItemRoutingManager )
          CSpecialItemRoutingManager::ProcessFailEtc(SpecialItemRoutingManager, Party, 0);
      }

      *((_BYTE *)a3 + 57) = CParty::get_party_seatno((CUser **)Party, a2);
      CParty::move_map(Party, (int)a3);

  if ( (CUser *)CParty::getManager(Party) != a2 )
    return MAX_INT32;

  if ( (unsigned __int8)CParty::get_state(Party) != 1 )
    return MAX_INT32;

  if ( *((_BYTE *)a3 + 17) == 1 && (unsigned __int8)CParty::is_quick_party(Party) )
    return MAX_INT32;

  if ( (CUser *)CParty::getManager(v6) != a2 )
    return MAX_INT32;

  if ( (unsigned __int8)CParty::get_state(v6) == 1 )
    return 0;

  if ( Party && (CUser *)CParty::getManager(Party) != a3 )
  {
    v40[1] = MAX_INT32;
    return 0;
  }

      user = (CUser *)CParty::get_user(Party, i);
      if ( user )

        CParty::leave_user((std::string **)Party, (std::string *)user, 3);
        CUser::DisConnSig((pthread_mutex_t *)user, 24, 1, 0);

    *(_DWORD *)v49 = CParty::get_member_count((CUser **)Party);
    cMyTrace::cMyTrace(

      v54 = (CUser *)CParty::get_user(Party, j);
      if ( v54 )

        if ( (CurCharacVill != v55 || area != v56) && (unsigned __int8)CParty::is_quick_party(Party) != 1 )
        {
          a1 = v55;
          v10 = CurCharacVill;
          v11 = CUser::get_acc_id(v54);
          v12 = (const char *)NumberToString(v11, 0);
          LogManager::logFormat(
            (LogManager *)1,
            (int)"PacketDispatcher_Impl_1.cpp",
            "virtual int DisPatcher_SelectDungeon::process(CUser*, MSG_BASE&, ParamBase&)",
            (const char *)0x19DF,
            "[HACK_VILL_AREA] m_id : %s, Vill(%d,%d), Area(%d,%d)",
            v12,
            v10,
            a1,
            area,
            v56);
          v50 = 1;
          break;
        }

        v58 = (CUser *)CParty::get_user(Party, k);
        if ( v58 )

          if ( (unsigned __int8)CParty::checkValidUser((CUser **)Party, m) )
          {
            v20 = (CUserCharacInfo *)CParty::get_user(Party, m);
            v21 = (CUser *)CParty::get_user(Party, m);
            HackAnalyzer = CUser::getHackAnalyzer(v21);
            WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)HackAnalyzer, v20, 0x1F8u, 1, 0, 0);
          }

            if ( (unsigned __int8)CParty::checkValidUser((CUser **)Party, ii) )
            {
              v30 = (CUserCharacInfo *)CParty::get_user(Party, ii);
              v31 = CUserCharacInfo::get_charac_level(v30);
              if ( v31 < CDungeon::get_min_level(v46) )
              {
                v40[3] = 14;
                return 0;
              }
            }

            v71 = CParty::get_member_count((CUser **)Party);
            if ( (unsigned __int8)village_attacked::CRevengeDungeon::IsOpenRevengeDungeon(GlobalData::s_revengeDungeonMgr) != 1 )

              if ( (unsigned __int8)CParty::checkValidUser((CUser **)Party, jj) )
              {
                v33 = (CUserCharacInfo *)CParty::get_user(Party, jj);
                CurRevengeDungeonCount = CUserCharacInfo::GetCurRevengeDungeonCount(v33);
                if ( !CurRevengeDungeonCount || *((_BYTE *)v46 + 2171) < CurRevengeDungeonCount )
                {
                  v34 = (CUserCharacInfo *)CParty::get_user(Party, jj);
                  CUserCharacInfo::SetCurRevengeDungeonCount(v34, 0);
                  v40[3] = 173;
                  v40[5] = jj;
                  return 0;
                }
              }

          v74 = CParty::dungeon_start(
                  Party,

    PartyTelePort = CParty::GetPartyTelePort(Party);
    CPartyTelePort::set_teleport_data((CPartyTelePort *)PartyTelePort, v7, v6, v5, v12, v11, 0);

    MemberSlotNo = CParty::GetMemberSlotNo((const CUser **)Party, a2);
    v9 = CParty::GetPartyTelePort(Party);

    v10 = CParty::GetPartyTelePort(Party);
    CPartyTelePort::check_cur_teleport_state((CUser ***)v10);

  if ( (CUser *)CParty::getManager(Party) != a2 )
    return 23;

    if ( (unsigned __int8)CParty::checkValidUser((CUser **)Party, i) )
    {
      user = (CUser *)CParty::get_user(Party, i);
      if ( user )
      {
        v16 = *((unsigned __int8 *)v19 + 19);
        v17 = *(__int16 *)((char *)v19 + 17);
        v12 = *(__int16 *)((char *)v19 + 15);
        v13 = (char *)*((unsigned __int8 *)v19 + 14);
        v14 = *((unsigned __int8 *)v19 + 13);
        v15 = G_GameWorld();
        v26 = GameWorld::check_move_area(v15, user, v14, v13, v12, v17, v16, 1, 0);
        if ( v26 > 0 )
          return v26;
      }
    }

      PartyTelePort = CParty::GetPartyTelePort(Party);
      CPartyTelePort::send_teleport_effect_at_different_place((CParty **)PartyTelePort, 3);

      v4 = CParty::GetPartyTelePort(Party);
      CPartyTelePort::send_teleport_status((CPartyTelePort *)v4, 0);

      v5 = CParty::GetPartyTelePort(Party);
      CPartyTelePort::process_teleport((CUser ***)v5);

    MemberSlotNo = CParty::GetMemberSlotNo(Party, a2);
    if ( *((_BYTE *)a3 + 13) == 1 )

      PartyTelePort = CParty::GetPartyTelePort((CParty *)Party);
      CPartyTelePort::set_teleport_member_state((CParty **)PartyTelePort, MemberSlotNo, 1u);

      v6 = CParty::GetPartyTelePort((CParty *)Party);
      CPartyTelePort::check_cur_teleport_state((CUser ***)v6);

        v9 = CParty::GetPartyTelePort((CParty *)Party);
        CPartyTelePort::set_teleport_state((CPartyTelePort *)v9, 3);

      v7 = CParty::GetPartyTelePort((CParty *)Party);
      CPartyTelePort::set_teleport_member_state((CParty **)v7, MemberSlotNo, 0);

      v8 = CParty::GetPartyTelePort((CParty *)Party);
      CPartyTelePort::set_teleport_state((CPartyTelePort *)v8, 2);

    && ((PartyTelePort = CParty::GetPartyTelePort(Party),
         (char)CPartyTelePort::get_teleport_state((CPartyTelePort *)PartyTelePort) >= -1)

     || (v7 = CParty::GetPartyTelePort(Party), (char)CPartyTelePort::get_teleport_state((CPartyTelePort *)v7) <= 3)
      ? (v8 = 0)

    PartyTelePort = CParty::GetPartyTelePort(result);
    CPartyTelePort::send_teleport_status((CPartyTelePort *)PartyTelePort, 0);

    v5 = CParty::GetPartyTelePort(v7);
    CPartyTelePort::process_teleport((CUser ***)v5);

      v6 = CParty::GetPartyTelePort(v7);
      return CPartyTelePort::reset_teleport_data((CParty **)v6);

        if ( CParty::get_user(Party, i) )
        {
          user = (CUser *)CParty::get_user(Party, i);
          CurCharacTotalFatigue = CUser::getCurCharacTotalFatigue(user);
          v7 = (CUser *)CParty::get_user(Party, i);
          if ( CurCharacTotalFatigue == (unsigned __int16)CUser::getCurCharacTotalMaxFatigue(v7) )
            InterfacePacketBuf::put_byte(v8, 0);
          else
            InterfacePacketBuf::put_byte(v8, 1);
        }

      CParty::send_to_party((CUser **)Party, (PacketGuard *)v8);
      CParty::SetTournamentDungeonClearState(Party, (CParty *)2);

      CParty::InsertTournamentDungeonTimer((int)Party, 47);
    }
  }
  PacketGuard::~PacketGuard(v8);
}

//----- (081DFF32) --------------------------------------------------------
int __cdecl Dispatcher_TournamentRewardSelectState::check_error(
        Dispatcher_TournamentRewardSelectState *this,
        CUser *user)
{
  int PartyIndex; // ebx
  CGameManager *v4; // eax
  CParty *Party; // [esp+1Ch] [ebp-Ch]

  if ( !a2 )
    return -1;
  if ( CUser::get_state(a2) != 5 )
    return MAX_INT32;
  PartyIndex = (__int16)CUser::GetPartyIndex(a2);
  v4 = G_CGameManager();
  Party = (CParty *)CGameManager::GetParty(v4, PartyIndex);
  if ( !Party )
    return -1;
  if ( (unsigned __int8)CParty::GetTournamentDungeonClearState(Party) != 1 )
    return MAX_INT32;
  if ( (unsigned __int8)CParty::CheckRecvFromAllMember((const CUser **)Party, a2) != 1 )
    return MAX_INT32;
  return 0;
}

//----- (081DFFDA) --------------------------------------------------------
int __cdecl Dispatcher_TournamentRewardSelect::read(
        Dispatcher_TournamentRewardSelect *this,
        PacketBuf *a2,
        MSG_BASE *a3)
{
  if ( (unsigned __int8)PacketBuf::get_byte(a2, (char *)a3 + 13) != 1 )
    return LineFunc(21245, "virtual int Dispatcher_TournamentRewardSelect::read(PacketBuf&, MSG_BASE&)", 0, 0);
  if ( *((_BYTE *)a3 + 13) >= 2u )
    return LineFunc(21248, "virtual int Dispatcher_TournamentRewardSelect::read(PacketBuf&, MSG_BASE&)", 0, 0);
  if ( (unsigned __int8)PacketBuf::get_byte(a2, (char *)a3 + 14) != 1 )
    return LineFunc(21251, "virtual int Dispatcher_TournamentRewardSelect::read(PacketBuf&, MSG_BASE&)", 0, 0);
  if ( *((_BYTE *)a3 + 14) < 2u )
    return 0;
  return LineFunc(21254, "virtual int Dispatcher_TournamentRewardSelect::read(PacketBuf&, MSG_BASE&)", 0, 0);
}

//----- (081E00F0) --------------------------------------------------------
int __cdecl Dispatcher_TournamentRewardSelect::process(
        Dispatcher_TournamentRewardSelect *this,
        CUser *user,
        MSG_BASE *a3,
        ParamBase *a4)
{
  int id; // ebx
  int PartyIndex; // ebx
  CGameManager *v6; // eax
  unsigned __int8 v7; // si
  unsigned __int8 v8; // bl
  unsigned int MemberSlotNo; // eax
  int id[3]; // [esp+10h] [ebp-28h] BYREF
  ParamBase *v12; // [esp+1Ch] [ebp-1Ch]
  MSG_BASE *v13; // [esp+20h] [ebp-18h]
  CParty *Party; // [esp+24h] [ebp-14h]
  CTournamentDungeonReward *v15; // [esp+28h] [ebp-10h]
  char isCardSelectComplete; // [esp+2Eh] [ebp-Ah]

  v12 = a4;
  *((_DWORD *)a4 + 1) = Dispatcher_TournamentRewardSelect::check_error(this, a2);
  if ( *((int *)v12 + 1) > 0 )
    return 0;
  if ( *((int *)v12 + 1) < 0 )
    return LineFunc(
             21269,
             "virtual int Dispatcher_TournamentRewardSelect::process(CUser*, MSG_BASE&, ParamBase&)",
             *((_DWORD *)v12 + 1),
             0);
  v13 = a3;
  PartyIndex = (__int16)CUser::GetPartyIndex(a2);
  v6 = G_CGameManager();
  Party = (CParty *)CGameManager::GetParty(v6, PartyIndex, a3);
  if ( !Party )
    return LineFunc(
             21276,
             "virtual int Dispatcher_TournamentRewardSelect::process(CUser*, MSG_BASE&, ParamBase&)",
             0,
             0);
  std::vector<int>::vector(v11);
  CParty::GetValidPartyMember((CUser **)Party, v11);
  v15 = (CParty *)((char *)Party + 6076);
  isCardSelectComplete = CTournamentDungeonReward::isCardSelectComplete((int)Party + 6076, 2, v11);
  if ( isCardSelectComplete != 1 )
  {
    v7 = *((_BYTE *)v13 + 14);
    v8 = *((_BYTE *)v13 + 13);
    MemberSlotNo = CParty::GetMemberSlotNo((const CUser **)Party, a2);
    if ( !(unsigned __int8)CTournamentDungeonReward::SelectCard(v15, MemberSlotNo, v8, v7) )
      *((_DWORD *)v12 + 1) = MAX_INT32;
    v4 = 0;
  }
  else
  {
    *((_DWORD *)v12 + 1) = MAX_INT32;
    v4 = 0;
  }
  std::vector<int>::~vector((void **)v11);
  return v4;
}

//----- (081E02B0) --------------------------------------------------------
void __cdecl Dispatcher_TournamentRewardSelect::send(Dispatcher_TournamentRewardSelect *this, CUser *user, ParamBase *a3)
{
  int PartyIndex; // ebx
  CGameManager *v4; // eax
  int id[3]; // [esp+18h] [ebp-30h] BYREF
  PacketBuf *v6[3]; // [esp+24h] [ebp-24h] BYREF
  ParamBase *v7; // [esp+30h] [ebp-18h]
  CParty *Party; // [esp+34h] [ebp-14h]
  CTournamentDungeonReward *v9; // [esp+38h] [ebp-10h]
  bool isTournamentVictory; // [esp+3Fh] [ebp-9h]

  v7 = a3;
  PacketGuard::PacketGuard((PacketGuard *)v6);
  InterfacePacketBuf::clear(v6);
  InterfacePacketBuf::put_header(v6, 1, 459);
  if ( !*((_DWORD *)v7 + 1) )
  {
    InterfacePacketBuf::put_byte(v6, 1);
    PartyIndex = (__int16)CUser::GetPartyIndex(a2);
    v4 = G_CGameManager();
    Party = (CParty *)CGameManager::GetParty(v4, PartyIndex);
    if ( Party )
    {

      CParty::send_to_party((CUser **)Party, (PacketGuard *)v6);
      if ( (unsigned __int8)CParty::CheckRecvFromAllMember((const CUser **)Party, a2) == 1 )

        CParty::GetValidPartyMember((CUser **)Party, v5);
        if ( (unsigned __int8)CTournamentDungeonReward::isCardSelectComplete((int)v9, 2, v5) == 1 )

          isTournamentVictory = CParty::isTournamentVictory(Party);
          CParty::SetTournamentDungeonClearState(Party, (CParty *)3);

          CParty::TournamentDungeonClear(Party, isTournamentVictory);
        }
        std::vector<int>::~vector((void **)v5);
      }
    }
  }
  PacketGuard::~PacketGuard(v6);
}

//----- (081E046C) --------------------------------------------------------
int __cdecl Dispatcher_TournamentRewardSelect::check_error(Dispatcher_TournamentRewardSelect *this, CUser *user)
{
  int PartyIndex; // ebx
  CGameManager *v4; // eax
  CParty *Party; // [esp+1Ch] [ebp-Ch]

  if ( !a2 )
    return -1;
  if ( CUser::get_state(a2) != 5 )
    return MAX_INT32;
  PartyIndex = (__int16)CUser::GetPartyIndex(a2);
  v4 = G_CGameManager();
  Party = (CParty *)CGameManager::GetParty(v4, PartyIndex);
  if ( !Party )
    return -1;
  if ( (unsigned __int8)CParty::GetTournamentDungeonClearState(Party) == 2 )
    return 0;
  return MAX_INT32;
}

//----- (081E04F2) --------------------------------------------------------
int __cdecl Dispatcher_AvatarOptionChange::read(Dispatcher_AvatarOptionChange *this, PacketBuf *a2, MSG_BASE *a3)
{
  if ( (unsigned __int8)PacketBuf::get_short(a2, (__int16 *)((char *)a3 + 13)) != 1 )
    return LineFunc(21376, "virtual int Dispatcher_AvatarOptionChange::read(PacketBuf&, MSG_BASE&)", 0, 0);
  if ( *(_WORD *)((char *)a3 + 13) >= 0x138u )
    return LineFunc(21379, "virtual int Dispatcher_AvatarOptionChange::read(PacketBuf&, MSG_BASE&)", 0, 0);
  if ( (unsigned __int8)PacketBuf::get_int(a2, (int *)((char *)a3 + 15)) != 1 )
    return LineFunc(21382, "virtual int Dispatcher_AvatarOptionChange::read(PacketBuf&, MSG_BASE&)", 0, 0);
  if ( *(int *)((char *)a3 + 15) < 0 )
    return LineFunc(21385, "virtual int Dispatcher_AvatarOptionChange::read(PacketBuf&, MSG_BASE&)", 0, 0);
  if ( (unsigned __int8)PacketBuf::get_short(a2, (__int16 *)((char *)a3 + 19)) != 1 )
    return LineFunc(21388, "virtual int Dispatcher_AvatarOptionChange::read(PacketBuf&, MSG_BASE&)", 0, 0);
  if ( *(_WORD *)((char *)a3 + 19) > 0x68u )
    return LineFunc(21392, "virtual int Dispatcher_AvatarOptionChange::read(PacketBuf&, MSG_BASE&)", 0, 0);
  if ( (unsigned __int8)PacketBuf::get_int(a2, (int *)((char *)a3 + 21)) != 1 )
    return LineFunc(21395, "virtual int Dispatcher_AvatarOptionChange::read(PacketBuf&, MSG_BASE&)", 0, 0);
  if ( *(int *)((char *)a3 + 21) < 0 )
    return LineFunc(21398, "virtual int Dispatcher_AvatarOptionChange::read(PacketBuf&, MSG_BASE&)", 0, 0);
  if ( (unsigned __int8)PacketBuf::get_byte(a2, (char *)a3 + 25) != 1 )
    return LineFunc(21401, "virtual int Dispatcher_AvatarOptionChange::read(PacketBuf&, MSG_BASE&)", 0, 0);
  if ( *((char *)a3 + 25) >= 0 )
    return 0;
  return LineFunc(21404, "virtual int Dispatcher_AvatarOptionChange::read(PacketBuf&, MSG_BASE&)", 0, 0);
}

//----- (081E0774) --------------------------------------------------------
int __cdecl Dispatcher_AvatarOptionChange::process(
        Dispatcher_AvatarOptionChange *this,
        CUser **a2,
        MSG_BASE *a3,
        ParamBase *a4)
{
  int id; // ebx
  const CInventory *CurCharacInvenRefR; // eax
  _BYTE v7[49471]; // [esp+21h] [ebp-C917h] BYREF
  _BYTE v8[1976]; // [esp+C160h] [ebp-7D8h] BYREF
  ParamBase *v9; // [esp+C918h] [ebp-20h]
  MSG_BASE *v10; // [esp+C91Ch] [ebp-1Ch]
  CInventory *CurCharacInvenW; // [esp+C920h] [ebp-18h]
  char flag; // [esp+C927h] [ebp-11h]
  char *InvenRef; // [esp+C92Ch] [ebp-Ch]

  v9 = a4;
  *((_DWORD *)a4 + 1) = Dispatcher_AvatarOptionChange::check_error(this, (CUser *)a2, a3);
  if ( *((int *)v9 + 1) > 0 )
    return 0;
  if ( *((int *)v9 + 1) < 0 )
    return LineFunc(
             21419,
             "virtual int Dispatcher_AvatarOptionChange::process(CUser*, MSG_BASE&, ParamBase&)",
             *((_DWORD *)v9 + 1),
             0);
  v10 = a3;
  CurCharacInvenW = (CInventory *)CUserCharacInfo::getCurCharacInvenW((CUserCharacInfo *)a2);
  CInventory::CInventory((CInventory *)v8);
  InventoryMemory::InventoryMemory((InventoryMemory *)v7);
  CInventory::SetInventoryMemory((CInventory *)v8, (InventoryMemory *)v7);
  CurCharacInvenRefR = (const CInventory *)CUserCharacInfo::getCurCharacInvenRefR((CUserCharacInfo *)a2);
  CInventory::setCopy((CInventory *)v8, CurCharacInvenRefR);
  v12 = CInventory::delete_item((CInventory *)v8, 1, *(__int16 *)((char *)v10 + 13), 1, 47, 0);
  if ( v12 != 1 )
  {
    *((_DWORD *)v9 + 1) = 22;
    v4 = 0;
  }
  else
  {
    if ( CInventory::GetInvenRef((CInventory *)v8, 2, *(__int16 *)((char *)v10 + 19)) )
    {
      CInventory::delete_item(CurCharacInvenW, 1, *(__int16 *)((char *)v10 + 13), 1, 47, 1);
      InvenRef = CInventory::GetInvenRef(CurCharacInvenW, 2, *(__int16 *)((char *)v10 + 19));
      cUserHistoryLog::UseAvatarOptionChange(
        a2 + CUSER_OFFSET_HISTORY_LOG_ALT,
        *(_DWORD *)(InvenRef + 7),
        *(_DWORD *)(InvenRef + 2),
        *(unsigned __int16 *)(InvenRef + 11),
        *((char *)v10 + 25));
      *(_WORD *)(InvenRef + 11) = *((char *)v10 + 25);
      CInventory::SendAvatarEvent((CUser **)CurCharacInvenW, 707, *(_DWORD *)(InvenRef + 7), *((char *)v10 + 25), 0, 0);
      *((_WORD *)v9 + 4) = *(_WORD *)((char *)v10 + 13);
      *((_WORD *)v9 + 5) = *(_WORD *)((char *)v10 + 19);
      *((_BYTE *)v9 + 12) = *((_BYTE *)v10 + 25);
    }
    else
    {
      *((_DWORD *)v9 + 1) = 17;
    }
    v4 = 0;
  }
  CInventory::~CInventory((CInventory *)v8);
  return v4;
}

//----- (081E0A40) --------------------------------------------------------
int __cdecl Dispatcher_AvatarOptionChange::check_error(Dispatcher_AvatarOptionChange *this, CUser *user, MSG_BASE *a3)
{
  int id; // ebx
  int *CurCharacInvenR; // eax
  unsigned int id; // ebx
  CItemList **v7; // eax
  int id; // ebx
  int *v9; // eax
  unsigned int id; // ebx
  CItemList **v11; // eax
  int id; // eax
  int grade; // eax
  _BYTE v14[2]; // [esp+12h] [ebp-B6h] BYREF
  int id; // [esp+14h] [ebp-B4h]
  _BYTE v16[2]; // [esp+4Fh] [ebp-79h] BYREF
  int id; // [esp+51h] [ebp-77h]
  int id; // [esp+56h] [ebp-72h]
  int id; // [esp+8Ch] [ebp-3Ch] BYREF
  int id; // [esp+90h] [ebp-38h] BYREF
  int id; // [esp+94h] [ebp-34h] BYREF
  int id; // [esp+98h] [ebp-30h] BYREF
  int id; // [esp+9Ch] [ebp-2Ch] BYREF
  int id; // [esp+A0h] [ebp-28h] BYREF
  int id; // [esp+A4h] [ebp-24h]
  MSG_BASE *v26; // [esp+A8h] [ebp-20h]
  CItem *item; // [esp+ACh] [ebp-1Ch]
  CStackableItem *v28; // [esp+B0h] [ebp-18h]
  CItem *v29; // [esp+B4h] [ebp-14h]
  CItem *v30; // [esp+B8h] [ebp-10h]
  char *v31; // [esp+BCh] [ebp-Ch]

  if ( !a2 )
    return -1;
  if ( CUser::get_state(a2) <= 2 )
    return -1;
  v25 = CSecu_ProtectionField::Check(GlobalData::s_pSecuProtectionField, a2, 42);
  if ( v25 )
    return v25;
  v26 = a3;
  v4 = *(__int16 *)((char *)a3 + 13);
  CurCharacInvenR = (int *)CUserCharacInfo::getCurCharacInvenR(a2);
  CInventory::GetInvenSlot((CInventory *)v16, CurCharacInvenR, 1, v4);
  if ( v17 != *(_DWORD *)((char *)v26 + 15) )
    return 17;
  if ( v18 <= 0 )
    return 22;
  v6 = v17;
  v7 = (CItemList **)G_CDataManager();
  item = (CItem *)CDataManager::find_item(v7, v6);
  if ( !item )
    return 17;
  if ( (unsigned __int8)CItem::is_stackable(item) != 1 )
    return 17;
  v28 = item;
  if ( (unsigned __int8)CStackableItem::IsAvatarOptionChangeItem(item) != 1 )
    return 17;
  v8 = *(__int16 *)((char *)v26 + 19);
  v9 = (int *)CUserCharacInfo::getCurCharacInvenR(a2);
  CInventory::GetInvenSlot((CInventory *)v14, v9, 2, v8);
  if ( v15 != *(_DWORD *)((char *)v26 + 21) )
    return 17;
  v10 = *(_DWORD *)((char *)v26 + 21);
  v11 = (CItemList **)G_CDataManager();
  v29 = (CItem *)CDataManager::find_item(v11, v10);
  if ( !v29 )
    return 17;
  if ( (unsigned __int8)CItem::is_stackable(v29) )
    return 17;
  if ( (*(unsigned __int8 (__cdecl **)(CItem *))(*(_DWORD *)v29 + 16))(v29) != 1 )
    return 17;
  if ( CUser::CheckItemLock(a2, 2, *(__int16 *)((char *)v26 + 19)) )
    return 213;
  v30 = v29;
  if ( !std::set<int>::empty((int)v28 + 872) )
  {
    v21 = v15;
    std::set<int>::find(&v20, (int)v28 + 872, &v21);
    std::set<int>::end(&v22, (int)v28 + 872);
    if ( std::_Rb_tree_const_iterator<int>::operator==(&v20, &v22) )
      return 255;
  }
  else
  {
    v31 = (char *)v28 + 869;
    if ( (unsigned __int8)CItem::check_job_type(v30, *((_BYTE *)v28 + 869)) != 1 && *v31 != 11 )
      return 254;
    v12 = (*(int (__cdecl **)(CItem *))(*(_DWORD *)v30 + 12))(v30);
    if ( v12 != v31[1] && v31[1] != 26 )
      return 253;
    grade = CItem::get_grade(v30);
    if ( grade != v31[2] && v31[2] )
      return 252;
  }
  if ( !std::set<int>::empty((int)v28 + 896) )
  {
    v23 = *((char *)v26 + 25);
    std::set<int>::find(&v19, (int)v28 + 896, &v23);
    std::set<int>::end(&v24, (int)v28 + 896);
    if ( std::_Rb_tree_const_iterator<int>::operator!=(&v19, &v24) )
      return 251;
  }
  if ( !CEquipItem::IsAvatarOptionValid(v30, *((_BYTE *)v26 + 25)) )
    return 250;
  return 0;
}

//----- (081E0E32) --------------------------------------------------------
void __cdecl Dispatcher_AvatarOptionChange::send(Dispatcher_AvatarOptionChange *this, CUser *user, ParamBase *a3)
{
  PacketBuf *v3[3]; // [esp+10h] [ebp-18h] BYREF
  ParamBase *v4; // [esp+1Ch] [ebp-Ch]

  v4 = a3;
  if ( *((_DWORD *)a3 + 1) )
  {
    CUser::SendCmdErrorPacket(a2, 460, *((_DWORD *)v4 + 1));
  }
  else
  {
    PacketGuard::PacketGuard((PacketGuard *)v3);
    InterfacePacketBuf::clear(v3);
    InterfacePacketBuf::put_header(v3, 1, 460);
    InterfacePacketBuf::put_byte(v3, 1);
    InterfacePacketBuf::put_short(v3, *((_WORD *)v4 + 5));
    InterfacePacketBuf::put_byte(v3, *((_BYTE *)v4 + 12));
    InterfacePacketBuf::finalize(v3, 1);
    CUser::Send(a2, (PacketGuard *)v3);
    CUser::SendUpdateItemList(a2, 1, 0, *((__int16 *)v4 + 4));
    PacketGuard::~PacketGuard(v3);
  }
}

//----- (081E0F60) --------------------------------------------------------
int __cdecl Dispatcher_LimitNpcBuyItem::read(Dispatcher_LimitNpcBuyItem *this, PacketBuf *a2, MSG_BASE *a3)
{
  if ( (unsigned __int8)PacketBuf::get_int(a2, (unsigned int *)((char *)a3 + 13)) != 1 )
    return LineFunc(21914, "virtual int Dispatcher_LimitNpcBuyItem::read(PacketBuf&, MSG_BASE&)", 0, 0);
  if ( (unsigned __int8)PacketBuf::get_int(a2, (unsigned int *)((char *)a3 + 17)) != 1 )
    return LineFunc(21917, "virtual int Dispatcher_LimitNpcBuyItem::read(PacketBuf&, MSG_BASE&)", 0, 0);
  return 0;
}

//----- (081E0FF8) --------------------------------------------------------
int __cdecl Dispatcher_LimitNpcBuyItem::process(
        Dispatcher_LimitNpcBuyItem *this,
        CUser *user,
        MSG_BASE *a3,
        ParamBase *a4)
{
  int ServerGroup; // eax
  std::string *ServerProxy; // eax
  char buf[10]; // [esp+12h] [ebp-36h] BYREF
  int UID; // [esp+1Ch] [ebp-2Ch]
  int acc_id; // [esp+20h] [ebp-28h]
  int charac_no; // [esp+24h] [ebp-24h]
  int id; // [esp+28h] [ebp-20h]
  int id; // [esp+2Ch] [ebp-1Ch]
  ParamBase *v13; // [esp+38h] [ebp-10h]
  MSG_BASE *v14; // [esp+3Ch] [ebp-Ch]

  v13 = a4;
  *((_DWORD *)a4 + 1) = Dispatcher_LimitNpcBuyItem::check_error(this, a2, a3);
  if ( *((int *)v13 + 1) > 0 )
    return 0;
  if ( *((int *)v13 + 1) < 0 )
    return LineFunc(
             21934,
             "virtual int Dispatcher_LimitNpcBuyItem::process(CUser*, MSG_BASE&, ParamBase&)",
             *((_DWORD *)v13 + 1),
             0);
  v14 = a3;
  LimitNpcBuyItemInfo::LimitNpcBuyItemInfo((LimitNpcBuyItemInfo *)buf);
  UID = CUser::GetUID(a2);
  acc_id = CUser::get_acc_id(a2);
  charac_no = CUser::get_charac_no(a2, -1);
  v11 = *(_DWORD *)((char *)v14 + 13);
  v12 = 1;
  ServerGroup = CUser::GetServerGroup(a2);
  ServerProxy = (std::string *)CServerProxyMgr<CMonitorServerProxy>::GetServerProxy(
                                 GlobalData::s_monitor_proxy_mgr,
                                 ServerGroup);
  CMonitorServerProxy::SendPacket(ServerProxy, buf, 0x26u);
  return 0;
}

//----- (081E1100) --------------------------------------------------------
void __cdecl Dispatcher_LimitNpcBuyItem::send(Dispatcher_LimitNpcBuyItem *this, CUser *user, ParamBase *a3)
{
  PacketPool *v3[3]; // [esp+10h] [ebp-18h] BYREF
  ParamBase *v4; // [esp+1Ch] [ebp-Ch]

  v4 = a3;
  PacketGuard::PacketGuard((PacketGuard *)v3);
  if ( *((int *)v4 + 1) > 0 )
    CUser::SendCmdErrorPacket(a2, 465, *((_DWORD *)v4 + 1));
  PacketGuard::~PacketGuard(v3);
}

//----- (081E1172) --------------------------------------------------------
int __cdecl Dispatcher_LimitNpcBuyItem::check_error(Dispatcher_LimitNpcBuyItem *this, CUser *user, MSG_BASE *a3)
{
  unsigned int id; // esi
  unsigned int id; // ebx
  CItemShop **v7; // eax
  unsigned int acc_id; // eax
  char flag; // bl
  private_store::CPrivateStoreMgr *InstancePrivateStoreMgr; // eax
  unsigned int id; // ebx
  CItemList **v12; // eax
  NeedMaterialDataManager *Instance; // eax
  int *NeedMaterial; // eax
  int id; // edx
  char *v16; // ebx
  CInventory *CurCharacInvenRefR; // eax
  int id; // ebx
  CDataManager *v20; // eax
  int id; // eax
  long double v22; // [esp+56h] [ebp-A2h] BYREF
  _BYTE v23[61]; // [esp+93h] [ebp-65h] BYREF
  int id; // [esp+D0h] [ebp-28h]
  int id; // [esp+D4h] [ebp-24h]
  int id[4]; // [esp+D8h] [ebp-20h] BYREF
  MSG_BASE *v27; // [esp+E8h] [ebp-10h]
  CItem *item; // [esp+ECh] [ebp-Ch]

  if ( !a2 )
    return -1;
  if ( CUser::get_state(a2) <= 2 || !CUserCharacInfo::getCurCharacR(a2) )
    return -1;
  v27 = a3;
  v5 = *(_DWORD *)((char *)a3 + 13);
  v6 = *(_DWORD *)((char *)a3 + 17);
  v7 = (CItemShop **)G_CDataManager();
  if ( (unsigned __int8)CDataManager::checkLimitNpcBuyItemList(v7, v6, v5) != 1 )
  {
    CUser::get_charac_no(a2, -1);
    acc_id = CUser::get_acc_id(a2);
    v9 = NumberToString(acc_id, 0);
    cMyTrace::cMyTrace((cMyTrace *)v26, "int Dispatcher_LimitNpcBuyItem::check_error(CUser*, MSG_BASE&)", 21985, 5);
    cMyTrace::operator()((int)v26, "May be HackUser or Must be HackUser : m_id=%s, charac_no=%u", v9);
    return -1;
  }
  else if ( (unsigned __int8)CUser::CheckInTrade(a2) )
  {
    return 19;
  }
  else
  {
    InstancePrivateStoreMgr = (private_store::CPrivateStoreMgr *)private_store::GetInstancePrivateStoreMgr();
    if ( private_store::CPrivateStoreMgr::IsBusyPrivateStore(InstancePrivateStoreMgr, a2) )
    {
      return 60;
    }
    else
    {
      v11 = *(_DWORD *)((char *)v27 + 13);
      v12 = (CItemList **)G_CDataManager();
      item = (CItem *)CDataManager::find_item(v12, v11);
      if ( item )
      {
        Inven_Item::Inven_Item((Inven_Item *)v23);
        Instance = (NeedMaterialDataManager *)NeedMaterialDataManager::getInstance();
        NeedMaterial = (int *)NeedMaterialDataManager::getNeedMaterial(Instance, item);
        v15 = NeedMaterial[1];
        v24 = *NeedMaterial;
        v25 = v15;
        if ( v24 && v24 != -1 )
        {

    CParty::setMonsterMoveHack(Party);
  }
  if ( (unsigned __int8)PacketBuf::get_short(a4, &v92) != 1 )
    return LineFunc(27966, "virtual int DisPatcher_DieMob::dispatch_sig(CUser*, PacketBuf&)", 0, 0);
  if ( (unsigned __int8)PacketBuf::get_byte(a4, &v77) != 1 )
    return LineFunc(27969, "virtual int DisPatcher_DieMob::dispatch_sig(CUser*, PacketBuf&)", 0, 0);
  if ( (unsigned __int8)PacketBuf::get_int(a4, &v80) != 1 )
    return LineFunc(27970, "virtual int DisPatcher_DieMob::dispatch_sig(CUser*, PacketBuf&)", 0, 0);
  if ( (unsigned __int8)PacketBuf::get_int(a4, &v81) != 1 )
    return LineFunc(27971, "virtual int DisPatcher_DieMob::dispatch_sig(CUser*, PacketBuf&)", 0, 0);
  if ( (unsigned __int8)PacketBuf::get_short(a4, &v78) != 1 )
    return LineFunc(27972, "virtual int DisPatcher_DieMob::dispatch_sig(CUser*, PacketBuf&)", 0, 0);
  if ( (unsigned __int8)PacketBuf::get_short(a4, &v79) != 1 )
    return LineFunc(27973, "virtual int DisPatcher_DieMob::dispatch_sig(CUser*, PacketBuf&)", 0, 0);
  if ( (unsigned __int8)PacketBuf::get_short(a4, &v82) != 1 )
    return LineFunc(27974, "virtual int DisPatcher_DieMob::dispatch_sig(CUser*, PacketBuf&)", 0, 0);
  if ( (unsigned __int8)PacketBuf::get_short(a4, &v83) != 1 )
    return LineFunc(27975, "virtual int DisPatcher_DieMob::dispatch_sig(CUser*, PacketBuf&)", 0, 0);
  if ( (unsigned __int8)PacketBuf::get_short(a4, &v84) != 1 )
    return LineFunc(27976, "virtual int DisPatcher_DieMob::dispatch_sig(CUser*, PacketBuf&)", 0, 0);
  if ( (unsigned __int8)PacketBuf::get_short(a4, &v85) != 1 )
    return LineFunc(27977, "virtual int DisPatcher_DieMob::dispatch_sig(CUser*, PacketBuf&)", 0, 0);
  if ( (unsigned __int8)PacketBuf::get_short(a4, &v86) != 1 )
    return LineFunc(27978, "virtual int DisPatcher_DieMob::dispatch_sig(CUser*, PacketBuf&)", 0, 0);
  if ( CUser::get_state(a3) == 5 )
  {

      CParty::DestroyPassiveObject(a1, v105, v66);
      return 0;

    if ( (unsigned __int8)CParty::kill_monster(v105, a3, v66, v67, (int)v65, (_IO_FILE **)&v95) != 1 )
    {
      PacketGuard::PacketGuard((PacketGuard *)v90);
      InterfacePacketBuf::put_header(v90, 1, 42);
      InterfacePacketBuf::put_byte(v90, 0);
      InterfacePacketBuf::put_byte(v90, 5);
      InterfacePacketBuf::put_short(v90, v66);
      InterfacePacketBuf::finalize(v90, 1);
      CUser::Send(a3, (PacketGuard *)v90);
      PacketGuard::~PacketGuard(v90);
    }

                    && (CParty::get_member_count((CUser **)Party) > 3 || !CParty::CanDoPartyActionAssalutState(Party)) )
                  {
                    goto LABEL_147;
                  }

                       && (!v66 || !(unsigned __int8)CParty::CheckMemberLastTryAssaultTime(v66) ? (v29 = 0) : (v29 = 1),
                           !v29))

                        || !(unsigned __int8)CParty::CheckMemberLastTryAssaultTime(Party)
                        || !(unsigned __int8)expert_job::CAlchemist::GetAssaultPrevent(v61, 0)

                  || !(unsigned __int8)CParty::CheckMemberLastTryAssaultTime(Party)
                  || !(unsigned __int8)expert_job::CAlchemist::GetAssaultPrevent(v74, 0)

                   || !(unsigned __int8)CParty::CheckMemberLastTryAssaultTime(v80)
                   || !(unsigned __int8)expert_job::CAlchemist::GetAssaultPrevent(a4, 0)

            CParty::create_party(v76, v74);
            CParty::setDungIndex(v76, 0);

            CParty::setDungDiffi(v76, 5u);
            CParty::join_user(v76, a4);

            CParty::send_party_realtime_info((CUser **)v76);
            InterfacePacketBuf::clear(v68);

            CParty::send_party_ipinfo((CUser **)v76);
            v22 = G_GameWorld();

            if ( !CParty::CanDoPartyActionAssalutState(v76) )
              goto LABEL_123;

            v81 = CParty::join_user(v76, v74);
            if ( v81 <= 0 )

            CParty::send_quick_party_matching_result((CUser **)v76, 0, 0);
            CParty::set_quick_party(v76, 0);

            CParty::send_party_realtime_info((CUser **)v76);
            InterfacePacketBuf::clear(v68);

            CParty::send_party_ipinfo((CUser **)v76);
            v25 = G_GameWorld();

              if ( !CParty::CanDoPartyActionAssalutState(v76) )
              {
LABEL_123:
                InterfacePacketBuf::clear(v68);
                InterfacePacketBuf::put_header(v68, 1, 11);
                InterfacePacketBuf::put_byte(v68, 0);
                InterfacePacketBuf::put_byte(v68, 19);
                InterfacePacketBuf::put_byte(v68, 0);
                InterfacePacketBuf::finalize(v68, 1);
                CUser::Send(a4, (PacketGuard *)v68);
                a1 = 0;
                v8 = 0;
                goto LABEL_132;
              }

              v82 = CParty::join_user(v76, a4);
              if ( v82 <= 0 )

              CParty::send_quick_party_matching_result((CUser **)v76, 0, 0);
              CParty::set_quick_party(v76, 0);

              CParty::send_party_realtime_info((CUser **)v76);
              InterfacePacketBuf::clear(v68);

              CParty::send_party_ipinfo((CUser **)v76);
              v30 = G_GameWorld();

      if ( CParty::checkFreeRevivalCondition(Party, v8) )
        return 1;

  CParty::send_to_party(a2, (PacketGuard *)v4);
  PacketGuard::~PacketGuard(v4);

  CParty::send_to_party(a2, (PacketGuard *)v3);
  PacketGuard::~PacketGuard(v3);

      if ( (CUser *)CParty::getManager(Party) == a3 )
      {
        if ( (unsigned __int8)CParty::GetEPLPState(Party) == 2 || !Dispatcher_EPLPCommand::excludeEplpState(this, a3) )
        {
          Dispatcher_EPLPCommand::_BroadCastPacket((int)this, (CUser **)Party, (int)v9);
          if ( v10 == 1 )
          {
            SecretShopData = (secretshop::SECRET_SHOP_DATA *)CUser::GetSecretShopData(a3);
            v6 = SecretShopData && secretshop::SECRET_SHOP_DATA::IsOpen(SecretShopData);
            if ( v6 && (unsigned __int8)secretshop::SECRET_SHOP_DATA::IsCompleteBuy(SecretShopData) != 1 )
            {
              Dispatcher_EPLPCommand::_SendEPLPError(this, (CUser **)Party, v11, 133);
              return 0;
            }
            else
            {
              v14 = 3;
              if ( v11 == 2 )
              {
                v14 = 1;
              }
              else if ( v11 > 2u )
              {
                if ( v11 == 3 )
                {
                  CParty::SetSelectedEPLPCmd(Party, 3);
                  return 0;
                }
                Dispatcher_EPLPCommand::_SendEPLPError(this, (CUser **)Party, v11, 1);
              }
              if ( CUser::get_state(a3) == 12 )
              {
                BossTower = (WongWork::CBossTower *)CUser::getBossTower(a3);
                if ( BossTower )
                  WongWork::CBossTower::handleEPLPCommand(a1, BossTower, v11);
                return 0;
              }
              else if ( CUser::get_state(a3) == 10 )
              {
                DeathTower = (WongWork::CDeathTower *)CUser::getDeathTower(a3);
                if ( DeathTower )
                {
                  WongWork::CDeathTower::handleSelectEPLPCommand((const CUser ***)DeathTower, a3, v14);
                  return 0;
                }
                else
                {
                  return LineFunc(37561, "virtual int Dispatcher_EPLPCommand::dispatch_sig(CUser*, PacketBuf&)", 0, 0);
                }
              }
              else
              {
                if ( Dispatcher_EPLPCommand::isReTurnToVillage(this, a3) )
                  CParty::ReturnToVillage(a1, Party);
                else
                  CParty::SelectEPLPCommand(Party, v14);
                return 0;
              }
            }
          }
          else
          {
            return 0;
          }
        }
        else
        {
          return 0;
        }
      }

  if ( (unsigned __int8)CParty::CheckRecvFromAllMember((const CUser **)Party, a2) != 1 )
    return 0;

  if ( (char)CParty::get_dungeon_clear_state(Party) <= 2 )
  {
    PacketGuard::PacketGuard((PacketGuard *)v7);
    InterfacePacketBuf::clear(v7);
    InterfacePacketBuf::put_header(v7, 1, 72);
    InterfacePacketBuf::put_byte(v7, 1);
    InterfacePacketBuf::finalize(v7, 1);
    CParty::send_to_party((CUser **)Party, (PacketGuard *)v7);
    CParty::set_dungeon_clear_state(Party, (CParty *)3);
    CParty::insert_time_dungeon_score_scroll_msg(Party, 30);
    v2 = 0;
    PacketGuard::~PacketGuard(v7);
  }

  if ( (unsigned __int8)CParty::CheckRecvFromAllMember((const CUser **)Party, a2) != 1 )
    return 0;

  if ( (char)CParty::get_dungeon_clear_state(Party) <= 3 )
  {
    PacketGuard::PacketGuard((PacketGuard *)v11);
    InterfacePacketBuf::put_header(v11, 1, 73);
    InterfacePacketBuf::put_byte(v11, 1);
    for ( i = 0; i <= 3; ++i )
    {
      if ( CParty::get_user(Party, i) )
      {
        user = (CUser *)CParty::get_user(Party, i);
        CurCharacTotalFatigue = CUser::getCurCharacTotalFatigue(user);
        v9 = (CUser *)CParty::get_user(Party, i);
        if ( CurCharacTotalFatigue == (unsigned __int16)CUser::getCurCharacTotalMaxFatigue(v9) )
          InterfacePacketBuf::put_short(v11, 0);
        else
          InterfacePacketBuf::put_short(v11, 1);
      }
      else
      {
        InterfacePacketBuf::put_short(v11, -1);
      }
    }
    InterfacePacketBuf::finalize(v11, 1);
    CParty::send_to_party((CUser **)Party, (PacketGuard *)v11);
    CParty::set_dungeon_clear_state(Party, (CParty *)4);
    CParty::insert_time_card_select_right_msg(Party, 30);
    CParty::insert_time_card_select_msg(Party, 4);
    v2 = 0;
    PacketGuard::~PacketGuard(v11);
  }

  if ( (char)CParty::get_dungeon_clear_state(Party) > 4 )
    return 0;

  if ( (unsigned __int8)CParty::IsCompleteCardSelectOfMembers((int)Party, 2) != 1 )
  {
    if ( (unsigned __int8)CParty::SetCardNumber((const CUser **)Party, a3, v9, v8) != 1 )
      return 0;
    CParty::SendEachCardInfo(Party);
  }

  if ( (unsigned __int8)CParty::CheckRecvFromAllMember((const CUser **)Party, a3) != 1 )
    return 0;

  if ( (unsigned __int8)CParty::IsCompleteCardSelectOfMembers((int)Party, 2) == 1 )
  {
    CParty::set_dungeon_clear_state(Party, (CParty *)5);
    CParty::dungeon_clear(a1, (CUser **)Party);
  }

          CParty::send_invite_warroom((CUser **)Party, unique_id);
          v6 = G_CGameManager();

          CParty::send_invite_warroom((CUser **)Party, unique_id);
          v7 = G_CGameManager();

    PartyTelePort = CParty::GetPartyTelePort(Party);
    if ( (CPartyTelePort::get_teleport_state((CPartyTelePort *)PartyTelePort) & FLAG_BIT7) == 0

      && (v5 = CParty::GetPartyTelePort(Party), (char)CPartyTelePort::get_teleport_state((CPartyTelePort *)v5) <= 3) )
    {
      CUser::SendCmdErrorPacket(a2, 124, 158);
      return 0;
    }

      v8 = CParty::change_manager(Party, a2, v7);
      if ( v8 > 0 )

      if ( (unsigned __int8)CParty::GetEPLPState(Party) )
      {
        return 0;
      }

      else if ( (CUser *)CParty::getManager(Party) == a3 )
      {
        if ( CParty::get_quick_party_index(Party) != -1 )
        {
          v6 = G_CGameManager();
          CGameManager::CheckOutQuickParty(v6, Party, 1);
        }
        CParty::ReturnToVillage(a1, Party);
        return 0;
      }

        v9 = CParty::leave_user((std::string **)Party, (std::string *)a3, 2);
        if ( v9 != 1 )

        CParty::giveup_game(a1, Party, a3, 1, 0, 0);
    }
    return 0;
  }
  for ( i = 0; (int)i <= 77; ++i )
    CUserCharacInfo::setCurCharacTutorialFlag(a3, i);
  return 0;
}

//----- (08206B5C) --------------------------------------------------------
int __cdecl Dispatcher_CompleteLoadAfterAssault::dispatch_sig(Dispatcher_CompleteLoadAfterAssault *this, CUser *user)
{
  int CurCharacNo; // edi
  int LoginTick; // esi
  int UID; // ebx
  TimerQueue *v6; // eax
  int id; // [esp+2Ch] [ebp-2Ch]
  PacketBuf *v8[3]; // [esp+30h] [ebp-28h] BYREF
  CParty *Party; // [esp+3Ch] [ebp-1Ch]

  if ( CUser::get_state(a2) <= 1 )
    return LineFunc(41297, "virtual int Dispatcher_CompleteLoadAfterAssault::dispatch_sig(CUser*, PacketBuf&)", 0, 0);
  if ( (unsigned __int8)CUserCharacInfo::GetCurCharacSuperState(a2) )
  {
    CurCharacNo = CUserCharacInfo::getCurCharacNo(a2);
    LoginTick = CUserCharacInfo::GetLoginTick(a2);
    v7 = *((_DWORD *)G_CDataManager() + 10658);
    UID = CUser::GetUID(a2);
    v6 = G_TimerQueue();
    TimerQueue::InsertTimer(LoginTick, v6, 0, UID, 127, v7, LoginTick, CurCharacNo);
  }
  Party = (CParty *)CUser::GetParty(a2);
  if ( Party )
  {
    if ( (unsigned __int8)CParty::CheckRecvFromAllMember((const CUser **)Party, a2) )
    {

      CParty::send_to_party((CUser **)Party, (PacketGuard *)v8);
      CParty::SetRecvResultFlag(Party, 1);

        if ( (unsigned __int8)CParty::checkValidUser((CUser **)Party, i) )
        {
          if ( !v15[i] && (unsigned __int8)CParty::IsSinglePlay(Party) )
          {
            user = (CUserCharacInfo *)CParty::get_user(Party, i);
            v7 = (CUser *)CParty::get_user(Party, i);
            HackAnalyzer = CUser::getHackAnalyzer(v7);
            WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)HackAnalyzer, user, 0xD5u, 1, 0, 0);
          }
          v9 = (CUser *)CParty::get_user(Party, i);
          v10 = CUser::getDeathTower(v9);
          WongWork::CDeathTower::stMapMonsterKillChecker_t::init((WongWork::CDeathTower::stMapMonsterKillChecker_t *)(v10 + 2904));
          v11 = WongWork::CDeathTower::getCStage(DeathTower);
          v12 = WongWork::CDeathTower::CStage::getStageClearTime((WongWork::CDeathTower::CStage *)v11) / 0x3E8u;
          v13 = v15[i];
          v14 = CParty::get_user(Party, i);
          cUserHistoryLog::MapClear((CUser **)(v14 + CUSER_OFFSET_HISTORY_LOG), v13, v12, 0);
        }

          if ( Party && CParty::get_member_count((CUser **)Party) > 1 )
          {
            CParty::send_quick_party_matching_result((CUser **)Party, 1, 0);
            CParty::set_quick_party(Party, 1);
          }

          if ( v810 && CParty::get_member_count((CUser **)v810) > 1 )
          {
            CParty::send_quick_party_matching_result((CUser **)v810, 0, 0);
            CParty::set_quick_party(v810, 0);
          }

          if ( v812 && CParty::get_member_count((CUser **)v812) > 1 )
          {
            CParty::set_gm_random_buff_type((int)v812, v811);
            memset(s, 0, 255);
            sprintf(s, "GM QUICK PARTY RANDOM BUFF (%d) TYPE!!", v811);
            PacketGuard::PacketGuard((PacketGuard *)v681);
            InterfacePacketBuf::put_header(v681, 0, 12);
            InterfacePacketBuf::put_byte(v681, 0);
            InterfacePacketBuf::put_short(v681, 0);
            InterfacePacketBuf::put_byte(v681, 0);
            v483 = strlen(s);
            InterfacePacketBuf::put_int(v681, v483);
            v484 = strlen(s);
            InterfacePacketBuf::put_str(v681, s, v484);
            InterfacePacketBuf::finalize(v681, 1);
            CParty::send_to_party((CUser **)v812, (PacketGuard *)v681);
            PacketGuard::~PacketGuard(v681);
          }

          if ( v814 && CParty::get_member_count((CUser **)v814) > 1 )
          {
            BattleData::SetHellPartyValueTotal((CParty *)((char *)v814 + 812), v813);
            CParty::send_hellParty_value(v814);
            memset(s, 0, 255);
            HellPartyValueTotal = BattleData::GetHellPartyValueTotal((CParty *)((char *)v814 + 812));
            sprintf(s, "HELL PARTY VALUE (%d)", HellPartyValueTotal);
            PacketGuard::PacketGuard((PacketGuard *)v680);
            InterfacePacketBuf::put_header(v680, 0, 12);
            InterfacePacketBuf::put_byte(v680, 0);
            InterfacePacketBuf::put_short(v680, 0);
            InterfacePacketBuf::put_byte(v680, 0);
            v492 = strlen(s);
            InterfacePacketBuf::put_int(v680, v492);
            v493 = strlen(s);
            InterfacePacketBuf::put_str(v680, s, v493);
            InterfacePacketBuf::finalize(v680, 1);
            CParty::send_to_party((CUser **)v814, (PacketGuard *)v680);
            PacketGuard::~PacketGuard(v680);
          }

      CParty::send_to_party((CUser **)Party, (PacketGuard *)v7);
      PacketGuard::~PacketGuard(v7);

    CParty::kill_blood_monster(Party, a2, v10, v8);
  }
  return 0;
}
// 8220914: conditional instruction was optimized away because bl.1 is in (==0|==FE)

//----- (08220A60) --------------------------------------------------------
int __cdecl Dispatcher_CompoundEmblem::_selectJewelTypeIndex(
        Dispatcher_CompoundEmblem *this,
        unsigned int id,
        const unsigned __int16 *a3)
{
  int id; // [esp+28h] [ebp-50h] BYREF
  _DWORD value[5]; // [esp+2Ch] [ebp-4Ch] BYREF
  int id[5]; // [esp+40h] [ebp-38h] BYREF
  int id; // [esp+54h] [ebp-24h]
  signed int rand_int; // [esp+58h] [ebp-20h]
  unsigned int i; // [esp+5Ch] [ebp-1Ch]
  int AvatarEmblemTypeIdxForCompountEmblem; // [esp+60h] [ebp-18h]
  int j; // [esp+64h] [ebp-14h]
  int id; // [esp+68h] [ebp-10h]
  int k; // [esp+6Ch] [ebp-Ch]

  memset(v6, 0, sizeof(v6));
  memset(v5, 0, sizeof(v5));
  v4 = 0;
  v7 = 0;
  for ( i = 0; i < a2; ++i )
  {

      CParty::set_single_play(Party, a3);
  }
  if ( Party )
  {

    v10 = CParty::OnFightVillageMonster(a1, (CUser **)Party, a3, v8);
    if ( v10 )

    v7 = CParty::OnFinishVillageMonsterFighting(a1, Party, a3, 0);
    if ( v7 )

  if ( (unsigned __int8)CParty::SetRidableObjectState(Party, v4, v7) != 1 )
    CUser::SendCmdErrorPacket((CUser *)a2, 317, 19);

  if ( !Party || (CUser *)CParty::getManager(Party) != a2 )
    return 0;

      CParty::send_to_party((CUser **)Party, (PacketGuard *)v8);
      CBattle_Field::startUltimateRound((CParty *)((char *)Party + 2852));

  if ( (CUser *)CParty::getManager(Party) != a2 )
    return 0;

    CParty::switchPostion(Party, v8, v7);
    CParty::send_party_realtime_info((CUser **)Party);

    CParty::send_party_ipinfo((CUser **)Party);
    InterfacePacketBuf::put_header(v6, 1, 336);

int __cdecl CParty::IsSinglePlay(CParty *this)
{
  return *((unsigned __int8 *)this + 105);
}

CParty *__cdecl CParty::SetRecvResultFlag(CParty *this, bool a2)
{
  *((_BYTE *)this + 280) = a2;
  return this;
}

int __cdecl CParty::get_state(CParty *this)
{
  return *((unsigned __int8 *)this + 106);
}

int __cdecl CParty::GetPartyIndex(CParty *this)
{
  return *((_DWORD *)this + 25);
}

CParty *__cdecl CParty::setDungIndex(CParty *this, unsigned __int16 a2)
{
  *((_WORD *)this + 159) = a2;
  return this;
}

CParty *__cdecl CParty::setDungDiffi(CParty *this, unsigned __int8 a2)
{
  *((_BYTE *)this + 320) = a2;
  return this;
}

int __cdecl CParty::IsAutoCreated(CParty *this)
{
  return *((unsigned __int8 *)this + 316);
}

int __cdecl CParty::get_dungeon_clear_state(CParty *this)
{
  return *((unsigned __int8 *)this + 480);
}

CParty *__cdecl CParty::set_dungeon_clear_state(CParty *this, CParty *party)
{
  CParty *result; // eax

  result = a2;
  if ( (char)a2 <= 6 )
  {
    *((_BYTE *)this + 480) = (_BYTE)a2;
    return this;
  }
  return result;
}

CParty *__cdecl CParty::IncreaseUsedCoinCount(CParty *this)
{
  ++*((_DWORD *)this + 1560);
  return this;
}

int __cdecl CParty::GetUsedCoinCount(CParty *this)
{
  return *((_DWORD *)this + 1560);
}

_BOOL4 __cdecl CParty::checkBossRoom(CParty *this)
{
  return CBattle_Field::check_end_point((CParty *)((char *)this + 2852));
}

char *__cdecl CParty::GetPartyTelePort(CParty *this)
{
  return (char *)this + 6864;
}

char *__cdecl CParty::GetMember(CParty *this)
{
  return (char *)this + 120;
}

int __cdecl CParty::get_quick_party_index(CParty *this)
{
  return *((_DWORD *)this + 1720);
}

CParty *__cdecl CParty::set_quick_party(CParty *this, bool a2)
{
  *((_BYTE *)this + 6884) = a2;
  return this;
}

int __cdecl CParty::is_quick_party(CParty *this)
{
  return *((unsigned __int8 *)this + 6884);
}

int __cdecl CParty::set_gm_random_buff_type(int value, int value2)
{
  *(_DWORD *)(a1 + 6892) = a2;
  return a1;
}

      if ( CParty::isMonsterDrop_item(Party, a4) )
      {
        if ( *((_BYTE *)Party + 3308) != 1
          || (v4 = *((_DWORD *)Party + 824), v4 != CBattle_Field::get_cur_map((CParty *)((char *)Party + 2852))) )
        {
          ++*((_DWORD *)this + 145693);
        }
      }

    Party = CParty::get_member_count((CUser **)v31);
    v32 = Party;

        if ( (unsigned __int8)CParty::checkValidUser((CUser **)v31, i) == 1 )
        {
          user = (CUserCharacInfo *)CParty::get_user(v31, i);
          if ( user )
          {
            if ( i )
            {
              v3 = charac_job;
              LOBYTE(Party) = v3 != CUserCharacInfo::get_charac_job(user)
                           || (CurCharFirstGrowType = CUserCharacInfo::getCurCharFirstGrowType(user),
                               CurCharFirstGrowType != v27);
              if ( (_BYTE)Party )
              {
                v24 = 0;
                v25 = 0;
                break;
              }
              n = 0;
              *(_DWORD *)s2 = 0;
              v17 = 0;
              v18 = 0;
              v19 = 0;
              CNetwork<NETWORK_BUFFER_SIZE_4K,MAX_NETWORK_CONNECTIONS_450K>::GetPeerIP2((int *)user + 56, s2, 16);
              n = strlen(s);
              Party = strncmp(s, s2, n);
              if ( Party )
              {
                v24 = 0;
                v25 = 0;
                break;
              }
              v36 = 0;
              Party = CUserCharacInfo::getCurCharacSkillR(user);
              v36 = (SkillSlot *)Party;
              if ( !Party )
              {
                v24 = 0;
                v25 = 0;
                break;
              }
              skill_index = SkillSlot::get_skill_index((CUserCharacInfo **)v36, 0);
              if ( skill_index != v28
                || (v6 = SkillSlot::get_skill_index((CUserCharacInfo **)v36, 1), v6 != v29)
                || (v7 = SkillSlot::get_skill_index((CUserCharacInfo **)v36, 2), v7 != v30) )
              {
                v24 = 0;
              }
            }
            else
            {
              charac_job = CUserCharacInfo::get_charac_job(user);
              v27 = CUserCharacInfo::getCurCharFirstGrowType(user);
              v37 = 0;
              *(_DWORD *)src = 0;
              v13 = 0;
              v14 = 0;
              v15 = 0;
              v38 = 0;
              v39 = 0;
              CNetwork<NETWORK_BUFFER_SIZE_4K,MAX_NETWORK_CONNECTIONS_450K>::GetPeerIP2((int *)user + 56, src, 16);
              Party = strlen(src);
              if ( (unsigned int)Party <= 8 )
              {
                v24 = 0;
                v25 = 0;
                break;
              }
              Party = (int)std::strrchr(src, 46);
              v37 = (char *)Party;
              if ( !Party )
              {
                v24 = 0;
                v25 = 0;
                break;
              }
              v38 = strlen(src);
              v39 = strlen(v37);
              strncpy(s, src, v38 - v39);
              v40 = 0;
              Party = CUserCharacInfo::getCurCharacSkillR(user);
              v40 = (SkillSlot *)Party;
              if ( !Party )
              {
                v24 = 0;
                v25 = 0;
                break;
              }
              v28 = SkillSlot::get_skill_index((CUserCharacInfo **)v40, 0);
              v29 = SkillSlot::get_skill_index((CUserCharacInfo **)v40, 1);
              v30 = SkillSlot::get_skill_index((CUserCharacInfo **)v40, 2);
            }
          }
        }

      CParty::send_to_party((CUser **)party_object, (PacketGuard *)v5);
      PacketGuard::~PacketGuard(v5);

  member_count = CParty::get_member_count(a3);
  if ( member_count <= 0 || member_count > 3 )

    return CParty::checkInoutConditionDungeon(a3, dungeon, 0, 444);
  }
}

//----- (08269F3C) --------------------------------------------------------
int __usercall QuickParty::CQuickPartySystemManager::match_proper_party@<eax>(
        int id@<esi>,
        QuickParty::CQuickPartySystemManager *this,
        const QuickParty::STQuickPartyPoolMap_Key *a3,
        CParty *party,
        CUser *user)
{
  const char *CurCharacNo; // eax
  const char *quick_party_index; // eax
  int id; // ebx
  CDungeonList **v10; // eax
  int id; // eax
  int id; // eax
  int PartyIndex; // eax
  char flag; // bl
  int id; // eax
  int id; // eax
  int id; // eax
  char flag; // [esp+8h] [ebp-70h]
  int id; // [esp+2Ch] [ebp-4Ch] BYREF
  int id; // [esp+30h] [ebp-48h] BYREF
  int id[4]; // [esp+34h] [ebp-44h] BYREF
  int id[4]; // [esp+44h] [ebp-34h] BYREF
  int id; // [esp+54h] [ebp-24h]
  int member_count; // [esp+58h] [ebp-20h]
  CDungeon *dungeon; // [esp+5Ch] [ebp-1Ch]
  int limit_party_count; // [esp+60h] [ebp-18h]
  int i; // [esp+64h] [ebp-14h]
  QuickParty::CQuickParty *quick_party_in_pool; // [esp+68h] [ebp-10h]
  char *quick_party_count; // [esp+6Ch] [ebp-Ch]

  if ( !a4 || CParty::get_quick_party_index(a4) != -1 )
  {

      quick_party_index = (const char *)CParty::get_quick_party_index(a4);
      LogManager::logFormat(

      member_count = CParty::get_member_count((CUser **)a4);
      v9 = *(__int16 *)a3;

          PartyIndex = CParty::GetPartyIndex(a4);
          QuickParty::CQuickParty::set_party_index(quick_party_in_pool, PartyIndex);

  if ( !a2 || CParty::get_quick_party_index(a2) == -1 )
  {
    if ( a2 || !a4 )
    {
      quick_party_index = (const char *)CParty::get_quick_party_index(a2);
      LogManager::logFormat(
        (LogManager *)1,
        (int)"QuickParty.cpp",
        "bool QuickParty::CQuickPartySystemManager::cancel_quick_party(CParty*, bool, CUser*)",
        (const char *)0x1E5,
        "QUICK_PARTY_LOG : ABNOMAL QUICK PARTY INDEX (%d)!!",
        quick_party_index);
    }
    else
    {
      CurCharacNo = (const char *)CUserCharacInfo::getCurCharacNo(a4);
      LogManager::logFormat(
        (LogManager *)1,
        (int)"QuickParty.cpp",
        "bool QuickParty::CQuickPartySystemManager::cancel_quick_party(CParty*, bool, CUser*)",
        (const char *)0x1E3,
        "QUICK_PARTY_LOG : PARTY IS NULL!! (%d)",
        CurCharacNo);
    }
    return 0;
  }

    v8 = CParty::get_quick_party_index(a2);
    v9 = G_CGameManager();

    if ( (char)CParty::GetEPLPState(a2) > 1 && a3 )
    {
      CParty::SetSelectedEPLPCmd(a2, -1);
      dungeon_index = QuickParty::CQuickParty::get_dungeon_index(QuickParty);
      RandomBuffDungeonType = CBattle_Field::getRandomBuffDungeonType((CParty *)((char *)a2 + 2852), dungeon_index);
      CBattle_Field::setEplpWaitingTimer((int *)a2 + 713, RandomBuffDungeonType);
    }

    if ( CParty::get_member_count((CUser **)a2) == 1 || a3 )
    {
      PartyIndex = CParty::GetPartyIndex(a2);
      QuickParty::CQuickParty::delete_party_index(QuickParty, PartyIndex);
      quick_party_count = (char *)QuickParty::CQuickParty::get_quick_party_count(QuickParty);
      CParty::init_quick_party_data(a2);
      if ( (int)quick_party_count <= 0 )
      {
        QuickParty::CQuickParty::destroy(QuickParty);
        return 1;
      }
    }

        v15 = CParty::GetPartyIndex(a2);
        QuickParty::CQuickParty::delete_party_index(QuickParty, v15);

        CParty::init_quick_party_data(a2);
        QuickParty::CQuickParty::destroy(QuickParty);

  quick_party_index = CParty::get_quick_party_index(Party);
  v5 = G_CGameManager();

        CParty::send_to_party((CUser **)party_object, (PacketGuard *)v13);
        v3 = 1;

  v2 = CParty::gen_timer_key((int)a2, 45);
  PartyIndex = CParty::GetPartyIndex(a2);

  PartyIndex = CParty::GetPartyIndex(a5);
  v9 = QuickParty::CQuickPartySystemManager::pop_quick_party_in_pool(this, &v8, a3, a4, PartyIndex, a6);

          CParty::init_quick_party_data(party_object);
          PacketGuard::PacketGuard((PacketGuard *)v10);

          CParty::send_to_party((CUser **)party_object, (PacketGuard *)v10);
          PacketGuard::~PacketGuard(v10);

      if ( (char)CParty::GetEPLPState(Party) > 1 )
      {
        v15 = Party;
        break;
      }

      if ( (unsigned __int8)CParty::is_quick_party(Party) )
        v15 = Party;

    CParty::set_state(v15, 1);
    CParty::set_quick_party(v15, 1);

    CParty::setDungIndex(v15, 1u);
    CParty::setDungDiffi(v15, 0);

    if ( (unsigned __int8)CParty::IsAutoCreated(v15) )
    {
      CParty::SetSinglePlay(v15, 0);
      CParty::SetAutoCreated(v15, 0);
      v5 = G_GameWorld();
      GameWorld::send_party_info_to_all(v5, (CUser **)v15, 0);
    }

              if ( (unsigned __int8)CParty::checkValidUser((CUser **)v22, m) == 1 )
              {
                user = (CUser *)CParty::get_user(v22, m);
                CParty::delete_party_for_quick_party((CUser **)v22, user, (int)v13);
              }

      CParty::join_user(v15, *v8);
      std::_List_iterator<CUser *>::operator++(&v12);

CParty *__cdecl CParty::SetSinglePlay(CParty *this, bool a2)
{
  *((_BYTE *)this + 105) = a2;
  return this;
}

CParty *__cdecl CParty::set_state(CParty *this, char flag)
{
  *((_BYTE *)this + 106) = a2;
  return this;
}

CParty *__cdecl CParty::SetAutoCreated(CParty *this, bool a2)
{
  *((_BYTE *)this + 316) = a2;
  return this;
}

CParty *__cdecl CParty::set_quick_party_index(CParty *this, int value)
{
  *((_DWORD *)this + 1720) = a2;
  return this;
}

              if ( (unsigned __int8)CParty::checkValidUser((CUser **)*a1, i) )
              {
                user = (CUserCharacInfo *)CParty::get_user(*a1, i);
                v6 = (CUser *)CParty::get_user(*a1, i);
                HackAnalyzer = CUser::getHackAnalyzer(v6);
                WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)HackAnalyzer, user, 0xE6u, 1, 0, 0);
              }

              if ( (unsigned __int8)CParty::checkValidUser((CUser **)*a1, j) )
              {
                v9 = (CUserCharacInfo *)CParty::get_user(*a1, j);
                v10 = (CUser *)CParty::get_user(*a1, j);
                v11 = CUser::getHackAnalyzer(v10);
                WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)v11, v9, 0xE7u, 1, 0, 0);
              }

          if ( (unsigned __int8)CParty::checkValidUser(*(CUser ***)this, i) )
          {
            user = (CUser *)CParty::get_user(*(CParty **)this, i);
            v4 = 100 * CUserCharacInfo::get_charac_job(user);
            v5 = 10 * CUserCharacInfo::getCurCharFirstGrowType(user) + v4;
            v23 = v5 + CUserCharacInfo::getCurCharSecondGrowType(user);
            if ( v19 > 0 )
            {
              v6 = CUser::SetETC(user, 232, v23);
              v7 = v19;
              HackAnalyzer = CUser::getHackAnalyzer(user);
              WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)HackAnalyzer, user, 0xE8u, v7, v6, 0);
            }
            if ( v20 > 0 )
            {
              v9 = CUser::SetETC(user, 233, v23);
              v10 = v20;
              v11 = CUser::getHackAnalyzer(user);
              WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)v11, user, 0xE9u, v10, v9, 0);
            }
          }

    if ( (unsigned __int8)CParty::IsFirstMapClear(a3) != 1 )
    {
      CParty::SetFirstMapClear(a3, 1);
    }

      if ( ((unsigned __int8)CParty::IsAutoCreated(a3) || CParty::get_member_count((CUser **)a3) == 1)
        && map

  PartyTelePort = CParty::GetPartyTelePort(Party);
  if ( (unsigned __int8)CPartyTelePort::get_teleport_state((CPartyTelePort *)PartyTelePort) == MASK_BYTE )

  CParty::SetIDX(a1, v10, Index);
  PartyIndex = CParty::GetPartyIndex(v10);

    v9 = CParty::GetPartyIndex(v10);
    v4 = __gnu_cxx::hash_map<int,CParty *,__gnu_cxx::hash<int>,std::equal_to<int>,std::allocator<CParty *>>::operator[](

    v5 = (const char *)CParty::GetPartyIndex(v10);
    LogManager::logFormat(

    PartyIndex = CParty::GetPartyIndex(a2);
    __gnu_cxx::hash_map<int,CParty *,__gnu_cxx::hash<int>,std::equal_to<int>,std::allocator<CParty *>>::find(

      v5[0] = CParty::GetPartyIndex(a2);
      __gnu_cxx::hash_map<int,CParty *,__gnu_cxx::hash<int>,std::equal_to<int>,std::allocator<CParty *>>::erase(

    if ( v28 && (unsigned __int8)CParty::IsAutoCreated(v28) != 1 )
    {
      ++v26;
      PartyIndex = CParty::GetPartyIndex(v28);
      InterfacePacketBuf::put_short(v23, PartyIndex);
      InterfacePacketBuf::put_byte(v23, v27);
      if ( v27 <= 1u )
      {
        TitleIndex = CParty::getTitleIndex(v28);
        InterfacePacketBuf::put_byte(v23, TitleIndex);
        if ( !(unsigned __int8)CParty::getTitleIndex(v28) )
        {
          Title = CParty::getTitle(v28);
          v7 = strlen(Title);
          InterfacePacketBuf::put_int(v23, v7);
          v8 = CParty::getTitle(v28);
          v9 = strlen(v8);
          v10 = CParty::getTitle(v28);
          InterfacePacketBuf::put_str(v23, v10, v9);
        }
        if ( (unsigned __int8)CParty::IsReturnUserParty((CUser **)v28) )
          InterfacePacketBuf::put_byte(v23, 1);
        else
          InterfacePacketBuf::put_byte(v23, 0);
        UserMax = CParty::getUserMax(v28);
        InterfacePacketBuf::put_byte(v23, UserMax);
        DungIndex = CParty::getDungIndex(v28);
        InterfacePacketBuf::put_short(v23, DungIndex);
        DungDiffi = CParty::getDungDiffi(v28);
        InterfacePacketBuf::put_byte(v23, DungDiffi);
        if ( (unsigned __int8)CParty::IsEventCharacParty((CUserCharacInfo **)v28) )
          InterfacePacketBuf::put_byte(v23, 1);
        else
          InterfacePacketBuf::put_byte(v23, 0);
      }
      if ( !v27 || v27 == 2 )
      {
        v29 = 0;
        for ( i = 0; i <= 3; ++i )
        {
          user = (CUser *)CParty::get_user(v28, i);
          if ( user )
          {
            unique_id = CUser::get_unique_id(user);
            InterfacePacketBuf::put_short(v23, unique_id);
            Manager = (CUser *)CParty::getManager(v28);
            if ( Manager == user )
              v29 = i;
            Sex = CUser::getSex(user);
            if ( Sex == -1 )
              Sex = 1;
            InterfacePacketBuf::put_byte(v23, Sex);
          }
          else
          {
            InterfacePacketBuf::put_short(v23, INVALID_ID_U16);
            InterfacePacketBuf::put_byte(v23, 255);
          }
        }
        if ( (unsigned __int8)CParty::IsReturnUserParty((CUser **)v28) )
          InterfacePacketBuf::put_byte(v23, 1);
        else
          InterfacePacketBuf::put_byte(v23, 0);
        InterfacePacketBuf::put_byte(v23, v29);
        is_quick_party = CParty::is_quick_party(v28);
        InterfacePacketBuf::put_byte(v23, is_quick_party);
        if ( (unsigned __int8)CParty::IsEventCharacParty((CUserCharacInfo **)v28) )
          InterfacePacketBuf::put_byte(v23, 1);
        else
          InterfacePacketBuf::put_byte(v23, 0);
      }
      if ( v27 <= 2u )
      {
        v17 = G_GameWorld();
        is_dungeon_tag_matching_channel = GameWorld::is_dungeon_tag_matching_channel(v17, "[impossible]");
        InterfacePacketBuf::put_byte(v23, is_dungeon_tag_matching_channel);
        if ( is_dungeon_tag_matching_channel )
        {
          for ( j = 0; j <= 3; ++j )
          {
            v35 = (CUserCharacInfo *)CParty::get_user(v28, j);
            if ( v35 )
            {
              InterfacePacketBuf::put_byte(v23, 6);
              for ( k = 0; k <= 5; ++k )
              {
                v18 = G_CDataManager();
                dimensionInout = CDataManager::get_dimensionInout(v18, k);
                InterfacePacketBuf::put_byte(v23, dimensionInout);
                CurCharacR = CUserCharacInfo::getCurCharacR(v35);
                InterfacePacketBuf::put_byte(v23, *(_BYTE *)(CurCharacR + k + 3769));
              }
            }
            else
            {
              InterfacePacketBuf::put_byte(v23, 0);
            }
          }
        }
      }
    }

      if ( *((_DWORD *)v6 + 822) != 1 && (unsigned __int8)CParty::get_state(v6) == 2 )
      {
        v9 = 0;
        if ( v8 )
          v9 = *((_BYTE *)v8 + 2140) != 0;
        v10 = 0;
        if ( v8 )
          v10 = *((_BYTE *)v8 + 2170) != 0;
        if ( !v7 && v9 && v10 )
          CUser::giveup_panalty(a2);
      }

      return CParty::leave_user((std::string **)v6, (std::string *)a2, 2);
    }
  }
  return result;
}
// 8297C8D: variable 'v2' is possibly undefined

//----- (08297DF8) --------------------------------------------------------
void __cdecl CGameManager::CheckOutWarRoom(CGameManager *this, CUser *user)
{
  __int16 WarRoomIndex; // ax
  const char *charac_no; // eax
  CUser **WarRoom; // [esp+2Ch] [ebp-Ch]

  if ( (unsigned __int8)CUser::CheckInWarRoom(a2) == 1 )
  {
    WarRoomIndex = CUser::GetWarRoomIndex(a2);
    WarRoom = (CUser **)CGameManager::GetWarRoom(this, WarRoomIndex);
    if ( WarRoom )
    {

void __cdecl CParty::CParty(CParty *this)
{
  CParty::cMember *v1; // ebx
  int i; // esi
  int j; // [esp+1Ch] [ebp-Ch]

  CParty::CItemRoutingData::CItemRoutingData(this);
  Mutex::Mutex((pthread_mutex_t *)((char *)this + 76));
  v1 = (CParty *)((char *)this + 120);
  for ( i = 3; i != -1; --i )
  {
    CParty::cMember::cMember(v1);
    v1 = (CParty::cMember *)((char *)v1 + 24);
  }
  cElection<int,4,4>::cElection((int)this + 216);
  CPartyResultRecvFlag::CPartyResultRecvFlag((CParty *)((char *)this + 476));
  CTraceMobDieHack::CTraceMobDieHack((CParty *)((char *)this + 528));
  secretshop::SECRET_SHOP_DATA::SECRET_SHOP_DATA((CParty *)((char *)this + 676));
  BattleData::BattleData((CParty *)((char *)this + 812));
  CBattle_Field::CBattle_Field((CParty *)((char *)this + 2852));
  GameResultSet::GameResultSet((CParty *)((char *)this + 6132));
  std::map<int,int>::map((_DWORD *)this + 1553);
  Secu_HackLogCheckByParty::Secu_HackLogCheckByParty((CParty *)((char *)this + 6844));
  CPartyTelePort::CPartyTelePort((CParty *)((char *)this + 6864));
  std::vector<MapInfo>::vector((_DWORD *)this + 1725);
  for ( j = 11; j <= 47; ++j )
    *((_DWORD *)this + j + 71) = 0;
  *((_DWORD *)this + 1560) = 0;
  CBattle_Field::SetParty((CParty *)((char *)this + 2852), this);
  CParty::SetAssaultState(this, 0);
  CParty::CItemRoutingData::Reset(this);
  CPartyTelePort::init((CParty *)((char *)this + 6864), this);
  *((_DWORD *)this + 1723) = 11;
  *((_BYTE *)this + 6884) = 0;
  CParty::init_quick_party_data(this);
  Secu_HackLogCheckByParty::Init((CParty *)((char *)this + 6844), this);
  *((_DWORD *)this + 202) = 0;
}

  return CParty::init(a1, (CUser **)this);
}

//----- (082A59E6) --------------------------------------------------------
char *__cdecl CParty::getTitle(CParty *this)
{

int __cdecl CParty::getTitleIndex(CParty *this)
{
  return *((unsigned __int8 *)this + 283);
}

int __cdecl CParty::getUserMax(CParty *this)
{
  return *((unsigned __int8 *)this + 317);
}

int __cdecl CParty::getDungIndex(CParty *this)
{
  return *((unsigned __int16 *)this + 159);
}

int __cdecl CParty::getDungDiffi(CParty *this)
{
  return *((unsigned __int8 *)this + 320);
}

CParty *__cdecl CParty::SetAssaultState(CParty *this, char flag)
{
  *((_BYTE *)this + 109) = a2;
  return this;
}

  CParty::CParty(a1);
  *((_DWORD *)a1 + 1728) = 36641;

    user = (CUserCharacInfo *)CParty::get_user(Party, i);
    if ( user )

    if ( (unsigned __int8)CParty::is_limit_minimum_age(Party, v15, a3) != 1 )
    {
      v9 = _cxa_allocate_exception(4u);
      *v9 = 95;
      _cxa_throw(v9, (struct type_info *)&`typeinfo for'ENUM_ERROR, 0);
    }

    v5 = CParty::is_limit_minimum_level(Party, 30, a3) ^ 1;
    if ( (_BYTE)v5 )

    CParty::SetAssaultState(*((CParty **)this + 75), 2);
  if ( *((_DWORD *)this + 76) )

    CParty::SetAssaultState(*((CParty **)this + 76), 2);
  v8 = (const char *)&unk_8C1BCA0;

        CParty::SetAssaultState(*((CParty **)this + 75), 6);
      if ( *((_DWORD *)this + 76) )

        CParty::SetAssaultState(*((CParty **)this + 76), 6);
      if ( CPowerManager::IsPowerWarEventOn() )

        CParty::SetAssaultState(*((CParty **)this + 75), 4);
      if ( *((_DWORD *)this + 76) )

        CParty::SetAssaultState(*((CParty **)this + 76), 4);
      PacketGuard::PacketGuard((PacketGuard *)v16);

      CParty::SetAssaultState(*((CParty **)this + 75), 7);
    if ( *((_DWORD *)this + 76) )

      CParty::SetAssaultState(*((CParty **)this + 76), 7);
    if ( CPowerManager::IsPowerWarEventOn() )

      v14 = CParty::gen_timer_key((int)v27, 42);
      PartyIndex = CParty::GetPartyIndex(v27);

      v16 = CParty::gen_timer_key((int)v28, 42);
      v17 = CParty::GetPartyIndex(v28);

      CParty::SetAssaultState(*(this + 75), 5);
    if ( *(this + 76) )

      CParty::SetAssaultState(*(this + 76), 5);
    return pvp_assault::CAssaultPlace::EndAssault(CharacName, (pvp_assault::CAssaultPlace *)this);

      CParty::SetAssaultState(*(this + 75), 7);
    if ( *(this + 76) )

      CParty::SetAssaultState(*(this + 76), 7);
    *(this + 67) = (CUserCharacInfo *)7;

    CParty::SetAssaultState(*((CParty **)this + 75), 3);
  if ( *((_DWORD *)this + 76) )

    CParty::SetAssaultState(*((CParty **)this + 76), 3);
  return 1;

      CParty::SetAssaultState(*((CParty **)this + 75), 4);
    if ( *((_DWORD *)this + 76) )

      CParty::SetAssaultState(*((CParty **)this + 76), 4);
  }
  return started;
}
// 82EA7F7: variable 'v11' is possibly undefined
// 8B75F40: using guessed type int *`typeinfo for'ENUM_ERROR;

//----- (082EAB1A) --------------------------------------------------------
_BOOL4 __cdecl pvp_assault::CAssaultPlace::_IsBroadcastRunaway(pvp_assault::CAssaultPlace *this, CUser *user)
{
  return (!CPowerManager::IsPowerWarEventOn() || (unsigned __int8)CUserCharacInfo::getCurCharacVill(a2) != 7)
      && CUserCharacInfo::getCurCharacR(a2) != 0;
}

//----- (082EAB7A) --------------------------------------------------------
int __cdecl pvp_assault::CAssaultPlace::_BroadcastRunaway(pvp_assault::CAssaultPlace *this, CUser *user, int id)
{
  __int16 *p_s; // edx
  unsigned int id; // ebx
  const char *AnotherTeamCharacName; // esi
  const char *CurCharacName; // ebx
  CDataManager *v7; // eax
  char *AssaultUserRunawayNotice; // eax
  int ServerGroup; // eax
  std::string *ServerProxy; // eax
  __int16 s; // [esp+1Eh] [ebp-52Ah] BYREF
  int id; // [esp+20h] [ebp-528h] BYREF
  char buf[2]; // [esp+41Eh] [ebp-12Ah] BYREF
  unsigned __int16 v15; // [esp+420h] [ebp-128h]
  int id; // [esp+428h] [ebp-120h]
  unsigned __int8 v17; // [esp+42Ch] [ebp-11Ch]
  char flag[255]; // [esp+42Dh] [ebp-11Bh] BYREF
  size_t n; // [esp+52Ch] [ebp-1Ch]

  p_s = &s;
  v4 = BUFFER_SIZE_1K;
  if ( ((unsigned __int8)&s & 2) != 0 )
  {

        CParty::SetAssaultState(*(this + 75), 7);
      if ( *(this + 76) )

        CParty::SetAssaultState(*(this + 76), 7);
LABEL_41:

        CParty::SetAssaultState(*(this + 75), 7);
      if ( *(this + 76) )

        CParty::SetAssaultState(*(this + 76), 7);
      goto LABEL_41;

          CParty::resetStraightVictories(Party);
        else

        CParty::SetAssaultState(*(this + 75), 6);
      if ( *(this + 76) )

        CParty::SetAssaultState(*(this + 76), 6);
      *(this + 67) = (CUserCharacInfo *)6;

      CParty::SetAssaultState(*((CParty **)this + 75), 3);
    v5 = *((_DWORD *)this + 76);

      LOBYTE(v5) = (unsigned __int8)CParty::SetAssaultState(*((CParty **)this + 76), 3);
  }
  return v5;
}
// 8B75F40: using guessed type int *`typeinfo for'ENUM_ERROR;

//----- (082EB956) --------------------------------------------------------
char __usercall pvp_assault::CAssaultPlace::ConnectP2P@<al>(
        char flag@<sil>,
        pvp_assault::CAssaultPlace *this,
        CUser *user,
        char *src,
        int id)
{
  _DWORD *exception; // eax
  _DWORD *v6; // eax
  int id; // eax
  char flag; // [esp+1Bh] [ebp-Dh]
  int i; // [esp+1Ch] [ebp-Ch]

  if ( *((_DWORD *)this + 67) != 3 )
  {

      CParty::SetAssaultState(*((CParty **)this + 75), 4);
    v7 = *((_DWORD *)this + 76);

      LOBYTE(v7) = (unsigned __int8)CParty::SetAssaultState(*((CParty **)this + 76), 4);
  }
  return v7;
}
// 8B75F40: using guessed type int *`typeinfo for'ENUM_ERROR;

//----- (082EBAEE) --------------------------------------------------------
int pvp_assault::CAssaultPlace::_IsChangeDieState()
{
  return 1;
}

//----- (082EBAF8) --------------------------------------------------------
int __cdecl pvp_assault::CAssaultPlace::_ChangeDieState(CUserCharacInfo **this, int id)
{
  int RepeatEvent; // eax
  int result; // eax
  CUserCharacInfo *User; // eax

  pvp_assault::CAssaulter::SetState((int)(this + 8 * a2), 6);
  RepeatEvent = CEventManager::GetRepeatEvent(GlobalData::s_event_manager, 32);
  result = (*(int (__cdecl **)(int, _DWORD))(*(_DWORD *)RepeatEvent + 52))(RepeatEvent, 0);
  if ( !(_BYTE)result )
  {

        CParty::resetStraightVictories(Party);
      else

      CParty::SetAssaultState(*((CParty **)a1 + 75), 5);
    if ( *((_DWORD *)a1 + 76) )

      CParty::SetAssaultState(*((CParty **)a1 + 76), 5);
    return pvp_assault::CAssaultPlace::EndAssault(v6, a1);

      CParty::incStraightVictories(Party);
      StraightVictories = CParty::getStraightVictories(Party);

        member_count = CParty::get_member_count((CUser **)Party);
        InterfacePacketBuf::put_byte(v7, member_count);

          user = (CUserCharacInfo *)CParty::get_user(Party, i);
          if ( user )

    member_count = CParty::get_member_count((CUser **)Party);
    InterfacePacketBuf::put_byte(v8, member_count);

      user = (CUserCharacInfo *)CParty::get_user(Party, i);
      if ( user )

    v6 = CParty::get_member_count((CUser **)v16);
    InterfacePacketBuf::put_byte(v8, v6);

      v18 = (CUserCharacInfo *)CParty::get_user(v16, j);
      if ( v18 )

    CParty::send_to_party((CUser **)Party, (PacketGuard *)v6);
  else

        CParty::send_to_party((CUser **)Party, (PacketGuard *)v5);
      else

        CParty::SetAssaultState(*(this + 75), 7);
      if ( *(this + 76) )

        CParty::SetAssaultState(*(this + 76), 7);
      *(this + 67) = (CParty *)7;

    if ( Party && !CParty::CanDoPartyActionAssalutState(Party) )
    {
      v27 = _cxa_allocate_exception(4u);
      *v27 = 19;
      _cxa_throw(v27, (struct type_info *)&`typeinfo for'ENUM_ERROR, 0);
    }

  if ( v92 && (unsigned __int8)CParty::IsExistUser((CUser **)v92, a4) )
  {
    v57 = _cxa_allocate_exception(4u);
    *v57 = 19;
    _cxa_throw(v57, (struct type_info *)&`typeinfo for'ENUM_ERROR, 0);
  }

      if ( (unsigned __int8)CParty::IsThereGhostInParty((CUser **)v94) )
      {
        v59 = _cxa_allocate_exception(4u);
        *v59 = 19;
        _cxa_throw(v59, (struct type_info *)&`typeinfo for'ENUM_ERROR, 0);
      }

    CParty::ConnectP2P(Party, a2, src, a4);
  return 1;

  if ( !CParty::check_timer_key((int)Party, 42, a3) )
  {
    v5 = _cxa_allocate_exception(4u);
    *v5 = 23;
    _cxa_throw(v5, (struct type_info *)&`typeinfo for'ENUM_ERROR, 0);
  }

  if ( (unsigned __int8)CParty::TimeoutCompleteAfterAssault(Party) )
  {
    PacketGuard::PacketGuard((PacketGuard *)v7);
    InterfacePacketBuf::clear(v7);
    InterfacePacketBuf::put_header(v7, 1, 248);
    InterfacePacketBuf::put_byte(v7, 1);
    InterfacePacketBuf::finalize(v7, 1);
    CParty::send_to_party((CUser **)Party, (PacketGuard *)v7);
    PacketGuard::~PacketGuard(v7);
  }

  if ( Party && (unsigned __int8)CParty::TimeoutCompleteAfterAssault((CParty *)Party) )
    CParty::ClearBadP2PUser(Party);

    CParty::send_to_party((CUser **)Party, (PacketGuard *)v6);
  else

    CParty::send_to_party((CUser **)Party, (PacketGuard *)v6);
  else

    PartyTelePort = CParty::GetPartyTelePort(Party);
    if ( (CPartyTelePort::get_teleport_state((CPartyTelePort *)PartyTelePort) & FLAG_BIT7) == 0 )

      v6 = CParty::GetPartyTelePort(Party);
      if ( (char)CPartyTelePort::get_teleport_state((CPartyTelePort *)v6) <= 3 )

    v8 = CParty::GetPartyTelePort(v14);
    if ( (CPartyTelePort::get_teleport_state((CPartyTelePort *)v8) & FLAG_BIT7) == 0 )

      v9 = CParty::GetPartyTelePort(v14);
      if ( (char)CPartyTelePort::get_teleport_state((CPartyTelePort *)v9) <= 3 )

    if ( (unsigned __int8)CParty::is_limit_minimum_age((CUser **)Party, v13, &unique_id) != 1 )
    {
      v7 = _cxa_allocate_exception(4u);
      *v7 = 95;
      _cxa_throw(v7, (struct type_info *)&`typeinfo for'ENUM_ERROR, 0);
    }

    v4 = CParty::is_limit_minimum_level((CUser **)Party, 30, &unique_id) ^ 1;
    if ( (_BYTE)v4 )

int __cdecl CParty::IsExistUser(CUser **this, CUser *user)
{
  int i; // [esp+Ch] [ebp-4h]

  for ( i = 0; i <= 3; ++i )
  {
    if ( *(this + 6 * i + 30) == a2 )
      return 1;
  }
  return 0;
}

int __cdecl CParty::getStraightVictories(CParty *this)
{
  return *((unsigned __int16 *)this + 334);
}

CParty *__cdecl CParty::incStraightVictories(CParty *this)
{
  ++*((_WORD *)this + 334);
  return this;
}

CParty *__cdecl CParty::resetStraightVictories(CParty *this)
{
  *((_WORD *)this + 334) = 0;
  return this;
}

          && (unsigned __int8)CParty::CheckQuestConnection(*(CUser ***)a2, *(_DWORD *)(v18 + 80), i) )
        {
          v5 = *(_DWORD *)(v18 + 80);
          v6 = (QuestList **)G_CDataManager();
          quest = CDataManager::find_quest(v6, v5);
          if ( !quest || *(_BYTE *)(quest + 288) != 1 || CParty::get_member_count(*(CUser ***)a2) <= 1 )
          {
            if ( i )
            {
              v15 = v18;
              std::vector<MazeScript *>::push_back((int)v13, (int)&v15);
            }
            else if ( a3 >= *(_DWORD *)(v18 + 84) )
            {
              v14 = v18;
              std::vector<MazeScript *>::push_back((int)v13, (int)&v14);
            }
          }
        }

  member_count = CParty::get_member_count(*(CUser ***)a2);
  return CDungeon::GetRandMazebyPartyCount(*(CDungeon **)(a2 + 392), member_count);

            *((_BYTE *)a5 + 16) = CParty::getMemberMaxLevel(*(CUserCharacInfo ***)a2);
            if ( !*((_BYTE *)a5 + 16) )

  return CParty::historyBloodRoundClear(a4, *((_DWORD *)this + 2), v9 / 0x3E8, v7, DungeonName);
}
// 941F714: using guessed type time_t GlobalData::s_systemTime_[5];

//----- (08307096) --------------------------------------------------------
CBattle_Field::CBloodClearRewardData *__cdecl CBattle_Field::CBloodClearRewardData::incUltimateRoundClearPoint(
        CBattle_Field::CBloodClearRewardData *this,
        int id)
{

    CParty::GetLuckPoint((CUser **)*a1, (CLuckPoint *)v77, 1);
    if ( (unsigned __int8)CParty::IsSinglePlay(*a1) && CParty::getManager(*a1) )

      Manager = (CUser *)CParty::getManager(*a1);
      if ( CUser::getMoveSpace(Manager) == 1 )

        v14 = (CUser *)CParty::getManager(*a1);
        BlueMarble = (BlueMarble *)CUser::getBlueMarble(v14);

          v15 = (CUser *)CParty::getManager(*a1);
          Buff = BlueMarble::getBuff(BlueMarble, v15, 1);

          v16 = (CUser *)CParty::getManager(*a1);
          v83 = BlueMarble::getBuff(BlueMarble, v16, 2);

          v17 = (CUserCharacInfo *)CParty::getManager(*a1);
          charac_job = CUserCharacInfo::get_charac_job(v17);

          v18 = (CUserCharacInfo *)CParty::getManager(*a1);
          charac_level = CUserCharacInfo::get_charac_level(v18);

  HIBYTE(v69) = CParty::getMemberMaxLevel((CUserCharacInfo **)*a1);
  v70 = v57;

  PartyMemberForceDropFlag = CParty::GetPartyMemberForceDropFlag(*a1);
  if ( *(_BYTE *)(a4 + 841) != 1 || ItemDropDungeonType )

        if ( (unsigned __int8)CParty::hasPCRoomPlayer((CUser **)*a1) && *(_BYTE *)(a4 + 841) != 1 )
        {
          v36 = G_CDataManager();
          v37 = CDataManager::getItemGenerator(v36);
          v38 = WongWork::CItemGeneratorMgr::getGenerator(v37, 5);
          (*(void (__cdecl **)(int, int *, void **))(*(_DWORD *)v38 + 8))(v38, v60, v76);
        }

        if ( (unsigned __int8)CParty::hasPCRoomPlayer((CUser **)*a1) && *(_BYTE *)(a4 + 841) != 1 )
        {
          v41 = G_CDataManager();
          v42 = CDataManager::getItemGenerator(v41);
          v43 = WongWork::CItemGeneratorMgr::getGenerator(v42, 6);
          (*(void (__cdecl **)(int, int *, void **))(*(_DWORD *)v43 + 8))(v43, v60, v76);
        }

  if ( (unsigned __int8)CParty::hasPCRoomPlayer((CUser **)*a1) )
  {
    v44 = G_CDataManager();
    v45 = CDataManager::getItemGenerator(v44);
    v46 = WongWork::CItemGeneratorMgr::getGenerator(v45, 7);
    (*(void (__cdecl **)(int, int *, void **))(*(_DWORD *)v46 + 8))(v46, v60, v76);
  }

      if ( (unsigned __int8)CParty::IsSinglePlay(*a1) && CParty::getManager(*a1) )
      {
        v49 = (CUser *)CParty::getManager(*a1);
        if ( CUser::getMoveSpace(v49) == 1 && !*(_DWORD *)((char *)&v59[4] + 2) )
        {
          v50 = G_CDataManager();
          v88 = *(float *)(CDataManager::getBlueMarbleScript(v50) + 104);
          v51 = G_CDataManager();
          v89 = *(float *)(CDataManager::getBlueMarbleScript(v51) + 108);
          if ( Buff <= 0 )
            Buff = 1;
          LOBYTE(v52) = v55;
          HIBYTE(v52) = 12;
          v54 = v52;
          v56 = (int)((long double)*(_DWORD *)((char *)&v59[5] + 3) * ((long double)Buff * v89 + v88));
          *(_DWORD *)((char *)&v59[5] + 3) = v56;
        }
      }

        Manager = (CUserCharacInfo *)CParty::getManager(*(CParty **)this);
        CurCharacNo = CUserCharacInfo::getCurCharacNo(Manager);

        v21 = (CUserCharacInfo *)CParty::getManager(*(CParty **)this);
        v22 = CUserCharacInfo::getCurCharacNo(v21);

  if ( (unsigned __int8)CParty::IsSinglePlay(*(CParty **)this) )
  {
    v112 = CParty::getManager(*(CParty **)this);
    CharacExpandData = (CKillMonsterInfo *)CUser::GetCharacExpandData(v112, 0xCu);
    if ( *((_BYTE *)a3 + 8) == 3 )
    {
      dungeon_diff = CBattle_Field::get_dungeon_diff(this);
      if ( !CKillMonsterInfo::CheckBossKill(CharacExpandData, (int)a3[3], dungeon_diff) )
      {
        v24 = CBattle_Field::get_dungeon_diff(this);
        if ( (unsigned __int8)CKillMonsterInfo::RegisterBossKill(CharacExpandData, a3[3], v24) )
          v103 = 1;
      }
    }
    else if ( *((_BYTE *)a3 + 8) == 8 )
    {
      v25 = CBattle_Field::get_dungeon_diff(this);
      if ( !CKillMonsterInfo::CheckAPCBossKill(CharacExpandData, (int)a3[3], v25) )
      {
        v26 = CBattle_Field::get_dungeon_diff(this);
        if ( (unsigned __int8)CKillMonsterInfo::RegisterAPCBossKill(CharacExpandData, a3[3], v26) )
          v103 = 1;
      }
    }
    else if ( map_monster::isNamedMonster((map_monster *)a3) )
    {
      v27 = CBattle_Field::get_dungeon_diff(this);
      if ( !CKillMonsterInfo::CheckNamedKill(CharacExpandData, (int)a3[3], v27) )
      {
        v28 = CBattle_Field::get_dungeon_diff(this);
        if ( (unsigned __int8)CKillMonsterInfo::RegisterNamedKill(CharacExpandData, a3[3], v28) )
          v104 = 1;
      }
    }
    goto LABEL_71;
  }

  v114 = CParty::getManager(*(CParty **)this);
  v115 = (CKillMonsterInfo *)CUser::GetCharacExpandData(v114, 0xCu);

    CParty::IsSinglePlay(*(CParty **)this);
    v38 = (CUserCharacInfo *)CParty::getManager(*(CParty **)this);

      if ( (unsigned __int8)CParty::checkValidUser(*(CUser ***)this, k) == 1 )
      {
        v44 = CParty::GetMember(*(CParty **)this);
        if ( CUser::isAffectedPremium(*(_DWORD *)&v44[24 * k], 79) )
          ++v116;
      }

        if ( (unsigned __int8)CParty::checkValidUser(*(CUser ***)this, m) == 1 )
        {
          v45 = v116;
          v46 = CParty::GetMember(*(CParty **)this);
          AddIndependentDropRateFromPremium = CUser::getAddIndependentDropRateFromPremium(
                                                *(_DWORD *)&v46[24 * m],
                                                0x4Fu,
                                                v45);
          break;
        }

        if ( (unsigned __int8)CParty::checkValidUser(*(CUser ***)this, n) )
        {
          user = (CUser *)CParty::get_user(*(CParty **)this, n);
          if ( user )
          {
            HackAnalyzer = CUser::getHackAnalyzer(user);
            WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)HackAnalyzer, user, 0xD0u, 1, 0, 0);
            break;
          }
        }

      if ( (unsigned __int8)CParty::checkValidUser(*(CUser ***)this, jj) )
      {
        v132 = (CUser *)CParty::get_user(*(CParty **)this, jj);
        if ( v132 )
        {
          v67 = CUser::getHackAnalyzer(v132);
          WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)v67, v132, 0xDEu, 1, 0, 0);
        }
      }

int __cdecl CParty::get_random_buff_type(CParty *this)
{
  return *((_DWORD *)this + 1722);
}

    if ( (unsigned __int8)CParty::is_quick_party(Party) )
    {
      std::vector<stConditionEventRewardInfo>::vector(v7);
      CConditionEventManager::GetConditionRewardInfo((int)this, 1, (int *)v7, CurEventIndex, a3);
      IsDoubleGoldCardProcess = CConditionEventManager::IsDoubleGoldCardProcess((int)this, v7);
      std::vector<stConditionEventRewardInfo>::~vector(v7);
    }

    if ( !CParty::get_user(a4, i) || (unsigned __int8)CParty::checkValidUser((CUser **)a4, i) != 1 )
    {
      InterfacePacketBuf::put_byte(this, 0);
      InterfacePacketBuf::put_int(this, 0);
    }

    user = (CUserCharacInfo *)CParty::get_user(a2, i);
    if ( user && (unsigned __int8)CParty::checkValidUser((CUser **)a2, i) == 1 )

  member_count = CParty::get_member_count((CUser **)a2);
  *((_DWORD *)this + 626) = member_count;

      CParty::GetLuckPoint(a7, (CLuckPoint *)&v51, 0);
    rand_int = get_rand_int(STATIC_POOL_SIZE_1000);

            user = (CLuckPoint *)CParty::get_user((CParty *)a7, a2);
            CLuckPoint::UseLuckPoint(user, v23, rarity, grade);

        CParty::set_basic_reward((CParty *)a7, 0xBu, v53, index, (int)v52);
        v11 = __gnu_cxx::__normal_iterator<Inven_Item *,std::vector<Inven_Item>>::operator*((int)&v55);

  user = (CUserCharacInfo *)CParty::get_user(a5, a2);
  v35 = 0;

  AvatarBonus = CParty::getAvatarBonus((CUser **)a5);
  CreatureBonus = CParty::getCreatureBonus(a5, user);

  v33 = CParty::CalcurateManageBonus((int)a5, 0, user, (int)v35);
  ClearRewardBonusExp = CParty::getClearRewardBonusExp(

    if ( (unsigned __int8)CParty::checkValidUser(*(CUser ***)this, i) )
    {
      v2 = *((_DWORD *)this + 1);
      user = (CUser *)CParty::get_user(*(CParty **)this, i);
      CUser::setDeathTowerIndex(user, v2);
    }

      if ( !CParty::get_member_count(*(CUser ***)a2) )
      {
LABEL_7:
        WongWork::CDeathTower::_destroy(a2);
        return 1;
      }

        user = (CUser *)CParty::get_user(*(CParty **)a2, i);
        v5 = !user || (unsigned __int8)CParty::checkValidUser(*(CUser ***)a2, i) != 1;

      member_count = CParty::get_member_count(*(CUser ***)a2);
      if ( v53 && member_count > 0 )

        CParty::send_to_party(*(CUser ***)a2, (PacketGuard *)v50);
        WongWork::CDeathTower::CPacketHandler::~CPacketHandler(v50);

        v59 = (CUser *)CParty::get_user(*(CParty **)a2, k);
        if ( v59 && (unsigned __int8)CParty::checkValidUser(*(CUser ***)a2, k) == 1 )

        v62 = (CUserCharacInfo *)CParty::get_user(*(CParty **)a2, m);
        if ( v62 && (unsigned __int8)CParty::checkValidUser(*(CUser ***)a2, m) == 1 )

      CParty::send_to_party(*(CUser ***)a2, (PacketGuard *)v48);
      for ( n = 0; n <= 3; ++n )

        v65 = (CUserCharacInfo *)CParty::get_user(*(CParty **)a2, n);
        if ( v65 && (unsigned __int8)CParty::checkValidUser(*(CUser ***)a2, n) == 1 )

      CParty::SetEPLPState(*(CParty **)a2, 2);
      v35 = WongWork::CDeathTower::_genTimerKey((int)a2, 77);

      if ( (unsigned __int8)CParty::GetEPLPState(*(CParty **)a2) != 2 )
        return 0;

      if ( (unsigned __int8)CParty::GetSelectedEPLPCmd(*(CParty **)a2) == 3 )
        return 0;

      if ( (unsigned __int8)CParty::GetEPLPState(*(CParty **)a2) != 3 )
        return 0;

      if ( CParty::get_member_count(*(CUser ***)a2) <= 0 )
        goto LABEL_7;

    MemberNames = CParty::getMemberNames(*this, &v17);
    Dungeon = (std::string *)WongWork::CDeathTower::CDungeonMgr::getDungeon((WongWork::CDeathTower::CDungeonMgr *)(this + 3));

  if ( !*this || CParty::get_member_count(*this) <= 0 )
    WongWork::CDeathTower::_destroy((WongWork::CDeathTower *)this);

  if ( !CParty::CheckEnterAncientDungeon(*this, Dungeon, a2, (Inven_Item *)s, v8) )
    return 17;

  return (unsigned __int8)CParty::UseAncientDungeonItems((CParty *)*this, v6, (Inven_Item *)s, v8) != 1;
}

//----- (0846398E) --------------------------------------------------------
int __cdecl WongWork::CDeathTower::handleStartGame(CUserCharacInfo ***a1, int id, int value, int count)
{

    user = (CUserCharacInfo *)CParty::get_user((CParty *)*a1, i);
    if ( user && (unsigned __int8)CParty::checkValidUser(*a1, i) == 1 )

  CParty::gen_timer_key((int)*a1, 45);
  CParty::SetSelectedEPLPCmd((CParty *)*a1, -1);

    random_buff_type = CParty::get_random_buff_type((CParty *)*a1);
    InterfacePacketBuf::put_byte(v20, random_buff_type);

  CParty::send_to_party(*a1, (PacketGuard *)v20);
  InterfacePacketBuf::clear(v20);

  CParty::send_to_party(*a1, (PacketGuard *)v20);
  WongWork::CDeathTower::_checkMemberReady(a1);

    member_count = CParty::get_member_count(*this);
    if ( member_count <= 0 )

    CParty::send_to_party(*this, (PacketGuard *)v36);
    v5 = 1;

      CParty::send_to_party(*this, (PacketGuard *)v12);
      InterfacePacketBuf::clear(v12);

            member_count = CParty::get_member_count(*a2);
            for ( j = 0; j < member_count; ++j )

        user = (CUserCharacInfo *)CParty::get_user((CParty *)*a2, k);
        v26 = !user || (unsigned __int8)CParty::checkValidUser(*a2, k) != 1;

          MonsterTotalExp = CParty::getMonsterTotalExp((CParty *)*a2, MonsterTotalExp, 0, 0, 1.0, 1.0, 0);
          QuickParty::RewardData_Param::RewardData_Param((QuickParty::RewardData_Param *)v76, (CParty *)*a2);

          CParty::set_random_reward((CParty *)*a2, 0, v76[0], v76[1], (int)v75);
          MonsterTotalExp = v75[0];

          MonsterTotalExp = CParty::sumGainedEachExp((CParty *)*a2, user, (const STGainedEachExpData *)v74);
          if ( PvPWinPoint )

      CParty::send_to_party(*a2, (PacketGuard *)v86);
      if ( (unsigned __int8)WongWork::CDeathTower::CStage::checkClearStage((WongWork::CDeathTower::CStage *)(a2 + 5)) )

    if ( (unsigned __int8)CParty::checkValidUser(*this, i) && (CUser *)CParty::get_user((CParty *)*this, i) == a2 )
    {
      if ( (unsigned __int8)WongWork::CDeathTower::CPlayData::isMemberAlive(
                              (WongWork::CDeathTower::CPlayData *)(this + 68),
                              i) != 1 )
      {
        CUser::SendCmdErrorPacket(a2, 43, 18);
        return 0;
      }
      WongWork::CDeathTower::CPlayData::setMemberAlive((WongWork::CDeathTower::CPlayData *)(this + 68), i, 0);
      WongWork::CDeathTower::CPacketHandler::CPacketHandler((WongWork::CDeathTower::CPacketHandler *)v4);
      unique_id = CUser::get_unique_id(a2);
      WongWork::CDeathTower::CPacketHandler::makeDieState(v4, unique_id);
      CParty::send_to_party(*this, (PacketGuard *)v4);
      WongWork::CDeathTower::CPacketHandler::~CPacketHandler(v4);
      break;
    }

    if ( (unsigned __int8)CParty::checkValidUser((CUser **)*this, i)
      && (CUser *)CParty::get_user((CParty *)*this, i) == a3 )

      CParty::leave_user(*this, (std::string *)a3, 2);
      WongWork::CDeathTower::CPlayData::setMemberAlive((WongWork::CDeathTower::CPlayData *)(this + 68), i, 0);

  if ( CParty::get_member_count((CUser **)*this) > 0 && WongWork::CDeathTower::_checkMemberDie((CUser ***)this) )
  {
    WongWork::CDeathTower::_onFinishStage((CParty **)this);
    WongWork::CDeathTower::_onPrepareFinishTower((WongWork::CDeathTower *)this);
  }

    CParty::send_to_party(*this, (PacketGuard *)v10);
    v1 = 0;

      CParty::send_to_party(*this, (PacketGuard *)v8);
      WongWork::CDeathTower::_checkMemberReady(this);

      CParty::send_to_party(*this, (PacketGuard *)v7);
      v1 = 0;

    CParty::send_to_party(*this, (PacketGuard *)v9);
    v1 = 0;

  MemberSlotNo = CParty::GetMemberSlotNo(*this, a2);
  if ( MemberSlotNo == -1 || !(unsigned __int8)CParty::checkValidUser(*this, MemberSlotNo) )

  member_count = CParty::get_member_count(*this);
  if ( (unsigned __int8)WongWork::CDeathTower::CPlayData::checkAllMemberReady(

    CParty::send_to_party(*this, (PacketGuard *)v5);
    WongWork::CDeathTower::CPacketHandler::~CPacketHandler(v5);

  if ( (CUser *)CParty::getManager(*this) != a2 )
    return 0;

  MemberSlotNo = CParty::GetMemberSlotNo(*this, a2);
  if ( MemberSlotNo == -1 || !(unsigned __int8)CParty::checkValidUser(*this, MemberSlotNo) )

  CParty::SetEPLPState((CParty *)*this, 3);
  v4 = WongWork::CDeathTower::_genTimerKey((int)this, 78);

    user = (CUser *)CParty::get_user(*this, i);
    if ( user && (unsigned __int8)CParty::checkValidUser((CUser **)*this, i) == 1 )

        user = (CUserCharacInfo *)CParty::get_user(*this, i);
        if ( user

          && (unsigned __int8)CParty::checkValidUser((CUser **)*this, i) == 1
          && (unsigned __int8)WongWork::CDeathTower::CPlayData::isMemberAlive(

        v24 = (CUser *)CParty::get_user(*this, j);
        v14 = !v24 || (unsigned __int8)CParty::checkValidUser((CUser **)*this, j) != 1;

  if ( CParty::get_member_count(*this) == 1 )
    v1 = CONST_SINGLE_MAP_LOADING_WAIT_TIMEOUT;

    if ( (unsigned __int8)CParty::checkValidUser(*this, i)
      && (unsigned __int8)WongWork::CDeathTower::CPlayData::isMemberAlive(

      CParty::SetEPLPState(*this, 2);
      v2 = WongWork::CDeathTower::_genTimerKey((int)this, 77);

      CParty::send_to_party((CUser **)*this, (PacketGuard *)v71);
      PacketGuard::~PacketGuard(v71);

        user = (CUserCharacInfo *)CParty::get_user(*this, i);
        if ( user && (unsigned __int8)CParty::checkValidUser((CUser **)*this, i) == 1 )

          CParty::set_random_reward(*this, 0, v67[0], v67[1], (int)v66);
          v68 = v66[0];

  CParty::getMemberNames(*this, v10);
  for ( i = 0; ; ++i )

    user = (CUser *)CParty::get_user((CParty *)*this, i);
    if ( user && (unsigned __int8)CParty::checkValidUser(*this, i) == 1 )

    user = (CUser *)CParty::get_user(*this, i);
    if ( user && (unsigned __int8)CParty::checkValidUser((CUser **)*this, i) == 1 )

            CParty::Reset_party_overlapped_drop_ratio(Party);
          CInventory::GetInvenSlot((CInventory *)v17, (int *)CurCharacInvenW, 0, 10);

      LOBYTE(Party) = CParty::set_single_play(v9, v5);
    }
  }
  if ( v9 )
  {
    v6 = (CUser *)game_master::CCommand::GetUser(this);
    v10 = CParty::OnFightVillageMonster(a1, (CUser **)v9, v6, 0);
    LOBYTE(Party) = v10 != 0;
    if ( v10 )
    {

    return CParty::setStraightVictories(Party, *((_WORD *)this + 4));
  else

          CParty::send_to_party((CUser **)Party, (PacketGuard *)v6);
          CParty::send_to_party((CUser **)Party, (PacketGuard *)v5);

          return CParty::drop_item_gm((CUser **)result, v5, *((_DWORD *)this + 2));
      }
    }
  }
  return result;
}

//----- (084ABDB4) --------------------------------------------------------
void *__cdecl game_master::CDeleteAccountCargo::execute(game_master::CDeleteAccountCargo *this)
{
  CAccountCargo *User; // eax

  User = (CAccountCargo *)game_master::CCommand::GetUser(this);
  return CAccountCargo::DeleteAccountCargo(User);
}

//----- (084ABDD0) --------------------------------------------------------
void __cdecl game_master::CLinkSystem::execute(game_master::CLinkSystem *this)
{
  char *v1; // eax
  char flag; // al
  CDataManager *v3; // eax
  GameWorld *v4; // eax
  CDataManager *v5; // eax
  CDataManager *v6; // eax
  GameWorld *v7; // eax
  int id; // [esp+18h] [ebp-10h] BYREF
  int id; // [esp+1Eh] [ebp-Ah] BYREF

  std::allocator<char>::allocator();
  std::string::string(&v8, "gm Link Charac : ");
  std::allocator<char>::~allocator();
  trim((std::string *)(*((_DWORD *)this + 1) + 12));
  v1 = std::string::c_str((std::string *)(*((_DWORD *)this + 1) + 12));
  v2 = atoi(v1);
  BYTE1(v9) = v2;
  if ( v2 )
  {
    if ( v2 == 1 )
    {
      v5 = G_CDataManager();
      if ( (unsigned __int8)ServerParameterScript::isDungeonOpen((CDataManager *)((char *)v5 + 104), 11007, &v9) != 1 )
      {

        PassedMapInfoList = CParty::getPassedMapInfoList(Party);
        std::vector<MapInfo>::clear((MapInfo **)PassedMapInfoList);

        CParty::setDungeonMapSaving(Party, 1);
      }
      else if ( v2 )
      {

          CParty::setDungeonMapSaving(Party, 0);
      }
    }
    else
    {
      game_master::CBoosterTest::SendChatMsg(User, (CUser *)&unk_8C7FD9C);
    }
  }
}

//----- (084ACF98) --------------------------------------------------------
void __cdecl game_master::CMonsterDropTest::execute(game_master::CMonsterDropTest *this)
{
  char *v1; // edx
  unsigned int id; // ebx
  char *v3; // edi
  char *v4; // edx
  int id; // eax
  int id; // eax
  char *v7; // edx
  unsigned int id; // ebx
  char *v9; // edi
  char *v10; // edx
  int id; // eax
  int id; // eax
  int id; // esi
  int id; // ebx
  _DWORD *v15; // eax
  char *v16; // edx
  unsigned int id; // ebx
  char *v18; // edi
  char *v19; // edx
  int id; // eax
  int id; // eax
  int id; // esi
  int id; // ebx
  _DWORD *v24; // eax
  char *v25; // eax
  char dest[95000]; // [esp+29h] [ebp-178AFh] BYREF
  char flag; // [esp+17341h] [ebp-597h] BYREF
  _BYTE v28[1023]; // [esp+17342h] [ebp-596h] BYREF
  char flag; // [esp+17741h] [ebp-197h] BYREF
  __int16 v30; // [esp+17742h] [ebp-196h] BYREF
  int id; // [esp+17840h] [ebp-98h] BYREF
  int j; // [esp+17844h] [ebp-94h] BYREF
  int id; // [esp+17848h] [ebp-90h] BYREF
  int i; // [esp+1784Ch] [ebp-8Ch] BYREF
  std::_Rb_tree_node_base *v35; // [esp+17850h] [ebp-88h] BYREF
  std::string v36; // [esp+17854h] [ebp-84h] BYREF
  _DWORD value[6]; // [esp+17858h] [ebp-80h] BYREF
  int id[6]; // [esp+17870h] [ebp-68h] BYREF
  char s[21]; // [esp+1788Bh] [ebp-4Dh] BYREF
  int id; // [esp+178A0h] [ebp-38h] BYREF
  int id; // [esp+178A4h] [ebp-34h] BYREF
  int id; // [esp+178A8h] [ebp-30h] BYREF
  char flag[4]; // [esp+178ACh] [ebp-2Ch] BYREF
  game_master::CBoosterTest *User; // [esp+178B0h] [ebp-28h]
  CParty *Party; // [esp+178B4h] [ebp-24h]
  CUser *user; // [esp+178B8h] [ebp-20h]
  unsigned __int8 v47; // [esp+178BFh] [ebp-19h]

  User = (game_master::CBoosterTest *)game_master::CCommand::GetUser(this);
  if ( !User )
    return;
  Party = (CParty *)CUser::GetParty(User);
  if ( !Party )
    return;
  v46 = (CUser *)game_master::CMonsterDropTest::checkCommandExecuteError(this, Party);
  if ( v46 )
  {
    game_master::CBoosterTest::SendChatMsg(User, v46);
    return;
  }
  game_master::CBoosterTest::SendChatMsg(User, (CUser *)&unk_8C7FDB0);
  if ( !*((_DWORD *)this + 3) )
    *((_DWORD *)this + 3) = MAX_LIMIT_10000;
  sprintf(s, aD_37, *((_DWORD *)this + 3));
  game_master::CBoosterTest::SendChatMsg(User, (CUser *)s);
  v1 = &v29;
  v2 = 255;
  if ( ((unsigned __int8)&v29 & 1) != 0 )
  {
    v29 = 0;
    v1 = (char *)&v30;
    v2 = 254;
  }
  if ( ((unsigned __int8)v1 & 2) != 0 )
  {
    *(_WORD *)v1 = 0;
    v1 += 2;
    v2 -= 2;
  }
  memset(v1, 0, 4 * (v2 >> 2));
  v3 = &v1[4 * (v2 >> 2)];
  v4 = v3;
  if ( (v2 & 2) != 0 )
  {
    *(_WORD *)v3 = 0;
    v4 = v3 + 2;
  }
  if ( (v2 & 1) != 0 )
    *v4 = 0;
  memset(dest, 0, sizeof(dest));
  std::map<int,int>::map(v38);
  std::map<int,std::map<int,int>>::map(v37);
  game_master::CMonsterDropTest::processDungeonDrop((int)this, Party, (int)v38, (int)v37);
  v47 = 1;
  v5 = *((_DWORD *)this + 2);
  switch ( v5 )
  {
    case 2:
      goto LABEL_18;
    case 3:
      memcpy(dest, "item_name\tgold_quantity\t drop_cnt\n", 0x23u);
      break;
    case 0:
LABEL_18:
      memcpy(dest, "item_id\t item_name\t rarity\t drop_cnt\t price\t sell_price\t usable_lv\n", 0x44u);
      break;
  }
  while ( v47 )
  {
    v6 = *((_DWORD *)this + 2);
    switch ( v6 )
    {
      case 2:
        sprintf(&v29, "passed_dungeon_drop_%d.txt", *((_DWORD *)this + 3));
        game_master::CBoosterTest::makeFileContents(User, dest, (int)v38);
        break;
      case 3:
        sprintf(&v29, "gold_dungeon_drop_%d.txt", *((_DWORD *)this + 3));
        game_master::CMonsterDropTest::makeFileGoldContents(User, dest, (int)v38);
        break;
      case 0:
        sprintf(&v29, "single_dungeon_drop_%d.txt", *((_DWORD *)this + 3));
        game_master::CBoosterTest::makeFileContents(User, dest, (int)v38);
        break;
    }
    if ( std::map<int,int>::empty((int)v38) )
      v47 = 0;
    game_master::CBoosterTest::printClient(User, (CUser *)v47, &v29, dest);
    memset(dest, 0, sizeof(dest));
  }
  if ( std::map<int,std::map<int,int>>::size((int)v37) )
  {
    std::string::string(&v36);
    std::string::clear(&v36);
    std::map<int,std::map<int,int>>::begin(&v35, (int)v37);
    while ( 1 )
    {
      std::map<int,std::map<int,int>>::end(&v40, (int)v37);
      if ( !std::_Rb_tree_iterator<std::pair<int const,std::map<int,int>>>::operator!=(&v35, &v40) )
        break;
      for ( i = 0; i <= 10; ++i )
      {

    PassedMapInfoList = CParty::getPassedMapInfoList(a2);
    std::vector<MapInfo>::begin(&v11, PassedMapInfoList);

      v9 = CParty::getPassedMapInfoList(a2);
      std::vector<MapInfo>::end(&v14, (int)v9);

    if ( (unsigned __int8)CParty::checkValidUser((CUser **)a2, i) == 1 )
    {
      Member = CParty::GetMember(a2);
      if ( CUser::isAffectedPremium(*(_DWORD *)&Member[24 * i], 79) )
        ++v23;
    }

      if ( (unsigned __int8)CParty::checkValidUser((CUser **)a2, j) == 1 )
      {
        v12 = v23;
        v13 = CParty::GetMember(a2);
        AddIndependentDropRateFromPremium = CUser::getAddIndependentDropRateFromPremium(
                                              *(_DWORD *)&v13[24 * j],
                                              0x4Fu,
                                              v12);
        break;
      }

  member_count = CParty::get_member_count((CUser **)a2);
  CBattle_Field::_MakeDropItems(

      DungIndex = (unsigned __int16)CParty::getDungIndex(Party);
      v2 = (CDungeonList **)G_CDataManager();

              standard_level = CParty::getStandardDimensionLevel(Party);
            WongWork::stGenerateRefData_t::stGenerateRefData_t((WongWork::stGenerateRefData_t *)v24);

              CParty::send_party_realtime_info((CUser **)Party);
          }
          CUserCharacInfo::set_charac_memberkey(a2, *(_DWORD *)(v7 + 17));
        }
        PacketGuard::PacketGuard((PacketGuard *)v6);
        InterfacePacketBuf::put_header(v6, 1, 80);
        InterfacePacketBuf::put_byte(v6, 1);
        InterfacePacketBuf::put_byte(v6, v7[11]);
        InterfacePacketBuf::put_byte(v6, v7[21]);
        v8 = strlen(v7 + 22);
        InterfacePacketBuf::put_int(v6, v8);
        InterfacePacketBuf::put_str(v6, v7 + 22, v8);
        InterfacePacketBuf::finalize(v6, 1);
        CUser::Send(a2, (PacketGuard *)v6);
        PacketGuard::~PacketGuard(v6);
      }
    }
    else
    {
      v3 = *(_DWORD *)(v7 + 17);
      charac_no = (const char *)CUser::get_charac_no(a2, -1);
      LogManager::logFormat(
        (LogManager *)1,
        (int)"InterDispatcher.cpp",
        "virtual int Inter_MonitorNoticeMemberEnterReply::dispatch_sig(CUser*, char*, int)",
        (const char *)0x28D9,
        "MEMBER : Inter_MonitorNoticeMemberEnterReply::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
        charac_no,
        v3);
    }
  }
  return 0;
}

//----- (084CD83A) --------------------------------------------------------
int __cdecl Inter_MonitorNoticeMemberSecede::dispatch_sig(Inter_MonitorNoticeMemberSecede *this, CUser *user, char *a3)
{
  int id; // ebx
  const char *charac_no; // eax
  PacketBuf *v6[3]; // [esp+28h] [ebp-20h] BYREF
  char *v7; // [esp+34h] [ebp-14h]
  int id; // [esp+38h] [ebp-10h]
  CParty *Party; // [esp+3Ch] [ebp-Ch]

  if ( CUser::get_state(a2) )
  {
    v7 = a3;
    if ( CUser::get_charac_no(a2, -1) == *(_DWORD *)(a3 + 14) )
    {
      if ( v7[18] == 1 )
      {
        CUserCharacInfo::set_connect_upper_member(a2, 0);
        CUserCharacInfo::set_uppermember_exp_level(a2, 0);
        Party = (CParty *)CUser::GetParty(a2);
        if ( Party )
          CParty::send_party_realtime_info((CUser **)Party);
      }
      PacketGuard::PacketGuard((PacketGuard *)v6);
      InterfacePacketBuf::put_header(v6, 1, 81);
      if ( v7[18] == 1 || v7[18] == 2 )
        InterfacePacketBuf::put_byte(v6, 1);
      else
        InterfacePacketBuf::put_byte(v6, 0);
      v8 = strlen(v7 + 19);
      InterfacePacketBuf::put_int(v6, v8);
      InterfacePacketBuf::put_str(v6, v7 + 19, v8);
      if ( v7[18] == 1 || v7[18] == 2 )
        InterfacePacketBuf::put_byte(v6, 1);
      else
        InterfacePacketBuf::put_byte(v6, v7[18]);
      InterfacePacketBuf::finalize(v6, 1);
      CUser::Send(a2, (PacketGuard *)v6);
      PacketGuard::~PacketGuard(v6);
    }
    else
    {
      v3 = *(_DWORD *)(v7 + 14);
      charac_no = (const char *)CUser::get_charac_no(a2, -1);
      LogManager::logFormat(
        (LogManager *)1,
        (int)"InterDispatcher.cpp",
        "virtual int Inter_MonitorNoticeMemberSecede::dispatch_sig(CUser*, char*, int)",
        (const char *)0x2914,
        "MEMBER : Inter_MonitorNoticeMemberSecede::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCallerCharID(%d)",
        charac_no,
        v3);
    }
  }
  return 0;
}

//----- (084CDA74) --------------------------------------------------------
int __cdecl Inter_MonitorNoticeMemberSecedeToSeceder::dispatch_sig(
        Inter_MonitorNoticeMemberSecedeToSeceder *this,
        CUser *user,
        char *a3)
{
  int id; // ebx
  const char *charac_no; // eax
  PacketBuf *v6[3]; // [esp+28h] [ebp-20h] BYREF
  char *v7; // [esp+34h] [ebp-14h]
  int id; // [esp+38h] [ebp-10h]
  CParty *Party; // [esp+3Ch] [ebp-Ch]

  if ( CUser::get_state(a2) )
  {
    v7 = a3;
    if ( CUser::get_charac_no(a2, -1) == *(_DWORD *)(a3 + 14) )
    {
      if ( v7[18] == 1 )
      {
        CUserCharacInfo::set_connect_upper_member(a2, 0);
        CUserCharacInfo::set_uppermember_exp_level(a2, 0);
        Party = (CParty *)CUser::GetParty(a2);
        if ( Party )
          CParty::send_party_realtime_info((CUser **)Party);
      }
      PacketGuard::PacketGuard((PacketGuard *)v6);
      InterfacePacketBuf::put_header(v6, 0, 77);
      v8 = strlen(v7 + 19);
      InterfacePacketBuf::put_int(v6, v8);
      InterfacePacketBuf::put_str(v6, v7 + 19, v8);
      InterfacePacketBuf::finalize(v6, 1);
      CUser::Send(a2, (PacketGuard *)v6);
      PacketGuard::~PacketGuard(v6);
    }
    else
    {
      v3 = *(_DWORD *)(v7 + 14);
      charac_no = (const char *)CUser::get_charac_no(a2, -1);
      LogManager::logFormat(
        (LogManager *)1,
        (int)"InterDispatcher.cpp",
        "virtual int Inter_MonitorNoticeMemberSecedeToSeceder::dispatch_sig(CUser*, char*, int)",
        (const char *)0x2947,
        "MEMBER : Inter_MonitorNoticeMemberSecede::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uSecededCharID(%d)",
        charac_no,
        v3);
    }
  }
  return 0;
}

//----- (084CDC2C) --------------------------------------------------------
int __cdecl Inter_MonitorNoticeDeleteMemberId::dispatch_sig(
        Inter_MonitorNoticeDeleteMemberId *this,
        CUser *user,
        char *a3)
{
  int id; // ebx
  const char *charac_no; // eax
  PacketBuf *v6[3]; // [esp+2Ch] [ebp-1Ch] BYREF
  char *v7; // [esp+38h] [ebp-10h]
  CParty *Party; // [esp+3Ch] [ebp-Ch]

  if ( CUser::get_state(a2) )
  {
    v7 = a3;
    if ( CUser::get_charac_no(a2, -1) == *(_DWORD *)(a3 + 14) )
    {
      CUserCharacInfo::set_connect_upper_member(a2, 0);
      CUserCharacInfo::set_uppermember_exp_level(a2, 0);
      CUserCharacInfo::set_charac_memberkey(a2, 0);
      Party = (CParty *)CUser::GetParty(a2);
      if ( Party )
        CParty::send_party_realtime_info((CUser **)Party);
      PacketGuard::PacketGuard((PacketGuard *)v6);
      InterfacePacketBuf::put_header(v6, 0, 75);
      InterfacePacketBuf::finalize(v6, 1);
      CUser::Send(a2, (PacketGuard *)v6);
      PacketGuard::~PacketGuard(v6);
    }
    else
    {
      v3 = *(_DWORD *)(v7 + 14);
      charac_no = (const char *)CUser::get_charac_no(a2, -1);
      LogManager::logFormat(
        (LogManager *)1,
        (int)"InterDispatcher.cpp",
        "virtual int Inter_MonitorNoticeDeleteMemberId::dispatch_sig(CUser*, char*, int)",
        (const char *)0x296D,
        "MEMBER : Inter_MonitorNoticeDeleteMemberId::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
        charac_no,
        v3);
    }
  }
  return 0;
}

//----- (084CDDAC) --------------------------------------------------------
int __cdecl Inter_MonitorCallMemberList::dispatch_sig(Inter_MonitorCallMemberList *this, CUser *user, char *a3)
{
  const char *v3; // eax
  int id; // ebx
  const char *charac_no; // eax
  PacketBuf *v7[3]; // [esp+2Ch] [ebp-2Ch] BYREF
  char *v8; // [esp+38h] [ebp-20h]
  char *v9; // [esp+3Ch] [ebp-1Ch]
  int id; // [esp+40h] [ebp-18h]
  char *v11; // [esp+44h] [ebp-14h]
  int i; // [esp+48h] [ebp-10h]
  int id; // [esp+4Ch] [ebp-Ch]

  if ( CUser::get_state(a2) )
  {
    if ( CUserCharacInfo::get_charac_memberkey(a2) )
    {
      v8 = a3;
      if ( CUser::get_charac_no(a2, -1) == *(_DWORD *)(a3 + 14) )
      {
        PacketGuard::PacketGuard((PacketGuard *)v7);
        InterfacePacketBuf::put_header(v7, 1, 82);
        InterfacePacketBuf::put_byte(v7, 1);
        v9 = v8 + 18;
        InterfacePacketBuf::put_byte(v7, v8[18]);
        InterfacePacketBuf::put_byte(v7, v9[1]);
        v10 = strlen(v9 + 2);
        InterfacePacketBuf::put_int(v7, v10);
        InterfacePacketBuf::put_str(v7, v9 + 2, v10);
        InterfacePacketBuf::put_byte(v7, v9[32]);
        InterfacePacketBuf::put_byte(v7, v9[33]);
        InterfacePacketBuf::put_int(v7, *(_DWORD *)(v9 + 34));
        InterfacePacketBuf::put_int(v7, *(_DWORD *)(v9 + 38));
        v11 = v9 + 43;
        InterfacePacketBuf::put_byte(v7, v9[42]);
        for ( i = 0; (unsigned __int8)v9[42] > i; ++i )
        {
          InterfacePacketBuf::put_byte(v7, v11[42 * i]);
          InterfacePacketBuf::put_byte(v7, v11[42 * i + 1]);
          v13 = strlen(&v11[42 * i + 2]);
          InterfacePacketBuf::put_int(v7, v13);
          InterfacePacketBuf::put_str(v7, &v11[42 * i + 2], v13);
          InterfacePacketBuf::put_byte(v7, v11[42 * i + 32]);
          InterfacePacketBuf::put_byte(v7, v11[42 * i + 33]);
          InterfacePacketBuf::put_int(v7, *(_DWORD *)&v11[42 * i + 34]);
          InterfacePacketBuf::put_int(v7, *(_DWORD *)&v11[42 * i + 38]);
        }
        InterfacePacketBuf::finalize(v7, 1);
        CUser::Send(a2, (PacketGuard *)v7);
        PacketGuard::~PacketGuard(v7);
      }
      else
      {
        v4 = *(_DWORD *)(v8 + 14);
        charac_no = (const char *)CUser::get_charac_no(a2, -1);
        LogManager::logFormat(
          (LogManager *)1,
          (int)"InterDispatcher.cpp",
          "virtual int Inter_MonitorCallMemberList::dispatch_sig(CUser*, char*, int)",
          (const char *)0x2993,
          "MEMBER : Inter_MonitorCallMemberList::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
          charac_no,
          v4);
      }
    }
    else
    {
      v3 = (const char *)CUser::get_charac_no(a2, -1);
      LogManager::logFormat(
        (LogManager *)1,
        (int)"InterDispatcher.cpp",
        "virtual int Inter_MonitorCallMemberList::dispatch_sig(CUser*, char*, int)",
        (const char *)0x298C,
        "MEMBER : Inter_MonitorCallMemberList::dispatch_sig() : 0 == pUser->get_charac_memberkey() (Char id : %d)",
        v3);
    }
  }
  return 0;
}

//----- (084CE164) --------------------------------------------------------
int __cdecl Inter_MonitorPayTaxToUpper::dispatch_sig(Inter_MonitorPayTaxToUpper *this, CUser *user, char *a3)
{
  const char *v4; // eax
  int id; // ebx
  const char *charac_no; // eax
  unsigned int id; // ebx
  int CurCharacInvenW; // eax
  PacketBuf *v9[3]; // [esp+28h] [ebp-50h] BYREF
  PacketBuf *v10[3]; // [esp+34h] [ebp-44h] BYREF
  int id; // [esp+40h] [ebp-38h] BYREF
  int id; // [esp+44h] [ebp-34h] BYREF
  int id; // [esp+48h] [ebp-30h] BYREF
  int id; // [esp+4Ch] [ebp-2Ch] BYREF
  char *v15; // [esp+50h] [ebp-28h]
  int id; // [esp+54h] [ebp-24h]
  char flag; // [esp+5Bh] [ebp-1Dh]
  __int16 CurCharacMemberBonusFatigue; // [esp+5Ch] [ebp-1Ch]
  __int16 AccountMemberBonusFatigue; // [esp+5Eh] [ebp-1Ah]
  int id; // [esp+60h] [ebp-18h]
  int id; // [esp+64h] [ebp-14h]
  unsigned int id; // [esp+68h] [ebp-10h]
  int id; // [esp+6Ch] [ebp-Ch]

  if ( !CUser::get_state(a2) )
    return 0;
  v15 = a3;
  if ( CUserCharacInfo::get_charac_memberkey(a2) )
  {
    if ( CUser::get_charac_no(a2, -1) == *(_DWORD *)(v15 + 14) )
    {
      if ( *(_DWORD *)(v15 + 18) )
      {
        v7 = *(_DWORD *)(v15 + 18);
        CurCharacInvenW = CUserCharacInfo::getCurCharacInvenW(a2);
        CInventory::gain_money(CurCharacInvenW, v7, (char *)0xA, 1, 0);
      }
      v16 = 0;
      v17 = 0;
      if ( *(_DWORD *)(v15 + 22) )
      {
        CurCharacMemberBonusFatigue = CUserCharacInfo::getCurCharacMemberBonusFatigue(a2);
        AccountMemberBonusFatigue = CUser::getAccountMemberBonusFatigue(a2);
        v20 = *((_DWORD *)G_CDataManager() + 4669);
        v21 = *((_DWORD *)G_CDataManager() + 4670);
        v22 = 0;
        v13 = v20 - CurCharacMemberBonusFatigue;
        v12 = v21 - AccountMemberBonusFatigue;
        v11 = *std::min<int>(&v13, &v12);
        v14 = 0;
        v22 = *std::max<int>(&v14, &v11);
        if ( v22 )
        {
          if ( v22 <= *(_DWORD *)(v15 + 22) )
          {

          party_overlapped_drop_ratio_unique = CParty::Get_party_overlapped_drop_ratio_unique(Party);
          party_overlapped_drop_ratio_rare = CParty::Get_party_overlapped_drop_ratio_rare(Party);

int __cdecl CParty::Get_party_overlapped_drop_ratio_rare(CParty *this)
{
  return *((_DWORD *)this + 1563);
}

int __cdecl CParty::Get_party_overlapped_drop_ratio_unique(CParty *this)
{
  return *((_DWORD *)this + 1564);
}

            if ( (unsigned __int8)CParty::checkValidUser((CUser **)this, i) == 1 )
            {
              inserted = -1;
              CurCharacInvenW = (int *)CUserCharacInfo::getCurCharacInvenW(*((CUserCharacInfo **)this + 6 * i + 30));
              inserted = CInventory::insertItemIntoInventory(
                           CurCharacInvenW,
                           v12,
                           v13,
                           v14,
                           v15,
                           v16,
                           v17,
                           v18,
                           v19,
                           v20,
                           v21,
                           v22,
                           v23,
                           v24,
                           v25,
                           v26,
                           56,
                           1);
              if ( inserted >= 0 )
              {
                CUser::SendUpdateItem(*((CUserCharacInfo **)this + 6 * i + 30), 1, 0, inserted);
              }
              else
              {
                String = RDARScriptStringManager::findString(
                           (RDARScriptStringManager *)&g_scriptStringManager_,
                           (char *)4,
                           "game_server_msg_53",
                           0);
                strncpy(dest, String, SIZE_20_BYTES);
                v6 = RDARScriptStringManager::findString(
                       (RDARScriptStringManager *)&g_scriptStringManager_,
                       (char *)4,
                       "game_server_msg_154",
                       0);
                strncpy(s, v6, MASK_BYTE);
                ServerGroup = CUser::GetServerGroup(*((CUser **)this + 6 * i + 30));
                v8 = strlen(s);
                CurCharacNo = CUserCharacInfo::getCurCharacNo(*((CUserCharacInfo **)this + 6 * i + 30));
                WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                  dest,
                  (Inven_Item *)&v12,
                  0,
                  CurCharacNo,
                  s,
                  v8,
                  0,
                  ServerGroup,
                  0,
                  0);
                Inven_Item::reset((Inven_Item *)&v12);
              }
            }

char __cdecl CParty::increase_dimension_inout_count(CParty *this, int id)
{
  int id; // eax
  CDataManager *v3; // eax
  int i; // [esp+10h] [ebp-18h]
  int j; // [esp+14h] [ebp-14h]
  int id; // [esp+18h] [ebp-10h]
  int id; // [esp+1Ch] [ebp-Ch]

  v2 = *((_DWORD *)this + 811);
  if ( v2 )
  {
    LOBYTE(v2) = (unsigned __int8)CDungeon::get_dimension_possible(*((CDungeon **)this + 811)) == 0;
    if ( !(_BYTE)v2 )
    {
      for ( i = 0; ; ++i )
      {
        LOBYTE(v2) = i <= 3;
        if ( i > 3 )
          break;
        if ( (unsigned __int8)CParty::checkValidUser((CUser **)this, i) == 1 )
        {
          for ( j = 0; j <= 5; ++j )
          {
            v3 = G_CDataManager();
            v7 = a2 + (char)CDataManager::get_dimensionInout(v3, j);
            v8 = a2 + (char)CUserCharacInfo::getDemensionInoutValue(*((CUserCharacInfo **)this + 6 * i + 30), j);
            if ( v8 > v7 )
              LOBYTE(v8) = v7;
            CUserCharacInfo::setDemensionInoutValue(*((CUserCharacInfo **)this + 6 * i + 30), j, (char)v8);
          }
        }
      }
    }
  }
  return v2;
}

int __cdecl CParty::leave_user(std::string **a1, std::string *a2, int id)
{
  char *PartyTelePort; // eax
  unsigned int UserPosInParty; // eax
  int id; // edi
  char *MemberNames; // esi
  int dungeon_diff; // ebx
  char *DungeonName; // eax
  char *HackAnalyzer; // eax
  CGameManager *v12; // eax
  int CurCharacNo; // esi
  unsigned int acc_id; // eax
  char flag; // bl
  std::string *v16; // edi
  int UID; // ebx
  int state; // eax
  char *v19; // eax
  char *v20; // eax
  GameWorld *v21; // eax
  int id; // eax
  GameWorld *v23; // eax
  int id; // eax
  const char *v25; // [esp+14h] [ebp-164h]
  char s[256]; // [esp+44h] [ebp-134h] BYREF
  int id[4]; // [esp+144h] [ebp-34h] BYREF
  char *member_count; // [esp+154h] [ebp-24h]
  char *j; // [esp+158h] [ebp-20h]
  int i; // [esp+15Ch] [ebp-1Ch]

  if ( a2 )
  {
    PartyTelePort = CParty::GetPartyTelePort((CParty *)a1);
    CPartyTelePort::process_leave_user_at_teleport((CPartyTelePort *)PartyTelePort, (CUser *)a2);
    if ( secretshop::SECRET_SHOP_DATA::IsOpen((secretshop::SECRET_SHOP_DATA *)(a1 + 169)) )
    {
      UserPosInParty = CUser::GetUserPosInParty((CUser *)a2);
      secretshop::SECRET_SHOP_DATA::SetBuying((secretshop::SECRET_SHOP_DATA *)(a1 + 169), UserPosInParty, 0);
    }
    if ( a1[811] )
    {
      memset(s, 0, sizeof(s));
      v6 = (int)a1[822];
      MemberNames = CParty::_getMemberNames((CUserCharacInfo **)a1, s);
      dungeon_diff = CBattle_Field::get_dungeon_diff((CBattle_Field *)(a1 + 713));
      DungeonName = CDungeon::GetDungeonName(a1[811]);
      cUserHistoryLog::LeaveDungeon((CUser **)&a2[CUSER_OFFSET_HISTORY_LOG_ALT], DungeonName, dungeon_diff, MemberNames, v6);
      if ( (unsigned __int8)CParty::IsSinglePlay((CParty *)a1) && CUser::isDungeonClearHackCheck((CUser *)a2) )
      {
        HackAnalyzer = CUser::getHackAnalyzer((CUser *)a2);
        WongWork::CHackAnalyzer::addServerHackCnt(
          (WongWork::CHackAnalyzer *)HackAnalyzer,
          (CUserCharacInfo *)a2,
          0x19Cu,
          1,
          0,
          0);
      }
    }
    if ( CParty::get_quick_party_index((CParty *)a1) != -1 )
    {
      v12 = G_CGameManager();
      CGameManager::CheckOutQuickParty(v12, (CParty *)a1, 0);
    }
    if ( (unsigned __int8)CParty::is_quick_party((CParty *)a1) )
    {
      CParty::send_quick_party_matching_result((CUser **)a1, 0, (CUser *)a2);
      if ( CParty::get_member_count((CUser **)a1) == 2 )
        CParty::set_quick_party((CParty *)a1, 0);
    }
    member_count = (char *)CParty::get_member_count((CUser **)a1);
    if ( (int)member_count > 1 )
    {
      v19 = CUser::getHackAnalyzer((CUser *)a2);
      WongWork::CHackAnalyzer::reportHackInfo((WongWork::CHackAnalyzer *)v19);
      v20 = CUser::getHackAnalyzer((CUser *)a2);
      WongWork::CHackAnalyzer::resetHackInfo((WongWork::CHackAnalyzer *)v20);
      if ( a1[29] == a2 )
      {
        CParty::change_manager((pthread_mutex_t **)a1);
        v21 = G_GameWorld();
        GameWorld::send_party_info_to_all(v21, (CUser **)a1, a3);
        CParty::send_party_realtime_info((CUser **)a1);
        v22 = CParty::checkInoutConditionDungeon(a3, (CParty *)a1);
        CParty::sendInoutConditionDungeon((CUser **)a1, v22);
        return 0;
      }
      else
      {
        for ( i = 0; ; ++i )
        {
          if ( i > 3 )
            return 1;
          if ( (unsigned __int8)CParty::_checkValidUser((CUser **)a1, i) == 1 && a1[6 * i + 30] == a2 )
            break;
        }
        a1[6 * i + 30] = 0;
        a1[6 * i + 32] = 0;
        CUser::SetPartyIndex((pthread_mutex_t *)a2, -1);
        CUserCharacInfo::set_charac_party_bonus_exp((CUserCharacInfo *)a2, 0);
        CUserCharacInfo::set_charac_member_bonus_exp((CUserCharacInfo *)a2, 0);
        CUserCharacInfo::set_charac_fatigue_buf_bonus_exp((CUserCharacInfo *)a2, 0);
        CUserCharacInfo::set_charac_seria_buf_bonus_exp((CUserCharacInfo *)a2, 0);
        if ( CUser::getMoveSpace((CUser *)a2) != 1 )
          CUser::set_state((CUserCharacInfo *)a2, 3);
        v23 = G_GameWorld();
        GameWorld::send_party_info_to_all(v23, (CUser **)a1, a3);
        CParty::send_party_realtime_info((CUser **)a1);
        v24 = CParty::checkInoutConditionDungeon(a3, (CParty *)a1);
        CParty::sendInoutConditionDungeon((CUser **)a1, v24);
        return 0;
      }
    }
    else
    {
      CUser::GetPartyIndex((CUser *)a2);
      LOBYTE(CurCharacNo) = CUser::get_state((CUser *)a2);
      acc_id = CUser::get_acc_id((CUser *)a2);
      v15 = NumberToString(acc_id, 0);
      cMyTrace::cMyTrace((cMyTrace *)v27, "int CParty::leave_user(CUser*, ENUM_PARTY_INFO_TYPE)", 1464, 0);
      cMyTrace::operator()((int)v27, "<IN_DUNGEON_NULL_PARTY> m_id(%s), user_state(%d), PartyIndex(%d)", v15);
      if ( (int)member_count <= 0 )
      {
        LogManager::logFormat(
          (LogManager *)1,
          (int)"party.cpp",
          "int CParty::leave_user(CUser*, ENUM_PARTY_INFO_TYPE)",
          (const char *)0x5C5,
          "[PARTYPOOL] member_count : %d, m_pManager : %x, dungeon_idx : %u, m_iIndex : %u, cState : %d",
          member_count,
          a1[29],
          *((unsigned __int16 *)a1 + 159),
          a1[25],
          *((char *)a1 + 106));
        for ( j = 0; (int)j <= 3; ++j )
        {
          if ( a1[6 * (_DWORD)j + 30] )
          {
            CurCharacNo = CUserCharacInfo::getCurCharacNo((CUserCharacInfo *)a1[6 * (_DWORD)j + 30]);
            v16 = a1[6 * (_DWORD)j + 32];
            UID = CUser::GetUID((CUser *)a1[6 * (_DWORD)j + 30]);
            state = CUser::get_state((CUser *)a1[6 * (_DWORD)j + 30]);
            LogManager::logFormat(
              (LogManager *)1,
              (int)"party.cpp",
              "int CParty::leave_user(CUser*, ENUM_PARTY_INFO_TYPE)",
              (const char *)0x5D8,
              "User[%d] %x, ch_state %d, uid : %d / %d, charac_no : %u",
              j,
              a1[6 * (_DWORD)j + 30],
              state,
              UID,
              v16,
              CurCharacNo);
          }
          else
          {
            LogManager::logFormat(
              (LogManager *)1,
              (int)"party.cpp",
              "int CParty::leave_user(CUser*, ENUM_PARTY_INFO_TYPE)",
              (const char *)0x5CD,
              "User[%d] NULL, ch_state NONE, uid : %d, charac_no : 0",
              j,
              a1[6 * (_DWORD)j + 32]);
          }
        }
      }
      CUser::SetPartyIndex((pthread_mutex_t *)a2, -1);
      CUserCharacInfo::set_charac_party_bonus_exp((CUserCharacInfo *)a2, 0);
      CUserCharacInfo::set_charac_member_bonus_exp((CUserCharacInfo *)a2, 0);
      CUserCharacInfo::set_charac_fatigue_buf_bonus_exp((CUserCharacInfo *)a2, 0);
      CUserCharacInfo::set_charac_seria_buf_bonus_exp((CUserCharacInfo *)a2, 0);
      if ( CUser::getMoveSpace((CUser *)a2) != 1 )
        CUser::set_state((CUserCharacInfo *)a2, 3);
      CParty::destroy(CurCharacNo, (CUser **)a1);
      return 0;
    }
  }
  else
  {
    LogManager::logFormat(
      (LogManager *)1,
      (int)"party.cpp",
      "int CParty::leave_user(CUser*, ENUM_PARTY_INFO_TYPE)",
      (const char *)0x56D,
      "CParty::leave_user pUser is NULL",
      v25);
    return 1;
  }
}

int __cdecl CParty::change_manager(pthread_mutex_t **this)
{
  const char *v2; // [esp+14h] [ebp-24h]
  int i; // [esp+28h] [ebp-10h]
  int j; // [esp+2Ch] [ebp-Ch]

  for ( i = 0; i <= 3; ++i )
  {
    if ( *(this + 6 * i + 30) == *(this + 29) )
    {
      CUser::SetPartyIndex(*(this + 29), -1);
      CUser::set_state((CUserCharacInfo *)*(this + 29), 3);
      *(this + 6 * i + 30) = 0;
      *(this + 6 * i + 32) = 0;
      break;
    }
  }
  for ( j = 0; j <= 3; ++j )
  {
    if ( (unsigned __int8)CParty::_checkValidUser((CUser **)this, j) )
    {
      *(this + 29) = *(this + 6 * j + 30);
      CParty::set_host((CParty *)this);
      CParty::send_host_info((CUser **)this);
      return 1;
    }
  }
  LogManager::logFormat(
    (LogManager *)1,
    (int)"party.cpp",
    "bool CParty::change_manager()",
    (const char *)0x689,
    "there is no manager(party empty)",
    v2);
  return 0;
}

int __cdecl CParty::change_manager(CParty *this, CUser *user, unsigned int id)
{
  GameWorld *v4; // eax

  if ( a3 >= 4 )
    return 21;
  if ( !*((_DWORD *)this + 6 * a3 + 30) )
    return 21;
  if ( *((CUser **)this + 29) != a2 )
    return 8;
  if ( *((_DWORD *)this + 6 * a3 + 30) == *((_DWORD *)this + 29) )
    return 18;
  *((_DWORD *)this + 29) = *((_DWORD *)this + 6 * a3 + 30);
  CParty::set_host(this);
  CParty::send_host_info((CUser **)this);
  v4 = G_GameWorld();
  GameWorld::send_party_info_to_all(v4, (CUser **)this, 2u);
  return 0;
}

int __cdecl CParty::walkout_member(CUser **this, CUser *user, int id)
{
  pvp_assault::CAssaultMgr *InstanceAssaultMgr; // eax
  std::string *v5; // [esp+18h] [ebp-10h]
  int id; // [esp+1Ch] [ebp-Ch]

  if ( *(this + 29) != a2 )
    return 8;
  if ( !*(this + 6 * a3 + 30) )
    return 4;
  v5 = (std::string *)*(this + 6 * a3 + 30);
  v6 = CParty::leave_user((std::string **)this, v5, 2);
  if ( !v6 )
  {
    InstanceAssaultMgr = (pvp_assault::CAssaultMgr *)pvp_assault::GetInstanceAssaultMgr();
    pvp_assault::CAssaultMgr::ForcedMoveAtPowerWarPlace(InstanceAssaultMgr, (CUser *)v5);
    return 0;
  }
  if ( v6 == 1 )
    return 1;
  else
    return 4;
}

int __cdecl CParty::SendWalkout(CUser **a1, char flag, char type)
{
  PacketBuf *v4[5]; // [esp+14h] [ebp-14h] BYREF

  PacketGuard::PacketGuard((PacketGuard *)v4);
  InterfacePacketBuf::put_header(v4, 0, 10);
  InterfacePacketBuf::put_byte(v4, a2);
  InterfacePacketBuf::put_byte(v4, a3);
  InterfacePacketBuf::finalize(v4, 1);
  CParty::send_to_party(a1, (PacketGuard *)v4);
  PacketGuard::~PacketGuard(v4);
  return 1;
}

int __cdecl CParty::get_party_realtime_info(CUser **this, PacketBuf **a2)
{
  __int16 unique_id; // ax
  int CurCharacR; // eax
  char isHelpAbuseParty; // al
  int index; // [esp+14h] [ebp-14h] BYREF
  int id; // [esp+18h] [ebp-10h]
  int i; // [esp+1Ch] [ebp-Ch]

  index = InterfacePacketBuf::get_index(a2);
  v7 = 0;
  InterfacePacketBuf::put_byte(a2, 0);
  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::_checkValidUser(this, i) )
    {
      unique_id = CUser::get_unique_id(*(this + 6 * i + 30));
      InterfacePacketBuf::put_short(a2, unique_id);
      if ( CUserCharacInfo::getCurCharacR(*(this + 6 * i + 30)) )
      {
        CurCharacR = CUserCharacInfo::getCurCharacR(*(this + 6 * i + 30));
        InterfacePacketBuf::put_byte(a2, *(_BYTE *)(CurCharacR + 117));
      }
      else
      {
        InterfacePacketBuf::put_byte(a2, 0);
      }
      isHelpAbuseParty = CParty::isHelpAbuseParty();
      InterfacePacketBuf::put_byte(a2, isHelpAbuseParty);
      InterfacePacketBuf::put_byte(a2, i);
      ++v7;
    }
  }
  InterfacePacketBuf::put_byte(a2, &index, v7);
  return 1;
}

void __cdecl CParty::send_party_realtime_info(CUser **this)
{
  PacketBuf *v1[5]; // [esp+14h] [ebp-14h] BYREF

  PacketGuard::PacketGuard((PacketGuard *)v1);
  InterfacePacketBuf::put_header(v1, 0, 153);
  if ( (unsigned __int8)CParty::get_party_realtime_info(this, v1) )
  {
    InterfacePacketBuf::finalize(v1, 1);
    CParty::send_to_party(this, (PacketGuard *)v1);
  }
  PacketGuard::~PacketGuard(v1);
}

int __cdecl CParty::set_host(CParty *this)
{
  *((_DWORD *)this + 28) = *((_DWORD *)this + 29);
  return 1;
}

void __cdecl CParty::send_host_info(CUser **this)
{
  PacketBuf *v1[3]; // [esp+10h] [ebp-18h] BYREF
  int i; // [esp+1Ch] [ebp-Ch]

  PacketGuard::PacketGuard((PacketGuard *)v1);
  InterfacePacketBuf::put_header(v1, 0, 26);
  for ( i = 0; i <= 3; ++i )
  {
    if ( *(this + 6 * i + 30) == *(this + 28) )
    {
      InterfacePacketBuf::put_byte(v1, i);
      InterfacePacketBuf::finalize(v1, 1);
      CParty::send_to_party(this, (PacketGuard *)v1);
      break;
    }
  }
  PacketGuard::~PacketGuard(v1);
}

void __cdecl CParty::send_party_ipinfo(CUser **this)
{
  char member_count; // al
  __int16 unique_id; // ax
  int inner_ip; // eax
  int ip; // eax
  __int16 port; // ax
  int acc_id; // eax
  char nat_type; // al
  int mtu; // eax
  int CurCharacR; // eax
  PacketBuf *v10[3]; // [esp+10h] [ebp-18h] BYREF
  int i; // [esp+1Ch] [ebp-Ch]

  PacketGuard::PacketGuard((PacketGuard *)v10);
  InterfacePacketBuf::put_header(v10, 0, 11);
  member_count = CParty::get_member_count(this);
  InterfacePacketBuf::put_byte(v10, member_count);
  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::_checkValidUser(this, i) )
    {
      unique_id = CUser::get_unique_id(*(this + 6 * i + 30));
      InterfacePacketBuf::put_short(v10, unique_id);
      inner_ip = CNetwork<NETWORK_BUFFER_SIZE_4K,MAX_NETWORK_CONNECTIONS_450K>::get_inner_ip((int)*(this + 6 * i + 30) + 224);
      InterfacePacketBuf::put_int(v10, inner_ip);
      ip = CNetwork<NETWORK_BUFFER_SIZE_4K,MAX_NETWORK_CONNECTIONS_450K>::get_ip((int)*(this + 6 * i + 30) + 224);
      InterfacePacketBuf::put_int(v10, ip);
      port = CNetwork<NETWORK_BUFFER_SIZE_4K,MAX_NETWORK_CONNECTIONS_450K>::get_port((int)*(this + 6 * i + 30) + 224);
      InterfacePacketBuf::put_short(v10, port);
      acc_id = CUser::get_acc_id(*(this + 6 * i + 30));
      InterfacePacketBuf::put_int(v10, acc_id);
      nat_type = CNetwork<NETWORK_BUFFER_SIZE_4K,MAX_NETWORK_CONNECTIONS_450K>::get_nat_type((int)*(this + 6 * i + 30) + 224);
      InterfacePacketBuf::put_byte(v10, nat_type);
      mtu = CNetwork<NETWORK_BUFFER_SIZE_4K,MAX_NETWORK_CONNECTIONS_450K>::get_mtu((int)*(this + 6 * i + 30) + 224);
      InterfacePacketBuf::put_int(v10, mtu);
      if ( CUserCharacInfo::getCurCharacR(*(this + 6 * i + 30)) )
      {
        CurCharacR = CUserCharacInfo::getCurCharacR(*(this + 6 * i + 30));
        InterfacePacketBuf::put_byte(v10, *(_BYTE *)(CurCharacR + 117));
      }
      else
      {
        InterfacePacketBuf::put_byte(v10, 0);
      }
    }
  }
  InterfacePacketBuf::finalize(v10, 1);
  CParty::send_to_party(this, (PacketGuard *)v10);
  PacketGuard::~PacketGuard(v10);
}

bool __cdecl CParty::send_to_party(CUser **this, PacketGuard *a2)
{
  bool result; // al
  int i; // [esp+1Ch] [ebp-Ch]

  for ( i = 0; ; ++i )
  {
    result = i <= 3;
    if ( i > 3 )
      break;
    if ( (unsigned __int8)CParty::_checkValidUser(this, i) )
      CUser::Send(*(this + 6 * i + 30), a2);
  }
  return result;
}

void __cdecl CParty::send_to_party_cmd_error(CUser **a1, __int16 a2, char flag)
{
  PacketBuf *v3[5]; // [esp+24h] [ebp-14h] BYREF

  PacketGuard::PacketGuard((PacketGuard *)v3);
  InterfacePacketBuf::put_header(v3, 1, a2);
  InterfacePacketBuf::put_byte(v3, 0);
  InterfacePacketBuf::put_byte(v3, a3);
  InterfacePacketBuf::finalize(v3, 1);
  CParty::send_to_party(a1, (PacketGuard *)v3);
  PacketGuard::~PacketGuard(v3);
}

void __cdecl CParty::send_invite_pvp(CUser **this, __int16 a2)
{
  unsigned __int16 unique_id; // ax
  PacketBuf *v3[3]; // [esp+10h] [ebp-18h] BYREF
  int i; // [esp+1Ch] [ebp-Ch]

  PacketGuard::PacketGuard((PacketGuard *)v3);
  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::_checkValidUser(this, i) )
    {
      InterfacePacketBuf::clear(v3);
      InterfacePacketBuf::put_header(v3, 0, 7);
      InterfacePacketBuf::put_short(v3, a2);
      InterfacePacketBuf::put_byte(v3, 2);
      unique_id = CUser::get_unique_id(*(this + 6 * i + 30));
      InterfacePacketBuf::put_int(v3, unique_id);
      InterfacePacketBuf::finalize(v3, 1);
      CUser::Send(*(this + 6 * i + 30), (PacketGuard *)v3);
    }
  }
  PacketGuard::~PacketGuard(v3);
}

void __cdecl CParty::send_invite_warroom(CUser **this, __int16 a2)
{
  unsigned __int16 unique_id; // ax
  PacketBuf *v3[3]; // [esp+10h] [ebp-18h] BYREF
  int i; // [esp+1Ch] [ebp-Ch]

  PacketGuard::PacketGuard((PacketGuard *)v3);
  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::_checkValidUser(this, i) )
    {
      InterfacePacketBuf::clear(v3);
      InterfacePacketBuf::put_header(v3, 0, 7);
      InterfacePacketBuf::put_short(v3, a2);
      InterfacePacketBuf::put_byte(v3, 3);
      unique_id = CUser::get_unique_id(*(this + 6 * i + 30));
      InterfacePacketBuf::put_int(v3, unique_id);
      InterfacePacketBuf::finalize(v3, 1);
      CUser::Send(*(this + 6 * i + 30), (PacketGuard *)v3);
    }
  }
  PacketGuard::~PacketGuard(v3);
}

void __cdecl CParty::send_invite_bluemarble(CUser **this, __int16 a2)
{
  unsigned __int16 unique_id; // ax
  PacketBuf *v3[3]; // [esp+10h] [ebp-18h] BYREF
  int i; // [esp+1Ch] [ebp-Ch]

  PacketGuard::PacketGuard((PacketGuard *)v3);
  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::_checkValidUser(this, i) )
    {
      InterfacePacketBuf::clear(v3);
      InterfacePacketBuf::put_header(v3, 0, 7);
      InterfacePacketBuf::put_short(v3, a2);
      InterfacePacketBuf::put_byte(v3, 9);
      unique_id = CUser::get_unique_id(*(this + 6 * i + 30));
      InterfacePacketBuf::put_int(v3, unique_id);
      InterfacePacketBuf::finalize(v3, 1);
      CUser::Send(*(this + 6 * i + 30), (PacketGuard *)v3);
    }
  }
  PacketGuard::~PacketGuard(v3);
}

int __cdecl CParty::CheckMemberFatigue(CUser **this)
{
  int i; // [esp+1Ch] [ebp-Ch]

  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::_checkValidUser(this, i) == 1 && !CUser::CheckFatigue(*(this + 6 * i + 30)) )
      return i + 1;
  }
  return 0;
}

int __cdecl CParty::CheckMemberArea(CUser **this)
{
  int area; // [esp+18h] [ebp-10h]
  int i; // [esp+1Ch] [ebp-Ch]

  area = -1;
  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::_checkValidUser(this, i) == 1 )
    {
      if ( area == -1 )
      {
        area = CUser::get_area(*(this + 6 * i + 30), 0);
      }
      else if ( CUser::get_area(*(this + 6 * i + 30), 0) != area )
      {
        return 0;
      }
    }
  }
  return 1;
}

int CParty::MemberPenaltyRate()
{
  return 1;
}

void __usercall CParty::game_start(int id@<esi>, CUser **this, CUser *user)
{
  __int16 member_count; // ax
  int id; // eax
  bool v5; // al
  int id; // esi
  int id; // ebx
  unsigned int acc_id; // eax
  const char *v9; // eax
  CUser *user; // ebx
  GameWorld *v11; // eax
  __int16 unique_id; // ax
  CGameManager *v13; // eax
  CGameManager *v14; // eax
  char *CurCharacQuestW; // eax
  int id; // ebx
  unsigned int id; // eax
  const char *v18; // eax
  CUser *Manager; // eax
  GameWorld *v20; // eax
  GameWorld *v21; // eax
  CDataManager *v22; // eax
  _DWORD *hellparty_script_values; // eax
  _DWORD *v25; // eax
  GameWorld *v26; // eax
  CDataManager *v27; // eax
  char *PartyTelePort; // eax
  int id; // [esp+40h] [ebp-78h] BYREF
  int id; // [esp+44h] [ebp-74h] BYREF
  void *v32[3]; // [esp+48h] [ebp-70h] BYREF
  int index; // [esp+54h] [ebp-64h] BYREF
  PacketBuf *v34[3]; // [esp+58h] [ebp-60h] BYREF
  char flag[12]; // [esp+64h] [ebp-54h] BYREF
  int TradeSpace; // [esp+70h] [ebp-48h]
  CTradeSpace *v37; // [esp+74h] [ebp-44h]
  int id; // [esp+78h] [ebp-40h]
  char flag; // [esp+7Eh] [ebp-3Ah]
  char CurCharacVill; // [esp+7Fh] [ebp-39h]
  int area; // [esp+80h] [ebp-38h]
  char flag[4]; // [esp+84h] [ebp-34h]
  int id; // [esp+88h] [ebp-30h]
  bool v44; // [esp+8Fh] [ebp-29h]
  char flag[4]; // [esp+90h] [ebp-28h]
  int i; // [esp+94h] [ebp-24h]
  char flag; // [esp+9Bh] [ebp-1Dh]
  int id; // [esp+9Ch] [ebp-1Ch]
  int j; // [esp+A0h] [ebp-18h]
  CUser *user; // [esp+A4h] [ebp-14h]
  int id; // [esp+A8h] [ebp-10h]
  int world_map; // [esp+ACh] [ebp-Ch]

  if ( *((_BYTE *)this + 106) != 2 )
  {
    PacketGuard::PacketGuard((PacketGuard *)v35);
    InterfacePacketBuf::clear((PacketBuf **)v35);
    InterfacePacketBuf::put_header((PacketBuf **)v35, 0, 2);
    InterfacePacketBuf::put_byte((PacketBuf **)v35, 1);
    member_count = CParty::get_member_count(this);
    InterfacePacketBuf::put_short((PacketBuf **)v35, member_count);
    *((_BYTE *)this + 282) = 0;
    v4 = CParty::get_member_count(this);
    *(this + 81) = (CUser *)get_rand_int(v4);
    *(this + 1559) = (CUser *)CSystemTime::getCurSec((CSystemTime *)GlobalData::s_systemTime_);
    PacketGuard::PacketGuard((PacketGuard *)v34);
    InterfacePacketBuf::put_header(v34, 0, 3);
    v38 = 0;
    index = InterfacePacketBuf::get_index(v34);
    InterfacePacketBuf::put_byte(v34, v38);
    v39 = 0;
    CurCharacVill = CUserCharacInfo::getCurCharacVill(a3);
    area = CUser::get_area(a3, 0);
    for ( i = 0; i <= 3; ++i )
    {
      if ( (unsigned __int8)CParty::_checkValidUser(this, i) )
      {
        v47 = CUserCharacInfo::getCurCharacVill(*(this + 6 * i + 30));
        v48 = CUser::get_area(*(this + 6 * i + 30), 0);
        v5 = (CurCharacVill != v47 || area != v48) && (unsigned __int8)CParty::is_quick_party((CParty *)this) != 1;
        if ( v5 && !*((_WORD *)this + 1574) )
        {
          v6 = v47;
          v7 = CurCharacVill;
          acc_id = CUser::get_acc_id(*(this + 6 * i + 30));
          v9 = (const char *)NumberToString(acc_id, 0);
          LogManager::logFormat(
            (LogManager *)1,
            (int)"party.cpp",
            "void CParty::game_start(CUser*)",
            (const char *)0x84C,
            "[HACK_VILL_AREA] m_id : %s, Vill(%d,%d), Area(%d,%d)",
            v9,
            v7,
            v6,
            area,
            v48);
          v39 = 1;
          break;
        }
        *((_BYTE *)this + i + 896) = 1;
        *((_BYTE *)this + i + 900) = 1;
        *((_BYTE *)this + i + 904) = 0;
        *((_BYTE *)this + i + 908) = 0;
        CUser::make_basic_info(a1, *(this + 6 * i + 30), v35, 1);
        v10 = *(this + 6 * i + 30);
        v11 = G_GameWorld();
        GameWorld::goto_dungeon(v11, v10);
        if ( CUser::get_state(*(this + 6 * i + 30)) == 5 )
        {
          unique_id = CUser::get_unique_id(*(this + 6 * i + 30));
          InterfacePacketBuf::put_short(v34, unique_id);
          InterfacePacketBuf::put_byte(v34, 1);
          ++v38;
        }
        TradeSpace = CUser::GetTradeSpace(*(this + 6 * i + 30));
        if ( TradeSpace != -1 )
        {
          v13 = G_CGameManager();
          v37 = (CTradeSpace *)CGameManager::GetTradeSpace(v13, TradeSpace);
          if ( v37 )
          {
            if ( !CTradeSpace::check_trade_possibility(v37) )
            {
              CTradeSpace::cancel_trade((CUserCharacInfo **)v37);
              v14 = G_CGameManager();
              CGameManager::PutTradeSpace(v14, (pthread_mutex_t **)v37);
            }
          }
        }
        CurCharacQuestW = CUser::getCurCharacQuestW(*(this + 6 * i + 30));
        _Quest_Authen_Data::reset((_Quest_Authen_Data *)(CurCharacQuestW + 30168));
      }
    }
    if ( v39 )
    {
      for ( j = 0; j <= 3; ++j )
      {
        user = (CUser *)CParty::get_user((CParty *)this, j);
        if ( user )
        {
          v16 = CurCharacVill;
          v17 = CUser::get_acc_id(user);
          v18 = (const char *)NumberToString(v17, 0);
          LogManager::logFormat(
            (LogManager *)1,
            (int)"party.cpp",
            "void CParty::game_start(CUser*)",
            (const char *)0x88A,
            "[HACK_VILL_AREA] m_id : %s, Vill(%d), Area(%d)",
            v18,
            v16,
            area);
          CUser::DisConnSig((pthread_mutex_t *)user, 24, 1, 0);
        }
      }
    }
    else
    {
      InterfacePacketBuf::put_byte(v34, &index, v38);
      InterfacePacketBuf::finalize(v34, 1);
      if ( (unsigned __int8)CParty::IsExistInvisible(this)
        || (Manager = (CUser *)CParty::getManager((CParty *)this), CUser::getMoveSpace(Manager) == 1) )
      {
        CParty::send_to_party(this, (PacketGuard *)v34);
      }
      else
      {
        v20 = G_GameWorld();
        GameWorld::send_all(v20, (PacketGuard *)v34);
      }
      InterfacePacketBuf::finalize((PacketBuf **)v35, 1);
      CParty::send_to_party(this, (PacketGuard *)v35);
      CParty::set_host((CParty *)this);
      CParty::send_host_info(this);
      *((_BYTE *)this + 6244) = 0;
      *(this + 1562) = 0;
      v21 = G_GameWorld();
      *(_DWORD *)v42 = GameWorld::GetWorldMapIndex(v21, a3);
      if ( *(int *)v42 > 0 )
      {
        v22 = G_CDataManager();
        *(this + 1562) = (CUser *)CWorldMapList::find_world_map((CDataManager *)((char *)v22 + 34688), *(_DWORD *)v42);
      }
      InterfacePacketBuf::clear((PacketBuf **)v35);
      InterfacePacketBuf::put_header((PacketBuf **)v35, 0, 27);
      if ( CParty::get_member_count(this) == 1 )
      {
        G_CDataManager();
        hellparty_script_values = CDataManager::get_hellparty_script_values();
        BattleData::SetHellPartyValueTotal((BattleData *)(this + 203), hellparty_script_values[2]);
      }
      v43 = (int)(*((float *)this + 707) * 100.0 / (long double)(int)*(this + 835));
      InterfacePacketBuf::put_int((PacketBuf **)v35, v43);
      std::vector<int>::vector(v32);
      if ( *(this + 1562)
        && (unsigned __int8)CWorldMap::IsInHellDungeon(*(this + 1562))
        && (unsigned __int8)CBattle_Field::IsEnableHellDungeon() )
      {
        CParty::CheckClearQuestHellParty((CParty *)this, (int *)v32);
        v51 = std::vector<int>::size(v32);
        if ( v51 <= 0 )
        {
          InterfacePacketBuf::put_byte((PacketBuf **)v35, 0);
          *((_BYTE *)this + 6244) = 1;
        }
        else
        {
          InterfacePacketBuf::put_byte((PacketBuf **)v35, v51);
          std::vector<int>::begin(&v31, v32);
          std::vector<int>::end(&v30, (int)v32);
          while ( __gnu_cxx::operator!=<int *,std::vector<int>>((int)&v31, (int)&v30) )
          {
            v25 = (_DWORD *)__gnu_cxx::__normal_iterator<int *,std::vector<int>>::operator*((int)&v31);
            InterfacePacketBuf::put_short((PacketBuf **)v35, *v25);
            __gnu_cxx::__normal_iterator<int *,std::vector<int>>::operator++(&v31);
          }
        }
      }
      else
      {
        InterfacePacketBuf::put_byte((PacketBuf **)v35, 0);
      }
      if ( (unsigned __int8)CParty::make_blood_dungeon_info((CParty *)this, a3, (PacketBuf **)v35) != 1 )
        CParty::make_blood_dungeon_info_blank(this, (PacketBuf **)v35);
      v44 = 0;
      v26 = G_GameWorld();
      *(_DWORD *)v45 = GameWorld::GetWorldMapIndex(v26, a3);
      if ( *(int *)v45 > 0 )
      {
        v27 = G_CDataManager();
        world_map = CWorldMapList::find_world_map((CDataManager *)((char *)v27 + 34688), *(_DWORD *)v45);
        if ( *(this + 1562) && (unsigned __int8)CWorldMap::hasDeathTower(*(this + 1562)) )
          v44 = 1;
      }
      if ( CParty::isDungeonDifficultyInfromNotice(this, v44) )
      {
        InterfacePacketBuf::put_byte((PacketBuf **)v35, 1);
        CUserCharacInfo::setMemberDungeonFlag(*(this + 29), 1);
      }
      else
      {
        InterfacePacketBuf::put_byte((PacketBuf **)v35, 0);
      }
      if ( v44 )
        CBattle_Field::setWorldMapTowerArea((CBattle_Field *)(this + 713), v44);
      InterfacePacketBuf::finalize((PacketBuf **)v35, 1);
      CParty::send_to_party(this, (PacketGuard *)v35);
      *((_BYTE *)this + 280) = 0;
      PartyTelePort = CParty::GetPartyTelePort((CParty *)this);
      CPartyTelePort::reset_teleport_data((CParty **)PartyTelePort);
      std::vector<int>::~vector(v32);
    }
    PacketGuard::~PacketGuard(v34);
    PacketGuard::~PacketGuard((PacketPool **)v35);
  }
}

int __cdecl CParty::make_blood_dungeon_info(CParty *this, CUser *user, PacketBuf **a3)
{
  GameWorld *v3; // eax
  CDataManager *v4; // eax
  char member_count; // al
  __int16 unique_id; // ax
  char *DungeonList; // eax
  int count; // ebx
  CDungeonList **v10; // eax
  _DWORD *v11; // eax
  int blood_dungeon_admission_fee; // eax
  CCharacBloodDungeon *CharacExpandData; // eax
  int EnterCount; // eax
  CCharacBloodDungeon *v15; // eax
  int WeeklyEnterCount; // eax
  char *v17; // eax
  int RepeatEvent; // eax
  CBloodDungeonRewardSecondEvent *v19; // eax
  int Gold; // eax
  int count; // eax
  CBloodDungeonRewardFirstEvent *v22; // eax
  int Jackpot; // eax
  CDataManager *v24; // eax
  int count; // [esp+14h] [ebp-24h] BYREF
  int index; // [esp+18h] [ebp-20h] BYREF
  int count; // [esp+1Ch] [ebp-1Ch] BYREF
  char flag[4]; // [esp+20h] [ebp-18h]
  int i; // [esp+24h] [ebp-14h]
  int count; // [esp+28h] [ebp-10h]
  int dungeon; // [esp+2Ch] [ebp-Ch]

  if ( !*((_DWORD *)this + 1562) )
  {
    v3 = G_GameWorld();
    *(_DWORD *)v28 = GameWorld::GetWorldMapIndex(v3, a2);
    if ( *(int *)v28 > 0 )
    {
      v4 = G_CDataManager();
      *((_DWORD *)this + 1562) = CWorldMapList::find_world_map((CDataManager *)((char *)v4 + 34688), *(_DWORD *)v28);
    }
  }
  if ( !*((_DWORD *)this + 1562) )
    return 0;
  if ( (unsigned __int8)CWorldMap::hasDeathTower(*((CWorldMap **)this + 1562)) != 1 )
    return 0;
  member_count = CParty::get_member_count((CUser **)this);
  InterfacePacketBuf::put_byte(a3, member_count);
  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::_checkValidUser((CUser **)this, i) )
    {
      unique_id = CUser::get_unique_id(*((CUser **)this + 6 * i + 30));
      InterfacePacketBuf::put_short(a3, unique_id);
      index = InterfacePacketBuf::get_index(a3);
      v30 = 0;
      InterfacePacketBuf::put_byte(a3, 0);
      DungeonList = CWorldMap::GetDungeonList(*((CWorldMap **)this + 1562));
      std::vector<int>::begin(&v25, DungeonList);
      while ( 1 )
      {
        v17 = CWorldMap::GetDungeonList(*((CWorldMap **)this + 1562));
        std::vector<int>::end(&v27, (int)v17);
        if ( !__gnu_cxx::operator!=<int *,std::vector<int>>((int)&v25, (int)&v27) )
          break;
        v9 = *(_DWORD *)__gnu_cxx::__normal_iterator<int *,std::vector<int>>::operator*((int)&v25);
        v10 = (CDungeonList **)G_CDataManager();
        dungeon = CDataManager::find_dungeon(v10, v9);
        if ( dungeon && *(_BYTE *)(dungeon + 2207) == 1 )
        {
          v11 = (_DWORD *)__gnu_cxx::__normal_iterator<int *,std::vector<int>>::operator*((int)&v25);
          InterfacePacketBuf::put_short(a3, *v11);
          blood_dungeon_admission_fee = CParty::get_blood_dungeon_admission_fee(this, *((CUser **)this + 6 * i + 30));
          InterfacePacketBuf::put_int(a3, blood_dungeon_admission_fee);
          CharacExpandData = (CCharacBloodDungeon *)CUser::GetCharacExpandData(*((_DWORD *)this + 6 * i + 30), 1u);
          EnterCount = CCharacBloodDungeon::GetEnterCount(CharacExpandData);
          InterfacePacketBuf::put_int(a3, EnterCount);
          v15 = (CCharacBloodDungeon *)CUser::GetCharacExpandData(*((_DWORD *)this + 6 * i + 30), 1u);
          WeeklyEnterCount = CCharacBloodDungeon::GetWeeklyEnterCount(v15);
          InterfacePacketBuf::put_int(a3, WeeklyEnterCount);
          ++v30;
        }
        __gnu_cxx::__normal_iterator<int *,std::vector<int>>::operator++(&v25);
      }
      InterfacePacketBuf::put_byte(a3, &index, v30);
    }
  }
  RepeatEvent = CEventManager::GetRepeatEvent(GlobalData::s_event_manager, 46);
  if ( (*(unsigned __int8 (__cdecl **)(int, _DWORD))(*(_DWORD *)RepeatEvent + 52))(RepeatEvent, 0) )
  {
    v19 = (CBloodDungeonRewardSecondEvent *)CEventManager::GetRepeatEvent(GlobalData::s_event_manager, 46);
    Gold = CBloodDungeonRewardSecondEvent::GetGold(v19);
    InterfacePacketBuf::put_int(a3, Gold);
  }
  else
  {
    v21 = CEventManager::GetRepeatEvent(GlobalData::s_event_manager, 45);
    if ( (*(unsigned __int8 (__cdecl **)(int, _DWORD))(*(_DWORD *)v21 + 52))(v21, 0) )
    {
      v22 = (CBloodDungeonRewardFirstEvent *)CEventManager::GetRepeatEvent(GlobalData::s_event_manager, 45);
      Jackpot = CBloodDungeonRewardFirstEvent::GetGold(v22);
    }
    else
    {
      v24 = G_CDataManager();
      Jackpot = ServerParameterScript::GetJackpot((CDataManager *)((char *)v24 + 104));
    }
    InterfacePacketBuf::put_int(a3, Jackpot);
  }
  return 1;
}

int __cdecl CParty::make_blood_dungeon_info_blank(CUser **this, PacketBuf **a2)
{
  char member_count; // al
  __int16 unique_id; // ax
  int i; // [esp+1Ch] [ebp-Ch]

  member_count = CParty::get_member_count(this);
  InterfacePacketBuf::put_byte(a2, member_count);
  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::_checkValidUser(this, i) )
    {
      unique_id = CUser::get_unique_id(*(this + 6 * i + 30));
      InterfacePacketBuf::put_short(a2, unique_id);
      InterfacePacketBuf::put_byte(a2, 0);
    }
  }
  return InterfacePacketBuf::put_int(a2, 0);
}

_BOOL4 __cdecl CParty::CheckEnterAncientDungeon(
        CUserCharacInfo **this,

    return (unsigned __int8)CParty::CheckEnterEventDungeon(this, a2, a3, a4, a5) == 1;
  if ( *((_DWORD *)a2 + 509) == -1 )

    if ( (unsigned __int8)CParty::_checkValidUser(this, i) == 1 )
    {
      if ( CDungeon::get_index(a2) == 1500 )
      {
        v6 = &a5[i];
        v7 = *((_DWORD *)a2 + 509);
        CurCharacInvenR = (CInventory *)CUserCharacInfo::getCurCharacInvenR(*(this + 6 * i + 30));
        *v6 = CInventory::check_vilmark_dungeon_item_exist(CurCharacInvenR, v7);
        if ( *v6 == -1 )
        {
          *a3 = i;
          return 0;
        }
      }
      else
      {
        v9 = &a5[i];
        v10 = (char *)*((_DWORD *)a2 + 509);
        v11 = (CInventory *)CUserCharacInfo::getCurCharacInvenR(*(this + 6 * i + 30));
        *v9 = CInventory::check_item_exist(v11, v10);
        if ( *v9 == -1 )
        {
          *a3 = i;
          return 0;
        }
      }
      v12 = (char *)a4 + 61 * i;
      v13 = a5[i];
      v14 = (int *)CUserCharacInfo::getCurCharacInvenR(*(this + 6 * i + 30));
      CInventory::GetInvenSlot((CInventory *)v15, v14, 1, v13);
      *(_DWORD *)v12 = v15[0];
      *((_DWORD *)v12 + 1) = v15[1];
      *((_DWORD *)v12 + 2) = v15[2];
      *((_DWORD *)v12 + 3) = v15[3];
      *((_DWORD *)v12 + 4) = v15[4];
      *((_DWORD *)v12 + 5) = v15[5];
      *((_DWORD *)v12 + 6) = v15[6];
      *((_DWORD *)v12 + 7) = v15[7];
      *((_DWORD *)v12 + 8) = v15[8];
      *((_DWORD *)v12 + 9) = v15[9];
      *((_DWORD *)v12 + 10) = v15[10];
      *((_DWORD *)v12 + 11) = v15[11];
      *((_DWORD *)v12 + 12) = v15[12];
      *((_DWORD *)v12 + 13) = v15[13];
      *((_DWORD *)v12 + 14) = v15[14];
      v12[60] = v16;
      if ( *(_DWORD *)((char *)a4 + 61 * i + 7) < *((_DWORD *)a2 + 510) )
      {
        *a3 = i;
        return 0;
      }
    }

_BOOL4 __cdecl CParty::CheckEnterAncientDungeon(CUserCharacInfo **this, char **a2)
{
  unsigned __int8 *v2; // ebx
  int i; // esi
  int id; // ebx
  char *v6; // esi
  CInventory *CurCharacInvenR; // eax
  int id; // ebx
  int id; // esi
  int *v10; // eax
  unsigned __int8 *v11; // ebx
  _DWORD value[15]; // [esp+20h] [ebp-158h] BYREF
  unsigned __int8 v13; // [esp+5Ch] [ebp-11Ch]
  unsigned __int8 s[244]; // [esp+67h] [ebp-111h] BYREF
  unsigned __int8 v15; // [esp+15Bh] [ebp-1Dh] BYREF
  int id[4]; // [esp+15Ch] [ebp-1Ch] BYREF
  int j; // [esp+16Ch] [ebp-Ch]

  v2 = s;
  for ( i = 3; i != -1; --i )
  {
    Inven_Item::Inven_Item((Inven_Item *)v2);
    v2 += 61;
  }
  memset(s, 0, sizeof(s));
  memset(v16, 0, sizeof(v16));
  if ( *((_BYTE *)a2 + 2140) )
  {
    v15 = 0;
    return (unsigned __int8)CParty::CheckEnterEventDungeon(this, (const CDungeon *)a2, &v15, (Inven_Item *)s, v16) == 1;
  }
  else if ( a2[509] == (char *)-1 )
  {
    return 1;
  }
  else if ( *((_BYTE *)a2 + 2044) != 1 )
  {
    return 1;
  }
  else if ( *(this + 822) == (CUserCharacInfo *)1 )
  {
    return 1;
  }
  else
  {
    for ( j = 0; j <= 3; ++j )
    {
      if ( (unsigned __int8)CParty::_checkValidUser(this, j) == 1 )
      {
        v5 = j;
        v6 = a2[509];
        CurCharacInvenR = (CInventory *)CUserCharacInfo::getCurCharacInvenR(*(this + 6 * j + 30));
        v16[v5] = CInventory::check_item_exist(CurCharacInvenR, v6);
        if ( v16[v5] == -1 )
          return 0;
        v8 = j;
        v9 = v16[j];
        v10 = (int *)CUserCharacInfo::getCurCharacInvenR(*(this + 6 * j + 30));
        v11 = &s[61 * v8];
        CInventory::GetInvenSlot((CInventory *)v12, v10, 1, v9);
        *(_DWORD *)v11 = v12[0];
        *((_DWORD *)v11 + 1) = v12[1];
        *((_DWORD *)v11 + 2) = v12[2];
        *((_DWORD *)v11 + 3) = v12[3];
        *((_DWORD *)v11 + 4) = v12[4];
        *((_DWORD *)v11 + 5) = v12[5];
        *((_DWORD *)v11 + 6) = v12[6];
        *((_DWORD *)v11 + 7) = v12[7];
        *((_DWORD *)v11 + 8) = v12[8];
        *((_DWORD *)v11 + 9) = v12[9];
        *((_DWORD *)v11 + 10) = v12[10];
        *((_DWORD *)v11 + 11) = v12[11];
        *((_DWORD *)v11 + 12) = v12[12];
        *((_DWORD *)v11 + 13) = v12[13];
        *((_DWORD *)v11 + 14) = v12[14];
        v11[60] = v13;
        if ( *(_DWORD *)&s[61 * j + 7] < (int)a2[510] )
          return 0;
      }
    }
    return 1;
  }
}

int __cdecl CParty::UseAncientDungeonItems(CParty *this, const CDungeon *dungeon, Inven_Item *item, int *a4)
{
  int count; // esi
  int count; // ebx
  CInventory *CurCharacInvenW; // eax
  char CurCharacNo; // bl
  char ItemAttr; // al
  char AbilityType; // al
  __int16 AbilityValue; // ax
  PacketBuf *v12[3]; // [esp+2Ch] [ebp-3Ch] BYREF
  int count[4]; // [esp+38h] [ebp-30h] BYREF
  int i; // [esp+48h] [ebp-20h]
  int count; // [esp+4Ch] [ebp-1Ch]

  if ( *((_DWORD *)this + 822) == 1 )
    return 1;
  if ( *((_BYTE *)a2 + 2044) )
  {
    for ( i = 0; i <= 3; ++i )
    {
      if ( (unsigned __int8)CParty::_checkValidUser((CUser **)this, i) == 1 )
      {
        v5 = *((_DWORD *)a2 + 510);
        v6 = a4[i];
        CurCharacInvenW = (CInventory *)CUserCharacInfo::getCurCharacInvenW(*((CUserCharacInfo **)this + 6 * i + 30));
        if ( (unsigned __int8)CInventory::delete_item(CurCharacInvenW, 1, v6, v5, 14, 1) != 1 )
        {
          CurCharacNo = CUserCharacInfo::getCurCharacNo(*((CUserCharacInfo **)this + 6 * i + 30));
          cMyTrace::cMyTrace(
            (cMyTrace *)v13,
            "bool CParty::UseAncientDungeonItems(const CDungeon*, Inven_Item*, int*)",
            2656,
            5);
          cMyTrace::operator()(
            (int)v13,
            "CParty::UseAncientDungeonItems, delete_item failed , User ch=%d , %d %d",
            CurCharacNo);
          return 0;
        }
        PacketGuard::PacketGuard((PacketGuard *)v12);
        InterfacePacketBuf::put_header(v12, 0, 14);
        InterfacePacketBuf::put_byte(v12, 0);
        InterfacePacketBuf::put_short(v12, 1);
        InterfacePacketBuf::put_short(v12, a4[i]);
        v15 = *(_DWORD *)((char *)a3 + 61 * i + 7) - *((_DWORD *)a2 + 510);
        if ( v15 )
          InterfacePacketBuf::put_int(v12, *(_DWORD *)((char *)a3 + 61 * i + 2));
        else
          InterfacePacketBuf::put_int(v12, -1);
        InterfacePacketBuf::put_int(v12, v15);
        ItemAttr = Inven_Item::GetItemAttr((Inven_Item *)((char *)a3 + 61 * i));
        InterfacePacketBuf::put_byte(v12, ItemAttr);
        InterfacePacketBuf::put_short(v12, *(_WORD *)((char *)a3 + 61 * i + 11));
        InterfacePacketBuf::put_byte(v12, *((_BYTE *)a3 + 61 * i));
        InterfacePacketBuf::put_int(v12, *(_DWORD *)((char *)a3 + 61 * i + 13));
        AbilityType = stAmplifyOption_t::getAbilityType((Inven_Item *)((char *)a3 + 61 * i + 17));
        InterfacePacketBuf::put_byte(v12, AbilityType);
        AbilityValue = stAmplifyOption_t::getAbilityValue((Inven_Item *)((char *)a3 + 61 * i + 17));
        InterfacePacketBuf::put_short(v12, AbilityValue);
        InterfacePacketBuf::put_byte(v12, 0);
        InterfacePacketBuf::put_packet(v12, (Inven_Item *)((char *)a3 + 61 * i));
        InterfacePacketBuf::finalize(v12, 1);
        CUser::Send(*((CUser **)this + 6 * i + 30), (PacketGuard *)v12);
        CUser::SaveInventory(*((CUser **)this + 6 * i + 30));
        PacketGuard::~PacketGuard(v12);
      }
    }
  }
  return 1;
}

int __cdecl CParty::CheckEnterEventDungeon(
        CUserCharacInfo **this,

    if ( (unsigned __int8)CParty::_checkValidUser(this, i) == 1 )
    {
      v35 = 1;
      v6 = &a5[i];
      v7 = (char *)*((_DWORD *)a2 + 536);
      CurCharacInvenR = (CInventory *)CUserCharacInfo::getCurCharacInvenR(*(this + 6 * i + 30));
      *v6 = CInventory::check_item_exist(CurCharacInvenR, v7);
      if ( *v6 == -1 )
      {
        *a3 = i;
        v35 = 0;
      }
      else
      {
        v9 = (char *)a4 + 61 * i;
        v10 = a5[i];
        v11 = (int *)CUserCharacInfo::getCurCharacInvenR(*(this + 6 * i + 30));
        CInventory::GetInvenSlot((CInventory *)&v18, v11, 1, v10);
        *(_DWORD *)v9 = v18;
        *((_DWORD *)v9 + 1) = v19;
        *((_DWORD *)v9 + 2) = v20;
        *((_DWORD *)v9 + 3) = v21;
        *((_DWORD *)v9 + 4) = v22;
        *((_DWORD *)v9 + 5) = v23;
        *((_DWORD *)v9 + 6) = v24;
        *((_DWORD *)v9 + 7) = v25;
        *((_DWORD *)v9 + 8) = v26;
        *((_DWORD *)v9 + 9) = v27;
        *((_DWORD *)v9 + 10) = v28;
        *((_DWORD *)v9 + 11) = v29;
        *((_DWORD *)v9 + 12) = v30;
        *((_DWORD *)v9 + 13) = v31;
        *((_DWORD *)v9 + 14) = v32;
        v9[60] = v33;
        if ( *(_DWORD *)((char *)a4 + 61 * i + 7) < *((_DWORD *)a2 + 537) )
        {
          *a3 = i;
          v35 = 0;
        }
      }
      if ( v35 != 1 )
      {
        v35 = 1;
        v12 = &a5[i];
        v13 = (char *)*((_DWORD *)a2 + 509);
        v14 = (CInventory *)CUserCharacInfo::getCurCharacInvenR(*(this + 6 * i + 30));
        *v12 = CInventory::check_item_exist(v14, v13);
        if ( *v12 == -1 )
        {
          *a3 = i;
          v35 = 0;
        }
        else
        {
          v15 = (char *)a4 + 61 * i;
          v16 = a5[i];
          v17 = (int *)CUserCharacInfo::getCurCharacInvenR(*(this + 6 * i + 30));
          CInventory::GetInvenSlot((CInventory *)&v18, v17, 1, v16);
          *(_DWORD *)v15 = v18;
          *((_DWORD *)v15 + 1) = v19;
          *((_DWORD *)v15 + 2) = v20;
          *((_DWORD *)v15 + 3) = v21;
          *((_DWORD *)v15 + 4) = v22;
          *((_DWORD *)v15 + 5) = v23;
          *((_DWORD *)v15 + 6) = v24;
          *((_DWORD *)v15 + 7) = v25;
          *((_DWORD *)v15 + 8) = v26;
          *((_DWORD *)v15 + 9) = v27;
          *((_DWORD *)v15 + 10) = v28;
          *((_DWORD *)v15 + 11) = v29;
          *((_DWORD *)v15 + 12) = v30;
          *((_DWORD *)v15 + 13) = v31;
          *((_DWORD *)v15 + 14) = v32;
          v15[60] = v33;
          if ( *(_DWORD *)((char *)a4 + 61 * i + 7) < *((_DWORD *)a2 + 510) )
          {
            *a3 = i;
            v35 = 0;
          }
        }
      }
      if ( v35 != 1 )
        return 0;
    }

int __cdecl CParty::CheckEnterDimensionDungeon(CUser **this, const CDungeon *dungeon, unsigned __int8 *a3, bool a4)
{
  int charac_level; // ebx
  int member_count; // [esp+24h] [ebp-14h]
  int id; // [esp+28h] [ebp-10h]
  int i; // [esp+2Ch] [ebp-Ch]

  member_count = CParty::get_member_count(this);
  v9 = (char)CDungeon::get_dimension_possible(a2) - 1;
  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::_checkValidUser(this, i) == 1 )
    {
      if ( a4
        && (char)CDungeon::get_dimension_min_partymem(a2) > member_count
        && (unsigned __int8)CUser::isGMUser(*(this + 6 * i + 30)) != 1 )
      {
        return 171;
      }
      charac_level = CUserCharacInfo::get_charac_level(*(this + 6 * i + 30));
      if ( charac_level < CDungeon::get_min_level(a2) )
      {
        *a3 = i;
        return 172;
      }
      if ( *(char *)(CUserCharacInfo::getCurCharacR(*(this + 6 * i + 30)) + v9 + 3769) <= 0
        && *(this + 822) != (CUser *)1
        && (unsigned __int8)CUser::isGMUser(*(this + 6 * i + 30)) != 1 )
      {
        *a3 = i;
        return 173;
      }
    }
  }
  return 1;
}

int __cdecl CParty::CheckEnterDimensionDungeon(CParty *this, const CDungeon *dungeon, unsigned __int8 *a3, char flag)
{
  _BYTE v5[3]; // [esp+28h] [ebp-10h] BYREF
  bool v6; // [esp+2Bh] [ebp-Dh]
  int member_count; // [esp+2Ch] [ebp-Ch]

  v6 = 1;
  DimensionPartyCount::DimensionPartyCount((DimensionPartyCount *)v5, *((_DWORD *)this + 822), a4, -1);
  if ( (unsigned __int8)CDungeon::get_dimension_member_count(a2, (DimensionPartyCount *)v5) )
  {
    member_count = CParty::get_member_count((CUser **)this);
    if ( v5[2] <= member_count )
      v6 = 0;
  }
  return CParty::CheckEnterDimensionDungeon((CUser **)this, a2, a3, v6);
}

int __cdecl CParty::CheckEnterDimensionDungeon(CDungeon **this, const CDungeon *dungeon)
{
  char dimension_min_partymem; // al
  int CurCharacR; // eax
  _BYTE v6[3]; // [esp+11h] [ebp-17h] BYREF
  int member_count; // [esp+14h] [ebp-14h]
  int id; // [esp+18h] [ebp-10h]
  int i; // [esp+1Ch] [ebp-Ch]

  if ( (char)CDungeon::get_dimension_possible(*(this + 811)) <= 0 )
    return 0;
  member_count = CParty::get_member_count(this);
  v8 = (char)CDungeon::get_dimension_possible(a2) - 1;
  DimensionPartyCount::DimensionPartyCount((DimensionPartyCount *)v6, (char)*(this + 822), (char)*(this + 817), -1);
  if ( (unsigned __int8)CDungeon::get_dimension_member_count(a2, (DimensionPartyCount *)v6) )
  {
    if ( v6[2] > member_count )
      return 171;
  }
  else
  {
    dimension_min_partymem = CDungeon::get_dimension_min_partymem(a2);
    if ( dimension_min_partymem > member_count )
      return 171;
  }
  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::_checkValidUser(this, i) == 1 )
    {
      CurCharacR = CUserCharacInfo::getCurCharacR(*(this + 6 * i + 30));
      if ( *(char *)(CurCharacR + v8 + 3769) <= 0
        && *(this + 822) != (CDungeon *)1
        && (unsigned __int8)CUser::isGMUser(*(this + 6 * i + 30)) != 1 )
      {
        return 173;
      }
    }
  }
  return 0;
}

int __cdecl CParty::getPartyMemberAvgLevel(CUserCharacInfo **this)
{
  int id; // [esp+10h] [ebp-18h]
  int id; // [esp+18h] [ebp-10h]
  int i; // [esp+1Ch] [ebp-Ch]

  v2 = 0;
  v3 = 0;
  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::_checkValidUser(this, i) == 1 )
    {
      v2 += CUserCharacInfo::get_charac_level(*(this + 6 * i + 30));
      ++v3;
    }
  }
  if ( v3 )
    return v2 / v3;
  else
    return 0;
}

int __cdecl CParty::setStandardDimensionLevel(CUser **this)
{
  int value; // [esp+20h] [ebp-18h]
  int value; // [esp+28h] [ebp-10h]
  int i; // [esp+2Ch] [ebp-Ch]

  v2 = 0;
  v3 = 0;
  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::_checkValidUser(this, i) == 1 )
    {
      CUser::isGMUser(*(this + 6 * i + 30));
      v2 += CUserCharacInfo::get_charac_level(*(this + 6 * i + 30));
      ++v3;
    }
  }
  *(this + 855) = (CUser *)(v2 / v3);
  return 1;
}

int __cdecl CParty::getStandardDimensionLevel(CParty *this)
{
  return *((_DWORD *)this + 855);
}

int __cdecl CParty::checkDugeonInoutTime(CUser **this, int id)
{
  CDataManager *v3; // eax
  int i; // [esp+1Ch] [ebp-Ch]

  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::_checkValidUser(this, i) == 1
      && (unsigned __int8)CUser::isGMUser(*(this + 6 * i + 30)) )
    {
      return 1;
    }
  }
  v3 = G_CDataManager();
  return ServerParameterScript::isDungeonOpen((CDataManager *)((char *)v3 + 104), a2);
}

int __cdecl CParty::get_blood_dungeon_admission_fee(CParty *this, CUser *user)
{
  CCharacBloodDungeon *CharacExpandData; // eax
  int charac_level; // ebx
  CDataManager *v4; // eax
  CDataManager *v5; // eax
  CDataManager *v6; // eax
  int EnterCount; // [esp+14h] [ebp-14h]
  int PriceAverage; // [esp+18h] [ebp-10h]
  int id; // [esp+1Ch] [ebp-Ch]

  CharacExpandData = (CCharacBloodDungeon *)CUser::GetCharacExpandData((int)a2, 1u);
  EnterCount = CCharacBloodDungeon::GetEnterCount(CharacExpandData);
  charac_level = CUserCharacInfo::get_charac_level(a2);
  v4 = G_CDataManager();
  PriceAverage = ServerParameterScript::GetPriceAverage((CDataManager *)((char *)v4 + 104), charac_level);
  v5 = G_CDataManager();
  v10 = PriceAverage * ServerParameterScript::GetAdmissionIncrease((CDataManager *)((char *)v5 + 104), EnterCount) / 100;
  v6 = G_CDataManager();
  return v10 + PriceAverage + ServerParameterScript::GetLottoCost((CDataManager *)((char *)v6 + 104));
}

int __cdecl CParty::check_dungeon_start(CParty *this, const CDungeon *dungeon, int *a3)
{
  CInventory *CurCharacInvenR; // eax
  int charac_level; // ebx
  CCharacBloodDungeon *CharacExpandData; // eax
  int index; // eax
  int id; // ebx
  int i; // [esp+14h] [ebp-24h]
  int blood_dungeon_admission_fee; // [esp+18h] [ebp-20h]
  int limit_Inout_count; // [esp+1Ch] [ebp-1Ch]
  int member_count; // [esp+24h] [ebp-14h]
  int j; // [esp+28h] [ebp-10h]
  int id; // [esp+2Ch] [ebp-Ch]

  if ( *((_BYTE *)a2 + 2207) == 1 )
  {
    for ( i = 0; i <= 3; ++i )
    {
      if ( (unsigned __int8)CParty::_checkValidUser((CUser **)this, i) == 1 )
      {
        blood_dungeon_admission_fee = CParty::get_blood_dungeon_admission_fee(this, *((CUser **)this + 6 * i + 30));
        CurCharacInvenR = (CInventory *)CUserCharacInfo::getCurCharacInvenR(*((CUserCharacInfo **)this + 6 * i + 30));
        if ( CInventory::get_money(CurCharacInvenR) < blood_dungeon_admission_fee )
        {
          *a3 = i;
          return 10;
        }
        charac_level = CUserCharacInfo::get_charac_level(*((CUserCharacInfo **)this + 6 * i + 30));
        if ( charac_level < CDungeon::get_min_level(a2) )
        {
          *a3 = i;
          return 95;
        }
        limit_Inout_count = (char)CDungeon::get_limit_Inout_count(a2);
        CharacExpandData = (CCharacBloodDungeon *)CUser::GetCharacExpandData(*((_DWORD *)this + 6 * i + 30), 1u);
        if ( limit_Inout_count <= CCharacBloodDungeon::GetEnterCount(CharacExpandData) )
          return 245;
      }
    }
  }
  else if ( *((_BYTE *)a2 + 2207) == 2 )
  {
    index = CDungeon::get_index(a2);
    if ( (unsigned __int8)CParty::checkDugeonInoutTime((CUser **)this, index) != 1 )
      return 244;
    if ( CPowerManager::IsPowerWarEventOn() )
      return 246;
    if ( village_attacked::CVillageMonsterMgr::OnEvent(GlobalData::s_villageMonsterMgr) )
      return 247;
    member_count = CParty::get_member_count((CUser **)this);
    if ( (char)CDungeon::get_limit_party_count(a2) < member_count )
      return 243;
    for ( j = 0; j <= 3; ++j )
    {
      if ( (unsigned __int8)CParty::_checkValidUser((CUser **)this, j) == 1 )
      {
        v8 = CUserCharacInfo::get_charac_level(*((CUserCharacInfo **)this + 6 * j + 30));
        if ( v8 < CDungeon::get_min_level(a2) )
        {
          *a3 = j;
          return 248;
        }
        if ( *(char *)(CUserCharacInfo::getCurCharacR(*((CUserCharacInfo **)this + 6 * j + 30)) + 3778) <= 0
          && *((_DWORD *)this + 822) != 1 )
        {
          *a3 = j;
          return 245;
        }
      }
    }
  }
  else
  {
    v15 = CParty::get_member_count((CUser **)this);
    if ( (char)CDungeon::get_limit_party_count(a2) < v15 )
      return 243;
  }
  return 0;
}

int __cdecl CParty::check_dungeon_start(CParty *this, const CDungeon *dungeon)
{
  CInventory *CurCharacInvenR; // eax
  int index; // eax
  int i; // [esp+10h] [ebp-18h]
  int blood_dungeon_admission_fee; // [esp+14h] [ebp-14h]
  int member_count; // [esp+18h] [ebp-10h]
  int j; // [esp+1Ch] [ebp-Ch]

  if ( *((_BYTE *)a2 + 2207) == 1 )
  {
    for ( i = 0; i <= 3; ++i )
    {
      if ( (unsigned __int8)CParty::_checkValidUser((CUser **)this, i) == 1 )
      {
        blood_dungeon_admission_fee = CParty::get_blood_dungeon_admission_fee(this, *((CUser **)this + 6 * i + 30));
        CurCharacInvenR = (CInventory *)CUserCharacInfo::getCurCharacInvenR(*((CUserCharacInfo **)this + 6 * i + 30));
        if ( CInventory::get_money(CurCharacInvenR) < blood_dungeon_admission_fee )
          return 10;
      }
    }
  }
  else if ( *((_BYTE *)a2 + 2207) == 2 )
  {
    index = CDungeon::get_index(a2);
    if ( (unsigned __int8)CParty::checkDugeonInoutTime((CUser **)this, index) != 1 )
      return 244;
    if ( CPowerManager::IsPowerWarEventOn() )
      return 246;
    if ( village_attacked::CVillageMonsterMgr::OnEvent(GlobalData::s_villageMonsterMgr) )
      return 247;
    member_count = CParty::get_member_count((CUser **)this);
    if ( (char)CDungeon::get_limit_party_count(a2) < member_count )
      return 243;
    for ( j = 0; j <= 3; ++j )
    {
      if ( (unsigned __int8)CParty::_checkValidUser((CUser **)this, j) == 1
        && *(char *)(CUserCharacInfo::getCurCharacR(*((CUserCharacInfo **)this + 6 * j + 30)) + 3778) <= 0
        && *((_DWORD *)this + 822) != 1 )
      {
        return 245;
      }
    }
  }
  return 0;
}

int __cdecl CParty::do_after_dungeon_start(CParty *this, const CDungeon *dungeon, CUser *user)
{
  int id; // ebx
  unsigned int index; // eax
  char *p_s; // edx
  unsigned int id; // ebx
  char *v7; // edi
  char *v8; // edx
  char *String; // eax
  size_t v10; // eax
  size_t v11; // eax
  int CurCharacInvenW; // eax
  char *v13; // edx
  unsigned int id; // ebx
  char *v15; // edi
  char *v16; // edx
  char *v17; // eax
  size_t v18; // eax
  size_t v19; // eax
  CCharacBloodDungeon *CharacExpandData; // eax
  CDataManager *v21; // eax
  int charac_level; // ebx
  CDataManager *v23; // eax
  CCharacBloodDungeon *v24; // eax
  CCharacBloodDungeon *v25; // eax
  char *v26; // ebx
  CCharacBloodDungeon *v27; // eax
  int CurCharacR; // eax
  GameWorld *v30; // eax
  int id; // ebx
  CInventory *CurCharacInvenR; // eax
  char *v34; // ebx
  CInventory *inventory; // eax
  int *v36; // eax
  int id; // ebx
  CInventory *inventory; // eax
  char CurCharacNo; // bl
  CInventory *inventory; // eax
  char flag; // [esp+2Fh] [ebp-2A9h] BYREF
  int id; // [esp+30h] [ebp-2A8h] BYREF
  char s; // [esp+12Fh] [ebp-1A9h] BYREF
  int id; // [esp+130h] [ebp-1A8h] BYREF
  char flag[7]; // [esp+22Fh] [ebp-A9h] BYREF
  int id; // [esp+236h] [ebp-A2h]
  PacketBuf *v48[3]; // [esp+26Ch] [ebp-6Ch] BYREF
  PacketBuf *v49[3]; // [esp+278h] [ebp-60h] BYREF
  PacketBuf *v50[3]; // [esp+284h] [ebp-54h] BYREF
  int id[4]; // [esp+290h] [ebp-48h] BYREF
  char *blood_dungeon_admission_fee; // [esp+2A0h] [ebp-38h]
  int EnterCount; // [esp+2A4h] [ebp-34h]
  int LottePoint; // [esp+2A8h] [ebp-30h]
  int PriceAverage; // [esp+2ACh] [ebp-2Ch]
  int id; // [esp+2B0h] [ebp-28h]
  int id; // [esp+2B4h] [ebp-24h]
  int started; // [esp+2B8h] [ebp-20h]
  unsigned __int8 CurRevengeDungeonCount; // [esp+2BFh] [ebp-19h]

  if ( (**(unsigned __int8 (__cdecl ***)(const CDungeon *, CUser *))a2)(a2, a3) != 1 )
    return 173;
  index = CDungeon::get_index(a2);
  CUserCharacInfo::setCurCharacLastPlayDungeonIndex(a3, index);
  if ( *((_BYTE *)a2 + 2207) == 1 )
  {
    if ( *((_DWORD *)this + 822) == 1 )
    {
      p_s = &s;
      v6 = 256;
      if ( ((unsigned __int8)&s & 1) != 0 )
      {
        s = 0;
        p_s = (char *)&v45;
        v6 = 255;
      }
      if ( ((unsigned __int8)p_s & 2) != 0 )
      {
        *(_WORD *)p_s = 0;
        p_s += 2;
        v6 -= 2;
      }
      memset(p_s, 0, 4 * (v6 >> 2));
      v7 = &p_s[4 * (v6 >> 2)];
      v8 = v7;
      if ( (v6 & 2) != 0 )
      {
        *(_WORD *)v7 = 0;
        v8 = v7 + 2;
      }
      if ( (v6 & 1) != 0 )
        *v8 = 0;
      String = (char *)RDARScriptStringManager::findString(
                         (RDARScriptStringManager *)&g_scriptStringManager_,
                         (char *)4,
                         "game_server_msg_110",
                         0);
      OS_API::snprintf((OS_API *)&s, MASK_BYTE, String, 0);
      PacketGuard::PacketGuard((PacketGuard *)v49);
      InterfacePacketBuf::put_header(v49, 0, 12);
      InterfacePacketBuf::put_byte(v49, 0);
      InterfacePacketBuf::put_short(v49, 0);
      InterfacePacketBuf::put_byte(v49, 0);
      v10 = strlen(&s);
      InterfacePacketBuf::put_int(v49, v10);
      v11 = strlen(&s);
      InterfacePacketBuf::put_str(v49, &s, v11);
      InterfacePacketBuf::finalize(v49, 1);
      CUser::Send(a3, (PacketGuard *)v49);
      v3 = 0;
      PacketGuard::~PacketGuard(v49);
      return v3;
    }
    blood_dungeon_admission_fee = (char *)CParty::get_blood_dungeon_admission_fee(this, a3);
    CurCharacInvenW = CUserCharacInfo::getCurCharacInvenW(a3);
    CInventory::use_money(CurCharacInvenW, (int)blood_dungeon_admission_fee, (char *)0x22, 1);
    CUser::SendUpdateItemList(a3, 1, 0, 0);
    v13 = &v42;
    v14 = 256;
    if ( ((unsigned __int8)&v42 & 1) != 0 )
    {
      v42 = 0;
      v13 = (char *)&v43;
      v14 = 255;
    }
    if ( ((unsigned __int8)v13 & 2) != 0 )
    {
      *(_WORD *)v13 = 0;
      v13 += 2;
      v14 -= 2;
    }
    memset(v13, 0, 4 * (v14 >> 2));
    v15 = &v13[4 * (v14 >> 2)];
    v16 = v15;
    if ( (v14 & 2) != 0 )
    {
      *(_WORD *)v15 = 0;
      v16 = v15 + 2;
    }
    if ( (v14 & 1) != 0 )
      *v16 = 0;
    v17 = (char *)RDARScriptStringManager::findString(
                    (RDARScriptStringManager *)&g_scriptStringManager_,
                    (char *)4,
                    "game_server_msg_110",
                    0);
    OS_API::snprintf((OS_API *)&v42, MASK_BYTE, v17, blood_dungeon_admission_fee);
    PacketGuard::PacketGuard((PacketGuard *)v50);
    InterfacePacketBuf::put_header(v50, 0, 12);
    InterfacePacketBuf::put_byte(v50, 0);
    InterfacePacketBuf::put_short(v50, 0);
    InterfacePacketBuf::put_byte(v50, 0);
    v18 = strlen(&v42);
    InterfacePacketBuf::put_int(v50, v18);
    v19 = strlen(&v42);
    InterfacePacketBuf::put_str(v50, &v42, v19);
    InterfacePacketBuf::finalize(v50, 1);
    CUser::Send(a3, (PacketGuard *)v50);
    CharacExpandData = (CCharacBloodDungeon *)CUser::GetCharacExpandData((int)a3, 1u);
    EnterCount = CCharacBloodDungeon::GetEnterCount(CharacExpandData);
    v21 = G_CDataManager();
    LottePoint = ServerParameterScript::GetLottePoint((CDataManager *)((char *)v21 + 104), EnterCount);
    charac_level = CUserCharacInfo::get_charac_level(a3);
    v23 = G_CDataManager();
    PriceAverage = ServerParameterScript::GetPriceAverage((CDataManager *)((char *)v23 + 104), charac_level);
    v24 = (CCharacBloodDungeon *)CUser::GetCharacExpandData((int)a3, 1u);
    CCharacBloodDungeon::IncEnterCount(v24);
    v25 = (CCharacBloodDungeon *)CUser::GetCharacExpandData((int)a3, 1u);
    CCharacBloodDungeon::IncPoint(v25, LottePoint);
    v26 = &blood_dungeon_admission_fee[-PriceAverage];
    v27 = (CCharacBloodDungeon *)CUser::GetCharacExpandData((int)a3, 1u);
    CCharacBloodDungeon::IncUseGold(v27, (int)v26);
    PacketGuard::~PacketGuard(v50);
  }
  if ( (char)CDungeon::get_dimension_possible(a2) > 0 )
  {
    if ( *((_DWORD *)this + 822) == 1 )
      return 0;
    v56 = (char)CDungeon::get_dimension_possible(a2) - 1;
    CurCharacR = CUserCharacInfo::getCurCharacR(a3);
    if ( *(char *)(CurCharacR + v56 + 3769) <= 0
      && *((_DWORD *)this + 822) != 1
      && (unsigned __int8)CUser::isGMUser(a3) != 1 )
    {
      return 173;
    }
    if ( *((_DWORD *)this + 822) != 1 )
    {
      CUserCharacInfo::subDemensionInoutValue(a3, (char)v56);
      v30 = G_GameWorld();
      GameWorld::send_party_info_to_all(v30, (CUser **)this, 1u);
    }
  }
  if ( *((_BYTE *)a2 + 2207) == 2 && *((_DWORD *)this + 822) != 1 )
    CUserCharacInfo::subUltimateInoutValue(a3, 2);
  if ( *((_BYTE *)a2 + 2204) || CDungeon::isTournamentDungeon(a2) )
  {
    if ( *((_DWORD *)this + 822) == 1 )
      return 0;
    if ( *((_DWORD *)a2 + 509) == -1 )
      return 0;
    if ( *((_BYTE *)a2 + 2044) != 1 )
      return 0;
    v57 = 0;
    if ( CDungeon::get_index(a2) == 1500 )
    {
      v32 = *((_DWORD *)a2 + 509);
      CurCharacInvenR = (CInventory *)CUserCharacInfo::getCurCharacInvenR(a3);
      v57 = CInventory::check_vilmark_dungeon_item_exist(CurCharacInvenR, v32);
      if ( v57 == -1 )
        return 17;
    }
    else
    {
      v34 = (char *)*((_DWORD *)a2 + 509);
      v35 = (CInventory *)CUserCharacInfo::getCurCharacInvenR(a3);
      v57 = CInventory::check_item_exist(v35, v34);
      if ( v57 == -1 )
        return 17;
    }
    v36 = (int *)CUserCharacInfo::getCurCharacInvenR(a3);
    CInventory::GetInvenSlot((CInventory *)v46, v36, 1, v57);
    if ( v47 < *((_DWORD *)a2 + 510) )
      return 17;
    v37 = *((_DWORD *)a2 + 510);
    v38 = (CInventory *)CUserCharacInfo::getCurCharacInvenW(a3);
    if ( (unsigned __int8)CInventory::delete_item(v38, 1, v57, v37, 14, 1) != 1 )
    {
      CurCharacNo = CUserCharacInfo::getCurCharacNo(a3);
      cMyTrace::cMyTrace((cMyTrace *)v51, "int CParty::do_after_dungeon_start(const CDungeon*, CUser*)", 3430, 5);
      cMyTrace::operator()(
        (int)v51,
        "CParty::UseAncientDungeonItems, delete_item failed , User ch=%d , %d %d",
        CurCharacNo);
      return 17;
    }
    PacketGuard::PacketGuard((PacketGuard *)v48);
    InterfacePacketBuf::put_header(v48, 0, 14);
    InterfacePacketBuf::put_byte(v48, 0);
    InterfacePacketBuf::put_short(v48, 1);
    v40 = (CInventory *)CUserCharacInfo::getCurCharacInvenR(a3);
    CInventory::MakeItemPacket(v40, 1, v57, v48);
    InterfacePacketBuf::finalize(v48, 1);
    CUser::Send(a3, (PacketGuard *)v48);
    PacketGuard::~PacketGuard(v48);
  }
  if ( !*((_BYTE *)a2 + 2140) )
  {
    if ( *((_BYTE *)a2 + 2170) )
    {
      CurRevengeDungeonCount = CUserCharacInfo::GetCurRevengeDungeonCount(a3);
      if ( !CurRevengeDungeonCount || *((_BYTE *)a2 + 2171) < CurRevengeDungeonCount )
      {
        CUserCharacInfo::SetCurRevengeDungeonCount(a3, 0);
        return 17;
      }
      CUserCharacInfo::DecreaseRevengeDungeonTicket(a3);
      CUserCharacInfo::VillageAttack_DBUpdate(a3);
    }
    return 0;
  }
  if ( !CDungeon::IsOpenScheduleDungeon(a2) )
    return 21;
  if ( !a3 )
    return 0;
  if ( *((_DWORD *)a2 + 536) == -1 && *((_DWORD *)a2 + 509) == -1 )
    return 0;
  if ( *((_DWORD *)a2 + 536) != -1 && *((_BYTE *)a2 + 2152) != 1 )
    return 0;
  if ( *((_DWORD *)a2 + 509) != -1 && *((_BYTE *)a2 + 2044) != 1 )
    return 0;
  started = 17;
  if ( *((_DWORD *)a2 + 536) == -1
    || (started = CParty::do_after_dungeon_start_checkndelete_item(this, (const CDungeon *)((char *)a2 + 2144), a3)) != 0 )
  {
    if ( *((_DWORD *)a2 + 509) != -1 )
    {
      started = CParty::do_after_dungeon_start_checkndelete_item(this, (const CDungeon *)((char *)a2 + 2036), a3);
      if ( !started )
        CUser::SetDeleteEventDungeonInvitation(a3, 1);
    }
    return started;
  }
  else
  {
    CUser::SetDeleteEventDungeonInvitation(a3, 1);
    return started;
  }
}

int __cdecl CParty::do_after_dungeon_start_checkndelete_item(CParty *this, const RequiredItem *a2, CUser *user)
{
  char *v3; // ebx
  CInventory *CurCharacInvenR; // eax
  int slot; // ebx
  int *v6; // eax
  int slot; // ebx
  CInventory *CurCharacInvenW; // eax
  char CurCharacNo; // bl
  CInventory *inventory; // eax
  _BYTE v12[7]; // [esp+23h] [ebp-65h] BYREF
  int slot; // [esp+2Ah] [ebp-5Eh]
  PacketBuf *v14[3]; // [esp+60h] [ebp-28h] BYREF
  int slot[4]; // [esp+6Ch] [ebp-1Ch] BYREF
  int slot; // [esp+7Ch] [ebp-Ch]

  v16 = 0;
  v3 = *(char **)a2;
  CurCharacInvenR = (CInventory *)CUserCharacInfo::getCurCharacInvenR(a3);
  v16 = CInventory::check_item_exist(CurCharacInvenR, v3);
  if ( v16 == -1 )
    return 17;
  v6 = (int *)CUserCharacInfo::getCurCharacInvenR(a3);
  CInventory::GetInvenSlot((CInventory *)v12, v6, 1, v16);
  if ( v13 < *((_DWORD *)a2 + 1) )
    return 17;
  v7 = *((_DWORD *)a2 + 1);
  CurCharacInvenW = (CInventory *)CUserCharacInfo::getCurCharacInvenW(a3);
  if ( (unsigned __int8)CInventory::delete_item(CurCharacInvenW, 1, v16, v7, 14, 1) != 1 )
  {
    CurCharacNo = CUserCharacInfo::getCurCharacNo(a3);
    cMyTrace::cMyTrace(
      (cMyTrace *)v15,
      "int CParty::do_after_dungeon_start_checkndelete_item(const RequiredItem&, CUser*)",
      3578,
      5);
    cMyTrace::operator()((int)v15, "CParty::isEventDungeon, delete_item failed , User ch=%d , %d %d", CurCharacNo);
    return 17;
  }
  else
  {
    PacketGuard::PacketGuard((PacketGuard *)v14);
    InterfacePacketBuf::put_header(v14, 0, 14);
    InterfacePacketBuf::put_byte(v14, 0);
    InterfacePacketBuf::put_short(v14, 1);
    v10 = (CInventory *)CUserCharacInfo::getCurCharacInvenR(a3);
    CInventory::MakeItemPacket(v10, 1, v16, v14);
    InterfacePacketBuf::finalize(v14, 1);
    CUser::Send(a3, (PacketGuard *)v14);
    v5 = 0;
    PacketGuard::~PacketGuard(v14);
  }
  return v5;
}

int __cdecl CParty::dungeon_start(CParty *party, char *a2, char flag, int id)
{
  int id; // ebx
  char *v5; // ebx
  CDungeonList **v6; // eax
  int PartyMemberCoinLimit; // ebx
  int member_count; // eax
  CUser *user; // ebx
  char *HackAnalyzer; // eax
  int id; // eax
  __int16 v13; // bx
  char *DungeonClearRefR; // eax
  int index; // eax
  char flag; // di
  CWorldMap *v17; // esi
  int id; // eax
  int RandomBuffDungeonType; // eax
  BestClearTime *CharacExpandData; // eax
  int id; // edi
  char *MemberNames; // ebx
  char *DungeonName; // eax
  CUserCharacInfo **CurCharacSkillW; // eax
  unsigned int id; // esi
  CUser *user; // ebx
  char *v27; // eax
  int CurCharFirstGrowType; // esi
  int charac_job; // ebx
  CInventory *CurCharacInvenW; // eax
  CUser *user; // ebx
  char *v32; // eax
  unsigned int id; // esi
  CUser *user; // ebx
  char *v35; // eax
  CUser *user; // ebx
  char *v37; // eax
  CUser *user; // ebx
  char *v39; // eax
  CUser *user; // ebx
  char *v41; // eax
  int id; // edx
  int id; // esi
  int standard_level; // ebx
  int id; // eax
  int RepeatEvent; // eax
  char *v48; // ebx
  char *CurCharacQuestW; // eax
  unsigned int TournamentRoundFatigue; // eax
  int id; // ebx
  int id; // ebx
  int MemberMaxLevelBeforeDungeon; // ebx
  unsigned int connect_upper_member; // eax
  int CurCharacGrowType; // esi
  int charac_level; // ebx
  _IO_FILE *v61; // eax
  CUser *Manager; // eax
  char *v63; // ebx
  CDungeonEntranceLog *v64; // eax
  CDungeonEntranceLog *InstanceDungeonEntranceLog; // eax
  CInventory *CurCharacInvenR; // eax
  char *CreatureMgrW; // eax
  __int16 v68; // ax
  char *Hades; // eax
  int id; // eax
  char flag; // [esp+3Ch] [ebp-1DCh]
  char s[256]; // [esp+4Ch] [ebp-1CCh] BYREF
  int id; // [esp+14Ch] [ebp-CCh] BYREF
  int id; // [esp+150h] [ebp-C8h] BYREF
  int id; // [esp+154h] [ebp-C4h] BYREF
  int id; // [esp+158h] [ebp-C0h] BYREF
  int id; // [esp+15Ch] [ebp-BCh] BYREF
  int id; // [esp+160h] [ebp-B8h] BYREF
  int id; // [esp+164h] [ebp-B4h] BYREF
  int id[7]; // [esp+168h] [ebp-B0h] BYREF
  int id[2]; // [esp+184h] [ebp-94h] BYREF
  unsigned int id; // [esp+18Ch] [ebp-8Ch] BYREF
  char flag; // [esp+193h] [ebp-85h] BYREF
  PacketBuf *v85[3]; // [esp+194h] [ebp-84h] BYREF
  int id; // [esp+1A0h] [ebp-78h] BYREF
  int isTOD; // [esp+1A4h] [ebp-74h]
  CDungeon *dungeon; // [esp+1A8h] [ebp-70h]
  int id; // [esp+1ACh] [ebp-6Ch]
  int min_level; // [esp+1B0h] [ebp-68h]
  int id; // [esp+1B4h] [ebp-64h]
  int id; // [esp+1BCh] [ebp-5Ch]
  char isPremiumGoldCardReward; // [esp+1C3h] [ebp-55h]
  int i; // [esp+1C4h] [ebp-54h]
  char ClearedDungeonDiff; // [esp+1CBh] [ebp-4Dh]
  int j; // [esp+1CCh] [ebp-4Ch]
  int k; // [esp+1D0h] [ebp-48h]
  int id; // [esp+1D4h] [ebp-44h]
  int m; // [esp+1D8h] [ebp-40h]
  int n; // [esp+1DCh] [ebp-3Ch]
  int ii; // [esp+1E0h] [ebp-38h]
  int jj; // [esp+1E4h] [ebp-34h]
  int kk; // [esp+1E8h] [ebp-30h]
  int mm; // [esp+1ECh] [ebp-2Ch]
  int nn; // [esp+1F0h] [ebp-28h]
  int i1; // [esp+1F4h] [ebp-24h]
  int i2; // [esp+1F8h] [ebp-20h]
  int i3; // [esp+1FCh] [ebp-1Ch]

  v72 = a3;
  CParty::resetMemberUseCoin(a1);
  *((_BYTE *)a1 + 804) = 0;
  CParty::ClearMapHitCount(a1);
  Secu_HackLogCheckByParty::startDungeon((CParty *)((char *)a1 + 6844));
  v86 = -1;
  isTOD = 0;
  isTOD = CParty::GetDungeonIndex_isTOD(a1, (int *)&a2, &v86);
  if ( !isTOD )
  {
    v5 = a2;
    v6 = (CDungeonList **)G_CDataManager();
    dungeon = (CDungeon *)CDataManager::find_dungeon(v6, (int)v5);
    if ( !dungeon )
      return 21;
    PacketGuard::PacketGuard((PacketGuard *)v85);
    v89 = *((_DWORD *)a1 + 822);
    PartyMemberCoinLimit = CDungeon::GetPartyMemberCoinLimit(dungeon);
    member_count = CParty::get_member_count((CUser **)a1);
    CParty::SetPartyMemberCoinLimit(a1, PartyMemberCoinLimit * member_count);
    CTraceMobDieHack::setDungeonDiff((CParty *)((char *)a1 + 528), v72);
    if ( (unsigned __int8)CParty::GetEPLPState(a1) )
    {
      for ( i = 0; i <= 3; ++i )
      {
        if ( (unsigned __int8)CParty::_checkValidUser((CUser **)a1, i) == 1 )
        {
          v9 = (CUser *)*((_DWORD *)a1 + 6 * i + 30);
          HackAnalyzer = CUser::getHackAnalyzer(v9);
          WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)HackAnalyzer, v9, 0x195u, 1, 0, 0);
        }
      }
      v4 = 8;
      goto LABEL_150;
    }
    if ( (unsigned __int8)IsLightServer() )
      CDungeon::get_min_level(dungeon);
    min_level = CDungeon::get_min_level(dungeon);
    if ( (unsigned __int8)CUser::IsGameMasterMode(*((CUser **)a1 + 29)) != 1
      && (v11 = CUserCharacInfo::get_charac_level(*((CUserCharacInfo **)a1 + 29)), v11 < min_level) )
    {
      v4 = 14;
    }
    else
    {
      if ( CParty::checkDungeonDifficulty((CUser **)a1, v72, dungeon) )
      {
        ClearedDungeonDiff = 0;
        v13 = (__int16)a2;
        DungeonClearRefR = CUserCharacInfo::getDungeonClearRefR(*((CUserCharacInfo **)a1 + 29));
        ClearedDungeonDiff = WongWork::CDungeonClear::getClearedDungeonDiff(
                               (WongWork::CDungeonClear *)DungeonClearRefR,
                               v13);
        if ( !ClearedDungeonDiff )
        {
          v4 = 19;
          goto LABEL_150;
        }
        if ( ClearedDungeonDiff < v72 && v72 != 4 )
        {
          v4 = 19;
          goto LABEL_150;
        }
      }
      v91 = 0;
      for ( j = 0; j <= 3; ++j )
      {
        if ( (unsigned __int8)CParty::_checkValidUser((CUser **)a1, j) == 1 && v89 != 1 )
        {
          index = CDungeon::get_index(dungeon);
          v91 = CUser::CheckQuestMap(*((CUser **)a1 + 6 * j + 30), index);
          if ( v91 > 0 )
            break;
        }
      }
      CParty::SetUsedCoinCount(a1, 0);
      if ( (unsigned __int8)CParty::CheckHaveHellPartyPassItem(a1, dungeon, a4) != 1 )
      {
        v4 = 0;
      }
      else
      {
        v84 = 0;
        if ( CParty::checkInoutConditionDungeon((CUser **)a1, dungeon, v89, 16) )
        {
          v4 = 0;
        }
        else
        {
          CBattle_Field::check_random_appear_hell_dungeon((CParty **)a1 + 713, dungeon, v89, a4, &v84);
          v16 = v84;
          v17 = (CWorldMap *)*((_DWORD *)a1 + 1562);
          v18 = CParty::get_member_count((CUser **)a1);
          if ( (unsigned __int8)CBattle_Field::SelectDungeon(
                                  (int)v17,
                                  (CParty *)((char *)a1 + 2852),
                                  a2,
                                  dungeon,
                                  v72,
                                  a4,
                                  v18,
                                  v17,
                                  v16,
                                  v91) != 1 )
          {
            v4 = 1;
          }
          else
          {
            RandomBuffDungeonType = CBattle_Field::getRandomBuffDungeonType((CParty *)((char *)a1 + 2852), 0);
            CParty::choose_random_buff(a1, RandomBuffDungeonType);
            memset(s, 0, sizeof(s));
            CParty::_getMemberNames((CUserCharacInfo **)a1, s);
            for ( k = 0; k <= 3; ++k )
            {
              if ( (unsigned __int8)CParty::_checkValidUser((CUser **)a1, k) == 1 )
              {
                CharacExpandData = (BestClearTime *)CUser::GetCharacExpandData(*((_DWORD *)a1 + 6 * k + 30), 0x12u);
                BestClearTime::setNewRecord(CharacExpandData, 0);
                v21 = v89;
                MemberNames = CParty::_getMemberNames((CUserCharacInfo **)a1, s);
                DungeonName = CDungeon::GetDungeonName((std::string *)dungeon);
                cUserHistoryLog::EnterDungeon(
                  (CUser **)(*((_DWORD *)a1 + 6 * k + 30) + CUSER_OFFSET_HISTORY_LOG),
                  DungeonName,
                  v72,
                  MemberNames,
                  v21);
                CUser::resetDungeonClearHackCheck(*((CUser **)a1 + 6 * k + 30));
                if ( !CUser::getMoveSpace(*((CUser **)a1 + 6 * k + 30)) )
                {
                  v80 = 0;
                  CurCharacSkillW = (CUserCharacInfo **)CUserCharacInfo::getCurCharacSkillW(*((CUserCharacInfo **)a1
                                                                                            + 6 * k
                                                                                            + 30));
                  v99 = SkillSlot::checkskillSlotEmptyCount(CurCharacSkillW, &v80);
                  for ( m = 0; m < v99; ++m )
                  {
                    v25 = CUser::SetETC(*((CUser **)a1 + 6 * k + 30), 422, v80);
                    v26 = (CUser *)*((_DWORD *)a1 + 6 * k + 30);
                    v27 = CUser::getHackAnalyzer(v26);
                    WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)v27, v26, 0x1A6u, 1, v25, 0);
                  }
                  v79 = 0;
                  v78 = 0;
                  v77 = 0;
                  v76 = 0;
                  v75 = 0;
                  v74 = 0;
                  CurCharFirstGrowType = CUserCharacInfo::getCurCharFirstGrowType(*((CUserCharacInfo **)a1 + 6 * k + 30));
                  charac_job = CUserCharacInfo::get_charac_job(*((CUserCharacInfo **)a1 + 6 * k + 30));
                  CurCharacInvenW = (CInventory *)CUserCharacInfo::getCurCharacInvenW(*((CUserCharacInfo **)a1
                                                                                      + 6 * k
                                                                                      + 30));
                  CInventory::checkEquipmentState(
                    CurCharacInvenW,
                    charac_job,
                    CurCharFirstGrowType,
                    &v79,
                    &v78,
                    &v77,
                    &v76,
                    &v75,
                    &v74);
                  if ( CUserCharacInfo::get_charac_level(*((CUserCharacInfo **)a1 + 6 * k + 30)) > 15 )
                  {
                    for ( n = 0; n < v79; ++n )
                    {
                      v31 = (CUser *)*((_DWORD *)a1 + 6 * k + 30);
                      v32 = CUser::getHackAnalyzer(v31);
                      WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)v32, v31, 0x1A4u, 1, 0, 0);
                    }
                  }
                  if ( CUserCharacInfo::get_charac_level(*((CUserCharacInfo **)a1 + 6 * k + 30)) > 23 )
                  {
                    for ( ii = 0; ii < v78; ++ii )
                    {
                      v33 = CUser::SetETC(*((CUser **)a1 + 6 * k + 30), 421, v77);
                      v34 = (CUser *)*((_DWORD *)a1 + 6 * k + 30);
                      v35 = CUser::getHackAnalyzer(v34);
                      WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)v35, v34, 0x1A5u, 1, v33, 0);
                    }
                  }
                  for ( jj = 0; jj < v76; ++jj )
                  {
                    v36 = (CUser *)*((_DWORD *)a1 + 6 * k + 30);
                    v37 = CUser::getHackAnalyzer(v36);
                    WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)v37, v36, 0x1A7u, 1, 0, 0);
                  }
                  for ( kk = 0; kk < v75; ++kk )
                  {
                    v38 = (CUser *)*((_DWORD *)a1 + 6 * k + 30);
                    v39 = CUser::getHackAnalyzer(v38);
                    WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)v39, v38, 0x1A8u, 1, 0, 0);
                  }
                  for ( mm = 0; mm < v74; ++mm )
                  {
                    v40 = (CUser *)*((_DWORD *)a1 + 6 * k + 30);
                    v41 = CUser::getHackAnalyzer(v40);
                    WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)v41, v40, 0x1A9u, 1, 0, 0);
                  }
                }
              }
            }
            if ( !*((_DWORD *)a1 + 1562)
              || !(unsigned __int8)CWorldMap::IsInHellDungeon(*((CWorldMap **)a1 + 1562))
              || a4 != 1
              || !*((_BYTE *)a1 + 6245)
              || (unsigned __int8)CParty::UseSealDoorItems(a1, (_DWORD *)(*((_DWORD *)a1 + 1562) + 52)) )
            {
              if ( *((char *)dungeon + 2207) <= 0 )
              {
                if ( CDungeon::isTournamentDungeon(dungeon) )
                {
                  InterfacePacketBuf::clear(v85);
                  InterfacePacketBuf::put_header(v85, 0, 386);
                  InterfacePacketBuf::put_short(v85, (__int16)a2);
                  InterfacePacketBuf::put_byte(v85, v72);
                  CBattle_Field::MakePacket((CParty *)((char *)a1 + 2852), 386, (InterfacePacketBuf *)v85);
                  CParty::send_to_party((CUser **)a1, (PacketGuard *)v85);
                  InterfacePacketBuf::clear(v85);
                  CBattle_Field::MakePacket((CParty *)((char *)a1 + 2852), 387, (InterfacePacketBuf *)v85);
                  CParty::send_to_party((CUser **)a1, (PacketGuard *)v85);
                }
                else
                {
                  InterfacePacketBuf::clear(v85);
                  InterfacePacketBuf::put_header(v85, 0, 28);
                  InterfacePacketBuf::put_short(v85, (__int16)a2);
                  InterfacePacketBuf::put_byte(v85, v72);
                  CBattle_Field::MakePacket((CParty *)((char *)a1 + 2852), 28, (InterfacePacketBuf *)v85);
                  CParty::send_to_party((CUser **)a1, (PacketGuard *)v85);
                  InterfacePacketBuf::clear(v85);
                  CBattle_Field::MakePacket((CParty *)((char *)a1 + 2852), 29, (InterfacePacketBuf *)v85);
                  CParty::send_to_party((CUser **)a1, (PacketGuard *)v85);
                  CParty::_Send_TOD_APCInfo((CDungeon **)a1, v86);
                }
              }
              else
              {
                InterfacePacketBuf::clear(v85);
                InterfacePacketBuf::put_header(v85, 0, 212);
                InterfacePacketBuf::put_short(v85, (__int16)a2);
                InterfacePacketBuf::put_byte(v85, v72);
                CBattle_Field::MakePacket((CParty *)((char *)a1 + 2852), 212, (InterfacePacketBuf *)v85);
                CParty::send_to_party((CUser **)a1, (PacketGuard *)v85);
                InterfacePacketBuf::clear(v85);
                CBattle_Field::MakePacket((CParty *)((char *)a1 + 2852), 211, (InterfacePacketBuf *)v85);
                CParty::send_to_party((CUser **)a1, (PacketGuard *)v85);
                if ( (unsigned __int8)CBattle_Field::setBloodState((int)a1 + 2852, 2) != 1 )
                {
                  v4 = 1;
                  goto LABEL_150;
                }
              }
              *((_DWORD *)a1 + 211) = CSystemTime::getCurTickCount((CSystemTime *)GlobalData::s_systemTime_);
              *((_DWORD *)a1 + 212) = 0;
              *((_DWORD *)a1 + 207) = 0;
              *((_DWORD *)a1 + 208) = 0;
              *((_DWORD *)a1 + 209) = 0;
              *((_DWORD *)a1 + 210) = 0;
              *((_DWORD *)a1 + 214) = 0;
              *((_DWORD *)a1 + 215) = 0;
              v42 = *((_DWORD *)a1 + 212);
              *((_DWORD *)a1 + 216) = *((_DWORD *)a1 + 211);
              *((_DWORD *)a1 + 217) = v42;
              CParty::set_state(a1, 2);
              CParty::SetEPLPState(a1, 1);
              CParty::gen_timer_key((int)a1, 45);
              CParty::SetSelectedEPLPCmd(a1, -1);
              v93 = CParty::get_member_count((CUser **)a1);
              CParty::setStartGamePartyCount(a1, v93);
              if ( *((char *)dungeon + 2207) > 0
                || CUser::get_state(*((CUser **)a1 + 29)) == 10
                || CDungeon::isTowerOfDespairDungeon(dungeon)
                || CDungeon::isTournamentDungeon(dungeon) )
              {
                *((_DWORD *)a1 + 169) = STATIC_POOL_SIZE_1000;
              }
              else
              {
                v44 = CParty::get_member_count((CUser **)a1);
                standard_level = CDungeon::get_standard_level(*((CDungeon **)a1 + 811));
                v46 = CDungeon::get_index(*((CDungeon **)a1 + 811));
                *((_DWORD *)a1 + 169) = secretshop::CSecretShop::LotteryNpc(
                                          (unsigned int ***)GlobalData::s_secret_shop,
                                          v46,
                                          standard_level,
                                          v44);
              }
              isPremiumGoldCardReward = 0;
              v83 = 0;
              RepeatEvent = CEventManager::GetRepeatEvent(GlobalData::s_event_manager, 65);
              if ( (*(unsigned __int8 (__cdecl **)(int, _DWORD))(*(_DWORD *)RepeatEvent + 52))(RepeatEvent, 0) )
              {
                if ( (unsigned __int8)CParty::IsReturnUserParty((CUser **)a1) )
                {
                  isPremiumGoldCardReward = CParty::isPremiumGoldCardReward(a1, *((const CDungeon **)a1 + 811), &v83);
                  if ( isPremiumGoldCardReward )
                  {
                    CParty::SetPremiumGoldCardParty(a1);
                    CParty::SetPremiumGoldCardDefaultItem(a1, v83);
                  }
                }
              }
              CPartyResultRecvFlag::Clear((CParty *)((char *)a1 + 476));
              for ( nn = 0; nn <= 3; ++nn )
              {
                for ( i1 = 0; i1 <= 1; ++i1 )
                  *((_BYTE *)a1 + 4 * i1 + nn + 516) = -1;
                *((_BYTE *)a1 + 480) = 0;
                if ( (unsigned __int8)CParty::_checkValidUser((CUser **)a1, nn) == 1 )
                {
                  secretshop::CSecretShop::SendSecretShopNpc(
                    GlobalData::s_secret_shop,
                    *((CUser **)a1 + 6 * nn + 30),
                    *((_DWORD *)a1 + 169));
                  v48 = a2;
                  CurCharacQuestW = CUser::getCurCharacQuestW(*((CUser **)a1 + 6 * nn + 30));
                  UserQuest::set_authen_data((int)CurCharacQuestW, 0, (int)v48, v72);
                  if ( dungeon )
                    CParty::SetProperLevelDungeon((CDungeon **)a1, *((CUser **)a1 + 6 * nn + 30));
                  if ( v89 != 1
                    && *((_BYTE *)dungeon + 2170) != 1
                    && !*((_BYTE *)dungeon + 2207)
                    && a2 != (char *)MAX_LIMIT_10000
                    && (unsigned __int8)CDungeon::isNoFatigueDungeon(dungeon) != 1 )
                  {
                    if ( CDungeon::isTournamentDungeon(dungeon) )
                    {
                      TournamentRoundFatigue = CDungeon::getTournamentRoundFatigue(dungeon);
                      CUser::FatigueUp(*((CUser **)a1 + 6 * nn + 30), TournamentRoundFatigue);
                      CUser::SendFatigue(*((CUser **)a1 + 6 * nn + 30));
                    }
                    else
                    {
                      CUser::FatigueUp(*((CUser **)a1 + 6 * nn + 30), 0);
                    }
                  }
                  CUser::setPlayingDungeonIndex(*((CUser **)a1 + 6 * nn + 30), (int)a2);
                  PlayInfo::Reset((PlayInfo *)(*((_DWORD *)a1 + 6 * nn + 30) + 581684));
                  *(_DWORD *)(*((_DWORD *)a1 + 6 * nn + 30) + 581716) = v93;
                  *(_BYTE *)(*((_DWORD *)a1 + 6 * nn + 30) + 581748) = 0;
                  v52 = CDungeon::get_standard_level(*((CDungeon **)a1 + 811)) - 5;
                  if ( v52 <= CUserCharacInfo::get_charac_level(*((CUserCharacInfo **)a1 + 6 * nn + 30)) )
                  {
                    v53 = CDungeon::get_standard_level(*((CDungeon **)a1 + 811)) + 3;
                    if ( v53 >= CUserCharacInfo::get_charac_level(*((CUserCharacInfo **)a1 + 6 * nn + 30)) )
                    {
                      *((_BYTE *)a1 + 852) = 1;
                      *((_BYTE *)a1 + 853) = 1;
                    }
                  }
                  MemberMaxLevelBeforeDungeon = CParty::getMemberMaxLevelBeforeDungeon((CUserCharacInfo **)a1);
                  if ( MemberMaxLevelBeforeDungeon
                     - CUserCharacInfo::get_charac_level_before_dungeon(*((CUserCharacInfo **)a1 + 6 * nn + 30)) > 6
                    && v93 != 1 )
                  {
                    *((_BYTE *)a1 + 854) = 1;
                    *((_BYTE *)a1 + 855) = !(unsigned __int8)CParty::isGuildParty((CUserCharacInfo **)a1)
                                        && (connect_upper_member = CUserCharacInfo::get_connect_upper_member(*((CUserCharacInfo **)a1 + 6 * nn + 30)),
                                            !CParty::FindUserByCharId(a1, connect_upper_member));
                  }
                  else
                  {
                    *((_BYTE *)a1 + 854) = 0;
                    *((_BYTE *)a1 + 855) = 0;
                  }
                  if ( (char)CDungeon::get_dimension_possible(*((CDungeon **)a1 + 811)) > 0 )
                  {
                    *((_BYTE *)a1 + 852) = 1;
                    *((_BYTE *)a1 + 853) = 1;
                  }
                  CurCharacGrowType = (char)CUserCharacInfo::getCurCharacGrowType(*((CUserCharacInfo **)a1 + 6 * nn + 30));
                  charac_level = CUserCharacInfo::get_charac_level(*((CUserCharacInfo **)a1 + 6 * nn + 30));
                  v61 = (_IO_FILE *)CUserCharacInfo::get_charac_job(*((CUserCharacInfo **)a1 + 6 * nn + 30));
                  HistoryLog::WriteDunStart(
                    *(FILE **)(*((_DWORD *)a1 + 6 * nn + 30) + 497400),
                    v61,
                    charac_level,
                    CurCharacGrowType);
                  Manager = (CUser *)CParty::getManager(a1);
                  if ( CUser::getMoveSpace(Manager) != 1 )
                  {
                    v63 = a2;
                    if ( v89 == 1 )
                    {
                      InstanceDungeonEntranceLog = (CDungeonEntranceLog *)GetInstanceDungeonEntranceLog();
                      CDungeonEntranceLog::IncrementDungeonEntrance(InstanceDungeonEntranceLog, (int)v63, 1);
                    }
                    else
                    {
                      v64 = (CDungeonEntranceLog *)GetInstanceDungeonEntranceLog();
                      CDungeonEntranceLog::IncrementDungeonEntrance(v64, (int)v63, 0);
                    }
                  }
                  if ( v89 != 1 )
                    CUserCharacInfo::IncCurCharacDungeonPlayCount(*((CUserCharacInfo **)a1 + 6 * nn + 30));
                  if ( v89 == 1 )
                  {
                    CurCharacInvenR = (CInventory *)CUserCharacInfo::getCurCharacInvenR(*((CUserCharacInfo **)a1
                                                                                        + 6 * nn
                                                                                        + 30));
                    CreatureMgrW = CInventory::GetCreatureMgrW(CurCharacInvenR);
                    user_creature::CCreatureMgr::TurnStomach((user_creature::CCreatureMgr *)CreatureMgrW, 0);
                  }
                  v68 = CUserCharacInfo::get_charac_level(*((CUserCharacInfo **)a1 + 6 * nn + 30));
                  CUserCharacInfo::set_level_before_dungeon(*((CUserCharacInfo **)a1 + 6 * nn + 30), v68);
                  if ( dungeon )
                    CParty::SetProperDungeonEvent((CDungeon **)a1, *((CUser **)a1 + 6 * nn + 30));
                  CParty::SetEndRouting(a1);
                  Hades = CUser::getHades(*((CUser **)a1 + 6 * nn + 30));
                  XNuclear::CHades::StdCateUpdate((CUser **)Hades, 2u, 0);
                }
              }
              for ( i2 = 0; i2 <= 1; ++i2 )
              {
                for ( i3 = 0; i3 <= 3; ++i3 )
                  *((_DWORD *)a1 + 4 * i2 + i3 + 121) = 0;
              }
              CParty::Reset_party_overlapped_drop_ratio(a1);
              CParty::check_passive_skill();
              CParty::send_loadingcheck_msg(a1);
              v70 = CEventManager::GetRepeatEvent(GlobalData::s_event_manager, 87);
              if ( (*(unsigned __int8 (__cdecl **)(int, _DWORD))(*(_DWORD *)v70 + 52))(v70, 0) )
                CParty::SetWeekendEvent(a1, 1);
              else
                CParty::SetWeekendEvent(a1, 0);
              QuickParty::RewardData_Param::RewardData_Param((QuickParty::RewardData_Param *)v82, a1);
              QuickParty::RewardData_Result::RewardData_Result((QuickParty::RewardData_Result *)v81);
              CParty::set_random_reward(a1, 9u, v82[0], v82[1], (int)v81);
              CParty::SendPartyMemberTagChracInfo(a1);
              v4 = 0;
              goto LABEL_150;
            }
            v4 = 1;
          }
        }
      }
    }
LABEL_150:
    PacketGuard::~PacketGuard(v85);
    return v4;
  }
  return isTOD;
}

int __cdecl CParty::FindUserByCharId(CParty *this, unsigned int id)
{
  int i; // [esp+1Ch] [ebp-Ch]

  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::_checkValidUser((CUser **)this, i) == 1
      && CUser::get_charac_no(*((CUser **)this + 6 * i + 30), -1) == a2 )
    {
      return *((_DWORD *)this + 6 * i + 30);
    }
  }
  return 0;
}

int CParty::isHelpAbuseParty()
{
  return 0;
}

int __cdecl CParty::isHelpAbuseParty(CParty *this)
{
  int i; // [esp+18h] [ebp-10h]

  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::checkValidUser((CUser **)this, i) == 1 )
    {
      CParty::get_user(this, i);
      if ( (unsigned __int8)CParty::isHelpAbuseParty() )
        return 1;
    }
  }
  return 0;
}

int __cdecl CParty::SelectEPLPCommand(CParty *this, int id)
{
  int id; // esi
  int PartyIndex; // ebx
  TimerQueue *v4; // eax

  CParty::SetEPLPState(this, 3);
  v2 = CParty::gen_timer_key((int)this, 20);
  PartyIndex = CParty::GetPartyIndex(this);
  v4 = G_TimerQueue();
  return TimerQueue::InsertTimer(v2, v4, 1, PartyIndex, 20, a2, v2, 0);
}

int __cdecl CParty::GetExpPenaltyPercent(CParty *this, int id, int id2)
{
  unsigned int id; // [esp+8h] [ebp-8h]
  unsigned int id; // [esp+Ch] [ebp-4h]

  if ( a2 > 19 )
  {
    if ( a2 < a3 )
      return 100 * a2 / a3;
    v5 = a2 - a3;
    if ( (unsigned int)(a2 - a3) > 5 && v5 <= 9 )
      return 20 * (10 - v5);
    if ( a2 - a3 > 9 )
      return 5;
  }
  else
  {
    v4 = abs32(a2 - a3);
    if ( v4 > 5 && v4 <= 9 )
      return 20 * (10 - v4);
    if ( v4 > 9 )
      return 5;
  }
  return 100;
}

int __cdecl CParty::getMonsterTotalExp(CParty *this, unsigned int id, char flag, int id2, float a5, float a6, bool a7)
{
  char *v9; // [esp+24h] [ebp-14h]
  float v10; // [esp+28h] [ebp-10h]

  v9 = (char *)G_CDataManager() + 104;
  if ( a3 > 3 )
    v10 = 1.0;
  else
    v10 = *(float *)&v9[4 * a3 + 1008];
  if ( a7 )
    v10 = v10 * 3.0;
  return (__int64)((long double)a2 / 2.0 * a6 * *(float *)&v9[4 * a4 + 920] * a5 * v10);
}

int __cdecl CParty::sumGainedEachExp(CParty *this, CUser *user, const STGainedEachExpData *a3)
{
  int RepeatEvent; // eax
  CDataManager *v4; // eax
  int AddExpRateFromPremium; // eax
  int id; // eax
  char *PremiumInfo; // eax
  int id; // [esp+2Ch] [ebp-4Ch] BYREF
  int id; // [esp+3Ch] [ebp-3Ch] BYREF
  int id; // [esp+40h] [ebp-38h]
  unsigned int id; // [esp+44h] [ebp-34h]
  float v13; // [esp+48h] [ebp-30h]
  int id; // [esp+4Ch] [ebp-2Ch]
  int id; // [esp+50h] [ebp-28h]
  int WeekendBonusExp; // [esp+54h] [ebp-24h]
  float AdvantageExpRate; // [esp+58h] [ebp-20h]
  int id; // [esp+5Ch] [ebp-1Ch]
  int id; // [esp+60h] [ebp-18h]
  int id; // [esp+64h] [ebp-14h]
  float v21; // [esp+6Ch] [ebp-Ch]

  v11 = 0;
  v12 = *(_DWORD *)a3;
  v13 = *((float *)a3 + 1);
  v14 = 0;
  v15 = 0;
  WeekendBonusExp = 0;
  if ( (unsigned __int8)CBattle_Field::isWorldMapTowerArea((CParty *)((char *)this + 2852)) != 1 )
  {
    if ( (unsigned __int16)CUserCharacInfo::getCurCharacFatigueGrownUpBuff(a2) )
    {
      v14 = (int)((long double)v12 * 0.3);
      v11 += v14;
      CUserCharacInfo::set_charac_fatigue_buf_bonus_exp(a2, v14);
    }
    STExpReward::STExpReward((STExpReward *)&v10);
    CUser::get_growth_power_exp_reward((CUser *)&v9, (int)a2);
    v10 = v9;
    v21 = (long double)BYTE1(v9) / 100.0;
    v15 = (int)((long double)v12 * v21);
    CUserCharacInfo::set_charac_seria_buf_bonus_exp(a2, v15);
    v11 += v15;
    RepeatEvent = CEventManager::GetRepeatEvent(GlobalData::s_event_manager, 87);
    if ( (*(unsigned __int8 (__cdecl **)(int, _DWORD))(*(_DWORD *)RepeatEvent + 52))(RepeatEvent, 0) )
    {
      v4 = G_CDataManager();
      CUserCharacInfo::setWeekendBonusExp(a2, v12, *((_DWORD *)v4 + 328));
      WeekendBonusExp = CUserCharacInfo::getWeekendBonusExp(a2);
      v11 += WeekendBonusExp;
    }
    if ( CUser::isAffectedPremium((int)a2, 79) )
    {
      AddExpRateFromPremium = CUser::getAddExpRateFromPremium((int)a2, 0x4Fu);
      v6 = CUserCharacInfo::setPremiumBonusExp(a2, v12, AddExpRateFromPremium);
      v11 += v6;
    }
  }
  AdvantageExpRate = 0.0;
  PremiumInfo = CUser::GetPremiumInfo(a2);
  AdvantageExpRate = (float)WongWork::CUserPremium::GetAdvantageExpRate((WongWork::CUserPremium *)PremiumInfo);
  AdvantageExpRate = (long double)CServerEvent::GetExpRate() + AdvantageExpRate;
  v12 = (__int64)((long double)v12 + (long double)v12 * AdvantageExpRate / 100.0);
  v11 += v12;
  v18 = v12 - (int)((long double)v12 / v13);
  CUserCharacInfo::set_charac_party_bonus_exp(a2, v18);
  v19 = CUser::gainExpAsUpperMember(a2, v12);
  v11 += v19;
  v20 = CUser::gainGuildSkillExp(a2, v12);
  v11 += v20;
  return CUser::gainPowerWarRewardExp() + v11;
}

int __cdecl CParty::get_party_seatno(CUser **this, CUser *user)
{
  int i; // [esp+1Ch] [ebp-Ch]

  if ( !a2 )
    return 0;
  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::_checkValidUser(this, i) == 1 && *(this + 6 * i + 30) == a2 )
      return i;
  }
  return 0;
}

int __cdecl CParty::kill_monster(CParty *party, CUser *user, __int16 a3, unsigned __int16 a4, int id, _IO_FILE **a6)
{
  int party_seatno; // eax
  char *acc_name; // esi
  char *charac_level; // ebx
  CDataManager *v10; // eax
  int CurCharacMoney; // eax
  void *v12; // eax
  char *HackAnalyzer; // eax
  int id; // ebx
  char *v17; // eax
  int id; // ebx
  char *v20; // eax
  _IO_FILE *v21; // ebx
  int dungeon_index; // eax
  float v23; // eax
  char isHelpAbuseParty; // bl
  CPartyExpBonusEvent *RepeatEvent; // eax
  long double BonusRate; // fst7
  bool isNamedMonster; // bl
  int dungeon_diff; // eax
  CParty *party; // esi
  int CurCharacExp; // eax
  _IO_FILE *v31; // ebx
  CDataManager *v32; // eax
  CDataManager *v33; // eax
  int id; // eax
  CDataManager *v35; // eax
  int id; // ebx
  CDataManager *v37; // eax
  unsigned __int8 v38; // al
  float v42; // [esp+10h] [ebp-848h]
  float exp_weight; // [esp+4Ch] [ebp-80Ch]
  float v44; // [esp+64h] [ebp-7F4h]
  float v45; // [esp+68h] [ebp-7F0h]
  void *v46[2]; // [esp+88h] [ebp-7D0h] BYREF
  char flag; // [esp+90h] [ebp-7C8h]
  _IO_FILE *v48; // [esp+94h] [ebp-7C4h]
  unsigned __int8 v49; // [esp+98h] [ebp-7C0h]
  int id; // [esp+A8h] [ebp-7B0h]
  int id; // [esp+ACh] [ebp-7ACh]
  int id; // [esp+B4h] [ebp-7A4h]
  int id; // [esp+C0h] [ebp-798h] BYREF
  PacketBuf *v54[3]; // [esp+3D4h] [ebp-484h] BYREF
  int id; // [esp+3E0h] [ebp-478h] BYREF
  int id; // [esp+3E4h] [ebp-474h] BYREF
  _DWORD value[2]; // [esp+3E8h] [ebp-470h] BYREF
  int id[7]; // [esp+3F0h] [ebp-468h] BYREF
  int id[2]; // [esp+40Ch] [ebp-44Ch] BYREF
  CUser *user[4]; // [esp+414h] [ebp-444h] BYREF
  int id[7]; // [esp+424h] [ebp-434h] BYREF
  int id[2]; // [esp+440h] [ebp-418h] BYREF
  _WORD v63[2]; // [esp+448h] [ebp-410h] BYREF
  char flag; // [esp+44Ch] [ebp-40Ch]
  int id; // [esp+450h] [ebp-408h]
  int id; // [esp+454h] [ebp-404h]
  bool v67; // [esp+458h] [ebp-400h]
  bool v68; // [esp+459h] [ebp-3FFh]
  int id; // [esp+45Ch] [ebp-3FCh]
  int id; // [esp+460h] [ebp-3F8h] BYREF
  unsigned int id; // [esp+464h] [ebp-3F4h] BYREF
  PacketBuf *v72[3]; // [esp+468h] [ebp-3F0h] BYREF
  int id[4]; // [esp+474h] [ebp-3E4h] BYREF
  int StdDropRate; // [esp+484h] [ebp-3D4h] BYREF
  int id[211]; // [esp+488h] [ebp-3D0h] BYREF
  int member_count; // [esp+7D4h] [ebp-84h]
  int id; // [esp+7D8h] [ebp-80h]
  int id; // [esp+7DCh] [ebp-7Ch]
  int id; // [esp+7E0h] [ebp-78h]
  char flag; // [esp+7E7h] [ebp-71h]
  float v81; // [esp+7E8h] [ebp-70h]
  int MonsterTotalExp; // [esp+7ECh] [ebp-6Ch]
  int id; // [esp+7F0h] [ebp-68h]
  int MemberLevelGap; // [esp+7F4h] [ebp-64h]
  int PvPWinPoint; // [esp+7F8h] [ebp-60h]
  unsigned int id; // [esp+7FCh] [ebp-5Ch]
  int i; // [esp+800h] [ebp-58h]
  int id; // [esp+804h] [ebp-54h]
  char flag[4]; // [esp+808h] [ebp-50h]
  int id; // [esp+80Ch] [ebp-4Ch]
  int j; // [esp+810h] [ebp-48h]
  int k; // [esp+814h] [ebp-44h]
  int id; // [esp+818h] [ebp-40h]
  int m; // [esp+81Ch] [ebp-3Ch]
  CAICharacter *v95; // [esp+820h] [ebp-38h]
  int n; // [esp+824h] [ebp-34h]
  int id; // [esp+828h] [ebp-30h]
  unsigned int id; // [esp+82Ch] [ebp-2Ch]
  unsigned int id; // [esp+830h] [ebp-28h]
  unsigned int id; // [esp+834h] [ebp-24h]
  BlueMarble *BlueMarble; // [esp+838h] [ebp-20h]
  int Buff; // [esp+83Ch] [ebp-1Ch]

  map_monster::map_monster((map_monster *)v46);
  PacketGuard::PacketGuard((PacketGuard *)v72);
  member_count = CParty::get_member_count((CUser **)a1);
  if ( *((_DWORD *)a1 + 811) && (unsigned __int8)CDungeon::IsEnterEachMap(*((CDungeon **)a1 + 811)) )
  {
    party_seatno = CParty::get_party_seatno((CUser **)a1, a2);
    CBattle_Field::SetCurSeatNo((CParty *)((char *)a1 + 2852), party_seatno);
  }
  v77 = 0;
  v78 = 0;
  v79 = 0;
  v70 = 100;
  for ( i = 0; i <= 3; ++i )
  {
    v88 = 0;
    if ( (unsigned __int8)CParty::_checkValidUser((CUser **)a1, i) == 1 )
    {
      acc_name = CUser::get_acc_name(*((CUser **)a1 + 6 * i + 30));
      charac_level = (char *)CUserCharacInfo::get_charac_level(*((CUserCharacInfo **)a1 + 6 * i + 30));
      v10 = G_CDataManager();
      *(_DWORD *)v89 = CDataManager::GetMoneyLimitPerLevel(v10, charac_level, acc_name);
      if ( *(int *)v89 > 0 )
      {
        CurCharacMoney = CUserCharacInfo::getCurCharacMoney(*((CUserCharacInfo **)a1 + 6 * i + 30));
        v88 = 100 * CurCharacMoney / *(_DWORD *)v89;
        v78 += v88;
        ++v79;
      }
      else
      {
        cMyTrace::cMyTrace(
          (cMyTrace *)v73,
          "bool CParty::kill_monster(CUser*, int, short unsigned int, const MSG_MONSTER_DIE&, int*)",
          5140,
          5);
        cMyTrace::operator()((int)v73, "WarField::HandleMonsterKill limitMoney(%d) is under 0", v89[0]);
      }
      v12 = ARAD::Singleton<ServiceRestrictManager>::Get();
      if ( (unsigned __int8)ServiceRestrictManager::isRestricted((int)v12, 3, 1) )
      {
        StdDropRate = CUser::getStdDropRate(*((CUser **)a1 + 6 * i + 30));
        v70 = *std::min<int>(&v70, &StdDropRate);
      }
    }
  }
  if ( v79 )
    v77 = v78 / v79;
  STKillMonsterData::STKillMonsterData((STKillMonsterData *)v63);
  v63[0] = a3;
  v63[1] = a4;
  v64 = member_count;
  v65 = v77;
  v66 = v70;
  v67 = *(_BYTE *)(a5 + 2594) != 0;
  v68 = *(_BYTE *)(a5 + 2595) != 0;
  QuickParty::RewardData_Param::RewardData_Param((QuickParty::RewardData_Param *)v62, a1);
  QuickParty::RewardData_Result::RewardData_Result((QuickParty::RewardData_Result *)v61);
  CParty::set_random_reward(a1, 3u, v62[0], v62[1], (int)v61);
  v69 = v61[5];
  if ( (unsigned __int8)CBattle_Field::kill_monster(
                          (CParty *)((char *)a1 + 2852),
                          v72,
                          (char **)v46,
                          &v71,
                          (const STKillMonsterData *)v63) != 1 )
  {
    if ( !CParty::checkBossRoom(a1)
      && ((unsigned __int8)CParty::IsAutoCreated(a1) || CParty::get_member_count((CUser **)a1) == 1) )
    {
      HackAnalyzer = CUser::getHackAnalyzer(a2);
      WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)HackAnalyzer, a2, 0xCFu, 1, 0, 0);
    }
    v15 = 0;
  }
  else
  {
    if ( a6 )
      *a6 = v48;
    if ( !v50 && !CBattle_Field::IsClearDungeon((CParty *)((char *)a1 + 2852)) )
    {
      v17 = CUser::getHackAnalyzer(a2);
      WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)v17, a2, 0x25Eu, 1, 0, 0);
    }
    v18 = v51;
    if ( v18 < ActiveStaticInfo::getHpMax((ActiveStaticInfo *)&v53)
      && !CBattle_Field::IsClearDungeon((CParty *)((char *)a1 + 2852)) )
    {
      v20 = CUser::getHackAnalyzer(a2);
      WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)v20, a2, 0x25Fu, 1, 0, 0);
    }
    map_monster::map_monster((map_monster *)v75, (const map_monster *)v46);
    CParty::OnKilledMonster(a1, a2, (int)v48, (int)v75, a5);
    map_monster::~map_monster((void **)v75);
    v80 = CBattle_Field::convertAPCRoleType2MonsterRoleType((CParty *)((char *)a1 + 2852), v47);
    if ( v80 == 1 )
    {
      ++*((_DWORD *)a1 + 219);
    }
    else if ( v80 == 3 )
    {
      ++*((_DWORD *)a1 + 220);
    }
    else
    {
      ++*((_DWORD *)a1 + 218);
      ++*((_DWORD *)a1 + 222);
      v90 = *((_DWORD *)G_CDataManager() + 4724);
      if ( CBattle_Field::GetCurrentMap((CParty *)((char *)a1 + 2852))
        && *(_DWORD *)(CBattle_Field::GetCurrentMap((CParty *)((char *)a1 + 2852)) + 240) )
      {
        v90 = *(_DWORD *)(CBattle_Field::GetCurrentMap((CParty *)((char *)a1 + 2852)) + 240);
      }
      if ( v90 )
      {
        if ( *((_DWORD *)a1 + 222) <= v90 )
          ++*((_DWORD *)a1 + 221);
      }
      else
      {
        ++*((_DWORD *)a1 + 221);
      }
    }
    if ( *((_DWORD *)a1 + 29) )
    {
      CTraceMobDieHack::setMobKiller((CParty *)((char *)a1 + 528), a4);
      if ( (unsigned __int8)CBattle_Field::check_grid_clear((CParty *)((char *)a1 + 2852)) )
      {
        memset(v60, 0, sizeof(v60));
        v60[0] = *((CUser **)a1 + 29);
        CTraceMobDieHack::traceEnd((CParty *)((char *)a1 + 528), *((CUser **)a1 + 29), v60);
        for ( j = 0; j <= 3; ++j )
        {
          if ( (unsigned __int8)CParty::_checkValidUser((CUser **)a1, j) )
            CUser::reqHumanCertify4ClearMap(*((CUser **)a1 + 6 * j + 30), 1);
        }
      }
    }
    if ( v80 == 3 )
      CDungeonClearTracer::IncrementKilledBossCount((CParty *)((char *)a1 + 3196));
    CDungeonClearTracer::IncrementKilledMonsterCount((CParty *)((char *)a1 + 3196));
    for ( k = 0; k <= 3; ++k )
    {
      if ( (unsigned __int8)CParty::_checkValidUser((CUser **)a1, k) == 1
        && *((_BYTE *)a1 + k + 896) == 1
        && *((_DWORD *)a1 + 822) != 1 )
      {
        v93 = 1;
        if ( v47 > 4 && v47 <= 8 )
          v93 = 2;
        v21 = v48;
        dungeon_index = CBattle_Field::get_dungeon_index((CParty *)((char *)a1 + 2852));
        CUser::CheckQuestMonster(*((CUserCharacInfo **)a1 + 6 * k + 30), dungeon_index, (int)v21, v93);
      }
    }
    for ( m = 0; m <= 3; ++m )
    {
      if ( (unsigned __int8)CParty::_checkValidUser((CUser **)a1, m) == 1 && *((CUser **)a1 + 6 * m + 30) == a2 )
        HistoryLog::WriteKillMob(*((FILE **)a2 + 124350), v48);
    }
    if ( member_count <= 0 )
      v23 = 1.0;
    else
      v23 = *((float *)G_CDataManager() + member_count + 243);
    v81 = v23;
    isHelpAbuseParty = CParty::isHelpAbuseParty(a1);
    RepeatEvent = (CPartyExpBonusEvent *)CEventManager::GetRepeatEvent(GlobalData::s_event_manager, 24);
    BonusRate = CPartyExpBonusEvent::getBonusRate(RepeatEvent, member_count, isHelpAbuseParty);
    v81 = BonusRate + v81;
    isNamedMonster = map_monster::isNamedMonster((map_monster *)v46);
    exp_weight = CDungeon::get_exp_weight((CDungeon *)*((_DWORD *)a1 + 811));
    dungeon_diff = CBattle_Field::get_dungeon_diff((CParty *)((char *)a1 + 2852));
    MonsterTotalExp = CParty::getMonsterTotalExp(a1, v71, v80, dungeon_diff, exp_weight, v81, isNamedMonster);
    v83 = 1;
    v29 = (CParty *)MonsterTotalExp;
    CurCharacExp = CUserCharacInfo::getCurCharacExp(a2);
    cUserHistoryLog::KillMonster(
      (CUser **)a2 + CUSER_OFFSET_HISTORY_LOG_ALT,
      (int)v48,
      *(_DWORD *)(a5 + 2590),
      *(_DWORD *)(a5 + 2586),
      *(_WORD *)(a5 + 2596),
      *(_WORD *)(a5 + 2598),
      a5,
      v47,
      v47 == 5,
      CurCharacExp,
      (int)v29,
      v83);
    MemberLevelGap = CParty::_getMemberLevelGap((CUserCharacInfo **)a1);
    PvPWinPoint = 0;
    if ( v47 > 4 )
    {
      v31 = v48;
      v32 = G_CDataManager();
      v95 = (CAICharacter *)CAICharacterList::get(*((CAICharacterList **)v32 + 8615), (unsigned int)v31);
      if ( v95 )
      {
        if ( v52 == 100 || v52 == 200 )
          PvPWinPoint = CAICharacter::getPvPWinPoint(v95);
      }
    }
    for ( n = 0; n <= 3 && !*(_BYTE *)(a5 + 2594); ++n )
    {
      if ( (unsigned __int8)CParty::_checkValidUser((CUser **)a1, n) == 1 && *((_DWORD *)a1 + 822) != 1 )
      {
        if ( *((_BYTE *)a1 + n + 900) == 1 || *(_BYTE *)(*((_DWORD *)a1 + 811) + 2207) )
        {
          v97 = CUserCharacInfo::get_charac_level(*((CUserCharacInfo **)a1 + 6 * n + 30));
          v98 = 0;
          if ( *(_BYTE *)(*((_DWORD *)a1 + 811) + 2206) || *((_WORD *)a1 + 1574) )
          {
            v33 = G_CDataManager();
            CDataManager::get_mob_reward(v33, v97, &v71);
            v44 = CDungeon::get_exp_weight((CDungeon *)*((_DWORD *)a1 + 811));
            v34 = CBattle_Field::get_dungeon_diff((CParty *)((char *)a1 + 2852));
            v99 = CParty::getMonsterTotalExp(a1, v71, v80, v34, v44, v81, 0);
            v98 = v99 / member_count;
          }
          else if ( CDungeon::get_index(*((CDungeon **)a1 + 811)) == 100 )
          {
            v35 = G_CDataManager();
            CDataManager::get_mob_reward(v35, v97, &v71);
            v42 = CDungeon::get_exp_weight((CDungeon *)*((_DWORD *)a1 + 811));
            v100 = CParty::getMonsterTotalExp(a1, v71, v80, *((_DWORD *)a1 + 817), v42, v81, 0);
            v98 = v100 / member_count;
          }
          else
          {
            v36 = v49;
            v37 = G_CDataManager();
            v45 = CDataManager::BaseExpPenalty(v37, v97, v36);
            v98 = (__int64)((long double)CParty::MemberPenaltyRate()
                          * v45
                          * (long double)(unsigned int)MonsterTotalExp
                          / (long double)member_count);
          }
          QuickParty::RewardData_Param::RewardData_Param((QuickParty::RewardData_Param *)v59, a1);
          QuickParty::RewardData_Result::RewardData_Result((QuickParty::RewardData_Result *)v58);
          v58[0] = v98;
          CParty::set_random_reward(a1, 0, v59[0], v59[1], (int)v58);
          v98 = v58[0];
          if ( PvPWinPoint )
            CUser::gainWinPoint(*((CUser ***)a1 + 6 * n + 30), PvPWinPoint, 2);
          STGainedEachExpData::STGainedEachExpData((STGainedEachExpData *)v57);
          v57[0] = v98;
          *(float *)&v57[1] = v81;
          v98 = CParty::sumGainedEachExp(a1, *((CUser **)a1 + 6 * n + 30), (const STGainedEachExpData *)v57);
          if ( *(_BYTE *)(*((_DWORD *)a1 + 811) + 2206) != 1 && !*((_WORD *)a1 + 1574) )
          {
            v38 = CParty::isHelpAbuseParty();
            CUserCharacInfo::calcHelpAbuseRatio(*((CUserCharacInfo **)a1 + 6 * n + 30), v98, v38);
          }
          v56 = 0;
          if ( !*(_BYTE *)(*((_DWORD *)a1 + 811) + 2207) )
          {
            if ( CUser::getMoveSpace(*((CUser **)a1 + 6 * n + 30)) == 1 )
            {
              BlueMarble = (BlueMarble *)CUser::getBlueMarble(*((CUser **)a1 + 6 * n + 30));
              if ( BlueMarble )
              {
                Buff = BlueMarble::getBuff(BlueMarble, *((CUser **)a1 + 6 * n + 30), 0);
                if ( Buff > 1 )
                  v98 *= Buff;
              }
            }
            v55 = 0;
            v29 = a1;
            if ( (unsigned __int8)CUser::gain_exp_sp(
                                    (int)a1,
                                    *((CUserCharacInfo **)a1 + 6 * n + 30),
                                    v98,
                                    &v56,
                                    &v55,
                                    1,
                                    (int)v48,
                                    0) )
              CUser::SendNotiPacket((int)a1, *((CUserCharacInfo **)a1 + 6 * n + 30), 2, 2, 1);
          }
        }
        else
        {
          CUser::SendZeroExpPacket(*((CUser **)a1 + 6 * n + 30));
        }
      }
    }
    if ( !map_monster::isNamedMonster((map_monster *)v46) && v80 != 1 && v80 != 3 )
    {
      ++*((_DWORD *)a1 + 1710);
      *((_DWORD *)a1 + *((_DWORD *)a1 + 1565) + 1659) += *(_DWORD *)(a5 + 2590);
      *((_DWORD *)a1 + *((_DWORD *)a1 + 1565) + 1613) += *(_DWORD *)(a5 + 2586);
    }
    if ( *(_DWORD *)(a5 + 2590) > 9u && !(*(_DWORD *)(a5 + 2590) % 0xAu) )
      *((_BYTE *)a1 + 6816) = 1;
    Secu_HackLogCheckByParty::DieMob((CParty **)a1 + 1711, a5, (map_monster *)v46);
    CParty::send_to_party((CUser **)a1, (PacketGuard *)v72);
    if ( v80 == 1 )
    {
      PacketGuard::PacketGuard((PacketGuard *)v54);
      InterfacePacketBuf::clear(v54);
      InterfacePacketBuf::put_header(v54, 0, 50);
      InterfacePacketBuf::put_byte(v54, *((_DWORD *)a1 + 819));
      InterfacePacketBuf::finalize(v54, 1);
      CParty::send_to_party((CUser **)a1, (PacketGuard *)v54);
      PacketGuard::~PacketGuard(v54);
    }
    CParty::prepare_dungeon_clear((int)v29, a1);
    if ( v47 == 3 )
    {
      v86 = 4;
    }
    else if ( v47 < 3 || (unsigned int)(v47 - 5) > 3 )
    {
      v86 = 2;
    }
    else
    {
      v86 = 3;
    }
    if ( CBattle_Field::ClearCondition((int)a1 + 2852, v86, (int)v48) )
      CParty::ClearDungeon((int)v29, a1);
    v15 = 1;
  }
  PacketGuard::~PacketGuard(v72);
  map_monster::~map_monster(v46);
  return v15;
}

int __cdecl CParty::kill_blood_monster(CParty *this, CUser *user, int id, unsigned __int16 *a4)
{
  return CBattle_Field::kill_blood_monster((CParty *)((char *)this + 2852), a2, a3, a4);
}

int __cdecl CParty::get_item(
        const CUser **this,

  MemberSlotNo = CParty::GetMemberSlotNo(this, a2);
  if ( MemberSlotNo != -1 && *((_BYTE *)this + MemberSlotNo + 896) != 1 )

  member_count = CParty::get_member_count(this);
  if ( member_count <= 0 )

    live_count_enter_map = CParty::get_live_count_enter_map(this);
    if ( live_count_enter_map <= 0 )

        "bool CParty::get_item(CUser*, int, bool, char, int&, unsigned char&, bool&)",
        5724,

        CParty::send_to_party(this, (PacketGuard *)v128);
        v157 = 0;

          CParty::send_to_party(this, (PacketGuard *)v128);
          CUser::Send(a2, (PacketGuard *)v128);

      if ( !(unsigned __int8)CParty::_checkValidUser(this, i) )
      {
        InterfacePacketBuf::put_byte(v128, 0);
        InterfacePacketBuf::put_int(v128, 0);
        continue;
      }

          "bool CParty::get_item(CUser*, int, bool, char, int&, unsigned char&, bool&)",
          5768,

        v31 = CParty::CalcurateManageBonus((int)this, 1, *(this + 6 * i + 30), v159);
        v162 = (int)v31;

        CParty::send_to_party(this, (PacketGuard *)v128);
        v170 = 0;

          CParty::send_to_party(this, (PacketGuard *)v128);
          CUser::Send(a2, (PacketGuard *)v128);

      if ( !(unsigned __int8)CParty::_checkValidUser(this, j) )
      {
        InterfacePacketBuf::put_byte(v128, 0);
        InterfacePacketBuf::put_int(v128, 0);
        continue;
      }

          "bool CParty::get_item(CUser*, int, bool, char, int&, unsigned char&, bool&)",
          6025,

        v63 = CParty::CalcurateManageBonus((int)this, 1, *(this + 6 * j + 30), v172);
        v177 = (int)v63;

      if ( CParty::get_member_count(this) <= 1 )
      {
        v143 = a2;
      }

            v85 = CParty::get_member_count(this);
            if ( v85 <= v188 )

              if ( (unsigned __int8)CParty::_checkValidUser(this, k) == 1 && *((_BYTE *)this + k + 896) == 1 )
              {
                v86 = CUserCharacInfo::getCurCharacInvenR(*(this + 6 * k + 30));
                if ( CInventory::tryInsertItemIntoInventory(v86, v110) != -1 )
                {
                  ++v129[k];
                  if ( v189 == (const CUser *)v188 )
                  {
                    v129[k] += 40;
                    *(this + 81) = v189;
                  }
                  if ( (unsigned __int8)CItem::is_stackable(item) != 1
                    && (*(int (__cdecl **)(CItem *))(*(_DWORD *)item + 12))(item) == 10 )
                  {
                    charac_job = CUserCharacInfo::get_charac_job(*(this + 6 * k + 30));
                    if ( (unsigned __int8)CItem::check_job_type(item, charac_job) )
                      v129[k] += 30;
                  }
                  v89 = k;
                  v90 = v129[k];
                  v129[v89] = v90 + get_rand_int(100 - v90);
                  if ( v124 )
                  {
                    if ( *(this + 6 * k + 30) == v124 )
                    {
                      v129[k] = v190;
                    }
                    else if ( v129[k] >= v190 )
                    {
                      v129[k] = v190 - 1;
                    }
                  }
                  v189 = (const CUser *)((char *)v189 + 1);
                }
              }

              if ( (unsigned __int8)CParty::_checkValidUser(this, m) == 1 && *((_BYTE *)this + m + 896) == 1 )
              {
                if ( v129[m] <= v191 )
                {
                  if ( v129[m] == v191 && v129[m] > 0 )
                    --v129[m];
                }
                else
                {
                  v191 = v129[m];
                  v143 = *(this + 6 * m + 30);
                }
              }

            CParty::_onGetItem(
              (CParty *)this,

            CParty::send_to_party(this, (PacketGuard *)v128);
            if ( v109[0] )

              "bool CParty::get_item(CUser*, int, bool, char, int&, unsigned char&, bool&)",
              6587,

        CParty::send_to_party(this, (PacketGuard *)v128);
        v16 = 0;

      "bool CParty::get_item(CUser*, int, bool, char, int&, unsigned char&, bool&)",
      (const char *)0x1878,

int __cdecl CParty::drop_item(CUser **this, CUser *user, __int16 a3, __int16 a4, char flag, int id, int value)
{
  const char *CurCharacName; // ebx
  unsigned int acc_id; // eax
  const char *v9; // eax
  int id; // ebx
  int *v11; // eax
  CCargo *CurCharacCargoR; // eax
  int *CurCharacInvenR; // eax
  int *v14; // eax
  unsigned int id; // ebx
  CItemList **v16; // eax
  unsigned __int8 v17; // al
  unsigned __int8 UpgradeSeparate; // al
  Store *v19; // eax
  void *v20; // eax
  int id; // eax
  char flag; // bl
  void *InstanceValueStatistic; // eax
  CInventory *inventory; // eax
  char flag; // bl
  CCargo *CurCharacCargoW; // eax
  char CurCharacNo; // bl
  CInventory *CurCharacInvenW; // eax
  char flag; // bl
  CInventory *inventory; // eax
  char flag; // bl
  int id; // ebx
  void *v33; // eax
  int id; // esi
  char flag; // bl
  __int16 unique_id; // ax
  char ItemAttr; // al
  int add_info; // eax
  char AbilityType; // al
  __int16 AbilityValue; // ax
  int id[27]; // [esp+4h] [ebp-1F4h] BYREF
  int id; // [esp+70h] [ebp-188h] BYREF
  int id; // [esp+74h] [ebp-184h]
  int id; // [esp+78h] [ebp-180h]
  int id; // [esp+7Ch] [ebp-17Ch]
  int id; // [esp+80h] [ebp-178h]
  int id; // [esp+84h] [ebp-174h]
  int id; // [esp+88h] [ebp-170h]
  int id; // [esp+8Ch] [ebp-16Ch]
  int id; // [esp+90h] [ebp-168h]
  __int128 v53; // [esp+94h] [ebp-164h]
  int id; // [esp+A4h] [ebp-154h]
  int id; // [esp+A8h] [ebp-150h]
  char flag; // [esp+ACh] [ebp-14Ch]
  char flag; // [esp+BCh] [ebp-13Ch]
  _WORD v58[43]; // [esp+CCh] [ebp-12Ch] BYREF
  _BYTE v59[32]; // [esp+123h] [ebp-D5h] BYREF
  int id; // [esp+143h] [ebp-B5h]
  __int128 v61; // [esp+147h] [ebp-B1h] BYREF
  int id; // [esp+157h] [ebp-A1h]
  int id; // [esp+15Bh] [ebp-9Dh]
  char flag; // [esp+15Fh] [ebp-99h]
  PacketBuf *v65[3]; // [esp+160h] [ebp-98h] BYREF
  int id; // [esp+16Ch] [ebp-8Ch] BYREF
  int id[4]; // [esp+170h] [ebp-88h] BYREF
  int id[4]; // [esp+180h] [ebp-78h] BYREF
  int id[4]; // [esp+190h] [ebp-68h] BYREF
  int id[4]; // [esp+1A0h] [ebp-58h] BYREF
  int id[4]; // [esp+1B0h] [ebp-48h] BYREF
  int id[4]; // [esp+1C0h] [ebp-38h] BYREF
  int id; // [esp+1D0h] [ebp-28h]
  CItem *item; // [esp+1D4h] [ebp-24h]
  int AttachType; // [esp+1D8h] [ebp-20h]
  CItem *v76; // [esp+1DCh] [ebp-1Ch]

  v57 = a5;
  if ( a7 < 0 )
  {
    CurCharacName = CUserCharacInfo::getCurCharacName(a2);
    acc_id = CUser::get_acc_id(a2);
    v9 = (const char *)NumberToString(acc_id, 0);
    LogManager::logFormat(
      (LogManager *)1,
      (int)"party.cpp",
      "int CParty::drop_item(CUser*, int, int, char, int, int)",
      (const char *)0x1A0C,
      "user - accid[%s] %s : count<0",
      v9,
      CurCharacName);
    return 17;
  }
  if ( (unsigned __int8)CUser::CheckInTrade(a2) )
    return 19;
  map_item::map_item((map_item *)v58);
  Inven_Item::Inven_Item((Inven_Item *)v59);
  if ( v57 == 2 )
  {
    CurCharacCargoR = (CCargo *)CUserCharacInfo::getCurCharacCargoR(a2);
    CCargo::get_cargo_slot((CCargo *)&v44, CurCharacCargoR, a6);
    *(_DWORD *)v59 = v44;
    *(_DWORD *)&v59[4] = v45;
    *(_DWORD *)&v59[8] = v46;
    *(_DWORD *)&v59[12] = v47;
    *(_DWORD *)&v59[16] = v48;
    *(_DWORD *)&v59[20] = v49;
    *(_DWORD *)&v59[24] = v50;
    *(_DWORD *)&v59[28] = v51;
    v60 = v52;
    v61 = v53;
    v62 = v54;
    v63 = v55;
    v64 = v56;
  }
  else if ( v57 > 2 )
  {
    if ( v57 == 3 )
    {
      CurCharacInvenR = (int *)CUserCharacInfo::getCurCharacInvenR(a2);
      CInventory::GetInvenSlot((CInventory *)&v44, CurCharacInvenR, 0, a6);
      *(_DWORD *)v59 = v44;
      *(_DWORD *)&v59[4] = v45;
      *(_DWORD *)&v59[8] = v46;
      *(_DWORD *)&v59[12] = v47;
      *(_DWORD *)&v59[16] = v48;
      *(_DWORD *)&v59[20] = v49;
      *(_DWORD *)&v59[24] = v50;
      *(_DWORD *)&v59[28] = v51;
      v60 = v52;
      v61 = v53;
      v62 = v54;
      v63 = v55;
      v64 = v56;
    }
    else if ( v57 == 7 )
    {
      v14 = (int *)CUserCharacInfo::getCurCharacInvenR(a2);
      CInventory::GetInvenSlot((CInventory *)&v44, v14, 3, a6);
      *(_DWORD *)v59 = v44;
      *(_DWORD *)&v59[4] = v45;
      *(_DWORD *)&v59[8] = v46;
      *(_DWORD *)&v59[12] = v47;
      *(_DWORD *)&v59[16] = v48;
      *(_DWORD *)&v59[20] = v49;
      *(_DWORD *)&v59[24] = v50;
      *(_DWORD *)&v59[28] = v51;
      v60 = v52;
      v61 = v53;
      v62 = v54;
      v63 = v55;
      v64 = v56;
      if ( BYTE1(v44) == 5 )
        return 23;
    }
  }
  else if ( !v57 )
  {
    if ( a6 )
    {
      v11 = (int *)CUserCharacInfo::getCurCharacInvenR(a2);
      CInventory::GetInvenSlot((CInventory *)&v44, v11, 1, a6);
      *(_DWORD *)v59 = v44;
      *(_DWORD *)&v59[4] = v45;
      *(_DWORD *)&v59[8] = v46;
      *(_DWORD *)&v59[12] = v47;
      *(_DWORD *)&v59[16] = v48;
      *(_DWORD *)&v59[20] = v49;
      *(_DWORD *)&v59[24] = v50;
      *(_DWORD *)&v59[28] = v51;
      v60 = v52;
      v61 = v53;
      v62 = v54;
      v63 = v55;
      v64 = v56;
    }
    else
    {
      LOBYTE(v58[0]) = 0;
      HIBYTE(v58[8]) = 2;
      *(_DWORD *)&v58[9] = 0;
      Inven_Item::set_add_info((Inven_Item *)&v58[8], a7);
      *(_WORD *)((char *)&v58[13] + 1) = 0;
      Inven_Item::ResetItemAttr((Inven_Item *)&v58[8]);
    }
  }
  v66 = 0;
  if ( !*(_DWORD *)&v59[2] )
  {
LABEL_38:
    if ( v57 == 2 )
    {
      CurCharacCargoW = (CCargo *)CUserCharacInfo::getCurCharacCargoW(a2);
      if ( (unsigned __int8)CCargo::delete_item(CurCharacCargoW, a6, a7, 6) != 1 )
      {
        CurCharacNo = CUserCharacInfo::getCurCharacNo(a2);
        cMyTrace::cMyTrace((cMyTrace *)v69, "int CParty::drop_item(CUser*, int, int, char, int, int)", 6854, 5);
        v43[3] = a7;
        v43[2] = a6;
        cMyTrace::operator()((int)v69, "CParty::drop_item , delete_item failed 2, User ch =%d , %d %d", CurCharacNo);
        return 17;
      }
    }
    else if ( v57 > 2 )
    {
      if ( v57 == 3 )
      {
        CurCharacInvenW = (CInventory *)CUserCharacInfo::getCurCharacInvenW(a2);
        if ( (unsigned __int8)CInventory::delete_item(CurCharacInvenW, 0, a6, 1, 6, 1) != 1 )
        {
          v29 = CUserCharacInfo::getCurCharacNo(a2);
          cMyTrace::cMyTrace((cMyTrace *)v70, "int CParty::drop_item(CUser*, int, int, char, int, int)", 6888, 5);
          v43[2] = a6;
          cMyTrace::operator()((int)v70, "CParty::drop_item , delete_item failed 3, User ch =%d , %d", v29);
          return 17;
        }
      }
      else
      {
        if ( v57 != 7 )
          goto LABEL_63;
        v30 = (CInventory *)CUserCharacInfo::getCurCharacInvenW(a2);
        if ( (unsigned __int8)CInventory::delete_item(v30, 3, a6, 1, 6, 1) != 1 )
        {
          v31 = CUserCharacInfo::getCurCharacNo(a2);
          cMyTrace::cMyTrace((cMyTrace *)v71, "int CParty::drop_item(CUser*, int, int, char, int, int)", 6905, 5);
          v43[2] = a6;
          cMyTrace::operator()((int)v71, "CParty::drop_item , delete_item failed 4, User ch =%d , %d", v31);
          return 17;
        }
      }
    }
    else
    {
      if ( v57 )
        goto LABEL_63;
      if ( !a6 )
      {
        v20 = ARAD::Singleton<ServiceRestrictManager>::Get();
        if ( (unsigned __int8)ServiceRestrictManager::isRestricted((int)v20, a2, 1, 2) )
          return 209;
        v21 = CUserCharacInfo::getCurCharacInvenW(a2);
        if ( (unsigned __int8)CInventory::use_money(v21, a7, (char *)6, 1) != 1 )
        {
          v22 = CUserCharacInfo::getCurCharacNo(a2);
          cMyTrace::cMyTrace((cMyTrace *)v67, "int CParty::drop_item(CUser*, int, int, char, int, int)", 6822, 5);
          v43[2] = a7;
          cMyTrace::operator()((int)v67, "CParty::drop_item , use_money failed, User ch =%d , %d %d", v22);
          return 17;
        }
        InstanceValueStatistic = GetInstanceValueStatistic();
        CValueStatistic::AddValueStatistic((int)InstanceValueStatistic, 0x17u, a2, a7);
        goto LABEL_63;
      }
      v24 = (CInventory *)CUserCharacInfo::getCurCharacInvenW(a2);
      if ( (unsigned __int8)CInventory::delete_item(v24, 1, a6, a7, 6, 1) != 1 )
      {
        v25 = CUserCharacInfo::getCurCharacNo(a2);
        cMyTrace::cMyTrace((cMyTrace *)v68, "int CParty::drop_item(CUser*, int, int, char, int, int)", 6837, 5);
        v43[3] = a7;
        v43[2] = a6;
        cMyTrace::operator()((int)v68, "CParty::drop_item , delete_item failed, User ch =%d , %d %d", v25);
        return 17;
      }
    }
    v32 = v66;
    v33 = GetInstanceValueStatistic();
    CValueStatistic::AddValueStatistic((int)v33, 0x16u, a2, v32);
LABEL_63:
    if ( !Inven_Item::isEquipableItemType((Inven_Item *)v59)
      && *(_DWORD *)&v58[9]
      && Inven_Item::get_add_info((Inven_Item *)&v58[8]) > 2000 )
    {
      v35 = *(_DWORD *)((char *)&v58[11] + 1);
      v36 = v58[9];
      cMyTrace::cMyTrace((cMyTrace *)v72, "int CParty::drop_item(CUser*, int, int, char, int, int)", 6924, 4);
      v43[3] = v35;
      v43[2] = 2000;
      cMyTrace::operator()((int)v72, "drop item(%d) stack size over %d add_info(%d)", v36);
    }
    v43[26] = (int)(this + 713);
    qmemcpy(v43, v58, 0x54u);
    v73 = CBattle_Field::drop_item((CBattle_Field *)(this + 713), v43[0], v43[1]);
    if ( v73 == -1 )
      return 22;
    PacketGuard::PacketGuard((PacketGuard *)v65);
    InterfacePacketBuf::put_header(v65, 0, 40);
    unique_id = CUser::get_unique_id(a2);
    InterfacePacketBuf::put_short(v65, unique_id);
    InterfacePacketBuf::put_short(v65, a3);
    InterfacePacketBuf::put_short(v65, a4);
    InterfacePacketBuf::put_short(v65, v73);
    InterfacePacketBuf::put_int(v65, *(int *)&v58[9]);
    ItemAttr = Inven_Item::GetItemAttr((Inven_Item *)&v58[8]);
    InterfacePacketBuf::put_byte(v65, ItemAttr);
    add_info = Inven_Item::get_add_info((Inven_Item *)&v58[8]);
    InterfacePacketBuf::put_int(v65, add_info);
    InterfacePacketBuf::put_short(v65, *(_WORD *)((char *)&v58[13] + 1));
    InterfacePacketBuf::put_int(v65, *(int *)((char *)&v58[14] + 1));
    AbilityType = stAmplifyOption_t::getAbilityType((stAmplifyOption_t *)((char *)&v58[16] + 1));
    InterfacePacketBuf::put_byte(v65, AbilityType);
    AbilityValue = stAmplifyOption_t::getAbilityValue((stAmplifyOption_t *)((char *)&v58[16] + 1));
    InterfacePacketBuf::put_short(v65, AbilityValue);
    InterfacePacketBuf::put_packet(v65, (const Inven_Item *)&v58[8]);
    InterfacePacketBuf::put_byte(v65, LOBYTE(v58[0]) == 0);
    InterfacePacketBuf::finalize(v65, 1);
    CParty::send_to_party(this, (PacketGuard *)v65);
    InterfacePacketBuf::clear(v65);
    InterfacePacketBuf::put_header(v65, 1, 50);
    InterfacePacketBuf::put_byte(v65, 1);
    InterfacePacketBuf::put_byte(v65, v57);
    InterfacePacketBuf::put_short(v65, a6);
    InterfacePacketBuf::put_int(v65, a7);
    InterfacePacketBuf::finalize(v65, 1);
    CUser::Send(a2, (PacketGuard *)v65);
    v10 = 0;
    PacketGuard::~PacketGuard(v65);
    return v10;
  }
  v15 = *(_DWORD *)&v59[2];
  v16 = (CItemList **)G_CDataManager();
  item = (CItem *)CDataManager::find_item(v16, v15);
  if ( item )
  {
    AttachType = CItem::GetAttachType(item);
    switch ( AttachType )
    {
      case 1:
      case 2:
      case 5:
        v10 = 23;
        break;
      case 3:
        if ( v59[0] == 1 )
          goto LABEL_26;
        v10 = 23;
        break;
      default:
LABEL_26:
        if ( UpgradeSeparateInfo::IsTradeRestriction((UpgradeSeparateInfo *)((char *)&v61 + 15)) )
        {
          v10 = 23;
        }
        else if ( CItem::get_rarity(item) <= 2 )
        {
          if ( !Inven_Item::isEquipableItemType((Inven_Item *)v59)
            || (v76 = item, *((_DWORD *)item + 141) != 11) && *((int *)v76 + 141) > 9 )
          {
            LOBYTE(v58[0]) = 0;
            HIBYTE(v58[8]) = v59[1];
            *(_DWORD *)&v58[9] = *(_DWORD *)&v59[2];
            if ( !Inven_Item::isEquipableItemType((Inven_Item *)v59) )
              Inven_Item::set_add_info((Inven_Item *)&v58[8], a7);
            else
              Inven_Item::set_add_info((Inven_Item *)&v58[8], *(int *)&v59[7]);
            v17 = Inven_Item::GetItemAttr((Inven_Item *)v59);
            Inven_Item::SetItemAttr((Inven_Item *)&v58[8], v17);
            *(_WORD *)((char *)&v58[13] + 1) = *(_WORD *)&v59[11];
            LOBYTE(v58[8]) = v59[0];
            *(_DWORD *)((char *)&v58[14] + 1) = *(_DWORD *)&v59[13];
            *(_DWORD *)((char *)&v58[20] + 1) = *(_DWORD *)&v59[25];
            *(_WORD *)((char *)&v58[22] + 1) = *(_WORD *)&v59[29];
            *(_QWORD *)((char *)&v58[16] + 1) = *(_QWORD *)&v59[17];
            *(_QWORD *)((char *)&v58[26] + 1) = *(_QWORD *)((char *)&v61 + 1);
            *(_DWORD *)((char *)&v58[30] + 1) = *(_DWORD *)((char *)&v61 + 9);
            *(_WORD *)((char *)&v58[32] + 1) = *(_WORD *)((char *)&v61 + 13);
            UpgradeSeparate = UpgradeSeparateInfo::GetUpgradeSeparate((UpgradeSeparateInfo *)((char *)&v61 + 15));
            UpgradeSeparateInfo::SetUpgradeSeparate((UpgradeSeparateInfo *)((char *)&v58[33] + 1), UpgradeSeparate);
            v19 = (Store *)G_Store();
            Store::GetSellItemPrice(v19, (Inven_Item *)v59, item, a7, 0, &v66);
            goto LABEL_38;
          }
          v10 = 23;
        }
        else
        {
          v10 = 13;
        }
        break;
    }
  }
  else
  {
    LogManager::logFormat(
      (LogManager *)1,
      (int)"party.cpp",
      "int CParty::drop_item(CUser*, int, int, char, int, int)",
      (const char *)0x1A48,
      "[CParty::drop_item] Null Item",
      (const char *)v43[4]);
    return -1;
  }
  return v10;
}

int __cdecl CParty::drop_item_gm(CUser **this, CUser *user, unsigned int id)
{
  int id; // ebx
  CItemList **v4; // eax
  unsigned __int8 ItemAttr; // al
  unsigned __int8 UpgradeSeparate; // al
  __int16 unique_id; // ax
  char flag; // al
  int add_info; // eax
  char AbilityType; // al
  __int16 AbilityValue; // ax
  int id[27]; // [esp+4h] [ebp-134h] BYREF
  _WORD v14[43]; // [esp+70h] [ebp-C8h] BYREF
  char flag; // [esp+C7h] [ebp-71h] BYREF
  char flag; // [esp+C8h] [ebp-70h]
  unsigned int id; // [esp+C9h] [ebp-6Fh]
  __int16 v18; // [esp+D2h] [ebp-66h]
  int id; // [esp+D4h] [ebp-64h]
  int id; // [esp+D8h] [ebp-60h]
  int id; // [esp+DCh] [ebp-5Ch]
  int id; // [esp+E0h] [ebp-58h]
  __int16 v23; // [esp+E4h] [ebp-54h]
  int id; // [esp+ECh] [ebp-4Ch]
  int id; // [esp+F0h] [ebp-48h]
  int id; // [esp+F4h] [ebp-44h]
  __int16 v27; // [esp+F8h] [ebp-40h]
  __int16 v28; // [esp+FAh] [ebp-3Eh] BYREF
  PacketBuf *v29[3]; // [esp+104h] [ebp-34h] BYREF
  _DWORD *item; // [esp+110h] [ebp-28h]
  int id; // [esp+114h] [ebp-24h]
  __int16 v32; // [esp+118h] [ebp-20h]
  __int16 v33; // [esp+11Ah] [ebp-1Eh]
  _DWORD *v34; // [esp+11Ch] [ebp-1Ch]

  if ( (unsigned __int8)CUser::CheckInTrade(a2) )
    return 19;
  v4 = (CItemList **)G_CDataManager();
  item = (_DWORD *)CDataManager::find_item(v4, a3);
  if ( !item )
    return -1;
  map_item::map_item((map_item *)v14);
  Inven_Item::Inven_Item((Inven_Item *)&v15);
  v17 = a3;
  (*(void (__cdecl **)(_DWORD *, char *))(*item + 8))(item, &v15);
  if ( v17 )
  {
    if ( v16 == 1 )
    {
      v34 = item;
      if ( item[141] == 11 || (int)v34[141] <= 9 )
        return 23;
    }
    LOBYTE(v14[0]) = 0;
    HIBYTE(v14[8]) = v16;
    *(_DWORD *)&v14[9] = v17;
    Inven_Item::set_add_info((Inven_Item *)&v14[8], 1);
    ItemAttr = Inven_Item::GetItemAttr((Inven_Item *)&v15);
    Inven_Item::SetItemAttr((Inven_Item *)&v14[8], ItemAttr);
    *(_WORD *)((char *)&v14[13] + 1) = v18;
    LOBYTE(v14[8]) = v15;
    *(_DWORD *)((char *)&v14[14] + 1) = v19;
    *(_DWORD *)((char *)&v14[18] + 1) = v21;
    *(_DWORD *)((char *)&v14[20] + 1) = v22;
    *(_WORD *)((char *)&v14[22] + 1) = v23;
    *(_DWORD *)((char *)&v14[16] + 1) = v20;
    *(_DWORD *)((char *)&v14[26] + 1) = v24;
    *(_DWORD *)((char *)&v14[28] + 1) = v25;
    *(_DWORD *)((char *)&v14[30] + 1) = v26;
    *(_WORD *)((char *)&v14[32] + 1) = v27;
    UpgradeSeparate = UpgradeSeparateInfo::GetUpgradeSeparate((UpgradeSeparateInfo *)&v28);
    UpgradeSeparateInfo::SetUpgradeSeparate((UpgradeSeparateInfo *)((char *)&v14[33] + 1), UpgradeSeparate);
  }
  v13[26] = (int)(this + 713);
  qmemcpy(v13, v14, 0x54u);
  v31 = CBattle_Field::drop_item((CBattle_Field *)(this + 713), v13[0], v13[1]);
  if ( v31 == -1 )
    return 22;
  v32 = 300;
  v33 = 240;
  PacketGuard::PacketGuard((PacketGuard *)v29);
  InterfacePacketBuf::put_header(v29, 0, 40);
  unique_id = CUser::get_unique_id(a2);
  InterfacePacketBuf::put_short(v29, unique_id);
  InterfacePacketBuf::put_short(v29, v32);
  InterfacePacketBuf::put_short(v29, v33);
  InterfacePacketBuf::put_short(v29, v31);
  InterfacePacketBuf::put_int(v29, *(int *)&v14[9]);
  v8 = Inven_Item::GetItemAttr((Inven_Item *)&v14[8]);
  InterfacePacketBuf::put_byte(v29, v8);
  add_info = Inven_Item::get_add_info((Inven_Item *)&v14[8]);
  InterfacePacketBuf::put_int(v29, add_info);
  InterfacePacketBuf::put_short(v29, *(_WORD *)((char *)&v14[13] + 1));
  InterfacePacketBuf::put_int(v29, *(int *)((char *)&v14[14] + 1));
  AbilityType = stAmplifyOption_t::getAbilityType((stAmplifyOption_t *)((char *)&v14[16] + 1));
  InterfacePacketBuf::put_byte(v29, AbilityType);
  AbilityValue = stAmplifyOption_t::getAbilityValue((stAmplifyOption_t *)((char *)&v14[16] + 1));
  InterfacePacketBuf::put_short(v29, AbilityValue);
  InterfacePacketBuf::put_packet(v29, (const Inven_Item *)&v14[8]);
  InterfacePacketBuf::put_byte(v29, LOBYTE(v14[0]) == 0);
  InterfacePacketBuf::finalize(v29, 1);
  CParty::send_to_party(this, (PacketGuard *)v29);
  v3 = 0;
  PacketGuard::~PacketGuard(v29);
  return v3;
}

_BOOL4 __cdecl CParty::isMonsterDrop_item(CParty *this, int id)
{
  _BYTE v3[87]; // [esp+18h] [ebp-60h] BYREF
  char field_item; // [esp+6Fh] [ebp-9h]

  map_item::map_item((map_item *)v3);
  field_item = CBattle_Field::get_field_item((CParty *)((char *)this + 2852), a2, (map_item *)v3);
  return !field_item || v3[0] == 1;
}

int __cdecl CParty::use_skill(CParty *this, CUser *user, int id)
{
  return CParty::OnUsedSkill(this, a2, a3);
}

_DWORD *__cdecl CParty::useItem(CParty *this, CUser *user, const Inven_Item *item)
{
  return CBattle_Field::onUseItem((CParty *)((char *)this + 2852), a2, a3);
}

void __cdecl CParty::die_user(CParty *this, CUser *user)
{
  int RevivalTime; // ebx
  PacketBuf *v3[3]; // [esp+1Ch] [ebp-1Ch] BYREF
  int id; // [esp+28h] [ebp-10h]
  _IO_FILE *i; // [esp+2Ch] [ebp-Ch]

  v4 = -1;
  for ( i = 0; (int)i <= 3; i = (_IO_FILE *)((char *)i + 1) )
  {
    if ( (unsigned __int8)CParty::_checkValidUser((CUser **)this, (int)i)
      && *((CUser **)this + 6 * (_DWORD)i + 30) == a2 )
    {
      ++*(_DWORD *)(*((_DWORD *)this + 6 * (_DWORD)i + 30) + 581688);
      v4 = (int)i;
      HistoryLog::WriteDie(*((FILE **)a2 + 124350));
      break;
    }
  }
  PacketGuard::PacketGuard((PacketGuard *)v3);
  InterfacePacketBuf::put_header(v3, 1, 43);
  InterfacePacketBuf::put_byte(v3, 0);
  if ( v4 == -1 )
  {
    InterfacePacketBuf::put_byte(v3, 1);
    InterfacePacketBuf::finalize(v3, 1);
    CUser::Send(a2, (PacketGuard *)v3);
  }
  else if ( *((_BYTE *)this + v4 + 896) != 1 )
  {
    InterfacePacketBuf::put_byte(v3, 18);
    InterfacePacketBuf::finalize(v3, 1);
    CUser::Send(a2, (PacketGuard *)v3);
  }
  else
  {
    CParty::set_charac_live(this, a2, 0);
    RevivalTime = CUser::GetRevivalTime(a2);
    if ( RevivalTime > CSystemTime::getCurSec((CSystemTime *)&GlobalData::s_systemTime_) )
    {
      CParty::set_charac_live(this, a2, 2u);
      CUser::SetRevivalTime(a2, 0);
    }
    CUser::SetUseAPCPotionTime(a2, 0);
    if ( (unsigned __int8)CParty::check_allmember_die((CUser **)this)
      && *((_BYTE *)this + 282) != 1
      && *((_DWORD *)this + 822) != 1 )
    {
      CParty::send_dungeon_fail_msg(this);
    }
  }
  PacketGuard::~PacketGuard(v3);
}

bool __cdecl CParty::CheckClearQuestHellParty(CParty *party, int *a2)
{
  char *CurCharacQuestR; // eax
  bool result; // al
  int id; // [esp+10h] [ebp-18h] BYREF
  int id; // [esp+14h] [ebp-14h] BYREF
  int i; // [esp+18h] [ebp-10h] BYREF
  unsigned int id; // [esp+1Ch] [ebp-Ch]

  std::vector<int>::clear(a2);
  for ( i = 0; ; ++i )
  {
    result = i <= 3;
    if ( i > 3 )
      break;
    if ( (unsigned __int8)CParty::_checkValidUser((CUser **)a1, i) == 1 )
    {
      v7 = 0;
      std::vector<int>::begin(&v5, (_DWORD *)(*((_DWORD *)a1 + 1562) + 28));
      std::vector<int>::end(&v4, *((_DWORD *)a1 + 1562) + 28);
      while ( __gnu_cxx::operator!=<int *,std::vector<int>>((int)&v5, (int)&v4) )
      {
        v7 = *(_DWORD *)__gnu_cxx::__normal_iterator<int *,std::vector<int>>::operator*((int)&v5);
        CurCharacQuestR = CUser::getCurCharacQuestR(*((CUser **)a1 + 6 * i + 30));
        if ( (unsigned __int8)UserQuest::isClearQuest((UserQuest *)CurCharacQuestR, v7) != 1 )
        {
          std::vector<int>::push_back((int)a2, &i);
          break;
        }
        __gnu_cxx::__normal_iterator<int *,std::vector<int>>::operator++(&v5);
      }
    }
  }
  return result;
}

int __cdecl CParty::CheckHaveHellPartyPassItem(CParty *party, CDungeon *dungeon, int id)
{
  int id; // ebx
  char flag; // al
  _DWORD *v6; // eax
  int id; // [esp+10h] [ebp-28h] BYREF
  PacketBuf *v9[3]; // [esp+14h] [ebp-24h] BYREF
  int id[3]; // [esp+20h] [ebp-18h] BYREF
  _DWORD value[3]; // [esp+2Ch] [ebp-Ch] BYREF

  if ( !*((_DWORD *)a1 + 1562) )
    return 1;
  if ( (unsigned __int8)CWorldMap::IsInHellDungeon(*((CWorldMap **)a1 + 1562)) != 1 || a3 != 1 )
    return 1;
  std::vector<int>::vector(v10);
  if ( (unsigned __int8)CParty::CheckDestroyConditionSealDoor(a1, a2, (int)v10, (_DWORD *)(*((_DWORD *)a1 + 1562) + 52)) )
  {
    *((_BYTE *)a1 + 6245) = 1;
  }
  else
  {
    PacketGuard::PacketGuard((PacketGuard *)v9);
    InterfacePacketBuf::put_header(v9, 1, 16);
    InterfacePacketBuf::put_byte(v9, 0);
    InterfacePacketBuf::put_byte(v9, 27);
    v5 = std::vector<int>::size(v10);
    InterfacePacketBuf::put_byte(v9, v5);
    std::vector<int>::begin(&v8, v10);
    while ( 1 )
    {
      std::vector<int>::end(v11, (int)v10);
      if ( !__gnu_cxx::operator!=<int *,std::vector<int>>((int)&v8, (int)v11) )
        break;
      v6 = (_DWORD *)__gnu_cxx::__normal_iterator<int *,std::vector<int>>::operator*((int)&v8);
      InterfacePacketBuf::put_short(v9, *v6);
      __gnu_cxx::__normal_iterator<int *,std::vector<int>>::operator++(&v8);
    }
    InterfacePacketBuf::finalize(v9, 1);
    CParty::send_to_party((CUser **)a1, (PacketGuard *)v9);
    *((_BYTE *)a1 + 6245) = 0;
    PacketGuard::~PacketGuard(v9);
  }
  v3 = *((unsigned __int8 *)a1 + 6245);
  std::vector<int>::~vector((void **)v10);
  return v3;
}

int __cdecl CParty::CheckDestroyConditionSealDoor(CParty *party, CDungeon *dungeon, int id, _DWORD *a4)
{
  int min_level; // eax
  char *v6; // ebx
  CInventory *CurCharacInvenR; // eax
  int *v8; // eax
  int id; // ebx
  int id; // [esp+10h] [ebp-B8h] BYREF
  unsigned __int64 v11; // [esp+14h] [ebp-B4h]
  int id; // [esp+1Ch] [ebp-ACh]
  int id; // [esp+20h] [ebp-A8h]
  int id; // [esp+24h] [ebp-A4h]
  int id; // [esp+28h] [ebp-A0h]
  int id; // [esp+2Ch] [ebp-9Ch]
  int id; // [esp+30h] [ebp-98h]
  int id; // [esp+34h] [ebp-94h]
  int id; // [esp+38h] [ebp-90h]
  int id; // [esp+3Ch] [ebp-8Ch]
  int id; // [esp+40h] [ebp-88h]
  int id; // [esp+44h] [ebp-84h]
  int id; // [esp+48h] [ebp-80h]
  char flag; // [esp+4Ch] [ebp-7Ch]
  int id; // [esp+5Fh] [ebp-69h] BYREF
  unsigned __int64 v26; // [esp+63h] [ebp-65h]
  int id; // [esp+6Bh] [ebp-5Dh]
  int id; // [esp+6Fh] [ebp-59h]
  int id; // [esp+73h] [ebp-55h]
  int id; // [esp+77h] [ebp-51h]
  int id; // [esp+7Bh] [ebp-4Dh]
  int id; // [esp+7Fh] [ebp-49h]
  int id; // [esp+83h] [ebp-45h]
  int id; // [esp+87h] [ebp-41h]
  int id; // [esp+8Bh] [ebp-3Dh]
  int id; // [esp+8Fh] [ebp-39h]
  int id; // [esp+93h] [ebp-35h]
  int id; // [esp+97h] [ebp-31h]
  char flag; // [esp+9Bh] [ebp-2Dh]
  int id; // [esp+9Ch] [ebp-2Ch] BYREF
  int i; // [esp+A0h] [ebp-28h] BYREF
  int id; // [esp+A4h] [ebp-24h] BYREF
  int id; // [esp+A8h] [ebp-20h]
  int id; // [esp+ACh] [ebp-1Ch]
  int id; // [esp+B0h] [ebp-18h]
  int id; // [esp+B4h] [ebp-14h]
  unsigned __int8 v47; // [esp+BBh] [ebp-Dh]
  int HellPartyEntranceItemNeedCount; // [esp+BCh] [ebp-Ch]

  v43 = 0;
  v44 = 0;
  v45 = 0;
  v46 = 0;
  Inven_Item::Inven_Item((Inven_Item *)&v25);
  v47 = 0;
  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::_checkValidUser((CUser **)a1, i) == 1 )
    {
      ++v44;
      v45 = 0;
      if ( CParty::CheckHellDungeonFreepassItemHaveAndDel(a1, *((CUser **)a1 + 6 * i + 30), 0) )
      {
        ++v43;
      }
      else
      {
        if ( CDungeon::get_min_level(a2) <= 44 )
          return 0;
        min_level = CDungeon::get_min_level(a2);
        HellPartyEntranceItemNeedCount = getHellPartyEntranceItemNeedCount(min_level);
        std::vector<int>::begin(&v40, a4);
        while ( 1 )
        {
          std::vector<int>::end(&v42, (int)a4);
          if ( !__gnu_cxx::operator!=<int *,std::vector<int>>((int)&v40, (int)&v42) )
            break;
          v6 = *(char **)__gnu_cxx::__normal_iterator<int *,std::vector<int>>::operator*((int)&v40);
          CurCharacInvenR = (CInventory *)CUserCharacInfo::getCurCharacInvenR(*((CUserCharacInfo **)a1 + 6 * i + 30));
          v46 = CInventory::check_item_exist(CurCharacInvenR, v6);
          if ( v46 != -1 )
          {
            v8 = (int *)CUserCharacInfo::getCurCharacInvenR(*((CUserCharacInfo **)a1 + 6 * i + 30));
            CInventory::GetInvenSlot((CInventory *)&v10, v8, 1, v46);
            v25 = v10;
            v26 = v11;
            v27 = v12;
            v28 = v13;
            v29 = v14;
            v30 = v15;
            v31 = v16;
            v32 = v17;
            v33 = v18;
            v34 = v19;
            v35 = v20;
            v36 = v21;
            v37 = v22;
            v38 = v23;
            v39 = v24;
            if ( (int)(v11 >> 24) >= HellPartyEntranceItemNeedCount )
              ++v45;
          }
          __gnu_cxx::__normal_iterator<int *,std::vector<int>>::operator++(&v40);
        }
        v9 = v45;
        if ( v9 == std::vector<int>::size(a4) )
          ++v43;
        else
          std::vector<int>::push_back(a3, &i);
      }
    }
  }
  if ( v44 <= v43 )
    return 1;
  return v47;
}

int __cdecl CParty::UseSealDoorItems(CParty *party, _DWORD *a2)
{
  int min_level; // eax
  char *v4; // ebx
  CInventory *CurCharacInvenR; // eax
  CInventory *CurCharacInvenW; // eax
  char CurCharacNo; // bl
  _BYTE v8[61]; // [esp+2Fh] [ebp-69h] BYREF
  int count; // [esp+6Ch] [ebp-2Ch] BYREF
  int count; // [esp+70h] [ebp-28h] BYREF
  int count[4]; // [esp+74h] [ebp-24h] BYREF
  int count; // [esp+84h] [ebp-14h]
  int i; // [esp+88h] [ebp-10h]
  int HellPartyEntranceItemNeedCount; // [esp+8Ch] [ebp-Ch]

  v12 = 0;
  Inven_Item::Inven_Item((Inven_Item *)v8);
  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::_checkValidUser((CUser **)a1, i) == 1
      && !CParty::CheckHellDungeonFreepassItemHaveAndDel(a1, *((CUser **)a1 + 6 * i + 30), 1) )
    {
      std::vector<int>::begin(&v9, a2);
      while ( 1 )
      {
        std::vector<int>::end(&v10, (int)a2);
        if ( !__gnu_cxx::operator!=<int *,std::vector<int>>((int)&v9, (int)&v10) )
          break;
        if ( CDungeon::get_min_level(*((CDungeon **)a1 + 811)) <= 44 )
        {
          *((_BYTE *)a1 + 3309) = 0;
          return 0;
        }
        min_level = CDungeon::get_min_level(*((CDungeon **)a1 + 811));
        HellPartyEntranceItemNeedCount = getHellPartyEntranceItemNeedCount(min_level);
        v4 = *(char **)__gnu_cxx::__normal_iterator<int *,std::vector<int>>::operator*((int)&v9);
        CurCharacInvenR = (CInventory *)CUserCharacInfo::getCurCharacInvenR(*((CUserCharacInfo **)a1 + 6 * i + 30));
        v12 = CInventory::check_item_exist(CurCharacInvenR, v4);
        if ( v12 == -1 )
        {
          *((_BYTE *)a1 + 3309) = 0;
          return 0;
        }
        CurCharacInvenW = (CInventory *)CUserCharacInfo::getCurCharacInvenW(*((CUserCharacInfo **)a1 + 6 * i + 30));
        if ( (unsigned __int8)CInventory::delete_item(CurCharacInvenW, 1, v12, HellPartyEntranceItemNeedCount, 23, 1) != 1 )
        {
          CurCharacNo = CUserCharacInfo::getCurCharacNo(*((CUserCharacInfo **)a1 + 6 * i + 30));
          cMyTrace::cMyTrace((cMyTrace *)v11, "bool CParty::UseSealDoorItems(IntVector&)", 7458, 5);
          cMyTrace::operator()(
            (int)v11,
            "CParty::UseSealDoorItems, delete_item failed , User ch =%d , %d %d",
            CurCharacNo);
        }
        else
        {
          CUser::SendUpdateItem(*((CUserCharacInfo **)a1 + 6 * i + 30), 1, 0, v12);
        }
        __gnu_cxx::__normal_iterator<int *,std::vector<int>>::operator++(&v9);
      }
    }
  }
  *((_BYTE *)a1 + 3309) = 1;
  return 1;
}

int __cdecl CParty::IsClearedQuest(CUser **this, unsigned int id)
{
  char *CurCharacQuestR; // eax
  int i; // [esp+1Ch] [ebp-Ch]

  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::_checkValidUser(this, i) )
    {
      CurCharacQuestR = CUser::getCurCharacQuestR(*(this + 6 * i + 30));
      if ( (unsigned __int8)UserQuest::isClearQuest((UserQuest *)CurCharacQuestR, a2) != 1 )
        return 0;
    }
  }
  return 1;
}

_BOOL4 __cdecl CParty::CheckHellCoinLimint(CParty *this, int id)
{
  return !(unsigned __int8)CBattle_Field::IsEnableHellDungeon()
      || *((_DWORD *)this + 821) != 1
      || (unsigned __int8)CBattle_Field::IsKilledAllHellGruoups((CParty *)((char *)this + 2852)) == 1
      || CParty::GetUsedCoinCount(this) < a2;
}

int __cdecl CParty::checkMoveMapHackCnt(CParty *party, int id)
{
  int result; // eax
  int LastMapInfo; // eax
  CUser *user; // ebx
  char *HackAnalyzer; // eax
  int id; // esi
  CUser *user; // ebx
  char *v8; // eax
  CUser *user; // ebx
  char *v11; // eax
  char *CurCharacQuestR; // eax
  char *v13; // eax
  char *v14; // eax
  unsigned int id; // esi
  CUser *user; // ebx
  char *v18; // eax
  CUser *user; // ebx
  char *v21; // eax
  int id; // [esp+38h] [ebp-40h] BYREF
  unsigned int id; // [esp+3Ch] [ebp-3Ch] BYREF
  int id; // [esp+40h] [ebp-38h]
  int id; // [esp+44h] [ebp-34h]
  int id; // [esp+48h] [ebp-30h]
  int id; // [esp+4Ch] [ebp-2Ch]
  int i; // [esp+50h] [ebp-28h]
  unsigned int id; // [esp+54h] [ebp-24h]
  unsigned int id; // [esp+58h] [ebp-20h]
  unsigned int id; // [esp+5Ch] [ebp-1Ch]

  v24 = *((_DWORD *)a1 + 222);
  v24 = *((_DWORD *)a1 + 1710);
  *((_DWORD *)a1 + 1710) = 0;
  *((_DWORD *)a1 + 222) = 0;
  *(_QWORD *)((char *)a1 + 820) = (unsigned int)CSystemTime::getCurTickCount((CSystemTime *)GlobalData::s_systemTime_)
                                - *(_QWORD *)((char *)a1 + 836)
                                - *(_QWORD *)((char *)a1 + 828);
  result = CBattle_Field::IsVisitedCurrentMap((CParty *)((char *)a1 + 2852)) ^ 1;
  if ( (_BYTE)result )
  {
    v25 = 0;
    if ( v24 )
    {
      v26 = *((_DWORD *)a1 + *((_DWORD *)a1 + 1565) + 1659);
      *((_DWORD *)a1 + *((_DWORD *)a1 + 1565) + 1659) = v26 / v24;
      *((_DWORD *)a1 + 1658) += *((_DWORD *)a1 + *((_DWORD *)a1 + 1565) + 1659);
      v27 = *((_DWORD *)a1 + *((_DWORD *)a1 + 1565) + 1613);
      *((_DWORD *)a1 + *((_DWORD *)a1 + 1565) + 1613) = v27 / v24;
      *((_DWORD *)a1 + 1612) += *((_DWORD *)a1 + *((_DWORD *)a1 + 1565) + 1613);
    }
    for ( i = 0; i <= 3; ++i )
    {
      if ( *(_WORD *)(a2 + 2 * (i + 8) + 11) != INVALID_ID_U16
        && (unsigned __int8)CParty::checkValidUser((CUser **)a1, i)
        && (!(unsigned __int8)CDungeon::IsEnterEachMap(*((CDungeon **)a1 + 811)) || *(char *)(a2 + 57) == i) )
      {
        LastMapInfo = CBattle_Field::GetLastMapInfo((CParty *)((char *)a1 + 2852));
        cUserHistoryLog::MapClear(
          (CUser **)(*((_DWORD *)a1 + 6 * i + 30) + CUSER_OFFSET_HISTORY_LOG),
          *(_WORD *)(a2 + 2 * (i + 8) + 11),
          *((_DWORD *)a1 + 205) / 0x3E8u,
          *(_DWORD *)(LastMapInfo + 4));
        if ( !*(_WORD *)(a2 + 2 * (i + 8) + 11) )
        {
          v4 = (CUser *)*((_DWORD *)a1 + 6 * i + 30);
          HackAnalyzer = CUser::getHackAnalyzer(v4);
          WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)HackAnalyzer, v4, 0x19Bu, 1, 0, 0);
        }
        v29 = *((_DWORD *)a1 + 205) / 0x3E8u;
        if ( v29 )
        {
          v31 = *(unsigned __int16 *)(a2 + 2 * (i + 8) + 11) / v29;
          if ( v31 > 4 )
          {
            v22 = 100;
            v23 = v31 - 4;
            v6 = *std::min<unsigned int>(&v23, &v22);
            v7 = (CUser *)*((_DWORD *)a1 + 6 * i + 30);
            v8 = CUser::getHackAnalyzer(v7);
            WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)v8, v7, 0x340u, v6, 0, 0);
          }
        }
        v25 += *(unsigned __int16 *)(a2 + 2 * (i + 8) + 11);
        if ( *((_DWORD *)a1 + 811)
          && *(_BYTE *)(*((_DWORD *)a1 + 811) + 2204) != 1
          && !(unsigned __int8)CDungeon::get_dimension_possible(*((CDungeon **)a1 + 811))
          && v24
          && *((_BYTE *)a1 + 6816) != 1
          && (int)++*(_DWORD *)(*((_DWORD *)a1 + 6 * i + 30) + 584520) > 9 )
        {
          *(_DWORD *)(*((_DWORD *)a1 + 6 * i + 30) + 584520) = 0;
          v10 = (CUser *)*((_DWORD *)a1 + 6 * i + 30);
          v11 = CUser::getHackAnalyzer(v10);
          WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)v11, v10, 0xDCu, 1, 0, 0);
        }
        *((_BYTE *)a1 + 6816) = 0;
        v30 = *((_DWORD *)a1 + 205) / 0x3E8u;
        if ( CUserCharacInfo::get_charac_level(*((CUserCharacInfo **)a1 + 6 * i + 30)) > 19
          && *((_BYTE *)a1 + i + 896)
          && (!*((_DWORD *)a1 + i + 1705)
           || !(v30 % 5) && *((_DWORD *)a1 + i + 1705) <= v30 / 5
           || v30 % 5 && *((_DWORD *)a1 + i + 1705) < v30 / 5) )
        {
          CurCharacQuestR = CUser::getCurCharacQuestR(*((CUser **)a1 + 6 * i + 30));
          if ( (unsigned __int8)UserQuest::IsDoingQuest((UserQuest *)CurCharacQuestR, 4307) != 1
            || CBattle_Field::get_dungeon_index((CParty *)((char *)a1 + 2852)) != 11
            || CBattle_Field::get_dungeon_diff((CParty *)((char *)a1 + 2852)) != 2 )
          {
            v13 = CUser::getCurCharacQuestR(*((CUser **)a1 + 6 * i + 30));
            if ( (unsigned __int8)UserQuest::IsDoingQuest((UserQuest *)v13, 4308) != 1
              || CBattle_Field::get_dungeon_index((CParty *)((char *)a1 + 2852)) != 12
              || CBattle_Field::get_dungeon_diff((CParty *)((char *)a1 + 2852)) != 2 )
            {
              v14 = CUser::getCurCharacQuestR(*((CUser **)a1 + 6 * i + 30));
              if ( (unsigned __int8)UserQuest::IsDoingQuest((UserQuest *)v14, 4312) != 1
                || CBattle_Field::get_dungeon_index((CParty *)((char *)a1 + 2852)) != 15
                || CBattle_Field::get_dungeon_diff((CParty *)((char *)a1 + 2852)) <= 0 )
              {
                v16 = CUser::SetETC(*((CUser **)a1 + 6 * i + 30), 132, *((_DWORD *)a1 + i + 1705));
                v17 = (CUser *)*((_DWORD *)a1 + 6 * i + 30);
                v18 = CUser::getHackAnalyzer(v17);
                WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)v18, v17, 0x84u, 1, v16, 0);
              }
            }
          }
        }
        *((_DWORD *)a1 + i + 1705) = 0;
        if ( !*(_DWORD *)(a2 + 53) && (unsigned __int8)CParty::IsSinglePlay(a1) )
        {
          v20 = (CUser *)*((_DWORD *)a1 + 6 * i + 30);
          v21 = CUser::getHackAnalyzer(v20);
          WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)v21, v20, 0x1A0u, 1, 0, 0);
        }
      }
    }
    *((_DWORD *)a1 + *((_DWORD *)a1 + 1565) + 1567) = v25;
    *((_DWORD *)a1 + 1566) += v25;
    result = *((_DWORD *)a1 + 1565);
    if ( result <= 43 )
    {
      ++*((_DWORD *)a1 + 1565);
      return (int)a1;
    }
  }
  return result;
}

_BOOL4 __cdecl CParty::canFatigueUp(CParty *this)
{
  if ( *((_DWORD *)this + 822) == 1 )
    return 0;
  if ( !*((_DWORD *)this + 811) )
    return 0;
  return !*(_BYTE *)(*((_DWORD *)this + 811) + 2207)
      && *(_BYTE *)(*((_DWORD *)this + 811) + 2170) != 1
      && (unsigned __int8)CDungeon::isNoFatigueDungeon(*((CDungeon **)this + 811)) != 1;
}

int __cdecl CParty::changePartyMemberInfo(CUser **a1, int id, char flag)
{
  int result; // eax

  result = CParty::_checkValidUser(a1, a3);
  if ( (_BYTE)result )
  {
    if ( *((_BYTE *)a1 + a3 + 896) != 1 )
    {
      if ( *((_BYTE *)a1 + a3 + 900) != 1 && *((_BYTE *)a1 + a3 + 904) != 1 )
        a1[a3 + 228] = (CUser *)((char *)a1[a3 + 228] + 1);
      else
        *((_BYTE *)a1 + a3 + 900) = 0;
    }
    *((_BYTE *)a1 + a3 + 904) = 0;
    result = CParty::canFatigueUp((CParty *)a1);
    if ( (_BYTE)result )
      return CUser::FatigueUp(a1[6 * a3 + 30], 0);
  }
  return result;
}

bool __cdecl CParty::resetFinishLoadingVar(CParty *this)
{
  bool result; // al
  int i; // [esp+Ch] [ebp-4h]

  for ( i = 0; ; ++i )
  {
    result = i <= 3;
    if ( i > 3 )
      break;
    *((_BYTE *)this + i + 892) = 0;
  }
  return result;
}

char __cdecl CParty::changePartyMemberInfoOnMoveMap(CUser **a1, int id, char flag)
{
  char result; // al
  int i; // [esp+2Ch] [ebp-Ch]

  result = a3;
  if ( !a3 )
  {
    if ( a1[811] && (unsigned __int8)CDungeon::IsEnterEachMap(a1[811]) )
    {
      return CParty::changePartyMemberInfo(a1, a2, *(_BYTE *)(a2 + 57));
    }
    else
    {
      for ( i = 0; ; ++i )
      {
        result = i <= 3;
        if ( i > 3 )
          break;
        CParty::changePartyMemberInfo(a1, a2, i);
      }
    }
  }
  return result;
}

CParty *__cdecl CParty::changeBattleDataOnMoveMap(CParty *this, bool a2)
{
  CParty *result; // eax

  *((_DWORD *)this + 216) = CSystemTime::getCurTickCount((CSystemTime *)GlobalData::s_systemTime_);
  *((_DWORD *)this + 217) = 0;
  *((_DWORD *)this + 209) = CSystemTime::getCurTickCount((CSystemTime *)GlobalData::s_systemTime_);
  *((_DWORD *)this + 210) = 0;
  result = (CParty *)!a2;
  if ( (_BYTE)result )
  {
    ++*((_DWORD *)this + 622);
    return this;
  }
  return result;
}

int __cdecl CParty::move_map(CParty *party, int id)
{
  int id; // ebx
  int PartyType; // ebx
  void *v5; // eax
  void *InstanceCommonStatisticsMgr; // eax
  MapInfo *CurrentMapInfo; // eax
  bool v9; // [esp+27h] [ebp-31h] BYREF
  PacketPool *v10[3]; // [esp+28h] [ebp-30h] BYREF
  int id; // [esp+34h] [ebp-24h]
  CDungeon *dungeon; // [esp+38h] [ebp-20h]
  int CurPosX; // [esp+3Ch] [ebp-1Ch]
  int CurPosY; // [esp+40h] [ebp-18h]
  char flag[4]; // [esp+44h] [ebp-14h]
  int map_index; // [esp+48h] [ebp-10h]
  int MemberLevelGap; // [esp+4Ch] [ebp-Ch]

  PacketGuard::PacketGuard((PacketGuard *)v10);
  v11 = 0;
  if ( *((_BYTE *)a1 + 106) != 2 )
    goto LABEL_2;
  v12 = (CDungeon *)*((_DWORD *)a1 + 811);
  if ( v12 && (unsigned __int8)CDungeon::IsEnterEachMap(v12) )
  {
    CBattle_Field::SetCurSeatNo((CParty *)((char *)a1 + 2852), *(char *)(a2 + 57));
  }
  else if ( CBattle_Field::IsClearDungeon((CParty *)((char *)a1 + 2852)) )
  {
LABEL_2:
    CParty::send_to_party_cmd_error((CUser **)a1, 48, 19);
    v2 = 0;
    goto LABEL_25;
  }
  CurPosX = CBattle_Field::GetCurPosX((CParty *)((char *)a1 + 2852));
  CurPosY = CBattle_Field::GetCurPosY((CParty *)((char *)a1 + 2852));
  *(_DWORD *)v15 = CBattle_Field::get_map_index((MazeScript **)a1 + 713, CurPosX, CurPosY);
  if ( *(_BYTE *)(a2 + 23) == 1 )
    CBattle_Field::checkLayeredMap((CParty *)((char *)a1 + 2852), *(char *)(a2 + 13), *(char *)(a2 + 14));
  map_index = CBattle_Field::get_map_index((MazeScript **)a1 + 713, *(char *)(a2 + 13), *(char *)(a2 + 14));
  Secu_HackLogCheckByParty::CheckMoveMapFitInPathHackCnt(
    (CParty *)((char *)a1 + 6844),
    *((CUser **)a1 + 28),
    a1,
    *(_DWORD *)(a2 + 15),
    *(_DWORD *)(a2 + 19),
    *(int *)v15);
  v9 = 0;
  v11 = CBattle_Field::MoveMap((MazeScript **)a1 + 713, *(char *)(a2 + 13), *(char *)(a2 + 14), &v9);
  if ( v11 <= 0 )
  {
    CParty::checkMoveMapHackCnt(a1, a2);
    if ( *(char *)(*((_DWORD *)a1 + 811) + 2207) <= 0 )
      CBattle_Field::MakePacket((CParty *)((char *)a1 + 2852), 29, (InterfacePacketBuf *)v10);
    else
      CBattle_Field::MakePacket((CParty *)((char *)a1 + 2852), 211, (InterfacePacketBuf *)v10);
    CParty::send_to_party((CUser **)a1, (PacketGuard *)v10);
    CParty::check_passive_skill();
    CParty::send_loadingcheck_msg(a1);
    CParty::resetFinishLoadingVar(a1);
    CParty::changePartyMemberInfoOnMoveMap((CUser **)a1, a2, v9);
    MemberLevelGap = CParty::_getMemberLevelGap((CUserCharacInfo **)a1);
    if ( CParty::get_member_count((CUser **)a1) > 1 )
    {
      if ( MemberLevelGap <= 6 )
      {
        InstanceCommonStatisticsMgr = GetInstanceCommonStatisticsMgr();
        CCommonStatisticsMgr::IncreaseQuantity((int)InstanceCommonStatisticsMgr, 0, 3);
      }
      else
      {
        PartyType = CParty::GetPartyType((CUserCharacInfo **)a1);
        v5 = GetInstanceCommonStatisticsMgr();
        CCommonStatisticsMgr::IncreaseQuantity((int)v5, 0, PartyType);
      }
    }
    CParty::changeBattleDataOnMoveMap(a1, v9);
    if ( (unsigned __int8)CParty::getDungeonDropTestFlag(a1) && !v9 )
    {
      CurrentMapInfo = (MapInfo *)CBattle_Field::GetCurrentMapInfo((CParty *)((char *)a1 + 2852));
      std::vector<MapInfo>::push_back((int)a1 + 6900, CurrentMapInfo);
    }
    v2 = 1;
  }
  else
  {
    CParty::send_to_party_cmd_error((CUser **)a1, 48, v11);
    v2 = 0;
  }
LABEL_25:
  PacketGuard::~PacketGuard(v10);
  return v2;
}

void CParty::check_passive_skill()
{
  ;
}

void __usercall CParty::ClearDungeon(int id@<esi>, CParty *this)
{
  int id; // esi
  int PartyIndex; // ebx
  TimerQueue *v4; // eax
  std::string *v5; // ebx
  __int16 BloodRoundOrder; // ax
  int id; // esi
  int id; // ebx
  TimerQueue *v9; // eax
  int Manager; // eax
  unsigned __int16 Layer; // ax
  __int16 v12; // ax
  size_t v13; // eax
  size_t v14; // eax
  CUserCharacInfo *charac_info; // eax
  size_t v16; // eax
  size_t v17; // eax
  GameWorld *v18; // eax
  CDataManager *v19; // eax
  int ItemGenerator; // eax
  int Generator; // eax
  unsigned int id; // eax
  unsigned int id; // ebx
  CItemList **v24; // eax
  _DWORD *v25; // eax
  int id; // eax
  unsigned int id; // eax
  CUserCharacInfo *charac_info; // eax
  CInventory *CurCharacInvenW; // eax
  int id; // ebx
  CUserCharacInfo *charac_info; // eax
  int id; // esi
  int id; // ebx
  TimerQueue *v35; // eax
  std::string *ServerProxy; // eax
  char *CurCharacQuestW; // eax
  int id; // ebx
  char *v39; // eax
  int standard_level; // ebx
  int index; // eax
  _DWORD *hellparty_script_values; // eax
  int id; // esi
  int rate_level_difference; // ebx
  int dungeon_diff; // eax
  unsigned __int64 v47; // [esp+0h] [ebp-178h]
  int id; // [esp+Ch] [ebp-16Ch]
  void *v49[13]; // [esp+2Ch] [ebp-14Ch] BYREF
  char flag; // [esp+60h] [ebp-118h]
  char flag; // [esp+61h] [ebp-117h]
  int id; // [esp+64h] [ebp-114h]
  int id; // [esp+68h] [ebp-110h]
  int id; // [esp+6Ch] [ebp-10Ch]
  void *v55[12]; // [esp+74h] [ebp-104h] BYREF
  PacketBuf *v56[3]; // [esp+A4h] [ebp-D4h] BYREF
  char buf[14]; // [esp+B1h] [ebp-C7h] BYREF
  int id; // [esp+BFh] [ebp-B9h]
  char flag; // [esp+C3h] [ebp-B5h]
  int id[3]; // [esp+C8h] [ebp-B0h] BYREF
  int id[3]; // [esp+D4h] [ebp-A4h] BYREF
  PacketBuf *v62[3]; // [esp+E0h] [ebp-98h] BYREF
  int id[7]; // [esp+ECh] [ebp-8Ch] BYREF
  int id[2]; // [esp+108h] [ebp-70h] BYREF
  PacketBuf *v65[3]; // [esp+110h] [ebp-68h] BYREF
  _BYTE v66[2]; // [esp+11Ch] [ebp-5Ch] BYREF
  _BYTE v67[2]; // [esp+11Eh] [ebp-5Ah] BYREF
  _BYTE v68[2]; // [esp+120h] [ebp-58h] BYREF
  _BYTE v69[2]; // [esp+122h] [ebp-56h] BYREF
  _DWORD value[2]; // [esp+124h] [ebp-54h] BYREF
  _BYTE v71[2]; // [esp+12Eh] [ebp-4Ah] BYREF
  int member_count; // [esp+130h] [ebp-48h]
  int id; // [esp+134h] [ebp-44h]
  int i; // [esp+138h] [ebp-40h]
  int id; // [esp+13Ch] [ebp-3Ch]
  int j; // [esp+140h] [ebp-38h]
  TOD_UserState *CharacExpandData; // [esp+144h] [ebp-34h]
  int id; // [esp+148h] [ebp-30h]
  int inserted; // [esp+14Ch] [ebp-2Ch]
  char *s; // [esp+150h] [ebp-28h]
  int k; // [esp+154h] [ebp-24h]
  unsigned int m; // [esp+158h] [ebp-20h]
  int id; // [esp+15Ch] [ebp-1Ch]
  CItem *item; // [esp+160h] [ebp-18h]
  unsigned int id; // [esp+164h] [ebp-14h]
  int id; // [esp+168h] [ebp-10h]
  int n; // [esp+16Ch] [ebp-Ch]

  if ( !*((_BYTE *)this + 282) )
  {
    if ( *((_DWORD *)this + 822) == 1 )
    {
      for ( i = 0; i <= 3; ++i )
      {
        if ( (unsigned __int8)CParty::_checkValidUser((CUser **)this, i) == 1 )
          CParty::OnSuccessedDungeonClear(a1, this, *((CUser ***)this + 6 * i + 30));
      }
      CParty::SetEPLPState(this, 2);
      v2 = CParty::gen_timer_key((int)this, 21);
      PartyIndex = CParty::GetPartyIndex(this);
      v4 = G_TimerQueue();
      TimerQueue::InsertTimer(v2, v4, 1, PartyIndex, 21, 60, v2, 0);
      CParty::set_state(this, 1);
      PacketGuard::PacketGuard((PacketGuard *)v62);
      InterfacePacketBuf::put_header(v62, 0, 254);
      InterfacePacketBuf::finalize(v62, 1);
      CParty::send_to_party((CUser **)this, (PacketGuard *)v62);
      CDungeonClearTracer::Trace((CParty *)((char *)this + 3196), "void CParty::ClearDungeon()");
      *((_BYTE *)this + 282) = 1;
      PacketGuard::~PacketGuard(v62);
    }
    else
    {
      *(_QWORD *)((char *)this + 812) = (unsigned int)CSystemTime::getCurTickCount((CSystemTime *)GlobalData::s_systemTime_)
                                      - *(_QWORD *)((char *)this + 844)
                                      - *((_QWORD *)this + 107);
      member_count = CParty::get_member_count((CUser **)this);
      if ( *((_DWORD *)this + 811) )
      {
        v75 = 100;
        for ( j = 0; j <= 3; ++j )
        {
          if ( (unsigned __int8)CParty::_checkValidUser((CUser **)this, j) == 1 )
          {
            if ( *(_BYTE *)(*((_DWORD *)this + 811) + 2170)
              || CDungeon::get_index(*((CDungeon **)this + 811)) == 100
              || *(_BYTE *)(*((_DWORD *)this + 811) + 2204)
              || (char)CDungeon::get_dimension_possible(*((CDungeon **)this + 811)) > 0 )
            {
              v48 = 0;
              APSystem::CUserProc::ClearActionAndSendtoUser();
            }
            if ( (unsigned __int8)CUser::IsUsedCoinInDungeon(*((CUser **)this + 6 * j + 30)) != 1 )
            {
              v48 = 0;
              APSystem::CUserProc::ClearActionAndSendtoUser();
            }
          }
        }
      }
      PacketGuard::PacketGuard((PacketGuard *)v65);
      if ( *(char *)(*((_DWORD *)this + 811) + 2207) <= 0 )
      {
        if ( CDungeon::isTowerOfDespairDungeon(*((CDungeon **)this + 811)) )
        {
          *((_BYTE *)this + 282) = 1;
          Manager = CParty::getManager(this);
          CharacExpandData = (TOD_UserState *)CUser::GetCharacExpandData(Manager, 0xDu);
          TOD_UserState::ClearLayer(CharacExpandData);
          v78 = 5;
          *((_DWORD *)this + 1517) = -1;
          *((_DWORD *)this + 1518) = 0;
          TOD_UserState::getLastEnterLayer((TOD_UserState *)v66, (int)CharacExpandData);
          Layer = TOD_Layer::GetLayer((TOD_Layer *)v66);
          if ( TowerOfDespairMgr::Get_APCIndexByLayer(GlobalData::s_pTowerOfDespairMgr, Layer) != -1 )
          {
            TOD_UserState::getLastEnterLayer((TOD_UserState *)v67, (int)CharacExpandData);
            s = TowerOfDespairMgr::Get_APCNameByLayer(
                  (TowerOfDespair_APC_Mgr **)GlobalData::s_pTowerOfDespairMgr,
                  (const TOD_Layer *)v67);
            if ( s && *s )
            {
              PacketGuard::PacketGuard((PacketGuard *)v56);
              InterfacePacketBuf::put_header(v56, 0, 86);
              InterfacePacketBuf::put_byte(v56, 8);
              TOD_UserState::getLastEnterLayer((TOD_UserState *)v68, (int)CharacExpandData);
              v12 = TOD_Layer::GetLayer((TOD_Layer *)v68);
              InterfacePacketBuf::put_short(v56, v12);
              v13 = strlen(s);
              InterfacePacketBuf::put_int(v56, v13);
              v14 = strlen(s);
              InterfacePacketBuf::put_str(v56, s, v14);
              v15 = (CUserCharacInfo *)CParty::getManager(this);
              s = (char *)CUserCharacInfo::getCurCharacName(v15);
              if ( s )
              {
                v16 = strlen(s);
                InterfacePacketBuf::put_int(v56, v16);
                v17 = strlen(s);
                InterfacePacketBuf::put_str(v56, s, v17);
              }
              else
              {
                InterfacePacketBuf::put_int(v56, 5);
                InterfacePacketBuf::put_str(v56, "Neople", 6);
              }
              InterfacePacketBuf::finalize(v56, 1);
              v18 = G_GameWorld();
              GameWorld::send_all(v18, (PacketGuard *)v56);
              PacketGuard::~PacketGuard(v56);
            }
            v78 = 9;
            *((_DWORD *)this + 1517) = 1252;
            *((_DWORD *)this + 1518) = 1;
          }
          TOD_UserState::getLastEnterLayer((TOD_UserState *)v69, (int)CharacExpandData);
          if ( (unsigned __int16)TOD_Layer::GetLayer((TOD_Layer *)v69) == 100 )
          {
            *((_DWORD *)this + 1517) = 3314;
            *((_DWORD *)this + 1518) = 1;
          }
          WongWork::stGenerateRefData_t::stGenerateRefData_t((WongWork::stGenerateRefData_t *)v49);
          WongWork::stGenerateResult_t::stGenerateResult_t((WongWork::stGenerateResult_t *)v55);
          std::vector<std::pair<int,int>>::vector(v61);
          v51 = 0;
          v50 = 70;
          v52 = 1065353216;
          v54 = *((_DWORD *)this + 811);
          v53 = 0;
          for ( k = 0; k < v78; ++k )
          {
            v19 = G_CDataManager();
            ItemGenerator = CDataManager::getItemGenerator(v19);
            Generator = WongWork::CItemGeneratorMgr::getGenerator(ItemGenerator, 2);
            (*(void (__cdecl **)(int, void **, void **, int))(*(_DWORD *)Generator + 8))(Generator, v49, v55, v48);
          }
          std::pair<int,int>::pair(v70);
          v22 = std::vector<Inven_Item>::size(v55);
          std::vector<std::pair<int,int>>::resize(v61, v22, v70[0], v70[1]);
          for ( m = 0; ; ++m )
          {
            v27 = std::vector<Inven_Item>::size(v55);
            if ( v27 <= m )
              break;
            v83 = std::vector<Inven_Item>::operator[](v55, m);
            v23 = *(_DWORD *)(v83 + 2);
            v24 = (CItemList **)G_CDataManager();
            item = (CItem *)CDataManager::find_item(v24, v23);
            v25 = (_DWORD *)std::vector<std::pair<int,int>>::operator[](v61, m);
            *v25 = *(_DWORD *)(v83 + 2);
            if ( (unsigned __int8)CItem::is_stackable(item) )
            {
              v26 = std::vector<std::pair<int,int>>::operator[](v61, m);
              *(_DWORD *)(v26 + 4) = *(_DWORD *)(v83 + 7);
            }
            else
            {
              *(_DWORD *)(std::vector<std::pair<int,int>>::operator[](v61, m) + 4) = 1;
            }
          }
          std::vector<std::pair<int,int>>::operator=((int *)this + 1514, v61);
          if ( *((_DWORD *)this + 1517) != -1 )
            std::vector<std::pair<int,int>>::push_back((int)v61, (int *)this + 1517);
          std::vector<int>::vector(v60);
          inserted = 0;
          v28 = (CUserCharacInfo *)CParty::getManager(this);
          CurCharacInvenW = (CInventory *)CUserCharacInfo::getCurCharacInvenW(v28);
          inserted = CInventory::insert_event_items(
                       CurCharacInvenW,
                       v61,
                       (int)v60,
                       (char *)MASK_5BIT,
                       55,
                       1,
                       "game_server_msg_191",
                       "game_server_msg_192");
          if ( inserted > 0 && (unsigned int)std::vector<int>::size(v60) > 2 )
          {
            v85 = 0;
            v86 = 0;
            while ( inserted > v85 )
            {
              v31 = *(_DWORD *)std::vector<int>::operator[](v60, v86);
              v32 = (CUserCharacInfo *)CParty::getManager(this);
              CUser::SendUpdateItem(v32, 1, 0, v31);
              ++v85;
              v86 += 3;
            }
          }
          v33 = CParty::gen_timer_key((int)this, 44);
          v34 = CParty::GetPartyIndex(this);
          v35 = G_TimerQueue();
          TimerQueue::InsertTimer(v33, v35, 1, v34, 44, 2, v33, 44);
          Packet_TowerOfDespair_Statistic_GTS::Packet_TowerOfDespair_Statistic_GTS((Packet_TowerOfDespair_Statistic_GTS *)buf);
          TOD_UserState::getLastEnterLayer((TOD_UserState *)v71, (int)CharacExpandData);
          v58 = (unsigned __int16)TOD_Layer::GetLayer((TOD_Layer *)v71);
          v59 = 0;
          ServerProxy = (std::string *)CServerProxyMgr<CStatisticServerProxy>::GetServerProxy(
                                         GlobalData::s_statistic_proxy_mgr,
                                         0);
          CStatisticServerProxy::SendPacket(ServerProxy, buf, 0x17u);
          std::vector<int>::~vector((void **)v60);
          std::vector<std::pair<int,int>>::~vector((void **)v61);
          WongWork::stGenerateResult_t::~stGenerateResult_t(v55);
          WongWork::stGenerateRefData_t::~stGenerateRefData_t(v49);
        }
        else if ( CDungeon::isTournamentDungeon(*((CDungeon **)this + 811)) )
        {
          *((_BYTE *)this + 282) = 1;
          CTournamentDungeonReward::ResetReward((CParty *)((char *)this + 6076));
          CParty::MakeTournamentReward(a1, this, (CParty *)((char *)this + 6076));
        }
        else
        {
          InterfacePacketBuf::put_header(v65, 0, 31);
          InterfacePacketBuf::finalize(v65, 1);
        }
      }
      else
      {
        v5 = (std::string *)*((_DWORD *)this + 811);
        BloodRoundOrder = CBattle_Field::CBloodRound::getBloodRoundOrder((CParty *)((char *)this + 3536));
        CBattle_Field::CBloodClearRewardData::onFinishBloodRound(
          (CParty *)((char *)this + 3548),
          1,
          BloodRoundOrder,
          (CUserCharacInfo **)this,
          v5);
        v7 = CParty::gen_timer_key((int)this, 34);
        v8 = CParty::GetPartyIndex(this);
        v9 = G_TimerQueue();
        TimerQueue::InsertTimer(v7, v9, 1, v8, 34, 5, v7, 34);
      }
      for ( n = 0; n <= 3; ++n )
      {
        if ( (unsigned __int8)CParty::_checkValidUser((CUser **)this, n) == 1 )
        {
          CurCharacQuestW = CUser::getCurCharacQuestW(*((CUser **)this + 6 * n + 30));
          UserQuest::set_authen_data((int)CurCharacQuestW, 4, member_count, 0);
          v38 = *(_QWORD *)((char *)this + 812) / 1000LL;
          v39 = CUser::getCurCharacQuestW(*((CUser **)this + 6 * n + 30));
          UserQuest::set_authen_data((int)v39, 2, v38, 0);
          if ( !*(_BYTE *)(*((_DWORD *)this + 811) + 2207)
            && !CDungeon::isTowerOfDespairDungeon(*((CDungeon **)this + 811))
            && !CDungeon::isTournamentDungeon(*((CDungeon **)this + 811)) )
          {
            CUser::Send(*((CUser **)this + 6 * n + 30), (PacketGuard *)v65);
          }
          if ( secretshop::SECRET_SHOP_DATA::IsOpen((CParty *)((char *)this + 676)) )
          {
            standard_level = CDungeon::get_standard_level(*((CDungeon **)this + 811));
            index = CDungeon::get_index(*((CDungeon **)this + 811));
            v47 = __PAIR64__((unsigned int)this + 28 * n + 680, (unsigned int)GlobalData::s_secret_shop);
            secretshop::CSecretShop::LotteryItems(
              v47,
              (_DWORD *)HIDWORD(v47),
              *((_DWORD *)this + 169),
              index,
              standard_level);
            CParty::SendSecretShopItemList((CUser **)this, *((CUser **)this + 6 * n + 30));
            APSystem::CUserProc::ClearActionAndSendtoUser();
          }
          if ( CUser::getBreakAwayDungeonClearCnt(*((CUser **)this + 6 * n + 30)) > 0 )
            CUser::subBreakAwayDungeonClearCnt(*((CUser **)this + 6 * n + 30));
        }
      }
      v73 = CParty::get_member_count((CUser **)this);
      if ( v73 == 1 )
      {
        G_CDataManager();
        hellparty_script_values = CDataManager::get_hellparty_script_values();
        BattleData::SetHellPartyValueTotal((CParty *)((char *)this + 812), hellparty_script_values[2]);
      }
      if ( v73 != 1 && *((_BYTE *)this + 3310) != 1 && *((_BYTE *)this + 3311) != 1 && *((_BYTE *)this + 3292) != 1 )
      {
        v44 = v73 - 1;
        rate_level_difference = CParty::get_rate_level_difference(this);
        dungeon_diff = CBattle_Field::get_dungeon_diff((CParty *)((char *)this + 2852));
        CParty::cal_hellparty_value(this, dungeon_diff, rate_level_difference, v44);
      }
      CDungeonClearTracer::Trace((CParty *)((char *)this + 3196), "void CParty::ClearDungeon()");
      *((_BYTE *)this + 282) = 1;
      QuickParty::RewardData_Param::RewardData_Param((QuickParty::RewardData_Param *)v64, this);
      QuickParty::RewardData_Result::RewardData_Result((QuickParty::RewardData_Result *)v63);
      CParty::set_random_reward(this, 0xAu, v64[0], v64[1], (int)v63);
      QuickParty::RewardData_Result::clear((QuickParty::RewardData_Result *)v63);
      PacketGuard::~PacketGuard(v65);
    }
  }
}

void __usercall CParty::prepare_dungeon_clear(int id@<esi>, CParty *this)
{
  bool v2; // al
  int cur_map; // ebx
  char *CurCharacQuestW; // eax
  CInventory *CurCharacInvenR; // eax
  char *CreatureMgrR; // eax
  int id; // ebx
  CInventory *CurCharacInvenW; // eax
  char *CreatureMgrW; // eax
  int CurrentMapIndex; // eax
  int i; // [esp+1Ch] [ebp-Ch]

  CDungeonClearTracer::SetClearedDungeon((CParty *)((char *)this + 3196), *((_BYTE *)this + 282));
  v2 = CBattle_Field::check_end_point((CParty *)((char *)this + 2852));
  CDungeonClearTracer::SetEndPoint((CParty *)((char *)this + 3196), v2);
  if ( (unsigned __int8)CBattle_Field::check_grid_clear((CParty *)((char *)this + 2852)) )
  {
    for ( i = 0; i <= 3; ++i )
    {
      if ( (unsigned __int8)CParty::_checkValidUser((CUser **)this, i) == 1 && *((_DWORD *)this + 822) != 1 )
      {
        cur_map = CBattle_Field::get_cur_map((CParty *)((char *)this + 2852));
        CurCharacQuestW = CUser::getCurCharacQuestW(*((CUser **)this + 6 * i + 30));
        UserQuest::set_authen_data((int)CurCharacQuestW, 1, cur_map, 0);
        if ( *(int *)(*((_DWORD *)this + 6 * i + 30) + 581772) > 0 )
        {
          CurCharacInvenR = (CInventory *)CUserCharacInfo::getCurCharacInvenR(*((CUserCharacInfo **)this + 6 * i + 30));
          CreatureMgrR = CInventory::GetCreatureMgrR(CurCharacInvenR);
          if ( !user_creature::CCreatureMgr::IsGrowCreature_Equipped_Creature((user_creature::CCreatureMgr *)CreatureMgrR) )
          {
            v7 = *(_DWORD *)(*((_DWORD *)this + 6 * i + 30) + 581772);
            CurCharacInvenW = (CInventory *)CUserCharacInfo::getCurCharacInvenW(*((CUserCharacInfo **)this + 6 * i + 30));
            CreatureMgrW = CInventory::GetCreatureMgrW(CurCharacInvenW);
            user_creature::CCreatureMgr::GainExp((user_creature::CCreatureMgr *)CreatureMgrW, v7);
          }
        }
        *(_DWORD *)(*((_DWORD *)this + 6 * i + 30) + 581772) = 0;
      }
    }
    CurrentMapIndex = CBattle_Field::GetCurrentMapIndex((CMap **)this + 713);
    if ( CBattle_Field::ClearCondition((int)this + 2852, 1u, CurrentMapIndex)
      || CBattle_Field::check_end_point((CParty *)((char *)this + 2852)) )
    {
      CParty::ClearDungeon(a1, this);
    }
  }
}

int __cdecl CParty::OnTimeoutBossDie(CParty *this, int id, unsigned int value)
{
  int id; // ebx
  int party_seatno; // eax
  int MonsterMap; // eax
  int id; // eax
  int id; // eax
  _DWORD *v9; // ebx
  int id; // eax
  int id; // eax
  int id; // eax
  _DWORD *v13; // eax
  int id; // ebx
  int id; // esi
  int PartyIndex; // ebx
  TimerQueue *v17; // eax
  std::string *v18; // ebx
  __int16 BloodRoundOrder; // ax
  int id; // esi
  int id; // ebx
  TimerQueue *v22; // eax
  void *s; // [esp+24h] [ebp-AA4h] BYREF
  _DWORD value[655]; // [esp+28h] [ebp-AA0h] BYREF
  PacketBuf *v26[3]; // [esp+A64h] [ebp-64h] BYREF
  PacketBuf *v27[3]; // [esp+A70h] [ebp-58h] BYREF
  int id; // [esp+A7Ch] [ebp-4Ch] BYREF
  int id; // [esp+A80h] [ebp-48h] BYREF
  std::_Rb_tree_node_base *v30; // [esp+A84h] [ebp-44h] BYREF
  void *v31[3]; // [esp+A88h] [ebp-40h] BYREF
  int id; // [esp+A94h] [ebp-34h] BYREF
  _BYTE dest[4]; // [esp+A98h] [ebp-30h] BYREF
  int id; // [esp+A9Ch] [ebp-2Ch] BYREF
  int id; // [esp+AA0h] [ebp-28h] BYREF
  int id[4]; // [esp+AA4h] [ebp-24h] BYREF
  int id; // [esp+AB4h] [ebp-14h] BYREF
  CUser *MemberSlotNo; // [esp+AB8h] [ebp-10h]
  char flag; // [esp+ABFh] [ebp-9h]

  if ( *((_DWORD *)this + 786) != a2 )
    return 0;
  MemberSlotNo = (CUser *)CParty::GetMemberSlotNo(this, a3);
  if ( !MemberSlotNo )
    return 0;
  if ( *((_DWORD *)this + 811) && (unsigned __int8)CDungeon::IsEnterEachMap(*((CDungeon **)this + 811)) )
  {
    party_seatno = CParty::get_party_seatno((CUser **)this, MemberSlotNo);
    CBattle_Field::SetCurSeatNo((CParty *)((char *)this + 2852), party_seatno);
  }
  std::vector<int>::vector(v31);
  MonsterMap = CBattle_Field::GetMonsterMap((CParty *)((char *)this + 2852));
  std::map<int,map_monster>::begin(&v30, MonsterMap);
  while ( 1 )
  {
    v8 = CBattle_Field::GetMonsterMap((CParty *)((char *)this + 2852));
    std::map<int,map_monster>::end(&v32, v8);
    if ( !std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator!=(&v30, &v32) )
      break;
    map_monster::map_monster((map_monster *)&s);
    v7 = std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator->(&v30);
    map_monster::operator=((int)&s, v7 + 4);
    if ( v25[10] )
      std::vector<int>::push_back((int)v31, v25);
    map_monster::~map_monster(&s);
    std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator++(dest, &v30);
  }
  v39 = 0;
  std::vector<int>::begin(&v29, v31);
  while ( 1 )
  {
    std::vector<int>::end(&v34, (int)v31);
    if ( !__gnu_cxx::operator!=<int *,std::vector<int>>((int)&v29, (int)&v34) )
    {
      v3 = 1;
      goto LABEL_29;
    }
    memset(&s, 0, 0xA3Fu);
    v9 = (_DWORD *)__gnu_cxx::__normal_iterator<int *,std::vector<int>>::operator*((int)&v29);
    v10 = CBattle_Field::GetMonsterMap((CParty *)((char *)this + 2852));
    std::map<int,map_monster>::find(&v28, v10, v9);
    v11 = CBattle_Field::GetMonsterMap((CParty *)((char *)this + 2852));
    std::map<int,map_monster>::end(&v35, v11);
    if ( std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator!=(&v28, &v35) )
    {
      v12 = std::_Rb_tree_iterator<std::pair<int const,map_monster>>::operator->(&v28);
      *(_DWORD *)((char *)&v25[645] + 2) = ActiveStaticInfo::getHpMax((ActiveStaticInfo *)(v12 + 60));
    }
    v13 = (_DWORD *)__gnu_cxx::__normal_iterator<int *,std::vector<int>>::operator*((int)&v29);
    if ( (unsigned __int8)CParty::kill_monster(this, MemberSlotNo, *v13, INVALID_ID_U16, (int)&s, 0) != 1 )
      break;
    v39 = 1;
    __gnu_cxx::__normal_iterator<int *,std::vector<int>>::operator++(&v37, &v29);
  }
  v14 = *(_DWORD *)__gnu_cxx::__normal_iterator<int *,std::vector<int>>::operator*((int)&v29);
  cMyTrace::cMyTrace((cMyTrace *)v36, "bool CParty::OnTimeoutBossDie(int, memberIdentificationNumber_t)", 8910, 0);
  cMyTrace::operator()((int)v36, "KILLBOSS2 server - monster id(%d) failed", v14);
  if ( *((_DWORD *)this + 822) == 1 )
  {
    CParty::SetEPLPState(this, 2);
    v15 = CParty::gen_timer_key((int)this, 21);
    PartyIndex = CParty::GetPartyIndex(this);
    v17 = G_TimerQueue();
    TimerQueue::InsertTimer(v15, v17, 1, PartyIndex, 21, 60, v15, 0);
    CParty::set_state(this, 1);
    PacketGuard::PacketGuard((PacketGuard *)v27);
    InterfacePacketBuf::put_header(v27, 0, 254);
    InterfacePacketBuf::finalize(v27, 1);
    CParty::send_to_party((CUser **)this, (PacketGuard *)v27);
    v3 = 1;
    PacketGuard::~PacketGuard(v27);
  }
  else
  {
    if ( *(char *)(*((_DWORD *)this + 811) + 2207) <= 0 )
    {
      PacketGuard::PacketGuard((PacketGuard *)v26);
      InterfacePacketBuf::put_header(v26, 0, 31);
      InterfacePacketBuf::finalize(v26, 1);
      CParty::send_to_party((CUser **)this, (PacketGuard *)v26);
      PacketGuard::~PacketGuard(v26);
    }
    else
    {
      v18 = (std::string *)*((_DWORD *)this + 811);
      BloodRoundOrder = CBattle_Field::CBloodRound::getBloodRoundOrder((CParty *)((char *)this + 3536));
      CBattle_Field::CBloodClearRewardData::onFinishBloodRound(
        (CParty *)((char *)this + 3548),
        1,
        BloodRoundOrder,
        (CUserCharacInfo **)this,
        v18);
      v20 = CParty::gen_timer_key((int)this, 34);
      v21 = CParty::GetPartyIndex(this);
      v22 = G_TimerQueue();
      TimerQueue::InsertTimer(v20, v22, 1, v21, 34, 5, v20, 34);
    }
    v3 = 1;
  }
LABEL_29:
  std::vector<int>::~vector(v31);
  return v3;
}

int __cdecl CParty::_putItemIntoUser(CUser **this, CUser *user, Inven_Item *item, PacketBuf **a4)
{
  CInventory *CurCharacInvenR; // eax
  unsigned int acc_id; // eax
  char flag; // bl
  unsigned int add_info; // ebx
  int CurCharacInvenW; // eax
  int id; // eax
  int id; // ebx
  int id; // eax
  int id; // ebx
  void *InstanceValueStatistic; // eax
  int id; // ebx
  void *InstanceCirculationStatistic; // eax
  void *v16; // eax
  int id; // eax
  CInventory *inventory; // eax
  int money; // eax
  int id; // ebx
  char ItemSpace; // al
  char *v22; // ebx
  int *v23; // eax
  int InventoryType; // ebx
  CInventory *inventory; // eax
  int id; // eax
  char ItemAttr; // al
  char AbilityType; // al
  __int16 AbilityValue; // ax
  WongWork::CAvatarItemMgr *v30; // ebx
  CInventory *inventory; // eax
  char *AvatarItemMgrR; // eax
  char *JewelSocketData; // eax
  WongWork::CAvatarItemMgr *v34; // ebx
  CInventory *inventory; // eax
  char *v36; // eax
  unsigned int id; // ebx
  CItemList **v38; // eax
  __int16 v39; // bx
  Store *v40; // eax
  int id; // ebx
  void *v42; // eax
  int grade; // ebx
  int rarity; // eax
  int RepeatEvent; // eax
  unsigned int id; // ebx
  CItemList **v48; // eax
  const char *String; // eax
  const char *v50; // eax
  int ServerGroup; // ebx
  size_t v52; // esi
  int CurCharacNo; // eax
  unsigned int id; // ebx
  CItemList **v55; // eax
  const char *v56; // eax
  const char *v57; // eax
  int id; // ebx
  size_t v59; // esi
  int id; // eax
  char flag[4]; // [esp+58h] [ebp-1B0h]
  char s[256]; // [esp+6Eh] [ebp-19Ah] BYREF
  char src[21]; // [esp+16Eh] [ebp-9Ah] BYREF
  char dest[21]; // [esp+183h] [ebp-85h] BYREF
  int id; // [esp+198h] [ebp-70h] BYREF
  char flag[4]; // [esp+19Ch] [ebp-6Ch] BYREF
  int index; // [esp+1A0h] [ebp-68h] BYREF
  int id[4]; // [esp+1A4h] [ebp-64h] BYREF
  int id[4]; // [esp+1B4h] [ebp-54h] BYREF
  int id; // [esp+1C4h] [ebp-44h]
  int id; // [esp+1C8h] [ebp-40h]
  int inserted; // [esp+1CCh] [ebp-3Ch]
  CDungeon *dungeon; // [esp+1D0h] [ebp-38h]
  bool isTournamentDungeon; // [esp+1D7h] [ebp-31h]
  int id; // [esp+1D8h] [ebp-30h]
  Inven_Item *InvenRef; // [esp+1DCh] [ebp-2Ch]
  CItem *item; // [esp+1E0h] [ebp-28h]
  char *ExpansionInfo; // [esp+1E4h] [ebp-24h]

  v71 = 0;
  v72 = 0;
  InterfacePacketBuf::clear(a4);
  InterfacePacketBuf::put_header(a4, 0, 14);
  InterfacePacketBuf::put_byte(a4, 0);
  index = InterfacePacketBuf::get_index(a4);
  InterfacePacketBuf::put_short(a4, 0);
  if ( Inven_Item::get_add_info(a3) > 0 )
  {
    if ( Inven_Item::get_add_info(a3) > 499999 )
    {
      CBattle_Field::get_dungeon_diff((CBattle_Field *)(this + 713));
      CParty::get_member_count(this);
      Inven_Item::get_add_info(a3);
      *(_DWORD *)v62 = *(this + 814);
      cMyTrace::cMyTrace((cMyTrace *)v69, "int CParty::_putItemIntoUser(CUser*, Inven_Item*, PacketGuard&)", 9005, 4);
      cMyTrace::operator()((int)v69, "Dungeon(%d) reward money(%d) on %d' party member with difficulty %d", v62[0]);
      CurCharacInvenR = (CInventory *)CUserCharacInfo::getCurCharacInvenR(a2);
      CInventory::get_money(CurCharacInvenR);
      Inven_Item::get_add_info(a3);
      CUserCharacInfo::getCurCharacName(a2);
      acc_id = CUser::get_acc_id(a2);
      v6 = NumberToString(acc_id, 0);
      cMyTrace::cMyTrace((cMyTrace *)v70, "int CParty::_putItemIntoUser(CUser*, Inven_Item*, PacketGuard&)", 9015, 4);
      cMyTrace::operator()((int)v70, "User(%s) Charac(%s) gain money(%d), old money is %d.", v6);
    }
    add_info = Inven_Item::get_add_info(a3);
    CurCharacInvenW = CUserCharacInfo::getCurCharacInvenW(a2);
    v76 = CInventory::gain_money(CurCharacInvenW, add_info, (char *)5, 1, 0);
    v9 = Inven_Item::get_add_info(a3);
    if ( v9 > v76 )
    {
      v10 = v76;
      v11 = Inven_Item::get_add_info(a3);
      CUser::SendMoneyFullReason(a2, 0, v11, v10);
    }
    v12 = v76;
    InstanceValueStatistic = GetInstanceValueStatistic();
    CValueStatistic::AddValueStatistic((int)InstanceValueStatistic, 3u, a2, v12);
    if ( *(this + 811) )
    {
      v14 = v76;
      if ( *((_BYTE *)*(this + 811) + 2206) )
      {
        InstanceCirculationStatistic = GetInstanceCirculationStatistic();
        CCirculationStatistic::AddCirculationStatistic((int)InstanceCirculationStatistic, 0xFu, a2, v14);
      }
      else
      {
        v16 = GetInstanceCirculationStatistic();
        CCirculationStatistic::AddCirculationStatistic((int)v16, 1u, a2, v14);
      }
    }
    v17 = Inven_Item::get_add_info(a3);
    CUser::saveTaxMoneyForUpperMember(a2, v17);
    InterfacePacketBuf::put_short(a4, 0);
    InterfacePacketBuf::put_int(a4, 0);
    v18 = (CInventory *)CUserCharacInfo::getCurCharacInvenR(a2);
    money = CInventory::get_money(v18);
    InterfacePacketBuf::put_int(a4, money);
    InterfacePacketBuf::put_byte(a4, 0);
    InterfacePacketBuf::put_short(a4, 0);
    InterfacePacketBuf::put_byte(a4, 0);
    InterfacePacketBuf::put_int(a4, 0);
    InterfacePacketBuf::put_byte(a4, 0);
    InterfacePacketBuf::put_short(a4, 0);
    InterfacePacketBuf::put_byte(a4, 0);
    InterfacePacketBuf::put_packet(a4, (const Inven_Item *)&g_emptySlot);
    ++v71;
  }
  v20 = v72;
  if ( v20 != Inven_Item::GetItemSpace((Inven_Item *)((char *)a3 + 61)) )
  {
    if ( v71 > 0 )
    {
      InterfacePacketBuf::put_short(a4, &index, v71);
      InterfacePacketBuf::finalize(a4, 1);
      CUser::Send(a2, (PacketGuard *)a4);
    }
    v71 = 0;
    InterfacePacketBuf::clear(a4);
    InterfacePacketBuf::put_header(a4, 0, 14);
    ItemSpace = Inven_Item::GetItemSpace((Inven_Item *)((char *)a3 + 61));
    InterfacePacketBuf::put_byte(a4, ItemSpace);
    index = InterfacePacketBuf::get_index(a4);
    InterfacePacketBuf::put_short(a4, v71);
  }
  v22 = (char *)a3 + 61;
  v23 = (int *)CUserCharacInfo::getCurCharacInvenW(a2);
  inserted = CInventory::insertItemIntoInventory(
               v23,
               *(_QWORD *)v22,
               *((_DWORD *)v22 + 2),
               *((_DWORD *)v22 + 3),
               *((_DWORD *)v22 + 4),
               *((_DWORD *)v22 + 5),
               *((_DWORD *)v22 + 6),
               *((_DWORD *)v22 + 7),
               *((_DWORD *)v22 + 8),
               *((_DWORD *)v22 + 9),
               *((_DWORD *)v22 + 10),
               *((_DWORD *)v22 + 11),
               *((_DWORD *)v22 + 12),
               *((_DWORD *)v22 + 13),
               *((_DWORD *)v22 + 14),
               v22[60],
               11,
               1);
  v74 = *(this + 811);
  isTournamentDungeon = 0;
  if ( v74 )
    isTournamentDungeon = CDungeon::isTournamentDungeon(v74);
  if ( inserted <= 0 )
  {
    RepeatEvent = CEventManager::GetRepeatEvent(GlobalData::s_event_manager, 65);
    if ( (*(unsigned __int8 (__cdecl **)(int, _DWORD))(*(_DWORD *)RepeatEvent + 52))(RepeatEvent, 0)
      && *(_DWORD *)((char *)a3 + 63)
      && *(_DWORD *)((char *)a3 + 63) != -1
      && !isTournamentDungeon )
    {
      v47 = *(_DWORD *)((char *)a3 + 63);
      v48 = (CItemList **)G_CDataManager();
      if ( CDataManager::find_item(v48, v47) )
      {
        String = RDARScriptStringManager::findString(
                   (RDARScriptStringManager *)&g_scriptStringManager_,
                   (char *)4,
                   "game_server_msg_155",
                   0);
        strncpy(dest, String, SIZE_20_BYTES);
        v50 = RDARScriptStringManager::findString(
                (RDARScriptStringManager *)&g_scriptStringManager_,
                (char *)4,
                "game_server_msg_104",
                0);
        strncpy(s, v50, MASK_BYTE);
        ServerGroup = CUser::GetServerGroup(a2);
        v52 = strlen(s);
        CurCharacNo = CUserCharacInfo::getCurCharacNo(a2);
        WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
          dest,
          (Inven_Item *)((char *)a3 + 61),
          0,
          CurCharacNo,
          s,
          v52,
          8,
          ServerGroup,
          0,
          0);
      }
    }
    else if ( *(_DWORD *)((char *)a3 + 63) )
    {
      if ( *(_DWORD *)((char *)a3 + 63) != -1 && isTournamentDungeon )
      {
        v54 = *(_DWORD *)((char *)a3 + 63);
        v55 = (CItemList **)G_CDataManager();
        if ( CDataManager::find_item(v55, v54) )
        {
          v56 = RDARScriptStringManager::findString(
                  (RDARScriptStringManager *)&g_scriptStringManager_,
                  (char *)4,
                  "game_server_msg_250",
                  0);
          strncpy(src, v56, SIZE_20_BYTES);
          v57 = RDARScriptStringManager::findString(
                  (RDARScriptStringManager *)&g_scriptStringManager_,
                  (char *)4,
                  "game_server_msg_104",
                  0);
          strncpy(s, v57, MASK_BYTE);
          v58 = CUser::GetServerGroup(a2);
          v59 = strlen(s);
          v60 = CUserCharacInfo::getCurCharacNo(a2);
          WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
            src,
            (Inven_Item *)((char *)a3 + 61),
            0,
            v60,
            s,
            v59,
            8,
            v58,
            0,
            0);
        }
      }
    }
  }
  else
  {
    InventoryType = Inven_Item::GetInventoryType((Inven_Item *)((char *)a3 + 61));
    v25 = (CInventory *)CUserCharacInfo::getCurCharacInvenR(a2);
    InvenRef = (Inven_Item *)CInventory::GetInvenRef(v25, InventoryType, inserted);
    InterfacePacketBuf::put_short(a4, inserted);
    InterfacePacketBuf::put_int(a4, *(_DWORD *)((char *)InvenRef + 2));
    v26 = Inven_Item::get_add_info(InvenRef);
    InterfacePacketBuf::put_int(a4, v26);
    ItemAttr = Inven_Item::GetItemAttr(InvenRef);
    InterfacePacketBuf::put_byte(a4, ItemAttr);
    InterfacePacketBuf::put_short(a4, *(_WORD *)((char *)InvenRef + 11));
    InterfacePacketBuf::put_byte(a4, *(_BYTE *)InvenRef);
    InterfacePacketBuf::put_int(a4, *(_DWORD *)((char *)InvenRef + 13));
    AbilityType = stAmplifyOption_t::getAbilityType((Inven_Item *)((char *)InvenRef + 17));
    InterfacePacketBuf::put_byte(a4, AbilityType);
    AbilityValue = stAmplifyOption_t::getAbilityValue((Inven_Item *)((char *)InvenRef + 17));
    InterfacePacketBuf::put_short(a4, AbilityValue);
    if ( Inven_Item::isAvatarItemType(InvenRef) )
    {
      InterfacePacketBuf::put_int(a4, 30);
      v30 = (WongWork::CAvatarItemMgr *)Inven_Item::get_add_info(InvenRef);
      v31 = (CInventory *)CUserCharacInfo::getCurCharacInvenR(a2);
      AvatarItemMgrR = CInventory::GetAvatarItemMgrR(v31);
      JewelSocketData = WongWork::CAvatarItemMgr::getJewelSocketData((WongWork::CAvatarItemMgr *)AvatarItemMgrR, v30);
      InterfacePacketBuf::put_binary(a4, JewelSocketData, 30);
      v34 = (WongWork::CAvatarItemMgr *)Inven_Item::get_add_info(InvenRef);
      v35 = (CInventory *)CUserCharacInfo::getCurCharacInvenR(a2);
      v36 = CInventory::GetAvatarItemMgrR(v35);
      ExpansionInfo = WongWork::CAvatarItemMgr::GetExpansionInfo((WongWork::CAvatarItemMgr *)v36, v34);
      InterfacePacketBuf::put_int(a4, 4);
      if ( ExpansionInfo )
      {
        InterfacePacketBuf::put_binary(a4, ExpansionInfo, 4);
      }
      else
      {
        stAvatarExpansionInfo_t::init((stAvatarExpansionInfo_t *)v67);
        InterfacePacketBuf::put_binary(a4, v67, 4);
      }
    }
    InterfacePacketBuf::put_byte(a4, 0);
    InterfacePacketBuf::put_packet(a4, InvenRef);
    ++v71;
    v37 = *(_DWORD *)((char *)a3 + 63);
    v38 = (CItemList **)G_CDataManager();
    item = (CItem *)CDataManager::find_item(v38, v37);
    if ( item )
    {
      v66 = 0;
      v39 = *((_DWORD *)a3 + 17);
      v40 = (Store *)G_Store();
      Store::GetSellItemPrice(v40, (Inven_Item *)((char *)a3 + 61), item, v39, 0, &v66);
      v41 = v66;
      v42 = GetInstanceValueStatistic();
      CValueStatistic::AddValueStatistic((int)v42, 4u, a2, v41);
    }
    if ( *((_BYTE *)a3 + 62) == 1 && item )
    {
      grade = CItem::get_grade(item);
      rarity = CItem::get_rarity(item);
      CLuckPoint::UseLuckPoint(a2, (CUser *)*((unsigned __int8 *)a3 + 62), rarity, grade);
    }
  }
  if ( v71 > 0 )
  {
    InterfacePacketBuf::put_short(a4, &index, v71);
    InterfacePacketBuf::finalize(a4, 1);
    CUser::Send(a2, (PacketGuard *)a4);
  }
  return v71;
}

void __usercall CParty::dungeon_clear(int id@<esi>, CUser **this)
{
  int id; // eax
  int id; // eax
  unsigned int id; // ebx
  CItemList **v5; // eax
  __int16 v6; // bx
  Inven_Item *item; // esi
  Store *v8; // eax
  CUser *user; // ebx
  void *InstanceValueStatistic; // eax
  int id; // eax
  int RepeatEvent; // eax
  bool v13; // al
  int id; // eax
  int id; // esi
  int PartyIndex; // ebx
  TimerQueue *v17; // eax
  int id; // eax
  char standard_level; // al
  int id; // [esp+28h] [ebp-30h] BYREF
  PacketBuf *v21[3]; // [esp+2Ch] [ebp-2Ch] BYREF
  int id; // [esp+38h] [ebp-20h]
  int member_count; // [esp+3Ch] [ebp-1Ch]
  int id; // [esp+40h] [ebp-18h]
  int i; // [esp+44h] [ebp-14h]
  int id; // [esp+48h] [ebp-10h]
  CItem *item; // [esp+4Ch] [ebp-Ch]

  CParty::SaveCoin(this);
  v22 = 0;
  PacketGuard::PacketGuard((PacketGuard *)v21);
  member_count = CParty::get_member_count(this);
  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::_checkValidUser(this, i) == 1 )
    {
      v26 = 0;
      v2 = CParty::_putItemIntoUser(this, *(this + 6 * i + 30), (Inven_Item *)((char *)this + 122 * i + 928), v21);
      v26 += v2;
      v3 = CParty::_putItemIntoUser(this, *(this + 6 * i + 30), (Inven_Item *)((char *)this + 122 * i + 1416), v21);
      v26 += v3;
      if ( (unsigned __int8)CParty::HaveAlreadySelectedCard((int)this, i, 1) )
      {
        v4 = *(unsigned int *)((char *)this + 122 * i + 1967);
        v5 = (CItemList **)G_CDataManager();
        item = (CItem *)CDataManager::find_item(v5, v4);
        if ( item )
        {
          v20 = 0;
          v6 = (unsigned __int16)*(CUser **)((char *)this + 122 * i + 1972);
          v7 = (Inven_Item *)((char *)this + 122 * i + 1965);
          v8 = (Store *)G_Store();
          Store::GetSellItemPrice(v8, v7, item, v6, 0, &v20);
          a1 = v20;
          v9 = *(this + 6 * i + 30);
          InstanceValueStatistic = GetInstanceValueStatistic();
          CValueStatistic::AddValueStatistic((int)InstanceValueStatistic, 5u, v9, a1);
        }
        v11 = CParty::_putItemIntoUser(this, *(this + 6 * i + 30), (Inven_Item *)((char *)this + 122 * i + 1904), v21);
        v26 += v11;
        RepeatEvent = CEventManager::GetRepeatEvent(GlobalData::s_event_manager, 64);
        v13 = (*(unsigned __int8 (__cdecl **)(int, _DWORD))(*(_DWORD *)RepeatEvent + 52))(RepeatEvent, 0)
           || (unsigned __int8)CParty::IsReturnUserParty(this)
           || (unsigned __int8)CParty::IsEventCharacParty(this);
        if ( v13 && (unsigned __int8)CParty::IsPremiumGoldCardParty((CParty *)this) != 1 )
        {
          v14 = CParty::_putItemIntoUser(this, *(this + 6 * i + 30), (Inven_Item *)((char *)this + 122 * i + 1904), v21);
          v26 += v14;
        }
        ++v22;
      }
      Inven_Item::reset((Inven_Item *)((char *)this + 122 * i + 928));
      Inven_Item::reset((Inven_Item *)((char *)this + 122 * i + 989));
      Inven_Item::reset((Inven_Item *)((char *)this + 122 * i + 1416));
      Inven_Item::reset((Inven_Item *)((char *)this + 122 * i + 1477));
      Inven_Item::reset((Inven_Item *)((char *)this + 122 * i + 1904));
      Inven_Item::reset((Inven_Item *)((char *)this + 122 * i + 1965));
      BattleData::ResetEventDungeonClearPoint((BattleData *)(this + 203));
      HistoryLog::WriteDunClear(*((FILE **)*(this + 6 * i + 30) + 124350));
      CParty::OnSuccessedDungeonClear(a1, (CParty *)this, (CUser **)*(this + 6 * i + 30));
    }
  }
  CParty::SetEPLPState((CParty *)this, 2);
  v24 = 60;
  if ( secretshop::SECRET_SHOP_DATA::IsOpen((secretshop::SECRET_SHOP_DATA *)(this + 169)) )
    v24 = 60 * *((_DWORD *)G_CDataManager() + 10721);
  v15 = CParty::gen_timer_key((int)this, 21);
  PartyIndex = CParty::GetPartyIndex((CParty *)this);
  v17 = G_TimerQueue();
  TimerQueue::InsertTimer(v15, v17, 1, PartyIndex, 21, v24, v15, 0);
  CParty::set_state((CParty *)this, 1);
  v18 = CParty::checkInoutConditionDungeon(PartyIndex, (CParty *)this);
  CParty::sendInoutConditionDungeon(this, v18);
  if ( (unsigned __int8)CParty::IsPremiumGoldCardParty((CParty *)this) )
  {
    standard_level = CDungeon::get_standard_level(*(this + 811));
    CParty::sendPremiumGoldCardStatistic((CParty *)this, standard_level, member_count, v22);
  }
  CParty::CheckHackAverageHitCount((CParty *)this);
  CParty::ClearMapHitCount((CParty *)this);
  Secu_HackLogCheckByParty::dungeonClear((Secu_HackLogCheckByParty *)(this + 1711));
  PacketGuard::~PacketGuard(v21);
}

int __cdecl CParty::checkInoutConditionDungeon(CUser **a1, CDungeon *dungeon, int id, int id2)
{
  int id; // ebx
  Inven_Item *item; // ebx
  int i; // esi
  int id; // eax
  bool v8; // al
  char flag; // al
  const char *CurCharacNo; // eax
  _BYTE s[244]; // [esp+28h] [ebp-140h] BYREF
  PacketBuf *v13[3]; // [esp+11Ch] [ebp-4Ch] BYREF
  int id; // [esp+128h] [ebp-40h] BYREF
  int id[4]; // [esp+12Ch] [ebp-3Ch] BYREF
  unsigned __int8 v16; // [esp+13Fh] [ebp-29h] BYREF
  PacketBuf *v17[3]; // [esp+140h] [ebp-28h] BYREF
  int id; // [esp+14Ch] [ebp-1Ch]
  int j; // [esp+150h] [ebp-18h]
  int k; // [esp+154h] [ebp-14h]
  int id; // [esp+158h] [ebp-10h]
  int m; // [esp+15Ch] [ebp-Ch]

  if ( a2 )
  {
    PacketGuard::PacketGuard((PacketGuard *)v17);
    v16 = 0;
    v5 = (Inven_Item *)s;
    for ( i = 3; i != -1; --i )
    {
      Inven_Item::Inven_Item(v5);
      v5 = (Inven_Item *)((char *)v5 + 61);
    }
    memset(s, 0, sizeof(s));
    memset(v15, 0, sizeof(v15));
    if ( a4 == 444 )
    {
      for ( j = 0; j <= 3; ++j )
        CParty::checkValidUser(a1, j);
      for ( k = 0; k <= 3; ++k )
      {
        if ( (unsigned __int8)CParty::checkValidUser(a1, k) == 1
          && (unsigned __int8)CUserCharacInfo::getCurCharacStamina(a1[6 * k + 30]) != 100 )
        {
          InterfacePacketBuf::clear(v17);
          InterfacePacketBuf::put_header(v17, 1, 444);
          InterfacePacketBuf::put_byte(v17, 0);
          InterfacePacketBuf::put_byte(v17, 234);
          InterfacePacketBuf::put_byte(v17, v16);
          InterfacePacketBuf::finalize(v17, 1);
          CParty::send_to_party(a1, (PacketGuard *)v17);
          v4 = 234;
          goto LABEL_59;
        }
      }
    }
    if ( !CParty::CheckEnterAncientDungeon(a1, a2, &v16, (Inven_Item *)s, v15) )
    {
      InterfacePacketBuf::clear(v17);
      InterfacePacketBuf::put_header(v17, 1, a4);
      InterfacePacketBuf::put_byte(v17, 0);
      InterfacePacketBuf::put_byte(v17, 17);
      InterfacePacketBuf::put_byte(v17, v16);
      InterfacePacketBuf::finalize(v17, 1);
      CParty::send_to_party(a1, (PacketGuard *)v17);
      v4 = 17;
LABEL_59:
      PacketGuard::~PacketGuard(v17);
      return v4;
    }
    if ( CDungeon::get_index(a2) == 100
      && (unsigned __int8)CParty::UseAncientDungeonItems((CParty *)a1, a2, (Inven_Item *)s, v15) != 1 )
    {
      PacketGuard::PacketGuard((PacketGuard *)v13);
      InterfacePacketBuf::put_header(v13, 1, 16);
      InterfacePacketBuf::put_byte(v13, 0);
      InterfacePacketBuf::put_int(v13, 1);
      InterfacePacketBuf::finalize(v13, 1);
      CParty::send_to_party(a1, (PacketGuard *)v13);
      v4 = 17;
      PacketGuard::~PacketGuard(v13);
      goto LABEL_59;
    }
    if ( (char)CDungeon::get_dimension_possible(a2) > 0 )
    {
      v16 = 0;
      v7 = CParty::CheckEnterDimensionDungeon(a1, a2, &v16, 1);
      v21 = v7;
      if ( v7 == 172 )
      {
        InterfacePacketBuf::clear(v17);
        InterfacePacketBuf::put_header(v17, 1, a4);
        InterfacePacketBuf::put_byte(v17, 0);
        InterfacePacketBuf::put_byte(v17, 172);
        InterfacePacketBuf::put_byte(v17, v16);
        InterfacePacketBuf::finalize(v17, 1);
        CParty::send_to_party(a1, (PacketGuard *)v17);
        v4 = 172;
        goto LABEL_59;
      }
      if ( v7 == 173 )
      {
        InterfacePacketBuf::clear(v17);
        InterfacePacketBuf::put_header(v17, 1, a4);
        InterfacePacketBuf::put_byte(v17, 0);
        InterfacePacketBuf::put_byte(v17, 173);
        InterfacePacketBuf::put_byte(v17, v16);
        InterfacePacketBuf::finalize(v17, 1);
        CParty::send_to_party(a1, (PacketGuard *)v17);
        v4 = 173;
        goto LABEL_59;
      }
      if ( v7 == 171 || (unsigned __int8)CParty::setStandardDimensionLevel(a1) != 1 )
      {
        InterfacePacketBuf::clear(v17);
        InterfacePacketBuf::put_header(v17, 1, a4);
        InterfacePacketBuf::put_byte(v17, 0);
        InterfacePacketBuf::put_byte(v17, 171);
        InterfacePacketBuf::finalize(v17, 1);
        CParty::send_to_party(a1, (PacketGuard *)v17);
        v4 = 171;
        goto LABEL_59;
      }
    }
    v18 = 0;
    v14 = 0;
    if ( a3 == 1 || (v18 = CParty::check_dungeon_start((CParty *)a1, a2, &v14)) == 0 )
    {
LABEL_42:
      v8 = CDungeon::get_index(a2) == 120 || CDungeon::get_index(a2) == 121;
      if ( v8 && (CParty::get_member_count(a1) <= 1 && a3 != 1 ? (v9 = 0) : (v9 = 1), v9) )
      {
        for ( m = 0; m <= 3; ++m )
        {
          if ( (unsigned __int8)CParty::_checkValidUser(a1, m) == 1 )
          {
            CurCharacNo = (const char *)CUserCharacInfo::getCurCharacNo(a1[6 * m + 30]);
            LogManager::logFormat(
              (LogManager *)1,
              (int)"party.cpp",
              "ENUM_ERROR CParty::checkInoutConditionDungeon(const CDungeon*, ENUM_DUNGEON_MODE, ENUM_CMDPACKET)",
              (const char *)0x25EB,
              "TOURNAMENT_DUNGEON_ERROR_LOG : CHARAC_NO (%d) MODE (%d)",
              CurCharacNo,
              a3);
          }
        }
        InterfacePacketBuf::clear(v17);
        InterfacePacketBuf::put_header(v17, 1, a4);
        InterfacePacketBuf::put_byte(v17, 0);
        InterfacePacketBuf::put_byte(v17, 243);
        InterfacePacketBuf::finalize(v17, 1);
        CParty::send_to_party(a1, (PacketGuard *)v17);
        v4 = 1;
      }
      else
      {
        v4 = 0;
      }
      goto LABEL_59;
    }
    if ( v18 > 244 )
    {
      if ( v18 > 247 )
      {
        if ( v18 != 248 )
          goto LABEL_42;
        goto LABEL_40;
      }
      if ( v18 < 246 )
        goto LABEL_40;
    }
    else if ( v18 < 243 )
    {
      if ( v18 != 10 && v18 != 95 )
        goto LABEL_42;
LABEL_40:
      InterfacePacketBuf::clear(v17);
      InterfacePacketBuf::put_header(v17, 1, a4);
      InterfacePacketBuf::put_byte(v17, 0);
      InterfacePacketBuf::put_byte(v17, v18);
      InterfacePacketBuf::put_byte(v17, v14);
      InterfacePacketBuf::finalize(v17, 1);
      CParty::send_to_party(a1, (PacketGuard *)v17);
      v4 = v18;
      goto LABEL_59;
    }
    InterfacePacketBuf::clear(v17);
    InterfacePacketBuf::put_header(v17, 1, a4);
    InterfacePacketBuf::put_byte(v17, 0);
    InterfacePacketBuf::put_byte(v17, v18);
    InterfacePacketBuf::finalize(v17, 1);
    CParty::send_to_party(a1, (PacketGuard *)v17);
    v4 = v18;
    goto LABEL_59;
  }
  return 1;
}

  if ( (char)CParty::GetEPLPState(this) <= 1 )
    return 10;

  if ( !CParty::CheckEnterAncientDungeon(
          (CUserCharacInfo **)this,

  v8 = CParty::CheckEnterDimensionDungeon((CDungeon **)this, *((const CDungeon **)this + 811));
  if ( v8 == 171 || v8 == 173 )

  v9 = CParty::check_dungeon_start(this, *((const CDungeon **)this + 811));
  if ( v9 == 10 || v9 >= 10 && (unsigned int)(v9 - 243) <= 4 )

  if ( CParty::CheckEnterVillageAttackRevenge((CUser **)this, *((const CDungeon **)this + 811)) )
    return 7;

         (unsigned __int8)CParty::CheckDestroyConditionSealDoor(
                            this,

    if ( v15 && (v24 = CParty::CheckMemberFatigue((CUser **)this), v24 > 0) )
      return 1;

void __cdecl CParty::sendInoutConditionDungeon(CUser **a1, int id)
{
  PacketBuf *v2[5]; // [esp+14h] [ebp-14h] BYREF

  if ( a2 != 10 )
  {
    PacketGuard::PacketGuard((PacketGuard *)v2);
    InterfacePacketBuf::clear(v2);
    InterfacePacketBuf::put_header(v2, 0, 260);
    InterfacePacketBuf::put_byte(v2, a2);
    InterfacePacketBuf::finalize(v2, 1);
    CParty::send_to_party(a1, (PacketGuard *)v2);
    PacketGuard::~PacketGuard(v2);
  }
}

int __cdecl CParty::sendPremiumGoldCardStatistic(CParty *this, char flag, int id, int id2)
{
  std::string *ServerProxy; // eax
  char buf[11]; // [esp+1Dh] [ebp-1Bh] BYREF
  int id; // [esp+28h] [ebp-10h]
  int id; // [esp+2Ch] [ebp-Ch]

  Packet_Goldcard_Event_Statistic_GTS::Packet_Goldcard_Event_Statistic_GTS((Packet_Goldcard_Event_Statistic_GTS *)buf);
  buf[10] = a2;
  v7 = a3;
  v8 = a4;
  ServerProxy = (std::string *)CServerProxyMgr<CStatisticServerProxy>::GetServerProxy(
                                 GlobalData::s_statistic_proxy_mgr,
                                 0);
  return CStatisticServerProxy::SendPacket(ServerProxy, buf, 0x13u);
}

void __usercall CParty::ReturnToVillage(int id@<esi>, CParty *this)
{
  int id; // edi
  char *MemberNames; // esi
  int id; // ebx
  char *v5; // eax
  CUser *user; // ebx
  char *v8; // eax
  CUser *user; // ebx
  GameWorld *v10; // eax
  char state; // si
  unsigned int acc_id; // eax
  CUser *Manager; // eax
  CUser *user; // ebx
  GameWorld *v15; // eax
  __int16 member_count; // ax
  int dungeon_diff; // ebx
  char *DungeonName; // eax
  CUser *user; // ebx
  char *HackAnalyzer; // eax
  CUser *user; // ebx
  GameWorld *v23; // eax
  GameWorld *v24; // eax
  char s[256]; // [esp+20h] [ebp-138h] BYREF
  char flag[12]; // [esp+120h] [ebp-38h] BYREF
  int id[4]; // [esp+12Ch] [ebp-2Ch] BYREF
  int i; // [esp+13Ch] [ebp-1Ch]

  CBattle_Field::setBloodState((int)this + 2852, 0);
  PacketGuard::PacketGuard((PacketGuard *)v26);
  InterfacePacketBuf::put_header((PacketBuf **)v26, 0, 2);
  if ( !*((_BYTE *)this + 316) )
  {
    InterfacePacketBuf::put_byte((PacketBuf **)v26, 0);
    member_count = CParty::get_member_count((CUser **)this);
    InterfacePacketBuf::put_short((PacketBuf **)v26, member_count);
    memset(s, 0, sizeof(s));
    CParty::_getMemberNames((CUserCharacInfo **)this, s);
    for ( i = 0; i <= 3; ++i )
    {
      if ( (unsigned __int8)CParty::_checkValidUser((CUser **)this, i) )
      {
        if ( *((_DWORD *)this + 811) )
        {
          a1 = *((_DWORD *)this + 822);
          dungeon_diff = CBattle_Field::get_dungeon_diff((CParty *)((char *)this + 2852));
          DungeonName = CDungeon::GetDungeonName(*((std::string **)this + 811));
          cUserHistoryLog::LeaveDungeon(
            (CUser **)(*((_DWORD *)this + 6 * i + 30) + CUSER_OFFSET_HISTORY_LOG),
            DungeonName,
            dungeon_diff,
            s,
            a1);
        }
        if ( *((_DWORD *)this + 811) )
        {
          if ( (unsigned __int8)CParty::IsSinglePlay(this) && CUser::isDungeonClearHackCheck(*((CUser **)this + 29)) )
          {
            v20 = (CUser *)*((_DWORD *)this + 29);
            HackAnalyzer = CUser::getHackAnalyzer(v20);
            WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)HackAnalyzer, v20, 0x19Cu, 1, 0, 0);
          }
        }
        v22 = (CUser *)*((_DWORD *)this + 6 * i + 30);
        v23 = G_GameWorld();
        GameWorld::out_from_dungeon(v23, v22);
        CUser::make_basic_info((unsigned __int8)a1, *((CUser **)this + 6 * i + 30), v26, 0);
      }
    }
    CParty::set_state(this, 1);
    CParty::battle_data_reset(a1, (CUser **)this);
    InterfacePacketBuf::finalize((PacketBuf **)v26, 1);
    if ( !(unsigned __int8)CParty::IsExistInvisible((CUser **)this) )
    {
      v24 = G_GameWorld();
      GameWorld::send_all(v24, (PacketGuard *)v26);
      goto LABEL_31;
    }
    goto LABEL_29;
  }
  if ( *((_DWORD *)this + 811) )
  {
    memset(s, 0, sizeof(s));
    v2 = *((_DWORD *)this + 822);
    MemberNames = CParty::_getMemberNames((CUserCharacInfo **)this, s);
    v4 = CBattle_Field::get_dungeon_diff((CParty *)((char *)this + 2852));
    v5 = CDungeon::GetDungeonName(*((std::string **)this + 811));
    cUserHistoryLog::LeaveDungeon((CUser **)(*((_DWORD *)this + 29) + CUSER_OFFSET_HISTORY_LOG), v5, v4, MemberNames, v2);
    if ( (unsigned __int8)CParty::IsSinglePlay(this) && CUser::isDungeonClearHackCheck(*((CUser **)this + 29)) )
    {
      v7 = (CUser *)*((_DWORD *)this + 29);
      v8 = CUser::getHackAnalyzer(v7);
      WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)v8, v7, 0x19Cu, 1, 0, 0);
    }
  }
  v9 = (CUser *)*((_DWORD *)this + 29);
  v10 = G_GameWorld();
  GameWorld::out_from_dungeon(v10, v9);
  InterfacePacketBuf::put_byte((PacketBuf **)v26, 0);
  InterfacePacketBuf::put_short((PacketBuf **)v26, 1);
  CUser::GetPartyIndex(*((CUser **)this + 29));
  state = CUser::get_state(*((CUser **)this + 29));
  acc_id = CUser::get_acc_id(*((CUser **)this + 29));
  LOBYTE(v9) = NumberToString(acc_id, 0);
  cMyTrace::cMyTrace((cMyTrace *)v27, "void CParty::ReturnToVillage()", 10004, 0);
  cMyTrace::operator()((int)v27, "<IN_DUNGEON_NULL_PARTY> m_id(%s), user_state(%d), PartyIndex(%d)", (char)v9);
  CUser::SetPartyIndex(*((pthread_mutex_t **)this + 29), -1);
  if ( CUser::getMoveSpace(*((CUser **)this + 29)) != 1 )
    CUser::set_state(*((CUserCharacInfo **)this + 29), 3);
  CUser::make_basic_info(state, *((CUser **)this + 29), v26, 0);
  CParty::destroy(state, (CUser **)this);
  InterfacePacketBuf::finalize((PacketBuf **)v26, 1);
  Manager = (CUser *)CParty::getManager(this);
  if ( CUser::getMoveSpace(Manager) == 1 )
  {
LABEL_29:
    CParty::send_to_party((CUser **)this, (PacketGuard *)v26);
    goto LABEL_31;
  }
  v14 = (CUser *)*((_DWORD *)this + 29);
  v15 = G_GameWorld();
  GameWorld::send_all(v15, (PacketGuard *)v26, v14);
LABEL_31:
  CParty::SetEPLPState(this, 0);
  PacketGuard::~PacketGuard((PacketPool **)v26);
}

long double __cdecl CParty::CalcAvatarBonus(CUser **this)
{
  float v2; // [esp+24h] [ebp-14h]
  char flag; // [esp+2Bh] [ebp-Dh]
  int i; // [esp+2Ch] [ebp-Ch]

  v2 = 0.0;
  v3 = 0;
  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::_checkValidUser(this, i) == 1
      && (unsigned __int8)CUser::IsEquipAvatar(*(this + 6 * i + 30)) )
    {
      v3 = 1;
      break;
    }
  }
  if ( v3 )
  {
    if ( CParty::get_member_count(this) > 1 )
      return 0.050000001;
    else
      return 0.02;
  }
  return v2;
}

long double __cdecl CParty::CalcCreatureBonus(CParty *this, CUser *user)
{
  CInventory *CurCharacInvenR; // eax
  char *CreatureMgrR; // eax

  CurCharacInvenR = (CInventory *)CUserCharacInfo::getCurCharacInvenR(a2);
  CreatureMgrR = CInventory::GetCreatureMgrR(CurCharacInvenR);
  if ( user_creature::CCreatureMgr::IsEquippedCreature((user_creature::CCreatureMgr *)CreatureMgrR) )
    return 0.050000001;
  else
    return 0.0;
}

int __cdecl CParty::_makeRewardItemInfo(CParty *this, const CUser *user, Inven_Item *item, PacketBuf **a4)
{
  int CurCharacInvenR; // eax
  int add_info; // eax
  int result; // eax
  int count; // eax

  if ( *(_DWORD *)((char *)a3 + 63) == -1 )
  {
    *(_DWORD *)((char *)a3 + 63) = 0;
    Inven_Item::set_add_info((Inven_Item *)((char *)a3 + 61), 0);
  }
  if ( *(_DWORD *)((char *)a3 + 63) )
  {
    CurCharacInvenR = CUserCharacInfo::getCurCharacInvenR(a2);
    if ( CInventory::tryInsertItemIntoInventory(CurCharacInvenR, *(long double *)((char *)a3 + 61)) < 0 )
    {
      InterfacePacketBuf::put_byte(a4, 1);
      *(_DWORD *)((char *)a3 + 63) = 0;
      Inven_Item::set_add_info((Inven_Item *)((char *)a3 + 61), 0);
    }
    else
    {
      InterfacePacketBuf::put_byte(a4, 2);
    }
  }
  else
  {
    InterfacePacketBuf::put_byte(a4, 1);
  }
  InterfacePacketBuf::put_int(a4, 0);
  add_info = Inven_Item::get_add_info(a3);
  InterfacePacketBuf::put_int(a4, add_info);
  result = *(_DWORD *)((char *)a3 + 63);
  if ( result )
  {
    InterfacePacketBuf::put_int(a4, *(_DWORD *)((char *)a3 + 63));
    v7 = Inven_Item::get_add_info((Inven_Item *)((char *)a3 + 61));
    return InterfacePacketBuf::put_int(a4, v7);
  }
  return result;
}

char *__cdecl CParty::getClearRewardBonusExp(
        CParty *this,

      "unsigned int CParty::getClearRewardBonusExp(CUser*, unsigned int&, const stClearRewardExpParameter_t&, stClearRewardBonusExp_t&)",
      (const char *)0x28B1,

      "unsigned int CParty::getClearRewardBonusExp(CUser*, unsigned int&, const stClearRewardExpParameter_t&, stClearRewardBonusExp_t&)",
      (const char *)0x28C2,

long double __cdecl CParty::calDeadUserDungeonPenalty(CParty *this, int id)
{
  float v4; // [esp+10h] [ebp-8h]
  int id; // [esp+14h] [ebp-4h]

  v4 = 0.0;
  v5 = *((_DWORD *)this + 622);
  if ( a2 >= 0 )
  {
    if ( v5 > 1 )
    {
      v4 = (long double)*((int *)this + a2 + 228) / (long double)(v5 - 1);
      if ( v4 < 0.30000001 )
        return 0.0;
    }
    return v4;
  }
  else
  {
    return 0.0;
  }
}

long double __cdecl CParty::CalcurateManageBonus(int id, int value, CUserCharacInfo *charac_info, int count)
{
  int id; // ebx
  int id; // ebx
  CDataManager *v7; // eax
  int ManageLevel; // ebx
  CDataManager *v9; // eax
  float v10; // [esp+14h] [ebp-14h]

  if ( !a3 )
    return 0.0;
  if ( !CUserCharacInfo::getCurCharacR(a3) )
    return 0.0;
  v5 = *(__int16 *)(CUserCharacInfo::getCurCharacR(a3) + 39);
  if ( v5 >= CUser::GetUserMaxLevel(a3) )
    return 0.0;
  if ( a2 )
  {
    if ( a2 == 1 )
    {
      v10 = (float)(unsigned int)a4;
      ManageLevel = CUser::GetManageLevel(a3);
      v9 = G_CDataManager();
      return CharacManageScript::GetGoldBonus((CharacManageScript *)*((_DWORD *)v9 + 6373), ManageLevel) * v10;
    }
    else
    {
      return 0.0;
    }
  }
  else
  {
    v6 = CUser::GetManageLevel(a3);
    v7 = G_CDataManager();
    return CharacManageScript::GetExpBonus((CharacManageScript *)*((_DWORD *)v7 + 6373), v6);
  }
}

void __cdecl CParty::set_clear_reward_exp_parameter(
        CUser **this,

      v29 = CParty::CalcAvatarBonus(this);
      v30 = 0.0;

        if ( (unsigned __int8)CParty::existSamePCRoomUser(this, PCRoomNo) )
          v33 = *((_DWORD *)G_CDataManager() + 501);

      if ( (unsigned __int8)CParty::IsReturnUserParty(this) )
        v34 = 1045220557;

      *(float *)a4 = CParty::calDeadUserDungeonPenalty((CParty *)this, *((_DWORD *)a4 + 22));
      *((float *)a4 + 1) = ServerParameterScript::GetExpBonusWeight(v28, *((_DWORD *)a4 + 23));

      *((float *)a4 + 3) = CParty::CalcCreatureBonus((CParty *)this, a2);
      *((float *)a4 + 4) = v30;

      if ( (unsigned __int8)CParty::IsEventCharacParty(this) )
      {
        v37 = (EventNewCharacterReward *)CEventManager::GetRepeatEvent(GlobalData::s_event_manager, 162);
        if ( v37 )
          *((float *)a4 + 18) = EventNewCharacterReward::eventExpRatio(v37);
      }

      *((float *)a4 + 15) = CParty::CalcurateManageBonus((int)this, 0, a2, a3);
      STExpReward::STExpReward((STExpReward *)&v25);

      "void CParty::set_clear_reward_exp_parameter(CUser*, uint, stClearRewardExpParameter_t&)",
      (const char *)0x294A,

void __usercall CParty::clear_reward(int rarity@<esi>, CUser **this, int id)
{
  int dungeon_index; // eax
  const char *v4; // eax
  const char *v5; // eax
  MazeScript *Maze; // eax
  CDataManager *v7; // eax
  int ItemGenerator; // eax
  CDataManager *v9; // eax
  int id; // eax
  unsigned __int8 charac_level; // al
  CDataManager *v12; // eax
  int id; // eax
  int id; // eax
  void *v15; // eax
  int charac_guildkey; // ebx
  CDataManager *v18; // eax
  CDataManager *v19; // eax
  int id; // ebx
  CDataManager *v21; // eax
  CDataManager *v23; // eax
  void *v24; // eax
  const Inven_Item *item; // eax
  int id; // eax
  int id; // eax
  CDataManager *v28; // eax
  int id; // eax
  int id; // eax
  const Inven_Item *item; // eax
  int id; // eax
  CDataManager *v33; // eax
  int id; // eax
  int RepeatEvent; // eax
  int id; // eax
  CGameManager *v39; // eax
  char CurConditionEventStep; // al
  int id; // ebx
  int GoldCardCommission; // eax
  int id; // eax
  int PremiumGoldCardDefaultItem; // eax
  const Inven_Item *item; // eax
  int id; // eax
  CDataManager *v49; // eax
  CDataManager *v50; // eax
  int id; // eax
  int id; // eax
  const Inven_Item *item; // eax
  CDataManager *v54; // eax
  int id; // eax
  int id; // eax
  CDataManager *v58; // eax
  char isHelpAbuseParty; // bl
  CPartyExpBonusEvent *v60; // eax
  long double BonusRate; // fst7
  int id; // eax
  int id; // ebx
  CDataManager *v64; // eax
  int id; // eax
  long double v66; // fst7
  int id; // ebx
  CDataManager *v68; // eax
  unsigned __int8 v69; // al
  char flag; // bl
  CPartyExpBonusEvent *v71; // eax
  int id; // ebx
  QuestList **v73; // eax
  int id; // ebx
  int id; // eax
  int id; // eax
  _DWORD *v77; // eax
  int id; // edx
  int id; // ebx
  CDataManager *v80; // eax
  int *CurCharacInvenW; // eax
  int id; // eax
  char AbilityType; // al
  __int16 AbilityValue; // ax
  unsigned int id; // ebx
  CItemList **v86; // eax
  int grade; // ebx
  int id; // eax
  CUser **Party; // eax
  CUserCharacInfo **v91; // eax
  CUser *user; // ebx
  void *v95; // eax
  CGameManager *v96; // eax
  bool v97; // al
  int id; // [esp+Ch] [ebp-3BCh]
  double guild_temp_exp; // [esp+68h] [ebp-360h]
  double v100; // [esp+70h] [ebp-358h]
  double v101; // [esp+78h] [ebp-350h]
  double v102; // [esp+80h] [ebp-348h]
  float v103; // [esp+8Ch] [ebp-33Ch]
  float v104; // [esp+90h] [ebp-338h]
  float v105; // [esp+94h] [ebp-334h]
  float v106; // [esp+98h] [ebp-330h]
  float v107; // [esp+9Ch] [ebp-32Ch]
  float v108; // [esp+A0h] [ebp-328h]
  float v109; // [esp+A4h] [ebp-324h]
  float v110; // [esp+A8h] [ebp-320h]
  float v111; // [esp+ACh] [ebp-31Ch]
  float v112; // [esp+B0h] [ebp-318h]
  float v113; // [esp+B4h] [ebp-314h]
  int TotalKilledMonsterCount; // [esp+C8h] [ebp-300h]
  int id; // [esp+C8h] [ebp-300h]
  float v116; // [esp+CCh] [ebp-2FCh]
  int id[3]; // [esp+D8h] [ebp-2F0h] BYREF
  int id; // [esp+E4h] [ebp-2E4h] BYREF
  float v119; // [esp+FCh] [ebp-2CCh]
  int id; // [esp+130h] [ebp-298h]
  int id; // [esp+134h] [ebp-294h]
  __int64 __a; // [esp+138h] [ebp-290h] BYREF
  int id; // [esp+140h] [ebp-288h]
  int id; // [esp+144h] [ebp-284h]
  int id; // [esp+148h] [ebp-280h]
  int id; // [esp+14Ch] [ebp-27Ch]
  int id; // [esp+150h] [ebp-278h]
  int id; // [esp+154h] [ebp-274h]
  int id; // [esp+158h] [ebp-270h]
  int id; // [esp+15Ch] [ebp-26Ch]
  int id; // [esp+160h] [ebp-268h]
  int id; // [esp+164h] [ebp-264h]
  int id; // [esp+168h] [ebp-260h]
  int id; // [esp+16Ch] [ebp-25Ch]
  int id; // [esp+170h] [ebp-258h]
  int id; // [esp+174h] [ebp-254h]
  int id; // [esp+178h] [ebp-250h]
  int id; // [esp+17Ch] [ebp-24Ch]
  int id; // [esp+180h] [ebp-248h] BYREF
  float v140; // [esp+184h] [ebp-244h]
  char flag; // [esp+1B4h] [ebp-214h]
  char dungeon_diff; // [esp+1B5h] [ebp-213h]
  char flag; // [esp+1B6h] [ebp-212h]
  float Rate; // [esp+1B8h] [ebp-210h]
  int id; // [esp+1BCh] [ebp-20Ch]
  CDungeon *dungeon; // [esp+1C0h] [ebp-208h]
  __int64 v147; // [esp+1CBh] [ebp-1FDh] BYREF
  __int64 v148; // [esp+1D3h] [ebp-1F5h]
  int id[11]; // [esp+1DBh] [ebp-1EDh] BYREF
  char flag; // [esp+207h] [ebp-1C1h]
  void *v151[12]; // [esp+208h] [ebp-1C0h] BYREF
  PacketBuf *v152[3]; // [esp+238h] [ebp-190h] BYREF
  _DWORD value[2]; // [esp+244h] [ebp-184h] BYREF
  void *v154[3]; // [esp+24Ch] [ebp-17Ch] BYREF
  int index; // [esp+258h] [ebp-170h] BYREF
  int id; // [esp+25Ch] [ebp-16Ch] BYREF
  int id; // [esp+260h] [ebp-168h] BYREF
  int id[7]; // [esp+264h] [ebp-164h] BYREF
  int id[2]; // [esp+280h] [ebp-148h] BYREF
  unsigned int id; // [esp+288h] [ebp-140h] BYREF
  _DWORD value[2]; // [esp+28Ch] [ebp-13Ch] BYREF
  char flag[12]; // [esp+294h] [ebp-134h] BYREF
  PacketBuf *v163[3]; // [esp+2A0h] [ebp-128h] BYREF
  unsigned int id; // [esp+2ACh] [ebp-11Ch] BYREF
  int id; // [esp+2B0h] [ebp-118h]
  int id; // [esp+2B4h] [ebp-114h] BYREF
  int id; // [esp+2B8h] [ebp-110h] BYREF
  int id; // [esp+2BCh] [ebp-10Ch] BYREF
  int StdDropRate; // [esp+2C0h] [ebp-108h] BYREF
  int id; // [esp+2C4h] [ebp-104h] BYREF
  int id; // [esp+2C8h] [ebp-100h] BYREF
  int id; // [esp+2CCh] [ebp-FCh] BYREF
  int id; // [esp+2D0h] [ebp-F8h] BYREF
  void *v174[3]; // [esp+2D4h] [ebp-F4h] BYREF
  CDataManager *v175; // [esp+2E0h] [ebp-E8h]
  CDungeon *dungeon; // [esp+2E4h] [ebp-E4h]
  char *member_count; // [esp+2E8h] [ebp-E0h]
  int RankBonusIndex; // [esp+2ECh] [ebp-DCh]
  char *v179; // [esp+2F0h] [ebp-D8h]
  int standard_level; // [esp+2F4h] [ebp-D4h]
  int id; // [esp+2F8h] [ebp-D0h]
  int id; // [esp+2FCh] [ebp-CCh]
  float v183; // [esp+300h] [ebp-C8h]
  WongWork::CClearReward *Generator; // [esp+304h] [ebp-C4h]
  int RewardItemRatePerMapMaxCount; // [esp+308h] [ebp-C0h]
  int id; // [esp+30Ch] [ebp-BCh]
  float v187; // [esp+310h] [ebp-B8h]
  unsigned int id; // [esp+314h] [ebp-B4h]
  int i; // [esp+318h] [ebp-B0h]
  int j; // [esp+31Ch] [ebp-ACh]
  _DWORD *GridR; // [esp+320h] [ebp-A8h]
  WongWork::CClearReward *v192; // [esp+324h] [ebp-A4h]
  int k; // [esp+328h] [ebp-A0h]
  int m; // [esp+32Ch] [ebp-9Ch]
  int n; // [esp+330h] [ebp-98h]
  int ii; // [esp+334h] [ebp-94h]
  int jj; // [esp+338h] [ebp-90h]
  int id; // [esp+33Ch] [ebp-8Ch]
  int id; // [esp+340h] [ebp-88h]
  int id; // [esp+344h] [ebp-84h]
  int id; // [esp+348h] [ebp-80h]
  int id; // [esp+34Ch] [ebp-7Ch]
  int add_info; // [esp+350h] [ebp-78h]
  WongWork::CClearReward *v204; // [esp+354h] [ebp-74h]
  float v205; // [esp+358h] [ebp-70h]
  CConditionEventManager *ConditionEventManager; // [esp+35Ch] [ebp-6Ch]
  WongWork::CClearReward *v207; // [esp+360h] [ebp-68h]
  int inserted; // [esp+364h] [ebp-64h]
  int id; // [esp+368h] [ebp-60h]
  int id; // [esp+36Ch] [ebp-5Ch]
  int kk; // [esp+370h] [ebp-58h]
  int id; // [esp+374h] [ebp-54h]
  unsigned int ClearRewardBonusExp; // [esp+378h] [ebp-50h]
  int id; // [esp+37Ch] [ebp-4Ch]
  char flag; // [esp+383h] [ebp-45h]
  int id; // [esp+384h] [ebp-44h]
  int id; // [esp+388h] [ebp-40h]
  int mm; // [esp+38Ch] [ebp-3Ch]
  char *CurCharacQuestRefR; // [esp+390h] [ebp-38h]
  Quest *quest; // [esp+394h] [ebp-34h]
  signed int nn; // [esp+398h] [ebp-30h]
  CItem *item; // [esp+39Ch] [ebp-2Ch]
  int i1; // [esp+3A0h] [ebp-28h]
  int i2; // [esp+3A4h] [ebp-24h]
  int i3; // [esp+3A8h] [ebp-20h]
  CConditionEventManager *v226; // [esp+3ACh] [ebp-1Ch]

  v175 = G_CDataManager();
  dungeon_index = CBattle_Field::get_dungeon_index((CBattle_Field *)(this + 713));
  dungeon = (CDungeon *)CDataManager::find_dungeon((CDungeonList **)v175, dungeon_index);
  member_count = (char *)CParty::get_member_count(this);
  if ( dungeon )
  {
    if ( (int)member_count > 0 )
    {
      if ( CDungeon::isTournamentDungeon(dungeon) )
      {
        v5 = (const char *)CBattle_Field::get_dungeon_index((CBattle_Field *)(this + 713));
        LogManager::logFormat(
          (LogManager *)1,
          (int)"party.cpp",
          "void CParty::clear_reward(int)",
          (const char *)0x2A13,
          "TournamentDungeon(%d) can not call function",
          v5);
      }
      else
      {
        RankBonusIndex = CDataManager::getRankBonusIndex(v175, a3);
        v179 = (char *)v175 + 104;
        standard_level = 0;
        if ( (char)CDungeon::get_dimension_possible(dungeon) <= 0 )
          standard_level = CDungeon::get_standard_level(dungeon);
        else
          standard_level = CParty::getStandardDimensionLevel((CParty *)this);
        v181 = standard_level;
        *(this + 622) = (CUser *)((char *)*(this + 622) + 1);
        v182 = 0;
        for ( i = 0; *(_DWORD *)CBattle_Field::getMaze((CBattle_Field *)(this + 713)) > i; ++i )
        {
          for ( j = 0; *(_DWORD *)(CBattle_Field::getMaze((CBattle_Field *)(this + 713)) + 4) > j; ++j )
          {
            Maze = (MazeScript *)CBattle_Field::getMaze((CBattle_Field *)(this + 713));
            GridR = (_DWORD *)MazeScript::getGridR(Maze, i, j);
            if ( *GridR )
              ++v182;
          }
        }
        v183 = (long double)(int)*(this + 622) / (long double)v182;
        v7 = G_CDataManager();
        ItemGenerator = CDataManager::getItemGenerator(v7);
        Generator = (WongWork::CClearReward *)WongWork::CItemGeneratorMgr::getGenerator(ItemGenerator, 2);
        RewardItemRatePerMapMaxCount = WongWork::CClearReward::getRewardItemRatePerMapMaxCount(Generator, v182);
        if ( RewardItemRatePerMapMaxCount )
          v183 = (long double)(unsigned int)RewardItemRatePerMapMaxCount * v183 / MAX_LIMIT_10000.0;
        WongWork::stGenerateRefData_t::stGenerateRefData_t((WongWork::stGenerateRefData_t *)&v139);
        dungeon_diff = CBattle_Field::get_dungeon_diff((CBattle_Field *)(this + 713));
        v141 = v181;
        v143 = (char)member_count;
        Rate = *(float *)&RankBonusIndex;
        LOWORD(v145) = (unsigned __int16)*(this + 221);
        HIWORD(v145) = (unsigned __int16)*(this + 219);
        LOWORD(v146) = 0;
        HIWORD(v146) = (unsigned __int16)*(this + 220);
        WongWork::stGenerateResult_t::stGenerateResult_t((WongWork::stGenerateResult_t *)v151);
        if ( *((_BYTE *)*(this + 811) + 2206) )
        {
          v9 = G_CDataManager();
          v10 = CDataManager::getItemGenerator(v9);
          v192 = (WongWork::CClearReward *)WongWork::CItemGeneratorMgr::getGenerator(v10, 2);
          for ( k = 0; k <= 3; ++k )
          {
            if ( (unsigned __int8)CParty::_checkValidUser(this, k) == 1 )
            {
              charac_level = CUserCharacInfo::get_charac_level(*(this + 6 * k + 30));
              WongWork::CClearReward::power_war_dungeon_gold_clear_reward(
                v192,
                (const WongWork::stGenerateRefData_t *)&v139,
                (WongWork::stGenerateResult_t *)v151,
                charac_level);
            }
          }
        }
        else
        {
          v12 = G_CDataManager();
          v13 = CDataManager::getItemGenerator(v12);
          v14 = WongWork::CItemGeneratorMgr::getGenerator(v13, 2);
          (*(void (__cdecl **)(int, int *, void **))(*(_DWORD *)v14 + 12))(v14, &v139, v151);
        }
        CParty::increase_result_card_gold((CParty *)this, (WongWork::stGenerateResult_t *)v151);
        std::vector<Inven_Item>::begin(&v168, v151);
        __gnu_cxx::__normal_iterator<Inven_Item const*,std::vector<Inven_Item>>::__normal_iterator<Inven_Item*>(
          &v167,
          (int)&v168);
        dungeon_diff = CBattle_Field::get_dungeon_diff((CBattle_Field *)(this + 713));
        v141 = v181;
        Rate = v183;
        v146 = dungeon;
        v166 = 100;
        for ( m = 0; m <= 3; ++m )
        {
          if ( (unsigned __int8)CParty::_checkValidUser(this, m) == 1 )
          {
            v15 = ARAD::Singleton<ServiceRestrictManager>::Get();
            if ( (unsigned __int8)ServiceRestrictManager::isRestricted((int)v15, 3, 1) )
            {
              StdDropRate = CUser::getStdDropRate(*(this + 6 * m + 30));
              v166 = *std::min<int>(&StdDropRate, &v166);
            }
          }
        }
        v165 = 0;
        for ( n = 0; n <= 3; ++n )
        {
          if ( (unsigned __int8)CParty::_checkValidUser(this, n) == 1
            && (unsigned __int8)CUser::IsTodayGuildMember(*(this + 6 * n + 30)) )
          {
            *((_BYTE *)&v165 + n) = 1;
            for ( ii = 0; ii <= 3; ++ii )
            {
              if ( (unsigned __int8)CParty::_checkValidUser(this, ii) == 1 )
              {
                charac_guildkey = CUserCharacInfo::get_charac_guildkey(*(this + 6 * n + 30));
                if ( charac_guildkey == CUserCharacInfo::get_charac_guildkey(*(this + 6 * ii + 30)) )
                  *((_BYTE *)&v165 + ii) = 1;
              }
            }
          }
        }
        v186 = 100;
        for ( jj = 0; jj <= 3; ++jj )
        {
          if ( (unsigned __int8)CParty::_checkValidUser(this, jj) == 1 )
          {
            if ( *((_BYTE *)*(this + 811) + 2140)
              && (unsigned __int8)CUser::IsDeleteEventDungeonInvitation(*(this + 6 * jj + 30)) )
            {
              CUser::EventDungeonClearReward(*(this + 6 * jj + 30), (_DWORD *)*(this + 811) + 539);
            }
            v18 = G_CDataManager();
            if ( (unsigned int)CDataManager::getNewRankIndex(v18, a3) <= 2 )
            {
              v98 = 0;
              APSystem::CUserProc::ClearActionAndSendtoUser();
            }
            if ( *((_BYTE *)*(this + 811) + 2206) )
            {
              v141 = CUserCharacInfo::get_charac_level(*(this + 6 * jj + 30));
              v199 = *((unsigned __int16 *)CUser::GetGuildDBInfo(*(this + 6 * jj + 30)) + 17);
              guild_temp_exp = (double)(unsigned int)CUserCharacInfo::get_guild_temp_exp(*(this + 6 * jj + 30));
              v19 = G_CDataManager();
              v200 = (int)(GuildParameterScript::getExpWeight((CDataManager *)((char *)v19 + 42252), v199)
                         * guild_temp_exp);
              if ( *((_BYTE *)&v165 + jj) )
              {
                v100 = (double)v200;
                v200 += (int)(*((double *)G_CDataManager() + 5313) / 100.0 * v100);
                CUserCharacInfo::add_guild_today_exp(*(this + 6 * jj + 30), v200);
              }
              else
              {
                CUserCharacInfo::add_guild_exp(*(this + 6 * jj + 30), v200);
              }
            }
            else
            {
              v20 = CUserCharacInfo::get_charac_level(*(this + 6 * jj + 30));
              v21 = G_CDataManager();
              if ( CDataManager::BaseExpPenalty(v21, v20, v181) >= 1.0
                || (unsigned __int8)CDungeon::isRiskDungeon(*(this + 811)) )
              {
                v201 = *((unsigned __int16 *)CUser::GetGuildDBInfo(*(this + 6 * jj + 30)) + 17);
                v101 = (double)(unsigned int)CUserCharacInfo::get_guild_temp_exp(*(this + 6 * jj + 30));
                v23 = G_CDataManager();
                v202 = (int)(GuildParameterScript::getExpWeight((CDataManager *)((char *)v23 + 42252), v201) * v101);
                if ( *((_BYTE *)&v165 + jj) )
                {
                  v102 = (double)v202;
                  v202 += (int)(*((double *)G_CDataManager() + 5313) / 100.0 * v102);
                  CUserCharacInfo::add_guild_today_exp(*(this + 6 * jj + 30), v202);
                }
                else
                {
                  CUserCharacInfo::add_guild_exp(*(this + 6 * jj + 30), v202);
                }
              }
            }
            CUserCharacInfo::reset_guild_temp_exp(*(this + 6 * jj + 30));
            v24 = ARAD::Singleton<ServiceRestrictManager>::Get();
            if ( (unsigned __int8)ServiceRestrictManager::isRestricted((int)v24, 3, 1) )
              v139 = CUser::getStdDropRate(*(this + 6 * jj + 30));
            std::vector<Inven_Item>::end(&v170, (int)v151);
            if ( __gnu_cxx::operator!=<Inven_Item const*,Inven_Item*,std::vector<Inven_Item>>((int)&v167, (int)&v170) )
            {
              v25 = (const Inven_Item *)__gnu_cxx::__normal_iterator<Inven_Item const*,std::vector<Inven_Item>>::operator*((int)&v167);
              Inven_Item::setCopy((Inven_Item *)((char *)this + 122 * jj + 928), v25);
              __gnu_cxx::__normal_iterator<Inven_Item const*,std::vector<Inven_Item>>::operator++(&v167);
            }
            if ( *((_BYTE *)this + jj + 896) != 1
              && *((_BYTE *)this + jj + 900) != 1
              && *((_BYTE *)this + jj + 904) != 1 )
            {
              *(this + jj + 228) = (CUser *)((char *)*(this + jj + 228) + 1);
            }
            v140 = CParty::calDeadUserDungeonPenalty((CParty *)this, jj);
            if ( v140 > 0.0 )
            {
              add_info = Inven_Item::get_add_info((Inven_Item *)((char *)this + 122 * jj + 928));
              Inven_Item::set_add_info(
                (Inven_Item *)((char *)this + 122 * jj + 928),
                (int)((long double)add_info * (1.0 - v140)));
              *((_BYTE *)this + jj + 908) = 1;
            }
            v26 = Inven_Item::get_add_info((Inven_Item *)((char *)this + 122 * jj + 928));
            v116 = (long double)v166 / 100.0;
            v198 = (int)((long double)v26 * v116);
            Inven_Item::set_add_info((Inven_Item *)((char *)this + 122 * jj + 928), v198);
            if ( CUser::isAffectedPremium((int)*(this + 6 * jj + 30), 12) )
              v27 = 3;
            else
              v27 = 0;
            v145 = v27;
            WongWork::stGenerateResult_t::stGenerateResult_t((WongWork::stGenerateResult_t *)v117);
            CParty::GetLuckPoint(this, (CLuckPoint *)&v118, 0);
            v28 = G_CDataManager();
            v29 = CDataManager::getItemGenerator(v28);
            v30 = WongWork::CItemGeneratorMgr::getGenerator(v29, 2);
            (*(void (__cdecl **)(int, int *, int *, int))(*(_DWORD *)v30 + 8))(v30, &v139, v117, v98);
            if ( !std::vector<Inven_Item>::empty(v117) )
            {
              std::vector<Inven_Item>::begin(&v171, v117);
              v31 = (const Inven_Item *)__gnu_cxx::__normal_iterator<Inven_Item *,std::vector<Inven_Item>>::operator*((int)&v171);
              Inven_Item::setCopy((Inven_Item *)((char *)this + 122 * jj + 989), v31);
            }
            if ( CUser::isAffectedPremium((int)*(this + 6 * jj + 30), 12) )
              v32 = 4;
            else
              v32 = 1;
            v145 = v32;
            Inven_Item::setCopy(
              (Inven_Item *)((char *)this + 122 * jj + 1904),
              (const Inven_Item *)((char *)this + 122 * jj + 928));
            Inven_Item::set_add_info((Inven_Item *)((char *)this + 122 * jj + 1904), 0);
            v33 = G_CDataManager();
            v34 = CDataManager::getItemGenerator(v33);
            v204 = (WongWork::CClearReward *)WongWork::CItemGeneratorMgr::getGenerator(v34, 2);
            if ( v204 )
            {
              RepeatEvent = CEventManager::GetRepeatEvent(GlobalData::s_event_manager, 16);
              if ( (*(unsigned __int8 (__cdecl **)(int, _DWORD))(*(_DWORD *)RepeatEvent + 52))(RepeatEvent, 0) != 1 )
              {
                v205 = 1.0;
                v37 = CEventManager::GetRepeatEvent(GlobalData::s_event_manager, 65);
                if ( (*(unsigned __int8 (__cdecl **)(int, _DWORD))(*(_DWORD *)v37 + 52))(v37, 0)
                  && (unsigned __int8)CParty::IsPremiumGoldCardParty((CParty *)this) )
                {
                  v205 = *((float *)G_CDataManager() + 4671);
                }
                v39 = G_CGameManager();
                ConditionEventManager = (CConditionEventManager *)CGameManager::GetConditionEventManager(v39);
                if ( ConditionEventManager )
                {
                  CurConditionEventStep = CUserCharacInfo::GetCurConditionEventStep(*(this + 6 * jj + 30));
                  if ( (unsigned __int8)CConditionEventManager::IsFreeGoldCard(
                                          ConditionEventManager,
                                          *(this + 6 * jj + 30),
                                          CurConditionEventStep + 1) )
                    v205 = 0.0;
                }
                v41 = jj;
                GoldCardCommission = WongWork::CClearReward::getGoldCardCommission(v204, v181);
                *(this + v41 + 125) = (CUser *)(int)((long double)GoldCardCommission * v205);
                Rate = WongWork::CClearReward::getGoldCardCreateRate(v204);
              }
            }
            std::vector<Inven_Item>::clear(v117);
            v43 = CEventManager::GetRepeatEvent(GlobalData::s_event_manager, 65);
            if ( (*(unsigned __int8 (__cdecl **)(int, _DWORD))(*(_DWORD *)v43 + 52))(v43, 0)
              && (unsigned __int8)CParty::IsPremiumGoldCardParty((CParty *)this) )
            {
              PremiumGoldCardDefaultItem = CParty::GetPremiumGoldCardDefaultItem((CParty *)this);
              WongWork::CClearReward::generatePremiumGoldCardItem(
                v204,
                (int)&v139,
                (int)v117,
                -1,
                PremiumGoldCardDefaultItem);
            }
            else
            {
              (*(void (__cdecl **)(WongWork::CClearReward *, int *, int *))(*(_DWORD *)v204 + 8))(v204, &v139, v117);
              if ( std::vector<Inven_Item>::empty(v117)
                && ((unsigned __int8)CParty::IsReturnUserParty(this) || (unsigned __int8)CParty::IsEventCharacParty(this)) )
              {
                WongWork::CClearReward::generatePremiumGoldCardItem(v204, (int)&v139, (int)v117, 1, MAX_LIMIT_10000);
              }
            }
            if ( !std::vector<Inven_Item>::empty(v117) )
            {
              std::vector<Inven_Item>::begin(&v172, v117);
              v47 = (const Inven_Item *)__gnu_cxx::__normal_iterator<Inven_Item *,std::vector<Inven_Item>>::operator*((int)&v172);
              Inven_Item::setCopy((Inven_Item *)((char *)this + 122 * jj + 1965), v47);
              if ( *(CUser **)((char *)this + 122 * jj + 1967) == (CUser *)-1 || *((_BYTE *)*(this + 811) + 2206) )
                Inven_Item::reset((Inven_Item *)((char *)this + 122 * jj + 1965));
            }
            else
            {
              Inven_Item::reset((Inven_Item *)((char *)this + 122 * jj + 1965));
              v48 = CEventManager::GetRepeatEvent(GlobalData::s_event_manager, 19);
              if ( (*(unsigned __int8 (__cdecl **)(int, _DWORD))(*(_DWORD *)v48 + 52))(v48, 0) )
                WongWork::CClearReward::generateGoldCardBlankItem(v204, (Inven_Item *)((char *)this + 122 * jj + 1965));
            }
            if ( *((_BYTE *)*(this + 811) + 2170) )
            {
              Inven_Item::Inven_Item((Inven_Item *)&__a);
              *(_DWORD *)((char *)&__a + 2) = *((_DWORD *)G_CDataManager() + 323);
              v49 = G_CDataManager();
              CItemList::create_item(*((CItemList **)v49 + 3), 1, (Inven_Item *)&__a, 1);
              Inven_Item::reset((Inven_Item *)((char *)this + 122 * jj + 1904));
              Inven_Item::reset((Inven_Item *)((char *)this + 122 * jj + 1965));
              Inven_Item::reset((Inven_Item *)((char *)this + 122 * jj + 928));
              Inven_Item::reset((Inven_Item *)((char *)this + 122 * jj + 989));
              Inven_Item::setCopy((Inven_Item *)((char *)this + 122 * jj + 989), (const Inven_Item *)&__a);
              BattleData::ResetEventDungeonClearPoint((BattleData *)(this + 203));
            }
            Rate = v183;
            if ( CUser::isAffectedPremium((int)*(this + 6 * jj + 30), 12) )
            {
              Inven_Item::setCopy(
                (Inven_Item *)((char *)this + 122 * jj + 1416),
                (const Inven_Item *)((char *)this + 122 * jj + 928));
              Inven_Item::set_add_info((Inven_Item *)((char *)this + 122 * jj + 1416), 0);
              v145 = 5;
              std::vector<Inven_Item>::clear(v117);
              v50 = G_CDataManager();
              v51 = CDataManager::getItemGenerator(v50);
              v52 = WongWork::CItemGeneratorMgr::getGenerator(v51, 2);
              (*(void (__cdecl **)(int, int *, int *))(*(_DWORD *)v52 + 8))(v52, &v139, v117);
              if ( !std::vector<Inven_Item>::empty(v117) )
              {
                std::vector<Inven_Item>::begin(&v173, v117);
                v53 = (const Inven_Item *)__gnu_cxx::__normal_iterator<Inven_Item *,std::vector<Inven_Item>>::operator*((int)&v173);
                Inven_Item::setCopy((Inven_Item *)((char *)this + 122 * jj + 1477), v53);
              }
              else
              {
                v54 = G_CDataManager();
                v55 = CDataManager::getItemGenerator(v54);
                v207 = (WongWork::CClearReward *)WongWork::CItemGeneratorMgr::getGenerator(v55, 2);
                Inven_Item::reset((Inven_Item *)((char *)this + 122 * jj + 1477));
                if ( v207 )
                {
                  v56 = CEventManager::GetRepeatEvent(GlobalData::s_event_manager, 29);
                  if ( (*(unsigned __int8 (__cdecl **)(int, _DWORD))(*(_DWORD *)v56 + 52))(v56, 0) )
                    WongWork::CClearReward::generatePcRoomCardBlankItem(
                      v207,
                      (Inven_Item *)((char *)this + 122 * jj + 1477));
                }
              }
            }
            WongWork::stGenerateResult_t::~stGenerateResult_t((void **)v117);
          }
        }
        if ( *((_BYTE *)*(this + 811) + 2206) != 1 )
        {
          v58 = G_CDataManager();
          CDataManager::get_mob_reward(v58, v181, &v164);
        }
        v187 = *(float *)&v179[4 * (_DWORD)(member_count + 215) + 8];
        isHelpAbuseParty = CParty::isHelpAbuseParty((CParty *)this);
        v60 = (CPartyExpBonusEvent *)CEventManager::GetRepeatEvent(GlobalData::s_event_manager, 24);
        BonusRate = CPartyExpBonusEvent::getBonusRate(v60, (int)member_count, isHelpAbuseParty);
        v187 = BonusRate + v187;
        v188 = 0;
        if ( *((_BYTE *)*(this + 811) + 2206) != 1 )
        {
          v103 = (float)v164;
          TotalKilledMonsterCount = BattleData::GetTotalKilledMonsterCount((BattleData *)(this + 203));
          v104 = (long double)TotalKilledMonsterCount * v103 / 2.0 * v187;
          v62 = CBattle_Field::get_dungeon_diff((CBattle_Field *)(this + 713));
          v105 = *(float *)&v179[4 * v62 + 920] * v104;
          v188 = (__int64)(CDungeon::get_exp_weight(*(this + 811)) * v105);
        }
        PacketGuard::PacketGuard((PacketGuard *)v163);
        PacketGuard::PacketGuard((PacketGuard *)v162);
        std::list<map_item>::list(v161);
        Inven_Item::Inven_Item((Inven_Item *)&v147);
        inserted = 0;
        v209 = 0;
        for ( kk = 0; kk <= 3; ++kk )
        {
          if ( (unsigned __int8)CParty::_checkValidUser(this, kk) == 1 )
          {
            CUser::setChattingMessageCount(*(this + 6 * kk + 30), 0);
            if ( *((_BYTE *)*(this + 811) + 2206) )
            {
              v63 = CUserCharacInfo::get_charac_level(*(this + 6 * kk + 30));
              v64 = G_CDataManager();
              CDataManager::get_mob_reward(v64, v63, &v164);
              v106 = (float)v164;
              v115 = BattleData::GetTotalKilledMonsterCount((BattleData *)(this + 203));
              v107 = (long double)v115 * v106 / 2.0 * v187;
              v65 = CBattle_Field::get_dungeon_diff((CBattle_Field *)(this + 713));
              v108 = *(float *)&v179[4 * v65 + 920] * v107;
              v188 = (__int64)(CDungeon::get_exp_weight(*(this + 811)) * v108);
            }
            v212 = CUserCharacInfo::get_charac_level(*(this + 6 * kk + 30));
            v160 = 0;
            if ( *((_BYTE *)*(this + 811) + 2206) != 1 )
            {
              if ( (unsigned __int8)CDungeon::isRiskDungeon(*(this + 811)) )
              {
                v109 = (float)v188;
                v66 = (long double)CParty::MemberPenaltyRate() * v109 / (long double)(int)member_count;
              }
              else
              {
                v110 = (float)v188;
                v67 = v212;
                v68 = G_CDataManager();
                v111 = CDataManager::BaseExpPenalty(v68, v67, v181) * v110;
                v66 = (long double)CParty::MemberPenaltyRate() * v111 / (long double)(int)member_count;
              }
              v160 = (__int64)v66;
            }
            else
            {
              CDungeon::get_index(*(this + 811));
              v160 = v188 / (unsigned int)member_count;
            }
            QuickParty::RewardData_Param::RewardData_Param((QuickParty::RewardData_Param *)v159, (CParty *)this);
            QuickParty::RewardData_Result::RewardData_Result((QuickParty::RewardData_Result *)v158);
            v158[0] = v160;
            CParty::set_random_reward((CParty *)this, 0, v159[0], v159[1], (int)v158);
            v160 = v158[0];
            stClearRewardExpParameter_t::stClearRewardExpParameter_t((stClearRewardExpParameter_t *)v117);
            v120 = kk;
            v121 = RankBonusIndex;
            CParty::set_clear_reward_exp_parameter(
              this,
              *(this + 6 * kk + 30),
              v160,
              (stClearRewardExpParameter_t *)v117);
            stClearRewardBonusExp_t::stClearRewardBonusExp_t((stClearRewardBonusExp_t *)&__a);
            ClearRewardBonusExp = (unsigned int)CParty::getClearRewardBonusExp(
                                                  (CParty *)this,
                                                  *(this + 6 * kk + 30),
                                                  &v160,
                                                  (const stClearRewardExpParameter_t *)v117,
                                                  (size_t *)&__a);
            v69 = CParty::isHelpAbuseParty();
            CUserCharacInfo::calcHelpAbuseRatio(*(this + 6 * kk + 30), ClearRewardBonusExp, v69);
            CUserCharacInfo::reset_level_before_dungeon(*(this + 6 * kk + 30));
            v157 = 0;
            v156 = 0;
            if ( (unsigned __int8)CUser::gain_exp_sp(
                                    rarity,
                                    *(this + 6 * kk + 30),
                                    ClearRewardBonusExp,
                                    &v157,
                                    &v156,
                                    2,
                                    0,
                                    0) )
            {
              InterfacePacketBuf::clear((PacketBuf **)v162);
              InterfacePacketBuf::put_header((PacketBuf **)v162, 0, 2);
              InterfacePacketBuf::put_byte((PacketBuf **)v162, 1);
              InterfacePacketBuf::put_short((PacketBuf **)v162, 1);
              CUser::make_basic_info(rarity, *(this + 6 * kk + 30), v162, 1);
              InterfacePacketBuf::finalize((PacketBuf **)v162, 1);
              CUser::Send(*(this + 6 * kk + 30), (PacketGuard *)v162);
            }
            if ( CUser::IsHavePremiumAdvantage(*(this + 6 * kk + 30)) )
            {
              *((_DWORD *)*(this + 6 * kk + 30) + 145431) -= (int)v119;
              *((_DWORD *)*(this + 6 * kk + 30) + 145434) -= (int)v119;
            }
            std::list<map_item>::clear(v161);
            InterfacePacketBuf::clear(v163);
            InterfacePacketBuf::put_header(v163, 0, 35);
            InterfacePacketBuf::put_int(v163, v126 + v160);
            InterfacePacketBuf::put_int(v163, SHIDWORD(__a));
            CUserCharacInfo::set_charac_party_bonus_exp(*(this + 6 * kk + 30), SHIDWORD(__a));
            rarity = v160;
            v112 = (float)v160;
            v113 = *(float *)&v179[4 * (_DWORD)(member_count + 215) + 8];
            v70 = CParty::isHelpAbuseParty((CParty *)this);
            v71 = (CPartyExpBonusEvent *)CEventManager::GetRepeatEvent(GlobalData::s_event_manager, 24);
            v214 = rarity - (int)(v112 / (CPartyExpBonusEvent::getBonusRate(v71, (int)member_count, v70) + v113));
            if ( v160 )
              v214 *= (v129 + v128 + v127 + v126 + v124 + (int)__a + HIDWORD(__a) + v130) / v160 + 1;
            else
              v214 = 0;
            InterfacePacketBuf::put_int(v163, v214);
            InterfacePacketBuf::put_int(v163, __a);
            InterfacePacketBuf::put_byte(v163, v123);
            InterfacePacketBuf::put_int(v163, v128 + v124 + v125);
            InterfacePacketBuf::put_int(v163, v132);
            InterfacePacketBuf::put_int(v163, v127);
            InterfacePacketBuf::put_int(v163, v133);
            InterfacePacketBuf::put_int(v163, v129);
            InterfacePacketBuf::put_int(v163, v131);
            InterfacePacketBuf::put_int(v163, v130);
            InterfacePacketBuf::put_int(v163, v134);
            InterfacePacketBuf::put_int(v163, v135);
            InterfacePacketBuf::put_int(v163, v136);
            InterfacePacketBuf::put_int(v163, v137);
            InterfacePacketBuf::put_int(v163, v138);
            v215 = 0;
            index = InterfacePacketBuf::get_index(v163);
            InterfacePacketBuf::put_byte(v163, 0);
            v216 = 0;
            for ( mm = 0; mm <= 19; ++mm )
            {
              CurCharacQuestRefR = CUser::getCurCharacQuestRefR(*(this + 6 * kk + 30));
              if ( *(_DWORD *)&CurCharacQuestRefR[4 * mm + 30008] )
              {
                if ( *(_DWORD *)&CurCharacQuestRefR[4 * mm + 30088] )
                {
                  v72 = *(_DWORD *)&CurCharacQuestRefR[4 * mm + 30008];
                  v73 = (QuestList **)G_CDataManager();
                  quest = (Quest *)CDataManager::find_quest(v73, v72);
                  if ( quest )
                  {
                    v74 = *((__int16 *)CurCharacQuestRefR + 15085);
                    v75 = CBattle_Field::get_dungeon_index((CBattle_Field *)(this + 713));
                    if ( (unsigned __int8)Quest::check_clear_item(quest, v75, v74) == 1 )
                    {
                      v210 = *(_DWORD *)&CurCharacQuestRefR[4 * mm + 30008];
                      if ( v210 > 0 )
                      {
                        std::vector<std::pair<int,int>>::vector(v154);
                        std::pair<int,int>::pair(v153);
                        if ( quest )
                        {
                          v76 = CBattle_Field::get_dungeon_index((CBattle_Field *)(this + 713));
                          Quest::get_clear_itemlist((Quest *)v174, (int)quest, v76);
                          std::vector<std::pair<int,int>>::operator=((int *)v154, v174);
                          std::vector<std::pair<int,int>>::~vector(v174);
                          for ( nn = 0; ; ++nn )
                          {
                            v88 = std::vector<std::pair<int,int>>::size(v154);
                            if ( v88 <= nn )
                              break;
                            Inven_Item::reset((Inven_Item *)&v147);
                            v77 = (_DWORD *)std::vector<std::pair<int,int>>::at(v154, nn);
                            v78 = v77[1];
                            v153[0] = *v77;
                            v153[1] = v78;
                            *(_DWORD *)((char *)&v147 + 2) = v153[0];
                            v79 = v78;
                            v80 = G_CDataManager();
                            CItemList::create_item(*((CItemList **)v80 + 3), 1, (Inven_Item *)&v147, v79);
                            CurCharacInvenW = (int *)CUserCharacInfo::getCurCharacInvenW(*(this + 6 * kk + 30));
                            inserted = CInventory::insertItemIntoInventory(
                                         CurCharacInvenW,
                                         v147,
                                         v148,
                                         SHIDWORD(v148),
                                         v149[0],
                                         v149[1],
                                         v149[2],
                                         v149[3],
                                         v149[4],
                                         v149[5],
                                         v149[6],
                                         v149[7],
                                         v149[8],
                                         v149[9],
                                         v149[10],
                                         v150,
                                         11,
                                         1);
                            if ( inserted >= 0 )
                            {
                              InterfacePacketBuf::put_short(v163, inserted);
                              InterfacePacketBuf::put_int(v163, *(int *)((char *)&v147 + 2));
                              v82 = Inven_Item::get_add_info((Inven_Item *)&v147);
                              InterfacePacketBuf::put_int(v163, v82);
                              InterfacePacketBuf::put_short(v163, *(__int16 *)((char *)&v148 + 3));
                              AbilityType = stAmplifyOption_t::getAbilityType((stAmplifyOption_t *)((char *)v149 + 1));
                              InterfacePacketBuf::put_byte(v163, AbilityType);
                              AbilityValue = stAmplifyOption_t::getAbilityValue((stAmplifyOption_t *)((char *)v149 + 1));
                              InterfacePacketBuf::put_short(v163, AbilityValue);
                              ++v216;
                              if ( BYTE1(v147) == 1 )
                              {
                                v85 = *(_DWORD *)((char *)&v147 + 2);
                                v86 = (CItemList **)G_CDataManager();
                                item = (CItem *)CDataManager::find_item(v86, v85);
                                if ( item )
                                {
                                  grade = CItem::get_grade(item);
                                  rarity = CItem::get_rarity(item);
                                  CLuckPoint::UseLuckPoint(*(this + 6 * kk + 30), (CUser *)BYTE1(v147), rarity, grade);
                                }
                              }
                            }
                          }
                        }
                        std::vector<std::pair<int,int>>::~vector(v154);
                      }
                    }
                  }
                }
              }
            }
            if ( v216 )
              InterfacePacketBuf::put_byte(v163, &index, v216);
            for ( i1 = 0; i1 <= 3; ++i1 )
            {
              if ( (unsigned __int8)CParty::_checkValidUser(this, i1) != 1 )
                InterfacePacketBuf::put_byte(v163, 0);
              else
                CParty::_makeRewardItemInfo(
                  (CParty *)this,
                  *(this + 6 * i1 + 30),
                  (Inven_Item *)((char *)this + 122 * i1 + 928),
                  v163);
            }
            InterfacePacketBuf::put_int(v163, (int)*(this + kk + 125));
            for ( i2 = 0; i2 <= 3; ++i2 )
            {
              CParty::_checkValidUser(this, i2);
              InterfacePacketBuf::put_byte(v163, 0);
            }
            for ( i3 = 0; i3 <= 3; ++i3 )
            {
              if ( (unsigned __int8)CParty::_checkValidUser(this, i3) != 1
                || !CUser::isAffectedPremium((int)*(this + 6 * i3 + 30), 12) )
              {
                InterfacePacketBuf::put_byte(v163, 0);
              }
              else
              {
                CParty::_makeRewardItemInfo(
                  (CParty *)this,
                  *(this + 6 * i3 + 30),
                  (Inven_Item *)((char *)this + 122 * i3 + 1416),
                  v163);
              }
            }
            v217 = (int)(*((float *)this + 707) * 100.0 / (long double)(int)*(this + 835));
            InterfacePacketBuf::put_int(v163, v217);
            if ( CUser::GetParty(*(this + 6 * kk + 30))
              && (Party = (CUser **)CUser::GetParty(*(this + 6 * kk + 30)),
                  (unsigned __int8)CParty::IsReturnUserParty(Party)) )
            {
              InterfacePacketBuf::put_byte(v163, 1);
            }
            else
            {
              InterfacePacketBuf::put_byte(v163, 0);
            }
            if ( CUser::GetParty(*(this + 6 * kk + 30))
              && (v91 = (CUserCharacInfo **)CUser::GetParty(*(this + 6 * kk + 30)),
                  (unsigned __int8)CParty::IsEventCharacParty(v91)) )
            {
              InterfacePacketBuf::put_byte(v163, 1);
            }
            else
            {
              InterfacePacketBuf::put_byte(v163, 0);
            }
            if ( *(float *)v117 == 0.0 )
              InterfacePacketBuf::put_byte(v163, 0);
            else
              InterfacePacketBuf::put_byte(v163, 1);
            InterfacePacketBuf::finalize(v163, 1);
            CUser::Send(*(this + 6 * kk + 30), (PacketGuard *)v163);
            if ( *(this + 811)
              && *(this + 6 * kk + 30)
              && *(this + 822) != (CUser *)1
              && CUserCharacInfo::getCurCharacR(*(this + 6 * kk + 30))
              && !*((_BYTE *)*(this + 811) + 2207)
              && !(unsigned __int8)CDungeon::get_dimension_possible(*(this + 811))
              && *((_BYTE *)*(this + 811) + 2206) != 1
              && !CDungeon::isTowerOfDespairDungeon(*(this + 811)) )
            {
              if ( (unsigned __int8)CUserCharacInfo::IsProperDungeonUser(*(this + 6 * kk + 30)) )
              {
                v94 = *(this + 6 * kk + 30);
                v95 = ARAD::Singleton<HeroMissionEvent>::Get();
                HeroMissionEvent::processMission((int)v95, v94, 1, 1);
                CUserCharacInfo::IncProperDungeonClearCount(*(this + 6 * kk + 30));
                v96 = G_CGameManager();
                v226 = (CConditionEventManager *)CGameManager::GetConditionEventManager(v96);
                if ( v226 )
                  CConditionEventManager::ProcessCheckStepUp(v226, *(this + 6 * kk + 30), 2);
              }
            }
            if ( *(this + 811)
              && *(this + 6 * kk + 30)
              && *(this + 822) != (CUser *)1
              && CUserCharacInfo::getCurCharacR(*(this + 6 * kk + 30)) )
            {
              v97 = !*((_BYTE *)*(this + 811) + 2207)
                 && *((_BYTE *)*(this + 811) + 2206) != 1
                 && !CDungeon::isTowerOfDespairDungeon(*(this + 811));
              if ( v97 && (unsigned __int8)CUser::IsProperLevelDungeonUser(*(this + 6 * kk + 30)) )
                CUser::ProcProperLevelDungeonClear(*(this + 6 * kk + 30));
            }
          }
        }
        if ( (unsigned __int8)CParty::IsPremiumGoldCardParty((CParty *)this) )
        {
          PacketGuard::PacketGuard((PacketGuard *)v152);
          InterfacePacketBuf::put_header(v152, 0, 298);
          InterfacePacketBuf::finalize(v152, 1);
          CParty::send_to_party(this, (PacketGuard *)v152);
          PacketGuard::~PacketGuard(v152);
        }
        if ( !*((_BYTE *)*(this + 811) + 2140) )
          CParty::DungeonPermission((CParty *)this, a3);
        std::list<map_item>::~list(v161);
        PacketGuard::~PacketGuard((PacketPool **)v162);
        PacketGuard::~PacketGuard(v163);
        WongWork::stGenerateResult_t::~stGenerateResult_t(v151);
        WongWork::stGenerateRefData_t::~stGenerateRefData_t((void **)&v139);
      }
    }
    else
    {
      LogManager::logFormat(
        (LogManager *)1,
        (int)"party.cpp",
        "void CParty::clear_reward(int)",
        (const char *)0x2A0C,
        "No Member[%d] in party",
        member_count);
    }
  }
  else
  {
    v4 = (const char *)CBattle_Field::get_dungeon_index((CBattle_Field *)(this + 713));
    LogManager::logFormat(
      (LogManager *)1,
      (int)"party.cpp",
      "void CParty::clear_reward(int)",
      (const char *)0x2A07,
      "Unknown Dungeon %d",
      v4);
  }
}

int __cdecl CParty::isPremiumGoldCardReward(CParty *this, const CDungeon *dungeon, unsigned int *a3)
{
  int standard_level; // [esp+10h] [ebp-28h]
  int id; // [esp+14h] [ebp-24h]
  int i; // [esp+18h] [ebp-20h]
  int id; // [esp+1Ch] [ebp-1Ch]
  int id; // [esp+20h] [ebp-18h]
  int id; // [esp+24h] [ebp-14h]
  unsigned int id; // [esp+28h] [ebp-10h]

  standard_level = CDungeon::get_standard_level(a2);
  v5 = *((__int16 *)G_CDataManager() + 9344);
  for ( i = 0; i < v5; ++i )
  {
    v7 = *((_DWORD *)G_CDataManager() + 4 * i + 4673);
    v8 = *((_DWORD *)G_CDataManager() + 4 * i + 4674);
    v9 = *((_DWORD *)G_CDataManager() + 4 * i + 4675);
    v10 = *((_DWORD *)G_CDataManager() + 4 * i + 4676);
    if ( v7 <= 0 || v8 <= 0 || v9 <= 0 )
      break;
    if ( standard_level >= v7 && standard_level <= v8 && (int)get_rand_int(STATIC_POOL_SIZE_1000) <= v9 )
    {
      *a3 = v10;
      return 1;
    }
  }
  return 0;
}

char __cdecl CParty::DungeonPermission(CParty *this, int id)
{
  int id; // ebx
  CDungeonList **v3; // eax
  CDungeon *dungeon; // eax
  int charac_level; // eax
  int dungeon_diff; // eax
  CDataManager *v7; // eax
  char *UltimateDungeonList; // eax
  CDataManager *v9; // eax
  char *v10; // eax
  __int16 v11; // ax
  PacketBuf *v13[3]; // [esp+18h] [ebp-30h] BYREF
  int id; // [esp+24h] [ebp-24h] BYREF
  int index; // [esp+28h] [ebp-20h] BYREF
  int id; // [esp+2Ch] [ebp-1Ch] BYREF
  CDungeon *dungeon; // [esp+30h] [ebp-18h]
  int min_level; // [esp+34h] [ebp-14h]
  char flag; // [esp+3Bh] [ebp-Dh]
  int i; // [esp+3Ch] [ebp-Ch]

  v2 = *((_DWORD *)this + 814);
  v3 = (CDungeonList **)G_CDataManager();
  dungeon = (CDungeon *)CDataManager::find_dungeon(v3, v2);
  v17 = dungeon;
  if ( dungeon )
  {
    min_level = CDungeon::get_min_level(v17);
    for ( i = 0; ; ++i )
    {
      LOBYTE(dungeon) = i <= 3;
      if ( i > 3 )
        break;
      v19 = 0;
      if ( (unsigned __int8)CParty::_checkValidUser((CUser **)this, i) == 1 )
      {
        charac_level = CUserCharacInfo::get_charac_level(*((CUserCharacInfo **)this + 6 * i + 30));
        if ( charac_level >= min_level )
        {
          dungeon_diff = CBattle_Field::get_dungeon_diff((CParty *)((char *)this + 2852));
          switch ( dungeon_diff )
          {
            case 1:
              if ( *((unsigned __int16 *)G_CDataManager() + 10392) <= a2 )
                v19 = CUser::AddDungeonClear(*((CUser **)this + 6 * i + 30), *((_DWORD *)this + 814), 2);
              break;
            case 2:
              index = CDungeon::get_index(v17);
              v7 = G_CDataManager();
              UltimateDungeonList = CDataManager::getUltimateDungeonList(v7);
              std::set<int>::find(&v14, (int)UltimateDungeonList, &index);
              v9 = G_CDataManager();
              v10 = CDataManager::getUltimateDungeonList(v9);
              std::set<int>::end(&v16, (int)v10);
              if ( !std::_Rb_tree_const_iterator<int>::operator==(&v14, &v16) )
                v19 = CParty::addDungeonClear(this, v17, a2, i);
              break;
            case 0:
              v19 = CUser::AddDungeonClear(*((CUser **)this + 6 * i + 30), *((_DWORD *)this + 814), 1);
              break;
          }
          if ( v19 )
          {
            PacketGuard::PacketGuard((PacketGuard *)v13);
            InterfacePacketBuf::put_header(v13, 0, 5);
            InterfacePacketBuf::put_short(v13, 1);
            InterfacePacketBuf::put_short(v13, *((_DWORD *)this + 814));
            v11 = CBattle_Field::get_dungeon_diff((CParty *)((char *)this + 2852));
            InterfacePacketBuf::put_short(v13, v11 + 1);
            InterfacePacketBuf::finalize(v13, 1);
            CUser::Send(*((CUser **)this + 6 * i + 30), (PacketGuard *)v13);
            PacketGuard::~PacketGuard(v13);
          }
        }
      }
    }
  }
  return (char)dungeon;
}

void __usercall CParty::finish_loading(char *a1@<esi>, CParty *this, CUser *user)
{
  bool v3; // al
  CDungeon *Dungeon; // [esp+18h] [ebp-10h]
  int i; // [esp+1Ch] [ebp-Ch]

  for ( i = 0; i <= 3; ++i )
  {
    if ( *((CUser **)this + 6 * i + 30) == a3 )
    {
      if ( *((_BYTE *)this + i + 892) )
        return;
      *((_BYTE *)this + i + 892) = 1;
      if ( *((_BYTE *)this + i + 2832) != 1 && (unsigned __int8)CParty::OnStartMapFinishLoading(a1, this, a3) )
        *((_BYTE *)this + i + 2832) = 1;
    }
  }
  Dungeon = (CDungeon *)CBattle_Field::GetDungeon((CParty *)((char *)this + 2852));
  v3 = Dungeon && (unsigned __int8)CDungeon::IsEnterEachMap(Dungeon);
  if ( !v3 || (unsigned __int8)CBattle_Field::IsFirstEnterDungeon((CParty *)((char *)this + 2852)) )
    CParty::check_allmember_loading((char)a1, this, 0);
  else
    CParty::check_allmember_loading((char)a1, this, 1);
}

        CParty::do_after_dungeon_start(this, Dungeon, a3)) )
  {
    CParty::giveup_game(a1, this, a3, 0, 0, 0);
    return 0;
  }

void __usercall CParty::check_allmember_loading(char flag@<sil>, CParty *this, bool a3)
{
  char flag; // bl
  unsigned int id; // ebx
  unsigned int id; // esi
  bool v6; // al
  bool v7; // bl
  int CurSec; // eax
  unsigned int acc_id; // eax
  char state; // si
  CUser *user; // ebx
  char *HackAnalyzer; // eax
  int id; // ebx
  int id; // esi
  TimerQueue *v16; // eax
  int id; // ebx
  int id; // edi
  int id; // esi
  TimerQueue *v20; // eax
  int id; // esi
  int PartyIndex; // ebx
  TimerQueue *v24; // eax
  int id; // [esp+38h] [ebp-70h]
  PacketBuf *v26[3]; // [esp+40h] [ebp-68h] BYREF
  PacketBuf *v27[3]; // [esp+4Ch] [ebp-5Ch] BYREF
  int id[4]; // [esp+58h] [ebp-50h] BYREF
  int id[4]; // [esp+68h] [ebp-40h] BYREF
  int member_count; // [esp+78h] [ebp-30h]
  int id; // [esp+7Ch] [ebp-2Ch]
  int i; // [esp+80h] [ebp-28h]
  CDungeon *Dungeon; // [esp+84h] [ebp-24h]
  int j; // [esp+88h] [ebp-20h]
  int k; // [esp+8Ch] [ebp-1Ch]

  member_count = CParty::get_member_count((CUser **)this);
  if ( member_count )
  {
    v31 = 0;
    for ( i = 0; i <= 3; ++i )
    {
      if ( (unsigned __int8)CParty::_checkValidUser((CUser **)this, i) == 1 && *((_BYTE *)this + i + 892) )
        ++v31;
    }
    if ( v31 == member_count || a3 && v31 > 0 )
    {
      CBattle_Field::ResetFirstEnterDungeon((CParty *)((char *)this + 2852));
      PacketGuard::PacketGuard((PacketGuard *)v27);
      InterfacePacketBuf::put_header(v27, 0, 30);
      InterfacePacketBuf::put_int(v27, *((_DWORD *)this + 790));
      InterfacePacketBuf::finalize(v27, 1);
      CParty::send_to_party((CUser **)this, (PacketGuard *)v27);
      v4 = *((_DWORD *)this + 214);
      v5 = *((_DWORD *)this + 215);
      *((_QWORD *)this + 107) = __PAIR64__(v5, v4)
                              + (unsigned int)CSystemTime::getCurTickCount((CSystemTime *)GlobalData::s_systemTime_)
                              - *((_QWORD *)this + 108);
      *(_QWORD *)((char *)this + 828) = (unsigned int)CSystemTime::getCurTickCount((CSystemTime *)GlobalData::s_systemTime_)
                                      - *(_QWORD *)((char *)this + 836);
      v6 = (unsigned __int8)CParty::IsAutoCreated(this) != 1 && CParty::get_member_count((CUser **)this) != 1;
      v7 = v6;
      CurSec = CSystemTime::getCurSec((CSystemTime *)GlobalData::s_systemTime_);
      CTraceMobDieHack::traceBegin((CParty *)((char *)this + 528), CurSec, v7);
      for ( j = 0; j <= 3; ++j )
      {
        if ( (unsigned __int8)CParty::_checkValidUser((CUser **)this, j) == 1 )
        {
          if ( CUser::get_state(*((CUser **)this + 6 * j + 30)) <= 2
            || !CUserCharacInfo::getCurCharacR(*((CUserCharacInfo **)this + 6 * j + 30)) )
          {
            acc_id = CUser::get_acc_id(*((CUser **)this + 6 * j + 30));
            NumberToString(acc_id, 0);
            state = CUser::get_state(*((CUser **)this + 6 * j + 30));
            cMyTrace::cMyTrace((cMyTrace *)v29, "void CParty::check_allmember_loading(bool)", 12579, 5);
            cMyTrace::operator()(
              (int)v29,
              "PCROOM : Dispatcher_RequestPCRoomPlayerCount => User State : %d, m_id : %s",
              state);
          }
          CUser::GetPCRoomNo(*((CUser **)this + 6 * j + 30));
        }
      }
      for ( k = 0; k <= 3; ++k )
      {
        if ( (unsigned __int8)CParty::_checkValidUser((CUser **)this, k) == 1 )
        {
          v12 = (CUser *)*((_DWORD *)this + 6 * k + 30);
          HackAnalyzer = CUser::getHackAnalyzer(v12);
          WongWork::CHackAnalyzer::beginCollectHackInfo((WongWork::CHackAnalyzer *)HackAnalyzer, v12);
        }
      }
      *((_BYTE *)this + 281) = 1;
      CBattle_Field::onStartMap((CParty *)((char *)this + 2852));
      if ( *((_DWORD *)this + 814) == 53 && *((_BYTE *)this + 3166) )
      {
        *((_BYTE *)this + 3165) = 0;
        *((_BYTE *)this + 3164) = 2;
        *((_BYTE *)this + 3166) = 0;
        v14 = CParty::gen_timer_key((int)this, 40);
        v15 = *((_DWORD *)this + 25);
        v16 = G_TimerQueue();
        TimerQueue::InsertTimer(v15, v16, 1, v15, 40, 15, v14, 0);
      }
      if ( *((_WORD *)this + 1574) )
      {
        v25 = *((unsigned __int16 *)this + 1574);
        v17 = CParty::gen_timer_key((int)this, 39);
        v18 = *((_DWORD *)this + 790);
        v19 = *((_DWORD *)this + 25);
        v20 = G_TimerQueue();
        TimerQueue::InsertTimer(v19, v20, 1, v19, 39, v18, v17, v25);
      }
      Dungeon = (CDungeon *)CBattle_Field::GetDungeon((CParty *)((char *)this + 2852));
      if ( Dungeon )
      {
        if ( (unsigned __int8)CDungeon::getQuestNpcDungeon(Dungeon) && CParty::checkBossRoom(this) )
        {
          CParty::SetEPLPState(this, 2);
          v22 = CParty::gen_timer_key((int)this, 21);
          PartyIndex = CParty::GetPartyIndex(this);
          v24 = G_TimerQueue();
          TimerQueue::InsertTimer(v22, v24, 1, PartyIndex, 21, 600, v22, 0);
          CParty::set_state(this, 1);
          PacketGuard::PacketGuard((PacketGuard *)v26);
          InterfacePacketBuf::put_header(v26, 0, 254);
          InterfacePacketBuf::finalize(v26, 1);
          CParty::send_to_party((CUser **)this, (PacketGuard *)v26);
          PacketGuard::~PacketGuard(v26);
        }
      }
      PacketGuard::~PacketGuard(v27);
    }
  }
  else
  {
    v3 = CParty::GetPartyIndex(this);
    cMyTrace::cMyTrace((cMyTrace *)v28, "void CParty::check_allmember_loading(bool)", 12512, 0);
    cMyTrace::operator()((int)v28, "<IN_DUNGEON_NULL_PARTY> not_member PartyIndex(%d)", v3);
    CParty::destroy(a1, (CUser **)this);
    LogManager::logFormat(
      (LogManager *)1,
      (int)"party.cpp",
      "void CParty::check_allmember_loading(bool)",
      (const char *)0x30E3,
      "CParty::check_allmember_loading : 1 Using: %d, Member = 0\n",
      (const char *)*((unsigned __int8 *)this + 104));
  }
}

void __usercall CParty::map_load_forced_finish(char *a1@<esi>, CParty *this)
{
  char PartyIndex; // bl
  int id[4]; // [esp+28h] [ebp-20h] BYREF
  int i; // [esp+3Ch] [ebp-Ch]

  if ( !*((_BYTE *)this + 281) )
  {
    if ( CParty::get_member_count((CUser **)this) )
    {
      for ( i = 0; i <= 3; ++i )
      {
        if ( (unsigned __int8)CParty::_checkValidUser((CUser **)this, i) == 1 && *((_BYTE *)this + i + 892) != 1 )
          CParty::giveup_game(a1, this, *((CUser **)this + 6 * i + 30), 0, 0, 0);
      }
      if ( CParty::get_member_count((CUser **)this) > 0 )
        CParty::check_allmember_loading((char)a1, this, 1);
    }
    else
    {
      PartyIndex = CParty::GetPartyIndex(this);
      cMyTrace::cMyTrace((cMyTrace *)v3, "void CParty::map_load_forced_finish()", 12676, 0);
      cMyTrace::operator()((int)v3, "<IN_DUNGEON_NULL_PARTY> not_member PartyIndex(%d)", PartyIndex);
      CParty::destroy((char)a1, (CUser **)this);
      LogManager::logFormat(
        (LogManager *)1,
        (int)"party.cpp",
        "void CParty::map_load_forced_finish()",
        (const char *)0x3187,
        "CParty::map_load_forced_finish 1, Using: %d, Member = 0\n",
        (const char *)*((unsigned __int8 *)this + 104));
    }
  }
}

int __cdecl CParty::send_loadingcheck_msg(CParty *this)
{
  int id; // eax
  int id; // ebx
  int id; // esi
  TimerQueue *v4; // eax
  int id; // [esp+2Ch] [ebp-Ch]

  *((_BYTE *)this + 281) = 0;
  if ( CParty::get_member_count((CUser **)this) == 1 )
    v1 = CONST_SINGLE_MAP_LOADING_WAIT_TIMEOUT;
  else
    v1 = CONST_PARTY_MAP_LOADING_WAIT_TIMEOUT;
  v6 = v1;
  v2 = CParty::gen_timer_key((int)this, 11);
  v3 = *((_DWORD *)this + 25);
  v4 = G_TimerQueue();
  return TimerQueue::InsertTimer(v3, v4, 1, v3, 11, v6, v2, 0);
}

int __cdecl CParty::set_play_result(CUser **this, CUser *user)
{
  int i; // [esp+1Ch] [ebp-Ch]

  if ( *(this + 28) != a2 )
    return 0;
  if ( *((_BYTE *)this + 280) )
    return 0;
  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::_checkValidUser(this, i) == 1 )
      CUser::setPlayingDungeonIndex(*(this + 6 * i + 30), -1);
  }
  *((_BYTE *)this + 106) = 3;
  *((_BYTE *)this + 280) = 1;
  return 0;
}

int __cdecl CParty::insert_time_dungeon_clear_msg(CParty *this, int id)
{
  int id; // ebx
  int id; // esi
  TimerQueue *v4; // eax

  v2 = CParty::gen_timer_key((int)this, 13);
  v3 = *((_DWORD *)this + 25);
  v4 = G_TimerQueue();
  return TimerQueue::InsertTimer(v3, v4, 1, v3, 13, a2, v2, 0);
}

int __cdecl CParty::insert_time_card_select_msg(CParty *this, int id)
{
  int id; // ebx
  int id; // esi
  TimerQueue *v4; // eax

  v2 = CParty::gen_timer_key((int)this, 16);
  v3 = *((_DWORD *)this + 25);
  v4 = G_TimerQueue();
  return TimerQueue::InsertTimer(v3, v4, 1, v3, 16, a2, v2, 0);
}

int __cdecl CParty::insert_time_clear_dungeon_reward_msg(CParty *this, int id)
{
  int id; // ebx
  int id; // esi
  TimerQueue *v4; // eax

  v2 = CParty::gen_timer_key((int)this, 17);
  v3 = *((_DWORD *)this + 25);
  v4 = G_TimerQueue();
  return TimerQueue::InsertTimer(v3, v4, 1, v3, 17, a2, v2, 0);
}

int __cdecl CParty::insert_time_dungeon_score_scroll_msg(CParty *this, int id)
{
  int id; // ebx
  int id; // esi
  TimerQueue *v4; // eax

  v2 = CParty::gen_timer_key((int)this, 18);
  v3 = *((_DWORD *)this + 25);
  v4 = G_TimerQueue();
  return TimerQueue::InsertTimer(v3, v4, 1, v3, 18, a2, v2, 0);
}

int __cdecl CParty::insert_time_card_select_right_msg(CParty *this, int id)
{
  int id; // ebx
  int id; // esi
  TimerQueue *v4; // eax

  v2 = CParty::gen_timer_key((int)this, 19);
  v3 = *((_DWORD *)this + 25);
  v4 = G_TimerQueue();
  return TimerQueue::InsertTimer(v3, v4, 1, v3, 19, a2, v2, 0);
}

int __cdecl CParty::IncreaseSetPlayResultCount(const CUser **this, CUser *user)
{
  int result; // eax
  int value; // [esp+14h] [ebp-4h]

  result = CParty::GetMemberSlotNo(this, a2);
  v3 = result;
  if ( result >= 0 )
  {
    *((_BYTE *)this + 24 * v3 + 124) = 1;
    return (int)(this + 6 * result + 28);
  }
  return result;
}

int __cdecl CParty::GetSetPlayResultCount(CParty *this)
{
  int id; // [esp+8h] [ebp-8h]
  int i; // [esp+Ch] [ebp-4h]

  v2 = 0;
  for ( i = 0; i <= 3; ++i )
  {
    if ( *((_DWORD *)this + 6 * i + 30) && *((_BYTE *)this + 24 * i + 124) )
      ++v2;
  }
  return v2;
}

const char *__cdecl CParty::GetNoSendSetPlayResultUser(CUserCharacInfo **this, int id)
{
  if ( *(this + 6 * a2 + 30)
    && *((_BYTE *)this + 24 * a2 + 124)
    && CUserCharacInfo::getCurCharacR(*(this + 6 * a2 + 30)) )
  {
    return CUserCharacInfo::getCurCharacName(*(this + 6 * a2 + 30));
  }
  else
  {
    return (const char *)&unk_8CB9760;
  }
}

int __cdecl CParty::SetPlayResult(CParty *this, CUser *user)
{
  int value; // esi
  int StartGamePartyCount; // ebx
  CDataManager *v4; // eax
  CDataManager *v5; // eax
  CDataManager *v6; // eax
  CDataManager *v7; // eax
  unsigned __int8 v8; // al
  int member_count; // ebx
  char flag; // bl
  int value[5]; // [esp+3Ch] [ebp-4Ch] BYREF
  int value; // [esp+50h] [ebp-38h]
  char flag[4]; // [esp+54h] [ebp-34h]
  int dungeon_diff; // [esp+58h] [ebp-30h]
  int DungeonClearAverageTime; // [esp+5Ch] [ebp-2Ch]
  int value; // [esp+60h] [ebp-28h]
  int ClearTimeBonusPoint; // [esp+64h] [ebp-24h]
  int value; // [esp+68h] [ebp-20h]
  int value; // [esp+6Ch] [ebp-1Ch]

  if ( (unsigned __int8)CParty::get_dungeon_clear_state(this) == 1 )
  {
    v12[4] = CParty::set_play_result((CUser **)this, a2);
    v13 = *((_DWORD *)this + 203);
    *(_DWORD *)v14 = CBattle_Field::get_dungeon_index((CParty *)((char *)this + 2852));
    dungeon_diff = CBattle_Field::get_dungeon_diff((CParty *)((char *)this + 2852));
    v2 = dungeon_diff;
    StartGamePartyCount = CParty::getStartGamePartyCount(this);
    v4 = G_CDataManager();
    DungeonClearAverageTime = CDataManager::getDungeonClearAverageTime(
                                (int)v4,
                                *(int *)v14,
                                StartGamePartyCount,
                                dungeon_diff);
    v17 = *((char *)this + 6141);
    v5 = G_CDataManager();
    ClearTimeBonusPoint = CDataManager::getClearTimeBonusPoint(v5, DungeonClearAverageTime, v13);
    v6 = G_CDataManager();
    v19 = CDataManager::calRankPoint(v6, ClearTimeBonusPoint, v17);
    v7 = G_CDataManager();
    *((_WORD *)this + 3102) = CDataManager::getRankIndex(v7, v19);
    *((_DWORD *)this + 1536) = v13;
    *((_BYTE *)this + 6148) = ClearTimeBonusPoint;
    if ( (unsigned __int8)CBattle_Field::isQuestMaze((CParty *)((char *)this + 2852)) != 1 )
      CParty::setBestClearTime(this, v14[0], dungeon_diff, v13);
    v8 = DisPatcher_SetPlayResult::SendResult((CUser **)this, (CParty *)((char *)this + 6132));
    v20 = v8;
    if ( !v8 )
      return 0;
    CParty::clear_reward(v2, (CUser **)this, *((unsigned __int16 *)this + 3102));
    CParty::set_dungeon_clear_state(this, (CParty *)2);
    CParty::insert_time_clear_dungeon_reward_msg(this, 30);
    member_count = CParty::get_member_count((CUser **)this);
    if ( member_count != CParty::GetSetPlayResultCount(this) )
    {
      CParty::GetNoSendSetPlayResultUser((CUserCharacInfo **)this, 3);
      CParty::GetNoSendSetPlayResultUser((CUserCharacInfo **)this, 2);
      CParty::GetNoSendSetPlayResultUser((CUserCharacInfo **)this, 1);
      CParty::GetNoSendSetPlayResultUser((CUserCharacInfo **)this, 0);
      CParty::GetSetPlayResultCount(this);
      v11 = CParty::get_member_count((CUser **)this);
      cMyTrace::cMyTrace((cMyTrace *)v12, "bool CParty::SetPlayResult(CUser*)", 12897, 0);
      cMyTrace::operator()((int)v12, "DUNGEON_CLEAR_BUG_CHECK total(%d),recv(%d), (%s,%s,%s,%s)", v11);
    }
  }
  return 1;
}

void __cdecl CParty::forced_set_play_result(CUser **this)
{
  PacketBuf *v1[5]; // [esp+14h] [ebp-14h] BYREF

  if ( *((_BYTE *)this + 106) != 3 )
  {
    PacketGuard::PacketGuard((PacketGuard *)v1);
    InterfacePacketBuf::put_header(v1, 0, 34);
    InterfacePacketBuf::put_byte(v1, 0);
    InterfacePacketBuf::put_int(v1, 0);
    InterfacePacketBuf::put_byte(v1, 0);
    InterfacePacketBuf::put_byte(v1, 0);
    InterfacePacketBuf::finalize(v1, 1);
    CParty::send_to_party(this, (PacketGuard *)v1);
    PacketGuard::~PacketGuard(v1);
  }
}

int __cdecl CParty::ReviveUser(CParty *this, CUser *user, unsigned __int16 a3)
{
  _IO_FILE *CurCharacName; // eax
  CUserCharacInfo *charac_info; // [esp+20h] [ebp-18h]
  bool v6; // [esp+27h] [ebp-11h]
  int i; // [esp+28h] [ebp-10h]
  int j; // [esp+2Ch] [ebp-Ch]

  if ( (unsigned __int8)CParty::get_state(this) != 2 && !(unsigned __int8)CParty::GetEPLPState(this) )
    return 19;
  v5 = 0;
  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::_checkValidUser((CUser **)this, i) == 1
      && (unsigned __int16)CUser::get_unique_id(*((CUser **)this + 6 * i + 30)) == a3 )
    {
      if ( *((_BYTE *)this + i + 896) )
        return 18;
      v5 = (CUserCharacInfo *)*((_DWORD *)this + 6 * i + 30);
      break;
    }
  }
  if ( !v5 )
    return 21;
  v6 = CParty::checkFreeRevivalCondition(this, 0);
  for ( j = 0; j <= 3; ++j )
  {
    if ( (unsigned __int8)CParty::_checkValidUser((CUser **)this, j) == 1 && *((CUser **)this + 6 * j + 30) == a2 && !v6 )
    {
      if ( !*((_DWORD *)this + j + 709) )
        return 1;
      CurCharacName = (_IO_FILE *)CUserCharacInfo::getCurCharacName(v5);
      HistoryLog::WriteUseCoin(*((FILE **)a2 + 124350), CurCharacName);
      --*((_DWORD *)this + j + 709);
    }
  }
  CParty::set_charac_live(this, v5, 1u);
  CParty::Reset_party_overlapped_drop_ratio(this);
  return 0;
}

void __cdecl CParty::set_charac_live(CParty *party, CUser *user, unsigned int value)
{
  __int16 unique_id; // ax
  PacketBuf *v4[3]; // [esp+10h] [ebp-18h] BYREF
  int i; // [esp+1Ch] [ebp-Ch]

  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::_checkValidUser((CUser **)a1, i) == 1 && *((CUser **)a1 + 6 * i + 30) == a2 )
    {
      if ( a3 )
      {
        if ( a3 <= 2 )
        {
          *((_BYTE *)a1 + i + 896) = 1;
          *((_BYTE *)a1 + i + 900) = 1;
          *((_BYTE *)a1 + i + 904) = 1;
          if ( a3 == 1 )
            ++*((_DWORD *)a1 + 6 * i + 35);
        }
      }
      else
      {
        *((_BYTE *)a1 + i + 896) = 0;
      }
      PacketGuard::PacketGuard((PacketGuard *)v4);
      InterfacePacketBuf::put_header(v4, 0, 32);
      unique_id = CUser::get_unique_id(a2);
      InterfacePacketBuf::put_short(v4, unique_id);
      InterfacePacketBuf::put_byte(v4, a3);
      InterfacePacketBuf::finalize(v4, 1);
      CParty::send_to_party((CUser **)a1, (PacketGuard *)v4);
      PacketGuard::~PacketGuard(v4);
      return;
    }
  }
}

int __cdecl CParty::check_allmember_die(CUser **this)
{
  int i; // [esp+1Ch] [ebp-Ch]

  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::_checkValidUser(this, i) == 1 && *((_BYTE *)this + i + 896) )
      return 0;
  }
  return 1;
}

void __usercall CParty::giveup_game(char *MemberNames@<esi>, CParty *this, CUser *user, bool a4, bool a5, bool a6)
{
  int CurCharacR; // eax
  _IO_FILE *CurCharacName; // eax
  char *HackAnalyzer; // eax
  CUser *Manager; // eax
  int index; // ebx
  CDungeonEntranceLog *InstanceDungeonEntranceLog; // eax
  GameWorld *v16; // eax
  CUser *user; // eax
  GameWorld *v18; // eax
  int id; // edi
  int dungeon_diff; // ebx
  char *DungeonName; // eax
  CUser *user; // ebx
  GameWorld *v24; // eax
  __int16 v25; // ax
  GameWorld *v26; // eax
  GameWorld *v27; // eax
  GameWorld *v28; // eax
  int id; // eax
  CGameManager *v30; // eax
  char s[256]; // [esp+3Ch] [ebp-14Ch] BYREF
  char flag[15]; // [esp+13Ch] [ebp-4Ch] BYREF
  bool v34; // [esp+14Bh] [ebp-3Dh]
  CDungeon *dungeon; // [esp+14Ch] [ebp-3Ch]
  bool v36; // [esp+153h] [ebp-35h]
  int member_count; // [esp+154h] [ebp-34h]
  char EPLPState; // [esp+15Bh] [ebp-2Dh]
  int id; // [esp+15Ch] [ebp-2Ch]
  CSpecialItemRoutingManager *SpecialItemRoutingManager; // [esp+160h] [ebp-28h]
  int i; // [esp+164h] [ebp-24h]
  int j; // [esp+168h] [ebp-20h]
  int k; // [esp+16Ch] [ebp-1Ch]

  v34 = 0;
  v35 = (CDungeon *)*((_DWORD *)this + 811);
  if ( v35 )
  {
    v34 = *((_BYTE *)v35 + 2207) > 0;
    if ( CDungeon::isTowerOfDespairDungeon(v35) )
      v34 = 1;
  }
  if ( v35 )
  {
    if ( (char)CDungeon::get_dimension_possible(v35) > 0 || *((_BYTE *)v35 + 2204) || CDungeon::isTournamentDungeon(v35) )
      a4 = 0;
  }
  if ( v35 && *((_BYTE *)v35 + 2140) )
    a4 = 0;
  if ( v35 && *((_BYTE *)v35 + 2170) )
    a4 = 0;
  v36 = *((_DWORD *)this + 822) == 1;
  CUserCharacInfo::reset_level_before_dungeon(a3);
  CDungeonClearTracer::Trace((CParty *)((char *)this + 3196), "void CParty::giveup_game(CUser*, bool, bool, bool)");
  CParty::Reset_party_overlapped_drop_ratio(this);
  member_count = CParty::get_member_count((CUser **)this);
  if ( a3 && CUserCharacInfo::getCurCharacR(a3) && !v36 )
  {
    CurCharacR = CUserCharacInfo::getCurCharacR(a3);
    Statistics::DungeonFail((Statistics *)*(__int16 *)(CurCharacR + 39));
  }
  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::_checkValidUser((CUser **)this, i) == 1 )
    {
      CurCharacName = (_IO_FILE *)CUserCharacInfo::getCurCharacName(a3);
      HistoryLog::WriteGiveUp(*(FILE **)(*((_DWORD *)this + 6 * i + 30) + 497400), CurCharacName);
    }
  }
  if ( *((_WORD *)this + 1574) || v35 && CDungeon::get_index(v35) == MAX_LIMIT_10000 )
    a4 = 0;
  if ( a5 )
  {
    CParty::OnSuccessedDungeonClear((unsigned __int8)MemberNames, this, (CUser **)a3);
  }
  else
  {
    if ( CParty::get_member_count((CUser **)this) == 1 )
    {
      CParty::CheckHackAverageHitCount(this);
      Secu_HackLogCheckByParty::dungeonFail((CParty *)((char *)this + 6844));
    }
    CParty::OnFailedDungeonClear((unsigned __int8)MemberNames, this, (CUser **)a3);
  }
  if ( a3 && CBattle_Field::check_start_point((CParty *)((char *)this + 2852)) )
  {
    HackAnalyzer = CUser::getHackAnalyzer(a3);
    WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)HackAnalyzer, a3, 0x325u, 1, 0, 0);
  }
  CParty::SaveCoin((CUser **)this);
  EPLPState = CParty::GetEPLPState(this);
  if ( EPLPState == 1 )
  {
    Manager = (CUser *)CParty::getManager(this);
    if ( CUser::getMoveSpace(Manager) != 1 )
    {
      index = CDungeon::get_index(v35);
      InstanceDungeonEntranceLog = (CDungeonEntranceLog *)GetInstanceDungeonEntranceLog();
      CDungeonEntranceLog::DecrementDungeonEntrance(InstanceDungeonEntranceLog, index);
    }
  }
  PacketGuard::PacketGuard((PacketGuard *)v33);
  if ( *((_BYTE *)this + 316) )
  {
    CParty::leave_user((std::string **)this, (std::string *)a3, 2);
    v16 = G_GameWorld();
    GameWorld::out_from_dungeon(v16, a3);
    if ( !v36 && !v34 && EPLPState == 1 && a4 )
      CUser::giveup_panalty(a3);
    InterfacePacketBuf::clear((PacketBuf **)v33);
    InterfacePacketBuf::put_header((PacketBuf **)v33, 0, 2);
    InterfacePacketBuf::put_byte((PacketBuf **)v33, 0);
    InterfacePacketBuf::put_short((PacketBuf **)v33, 1);
    CUser::make_basic_info((unsigned __int8)MemberNames, a3, v33, 0);
    InterfacePacketBuf::finalize((PacketBuf **)v33, 1);
    v17 = (CUser *)CParty::getManager(this);
    if ( CUser::getMoveSpace(v17) == 1 )
    {
      CParty::send_to_party((CUser **)this, (PacketGuard *)v33);
    }
    else
    {
      v18 = G_GameWorld();
      GameWorld::send_all(v18, (PacketGuard *)v33, a3);
    }
  }
  else
  {
    if ( !a5 )
      CParty::set_charac_live(this, a3, 0);
    memset(s, 0, sizeof(s));
    CParty::_getMemberNames((CUserCharacInfo **)this, s);
    if ( (unsigned __int8)CParty::GetSelectedEPLPCmd(this) != 3
      && (unsigned __int8)CParty::check_allmember_die((CUser **)this)
      || a5
      || a6 )
    {
      for ( j = 0; j <= 3; ++j )
      {
        if ( (unsigned __int8)CParty::_checkValidUser((CUser **)this, j) == 1 )
        {
          if ( v35 )
          {
            v20 = *((_DWORD *)this + 822);
            MemberNames = CParty::_getMemberNames((CUserCharacInfo **)this, s);
            dungeon_diff = CBattle_Field::get_dungeon_diff((CParty *)((char *)this + 2852));
            DungeonName = CDungeon::GetDungeonName((std::string *)v35);
            cUserHistoryLog::LeaveDungeon(
              (CUser **)(*((_DWORD *)this + 6 * j + 30) + CUSER_OFFSET_HISTORY_LOG),
              DungeonName,
              dungeon_diff,
              MemberNames,
              v20);
          }
          if ( *((_WORD *)this + 1574) && *((CUser **)this + 6 * j + 30) != a3 )
          {
            if ( a5 )
              CParty::OnSuccessedDungeonClear((unsigned __int8)MemberNames, this, *((CUser ***)this + 6 * j + 30));
            else
              CParty::OnFailedDungeonClear((unsigned __int8)MemberNames, this, *((CUser ***)this + 6 * j + 30));
          }
          v23 = (CUser *)*((_DWORD *)this + 6 * j + 30);
          v24 = G_GameWorld();
          GameWorld::out_from_dungeon(v24, v23);
          if ( !v36 && !v34 && EPLPState == 1 && a4 )
            CUser::giveup_panalty(*((CUser **)this + 6 * j + 30));
        }
      }
      InterfacePacketBuf::clear((PacketBuf **)v33);
      InterfacePacketBuf::put_header((PacketBuf **)v33, 0, 2);
      InterfacePacketBuf::put_byte((PacketBuf **)v33, 0);
      v25 = CParty::get_member_count((CUser **)this);
      InterfacePacketBuf::put_short((PacketBuf **)v33, v25);
      for ( k = 0; k <= 3; ++k )
      {
        if ( (unsigned __int8)CParty::_checkValidUser((CUser **)this, k) == 1 )
          CUser::make_basic_info((unsigned __int8)MemberNames, *((CUser **)this + 6 * k + 30), v33, 0);
      }
      InterfacePacketBuf::finalize((PacketBuf **)v33, 1);
      if ( (unsigned __int8)CParty::IsExistInvisible((CUser **)this) )
      {
        CParty::send_to_party((CUser **)this, (PacketGuard *)v33);
      }
      else
      {
        v26 = G_GameWorld();
        GameWorld::send_all(v26, (PacketGuard *)v33);
      }
      CParty::set_state(this, 1);
      CParty::battle_data_reset((char)MemberNames, (CUser **)this);
    }
    else
    {
      v39 = CParty::leave_user((std::string **)this, (std::string *)a3, 2);
      if ( v39 != 1 )
      {
        v27 = G_GameWorld();
        GameWorld::out_from_dungeon(v27, a3);
        if ( !v36 && !v34 && EPLPState == 1 && a4 )
          CUser::giveup_panalty(a3);
        InterfacePacketBuf::clear((PacketBuf **)v33);
        InterfacePacketBuf::put_header((PacketBuf **)v33, 0, 2);
        InterfacePacketBuf::put_byte((PacketBuf **)v33, 0);
        InterfacePacketBuf::put_short((PacketBuf **)v33, 1);
        CUser::make_basic_info((unsigned __int8)MemberNames, a3, v33, 0);
        InterfacePacketBuf::finalize((PacketBuf **)v33, 1);
        v28 = G_GameWorld();
        GameWorld::send_all(v28, (PacketGuard *)v33, a3);
      }
      v29 = CParty::checkInoutConditionDungeon((int)s, this);
      CParty::sendInoutConditionDungeon((CUser **)this, v29);
    }
    v30 = G_CGameManager();
    SpecialItemRoutingManager = (CSpecialItemRoutingManager *)CGameManager::GetSpecialItemRoutingManager(v30);
    if ( SpecialItemRoutingManager )
      CSpecialItemRoutingManager::ProcessGiveUp(SpecialItemRoutingManager, this);
  }
  PacketGuard::~PacketGuard((PacketPool **)v33);
}

bool __cdecl CParty::SaveCoin(CUser **a1)
{
  bool result; // al
  int i; // [esp+1Ch] [ebp-Ch]

  for ( i = 0; ; ++i )
  {
    result = i <= 3;
    if ( i > 3 )
      break;
    if ( (unsigned __int8)CParty::_checkValidUser(a1, i) == 1 )
      CUser::SaveMoneyCoin(a1[6 * i + 30]);
  }
  return result;
}

int __cdecl CParty::GetCoinCount(CParty *this, CUser *user)
{
  int i; // [esp+1Ch] [ebp-Ch]

  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::_checkValidUser((CUser **)this, i) == 1 && *((CUser **)this + 6 * i + 30) == a2 )
      return MAX_INT32 - *((_DWORD *)this + i + 709);
  }
  return 0;
}

int __cdecl CParty::send_dungeon_fail_msg(CParty *this)
{
  int id; // ebx
  int id; // esi
  TimerQueue *v3; // eax
  int id; // [esp+2Ch] [ebp-Ch]

  v5 = 10;
  if ( *((_WORD *)this + 1574) )
    v5 = 3;
  v1 = CParty::gen_timer_key((int)this, 14);
  v2 = *((_DWORD *)this + 25);
  v3 = G_TimerQueue();
  return TimerQueue::InsertTimer(v2, v3, 1, v2, 14, v5, v1, 0);
}

void __usercall CParty::party_dungeon_fail(char flag@<sil>, CParty *this)
{
  CUser *user; // ebx
  GameWorld *v4; // eax
  int index; // ebx
  CDungeonEntranceLog *InstanceDungeonEntranceLog; // eax
  __int16 member_count; // ax
  char state; // si
  unsigned int acc_id; // eax
  char flag; // bl
  CUser *Manager; // eax
  CUser *user; // eax
  CUser *user; // ebx
  GameWorld *v14; // eax
  GameWorld *v15; // eax
  char flag[12]; // [esp+2Ch] [ebp-4Ch] BYREF
  int id[4]; // [esp+38h] [ebp-40h] BYREF
  bool v18; // [esp+4Bh] [ebp-2Dh]
  CDungeon *dungeon; // [esp+4Ch] [ebp-2Ch]
  int i; // [esp+50h] [ebp-28h]
  BlueMarble *BlueMarble; // [esp+54h] [ebp-24h]
  bool v22; // [esp+5Ah] [ebp-1Eh]
  bool v23; // [esp+5Bh] [ebp-1Dh]
  int j; // [esp+5Ch] [ebp-1Ch]

  if ( !*((_BYTE *)this + 282)
    && (unsigned __int8)CParty::check_allmember_die((CUser **)this) == 1
    && *((_DWORD *)this + 811) )
  {
    v18 = 0;
    v19 = (CDungeon *)*((_DWORD *)this + 811);
    if ( v19 )
    {
      v18 = *((_BYTE *)v19 + 2207) > 0;
      if ( CDungeon::isTowerOfDespairDungeon(v19) )
        v18 = 1;
    }
    if ( *((_WORD *)this + 1574) )
      v18 = 1;
    if ( v19 )
    {
      if ( (char)CDungeon::get_dimension_possible(v19) > 0 || *((_BYTE *)v19 + 2204) )
        v18 = 1;
    }
    if ( CDungeon::isTournamentDungeon(*((CDungeon **)this + 811)) )
    {
      CTournamentDungeonReward::ResetReward((CParty *)((char *)this + 6076));
      CParty::MakeTournamentReward(a1, this, (CParty *)((char *)this + 6076));
      return;
    }
    *(_QWORD *)((char *)this + 812) = (unsigned int)CSystemTime::getCurTickCount((CSystemTime *)GlobalData::s_systemTime_)
                                    - *(_QWORD *)((char *)this + 844)
                                    - *((_QWORD *)this + 107);
    CParty::SaveCoin((CUser **)this);
    CParty::CheckHackAverageHitCount(this);
    Secu_HackLogCheckByParty::dungeonFail((CParty *)((char *)this + 6844));
    PacketGuard::PacketGuard((PacketGuard *)v16);
    for ( i = 0; i <= 3; ++i )
    {
      if ( (unsigned __int8)CParty::_checkValidUser((CUser **)this, i) )
      {
        CParty::OnFailedDungeonClear(a1, this, *((CUser ***)this + 6 * i + 30));
        v3 = (CUser *)*((_DWORD *)this + 6 * i + 30);
        v4 = G_GameWorld();
        GameWorld::out_from_dungeon(v4, v3);
        if ( CUser::getMoveSpace(*((CUser **)this + 6 * i + 30)) != 1 )
        {
          index = CDungeon::get_index(*((CDungeon **)this + 811));
          InstanceDungeonEntranceLog = (CDungeonEntranceLog *)GetInstanceDungeonEntranceLog();
          CDungeonEntranceLog::DecrementDungeonEntrance(InstanceDungeonEntranceLog, index);
        }
        if ( CUser::getMoveSpace(*((CUser **)this + 6 * i + 30)) == 1 )
        {
          BlueMarble = (BlueMarble *)CUser::getBlueMarble(*((CUser **)this + 6 * i + 30));
          if ( BlueMarble )
            BlueMarble::giveupDungeon((BlueMarbleScriptManager **)BlueMarble, *((CUser **)this + 6 * i + 30));
        }
        if ( *((_DWORD *)this + 822) != 1 )
        {
          v22 = 0;
          if ( v19 )
            v22 = *((_BYTE *)v19 + 2140) != 0;
          v23 = 0;
          if ( v19 )
            v23 = *((_BYTE *)v19 + 2170) != 0;
          if ( !v18 && !v22 && !v23 )
            CUser::giveup_panalty(*((CUser **)this + 6 * i + 30));
        }
      }
    }
    InterfacePacketBuf::clear((PacketBuf **)v16);
    InterfacePacketBuf::put_header((PacketBuf **)v16, 0, 2);
    InterfacePacketBuf::put_byte((PacketBuf **)v16, 0);
    member_count = CParty::get_member_count((CUser **)this);
    InterfacePacketBuf::put_short((PacketBuf **)v16, member_count);
    if ( *((_BYTE *)this + 316) )
    {
      CUser::GetPartyIndex(*((CUser **)this + 29));
      state = CUser::get_state(*((CUser **)this + 29));
      acc_id = CUser::get_acc_id(*((CUser **)this + 29));
      v10 = NumberToString(acc_id, 0);
      cMyTrace::cMyTrace((cMyTrace *)v17, "void CParty::party_dungeon_fail()", 13633, 0);
      cMyTrace::operator()((int)v17, "<IN_DUNGEON_NULL_PARTY> m_id(%s), user_state(%d), PartyIndex(%d)", v10);
      CUser::SetPartyIndex(*((pthread_mutex_t **)this + 29), -1);
      Manager = (CUser *)CParty::getManager(this);
      if ( CUser::getMoveSpace(Manager) != 1 )
        CUser::set_state(*((CUserCharacInfo **)this + 29), 3);
      CUser::make_basic_info(state, *((CUser **)this + 29), v16, 0);
      CParty::destroy(state, (CUser **)this);
      InterfacePacketBuf::finalize((PacketBuf **)v16, 1);
      v12 = (CUser *)CParty::getManager(this);
      if ( CUser::getMoveSpace(v12) != 1 )
      {
        v13 = (CUser *)*((_DWORD *)this + 29);
        v14 = G_GameWorld();
        GameWorld::send_all(v14, (PacketGuard *)v16, v13);
LABEL_50:
        PacketGuard::~PacketGuard((PacketPool **)v16);
        return;
      }
    }
    else
    {
      for ( j = 0; j <= 3; ++j )
      {
        if ( (unsigned __int8)CParty::_checkValidUser((CUser **)this, j) )
          CUser::make_basic_info(a1, *((CUser **)this + 6 * j + 30), v16, 0);
      }
      CParty::set_state(this, 1);
      CParty::battle_data_reset(a1, (CUser **)this);
      InterfacePacketBuf::finalize((PacketBuf **)v16, 1);
      if ( !(unsigned __int8)CParty::IsExistInvisible((CUser **)this) )
      {
        v15 = G_GameWorld();
        GameWorld::send_all(v15, (PacketGuard *)v16);
        goto LABEL_50;
      }
    }
    CParty::send_to_party((CUser **)this, (PacketGuard *)v16);
    goto LABEL_50;
  }
}

int __cdecl CParty::VoteLackUser(CParty *this, CUser *user, unsigned int id)
{
  int id; // ebx
  int id; // esi
  TimerQueue *v6; // eax
  int id[4]; // [esp+28h] [ebp-30h] BYREF
  int id[4]; // [esp+38h] [ebp-20h] BYREF
  CUser *user; // [esp+48h] [ebp-10h]
  unsigned int MemberSlotNo; // [esp+4Ch] [ebp-Ch]

  if ( a3 < 4 )
  {
    user = (CUser *)CParty::get_user(this, a3);
    if ( a2 != user && user )
    {
      MemberSlotNo = CParty::GetMemberSlotNo((const CUser **)this, a2);
      if ( MemberSlotNo < 4 )
      {
        cElection<int,4,4>::Vote((int)this + 216, MemberSlotNo, a3);
        v4 = CParty::gen_timer_key((int)this, 15);
        v5 = *((_DWORD *)this + 25);
        v6 = G_TimerQueue();
        TimerQueue::InsertTimerInMilisecond(v5, v6, 1, v5, 15, 2500, v4, 0);
        return 1;
      }
      else
      {
        cMyTrace::cMyTrace((cMyTrace *)v8, "bool CParty::VoteLackUser(CUser*, int)", 13729, 5);
        cMyTrace::operator()((int)v8, "[%s][%d]", (char)"bool CParty::VoteLackUser(CUser*, int)");
        return 0;
      }
    }
    else
    {
      return 0;
    }
  }
  else
  {
    cMyTrace::cMyTrace((cMyTrace *)v7, "bool CParty::VoteLackUser(CUser*, int)", 13713, 5);
    cMyTrace::operator()((int)v7, "[%s][%d]", (char)"bool CParty::VoteLackUser(CUser*, int)");
    return 0;
  }
}

int __cdecl CParty::OnTimerWalkOutLackUser(CParty *this)
{
  GameWorld *v2; // eax
  const char *v3; // [esp+14h] [ebp-24h]
  int i; // [esp+28h] [ebp-10h]
  CUser *user; // [esp+2Ch] [ebp-Ch]

  for ( i = cElection<int,4,4>::GetElected((int)this + 216); i >= 0; i = cElection<int,4,4>::GetElected((int)this + 216) )
  {
    cElection<int,4,4>::CancelCandidate((int)this + 216, i);
    cElection<int,4,4>::CancelVoter((int)this + 216, i);
    v5 = (CUser *)*((_DWORD *)this + 6 * i + 30);
    if ( !v5 )
    {
      LogManager::logFormat(
        (LogManager *)1,
        (int)"party.cpp",
        "bool CParty::OnTimerWalkOutLackUser()",
        (const char *)0x35C6,
        "pVictim is null",
        v3);
      return 0;
    }
    CParty::SendWalkout((CUser **)this, i, 1);
    CParty::walkout_member((CUser **)this, *((CUser **)this + 29), i);
    v2 = G_GameWorld();
    GameWorld::out_from_dungeon(v2, v5);
  }
  return 1;
}

int __cdecl CParty::set_recv_party_packet_flag(const CUser **this, CUser *user)
{
  int result; // eax

  result = CParty::GetMemberSlotNo(this, a2);
  if ( result >= 0 )
    return CPartyResultRecvFlag::SetRecvFlag((CPartyResultRecvFlag *)(this + 119), result);
  return result;
}

int __cdecl CParty::recv_party_packet_all(CUser **this)
{
  int i; // [esp+1Ch] [ebp-Ch]

  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::_checkValidUser(this, i) == 1
      && (unsigned __int8)CPartyResultRecvFlag::IsRecv((CPartyResultRecvFlag *)(this + 119), i) != 1 )
    {
      return 0;
    }
  }
  return 1;
}

int __cdecl CParty::CheckRecvFromAllMember(const CUser **this, CUser *user)
{
  CParty::set_recv_party_packet_flag(this, a2);
  if ( (unsigned __int8)CParty::recv_party_packet_all(this) != 1 )
    return 0;
  CParty::reset_recv_flag((CParty *)this);
  return 1;
}

int __cdecl CParty::HaveAlreadySelectedCard(int id, char flag, int id2)
{
  int i; // [esp+10h] [ebp-4h]

  if ( a3 > 1 )
    return 0;
  for ( i = 0; i <= 3; ++i )
  {
    if ( *(_BYTE *)(4 * a3 + a1 + i + 516) == a2 )
      return 1;
  }
  return 0;
}

int __cdecl CParty::_checkGoldCardSelectable(int id, CUserCharacInfo *charac_info, unsigned int id2, unsigned int id3)
{
  int id; // ebx
  int CurCharacInvenW; // eax

  if ( !a2 )
    return 0;
  if ( a3 >= 2 )
    return 0;
  if ( a4 >= 4 )
    return 0;
  if ( !*(_DWORD *)(a1 + 4 * (4 * a3 + a4 + 120) + 4) )
    return 1;
  v5 = *(_DWORD *)(a1 + 4 * (4 * a3 + a4 + 120) + 4);
  CurCharacInvenW = CUserCharacInfo::getCurCharacInvenW(a2);
  if ( !(unsigned __int8)CInventory::use_money(CurCharacInvenW, v5, (char *)SIZE_20_BYTES, 1) )
    return 0;
  CUser::SendUpdateItemList(a2, 1, 0, 0);
  return 1;
}

int __cdecl CParty::SetCardNumber(const CUser **a1, CUserCharacInfo *charac_info, char flag, int value)
{
  int RepeatEvent; // eax
  int CurCharacInvenR; // eax
  int value; // eax
  int CurCharacInvenW; // eax
  CItemList **v10; // eax
  unsigned int MemberSlotNo; // [esp+60h] [ebp-18h]
  unsigned int value; // [esp+64h] [ebp-14h]
  CItem *item; // [esp+68h] [ebp-10h]

  MemberSlotNo = CParty::GetMemberSlotNo(a1, a2);
  if ( MemberSlotNo == -1 || a3 > 3 )
    return 0;
  if ( (unsigned __int8)CParty::HaveAlreadySelectedCard((int)a1, MemberSlotNo, a4) )
    return 0;
  if ( *((_BYTE *)&a1[a4 + 129] + a3) != MASK_BYTE )
    return 0;
  if ( (unsigned __int8)CParty::_checkGoldCardSelectable((int)a1, a2, a4, MemberSlotNo) != 1 )
    return 0;
  *((_BYTE *)&a1[a4 + 129] + a3) = MemberSlotNo;
  if ( a4 == 1 )
  {
    RepeatEvent = CEventManager::GetRepeatEvent(GlobalData::s_event_manager, 65);
    if ( (*(unsigned __int8 (__cdecl **)(int, _DWORD))(*(_DWORD *)RepeatEvent + 52))(RepeatEvent, 0) != 1 )
    {
      CurCharacInvenR = CUserCharacInfo::getCurCharacInvenR(a2);
      if ( CInventory::tryInsertItemIntoInventory(
             CurCharacInvenR,
             *(long double *)((char *)a1 + 122 * MemberSlotNo + 1965)) == -1 )
      {
        *(const CUser **)((char *)a1 + 122 * MemberSlotNo + 1967) = 0;
        *(const CUser **)((char *)a1 + 122 * MemberSlotNo + 1972) = 0;
      }
      else
      {
        v7 = CEventManager::GetRepeatEvent(GlobalData::s_event_manager, 64);
        if ( ((*(unsigned __int8 (__cdecl **)(int, _DWORD))(*(_DWORD *)v7 + 52))(v7, 0)
           || (unsigned __int8)CParty::IsReturnUserParty(a1)
           || (unsigned __int8)CParty::IsEventCharacParty(a1))
          && *((_BYTE *)a1 + 122 * MemberSlotNo + 1966) == 1 )
        {
          CurCharacInvenW = CUserCharacInfo::getCurCharacInvenW(a2);
          if ( (unsigned __int8)CInventory::check_empty_count(CurCharacInvenW, 1, 2) != 1 )
          {
            *(const CUser **)((char *)a1 + 122 * MemberSlotNo + 1967) = 0;
            *(const CUser **)((char *)a1 + 122 * MemberSlotNo + 1972) = 0;
          }
        }
      }
    }
    if ( a2 )
    {
      APSystem::CUserProc::ClearActionAndSendtoUser();
      v12 = *(unsigned int *)((char *)a1 + 122 * MemberSlotNo + 1967);
      if ( v12 )
      {
        v10 = (CItemList **)G_CDataManager();
        item = (CItem *)CDataManager::find_item(v10, v12);
        if ( item )
        {
          if ( (unsigned int)CItem::get_rarity(item) <= 4 )
            APSystem::CUserProc::ClearActionAndSendtoUser();
        }
      }
    }
  }
  return 1;
}

void __cdecl CParty::SendEachCardInfo(CParty *this)
{
  int add_info; // eax
  int id; // eax
  int index; // [esp+1Ch] [ebp-1Ch] BYREF
  PacketBuf *v4[3]; // [esp+20h] [ebp-18h] BYREF
  int i; // [esp+2Ch] [ebp-Ch]

  PacketGuard::PacketGuard((PacketGuard *)v4);
  InterfacePacketBuf::clear(v4);
  InterfacePacketBuf::put_header(v4, 1, 74);
  InterfacePacketBuf::put_byte(v4, 1);
  for ( i = 0; i <= 3; ++i )
  {
    InterfacePacketBuf::put_byte(v4, *((_BYTE *)this + i + 516));
    InterfacePacketBuf::put_byte(v4, *((_BYTE *)this + i + 520));
    if ( *((_BYTE *)this + i + 520) == MASK_BYTE )
    {
      InterfacePacketBuf::put_byte(v4, 0);
    }
    else
    {
      index = InterfacePacketBuf::get_index(v4);
      InterfacePacketBuf::put_byte(v4, 1);
      InterfacePacketBuf::put_int(v4, 0);
      add_info = Inven_Item::get_add_info((CParty *)((char *)this + 122 * *((char *)this + i + 520) + 1904));
      InterfacePacketBuf::put_int(v4, add_info);
      if ( *(_DWORD *)((char *)this + 122 * *((char *)this + i + 520) + 1967) )
      {
        InterfacePacketBuf::put_int(v4, *(_DWORD *)((char *)this + 122 * *((char *)this + i + 520) + 1967));
        v2 = Inven_Item::get_add_info((CParty *)((char *)this + 122 * *((char *)this + i + 520) + 1965));
        InterfacePacketBuf::put_int(v4, v2);
        InterfacePacketBuf::put_byte(v4, &index, 2);
      }
    }
    InterfacePacketBuf::put_byte(v4, *((_BYTE *)this + i + 908));
  }
  InterfacePacketBuf::finalize(v4, 1);
  CParty::send_to_party((CUser **)this, (PacketGuard *)v4);
  PacketGuard::~PacketGuard(v4);
}

int __cdecl CParty::IsSelectCard(int id, char flag, int id2)
{
  int i; // [esp+10h] [ebp-4h]

  for ( i = 0; i <= 3; ++i )
  {
    if ( *(_BYTE *)(4 * a3 + a1 + i + 516) == a2 )
      return 1;
  }
  return 0;
}

bool __cdecl CParty::CompleteCardSelect(CParty *this)
{
  bool result; // al
  int i; // [esp+28h] [ebp-10h]
  int j; // [esp+2Ch] [ebp-Ch]

  for ( i = 0; ; ++i )
  {
    result = i <= 3;
    if ( i > 3 )
      break;
    if ( *((_BYTE *)this + i + 516) == MASK_BYTE )
    {
      for ( j = 0; j <= 3; ++j )
      {
        if ( (unsigned __int8)CParty::_checkValidUser((CUser **)this, j) == 1
          && (unsigned __int8)CParty::IsSelectCard((int)this, j, 0) != 1 )
        {
          *((_BYTE *)this + i + 516) = j;
          if ( *((_BYTE *)this + i + 516) == MASK_BYTE )
            LogManager::logFormat(
              (LogManager *)1,
              (int)"party.cpp",
              "void CParty::CompleteCardSelect()",
              (const char *)0x36F8,
              "INIT_CARD_NUM != dungeon_reward_card_[i]===>> dungeon_reward_card_[i] = %d, i = %d, z = %d",
              (const char *)*((char *)this + i + 516),
              i,
              j);
          break;
        }
      }
    }
  }
  return result;
}

int __cdecl CParty::IsCompleteCardSelectOfMembers(int id, int id2)
{
  int i; // [esp+8h] [ebp-8h]
  int j; // [esp+Ch] [ebp-4h]

  for ( i = 0; i <= 3; ++i )
  {
    if ( *(_DWORD *)(a1 + 24 * i + 120) )
    {
      if ( a2 == 2 )
      {
        for ( j = 0; j <= 1; ++j )
        {
          if ( *(_BYTE *)(4 * j + a1 + i + 516) == MASK_BYTE )
            return 0;
        }
      }
      else if ( *(_BYTE *)(4 * a2 + a1 + i + 516) == MASK_BYTE )
      {
        return 0;
      }
    }
  }
  return 1;
}

void __cdecl CParty::SendCardSelectComplete(CUser **this)
{
  PacketBuf *v1[5]; // [esp+14h] [ebp-14h] BYREF

  PacketGuard::PacketGuard((PacketGuard *)v1);
  InterfacePacketBuf::put_header(v1, 0, 67);
  InterfacePacketBuf::finalize(v1, 1);
  CParty::send_to_party(this, (PacketGuard *)v1);
  PacketGuard::~PacketGuard(v1);
}

int __cdecl CParty::isGuildParty(CUserCharacInfo **this)
{
  unsigned __int8 v2; // [esp+13h] [ebp-15h]
  int charac_guildkey; // [esp+14h] [ebp-14h]
  int id; // [esp+18h] [ebp-10h]
  int i; // [esp+1Ch] [ebp-Ch]

  v2 = 0;
  v4 = -1;
  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::_checkValidUser(this, i) == 1 )
    {
      charac_guildkey = CUserCharacInfo::get_charac_guildkey(*(this + 6 * i + 30));
      if ( !charac_guildkey )
        return 0;
      if ( charac_guildkey == v4 )
      {
        v2 = 1;
      }
      else
      {
        if ( v4 != -1 )
          return 0;
        v2 = 1;
      }
      v4 = charac_guildkey;
    }
  }
  return v2;
}

int __cdecl CParty::hasPCRoomPlayer(CUser **this)
{
  int i; // [esp+1Ch] [ebp-Ch]

  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::_checkValidUser(this, i) == 1
      && CUser::isAffectedPremium((int)*(this + 6 * i + 30), 12) )
    {
      return 1;
    }
  }
  return 0;
}

int __cdecl CParty::_getMemberLevelGap(CUserCharacInfo **this)
{
  int id; // [esp+1Ch] [ebp-1Ch] BYREF
  int id; // [esp+20h] [ebp-18h] BYREF
  int charac_level; // [esp+24h] [ebp-14h] BYREF
  int id; // [esp+28h] [ebp-10h] BYREF
  int i; // [esp+2Ch] [ebp-Ch]

  if ( *(this + 166) == (CUserCharacInfo *)-1 )
  {
    v3 = 99;
    v2 = 0;
    for ( i = 0; i <= 3; ++i )
    {
      if ( (unsigned __int8)CParty::_checkValidUser(this, i) == 1 )
      {
        charac_level = CUserCharacInfo::get_charac_level(*(this + 6 * i + 30));
        v3 = *std::min<int>(&v3, &charac_level);
        v5 = CUserCharacInfo::get_charac_level(*(this + 6 * i + 30));
        v2 = *std::max<int>(&v2, &v5);
      }
    }
    *(this + 166) = (CUserCharacInfo *)abs32(v2 - v3);
  }
  return (int)*(this + 166);
}

char *__cdecl CParty::_getMemberNames(CUserCharacInfo **this, char *s)
{
  const char *CurCharacName; // eax
  const char *v3; // eax
  const char *v4; // eax
  char src[34]; // [esp+1Ah] [ebp-2Eh] BYREF
  int i; // [esp+3Ch] [ebp-Ch]

  if ( *(this + 29) )
    CurCharacName = CUserCharacInfo::getCurCharacName(*(this + 29));
  else
    CurCharacName = "NoMember";
  sprintf(s, "\"%s\",", CurCharacName);
  for ( i = 0; i <= 3; ++i )
  {
    if ( i == 3 )
    {
      if ( (unsigned __int8)CParty::_checkValidUser(this, 3) )
        v3 = CUserCharacInfo::getCurCharacName(*(this + 6 * i + 30));
      else
        v3 = "N/A";
      sprintf(src, "\"%s\"", v3);
    }
    else
    {
      if ( (unsigned __int8)CParty::_checkValidUser(this, i) )
        v4 = CUserCharacInfo::getCurCharacName(*(this + 6 * i + 30));
      else
        v4 = "N/A";
      sprintf(src, "\"%s\",", v4);
    }
    strncat(s, src, MASK_BYTE);
  }
  s[255] = 0;
  return s;
}

int __cdecl CParty::_checkValidUser(CUser **this, int id)
{
  int id; // ebx
  unsigned int acc_id; // eax
  char flag; // bl
  int id[4]; // [esp+3Ch] [ebp-5Ch] BYREF
  int id[4]; // [esp+4Ch] [ebp-4Ch] BYREF
  int id[4]; // [esp+5Ch] [ebp-3Ch] BYREF
  int id[4]; // [esp+6Ch] [ebp-2Ch] BYREF
  char flag[4]; // [esp+7Ch] [ebp-1Ch]

  if ( *(this + 6 * a2 + 30) )
  {
    if ( (CUser *)CUser::GetUID(*(this + 6 * a2 + 30)) == *(this + 6 * a2 + 32)
      && CUserCharacInfo::getCurCharacR(*(this + 6 * a2 + 30)) )
    {
      return 1;
    }
    v3 = (int)*(this + 29);
    cMyTrace::cMyTrace((cMyTrace *)v6, "bool CParty::_checkValidUser(int)", 14287, 5);
    cMyTrace::operator()((int)v6, "[PARTYPOOL] m_pManager : %x, dungeon_idx : %u, m_iIndex : %u, cState : %d", v3);
    *(_DWORD *)v10 = 0;
    while ( *(int *)v10 <= 3 )
    {
      if ( *(this + 6 * *(_DWORD *)v10 + 30) )
      {
        CUserCharacInfo::getCurCharacNo(*(this + 6 * *(_DWORD *)v10 + 30));
        CUser::GetUID(*(this + 6 * *(_DWORD *)v10 + 30));
        CUser::get_state(*(this + 6 * *(_DWORD *)v10 + 30));
        cMyTrace::cMyTrace((cMyTrace *)v7, "bool CParty::_checkValidUser(int)", 14298, 5);
        cMyTrace::operator()((int)v7, "User[%d] %x, ch_state %d, uid : %d / %d, charac_no : %u", v10[0]);
      }
      else
      {
        cMyTrace::cMyTrace((cMyTrace *)v8, "bool CParty::_checkValidUser(int)", 14309, 5);
        cMyTrace::operator()((int)v8, "User[%d] NULL uid : %d", v10[0]);
      }
      ++*(_DWORD *)v10;
    }
    CUser::GetUID(*(this + 6 * a2 + 30));
    acc_id = CUser::get_acc_id(*(this + 6 * a2 + 30));
    v5 = NumberToString(acc_id, 0);
    cMyTrace::cMyTrace((cMyTrace *)v9, "bool CParty::_checkValidUser(int)", 14317, 5);
    cMyTrace::operator()((int)v9, "User missmatched in party (m_id: %s)(uid: %d)", v5);
    CUser::DisConnSig((pthread_mutex_t *)*(this + 6 * a2 + 30), 23, 0, 0);
    CParty::cMember::Init((CParty::cMember *)(this + 6 * a2 + 30));
  }
  return 0;
}

int __cdecl CParty::SendDungeonCharacInfo(CParty *this, CUser *user, bool a3)
{
  *((_DWORD *)a2 + 145421) = CSystemTime::getCurSec((CSystemTime *)GlobalData::s_systemTime_) - *((_DWORD *)this + 1559);
  *((_DWORD *)a2 + 145430) = CUserCharacInfo::get_charac_level(a2);
  if ( *((_DWORD *)a2 + 145429) )
    CParty::SendDungeonStatisticParty(this, a3, a2);
  if ( *((_DWORD *)a2 + 145429) )
    CParty::SendDungeonStatisticPartyJob(this, a3, a2);
  if ( *((_DWORD *)a2 + 145429) )
    CParty::SendDungeonStatisticPartyCharac(this, a3, a2);
  return 1;
}

int __cdecl CParty::SendDungeonStatisticParty(CParty *this, bool a2, CUser *user)
{
  CEnvironment *v3; // eax
  int ServerGroup; // eax
  std::string *ServerProxy; // eax
  char buf[10]; // [esp+20h] [ebp-48h] BYREF
  __int16 channel_no; // [esp+2Ah] [ebp-3Eh]
  int dungeon_index; // [esp+2Ch] [ebp-3Ch]
  char dungeon_diff; // [esp+30h] [ebp-38h]
  char flag; // [esp+31h] [ebp-37h]
  char flag; // [esp+32h] [ebp-36h]
  char flag; // [esp+33h] [ebp-35h]
  bool v14; // [esp+34h] [ebp-34h]
  char flag; // [esp+35h] [ebp-33h]
  int id; // [esp+36h] [ebp-32h]
  int id; // [esp+3Ah] [ebp-2Eh]
  int id; // [esp+3Eh] [ebp-2Ah]
  int id; // [esp+42h] [ebp-26h]
  int id; // [esp+46h] [ebp-22h]
  int id; // [esp+4Ah] [ebp-1Eh]
  int id; // [esp+4Eh] [ebp-1Ah]
  int id; // [esp+52h] [ebp-16h]
  int id; // [esp+56h] [ebp-12h]
  int id; // [esp+5Ah] [ebp-Eh]
  __int16 v26; // [esp+5Eh] [ebp-Ah]

  Packet_Dungeon_Statistic_Party::Packet_Dungeon_Statistic_Party((Packet_Dungeon_Statistic_Party *)buf);
  v3 = G_CEnvironment();
  channel_no = CEnvironment::get_channel_no(v3);
  dungeon_index = CBattle_Field::get_dungeon_index((CParty *)((char *)this + 2852));
  dungeon_diff = CBattle_Field::get_dungeon_diff((CParty *)((char *)this + 2852));
  v11 = *((_BYTE *)this + 853);
  v12 = *((_BYTE *)this + 854);
  v13 = *((_BYTE *)this + 855);
  v14 = a2;
  v15 = *((_DWORD *)a3 + 145429);
  v16 = *((_DWORD *)a3 + 145421);
  v17 = *((_DWORD *)a3 + 145422);
  v18 = *((_DWORD *)a3 + 145423);
  v19 = *((_DWORD *)a3 + 145424);
  v20 = *((_DWORD *)a3 + 145425);
  v21 = *((_DWORD *)a3 + 145426);
  v22 = *((_DWORD *)a3 + 145427);
  v23 = *((_DWORD *)a3 + 145428);
  v24 = *((_DWORD *)a3 + 145434);
  v25 = *((_DWORD *)a3 + 145433);
  v26 = *((_DWORD *)a3 + 145430);
  ServerGroup = CUser::GetServerGroup(a3);
  ServerProxy = (std::string *)CServerProxyMgr<CStatisticServerProxy>::GetServerProxy(
                                 GlobalData::s_statistic_proxy_mgr,
                                 ServerGroup);
  CStatisticServerProxy::SendPacket(ServerProxy, buf, SIZE_64_BYTES);
  return 1;
}

int __cdecl CParty::SendDungeonStatisticPartyJob(CParty *this, bool a2, CUser *user)
{
  CEnvironment *v3; // eax
  int ServerGroup; // eax
  std::string *ServerProxy; // eax
  char buf[10]; // [esp+21h] [ebp-27h] BYREF
  __int16 channel_no; // [esp+2Bh] [ebp-1Dh]
  int dungeon_index; // [esp+2Dh] [ebp-1Bh]
  char dungeon_diff; // [esp+31h] [ebp-17h]
  char flag; // [esp+32h] [ebp-16h]
  char flag; // [esp+33h] [ebp-15h]
  char flag; // [esp+34h] [ebp-14h]
  bool v14; // [esp+35h] [ebp-13h]
  char flag; // [esp+36h] [ebp-12h]
  int charac_job; // [esp+37h] [ebp-11h]
  char CurCharacGrowType; // [esp+3Bh] [ebp-Dh]
  int id; // [esp+3Ch] [ebp-Ch]

  Packet_Dungeon_Statistic_Party_Job::Packet_Dungeon_Statistic_Party_Job((Packet_Dungeon_Statistic_Party_Job *)buf);
  v3 = G_CEnvironment();
  channel_no = CEnvironment::get_channel_no(v3);
  dungeon_index = CBattle_Field::get_dungeon_index((CParty *)((char *)this + 2852));
  dungeon_diff = CBattle_Field::get_dungeon_diff((CParty *)((char *)this + 2852));
  v11 = *((_BYTE *)this + 853);
  v12 = *((_BYTE *)this + 854);
  v13 = *((_BYTE *)this + 855);
  v14 = a2;
  charac_job = CUserCharacInfo::get_charac_job(a3);
  CurCharacGrowType = CUserCharacInfo::getCurCharacGrowType(a3);
  v15 = *((_DWORD *)a3 + 145429);
  if ( a2 )
    v18 = *((unsigned __int16 *)this + 3102);
  else
    v18 = 0;
  ServerGroup = CUser::GetServerGroup(a3);
  ServerProxy = (std::string *)CServerProxyMgr<CStatisticServerProxy>::GetServerProxy(
                                 GlobalData::s_statistic_proxy_mgr,
                                 ServerGroup);
  CStatisticServerProxy::SendPacket(ServerProxy, buf, MASK_5BIT);
  return 1;
}

int __cdecl CParty::SendDungeonStatisticPartyCharac(CParty *this, bool a2, CUser *user)
{
  CEnvironment *v3; // eax
  int ServerGroup; // eax
  std::string *ServerProxy; // eax
  char buf[10]; // [esp+29h] [ebp-4Fh] BYREF
  __int16 channel_no; // [esp+33h] [ebp-45h]
  int dungeon_index; // [esp+35h] [ebp-43h]
  char dungeon_diff; // [esp+39h] [ebp-3Fh]
  char flag; // [esp+3Ah] [ebp-3Eh]
  bool v12; // [esp+3Bh] [ebp-3Dh]
  int charac_job; // [esp+3Ch] [ebp-3Ch]
  char CurCharacGrowType; // [esp+40h] [ebp-38h]
  char flag; // [esp+41h] [ebp-37h]
  int id; // [esp+42h] [ebp-36h]
  int id; // [esp+46h] [ebp-32h]
  int id; // [esp+4Ah] [ebp-2Eh]
  int id; // [esp+4Eh] [ebp-2Ah]
  int id; // [esp+52h] [ebp-26h]
  int id; // [esp+56h] [ebp-22h]
  int id; // [esp+5Ah] [ebp-1Eh]
  int id; // [esp+5Eh] [ebp-1Ah]
  int id; // [esp+62h] [ebp-16h]
  int id; // [esp+66h] [ebp-12h]
  __int16 v26; // [esp+6Ah] [ebp-Eh]
  int id; // [esp+6Ch] [ebp-Ch]

  Packet_Dungeon_Statistic_Party_Charac::Packet_Dungeon_Statistic_Party_Charac((Packet_Dungeon_Statistic_Party_Charac *)buf);
  v3 = G_CEnvironment();
  channel_no = CEnvironment::get_channel_no(v3);
  dungeon_index = CBattle_Field::get_dungeon_index((CParty *)((char *)this + 2852));
  dungeon_diff = CBattle_Field::get_dungeon_diff((CParty *)((char *)this + 2852));
  v11 = *((_BYTE *)this + 852);
  v12 = a2;
  charac_job = CUserCharacInfo::get_charac_job(a3);
  CurCharacGrowType = CUserCharacInfo::getCurCharacGrowType(a3);
  v15 = *((_DWORD *)a3 + 145429);
  v16 = *((_DWORD *)a3 + 145421);
  v17 = *((_DWORD *)a3 + 145422);
  v18 = *((_DWORD *)a3 + 145423);
  v19 = *((_DWORD *)a3 + 145424);
  v20 = *((_DWORD *)a3 + 145425);
  v21 = *((_DWORD *)a3 + 145426);
  v22 = *((_DWORD *)a3 + 145427);
  v23 = *((_DWORD *)a3 + 145428);
  v24 = *((_DWORD *)a3 + 145434);
  v25 = *((_DWORD *)a3 + 145433);
  v26 = *((_DWORD *)a3 + 145430);
  if ( a2 )
    v27 = *((unsigned __int16 *)this + 3102);
  else
    v27 = 0;
  ServerGroup = CUser::GetServerGroup(a3);
  ServerProxy = (std::string *)CServerProxyMgr<CStatisticServerProxy>::GetServerProxy(
                                 GlobalData::s_statistic_proxy_mgr,
                                 ServerGroup);
  CStatisticServerProxy::SendPacket(ServerProxy, buf, 0x47u);
  return 1;
}

    CParty::SendDungeonCharacInfo(this, (CUser *)a3, a4);
  }
  if ( *((_DWORD *)this + 822) != 1 )
  {

void __cdecl CParty::cal_hellparty_value(CParty *this, int id, int value, int count)
{
  float v4; // [esp+38h] [ebp-10h]

  if ( a3 && a4 >= 0 && a2 <= 4 )
  {
    v4 = *((float *)this + 854);
    if ( *(_BYTE *)(*((_DWORD *)this + 811) + 2204) != 1 )
      v4 = 0.0;
    *((float *)this + 707) = (long double)*((int *)this + 838)
                           * *((float *)this + a3 + 841)
                           * (*((float *)this + a2 + 845) + 1.0 + v4 + *((float *)this + a4 + 850))
                           + *((float *)this + 707);
    if ( (int)*((float *)this + 707) > *((_DWORD *)this + 835) )
      *((float *)this + 707) = (float)*((int *)this + 835);
    CParty::send_hellParty_value(this);
  }
}

int __cdecl CParty::get_rate_level_difference(CParty *this)
{
  int standard_level; // ebx
  int id; // [esp+1Ch] [ebp-Ch]

  if ( !*((_DWORD *)this + 811) )
    return 0;
  standard_level = CDungeon::get_standard_level(*((CDungeon **)this + 811));
  v3 = abs32(standard_level - CParty::getMemberMaxLevel((CUserCharacInfo **)this));
  if ( v3 <= 10 )
    return 1;
  if ( v3 > 19 )
    return 3;
  return 2;
}

int __cdecl CParty::OnKilledMonster(CParty *party, CUser *user, int id, int value, int count)
{
  bool v5; // al
  char *HackAnalyzer; // eax
  char *v7; // eax
  char *v9; // eax
  char *v10; // eax
  char *v11; // eax
  int CurrentMapInfo; // eax
  char *v14; // eax
  char flag; // [esp+27h] [ebp-21h]
  int dungeon_index; // [esp+28h] [ebp-20h]
  int i; // [esp+2Ch] [ebp-1Ch]
  CUser *user; // [esp+30h] [ebp-18h]
  int Party; // [esp+34h] [ebp-14h]
  int id; // [esp+38h] [ebp-10h]
  int id; // [esp+3Ch] [ebp-Ch]

  IsCollectDungeonStatistic();
  v5 = !CBattle_Field::IsClearDungeon((CParty *)((char *)a1 + 2852)) && !*(_BYTE *)(a5 + 2594);
  if ( v5
    && CTraceMobDieHack::addMonsterDamage(
         (CParty *)((char *)a1 + 528),
         *(unsigned __int16 *)(a5 + 15),
         a3,
         *(_DWORD *)(a5 + 2586)) > 2
    && a2 )
  {
    HackAnalyzer = CUser::getHackAnalyzer(a2);
    WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)HackAnalyzer, a2, 0xC9u, 1, 0, 0);
  }
  v16 = 1;
  dungeon_index = CBattle_Field::get_dungeon_index((CParty *)((char *)a1 + 2852));
  if ( dungeon_index == 62 || dungeon_index == 63 || dungeon_index == 64 )
    v16 = 0;
  if ( v16
    && !*(_BYTE *)(a5 + 2594)
    && *(char *)(a4 + 8) <= 3
    && (unsigned int)ActiveStaticInfo::getHpMax((ActiveStaticInfo *)(a4 + 56)) > *(_DWORD *)(a5 + 2586)
    && a3 != 10
    && a3 != 50001
    && a3 != 1034 )
  {
    for ( i = 0; i <= 3; ++i )
    {
      if ( (unsigned __int8)CParty::checkValidUser((CUser **)a1, i) == 1 )
      {
        user = (CUser *)CParty::get_user(a1, i);
        Party = CUser::GetParty(user);
        v21 = CUser::GetParty(a2);
        if ( Party )
        {
          if ( v21 && user )
          {
            if ( !*(_DWORD *)(Party + 3244) || !*(_DWORD *)(v21 + 3244) )
            {
              if ( user != a2 )
                goto LABEL_28;
LABEL_39:
              v11 = CUser::getHackAnalyzer(a2);
              WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)v11, a2, 0x70u, 1, 0, 0);
              continue;
            }
            if ( CDungeon::get_index(*(CDungeon **)(Party + 3244)) != 11006
              && CDungeon::get_index(*(CDungeon **)(v21 + 3244)) != 11006 )
            {
              if ( user != a2 )
              {
LABEL_28:
                v7 = CUser::getHackAnalyzer(user);
                WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)v7, user, 0x71u, 1, 0, 0);
                continue;
              }
              goto LABEL_39;
            }
            if ( user == a2 )
            {
              v10 = CUser::getHackAnalyzer(a2);
              WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)v10, a2, 0x70u, 1, 2u, 0);
            }
            else
            {
              v9 = CUser::getHackAnalyzer(user);
              WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)v9, user, 0x71u, 1, 2u, 0);
            }
          }
        }
      }
    }
  }
  if ( !*(_BYTE *)(a5 + 2594) && *(char *)(a4 + 8) <= 3 )
  {
    CurrentMapInfo = CBattle_Field::GetCurrentMapInfo((CParty *)((char *)a1 + 2852));
    if ( !stMapMonsterKillChecker_t::getUseSkillMaterialTime((stMapMonsterKillChecker_t *)(CurrentMapInfo + 76)) )
    {
      if ( (unsigned int)(4 * ActiveStaticInfo::getHpMax((ActiveStaticInfo *)(a4 + 56))) >= *(_DWORD *)(a5 + 2586) )
        v22 = (unsigned int)(3 * ActiveStaticInfo::getHpMax((ActiveStaticInfo *)(a4 + 56))) >= *(_DWORD *)(a5 + 2586)
            ? (unsigned int)(2 * ActiveStaticInfo::getHpMax((ActiveStaticInfo *)(a4 + 56))) < *(_DWORD *)(a5 + 2586)
            : 2;
      else
        v22 = 3;
      if ( v22 > 0 )
      {
        v14 = CUser::getHackAnalyzer(a2);
        WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)v14, a2, 0x86u, v22, 0, 0);
      }
    }
  }
  if ( (unsigned __int8)CBattle_Field::check_grid_clear((CParty *)((char *)a1 + 2852)) )
    CBattle_Field::onClearMap((CParty *)((char *)a1 + 2852), 1);
  return 1;
}

int __cdecl CParty::OnUsedSkill(CParty *this, CUser *user, int id)
{
  int charac_job; // eax
  int i; // [esp+1Ch] [ebp-Ch]

  IsCollectDungeonStatistic();
  for ( i = 0; i <= 3; ++i )
  {
    if ( *((CUser **)this + 6 * i + 30) == a2 )
    {
      ++*((_DWORD *)this + i + 703);
      charac_job = CUserCharacInfo::get_charac_job(a2);
      if ( CParty::isCheckUseSkill(this, charac_job, a3) )
        ++*((_DWORD *)this + i + 1705);
    }
  }
  return 1;
}

int __cdecl CParty::getMemberMinLevel(CUserCharacInfo **this)
{
  int id; // [esp+14h] [ebp-14h] BYREF
  int charac_level; // [esp+18h] [ebp-10h] BYREF
  int i; // [esp+1Ch] [ebp-Ch]

  v2 = MAX_INT32;
  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::_checkValidUser(this, i) == 1 )
    {
      charac_level = CUserCharacInfo::get_charac_level(*(this + 6 * i + 30));
      v2 = *std::min<int>(&v2, &charac_level);
    }
  }
  return v2;
}

int __cdecl CParty::getMemberMaxLevel(CUserCharacInfo **this)
{
  int id; // [esp+14h] [ebp-14h] BYREF
  int charac_level; // [esp+18h] [ebp-10h] BYREF
  int i; // [esp+1Ch] [ebp-Ch]

  v2 = 0;
  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::_checkValidUser(this, i) == 1 )
    {
      charac_level = CUserCharacInfo::get_charac_level(*(this + 6 * i + 30));
      v2 = *std::max<int>(&v2, &charac_level);
    }
  }
  return v2;
}

int __cdecl CParty::getMemberMaxLevelBeforeDungeon(CUserCharacInfo **this)
{
  int id; // [esp+14h] [ebp-14h] BYREF
  int charac_level_before_dungeon; // [esp+18h] [ebp-10h] BYREF
  int i; // [esp+1Ch] [ebp-Ch]

  v2 = 0;
  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::_checkValidUser(this, i) == 1 )
    {
      charac_level_before_dungeon = CUserCharacInfo::get_charac_level_before_dungeon(*(this + 6 * i + 30));
      v2 = *std::max<int>(&v2, &charac_level_before_dungeon);
    }
  }
  return v2;
}

bool __cdecl CParty::GetLuckPoint(CUser **this, CLuckPoint *a2, bool a3)
{
  bool result; // al
  int i; // [esp+2Ch] [ebp-Ch]

  result = a3;
  if ( a2 )
  {
    for ( i = 0; ; ++i )
    {
      result = i <= 3;
      if ( i > 3 )
        break;
      if ( (unsigned __int8)CParty::_checkValidUser(this, i) == 1 && (!a3 || *((_BYTE *)this + i + 896) == 1) )
        CLuckPoint::AddUser(a2, *(this + 6 * i + 30));
    }
  }
  return result;
}

int __cdecl CParty::IsExistCharacter(CUser **this, int id)
{
  int i; // [esp+1Ch] [ebp-Ch]

  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::_checkValidUser(this, i) == 1
      && CUserCharacInfo::getCurCharacNo(*(this + 6 * i + 30)) == a2 )
    {
      return 1;
    }
  }
  return 0;
}

int __cdecl CParty::GetPartyType(CUserCharacInfo **this)
{
  int connect_upper_member; // eax
  int charac_guildkey; // [esp+1Ch] [ebp-1Ch]
  char flag; // [esp+23h] [ebp-15h]
  int id; // [esp+24h] [ebp-14h]
  int id; // [esp+28h] [ebp-10h]
  int i; // [esp+2Ch] [ebp-Ch]

  charac_guildkey = 0;
  v4 = 0;
  v5 = 0;
  v6 = 0;
  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::_checkValidUser(this, i) == 1 )
    {
      if ( CUserCharacInfo::get_charac_guildkey(*(this + 6 * i + 30)) )
      {
        if ( charac_guildkey )
        {
          if ( CUserCharacInfo::get_charac_guildkey(*(this + 6 * i + 30)) != charac_guildkey )
            v4 = 0;
        }
        else
        {
          charac_guildkey = CUserCharacInfo::get_charac_guildkey(*(this + 6 * i + 30));
          v4 = 1;
        }
      }
      connect_upper_member = CUserCharacInfo::get_connect_upper_member(*(this + 6 * i + 30));
      if ( (unsigned __int8)CParty::IsExistCharacter(this, connect_upper_member) )
        ++v5;
      ++v6;
    }
  }
  if ( v4 )
    return 0;
  if ( v5 <= 0 )
    return 2;
  if ( v6 - 1 == v5 )
    return 1;
  return 4;
}

int __cdecl CParty::IsExistInvisible(CUser **this)
{
  int i; // [esp+1Ch] [ebp-Ch]

  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::_checkValidUser(this, i) && !CUserCharacInfo::IsCurCharacVisible(*(this + 6 * i + 30)) )
      return 1;
  }
  return 0;
}

int __cdecl CParty::CheckQuestConnection(CUser **a1, unsigned int id, int id2)
{
  char *v3; // eax
  char *CurCharacQuestR; // eax
  int i; // [esp+1Ch] [ebp-Ch]

  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::_checkValidUser(a1, i) )
    {
      if ( a3 )
      {
        if ( a3 == 1 )
        {
          CurCharacQuestR = CUser::getCurCharacQuestR(a1[6 * i + 30]);
          if ( (unsigned __int8)UserQuest::isClearQuest((UserQuest *)CurCharacQuestR, a2) )
            return 1;
        }
      }
      else
      {
        v3 = CUser::getCurCharacQuestR(a1[6 * i + 30]);
        if ( (unsigned __int8)UserQuest::IsDoingQuest((UserQuest *)v3, a2) )
          return 1;
      }
    }
  }
  return 0;
}

    CParty::ClearDungeon(a1, this);
  return v4;

int __cdecl CParty::HasNoQuestGoWithApcInParty(CUser **this, const Quest *a2)
{
  int id; // ebx
  char *CurCharacQuestR; // eax
  int i; // [esp+1Ch] [ebp-Ch]

  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::_checkValidUser(this, i) )
    {
      if ( CUser::get_state(*(this + 6 * i + 30)) == 5 || CUser::get_state(*(this + 6 * i + 30)) == 10 )
      {
        v2 = *((_DWORD *)a2 + 1);
        CurCharacQuestR = CUser::getCurCharacQuestR(*(this + 6 * i + 30));
        if ( (unsigned __int8)UserQuest::IsDoingQuest((UserQuest *)CurCharacQuestR, v2) )
        {
          if ( *((_DWORD *)a2 + 31) == 3 && !*((_DWORD *)a2 + 32) )
            return 0;
        }
      }
    }
  }
  return 1;
}

char __cdecl CParty::enumPartyMember(CParty *this, bool (*a2)(CUser *))
{
  int id; // eax
  int i; // [esp+1Ch] [ebp-Ch]

  for ( i = 0; ; ++i )
  {
    LOBYTE(v2) = i <= 3;
    if ( i > 3 )
      break;
    if ( (unsigned __int8)CParty::_checkValidUser((CUser **)this, i) )
    {
      v2 = ((int (__cdecl *)(_DWORD))a2)(*((_DWORD *)this + 6 * i + 30)) ^ 1;
      if ( (_BYTE)v2 )
        break;
    }
  }
  return v2;
}

int __cdecl CParty::isMyMember(CUser **this, unsigned int id)
{
  int i; // [esp+1Ch] [ebp-Ch]

  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::_checkValidUser(this, i)
      && (unsigned __int16)CUser::get_unique_id(*(this + 6 * i + 30)) == a2 )
    {
      return 1;
    }
  }
  return 0;
}

__int64 __cdecl CParty::GetMapPlayingTime(CParty *this)
{
  *(_QWORD *)((char *)this + 820) = (unsigned int)CSystemTime::getCurTickCount((CSystemTime *)GlobalData::s_systemTime_)
                                  - *(_QWORD *)((char *)this + 836)
                                  - *(_QWORD *)((char *)this + 828);
  return *(_QWORD *)((char *)this + 820);
}

int __cdecl CParty::existSamePCRoomUser(CUser **this, unsigned int id)
{
  int id; // [esp+18h] [ebp-10h]
  int i; // [esp+1Ch] [ebp-Ch]

  v4 = 0;
  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::_checkValidUser(this, i) )
    {
      if ( CUser::GetPCRoomNo(*(this + 6 * i + 30)) == a2 && CUser::GetPCRoomNo(*(this + 6 * i + 30)) && ++v4 > 1 )
        return 1;
    }
  }
  return 0;
}

int __cdecl CParty::isSamePCRoomParty(CUser **this)
{
  int id; // [esp+14h] [ebp-14h]
  int i; // [esp+18h] [ebp-10h]
  int PCRoomNo; // [esp+1Ch] [ebp-Ch]

  v2 = 0;
  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::_checkValidUser(this, i) )
    {
      PCRoomNo = CUser::GetPCRoomNo(*(this + 6 * i + 30));
      if ( !PCRoomNo )
        return 0;
      if ( v2 )
      {
        if ( v2 != PCRoomNo )
          return 0;
      }
      else
      {
        v2 = PCRoomNo;
      }
    }
  }
  return 1;
}

void __cdecl CParty::sub_hellparty_value(CParty *this)
{
  float v1; // [esp+2Ch] [ebp-Ch]

  v1 = *((float *)this + 707) * *((float *)this + 839);
  if ( (int)v1 < *((_DWORD *)this + 836) )
    v1 = (float)*((int *)this + 836);
  *((float *)this + 707) = v1;
  CParty::send_hellParty_value(this);
}

void __cdecl CParty::send_hellParty_value(CParty *this)
{
  PacketBuf *v1[3]; // [esp+20h] [ebp-18h] BYREF
  int id; // [esp+2Ch] [ebp-Ch]

  PacketGuard::PacketGuard((PacketGuard *)v1);
  InterfacePacketBuf::put_header(v1, 0, 207);
  v2 = (int)(*((float *)this + 707) * 100.0 / (long double)*((int *)this + 835));
  InterfacePacketBuf::put_int(v1, v2);
  InterfacePacketBuf::finalize(v1, 1);
  CParty::send_to_party((CUser **)this, (PacketGuard *)v1);
  PacketGuard::~PacketGuard(v1);
}

  if ( !CParty::gen_timer_key((int)a2, a3) )
    return 0;

        if ( CParty::get_user((CParty *)a2, i) && (unsigned __int8)CParty::checkValidUser(a2, i) == 1 )
        {
          RewardItem = CBattle_Field::CBloodClearRewardData::getRewardItem(
                         (CBattle_Field::CBloodClearRewardData *)(a2 + 887),
                         3);
          RewardItemCount = CBattle_Field::CBloodClearRewardData::getRewardItemCount(
                              (CBattle_Field::CBloodClearRewardData *)(a2 + 887),
                              3);
          v42 = (Inven_Item *)CBattle_Field::CBloodClearRewardData::getRewardItem(
                                (CBattle_Field::CBloodClearRewardData *)(a2 + 887),
                                2);
          v43 = CBattle_Field::CBloodClearRewardData::getRewardItemCount(
                  (CBattle_Field::CBloodClearRewardData *)(a2 + 887),
                  2);
          v44 = (Inven_Item *)CBattle_Field::CBloodClearRewardData::getRewardItem(
                                (CBattle_Field::CBloodClearRewardData *)(a2 + 887),
                                1);
          v45 = CBattle_Field::CBloodClearRewardData::getRewardItemCount(
                  (CBattle_Field::CBloodClearRewardData *)(a2 + 887),
                  1);
          v6 = CBattle_Field::CBloodClearRewardData::getRewardItem(
                 (CBattle_Field::CBloodClearRewardData *)(a2 + 887),
                 0);
          v7 = CBattle_Field::CBloodClearRewardData::getRewardItemCount(
                 (CBattle_Field::CBloodClearRewardData *)(a2 + 887),
                 0);
          CurrentBloodRound = CBattle_Field::CBloodClearRewardData::getCurrentBloodRound((CBattle_Field::CBloodClearRewardData *)(a2 + 887));
          CParty::BloodClearRewardPacket(
            a2,
            i,
            CurrentBloodRound,
            v7,
            (const Inven_Item *)v6,
            v45,
            v44,
            v43,
            v42,
            RewardItemCount,
            (const Inven_Item *)RewardItem);
        }

      PartyIndex = CParty::GetPartyIndex((CParty *)a2);
      v11 = G_TimerQueue();

        if ( CParty::get_user((CParty *)a2, j) && (unsigned __int8)CParty::checkValidUser(a2, j) == 1 )
        {
          for ( k = 0; ; ++k )
          {
            v34 = CBattle_Field::CBloodClearRewardData::getRewardItemCount(
                    (CBattle_Field::CBloodClearRewardData *)(a2 + 887),
                    j);
            if ( v34 <= k )
              break;
            v55 = CBattle_Field::CBloodClearRewardData::getRewardItem(
                    (CBattle_Field::CBloodClearRewardData *)(a2 + 887),
                    j);
            if ( *(_DWORD *)&v55[61 * k + 2] != -1 )
            {
              inserted = -1;
              if ( *(_DWORD *)&v55[61 * k + 2] )
              {
                v22 = &v55[61 * k];
                CurCharacInvenW = (int *)CUserCharacInfo::getCurCharacInvenW(a2[6 * j + 30]);
                inserted = CInventory::insertItemIntoInventory(
                             CurCharacInvenW,
                             *(_QWORD *)v22,
                             *((_DWORD *)v22 + 2),
                             *((_DWORD *)v22 + 3),
                             *((_DWORD *)v22 + 4),
                             *((_DWORD *)v22 + 5),
                             *((_DWORD *)v22 + 6),
                             *((_DWORD *)v22 + 7),
                             *((_DWORD *)v22 + 8),
                             *((_DWORD *)v22 + 9),
                             *((_DWORD *)v22 + 10),
                             *((_DWORD *)v22 + 11),
                             *((_DWORD *)v22 + 12),
                             *((_DWORD *)v22 + 13),
                             *((_DWORD *)v22 + 14),
                             v22[60],
                             11,
                             1);
              }
              else
              {
                add_info = Inven_Item::get_add_info((Inven_Item *)&v55[61 * k]);
                v14 = CUserCharacInfo::getCurCharacInvenW(a2[6 * j + 30]);
                v57 = CInventory::gain_money(v14, add_info, (char *)5, 1, 0);
                v15 = Inven_Item::get_add_info((Inven_Item *)&v55[61 * k]);
                if ( v15 > v57 )
                {
                  v16 = v57;
                  v17 = Inven_Item::get_add_info((Inven_Item *)&v55[61 * k]);
                  a1 = (CParty *)a2;
                  CUser::SendMoneyFullReason(a2[6 * j + 30], 0, v17, v16);
                }
                inserted = 0;
                if ( *((_BYTE *)a2[811] + 2207) == 1 )
                {
                  a1 = (CParty *)v57;
                  v18 = a2[6 * j + 30];
                  InstanceCirculationStatistic = GetInstanceCirculationStatistic();
                  CCirculationStatistic::AddCirculationStatistic((int)InstanceCirculationStatistic, 0xBu, v18, (int)a1);
                }
                else if ( *((_BYTE *)a2[811] + 2207) == 2 )
                {
                  a1 = (CParty *)v57;
                  v20 = a2[6 * j + 30];
                  v21 = GetInstanceCirculationStatistic();
                  CCirculationStatistic::AddCirculationStatistic((int)v21, 0xCu, v20, (int)a1);
                }
              }
              if ( inserted < 0 )
              {
                if ( *(_DWORD *)&v55[61 * k + 2] == 1251 || *(_DWORD *)&v55[61 * k + 2] == 1252 )
                {
                  v28 = *(_DWORD *)&v55[61 * k + 2];
                  v29 = (CItemList **)G_CDataManager();
                  if ( CDataManager::find_item(v29, v28) )
                  {
                    String = RDARScriptStringManager::findString(
                               (RDARScriptStringManager *)&g_scriptStringManager_,
                               (char *)4,
                               "game_server_msg_153",
                               0);
                    strncpy(dest, String, SIZE_20_BYTES);
                    v31 = RDARScriptStringManager::findString(
                            (RDARScriptStringManager *)&g_scriptStringManager_,
                            (char *)4,
                            "game_server_msg_154",
                            0);
                    strncpy(s, v31, MASK_BYTE);
                    ServerGroup = CUser::GetServerGroup(a2[6 * j + 30]);
                    a1 = (CParty *)strlen(s);
                    CurCharacNo = CUserCharacInfo::getCurCharacNo(a2[6 * j + 30]);
                    WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
                      dest,
                      (Inven_Item *)&v55[61 * k],
                      0,
                      CurCharacNo,
                      s,
                      (int)a1,
                      8,
                      ServerGroup,
                      0,
                      0);
                  }
                }
              }
              else
              {
                PacketGuard::PacketGuard((PacketGuard *)v48);
                InterfacePacketBuf::put_header(v48, 0, 14);
                ItemSpace = Inven_Item::GetItemSpace((Inven_Item *)&v55[61 * k]);
                InterfacePacketBuf::put_byte(v48, ItemSpace);
                InterfacePacketBuf::put_short(v48, 1);
                v25 = Inven_Item::GetItemSpace((Inven_Item *)&v55[61 * k]);
                InvenTypeFromItemSpace = GetInvenTypeFromItemSpace(v25);
                CurCharacInvenR = (CInventory *)CUserCharacInfo::getCurCharacInvenR(a2[6 * j + 30]);
                CInventory::MakeItemPacket(CurCharacInvenR, InvenTypeFromItemSpace, inserted, v48);
                InterfacePacketBuf::finalize(v48, 1);
                CUser::Send(a2[6 * j + 30], (PacketGuard *)v48);
                PacketGuard::~PacketGuard(v48);
              }
            }
          }
          v50 = 0;
          RewardExp = (char *)CBattle_Field::CBloodClearRewardData::getRewardExp(
                                (CBattle_Field::CBloodClearRewardData *)(a2 + 887),
                                j);
          if ( (unsigned int)RewardExp > 0xF4240 )
          {
            LogManager::logFormat(
              (LogManager *)1,
              (int)"party.cpp",
              "bool CParty::onBloodRewardTimer(TIMER_MESSAGE)",
              (const char *)0x3BD4,
              "BLOOD_LOG : EROOR BLOOD CLEAR REWARD EXP!! (%u)\n",
              RewardExp);
            RewardExp = (char *)1000000;
          }
          v49 = 0;
          if ( (unsigned __int8)CUser::gain_exp_sp((int)a1, a2[6 * j + 30], (signed int)RewardExp, &v50, &v49, 2, 0, 0) )
            CUser::SendNotiPacket((int)a1, a2[6 * j + 30], 2, 2, 1);
        }

      v35 = CParty::gen_timer_key((int)a2, 37);
      v36 = CParty::GetPartyIndex((CParty *)a2);

      v38 = CParty::checkInoutConditionDungeon(v36, (CParty *)a2);
      CParty::sendInoutConditionDungeon(a2, v38);

      if ( (unsigned __int8)CParty::GetEPLPState((CParty *)a2) != 2 )
        return 0;

      if ( (unsigned __int8)CParty::GetSelectedEPLPCmd((CParty *)a2) == 3 )
        return 0;

      CParty::finishBloodDungeon((int)a1, (CParty *)a2);
      return 1;

      if ( CParty::get_member_count(a2) <= 0 )
        return 0;

      if ( (unsigned __int8)CParty::check_allmember_die(a2) )
      {
        if ( !(unsigned __int8)CBattle_Field::check_grid_clear((CBattle_Field *)(a2 + 713))
          || !CBattle_Field::check_end_point((CBattle_Field *)(a2 + 713))
          || !(unsigned __int8)CBattle_Field::CBloodClearRewardData::isBloodClear((CBattle_Field::CBloodClearRewardData *)(a2 + 887)) )
        {
          CParty::beginBloodClearProcess((int)a1, (CParty *)a2, 0);
          return 1;
        }
      }

      CParty::beginBloodClearProcess((int)a1, (CParty *)a2, 1);
      break;

        if ( CParty::get_user(this, i) && (unsigned __int8)CParty::checkValidUser((CUser **)this, i) == 1 )
        {
          user = (CUser *)CParty::get_user(this, i);
          CUser::setChattingMessageCount(user, 0);
          CParty::onBloodClear(a1, this, a3, i);
          CUserCharacInfo::set_charac_party_bonus_exp(*((CUserCharacInfo **)this + 6 * i + 30), 0);
          CUserCharacInfo::set_charac_member_bonus_exp(*((CUserCharacInfo **)this + 6 * i + 30), 0);
          CUserCharacInfo::set_charac_fatigue_buf_bonus_exp(*((CUserCharacInfo **)this + 6 * i + 30), 0);
          CUserCharacInfo::set_charac_seria_buf_bonus_exp(*((CUserCharacInfo **)this + 6 * i + 30), 0);
          v35 = 0;
          dungeon_index = CBattle_Field::get_dungeon_index((CParty *)((char *)this + 2852));
          CParty::checkBloodRenewMyRecord((CUserCharacInfo **)this, i, PlayTime, BloodMaxRound, dungeon_index);
          v8 = 10 * BloodMaxRound;
          v36 = v8 / CBattle_Field::getBloodMaxRound((CParty *)((char *)this + 2852));
          if ( !v36 )
            v36 = 1;
          v27 = 10;
          v9 = *std::min<int>(&v27, &BloodMaxRound);
          charac_level = CUserCharacInfo::get_charac_level(*((CUserCharacInfo **)this + 6 * i + 30));
          CParty::generateBloodRewardItem((CUser **)this, i, v36, charac_level, v9, a3);
          v25 = 0;
          v11 = CUserCharacInfo::get_charac_level(*((CUserCharacInfo **)this + 6 * i + 30));
          v12 = G_CDataManager();
          CDataManager::get_mob_reward(v12, v11, &v25);
          QuickParty::RewardData_Param::RewardData_Param((QuickParty::RewardData_Param *)v24, this);
          QuickParty::RewardData_Result::RewardData_Result((QuickParty::RewardData_Result *)v23);
          v23[0] = v25;
          CParty::set_random_reward(this, 0, v24[0], v24[1], (int)v23);
          v25 = v23[0];
          v13 = G_CDataManager();
          v37 = std::vector<float>::size((_DWORD *)v13 + 6233);
          CharacExpandData = (CCharacBloodDungeon *)CUser::GetCharacExpandData(*((_DWORD *)this + 6 * i + 30), 1u);
          EnterCount = CCharacBloodDungeon::GetEnterCount(CharacExpandData);
          v28 = v37 - 1;
          v15 = std::min<int>(&v28, &EnterCount);
          CParty::generateBloodRewardExp((CDungeon **)this, i, BloodMaxRound, v25, *v15);
          RewardExp = (char *)CBattle_Field::CBloodClearRewardData::getRewardExp((CParty *)((char *)this + 3548), i);
          if ( (unsigned int)RewardExp > 0xF4240 )
          {
            LogManager::logFormat(
              (LogManager *)1,
              (int)"party.cpp",
              "void CParty::beginBloodClearProcess(bool)",
              (const char *)0x3C79,
              "BLOOD_LOG : EROOR BLOOD CLEAR REWARD EXP(%u)\n",
              RewardExp);
            RewardExp = (char *)1000000;
            CBattle_Field::CBloodClearRewardData::setRewardExp((CParty *)((char *)this + 3548), i, 0xF4240u);
          }
          if ( !BloodMaxRound )
          {
            PlayTime = 0;
            BloodMaxRound = 0;
          }
          BloodBestTimeRecord = CUserCharacInfo::getBloodBestTimeRecord(*((CUserCharacInfo **)this + 6 * i + 30));
          BloodBestRoundRecord = CUserCharacInfo::getBloodBestRoundRecord(*((CUserCharacInfo **)this + 6 * i + 30));
          if ( BloodBestRoundRecord == -1 )
          {
            BloodBestTimeRecord = PlayTime;
            BloodBestRoundRecord = BloodMaxRound;
          }
          PacketGuard::PacketGuard((PacketGuard *)v21);
          InterfacePacketBuf::put_header(v21, 0, 208);
          InterfacePacketBuf::put_int(v21, PlayTime);
          InterfacePacketBuf::put_int(v21, BloodMaxRound);
          InterfacePacketBuf::put_int(v21, BloodBestTimeRecord);
          InterfacePacketBuf::put_int(v21, BloodBestRoundRecord);
          v16 = CBattle_Field::getBloodMaxRound((CParty *)((char *)this + 2852));
          InterfacePacketBuf::put_int(v21, v16);
          v17 = CBattle_Field::CBloodClearRewardData::getRewardExp((CParty *)((char *)this + 3548), i);
          InterfacePacketBuf::put_int(v21, v17);
          InterfacePacketBuf::finalize(v21, 1);
          CUser::Send(*((CUser **)this + 6 * i + 30), (PacketGuard *)v21);
          a1 = CParty::gen_timer_key((int)this, 35);
          PartyIndex = CParty::GetPartyIndex(this);
          v19 = G_TimerQueue();
          TimerQueue::InsertTimer(a1, v19, 1, PartyIndex, 35, 8, a1, 35);
          PacketGuard::~PacketGuard(v21);
        }

    return CParty::OnFailedDungeonClear(a1, this, *((CUser ***)this + 6 * a4 + 30));
  MapPlayingTime = CParty::GetMapPlayingTime(this);

  return CParty::OnSuccessedDungeonClear((int)this, this, *((CUser ***)this + 6 * a4 + 30));
}

//----- (085B7DB6) --------------------------------------------------------
bool __cdecl CParty::historyBloodRoundClear(
        CUserCharacInfo **this,
        int id,
        unsigned int id,
        unsigned int id,
        const char *a5)
{

  CParty::getMemberNames(this, v7);
  for ( i = 0; ; ++i )

    if ( CParty::get_user((CParty *)this, i) && (unsigned __int8)CParty::checkValidUser(this, i) == 1 )
      cUserHistoryLog::BloodRoundClear((CUser **)*(this + 6 * i + 30) + CUSER_OFFSET_HISTORY_LOG_ALT, a2, a3, a4, a5, v7);

CBattle_Field::CBloodClearRewardData *__cdecl CParty::generateBloodRewardExp(
        CDungeon **this,

  user = (CUserCharacInfo *)CParty::get_user((CParty *)this, a2);
  v36 = 0;

      "void CParty::generateBloodRewardExp(int, int, unsigned int, short int)",
      (const char *)0x3CEC,

  AvatarBonus = CParty::getAvatarBonus(this);
  CreatureBonus = CParty::getCreatureBonus((CParty *)this, user);

  v34 = CParty::CalcurateManageBonus((int)this, 0, user, (int)v36);
  ClearRewardBonusExp = CParty::getClearRewardBonusExp(

      "void CParty::generateBloodRewardExp(int, int, unsigned int, short int)",
      (const char *)0x3D12,

      "void CParty::generateBloodRewardExp(int, int, unsigned int, short int)",
      (const char *)0x3D1C,

void __cdecl CParty::generateBloodRewardItem(CUser **this, int count, int count2, char flag, unsigned int count3, bool a6)
{
  CDataManager *v6; // eax
  int count; // eax
  int count; // eax
  int count; // eax
  int count; // eax
  CDataManager *v11; // eax
  CDataManager *v12; // eax
  int ItemGenerator; // eax
  int Generator; // eax
  int count; // eax
  int count; // eax
  int count; // ebx
  CDataManager *v18; // eax
  int grade; // ebx
  int rarity; // esi
  CDataManager *v21; // eax
  CDataManager *v22; // eax
  int count; // eax
  int count; // [esp+40h] [ebp-1B8h]
  void *v25[13]; // [esp+64h] [ebp-194h] BYREF
  char flag; // [esp+98h] [ebp-160h]
  char flag; // [esp+99h] [ebp-15Fh]
  char flag; // [esp+9Ah] [ebp-15Eh]
  int count; // [esp+9Ch] [ebp-15Ch]
  int count; // [esp+A0h] [ebp-158h]
  int count; // [esp+A4h] [ebp-154h]
  _BYTE v32[2]; // [esp+AEh] [ebp-14Ah] BYREF
  int count; // [esp+B0h] [ebp-148h]
  __int64 v34; // [esp+EBh] [ebp-10Dh]
  int count; // [esp+F3h] [ebp-105h]
  int count; // [esp+F7h] [ebp-101h]
  int count; // [esp+FBh] [ebp-FDh]
  int count; // [esp+FFh] [ebp-F9h]
  int count; // [esp+103h] [ebp-F5h]
  int count; // [esp+107h] [ebp-F1h]
  int count; // [esp+10Bh] [ebp-EDh]
  int count; // [esp+10Fh] [ebp-E9h]
  int count; // [esp+113h] [ebp-E5h]
  int count; // [esp+117h] [ebp-E1h]
  int count; // [esp+11Bh] [ebp-DDh]
  int count; // [esp+11Fh] [ebp-D9h]
  int count; // [esp+123h] [ebp-D5h]
  char flag; // [esp+127h] [ebp-D1h]
  void *v49[12]; // [esp+128h] [ebp-D0h] BYREF
  void *v50[3]; // [esp+158h] [ebp-A0h] BYREF
  int count; // [esp+164h] [ebp-94h] BYREF
  int count[7]; // [esp+188h] [ebp-70h] BYREF
  int count; // [esp+1A4h] [ebp-54h] BYREF
  int count; // [esp+1A8h] [ebp-50h]
  int count; // [esp+1ACh] [ebp-4Ch] BYREF
  int count; // [esp+1B0h] [ebp-48h] BYREF
  int count; // [esp+1B4h] [ebp-44h] BYREF
  int count; // [esp+1B8h] [ebp-40h] BYREF
  int count; // [esp+1BCh] [ebp-3Ch] BYREF
  int i; // [esp+1C0h] [ebp-38h]
  unsigned int rand_int; // [esp+1C4h] [ebp-34h]
  unsigned int count; // [esp+1C8h] [ebp-30h]
  unsigned int count; // [esp+1CCh] [ebp-2Ch]
  int index; // [esp+1D0h] [ebp-28h]
  CItem *item; // [esp+1D4h] [ebp-24h]
  int UltimateRewardItem; // [esp+1D8h] [ebp-20h]

  CBattle_Field::CBloodClearRewardData::setRewardItem((CBattle_Field::CBloodClearRewardData *)(this + 887), a2, a3);
  for ( i = 0; i < a3; ++i )
  {
    WongWork::stGenerateRefData_t::stGenerateRefData_t((WongWork::stGenerateRefData_t *)v25);
    WongWork::stGenerateResult_t::stGenerateResult_t((WongWork::stGenerateResult_t *)v50);
    CParty::GetLuckPoint(this, (CLuckPoint *)&v51, 0);
    rand_int = get_rand_int(STATIC_POOL_SIZE_1000);
    v62 = a5 * *((_DWORD *)G_CDataManager() + 6229);
    v63 = a5 * *((_DWORD *)G_CDataManager() + 6230);
    if ( rand_int >= v62 )
    {
      if ( rand_int < v63 )
      {
        v27 = 0;
        v26 = a4;
        v29 = 1065353216;
        v31 = (int)*(this + 811);
        v30 = 0;
        v12 = G_CDataManager();
        ItemGenerator = CDataManager::getItemGenerator(v12);
        Generator = WongWork::CItemGeneratorMgr::getGenerator(ItemGenerator, 2);
        (*(void (__cdecl **)(int, void **, void **))(*(_DWORD *)Generator + 8))(Generator, v25, v50);
        if ( !std::vector<Inven_Item>::empty(v50) )
        {
          std::vector<Inven_Item>::begin(&v57, v50);
          v15 = __gnu_cxx::__normal_iterator<Inven_Item *,std::vector<Inven_Item>>::operator*((int)&v57);
          LOBYTE(v24) = *(_BYTE *)(v15 + 60);
          CBattle_Field::CBloodClearRewardData::setRewarItemCopy(
            (int)(this + 887),
            *(_DWORD *)v15,
            *(_DWORD *)(v15 + 4),
            *(_DWORD *)(v15 + 8),
            *(_DWORD *)(v15 + 12),
            *(_DWORD *)(v15 + 16),
            *(_DWORD *)(v15 + 20),
            *(_DWORD *)(v15 + 24),
            *(_DWORD *)(v15 + 28),
            *(_DWORD *)(v15 + 32),
            *(_DWORD *)(v15 + 36),
            *(_DWORD *)(v15 + 40),
            *(_DWORD *)(v15 + 44),
            *(_DWORD *)(v15 + 48),
            *(_DWORD *)(v15 + 52),
            *(_DWORD *)(v15 + 56),
            v24,
            a2,
            i);
          std::vector<Inven_Item>::begin(&v58, v50);
          v16 = __gnu_cxx::__normal_iterator<Inven_Item *,std::vector<Inven_Item>>::operator*((int)&v58);
          v34 = *(_QWORD *)v16;
          v35 = *(_DWORD *)(v16 + 8);
          v36 = *(_DWORD *)(v16 + 12);
          v37 = *(_DWORD *)(v16 + 16);
          v38 = *(_DWORD *)(v16 + 20);
          v39 = *(_DWORD *)(v16 + 24);
          v40 = *(_DWORD *)(v16 + 28);
          v41 = *(_DWORD *)(v16 + 32);
          v42 = *(_DWORD *)(v16 + 36);
          v43 = *(_DWORD *)(v16 + 40);
          v44 = *(_DWORD *)(v16 + 44);
          v45 = *(_DWORD *)(v16 + 48);
          v46 = *(_DWORD *)(v16 + 52);
          v47 = *(_DWORD *)(v16 + 56);
          v48 = *(_BYTE *)(v16 + 60);
          v17 = *(_DWORD *)((char *)&v34 + 2);
          v18 = G_CDataManager();
          item = (CItem *)CItemList::find_item(*((CItemList **)v18 + 3), v17);
          if ( item )
          {
            grade = CItem::get_grade(item);
            rarity = CItem::get_rarity(item);
            CLuckPoint::UseLuckPoint(*(this + 6 * a2 + 30), (CUser *)BYTE1(v34), rarity, grade);
          }
        }
      }
    }
    else
    {
      v27 = 0;
      v26 = a4;
      v28 = 1;
      v29 = 1;
      v30 = 65537;
      v31 = 65537;
      WongWork::stGenerateResult_t::stGenerateResult_t((WongWork::stGenerateResult_t *)v49);
      v6 = G_CDataManager();
      v7 = CDataManager::getItemGenerator(v6);
      v8 = WongWork::CItemGeneratorMgr::getGenerator(v7, 2);
      (*(void (__cdecl **)(int, void **, void **))(*(_DWORD *)v8 + 12))(v8, v25, v49);
      std::vector<Inven_Item>::begin(&v56, v49);
      v9 = __gnu_cxx::__normal_iterator<Inven_Item *,std::vector<Inven_Item>>::operator*((int)&v56);
      LOBYTE(v24) = *(_BYTE *)(v9 + 60);
      CBattle_Field::CBloodClearRewardData::setRewarItemCopy(
        (int)(this + 887),
        *(_DWORD *)v9,
        *(_DWORD *)(v9 + 4),
        *(_DWORD *)(v9 + 8),
        *(_DWORD *)(v9 + 12),
        *(_DWORD *)(v9 + 16),
        *(_DWORD *)(v9 + 20),
        *(_DWORD *)(v9 + 24),
        *(_DWORD *)(v9 + 28),
        *(_DWORD *)(v9 + 32),
        *(_DWORD *)(v9 + 36),
        *(_DWORD *)(v9 + 40),
        *(_DWORD *)(v9 + 44),
        *(_DWORD *)(v9 + 48),
        *(_DWORD *)(v9 + 52),
        *(_DWORD *)(v9 + 56),
        v24,
        a2,
        i);
      index = 0;
      if ( *(this + 811) )
        index = CDungeon::get_index(*(this + 811));
      std::vector<Inven_Item>::begin(&v55, v49);
      QuickParty::RewardData_Param::RewardData_Param((QuickParty::RewardData_Param *)&v53, (CParty *)this);
      v54 = index;
      QuickParty::RewardData_Result::RewardData_Result((QuickParty::RewardData_Result *)v52);
      v52[0] = *(_DWORD *)(__gnu_cxx::__normal_iterator<Inven_Item *,std::vector<Inven_Item>>::operator*((int)&v55) + 7);
      CParty::set_basic_reward((CParty *)this, 0xBu, v53, v54, (int)v52);
      v10 = __gnu_cxx::__normal_iterator<Inven_Item *,std::vector<Inven_Item>>::operator*((int)&v55);
      *(_DWORD *)(v10 + 7) = v52[0];
      v11 = G_CDataManager();
      CBattle_Field::CBloodClearRewardData::setRewardItemAddInfo(
        (CBattle_Field::CBloodClearRewardData *)(this + 887),
        *((float *)v11 + 6232),
        a2,
        i);
      WongWork::stGenerateResult_t::~stGenerateResult_t(v49);
    }
    WongWork::stGenerateResult_t::~stGenerateResult_t(v50);
    WongWork::stGenerateRefData_t::~stGenerateRefData_t(v25);
  }
  if ( a6 && *((_BYTE *)*(this + 811) + 2207) == 2 )
  {
    UltimateRewardItem = CBattle_Field::CBloodClearRewardData::getUltimateRewardItem((CBattle_Field::CBloodClearRewardData *)(this + 887));
    v21 = G_CDataManager();
    if ( CItemList::find_item(*((CItemList **)v21 + 3), UltimateRewardItem) )
    {
      WongWork::stGenerateResult_t::stGenerateResult_t((WongWork::stGenerateResult_t *)v49);
      Inven_Item::Inven_Item((Inven_Item *)v32);
      v33 = UltimateRewardItem;
      v22 = G_CDataManager();
      CItemList::create_item(*((CItemList **)v22 + 3), 1, (Inven_Item *)v32, 1);
      std::vector<Inven_Item>::push_back((int)v49, (int)v32);
      std::vector<Inven_Item>::begin(&v59, v49);
      v23 = __gnu_cxx::__normal_iterator<Inven_Item *,std::vector<Inven_Item>>::operator*((int)&v59);
      LOBYTE(v24) = *(_BYTE *)(v23 + 60);
      CBattle_Field::CBloodClearRewardData::setRewarItemCopy(
        (int)(this + 887),
        *(_DWORD *)v23,
        *(_DWORD *)(v23 + 4),
        *(_DWORD *)(v23 + 8),
        *(_DWORD *)(v23 + 12),
        *(_DWORD *)(v23 + 16),
        *(_DWORD *)(v23 + 20),
        *(_DWORD *)(v23 + 24),
        *(_DWORD *)(v23 + 28),
        *(_DWORD *)(v23 + 32),
        *(_DWORD *)(v23 + 36),
        *(_DWORD *)(v23 + 40),
        *(_DWORD *)(v23 + 44),
        *(_DWORD *)(v23 + 48),
        *(_DWORD *)(v23 + 52),
        *(_DWORD *)(v23 + 56),
        v24,
        a2,
        a3 - 1);
      WongWork::stGenerateResult_t::~stGenerateResult_t(v49);
    }
  }
}

int __cdecl CParty::checkBloodRenewMyRecord(CUserCharacInfo **this, int id, unsigned int id2, int id3, int id4)
{
  return CUserCharacInfo::setBloodBestRecord(*(this + 6 * a2 + 30), a3, a4, a5, 0);
}

void __cdecl CParty::BloodClearRewardPacket(
        CUser **this,

int __cdecl CParty::BloodHandleDieUser(CParty *this, CUser *user)
{
  int id; // esi
  int PartyIndex; // ebx
  TimerQueue *v5; // eax
  int i; // [esp+2Ch] [ebp-Ch]

  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::checkValidUser((CUser **)this, i) && (CUser *)CParty::get_user(this, i) == a2 )
    {
      if ( *((_BYTE *)this + i + 896) != 1 )
      {
        CUser::SendCmdErrorPacket(a2, 43, 18);
        return 0;
      }
      CParty::set_charac_live(this, a2, 0);
      break;
    }
  }
  if ( (unsigned __int8)CParty::check_allmember_die((CUser **)this) )
  {
    if ( *((_DWORD *)this + 822) != 1 )
    {
      v3 = CParty::gen_timer_key((int)this, 34);
      PartyIndex = CParty::GetPartyIndex(this);
      v5 = G_TimerQueue();
      TimerQueue::InsertTimer(v3, v5, 1, PartyIndex, 34, 10, v3, 34);
    }
  }
  return 1;
}

void __usercall CParty::finishBloodDungeon(int id@<esi>, CParty *this)
{
  CParty::ReturnToVillage(a1, this);
}

int __cdecl CParty::IsThereGhostInParty(CUser **this)
{
  int i; // [esp+1Ch] [ebp-Ch]

  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::_checkValidUser(this, i)
      && (unsigned __int8)CUserCharacInfo::IsCurCharacGhost(*(this + 6 * i + 30)) )
    {
      return 1;
    }
  }
  return 0;
}

_BOOL4 __cdecl CParty::getMemberPassParty(CUserCharacInfo **this, CUser *user)
{
  int connect_upper_member; // ebx
  int charac_no; // ebx
  int id; // ebx
  int id; // [esp+14h] [ebp-14h] BYREF
  int charac_level; // [esp+18h] [ebp-10h] BYREF
  int i; // [esp+1Ch] [ebp-Ch]

  v8 = 0;
  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::_checkValidUser(this, i) == 1 )
    {
      connect_upper_member = CUserCharacInfo::get_connect_upper_member(a2);
      if ( connect_upper_member != CUser::get_charac_no(*(this + 6 * i + 30), -1) )
      {
        charac_no = CUser::get_charac_no(a2, -1);
        if ( charac_no != CUser::get_charac_no(*(this + 6 * i + 30), -1) )
        {
          charac_level = CUserCharacInfo::get_charac_level(*(this + 6 * i + 30));
          v8 = *std::max<int>(&v8, &charac_level);
        }
      }
    }
  }
  if ( v8 && (v5 = v8, v5 - CUserCharacInfo::get_charac_level_before_dungeon(a2) <= 6) )
    return 0;
  else
    return v8 != 0;
}

char __cdecl CParty::Reset_party_overlapped_drop_ratio(CParty *this)
{
  int *CurCharacInvenR; // eax
  int *v2; // eax
  int *v3; // eax
  char result; // al
  _BYTE v5[2]; // [esp+15h] [ebp-E3h] BYREF
  unsigned int value; // [esp+17h] [ebp-E1h]
  _BYTE v7[2]; // [esp+52h] [ebp-A6h] BYREF
  unsigned int value; // [esp+54h] [ebp-A4h]
  _BYTE v9[2]; // [esp+8Fh] [ebp-69h] BYREF
  unsigned int value; // [esp+91h] [ebp-67h]
  int value; // [esp+CCh] [ebp-2Ch]
  int value; // [esp+D0h] [ebp-28h]
  int value; // [esp+D4h] [ebp-24h]
  int value; // [esp+D8h] [ebp-20h]
  char flag; // [esp+DEh] [ebp-1Ah]
  char flag; // [esp+DFh] [ebp-19h]
  int i; // [esp+E0h] [ebp-18h]
  unsigned int value; // [esp+E4h] [ebp-14h]
  unsigned int value; // [esp+E8h] [ebp-10h]
  unsigned int value; // [esp+ECh] [ebp-Ch]

  *((_DWORD *)this + 1563) = 0;
  *((_DWORD *)this + 1564) = 0;
  v11 = 0;
  v12 = 0;
  v13 = 0;
  v14 = 0;
  v15 = 0;
  v16 = 0;
  for ( i = 0; ; ++i )
  {
    result = i <= 3;
    if ( i > 3 )
      break;
    if ( (unsigned __int8)CParty::_checkValidUser((CUser **)this, i) == 1 && *((_BYTE *)this + i + 896) )
    {
      v15 = 0;
      CurCharacInvenR = (int *)CUserCharacInfo::getCurCharacInvenR(*((CUserCharacInfo **)this + 6 * i + 30));
      CInventory::GetInvenSlot((CInventory *)v5, CurCharacInvenR, 0, 19);
      v18 = v6;
      v2 = (int *)CUserCharacInfo::getCurCharacInvenR(*((CUserCharacInfo **)this + 6 * i + 30));
      CInventory::GetInvenSlot((CInventory *)v7, v2, 0, 18);
      v19 = v8;
      v3 = (int *)CUserCharacInfo::getCurCharacInvenR(*((CUserCharacInfo **)this + 6 * i + 30));
      CInventory::GetInvenSlot((CInventory *)v9, v3, 0, 17);
      v20 = v10;
      if ( v18 <= 0x5E7F || v18 > 0x5E87 )
      {
        if ( v18 == 24191 )
        {
          ++v11;
        }
        else if ( v18 == 24281 )
        {
          ++v12;
          ++v15;
        }
      }
      else
      {
        ++v12;
        ++v15;
      }
      if ( v19 <= 0x5692 || v19 > 0x569A )
      {
        if ( v18 == 24281 )
        {
          ++v13;
          ++v15;
        }
      }
      else
      {
        ++v13;
        ++v15;
      }
      if ( v20 <= 0x4ECA || v20 > 0x4ED2 )
      {
        if ( v18 == 24281 )
        {
          ++v14;
          ++v15;
        }
      }
      else
      {
        ++v14;
        ++v15;
      }
      if ( v15 == 3 )
        v16 = 1;
    }
  }
  if ( v11 )
  {
    result = (char)this;
    *((_DWORD *)this + 1563) += v11 + 2;
  }
  if ( v12 )
  {
    *((_DWORD *)this + 1563) += 2 * (v12 + 2);
    result = (char)this;
    *((_DWORD *)this + 1564) += v12 + 2;
  }
  if ( v13 )
  {
    *((_DWORD *)this + 1563) += 2 * (v13 + 2);
    result = (char)this;
    *((_DWORD *)this + 1564) += v13 + 2;
  }
  if ( v14 )
  {
    *((_DWORD *)this + 1563) += 2 * (v14 + 2);
    result = (char)this;
    *((_DWORD *)this + 1564) += v14 + 2;
  }
  if ( v16 )
  {
    *((_DWORD *)this + 1563) += 5;
    *((_DWORD *)this + 1564) += 2;
    return (char)this;
  }
  return result;
}

char __cdecl CParty::_onGetItem(CParty *this, CUser *user, unsigned int id, unsigned __int16 a4)
{
  CItemList **v4; // eax
  int item; // eax
  CDataManager *v6; // eax
  EventClassify::CEventScriptMng *event_script_mng; // eax
  CDataManager *v8; // eax
  EventClassify::CEventScriptMng *v9; // eax
  CItem *v11; // [esp+14h] [ebp-14h]
  char flag; // [esp+1Ch] [ebp-Ch]

  v4 = (CItemList **)G_CDataManager();
  item = CDataManager::find_item(v4, a3);
  v11 = (CItem *)item;
  if ( item )
  {
    v6 = G_CDataManager();
    event_script_mng = (EventClassify::CEventScriptMng *)CDataManager::get_event_script_mng(v6);
    if ( (unsigned __int8)EventClassify::CEventScriptMng::is_eventing(event_script_mng, 0x193u) )
    {
      v8 = G_CDataManager();
      v9 = (EventClassify::CEventScriptMng *)CDataManager::get_event_script_mng(v8);
      EventClassify::CEventScriptMng::process_add_item_count_db(v9, a2, a3, a4);
    }
    LOBYTE(item) = CItem::is_stackable(v11);
    if ( (_BYTE)item )
    {
      LOBYTE(item) = (*(int (__cdecl **)(CItem *))(*(_DWORD *)v11 + 12))(v11) == 26;
      if ( (_BYTE)item )
      {
        v12 = 0;
        switch ( a3 )
        {
          case 0x289749u:
            v12 = 2;
            break;
          case 0x28982Du:
            LOBYTE(item) = CUser::SendRandomBoxStatistic(1, 0);
            return item;
          case 0x380u:
            LOBYTE(item) = CUser::SendRandomBoxStatistic(0, 0);
            return item;
        }
        LOBYTE(item) = CUser::SendRandomBoxStatistic(v12, 0);
      }
    }
  }
  return item;
}

  CParty::game_start(a1, this, a3);
  *(_DWORD *)v16 = CBattle_Field::get_dungeon_index((CBattle_Field *)(this + 713));

  member_count = CParty::get_member_count(this);
  if ( (unsigned __int8)CBattle_Field::SelectDungeon(

  CParty::_getMemberNames(this, s);
  for ( i = 0; i <= 3; ++i )

    if ( (unsigned __int8)CParty::_checkValidUser(this, i) == 1 )
    {
      MemberNames = CParty::_getMemberNames(this, s);
      DungeonName = CDungeon::GetDungeonName((std::string *)dungeon);
      cUserHistoryLog::EnterDungeon((CUser **)*(this + 6 * i + 30) + CUSER_OFFSET_HISTORY_LOG_ALT, DungeonName, v17, MemberNames, 0);
    }

  CParty::set_state((CParty *)this, 2);
  CParty::SetEPLPState((CParty *)this, 1);

  CParty::send_loadingcheck_msg((CParty *)this);
  InterfacePacketBuf::clear(v14);

    if ( (unsigned __int8)CParty::checkValidUser((CUser **)this, i) == 1 )
    {
      v4 = i;
      v16[v4] = (CUserCharacInfo *)CParty::get_user(this, i);
    }

      if ( (unsigned __int8)CParty::checkValidUser((CUser **)this, j) == 1 )
      {
        user = (CUser *)CParty::get_user(this, j);
        break;
      }

  CParty::giveup_game(a1, this, user, 0, v18, a4);
  *((_WORD *)this + 1574) = 0;

int __cdecl CParty::GetUserPosition(CUser **this, CUser *user)
{
  int i; // [esp+1Ch] [ebp-Ch]

  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::_checkValidUser(this, i) == 1 && *(this + 6 * i + 30) == a2 )
      return i;
  }
  return -1;
}

CUser **__cdecl CParty::GetUserSecretShop(CUser **this, CUser *user)
{
  unsigned int UserPosition; // [esp+1Ch] [ebp-Ch]

  UserPosition = CParty::GetUserPosition(this, a2);
  if ( UserPosition >= 4 )
    return 0;
  else
    return this + 7 * UserPosition + 170;
}

void __cdecl CParty::SendSecretShopItemList(CUser **this, CUser *user)
{
  unsigned int UserPosition; // [esp+1Ch] [ebp-Ch]

  UserPosition = CParty::GetUserPosition(this, a2);
  if ( UserPosition < 4 )
    secretshop::CSecretShop::SendSecretShopItemList((int)GlobalData::s_secret_shop, a2, this + 7 * UserPosition + 170);
}

int __cdecl CParty::SetRidableObjectState(int value, int value2, int value3)
{
  return CBattle_Field::SetRidableObjectState(a1 + 2852, a2, a3);
}

int __cdecl CParty::IsReturnUserParty(CUser **this)
{
  int i; // [esp+1Ch] [ebp-Ch]

  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::checkValidUser(this, i) == 1
      && *(this + 6 * i + 30)
      && (unsigned __int8)CUser::IsReturnUser(*(this + 6 * i + 30)) )
    {
      return 1;
    }
  }
  return 0;
}

int __cdecl CParty::IsEventCharacParty(CUserCharacInfo **this)
{
  int charac_job; // eax
  EventNewCharacterReward *RepeatEvent; // [esp+18h] [ebp-10h]
  int i; // [esp+1Ch] [ebp-Ch]

  RepeatEvent = (EventNewCharacterReward *)CEventManager::GetRepeatEvent(GlobalData::s_event_manager, 162);
  if ( !RepeatEvent
    || (*(unsigned __int8 (__cdecl **)(EventNewCharacterReward *, _DWORD))(*(_DWORD *)RepeatEvent + 52))(RepeatEvent, 0) != 1 )
  {
    return 0;
  }
  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::checkValidUser(this, i) == 1 )
    {
      if ( *(this + 6 * i + 30) )
      {
        charac_job = CUserCharacInfo::get_charac_job(*(this + 6 * i + 30));
        if ( EventNewCharacterReward::isEventCharacter(RepeatEvent, charac_job) )
          return 1;
      }
    }
  }
  return 0;
}

char __cdecl CParty::SetMemberRoutingState(CParty *this, CUser *user, char flag)
{
  char result; // al
  int i; // [esp+2Ch] [ebp-Ch]

  for ( i = 0; ; ++i )
  {
    result = i <= 3;
    if ( i > 3 )
      break;
    if ( (unsigned __int8)CParty::_checkValidUser((CUser **)this, i) == 1 && *((CUser **)this + 6 * i + 30) == a2 )
      return CParty::CItemRoutingData::SetMemberRoutingState(this, i, a3);
  }
  return result;
}

int __cdecl CParty::GetMemberRoutingState(CParty *this, CUser *user)
{
  int i; // [esp+1Ch] [ebp-Ch]

  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::_checkValidUser((CUser **)this, i) == 1 && *((CUser **)this + 6 * i + 30) == a2 )
      return CParty::CItemRoutingData::GetMemberRoutingState(this, i);
  }
  return -1;
}

int __cdecl CParty::HasAllMemberSelection(CUser **this)
{
  int i; // [esp+1Ch] [ebp-Ch]

  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::_checkValidUser(this, i) == 1
      && !(unsigned __int8)CParty::CItemRoutingData::GetMemberRoutingState((CParty::CItemRoutingData *)this, i) )
    {
      return 0;
    }
  }
  return 1;
}

bool __cdecl CParty::SetRoutingTimerOver(CParty *this)
{
  bool result; // al
  int i; // [esp+1Ch] [ebp-Ch]

  for ( i = 0; ; ++i )
  {
    result = i <= 3;
    if ( i > 3 )
      break;
    if ( (unsigned __int8)CParty::_checkValidUser((CUser **)this, i) == 1
      && !(unsigned __int8)CParty::CItemRoutingData::GetMemberRoutingState(this, i) )
    {
      CParty::CItemRoutingData::SetMemberRoutingState(this, i, 2u);
    }
  }
  return result;
}

bool __cdecl CParty::GetRoutingDiceMemberList(CUserCharacInfo **a1, _DWORD *a2, CItem *a3)
{
  int CurCharacGrowType; // ebx
  int charac_job; // eax
  char flag; // al
  bool result; // al
  int k; // [esp+10h] [ebp-38h] BYREF
  int j; // [esp+14h] [ebp-34h] BYREF
  int i; // [esp+18h] [ebp-30h] BYREF
  _DWORD value[2]; // [esp+1Ch] [ebp-2Ch] BYREF
  int CurCharacNo; // [esp+24h] [ebp-24h] BYREF
  _DWORD value[2]; // [esp+28h] [ebp-20h] BYREF
  int id; // [esp+30h] [ebp-18h] BYREF
  _DWORD value[2]; // [esp+34h] [ebp-14h] BYREF
  int id[3]; // [esp+3Ch] [ebp-Ch] BYREF

  if ( (unsigned __int8)CItem::IsRoutingItem(a3) )
  {
    for ( i = 0; i <= 3; ++i )
    {
      if ( (unsigned __int8)CParty::_checkValidUser(a1, i) )
      {
        if ( (unsigned __int8)CParty::CItemRoutingData::GetMemberRoutingState((CParty::CItemRoutingData *)a1, i) == 1 )
        {
          CurCharacGrowType = (char)CUserCharacInfo::getCurCharacGrowType(a1[6 * i + 30]);
          charac_job = CUserCharacInfo::get_charac_job(a1[6 * i + 30]);
          if ( (unsigned __int8)CItem::IsPriorityJob(a3, charac_job, CurCharacGrowType) )
          {
            CurCharacNo = CUserCharacInfo::getCurCharacNo(a1[6 * i + 30]);
            std::pair<int,int>::pair<int &,unsigned int>(v13, (int)&i, (int)&CurCharacNo);
            std::vector<std::pair<int,int>>::push_back((int)a2, (int)v13);
          }
        }
      }
    }
  }
  if ( std::vector<std::pair<int,int>>::empty(a2) )
  {
    for ( j = 0; j <= 3; ++j )
    {
      if ( (unsigned __int8)CParty::_checkValidUser(a1, j) )
      {
        if ( (unsigned __int8)CParty::CItemRoutingData::GetMemberRoutingState((CParty::CItemRoutingData *)a1, j) == 1 )
        {
          v6 = CUserCharacInfo::get_charac_job(a1[6 * j + 30]);
          if ( (unsigned __int8)CItem::check_job_type(a3, v6) )
          {
            v16 = CUserCharacInfo::getCurCharacNo(a1[6 * j + 30]);
            std::pair<int,int>::pair<int &,unsigned int>(v15, (int)&j, (int)&v16);
            std::vector<std::pair<int,int>>::push_back((int)a2, (int)v15);
          }
        }
      }
    }
  }
  result = std::vector<std::pair<int,int>>::empty(a2);
  if ( result )
  {
    for ( k = 0; ; ++k )
    {
      result = k <= 3;
      if ( k > 3 )
        break;
      if ( (unsigned __int8)CParty::_checkValidUser(a1, k)
        && (unsigned __int8)CParty::CItemRoutingData::GetMemberRoutingState((CParty::CItemRoutingData *)a1, k) == 1 )
      {
        v18[0] = CUserCharacInfo::getCurCharacNo(a1[6 * k + 30]);
        std::pair<int,int>::pair<int &,unsigned int>(v17, (int)&k, (int)v18);
        std::vector<std::pair<int,int>>::push_back((int)a2, (int)v17);
      }
    }
  }
  return result;
}

int __cdecl CParty::GetFirstValidMemberState(CUser **this)
{
  int i; // [esp+1Ch] [ebp-Ch]

  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::_checkValidUser(this, i) == 1 )
      return CUser::get_state(*(this + 6 * i + 30));
  }
  return 0;
}

bool __cdecl CParty::send_to_alive_member(CUser **this, PacketGuard *a2)
{
  bool result; // al
  int i; // [esp+1Ch] [ebp-Ch]

  for ( i = 0; ; ++i )
  {
    result = i <= 3;
    if ( i > 3 )
      break;
    if ( (unsigned __int8)CParty::_checkValidUser(this, i) )
    {
      if ( CUser::get_state(*(this + 6 * i + 30)) == 5 && *((_BYTE *)this + i + 896) )
        CUser::Send(*(this + 6 * i + 30), a2);
    }
  }
  return result;
}

int __cdecl CParty::switchPostion(CParty *this, char flag, char type)
{
  _DWORD *v3; // edx
  _DWORD *v4; // eax
  int id; // [esp+10h] [ebp-1Ch]
  int id; // [esp+14h] [ebp-18h]
  int id; // [esp+18h] [ebp-14h]
  int id; // [esp+1Ch] [ebp-10h]
  int id; // [esp+20h] [ebp-Ch]
  int id; // [esp+24h] [ebp-8h]

  v6 = *((_DWORD *)this + 6 * a2 + 30);
  v7 = *((_DWORD *)this + 6 * a2 + 31);
  v8 = *((_DWORD *)this + 6 * a2 + 32);
  v9 = *((_DWORD *)this + 6 * a2 + 33);
  v10 = *((_DWORD *)this + 6 * a2 + 34);
  v11 = *((_DWORD *)this + 6 * a2 + 35);
  v3 = (_DWORD *)((char *)this + 24 * a2 + 112);
  v3[2] = *((_DWORD *)this + 6 * a3 + 30);
  v3[3] = *((_DWORD *)this + 6 * a3 + 31);
  v3[4] = *((_DWORD *)this + 6 * a3 + 32);
  v3[5] = *((_DWORD *)this + 6 * a3 + 33);
  v3[6] = *((_DWORD *)this + 6 * a3 + 34);
  v3[7] = *((_DWORD *)this + 6 * a3 + 35);
  v4 = (_DWORD *)((char *)this + 24 * a3 + 112);
  v4[2] = v6;
  v4[3] = v7;
  v4[4] = v8;
  v4[5] = v9;
  v4[6] = v10;
  v4[7] = v11;
  return 1;
}

bool __cdecl CParty::setMonsterMoveHack(CParty *this)
{
  CUser *user; // ebx
  char *HackAnalyzer; // eax
  bool result; // al
  int i; // [esp+2Ch] [ebp-Ch]

  for ( i = 0; ; ++i )
  {
    result = i <= 3;
    if ( i > 3 )
      break;
    if ( (unsigned __int8)CParty::_checkValidUser((CUser **)this, i) )
    {
      v1 = (CUser *)*((_DWORD *)this + 6 * i + 30);
      HackAnalyzer = CUser::getHackAnalyzer(v1);
      WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)HackAnalyzer, v1, 0xD3u, 1, 0, 0);
    }
  }
  return result;
}

int __cdecl CParty::CheckEnterVillageAttackRevenge(CUser **this, const CDungeon *dungeon)
{
  CUserCharacInfo *user; // eax
  CUserCharacInfo *charac_info; // eax
  int member_count; // [esp+14h] [ebp-14h]
  int i; // [esp+18h] [ebp-10h]
  unsigned __int8 CurRevengeDungeonCount; // [esp+1Fh] [ebp-9h]

  if ( *((_BYTE *)a2 + 2170) )
  {
    member_count = CParty::get_member_count(this);
    if ( (unsigned __int8)village_attacked::CRevengeDungeon::IsOpenRevengeDungeon(GlobalData::s_revengeDungeonMgr) != 1 )
      return 21;
    if ( *((char *)a2 + 2169) < member_count || *((char *)a2 + 2168) > member_count )
      return 171;
    for ( i = 0; i <= 3; ++i )
    {
      if ( (unsigned __int8)CParty::checkValidUser(this, i) )
      {
        user = (CUserCharacInfo *)CParty::get_user((CParty *)this, i);
        CurRevengeDungeonCount = CUserCharacInfo::GetCurRevengeDungeonCount(user);
        if ( !CurRevengeDungeonCount || *((_BYTE *)a2 + 2171) < CurRevengeDungeonCount )
        {
          v4 = (CUserCharacInfo *)CParty::get_user((CParty *)this, i);
          CUserCharacInfo::SetCurRevengeDungeonCount(v4, 0);
          return 173;
        }
      }
    }
  }
  return 0;
}

int __cdecl CParty::OnTODRewardTimer(CParty *party, int id)
{
  int id; // esi
  int PartyIndex; // ebx
  TimerQueue *v4; // eax
  int id; // esi
  int id; // ebx
  TimerQueue *v7; // eax

  if ( a2 == 44 )
  {
    CParty::_Send_TOD_ClearReward(a1);
    CParty::SetEPLPState(a1, 2);
    v2 = CParty::gen_timer_key((int)a1, 21);
    PartyIndex = CParty::GetPartyIndex(a1);
    v4 = G_TimerQueue();
    TimerQueue::InsertTimer(v2, v4, 1, PartyIndex, 21, 60, v2, 0);
    CParty::set_state(a1, 1);
    v5 = CParty::gen_timer_key((int)a1, 36);
    v6 = CParty::GetPartyIndex(a1);
    v7 = G_TimerQueue();
    TimerQueue::InsertTimer(v5, v7, 1, v6, 44, 8, v5, 36);
  }
  return 1;
}

int __cdecl CParty::_Send_TOD_ClearReward(CParty *this)
{
  int Manager; // eax
  __int16 Layer; // ax
  int *v3; // eax
  int id; // eax
  unsigned int id; // eax
  int id; // eax
  PacketBuf *v8[3]; // [esp+14h] [ebp-24h] BYREF
  _BYTE v9[2]; // [esp+22h] [ebp-16h] BYREF
  int CharacExpandData; // [esp+24h] [ebp-14h]
  unsigned int i; // [esp+28h] [ebp-10h]
  unsigned int j; // [esp+2Ch] [ebp-Ch]

  PacketGuard::PacketGuard((PacketGuard *)v8);
  InterfacePacketBuf::put_header(v8, 0, 347);
  InterfacePacketBuf::put_int(v8, *((_DWORD *)this + 203));
  Manager = CParty::getManager(this);
  CharacExpandData = CUser::GetCharacExpandData(Manager, 0xDu);
  TOD_UserState::getLastEnterLayer((TOD_UserState *)v9, CharacExpandData);
  Layer = TOD_Layer::GetLayer((TOD_Layer *)v9);
  InterfacePacketBuf::put_short(v8, Layer);
  InterfacePacketBuf::put_byte(v8, 10);
  std::vector<std::pair<int,int>>::size((_DWORD *)this + 1514);
  for ( i = 0; ; ++i )
  {
    v5 = std::vector<std::pair<int,int>>::size((_DWORD *)this + 1514);
    if ( v5 <= i )
      break;
    v3 = (int *)std::vector<std::pair<int,int>>::operator[]((_DWORD *)this + 1514, i);
    InterfacePacketBuf::put_int(v8, *v3);
    v4 = std::vector<std::pair<int,int>>::operator[]((_DWORD *)this + 1514, i);
    InterfacePacketBuf::put_int(v8, *(_DWORD *)(v4 + 4));
  }
  for ( j = 0; ; ++j )
  {
    v6 = std::vector<std::pair<int,int>>::size((_DWORD *)this + 1514);
    if ( 9 - v6 <= j )
      break;
    InterfacePacketBuf::put_int(v8, -1);
    InterfacePacketBuf::put_int(v8, 0);
  }
  InterfacePacketBuf::put_int(v8, *((_DWORD *)this + 1517));
  InterfacePacketBuf::put_int(v8, *((_DWORD *)this + 1518));
  InterfacePacketBuf::finalize(v8, 1);
  CParty::send_to_party((CUser **)this, (PacketGuard *)v8);
  PacketGuard::~PacketGuard(v8);
  return 1;
}

int __cdecl CParty::_Send_TOD_APCInfo(CDungeon **this, int id)
{
  CUser *Manager; // eax
  CUser *user; // eax
  _BYTE v5[2]; // [esp+18h] [ebp-10h] BYREF
  _BYTE v6[2]; // [esp+1Ah] [ebp-Eh] BYREF
  TowerOfDespairMgr *v7; // [esp+1Ch] [ebp-Ch]

  if ( CDungeon::isTowerOfDespairDungeon(*(this + 811)) )
  {
    v7 = GlobalData::s_pTowerOfDespairMgr;
    TOD_Layer::TOD_Layer((TOD_Layer *)v6, 0);
    Manager = (CUser *)CParty::getManager((CParty *)this);
    TowerOfDespairMgr::SendAPCInfo((TowerOfDespair_APC_Mgr **)v7, (const TOD_Layer *)v6, Manager);
    if ( a2 != -1 )
    {
      TOD_Layer::TOD_Layer((TOD_Layer *)v5, a2);
      v3 = (CUser *)CParty::getManager((CParty *)this);
      TowerOfDespairMgr::SendAPCInfo((TowerOfDespair_APC_Mgr **)v7, (const TOD_Layer *)v5, v3);
    }
  }
  return 1;
}

int __cdecl CParty::GetDungeonIndex_isTOD(CParty *this, int *a2, int *a3)
{
  unsigned __int16 Layer; // ax
  int id; // ebx
  CDungeonList **v6; // eax
  const CDungeon *dungeon; // eax
  unsigned __int16 v8; // ax
  unsigned __int16 v9; // ax
  char flag; // [esp+8h] [ebp-140h]
  char s[256]; // [esp+18h] [ebp-130h] BYREF
  _BYTE v12[3]; // [esp+118h] [ebp-30h] BYREF
  char flag; // [esp+11Bh] [ebp-2Dh] BYREF
  int id[4]; // [esp+11Ch] [ebp-2Ch] BYREF
  char flag; // [esp+12Fh] [ebp-19h]
  TowerOfDespairMgr *v16; // [esp+130h] [ebp-18h]
  CUser *user; // [esp+134h] [ebp-14h]
  int CharacExpandData; // [esp+138h] [ebp-10h]
  int id; // [esp+13Ch] [ebp-Ch]

  v15 = 0;
  v16 = GlobalData::s_pTowerOfDespairMgr;
  if ( !GlobalData::s_pTowerOfDespairMgr )
  {
    cMyTraceNoop::cMyTraceNoop((cMyTraceNoop *)&v13);
    cMyTraceNoop::operator()();
    return 19;
  }
  if ( TowerOfDespairMgr::GetBaseLayerIndex(v16) == *a2 )
  {
    v15 = 1;
    v17 = (CUser *)*((_DWORD *)this + 29);
    if ( !v17 )
    {
      cMyTrace::cMyTrace((cMyTrace *)v14, "int CParty::GetDungeonIndex_isTOD(int&, int&)", 16827, 5);
      cMyTrace::operator()((int)v14, "TOD no Dungeon Manager!", v10);
      return 19;
    }
    CharacExpandData = CUser::GetCharacExpandData((int)v17, 0xDu);
    TOD_UserState::getTodayEnterLayer((TOD_UserState *)v12, CharacExpandData);
    Layer = TOD_Layer::GetLayer((TOD_Layer *)v12);
    *a2 = TowerOfDespairMgr::Get_DungeonIndexByLayer(v16, Layer);
    v5 = *a2;
    v6 = (CDungeonList **)G_CDataManager();
    dungeon = (const CDungeon *)CDataManager::find_dungeon(v6, v5);
    v19 = TowerOfDespairMgr::CheckEnterable(v16, v17, dungeon);
    if ( v19 )
      return v19;
    v8 = TOD_Layer::GetLayer((TOD_Layer *)v12);
    *a3 = TowerOfDespairMgr::Get_APCIndexByLayer(v16, v8);
    v9 = TOD_Layer::GetLayer((TOD_Layer *)v12);
    sprintf(s, "Enter Layer : %d", v9);
    TowerOfDespairMgr::SendMessageToCUser(v17, (CUser *)s);
  }
  return 0;
}

char __cdecl CParty::MemberRegisterKillMonster(CParty *this, char **a2, unsigned __int8 a3)
{
  char result; // al
  int i; // [esp+2Ch] [ebp-2Ch]
  int user; // [esp+30h] [ebp-28h]
  CKillMonsterInfo *CharacExpandData; // [esp+34h] [ebp-24h]
  int j; // [esp+38h] [ebp-20h]
  int id; // [esp+3Ch] [ebp-1Ch]
  CKillMonsterInfo *v9; // [esp+40h] [ebp-18h]
  int k; // [esp+44h] [ebp-14h]
  int id; // [esp+48h] [ebp-10h]
  CKillMonsterInfo *v12; // [esp+4Ch] [ebp-Ch]

  if ( *((_BYTE *)a2 + 8) == 3 )
  {
    for ( i = 1; ; ++i )
    {
      result = i <= 3;
      if ( i > 3 )
        break;
      if ( (unsigned __int8)CParty::checkValidUser((CUser **)this, i) == 1 )
      {
        user = CParty::get_user(this, i);
        CharacExpandData = (CKillMonsterInfo *)CUser::GetCharacExpandData(user, 0xCu);
        if ( !CKillMonsterInfo::CheckBossKill(CharacExpandData, (int)a2[3], a3) )
          CKillMonsterInfo::RegisterBossKill(CharacExpandData, a2[3], a3);
      }
    }
  }
  else if ( *((_BYTE *)a2 + 8) == 8 )
  {
    for ( j = 1; ; ++j )
    {
      result = j <= 3;
      if ( j > 3 )
        break;
      if ( (unsigned __int8)CParty::checkValidUser((CUser **)this, j) == 1 )
      {
        v8 = CParty::get_user(this, j);
        v9 = (CKillMonsterInfo *)CUser::GetCharacExpandData(v8, 0xCu);
        if ( !CKillMonsterInfo::CheckAPCBossKill(v9, (int)a2[3], a3) )
          CKillMonsterInfo::RegisterAPCBossKill(v9, a2[3], a3);
      }
    }
  }
  else
  {
    result = map_monster::isNamedMonster((map_monster *)a2);
    if ( result )
    {
      for ( k = 1; ; ++k )
      {
        result = k <= 3;
        if ( k > 3 )
          break;
        if ( (unsigned __int8)CParty::checkValidUser((CUser **)this, k) == 1 )
        {
          v11 = CParty::get_user(this, k);
          v12 = (CKillMonsterInfo *)CUser::GetCharacExpandData(v11, 0xCu);
          if ( !CKillMonsterInfo::CheckNamedKill(v12, (int)a2[3], a3) )
            CKillMonsterInfo::RegisterNamedKill(v12, a2[3], a3);
        }
      }
    }
  }
  return result;
}

char __cdecl CParty::CheckHackAverageHitCount(CParty *this)
{
  int id; // eax
  CUser *user; // ebx
  char *HackAnalyzer; // eax
  CUser *user; // ebx
  char *v5; // eax
  CUser *user; // ebx
  char *v7; // eax
  float v9; // [esp+3Ch] [ebp-5Ch]
  float v10; // [esp+40h] [ebp-58h]
  float v11; // [esp+40h] [ebp-58h]
  float v12; // [esp+40h] [ebp-58h]
  int id; // [esp+44h] [ebp-54h]
  int id; // [esp+48h] [ebp-50h]
  float v15; // [esp+4Ch] [ebp-4Ch]
  int id; // [esp+50h] [ebp-48h]
  int id; // [esp+54h] [ebp-44h]
  int id; // [esp+58h] [ebp-40h]
  float v19; // [esp+5Ch] [ebp-3Ch]
  int id; // [esp+68h] [ebp-30h]
  int id; // [esp+6Ch] [ebp-2Ch]
  int id; // [esp+70h] [ebp-28h]
  int id; // [esp+74h] [ebp-24h]
  int id; // [esp+78h] [ebp-20h]
  int id; // [esp+7Ch] [ebp-1Ch]
  int i; // [esp+80h] [ebp-18h]
  int j; // [esp+84h] [ebp-14h]
  int k; // [esp+88h] [ebp-10h]
  int m; // [esp+8Ch] [ebp-Ch]

  LOBYTE(v1) = village_attacked::CVillageMonsterMgr::OnEvent(GlobalData::s_villageMonsterMgr);
  if ( !(_BYTE)v1 )
  {
    v1 = *((_DWORD *)this + 1565);
    if ( v1 <= 45 )
    {
      LOBYTE(v1) = *((_DWORD *)this + 811)
                && (*(_BYTE *)(*((_DWORD *)this + 811) + 2204)
                 || (char)CDungeon::get_dimension_possible(*((CDungeon **)this + 811)) > 0)
                || *((int *)this + 1565) <= 1;
      if ( !(_BYTE)v1 )
      {
        v9 = (long double)*((int *)this + 1566) / (long double)*((int *)this + 1565);
        v10 = v9 * 0.1;
        v13 = (int)(v9 + v10 + 0.5);
        v14 = (int)(v9 - v10);
        v15 = (long double)*((int *)this + 1612) / (long double)*((int *)this + 1565);
        v11 = v15 * 0.1;
        v16 = (int)(v15 + v11 + 0.5);
        v17 = (int)(v15 - v11);
        v18 = 0;
        v19 = (long double)*((int *)this + 1658) / (long double)*((int *)this + 1565);
        v12 = v19 * 0.1;
        v20 = 0;
        v21 = 0;
        v22 = 0;
        v23 = 0;
        for ( i = 0; *((_DWORD *)this + 1565) > i; ++i )
        {
          if ( *((_DWORD *)this + i + 1567) <= v13 && *((_DWORD *)this + i + 1567) >= v14 )
            ++v21;
          if ( *((_DWORD *)this + i + 1613) )
          {
            if ( *((_DWORD *)this + i + 1613) <= v16 && *((_DWORD *)this + i + 1613) >= v17 )
              ++v18;
          }
          else
          {
            ++v22;
          }
          if ( *((_DWORD *)this + i + 1659) )
          {
            if ( *((_DWORD *)this + i + 1659) <= (int)(v19 + v12 + 0.5)
              && *((_DWORD *)this + i + 1659) >= (int)(v19 - v12) )
            {
              ++v20;
            }
          }
          else
          {
            ++v23;
          }
        }
        if ( *((_DWORD *)this + 1565) / 2 <= v21 )
        {
          for ( j = 0; j <= 3; ++j )
          {
            if ( (unsigned __int8)CParty::_checkValidUser((CUser **)this, j) )
            {
              v2 = (CUser *)*((_DWORD *)this + 6 * j + 30);
              HackAnalyzer = CUser::getHackAnalyzer(v2);
              WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)HackAnalyzer, v2, 0xD9u, 1, 0, 0);
            }
          }
        }
        v24 = *((_DWORD *)this + 1565) - v22;
        if ( v24 > 0 && v24 / 2 <= v18 )
        {
          for ( k = 0; k <= 3; ++k )
          {
            if ( (unsigned __int8)CParty::_checkValidUser((CUser **)this, k) )
            {
              v4 = (CUser *)*((_DWORD *)this + 6 * k + 30);
              v5 = CUser::getHackAnalyzer(v4);
              WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)v5, v4, 0xDAu, 1, 0, 0);
            }
          }
        }
        v1 = *((_DWORD *)this + 1565) - v23;
        v25 = v1;
        if ( v1 > 0 )
        {
          v1 /= 2;
          if ( v25 / 2 <= v20 )
          {
            for ( m = 0; ; ++m )
            {
              LOBYTE(v1) = m <= 3;
              if ( m > 3 )
                break;
              if ( (unsigned __int8)CParty::_checkValidUser((CUser **)this, m) )
              {
                v6 = (CUser *)*((_DWORD *)this + 6 * m + 30);
                v7 = CUser::getHackAnalyzer(v6);
                WongWork::CHackAnalyzer::addServerHackCnt((WongWork::CHackAnalyzer *)v7, v6, 0xDBu, 1, 0, 0);
              }
            }
          }
        }
      }
    }
  }
  return v1;
}

_BOOL4 __cdecl CParty::isCheckUseSkill(CParty *this, int id, int id2)
{
  return a3 <= 168
      && (a2 || a3 != 46)
      && (a2 != 1 || a3 != 5)
      && (a2 != 2 || a3 != 4)
      && (a2 != 3 || a3 != 12)
      && (a2 != 4 || a3 != 1)
      && (a2 != 5 || a3 != 4)
      && (a2 != 6 || a3 != 1)
      && (a2 != 7 || a3 != 5);
}

int __cdecl CParty::SetProperDungeonEvent(CDungeon **this, CUser *user)
{
  int index; // ebx
  CDataManager *v3; // eax
  int result; // eax
  int charac_level; // ebx
  CDataManager *v6; // eax
  int value; // [esp+18h] [ebp-20h] BYREF
  int min_level; // [esp+1Ch] [ebp-1Ch] BYREF
  int value; // [esp+20h] [ebp-18h] BYREF
  int value; // [esp+24h] [ebp-14h] BYREF
  int standard_level; // [esp+28h] [ebp-10h]
  int value; // [esp+2Ch] [ebp-Ch]

  CUserCharacInfo::ClearProperDungeonUser(a2);
  index = CDungeon::get_index(*(this + 811));
  v3 = G_CDataManager();
  result = ConditionLevelChkDungeon::isEventDungeon((CDataManager *)((char *)v3 + 19944), index) ^ 1;
  if ( !(_BYTE)result )
  {
    charac_level = CUserCharacInfo::get_charac_level(a2);
    v6 = G_CDataManager();
    result = !ConditionLevelChkDungeon::isApplyLevel((CDataManager *)((char *)v6 + 19944), charac_level);
    if ( !(_BYTE)result )
    {
      standard_level = CDungeon::get_standard_level(*(this + 811));
      min_level = CDungeon::get_min_level(*(this + 811));
      v9 = 0;
      min_level = *std::max<int>(&min_level, &v9);
      v7 = standard_level + 1;
      v10 = 70;
      v7 = *std::min<int>(&v7, &v10);
      v12 = CUserCharacInfo::get_charac_level(a2);
      result = min_level;
      if ( v12 >= min_level )
      {
        result = v7;
        if ( v12 <= v7 )
          return CUserCharacInfo::SetProperDungeonUser(a2);
      }
    }
  }
  return result;
}

CUser *__cdecl CParty::SetProperLevelDungeon(CDungeon **this, CUser *user)
{
  int index; // ebx
  CDataManager *v3; // eax
  CUser *result; // eax
  CUser *user; // [esp+18h] [ebp-20h] BYREF
  int min_level; // [esp+1Ch] [ebp-1Ch] BYREF
  int value; // [esp+20h] [ebp-18h] BYREF
  int value; // [esp+24h] [ebp-14h] BYREF
  int standard_level; // [esp+28h] [ebp-10h]
  int charac_level; // [esp+2Ch] [ebp-Ch]

  CUser::ClearProperLevelDungeonUser(a2);
  index = CDungeon::get_index(*(this + 811));
  v3 = G_CDataManager();
  result = (CUser *)(ConditionLevelChkDungeon::isEventDungeon((CDataManager *)((char *)v3 + 19944), index) ^ 1);
  if ( !(_BYTE)result )
  {
    standard_level = CDungeon::get_standard_level(*(this + 811));
    min_level = CDungeon::get_min_level(*(this + 811));
    v7 = 0;
    min_level = *std::max<int>(&min_level, &v7);
    v5 = (CUser *)(standard_level + 1);
    v8 = 70;
    v5 = (CUser *)*std::min<int>(&v5, &v8);
    charac_level = CUserCharacInfo::get_charac_level(a2);
    result = (CUser *)min_level;
    if ( charac_level >= min_level )
    {
      result = v5;
      if ( charac_level <= (int)v5 )
        return CUser::SetProperLevelDungeonUser(a2);
    }
  }
  return result;
}

      v4 = &CParty::GetMember(*this)[24 * a2];
      v4[18] = a3;

      if ( (unsigned __int8)CParty::checkValidUser(*(CUser ***)this, i) )
      {
        user = (CUser *)CParty::get_user(*(CParty **)this, i);
        if ( user )
        {
          unique_id = CUser::get_unique_id(user);
          teleport_member_state = CPartyTelePort::get_teleport_member_state((CParty **)this, i);
        }
      }

    CParty::send_to_party(*(CUser ***)this, (PacketGuard *)v4);
    PacketGuard::~PacketGuard(v4);

    if ( (unsigned __int8)CParty::checkValidUser(*this, i) == 1
      && (unsigned __int8)CPartyTelePort::get_teleport_member_state((CParty **)this, i) != 4

      if ( (unsigned __int8)CParty::checkValidUser(*this, i) == 1
        && (unsigned __int8)CPartyTelePort::get_teleport_member_state((CParty **)this, i) != 4 )

        user = (CUser *)CParty::get_user((CParty *)*this, i);
        if ( user )

          Manager = (CUser *)CParty::getManager((CParty *)*this);
          if ( Manager == user )

        if ( (unsigned __int8)CParty::checkValidUser(*this, j) == 1
          && (unsigned __int8)CPartyTelePort::get_teleport_member_state((CParty **)this, j) != 4 )

          v27 = (CUserCharacInfo *)CParty::get_user((CParty *)*this, j);
          if ( v27 )

    v28 = (CUserCharacInfo *)CParty::getManager((CParty *)*this);
    if ( v28 )

            MemberSlotNo = CParty::GetMemberSlotNo(*(const CUser ***)this, a2);
            v4 = MemberSlotNo;

              MemberSlotNo = CParty::checkValidUser(*(CUser ***)this, MemberSlotNo) ^ 1;
              if ( !(_BYTE)MemberSlotNo )

                if ( (CUser *)CParty::getManager(*(CParty **)this) == a2 )
                {
                  CPartyTelePort::set_teleport_member_state((CParty **)this, v4, 0);
                  CPartyTelePort::set_teleport_state(this, 2);
                  CPartyTelePort::send_teleport_status(this, 8);
                  LOBYTE(MemberSlotNo) = (unsigned __int8)CPartyTelePort::reset_teleport_data((CParty **)this);
                }

    return (unsigned __int8)CParty::GetMember(*this)[24 * a2 + 18];
  return -1;

      if ( (unsigned __int8)CParty::checkValidUser((CUser **)*this, i) == 1 )
      {
        user = (CUser *)CParty::get_user(*this, i);
        if ( user )
        {
          std::vector<effect_data>::begin(&v18, v19);
          std::vector<effect_data>::end(&v17, (int)v19);
          v24 = 0;
          while ( __gnu_cxx::operator!=<effect_data *,std::vector<effect_data>>((int)&v18, (int)&v17) )
          {
            v2 = *(_DWORD *)(__gnu_cxx::__normal_iterator<effect_data *,std::vector<effect_data>>::operator->((int)&v18)
                           + 4);
            if ( v2 == CUser::get_area(user, 0) )
            {
              v3 = *(_BYTE *)__gnu_cxx::__normal_iterator<effect_data *,std::vector<effect_data>>::operator->((int)&v18);
              if ( v3 == (unsigned __int8)CUserCharacInfo::getCurCharacVill(user) )
              {
                v24 = 1;
                unique_id = CUser::get_unique_id(user);
                v5 = __gnu_cxx::__normal_iterator<effect_data *,std::vector<effect_data>>::operator->((int)&v18);
                std::vector<unsigned short>::push_back(v5 + 8, (int)&unique_id);
                break;
              }
            }
            __gnu_cxx::__normal_iterator<effect_data *,std::vector<effect_data>>::operator++(&v18);
          }
          if ( v24 != 1 )
          {
            effect_data::effect_data((effect_data *)v15);
            v15[1] = (void *)CUser::get_area(user, 0);
            LOBYTE(v15[0]) = CUserCharacInfo::getCurCharacVill(user);
            v21 = CUser::get_unique_id(user);
            std::vector<unsigned short>::push_back((int)&v16, (int)&v21);
            std::vector<effect_data>::push_back((int)v19, (effect_data *)v15);
            v25 = 0;
            effect_data::~effect_data(v15);
          }
        }
      }

    Manager = (CUser *)CParty::getManager(*this);
    if ( Manager )

          if ( (unsigned __int8)CParty::checkValidUser((CUser **)*this, i) == 1 )
          {
            user = (CUser *)CParty::get_user(*this, i);
            if ( user )
            {
              unique_id = CUser::get_unique_id(user);
              std::vector<unsigned short>::push_back((int)v7, (int)&unique_id);
            }
          }

_BOOL4 __cdecl CParty::CheckHellDungeonFreepassItemHaveAndDel(CParty *this, CUser *user, bool a3)
{
  GameWorld *v3; // eax
  CDataManager *v4; // eax
  char *v6; // ebx
  CInventory *CurCharacInvenR; // eax
  int id; // ebx
  CInventory *CurCharacInvenW; // eax
  char CurCharacNo; // bl
  int id; // [esp+3Ch] [ebp-2Ch] BYREF
  int id; // [esp+40h] [ebp-28h] BYREF
  int id; // [esp+44h] [ebp-24h] BYREF
  int id[4]; // [esp+48h] [ebp-20h] BYREF
  int id; // [esp+58h] [ebp-10h]
  char flag[4]; // [esp+5Ch] [ebp-Ch]

  v15 = 0;
  if ( !*((_DWORD *)this + 1562) )
  {
    v3 = G_GameWorld();
    *(_DWORD *)v16 = GameWorld::GetWorldMapIndex(v3, a2);
    if ( *(int *)v16 > 0 )
    {
      v4 = G_CDataManager();
      *((_DWORD *)this + 1562) = CWorldMapList::find_world_map((CDataManager *)((char *)v4 + 34688), *(_DWORD *)v16);
    }
    if ( !*((_DWORD *)this + 1562) )
      return 0;
  }
  std::vector<std::pair<int,int>>::begin(&v12, (_DWORD *)(*((_DWORD *)this + 1562) + 40));
  __gnu_cxx::__normal_iterator<std::pair<int,int> const*,std::vector<std::pair<int,int>>>::__normal_iterator<std::pair<int,int>*>(
    &v11,
    (int)&v12);
  while ( 1 )
  {
    std::vector<std::pair<int,int>>::end(&v13, *((_DWORD *)this + 1562) + 40);
    if ( !__gnu_cxx::operator!=<std::pair<int,int> const*,std::pair<int,int>*,std::vector<std::pair<int,int>>>(
            (int)&v11,
            (int)&v13) )
      break;
    v6 = *(char **)__gnu_cxx::__normal_iterator<std::pair<int,int> const*,std::vector<std::pair<int,int>>>::operator->((int)&v11);
    CurCharacInvenR = (CInventory *)CUserCharacInfo::getCurCharacInvenR(a2);
    v15 = CInventory::check_item_exist(CurCharacInvenR, v6);
    if ( v15 != -1 )
    {
      if ( !a3 )
        return v15 > 0;
      v8 = *(_DWORD *)(__gnu_cxx::__normal_iterator<std::pair<int,int> const*,std::vector<std::pair<int,int>>>::operator->((int)&v11)
                     + 4);
      CurCharacInvenW = (CInventory *)CUserCharacInfo::getCurCharacInvenW(a2);
      if ( (unsigned __int8)CInventory::delete_item(CurCharacInvenW, 1, v15, v8, 23, 1) == 1 )
      {
        CUser::SendUpdateItem(a2, 1, 0, v15);
        return v15 > 0;
      }
      __gnu_cxx::__normal_iterator<std::pair<int,int> const*,std::vector<std::pair<int,int>>>::operator->((int)&v11);
      CurCharacNo = CUserCharacInfo::getCurCharacNo(a2);
      cMyTrace::cMyTrace((cMyTrace *)v14, "bool CParty::CheckHellDungeonFreepassItemHaveAndDel(CUser*, bool)", 17659, 5);
      cMyTrace::operator()(
        (int)v14,
        "CParty::UseSealDoorItems, freepass delete_item failed , User ch=%d , %d %d",
        CurCharacNo);
    }
    __gnu_cxx::__normal_iterator<std::pair<int,int> const*,std::vector<std::pair<int,int>>>::operator++(&v11);
  }
  return v15 > 0;
}

CParty *__cdecl CParty::SetWeekendEvent(CParty *this, bool a2)
{
  *((_BYTE *)this + 6896) = a2;
  return this;
}

int __cdecl CParty::IsWeekendEvent(CParty *this)
{
  return *((unsigned __int8 *)this + 6896);
}

int __cdecl CParty::CheckMemberLastTryAssaultTime(CParty *this)
{
  int i; // [esp+10h] [ebp-18h]
  CUserCharacInfo *charac_info; // [esp+14h] [ebp-14h]
  int CurSec; // [esp+18h] [ebp-10h]

  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int8)CParty::checkValidUser((CUser **)this, i) == 1 )
    {
      v3 = (CUserCharacInfo *)*((_DWORD *)this + 6 * i + 30);
      CurSec = CSystemTime::getCurSec((CSystemTime *)GlobalData::s_systemTime_);
      if ( CUserCharacInfo::GetCurCharacChaosKillTime(v3) + 600 > CurSec )
        return 1;
    }
  }
  return 0;
}

void __usercall CParty::MakeTournamentReward(int count@<esi>, CParty *this, CTournamentDungeonReward *a3)
{
  int TournamentCurRound; // eax
  int dungeon_index; // eax
  const char *v5; // eax
  CDataManager *v6; // eax
  int ItemGenerator; // eax
  int Generator; // eax
  unsigned __int8 isHelpAbuseParty; // al
  void *v10[13]; // [esp+20h] [ebp-D8h] BYREF
  char flag; // [esp+54h] [ebp-A4h]
  char dungeon_diff; // [esp+55h] [ebp-A3h]
  char flag; // [esp+56h] [ebp-A2h]
  int count; // [esp+58h] [ebp-A0h]
  __int16 v15; // [esp+5Ch] [ebp-9Ch]
  __int16 v16; // [esp+5Eh] [ebp-9Ah]
  __int16 v17; // [esp+60h] [ebp-98h]
  __int16 v18; // [esp+62h] [ebp-96h]
  int count[12]; // [esp+68h] [ebp-90h] BYREF
  char flag[12]; // [esp+98h] [ebp-60h] BYREF
  int count; // [esp+A4h] [ebp-54h] BYREF
  int count; // [esp+A8h] [ebp-50h] BYREF
  PacketBuf *v23[3]; // [esp+ACh] [ebp-4Ch] BYREF
  int count; // [esp+B8h] [ebp-40h] BYREF
  char flag; // [esp+BFh] [ebp-39h] BYREF
  int count; // [esp+C0h] [ebp-38h] BYREF
  bool v27; // [esp+C7h] [ebp-31h]
  CDataManager *v28; // [esp+C8h] [ebp-30h]
  CDungeon *dungeon; // [esp+CCh] [ebp-2Ch]
  char *member_count; // [esp+D0h] [ebp-28h]
  int standard_level; // [esp+D4h] [ebp-24h]
  int count; // [esp+D8h] [ebp-20h]
  float v33; // [esp+DCh] [ebp-1Ch]
  unsigned int count; // [esp+E0h] [ebp-18h]
  int count; // [esp+E4h] [ebp-14h]
  int i; // [esp+E8h] [ebp-10h]
  int count; // [esp+ECh] [ebp-Ch]

  v27 = *((_BYTE *)this + 282);
  TournamentCurRound = CBattle_Field::GetTournamentCurRound((CParty *)((char *)this + 2852));
  CTournamentDungeonReward::MakeReward(
    (int)a3,
    *((_DWORD *)this + 811) + 2220,
    (_DWORD *)(*((_DWORD *)this + 811) + 2244),
    TournamentCurRound,
    v27);
  v28 = G_CDataManager();
  dungeon_index = CBattle_Field::get_dungeon_index((CParty *)((char *)this + 2852));
  dungeon = (CDungeon *)CDataManager::find_dungeon((CDungeonList **)v28, dungeon_index);
  member_count = (char *)CParty::get_member_count((CUser **)this);
  if ( dungeon )
  {
    if ( (int)member_count > 0 )
    {
      standard_level = CDungeon::get_standard_level(dungeon);
      v32 = standard_level;
      WongWork::stGenerateRefData_t::stGenerateRefData_t((WongWork::stGenerateRefData_t *)v10);
      dungeon_diff = CBattle_Field::get_dungeon_diff((CParty *)((char *)this + 2852));
      v11 = v32;
      v13 = (char)member_count;
      v14 = 1;
      v15 = *((_DWORD *)this + 221);
      v16 = *((_DWORD *)this + 219);
      v17 = 0;
      v18 = *((_DWORD *)this + 220);
      WongWork::stGenerateResult_t::stGenerateResult_t((WongWork::stGenerateResult_t *)v19);
      v6 = G_CDataManager();
      ItemGenerator = CDataManager::getItemGenerator(v6);
      Generator = WongWork::CItemGeneratorMgr::getGenerator(ItemGenerator, 2);
      (*(void (__cdecl **)(int, void **, int *))(*(_DWORD *)Generator + 12))(Generator, v10, v19);
      v33 = *((float *)dungeon + 565);
      if ( v33 < 0.0 )
        v33 = 1.0;
      CTournamentDungeonReward::SetRewardGold((int)a3, v19, v33);
      v34 = 0;
      v35 = CBattle_Field::GetTournamentCurRound((CParty *)((char *)this + 2852));
      if ( !v27 )
        --v35;
      if ( v35 < 0 )
        v35 = 0;
      v25 = v35;
      std::map<unsigned char,unsigned int>::find(&v24, (int)dungeon + 2264, &v25);
      std::map<unsigned char,unsigned int>::end(&v26, (int)dungeon + 2264);
      if ( std::_Rb_tree_const_iterator<std::pair<unsigned char const,unsigned int>>::operator!=(&v24, &v26) )
        v34 = *(_DWORD *)(std::_Rb_tree_const_iterator<std::pair<unsigned char const,unsigned int>>::operator->(&v24) + 4);
      for ( i = 0; i <= 3; ++i )
      {
        PacketGuard::PacketGuard((PacketGuard *)v23);
        InterfacePacketBuf::clear(v23);
        InterfacePacketBuf::put_header(v23, 0, 388);
        v37 = 0;
        if ( (unsigned __int8)CParty::_checkValidUser((CUser **)this, i) == 1 )
        {
          CUser::setChattingMessageCount(*((CUser **)this + 6 * i + 30), 0);
          isHelpAbuseParty = CParty::isHelpAbuseParty();
          CUserCharacInfo::calcHelpAbuseRatio(*((CUserCharacInfo **)this + 6 * i + 30), v34, isHelpAbuseParty);
          CUserCharacInfo::reset_level_before_dungeon(*((CUserCharacInfo **)this + 6 * i + 30));
          v22 = 0;
          v21 = 0;
          if ( (unsigned __int8)CUser::gain_exp_sp(
                                  a1,
                                  *((CUserCharacInfo **)this + 6 * i + 30),
                                  v34,
                                  &v22,
                                  &v21,
                                  2,
                                  0,
                                  0) )
          {
            PacketGuard::PacketGuard((PacketGuard *)v20);
            InterfacePacketBuf::clear((PacketBuf **)v20);
            InterfacePacketBuf::put_header((PacketBuf **)v20, 0, 2);
            InterfacePacketBuf::put_byte((PacketBuf **)v20, 1);
            InterfacePacketBuf::put_short((PacketBuf **)v20, 1);
            CUser::make_basic_info(a1, *((CUser **)this + 6 * i + 30), v20, 1);
            InterfacePacketBuf::finalize((PacketBuf **)v20, 1);
            CUser::Send(*((CUser **)this + 6 * i + 30), (PacketGuard *)v20);
            PacketGuard::~PacketGuard((PacketPool **)v20);
          }
          InterfacePacketBuf::put_byte(v23, v35);
          InterfacePacketBuf::put_int(v23, v34);
          if ( v27 )
            CParty::MakeQuestRewardPacket(this, v23, *((CUser **)this + 6 * i + 30));
          else
            InterfacePacketBuf::put_byte(v23, 0);
          CTournamentDungeonReward::MakeRewardPacket(a3, v23, v27);
          InterfacePacketBuf::finalize(v23, 1);
          CUser::Send(*((CUser **)this + 6 * i + 30), (PacketGuard *)v23);
        }
        PacketGuard::~PacketGuard(v23);
      }
      CParty::SetTournamentDungeonClearState(this, (CParty *)1);
      CParty::InsertTournamentDungeonTimer((int)this, 46);
      WongWork::stGenerateResult_t::~stGenerateResult_t((void **)v19);
      WongWork::stGenerateRefData_t::~stGenerateRefData_t(v10);
    }
    else
    {
      LogManager::logFormat(
        (LogManager *)1,
        (int)"party.cpp",
        "void CParty::MakeTournamentReward(CTournamentDungeonReward&)",
        (const char *)0x4545,
        "No Member[%d] in party",
        member_count);
    }
  }
  else
  {
    v5 = (const char *)CBattle_Field::get_dungeon_index((CParty *)((char *)this + 2852));
    LogManager::logFormat(
      (LogManager *)1,
      (int)"party.cpp",
      "void CParty::MakeTournamentReward(CTournamentDungeonReward&)",
      (const char *)0x4540,
      "Unknown Dungeon %d",
      v5);
  }
}

bool __cdecl CParty::GetValidPartyMember(CUser **a1, int *a2)
{
  bool result; // al
  int id[3]; // [esp+1Ch] [ebp-Ch] BYREF

  std::vector<int>::clear(a2);
  for ( v3[0] = 0; ; ++v3[0] )
  {
    result = v3[0] <= 3;
    if ( v3[0] > 3 )
      break;
    if ( (unsigned __int8)CParty::checkValidUser(a1, v3[0]) )
      std::vector<int>::push_back((int)a2, v3);
  }
  return result;
}

void __cdecl CParty::TournamentDungeonClear(CParty *this, bool a2)
{
  const char *dungeon_index; // eax
  Inven_Item *item; // ebx
  int j; // esi
  int id; // esi
  int PartyIndex; // ebx
  TimerQueue *v8; // eax
  int id; // eax
  const char *v10; // [esp+14h] [ebp-C4h]
  _BYTE v11[122]; // [esp+36h] [ebp-A2h] BYREF
  PacketBuf *v12[3]; // [esp+B0h] [ebp-28h] BYREF
  CDungeon *dungeon; // [esp+BCh] [ebp-1Ch]
  int id; // [esp+C0h] [ebp-18h]
  int i; // [esp+C4h] [ebp-14h]
  char ItemFromCard; // [esp+CBh] [ebp-Dh]
  int k; // [esp+CCh] [ebp-Ch]

  v13 = (CDungeon *)*((_DWORD *)this + 811);
  if ( v13 )
  {
    if ( !CDungeon::isTournamentDungeon(v13) )
    {
      dungeon_index = (const char *)CBattle_Field::get_dungeon_index((CParty *)((char *)this + 2852));
      LogManager::logFormat(
        (LogManager *)1,
        (int)"party.cpp",
        "void CParty::TournamentDungeonClear(bool)",
        (const char *)0x45C2,
        "Dungeon(%d) is can not call function",
        dungeon_index);
    }
    else
    {
      CParty::SaveCoin((CUser **)this);
      PacketGuard::PacketGuard((PacketGuard *)v12);
      for ( i = 0; i <= 3; ++i )
      {
        if ( (unsigned __int8)CParty::_checkValidUser((CUser **)this, i) == 1 )
        {
          v3 = (Inven_Item *)v11;
          for ( j = 1; j != -1; --j )
          {
            Inven_Item::Inven_Item(v3);
            v3 = (Inven_Item *)((char *)v3 + 61);
          }
          ItemFromCard = CTournamentDungeonReward::GetItemFromCard((int)this + 6076, 0, i, (Inven_Item *)v11, 2u);
          if ( ItemFromCard )
            CParty::_putItemIntoUser((CUser **)this, *((CUser **)this + 6 * i + 30), (Inven_Item *)v11, v12);
          if ( a2 )
          {
            for ( k = 0; k <= 1; ++k )
              Inven_Item::reset((Inven_Item *)&v11[61 * k]);
            ItemFromCard = CTournamentDungeonReward::GetItemFromCard((int)this + 6076, 1u, i, (Inven_Item *)v11, 2u);
            if ( ItemFromCard )
              CParty::_putItemIntoUser((CUser **)this, *((CUser **)this + 6 * i + 30), (Inven_Item *)v11, v12);
            HistoryLog::WriteDunClear(*(FILE **)(*((_DWORD *)this + 6 * i + 30) + 497400));
            CParty::OnSuccessedDungeonClear(-1, this, *((CUser ***)this + 6 * i + 30));
            if ( *((_DWORD *)this + 6 * i + 30)
              && (unsigned __int8)CUser::IsProperLevelDungeonUser(*((CUser **)this + 6 * i + 30)) )
            {
              CUser::ProcProperLevelDungeonClear(*((CUser **)this + 6 * i + 30));
            }
          }
          else
          {
            CParty::OnFailedDungeonClear(-1, this, *((CUser ***)this + 6 * i + 30));
          }
        }
      }
      CParty::SetEPLPState(this, 2);
      v14 = 60;
      if ( secretshop::SECRET_SHOP_DATA::IsOpen((CParty *)((char *)this + 676)) )
        v14 = 60 * *((_DWORD *)G_CDataManager() + 10721);
      v6 = CParty::gen_timer_key((int)this, 21);
      PartyIndex = CParty::GetPartyIndex(this);
      v8 = G_TimerQueue();
      TimerQueue::InsertTimer(v6, v8, 1, PartyIndex, 21, v14, v6, 0);
      CParty::set_state(this, 1);
      v9 = CParty::checkInoutConditionDungeon(PartyIndex, this);
      CParty::sendInoutConditionDungeon((CUser **)this, v9);
      CParty::CheckHackAverageHitCount(this);
      CParty::ClearMapHitCount(this);
      Secu_HackLogCheckByParty::dungeonClear((CParty *)((char *)this + 6844));
      PacketGuard::~PacketGuard(v12);
    }
  }
  else
  {
    LogManager::logFormat(
      (LogManager *)1,
      (int)"party.cpp",
      "void CParty::TournamentDungeonClear(bool)",
      (const char *)0x45BC,
      "Tournament Dungeon is NULL",
      v10);
  }
}

int __cdecl CParty::GetTournamentDungeonClearState(CParty *this)
{
  return *((unsigned __int8 *)this + 524);
}

CParty *__cdecl CParty::SetTournamentDungeonClearState(CParty *this, CParty *party)
{
  CParty *result; // eax

  result = a2;
  if ( (char)a2 <= 3 )
  {
    *((_BYTE *)this + 524) = (_BYTE)a2;
    return this;
  }
  return result;
}

int __cdecl CParty::InsertTournamentDungeonTimer(int id, int value)
{
  int result; // eax
  int id; // ebx
  int id; // esi
  TimerQueue *v5; // eax
  int id; // ebx
  int id; // esi
  TimerQueue *v8; // eax

  result = a2;
  if ( a2 == 46 )
  {
    v3 = CParty::gen_timer_key(a1, 46);
    v4 = *(_DWORD *)(a1 + 100);
    v5 = G_TimerQueue();
    return TimerQueue::InsertTimer(v4, v5, 1, v4, 46, 40, v3, 0);
  }
  else if ( a2 == 47 )
  {
    v6 = CParty::gen_timer_key(a1, 47);
    v7 = *(_DWORD *)(a1 + 100);
    v8 = G_TimerQueue();
    return TimerQueue::InsertTimer(v7, v8, 1, v7, 47, 4, v6, 0);
  }
  return result;
}

int __cdecl CParty::isTournamentVictory(CParty *this)
{
  return *((unsigned __int8 *)this + 282);
}

char __cdecl CParty::MakeQuestRewardPacket(CParty *this, PacketBuf **a2, CUser *user)
{
  int count; // ebx
  QuestList **v4; // eax
  int count; // ebx
  int dungeon_index; // eax
  int count; // eax
  _DWORD *v8; // eax
  int count; // edx
  int count; // ebx
  CDataManager *v11; // eax
  int *CurCharacInvenW; // eax
  int add_info; // eax
  char AbilityType; // al
  __int16 AbilityValue; // ax
  unsigned int count; // ebx
  CItemList **v17; // eax
  int grade; // ebx
  int rarity; // eax
  int count; // eax
  char result; // al
  __int64 v22; // [esp+5Fh] [ebp-89h] BYREF
  __int64 v23; // [esp+67h] [ebp-81h]
  int count[11]; // [esp+6Fh] [ebp-79h] BYREF
  char flag; // [esp+9Bh] [ebp-4Dh]
  _DWORD value[2]; // [esp+9Ch] [ebp-4Ch] BYREF
  void *v27[3]; // [esp+A4h] [ebp-44h] BYREF
  int index; // [esp+B0h] [ebp-38h] BYREF
  void *v29[3]; // [esp+B4h] [ebp-34h] BYREF
  int count; // [esp+C0h] [ebp-28h]
  int count; // [esp+C4h] [ebp-24h]
  int i; // [esp+C8h] [ebp-20h]
  char *CurCharacQuestRefR; // [esp+CCh] [ebp-1Ch]
  Quest *quest; // [esp+D0h] [ebp-18h]
  signed int j; // [esp+D4h] [ebp-14h]
  int inserted; // [esp+D8h] [ebp-10h]
  CItem *item; // [esp+DCh] [ebp-Ch]

  v30 = 0;
  v31 = 0;
  index = InterfacePacketBuf::get_index(a2);
  InterfacePacketBuf::put_byte(a2, 0);
  for ( i = 0; ; ++i )
  {
    result = i <= 19;
    if ( i > 19 )
      break;
    CurCharacQuestRefR = CUser::getCurCharacQuestRefR(a3);
    if ( *(_DWORD *)&CurCharacQuestRefR[4 * i + 30008] )
    {
      if ( *(_DWORD *)&CurCharacQuestRefR[4 * i + 30088] )
      {
        v3 = *(_DWORD *)&CurCharacQuestRefR[4 * i + 30008];
        v4 = (QuestList **)G_CDataManager();
        quest = (Quest *)CDataManager::find_quest(v4, v3);
        if ( quest )
        {
          v5 = *((__int16 *)CurCharacQuestRefR + 15085);
          dungeon_index = CBattle_Field::get_dungeon_index((CParty *)((char *)this + 2852));
          if ( (unsigned __int8)Quest::check_clear_item(quest, dungeon_index, v5) == 1 )
          {
            v31 = *(_DWORD *)&CurCharacQuestRefR[4 * i + 30008];
            if ( v31 > 0 )
            {
              std::vector<std::pair<int,int>>::vector(v27);
              std::pair<int,int>::pair(v26);
              if ( quest )
              {
                v7 = CBattle_Field::get_dungeon_index((CParty *)((char *)this + 2852));
                Quest::get_clear_itemlist((Quest *)v29, (int)quest, v7);
                std::vector<std::pair<int,int>>::operator=((int *)v27, v29);
                std::vector<std::pair<int,int>>::~vector(v29);
                for ( j = 0; ; ++j )
                {
                  v20 = std::vector<std::pair<int,int>>::size(v27);
                  if ( v20 <= j )
                    break;
                  Inven_Item::Inven_Item((Inven_Item *)&v22);
                  Inven_Item::reset((Inven_Item *)&v22);
                  v8 = (_DWORD *)std::vector<std::pair<int,int>>::at(v27, j);
                  v9 = v8[1];
                  v26[0] = *v8;
                  v26[1] = v9;
                  *(_DWORD *)((char *)&v22 + 2) = v26[0];
                  v10 = v9;
                  v11 = G_CDataManager();
                  CItemList::create_item(*((CItemList **)v11 + 3), 1, (Inven_Item *)&v22, v10);
                  CurCharacInvenW = (int *)CUserCharacInfo::getCurCharacInvenW(a3);
                  inserted = CInventory::insertItemIntoInventory(
                               CurCharacInvenW,
                               v22,
                               v23,
                               SHIDWORD(v23),
                               v24[0],
                               v24[1],
                               v24[2],
                               v24[3],
                               v24[4],
                               v24[5],
                               v24[6],
                               v24[7],
                               v24[8],
                               v24[9],
                               v24[10],
                               v25,
                               11,
                               1);
                  if ( inserted >= 0 )
                  {
                    InterfacePacketBuf::put_short(a2, inserted);
                    InterfacePacketBuf::put_int(a2, *(int *)((char *)&v22 + 2));
                    add_info = Inven_Item::get_add_info((Inven_Item *)&v22);
                    InterfacePacketBuf::put_int(a2, add_info);
                    InterfacePacketBuf::put_short(a2, *(__int16 *)((char *)&v23 + 3));
                    AbilityType = stAmplifyOption_t::getAbilityType((stAmplifyOption_t *)((char *)v24 + 1));
                    InterfacePacketBuf::put_byte(a2, AbilityType);
                    AbilityValue = stAmplifyOption_t::getAbilityValue((stAmplifyOption_t *)((char *)v24 + 1));
                    InterfacePacketBuf::put_short(a2, AbilityValue);
                    ++v30;
                    if ( BYTE1(v22) == 1 )
                    {
                      v16 = *(_DWORD *)((char *)&v22 + 2);
                      v17 = (CItemList **)G_CDataManager();
                      item = (CItem *)CDataManager::find_item(v17, v16);
                      if ( item )
                      {
                        grade = CItem::get_grade(item);
                        rarity = CItem::get_rarity(item);
                        CLuckPoint::UseLuckPoint(a3, (CUser *)BYTE1(v22), rarity, grade);
                      }
                    }
                  }
                }
              }
              std::vector<std::pair<int,int>>::~vector(v27);
            }
          }
        }
      }
    }
  }
  if ( v30 )
    return InterfacePacketBuf::put_byte(a2, &index, v30);
  return result;
}

int __cdecl CParty::GetPartyMemberForceDropFlag(CParty *this)
{
  int i; // [esp+18h] [ebp-10h]
  CUser *user; // [esp+1Ch] [ebp-Ch]

  for ( i = 0; i <= 3; ++i )
  {
    user = (CUser *)CParty::get_user(this, i);
    if ( user )
    {
      if ( (unsigned __int8)CUser::isGMUser(user) && (unsigned __int8)CUser::getForceDropFlag(user) )
        return 1;
    }
  }
  return 0;
}

int __cdecl CParty::getDungeonDropTestFlag(CParty *this)
{
  int i; // [esp+18h] [ebp-10h]
  CUser *user; // [esp+1Ch] [ebp-Ch]

  for ( i = 0; i <= 3; ++i )
  {
    user = (CUser *)CParty::get_user(this, i);
    if ( user )
    {
      if ( (unsigned __int8)CUser::isGMUser(user) && (unsigned __int8)CParty::getDungeonMapSaving(this) )
        return 1;
    }
  }
  return 0;
}

bool __cdecl CParty::resetMemberUseCoin(CParty *this)
{
  bool result; // al
  int i; // [esp+Ch] [ebp-4h]

  for ( i = 0; ; ++i )
  {
    result = i <= 3;
    if ( i > 3 )
      break;
    *((_DWORD *)this + 6 * i + 35) = 0;
  }
  return result;
}

int __cdecl CParty::getDungeonTypeKind(CParty *this, const CDungeon *dungeon)
{
  if ( !a2 )
    return 8;
  if ( *((_BYTE *)a2 + 2204) )
    return 1;
  if ( (char)CDungeon::get_dimension_possible(a2) > 0 )
    return 2;
  if ( CDungeon::getDungeonKind(a2) == 1 )
    return 3;
  if ( (unsigned __int8)CDungeon::get_blood_dungeon_type(a2) == 1 )
    return 4;
  if ( (unsigned __int8)CDungeon::get_blood_dungeon_type(a2) == 2 )
    return 5;
  if ( CDungeon::isTowerOfDespairDungeon(a2) )
    return 6;
  if ( CDungeon::isTournamentDungeon(a2) )
    return 7;
  return 0;
}

int __cdecl CParty::addDungeonClear(CParty *this, const CDungeon *dungeon, int id, int value)
{
  int DungeonTypeKind; // eax
  int i; // [esp+1Ch] [ebp-Ch]

  for ( i = 0; i <= 3; ++i )
  {
    if ( (unsigned __int16)CUser::get_unique_id(*((CUser **)this + 6 * a4 + 30)) == *((_WORD *)this + 4 * i + 3076) )
    {
      DungeonTypeKind = CParty::getDungeonTypeKind(this, a2);
      if ( DungeonTypeKind == 1 )
      {
        if ( *((unsigned __int16 *)G_CDataManager() + 10391) <= a3
          && *((int *)this + 2 * i + 1539) <= 30
          && !*((_DWORD *)this + 6 * a4 + 35) )
        {
          return CUser::AddDungeonClear(*((CUser **)this + 6 * a4 + 30), *((_DWORD *)this + 814), 3);
        }
      }
      else if ( DungeonTypeKind == 2 )
      {
        if ( *((unsigned __int16 *)G_CDataManager() + 10391) <= a3
          && *((int *)this + 2 * i + 1539) <= 50
          && !*((_DWORD *)this + 6 * a4 + 35) )
        {
          return CUser::AddDungeonClear(*((CUser **)this + 6 * a4 + 30), *((_DWORD *)this + 814), 3);
        }
      }
      else
      {
        if ( DungeonTypeKind )
          return 0;
        if ( *((unsigned __int16 *)G_CDataManager() + 10390) <= a3
          && *((int *)this + 2 * i + 1539) <= 10
          && !*((_DWORD *)this + 6 * a4 + 35) )
        {
          return CUser::AddDungeonClear(*((CUser **)this + 6 * a4 + 30), *((_DWORD *)this + 814), 3);
        }
      }
    }
  }
  return 0;
}

void __cdecl CParty::SendPartyMemberTagChracInfo(CParty *this)
{
  int index; // [esp+14h] [ebp-24h] BYREF
  PacketBuf *v2[3]; // [esp+18h] [ebp-20h] BYREF
  int id; // [esp+24h] [ebp-14h]
  int i; // [esp+28h] [ebp-10h]
  CUser *user; // [esp+2Ch] [ebp-Ch]

  PacketGuard::PacketGuard((PacketGuard *)v2);
  InterfacePacketBuf::clear(v2);
  InterfacePacketBuf::put_header(v2, 0, 414);
  index = InterfacePacketBuf::get_index(v2);
  InterfacePacketBuf::put_short(v2, 0);
  v3 = 0;
  for ( i = 0; i <= 3; ++i )
  {
    user = (CUser *)CParty::get_user(this, i);
    if ( user && (unsigned __int8)CUser::MakeTagCharacInfo(user, v2) )
      ++v3;
  }
  InterfacePacketBuf::put_short(v2, &index, v3);
  InterfacePacketBuf::finalize(v2, 1);
  CParty::send_to_party((CUser **)this, (PacketGuard *)v2);
  PacketGuard::~PacketGuard(v2);
}

bool __cdecl CParty::setBestClearTime(CParty *this, char flag, int value, int value2)
{
  BestClearTime *CharacExpandData; // eax
  bool result; // al
  int i; // [esp+1Ch] [ebp-Ch]

  for ( i = 0; ; ++i )
  {
    result = i <= 3;
    if ( i > 3 )
      break;
    if ( (unsigned __int8)CParty::checkValidUser((CUser **)this, i) == 1 )
    {
      CharacExpandData = (BestClearTime *)CUser::GetCharacExpandData(*((_DWORD *)this + 6 * i + 30), 0x12u);
      BestClearTime::setBestClearTime(CharacExpandData, a2, a3, a4);
    }
  }
  return result;
}

bool __cdecl CParty::makeBestClearTimePacket(CUser **this, PacketBuf **a2, int count)
{
  char flag; // al
  char member_count; // al
  int dungeon_diff; // esi
  char dungeon_index; // bl
  BestClearTime *CharacExpandData; // eax
  BestClearTime *v8; // eax
  __int16 unique_id; // ax
  bool result; // al
  int BestClearTime; // [esp+14h] [ebp-14h]
  char flag; // [esp+1Bh] [ebp-Dh]
  int i; // [esp+1Ch] [ebp-Ch]

  v3 = (unsigned __int8)CBattle_Field::isQuestMaze((CBattle_Field *)(this + 713)) != 0;
  InterfacePacketBuf::put_byte(a2, v3);
  member_count = CParty::get_member_count(this);
  InterfacePacketBuf::put_byte(a2, member_count);
  for ( i = 0; ; ++i )
  {
    result = i <= 3;
    if ( i > 3 )
      break;
    if ( (unsigned __int8)CParty::checkValidUser(this, i) == 1 )
    {
      dungeon_diff = CBattle_Field::get_dungeon_diff((CBattle_Field *)(this + 713));
      dungeon_index = CBattle_Field::get_dungeon_index((CBattle_Field *)(this + 713));
      CharacExpandData = (BestClearTime *)CUser::GetCharacExpandData((int)*(this + 6 * i + 30), 0x12u);
      BestClearTime = BestClearTime::getBestClearTime(CharacExpandData, dungeon_index, dungeon_diff);
      v8 = (BestClearTime *)CUser::GetCharacExpandData((int)*(this + 6 * i + 30), 0x12u);
      v12 = BestClearTime::checkNewRecord(v8);
      if ( !BestClearTime )
        BestClearTime = a3;
      unique_id = CUser::get_unique_id(*(this + 6 * i + 30));
      InterfacePacketBuf::put_short(a2, unique_id);
      InterfacePacketBuf::put_int(a2, BestClearTime);
      InterfacePacketBuf::put_byte(a2, v12 != 0);
    }
  }
  return result;
}

_BOOL4 __cdecl CParty::checkFreeRevivalCondition(CParty *this, const CDungeon *dungeon)
{
  CDungeonList **v3; // eax
  int dungeon_diff; // eax
  int id; // eax
  CDimensionActivationEvent *RepeatEvent; // [esp+18h] [ebp-10h]
  int dungeon_index; // [esp+1Ch] [ebp-Ch]

  if ( *((_DWORD *)this + 822) == 1 )
    return 1;
  if ( !dungeon )
  {
    dungeon_index = CBattle_Field::get_dungeon_index((CParty *)((char *)this + 2852));
    v3 = (CDungeonList **)G_CDataManager();
    dungeon = (const CDungeon *)CDataManager::find_dungeon(v3, dungeon_index);
    if ( !dungeon )
      return 0;
  }
  dungeon_diff = CBattle_Field::get_dungeon_diff((CParty *)((char *)this + 2852));
  if ( CDungeon::checkFreeRevivalCondition(dungeon, dungeon_diff) )
    return 1;
  if ( (char)CDungeon::get_dimension_possible(dungeon) <= 0 )
    return 0;
  RepeatEvent = (CDimensionActivationEvent *)CEventManager::GetRepeatEvent(GlobalData::s_event_manager, 94);
  if ( !RepeatEvent )
    return 0;
  if ( (*(unsigned __int8 (__cdecl **)(CDimensionActivationEvent *, _DWORD))(*(_DWORD *)RepeatEvent + 52))(
         RepeatEvent,
         0) != 1 )
    return 0;
  v5 = CBattle_Field::get_dungeon_diff((CParty *)((char *)this + 2852));
  return CDimensionActivationEvent::checkFreeRevivalCondition(RepeatEvent, v5);
}

_BOOL4 __cdecl CParty::checkUnLimitUsedCoinCondition(CParty *this, const CDungeon *dungeon)
{
  CDungeonList **v2; // eax
  int RepeatEvent; // eax
  int dungeon_index; // [esp+1Ch] [ebp-Ch]

  if ( !dungeon )
  {
    dungeon_index = CBattle_Field::get_dungeon_index((CParty *)((char *)this + 2852));
    v2 = (CDungeonList **)G_CDataManager();
    dungeon = (const CDungeon *)CDataManager::find_dungeon(v2, dungeon_index);
    if ( !dungeon )
      return 0;
  }
  RepeatEvent = CEventManager::GetRepeatEvent(GlobalData::s_event_manager, 94);
  return (*(unsigned __int8 (__cdecl **)(int, _DWORD))(*(_DWORD *)RepeatEvent + 52))(RepeatEvent, 0)
      && (char)CDungeon::get_dimension_possible(dungeon) > 0;
}

int __cdecl CParty::GetPartyMemberCoinLimit(CParty *this)
{
  return *((_DWORD *)this + 202);
}

CParty *__cdecl CParty::SetPartyMemberCoinLimit(CParty *this, int value)
{
  *((_DWORD *)this + 202) = a2;
  return this;
}

int __cdecl CParty::checkLimitUsedCoinCondition(CParty *this, const CDungeon *dungeon)
{
  CDungeonList **v2; // eax
  int UsedCoinCount; // ebx
  int dungeon_diff; // eax
  int id; // eax
  int id; // [esp+18h] [ebp-10h] BYREF
  char flag[4]; // [esp+1Ch] [ebp-Ch]

  if ( !dungeon )
  {
    *(_DWORD *)v8 = CBattle_Field::get_dungeon_index((CParty *)((char *)this + 2852));
    v2 = (CDungeonList **)G_CDataManager();
    dungeon = (const CDungeon *)CDataManager::find_dungeon(v2, *(int *)v8);
    if ( !dungeon )
      return 19;
  }
  if ( CParty::GetPartyMemberCoinLimit(this) )
  {
    UsedCoinCount = CParty::GetUsedCoinCount(this);
    if ( UsedCoinCount >= CParty::GetPartyMemberCoinLimit(this) )
      return 22;
  }
  v7 = 0;
  dungeon_diff = CBattle_Field::get_dungeon_diff((CParty *)((char *)this + 2852));
  if ( (unsigned __int8)CDungeon::getLimitCoinDiff(dungeon, dungeon_diff, &v7) )
  {
    if ( v7 != -1 )
    {
      v6 = CParty::GetUsedCoinCount(this);
      if ( v6 >= v7 )
        return 22;
    }
  }
  if ( *((_DWORD *)dungeon + 543) != -1 && CParty::GetUsedCoinCount(this) >= *((_DWORD *)dungeon + 543) )
    return 22;
  if ( *((_DWORD *)dungeon + 550) == -1 || CParty::CheckHellCoinLimint(this, *((_DWORD *)dungeon + 550)) )
    return 0;
  return 22;
}

_BOOL4 __cdecl CParty::isDungeonDifficultyInfromNotice(CUserCharacInfo **this, bool a2)
{
  CUser *Manager; // eax

  if ( (unsigned __int8)CUserCharacInfo::getMemberDungeonFlag(*(this + 29)) )
    return 0;
  if ( !CUserCharacInfo::getCurCharacTutorialFlag(*(this + 29)) )
    return 0;
  if ( a2 )
    return 0;
  if ( *((_WORD *)this + 1574) )
    return 0;
  Manager = (CUser *)CParty::getManager((CParty *)this);
  return CUser::getMoveSpace(Manager) != 1;
}

_BOOL4 __cdecl CParty::checkDungeonDifficulty(CUser **this, char flag, const CDungeon *dungeon)
{
  CUser *Manager; // eax

  if ( !a3 )
    return 0;
  if ( !a2 )
    return 0;
  if ( (unsigned __int8)CUser::IsGameMasterMode(*(this + 29)) )
    return 0;
  if ( CDungeon::isTournamentDungeon(a3) )
    return 0;
  Manager = (CUser *)CParty::getManager((CParty *)this);
  return CUser::getMoveSpace(Manager) != 1 && (unsigned __int8)CDungeon::isRiskDungeon(a3) == 0;
}

_BOOL4 __cdecl CParty::IsUseFatigueDungeon(CParty *this)
{
  _BOOL4 result; // eax

  result = 0;
  if ( *((_DWORD *)this + 822) != 1
    && *((_DWORD *)this + 811)
    && !*(_BYTE *)(*((_DWORD *)this + 811) + 2207)
    && *(_BYTE *)(*((_DWORD *)this + 811) + 2170) != 1
    && (unsigned __int8)CDungeon::isNoFatigueDungeon(*((CDungeon **)this + 811)) != 1 )
  {
    return 1;
  }
  return result;
}

int __cdecl CParty::CheckMapClear(CParty *this)
{
  return CBattle_Field::check_grid_clear((CParty *)((char *)this + 2852));
}

void *__cdecl CParty::reset_recv_flag(CParty *this)
{
  return CPartyResultRecvFlag::Clear((CParty *)((char *)this + 476));
}

int __cdecl CParty::IsPremiumGoldCardParty(CParty *this)
{
  return *((unsigned __int8 *)this + 796);
}

CParty *__cdecl CParty::SetPremiumGoldCardParty(CParty *this)
{
  *((_BYTE *)this + 796) = 1;
  return this;
}

CParty *__cdecl CParty::ResetPremiumGoldCardParty(CParty *this)
{
  *((_BYTE *)this + 796) = 0;
  return this;
}

int __cdecl CParty::GetPremiumGoldCardDefaultItem(CParty *this)
{
  return *((_DWORD *)this + 200);
}

CParty *__cdecl CParty::SetPremiumGoldCardDefaultItem(CParty *this, unsigned int value)
{
  *((_DWORD *)this + 200) = a2;
  return this;
}

CParty *__cdecl CParty::ResetPremiumGoldCardDefaultItem(CParty *this)
{
  *((_DWORD *)this + 200) = 0;
  return this;
}

CParty *__cdecl CParty::setStartGamePartyCount(CParty *this, int value)
{
  *((_DWORD *)this + 168) = a2;
  return this;
}

int __cdecl CParty::getStartGamePartyCount(CParty *this)
{
  return *((_DWORD *)this + 168);
}

char *__cdecl CParty::setTitle(CParty *this, char *src)
{
  return strncpy((char *)this + 284, src, SIZE_32_BYTES);
}

CParty *__cdecl CParty::setTitleIndex(CParty *this, unsigned __int8 a2)
{
  *((_BYTE *)this + 283) = a2;
  return this;
}

CParty *__cdecl CParty::setUserMax(CParty *this, unsigned __int8 a2)
{
  *((_BYTE *)this + 317) = a2;
  return this;
}

  return CParty::OnDungeonClear(a1, this, a3, 0);
}

//----- (085BF7FC) --------------------------------------------------------
int __usercall CParty::OnSuccessedDungeonClear@<eax>(int value@<esi>, CParty *this, CUser **a3)
{

  return CParty::OnDungeonClear(a1, this, a3, 1);
}

//----- (085BF81E) --------------------------------------------------------
int __cdecl CParty::GetAssaultState(CParty *this)
{

bool __cdecl CParty::SetEndRouting(CParty *this)
{
  return CParty::CItemRoutingData::Reset(this);
}

CParty *__cdecl CParty::SetUsedCoinCount(CParty *this, int value)
{
  *((_DWORD *)this + 1560) = a2;
  return this;
}

CParty *__cdecl CParty::ClearMapHitCount(CParty *this)
{
  *((_DWORD *)this + 1565) = 0;
  *((_DWORD *)this + 1566) = 0;
  memset((char *)this + 6268, 0, 0xB4u);
  *((_DWORD *)this + 1612) = 0;
  memset((char *)this + 6452, 0, 0xB4u);
  *((_DWORD *)this + 1658) = 0;
  memset((char *)this + 6636, 0, 0xB4u);
  *((_BYTE *)this + 6816) = 0;
  memset((char *)this + 6820, 0, SIZE_16_BYTES);
  *((_DWORD *)this + 1709) = 0;
  *((_DWORD *)this + 1710) = 0;
  return this;
}

int __cdecl CParty::getDungeonMapSaving(CParty *this)
{
  return *((unsigned __int8 *)this + 6897);
}

      user = (expert_job::CAlchemist *)CParty::get_user(Party, i);
      if ( user && user != this && expert_job::CAlchemist::IsExistContinuousEffectItem(user, (CUser *)0x27AC4A) )

        user = (CUser *)CParty::get_user(Party, i);
        if ( user )

  if ( CParty::IsRoutingState(Party) )
    return 18;

  CParty::SetStartRouting(Party);
  CParty::SetRoutingItem(Party, a3, a4);

  CParty::send_to_party((CUser **)Party, (PacketGuard *)v9);
  v5 = 0;

  if ( (unsigned __int8)CParty::GetMemberRoutingState(a3, a2) )
    return 0;

      CParty::SetMemberRoutingState(a3, a2, 2);
  }
  else
  {

    CParty::SetMemberRoutingState(a3, a2, 1);
  }
  if ( (unsigned __int8)CParty::HasAllMemberSelection((CUser **)a3) )
  {

  if ( CParty::IsRoutingState(a2)
    && ((FirstValidMemberState = CParty::GetFirstValidMemberState((CUser **)a2), FirstValidMemberState == 5)

    CParty::SetRoutingTimerOver(a2);
    if ( (unsigned __int8)CParty::HasAllMemberSelection((CUser **)a2) )

    CParty::SetEndRouting(a2);
    return 1;

  RoutingInvenItem = CParty::GetRoutingInvenItem(a2);
  if ( !*(_DWORD *)(RoutingInvenItem + 2)

    || (RoutingItemID = CParty::GetRoutingItemID(a2),
        v2 = (CItemList **)G_CDataManager(),

  CParty::GetRoutingDiceMemberList((CUserCharacInfo **)a2, v19, v25);
  user = 0;

    RoutingItemUniqueID = CParty::GetRoutingItemUniqueID(a2);
    InterfacePacketBuf::put_int(v18, RoutingItemUniqueID);

    user = (CUserCharacInfo *)CParty::get_user(a2, v15);
    goto LABEL_11;

  user = (CUserCharacInfo *)CParty::get_user(a2, v27);
LABEL_15:

      v8 = CParty::GetRoutingItemUniqueID(a2);
      InterfacePacketBuf::put_int(v18, v8);

      *(_DWORD *)v13 = CParty::GetRoutingItemUniqueID(a2);
      v32 = 0;

        v32 = CParty::DeleteDungeonDropItem(a2, *(int *)v13);
      }
      else if ( state == 12 )
      {

  CParty::send_to_party((CUser **)a2, (PacketGuard *)v18);
  CParty::SetEndRouting(a2);

    if ( CParty::IsRoutingState(a2)
      && (unsigned __int8)CParty::HasAllMemberSelection((CUser **)a2)

      && ((unsigned __int8)CParty::GetFirstValidMemberState((CUser **)a2) == 5
       || (unsigned __int8)CParty::GetFirstValidMemberState((CUser **)a2) == 12) )

    if ( CParty::IsRoutingState(a2)
      && ((unsigned __int8)CParty::GetFirstValidMemberState((CUser **)a2) == 5

       || (unsigned __int8)CParty::GetFirstValidMemberState((CUser **)a2) == 12) )
    {
      PacketGuard::PacketGuard((PacketGuard *)v5);
      InterfacePacketBuf::put_header(v5, 0, 282);
      InterfacePacketBuf::put_byte(v5, 0);
      InterfacePacketBuf::put_byte(v5, a3);
      RoutingItemUniqueID = CParty::GetRoutingItemUniqueID(a2);
      InterfacePacketBuf::put_int(v5, RoutingItemUniqueID);
      InterfacePacketBuf::finalize(v5, 1);
      CParty::send_to_party((CUser **)a2, (PacketGuard *)v5);
      CSpecialItemRoutingManager::cancelTimer(this, a2);
      CParty::SetEndRouting(a2);
      PacketGuard::~PacketGuard(v5);
    }

  v2 = CParty::gen_timer_key((int)a2, 43);
  PartyIndex = CParty::GetPartyIndex(a2);

  return CParty::gen_timer_key((int)a2, 43);
}

//----- (0860B4DD) --------------------------------------------------------
int __cdecl __static_initialization_and_destruction_0(int id, int value)
{

unsigned __int8 __cdecl CParty::SetStartRouting(CParty *this)
{
  unsigned __int8 result; // al

  result = (unsigned __int8)CParty::CItemRoutingData::GetRoutingState(this) != 1;
  if ( result )
    return (unsigned __int8)CParty::CItemRoutingData::SetRoutingState(this, 1);
  return result;
}

CParty::CItemRoutingData *__cdecl CParty::SetRoutingItem(CParty *this, Inven_Item *item, unsigned int value)
{
  return CParty::CItemRoutingData::SetRoutingItemIndex(this, a2, a3);
}

int __cdecl CParty::GetRoutingItemID(CParty *this)
{
  return CParty::CItemRoutingData::GetRoutingItemIndex(this);
}

char *__cdecl CParty::GetRoutingInvenItem(CParty *this)
{
  return CParty::CItemRoutingData::GetRoutingInvenItem(this);
}

int __cdecl CParty::GetRoutingItemUniqueID(CParty *this)
{
  return CParty::CItemRoutingData::GetRoutingItemUniqueID(this);
}

int __cdecl CParty::DeleteDungeonDropItem(CParty *this, int slot)
{
  return CBattle_Field::pickup_item((CParty *)((char *)this + 2852), a2);
}

  if ( !CParty::check_timer_key((int)Party, 13, a4) )
    return 0;

  CParty::dungeon_clear(a1, Party);
  return 1;

  if ( !CParty::check_timer_key((int)Party, 14, a4) )
    return 0;

  CParty::party_dungeon_fail(a1, Party);
  return 1;

  if ( !CParty::check_timer_key((int)Party, 11, a4) )
    return 0;

  CParty::map_load_forced_finish(a1, Party);
  return 1;

  if ( !CParty::check_timer_key((int)Party, 12, a3) )
    return 0;

  CParty::forced_set_play_result(Party);
  return 1;

  if ( !CParty::check_timer_key((int)Party, 15, a3) )
    return 0;

  return (unsigned __int8)CParty::OnTimerWalkOutLackUser(Party) == 1;
}

//----- (08633E86) --------------------------------------------------------
int __cdecl TimerPVPWalkOutLackUser::dispatch_sig(TimerPVPWalkOutLackUser *this, int id, int id2)
{

  if ( !CParty::check_timer_key((int)Party, 16, a4) )
    return 0;

  if ( !(unsigned __int8)CParty::get_dungeon_clear_state(Party)
    || (unsigned __int8)CParty::get_dungeon_clear_state(Party) == 5 )

  if ( (unsigned __int8)CParty::IsCompleteCardSelectOfMembers((int)Party, 0) != 1 )
  {
    CParty::CompleteCardSelect(Party);
    CParty::SendEachCardInfo(Party);
  }

  CParty::set_dungeon_clear_state(Party, (CParty *)5);
  CParty::dungeon_clear(a1, (CUser **)Party);

  if ( !CParty::check_timer_key((int)Party, 17, a3) )
    return 0;

  if ( (unsigned __int8)CParty::get_dungeon_clear_state(Party) == 2 )
  {
    PacketGuard::PacketGuard((PacketGuard *)v5);
    InterfacePacketBuf::clear(v5);
    InterfacePacketBuf::put_header(v5, 1, 72);
    InterfacePacketBuf::put_byte(v5, 1);
    InterfacePacketBuf::finalize(v5, 1);
    CParty::send_to_party((CUser **)Party, (PacketGuard *)v5);
    CParty::set_dungeon_clear_state(Party, (CParty *)3);
    CParty::insert_time_dungeon_score_scroll_msg(Party, 30);
    PacketGuard::~PacketGuard(v5);
  }

  if ( !CParty::check_timer_key((int)Party, 18, a3) )
    return 0;

  if ( (unsigned __int8)CParty::get_dungeon_clear_state(Party) == 3 )
  {
    PacketGuard::PacketGuard((PacketGuard *)v8);
    InterfacePacketBuf::put_header(v8, 1, 73);
    InterfacePacketBuf::put_byte(v8, 1);
    for ( i = 0; i <= 3; ++i )
    {
      if ( CParty::get_user(Party, i) )
      {
        user = (CUser *)CParty::get_user(Party, i);
        CurCharacTotalFatigue = CUser::getCurCharacTotalFatigue(user);
        v7 = (CUser *)CParty::get_user(Party, i);
        if ( CurCharacTotalFatigue == (unsigned __int16)CUser::getCurCharacTotalMaxFatigue(v7) )
          InterfacePacketBuf::put_short(v8, 0);
        else
          InterfacePacketBuf::put_short(v8, 1);
      }
      else
      {
        InterfacePacketBuf::put_short(v8, -1);
      }
    }
    InterfacePacketBuf::finalize(v8, 1);
    CParty::send_to_party((CUser **)Party, (PacketGuard *)v8);
    CParty::set_dungeon_clear_state(Party, (CParty *)4);
    CParty::insert_time_dungeon_score_scroll_msg(Party, 30);
    CParty::insert_time_card_select_msg(Party, 4);
    PacketGuard::~PacketGuard(v8);
  }

  if ( !CParty::check_timer_key((int)Party, 19, a3) )
    return 0;

  if ( (unsigned __int8)CParty::get_dungeon_clear_state(Party) == 4 )
  {
    CParty::set_dungeon_clear_state(Party, (CParty *)5);
    CParty::insert_time_dungeon_clear_msg(Party, 5);
  }

  if ( !CParty::check_timer_key((int)Party, 20, a4) )
    return 0;

  CParty::ReturnToVillage(a1, Party);
  return 1;

  if ( !CParty::check_timer_key((int)Party, 21, a4) )
    return 0;

  if ( (unsigned __int8)CParty::GetSelectedEPLPCmd(Party) == 3 )
    return 0;

  if ( (unsigned __int8)CParty::GetEPLPState(Party) == 2 )
    CParty::ReturnToVillage(a1, Party);

  if ( !CParty::check_timer_key(Party, 22, a3) )
    return 0;

  if ( !CParty::check_timer_key(Party, 23, a3) )
    return 0;

  if ( !CParty::check_timer_key(Party, 24, a3) )
    return 0;

  if ( !CParty::check_timer_key(Party, 25, a3) )
    return 0;

  if ( !CParty::check_timer_key(Party, 26, a3) )
    return 0;

  if ( !CParty::check_timer_key(Party, 27, a3) )
    return 0;

  if ( !CParty::check_timer_key(Party, 28, a3) )
    return 0;

  if ( !CParty::check_timer_key(Party, 29, a3) )
    return 0;

  if ( !CParty::check_timer_key(Party, 30, a3) )
    return 0;

  if ( !CParty::check_timer_key(Party, 31, a3) )
    return 0;

  if ( !CParty::check_timer_key(Party, 32, a3) )
    return 0;

  if ( !CParty::check_timer_key(Party, 33, a3) )
    return 0;

  if ( !CParty::check_timer_key((int)Party, 41, a3) )
    return 0;

  Manager = (CUser *)CParty::getManager(Party);
  CParty::SetPlayResult(Party, Manager);

    return CParty::OnTimeoutBossDie(Party, a3, a4);
  else

  if ( !CParty::check_timer_key(Party, 40, a3) )
    return 0;

  if ( !CParty::check_timer_key((int)Party, a5, a4) )
    return 0;

  return (unsigned __int8)CParty::onBloodRewardTimer(a1, Party, a5) == 1;
}

//----- (08638D2C) --------------------------------------------------------
int __cdecl TimerUltimateNextRoundStart::dispatch_sig(
        TimerUltimateNextRoundStart *this,
        int id,
        int id,
        unsigned int id)
{

  if ( !CParty::check_timer_key((int)Party, a4, a3) )
    return 0;

  if ( !CParty::check_timer_key((int)Party, 39, a4) )
    return 0;

  CParty::OnFinishVillageMonsterFighting(a1, (CParty *)Party, 0, 1);
  return 1;

  if ( !CParty::check_timer_key((int)Party, 43, a3) )
    return 0;

      CParty::send_to_party((CUser **)Party, (PacketGuard *)v10);
    else

    if ( (unsigned __int8)CParty::IsReturnUserParty((CUser **)Party) != 1
      || (unsigned __int8)CParty::IsEventCharacParty((CUserCharacInfo **)Party) != 1 )

  if ( !CParty::check_timer_key((int)Party, a4, a3) )
    return 0;

  return (unsigned __int8)CParty::OnTODRewardTimer(Party, a4) == 1;
}

//----- (0863A916) --------------------------------------------------------
int __usercall Timer_OneADayItemShop::dispatch_sig@<eax>(char flag@<sil>)
{

  CParty::send_to_party((CUser **)party_object, (PacketGuard *)v6);
  v2 = 0;

  if ( !CParty::check_timer_key((int)Party, a5, a4) )
    return 0;

  CParty::send_quick_party_matching_result((CUser **)Party, 0, 0);
  CParty::set_quick_party(Party, 0);

  Manager = (CUserCharacInfo *)CParty::getManager(Party);
  CurCharacName = (unsigned __int8)CUserCharacInfo::getCurCharacName(Manager);

    if ( (unsigned __int8)CParty::checkValidUser((CUser **)Party, i) == 1 )
    {
      user = (CUser *)CParty::get_user(Party, i);
      v16 = CParty::leave_user((std::string **)Party, (std::string *)user, 2);
      if ( v16 != 1 )
      {
        v9 = G_GameWorld();
        GameWorld::out_from_dungeon(v9, user);
        PacketGuard::PacketGuard((PacketGuard *)v11);
        InterfacePacketBuf::clear((PacketBuf **)v11);
        InterfacePacketBuf::put_header((PacketBuf **)v11, 0, 2);
        InterfacePacketBuf::put_byte((PacketBuf **)v11, 0);
        InterfacePacketBuf::put_short((PacketBuf **)v11, 1);
        CUser::make_basic_info(a1, user, v11, 0);
        InterfacePacketBuf::finalize((PacketBuf **)v11, 1);
        v10 = G_GameWorld();
        GameWorld::send_all(v10, (PacketGuard *)v11, user);
        PacketGuard::~PacketGuard((PacketPool **)v11);
      }
    }

  if ( !CParty::check_timer_key((int)Party, 46, a3) )
    return 0;

  if ( (unsigned __int8)CParty::GetTournamentDungeonClearState(Party) == 1 )
  {
    PacketGuard::PacketGuard((PacketGuard *)v8);
    InterfacePacketBuf::clear(v8);
    InterfacePacketBuf::put_header(v8, 1, 458);
    InterfacePacketBuf::put_byte(v8, 1);
    for ( i = 0; i <= 3; ++i )
    {
      if ( CParty::get_user(Party, i) )
      {
        user = (CUser *)CParty::get_user(Party, i);
        CurCharacTotalFatigue = CUser::getCurCharacTotalFatigue(user);
        v7 = (CUser *)CParty::get_user(Party, i);
        if ( CurCharacTotalFatigue == (unsigned __int16)CUser::getCurCharacTotalMaxFatigue(v7) )
          InterfacePacketBuf::put_byte(v8, 0);
        else
          InterfacePacketBuf::put_byte(v8, 1);
      }
      else
      {
        InterfacePacketBuf::put_byte(v8, -1);
      }
    }
    InterfacePacketBuf::finalize(v8, 1);
    CParty::send_to_party((CUser **)Party, (PacketGuard *)v8);
    CParty::SetTournamentDungeonClearState(Party, (CParty *)2);
    CParty::InsertTournamentDungeonTimer((int)Party, 47);
    PacketGuard::~PacketGuard(v8);
  }

  if ( !CParty::check_timer_key((int)Party, 47, a3) )
    return 0;

  if ( (unsigned __int8)CParty::GetTournamentDungeonClearState(Party) == 2 )
  {
    PacketGuard::PacketGuard((PacketGuard *)v6);
    InterfacePacketBuf::clear(v6);
    InterfacePacketBuf::put_header(v6, 1, 459);
    InterfacePacketBuf::put_byte(v6, 1);
    v8 = (CParty *)((char *)Party + 6076);
    std::vector<int>::vector(v5);
    CParty::GetValidPartyMember((CUser **)Party, v5);
    isTournamentVictory = CParty::isTournamentVictory(Party);
    if ( (unsigned __int8)CTournamentDungeonReward::isCardSelectComplete((int)v8, 2, v5) != 1 )
      CTournamentDungeonReward::ForceSelectCard((int)v8, v5, isTournamentVictory);
    CTournamentDungeonReward::MakeSelectPacket(v8, v6);
    InterfacePacketBuf::finalize(v6, 1);
    CParty::send_to_party((CUser **)Party, (PacketGuard *)v6);
    CParty::SetTournamentDungeonClearState(Party, (CParty *)3);
    CParty::TournamentDungeonClear(Party, isTournamentVictory);
    std::vector<int>::~vector((void **)v5);
    PacketGuard::~PacketGuard(v6);
  }

  if ( CParty::get_member_count((CUser **)Party) <= 1 )
    return 0;

              CParty::Reset_party_overlapped_drop_ratio(Party);
            return 0;

                CParty::Reset_party_overlapped_drop_ratio(Party);
              v10 = 0;

              CParty::send_to_party((CUser **)Party, (PacketGuard *)v172);
            }
          }
          else if ( *(this + 144369) == (CUser *)8 )
          {
            WarRoom = (WarRoom *)CUser::GetWarRoom((CUser *)this);
            if ( WarRoom )
            {

              member_count = CParty::get_member_count((CUser **)Party);
              if ( member_count <= 0 )

      if ( (unsigned __int8)CParty::HasNoQuestGoWithApcInParty((CUser **)Party, a2) )
      {
        PacketGuard::PacketGuard((PacketGuard *)v3);
        InterfacePacketBuf::put_header(v3, 0, 168);
        InterfacePacketBuf::put_int(v3, *((_DWORD *)a2 + 1));
        InterfacePacketBuf::finalize(v3, 1);
        CParty::send_to_party((CUser **)Party, (PacketGuard *)v3);
        PacketGuard::~PacketGuard(v3);
      }

        CParty::send_to_party(Party, a3);
        return 1;

      user = (CUser *)CParty::get_user(Party, i);
      if ( user && a1 != user )

    return (unsigned __int16)CParty::getStraightVictories(Party);
  else

    return CParty::GetSecretShopData(Party);
  else

    return CParty::GetUserSecretShop(Party, this);
  else

    CParty::SendSecretShopItemList(Party, this);
}

//----- (08688638) --------------------------------------------------------
int __cdecl CUser::GetUserPosInParty(CUser *this)
{

    return CParty::GetUserPosition(Party, this);
  else

        CParty::send_to_party((CUser **)Party, (PacketGuard *)v14);
    }
    if ( a9 )
      CUser::decide_growth_power_reward_system(a2);
  }
  PacketGuard::~PacketGuard((PacketPool **)v14);
}

//----- (0868B480) --------------------------------------------------------
CUser *__cdecl CUser::setCharacSlotCountInfo(CUser *this, unsigned __int8 a2, unsigned __int8 a3)
{
  unsigned __int8 CharacSlotCountDefaultInfo; // [esp+2Fh] [ebp-9h]

  if ( a2 && a3 )
  {
    CUser::setSlotEffectCount(this, a2);
    return CUser::setCharacSlotLimit(this, a3);
  }
  else
  {
    CharacSlotCountDefaultInfo = CUser::getCharacSlotCountDefaultInfo();
    CUser::setSlotEffectCount(this, CharacSlotCountDefaultInfo);
    return CUser::setCharacSlotLimit(this, CharacSlotCountDefaultInfo);
  }
}

//----- (0868B4FC) --------------------------------------------------------
int CUser::getCharacSlotCountDefaultInfo()
{
  int id; // eax
  int id; // eax

  v0 = *((_DWORD *)G_CEnvironment() + 106);
  if ( v0 == 1 )
    return 1;
  if ( v0 == 2 )
    return 3;
  v2 = *((_DWORD *)G_CEnvironment() + 222);
  if ( v2 == 5 )
    return 24;
  if ( v2 == 11 )
    return 24;
  return 17;
}

//----- (0868B552) --------------------------------------------------------
int __cdecl CUser::VerifyPresentAvengerTitle(CUser *this)
{
  unsigned int acc_id; // ebx
  time_t Time; // esi
  ARAD::Arad_DataManager *v3; // eax
  int GameScript; // eax
  char flag; // bl
  char flag; // si
  ARAD::Arad_DataManager *v9; // eax
  int id; // eax
  bool v11; // al
  bool v12; // di
  CDataManager *v13; // eax
  __int16 *p_s; // edx
  unsigned int id; // ebx
  char *p_dest; // eax
  int id; // edx
  unsigned int id; // ebx
  char *v19; // eax
  const char *String; // eax
  const char *v21; // eax
  int ServerGroup; // ebx
  size_t v23; // esi
  int CurCharacNo; // eax
  int id; // esi
  int id; // ebx
  DB_SaveGiveAvengerTitleFlag *UID; // eax
  unsigned int id; // eax
  __int16 s; // [esp+4Ah] [ebp-18Eh] BYREF
  int id; // [esp+4Ch] [ebp-18Ch] BYREF
  _BYTE v31[2]; // [esp+14Ah] [ebp-8Eh] BYREF
  int id; // [esp+14Ch] [ebp-8Ch]
  char dest; // [esp+187h] [ebp-51h] BYREF
  int id; // [esp+188h] [ebp-50h] BYREF
  std::string v35; // [esp+19Ch] [ebp-3Ch] BYREF
  std::string v36; // [esp+1A3h] [ebp-35h] BYREF
  int id[4]; // [esp+1ABh] [ebp-2Dh] BYREF
  unsigned int CurSec; // [esp+1BCh] [ebp-1Ch]

  acc_id = CUser::get_acc_id(this);
  Time = CUserCharacInfo::getCurCharacCreateTime(this);
  std::allocator<char>::allocator();
  std::string::string(&v35, "Arad_EventPeriodDataManager");
  v3 = (ARAD::Arad_DataManager *)ARAD::Singleton<ARAD::Arad_DataManager>::Get();
  GameScript = ARAD::Arad_DataManager::findGameScript(v3, (std::string)&v35);
  LOBYTE(acc_id) = !ARAD::Arad_EventPeriodDataManager::checkCreateTime(GameScript, 142, Time, acc_id);
  std::string::~string(&v35);
  std::allocator<char>::~allocator();
  if ( (_BYTE)acc_id )
    return 0;
  if ( CUserCharacInfo::get_charac_level(this) <= 19
    || CUserCharacInfo::get_charac_job(this) != 4
    || CUserCharacInfo::getCurCharFirstGrowType(this) )
  {
    return 0;
  }
  CurSec = CSystemTime::getCurSec((CSystemTime *)&GlobalData::s_systemTime_);
  v7 = 0;
  v8 = 0;
  v11 = 0;
  if ( (unsigned __int8)CUser::getFlagPresentAvengerTitle(this) != 1 )
  {
    std::allocator<char>::allocator();
    v7 = 1;
    std::string::string((char **)((char *)&v36._M_dataplus._M_p + 1), "Arad_EventPeriodDataManager");
    v8 = 1;
    v9 = (ARAD::Arad_DataManager *)ARAD::Singleton<ARAD::Arad_DataManager>::Get();
    v10 = ARAD::Arad_DataManager::findGameScript(v9, (std::string)((char *)&v36._M_dataplus._M_p + 1));
    if ( ARAD::Arad_EventPeriodDataManager::isApplied(v10, 142, CurSec) )
      v11 = 1;
  }
  v12 = v11;
  if ( v8 )
    std::string::~string((char **)((char *)&v36._M_dataplus._M_p + 1));
  if ( v7 )
    std::allocator<char>::~allocator();
  if ( !v12 )
    return 0;
  Inven_Item::Inven_Item((Inven_Item *)v31);
  v32 = 2675631;
  v13 = G_CDataManager();
  CItemList::create_item(*((CItemList **)v13 + 3), 0, (Inven_Item *)v31, 1);
  if ( !v32 || v32 == -1 )
    return 0;
  CUser::setFlagPresentAvengerTitle(this, 1);
  p_s = &s;
  v15 = 256;
  if ( ((unsigned __int8)&s & 2) != 0 )
  {
    s = 0;
    p_s = (__int16 *)&v30;
    v15 = 254;
  }
  memset(p_s, 0, 4 * (v15 >> 2));
  if ( (v15 & 2) != 0 )
    p_s[2 * (v15 >> 2)] = 0;
  p_dest = &dest;
  v17 = 21;
  if ( ((unsigned __int8)&dest & 1) != 0 )
  {
    dest = 0;
    p_dest = (char *)&v34;
    v17 = 20;
  }
  if ( ((unsigned __int8)p_dest & 2) != 0 )
  {
    *(_WORD *)p_dest = 0;
    p_dest += 2;
    v17 -= 2;
  }
  v18 = 0;
  do
  {
    *(_DWORD *)&p_dest[v18] = 0;
    v18 += 4;
  }
  while ( v18 < (v17 & 0xFFFFFFFC) );
  v19 = &p_dest[v18];
  if ( (v17 & 2) != 0 )
  {
    *(_WORD *)v19 = 0;
    v19 += 2;
  }
  if ( (v17 & 1) != 0 )
    *v19 = 0;
  String = RDARScriptStringManager::findString(
             (RDARScriptStringManager *)&g_scriptStringManager_,
             (char *)4,
             "ready_to_avenger_title",
             0);
  strncpy(&dest, String, SIZE_20_BYTES);
  v21 = RDARScriptStringManager::findString(
          (RDARScriptStringManager *)&g_scriptStringManager_,
          (char *)4,
          "ready_to_avenger_mail",
          0);
  strncpy((char *)&s, v21, MASK_BYTE);
  ServerGroup = CUser::GetServerGroup(this);
  v23 = strlen((const char *)&s);
  CurCharacNo = CUserCharacInfo::getCurCharacNo(this);
  WongWork::CMailBoxHelper::ReqDBSendNewSystemMail(
    &dest,
    (Inven_Item *)v31,
    0,
    CurCharacNo,
    (char *)&s,
    v23,
    0,
    ServerGroup,
    0,
    0);
  v25 = CUserCharacInfo::getCurCharacNo(this);
  v26 = CUser::get_acc_id(this);
  UID = (DB_SaveGiveAvengerTitleFlag *)CUser::GetUID(this);
  DB_SaveGiveAvengerTitleFlag::makeRequest(UID, v26, v25);
  CUserCharacInfo::getCurCharacNo(this);
  v28 = CUser::get_acc_id(this);
  LOBYTE(v26) = NumberToString(v28, 0);
  cMyTrace::cMyTrace((cMyTrace *)((char *)v37 + 1), "bool CUser::VerifyPresentAvengerTitle()", 42731, 0);
  cMyTrace::operator()((int)v37 + 1, "avenger plan event item post m_id(%s),charac_no(%u), itemidx(%u)", v26);
  return 1;
}
// 868B825: conditional instruction was optimized away because bl.1 is in (==0|==FE)

//----- (0868BA38) --------------------------------------------------------
int __cdecl CUser::getBeforeAreaTime(CUser *this)
{
  return *((_DWORD *)this + 124316);
}

//----- (0868BA46) --------------------------------------------------------
CUser *__cdecl CUser::setBeforeAreaTime(CUser *this, int value)
{
  *((_DWORD *)this + 124316) = a2;
  return this;
}

//----- (0868BA58) --------------------------------------------------------
void __cdecl CUser::CalculateExpertJobMaterials(CUserCharacInfo *charac_info, _DWORD *a2)
{
  unsigned int CurCharacExpertJobExp; // eax
  int CurExpertJobLevel; // ebx
  int CurCharacExpertJobType; // esi
  int CurCharacInvenR; // eax
  int id; // ebx
  int id; // eax
  int id; // ebx
  int id; // esi
  _DWORD value[3]; // [esp+1Ch] [ebp-3Ch] BYREF
  int i; // [esp+28h] [ebp-30h] BYREF
  int id; // [esp+2Ch] [ebp-2Ch] BYREF
  void *v14[3]; // [esp+30h] [ebp-28h] BYREF
  int id; // [esp+3Ch] [ebp-1Ch] BYREF
  int id; // [esp+40h] [ebp-18h] BYREF
  int id; // [esp+44h] [ebp-14h] BYREF
  int id; // [esp+48h] [ebp-10h] BYREF
  float ExpertJobCompoundMaterialVariation; // [esp+4Ch] [ebp-Ch]

  CurCharacExpertJobExp = CUserCharacInfo::GetCurCharacExpertJobExp(a1);
  CurExpertJobLevel = CUser::GetCurExpertJobLevel(a1, CurCharacExpertJobExp);
  CurCharacExpertJobType = CUserCharacInfo::GetCurCharacExpertJobType(a1);
  CurCharacInvenR = CUserCharacInfo::getCurCharacInvenR(a1);
  ExpertJobCompoundMaterialVariation = CInventory::GetExpertJobCompoundMaterialVariation(
                                         CurCharacInvenR,
                                         CurCharacExpertJobType,
                                         CurExpertJobLevel);
  std::vector<std::pair<int,int>>::vector((int)v14, a2);
  ExpertJobCompoundMaterialVariation = ExpertJobCompoundMaterialVariation / 100.0;
  __gnu_cxx::__normal_iterator<std::pair<int,int> *,std::vector<std::pair<int,int>>>::__normal_iterator(&v13);
  __gnu_cxx::__normal_iterator<std::pair<int,int> *,std::vector<std::pair<int,int>>>::__normal_iterator(&i);
  std::vector<std::pair<int,int>>::begin(v11, v14);
  v13 = v11[0];
  std::vector<std::pair<int,int>>::begin(v11, a2);
  for ( i = v11[0];
        ;
        __gnu_cxx::__normal_iterator<std::pair<int,int> *,std::vector<std::pair<int,int>>>::operator++(&v17, &i) )
  {
    std::vector<std::pair<int,int>>::end(&v15, (int)v14);
    if ( !__gnu_cxx::operator!=<std::pair<int,int> *,std::vector<std::pair<int,int>>>((int)&v13, (int)&v15) )
      break;
    std::vector<std::pair<int,int>>::end(&v16, (int)a2);
    if ( !__gnu_cxx::operator!=<std::pair<int,int> *,std::vector<std::pair<int,int>>>((int)&i, (int)&v16) )
      break;
    v6 = __gnu_cxx::__normal_iterator<std::pair<int,int> *,std::vector<std::pair<int,int>>>::operator->((int)&v13);
    v7 = __gnu_cxx::__normal_iterator<std::pair<int,int> *,std::vector<std::pair<int,int>>>::operator->((int)&v13);
    *(_DWORD *)(v6 + 4) = (int)((long double)*(int *)(v7 + 4) * ExpertJobCompoundMaterialVariation);
    v8 = __gnu_cxx::__normal_iterator<std::pair<int,int> *,std::vector<std::pair<int,int>>>::operator->((int)&i);
    v9 = *(_DWORD *)(v8 + 4);
    *(_DWORD *)(v8 + 4) = v9
                        + *(_DWORD *)(__gnu_cxx::__normal_iterator<std::pair<int,int> *,std::vector<std::pair<int,int>>>::operator->((int)&v13)
                                    + 4);
    if ( !*(_DWORD *)(__gnu_cxx::__normal_iterator<std::pair<int,int> *,std::vector<std::pair<int,int>>>::operator->((int)&i)
                    + 4) )
      *(_DWORD *)(__gnu_cxx::__normal_iterator<std::pair<int,int> *,std::vector<std::pair<int,int>>>::operator->((int)&i)
                + 4) = 1;
    __gnu_cxx::__normal_iterator<std::pair<int,int> *,std::vector<std::pair<int,int>>>::operator++(&v18, &v13);
  }
  std::vector<std::pair<int,int>>::~vector(v14);
}

//----- (0868BC7C) --------------------------------------------------------
int __cdecl CUser::GetCurExpertJobLevel(CUser *this, unsigned int id)
{
  int CurCharacExpertJobType; // ebx
  CExpertJobList **v4; // eax
  CExpertJobList **v5; // eax
  int id; // ebx
  int id; // [esp+14h] [ebp-24h] BYREF
  char flag; // [esp+1Bh] [ebp-1Dh] BYREF
  int id; // [esp+1Ch] [ebp-1Ch] BYREF
  STExpertJobScript *ExpertJobScript; // [esp+20h] [ebp-18h]
  char isBoundaryExpValue; // [esp+27h] [ebp-11h]
  unsigned int Level; // [esp+28h] [ebp-10h]
  char *ExpertJobEtcScript; // [esp+2Ch] [ebp-Ch]

  if ( !CUserCharacInfo::getCurCharacR(this) )
    return -1;
  if ( !CUserCharacInfo::GetCurCharacExpertJobType(this) )
    return 0;
  CurCharacExpertJobType = CUserCharacInfo::GetCurCharacExpertJobType(this);
  v4 = (CExpertJobList **)G_CDataManager();
  ExpertJobScript = (STExpertJobScript *)CDataManager::GetExpertJobScript(v4, CurCharacExpertJobType);
  isBoundaryExpValue = STExpertJobScript::isBoundaryExpValue(ExpertJobScript, a2);
  Level = STExpertJobScript::GetLevel(ExpertJobScript, a2);
  if ( !isBoundaryExpValue )
    return Level;
  v5 = (CExpertJobList **)G_CDataManager();
  ExpertJobEtcScript = CDataManager::GetExpertJobEtcScript(v5);
  v8 = Level;
  std::map<unsigned char,short>::find(&v7, (int)ExpertJobEtcScript, &v8);
  std::map<unsigned char,short>::end(&v9, (int)ExpertJobEtcScript);
  if ( std::_Rb_tree_iterator<std::pair<unsigned char const,short>>::operator!=(&v7, &v9)
    && (v6 = *(__int16 *)(std::_Rb_tree_iterator<std::pair<unsigned char const,short>>::operator->(&v7) + 2),
        v6 <= CUserCharacInfo::get_charac_level(this)) )
  {
    return Level;
  }
  else
  {
    return Level - 1;
  }
}

//----- (0868BDAC) --------------------------------------------------------
int __cdecl CUserCharacInfo::SetInvisible(CUserCharacInfo *this)
{
  int result; // eax

  result = *((_DWORD *)this + 4);
  *(_BYTE *)(result + 3648) &= ~1u;
  return result;
}

//----- (0868BDC8) --------------------------------------------------------
int __cdecl CUserCharacInfo::SetVisible(CUserCharacInfo *this)
{
  int result; // eax

  result = *((_DWORD *)this + 4);
  *(_BYTE *)(result + 3648) |= 1u;
  return result;
}

//----- (0868BDE4) --------------------------------------------------------
int __cdecl CUserCharacInfo::get_charac_visible_values(CUserCharacInfo *this)
{
  if ( *((_DWORD *)this + 4) )
    return *(unsigned __int8 *)(*((_DWORD *)this + 4) + 3648);
  else
    return 0;
}

//----- (0868BE08) --------------------------------------------------------
int __cdecl CUserCharacInfo::set_charac_visible_values(CUserCharacInfo *this, char flag)
{
  int result; // eax

  result = *((_DWORD *)this + 4);
  if ( result )
  {
    result = *((_DWORD *)this + 4);
    *(_BYTE *)(result + 3648) = a2;
  }
  return result;
}

//----- (0868BE30) --------------------------------------------------------
int __cdecl CUserCharacInfo::set_charac_visible_grow_avatar(CUserCharacInfo *this, bool a2)
{
  int result; // eax
  int value; // edx

  result = *((_DWORD *)this + 4);
  if ( result )
  {
    v3 = *((_DWORD *)this + 4);
    result = (2 * (((char)(a2 << 7) >> 7) & 1)) | *(_BYTE *)(v3 + 3648) & 0xFD;
    *(_BYTE *)(v3 + 3648) = result;
  }
  return result;
}

//----- (0868BE70) --------------------------------------------------------
int __cdecl CUserCharacInfo::set_charac_visible_teleport_(CUserCharacInfo *this, bool a2)
{
  int result; // eax
  int value; // edx

  result = *((_DWORD *)this + 4);
  if ( result )
  {
    v3 = *((_DWORD *)this + 4);
    result = (4 * (((char)(a2 << 7) >> 7) & 1)) | *(_BYTE *)(v3 + 3648) & 0xFB;
    *(_BYTE *)(v3 + 3648) = result;
  }
  return result;
}

//----- (0868BEB4) --------------------------------------------------------
_BOOL4 __cdecl CUserCharacInfo::get_charac_visible_grow_avatar(CUserCharacInfo *this)
{
  return *((_DWORD *)this + 4) && (*(_BYTE *)(*((_DWORD *)this + 4) + 3648) & 2) != 0;
}

//----- (0868BEE4) --------------------------------------------------------
_BOOL4 __cdecl CUserCharacInfo::IsCurCharacVisible(CUserCharacInfo *this)
{
  return *((_DWORD *)this + 4) && (*(_BYTE *)(*((_DWORD *)this + 4) + 3648) & 1) != 0;
}

//----- (0868BF14) --------------------------------------------------------
int __cdecl CUser::select_charac_set_visible_values(CUser *this, char flag)
{
  char *GameOptionRef; // eax
  char *v4; // eax

  CUserCharacInfo::set_charac_visible_values(this, a2);
  CUserCharacInfo::SetVisible(this);
  if ( CUserCharacInfo::get_charac_visible_grow_avatar(this) )
  {
    GameOptionRef = CUser::GetGameOptionRef(this);
    return CGameOption::set_etc_option((int)GameOptionRef, 1, 1);
  }
  else
  {
    v4 = CUser::GetGameOptionRef(this);
    return CGameOption::set_etc_option((int)v4, 1, 0);
  }
}

//----- (0868BF98) --------------------------------------------------------
void __cdecl CUser::send_charac_visible_data(CUser *this)
{
  __int16 unique_id; // ax
  char charac_visible_values; // al
  GameWorld *v3; // eax
  GameWorld *v4; // eax
  int area; // ebx
  int state; // eax
  int id; // ebx
  int CurCharacVill; // esi
  GameWorld *v9; // eax
  GameWorld *v10; // eax
  PacketBuf *v11[3]; // [esp+1Ch] [ebp-1Ch] BYREF
  Village *UserVillage; // [esp+28h] [ebp-10h]
  CParty *Party; // [esp+2Ch] [ebp-Ch]

  PacketGuard::PacketGuard((PacketGuard *)v11);
  InterfacePacketBuf::put_header(v11, 0, 356);
  unique_id = CUser::get_unique_id(this);
  InterfacePacketBuf::put_short(v11, unique_id);
  charac_visible_values = CUserCharacInfo::get_charac_visible_values(this);
  InterfacePacketBuf::put_byte(v11, charac_visible_values);
  InterfacePacketBuf::finalize(v11, 1);
  v3 = G_GameWorld();
  if ( GameWorld::IsPVPChannel(v3) )
    goto LABEL_14;
  v4 = G_GameWorld();
  UserVillage = (Village *)GameWorld::getUserVillage(v4, this);
  if ( UserVillage )
  {
    area = CUser::get_area(this, 0);
    if ( area == Village::get_gate_area(UserVillage) )
    {

      CParty::send_to_party((CUser **)Party, (PacketGuard *)v11);
  }
LABEL_15:
  PacketGuard::~PacketGuard(v11);
}

//----- (0868C170) --------------------------------------------------------
int __cdecl CUser::send_ontime_reward_start_notify(CUser *this)
{
  CEnvironment *v2; // eax
  char *file_name; // eax
  int charac_no; // esi
  unsigned int acc_id; // eax
  const char *v6; // ebx
  int ontime_id; // ebx
  int reward_end_time; // eax
  char flag; // [esp+8h] [ebp-160h]
  char s[256]; // [esp+20h] [ebp-148h] BYREF
  PacketBuf *v11[3]; // [esp+120h] [ebp-48h] BYREF
  int id[4]; // [esp+12Ch] [ebp-3Ch] BYREF
  int id[2]; // [esp+13Ch] [ebp-2Ch] BYREF
  COnTimeEvent *RepeatEvent; // [esp+144h] [ebp-24h]
  time_t v15; // [esp+148h] [ebp-20h]
  int id; // [esp+14Ch] [ebp-1Ch]

  RepeatEvent = (COnTimeEvent *)CEventManager::GetRepeatEvent(GlobalData::s_event_manager, 51);
  if ( !RepeatEvent )
  {

char *__cdecl CParty::GetSecretShopData(CParty *this)
{
  return (char *)this + 676;
}

        user = (CUser *)CParty::get_user(Party, i);
        if ( user )

      CParty::send_to_party((CUser **)Party, (PacketGuard *)v8);
      PacketGuard::~PacketGuard(v8);

    CParty::send_to_party((CUser **)Party, (PacketGuard *)v8);
    PacketGuard::~PacketGuard(v8);

        PartyTelePort = CParty::GetPartyTelePort(Party);
        if ( (unsigned __int8)CPartyTelePort::get_teleport_state((CPartyTelePort *)PartyTelePort) == 1 )

  member_count = (char *)CParty::get_member_count(a2);
  if ( (int)member_count > 0 )

    PartyIndex = CParty::GetPartyIndex((CParty *)a2);
    InterfacePacketBuf::put_short(v23, PartyIndex);

      TitleIndex = CParty::getTitleIndex((CParty *)a2);
      InterfacePacketBuf::put_byte(v23, TitleIndex);

      if ( !(unsigned __int8)CParty::getTitleIndex((CParty *)a2) )
      {
        Title = CParty::getTitle((CParty *)a2);
        v6 = strlen(Title);
        InterfacePacketBuf::put_int(v23, v6);
        v7 = CParty::getTitle((CParty *)a2);
        v8 = strlen(v7);
        v9 = CParty::getTitle((CParty *)a2);
        InterfacePacketBuf::put_str(v23, v9, v8);
      }

      IsReturnUserParty = CParty::IsReturnUserParty(a2);
      InterfacePacketBuf::put_byte(v23, IsReturnUserParty);

      UserMax = CParty::getUserMax((CParty *)a2);
      InterfacePacketBuf::put_byte(v23, UserMax);

      DungIndex = CParty::getDungIndex((CParty *)a2);
      InterfacePacketBuf::put_short(v23, DungIndex);

      DungDiffi = CParty::getDungDiffi((CParty *)a2);
      InterfacePacketBuf::put_byte(v23, DungDiffi);

      IsEventCharacParty = CParty::IsEventCharacParty(a2);
      InterfacePacketBuf::put_byte(v23, IsEventCharacParty);

        user = (CUser *)CParty::get_user((CParty *)a2, i);
        if ( user )

          Manager = (CUser *)CParty::getManager((CParty *)a2);
          if ( Manager == user )

      v17 = CParty::IsReturnUserParty(a2);
      InterfacePacketBuf::put_byte(v23, v17);

      is_quick_party = CParty::is_quick_party((CParty *)a2);
      InterfacePacketBuf::put_byte(v23, is_quick_party);

      v19 = CParty::IsEventCharacParty(a2);
      InterfacePacketBuf::put_byte(v23, v19);

          v31 = (CUserCharacInfo *)CParty::get_user((CParty *)a2, j);
          if ( v31 )

                CParty::send_to_party((CUser **)Party, (PacketGuard *)v91);
            }
            break;
          case 3:
            UserVillage = 0;
            v35 = G_GameWorld();
            UserVillage = (Village *)GameWorld::getUserVillage(v35, (CUser *)a2);
            if ( UserVillage )
            {

            CParty::send_to_party((CUser **)Party, (PacketGuard *)v97);
        }
        goto LABEL_97;
      case 3:
      case 0x27:
        UserVillage = 0;
        v23 = G_GameWorld();
        UserVillage = (Village *)GameWorld::getUserVillage(v23, (CUser *)a2);
        if ( UserVillage )
        {

  if ( Party && (CUser *)CParty::getManager(Party) == a2 && a3 == CurCharacVill && a4 == area && a9 != 2 )
    return 19;

        DungIndex = (unsigned __int16)CParty::getDungIndex(Party);
      boost::date_time::second_clock<boost::posix_time::ptime>::local_time(&v5);

