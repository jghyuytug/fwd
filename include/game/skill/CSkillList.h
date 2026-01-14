#ifndef CSKILLLIST_H
#define CSKILLLIST_H

#include <common/defs.h>

// CSkillList declarations
void __cdecl CSkillList::CSkillList(CSkillList *this);
bool __cdecl CSkillList::destroy(CSkillList *this);
_DWORD *__cdecl CSkillList::insert_skill(CSkillList *this, int id, CSkill *a3);
int CSkillList::find_skill(CSkillList *this, int id, ...);
int __cdecl CSkillList::get_ran_groupskill_index(CSkillList *this, int id, int value, int count);
void __cdecl CSkillList::set_post_learning_skill(CSkillList *this, int id, CSkill *a3);
int __cdecl CSkillList::get_post_learning_skill(int id, int value, int count, int *a4);
char *__cdecl CSkillList::getSkillList(CSkillList *this, int id);

#endif // CSKILLLIST_H
