#include <skill/CSkillTreeParam.h>
#include <game/skill/CSkillTreeParam.h>

            CSkillTreeParam::CSkillTreeParam(
              (CSkillTreeParam *)v171,

              if ( CSkillTreeParam::SetSkillTree(PeerIP2, (CSkillTreeParam *)v171, this) )
              {
                v3 = 3020;
              }

                  CSkillTreeParam::SendSkillPointReturnInfo((CSkillTreeParam *)v171);
                  CUser::SendDungeonClearList(a2);

void __cdecl CSkillTreeParam::CSkillTreeParam(CSkillTreeParam *this, CUser *user, SIG_LOAD_ETC *a3, PacketGuard *a4)
{
  *(_DWORD *)this = a2;
  *((_DWORD *)this + 1) = a3;
  *((_DWORD *)this + 2) = a4;
  std::vector<stSPReturnInfo_t>::vector((_DWORD *)this + 7);
  std::vector<stSPReturnInfo_t>::vector((_DWORD *)this + 10);
  *((_DWORD *)this + 3) = 0;
  *((_DWORD *)this + 4) = 0;
  *((_DWORD *)this + 5) = 0;
  *((_DWORD *)this + 6) = 0;
}

int __cdecl CSkillTreeParam::SendSkillPointReturnInfo(CSkillTreeParam *this)
{
  int CurCharacSkillR; // eax
  int id; // eax
  __int16 v4; // ax
  __int16 *v5; // eax
  int id; // eax
  int id; // eax
  int id; // eax
  int id; // eax
  int id; // eax
  int result; // eax
  __int16 v12; // ax
  __int16 *v13; // eax
  int id; // eax
  int id; // eax
  int id; // eax
  int id; // [esp+10h] [ebp-28h] BYREF
  int id; // [esp+14h] [ebp-24h] BYREF
  int id; // [esp+18h] [ebp-20h] BYREF
  int id; // [esp+1Ch] [ebp-1Ch] BYREF
  int id; // [esp+20h] [ebp-18h] BYREF
  int id; // [esp+24h] [ebp-14h] BYREF
  int id; // [esp+28h] [ebp-10h] BYREF
  _DWORD value[3]; // [esp+2Ch] [ebp-Ch] BYREF

  CurCharacSkillR = CUserCharacInfo::getCurCharacSkillR(*(CUserCharacInfo **)this);
  *((_DWORD *)this + 3) = SkillSlot::get_remain_sp_at_index(CurCharacSkillR, 0) - *((_DWORD *)this + 3);
  v2 = CUserCharacInfo::getCurCharacSkillR(*(CUserCharacInfo **)this);
  *((_DWORD *)this + 5) = SkillSlot::get_remain_sfp_at_index(v2, (char *)2) - *((_DWORD *)this + 5);
  if ( (*((int *)this + 3) > 0 || *((int *)this + 5) > 0) && !std::vector<stSPReturnInfo_t>::empty((_DWORD *)this + 7) )
  {
    InterfacePacketBuf::clear(*((PacketBuf ***)this + 2));
    InterfacePacketBuf::put_header(*((PacketBuf ***)this + 2), 0, 122);
    InterfacePacketBuf::put_byte(*((PacketBuf ***)this + 2), 0);
    InterfacePacketBuf::put_int(*((PacketBuf ***)this + 2), *((_DWORD *)this + 5));
    InterfacePacketBuf::put_int(*((PacketBuf ***)this + 2), *((_DWORD *)this + 3));
    v4 = std::vector<stSPReturnInfo_t>::size((_DWORD *)this + 7);
    InterfacePacketBuf::put_short(*((PacketBuf ***)this + 2), v4);
    std::vector<stSPReturnInfo_t>::begin(&v21, (_DWORD *)this + 7);
    __gnu_cxx::__normal_iterator<stSPReturnInfo_t const*,std::vector<stSPReturnInfo_t>>::__normal_iterator<stSPReturnInfo_t*>(
      &v20,
      (int)&v21);
    std::vector<stSPReturnInfo_t>::end(&v22, (int)this + 28);
    __gnu_cxx::__normal_iterator<stSPReturnInfo_t const*,std::vector<stSPReturnInfo_t>>::__normal_iterator<stSPReturnInfo_t*>(
      &v19,
      (int)&v22);
    while ( __gnu_cxx::operator!=<stSPReturnInfo_t const*,std::vector<stSPReturnInfo_t>>((int)&v20, (int)&v19) )
    {
      v5 = (__int16 *)__gnu_cxx::__normal_iterator<stSPReturnInfo_t const*,std::vector<stSPReturnInfo_t>>::operator->((int)&v20);
      InterfacePacketBuf::put_short(*((PacketBuf ***)this + 2), *v5);
      v6 = __gnu_cxx::__normal_iterator<stSPReturnInfo_t const*,std::vector<stSPReturnInfo_t>>::operator->((int)&v20);
      InterfacePacketBuf::put_short(*((PacketBuf ***)this + 2), *(_WORD *)(v6 + 2));
      v7 = __gnu_cxx::__normal_iterator<stSPReturnInfo_t const*,std::vector<stSPReturnInfo_t>>::operator->((int)&v20);
      InterfacePacketBuf::put_int(*((PacketBuf ***)this + 2), *(_DWORD *)(v7 + 4));
      v8 = __gnu_cxx::__normal_iterator<stSPReturnInfo_t const*,std::vector<stSPReturnInfo_t>>::operator->((int)&v20);
      InterfacePacketBuf::put_int(*((PacketBuf ***)this + 2), *(_DWORD *)(v8 + 8));
      __gnu_cxx::__normal_iterator<stSPReturnInfo_t const*,std::vector<stSPReturnInfo_t>>::operator++(&v20);
    }
    InterfacePacketBuf::finalize(*((PacketBuf ***)this + 2), 1);
    CUser::Send(*(CUser **)this, *((PacketGuard **)this + 2));
  }
  v9 = CUserCharacInfo::getCurCharacSkillR(*(CUserCharacInfo **)this);
  *((_DWORD *)this + 4) = SkillSlot::get_remain_sp_at_index(v9, (char *)1) - *((_DWORD *)this + 4);
  v10 = CUserCharacInfo::getCurCharacSkillR(*(CUserCharacInfo **)this);
  *((_DWORD *)this + 6) = SkillSlot::get_remain_sfp_at_index(v10, (char *)3) - *((_DWORD *)this + 6);
  result = (*((int *)this + 4) > 0 || *((int *)this + 6) > 0)
        && !std::vector<stSPReturnInfo_t>::empty((_DWORD *)this + 10);
  if ( (_BYTE)result )
  {
    InterfacePacketBuf::clear(*((PacketBuf ***)this + 2));
    InterfacePacketBuf::put_header(*((PacketBuf ***)this + 2), 0, 122);
    InterfacePacketBuf::put_byte(*((PacketBuf ***)this + 2), 1);
    InterfacePacketBuf::put_int(*((PacketBuf ***)this + 2), *((_DWORD *)this + 6));
    InterfacePacketBuf::put_int(*((PacketBuf ***)this + 2), *((_DWORD *)this + 4));
    v12 = std::vector<stSPReturnInfo_t>::size((_DWORD *)this + 10);
    InterfacePacketBuf::put_short(*((PacketBuf ***)this + 2), v12);
    std::vector<stSPReturnInfo_t>::begin(&v23, (_DWORD *)this + 10);
    __gnu_cxx::__normal_iterator<stSPReturnInfo_t const*,std::vector<stSPReturnInfo_t>>::__normal_iterator<stSPReturnInfo_t*>(
      &v18,
      (int)&v23);
    std::vector<stSPReturnInfo_t>::end(v24, (int)this + 40);
    __gnu_cxx::__normal_iterator<stSPReturnInfo_t const*,std::vector<stSPReturnInfo_t>>::__normal_iterator<stSPReturnInfo_t*>(
      &v17,
      (int)v24);
    while ( __gnu_cxx::operator!=<stSPReturnInfo_t const*,std::vector<stSPReturnInfo_t>>((int)&v18, (int)&v17) )
    {
      v13 = (__int16 *)__gnu_cxx::__normal_iterator<stSPReturnInfo_t const*,std::vector<stSPReturnInfo_t>>::operator->((int)&v18);
      InterfacePacketBuf::put_short(*((PacketBuf ***)this + 2), *v13);
      v14 = __gnu_cxx::__normal_iterator<stSPReturnInfo_t const*,std::vector<stSPReturnInfo_t>>::operator->((int)&v18);
      InterfacePacketBuf::put_short(*((PacketBuf ***)this + 2), *(_WORD *)(v14 + 2));
      v15 = __gnu_cxx::__normal_iterator<stSPReturnInfo_t const*,std::vector<stSPReturnInfo_t>>::operator->((int)&v18);
      InterfacePacketBuf::put_int(*((PacketBuf ***)this + 2), *(_DWORD *)(v15 + 4));
      v16 = __gnu_cxx::__normal_iterator<stSPReturnInfo_t const*,std::vector<stSPReturnInfo_t>>::operator->((int)&v18);
      InterfacePacketBuf::put_int(*((PacketBuf ***)this + 2), *(_DWORD *)(v16 + 8));
      __gnu_cxx::__normal_iterator<stSPReturnInfo_t const*,std::vector<stSPReturnInfo_t>>::operator++(&v18);
    }
    InterfacePacketBuf::finalize(*((PacketBuf ***)this + 2), 1);
    return CUser::Send(*(CUser **)this, *((PacketGuard **)this + 2));
  }
  return result;
}

        "int CSkillTreeParam::SetSkillTree(Inter_LoadEtc*)",
        (const char *)0x9FF,

