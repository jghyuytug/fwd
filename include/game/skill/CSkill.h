#ifndef CSKILL_H
#define CSKILL_H

#include <common/defs.h>

// CSkill declarations
_BOOL4 __cdecl CSkill::IsSpecialSkill(CSkill *this);
int __cdecl CSkill::isCommandCustomizing(CSkill *this);
std::_List_node_base *CSkill::add_learned_skill(int id, ...);
std::_List_node_base *CSkill::add_need_sp(CSkill *this, ...);
std::_List_node_base *CSkill::add_need_sfp(CSkill *this, ...);
bool __cdecl CSkill::set_prelearned_skill(CSkill *this);
int __cdecl CSkill::get_guildSkillNeedGSP(CSkill *this, unsigned int id);
int __cdecl CSkill::get_guildSkillNeedGold(CSkill *this, unsigned int id);
int __cdecl CSkill::get_needGuildLevel(CSkill *this, int id);
int __cdecl CSkill::get_need_sfp_for_level(CSkill *this, int id, int value);
int __cdecl CSkill::get_need_sp_sfp_for_level(CSkill *this, int id, int value);
int __cdecl CSkill::get_need_sp_for_level(CSkill *this, int id, int value);
bool __cdecl CSkill::can_learn(CSkill *this, int id, int value, int count, int index);
int __cdecl CSkill::IsStealable(CSkill *this, int id, int value);
CSkill *__cdecl CSkill::set_second_growtype_fitness(CSkill *this, unsigned int id, int value, int count);
int __cdecl CSkill::get_second_growtype_fitness(CSkill *this, unsigned int id, unsigned int value);
CSkill *__cdecl CSkill::set_second_growtype_max_level(CSkill *this, unsigned int id, int value, int count);
int __cdecl CSkill::get_second_growtype_max_level(CSkill *this, unsigned int id, int value);
CSkill *__cdecl CSkill::set_fitness_second_growtype(CSkill *this, int id, int value);
int __cdecl CSkill::get_fitness_second_growtype(CSkill *this, int id);
int __cdecl CSkill::getMaxSkillLevel(CSkill *this, int id, int value, int count);
int __cdecl CSkill::reform_ui_group_no(CSkill *this, int *a2, bool a3, int id);
void __cdecl CSkill::CSkill(CSkill *this);
CSkill *__cdecl CSkill::SetStealable(CSkill *this, bool a2);
CSkill *__cdecl CSkill::SetStealLevelPenalty(CSkill *this, int id);
CSkill *__cdecl CSkill::SetStealSPPenalty(CSkill *this, int id);
CSkill *__cdecl CSkill::set_index(CSkill *this, int id);
int __cdecl CSkill::get_index(CSkill *this);
CSkill *__cdecl CSkill::set_group(CSkill *this, int id);
int __cdecl CSkill::get_group(CSkill *this);
CSkill *__cdecl CSkill::set_job_type(CSkill *this, int id);
CSkill *__cdecl CSkill::set_skill_type(CSkill *this, char flag);
CSkill *__cdecl CSkill::set_level_limit(CSkill *this, int id);
CSkill *__cdecl CSkill::set_fitness(CSkill *this, int id, int value);
CSkill *__cdecl CSkill::set_type_max_level(CSkill *this, int id, int value);
int __cdecl CSkill::get_type_max_level(CSkill *this, int id);
CSkill *__cdecl CSkill::set_fitGrowType(CSkill *this, int id, int value);
char *__cdecl CSkill::setSkillName(CSkill *this, char *src);
int __cdecl CSkill::GetStealLevelPenalty(CSkill *this);
int __cdecl CSkill::GetStealSPPenalty(CSkill *this);
bool __cdecl CSkill::is_active_skill(CSkill *this);
int __cdecl CSkill::get_fitness(CSkill *this, int id);
char *__cdecl CSkill::getSkillName(CSkill *this);

#endif // CSKILL_H
