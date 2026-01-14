#include <skill/CSkill.h>
#include <game/skill/CSkill.h>

      if ( (unsigned __int8)CSkill::isCommandCustomizing(skill) != 1 )
      {
        *((_DWORD *)v13 + 1) = 249;
        break;
      }

          if ( skill && CSkill::IsSpecialSkill(skill) )
            *((_BYTE *)v40 + 5) = 1;

_BOOL4 __cdecl CSkill::IsSpecialSkill(CSkill *this)
{
  return *((_BYTE *)this + 288) != 0;
}

int __cdecl CSkill::isCommandCustomizing(CSkill *this)
{
  return *((unsigned __int8 *)this + 304);
}

  return (unsigned __int8)CSkill::IsStealable(skill, *((char *)this + 5), *((char *)this + 6)) != 0;
}

//----- (082865CA) --------------------------------------------------------
int __cdecl CHackLog_InvalidUsingSkill::IsSkillMatchFirstGrowType(CHackLog_InvalidUsingSkill *this, const CSkill *a2)
{

  return CSkill::get_second_growtype_max_level(this, (char)a2, a3) > 0;
}

//----- (08286656) --------------------------------------------------------
int CHackLog_InvalidUsingSkill::GetHackType()
{

std::_List_node_base *CSkill::add_learned_skill(int id, ...)
{
  va_list va; // [esp+24h] [ebp+Ch] BYREF

  va_start(va, a1);
  return std::list<_pre_skill>::push_back(a1 + 24, (int)va);
}

std::_List_node_base *CSkill::add_need_sp(CSkill *this, ...)
{
  va_list va; // [esp+24h] [ebp+Ch] BYREF

  va_start(va, this);
  return std::list<int>::push_back((int)this + 8, (int)va);
}

std::_List_node_base *CSkill::add_need_sfp(CSkill *this, ...)
{
  va_list va; // [esp+24h] [ebp+Ch] BYREF

  va_start(va, this);
  return std::list<int>::push_back((int)this + 16, (int)va);
}

bool __cdecl CSkill::set_prelearned_skill(CSkill *this)
{
  int value; // esi
  int value; // ebx
  CDataManager *v3; // eax
  int group; // eax
  bool result; // al
  _DWORD *v6; // [esp+1Ch] [ebp-2Ch] BYREF
  _DWORD *v7; // [esp+28h] [ebp-20h] BYREF
  _BYTE dest[4]; // [esp+2Ch] [ebp-1Ch] BYREF
  int value; // [esp+30h] [ebp-18h]
  CSkill *skill; // [esp+34h] [ebp-14h]
  int i; // [esp+38h] [ebp-10h]
  int *v12; // [esp+3Ch] [ebp-Ch]

  std::_List_iterator<_pre_skill>::_List_iterator(&v7);
  v9 = std::list<_pre_skill>::size((int *)this + 6);
  std::list<_pre_skill>::begin(&v6, (int *)this + 6);
  v7 = v6;
  for ( i = 0; ; ++i )
  {
    result = i < v9;
    if ( i >= v9 )
      break;
    v12 = (int *)std::_List_iterator<_pre_skill>::operator*(&v7);
    v1 = *v12;
    v2 = *((_DWORD *)this + 1);
    v3 = G_CDataManager();
    skill = (CSkill *)CSkillList::find_skill(*((CSkillList **)v3 + 4), v2, v1);
    if ( skill )
    {
      group = CSkill::get_group(skill);
      v12[2] = group;
    }
    std::_List_iterator<_pre_skill>::operator++(dest, &v7);
  }
  return result;
}

int __cdecl CSkill::get_guildSkillNeedGSP(CSkill *this, unsigned int id)
{
  if ( std::vector<int>::size((_DWORD *)this + 57) >= a2 )
    return *(_DWORD *)std::vector<int>::at((_DWORD *)this + 57, a2 - 1);
  else
    return -1;
}

int __cdecl CSkill::get_guildSkillNeedGold(CSkill *this, unsigned int id)
{
  if ( std::vector<int>::size((_DWORD *)this + 60) >= a2 )
    return *(_DWORD *)std::vector<int>::at((_DWORD *)this + 60, a2 - 1);
  else
    return -1;
}

int __cdecl CSkill::get_needGuildLevel(CSkill *this, int id)
{
  return *((_DWORD *)this + 63) + (a2 - 1) * *((_DWORD *)this + 64);
}

int __cdecl CSkill::get_need_sfp_for_level(CSkill *this, int id, int id2)
{
  _DWORD *v4; // eax
  _DWORD *v5; // eax
  _DWORD *v6; // [esp+1Ch] [ebp-2Ch] BYREF
  _DWORD *v7; // [esp+2Ch] [ebp-1Ch] BYREF
  _BYTE dest[4]; // [esp+30h] [ebp-18h] BYREF
  int id; // [esp+34h] [ebp-14h]
  int id; // [esp+38h] [ebp-10h]
  int i; // [esp+3Ch] [ebp-Ch]

  std::_List_const_iterator<int>::_List_const_iterator(&v7);
  v9 = std::list<int>::size((int *)this + 4);
  if ( v9 < a3 )
    return -1;
  if ( a2 < 0 )
    return -1;
  v10 = 0;
  std::list<int>::begin(&v6, (int *)this + 4);
  v7 = v6;
  for ( i = 1; i < a3; ++i )
  {
    if ( i > a2 && i < a3 )
    {
      v4 = (_DWORD *)std::_List_const_iterator<int>::operator*(&v7);
      v10 += *v4;
    }
    std::_List_const_iterator<int>::operator++(dest, &v7);
  }
  v5 = (_DWORD *)std::_List_const_iterator<int>::operator*(&v7);
  return *v5 + v10;
}

int __cdecl CSkill::get_need_sp_sfp_for_level(CSkill *this, int id, int id2)
{
  if ( CSkill::IsSpecialSkill(this) )
    return CSkill::get_need_sfp_for_level(this, a2, a3);
  else
    return CSkill::get_need_sp_for_level(this, a2, a3);
}

int __cdecl CSkill::get_need_sp_for_level(CSkill *this, int id, int id2)
{
  _DWORD *v4; // eax
  _DWORD *v5; // eax
  _DWORD *v6; // [esp+1Ch] [ebp-2Ch] BYREF
  _DWORD *v7; // [esp+2Ch] [ebp-1Ch] BYREF
  _BYTE dest[4]; // [esp+30h] [ebp-18h] BYREF
  int id; // [esp+34h] [ebp-14h]
  int id; // [esp+38h] [ebp-10h]
  int i; // [esp+3Ch] [ebp-Ch]

  std::_List_const_iterator<int>::_List_const_iterator(&v7);
  v9 = std::list<int>::size((int *)this + 2);
  if ( v9 < a3 )
    return -1;
  if ( a2 < 0 )
    return -1;
  v10 = 0;
  std::list<int>::begin(&v6, (int *)this + 2);
  v7 = v6;
  for ( i = 1; i < a3; ++i )
  {
    if ( i > a2 && i < a3 )
    {
      v4 = (_DWORD *)std::_List_const_iterator<int>::operator*(&v7);
      v10 += *v4;
    }
    std::_List_const_iterator<int>::operator++(dest, &v7);
  }
  v5 = (_DWORD *)std::_List_const_iterator<int>::operator*(&v7);
  return *v5 + v10;
}

bool __cdecl CSkill::can_learn(CSkill *this, int id, int id2, int id3, int id4)
{
  int type_max_level; // eax
  int id; // eax
  int id; // [esp+14h] [ebp-14h]
  int second_growtype_max_level; // [esp+1Ch] [ebp-Ch]

  if ( a4 > 0 )
  {
    second_growtype_max_level = CSkill::get_second_growtype_max_level(this, a3, a4);
    if ( !second_growtype_max_level )
      second_growtype_max_level = CSkill::get_type_max_level(this, a3);
    if ( a5 <= 0 || second_growtype_max_level < a5 )
      return 0;
    v8 = a2 - *((_DWORD *)this + 49) + 1;
    if ( v8 > second_growtype_max_level )
      v8 = second_growtype_max_level;
    v9 = v8;
  }
  else
  {
    if ( a5 <= 0 || CSkill::get_type_max_level(this, a3) < a5 )
      return 0;
    if ( CSkill::get_type_max_level(this, a3) >= a2 - *((_DWORD *)this + 49) + 1 )
      type_max_level = a2 - *((_DWORD *)this + 49) + 1;
    else
      type_max_level = CSkill::get_type_max_level(this, a3);
    v9 = type_max_level;
  }
  return v9 >= a5 && *((_DWORD *)this + 49) + (a5 - 1) * *((_DWORD *)this + 50) <= a2;
}

int __cdecl CSkill::IsStealable(CSkill *this, int id, int id2)
{
  int i; // [esp+8h] [ebp-8h]
  int j; // [esp+Ch] [ebp-4h]

  if ( *((_BYTE *)this + 276) != 1 )
    return 0;
  for ( i = 0; i <= 5; ++i )
  {
    if ( *((_DWORD *)this + i + 51) == a2 )
      return 0;
  }
  if ( a3 > 0 )
  {
    for ( j = 0; j <= 1; ++j )
    {
      if ( *((_DWORD *)this + j + 45) == a3 )
        return 0;
    }
  }
  return 1;
}

CSkill *__cdecl CSkill::set_second_growtype_fitness(CSkill *this, unsigned int value, int value2, int value3)
{
  CSkill *result; // eax

  if ( a2 <= 5 && a3 > 0 && a3 <= 2 )
  {
    *((_DWORD *)this + 4 * a2 + a3 + 20) = a4;
    return this;
  }
  return result;
}

int __cdecl CSkill::get_second_growtype_fitness(CSkill *this, unsigned int id, unsigned int id2)
{
  if ( a2 > 6 || a3 > 2 )
    return -1;
  if ( a3 )
    return *((_DWORD *)this + 4 * a2 + a3 + 20);
  return 0;
}

CSkill *__cdecl CSkill::set_second_growtype_max_level(CSkill *this, unsigned int value, int value2, int value3)
{
  CSkill *result; // eax

  if ( a2 <= 5 && a3 > 0 && a3 <= 2 )
  {
    *((_DWORD *)this + 4 * a2 + a3 + 22) = a4;
    return this;
  }
  return result;
}

int __cdecl CSkill::get_second_growtype_max_level(CSkill *this, unsigned int id, int id2)
{
  if ( a2 <= 5 && a3 > 0 && a3 <= 2 )
    return *((_DWORD *)this + 4 * a2 + a3 + 22);
  else
    return -1;
}

CSkill *__cdecl CSkill::set_fitness_second_growtype(CSkill *this, int value, int value2)
{
  CSkill *result; // eax

  if ( a2 > 0 && a2 <= 2 )
  {
    *((_DWORD *)this + a2 + 44) = a3;
    return this;
  }
  return result;
}

int __cdecl CSkill::get_fitness_second_growtype(CSkill *this, int id)
{
  if ( a2 > 0 && a2 <= 2 )
    return *((_DWORD *)this + a2 + 44);
  else
    return -1;
}

int __cdecl CSkill::getMaxSkillLevel(CSkill *this, int id, int id2, int id3)
{
  int id; // eax
  unsigned int id; // eax
  int type_max_level; // [esp+28h] [ebp-10h]
  int id; // [esp+28h] [ebp-10h]
  int id; // [esp+2Ch] [ebp-Ch]

  if ( a4 <= 0 || (type_max_level = CSkill::get_second_growtype_max_level(this, a3, a4)) == 0 )
    type_max_level = CSkill::get_type_max_level(this, a3);
  if ( !type_max_level )
    return -1;
  v5 = a2 - *((_DWORD *)this + 49) + 1;
  if ( v5 > type_max_level )
    v5 = type_max_level;
  v8 = v5;
  v9 = (a2 - *((_DWORD *)this + 49)) / *((_DWORD *)this + 50) + 1;
  if ( v9 >= v5 )
    v6 = (unsigned int)v5 >> 31;
  else
    v6 = (unsigned int)v9 >> 31;
  if ( (_BYTE)v6 )
    return 0;
  if ( v9 >= v8 )
    return v8;
  return (a2 - *((_DWORD *)this + 49)) / *((_DWORD *)this + 50) + 1;
}

int __cdecl CSkill::reform_ui_group_no(CSkill *this, int *a2, bool a3, int id)
{
  int result; // eax

  if ( a3 )
  {
    *a2 = 3;
    return (int)a2;
  }
  else
  {
    result = *a2;
    if ( *a2 >= 0 )
    {
      if ( result <= 3 )
      {
        result = (int)a2;
        *a2 = a4 <= 2;
      }
      else if ( result == 4 )
      {
        *a2 = 2;
        return (int)a2;
      }
    }
  }
  return result;
}

      CSkill::CSkill(v14);
      v65 = v14;

      CSkill::set_index(v14, v35);
      v15 = std::vector<int>::size(v46);

      CSkill::reform_ui_group_no(v65, v42, v52, v15);
      CSkill::set_group(v65, v42[0]);

      CSkill::set_job_type(v65, (int)i);
      CSkill::set_skill_type(v65, v41);

      CSkill::set_level_limit(v65, v38[3]);
      *((_DWORD *)v65 + 50) = v38[4];

      CSkill::SetStealable(v65, v47);
      CSkill::SetStealLevelPenalty(v65, v49);

      CSkill::SetStealSPPenalty(v65, v48);
      for ( j = 0; ; ++j )

        CSkill::set_fitGrowType(v65, j, *v16);
      }
      for ( k = 0; k <= 5; ++k )
        CSkill::set_fitness(v65, k, v36[k + 24]);
      for ( m = 0; m <= 5; ++m )
        CSkill::set_type_max_level(v65, m, v36[m + 30]);
      for ( n = 0; n <= 1; ++n )
      {

            CSkill::set_fitness_second_growtype(v65, n + 1, *v19);
          }
        }
      }
      for ( ii = 0; ii <= 5; ++ii )
      {
        for ( jj = 0; jj <= 1; ++jj )
          CSkill::set_second_growtype_fitness(v65, ii, jj + 1, v36[4 * ii + 36 + jj]);
      }
      for ( kk = 0; kk <= 5; ++kk )
      {
        for ( mm = 0; mm <= 1; ++mm )
          CSkill::set_second_growtype_max_level(v65, kk, mm + 1, v36[4 * kk + 38 + mm]);
      }
      v69 = std::vector<int>::size(v37);
      for ( nn = 0; nn < v69; ++nn )
      {
        v20 = (_DWORD *)std::vector<int>::at(v37, nn);
        CSkill::add_need_sp(v65, *v20);
      }
      v70 = std::vector<int>::size(v38);
      for ( i1 = 0; i1 < v70; ++i1 )
      {
        v21 = (_DWORD *)std::vector<int>::at(v38, i1);
        CSkill::add_need_sfp(v65, *v21);
      }
      v71 = std::vector<int>::size(v39);
      for ( i2 = 0; i2 < v71; ++i2 )
      {
        v57 = *(_DWORD *)std::vector<int>::at(v39, i2);
        v58 = *(_DWORD *)std::vector<int>::at(v40, i2);
        CSkill::add_learned_skill((int)v65, v57, v58, v59);
      }
      v72 = std::vector<int>::size(v50);
      for ( i3 = 0; i3 < v72; ++i3 )
      {
        v22 = (_DWORD *)std::vector<int>::at(v50, i3);
        std::vector<int>::push_back((int)v65 + 228, v22);
      }
      v73 = std::vector<int>::size(v51);
      for ( i4 = 0; i4 < v73; ++i4 )
      {
        v23 = (_DWORD *)std::vector<int>::at(v51, i4);
        std::vector<int>::push_back((int)v65 + 240, v23);
      }
      *((_DWORD *)v65 + 63) = v38[6];
      *((_DWORD *)v65 + 64) = v38[7];
      *((_DWORD *)v65 + 65) = v36[4];
      for ( i5 = 0; ; ++i5 )
      {
        v27 = std::vector<std::vector<float>>::size(v45);
        if ( v27 <= i5 )
          break;
        std::vector<int>::vector(v55);
        for ( i6 = 0; ; ++i6 )
        {
          v25 = (_DWORD *)std::vector<std::vector<float>>::operator[](v45, i5);
          v26 = std::vector<float>::size(v25);
          if ( v26 <= i6 )
            break;
          v24 = (_DWORD *)std::vector<std::vector<float>>::operator[](v45, i5);
          v61 = (int)*(float *)std::vector<float>::at(v24, i6);
          std::vector<int>::push_back((int)v55, (int)&v61);
        }
        std::vector<std::vector<int>>::push_back((int)v65 + 264, v55);
        std::vector<int>::~vector(v55);
      }
      *((_BYTE *)v65 + 288) = v52;
      std::vector<STKeyCommandUnit>::operator=((int)v65 + 292, v44);
      *((_BYTE *)v65 + 304) = v53;
      v28 = std::string::c_str((std::string *)v36);
      CSkill::setSkillName(v65, v28);
      *((_DWORD *)v65 + 102) = v38[5];
      CSkillList::insert_skill(this, (int)i, v65);
      std::_Rb_tree_iterator<std::pair<int const,std::string>>::operator++(dest, &v56);
      ++v66;
    }
  }
  for ( i7 = 0; i7 <= 10; ++i7 )
  {
    __gnu_cxx::_Hashtable_iterator<std::pair<int const,CSkill *>,int,__gnu_cxx::hash<int>,std::_Select1st<std::pair<int const,CSkill *>>,std::equal_to<int>,std::allocator<CSkill *>>::_Hashtable_iterator();
    __gnu_cxx::hash_map<int,CSkill *,__gnu_cxx::hash<int>,std::equal_to<int>,std::allocator<CSkill *>>::begin(
      v30,
      (int)this + 20 * i7);
    v54[0] = v30[0];
    v54[1] = v30[1];
    while ( 1 )
    {
      __gnu_cxx::hash_map<int,CSkill *,__gnu_cxx::hash<int>,std::equal_to<int>,std::allocator<CSkill *>>::end(
        v63,
        (int)this + 20 * i7);
      if ( !__gnu_cxx::_Hashtable_iterator<std::pair<int const,CSkill *>,int,__gnu_cxx::hash<int>,std::_Select1st<std::pair<int const,CSkill *>>,std::equal_to<int>,std::allocator<CSkill *>>::operator!=(
              v54,
              v63) )
        break;
      v65 = *(CSkill **)(__gnu_cxx::_Hashtable_iterator<std::pair<int const,CSkill *>,int,__gnu_cxx::hash<int>,std::_Select1st<std::pair<int const,CSkill *>>,std::equal_to<int>,std::allocator<CSkill *>>::operator->(v54)
                       + 4);
      CSkill::set_prelearned_skill(v65);
      CSkillList::set_post_learning_skill(this, i7, v65);
      __gnu_cxx::_Hashtable_iterator<std::pair<int const,CSkill *>,int,__gnu_cxx::hash<int>,std::_Select1st<std::pair<int const,CSkill *>>,std::equal_to<int>,std::allocator<CSkill *>>::operator++(
        v64,
        v54);
    }
  }
  a1 = 1;
LABEL_86:
  STSkillScript::~STSkillScript((STSkillScript *)&v35);
  return a1;
}

//----- (083519A6) --------------------------------------------------------
_DWORD *__cdecl CSkillList::insert_skill(CSkillList *this, int id, CSkill *a3)
{
  _DWORD value[4]; // [esp+18h] [ebp-70h] BYREF
  _DWORD value[2]; // [esp+28h] [ebp-60h] BYREF
  int index; // [esp+30h] [ebp-58h] BYREF
  int id[4]; // [esp+34h] [ebp-54h] BYREF
  _DWORD value[2]; // [esp+44h] [ebp-44h] BYREF
  _BYTE v9[12]; // [esp+4Ch] [ebp-3Ch] BYREF
  int id[2]; // [esp+58h] [ebp-30h] BYREF
  int id; // [esp+60h] [ebp-28h] BYREF
  _DWORD value[2]; // [esp+64h] [ebp-24h] BYREF
  int group; // [esp+6Ch] [ebp-1Ch] BYREF
  int id[6]; // [esp+70h] [ebp-18h] BYREF

  if ( a3 )
  {
    index = CSkill::get_index(a3);
    __gnu_cxx::_Hashtable_iterator<std::pair<int const,CSkill *>,int,__gnu_cxx::hash<int>,std::_Select1st<std::pair<int const,CSkill *>>,std::equal_to<int>,std::allocator<CSkill *>>::_Hashtable_iterator();
    __gnu_cxx::hash_map<int,CSkill *,__gnu_cxx::hash<int>,std::equal_to<int>,std::allocator<CSkill *>>::find(
      v4,
      (int)this + 20 * a2,
      &index);
    v5[0] = v4[0];
    v5[1] = v4[1];
    __gnu_cxx::hash_map<int,CSkill *,__gnu_cxx::hash<int>,std::equal_to<int>,std::allocator<CSkill *>>::end(
      v8,
      (int)this + 20 * a2);
    if ( __gnu_cxx::_Hashtable_iterator<std::pair<int const,CSkill *>,int,__gnu_cxx::hash<int>,std::_Select1st<std::pair<int const,CSkill *>>,std::equal_to<int>,std::allocator<CSkill *>>::operator==(
           v5,
           v8) )
    {
      std::pair<int const,CSkill *>::pair<int &,CSkill *&>(v10, (int)&index, (int)&a3);
      __gnu_cxx::hash_map<int,CSkill *,__gnu_cxx::hash<int>,std::equal_to<int>,std::allocator<CSkill *>>::insert(
        (int)v9,
        (int)this + 20 * a2,
        v10);
      group = CSkill::get_group(a3);
      std::pair<int const,CSkill *>::pair<int,CSkill *&>(v12, (int)&group, (int)&a3);
      return std::multimap<int,CSkill *>::insert(&v11, (int)this + 24 * a2 + 220, (int)v12);
    }
    else
    {
      cMyTrace::cMyTrace((cMyTrace *)v14, "void CSkillList::insert_skill(int, CSkill*)", 4004, 5);
      return (_DWORD *)cMyTrace::operator()(
                         (int)v14,
                         "[%s][%d]_( itr != m_SkillList[job_type].end() )",
                         (char)"void CSkillList::insert_skill(int, CSkill*)");
    }
  }
  else
  {
    cMyTrace::cMyTrace((cMyTrace *)v7, "void CSkillList::insert_skill(int, CSkill*)", 3985, 5);
    return (_DWORD *)cMyTrace::operator()(
                       (int)v7,
                       "[%s][%d]_(0 == pSkill)",
                       (char)"void CSkillList::insert_skill(int, CSkill*)");
  }
}

//----- (08351B74) --------------------------------------------------------
int CSkillList::find_skill(CSkillList *this, int id, ...)
{
  _DWORD value[2]; // [esp+18h] [ebp-20h] BYREF
  _DWORD value[2]; // [esp+20h] [ebp-18h] BYREF
  _DWORD value[4]; // [esp+28h] [ebp-10h] BYREF
  va_list va; // [esp+48h] [ebp+10h] BYREF

  va_start(va, a2);
  if ( a2 > 10 )
    return 0;
  __gnu_cxx::_Hashtable_iterator<std::pair<int const,CSkill *>,int,__gnu_cxx::hash<int>,std::_Select1st<std::pair<int const,CSkill *>>,std::equal_to<int>,std::allocator<CSkill *>>::_Hashtable_iterator();
  __gnu_cxx::hash_map<int,CSkill *,__gnu_cxx::hash<int>,std::equal_to<int>,std::allocator<CSkill *>>::find(
    v3,
    (int)this + 20 * a2,
    (int *)va);
  v4[0] = v3[0];
  v4[1] = v3[1];
  __gnu_cxx::hash_map<int,CSkill *,__gnu_cxx::hash<int>,std::equal_to<int>,std::allocator<CSkill *>>::end(
    v5,
    (int)this + 20 * a2);
  if ( __gnu_cxx::_Hashtable_iterator<std::pair<int const,CSkill *>,int,__gnu_cxx::hash<int>,std::_Select1st<std::pair<int const,CSkill *>>,std::equal_to<int>,std::allocator<CSkill *>>::operator==(
         v4,
         v5) )
  {
    return 0;
  }
  else
  {
    return *(_DWORD *)(__gnu_cxx::_Hashtable_iterator<std::pair<int const,CSkill *>,int,__gnu_cxx::hash<int>,std::_Select1st<std::pair<int const,CSkill *>>,std::equal_to<int>,std::allocator<CSkill *>>::operator->(v4)
                     + 4);
  }
}

//----- (08351C1E) --------------------------------------------------------
int __cdecl CSkillList::get_ran_groupskill_index(CSkillList *this, int id, int id2, int id3)
{
  int index; // eax
  const std::_Rb_tree_node_base *v6; // [esp+1Ch] [ebp-4Ch] BYREF
  const std::_Rb_tree_node_base *v7; // [esp+20h] [ebp-48h] BYREF
  int id; // [esp+24h] [ebp-44h] BYREF
  _DWORD value[2]; // [esp+28h] [ebp-40h] BYREF
  _BYTE dest[4]; // [esp+30h] [ebp-38h] BYREF
  _BYTE v11[4]; // [esp+34h] [ebp-34h] BYREF
  _BYTE v12[4]; // [esp+38h] [ebp-30h] BYREF
  int id[4]; // [esp+3Ch] [ebp-2Ch] BYREF
  _BYTE v14[4]; // [esp+4Ch] [ebp-1Ch] BYREF
  int id; // [esp+50h] [ebp-18h]
  signed int rand_int; // [esp+54h] [ebp-14h]
  CSkill *v17; // [esp+58h] [ebp-10h]
  signed int i; // [esp+5Ch] [ebp-Ch]

  v15 = std::multimap<int,CSkill *>::count((int)this + 24 * a2 + 220, &a3);
  if ( !v15 )
    return -1;
  rand_int = get_rand_int(v15);
  std::multimap<int,CSkill *>::equal_range(v9, (int)this + 24 * a2 + 220, &a3);
  std::pair<std::_Rb_tree_const_iterator<std::pair<int const,CSkill *>>,std::_Rb_tree_const_iterator<std::pair<int const,CSkill *>>>::pair<std::_Rb_tree_iterator<std::pair<int const,CSkill *>>,std::_Rb_tree_iterator<std::pair<int const,CSkill *>>>(
    &v7,
    (int)v9);
  v6 = v7;
  for ( i = 0; i < rand_int; ++i )
    std::_Rb_tree_const_iterator<std::pair<int const,CSkill *>>::operator++(dest, &v6);
  v17 = *(CSkill **)(std::_Rb_tree_const_iterator<std::pair<int const,CSkill *>>::operator->(&v6) + 4);
  index = CSkill::get_index(v17);
  if ( index == a4 )
  {
    std::_Rb_tree_const_iterator<std::pair<int const,CSkill *>>::operator++(v11, &v6);
    if ( std::_Rb_tree_const_iterator<std::pair<int const,CSkill *>>::operator==(&v6, &v8) )
    {
      std::_Rb_tree_const_iterator<std::pair<int const,CSkill *>>::operator--(
        v12,
        (std::_Rb_tree_node_base::_Base_ptr *)&v6);
      if ( std::_Rb_tree_const_iterator<std::pair<int const,CSkill *>>::operator==(&v6, &v7) )
      {
        cMyTrace::cMyTrace((cMyTrace *)v13, "int CSkillList::get_ran_groupskill_index(int, int, int)", 4062, 5);
        cMyTrace::operator()(
          (int)v13,
          "[%s][%d]_( i == res.first )",
          (char)"int CSkillList::get_ran_groupskill_index(int, int, int)");
      }
      else
      {
        std::_Rb_tree_const_iterator<std::pair<int const,CSkill *>>::operator--(
          v14,
          (std::_Rb_tree_node_base::_Base_ptr *)&v6);
        v17 = *(CSkill **)(std::_Rb_tree_const_iterator<std::pair<int const,CSkill *>>::operator->(&v6) + 4);
      }
    }
    else
    {
      v17 = *(CSkill **)(std::_Rb_tree_const_iterator<std::pair<int const,CSkill *>>::operator->(&v6) + 4);
    }
  }
  return CSkill::get_index(v17);
}

//----- (08351E18) --------------------------------------------------------
void __cdecl CSkillList::set_post_learning_skill(CSkillList *this, int value, CSkill *a3)
{
  _DWORD *v3; // eax
  int value; // eax
  int value; // eax
  void *v6[3]; // [esp+14h] [ebp-44h] BYREF
  int value[2]; // [esp+20h] [ebp-38h] BYREF
  int value; // [esp+28h] [ebp-30h] BYREF
  _DWORD *v9; // [esp+2Ch] [ebp-2Ch] BYREF
  int value; // [esp+30h] [ebp-28h] BYREF
  int value; // [esp+34h] [ebp-24h] BYREF
  _BYTE v12[8]; // [esp+38h] [ebp-20h] BYREF
  _DWORD value[6]; // [esp+40h] [ebp-18h] BYREF

  if ( a3 && std::list<_pre_skill>::size((int *)a3 + 6) )
  {
    std::list<_pre_skill>::begin(&v9, (int *)a3 + 6);
    while ( 1 )
    {
      std::list<_pre_skill>::end(&v10, (int)a3 + 24);
      if ( !std::_List_iterator<_pre_skill>::operator!=(&v9, &v10) )
        break;
      v3 = (_DWORD *)std::_List_iterator<_pre_skill>::operator->(&v9);
      std::map<int,std::vector<std::pair<int,int>>>::find(&v8, (int)this + 24 * a2 + 484, v3);
      std::pair<int,int>::pair(v7);
      v7[0] = CSkill::get_index(a3);
      v7[1] = *(_DWORD *)(std::_List_iterator<_pre_skill>::operator->(&v9) + 4);
      std::map<int,std::vector<std::pair<int,int>>>::end(&v11, (int)this + 24 * a2 + 484);
      if ( std::_Rb_tree_iterator<std::pair<int const,std::vector<std::pair<int,int>>>>::operator==(&v8, &v11) )
      {
        std::vector<std::pair<int,int>>::vector(v6);
        std::vector<std::pair<int,int>>::push_back((int)v6, v7);
        v4 = std::_List_iterator<_pre_skill>::operator->(&v9);
        std::pair<int const,std::vector<std::pair<int,int>>>::pair<int &,std::vector<std::pair<int,int>>&>(
          v13,
          v4,
          (int)v6);
        std::map<int,std::vector<std::pair<int,int>>>::insert((int)v12, (int)this + 24 * a2 + 484, (int)v13);
        std::pair<int const,std::vector<std::pair<int,int>>>::~pair((int)v13);
        std::vector<std::pair<int,int>>::~vector(v6);
      }
      else
      {
        v5 = std::_Rb_tree_iterator<std::pair<int const,std::vector<std::pair<int,int>>>>::operator->(&v8);
        std::vector<std::pair<int,int>>::push_back(v5 + 4, v7);
      }
      std::_List_iterator<_pre_skill>::operator++(&v9);
    }
  }
}

//----- (08352028) --------------------------------------------------------
int __cdecl CSkillList::get_post_learning_skill(int id, int id2, int id3, int *a4)
{
  int id; // eax
  int id; // [esp+18h] [ebp-10h] BYREF
  _DWORD value[3]; // [esp+1Ch] [ebp-Ch] BYREF

  std::map<int,std::vector<std::pair<int,int>>>::find(&v6, a1 + 24 * a2 + 480 + 4, &a3);
  std::map<int,std::vector<std::pair<int,int>>>::end(v7, a1 + 24 * a2 + 480 + 4);
  if ( !std::_Rb_tree_iterator<std::pair<int const,std::vector<std::pair<int,int>>>>::operator!=(&v6, v7) )
    return 0;
  v4 = std::_Rb_tree_iterator<std::pair<int const,std::vector<std::pair<int,int>>>>::operator->(&v6);
  std::vector<std::pair<int,int>>::operator=(a4, (int *)(v4 + 4));
  return 1;
}

//----- (083520C8) --------------------------------------------------------
char __cdecl makeRewardItemInfo(_DWORD *a1, int count)
{
  char result; // al
  int count; // [esp+14h] [ebp-34h] BYREF
  int count[2]; // [esp+18h] [ebp-30h] BYREF
  int count; // [esp+20h] [ebp-28h] BYREF
  int count; // [esp+24h] [ebp-24h] BYREF
  int count; // [esp+28h] [ebp-20h] BYREF
  int count[4]; // [esp+2Ch] [ebp-1Ch] BYREF
  _DWORD value[3]; // [esp+3Ch] [ebp-Ch] BYREF

  std::pair<int,int>::pair(v4);
  std::vector<int>::begin(&v3, a1);
  while ( 1 )
  {
    std::vector<int>::end(&v5, (int)a1);
    result = __gnu_cxx::operator!=<int const*,std::vector<int>>((int)&v3, (int)&v5);
    if ( !result )
      break;
    v4[0] = *(_DWORD *)__gnu_cxx::__normal_iterator<int const*,std::vector<int>>::operator*((int)&v3);
    __gnu_cxx::__normal_iterator<int const*,std::vector<int>>::operator++(&v6, &v3);
    std::vector<int>::end(&v7, (int)a1);
    if ( __gnu_cxx::operator==<int const*,std::vector<int>>((int)&v3, (int)&v7) )
    {
      cMyTrace::cMyTrace((cMyTrace *)v8, "void makeRewardItemInfo(const INT_VECTOR&, IntPairVector&)", 4145, 5);
      return cMyTrace::operator()(
               (int)v8,
               "[%s][%d]_it == data.rewardSelectItems_.end()",
               (char)"void makeRewardItemInfo(const INT_VECTOR&, IntPairVector&)");
    }
    v4[1] = *(_DWORD *)__gnu_cxx::__normal_iterator<int const*,std::vector<int>>::operator*((int)&v3);
    __gnu_cxx::__normal_iterator<int const*,std::vector<int>>::operator++(v9, &v3);
    std::vector<std::pair<int,int>>::push_back(a2, v4);
  }
  return result;
}

//----- (08352203) --------------------------------------------------------
char __cdecl makeRewardItemInfo(_DWORD *a1, int count)
{
  char result; // al
  int count; // [esp+1Ch] [ebp-3Ch] BYREF
  _DWORD value[4]; // [esp+20h] [ebp-38h] BYREF
  int count; // [esp+30h] [ebp-28h] BYREF
  int count; // [esp+34h] [ebp-24h] BYREF
  int count; // [esp+38h] [ebp-20h] BYREF
  int count[4]; // [esp+3Ch] [ebp-1Ch] BYREF
  _DWORD value[3]; // [esp+4Ch] [ebp-Ch] BYREF

  RewardItemInfo::RewardItemInfo((RewardItemInfo *)v4);
  std::vector<int>::begin(&v3, a1);
  while ( 1 )
  {
    std::vector<int>::end(&v5, (int)a1);
    result = __gnu_cxx::operator!=<int const*,std::vector<int>>((int)&v3, (int)&v5);
    if ( !result )
      break;
    v4[0] = *(_DWORD *)__gnu_cxx::__normal_iterator<int const*,std::vector<int>>::operator*((int)&v3);
    __gnu_cxx::__normal_iterator<int const*,std::vector<int>>::operator++(&v6, &v3);
    std::vector<int>::end(&v7, (int)a1);
    if ( __gnu_cxx::operator==<int const*,std::vector<int>>((int)&v3, (int)&v7) )
    {
      cMyTrace::cMyTrace((cMyTrace *)v8, "void makeRewardItemInfo(const INT_VECTOR&, RewardItemInfoVector&)", 4173, 5);
      return cMyTrace::operator()(
               (int)v8,
               "[%s][%d]_it == data.rewardSelectItems_.end()",
               (char)"void makeRewardItemInfo(const INT_VECTOR&, RewardItemInfoVector&)");
    }
    v4[1] = *(_DWORD *)__gnu_cxx::__normal_iterator<int const*,std::vector<int>>::operator*((int)&v3);
    __gnu_cxx::__normal_iterator<int const*,std::vector<int>>::operator++(v9, &v3);
    std::vector<RewardItemInfo>::push_back(a2, v4);
  }
  return result;
}

//----- (0835233E) --------------------------------------------------------
int *__cdecl Quest::set_quest(int value, int value2)
{
  int value; // [esp+14h] [ebp-54h] BYREF
  int value[2]; // [esp+18h] [ebp-50h] BYREF
  int value[2]; // [esp+20h] [ebp-48h] BYREF
  int value; // [esp+28h] [ebp-40h] BYREF
  int value; // [esp+2Ch] [ebp-3Ch] BYREF
  int value; // [esp+30h] [ebp-38h] BYREF
  int value; // [esp+34h] [ebp-34h] BYREF
  int value[4]; // [esp+38h] [ebp-30h] BYREF
  int value; // [esp+48h] [ebp-20h] BYREF
  int value; // [esp+4Ch] [ebp-1Ch] BYREF
  int value; // [esp+50h] [ebp-18h] BYREF
  int value; // [esp+54h] [ebp-14h] BYREF
  signed int value; // [esp+58h] [ebp-10h]
  signed int i; // [esp+5Ch] [ebp-Ch]

  std::string::operator=((std::string *)a1, (const std::string *)(a2 + 4));
  *(_DWORD *)(a1 + 4) = *(_DWORD *)a2;
  *(_DWORD *)(a1 + 8) = *(_DWORD *)(a2 + 212);
  *(_DWORD *)(a1 + 12) = *(_DWORD *)(a2 + 228);
  std::vector<stQuestTargetCharacter_t>::operator=((int *)(a1 + 16), (int *)(a2 + 244));
  *(_DWORD *)(a1 + 28) = *(_DWORD *)(a2 + 220);
  *(_DWORD *)(a1 + 40) = *(_DWORD *)(a2 + 292);
  *(_DWORD *)(a1 + 32) = *(_DWORD *)(a2 + 256);
  *(_DWORD *)(a1 + 36) = *(_DWORD *)(a2 + 260);
  std::vector<std::vector<int>>::operator=(a1 + 44, (int *)(a2 + 268));
  std::vector<int>::operator=((int *)(a1 + 56), (int *)(a2 + 280));
  *(_BYTE *)(a1 + 68) = *(_BYTE *)(a2 + 304);
  std::vector<DungeonClearItem>::operator=((int *)(a1 + 72), (int *)(a2 + 312));
  std::vector<MonsterKillItem>::operator=((int *)(a1 + 84), (int *)(a2 + 324));
  *(_DWORD *)(a1 + 96) = *(_DWORD *)(a2 + 336);
  *(_DWORD *)(a1 + 100) = *(_DWORD *)(a2 + 340);
  *(_DWORD *)(a1 + 104) = *(_DWORD *)(a2 + 344);
  *(_DWORD *)(a1 + 108) = *(_DWORD *)(a2 + 348);
  std::vector<std::pair<int,int>>::operator=((int *)(a1 + 112), (int *)(a2 + 352));
  *(_DWORD *)(a1 + 124) = *(_DWORD *)(a2 + 376);
  *(_DWORD *)(a1 + 128) = *(_DWORD *)(a2 + 380);
  std::vector<int>::operator=((int *)(a1 + 132), (int *)(a2 + 384));
  std::vector<int>::operator=((int *)(a1 + 144), (int *)(a2 + 432));
  PostalReward::operator=((std::string *)(a1 + 156), (const std::string *)(a2 + 664));
  *(_BYTE *)(a1 + 257) = *(_BYTE *)(a2 + 558);
  *(_BYTE *)(a1 + 304) = *(_BYTE *)(a2 + 264);
  if ( *(_DWORD *)(a1 + 124) )
  {
    if ( *(_DWORD *)(a1 + 124) == 25 )
    {
      std::vector<std::pair<int,int>>::clear((int *)(a1 + 172));
      std::pair<int,int>::pair(v5);
      v16 = (unsigned int)std::vector<int>::size((_DWORD *)(a1 + 132)) >> 1;
      std::vector<int>::begin(&v4, (_DWORD *)(a1 + 132));
      for ( i = 0; i < v16; ++i )
      {
        std::vector<int>::end(&v13, a1 + 132);
        if ( !__gnu_cxx::operator!=<int *,std::vector<int>>((int)&v4, (int)&v13) )
          break;
        __gnu_cxx::__normal_iterator<int *,std::vector<int>>::operator++(&v14, &v4);
        v5[0] = *(_DWORD *)__gnu_cxx::__normal_iterator<int *,std::vector<int>>::operator*((int)&v14);
        __gnu_cxx::__normal_iterator<int *,std::vector<int>>::operator++(&v15, &v4);
        v5[1] = *(_DWORD *)__gnu_cxx::__normal_iterator<int *,std::vector<int>>::operator*((int)&v15);
        std::vector<std::pair<int,int>>::push_back(a1 + 172, v5);
      }
    }
  }
  else
  {
    std::vector<int>::begin(&v7, (_DWORD *)(a1 + 132));
    std::pair<int,int>::pair(v6);
    while ( 1 )
    {
      std::vector<int>::end(&v8, a1 + 132);
      if ( !__gnu_cxx::operator!=<int *,std::vector<int>>((int)&v7, (int)&v8) )
        break;
      v6[0] = *(_DWORD *)__gnu_cxx::__normal_iterator<int *,std::vector<int>>::operator*((int)&v7);
      __gnu_cxx::__normal_iterator<int *,std::vector<int>>::operator++(&v9, &v7);
      std::vector<int>::end(&v10, a1 + 132);
      if ( __gnu_cxx::operator==<int *,std::vector<int>>((int)&v7, (int)&v10) )
      {
        cMyTrace::cMyTrace((cMyTrace *)v11, "void Quest::set_quest(QuestScript)", 4253, 5);
        cMyTrace::operator()((int)v11, "[%s][%d]_(it == intDatas_.end())", (char)"void Quest::set_quest(QuestScript)");
        break;
      }
      v6[1] = *(_DWORD *)__gnu_cxx::__normal_iterator<int *,std::vector<int>>::operator*((int)&v7);
      __gnu_cxx::__normal_iterator<int *,std::vector<int>>::operator++(&v12, &v7);
      std::vector<std::pair<int,int>>::push_back(a1 + 172, v6);
    }
  }
  std::vector<int>::clear((int *)(a1 + 220));
  *(_DWORD *)(a1 + 232) = 0;
  std::vector<RewardItemInfo>::clear((int *)(a1 + 236));
  *(_DWORD *)(a1 + 184) = *(_DWORD *)(a2 + 468);
  *(_DWORD *)(a1 + 192) = *(_DWORD *)(a2 + 484);
  *(_DWORD *)(a1 + 188) = *(_DWORD *)(a2 + 552);
  if ( !*(_DWORD *)(a1 + 184) || *(_DWORD *)(a1 + 184) == 25 )
  {
    std::vector<RewardItemInfo>::operator=((int *)(a1 + 196), (int *)(a2 + 472));
    std::vector<RewardItemInfo>::operator=((int *)(a1 + 208), (int *)(a2 + 488));
    std::vector<int>::operator=((int *)(a1 + 220), (int *)(a2 + 524));
    *(_DWORD *)(a1 + 232) = *(_DWORD *)(a2 + 536);
    makeRewardItemInfo((_DWORD *)(a2 + 540), a1 + 236);
  }
  *(_DWORD *)(a1 + 248) = *(_DWORD *)(a2 + 296);
  *(_DWORD *)(a1 + 252) = *(_DWORD *)(a2 + 300);
  *(_BYTE *)(a1 + 256) = *(_BYTE *)(a2 + 557);
  *(_BYTE *)(a1 + 258) = *(_BYTE *)(a2 + 573);
  *(_DWORD *)(a1 + 260) = *(_DWORD *)(a2 + 576);
  *(_BYTE *)(a1 + 264) = *(_BYTE *)(a2 + 600);
  std::string::operator=((std::string *)(a1 + 268), (const std::string *)(a2 + 604));
  std::string::operator=((std::string *)(a1 + 272), (const std::string *)(a2 + 608));
  std::vector<int>::operator=((int *)(a1 + 276), (int *)(a2 + 612));
  *(_BYTE *)(a1 + 288) = *(_BYTE *)(a2 + 624);
  std::vector<dungeonEventHanaseStruct>::operator=(a1 + 292, (int *)(a2 + 512));
  *(_BYTE *)(a1 + 305) = *(_BYTE *)(a2 + 626);
  *(_DWORD *)(a1 + 308) = 0;
  *(_DWORD *)(a1 + 308) = *(_DWORD *)(a2 + 216);
  *(_WORD *)(a1 + 312) = *(_WORD *)(a2 + 640);
  *(_WORD *)(a1 + 314) = *(_WORD *)(a2 + 642);
  *(_DWORD *)(a1 + 316) = *(_DWORD *)(a2 + 680);
  *(_DWORD *)(a1 + 320) = *(_DWORD *)(a2 + 684);
  return std::vector<ENUM_QUEST_EXCEPTION_TYPE>::operator=((int *)(a1 + 324), (int *)(a2 + 720));
}

//----- (08352A5C) --------------------------------------------------------
int __cdecl Quest::checkPossibleJob(int id, int id2)
{
  int id; // [esp+18h] [ebp-10h] BYREF
  _DWORD value[3]; // [esp+1Ch] [ebp-Ch] BYREF

  std::vector<stQuestTargetCharacter_t>::begin(v5, (_DWORD *)(a1 + 16));
  std::vector<stQuestTargetCharacter_t>::end(&v4, a1 + 16);
  while ( __gnu_cxx::operator!=<stQuestTargetCharacter_t const*,std::vector<stQuestTargetCharacter_t>>(
            (int)v5,
            (int)&v4) )
  {
    if ( *(_DWORD *)__gnu_cxx::__normal_iterator<stQuestTargetCharacter_t const*,std::vector<stQuestTargetCharacter_t>>::operator->((int)v5) == a2
      || *(_DWORD *)__gnu_cxx::__normal_iterator<stQuestTargetCharacter_t const*,std::vector<stQuestTargetCharacter_t>>::operator->((int)v5) == 11 )
    {
      return 1;
    }
    __gnu_cxx::__normal_iterator<stQuestTargetCharacter_t const*,std::vector<stQuestTargetCharacter_t>>::operator++(v5);
  }
  return 0;
}

//----- (08352AFC) --------------------------------------------------------
int __cdecl Quest::checkMatchedGrowType(int id, int id2, int id3, int id4)
{
  int id; // [esp+18h] [ebp-10h] BYREF
  _DWORD value[3]; // [esp+1Ch] [ebp-Ch] BYREF

  std::vector<stQuestTargetCharacter_t>::begin(v7, (_DWORD *)(a1 + 16));
  std::vector<stQuestTargetCharacter_t>::end(&v6, a1 + 16);
  while ( __gnu_cxx::operator!=<stQuestTargetCharacter_t const*,std::vector<stQuestTargetCharacter_t>>(
            (int)v7,
            (int)&v6) )
  {
    if ( (*(_DWORD *)__gnu_cxx::__normal_iterator<stQuestTargetCharacter_t const*,std::vector<stQuestTargetCharacter_t>>::operator->((int)v7) == a2
       || *(_DWORD *)__gnu_cxx::__normal_iterator<stQuestTargetCharacter_t const*,std::vector<stQuestTargetCharacter_t>>::operator->((int)v7) == 11)
      && (*(_DWORD *)(__gnu_cxx::__normal_iterator<stQuestTargetCharacter_t const*,std::vector<stQuestTargetCharacter_t>>::operator->((int)v7)
                    + 4) == a3
       || *(_DWORD *)(__gnu_cxx::__normal_iterator<stQuestTargetCharacter_t const*,std::vector<stQuestTargetCharacter_t>>::operator->((int)v7)
                    + 4) == -1)
      && (*(_DWORD *)(__gnu_cxx::__normal_iterator<stQuestTargetCharacter_t const*,std::vector<stQuestTargetCharacter_t>>::operator->((int)v7)
                    + 8) == a4
       || *(_DWORD *)(__gnu_cxx::__normal_iterator<stQuestTargetCharacter_t const*,std::vector<stQuestTargetCharacter_t>>::operator->((int)v7)
                    + 8) == -1) )
    {
      return 1;
    }
    __gnu_cxx::__normal_iterator<stQuestTargetCharacter_t const*,std::vector<stQuestTargetCharacter_t>>::operator++(v7);
  }
  return 0;
}

//----- (08352BEE) --------------------------------------------------------
_BOOL4 __cdecl Quest::isRepeatableQuest(Quest *this)
{
  int id; // eax

  v1 = *((_DWORD *)this + 2);
  return v1 == 4 || v1 == 8;
}

//----- (08352C10) --------------------------------------------------------
int __cdecl Quest::_isNeededCheckGrowType(int id, int id2)
{
  int id; // [esp+18h] [ebp-10h] BYREF
  _DWORD value[3]; // [esp+1Ch] [ebp-Ch] BYREF

  std::vector<stQuestTargetCharacter_t>::begin(v5, (_DWORD *)(a1 + 16));
  std::vector<stQuestTargetCharacter_t>::end(&v4, a1 + 16);
  while ( __gnu_cxx::operator!=<stQuestTargetCharacter_t const*,std::vector<stQuestTargetCharacter_t>>(
            (int)v5,
            (int)&v4) )
  {
    if ( (*(_DWORD *)__gnu_cxx::__normal_iterator<stQuestTargetCharacter_t const*,std::vector<stQuestTargetCharacter_t>>::operator->((int)v5) == a2
       || *(_DWORD *)__gnu_cxx::__normal_iterator<stQuestTargetCharacter_t const*,std::vector<stQuestTargetCharacter_t>>::operator->((int)v5) == 11)
      && *(_DWORD *)(__gnu_cxx::__normal_iterator<stQuestTargetCharacter_t const*,std::vector<stQuestTargetCharacter_t>>::operator->((int)v5)
                   + 4) == -1 )
    {
      return 0;
    }
    __gnu_cxx::__normal_iterator<stQuestTargetCharacter_t const*,std::vector<stQuestTargetCharacter_t>>::operator++(v5);
  }
  return 1;
}

//----- (08352CC2) --------------------------------------------------------
int __cdecl Quest::check_power_side(Quest *this, char flag)
{
  char WinnerSide; // [esp+3Eh] [ebp-Ah]

  if ( *((_BYTE *)this + 305) == MASK_BYTE )
    return 1;
  WinnerSide = CPowerManager::GetWinnerSide(GlobalData::s_power_manager);
  if ( *((_BYTE *)this + 305) )
  {
    if ( *((_BYTE *)this + 305) != 1 )
    {
      LogManager::logFormat(
        (LogManager *)1,
        (int)"data_manager.cpp",
        "bool Quest::check_power_side(char)",
        (const char *)0x1161,
        "Quest Script Error q_index(%d)",
        *((const char **)this + 1));
      return 0;
    }
    if ( (unsigned __int8)CPowerManager::GetWinnerSide(GlobalData::s_power_manager) != a2 )
      return 1;
  }
  else if ( WinnerSide == a2 )
  {
    return 1;
  }
  return 0;
}

//----- (08352D86) --------------------------------------------------------
_BOOL4 __cdecl Quest::check_possible(Quest *this, const stSelectQuestParam *a2)
{
  int id; // ebx
  CDataManager *v4; // eax
  const char *v5; // [esp+14h] [ebp-34h]
  int id; // [esp+24h] [ebp-24h]
  int id; // [esp+28h] [ebp-20h]
  int id; // [esp+30h] [ebp-18h]
  int id; // [esp+34h] [ebp-14h]
  int id; // [esp+38h] [ebp-10h]
  int id; // [esp+3Ch] [ebp-Ch]

  v6 = *((_DWORD *)a2 + 1);
  v7 = *((_DWORD *)a2 + 2);
  v8 = *(_DWORD *)a2;
  v9 = *((_DWORD *)a2 + 4);
  v10 = *((_DWORD *)a2 + 7);
  v11 = *((_DWORD *)a2 + 8);
  if ( (unsigned __int8)Quest::checkPossibleJob((int)this, v6) != 1 )
    return 0;
  if ( (unsigned __int8)Quest::_isNeededCheckGrowType((int)this, v6) )
  {
    if ( *((_DWORD *)this + 7) == 1 )
    {
      LogManager::logFormat(
        (LogManager *)1,
        (int)"data_manager.cpp",
        "bool Quest::check_possible(const stSelectQuestParam&) const",
        (const char *)0x1193,
        "Quest Script Error Break !, [grow type] != -1 and [job change quest] = 1",
        v5);
      return 0;
    }
    if ( *((_DWORD *)this + 7) == 2 )
    {
      if ( (unsigned __int8)Quest::checkMatchedGrowType((int)this, v6, v7, -1) != 1 )
        return 0;
    }
    else if ( *((_DWORD *)this + 7) != 10
           && *((_DWORD *)this + 7) != 20
           && (unsigned __int8)Quest::checkMatchedGrowType((int)this, v6, v7, -1) != 1 )
    {
      return 0;
    }
  }
  if ( *((_DWORD *)this + 8) > v8 )
    return 0;
  if ( *((_DWORD *)this + 9) < v8 )
    return 0;
  if ( *((_DWORD *)this + 10) && *((_DWORD *)this + 10) > v9 )
    return 0;
  if ( *((_DWORD *)this + 2) == 1 )
  {
    v3 = *((_DWORD *)this + 1);
    v4 = G_CDataManager();
    if ( (unsigned __int8)CDataManager::isThereDailyTrainingQuestList(v4, v8, v3) != 1 )
      return 0;
  }
  if ( !*((_DWORD *)this + 79) || *((_DWORD *)this + 80) == -1 )
    return 1;
  if ( *((_DWORD *)this + 79) == -1 || *((_DWORD *)this + 79) == v10 )
    return *((_DWORD *)this + 80) <= v11;
  return 0;
}
// 8352E3A: variable 'v5' is possibly undefined

//----- (08352FB4) --------------------------------------------------------
int __cdecl Quest::get_init_achievement_trigger(
        Quest *this,
        unsigned __int16 *a2,
        unsigned __int16 *a3,
        unsigned __int16 *a4)
{
  unsigned __int16 *v5; // ebx
  unsigned int id; // eax
  unsigned __int16 *v7; // [esp+10h] [ebp-18h]
  unsigned __int16 *v8; // [esp+14h] [ebp-14h]
  unsigned __int16 *v9; // [esp+18h] [ebp-10h]
  unsigned int i; // [esp+1Ch] [ebp-Ch]

  if ( *((_DWORD *)this + 2) != 7 )
    return 0;
  v7 = a2;
  v8 = a3;
  v9 = a4;
  *a2 = 1;
  *v8 = 0;
  *v9 = 0;
  for ( i = 0; ; ++i )
  {
    v6 = (unsigned int)std::vector<int>::size((_DWORD *)this + 36) > 2 ? 3 : std::vector<int>::size((_DWORD *)this + 36);
    if ( v6 <= i )
      break;
    v5 = (&v7)[i];
    *v5 = *(_DWORD *)std::vector<int>::operator[]((_DWORD *)this + 36, i);
  }
  return 1;
}

//----- (08353082) --------------------------------------------------------
int __cdecl Quest::get_title_reward(Quest *this)
{
  if ( *((_DWORD *)this + 2) != 7 )
    return 0;
  if ( std::vector<RewardItemInfo>::size((_DWORD *)this + 49) )
    return *(_DWORD *)std::vector<RewardItemInfo>::operator[]((_DWORD *)this + 49, 0);
  return 0;
}

//----- (083530D6) --------------------------------------------------------
int __cdecl Quest::get_init_trigger(Quest *this)
{
  _BYTE v2[8]; // [esp+14h] [ebp-54h] BYREF
  _BYTE v3[10]; // [esp+1Ch] [ebp-4Ch] BYREF
  int id; // [esp+26h] [ebp-42h]
  __int16 v5; // [esp+2Ah] [ebp-3Eh]
  int id; // [esp+2Ch] [ebp-3Ch] BYREF
  int id; // [esp+30h] [ebp-38h] BYREF
  int id; // [esp+34h] [ebp-34h] BYREF
  int id; // [esp+38h] [ebp-30h] BYREF
  int id; // [esp+3Ch] [ebp-2Ch] BYREF
  int id; // [esp+40h] [ebp-28h] BYREF
  int id; // [esp+44h] [ebp-24h] BYREF
  int id; // [esp+48h] [ebp-20h]
  int id; // [esp+4Ch] [ebp-1Ch]
  int id; // [esp+50h] [ebp-18h]
  int id; // [esp+54h] [ebp-14h]
  int id; // [esp+58h] [ebp-10h]
  int id; // [esp+5Ch] [ebp-Ch]

  if ( *((_DWORD *)this + 31) == 2 || *((_DWORD *)this + 31) == 6 )
  {
    std::vector<int>::begin(&v7, (_DWORD *)this + 33);
    std::vector<int>::end(&v6, (int)this + 132);
    v4 = 0;
    v5 = 0;
    v13 = 0;
    while ( __gnu_cxx::operator!=<int const*,std::vector<int>>((int)&v7, (int)&v6) )
    {
      __gnu_cxx::__normal_iterator<int const*,std::vector<int>>::operator++(&v8, &v7);
      v14 = *(_DWORD *)__gnu_cxx::__normal_iterator<int const*,std::vector<int>>::operator*((int)&v8);
      __gnu_cxx::__normal_iterator<int const*,std::vector<int>>::operator++(&v9, &v7);
      v15 = *(_DWORD *)__gnu_cxx::__normal_iterator<int const*,std::vector<int>>::operator*((int)&v9);
      __gnu_cxx::__normal_iterator<int const*,std::vector<int>>::operator++(&v10, &v7);
      v16 = *(_DWORD *)__gnu_cxx::__normal_iterator<int const*,std::vector<int>>::operator*((int)&v10);
      if ( *((_DWORD *)this + 31) == 6 )
      {
        __gnu_cxx::__normal_iterator<int const*,std::vector<int>>::operator++(&v11, &v7);
        __gnu_cxx::__normal_iterator<int const*,std::vector<int>>::operator*((int)&v11);
      }
      __gnu_cxx::__normal_iterator<int const*,std::vector<int>>::operator++(&v12, &v7);
      v17 = *(_DWORD *)__gnu_cxx::__normal_iterator<int const*,std::vector<int>>::operator*((int)&v12);
      *((_WORD *)&v4 + v13++) = v17;
    }
    stQuestTriggerState_t::stQuestTriggerState_t((stQuestTriggerState_t *)v3);
    stQuestTriggerState_t::setTrigger((stQuestTriggerState_t *)v3, v4, SHIWORD(v4), v5);
    return stQuestTriggerState_t::getTrigger((stQuestTriggerState_t *)v3);
  }
  else if ( *((_DWORD *)this + 31) == 25 )
  {
    stQuestTriggerState_t::stQuestTriggerState_t((stQuestTriggerState_t *)v2);
    stQuestTriggerState_t::setTrigger((stQuestTriggerState_t *)v2, 1, 1, 0);
    return stQuestTriggerState_t::getTrigger((stQuestTriggerState_t *)v2);
  }
  else if ( *((_DWORD *)this + 31) == 1
         && *((_DWORD *)this + 32) == 6
         && std::vector<int>::size((_DWORD *)this + 33) == 3
         && (v18 = *(_DWORD *)std::vector<int>::at((_DWORD *)this + 33, 2u), v18 > 0) )
  {
    return v18;
  }
  else
  {
    return 1;
  }
}

//----- (08353342) --------------------------------------------------------
int __cdecl Quest::check_clear_item(Quest *this, int id, int id2)
{
  int *v3; // eax
  int id; // eax
  int id; // [esp+14h] [ebp-24h] BYREF
  int id; // [esp+18h] [ebp-20h]
  int id; // [esp+1Ch] [ebp-1Ch]
  int id; // [esp+20h] [ebp-18h]
  int id; // [esp+24h] [ebp-14h]
  int id; // [esp+28h] [ebp-10h]
  signed int i; // [esp+2Ch] [ebp-Ch]

  if ( std::vector<DungeonClearItem>::size((_DWORD *)this + 18) )
  {
    DungeonClearItem::DungeonClearItem((DungeonClearItem *)&v6);
    for ( i = 0; ; ++i )
    {
      v5 = std::vector<DungeonClearItem>::size((_DWORD *)this + 18);
      if ( v5 <= i )
        break;
      v3 = (int *)std::vector<DungeonClearItem>::at((_DWORD *)this + 18, i);
      v6 = *v3;
      v7 = v3[1];
      v8 = v3[2];
      v9 = v3[3];
      v10 = v3[4];
      v11 = v3[5];
      if ( v7 == -1 )
      {
        if ( v6 == a2 )
          return 1;
      }
      else if ( v6 == a2 && v7 == a3 )
      {
        return 1;
      }
    }
  }
  return 0;
}

//----- (08353404) --------------------------------------------------------
Quest *__stdcall Quest::get_clear_itemlist(Quest *this, int id, int id2)
{
  int id; // eax
  char *v4; // esi
  CItemList **v5; // eax
  int id; // eax
  char *v8; // [esp+2Ch] [ebp-4Ch] BYREF
  int id; // [esp+30h] [ebp-48h]
  _DWORD value[2]; // [esp+34h] [ebp-44h] BYREF
  char *v11; // [esp+3Ch] [ebp-3Ch]
  int id; // [esp+40h] [ebp-38h]
  signed int id; // [esp+44h] [ebp-34h]
  int id; // [esp+48h] [ebp-30h]
  CItem *item; // [esp+4Ch] [ebp-2Ch]
  signed int rand_int; // [esp+50h] [ebp-28h]
  signed int i; // [esp+54h] [ebp-24h]
  int id; // [esp+58h] [ebp-20h]
  int j; // [esp+5Ch] [ebp-1Ch]

  DungeonClearItem::DungeonClearItem((DungeonClearItem *)v10);
  std::vector<std::pair<int,int>>::vector(this);
  std::pair<int,int>::pair(&v8);
  item = 0;
  rand_int = 0;
  for ( i = 0; ; ++i )
  {
    v6 = std::vector<DungeonClearItem>::size((_DWORD *)(a2 + 72));
    if ( v6 <= i )
      break;
    v3 = std::vector<DungeonClearItem>::at((_DWORD *)(a2 + 72), i);
    v10[0] = *(_DWORD *)v3;
    v10[1] = *(_DWORD *)(v3 + 4);
    v11 = *(char **)(v3 + 8);
    v12 = *(_DWORD *)(v3 + 12);
    v13 = *(_DWORD *)(v3 + 16);
    v14 = *(_DWORD *)(v3 + 20);
    if ( v10[0] == a3 )
    {
      v4 = v11;
      v5 = (CItemList **)G_CDataManager();
      item = (CItem *)CDataManager::find_item(v5, (unsigned int)v4);
      if ( item )
      {
        if ( (unsigned __int8)CItem::is_stackable(item) )
        {
          v18 = 0;
          for ( j = 0; v12 > j; ++j )
          {

          need_sp_for_level = CSkill::get_need_sp_for_level(skill, 0, *(_DWORD *)(v9 + 4));
          v58 += need_sp_for_level;

          v63 = CSkill::get_need_sp_for_level(v62, 0, *(_DWORD *)(v16 + 4));
          v58 -= v63;

        index = (char *)CSkill::get_index(*(CSkill **)(v4 + 4));
        cMyTrace::cMyTrace((cMyTrace *)v60, "bool CDataManager::Verify() const", 10030, 9, 1, 1);

void __cdecl CSkill::CSkill(CSkill *this)
{
  STSecondGrowTypeFitness *v1; // ebx
  int i; // esi
  int j; // [esp+1Ch] [ebp-Ch]

  std::list<int>::list((_DWORD *)this + 2);
  std::list<int>::list((_DWORD *)this + 4);
  std::list<_pre_skill>::list((_DWORD *)this + 6);
  v1 = (CSkill *)((char *)this + 84);
  for ( i = 5; i != -1; --i )
  {
    STSecondGrowTypeFitness::STSecondGrowTypeFitness(v1);
    v1 = (STSecondGrowTypeFitness *)((char *)v1 + 16);
  }
  std::vector<int>::vector((_DWORD *)this + 57);
  std::vector<int>::vector((_DWORD *)this + 60);
  std::vector<std::vector<int>>::vector((_DWORD *)this + 66);
  std::vector<STKeyCommandUnit>::vector((_DWORD *)this + 73);
  memset((char *)this + 180, 0, 8u);
  *((_BYTE *)this + 288) = 0;
  std::vector<STKeyCommandUnit>::clear((STKeyCommandUnit **)this + 73);
  *((_BYTE *)this + 304) = 1;
  memset((char *)this + 305, 0, 0x64u);
  for ( j = 0; j <= 5; ++j )
    *((_DWORD *)this + j + 51) = -1;
}

CSkill *__cdecl CSkill::SetStealable(CSkill *this, bool a2)
{
  *((_BYTE *)this + 276) = a2;
  return this;
}

CSkill *__cdecl CSkill::SetStealLevelPenalty(CSkill *this, int value)
{
  *((_DWORD *)this + 71) = a2;
  return this;
}

CSkill *__cdecl CSkill::SetStealSPPenalty(CSkill *this, int value)
{
  *((_DWORD *)this + 70) = a2;
  return this;
}

CSkill *__cdecl CSkill::set_index(CSkill *this, int value)
{
  *(_DWORD *)this = a2;
  return this;
}

int __cdecl CSkill::get_index(CSkill *this)
{
  return *(_DWORD *)this;
}

CSkill *__cdecl CSkill::set_group(CSkill *this, int value)
{
  *((_DWORD *)this + 47) = a2;
  return this;
}

int __cdecl CSkill::get_group(CSkill *this)
{
  return *((_DWORD *)this + 47);
}

CSkill *__cdecl CSkill::set_job_type(CSkill *this, int value)
{
  *((_DWORD *)this + 1) = a2;
  return this;
}

CSkill *__cdecl CSkill::set_skill_type(CSkill *this, char flag)
{
  *((_BYTE *)this + 32) = a2;
  return this;
}

CSkill *__cdecl CSkill::set_level_limit(CSkill *this, int value)
{
  *((_DWORD *)this + 49) = a2;
  return this;
}

CSkill *__cdecl CSkill::set_fitness(CSkill *this, int value, int value2)
{
  *((_DWORD *)this + a2 + 9) = a3;
  return this;
}

CSkill *__cdecl CSkill::set_type_max_level(CSkill *this, int value, int value2)
{
  *((_DWORD *)this + a2 + 15) = a3;
  return this;
}

int __cdecl CSkill::get_type_max_level(CSkill *this, int id)
{
  return *((_DWORD *)this + a2 + 15);
}

CSkill *__cdecl CSkill::set_fitGrowType(CSkill *this, int value, int value2)
{
  *((_DWORD *)this + a2 + 51) = a3;
  return this;
}

char *__cdecl CSkill::setSkillName(CSkill *this, char *src)
{
  return strncpy((char *)this + 305, src, 0x64u);
}

  group = CSkill::get_group(*(CSkill **)&v9[3]);
  CurCharacSkillR = (CUserCharacInfo **)CUserCharacInfo::getCurCharacSkillR(a2);

    group = CSkill::get_group(skill);
    skillslot_no = 0;

    group = CSkill::get_group(skill);
    v19 = CItem::get_need_skill(a2);

    second_growtype_max_level = CSkill::get_second_growtype_max_level(skill, a4, a5);
    if ( !second_growtype_max_level )

      second_growtype_max_level = CSkill::get_type_max_level(skill, a4);
    if ( *a9 > second_growtype_max_level )

  else if ( CSkill::get_type_max_level(skill, a4) < *a9 )
  {
    return -3;
  }

  *a8 = CSkill::get_guildSkillNeedGold(skill, *a9);
  if ( *a8 < 0 || *((_DWORD *)a6 + 40) < (unsigned int)*a8 )

  *a10 = CSkill::get_guildSkillNeedGSP(skill, *a9);
  if ( *a10 < 0 || *a10 > a11 )

  if ( *((unsigned __int8 *)a6 + 27) < CSkill::get_needGuildLevel(skill, *a9) )
    return -6;

    SkillName = CSkill::getSkillName(skill);
    CUser::makeGuildSkillMessage(*this, SkillName, v16);

  group = CSkill::get_group(skill);
  if ( !*this )

  if ( (unsigned __int8)CSkill::IsStealable(skill, a5, a6) )
  {
    CurCharacQuestR = CUser::getCurCharacQuestR(*this);
    if ( (unsigned __int8)UserQuest::isClearQuest((UserQuest *)CurCharacQuestR, 0x163u) != 1
      && (unsigned __int8)CUser::is_clear_stealingSkillMission(*this) != 1 )
    {
      return -1;
    }
    v43 = 1;
    a3 -= CSkill::GetStealLevelPenalty(skill);
  }

    if ( !CSkill::can_learn(skill, 70, a5, a6, v40) )
      return -3;

  else if ( !CSkill::can_learn(skill, v47, a5, a6, v40) )
  {
    return -3;
  }

  second_growtype_fitness = CSkill::get_second_growtype_fitness(skill, a5, a6);
  if ( !second_growtype_fitness )

    second_growtype_fitness = CSkill::get_fitness(skill, a5);
  need_sp_sfp_for_level = CSkill::get_need_sp_sfp_for_level(skill, v41, v40);

    StealSPPenalty = CSkill::GetStealSPPenalty(skill);
    need_sp_sfp_for_level = getSkillSpendSPOnSteal(need_sp_sfp_for_level, StealSPPenalty);

  if ( CSkill::IsSpecialSkill(skill) )
  {
    if ( CUserCharacInfo::GetCurCharacSkillTreeIndex(*this) == -1 || !CUserCharacInfo::GetCurCharacSkillTreeIndex(*this) )
      cur_remain_sp = SkillSlot::get_cur_remain_sp(this, 2);
    else
      cur_remain_sp = SkillSlot::get_cur_remain_sp(this, 3);
  }

    is_active_skill = CSkill::is_active_skill(skill);
    skillslot_no = SkillSlot::insert_skill(this, a2, group, is_active_skill, v24, a7);

        group = CSkill::get_group(skill);
        if ( a4 )

            is_active_skill = CSkill::is_active_skill(skill);
            skillslot_no = SkillSlot::insert_skill(a1, a3, group, is_active_skill, a5, 1);

        else if ( skillslot_group == -1 && CSkill::get_group(skill) == v18 )
        {
          if ( CSkill::is_active_skill(v16) )
            is_active_skill = 1;
        }

        else if ( CSkill::get_group(v16) == skillslot_group && v18 == -1 )
        {
          if ( CSkill::is_active_skill(skill) )
            is_active_skill = 1;
        }

        else if ( CSkill::get_group(v16) == skillslot_group && CSkill::get_group(skill) == v18 )
        {
          is_active_skill = 1;
        }

        if ( CSkill::is_active_skill(skill) )
          is_active_skill = 1;

      else if ( CSkill::get_group(skill) == v18 )
      {
        is_active_skill = 1;
      }

      is_active_skill = CSkill::is_active_skill(v16);
    else

      is_active_skill = CSkill::get_group(v16) == skillslot_group;
  }
  else
  {

    group = CSkill::get_group(skill);
    v15 -= a4;

        if ( skill && CSkill::IsSpecialSkill(skill) )
        {
          *(_BYTE *)(a1 + 2 * (i + 232) + 14) = 0;
          *(_BYTE *)(a1 + 2 * (i + 232) + 15) = 0;
        }

      if ( v8 && CSkill::IsSpecialSkill(v8) )
      {
        *(_BYTE *)(a1 + 2 * (i + 32) + 6) = 0;
        *(_BYTE *)(a1 + 2 * (i + 32) + 7) = 0;
      }

    if ( CSkill::is_active_skill(skill) )
    {
      v14 = 0;
      v15 = 6;
      for ( j = 0; j < v15; ++j )
      {
        if ( !*((_BYTE *)a1 + 2 * j) )
        {
          v12 = 1;
          break;
        }
      }
    }

      group = CSkill::get_group(skill);
      if ( group == 1 )

      v5 = CSkill::get_group(skill);
      cMyTrace::cMyTrace((cMyTrace *)v8, "void addSkillOnCreateCharacter(_Mastered_skill*, int)", 1353, 5);

      if ( !CSkill::can_learn(skill, a2, a4, a5, *((char *)this + 2 * i + 71)) )
      {
        *((_BYTE *)this + 2 * i + 70) = 0;
        *((_BYTE *)this + 2 * i + 71) = 0;
      }

      if ( !CSkill::can_learn(v11, a2, a4, a5, *((char *)this + 2 * i + 479)) )
      {
        *((_BYTE *)this + 2 * i + 478) = 0;
        *((_BYTE *)this + 2 * i + 479) = 0;
      }

        if ( (unsigned __int8)CSkill::IsStealable(skill, v30, v29) )
        {
          v61 = 1;
          StealLevelPenalty = CSkill::GetStealLevelPenalty(skill);
          charac_level -= StealLevelPenalty;
        }

      if ( (unsigned __int8)CSkill::IsStealable(skill, CurCharFirstGrowType, CurCharSecondGrowType) )
      {
        v64 = 1;
        StealLevelPenalty = CSkill::GetStealLevelPenalty(skill);
        v65 -= StealLevelPenalty;
      }

      MaxSkillLevel = CSkill::getMaxSkillLevel(skill, v65, v18, v17);
      if ( MaxSkillLevel == -1 )

    second_growtype_fitness = CSkill::get_second_growtype_fitness(a7, CurCharFirstGrowType, CurCharSecondGrowType);
    if ( !second_growtype_fitness )

      second_growtype_fitness = CSkill::get_fitness(a7, v10);
    }
    need_sp_sfp_for_level = CSkill::get_need_sp_sfp_for_level(a7, i - 1, i);
    v14 = need_sp_sfp_for_level;
    if ( need_sp_sfp_for_level < 0 )
      break;
    if ( second_growtype_fitness > 0 )
    {

      StealSPPenalty = CSkill::GetStealSPPenalty(a7);
      need_sp_sfp_for_level = getSkillSpendSPOnSteal(v14, StealSPPenalty);

    if ( CSkill::IsSpecialSkill(a7) )
      *a3 += v14;

  group = CSkill::get_group(skill);
  if ( group == 4 )

              need_sp_sfp_for_level = CSkill::get_need_sp_sfp_for_level(
                                        skill,

              if ( (unsigned __int8)CSkill::IsStealable(skill, a5, a6) )
              {
                StealSPPenalty = CSkill::GetStealSPPenalty(skill);
                need_sp_sfp_for_level = getSkillSpendSPOnSteal(need_sp_sfp_for_level, StealSPPenalty);
              }

              if ( !CSkill::IsSpecialSkill(skill) )
              {
                v22 = (char *)CUserCharacInfo::GetCurCharacSkillTreeIndex(*this);
                remain_sp_at_index = SkillSlot::get_remain_sp_at_index((int)this, v22);
                v23 = CUserCharacInfo::GetCurCharacSkillTreeIndex(*this);
                SkillSlot::set_remain_sp_at_index((int)this, remain_sp_at_index + need_sp_sfp_for_level, v23);
                v24 = CUserCharacInfo::GetCurCharacSkillTreeIndex(*this);
                v25 = CUserCharacInfo::get_charac_job(*this);
                cUserHistoryLog::SkillDel((CUser **)*this + CUSER_OFFSET_HISTORY_LOG_ALT, v25, v24, a2, a7, 1);
                v26 = remain_sp_at_index + need_sp_sfp_for_level;
                v27 = CUserCharacInfo::GetCurCharacSkillTreeIndex(*this);
                cUserHistoryLog::SPAdd((CUser **)*this + CUSER_OFFSET_HISTORY_LOG_ALT, v27, v26, need_sp_sfp_for_level, 2);
                if ( a7 > 0 && need_sp_sfp_for_level / a7 > STATIC_POOL_SIZE_1000 )
                {
                  CurCharacNo = CUserCharacInfo::getCurCharacNo(*this);
                  acc_id = CUser::get_acc_id(*this);
                  v30 = (const char *)NumberToString(acc_id, 0);
                  LogManager::logFormat(
                    (LogManager *)1,
                    (int)"skill_slot.cpp",
                    "int SkillSlot::refund_skill(int, int, int, int, int, int, int&)",
                    (const char *)0x921,
                    "Wrong refund SP skill mId:%s char_no:%d rSkillIdx:%d rSp:%d rCount:%d",
                    v30,
                    CurCharacNo,
                    a2,
                    need_sp_sfp_for_level,
                    a7);
                }
              }

  group = CSkill::get_group(skill);
  if ( !*(_DWORD *)this )

  if ( (unsigned __int8)CSkill::IsStealable(skill, a5, a6) )
  {
    CurCharacQuestR = CUser::getCurCharacQuestR(*(CUser **)this);
    if ( (unsigned __int8)UserQuest::isClearQuest((UserQuest *)CurCharacQuestR, 0x163u) != 1
      && (unsigned __int8)CUser::is_clear_stealingSkillMission(*(CUser **)this) != 1 )
    {
      return -1;
    }
    v29 = 1;
    a3 -= CSkill::GetStealLevelPenalty(skill);
  }

    if ( !CSkill::can_learn(skill, 70, a5, a6, v27) )
      return -3;

  else if ( !CSkill::can_learn(skill, v33, a5, a6, v27) )
  {
    return -3;
  }

  second_growtype_fitness = CSkill::get_second_growtype_fitness(skill, a5, a6);
  if ( !second_growtype_fitness )

    second_growtype_fitness = CSkill::get_fitness(skill, a5);
  need_sp_sfp_for_level = CSkill::get_need_sp_sfp_for_level(skill, v28, v27);

    StealSPPenalty = CSkill::GetStealSPPenalty(skill);
    need_sp_sfp_for_level = getSkillSpendSPOnSteal(need_sp_sfp_for_level, StealSPPenalty);

  if ( CSkill::IsSpecialSkill(skill) )
    temp_remain_sfp = SkillSlot::get_temp_remain_sfp(this);

    is_active_skill = CSkill::is_active_skill(skill);
    return SkillSlot::insert_skill(this, (SkillSlot *)((char *)this + 886), a2, group, is_active_skill, a7);

  group = CSkill::get_group(skill);
  if ( group == 4 )

            need_sp_sfp_for_level = CSkill::get_need_sp_sfp_for_level(
                                      skill,

            if ( (unsigned __int8)CSkill::IsStealable(skill, a5, a6) )
            {
              StealSPPenalty = CSkill::GetStealSPPenalty(skill);
              need_sp_sfp_for_level = getSkillSpendSPOnSteal(need_sp_sfp_for_level, StealSPPenalty);
            }

            if ( !CSkill::IsSpecialSkill(skill) )
              temp_remain_sp = SkillSlot::get_temp_remain_sp(this);

  group = CSkill::get_group(skill);
  skillslot_no = SkillSlot::get_skillslot_no(a1, a2, group, a4, 0);

int __cdecl CSkill::GetStealLevelPenalty(CSkill *this)
{
  return *((_DWORD *)this + 71);
}

int __cdecl CSkill::GetStealSPPenalty(CSkill *this)
{
  return *((_DWORD *)this + 70);
}

bool __cdecl CSkill::is_active_skill(CSkill *this)
{
  return *((_BYTE *)this + 32) == 0;
}

int __cdecl CSkill::get_fitness(CSkill *this, int id)
{
  return *((_DWORD *)this + a2 + 9);
}

char *__cdecl CSkill::getSkillName(CSkill *this)
{
  return (char *)this + 305;
}

        group = CSkill::get_group(skill);
        skillslot_no = 0;

        v84 = CSkill::get_group(v83);
        v85 = 0;

    second_growtype_max_level = CSkill::get_second_growtype_max_level(
                                  skill,

      second_growtype_max_level = CSkill::get_type_max_level(skill, v11);
    }
    v12 = std::min<int>(&second_growtype_max_level, &a3);
    v20 = 0;
    a3 = *std::max<int>(&v20, v12);
  }
  else
  {
    v7 = CUserCharacInfo::getCurCharFirstGrowType(this);
    type_max_level = CSkill::get_type_max_level(skill, v7);
    v8 = std::min<int>(&type_max_level, &a3);
    v19 = 0;
    a3 = *std::max<int>(&v19, v8);
  }
  CurCharacSkillTreeIndex = CUserCharacInfo::GetCurCharacSkillTreeIndex(this);
  v14 = a3;
  v15 = (char *)CUserCharacInfo::get_charac_job(this);
  CurCharacSkillW = (CUserCharacInfo **)CUserCharacInfo::getCurCharacSkillW(this);
  SkillSlot::growtype_skill(CurCharacSkillW, v15, a2, v14, CurCharacSkillTreeIndex);
  return 1;
}

//----- (0867B6D4) --------------------------------------------------------
int __cdecl CUser::AddItem(CUser *user, unsigned int item_id, int item_id2, int item_id3, _DWORD *a5, int item_id4)
{
  CItemList **v7; // eax
  int *CurCharacInvenW; // eax
  int ExpirationDate; // esi
  int UsablePeriod; // ebx
  CInventory *inventory; // eax
  char *CreatureMgrW; // eax
  __int64 v13; // [esp+5Bh] [ebp-5Dh] BYREF
  int item_id; // [esp+63h] [ebp-55h]
  int item_id; // [esp+67h] [ebp-51h]
  int item_id; // [esp+6Bh] [ebp-4Dh]
  int item_id; // [esp+6Fh] [ebp-49h]
  int item_id; // [esp+73h] [ebp-45h]
  int item_id; // [esp+77h] [ebp-41h]
  int item_id; // [esp+7Bh] [ebp-3Dh]
  int item_id; // [esp+7Fh] [ebp-39h]
  int item_id; // [esp+83h] [ebp-35h]
  int item_id; // [esp+87h] [ebp-31h]
  int item_id; // [esp+8Bh] [ebp-2Dh]
  int item_id; // [esp+8Fh] [ebp-29h]
  int item_id; // [esp+93h] [ebp-25h]
  char quality; // [esp+97h] [ebp-21h]
  CItem *item; // [esp+98h] [ebp-20h]
  int inserted; // [esp+9Ch] [ebp-1Ch]

  if ( !CUserCharacInfo::getCurCharacR(a1) )
    return -1;
  if ( (unsigned __int8)CUser::CheckInTrade(a1) )
    return -1;
  v7 = (CItemList **)G_CDataManager();
  item = (CItem *)CDataManager::find_item(v7, a2);
  if ( !item )
    return -1;
  if ( a3 < 0 )
    return -1;
  Inven_Item::Inven_Item((Inven_Item *)&v13);
  *(_DWORD *)((char *)&v13 + 2) = a2;
  (*(void (__cdecl **)(CItem *, __int64 *))(*(_DWORD *)item + 8))(item, &v13);
  if ( (unsigned __int8)CItem::is_stackable(item) )
    Inven_Item::set_add_info((Inven_Item *)&v13, a3);
  *a5 = 0;
  if ( Inven_Item::IsCreatureItemType((Inven_Item *)&v13) )
    *a5 = 7;
  if ( a6 > 0 && a6 <= 31 && BYTE1(v13) == 1 )
    Inven_Item::SetUpgrade((Inven_Item *)&v13, a6);
  CurCharacInvenW = (int *)CUserCharacInfo::getCurCharacInvenW(a1);
  inserted = CInventory::insertItemIntoInventory(
               CurCharacInvenW,
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
               v27,
               a4,
               1);
  if ( inserted < 0 )
    return -1;
  if ( Inven_Item::IsCreatureItemType((Inven_Item *)&v13) )
  {
    ExpirationDate = CItem::getExpirationDate(item);
    UsablePeriod = CItem::getUsablePeriod(item);
    v11 = (CInventory *)CUserCharacInfo::getCurCharacInvenW(a1);
    CreatureMgrW = CInventory::GetCreatureMgrW(v11);
    user_creature::CCreatureMgr::InsertCreatureItem(
      (Stream **)CreatureMgrW,
      (Inven_Item *)&v13,
      inserted,
      a4,
      UsablePeriod,
      ExpirationDate);
  }
  return inserted;
}

//----- (0867B8FE) --------------------------------------------------------
int __cdecl CUser::SendPacket(CUser *user, int flags, PacketGuard *a3)
{
  GameWorld *v3; // eax
  CUser **Party; // [esp+18h] [ebp-10h]
  CUser **WarRoom; // [esp+1Ch] [ebp-Ch]

  if ( a2 == 1 )
    goto LABEL_15;
  if ( a2 > 1 )
  {
    if ( a2 == 2 )
    {

