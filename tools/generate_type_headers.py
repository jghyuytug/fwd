#!/usr/bin/env python3
"""
Generate Type Definition Headers
Automatically creates C++ header files for all 63 missing types
Based on PHASE1_TYPE_CLASSIFICATION.md and REMAINING_TYPES_ANALYSIS.md
"""

import os
from pathlib import Path

# Base directory
BASE_DIR = Path(__file__).parent.parent
INCLUDE_DIR = BASE_DIR / "include"

# Header template
HEADER_TEMPLATE = """#ifndef {guard}
#define {guard}

{includes}

{content}

#endif // {guard}
"""

# Type definitions organized by header file
HEADERS = {
    # ==================== NETWORK LAYER ====================
    "network/packet_types.h": {
        "guard": "NETWORK_PACKET_TYPES_H",
        "includes": ["<cstdint>"],
        "content": """// Client command packet types
// Confidence: 95% - Found explicit string table in Phase 2
// Evidence: part05.c lines 94501-94550
enum ENUM_CMDPACKET {
    // Connection & Login (0-9)
    ENUM_CMDPACKET_CHECK_CONNECTION = 0,
    ENUM_CMDPACKET_LOGIN = 1,
    ENUM_CMDPACKET_SET_UDP_IP_PORT = 2,
    ENUM_CMDPACKET_EXIT = 3,
    ENUM_CMDPACKET_SELECT_CHARACTER = 4,
    ENUM_CMDPACKET_CREATE_CHARACTER = 5,
    ENUM_CMDPACKET_DELETE_CHARACTER = 6,
    ENUM_CMDPACKET_RETURN_SELECT_CHARACTER = 7,
    ENUM_CMDPACKET_GET_USERINFO = 8,
    ENUM_CMDPACKET_RECOVER_STAMINA = 9,

    // Party & Peer (10-14)
    ENUM_CMDPACKET_REQUEST_PEER = 10,
    ENUM_CMDPACKET_RESPONSE_PEER = 11,
    ENUM_CMDPACKET_SET_PARTY_INFO = 12,
    ENUM_CMDPACKET_LEAVE_PARTY = 13,
    ENUM_CMDPACKET_WALKOUT_PARTY_MEMBER = 14,

    // Game Start & Dungeon (15-17)
    ENUM_CMDPACKET_START_GAME = 15,
    ENUM_CMDPACKET_SELECT_DUNGEON = 16,
    ENUM_CMDPACKET_SEND_MESSAGE = 17,

    // Inventory (18-29)
    ENUM_CMDPACKET_DELETE_ITEM = 18,
    ENUM_CMDPACKET_MOVE_ITEMSPACE = 19,
    ENUM_CMDPACKET_SORT_ITEM = 20,
    ENUM_CMDPACKET_BUY_ITEM = 21,
    ENUM_CMDPACKET_RENT_AVATAR = 22,
    ENUM_CMDPACKET_EXTEND_AVATAR = 23,
    ENUM_CMDPACKET_SELL_ITEM = 24,
    ENUM_CMDPACKET_REPAIR_EQUIPMENT = 25,
    ENUM_CMDPACKET_SET_ITEMTRADE_STATE = 26,
    ENUM_CMDPACKET_COMPOUND_ITEM = 27,
    ENUM_CMDPACKET_DISJOINT_ITEM = 28,
    ENUM_CMDPACKET_USE_LOTTERY_ITEM = 29,

    // Skills & Character (30-32)
    ENUM_CMDPACKET_CHANGE_SKILLSLOT = 30,
    ENUM_CMDPACKET_BUY_SKILL = 31,
    ENUM_CMDPACKET_INCREASE_STATUS = 32,

    // Quests (33-36)
    ENUM_CMDPACKET_ACCEPT_QUEST = 33,
    ENUM_CMDPACKET_GIVEUP_QUEST = 34,
    ENUM_CMDPACKET_SET_QUEST_TRIGGER = 35,
    ENUM_CMDPACKET_FINISH_QUEST = 36,

    // Gameplay (37-49)
    ENUM_CMDPACKET_SET_USER_POSITION = 37,
    ENUM_CMDPACKET_SET_USER_AREA = 38,
    ENUM_CMDPACKET_FISHING = 39,
    ENUM_CMDPACKET_FINISH_LOADING = 40,
    ENUM_CMDPACKET_USE_SKILL = 41,
    ENUM_CMDPACKET_DIE_MONSTER = 42,
    ENUM_CMDPACKET_DIE_CHARACTER = 43,
    ENUM_CMDPACKET_USE_COIN = 44,
    ENUM_CMDPACKET_GIVEUP_GAME = 45,
    ENUM_CMDPACKET_GET_ITEM = 46,
    ENUM_CMDPACKET_USE_STACKABLE = 47,
    ENUM_CMDPACKET_MOVE_MAP = 48,
    ENUM_CMDPACKET_SET_PLAY_RESULT = 49,

    // TODO: Extract remaining values (continuation beyond line 94550)
};

// Global server effect types
// Confidence: 90% - Clear usage in std::map
// Evidence: Used as key in std::map<ENUM_GLOBAL_EFFECT_ARAD, AradServerStateMessage>
enum ENUM_GLOBAL_EFFECT_ARAD {
    // TODO: Extract exact values from usage patterns
    // Examples (speculative):
    // GLOBAL_EFFECT_EXP_BOOST = 0,
    // GLOBAL_EFFECT_DROP_RATE = 1,
    // GLOBAL_EFFECT_EVENT_BONUS = 2,
};

// Japan region internal packets (placeholder)
// Confidence: 40% - No direct evidence found
enum ENUM_INTERNALPACKET_JPN {
    // TODO: Extract from usage if needed
};
"""
    },

    # ==================== DATABASE LAYER ====================
    "database/db_enums.h": {
        "guard": "DATABASE_DB_ENUMS_H",
        "includes": [],
        "content": """// Database connection handle indices
// Confidence: 85% - Clear usage in nested std::map
// Evidence: std::map<ENUM_SERVER_GROUP, std::map<ENUM_DB_HANDLE_IDX, STDBConnInfo>>
enum ENUM_DB_HANDLE_IDX {
    DB_HANDLE_ACCOUNT = 0,      // Account database
    DB_HANDLE_CHARACTER = 1,    // Character database
    DB_HANDLE_GAME = 2,         // Game state database
    DB_HANDLE_LOG = 3,          // Logging database
    DB_HANDLE_CASH = 4,         // Cash shop database
    // TODO: Extract remaining values from actual usage
};

// Server group enumeration
// Confidence: 85% - Used as map key for DB connections
enum ENUM_SERVER_GROUP {
    SERVER_GROUP_1 = 0,
    SERVER_GROUP_2 = 1,
    // TODO: Extract exact server group values
};
"""
    },

    "database/db_types.h": {
        "guard": "DATABASE_DB_TYPES_H",
        "includes": ["<ctime>", '"db_enums.h"'],
        "content": """// Database connection information
// Confidence: 70% - Inferred from usage pattern
struct STDBConnInfo {
    char host[256];
    int port;
    char username[64];
    char password[64];
    char database[64];
    // TODO: Verify exact field layout
};

// Bonus item table structure
// Confidence: 75% - Common pattern in DNF
// Error count: 996
struct AradBonusItem {
    int itemId;
    int count;
    int bonusType;
    int duration;
    // TODO: Extract exact fields from usage
};
"""
    },

    # ==================== GAME/ALTAR ====================
    "game/altar/altar_types.h": {
        "guard": "GAME_ALTAR_ALTAR_TYPES_H",
        "includes": ["<ctime>"],
        "content": """// Forward declarations
class CUser;

// Advance Altar entrance data
// Confidence: 80% - Clear usage pattern
// Error count: 1,248
struct STAdvanceAltarEntranceData {
    int altarId;
    int requiredLevel;
    int entryFee;
    int maxPartySize;
    time_t openTime;
    time_t closeTime;
    // TODO: Verify exact field layout from usage
};

// Advance Altar layer statistics
// Confidence: 80% - Clear usage pattern
// Error count: 1,248
struct STAdvanceAltarLayerStatData {
    int layerId;
    int completedCount;
    int bestTime;
    int bestScore;
    int clearCount;
    // TODO: Verify exact field layout from usage
};
"""
    },

    "game/altar/advancealtar.h": {
        "guard": "GAME_ALTAR_ADVANCEALTAR_H",
        "includes": ['"altar_types.h"'],
        "content": """// Forward declarations
class CUser;
struct _SummonObjectMs;

// Advanced Altar System Namespace
// Confidence: 85% - Complete structure found in Phase 2
// Error count: 9,156 (HIGHEST IMPACT - 24% of all errors!)
// Evidence: part01.c:70225-70230, part05.c global constructors
namespace advancealtar {
    // Constants (from Phase 2 analysis)
    extern int kRewardStar[3];              // {3, 2, 1}
    extern char kUnitSlotNoLock[6];         // Unit slot lock states
    extern char kSkillSlotNoLock[6];        // Skill slot lock states

    // Helper functions
    void getMemberNames(CUser *s);
    bool isOnEvent();
    bool isOpenAdvanceAltar();

    // Character data for altar runs
    class CharacData {
    public:
        CharacData();
        ~CharacData();
        int reset();
        void upGage();                          // Increase gauge
        int upgreadGage();                      // Upgrade gauge
        char setSlotItem();
        int summonSlotUnit(unsigned char slot, int unit);
        void sendTestCheckGage();
    };

    // Stage processing
    class ProcStage {
    public:
        // Nested summon object handler
        class ProcSummonObjectMs {
        public:
            ProcSummonObjectMs(CharacData *data, const int *params);
            ~ProcSummonObjectMs();
            int reset();
            bool isEndSummon();
            int insertSummonObject(_SummonObjectMs *obj);
            int getLastInsertDataIndex();
            bool completePushSummonObject(int index);
            char summonObjectAndSendPacket();
            bool debugLog();
            bool compare(const _SummonObjectMs *obj);
            static int dieMob(int a1, short a2, int a3, int a4);
        };

        ProcStage(CharacData *data);
        ~ProcStage();
        int resetAll();
        int reset(int stage);
        int startStage(int stage, int params);
    };

    // Manager class
    class Manager {
    public:
        static int gmTimeSpeed_;                // GM time speed multiplier
    };

    // Database handlers (minimal stubs)
    class DB_StatisticAdvanceAltar { };
    class DB_Update { };
    class DB_Load { };

    // Inter-server handlers (minimal stubs)
    class Inter_Load { };

    // Timer handlers (minimal stubs)
    class Timer_StageControl { };
    class Timer_MainTick { };

} // namespace advancealtar
"""
    },

    # ==================== GAME/DUNGEON ====================
    "game/dungeon/monster_types.h": {
        "guard": "GAME_DUNGEON_MONSTER_TYPES_H",
        "includes": [],
        "content": """// Monster/NPC on dungeon map
// Confidence: 85% - Complete class interface found
// Error count: 1,824
// Evidence: part13.c usage patterns, decl.h:9064-9103
class map_monster {
public:
    // Constructors/Destructors
    map_monster();
    map_monster(const map_monster& other);
    ~map_monster();

    // Operators
    map_monster& operator=(const map_monster& other);

    // Methods
    bool isNamedMonster() const;            // Check if boss/named monster

private:
    // TODO: Extract exact member layout from usage
    // Likely members:
    // - Monster ID
    // - Position on map
    // - HP/stats
    // - AI state
    // - Loot table reference
};
"""
    },

    "game/dungeon/dungeon_types.h": {
        "guard": "GAME_DUNGEON_DUNGEON_TYPES_H",
        "includes": ["<ctime>"],
        "content": """// Boss dungeon tower log
// Confidence: 75% - Common logging pattern
// Error count: 1,128
struct STBossDungeonTowerLog {
    int characterId;
    int towerId;
    int floorReached;
    int clearTime;
    int attempts;
    int bestScore;
    time_t lastAttemptTime;
    // TODO: Verify exact field layout
};

// Death tower structure
// Confidence: 50% - Speculative based on naming
// Error count: 336
struct stDeathTower_t {
    int towerId;
    int currentFloor;
    int highestFloor;
    int lives;
    // TODO: Extract from usage
};
"""
    },

    # ==================== INVENTORY ====================
    "inventory/inventory_types.h": {
        "guard": "INVENTORY_INVENTORY_TYPES_H",
        "includes": ["<map>", "<ctime>"],
        "content": """// Purchase limit tracking
// Confidence: 70%
// Error count: 996
struct PurchaseLimitItem {
    int itemId;
    int maxPurchase;
    int currentPurchase;
    time_t resetTime;
    // TODO: Verify exact fields
};

// Gift item set
// Confidence: 70%
// Error count: 996
struct Arad_GiftItem_Set {
    int setId;
    int itemIds[16];
    int itemCounts[16];
    int numItems;
    // TODO: Verify array sizes and exact layout
};

// Level-up reward table key
// Confidence: 55%
// Error count: 552
struct LevelUpRewardItemTableKey {
    int characterLevel;
    int jobId;
    int rewardTier;
    // TODO: Verify exact fields
};

// Map item container
// Confidence: 50%
// Error count: 420
struct _mapItem {
    std::map<int, int> items;  // itemId -> count
    // TODO: Verify actual structure
};

// Fatigue reward items
// Confidence: 50%
// Error count: 216
struct UsedFatigueRewardItem {
    int itemId;
    int fatigueRequired;
    bool claimed;
    // TODO: Extract from usage
};

// Item script data
// Confidence: 50%
// Error count: 144
struct STItemScript {
    int itemId;
    char scriptData[256];
    // TODO: Verify script data size
};

// Limited edition items
// Confidence: 40%
// Error count: 12
class CItemLimitEdition {
public:
    int itemId;
    int totalSupply;
    int remainingSupply;
    // TODO: Extract actual class interface
};

// Creature module type enum
// Confidence: 70% - Used in std::map
// Error count: 540
enum ENUM_MODULE_TYPE {
    MODULE_TYPE_WEAPON = 0,
    MODULE_TYPE_ARMOR = 1,
    MODULE_TYPE_ACCESSORY = 2,
    MODULE_TYPE_SKILL = 3,
    // TODO: Extract exact values
};

// Slot binding key
// Confidence: 45%
// Error count: 540
struct SlotBoundKey {
    int slotType;
    int slotIndex;
    int bindType;
    // TODO: Extract from usage
};
"""
    },

    # ==================== CHARACTER ====================
    "character/character_types.h": {
        "guard": "CHARACTER_CHARACTER_TYPES_H",
        "includes": [],
        "content": """// Character manager request result codes
// Confidence: 35% - Speculative based on naming
enum ENUM_CHARAC_MANAGER_REQUEST_SUCCESS_IDX {
    CHARAC_REQ_SUCCESS = 0,
    CHARAC_REQ_FAIL_UNKNOWN = 1,
    CHARAC_REQ_FAIL_NOT_FOUND = 2,
    CHARAC_REQ_FAIL_LOCKED = 3,
    // TODO: Extract exact values from usage
};

// Level-up condition
// Confidence: 45%
// Error count: 348
struct Condition_LevelUp {
    int requiredLevel;
    int requiredExp;
    int questId;
    // TODO: Extract from usage
};
"""
    },

    "character/character_rewards.h": {
        "guard": "CHARACTER_CHARACTER_REWARDS_H",
        "includes": [],
        "content": """// User reward data
// Confidence: 70%
// Error count: 1,140
struct RewardUserData {
    int userId;
    int characterId;
    int rewardType;
    int rewardValue;
    bool claimed;
    time_t claimTime;
    // TODO: Verify exact fields
};

// Character creation creature gift
// Confidence: 50%
// Error count: 48
struct CharacterCreateCreatureGiftScript {
    int jobId;
    int creatureId;
    char scriptPath[256];
    // TODO: Verify script path size
};
"""
    },

    # ==================== REWARDS ====================
    "rewards/reward_types.h": {
        "guard": "REWARDS_REWARD_TYPES_H",
        "includes": [],
        "content": """// Generic reward key
// Confidence: 65%
// Error count: 1,008
struct rewardKey {
    int keyType;
    int keyId;
    int keyValue;
    // TODO: Verify exact structure
};

// Level-up support scripts
// Confidence: 55%
// Error count: 684 each
struct LevelupSupportSciprt {  // Note: Original typo preserved
    int level;
    int jobId;
    char scriptPath[256];
    int rewardType;
    // TODO: Verify exact fields
};

struct STExpertJobScript {
    int expertJobId;
    char scriptPath[256];
    int unlockLevel;
    // TODO: Verify exact fields
};

struct LevelupSupport2ndScript {
    int level;
    int jobId;
    char scriptPath[256];
    int tier;
    // TODO: Verify exact fields
};

// Seria blessing script
// Confidence: 45%
// Error count: 60
struct stSeriaBlessingScript {
    int blessingLevel;
    char scriptPath[256];
    int duration;
    // TODO: Extract from usage
};
"""
    },

    "rewards/ap_system.h": {
        "guard": "REWARDS_AP_SYSTEM_H",
        "includes": [],
        "content": """// Forward declarations
class CUser;
class Stream;

// Action Point System Namespace
// Confidence: 65% - Partial structure found
// Error count: 720
// Evidence: part01.c:70189-70195, part06.c:67731-67734
namespace APSystem {
    // Global state
    extern char g_szZipAP_CLEAR_STATE[2048];    // Compressed AP clear state
    extern char g_szStrAP_CLEAR_STATE[2048];    // String AP clear state

    // Action point manager
    class CActionPointManager {
    public:
        static char _IsGMOn;                    // GM mode flag
    };

    // Inter-server message handlers (minimal stubs)
    class Inter_LoadRewardMedal {
    public:
        int dispatch_sig(CUser* user, char* data, int size);
    };

    class Inter_EmptyActionPoint {
    public:
        int dispatch_sig(CUser* user, char* data, int size);
    };

    class Inter_LoadActionPoint {
    public:
        int dispatch_sig(CUser* user, char* data, int size);
    };

    // Database handlers (minimal stubs)
    class DB_UpdateRewardMedal {
    public:
        int dispatch(int src, int dst, Stream* stream);
    };

    class DB_LoadRewardMedal {
    public:
        int dispatch(int src, int dst, Stream* stream);
    };

    class DB_UpdateActionPoint {
    public:
        int dispatch(int src, int dst, Stream* stream);
    };

    class DB_LoadActionPoint {
    public:
        int dispatch(int src, int dst, Stream* stream);
    };

    // Processing classes
    class CDBAndExchangeServerProc {
    public:
        static void RequestLoadActionPointFromDBAndES();
    };

    class CUserProc {
    public:
        static void ClearActionAndSendtoUser();
    };

} // namespace APSystem
"""
    },

    # ==================== EVENTS ====================
    "events/event_base.h": {
        "guard": "EVENTS_EVENT_BASE_H",
        "includes": [],
        "content": """// Forward declaration
class CUser;

// Base event class (abstract)
// Confidence: 70%
class CEventBase {
public:
    virtual ~CEventBase() = 0;
    virtual int StartEvent() = 0;
    virtual int EndEvent() = 0;
};

// Event classification
// Confidence: 70%
// Error count: 1,452
class EventClassify {
public:
    int eventId;
    int eventType;
    int priority;
    // TODO: Extract exact class structure
};
"""
    },

    "events/event_types.h": {
        "guard": "EVENTS_EVENT_TYPES_H",
        "includes": ['"event_base.h"', "<ctime>"],
        "content": """// Forward declaration
class CUser;

// Bingo event
// Confidence: 80% - Complete interface found in Phase 2
// Error count: 1,080
// Evidence: part01.c:68755, decl.h:1944-1956
class BingoEvent : public CEventBase {
public:
    BingoEvent();
    virtual ~BingoEvent();

    // Event lifecycle
    int StartEvent();
    static int StartAction();
    int EndEvent();
    int dailyresetData();

    // Bingo system
    char* getBingoSystem();
    int reqBingoMark(int mark);
    int reqBingoQuizMark(CUser *user, int mark);

    // Rewards
    void sendOneMatchLineReward(CUser *user);
    void sendMaxMatchLineReward(CUser *user);

private:
    // TODO: Extract member variables
};

// In-game event info
// Confidence: 70%
// Error count: 912
struct InGameEventInfo {
    int eventId;
    time_t startTime;
    time_t endTime;
    int eventType;
    int priority;
    // TODO: Verify exact fields
};

// Growth creature event
// Confidence: 50%
// Error count: 216
struct GrowthCreatureEvent {
    int eventId;
    int creatureId;
    int growthStage;
    int rewardTier;
    // TODO: Extract from usage
};
"""
    },

    "events/event_scripts.h": {
        "guard": "EVENTS_EVENT_SCRIPTS_H",
        "includes": [],
        "content": """// Event script data
// Confidence: 65%
// Error count: 744
struct InGameEventScript {
    int scriptId;
    int eventId;
    char scriptData[1024];
    // TODO: Verify script data size
};

// Upgrade jar event NPCs
// Confidence: 45%
// Error count: 120
struct ST_Event_Upgrade_Jar_EventNPCs_Script {
    int npcId;
    int jarLevel;
    char scriptPath[256];
    int rewardMultiplier;
    // TODO: Extract from usage
};
"""
    },

    # ==================== SHOP ====================
    "shop/nexon_cash.h": {
        "guard": "SHOP_NEXON_CASH_H",
        "includes": ["<cstdint>"],
        "content": """// Nexon Cash Shop Integration Namespace
// Confidence: 75% - Complete class structure found
// Error count: 996
// Evidence: decl.h.backup_round2:13459-13492
namespace nexon {
namespace cash {

    // Binary buffer for cash shop protocol
    class ByteBuffer {
    public:
        ByteBuffer& operator<<(char *str);
        ByteBuffer& operator<<(wchar_t *wstr);
        ByteBuffer& operator<<(int16_t value);
        int length() const;

    private:
        char* buffer;
        int size;
        int capacity;
    };

    // Implementation class
    class NCashImpl {
    public:
        NCashImpl(char *host, int16_t port, uint8_t protocol, uint16_t flags, int16_t timeout);
        int reconnect();

        // Protocol helpers
        NCashImpl* set_data_header(int *data, char type, char flags);
        NCashImpl* get_data_header(uint8_t *type, uint8_t *flags, unsigned int *size, uint8_t *data);

        // Error handling
        int handle_cash_error(int errorCode);
        int handle_coupon_error(int errorCode);

        // Cash operations
        int checkEntrance(char *account, char *character);
        int getBalance(char *account, int *outBalance);
        int purchase(int itemId, int count, int price, int flags, char *account, char *character, uint8_t *result);
        int gift(int itemId, int count, int price, int flags, char *sender, char *receiver, char *message, char *title, uint8_t *result);
        int confirm(char type, int transactionId);
        int cancel(char type, int transactionId);
        int chargeCash(unsigned int amount, ...);

    private:
        char host[256];
        int16_t port;
        int socket;
        ByteBuffer sendBuf;
        ByteBuffer recvBuf;
    };

    // Public wrapper class
    class NexonCash {
    public:
        NexonCash(char *host, int16_t port, char protocol, uint16_t flags, int16_t timeout);
        ~NexonCash();

        int getBalance(char *account, int *outBalance);
        int purchase(int itemId, int count, int price, int flags, char *account, char *character, uint8_t *result);
        int gift(int itemId, int count, int price, int flags, char *sender, char *receiver, char *message, char *title, uint8_t *result);
        int confirm(int transactionId);
        int cancel(int transactionId);
        int checkCoupon();
        int useCoupon();
        int checkCafeCoupon();

    private:
        NCashImpl* impl;
    };

} // namespace cash
} // namespace nexon
"""
    },

    "shop/cerashop.h": {
        "guard": "SHOP_CERASHOP_H",
        "includes": ["<map>", "<ctime>"],
        "content": """// Cash Shop Restriction System Namespace
// Confidence: 75% - Partial structure found
// Error count: 1,476
// Evidence: part24.c:22739-22978, part01.c:69901
namespace CerashopAddRestrict {
    // Helper functions
    int readTime(void* ctx);
    time_t uint32Time(void* ctx);

    // Daily restriction info
    class InfoDaily {
    public:
        InfoDaily(int type, unsigned int ipgNo,
                  time_t startTime, time_t endTime,
                  int param1, int param2);
    private:
        int restrictType;
        unsigned int ipgNo;           // Item package number
        time_t startTime;
        time_t endTime;
        // TODO: Extract additional fields
    };

    // Script loader
    class ScriptLoader {
    public:
        int LoadScript();
        int LoadScriptDaily(int index, int type);
        void ClearScript();
        bool FindIpgNo(unsigned int ipgNo);
        InfoDaily* GetRestrictInfo(unsigned int ipgNo);

    private:
        std::map<unsigned int, InfoDaily*> dailyRestrictions;
    };

    // Manager singleton
    class Manager {
    public:
        static int Load();
        static int Destroy();
    };

} // namespace CerashopAddRestrict
"""
    },

    "shop/taiwan_cash.h": {
        "guard": "SHOP_TAIWAN_CASH_H",
        "includes": ["<ctime>"],
        "content": """// Taiwan-specific cash shop packets
// Confidence: 60%
// Error count: 1,392
struct TaiwanInternalPack {
    int packetType;
    int accountId;
    int amount;
    int itemId;
    // TODO: Extract exact fields from usage
};

// Prepayment information
// Confidence: 65%
// Error count: 912
struct PrePayInfo {
    int accountId;
    int prepaidAmount;
    time_t expiryDate;
    int bonusPoints;
    // TODO: Verify exact fields
};

// Restriction categories
// Confidence: 50%
// Error count: 648
struct RestrictCategory {
    int categoryId;
    int restrictType;
    int maxDaily;
    int maxWeekly;
    // TODO: Extract from usage
};
"""
    },

    # ==================== GAME/UPGRADE ====================
    "game/upgrade/upgrade_types.h": {
        "guard": "GAME_UPGRADE_UPGRADE_TYPES_H",
        "includes": [],
        "content": """// Genuine damage upgrade material info
// Confidence: 60%
// Error count: 852
struct GenuineDamageUpgradeMaterialInfo {
    int materialId;
    int materialCount;
    int upgradeLevel;
    int successRate;
    // TODO: Extract exact fields
};

// Genuine damage upgrade info
// Confidence: 60%
// Error count: 852
struct GenuineDamageUpgradeInfo {
    int itemId;
    int currentLevel;
    int maxLevel;
    int damageIncrease;
    // TODO: Extract exact fields
};

// Investment amplify option
// Confidence: 40%
// Error count: 12
struct stInvestAmplifyOption_t {
    int optionId;
    int amplifyRate;
    int investAmount;
    // TODO: Extract from usage
};
"""
    },

    # ==================== GAME/PVP ====================
    "game/pvp/pvp_assault.h": {
        "guard": "GAME_PVP_PVP_ASSAULT_H",
        "includes": [],
        "content": """// PVP Assault Mode System Namespace
// Confidence: 75% - Partial structure found
// Error count: 996
// Evidence: part01.c:72064-72067, part06.c:67743-68118
namespace pvp_assault {
    // Constants
    extern int ASSAULT_RECT_PLAYER_POINT[16];  // Player spawn positions (x,y pairs)
    extern int ASSAULT_RECT_ICON_POINT;        // Icon position (130)

    // Static spawn disposition
    class CStaticDisposition {
    public:
        int SetVirtualMap(int mapId);
    };

    // Dynamic spawn disposition
    class CDynamicDisposition {
    public:
        int SetVirtualMap(int mapId, int flags);
    };

    // Main assault manager
    class CAssaultMgr {
    public:
        int GetRequestAssaultNeedMoney(int placeId, int level);
    private:
        static char _GetNextAssaultPlaceId_count[];
    };

    // Assault place factory
    class CAssaultPlaceFactory {
        // TODO: Extract class interface
    };

    // Global access functions
    CAssaultMgr* GetInstanceAssaultMgr();
    CAssaultPlaceFactory* GetInstanceAssaultPlaceFactory();
    int GetDirectionByTeam(int team);

} // namespace pvp_assault
"""
    },

    # ==================== UI ====================
    "ui/advertisement.h": {
        "guard": "UI_ADVERTISEMENT_H",
        "includes": ["<ctime>"],
        "content": """// Advertisement display info
// Confidence: 60%
// Error count: 1,056
class CAdvertisementExposeInfo {
public:
    int adId;
    int exposureType;
    time_t startTime;
    time_t endTime;
    char imageUrl[256];
    int priority;
    // TODO: Extract exact class interface
};
"""
    },

    "ui/visual_types.h": {
        "guard": "UI_VISUAL_TYPES_H",
        "includes": [],
        "content": """// Outline effect info
// Confidence: 40%
// Error count: 516
struct OutLineEFFECTInfoTemp {
    int effectId;
    int colorR;
    int colorG;
    int colorB;
    int thickness;
    // TODO: Extract from usage
};

// Avatar variations
// Confidence: 45%
// Error count: 324
struct AvatarVariation {
    int avatarId;
    int variationId;
    int colorScheme;
    // TODO: Extract from usage
};

// Avatar hidden options
// Confidence: 40%
// Error count: 12
struct AvatarFixedHiddenOptionServer {
    int avatarId;
    int hiddenOptionId;
    int optionValue;
    // TODO: Extract from usage
};
"""
    },

    # ==================== SERVER ====================
    "server/logging.h": {
        "guard": "SERVER_LOGGING_H",
        "includes": [],
        "content": """// Log class
// Confidence: 55%
// Error count: 984
class CFLog {
public:
    void log(const char* format, ...);
    void error(const char* format, ...);
    void warning(const char* format, ...);
    void debug(const char* format, ...);
    // TODO: Extract actual class interface
};
"""
    },

    "server/server_types.h": {
        "guard": "SERVER_SERVER_TYPES_H",
        "includes": ['"../network/packet_types.h"'],
        "content": """// Server state message
// Confidence: 45%
// Error count: 12
struct AradServerStateMessage {
    ENUM_GLOBAL_EFFECT_ARAD effectType;
    char message[256];
    int priority;
    // TODO: Verify exact structure
};

// Chat emoticon save message
// Confidence: 45%
// Error count: 228
struct MSG_SAVE_GAME_OPTION_CHATTING_EMOTICON {
    int characterId;
    int emoticonId;
    bool enabled;
    int slotIndex;
    // TODO: Extract from usage
};
"""
    },

    # ==================== EFFECTS ====================
    "effects/effect_types.h": {
        "guard": "EFFECTS_EFFECT_TYPES_H",
        "includes": ['"../network/packet_types.h"', "<ctime>"],
        "content": """// Global effect structure
// Confidence: 45%
// Error count: 180
struct stGlobalEffect_t {
    ENUM_GLOBAL_EFFECT_ARAD effectType;
    int duration;
    float value;
    time_t startTime;
    time_t endTime;
    // TODO: Verify exact fields
};
"""
    },

    # ==================== COMMON ====================
    "common/utilities.h": {
        "guard": "COMMON_UTILITIES_H",
        "includes": ["<ctime>"],
        "content": """// Map information
// Confidence: 60%
// Error count: 612
struct MapInfo {
    int mapId;
    char mapName[64];
    int width;
    int height;
    int difficulty;
    // TODO: Verify exact fields
};

// Disconnect detection
// Confidence: 40%
// Error count: 228
class disconnect_detecter {
public:
    void checkConnection();
    bool isDisconnected();
    // TODO: Extract class interface
};

// Timer entry
// Confidence: 45%
// Error count: 156
struct TimerEntry {
    int timerId;
    time_t triggerTime;
    void (*callback)();
    void* userData;
    // TODO: Verify exact structure
};

// TCHAR comparison functor
// Confidence: 50%
// Error count: 228
struct less_TCHAR {
    bool operator()(const char* a, const char* b) const;
};
"""
    },

    "common/containers.h": {
        "guard": "COMMON_CONTAINERS_H",
        "includes": ["<vector>", "<unordered_map>", "<string>"],
        "content": """// Random list container
// Confidence: 40%
// Error count: 660
template<typename T>
class RandomList {
public:
    void add(T item);
    T getRandom();
    void clear();
    size_t size() const;

private:
    std::vector<T> items;
};

// String unordered map alias
// Confidence: 60%
// Error count: 72
typedef std::unordered_map<std::string, std::string> StringUnorderedMap;

// Generic type placeholders (likely decompiler artifacts)
// Confidence: 5%
// Error count: 60 + 36 = 96
typedef void* a1;
typedef void* a2;
"""
    },

    "common/memory_pools.h": {
        "guard": "COMMON_MEMORY_POOLS_H",
        "includes": [],
        "content": """// Dynamic memory pool (grows as needed)
// Confidence: 30% - Speculative based on common pool patterns
// Error count: 468
template<typename T, size_t BlockSize = 4096>
class DynamicPool {
public:
    DynamicPool();
    ~DynamicPool();

    T* allocate();
    void deallocate(T* ptr);
    void clear();

private:
    struct Block {
        T data[BlockSize];
        Block* next;
    };

    Block* head;
    T* freeList;
    // TODO: Verify actual implementation
};

// Static memory pool (fixed size)
// Confidence: 30% - Speculative based on common pool patterns
// Error count: 84
template<typename T, size_t PoolSize>
class StaticPool {
public:
    StaticPool();
    ~StaticPool();

    T* allocate();
    void deallocate(T* ptr);
    bool isFull() const;
    size_t available() const;

private:
    T pool[PoolSize];
    bool used[PoolSize];
    size_t allocated;
    // TODO: Verify actual implementation
};
"""
    },

    # ==================== GAME/DIMENSION ====================
    "game/dimension/dimension_types.h": {
        "guard": "GAME_DIMENSION_DIMENSION_TYPES_H",
        "includes": [],
        "content": """// Dimension coin information
// Confidence: 45%
// Error count: 348
struct DimensionCoinInfo {
    int coinType;
    int amount;
    int maxAmount;
    int earnRate;
    // TODO: Extract from usage
};
"""
    },

    # ==================== GAME/SOCIAL ====================
    "game/social/social_types.h": {
        "guard": "GAME_SOCIAL_SOCIAL_TYPES_H",
        "includes": [],
        "content": """// War room
// Confidence: 30%
// Error count: 12
class WarRoom {
    // TODO: Extract class interface
};

// Trade space (auction house)
// Confidence: 35%
// Error count: 12
class CTradeSpace {
    // TODO: Extract class interface
};
"""
    },
}


def generate_headers():
    """Generate all header files"""
    print(f"Generating {len(HEADERS)} header files...")

    count = 0
    for filepath, data in HEADERS.items():
        full_path = INCLUDE_DIR / filepath
        full_path.parent.mkdir(parents=True, exist_ok=True)

        # Format includes
        includes_text = ""
        if data["includes"]:
            includes_text = "\n".join([f"#include {inc}" for inc in data["includes"]])
            includes_text += "\n\n"

        # Generate header content
        header_content = HEADER_TEMPLATE.format(
            guard=data["guard"],
            includes=includes_text.strip(),
            content=data["content"].strip()
        )

        # Write file
        with open(full_path, 'w', encoding='utf-8') as f:
            f.write(header_content)

        count += 1
        print(f"  [{count}/{len(HEADERS)}] Created {filepath}")

    print(f"\nSuccessfully generated {count} header files!")
    return count


if __name__ == "__main__":
    generated_count = generate_headers()
    print(f"\n=== Header Generation Complete ===")
    print(f"Total files generated: {generated_count}")
    print(f"Output directory: {INCLUDE_DIR}")
    print(f"\nNext steps:")
    print(f"  1. Review generated headers in {INCLUDE_DIR}")
    print(f"  2. Test compilation: cd src_split && make -j4 2>&1 | grep 'error:' | wc -l")
    print(f"  3. Expected error reduction: 49,404 → ~4,940 (90% coverage)")
