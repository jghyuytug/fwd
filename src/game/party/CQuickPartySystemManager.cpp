#include <party/CQuickPartySystemManager.h>
#include <game/party/CQuickPartySystemManager.h>

  matched = QuickParty::CQuickPartySystemManager::match_proper_party(
              a1,

  if ( (unsigned __int8)QuickParty::CQuickPartySystemManager::cancel_quick_party(QuickPartySystemManager, Party, 1, a2) != 1 )
    *((_DWORD *)a4 + 1) = 1;

  *((_DWORD *)a5 + 1) = QuickParty::CQuickPartySystemManager::direct_entrance_quick_party(
                          a1,

int __cdecl QuickParty::CQuickPartySystemManager::CQuickPartySystemManager(QuickParty::CQuickPartySystemManager *this)
{
  std::map<QuickParty::STQuickPartyPoolMap_Key,QuickParty::STQuickPartyPoolMap_Data>::map(this);
  return std::map<QuickParty::STQuickPartyPoolMap_Key,QuickParty::STQuickPartyPoolMap_Data>::clear((int)this);
}

void __cdecl QuickParty::CQuickPartySystemManager::init_quick_party_pool(
        QuickParty::CQuickPartySystemManager *this,

int __cdecl QuickParty::CQuickPartySystemManager::check_match_proper_party(
        QuickParty::CQuickPartySystemManager *this,

      "ENUM_ERROR QuickParty::CQuickPartySystemManager::check_match_proper_party(const QuickParty::STQuickPartyPoolMap_Key&, CParty*)",
      (const char *)0x16E,

        "ENUM_ERROR QuickParty::CQuickPartySystemManager::match_proper_party(const QuickParty::STQuickPartyPoolMap_Key&, "
        "CParty*, CUser*)",

        "ENUM_ERROR QuickParty::CQuickPartySystemManager::match_proper_party(const QuickParty::STQuickPartyPoolMap_Key&, "
        "CParty*, CUser*)",

    v23 = QuickParty::CQuickPartySystemManager::check_match_proper_party(this, a3, (CUser **)a4);
    if ( v23 )

              return !QuickParty::CQuickPartySystemManager::regist_quick_party(
                        this,

                return !QuickParty::CQuickPartySystemManager::regist_quick_party(
                          this,

              quick_party_in_pool = (QuickParty::CQuickParty *)QuickParty::CQuickPartySystemManager::find_quick_party_in_pool(
                                                                 (int)this,

            QuickParty::CQuickPartySystemManager::start_game(a1, this, quick_party_in_pool);
            return 0;

              "ENUM_ERROR QuickParty::CQuickPartySystemManager::match_proper_party(const QuickParty::STQuickPartyPoolMap_"
              "Key&, CParty*, CUser*)",

            if ( (unsigned __int8)QuickParty::CQuickPartySystemManager::push_quick_party_in_pool(
                                    this,

                "ENUM_ERROR QuickParty::CQuickPartySystemManager::match_proper_party(const QuickParty::STQuickPartyPoolMa"
                "p_Key&, CParty*, CUser*)",

            "ENUM_ERROR QuickParty::CQuickPartySystemManager::match_proper_party(const QuickParty::STQuickPartyPoolMap_Ke"
            "y&, CParty*, CUser*)",

int __cdecl QuickParty::CQuickPartySystemManager::cancel_quick_party(
        QuickParty::CQuickPartySystemManager *this,

        "bool QuickParty::CQuickPartySystemManager::cancel_quick_party(CParty*, bool, CUser*)",
        (const char *)0x1EC,

          "bool QuickParty::CQuickPartySystemManager::cancel_quick_party(CParty*, bool, CUser*)",
          (const char *)0x216,

          (unsigned __int8)QuickParty::CQuickPartySystemManager::push_quick_party_in_pool(
                             this,

        "bool QuickParty::CQuickPartySystemManager::cancel_quick_party(CParty*, bool, CUser*)",
        (const char *)0x229,

      if ( (unsigned __int8)QuickParty::CQuickPartySystemManager::cancel_quick_party(
                              QuickPartySystemManager,

    QuickParty::CQuickPartySystemManager::start_game(
      a1,

  if ( (unsigned __int8)QuickParty::CQuickPartySystemManager::remove_quick_party_in_pool(
                          QuickPartySystemManager,

  QuickParty::CQuickPartySystemManager::start_game(a1, this, a3);
  return 1;

int __cdecl QuickParty::CQuickPartySystemManager::find_quick_party_in_pool(int id, int *a2)
{
  CGameManager *v2; // eax
  char flag; // [esp+8h] [ebp-30h]
  int id[4]; // [esp+18h] [ebp-20h] BYREF
  int QuickParty; // [esp+28h] [ebp-10h]
  char flag[4]; // [esp+2Ch] [ebp-Ch]

  QuickParty = 0;
  while ( !std::list<int>::empty(a2) )
  {
    *(_DWORD *)v7 = *(_DWORD *)std::list<int>::front(a2);
    std::list<int>::pop_front(a2);
    v2 = G_CGameManager();
    QuickParty = CGameManager::GetQuickParty(v2, *(_DWORD *)v7);
    if ( QuickParty )
      break;
    cMyTrace::cMyTrace(
      (cMyTrace *)v5,
      "QuickParty::CQuickParty* QuickParty::CQuickPartySystemManager::find_quick_party_in_pool(QuickParty::LIST_QUICK_PARTY&)",
      668,
      5);
    cMyTrace::operator()((int)v5, "QUICK_PARTY_LOG : NO INTENDED ROUTINE!!", v4);
  }
  return QuickParty;
}

int __cdecl QuickParty::CQuickPartySystemManager::remove_quick_party_in_pool(
        QuickParty::CQuickPartySystemManager *this,

      "bool QuickParty::CQuickPartySystemManager::remove_quick_party_in_pool(QuickParty::CQuickParty*, const QuickParty::"
      "STQuickPartyPoolMap_Key&)",

int __cdecl QuickParty::CQuickPartySystemManager::check_quick_party_min_count(
        QuickParty::CQuickPartySystemManager *this,

int __cdecl QuickParty::CQuickPartySystemManager::insert_timer_waiting_party_matching(
        QuickParty::CQuickPartySystemManager *this,

int __cdecl QuickParty::CQuickPartySystemManager::insert_timer_waiting_start_game(
        QuickParty::CQuickPartySystemManager *this,

_BOOL4 __cdecl QuickParty::CQuickPartySystemManager::regist_quick_party(
        QuickParty::CQuickPartySystemManager *this,

      && (unsigned __int8)QuickParty::CQuickPartySystemManager::push_quick_party_in_pool(
                            this,

QuickParty::CQuickParty *__cdecl QuickParty::CQuickPartySystemManager::pop_quick_party_in_pool(
        QuickParty::CQuickPartySystemManager *this,

    quick_party_in_pool = (QuickParty::CQuickParty *)QuickParty::CQuickPartySystemManager::find_quick_party_in_pool(
                                                       (int)this,

        "QuickParty::CQuickParty* QuickParty::CQuickPartySystemManager::pop_quick_party_in_pool(int&, QuickParty::STQuick"
        "PartyPoolMap_Data&, int, int, int)",

int __cdecl QuickParty::CQuickPartySystemManager::push_quick_party_in_pool(
        QuickParty::CQuickPartySystemManager *this,

      QuickParty::CQuickPartySystemManager::insert_timer_waiting_party_matching(this, a6);
    matching_quick_party_master_unique_id = QuickParty::CQuickParty::get_matching_quick_party_master_unique_id(a6);

      "CParty* QuickParty::CQuickPartySystemManager::make_quick_party(QuickParty::CQuickParty*)",
      (const char *)0x3D0,

void __usercall QuickParty::CQuickPartySystemManager::start_game(
        int id@<esi>,

        "void QuickParty::CQuickPartySystemManager::start_game(QuickParty::CQuickParty*)",
        (const char *)0x412,

      "void QuickParty::CQuickPartySystemManager::start_game(QuickParty::CQuickParty*)",
      (const char *)0x40C,

    return QuickParty::CQuickPartySystemManager::cancel_quick_party(QuickPartySystemManager, a2, a3, 0) ^ 1;
  }
  return result;
}

//----- (0829844A) --------------------------------------------------------
void __cdecl CGameManager::checkOutBlueMarble(CGameManager *this, CUser *user)
{
  __int16 BlueMarbleIndex; // ax
  BlueMarble *BlueMarble; // [esp+1Ch] [ebp-Ch]

  if ( (unsigned __int8)CUser::checkInBlueMarble(a2) == 1 )
  {

    QuickParty::CQuickPartySystemManager::CQuickPartySystemManager(v1);
    *((_DWORD *)this + 191) = v1;

  QuickParty::CQuickPartySystemManager::start_game(
    a1,

  if ( !(unsigned __int8)QuickParty::CQuickPartySystemManager::cancel_quick_party(
                           QuickPartySystemManager,

