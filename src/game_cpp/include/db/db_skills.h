#ifndef DB_SKILLS_H
#define DB_SKILLS_H

#include <stdint.h>
#include "db/db_types.h"

#ifdef __cplusplus
extern "C" {
#endif

int db_get_skill(uint32_t charac_no, DBSkillInfo* skill);

#ifdef __cplusplus
}
#endif

#endif /* DB_SKILLS_H */
