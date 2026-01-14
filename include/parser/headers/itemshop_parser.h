/**
 * @file itemshop_parser.h
 * @brief Item Shop Script Parser for DNF Game Server
 *
 * Parses NPC shop configuration files that define:
 * - Shop type (etc/weapon/disjoint)
 * - Items available for purchase
 * - Limited-availability items
 * - Daily limited items with reset times
 * - Shop UI layout (tab names)
 *
 * Extracted from: df_game_r_part22.c:72176-72314 (138 lines)
 * Related: df_game_r_part14.c:40653-40678 (clear/constructor)
 *          df_game_r_part12.c:38802-38809 (destructor)
 *
 * @date 2025-01-XX
 */

#ifndef ITEMSHOP_PARSER_H
#define ITEMSHOP_PARSER_H

#include <string>
#include <vector>
#include <ctime>

/**
 * @enum ShopType
 * @brief Types of NPC shops
 */
enum class ShopType {
    ETC_SHOP = 0,      ///< Miscellaneous items shop
    WEAPON_SHOP = 1,   ///< Weapons and equipment shop
    DISJOINT_SHOP = 2  ///< Special/separated shop type
};

/**
 * @struct ItemShopScript
 * @brief Data structure for NPC shop configuration
 *
 * Layout (76+ bytes):
 * +0  scriptIndex         - File index in global script list
 * +4  shopType            - Shop type enumeration (0-2)
 * +8  npcId               - Associated NPC ID
 * +12 onlyBuy             - True if shop only allows purchases (no selling)
 * +16 message             - Shop greeting/message string
 * +20 sellItems           - Vector of item IDs available for purchase
 * +32 tabNames            - Vector of UI tab names
 * +44 sellLimitItems      - Vector of limited-availability item IDs
 * +56 oneADayItems        - Nested vector of daily limited item groups
 * +68 hasOneADayItems     - Flag indicating daily items exist
 * +72 oneADayStartTime    - Unix timestamp for daily reset time
 */
struct ItemShopScript {
    int scriptIndex;                        ///< Script file index from global list
    ShopType shopType;                      ///< Type of shop (etc/weapon/disjoint)
    int npcId;                              ///< NPC ID associated with this shop
    bool onlyBuy;                           ///< True if buy-only (no selling items)
    std::string message;                    ///< Shop greeting message
    std::vector<int> sellItems;            ///< List of item IDs for sale
    std::vector<std::string> tabNames;     ///< Tab names for shop UI
    std::vector<int> sellLimitItems;       ///< Limited-availability items
    std::vector<std::vector<int>> oneADayItems; ///< Daily limited items (grouped)
    bool hasOneADayItems;                  ///< Flag: has daily limited items
    std::time_t oneADayStartTime;          ///< Daily reset timestamp (Unix time)

    /**
     * @brief Default constructor
     */
    ItemShopScript()
        : scriptIndex(0)
        , shopType(ShopType::ETC_SHOP)
        , npcId(0)
        , onlyBuy(false)
        , hasOneADayItems(false)
        , oneADayStartTime(0) {
    }

    /**
     * @brief Reset all fields to default values
     */
    void clear() {
        scriptIndex = 0;
        shopType = ShopType::ETC_SHOP;
        npcId = 0;
        onlyBuy = false;
        message.clear();
        sellItems.clear();
        tabNames.clear();
        sellLimitItems.clear();
        oneADayItems.clear();
        hasOneADayItems = false;
        oneADayStartTime = 0;
    }
};

/**
 * @brief Parse item shop script file and populate ItemShopScript structure
 *
 * Recognizes 10 section types:
 * - [NPC]                  : NPC ID (single integer)
 * - [message]              : Shop message (string)
 * - [type]                 : Shop type ([etc shop]/[weapon shop]/[disjoint shop])
 * - [only buy]             : Buy-only flag (0 or 1)
 * - [sell item]            : List of item IDs for sale
 * - [sell limit item]      : List of limited-availability items
 * - [tab name]             : List of UI tab names (strings)
 * - [one a day start time] : Daily reset time (YYYY MM DD HH format)
 * - [one a day item]       : Daily limited item groups (nested lists)
 *
 * Example script file format:
 * @code
 * [NPC]
 *   100234
 *
 * [type]
 *   [weapon shop]
 *
 * [message]
 *   `Welcome to my weapon shop!`
 *
 * [only buy]
 *   0
 *
 * [sell item]
 *   10001
 *   10002
 *   10003
 *
 * [tab name]
 *   `Swords`
 *   `Axes`
 *   `Spears`
 *
 * [one a day start time]
 *   2025 1 15 0
 *
 * [one a day item]
 *   20001
 *   20002
 *
 * [one a day item]
 *   20003
 * @endcode
 *
 * @param[out] shopScript Pointer to ItemShopScript to populate
 * @param[in]  filename   Script filename (relative to g_itemShopScriptBaseDirectory)
 * @return 1 on success, 0 on failure (file not found or parse error)
 *
 * @note Uses global variables:
 *       - g_itemShopScriptBaseDirectory : Base directory for shop scripts
 *       - g_itemShopScriptFileList      : Global registry of all shop scripts
 *
 * @see loadRDARScriptFile() in lexer/rdar_loader.h
 * @see ScanType(), ScanInt(), ScanStr() in parser/scan_utils.h
 */
int importItemShopScript(ItemShopScript* shopScript, const char* filename);

#endif // ITEMSHOP_PARSER_H
