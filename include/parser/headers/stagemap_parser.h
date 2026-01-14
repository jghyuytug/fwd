/**
 * @file stagemap_parser.h
 * @brief Stage map script parser for DNF game server
 *
 * Parses stage map configuration files that define:
 * - Map metadata (name, image, dungeon ID)
 * - Area/district hierarchy
 * - Stage slot positions and types
 * - Stage difficulty and level requirements
 *
 * Extracted from: df_game_r_part20.c:65667-65841
 * Original decompiled code from DNF game server
 */

#ifndef STAGEMAP_PARSER_H
#define STAGEMAP_PARSER_H

#include <string>
#include <vector>
#include <map>

/**
 * @namespace stagemap
 * @brief Stage map data structures and parser
 */
namespace stagemap {

/**
 * @struct SlotType
 * @brief Defines a stage slot position and dimensions
 */
struct SlotType {
    int slotId;       ///< Slot identifier
    int posX;         ///< X coordinate
    int posY;         ///< Y coordinate
    int width;        ///< Slot width
    int height;       ///< Slot height

    SlotType() : slotId(0), posX(0), posY(0), width(0), height(0) {}

    void clear() {
        slotId = posX = posY = width = height = 0;
    }
};

/**
 * @struct StageData
 * @brief Stage difficulty and level requirements
 */
struct StageData {
    int stageId;      ///< Stage identifier
    int difficulty;   ///< Difficulty level
    int minLevel;     ///< Minimum player level
    int maxLevel;     ///< Maximum player level

    StageData() : stageId(0), difficulty(0), minLevel(0), maxLevel(0) {}

    void clear() {
        stageId = difficulty = minLevel = maxLevel = 0;
    }
};

/**
 * @struct DistrictData
 * @brief District within an area
 *
 * Contains stage layout information including:
 * - Visual assets (slot image, append image)
 * - Slot type definitions (position/size mapping)
 * - Stage list with difficulty/level data
 * - Survival mode flag
 */
struct DistrictData {
    int districtId;                     ///< District identifier
    std::string slotImage;              ///< Slot background image path
    std::string appendImage;            ///< Additional overlay image path
    int appendImageOffset;              ///< Overlay image offset/type
    std::map<int, SlotType> slotTypes;  ///< Slot ID → SlotType mapping
    std::vector<StageData> stages;      ///< Stage list
    int survivalFlag;                   ///< Survival mode enabled (0/1)

    DistrictData() : districtId(0), appendImageOffset(0), survivalFlag(0) {}

    void clear() {
        districtId = 0;
        slotImage.clear();
        appendImage.clear();
        appendImageOffset = 0;
        slotTypes.clear();
        stages.clear();
        survivalFlag = 0;
    }
};

/**
 * @struct AreaData
 * @brief Area containing multiple districts
 */
struct AreaData {
    int areaId;                          ///< Area identifier
    std::vector<DistrictData> districts; ///< District list

    AreaData() : areaId(0) {}

    void clear() {
        areaId = 0;
        districts.clear();
    }
};

/**
 * @struct StageMapScript
 * @brief Complete stage map configuration
 *
 * Root data structure containing all map metadata and hierarchical layout:
 * Map → Areas → Districts → Stages
 */
struct StageMapScript {
    int mapId;                    ///< Map identifier (from file list index)
    int dungeonId;                ///< Associated dungeon ID
    int mapType;                  ///< Map type/size
    std::string name;             ///< Map display name
    std::string image;            ///< Map overview image path
    std::vector<AreaData> areas;  ///< Area list

    StageMapScript() : mapId(0), dungeonId(0), mapType(0) {}

    void clear() {
        mapId = 0;
        dungeonId = 0;
        mapType = 0;
        name.clear();
        image.clear();
        areas.clear();
    }
};

} // namespace stagemap

/**
 * @brief Parse stage map script file
 * @param mapScript Output structure to populate
 * @param filename Script file name (relative to stage map directory)
 * @return 1 on success, 0 on error
 *
 * ## File Format
 *
 * ```
 * [name]
 * "Map Display Name"
 *
 * [map image]
 * "path/to/image.img"
 * 1  // map type
 *
 * [dungeon]
 * 1234  // dungeon ID
 *
 * [area]
 * 1  // area ID
 *   [district]
 *   10  // district ID
 *   [slot image]
 *   "slot_bg.img"
 *   [append image]
 *   "overlay.img"
 *   0  // offset
 *   [slot type]
 *   1 100 100 50 50  // slotId, x, y, w, h
 *   2 200 100 50 50
 *   [stage]
 *   101 1 1 50    // stageId, difficulty, minLvl, maxLvl
 *   102 2 20 60
 *   [survival]
 *   0  // not survival mode
 * [/area]
 * ```
 *
 * ## Usage Example
 *
 * ```cpp
 * stagemap::StageMapScript mapConfig;
 * if (importStageMapScript(&mapConfig, "stage_01.txt")) {
 *     printf("Loaded map: %s\n", mapConfig.name.c_str());
 *     printf("Areas: %zu\n", mapConfig.areas.size());
 * }
 * ```
 */
int importStageMapScript(stagemap::StageMapScript* mapScript, const char* filename);

#endif // STAGEMAP_PARSER_H
