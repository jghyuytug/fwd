/* Forward declarations for all missing classes */
/* Auto-generated based on compilation error analysis */
/* Total: 104 classes/structs */

#ifndef FORWARD_DECLARATIONS_H
#define FORWARD_DECLARATIONS_H

/* ========================================
 * 🎮 Game Core Classes (15)
 * ======================================== */
class CUser;
class CUserCharacInfo;
class CInventory;
class Inven_Item;
class BuffInfo;
class GameWorld;
class CreatureCommonScript;
class BestClearTime;
class TileIndexInfo;

struct charac_expand;
struct RandomOptionField;
struct RandomOptionSeed;
struct UpgradeSeparateInfo;
struct STComputeDamageVariable;
struct STEnchantedSkillOption;

/* ========================================
 * 🎉 Event System (8)
 * ======================================== */
class CEventBase;
class AttendanceEvent;
class BingoEvent;
class BingoData;
class CBingo;
class CBingoTest;
class BlueMarbleDungeonEvent;
class BlueMarble;
class BlueMarbleUserInfo;
class BoosterGageData;
class CBoosterGage;

/* ========================================
 * 🔒 Encryption & Security (24)
 * ======================================== */

// Hash/Checksum classes
class IChecksum;
class CNChecksumMd5;
class CNChecksumSha256;
class CNChecksumSha384;
class CNChecksumSha512;
class CSHA;
class CSHA1;

// Encryption algorithms
class ICryptoGraph;
class CNCryptoAnubis;
class CNCryptoBlowFish;
class CNCryptoCast5;
class CNCryptoKasumi;
class CNCryptoKhazad;
class CNCryptoMulti2;
class CNCryptoNoekeon;
class CNCryptoRc6;
class CNCryptoRijndael;
class CNCryptoShift;
class CNCryptoSkipjack;
class CNCryptoTea;
class CNCryptoTwoFish;
class CNCryptoXTea;

// Encryption utilities
class CBlowFish;
class CRijndael;
class CRijndael_fake;
class CTEA;
class Cipher;
class SimpleEncryption;
class ARIA;
class IMethod;

/* ========================================
 * 🛡️ Anti-Cheat System (5)
 * ======================================== */
namespace WongWork {
    // Forward declaration for namespace
}

class CodeHackCheckRangeEx;
class CodeHackChecksumData;
class CodeHackChecksumEx;
class CaptchaGen;

/* ========================================
 * 🌐 Network & Packets (6)
 * ======================================== */
class PacketBuf;
class InterfacePacketBuf;
class PIReceiverManager;
class PISenderManager;
class Stream;
class CStreamGuard;

/* ========================================
 * 💾 Database (5)
 * ======================================== */
class MySQL;
class DB_LoadCerashopAddRestrictType;
class DB_SaveCerashopAddRestrictType;
class Inter_LoadCerashopAddRestrictType;
// namespace CerashopAddRestrict {}  // Defined in missing_types.h as namespace

/* ========================================
 * 📄 Configuration & XML (3)
 * ======================================== */
namespace np_server_xml {
    // Forward declaration for namespace
}

class TiXmlDocument;
class TiXmlNode;

/* ========================================
 * 🔧 Utility Classes (11)
 * ======================================== */
class CharString;
class CharStringHashTrait;
class CMTRand;
class CSystemTime;
class Mutex;
class ReservedCapacity;
class SBlock;
class GreyImage;
class SlangFilter;

namespace server_utility {
    // Forward declaration for namespace
}

/* ========================================
 * 🔄 Status Synchronization (5)
 * ======================================== */
struct _SDC;
struct SDCActiveStatusSyncCommon;
struct SDCActiveStatusSyncServer;
struct STSDCActiveStatusSyncDataCLONG;
struct STSDCActiveStatusSyncDataSend;

/* ========================================
 * 🏗️ Managers & Framework (8)
 * ======================================== */
namespace ARAD {
    // Forward declaration for namespace
}

class CGameManager;
class CDataManager;
class CBusinessImpl;
class CEnvironment;
class Server_Envir;

/* ========================================
 * 📝 Script & Parsers (4)
 * ======================================== */
class FlexLexer;
class __slang_filter__FlexLexer;
class CNRDScriptData;
struct yy_buffer_state;

/* ========================================
 * 🔞 Additional Amplify Options (1)
 * ======================================== */
struct stAmplifyOption_t;
struct SIG_BEST_CLEAR_TIME_DIFFICULTY;

/* ========================================
 * 🔥 High-Frequency Missing Types (50)
 * Added from compilation error analysis
 * ======================================== */

// Memory management (3 types)
// class DynamicPool;  // Defined in missing_types.h as template
// class StaticPool;   // Defined in missing_types.h as template
template<typename T> class MemoryPool;

// Network messages (4 types)
class MSG_BASE;
class ParamBase;
class SmallStream;
class BigStream;

// Battle & Dungeon (3 types)
class CBattle_Field;
class CDungeon;
class CParty;

// Game systems (15 types)
// namespace advancealtar {}  // Defined in missing_types.h as namespace
class InstanceRentalSystem;
class user_creature;
class pvp_assault;
class LevelUpSkill;
class itemData;
namespace QuickParty {}
class MazeScript;
class ExtreamDungeon;
class PvP_Room;
// namespace APSystem {}  // Defined in missing_types.h as namespace
class DB_Login;

// Events & rewards (8 types)
class Arad_VillageAttackRewardItem;
class HeroMissionValue;
class HeroMissionCondition;
struct stConditionEventRewardInfo;
struct LevelUpRewardItemData;
struct STRewardData;
class Arad_OnePlusCardEvent_ItemList;
class Arad_Mileage_Data;

// Item & shop (5 types)
struct tagPackageItemSelection;
struct stStackableBoosterItemInfo_t;
class CCeraShopGoods;
struct STStackablePackageData;
class Arad_Script;

// Chatting & emotes (2 types)
class ChattingEmoticonBase;
class CChattingEmoticonConfigRow;

// Map & scripts (3 types)
struct _mapMonster;
struct map_item;
class LevelupSupportEventManger;

// Security & anti-cheat (2 types)
class CodeHackChecksum;
namespace nexon {}

// Data structures (8 types)
struct _DailySchedule;
struct stNotifyIngameADInfo;
struct stEVENTADDINFO;
struct SK_IP;
struct SecKillMonsterInfo;
struct STAssignBloodSystem;
struct STAssignBloodPhaseTime;
struct CHARAC_LOAD_MERCENARY;

// Enums (cannot forward declare in C++, removed)
// enum ENUM_ELEMENT;  // Cannot forward-declare enum

/* ========================================
 * 🆕 Additional Missing Types from Error Analysis
 * Added: 2025-10-29 (72 types, 36,000+ references)
 * ======================================== */

// High-frequency classes (>500 refs) - 30 types
class SECURITY_PROTCTION;  // 1,476 refs
class IRentalMachine;  // 1,164 refs
class RewardUserData;  // 1,140 refs
class MapInfo;  // 1,092 refs
class CAdvertisementExposeInfo;  // 1,056 refs
class rewardKey;  // 1,008 refs
class PurchaseLimitItem;  // 996 refs
class AradBonusItem;  // 996 refs
class Arad_GiftItem_Set;  // 996 refs
class CFLog;  // 984 refs
class InGameEventInfo;  // 912 refs
class PrePayInfo;  // 912 refs
class GenuineDamageUpgradeMaterialInfo;  // 852 refs
class GenuineDamageUpgradeInfo;  // 852 refs
class InGameEventScript;  // 744 refs
class LevelupSupportSciprt;  // 684 refs
class LevelupSupport2ndScript;  // 660 refs
class RandomList;  // 660 refs
class RestrictCategory;  // 648 refs
class LevelUpRewardItemTableKey;  // 552 refs
class SlotBoundKey;  // 540 refs
class OutLineEFFECTInfoTemp;  // 516 refs
class CGMRecord;  // 516 refs
class DungeonRidableObject;  // 492 refs

// Medium-frequency classes (200-500 refs) - 19 types
class DimensionCoinInfo;  // 348 refs
class Condition_LevelUp;  // 348 refs
class CLevelDungeonPlayStatistic;  // 348 refs
class TournamentRewardRate;  // 336 refs
class AvatarVariation;  // 324 refs
class less_TCHAR;  // 228 refs
class disconnect_detecter;  // 228 refs
class MSG_SAVE_GAME_OPTION_CHATTING_EMOTICON;  // 228 refs
class UsedFatigueRewardItem;  // 216 refs
class GrowthCreatureEvent;  // 216 refs
class secretshop;  // 216 refs
class CGlobalEffect;  // 168 refs
class TimerEntry;  // 156 refs
class CDungeonExpStatistic;  // 96 refs
class WarRoom;  // 84 refs
class CTradeSpace;  // 24 refs
class StackableAction;  // 24 refs
class CItemLimitEdition;  // 12 refs
class AvatarFixedHiddenOptionServer;  // 12 refs
class AradServerStateMessage;  // 12 refs

// High-frequency structs (>500 refs) - 11 types
struct map_monster;  // 1,824 refs
struct STAdvanceAltarEntranceData;  // 1,248 refs
struct STAdvanceAltarLayerStatData;  // 1,248 refs
struct stAchievement;  // 1,176 refs
struct STBossDungeonTowerLog;  // 1,128 refs
struct game_master;  // 900 refs
struct STExpertJobScript;  // 684 refs
struct stMonsterDamageLog_t;  // 672 refs
struct stTitleElement;  // 576 refs

// Medium-frequency structs (100-500 refs) - 12 types
struct STGuildProxy;  // 492 refs
struct _mapItem;  // 420 refs
struct stDeathTower_t;  // 336 refs
struct STPremiumItemData;  // 336 refs
struct stGlobalEffect_t;  // 180 refs
struct stTitleRewardBasis;  // 180 refs
struct STItemScript;  // 144 refs
struct ST_Event_Upgrade_Jar_EventNPCs_Script;  // 120 refs
struct stHackLog_t;  // 120 refs
struct stAmplifyOptionInfo_t;  // 120 refs
struct stInvestAmplifyOption_t;  // 96 refs
struct stSeriaBlessingScript;  // 60 refs

// Namespaces
namespace EventClassify {}  // 1,452 refs
namespace TaiwanInternalPack {}  // 1,392 refs

// Template specializations (cannot forward-declare enums)
// Note: ENUM_* types require full definition in all_generated.h

/* ========================================
 * NOTE: Some types are intentionally excluded:
 * - FILE: Standard C library type
 * - boost: External library namespace (handled separately)
 * - ENUM_* types: Cannot forward-declare enums in C++
 * ======================================== */

#endif /* FORWARD_DECLARATIONS_H */


/* Phase 5.2: Forward declarations for missing types */

// Advanced Altar System
namespace advancealtar {
    struct _SummonObjectMs;
}

// Monster Drop System
class CMonsterDrop;
class CMonsterDrop_Hell;
class CSpecialMonsterDrop;

// Reward System
class CClearReward;
