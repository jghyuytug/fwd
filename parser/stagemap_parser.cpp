/**
 * @file stagemap_parser.cpp
 * @brief Implementation of stage map script parser
 *
 * Extracted from df_game_r_part20.c:65667-65841
 * Refactored from Hex-Rays decompiled code
 */

#include "parser/stagemap_parser.h"
#include "parser/scan_utils.h"
#include "parser/script_loader.h"
#include <cstring>

// External dependencies
extern bool loadRDARScriptFile(const char* dir, const char* filename);
extern struct STScriptFileList g_stageMapScriptFileList;
extern const char g_stageMapScriptBaseDirectory[];

// ===== Helper Functions =====

namespace {

/**
 * @brief Parse slot type data (5 integers)
 * @param slot Output slot data
 * @return true on success, false on parse error
 */
bool parseSlotType(stagemap::SlotType* slot) {
    bool success = false;

    // Read 5 integers: slotId, posX, posY, width, height
    slot->slotId = ScanInt(&success);
    if (!success) return false;

    slot->posX = ScanInt(&success);
    if (!success) return false;

    slot->posY = ScanInt(&success);
    if (!success) return false;

    slot->width = ScanInt(&success);
    if (!success) return false;

    slot->height = ScanInt(&success);
    if (!success) return false;

    return true;
}

/**
 * @brief Parse stage data (4 integers)
 * @param stage Output stage data
 * @return true on success, false on parse error
 */
bool parseStageData(stagemap::StageData* stage) {
    bool success = false;

    // Read 4 integers: stageId, difficulty, minLevel, maxLevel
    stage->stageId = ScanInt(&success);
    if (!success) return false;

    stage->difficulty = ScanInt(&success);
    if (!success) return false;

    stage->minLevel = ScanInt(&success);
    if (!success) return false;

    stage->maxLevel = ScanInt(&success);
    if (!success) return false;

    return true;
}

/**
 * @brief Parse district data section
 * @param district Output district data
 * @return true on success, false on parse error
 */
bool parseDistrict(stagemap::DistrictData* district) {
    std::string token;

    while (ScanType(&token, true)) {
        if (token == "[district]") {
            // District ID
            bool success = false;
            district->districtId = ScanInt(&success);
            if (!success) return false;
        }
        else if (token == "[slot image]") {
            // Slot background image
            if (!ScanStr(&district->slotImage)) {
                return false;
            }
        }
        else if (token == "[append image]") {
            // Overlay image + offset
            if (!ScanStr(&district->appendImage)) {
                return false;
            }
            bool success = false;
            district->appendImageOffset = ScanInt(&success);
            if (!success) return false;
        }
        else if (token == "[slot type]") {
            // Parse slot type list
            stagemap::SlotType slot;
            while (parseSlotType(&slot)) {
                // Add to map (only if not duplicate)
                if (district->slotTypes.find(slot.slotId) == district->slotTypes.end()) {
                    district->slotTypes[slot.slotId] = slot;
                }
                slot.clear();
            }
        }
        else if (token == "[stage]") {
            // Parse stage list
            stagemap::StageData stage;
            while (parseStageData(&stage)) {
                district->stages.push_back(stage);
                stage.clear();
            }
        }
        else if (token == "[survival]") {
            // Survival mode flag
            bool success = false;
            district->survivalFlag = ScanInt(&success);
            if (!success) return false;
        }
        else if (token == "[/area]") {
            // End of area section
            return true;
        }
    }

    return true;
}

/**
 * @brief Parse area data section
 * @param area Output area data
 * @return true on success, false on parse error
 */
bool parseArea(stagemap::AreaData* area) {
    // Read area ID
    bool success = false;
    area->areaId = ScanInt(&success);
    if (!success) return false;

    // Parse districts within this area
    stagemap::DistrictData district;
    district.clear();

    if (parseDistrict(&district)) {
        area->districts.push_back(district);
        return true;
    }

    return false;
}

} // anonymous namespace

// ===== Public API =====

int importStageMapScript(stagemap::StageMapScript* mapScript, const char* filename) {
    // Step 1: Load script file
    if (!loadRDARScriptFile(g_stageMapScriptBaseDirectory, filename)) {
        return 0;  // File not found
    }

    // Step 2: Clear existing data
    mapScript->clear();

    // Step 3: Parse sections
    std::string token;
    bool success = false;

    while (ScanType(&token, true)) {
        if (token == "[name]") {
            // Map name
            if (!ScanStr(&mapScript->name)) {
                return 0;
            }
        }
        else if (token == "[map image]") {
            // Map image path + type
            if (!ScanStr(&mapScript->image)) {
                return 0;
            }
            mapScript->mapType = ScanInt(&success);
            if (!success) {
                // Map type is optional, default to 0
                mapScript->mapType = 0;
            }
        }
        else if (token == "[dungeon]") {
            // Dungeon ID
            mapScript->dungeonId = ScanInt(&success);
            if (!success) {
                return 0;
            }
        }
        else if (token == "[area]") {
            // Parse area section
            stagemap::AreaData area;
            area.clear();

            if (parseArea(&area)) {
                mapScript->areas.push_back(area);
            } else {
                return 0;  // Parse error
            }
        }
    }

    // Step 4: Set map ID from file list index
    // TODO: Implement STScriptFileList::GetIndex()
    // For now, use filename hash or default to 0
    mapScript->mapId = 0;

    return 1;  // Success
}
