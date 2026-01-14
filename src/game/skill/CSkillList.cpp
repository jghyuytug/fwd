#include <skill/CSkillList.h>
#include <game/skill/CSkillList.h>

void __cdecl CSkillList::CSkillList(CSkillList *this)
{
  CSkillList *v1; // edi
  int i; // esi
  _DWORD *v3; // edi
  int j; // ebx
  _DWORD *v5; // edi
  int k; // ebx

  v1 = this;
  for ( i = 10; i != -1; --i )
  {
    __gnu_cxx::hash_map<int,CSkill *,__gnu_cxx::hash<int>,std::equal_to<int>,std::allocator<CSkill *>>::hash_map((int)v1);
    v1 = (CSkillList *)((char *)v1 + 20);
  }
  v3 = (_DWORD *)((char *)this + 220);
  for ( j = 10; j != -1; --j )
  {
    std::multimap<int,CSkill *>::multimap(v3);
    v3 += 6;
  }
  v5 = (_DWORD *)((char *)this + 484);
  for ( k = 10; k != -1; --k )
  {
    std::map<int,std::vector<std::pair<int,int>>>::map(v5);
    v5 += 6;
  }
}

  CSkillList::destroy(this);
  if ( this != (CSkillList *)-484 )

bool __cdecl CSkillList::destroy(CSkillList *this)
{
  CSkill *v1; // eax
  CSkill *v2; // ebx
  bool result; // al
  _DWORD value[2]; // [esp+10h] [ebp-28h] BYREF
  _DWORD value[2]; // [esp+18h] [ebp-20h] BYREF
  int i; // [esp+20h] [ebp-18h]
  int id; // [esp+24h] [ebp-14h]
  CSkill *v8; // [esp+28h] [ebp-10h]
  int id; // [esp+2Ch] [ebp-Ch]

  for ( i = 0; ; ++i )
  {
    result = i <= 10;
    if ( i > 10 )
      break;
    v7 = (int)this + 20 * i;
    __gnu_cxx::hash_map<int,CSkill *,__gnu_cxx::hash<int>,std::equal_to<int>,std::allocator<CSkill *>>::begin(v5, v7);
    __gnu_cxx::hash_map<int,CSkill *,__gnu_cxx::hash<int>,std::equal_to<int>,std::allocator<CSkill *>>::end(v4, v7);
    while ( __gnu_cxx::_Hashtable_iterator<std::pair<int const,CSkill *>,int,__gnu_cxx::hash<int>,std::_Select1st<std::pair<int const,CSkill *>>,std::equal_to<int>,std::allocator<CSkill *>>::operator!=(
              v5,
              v4) )
    {
      v1 = *(CSkill **)(__gnu_cxx::_Hashtable_iterator<std::pair<int const,CSkill *>,int,__gnu_cxx::hash<int>,std::_Select1st<std::pair<int const,CSkill *>>,std::equal_to<int>,std::allocator<CSkill *>>::operator*(v5)
                      + 4);
      v8 = v1;
      v2 = v1;
      if ( v1 )
      {
        CSkill::~CSkill(v1);
        operator delete(v2);
      }
      __gnu_cxx::_Hashtable_iterator<std::pair<int const,CSkill *>,int,__gnu_cxx::hash<int>,std::_Select1st<std::pair<int const,CSkill *>>,std::equal_to<int>,std::allocator<CSkill *>>::operator++((int)v5);
    }
    __gnu_cxx::hash_map<int,CSkill *,__gnu_cxx::hash<int>,std::equal_to<int>,std::allocator<CSkill *>>::clear(v7);
    v9 = (int)this + 24 * i + 220;
    std::multimap<int,CSkill *>::clear(v9);
  }
  return result;
}

                                                                                                        CSkillList::CSkillList(v49);
                                                                                                        *((_DWORD *)this + 4) = v49;

                                                                                                        if ( (unsigned __int8)CSkillList::init((int)v49, *((CSkillList **)this + 4)) != 1 )
                                                                                                        {
                                                                                                          cMyTrace::cMyTrace((cMyTrace *)v421, "bool CDataManager::init()", 8013, 9, 1, 1);
                                                                                                          cMyTrace::operator()((int)v421, "\t- Loading Skills Fail", v174);
                                                                                                          v10 = 0;
                                                                                                        }

  return CSkillList::find_skill(*(this + 4), a2, a3);
}

//----- (0835FDC6) --------------------------------------------------------
int __cdecl CDataManager::find_quest(QuestList **this, int id)
{

    SkillList = CSkillList::getSkillList(*((CSkillList **)this + 4), *(int *)v106);
    __gnu_cxx::hash_map<int,CSkill *,__gnu_cxx::hash<int>,std::equal_to<int>,std::allocator<CSkill *>>::begin(

    v3 = CSkillList::getSkillList(*((CSkillList **)this + 4), *(int *)v106);
    __gnu_cxx::hash_map<int,CSkill *,__gnu_cxx::hash<int>,std::equal_to<int>,std::allocator<CSkill *>>::end(

char *__cdecl CSkillList::getSkillList(CSkillList *this, int id)
{
  if ( a2 <= 10 )
    return (char *)this + 20 * a2;
  else
    return 0;
}

              CSkillList::get_post_learning_skill(*((_DWORD *)v19 + 4), charac_job, a2, (int *)v38);
              for ( k = 0; ; ++k )

            CSkillList::get_post_learning_skill(*((_DWORD *)v17 + 4), charac_job, a2, (int *)v21);
            for ( k = 0; ; ++k )

    SkillList = CSkillList::getSkillList(*((CSkillList **)v9 + 4), charac_job);
    if ( SkillList )

