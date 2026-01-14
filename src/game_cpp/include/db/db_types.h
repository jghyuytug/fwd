#ifndef DB_TYPES_H
#define DB_TYPES_H

#include <stdint.h>

#ifndef MAX_CHARAC_NAME
#define MAX_CHARAC_NAME 20
#endif

#ifndef MAX_EQUIP_SLOTS
#define MAX_EQUIP_SLOTS 11
#endif

#ifndef MAX_BAG_ITEMS
#define MAX_BAG_ITEMS 64
#endif

#ifndef MAX_SKILL_SLOTS
#define MAX_SKILL_SLOTS 204
#endif

#ifndef MAX_PARSED_SKILLS
#define MAX_PARSED_SKILLS 64
#endif

#ifndef SKILL_SLOT_BLOB_SIZE
#define SKILL_SLOT_BLOB_SIZE 408
#endif

/* Equipment slot constants */
#ifndef EQUIP_SLOT_WEAPON
#define EQUIP_SLOT_WEAPON    0
#define EQUIP_SLOT_HELMET    1
#define EQUIP_SLOT_SHOULDER  2
#define EQUIP_SLOT_CHEST     3
#define EQUIP_SLOT_BELT      4
#define EQUIP_SLOT_PANTS     5
#define EQUIP_SLOT_SHOES     6
#define EQUIP_SLOT_BRACELET  7
#define EQUIP_SLOT_NECKLACE  8
#define EQUIP_SLOT_RING      9
#define EQUIP_SLOT_SUBWEAPON 10
#endif

typedef struct {
    uint32_t ui_id;
    uint16_t slot;
    uint32_t it_id;
    uint16_t count;
    uint8_t  upgrade;
    uint8_t  seal;
    uint16_t flags;
} DBBagItem;

typedef struct {
    uint32_t charac_no;
    uint32_t money;
    uint32_t coin;
    uint32_t pay_coin;
    uint32_t event_coin;
    uint32_t inventory_capacity;
    DBBagItem items[MAX_BAG_ITEMS];
    uint16_t item_count;
    uint8_t  inventory_loaded;
} DBInventoryInfo;

typedef struct {
    uint32_t charac_no;
    uint32_t remain_sp;
    uint32_t used_sp;
    uint32_t remain_sp_2nd;
    uint16_t skill_ids[MAX_PARSED_SKILLS];
    uint8_t  skill_count;
    uint8_t  skill_loaded;
} DBSkillInfo;

typedef struct {
    uint32_t ui_id;
    uint32_t slot;
    uint32_t it_id;
    uint8_t  stat;
    uint16_t hidden_option;
    uint8_t  endurance;
} DBItemInfo;

typedef struct {
    uint32_t charac_no;
    DBItemInfo equipment[MAX_EQUIP_SLOTS];
    uint8_t  equip_count;
    uint8_t  equip_loaded;
} DBEquipmentInfo;

#ifndef MAX_PETS
#define MAX_PETS 10
#endif

typedef struct {
    uint32_t ui_id;
    uint32_t it_id;
    char     name[13];
    uint32_t exp;
    uint8_t  endurance;
    uint8_t  slot;
    uint8_t  creature_type;
    uint8_t  stat;
} DBPetInfo;

typedef struct {
    uint32_t charac_no;
    char     charac_name[MAX_CHARAC_NAME];
    uint8_t  job;
    uint8_t  grow_type;
    uint8_t  level;
    uint32_t exp;
    uint16_t hp;
    uint16_t max_hp;
    uint16_t max_mp;
    int32_t  village;
    uint32_t gold;
    int16_t  fatigue;
    uint8_t  sex;
    uint8_t  deleted;
    uint32_t guild_id;
    uint32_t dungeon_clear_point;
    uint32_t create_time;
    uint8_t  expert_job;
    int8_t   skill_tree_index;
    uint16_t phy_attack;
    uint16_t phy_defense;
    uint16_t mag_attack;
    uint16_t mag_defense;
    uint16_t move_speed;
    uint16_t attack_speed;
    uint16_t cast_speed;
    uint16_t hit_recovery;
    uint16_t jump;
    int32_t  charac_weight;
    int16_t  max_fatigue;
    uint8_t  guild_right;
    uint8_t  member_flag;
    uint32_t equip_item_ids[MAX_EQUIP_SLOTS];
} DBCharacterInfo;

#ifndef MAX_ACCOUNT_NAME
#define MAX_ACCOUNT_NAME 256
#endif

typedef struct {
    uint32_t uid;
    char     accountname[MAX_ACCOUNT_NAME];
    char     password[256];
    char     vip[256];
} DBAccountInfo;

#endif /* DB_TYPES_H */
