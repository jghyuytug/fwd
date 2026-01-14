#include <party/CQuickParty.h>
#include <game/party/CQuickParty.h>

QuickParty::CQuickParty *__cdecl QuickParty::CQuickParty::CQuickParty(QuickParty::CQuickParty *this)
{
  return QuickParty::CQuickParty::init(this);
}

QuickParty::CQuickParty *__cdecl QuickParty::CQuickParty::init(QuickParty::CQuickParty *this)
{
  int i; // [esp+1Ch] [ebp-Ch]

  memset(this, -1, SIZE_16_BYTES);
  *((_DWORD *)this + 4) = -1;
  *((_BYTE *)this + 20) = 0;
  *((_WORD *)this + 11) = 0;
  *((_BYTE *)this + 24) = -1;
  *((_BYTE *)this + 25) = -1;
  for ( i = 157; i <= 158; ++i )
    *((_DWORD *)this + i - 150) = 0;
  *((_BYTE *)this + 36) = 0;
  return this;
}

int __cdecl QuickParty::CQuickParty::destroy(QuickParty::CQuickParty *this)
{
  CGameManager *v1; // eax
  CGameManager *v2; // eax
  char flag; // si
  CGameManager *v4; // eax
  char flag; // si
  int id[6]; // [esp+10h] [ebp-18h] BYREF

  v1 = G_CGameManager();
  CGameManager::PutQuickParty(v1, this);
  v2 = G_CGameManager();
  __gnu_cxx::hash_map<int,QuickParty::CQuickParty *,__gnu_cxx::hash<int>,std::equal_to<int>,std::allocator<QuickParty::CQuickParty *>>::size((int)v2 + 724);
  G_CGameManager();
  v3 = StaticPool<QuickParty::CQuickParty,300>::Size();
  v4 = G_CGameManager();
  v5 = v3 - StaticPool<QuickParty::CQuickParty,300>::Remain((int)v4 + 460);
  cMyTrace::cMyTrace((cMyTrace *)v7, "void QuickParty::CQuickParty::destroy()", 50, 0);
  return cMyTrace::operator()((int)v7, aQuickPartyLogQ_2, v5);
}

int __cdecl QuickParty::CQuickParty::get_timer_key(int id, int id2)
{
  return *(_DWORD *)(a1 + 4 * (a2 - 157 + 4) + 12);
}

int __cdecl QuickParty::CQuickParty::gen_timer_key(int id, int value)
{
  return ++*(_DWORD *)(a1 + 4 * (a2 - 157 + 4) + 12);
}

bool __cdecl QuickParty::CQuickParty::check_timer_key(int id, int id2, int id3)
{
  return *(_DWORD *)(a1 + 4 * (a2 - 157 + 4) + 12) == a3;
}

int __cdecl QuickParty::CQuickParty::get_party_object(QuickParty::CQuickParty *this, unsigned int id)
{
  int id; // ebx
  CGameManager *v4; // eax
  int Party; // [esp+1Ch] [ebp-Ch]

  if ( a2 >= 4 )
    return 0;
  if ( *((_DWORD *)this + a2) == -1 )
    return 0;
  v3 = *((_DWORD *)this + a2);
  v4 = G_CGameManager();
  Party = CGameManager::GetParty(v4, v3);
  if ( !Party )
    return 0;
  return Party;
}

int __cdecl QuickParty::CQuickParty::get_party_object(QuickParty::CQuickParty *this)
{
  int id; // ebx
  CGameManager *v2; // eax
  int i; // [esp+18h] [ebp-10h]
  int Party; // [esp+1Ch] [ebp-Ch]

  for ( i = 0; i <= 3; ++i )
  {
    if ( *((_DWORD *)this + i) != -1 )
    {
      v1 = *((_DWORD *)this + i);
      v2 = G_CGameManager();
      Party = CGameManager::GetParty(v2, v1);
      if ( Party )
        return Party;
    }
  }
  return 0;
}

char __cdecl QuickParty::CQuickParty::set_party_index(QuickParty::CQuickParty *this, int value)
{
  char result; // al
  int i; // [esp+Ch] [ebp-4h]

  for ( i = 0; ; ++i )
  {
    result = i <= 3;
    if ( i > 3 )
      break;
    if ( *((_DWORD *)this + i) == -1 )
    {
      *((_DWORD *)this + i) = a2;
      return (char)this;
    }
  }
  return result;
}

QuickParty::CQuickParty *__cdecl QuickParty::CQuickParty::set_party_index(
        QuickParty::CQuickParty *this,

int __cdecl QuickParty::CQuickParty::get_party_index(QuickParty::CQuickParty *this, unsigned int id)
{
  if ( a2 < 4 )
    return *((_DWORD *)this + a2);
  else
    return -1;
}

int __cdecl QuickParty::CQuickParty::delete_party_index(QuickParty::CQuickParty *this, int id)
{
  int i; // [esp+Ch] [ebp-4h]

  for ( i = 0; i <= 3; ++i )
  {
    if ( *((_DWORD *)this + i) == a2 )
    {
      *((_DWORD *)this + i) = -1;
      return 1;
    }
  }
  return 0;
}

bool __cdecl QuickParty::CQuickParty::delete_party_index(QuickParty::CQuickParty *this)
{
  bool result; // al
  int i; // [esp+Ch] [ebp-4h]

  for ( i = 0; ; ++i )
  {
    result = i <= 3;
    if ( i > 3 )
      break;
    *((_DWORD *)this + i) = -1;
  }
  return result;
}

int __cdecl QuickParty::CQuickParty::is_empty_quick_party(QuickParty::CQuickParty *this)
{
  int i; // [esp+Ch] [ebp-4h]

  for ( i = 0; i <= 3; ++i )
  {
    if ( *((_DWORD *)this + i) != -1 )
      return 0;
  }
  return 1;
}

int __cdecl QuickParty::CQuickParty::get_quick_party_count(QuickParty::CQuickParty *this)
{
  int id; // ebx
  CGameManager *v2; // eax
  int id; // [esp+10h] [ebp-18h]
  int i; // [esp+14h] [ebp-14h]
  CUser **Party; // [esp+18h] [ebp-10h]

  v4 = 0;
  for ( i = 0; i <= 3; ++i )
  {
    if ( *((_DWORD *)this + i) != -1 )
    {
      v1 = *((_DWORD *)this + i);
      v2 = G_CGameManager();
      Party = (CUser **)CGameManager::GetParty(v2, v1);
      if ( Party )
        v4 += CParty::get_member_count(Party);
    }
  }
  return v4;
}

int __cdecl QuickParty::CQuickParty::get_matching_quick_party_master_unique_id(QuickParty::CQuickParty *this)
{
  CGameManager *v1; // eax
  CGameManager *v2; // eax
  CGameManager *v3; // eax
  signed int id; // [esp+18h] [ebp-30h]
  unsigned __int16 v6; // [esp+1Eh] [ebp-2Ah]
  CParty *Party; // [esp+20h] [ebp-28h]
  signed int i; // [esp+24h] [ebp-24h]
  int party_index; // [esp+28h] [ebp-20h]
  signed int j; // [esp+2Ch] [ebp-1Ch]
  int id; // [esp+30h] [ebp-18h]
  int id; // [esp+34h] [ebp-14h]
  CParty *party; // [esp+38h] [ebp-10h]
  CUser *Manager; // [esp+3Ch] [ebp-Ch]

  v5 = -1;
  v6 = -1;
  for ( i = 0; i <= 3; ++i )
  {
    party_index = QuickParty::CQuickParty::get_party_index(this, i);
    v1 = G_CGameManager();
    Party = (CParty *)CGameManager::GetParty(v1, party_index);
    if ( Party )
    {
      if ( (char)CParty::GetEPLPState(Party) > 1 )
      {
        v5 = i;
        break;
      }
      if ( (unsigned __int8)CParty::is_quick_party(Party) )
        v5 = i;
    }
  }
  if ( v5 == -1 )
  {
    for ( j = 0; j <= 3; ++j )
    {
      v11 = QuickParty::CQuickParty::get_party_index(this, j);
      v2 = G_CGameManager();
      if ( CGameManager::GetParty(v2, v11) )
      {
        v5 = j;
        break;
      }
    }
  }
  if ( v5 >= 0 )
  {
    v12 = QuickParty::CQuickParty::get_party_index(this, v5);
    v3 = G_CGameManager();
    v13 = (CParty *)CGameManager::GetParty(v3, v12);
    if ( v13 )
    {
      Manager = (CUser *)CParty::getManager(v13);
      return (unsigned __int16)CUser::get_unique_id(Manager);
    }
  }
  return v6;
}

bool __cdecl QuickParty::CQuickParty::send_matching_update_quick_party(
        QuickParty::CQuickParty *this,

    party_object = (CParty *)QuickParty::CQuickParty::get_party_object(this, i);
    if ( party_object )

          quick_party_count = (char *)QuickParty::CQuickParty::get_quick_party_count(quick_party_in_pool);
          if ( quick_party_count == (char *)limit_party_count )

            v14 = QuickParty::CQuickParty::get_quick_party_index(quick_party_in_pool);
            cMyTrace::cMyTrace(

    dungeon_diff = QuickParty::CQuickParty::get_dungeon_diff(QuickParty);
    if ( (unsigned __int8)QuickParty::CQuickPartySystemManager::remove_quick_party_in_pool(

      QuickParty::CQuickParty::set_change_quick_party_count(QuickParty, 1);
    quick_party_count = 0;

      quick_party_count = (char *)(QuickParty::CQuickParty::get_quick_party_count(QuickParty) - 1);
      if ( (int)quick_party_count <= 0 )

  quick_party_count = QuickParty::CQuickParty::get_quick_party_count(QuickParty);
  if ( quick_party_count > 1 || (char)QuickParty::CQuickParty::get_first_matching_cnt(QuickParty) != quick_party_count )

    if ( (char)QuickParty::CQuickParty::get_first_matching_cnt(QuickParty) >= quick_party_count
      && (unsigned __int8)QuickParty::CQuickParty::is_change_quick_party_count(QuickParty) != 1 )

      party_object = (CParty *)QuickParty::CQuickParty::get_party_object(QuickParty);
      if ( !party_object )

    dungeon_diff = QuickParty::CQuickParty::get_dungeon_diff(QuickParty);
    v10 = G_CGameManager();

  quick_party_count = QuickParty::CQuickParty::get_quick_party_count(a3);
  if ( (unsigned __int8)QuickParty::CQuickPartySystemManager::check_quick_party_min_count(this, a4, quick_party_count) != 1 )

  pool_index = QuickParty::CQuickParty::get_pool_index(a2);
  if ( pool_index >= 4u )

    quick_party_index = QuickParty::CQuickParty::get_quick_party_index(a2);
    v4 = std::_Rb_tree_iterator<std::pair<QuickParty::STQuickPartyPoolMap_Key const,QuickParty::STQuickPartyPoolMap_Data>>::operator->(&v6);

    QuickParty::CQuickParty::destroy(a2);
    cMyTrace::cMyTrace(

  v2 = QuickParty::CQuickParty::gen_timer_key((int)a2, 158);
  quick_party_index = QuickParty::CQuickParty::get_quick_party_index(a2);

      QuickParty::CQuickParty::set_party_index(quick_party_in_pool, a5);
      *a2 = QuickParty::CQuickParty::get_quick_party_count(quick_party_in_pool);

      QuickParty::CQuickParty::set_party_index(quick_party_in_pool, a5);
      *a2 = a6;

      QuickParty::CQuickParty::set_first_matching_cnt(quick_party_in_pool, a6);
    }
    return quick_party_in_pool;
  }
  else
  {
    v7 = G_CGameManager();
    quick_party_in_pool = CGameManager::GetQuickParty(v7);
    if ( quick_party_in_pool )
    {

      QuickParty::CQuickParty::set_party_index(quick_party_in_pool, a5);
      *a2 = a6;

      QuickParty::CQuickParty::set_first_matching_cnt(quick_party_in_pool, a6);
      return quick_party_in_pool;

    QuickParty::CQuickParty::set_dungeon_index(a6, *(_WORD *)a2);
    QuickParty::CQuickParty::set_dungeon_diff(a6, *((_BYTE *)a2 + 2));

    QuickParty::CQuickParty::set_pool_index(a6, (char)a5);
    if ( a4 )

      quick_party_index = QuickParty::CQuickParty::get_quick_party_index(a6);
      CParty::set_quick_party_index(a4, quick_party_index);

    v11 = QuickParty::CQuickParty::get_quick_party_index(a6);
    std::list<int>::push_back((int)a3 + 8 * (_DWORD)a5, (int)&v11);

    quick_party_count = QuickParty::CQuickParty::get_quick_party_count(a6);
    first_matching_cnt = (char)QuickParty::CQuickParty::get_first_matching_cnt(a6);

      || (unsigned __int8)QuickParty::CQuickParty::is_change_quick_party_count(a6) )
    {
      v13 = 1;
    }

    QuickParty::CQuickParty::send_matching_update_quick_party(a6, (char)a5, matching_quick_party_master_unique_id, v13);
    return 1;

        party_object = (CParty *)QuickParty::CQuickParty::get_party_object(a6, i);
        if ( party_object )

      QuickParty::CQuickParty::destroy(a6);
    }
    LogManager::logFormat(
      (LogManager *)1,
      (int)"QuickParty.cpp",
      "bool QuickParty::CQuickPartySystemManager::push_quick_party_in_pool(const QuickParty::STQuickPartyPoolMap_Key&, Qu"
      "ickParty::STQuickPartyPoolMap_Data&, CParty*, int, QuickParty::CQuickParty*, bool)",
      (const char *)0x383,
      "QUICK_PARTY_LOG : ABNOMAL QUICK PARTY!! REGIST POOL INDEX(%d)",
      a5);
    return 0;
  }
}

//----- (0826B222) --------------------------------------------------------
CParty *__cdecl QuickParty::CQuickPartySystemManager::make_quick_party(
        QuickParty::CQuickPartySystemManager *this,
        QuickParty::CQuickParty *a2)
{
  CParty *party; // ebx
  CGameManager *v3; // eax
  CGameManager *v4; // eax
  GameWorld *v5; // eax
  CGameManager *v6; // eax
  CUser *user; // eax
  CUser **v8; // eax
  const char *v10; // [esp+14h] [ebp-54h]
  int id; // [esp+28h] [ebp-40h] BYREF
  _DWORD *v12; // [esp+2Ch] [ebp-3Ch] BYREF
  int id[2]; // [esp+30h] [ebp-38h] BYREF
  CParty *Party; // [esp+38h] [ebp-30h]
  CParty *party; // [esp+3Ch] [ebp-2Ch]
  int i; // [esp+40h] [ebp-28h]
  char flag[4]; // [esp+44h] [ebp-24h]
  int j; // [esp+48h] [ebp-20h]
  char flag[4]; // [esp+4Ch] [ebp-1Ch]
  int k; // [esp+50h] [ebp-18h]
  char flag[4]; // [esp+54h] [ebp-14h]
  CParty *party; // [esp+58h] [ebp-10h]
  int m; // [esp+5Ch] [ebp-Ch]

  if ( !a2 )
    return 0;
  std::list<CUser *>::list(v13);
  v15 = 0;
  for ( i = 0; i <= 3; ++i )
  {
    *(_DWORD *)v17 = QuickParty::CQuickParty::get_party_index(a2, i);
    v3 = G_CGameManager();
    Party = (CParty *)CGameManager::GetParty(v3, *(_DWORD *)v17);
    if ( Party )
    {

    *(_DWORD *)v19 = QuickParty::CQuickParty::get_party_index(a2, j);
    v4 = G_CGameManager();

      *(_DWORD *)v21 = QuickParty::CQuickParty::get_party_index(a2, k);
      if ( *(_DWORD *)v21 != -1 )

    QuickParty::CQuickParty::destroy(a2);
    CParty::init_quick_party_data(v15);

    QuickParty::CQuickParty::destroy(a2);
    LogManager::logFormat(

    if ( QuickParty::CQuickParty::get_quick_party_count(a3) > 1 )
    {
      quick_party = QuickParty::CQuickPartySystemManager::make_quick_party(this, a3);
      if ( quick_party )
      {
        QuickParty::CQuickPartySystemManager::insert_timer_waiting_start_game(this, quick_party);
        CParty::send_party_realtime_info((CUser **)quick_party);
        CParty::send_party_ipinfo((CUser **)quick_party);
        v3 = G_GameWorld();
        GameWorld::send_party_info_to_all(v3, (CUser **)quick_party, 2u);
        CParty::send_quick_party_matching_result((CUser **)quick_party, 1, 0);
        if ( (char)CParty::GetEPLPState(quick_party) <= 1 )
        {
          Manager = (CUser *)CParty::getManager(quick_party);
          CParty::game_start(a1, (CUser **)quick_party, Manager);
        }
      }
      else
      {
        LogManager::logFormat(
          (LogManager *)1,
          (int)"QuickParty.cpp",
          "void QuickParty::CQuickPartySystemManager::start_game(QuickParty::CQuickParty*)",
          (const char *)0x41E,
          "QUICK_PARTY_LOG : NO EXIST PARTY!!",
          v5);
      }
    }

int `global constructor keyed to'QuickParty::CQuickParty::CQuickParty()
{
  return __static_initialization_and_destruction_0(1, INVALID_ID_U16);
}

int __cdecl QuickParty::CQuickParty::get_quick_party_index(QuickParty::CQuickParty *this)
{
  return *((_DWORD *)this + 4);
}

int __cdecl QuickParty::CQuickParty::get_first_matching_cnt(QuickParty::CQuickParty *this)
{
  return *((unsigned __int8 *)this + 20);
}

QuickParty::CQuickParty *__cdecl QuickParty::CQuickParty::set_first_matching_cnt(
        QuickParty::CQuickParty *this,

QuickParty::CQuickParty *__cdecl QuickParty::CQuickParty::set_dungeon_index(QuickParty::CQuickParty *this, __int16 a2)
{
  *((_WORD *)this + 11) = a2;
  return this;
}

int __cdecl QuickParty::CQuickParty::get_dungeon_index(QuickParty::CQuickParty *this)
{
  return *((unsigned __int16 *)this + 11);
}

QuickParty::CQuickParty *__cdecl QuickParty::CQuickParty::set_dungeon_diff(QuickParty::CQuickParty *this, char flag)
{
  *((_BYTE *)this + 24) = a2;
  return this;
}

int __cdecl QuickParty::CQuickParty::get_dungeon_diff(QuickParty::CQuickParty *this)
{
  return *((char *)this + 24);
}

QuickParty::CQuickParty *__cdecl QuickParty::CQuickParty::set_pool_index(QuickParty::CQuickParty *this, char flag)
{
  *((_BYTE *)this + 25) = a2;
  return this;
}

int __cdecl QuickParty::CQuickParty::get_pool_index(QuickParty::CQuickParty *this)
{
  return *((char *)this + 25);
}

int __cdecl QuickParty::CQuickParty::is_change_quick_party_count(QuickParty::CQuickParty *this)
{
  return *((unsigned __int8 *)this + 36);
}

QuickParty::CQuickParty *__cdecl QuickParty::CQuickParty::set_change_quick_party_count(
        QuickParty::CQuickParty *this,

  QuickParty::CQuickParty::set_quick_party_index(v9, Index);
  quick_party_index = QuickParty::CQuickParty::get_quick_party_index(v9);

    v8 = QuickParty::CQuickParty::get_quick_party_index(v9);
    v3 = __gnu_cxx::hash_map<int,QuickParty::CQuickParty *,__gnu_cxx::hash<int>,std::equal_to<int>,std::allocator<QuickParty::CQuickParty *>>::operator[](

    v4 = (const char *)QuickParty::CQuickParty::get_quick_party_index(v9);
    LogManager::logFormat(

    quick_party_index = QuickParty::CQuickParty::get_quick_party_index(a2);
    __gnu_cxx::hash_map<int,QuickParty::CQuickParty *,__gnu_cxx::hash<int>,std::equal_to<int>,std::allocator<QuickParty::CQuickParty *>>::find(

      v5[0] = QuickParty::CQuickParty::get_quick_party_index(a2);
      __gnu_cxx::hash_map<int,QuickParty::CQuickParty *,__gnu_cxx::hash<int>,std::equal_to<int>,std::allocator<QuickParty::CQuickParty *>>::erase(

      QuickParty::CQuickParty::init(a2);
    }
  }
}

//----- (082956C8) --------------------------------------------------------
WongWork::CDeathTower *__cdecl CGameManager::getDeathTower(CGameManager *this)
{
  int Index; // eax
  _DWORD *v3; // eax
  _DWORD value[2]; // [esp+14h] [ebp-24h] BYREF
  int Idx; // [esp+1Ch] [ebp-1Ch] BYREF
  _DWORD value[2]; // [esp+20h] [ebp-18h] BYREF
  int id; // [esp+28h] [ebp-10h] BYREF
  WongWork::CDeathTower *v8; // [esp+2Ch] [ebp-Ch]

  v8 = (WongWork::CDeathTower *)StaticPool<WongWork::CDeathTower,600>::Acquire((int)this + 316);
  if ( !v8 )
    return 0;
  Index = StaticPool<WongWork::CDeathTower,600>::GetIndex((unsigned int *)this + 79, (unsigned int)v8);
  WongWork::CDeathTower::setIdx(v8, Index);
  Idx = WongWork::CDeathTower::getIdx(v8);
  __gnu_cxx::hash_map<int,WongWork::CDeathTower *,__gnu_cxx::hash<int>,std::equal_to<int>,std::allocator<WongWork::CDeathTower *>>::find(
    v4,
    (int)this + 664,
    &Idx);
  __gnu_cxx::hash_map<int,WongWork::CDeathTower *,__gnu_cxx::hash<int>,std::equal_to<int>,std::allocator<WongWork::CDeathTower *>>::end(
    v6,
    (int)this + 664);
  if ( !__gnu_cxx::_Hashtable_iterator<std::pair<int const,WongWork::CDeathTower *>,int,__gnu_cxx::hash<int>,std::_Select1st<std::pair<int const,WongWork::CDeathTower *>>,std::equal_to<int>,std::allocator<WongWork::CDeathTower *>>::operator==(
          v4,
          v6) )
    return 0;
  v7 = WongWork::CDeathTower::getIdx(v8);
  v3 = __gnu_cxx::hash_map<int,WongWork::CDeathTower *,__gnu_cxx::hash<int>,std::equal_to<int>,std::allocator<WongWork::CDeathTower *>>::operator[](
         (int)this + 664,
         (int)&v7);
  *v3 = v8;
  WongWork::CDeathTower::reset(v8);
  return v8;
}

//----- (082957BC) --------------------------------------------------------
int CGameManager::getDeathTower(CGameManager *this, ...)
{
  _DWORD value[2]; // [esp+10h] [ebp-18h] BYREF
  _DWORD value[4]; // [esp+18h] [ebp-10h] BYREF
  va_list va; // [esp+34h] [ebp+Ch] BYREF

  va_start(va, this);
  __gnu_cxx::hash_map<int,WongWork::CDeathTower *,__gnu_cxx::hash<int>,std::equal_to<int>,std::allocator<WongWork::CDeathTower *>>::find(
    v2,
    (int)this + 664,
    (int *)va);
  __gnu_cxx::hash_map<int,WongWork::CDeathTower *,__gnu_cxx::hash<int>,std::equal_to<int>,std::allocator<WongWork::CDeathTower *>>::end(
    v3,
    (int)this + 664);
  if ( __gnu_cxx::_Hashtable_iterator<std::pair<int const,WongWork::CDeathTower *>,int,__gnu_cxx::hash<int>,std::_Select1st<std::pair<int const,WongWork::CDeathTower *>>,std::equal_to<int>,std::allocator<WongWork::CDeathTower *>>::operator==(
         v2,
         v3) )
  {
    return 0;
  }
  else
  {
    return *(_DWORD *)(__gnu_cxx::_Hashtable_iterator<std::pair<int const,WongWork::CDeathTower *>,int,__gnu_cxx::hash<int>,std::_Select1st<std::pair<int const,WongWork::CDeathTower *>>,std::equal_to<int>,std::allocator<WongWork::CDeathTower *>>::operator->(v2)
                     + 4);
  }
}

//----- (0829582C) --------------------------------------------------------
int __cdecl CGameManager::getDeathTowerIdx(CGameManager *this, WongWork::CDeathTower *a2)
{
  return StaticPool<WongWork::CDeathTower,600>::GetIndex((unsigned int *)this + 79, (unsigned int)a2);
}

//----- (08295852) --------------------------------------------------------
void __cdecl CGameManager::returnDeathTower(CGameManager *this, WongWork::CDeathTower *a2)
{
  _DWORD value[2]; // [esp+18h] [ebp-20h] BYREF
  int Idx; // [esp+20h] [ebp-18h] BYREF
  _DWORD value[2]; // [esp+24h] [ebp-14h] BYREF
  int id[3]; // [esp+2Ch] [ebp-Ch] BYREF

  if ( a2 )
  {
    Idx = WongWork::CDeathTower::getIdx(a2);
    __gnu_cxx::hash_map<int,WongWork::CDeathTower *,__gnu_cxx::hash<int>,std::equal_to<int>,std::allocator<WongWork::CDeathTower *>>::find(
      v2,
      (int)this + 664,
      &Idx);
    __gnu_cxx::hash_map<int,WongWork::CDeathTower *,__gnu_cxx::hash<int>,std::equal_to<int>,std::allocator<WongWork::CDeathTower *>>::end(
      v4,
      (int)this + 664);
    if ( __gnu_cxx::_Hashtable_iterator<std::pair<int const,WongWork::CDeathTower *>,int,__gnu_cxx::hash<int>,std::_Select1st<std::pair<int const,WongWork::CDeathTower *>>,std::equal_to<int>,std::allocator<WongWork::CDeathTower *>>::operator!=(
           v2,
           v4) )
    {

QuickParty::CQuickParty *__cdecl QuickParty::CQuickParty::set_quick_party_index(QuickParty::CQuickParty *this, int value)
{
  *((_DWORD *)this + 4) = a2;
  return this;
}

  QuickParty::CQuickParty::CQuickParty(a1);
  *((_DWORD *)a1 + 10) = 36641;

  if ( !QuickParty::CQuickParty::check_timer_key((int)QuickParty, a5, a4) )
    return 0;

  quick_party_count = QuickParty::CQuickParty::get_quick_party_count(QuickParty);
  if ( quick_party_count <= 1 && (char)QuickParty::CQuickParty::get_first_matching_cnt(QuickParty) == quick_party_count )

  if ( QuickParty::CQuickParty::get_timer_key((int)QuickParty, a5) == 1
    && (unsigned __int8)Timer_QuickPartyWaitingPartyMatching::cancel_quick_party(this, QuickParty) != 1 )

  v8 = (char)QuickParty::CQuickParty::get_first_matching_cnt(QuickParty) >= quick_party_count
    && (unsigned __int8)QuickParty::CQuickParty::is_change_quick_party_count(QuickParty) != 1;

  dungeon_diff = QuickParty::CQuickParty::get_dungeon_diff(QuickParty);
  v9 = G_CGameManager();

  party_object = (CParty *)QuickParty::CQuickParty::get_party_object(a2);
  if ( !party_object )

