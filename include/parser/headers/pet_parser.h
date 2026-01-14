/**
 * @file pet_parser.h
 * @brief Pet script parser for DNF game server
 *
 * Parses pet configuration files defining:
 * - Basic properties (name, size, layer)
 * - Movement (speed, gravity, floating height)
 * - Animations (basic motion, etc motions)
 * - Attack information
 * - Custom data (int/string arrays)
 *
 * Extracted from: df_game_r_part23.c:23221-23318
 */

#ifndef PET_PARSER_H
#define PET_PARSER_H

#include <string>
#include <vector>

/**
 * @struct PetScript
 * @brief Complete pet configuration data
 */
struct PetScript {
    int petId;                      ///< Pet ID (from file list index)
    std::string name;               ///< Pet display name
    int width;                      ///< Pet width (pixels)
    int height;                     ///< Pet height (pixels)
    int floatingHeight;             ///< Vertical offset for floating pets
    int drawLayer;                  ///< Render layer (resolved from string)
    int recoveryTime;               ///< Recovery/cooldown time (ms)
    bool directionCorrect;          ///< Auto-correct facing direction

    // Movement properties
    int moveSpeed[5];               ///< Move speeds for different states
    int gravity;                    ///< Gravity value

    // Animation data
    std::string basicMotion;        ///< Basic motion file path
    std::vector<std::string> etcMotion;      ///< Additional motion files
    std::vector<std::string> attackInfo;     ///< Attack animation info

    // Custom data
    std::vector<int> intData;       ///< Integer parameters
    std::vector<std::string> stringData;     ///< String parameters

    std::string filePath;           ///< Source file path

    PetScript()
        : petId(0), width(0), height(0), floatingHeight(0)
        , drawLayer(0), recoveryTime(0), directionCorrect(false)
        , gravity(0)
    {
        for (int i = 0; i < 5; ++i) {
            moveSpeed[i] = 0;
        }
    }

    void clear() {
        petId = 0;
        name.clear();
        width = height = floatingHeight = 0;
        drawLayer = recoveryTime = 0;
        directionCorrect = false;
        for (int i = 0; i < 5; ++i) moveSpeed[i] = 0;
        gravity = 0;
        basicMotion.clear();
        etcMotion.clear();
        attackInfo.clear();
        intData.clear();
        stringData.clear();
        filePath.clear();
    }
};

/**
 * @brief Parse pet script file
 * @param petScript Output structure to populate
 * @param filename Script file name (relative to pet directory)
 * @return 1 on success, 0 on error
 *
 * ## File Format
 *
 * ```
 * [name]
 * "Pet Name"
 *
 * [width]
 * 50 60  // width, height
 *
 * [floating height]
 * 20
 *
 * [layer]
 * "normal"
 *
 * [recovery time]
 * 1000
 *
 * [direction correct]
 * 1
 *
 * [move speed]
 * 100 150 200 250 300  // 5 speeds
 *
 * [gravity]
 * 980
 *
 * [basic motion]
 * "motion/basic.ani"
 *
 * [etc motion]
 * "motion/run.ani"
 * "motion/jump.ani"
 *
 * [attack info]
 * "attack1"
 * "attack2"
 *
 * [int data]
 * 10 20 30
 *
 * [string data]
 * "data1"
 * "data2"
 * ```
 */
int importPetScript(PetScript* petScript, const char* filename);

#endif // PET_PARSER_H
