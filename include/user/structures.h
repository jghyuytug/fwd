/**
 * User Module Structure Definitions
 * Structures for user management, character data, and account information
 */

#ifndef INCLUDE_USER_STRUCTURES_H_
#define INCLUDE_USER_STRUCTURES_H_

#include <common/ida_types.h>
#include <time.h>

/* Forward declarations */
typedef struct CUserCharacInfo CUserCharacInfo;
typedef struct CInventory CInventory;

/**
 * User Login Session
 */
typedef struct {
    uint32_t user_id;
    uint32_t account_id;
    char username[64];
    char password_hash[128];
    time_t login_time;
    time_t last_activity;
    int is_logged_in;
} UserLoginInfo;

/**
 * Character information
 */
typedef struct CUserCharacInfo {
    uint32_t character_id;
    uint32_t user_id;
    char character_name[64];
    int character_level;
    int character_class;
    uint32_t experience;
    time_t creation_time;
    time_t last_login;
} CUserCharacInfo;

/**
 * Main user object - represents logged in player
 * This is a simplified version; actual structure may be much larger
 */
typedef struct {
    uint32_t user_id;
    uint32_t server_group;
    CUserCharacInfo current_character;
    CInventory* inventory;
    void* character_list;
    uint32_t active_dungeons;
    int fatigue_current;
    int fatigue_max;
    void* log_pointer;
} CUser;

#endif // INCLUDE_USER_STRUCTURES_H_
