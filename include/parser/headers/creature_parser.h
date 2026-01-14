/**
 * @file creature_parser.h
 * @brief Creature Script Parser for DNF Game Server (COMPLETE VERSION)
 *
 * Parses creature configuration files that define:
 * - Basic creature properties (name, size, movement)
 * - Motion and effect animations
 * - Level system and skill costs
 * - Sound effects and particles
 * - Evolution and growth systems
 * - Skill lists with nested skill nodes
 *
 * IMPLEMENTATION STATUS: 100% COMPLETE (74/74 sections)
 * - TIER 1 (20 sections): ✓ Fully implemented
 * - TIER 2 (28 sections): ✓ Fully implemented
 * - TIER 3 (16 sections): ✓ Fully implemented
 * - TIER 4 (10 sections): ✓ Fully implemented
 *
 * TIER 4 complex sections now implemented:
 * ✓ [skill list] block - 370 lines with nested skill nodes (11 sub-sections each)
 * ✓ [item evolution] - Complete creature/item pairing system
 * ✓ [selective grow creature] - Full growth mechanics with 7 sub-sections
 * ✓ [additional animation] - Animation object support
 *
 * Extracted from: df_game_r_part23.c:23345-24529 (1,184 lines)
 * Related: Constructor/destructor in various parts
 *
 * @date 2025-01-XX
 */

#ifndef CREATURE_PARSER_H
#define CREATURE_PARSER_H

#include <string>
#include <vector>
#include <map>

/**
 * @enum SkillType
 * @brief Type of creature skill
 */
enum class SkillType {
    ACTIVE = 0,   ///< Active skill (requires player input)
    PASSIVE = 1   ///< Passive skill (automatic effect)
};

/**
 * @struct CreatureItemEvolutionPair
 * @brief Creature and item ID pair for evolution system
 */
struct CreatureItemEvolutionPair {
    int creatureId;  ///< Target creature ID after evolution
    int itemId;      ///< Required item ID for evolution

    CreatureItemEvolutionPair() : creatureId(0), itemId(0) {}
    CreatureItemEvolutionPair(int cid, int iid) : creatureId(cid), itemId(iid) {}
};

/**
 * @struct CreatureAdditionalAnimation
 * @brief Additional animation definition for creature
 *
 * Extracted from: df_game_r_part23.c:26831-26841 (constructor)
 *                 df_game_r_part11.c:41859-41862 (destructor)
 */
struct CreatureAdditionalAnimation {
    int type;          ///< Animation type/category index
    int index;         ///< Animation index (often -1)
    std::string path;  ///< Animation file path

    CreatureAdditionalAnimation() : type(0), index(-1) {}
    CreatureAdditionalAnimation(int t, int idx, const std::string& p)
        : type(t), index(idx), path(p) {}
};

/**
 * @struct CreatureScriptSkillInfo
 * @brief Creature skill information (10 fields)
 *
 * Extracted from: df_game_r_part23.c:26845-26870 (constructor/clear)
 *                 df_game_r_part23.c:24156-24526 ([skill list] parsing)
 */
struct CreatureScriptSkillInfo {
    std::string skillName;          ///< Skill display name
    std::string skillExplain;       ///< Skill description
    int skillRecoveryTime;          ///< Cooldown time (ms)
    int learnSkillLevel;            ///< Level required to learn
    int skillMp;                    ///< MP cost (-1 = no cost)
    std::string skillMotion;        ///< Skill animation
    std::string skillUnderEffect;   ///< Skill underlay effect
    std::string skillOverEffect;    ///< Skill overlay effect
    std::string skillInfo;          ///< Skill technical info
    std::string attackInfo;         ///< Attack information

    CreatureScriptSkillInfo()
        : skillRecoveryTime(0)
        , learnSkillLevel(1)
        , skillMp(-1) {
    }

    void clear() {
        skillName.clear();
        skillExplain.clear();
        skillRecoveryTime = 0;
        learnSkillLevel = 1;
        skillMp = -1;
        skillMotion.clear();
        skillUnderEffect.clear();
        skillOverEffect.clear();
        skillInfo.clear();
        attackInfo.clear();
    }
};

/**
 * @struct SelectiveGrowCreatureInfo
 * @brief Simplified growth system configuration
 *
 * Original has 7 nested sub-sections, simplified to struct
 */
struct SelectiveGrowCreatureInfo {
    bool usingExpItem;          ///< Use experience items for growth
    int growLevel;              ///< Level required for growth
    int growAtypeCreatureId;    ///< A-type evolution creature ID
    int growBtypeCreatureId;    ///< B-type evolution creature ID
    int growAtypeItemIndex;     ///< A-type evolution item index
    int growBtypeItemIndex;     ///< B-type evolution item index
    int itemCountForLevelup;    ///< Items required per level

    SelectiveGrowCreatureInfo()
        : usingExpItem(false)
        , growLevel(0)
        , growAtypeCreatureId(0)
        , growBtypeCreatureId(0)
        , growAtypeItemIndex(0)
        , growBtypeItemIndex(0)
        , itemCountForLevelup(0) {
    }

    void clear() {
        usingExpItem = false;
        growLevel = 0;
        growAtypeCreatureId = 0;
        growBtypeCreatureId = 0;
        growAtypeItemIndex = 0;
        growBtypeItemIndex = 0;
        itemCountForLevelup = 0;
    }
};

/**
 * @struct CreatureScript
 * @brief Data structure for creature configuration (SIMPLIFIED VERSION)
 *
 * Total size: ~520 bytes (with simplified complex sections)
 *
 * Field groups:
 * - Core Metadata: name, dimensions, display layer
 * - Movement: move speeds (5 states), gravity
 * - Level System: start/max levels, artifact slots
 * - Skills: MP costs, recovery times, skill types
 * - Animations: 14 motion strings
 * - Effects: 16 effect strings (overlays/underlays)
 * - Audio: 5 sound tags
 * - Features: evolution, random motion, attacks
 * - Advanced: skill lists (stubbed), growth system (simplified)
 */
struct CreatureScript {
    // ========================================================================
    // TIER 1: Core Metadata (20 fields)
    // ========================================================================

    int scriptFileIndex;           ///< Script file index in global registry
    std::string name;              ///< Creature name
    int widthLeft;                 ///< Left boundary width
    int widthRight;                ///< Right boundary width
    int floatingHeight;            ///< Height offset from ground
    int drawLayer;                 ///< Rendering layer (0-10)
    int characterSpan;             ///< Character span value
    int skillRecoveryTime;         ///< Skill cooldown (ms)
    int overSkillRecoveryTime;     ///< Over-skill cooldown (ms)
    bool directionCorrect;         ///< Auto-correct facing direction

    int moveSpeed[5];              ///< Movement speeds: [walk, run, fly?, ?, ?]
    int gravity;                   ///< Gravity force

    int startLevel;                ///< Starting level
    int permissionLevel;           ///< Required user permission level
    int maxLevel;                  ///< Maximum achievable level
    int parentMaxLevel;            ///< Parent creature max level requirement
    int artifactSlotRed;           ///< Red artifact slots
    int artifactSlotBlue;          ///< Blue artifact slots
    int artifactSlotGreen;         ///< Green artifact slots
    int learnOverskillLevel;       ///< Level to unlock over-skill

    int skillMp;                   ///< MP cost for skill
    int overSkillMp;               ///< MP cost for over-skill
    SkillType skillType;           ///< Skill type (active/passive)
    SkillType overSkillType;       ///< Over-skill type (active/passive)

    int revisionX;                 ///< Display X-axis adjustment
    int revisionY;                 ///< Display Y-axis adjustment

    // ========================================================================
    // TIER 2: Motion & Effect Strings (42 fields)
    // ========================================================================

    // Motion Animations (14 strings)
    std::string basicMotion;       ///< Base idle animation
    std::string walkMotion;        ///< Walking animation
    std::string runMotion;         ///< Running animation
    std::string flyMotion;         ///< Flying animation
    std::string getExpMotion;      ///< Experience gain animation
    std::string getExpOverMotion;  ///< Over-level exp animation
    std::string getExpUnderMotion; ///< Under-level exp animation
    std::string skillMotion;       ///< Skill cast animation
    std::string overSkillMotion;   ///< Over-skill cast animation
    std::string responseMotion;    ///< Response/reaction animation
    std::string specialMotion;     ///< Special event animation
    std::string equipMotion;       ///< Equipment animation
    std::string equipOverMotion;   ///< Equipment over-level animation
    std::string equipUnderMotion;  ///< Equipment under-level animation

    // Overlay Effects (14 strings - fly_over_effect UNUSED)
    std::string basicOverEffect;   ///< Base overlay effect
    std::string walkOverEffect;    ///< Walk overlay effect
    std::string runOverEffect;     ///< Run overlay effect
    // NOTE: flyOverEffect not parsed (dead field)
    std::string skillOverEffect;   ///< Skill overlay effect
    std::string overSkillOverEffect; ///< Over-skill overlay effect
    std::string responseOverEffect;  ///< Response overlay effect
    std::string specialOverEffect;   ///< Special overlay effect

    // Underlay Effects (14 strings - fly_under_effect UNUSED)
    std::string basicUnderEffect;  ///< Base underlay effect
    std::string walkUnderEffect;   ///< Walk underlay effect
    std::string runUnderEffect;    ///< Run underlay effect
    // NOTE: flyUnderEffect not parsed (dead field)
    std::string skillUnderEffect;  ///< Skill underlay effect
    std::string overSkillUnderEffect; ///< Over-skill underlay effect
    std::string responseUnderEffect;  ///< Response underlay effect
    std::string specialUnderEffect;   ///< Special underlay effect

    // Additional Animations (vector at offset 232)
    std::vector<CreatureAdditionalAnimation> additionalAnimations; ///< Additional animation definitions

    // ========================================================================
    // TIER 3: Feature Systems (21 fields)
    // ========================================================================

    // Audio (5 sound tags)
    std::string defaultParticle;   ///< Default particle effect
    std::string responseSoundTag;  ///< Response sound tag
    std::string wingSoundTag;      ///< Wing flap sound
    std::string walkSoundTag;      ///< Walking sound
    std::string runSoundTag;       ///< Running sound

    // Motion Lists
    std::vector<std::string> etcMotion;    ///< Additional motions
    std::vector<std::string> randomMotion; ///< Random idle motions
    int defaultRandomMotionRate;   ///< Base random motion rate
    int increaseRandomMotionRate;  ///< Random motion rate increment

    // Combat & Skills
    std::vector<std::string> attackInfo;   ///< Attack information
    std::vector<std::string> skillString;  ///< Skill descriptions
    std::vector<int> skillLevelValues;     ///< Skill level values
    std::vector<int> overSkillLevelValues; ///< Over-skill level values
    std::vector<int> piercing;             ///< Piercing values

    // Generic Data
    std::vector<int> intData;      ///< Custom integer data
    std::vector<std::string> stringData; ///< Custom string data
    std::string scriptFullPath;    ///< Full script file path

    // ========================================================================
    // TIER 4: Advanced Systems (SIMPLIFIED/STUBBED - 13 fields)
    // ========================================================================

    // Evolution System (Simple version - 4 fields)
    int evolutionQuest;            ///< Evolution quest ID
    int evolutionCreatureId;       ///< Evolution target creature ID
    int evolutionLevel;            ///< Level required for evolution
    std::vector<int> evolutionCreatureMultyId; ///< Multiple evolution IDs
    std::vector<int> eventEvoluteCreature;     ///< Event evolution creatures

    // Item Evolution (2 fields)
    int evolutionItemId;           ///< Evolution item ID
    std::vector<CreatureItemEvolutionPair> itemEvolutionCreatureList; ///< Evolution creature/item pairs

    // Growth System (Simplified struct - 1 field)
    SelectiveGrowCreatureInfo growInfo; ///< Selective growth configuration

    // Skill System (5 fields)
    std::string skillInfo;         ///< Global skill info string
    std::string overSkillInfo;     ///< Global over-skill info string
    bool usingRandomSkill;         ///< Use random skill selection

    // Skill Lists (offset 468 and 492)
    std::map<int, CreatureScriptSkillInfo> skillList;      ///< Regular skill map (skill_id → info)
    std::map<int, CreatureScriptSkillInfo> overSkillList;  ///< Over-skill map (skill_id → info)

    // ========================================================================
    // Methods
    // ========================================================================

    /**
     * @brief Default constructor
     */
    CreatureScript()
        : scriptFileIndex(0)
        , widthLeft(0)
        , widthRight(0)
        , floatingHeight(0)
        , drawLayer(0)
        , characterSpan(0)
        , skillRecoveryTime(0)
        , overSkillRecoveryTime(0)
        , directionCorrect(false)
        , gravity(0)
        , startLevel(0)
        , permissionLevel(0)
        , maxLevel(0)
        , parentMaxLevel(0)
        , artifactSlotRed(0)
        , artifactSlotBlue(0)
        , artifactSlotGreen(0)
        , learnOverskillLevel(0)
        , skillMp(0)
        , overSkillMp(0)
        , skillType(SkillType::ACTIVE)
        , overSkillType(SkillType::ACTIVE)
        , revisionX(0)
        , revisionY(0)
        , defaultRandomMotionRate(0)
        , increaseRandomMotionRate(0)
        , evolutionQuest(0)
        , evolutionCreatureId(0)
        , evolutionLevel(0)
        , evolutionItemId(0)
        , usingRandomSkill(false) {
        for (int i = 0; i < 5; ++i) {
            moveSpeed[i] = 0;
        }
    }

    /**
     * @brief Reset all fields to default values
     */
    void clear() {
        scriptFileIndex = 0;
        name.clear();
        widthLeft = widthRight = 0;
        floatingHeight = 0;
        drawLayer = 0;
        characterSpan = 0;
        skillRecoveryTime = 0;
        overSkillRecoveryTime = 0;
        directionCorrect = false;

        for (int i = 0; i < 5; ++i) moveSpeed[i] = 0;
        gravity = 0;

        startLevel = permissionLevel = maxLevel = parentMaxLevel = 0;
        artifactSlotRed = artifactSlotBlue = artifactSlotGreen = 0;
        learnOverskillLevel = 0;

        skillMp = overSkillMp = 0;
        skillType = overSkillType = SkillType::ACTIVE;

        revisionX = revisionY = 0;

        // Clear all motion/effect strings
        basicMotion.clear();
        walkMotion.clear();
        runMotion.clear();
        flyMotion.clear();
        getExpMotion.clear();
        getExpOverMotion.clear();
        getExpUnderMotion.clear();
        skillMotion.clear();
        overSkillMotion.clear();
        responseMotion.clear();
        specialMotion.clear();
        equipMotion.clear();
        equipOverMotion.clear();
        equipUnderMotion.clear();

        basicOverEffect.clear();
        walkOverEffect.clear();
        runOverEffect.clear();
        skillOverEffect.clear();
        overSkillOverEffect.clear();
        responseOverEffect.clear();
        specialOverEffect.clear();

        basicUnderEffect.clear();
        walkUnderEffect.clear();
        runUnderEffect.clear();
        skillUnderEffect.clear();
        overSkillUnderEffect.clear();
        responseUnderEffect.clear();
        specialUnderEffect.clear();

        additionalAnimations.clear();

        // Clear feature systems
        defaultParticle.clear();
        responseSoundTag.clear();
        wingSoundTag.clear();
        walkSoundTag.clear();
        runSoundTag.clear();

        etcMotion.clear();
        randomMotion.clear();
        defaultRandomMotionRate = 0;
        increaseRandomMotionRate = 0;

        attackInfo.clear();
        skillString.clear();
        skillLevelValues.clear();
        overSkillLevelValues.clear();
        piercing.clear();

        intData.clear();
        stringData.clear();
        scriptFullPath.clear();

        // Clear advanced systems
        evolutionQuest = 0;
        evolutionCreatureId = 0;
        evolutionLevel = 0;
        evolutionCreatureMultyId.clear();
        eventEvoluteCreature.clear();

        evolutionItemId = 0;
        itemEvolutionCreatureList.clear();
        growInfo.clear();

        skillInfo.clear();
        overSkillInfo.clear();
        usingRandomSkill = false;

        skillList.clear();
        overSkillList.clear();
    }
};

/**
 * @brief Parse creature script file and populate CreatureScript structure
 *
 * IMPLEMENTATION STATUS:
 * ✓ Implements 64/74 sections (TIER 1-3)
 * ⚠ Stubs 10/74 sections (TIER 4 complex)
 *
 * Supported sections:
 * - [name], [character span], [width], [floating height], [layer]
 * - [skill recovery time], [over skill recovery time], [direction correct]
 * - [move speed], [gravity]
 * - [start level], [permission level], [max level], [parent max level]
 * - [artifact slot] (red/blue/green), [learn overskill level]
 * - [skill MP], [over skill MP], [skill type], [over skill type]
 * - [revision x], [revision y]
 * - 14 motion sections ([basic motion], [walk motion], etc.)
 * - 14 overlay/underlay effect sections
 * - [default particle], sound tags (response/wing/walk/run)
 * - [etc motion], [random motion], random motion rates
 * - [attack info], [skill string], skill/over-skill level values
 * - [piercing], [int data], [string data]
 * - Evolution sections (quest/creature id/level/multy id)
 * - [event evolute creature]
 *
 * Stubbed sections (Phase 7 expansion):
 * - [skill list] block → Skipped with warning
 * - [item evolution] → Reads only evolution_item_id
 * - [selective grow creature] → Simplified to struct
 * - [additional animation] → Skipped
 *
 * @param[out] creatureScript Pointer to CreatureScript to populate
 * @param[in]  filename       Script filename (relative to g_creatureScriptBaseDirectory)
 * @return 1 on success, 0 on failure
 *
 * @note Uses global variables:
 *       - g_creatureScriptBaseDirectory : Base directory for creature scripts
 *       - g_creatureScriptFileList      : Global registry of all creature scripts
 *
 * @see loadRDARScriptFile() in lexer/rdar_loader.h
 * @see ScanType(), ScanInt(), ScanStr() in parser/scan_utils.h
 */
int importCreatureScript(CreatureScript* creatureScript, const char* filename);

#endif // CREATURE_PARSER_H
