/**
 * @file pet_parser.cpp
 * @brief Implementation of pet script parser
 *
 * Extracted from df_game_r_part23.c:23221-23318
 */

#include "parser/pet_parser.h"
#include "parser/scan_utils.h"
#include "parser/script_loader.h"

// External dependencies
extern bool loadRDARScriptFile(const char* dir, const char* filename);
extern const char g_petScriptBaseDirectory[];
extern struct STScriptFileList g_petScriptFileList;

// Helper function
extern int getDrawLayer(const char* layerName);

int importPetScript(PetScript* petScript, const char* filename) {
    // Step 1: Load script file
    if (!loadRDARScriptFile(g_petScriptBaseDirectory, filename)) {
        return 0;
    }

    // Step 2: Clear existing data
    petScript->clear();

    // Step 3: Store file path
    // petScript->filePath = std::string(g_petScriptBaseDirectory) + "/" + filename;

    // Step 4: Parse sections
    std::string token;
    std::string tempStr;
    int tempInt;
    bool success;

    while (ScanType(&token, true)) {
        if (token == "[name]") {
            ScanStr(&petScript->name);
        }
        else if (token == "[width]") {
            petScript->width = ScanInt();
            petScript->height = ScanInt();
        }
        else if (token == "[floating height]") {
            petScript->floatingHeight = ScanInt();
        }
        else if (token == "[layer]") {
            ScanStr(&tempStr);
            petScript->drawLayer = getDrawLayer(tempStr.c_str());
        }
        else if (token == "[recovery time]") {
            petScript->recoveryTime = ScanInt();
        }
        else if (token == "[direction correct]") {
            petScript->directionCorrect = (ScanInt() != 0);
        }
        else if (token == "[move speed]") {
            for (int i = 0; i < 5; ++i) {
                petScript->moveSpeed[i] = ScanInt();
            }
        }
        else if (token == "[gravity]") {
            petScript->gravity = ScanInt();
        }
        else if (token == "[basic motion]") {
            ScanStr(&petScript->basicMotion);
        }
        else if (token == "[etc motion]") {
            petScript->etcMotion.clear();
            while (ScanStr(&tempStr)) {
                petScript->etcMotion.push_back(tempStr);
            }
        }
        else if (token == "[attack info]") {
            petScript->attackInfo.clear();
            while (ScanStr(&tempStr)) {
                petScript->attackInfo.push_back(tempStr);
            }
        }
        else if (token == "[int data]") {
            petScript->intData.clear();
            while (true) {
                success = false;
                tempInt = ScanInt(&success);
                if (!success) break;
                petScript->intData.push_back(tempInt);
            }
        }
        else if (token == "[string data]") {
            petScript->stringData.clear();
            while (ScanStr(&tempStr)) {
                petScript->stringData.push_back(tempStr);
            }
        }
    }

    // Step 5: Set pet ID from file list index
    // TODO: Implement STScriptFileList::FindIndexByFullScan()
    petScript->petId = 0;

    return 1;  // Success
}
