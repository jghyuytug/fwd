/**
 * @file dungeon_parser.h
 * @brief Dungeon Script Parser for DNF Game Server (EXPANDING VERSION)
 *
 * IMPLEMENTATION STATUS: ~76% (113/150+ sections)
 *
 * This expanding parser successfully loads dungeon files and parses:
 * - Basic identity information (8 sections)
 * - Complete map structure (8 sections)
 * - Monster spawn tables (4 sections)
 * - Dungeon type flags (11 sections)
 * - Restrictions - Simple & Vector (9 sections)
 * - Difficulty system - Simple & Array (8 sections)
 * - Rewards/Drops - Simple & Vector (14 sections)
 * - Special Mechanics - Simple (15 sections)
 * - Batch 1-4: Simple & Arrays (30 sections)
 * - Complex Structures: Event Monsters, Worldmap, Difficulty Blocks, Matrices (6 sections) [NEW]
 *
 * The original ImportDungeonScript is 5,755 lines (largest parser in codebase).
 *
 * IMPLEMENTATION STATUS:
 * ✓ Basic Identity: 8 sections (5%)
 * ✓ Map Structure: 8 sections (5%)
 * ✓ Monster Spawning: 4 sections (3%)
 * ✓ Dungeon Types: 11 sections (7%)
 * ✓ Restrictions (Simple + Vector): 9 sections (6%)
 * ✓ Difficulty (Simple + Array): 8 sections (6%)
 * ✓ Rewards/Drops (Simple + Vector): 14 sections (9%)
 * ✓ Special Mechanics (Simple): 15 sections (10%)
 * ✓ Batch 1-4 (Simple + Arrays): 30 sections (20%)
 * ✓ Complex Structures (Nested + Matrices): 6 sections (4%) [NEW SESSION C]
 * ⚠ Remaining: 37+ advanced sections (24%) - Planned for Phase 7
 *
 * Implemented sections:
 * - [name], [explain], [index]
 * - [minimap image], [entering title], [cutscene image]
 * - [worldmap info]
 * - [map] block with [size], [greed], [start map], [boss map]
 * - [map specification], [boss map specification], [layered map specification]
 * - [individual map movement]
 * - [spawn common monster index], [spawn common champion index]
 * - [spawn super champion index], [spawn boss index]
 * - [event dungeon], [hell dungeon], [risk dungeon]
 * - [tournament dungeon], [quest npc dungeon], [tower of despair]
 * - [ancient dungeon], [powerwar dungeon], [blood dungeon]
 * - [defense dungeon], [dimension dungeon]
 * - [minimum required level], [basis level], [fatigue], [fatigue result]
 * - [disable exit], [disable skill], [limit escape character]
 * - [required item], [impossible skills]
 * - [recommended level], [coin limit], [character coin limit]
 * - [experience increasing point], [hell coin limit], [party member coin limit]
 * - [designate dungeon difficulty], [champion]
 * - [event required item], [clear condition], [gold drop prob]
 * - [common/champion/super champion/boss monster item drop prob]
 * - [clear reward item], [dungeon drop]
 * - [common/champion/super champion/boss monster item drop list]
 * - [tower stage], [blood max round], [tower recovery], [tower fp cubepiece], [tower dialog]
 * - [magical attack add], [physical attack add], [magical defense add], [move speed], [hit count]
 * - [battle spawn time], [max monster], [spawn step max], [dungeon type], [use countdown]
 *
 * Major subsystems requiring future implementation:
 * - Additional monster spawning ([event monster], etc.) - 20+ sections
 * - Restrictions - Complex ([necessary party], [road necessary party]) - 2 sections
 * - Difficulty - Nested ([easy], [medium], [hard], [ultimate], [monster difficulty bonus]) - 5 sections
 * - Rewards/Drops - Complex ([tower item drop], [dungeon object drop], tournament/altar) - 8 sections
 * - Special Mechanics - Vector/Complex (103+ more sections)
 * - Remaining simple mechanics - 50+ sections
 *
 * Extracted from: df_game_r_part21.c:6273-12028 (5,755 lines)
 *
 * @date 2025-01-XX
 */

#ifndef DUNGEON_PARSER_H
#define DUNGEON_PARSER_H

#include <string>
#include <vector>
#include <utility>  // for std::pair

// ============================================================================
// Complex Structure Definitions (NEW)
// ============================================================================

/**
 * @struct EventMonster
 * @brief Event monster spawn configuration (5-field structure)
 *
 * Extracted from: df_game_r_part21.c:10439-10499
 * Size: 20 bytes (5 DWORDs)
 *
 * Each entry represents a special event monster with 5 integer parameters.
 * Field 3 (param3) is accumulated across all entries.
 */
struct EventMonster {
    int param1;  ///< First monster parameter
    int param2;  ///< Second monster parameter
    int param3;  ///< Third parameter (accumulated across all entries)
    int param4;  ///< Fourth monster parameter
    int param5;  ///< Fifth monster parameter

    EventMonster() : param1(0), param2(0), param3(0), param4(0), param5(0) {}
};

/**
 * @struct WorldmapPatternInfo
 * @brief Worldmap pattern display configuration (simplified)
 *
 * Extracted from: df_game_r_part21.c:7459-7474
 *
 * The full STWorldmapPattern uses table lookup (GetWorldMapCoordFromTable)
 * to populate 6 DWORD fields. This simplified version stores the parsed data.
 */
struct WorldmapPatternInfo {
    int patternId;       ///< Unique pattern identifier
    int coordX;          ///< X coordinate (0-indexed)
    int coordY;          ///< Y coordinate (0-indexed)
    int mapData[6];      ///< Coordinate lookup table data (6 DWORDs)
    std::string description; ///< Pattern description/name

    WorldmapPatternInfo() : patternId(0), coordX(0), coordY(0) {
        for (int i = 0; i < 6; i++) mapData[i] = 0;
    }

    void clear() {
        patternId = 0;
        coordX = 0;
        coordY = 0;
        for (int i = 0; i < 6; i++) mapData[i] = 0;
        description.clear();
    }
};

/**
 * @struct DifficultyRewardItem
 * @brief Reward item for difficulty-specific altar rewards
 *
 * Extracted from: df_game_r_part21.c:11859-11941 (advance altar clear reward)
 * Used in both altar clear and survival clear reward sections
 */
struct DifficultyRewardItem {
    std::string itemType;  ///< Reward item type (parsed by getRewardItemType)
    int itemCount;         ///< Number of items
    int parameter;         ///< Additional parameter

    DifficultyRewardItem() : itemCount(0), parameter(0) {}

    void clear() {
        itemType.clear();
        itemCount = 0;
        parameter = 0;
    }
};

/**
 * @struct DimensionCoinInfo
 * @brief Dimension activation coin configuration per difficulty
 *
 * Extracted from: df_game_r_part21.c:33620-33700
 */
struct DimensionCoinInfo {
    int coinValue1;  ///< First coin value
    int coinValue2;  ///< Second coin value

    DimensionCoinInfo() : coinValue1(0), coinValue2(0) {}
};

// ============================================================================
// Grid and Maze Structures
// ============================================================================

/**
 * @struct GridScript
 * @brief Single grid cell in dungeon map
 *
 * Extracted from: df_game_r_part21.c:12803-12813 (constructor/clear)
 *
 * Grid types (from g_gridSymbols, 0-15):
 * - 0 ("  "): Empty space
 * - 1 ("##"): Wall
 * - 2 ("XX"): Monster room
 * - 3 ("SS"): Start room
 * - 4 ("BB"): Boss room
 * - 5 ("CC"): Clear/exit room
 * - 6-15: Other room types (extended types for special dungeons)
 */
struct GridScript {
    int type;                              ///< Grid cell type (0-15)
    std::vector<int> specification;        ///< Monster IDs for this cell
    std::vector<int> bossSpecification;    ///< Boss monster IDs
    std::vector<int> layeredSpecification; ///< Layered monster IDs

    GridScript() : type(0) {}

    void clear() {
        type = 0;
        specification.clear();
        bossSpecification.clear();
        layeredSpecification.clear();
    }
};

/**
 * @struct MazeScript
 * @brief Dungeon map layout configuration
 *
 * Extracted from: df_game_r_part21.c:12854-12890 (constructor/clear/getGrid)
 *
 * Total size: ~116+ bytes
 */
struct MazeScript {
    int width;                                     ///< Map width (default: 1)
    int height;                                    ///< Map height (default: 1)
    std::vector<GridScript> grids;                 ///< Grid cells (size = width * height)
    std::vector<std::pair<int, int> > startMap;     ///< Start room indices (map_index, rate)
    std::vector<std::pair<int, int> > bossMap;      ///< Boss room indices (map_index, rate)

    // Note: Additional fields in original (clearMapIndex, ridableScript, clearConds, etc.)
    // are simplified/omitted in this version

    MazeScript() : width(1), height(1) {}

    void clear() {
        width = 1;
        height = 1;
        grids.clear();
        startMap.clear();
        bossMap.clear();
    }

    /**
     * @brief Get grid at position (x, y)
     * @param x X coordinate
     * @param y Y coordinate
     * @return Pointer to GridScript at position
     */
    GridScript* getGrid(int x, int y) {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            return &grids[x + y * width];
        }
        return NULL;
    }
};

/**
 * @struct STDungeonScript
 * @brief Dungeon configuration (EXPANDING VERSION)
 *
 * Original structure has ~200+ fields organized into subsystems:
 * - Basic identity and visual properties
 * - Map layout and room connections (vector<MazeScript>)
 * - Monster spawn tables and rates
 * - Reward/drop configurations
 * - Dungeon type flags (ancient/tower/hell/event/blood/defense)
 * - Difficulty-specific properties (Easy/Normal/Hard/Ultimate)
 * - Special mechanics (rounds, seals, pathgates, countdown timers)
 * - Restrictions (required items, disabled skills, party requirements)
 * - AI behavior configurations
 * - Recovery stair placements (vector<RecoveryStairInfo>)
 * - Object spawn data
 * - PvP/defense mode settings
 *
 * This skeleton version includes only 10 most critical fields.
 */
struct STDungeonScript {
    // ========================================================================
    // CORE IDENTITY (8 fields - IMPLEMENTED)
    // ========================================================================

    int dungeonIndex;          ///< Unique dungeon ID/index
    std::string name;          ///< Dungeon display name
    std::string explain;       ///< Dungeon description text
    std::string enteringTitle; ///< Entry screen title text
    std::string cutsceneImage; ///< Cutscene image asset path
    int cutsceneIndex;         ///< Cutscene index number
    std::string minimapImage;  ///< Minimap texture asset path

    /**
     * @brief Worldmap position data
     *
     * Format: worldmapPos[difficulty][coordinate]
     * - worldmapPos[0][0/1] = Easy difficulty (x, y)
     * - worldmapPos[1][0/1] = Normal difficulty (x, y)
     *
     * NOTE: Original has 4 difficulty levels (Easy/Normal/Hard/Ultimate)
     */
    int worldmapPos[2][2];

    // ========================================================================
    // METADATA (2 fields - IMPLEMENTED)
    // ========================================================================

    int scriptFileIndex;       ///< Index in global script file list
    std::string scriptFilePath; ///< Full path to script file

    // ========================================================================
    // MAP STRUCTURE (8 sections - IMPLEMENTED)
    // ========================================================================

    std::vector<MazeScript> mazeScripts;   ///< Dungeon map layouts (offset +368)
    bool individualMapMovement;            ///< Allow free room selection (offset +1284)

    // ========================================================================
    // MONSTER SPAWNING (4 sections - IMPLEMENTED)
    // ========================================================================

    std::vector<int> spawnCommonMonsterIndex;   ///< Common monster IDs (offset +104)
    std::vector<int> spawnCommonChampionIndex;  ///< Champion (elite) monster IDs (offset +107)
    std::vector<int> spawnSuperChampionIndex;   ///< Super champion monster IDs (offset +110)
    std::vector<int> spawnBossIndex;            ///< Boss monster IDs (offset +113)

    // ========================================================================
    // DUNGEON TYPES (11 sections - IMPLEMENTED)
    // ========================================================================

    bool eventDungeon;          ///< Event dungeon flag (offset +136)
    bool hellDungeon;           ///< Hell mode flag (offset +320)
    bool riskDungeon;           ///< Risk dungeon flag (offset +312)
    bool tournamentDungeon;     ///< Tournament dungeon flag
    bool questNpcDungeon;       ///< Quest NPC dungeon flag
    bool towerOfDespair;        ///< Tower of Despair mode (offset +681)
    bool ancientDungeon;        ///< Ancient dungeon flag (offset +652)
    bool powerwarDungeon;       ///< Powerwar dungeon flag (offset +653)
    int bloodDungeon;           ///< Blood dungeon level (0=off, >0=level) (offset +654)
    bool defenseDungeon;        ///< Defense mode flag (offset +683)
    bool dimensionDungeon;      ///< Dimension dungeon flag

    // ========================================================================
    // RESTRICTIONS - SIMPLE (7 sections - IMPLEMENTED)
    // ========================================================================

    int minimumRequiredLevel;   ///< Minimum level requirement (offset +30 DWORD)
    int basisLevel;             ///< Basis level for scaling (offset +43 DWORD)
    int fatigue;                ///< Fatigue point cost (offset +160 DWORD)
    int fatigueResult;          ///< Fatigue result value (offset +161 DWORD)
    int disableExit;            ///< Disable exit value (offset +322 DWORD)
    bool disableSkillFlag;      ///< Disable skill flag (offset +322 BYTE)
    bool limitEscapeCharacter;  ///< Limit escape character (offset +680 BYTE)

    // ========================================================================
    // RESTRICTIONS - VECTOR (2 sections - IMPLEMENTED)
    // ========================================================================

    int requiredItemId1;        ///< First required item ID (offset +31 DWORD)
    int requiredItemId2;        ///< Second required item ID (offset +32 DWORD)
    bool requiredItemFlag;      ///< Required item flag (offset +132 BYTE)
    std::vector<std::pair<int, int> > impossibleSkills;  ///< (job, skill_id) pairs (offset +324)

    // ========================================================================
    // DIFFICULTY SYSTEM - SIMPLE (7 sections - IMPLEMENTED)
    // ========================================================================

    int recommendedLevel1;      ///< Recommended level (first value) (offset +276 DWORD)
    int recommendedLevel2;      ///< Recommended level (second value) (offset +277 DWORD)
    int coinLimit;              ///< Coin/gold limit (offset +41 DWORD)
    int characterCoinLimit;     ///< Character coin limit (offset +42 DWORD)
    float experienceIncreasingPoint;  ///< XP multiplier (offset +44 FLOAT)
    int hellCoinLimit;          ///< Hell dungeon coin limit (offset +90 DWORD)
    int partyMemberCoinLimit;   ///< Party member coin limit (offset +2370)
    int designateDungeonDifficulty;  ///< Fixed difficulty override (offset +292 DWORD)

    // ========================================================================
    // DIFFICULTY SYSTEM - ARRAY (1 section - IMPLEMENTED)
    // ========================================================================

    int champion[5];            ///< Champion configuration (5 values) (offset +45 to +49)

    // ========================================================================
    // REWARDS/DROPS - SIMPLE (8 sections - IMPLEMENTED)
    // ========================================================================

    int eventRequiredItemId1;   ///< Event required item ID 1 (offset +35 DWORD)
    int eventRequiredItemId2;   ///< Event required item ID 2 (offset +36 DWORD)
    bool eventRequiredItemFlag; ///< Event required item flag (offset +148 BYTE)
    int clearCondition1;        ///< Clear condition value 1
    int clearCondition2;        ///< Clear condition value 2
    int goldDropProb;           ///< Gold drop probability (offset +121 DWORD)
    int commonMonsterItemDropProb;      ///< Common monster item drop probability (offset +122 DWORD)
    int commonChampionItemDropProb;     ///< Common champion item drop probability (offset +123 DWORD)
    int superChampionItemDropProb;      ///< Super champion item drop probability (offset +124 DWORD)
    int bossItemDropProb;       ///< Boss item drop probability (offset +125 DWORD)

    // ========================================================================
    // REWARDS/DROPS - VECTOR (6 sections - IMPLEMENTED)
    // ========================================================================

    std::vector<std::pair<int, int> > clearRewardItems;  ///< Clear reward items (item_id, count) (offset +38/152)
    std::vector<std::pair<int, int> > dungeonDrops;      ///< Dungeon drops (item_id, rate) (offset +56/224)
    std::vector<int> commonMonsterItemDropList;         ///< Common monster item drops (offset +126/504)
    std::vector<int> commonChampionItemDropList;        ///< Common champion item drops (offset +129/516)
    std::vector<int> superChampionItemDropList;         ///< Super champion item drops (offset +132/528)
    std::vector<int> bossItemDropList;                  ///< Boss item drops (offset +135/540)

    // ========================================================================
    // SPECIAL MECHANICS - SIMPLE (15 sections - IMPLEMENTED)
    // ========================================================================

    // Tower Mechanics (5 fields)
    std::string towerStage;     ///< Tower stage type (offset +1784)
    int bloodMaxRound;          ///< Blood max round (offset +1855)
    float towerRecovery;        ///< Tower recovery multiplier (offset +1975)
    float towerFpCubepiece;     ///< Tower FP cubepiece (offset +1969)
    float towerDialog;          ///< Tower dialog value (offset +1998)

    // Combat Stats (5 fields)
    int magicalAttackAdd;       ///< Magical attack bonus (offset +1378)
    int physicalAttackAdd;      ///< Physical attack bonus
    int magicalDefenseAdd;      ///< Magical defense bonus
    float moveSpeed;            ///< Move speed multiplier (offset +1369)
    int hitCount;               ///< Hit count (offset +1693)

    // Spawn/Mechanics (5 fields)
    int battleSpawnTime;        ///< Battle spawn time (offset +1324)
    int maxMonster;             ///< Max monster count (offset +1131)
    int spawnStepMax;           ///< Spawn step max (offset +1190)
    int dungeonType;            ///< Dungeon type (offset +1741)
    float useCountdown;         ///< Use countdown flag/value (offset +887)

    // ========================================================================
    // BATCH 1: BASIC INFO & CONFIG (10 sections - NEW)
    // ========================================================================

    std::string enteringTitleNext;      ///< Secondary entering title text
    std::string dungeonLoadingImagePath;///< Loading screen image path
    int herosmodeRequiredQuest;         ///< Heroes mode required quest ID
    int backgroundPos;                  ///< Background position value
    bool herosmodeEnable;               ///< Heroes mode enable flag
    bool escapeHell;                    ///< Escape hell flag
    int phaseFatigue;                   ///< Phase fatigue cost
    int warroomMapIndex;                ///< Warroom map index
    std::string revisionTable;          ///< Revision table name
    bool goldCardUse;                   ///< Gold card usage allowed

    // ========================================================================
    // BATCH 2: MONSTER SPAWN CONFIG (8 sections - NEW)
    // ========================================================================

    float monsterExpBonusPerUserDecrease; ///< Monster XP bonus decrease per user
    float resultExpBonusPerUserDecrease;  ///< Result XP bonus decrease per user
    int monsterSpawnBaseInterval;         ///< Base spawn interval (ms)
    int monsterSpawnRandomInterval;       ///< Random spawn interval variance (ms)
    int playerKc;                         ///< Player kill count value
    float killCountConst;                 ///< Kill count constant multiplier
    int aiCharacterAppearRate;            ///< AI character appearance rate (%)
    bool prohibitPractice;                ///< Prohibit practice mode flag

    // ========================================================================
    // BATCH 3: COMBAT & MECHANICS (7 sections - NEW)
    // ========================================================================

    float useZoomRate;                    ///< Camera zoom rate
    int sealDoorMapIndex;                 ///< Seal door map index
    bool roomMoveKeepState;               ///< Keep state when moving between rooms
    bool usableItem;                      ///< Items usable in dungeon
    int towerItemDrop;                    ///< Tower item drop value
    int towerMaxClearItemNum;             ///< Tower max clear item number
    float towerLimitOfStackableItem;      ///< Tower stackable item limit

    // ========================================================================
    // BATCH 4: SIMPLE ARRAYS (5 sections - NEW)
    // ========================================================================

    int pointByType[3];                   ///< Point by type array (3 values)
    int monsterTypeSpawnCost[4];          ///< Monster type spawn cost (4 types)
    float monsterTypeSpawnIntervalRate[4];///< Monster type spawn interval rate (4 types)
    std::vector<int> spawnStepResourcePool; ///< Spawn step resource pool
    std::vector<int> deadTowerMapIndex;     ///< Dead tower map indices

    // ========================================================================
    // COMPLEX STRUCTURES - EVENT MONSTERS (3 sections - NEW)
    // ========================================================================

    int eventMonsterConfig1;                ///< Event monster configuration value 1 (offset +245)
    int eventMonsterConfig2;                ///< Event monster configuration value 2 (offset +246)
    int eventMonsterAccumulator;            ///< Accumulated param3 from all event monsters (offset +247)
    std::vector<EventMonster> eventMonsters; ///< Event monster list (offset +992)

    // ========================================================================
    // COMPLEX STRUCTURES - WORLDMAP PATTERN (1 section - NEW)
    // ========================================================================

    WorldmapPatternInfo worldmapPattern;    ///< Worldmap pattern display configuration

    // ========================================================================
    // COMPLEX STRUCTURES - DIFFICULTY REWARDS (3 sections - NEW)
    // ========================================================================

    /**
     * @brief Altar clear rewards by difficulty
     *
     * Index: 0=Easy, 1=Medium, 2=Hard
     * Storage: 3 vectors at offsets 724, 736, 748 (12-byte spacing)
     */
    std::vector<DifficultyRewardItem> altarClearReward[3];

    /**
     * @brief Survival clear rewards by difficulty
     *
     * Index: 0=Easy, 1=Medium, 2=Hard
     * Storage: 3 vectors at offsets 724, 736, 748 (12-byte spacing)
     */
    std::vector<DifficultyRewardItem> survivalClearReward[3];

    /**
     * @brief Dimension activation coin info by difficulty
     *
     * Key: 0=Easy, 1=Medium, 2=Hard, 3=Ultimate
     * Storage: Map at offset +4
     *
     * Note: This uses 4 difficulty levels (including Ultimate)
     */
    DimensionCoinInfo dimensionCoinInfo[4];

    // ========================================================================
    // COMPLEX STRUCTURES - MONSTER DIFFICULTY BONUS (1 section - NEW)
    // ========================================================================

    /**
     * @brief Monster difficulty bonus matrix (13 levels × 5 bonuses)
     *
     * Structure: 2D array (vector of vectors)
     * Dimensions: 13 difficulty tiers × 5 bonus multipliers per tier
     * Storage: offset +1028
     * Total elements: 65 floats
     *
     * Unlike other difficulty systems, this uses 13 tiers (0-12)
     * instead of the standard 4 (Easy/Medium/Hard/Ultimate).
     */
    std::vector<std::vector<float> > monsterDifficultyBonus;

    // ========================================================================
    // COMPLEX STRUCTURES - PATHGATE OBJECTS (1 section - NEW)
    // ========================================================================

    /**
     * @brief Pathgate object configuration (4×3 matrix)
     *
     * Structure: 4 pathgates × 3 parameters each
     * Storage: 12 integers total
     */
    int pathgateObject[4][3];

    // ========================================================================
    // P0 SIMPLE PARAMETERS (Phase 1 - Session F)
    // ========================================================================

    /**
     * @brief Experience constants for different monster types
     */
    int bossExpConst;              ///< Boss experience constant (offset +1268)
    int commonChampionExpConst;    ///< Common champion experience constant (offset +1252)
    int commonMonsterExpConst;     ///< Common monster experience constant (offset +1244)
    int superChampionExpConst;     ///< Super champion experience constant (offset +1260)

    /**
     * @brief Entry and fatigue settings
     */
    int joinCostGold;              ///< Gold cost to enter dungeon (offset +1214)
    bool noFatigue;                ///< No fatigue consumption flag (offset +2164)
    bool regenerate;               ///< HP regeneration enabled (offset +952)

    /**
     * @brief Door and map settings
     */
    bool openDoorEvenEnemy;        ///< Open door even with enemies present (offset +2388)
    int minimapIcon;               ///< Minimap icon index (offset +957)

    /**
     * @brief Dummy and tower settings
     */
    int dummyAppearCount;          ///< Dummy character appear count (offset +1738)
    float towerHighSkillCoolTime[2];     ///< Tower high skill initial cool time (2 values) (offset +1961)
    float towerHighSkillCoolTimeRate[2]; ///< Tower high skill cool time rate (2 values) (offset +1965)

    /**
     * @brief P0 Vector/Array sections
     */
    int posX;                      ///< Position X coordinate (offset +1006)
    int posY;                      ///< Position Y coordinate (offset +1006)
    int towerRandomMapIndexCount;  ///< Tower random map index count (offset +1781)
    std::vector<std::pair<int, int> > startMap;        ///< Start map positions (offset +830)
    std::vector<std::pair<int, int> > deathTowerMapIndexes; ///< Death tower map index pairs (offset +1822)
    std::vector<int> seekingMonsters;                  ///< Seeking monster list (offset +variable)

    /**
     * @brief P1 Map/Monster configurations
     */
    int clearMap;                  ///< Clear map index
    int advanceAltarType;          ///< Advance altar type
    int bossRoomEntranceCondition; ///< Boss room entrance condition

    /**
     * @brief P1 Restrictions
     */
    int limitInoutCount;           ///< Limit in/out count
    int limitPartyCount;           ///< Limit party count
    int partyNumCheck;             ///< Party number check
    int questConnection;           ///< Quest connection ID

    /**
     * @brief P1 Hunt configurations
     */
    int huntApc;                   ///< Hunt APC value
    int huntBoss;                  ///< Hunt Boss value
    int huntMonster;               ///< Hunt Monster value

    /**
     * @brief P2 Tournament and special settings
     */
    int tournamentClearRewardExp;      ///< Tournament clear reward exp
    float tournamentClearRewardGoldRate; ///< Tournament clear reward gold rate
    int tournamentRoundFatigue;        ///< Tournament round fatigue
    int sealDoorAppearRate;            ///< Seal door appear rate
    std::vector<int> sealDoorPos;      ///< Seal door positions

    /**
     * @brief P1 Altar and special vectors
     */
    std::vector<int> advanceAltarMap;         ///< Advance altar map indices
    std::vector<int> advanceAltarSurvivalMap; ///< Advance altar survival map indices
    std::vector<int> specialPassiveObjectItem; ///< Special passive object items

    /**
     * @brief Final complex sections (simplified implementations)
     */
    std::vector<int> bossMapIndices;           ///< Boss map indices (simplified)
    int mapSpecificationCount;                 ///< Map specification count (stub)
    std::vector<int> eventMonster2Data;        ///< Event monster2 data (simplified)
    int mazeInfoWidth;                         ///< Maze info width (stub)
    int mazeInfoHeight;                        ///< Maze info height (stub)

    // ========================================================================
    // ADVANCED SUBSYSTEMS - EXPANSION DESIGN (NOT YET IMPLEMENTED)
    // ========================================================================

    /*
     * Additional Monster Spawning (20+ sections, ~70 fields)
     * - std::vector<EventMonsterInfo> eventMonsters: Event mobs
     * - int aiCharacterAppearRate: AI character spawn rate
     * Implementation: Phase 7 expansion
     */

    /*
     * Additional Dungeon Types (5+ sections)
     * - bool villageAttackRevengeDungeon: Village attack revenge mode
     * - bool warareadungeon: Wararea dungeon flag
     * - bool unableDungeon: Unable dungeon flag
     * Implementation: Phase 7 expansion
     */

    /*
     * Restrictions - Vector (2 sections)
     * - std::vector<int> requiredItems: [required item] - item ID list
     * - std::vector<std::pair<int,int>> impossibleSkills: [impossible skills] - (job, skill_id)
     * Implementation: Phase 7 expansion
     */

    /*
     * Restrictions - Complex (2 sections)
     * - int necessaryParty: [necessary party] - DimensionPartyCount
     * - std::vector<DimensionPartyCount> roadNecessaryParty: [road necessary party]
     * Implementation: Phase 7 expansion
     */

    /*
     * Difficulty System (15+ sections, ~60 fields)
     * struct DifficultyProperties {
     *     int recommendedLevel;   // Level requirement
     *     int monsterHpMultiplier; // Monster HP scaling
     *     int rewardMultiplier;   // Reward scaling
     * };
     * DifficultyProperties difficultyProps[4]; // Easy/Normal/Hard/Ultimate
     * Implementation: Phase 7 expansion
     */

    /*
     * Rewards & Drops (18+ sections, ~40 fields)
     * - std::vector<ClearRewardItem> clearRewards: Clear rewards
     * - std::vector<BossDropInfo> bossDropList: Boss drops
     * - std::vector<DungeonDropInfo> dungeonDrops: General drops
     * - std::vector<TowerItemDrop> towerDrops: Tower-specific drops
     * Implementation: Phase 7 expansion
     */

    /*
     * Entry Restrictions (15+ sections, ~30 fields)
     * - std::vector<int> requiredItems: Required items to enter
     * - std::vector<int> disabledSkills: Disabled skill IDs
     * - std::vector<int> impossibleSkills: Impossible skill IDs
     * - std::vector<int> allowedCharacters: Character class restrictions
     * - int necessaryPartySize: Required party size
     * - int maxPartySize: Maximum party size
     * Implementation: Phase 7 expansion
     */

    /*
     * Special Mechanics (35+ sections, ~70 fields)
     * - int maxRounds: Maximum rounds (survival mode)
     * - int bloodMaxRound: Blood dungeon rounds
     * - bool useCountdown: Enable countdown timer
     * - int countdownTime: Countdown duration
     * - std::vector<SealDoorInfo> sealDoors: Sealed door data
     * - std::vector<PathgateInfo> pathgates: Pathgate objects
     * - std::vector<RecoveryStairInfo> recoveryStairs: Recovery points
     * Implementation: Phase 7 expansion
     */

    /*
     * Object Placement (10+ sections, ~25 fields)
     * - std::vector<ObjectSpawnInfo> objectSpawns: Object spawn data
     * - int backgroundPosX, backgroundPosY: Background position
     * Implementation: Phase 7 expansion
     */

    /*
     * PvP & Defense Mode (10+ sections, ~20 fields)
     * - bool isPvPDungeon: PvP mode
     * - int defenseWaveCount: Defense wave count
     * - std::vector<DefenseWaveInfo> defenseWaves: Defense wave data
     * Implementation: Phase 7 expansion
     */

    // ========================================================================
    // Methods
    // ========================================================================

    /**
     * @brief Default constructor
     */
    STDungeonScript()
        : dungeonIndex(0)
        , cutsceneIndex(0)
        , scriptFileIndex(0)
        , individualMapMovement(false)
        , eventDungeon(false)
        , hellDungeon(false)
        , riskDungeon(false)
        , tournamentDungeon(false)
        , questNpcDungeon(false)
        , towerOfDespair(false)
        , ancientDungeon(false)
        , powerwarDungeon(false)
        , bloodDungeon(0)
        , defenseDungeon(false)
        , dimensionDungeon(false)
        , minimumRequiredLevel(0)
        , basisLevel(0)
        , fatigue(0)
        , fatigueResult(0)
        , disableExit(0)
        , disableSkillFlag(false)
        , limitEscapeCharacter(false)
        , requiredItemId1(0)
        , requiredItemId2(0)
        , requiredItemFlag(false)
        , recommendedLevel1(0)
        , recommendedLevel2(0)
        , coinLimit(0)
        , characterCoinLimit(0)
        , experienceIncreasingPoint(0.0f)
        , hellCoinLimit(0)
        , partyMemberCoinLimit(0)
        , designateDungeonDifficulty(0)
        , eventRequiredItemId1(0)
        , eventRequiredItemId2(0)
        , eventRequiredItemFlag(false)
        , clearCondition1(0)
        , clearCondition2(0)
        , goldDropProb(0)
        , commonMonsterItemDropProb(0)
        , commonChampionItemDropProb(0)
        , superChampionItemDropProb(0)
        , bossItemDropProb(0)
        , bloodMaxRound(0)
        , towerRecovery(0.0f)
        , towerFpCubepiece(0.0f)
        , towerDialog(0.0f)
        , magicalAttackAdd(0)
        , physicalAttackAdd(0)
        , magicalDefenseAdd(0)
        , moveSpeed(0.0f)
        , hitCount(0)
        , battleSpawnTime(0)
        , maxMonster(0)
        , spawnStepMax(0)
        , dungeonType(0)
        , useCountdown(0.0f)
        , herosmodeRequiredQuest(0)
        , backgroundPos(0)
        , herosmodeEnable(false)
        , escapeHell(false)
        , phaseFatigue(0)
        , warroomMapIndex(0)
        , goldCardUse(false)
        , monsterExpBonusPerUserDecrease(0.0f)
        , resultExpBonusPerUserDecrease(0.0f)
        , monsterSpawnBaseInterval(0)
        , monsterSpawnRandomInterval(0)
        , playerKc(0)
        , killCountConst(0.0f)
        , aiCharacterAppearRate(0)
        , prohibitPractice(false)
        , useZoomRate(0.0f)
        , sealDoorMapIndex(0)
        , roomMoveKeepState(false)
        , usableItem(false)
        , towerItemDrop(0)
        , towerMaxClearItemNum(0)
        , towerLimitOfStackableItem(0.0f)
        , eventMonsterConfig1(0)
        , eventMonsterConfig2(0)
        , eventMonsterAccumulator(0)
        , bossExpConst(0)
        , commonChampionExpConst(0)
        , commonMonsterExpConst(0)
        , superChampionExpConst(0)
        , joinCostGold(0)
        , noFatigue(false)
        , regenerate(false)
        , openDoorEvenEnemy(false)
        , minimapIcon(0)
        , dummyAppearCount(0)
        , posX(0)
        , posY(0)
        , towerRandomMapIndexCount(0)
        , clearMap(0)
        , advanceAltarType(0)
        , bossRoomEntranceCondition(0)
        , limitInoutCount(0)
        , limitPartyCount(0)
        , partyNumCheck(0)
        , questConnection(0)
        , huntApc(0)
        , huntBoss(0)
        , huntMonster(0)
        , tournamentClearRewardExp(0)
        , tournamentClearRewardGoldRate(0.0f)
        , tournamentRoundFatigue(0)
        , sealDoorAppearRate(0)
        , mapSpecificationCount(0)
        , mazeInfoWidth(0)
        , mazeInfoHeight(0) {
        worldmapPos[0][0] = worldmapPos[0][1] = 0;
        worldmapPos[1][0] = worldmapPos[1][1] = 0;
        for (int i = 0; i < 5; i++) champion[i] = 0;
        for (int i = 0; i < 3; i++) pointByType[i] = 0;
        for (int i = 0; i < 4; i++) {
            monsterTypeSpawnCost[i] = 0;
            monsterTypeSpawnIntervalRate[i] = 0.0f;
            dimensionCoinInfo[i] = DimensionCoinInfo();
            for (int j = 0; j < 3; j++) {
                pathgateObject[i][j] = 0;
            }
        }
        for (int i = 0; i < 2; i++) {
            towerHighSkillCoolTime[i] = 0.0f;
            towerHighSkillCoolTimeRate[i] = 0.0f;
        }
    }

    /**
     * @brief Reset all fields to default values
     */
    void clear() {
        dungeonIndex = 0;
        name.clear();
        explain.clear();
        enteringTitle.clear();
        cutsceneImage.clear();
        cutsceneIndex = 0;
        minimapImage.clear();
        worldmapPos[0][0] = worldmapPos[0][1] = 0;
        worldmapPos[1][0] = worldmapPos[1][1] = 0;
        scriptFileIndex = 0;
        scriptFilePath.clear();

        mazeScripts.clear();
        individualMapMovement = false;

        spawnCommonMonsterIndex.clear();
        spawnCommonChampionIndex.clear();
        spawnSuperChampionIndex.clear();
        spawnBossIndex.clear();

        eventDungeon = false;
        hellDungeon = false;
        riskDungeon = false;
        tournamentDungeon = false;
        questNpcDungeon = false;
        towerOfDespair = false;
        ancientDungeon = false;
        powerwarDungeon = false;
        bloodDungeon = 0;
        defenseDungeon = false;
        dimensionDungeon = false;

        minimumRequiredLevel = 0;
        basisLevel = 0;
        fatigue = 0;
        fatigueResult = 0;
        disableExit = 0;
        disableSkillFlag = false;
        limitEscapeCharacter = false;

        requiredItemId1 = 0;
        requiredItemId2 = 0;
        requiredItemFlag = false;
        impossibleSkills.clear();

        recommendedLevel1 = 0;
        recommendedLevel2 = 0;
        coinLimit = 0;
        characterCoinLimit = 0;
        experienceIncreasingPoint = 0.0f;
        hellCoinLimit = 0;
        partyMemberCoinLimit = 0;
        designateDungeonDifficulty = 0;

        for (int i = 0; i < 5; i++) champion[i] = 0;

        eventRequiredItemId1 = 0;
        eventRequiredItemId2 = 0;
        eventRequiredItemFlag = false;
        clearCondition1 = 0;
        clearCondition2 = 0;
        goldDropProb = 0;
        commonMonsterItemDropProb = 0;
        commonChampionItemDropProb = 0;
        superChampionItemDropProb = 0;
        bossItemDropProb = 0;

        clearRewardItems.clear();
        dungeonDrops.clear();
        commonMonsterItemDropList.clear();
        commonChampionItemDropList.clear();
        superChampionItemDropList.clear();
        bossItemDropList.clear();

        towerStage.clear();
        bloodMaxRound = 0;
        towerRecovery = 0.0f;
        towerFpCubepiece = 0.0f;
        towerDialog = 0.0f;
        magicalAttackAdd = 0;
        physicalAttackAdd = 0;
        magicalDefenseAdd = 0;
        moveSpeed = 0.0f;
        hitCount = 0;
        battleSpawnTime = 0;
        maxMonster = 0;
        spawnStepMax = 0;
        dungeonType = 0;
        useCountdown = 0.0f;

        enteringTitleNext.clear();
        dungeonLoadingImagePath.clear();
        herosmodeRequiredQuest = 0;
        backgroundPos = 0;
        herosmodeEnable = false;
        escapeHell = false;
        phaseFatigue = 0;
        warroomMapIndex = 0;
        revisionTable.clear();
        goldCardUse = false;

        monsterExpBonusPerUserDecrease = 0.0f;
        resultExpBonusPerUserDecrease = 0.0f;
        monsterSpawnBaseInterval = 0;
        monsterSpawnRandomInterval = 0;
        playerKc = 0;
        killCountConst = 0.0f;
        aiCharacterAppearRate = 0;
        prohibitPractice = false;

        useZoomRate = 0.0f;
        sealDoorMapIndex = 0;
        roomMoveKeepState = false;
        usableItem = false;
        towerItemDrop = 0;
        towerMaxClearItemNum = 0;
        towerLimitOfStackableItem = 0.0f;

        for (int i = 0; i < 3; i++) pointByType[i] = 0;
        for (int i = 0; i < 4; i++) {
            monsterTypeSpawnCost[i] = 0;
            monsterTypeSpawnIntervalRate[i] = 0.0f;
        }
        spawnStepResourcePool.clear();
        deadTowerMapIndex.clear();

        eventMonsterConfig1 = 0;
        eventMonsterConfig2 = 0;
        eventMonsterAccumulator = 0;
        eventMonsters.clear();

        // P0 Simple Parameters
        bossExpConst = 0;
        commonChampionExpConst = 0;
        commonMonsterExpConst = 0;
        superChampionExpConst = 0;
        joinCostGold = 0;
        noFatigue = false;
        regenerate = false;
        openDoorEvenEnemy = false;
        minimapIcon = 0;
        dummyAppearCount = 0;
        for (int i = 0; i < 2; i++) {
            towerHighSkillCoolTime[i] = 0.0f;
            towerHighSkillCoolTimeRate[i] = 0.0f;
        }

        // P0 Vectors
        posX = 0;
        posY = 0;
        towerRandomMapIndexCount = 0;
        startMap.clear();
        deathTowerMapIndexes.clear();
        seekingMonsters.clear();

        // P1 Simple Sections
        clearMap = 0;
        advanceAltarType = 0;
        bossRoomEntranceCondition = 0;
        limitInoutCount = 0;
        limitPartyCount = 0;
        partyNumCheck = 0;
        questConnection = 0;
        huntApc = 0;
        huntBoss = 0;
        huntMonster = 0;

        // P2 Tournament and special
        tournamentClearRewardExp = 0;
        tournamentClearRewardGoldRate = 0.0f;
        tournamentRoundFatigue = 0;
        sealDoorAppearRate = 0;
        sealDoorPos.clear();

        // P1 Altar and special vectors
        advanceAltarMap.clear();
        advanceAltarSurvivalMap.clear();
        specialPassiveObjectItem.clear();

        // Final complex sections
        bossMapIndices.clear();
        mapSpecificationCount = 0;
        eventMonster2Data.clear();
        mazeInfoWidth = 0;
        mazeInfoHeight = 0;

        worldmapPattern.clear();

        for (int i = 0; i < 3; i++) {
            altarClearReward[i].clear();
            survivalClearReward[i].clear();
        }
        for (int i = 0; i < 4; i++) {
            dimensionCoinInfo[i] = DimensionCoinInfo();
            for (int j = 0; j < 3; j++) {
                pathgateObject[i][j] = 0;
            }
        }

        monsterDifficultyBonus.clear();
    }
};

/**
 * @brief Parse dungeon script file (EXPANDING VERSION)
 *
 * IMPLEMENTATION STATUS: ~20% (31/150+ sections)
 *
 * This expanding parser successfully parses:
 *
 * ✓ Basic Identity (8 sections):
 *   - [name], [explain], [index]
 *   - [minimap image], [entering title], [cutscene image]
 *   - [worldmap info]
 *
 * ✓ Map Structure (8 sections):
 *   - [map] block with nested sub-sections:
 *     - [size] - Map dimensions (width x height)
 *     - [greed] - Grid layout string with 2-char symbols
 *     - [start map] - Starting room indices with rates
 *     - [boss map] - Boss room indices with rates
 *     - [map specification] - Monster IDs per grid cell
 *     - [boss map specification] - Boss monster IDs per cell
 *     - [layered map specification] - Layered monster IDs per cell
 *   - [individual map movement] - Free room selection flag
 *
 * ✓ Monster Spawning (4 sections):
 *   - [spawn common monster index] - Common monster ID list
 *   - [spawn common champion index] - Champion monster ID list
 *   - [spawn super champion index] - Super champion ID list
 *   - [spawn boss index] - Boss monster ID list
 *
 * ✓ Dungeon Types (11 sections):
 *   - [event dungeon], [hell dungeon], [risk dungeon]
 *   - [tournament dungeon], [quest npc dungeon]
 *   - [tower of despair], [ancient dungeon]
 *   - [powerwar dungeon], [blood dungeon]
 *   - [defense dungeon], [dimension dungeon]
 *
 * ⚠ Remaining sections skipped with warnings (119+ sections):
 *   - Additional monster spawning (20+ sections)
 *   - Additional dungeon types (5+ sections)
 *   - Rewards/drops (18+ sections)
 *   - Restrictions (15+ sections)
 *   - Special mechanics (35+ sections)
 *   - Difficulty variations (20+ sections)
 *
 * Original function: 5,755 lines with 150+ sections and nested blocks
 * Current function: ~520 lines with 31 sections (20% of original)
 *
 * @param[out] dungeonScript Pointer to STDungeonScript to populate
 * @param[in]  filename      Script filename (relative to g_szDungeonScriptBaseDirectory)
 * @return 1 on success, 0 on failure
 *
 * @note Uses global variables:
 *       - g_szDungeonScriptBaseDirectory : Base directory for dungeon scripts
 *       - g_stDungeonScriptFileList      : Global registry of all dungeon scripts
 *
 * @see loadRDARScriptFile() in lexer/rdar_loader.h
 * @see ScanType(), ScanInt(), ScanStr() in parser/scan_utils.h
 */
int ImportDungeonScript(STDungeonScript* dungeonScript, const char* filename);

#endif // DUNGEON_PARSER_H
