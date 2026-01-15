/**
 * @file itemshop_parser.cpp
 * @brief Implementation of Item Shop Script Parser
 *
 * Parses NPC shop configuration files with support for:
 * - Multiple shop types (etc/weapon/disjoint)
 * - Regular and limited-availability items
 * - Daily limited items with reset times
 * - Multi-tab shop UI configuration
 *
 * Extracted from: df_game_r_part22.c:72176-72314
 *
 * @date 2025-01-XX
 */

#include "parser/itemshop_parser.h"
#include "parser/scan_utils.h"
#include "lexer/rdar_loader.h"
#include <ctime>
#include <cstring>

// External global variables (defined in main game server)
extern char g_itemShopScriptBaseDirectory[256];

// Stub definitions for compilation (will be provided by main server)
struct STScriptFileList {
    static int FindIndexByFullScan(STScriptFileList*, const char*) { return 0; }
};

extern STScriptFileList g_itemShopScriptFileList;

// Stub function for file loading
inline bool loadRDARScriptFile(const char* dir, const char* filename) {
    // Stub: In production, loads file from RDAR archive
    return true;
}

/**
 * @brief Parse shop type string and convert to enum
 * @param typeStr Type string from script (e.g., "[weapon shop]")
 * @return Corresponding ShopType enum value
 */
static ShopType parseShopType(const std::string& typeStr) {
    if (typeStr == "[etc shop]") {
        return ShopType::ETC_SHOP;
    } else if (typeStr == "[weapon shop]") {
        return ShopType::WEAPON_SHOP;
    } else if (typeStr == "[disjoint shop]") {
        return ShopType::DISJOINT_SHOP;
    }
    return ShopType::ETC_SHOP; // Default
}

/**
 * @brief Parse date/time section and convert to Unix timestamp
 *
 * Reads 4 integers in format: YYYY MM DD HH
 * Converts to Unix timestamp using mktime()
 *
 * @param[out] timestamp Pointer to store resulting Unix timestamp
 * @return true if successfully parsed all 4 components, false otherwise
 */
static bool parseDateTimeStamp(std::time_t* timestamp) {
    bool success = false;

    // Read date/time components
    int year = ScanInt(&success);
    if (!success) return false;

    int month = ScanInt(&success);
    if (!success) return false;

    int day = ScanInt(&success);
    if (!success) return false;

    int hour = ScanInt(&success);
    if (!success) return false;

    // Convert to struct tm format
    struct tm timeInfo;
    std::memset(&timeInfo, 0, sizeof(timeInfo));

    timeInfo.tm_year = year - 1900;  // Years since 1900
    timeInfo.tm_mon = month - 1;     // Months since January (0-11)
    timeInfo.tm_mday = day;          // Day of month (1-31)
    timeInfo.tm_hour = hour;         // Hours since midnight (0-23)
    timeInfo.tm_min = 0;
    timeInfo.tm_sec = 0;
    timeInfo.tm_isdst = -1;          // Let mktime() determine DST

    // Convert to Unix timestamp
    *timestamp = std::mktime(&timeInfo);

    return true;
}

/**
 * @brief Parse list of integers until end of section
 * @param[out] itemList Vector to populate with parsed integers
 */
static void parseItemList(std::vector<int>* itemList) {
    itemList->clear();
    bool success = false;

    while (true) {
        int itemId = ScanInt(&success);
        if (!success) {
            break;  // End of list
        }
        itemList->push_back(itemId);
    }
}

/**
 * @brief Parse list of strings until end of section
 * @param[out] stringList Vector to populate with parsed strings
 */
static void parseStringList(std::vector<std::string>* stringList) {
    stringList->clear();
    std::string tempStr;

    while (true) {
        bool success = ScanStr(&tempStr);
        if (!success) {
            break;  // End of list
        }
        stringList->push_back(tempStr);
    }
}

int importItemShopScript(ItemShopScript* shopScript, const char* filename) {
    // Load script file from RDAR archive
    if (!loadRDARScriptFile(g_itemShopScriptBaseDirectory, filename)) {
        return 0;  // File not found or invalid
    }

    // Reset structure to defaults
    shopScript->clear();

    // Parse script sections
    std::string sectionName;
    std::string tempStr;
    bool success = false;

    while (ScanType(&sectionName, true)) {
        if (sectionName == "[NPC]") {
            // Parse NPC ID
            shopScript->npcId = ScanInt();
        }
        else if (sectionName == "[message]") {
            // Parse shop message string
            ScanStr(&shopScript->message);
        }
        else if (sectionName == "[type]") {
            // Parse shop type
            ScanStr(&tempStr);
            shopScript->shopType = parseShopType(tempStr);
        }
        else if (sectionName == "[only buy]") {
            // Parse buy-only flag (0 or 1)
            int flag = ScanInt();
            shopScript->onlyBuy = (flag != 0);
        }
        else if (sectionName == "[sell item]") {
            // Parse list of item IDs for sale
            parseItemList(&shopScript->sellItems);
        }
        else if (sectionName == "[sell limit item]") {
            // Parse list of limited-availability items
            parseItemList(&shopScript->sellLimitItems);
        }
        else if (sectionName == "[tab name]") {
            // Parse list of tab names for shop UI
            parseStringList(&shopScript->tabNames);
        }
        else if (sectionName == "[one a day start time]") {
            // Parse daily reset time (YYYY MM DD HH format)
            if (!parseDateTimeStamp(&shopScript->oneADayStartTime)) {
                break;  // Parse error
            }
        }
        else if (sectionName == "[one a day item]") {
            // Parse daily limited item group
            std::vector<int> dailyItemGroup;
            parseItemList(&dailyItemGroup);

            if (!dailyItemGroup.empty()) {
                shopScript->oneADayItems.push_back(dailyItemGroup);
            }
        }
        // Unknown sections are silently ignored
    }

    // Store script file index from global registry
    shopScript->scriptIndex = STScriptFileList::FindIndexByFullScan(
        &g_itemShopScriptFileList, filename);

    // Set flag if daily items exist
    if (!shopScript->oneADayItems.empty()) {
        shopScript->hasOneADayItems = true;
    }

    return 1;  // Success
}
