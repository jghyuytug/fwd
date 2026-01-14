#ifndef DB_CHARACTERS_H
#define DB_CHARACTERS_H

#include <stdint.h>
#include "db/db_types.h"

#ifdef __cplusplus
extern "C" {
#endif

int db_get_characters(uint32_t m_id, DBCharacterInfo* chars, int max_count);

/* Account functions */
int db_get_account_by_uid(uint32_t uid, DBAccountInfo* account);

/* Character functions */
int db_get_character(uint32_t charac_no, DBCharacterInfo* ch);
int db_soft_delete_character(uint32_t charac_no, uint32_t m_id);
int db_update_character_hp_mp(uint32_t charac_no, uint16_t hp, uint16_t mp);

/* Equipment functions */
int db_get_equipment(uint32_t charac_no, DBEquipmentInfo* equip);

#ifdef __cplusplus
}
#endif

#endif /* DB_CHARACTERS_H */
