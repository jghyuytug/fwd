#!/bin/bash
#
# add_missing_types.sh - Add forward declarations for missing types
#
# This script adds forward declarations for frequently missing types:
# - ENUM_INTERNALPACKET_JPN
# - STQuickPartyPoolMap_Key
# - RandomBuffType
# - nexon::cash namespace
# - Arad_InterfaceGameScript
# - ENUM_DB_HANDLE_IDX
# - ENUM_GLOBAL_EFFECT_ARAD
# - StackablePackage
# - ENUM_HACKTYPE
# - map_monster, map_item
#
# Expected error reduction: 50K-80K errors
#

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
SRC_DIR="$PROJECT_ROOT/src_split"
DECL_FILE="$SRC_DIR/df_game_r_decl.h"
BACKUP_DIR="$PROJECT_ROOT/backups"

echo -e "${GREEN}=== Add Missing Type Declarations ===${NC}"
echo "Target: $DECL_FILE"
echo ""

# Create backup
TIMESTAMP=$(date +%Y%m%d_%H%M%S)
mkdir -p "$BACKUP_DIR"
BACKUP_FILE="$BACKUP_DIR/df_game_r_decl_before_types_${TIMESTAMP}.h"
cp "$DECL_FILE" "$BACKUP_FILE"
echo -e "${GREEN}✓ Backup created: $BACKUP_FILE${NC}"

# Find insertion point (after StringUnorderedMap declaration)
LINE_NUM=$(grep -n "template<typename HashTrait, typename ValueType> class StringUnorderedMap" "$DECL_FILE" | cut -d: -f1 | head -1)

if [ -z "$LINE_NUM" ]; then
    # Fallback: insert after includes
    LINE_NUM=$(grep -n "^#include" "$DECL_FILE" | tail -1 | cut -d: -f1)
    ((LINE_NUM++))
fi

((LINE_NUM += 2))  # Skip blank line

echo "Inserting type declarations after line $LINE_NUM"

# Create temporary file with new declarations
TEMP_FILE=$(mktemp)

cat > "$TEMP_FILE" << 'EOF'

// ============================================================================
// Forward declarations for missing types (auto-generated)
// ============================================================================

// Enums
enum ENUM_INTERNALPACKET_JPN;
enum ENUM_DB_HANDLE_IDX;
enum ENUM_GLOBAL_EFFECT_ARAD;
enum ENUM_CMDPACKET;
enum SECURITY_PROTCTION;

// Structs
struct map_monster;
struct map_item;
struct StackablePackage;
struct InstanceRentalSystem;

// Namespaces
namespace nexon {
    namespace cash {
        class Transaction;
    }
}

namespace ARAD {
    enum ENUM_INTERNALPACKET_JPN;
    class Arad_InterfaceGameScript;
    namespace SCRIPT {
        namespace EmblemCompound {
            struct Result;
        }
    }
}

namespace QuickParty {
    struct STQuickPartyPoolMap_Key;
    enum RandomBuffType;
    class CQuickParty;
}

namespace WongWork {
    enum ENUM_HACKTYPE;
    class CDeathTower;
    class CBossTower;
    class CHackAnalyzer;
}

namespace EventClassify {
    class CEventEntity;
}

namespace advancealtar {
    class StageControl;
    struct _Stage;
    struct _Slot;
    struct _SlotItem;
    struct _Reward;
    struct _BuyItem;
    struct _MapMonsterSummary;
    struct _SummonObjectMs;
    struct _StarRestFeeByGold;
}

namespace TaiwanInternalPack {
    enum T;
}

namespace HeroMissionCondition {
    namespace MissionNo {
        enum T;
    }
}

namespace RestrictCategory {
    enum Enum;
}

namespace ItemUpgradeType {
    enum T;
}

namespace fair_pvp {
    struct JobKey;
}

namespace ServerParameterScript {
    struct PvPPlayerKey;
}

namespace StackableAction {
    struct input_param;
}

namespace LevelupSupportSciprt {
    struct ItemInfo;
}

namespace LevelupSupport2ndScript {
    struct ItemInfo;
}

namespace createChracScript {
    struct rewardInfoList;
}

namespace useCharacterJobFatigue {
    struct fatigueMatchGift;
}

namespace CharacterCreateCreatureGiftScript {
    struct eventJob;
}

namespace ExtreamDungeon {
    struct forceRule_ResultItem;
}

namespace user_creature {
    class CCreatureItem;
    class CCreatureScript;
}

namespace game_master {
    class CCommand;
}

namespace exchange_server {
    struct ExchangeServerInfo;
    class CEpoll;
}

namespace disconnect_detecter {
    struct DetectedObjectKey;
}

namespace pvp_assault {
    class CAssaultPlace;
}

namespace DBMgr {
    struct stUserDBInfo_t;
}

// Classes
class CUser;
class CParty;
class CDungeon;
class CMap;
class CMonster;
class CSkill;
class CItem;
class CWorldMap;
class Quest;
class PvP_Room;
class WarRoom;
class BlueMarble;
class CAICharacter;
class MySQL;
class CFLog;
class CCeraShopGoods;
class CAdvertisementExposeInfo;
class IRentalMachine;
class CBattle_Field;
class IMatch;
class CWaitingRoomList;
class InterfaceServicePolicy;
class CItemLimitEdition;
class BaseHeroMissionCondition;
class TaiwanInterfaceDBDispatcher;
class TaiwanInterfaceInterDispatcher;
class ChattingEmoticonBase;
class CerashopAddRestrict;
class CLocalChinaErrorDispatcher;

// More structs
struct ItemShopScript;
struct BaseItemValue;
struct ItemFarmingData;
struct STExpertJobScript;
struct STPremiumItemData;
struct STDBConnInfo;
struct STAdvanceAltarEntranceData;
struct STAdvanceAltarLayerStatData;
struct STBossDungeonTowerLog;
struct STUltimateRewardProb;
struct STRewardDataKey;
struct STRewardDataList;
struct STPacketLog;
struct STGuildProxy;
struct STEnchantSystemMapKey;
struct STEnchantSystemMapData;
struct STHellPartyGroupOrder;
struct SIG_LETTER_INFO;
struct SIG_BEST_CLEAR_TIME_DIFFICULTY;
struct CACHE_CHARACTER_TYPE;
struct CGMRecord;
struct FAIR_PVP_SCORE_MODE_COUNT;
struct PvPMapInfo;
struct TournamentRewardRate;
struct TournamentMonster;
struct RewardUserData;
struct LevelUpRewardItemTableKey;
struct LevelUpRewardItemData;
struct GenuineDamageUpgradeMaterialInfo;
struct GenuineDamageUpgradeInfo;
struct InGameEventInfo;
struct Condition_LevelUp;
struct DimensionCoinInfo;
struct SlotBoundKey;
struct SlotBound;
struct itemData;
struct rewardKey;
struct MapInfo;
struct PrePayInfo;
struct UseCoinDungeonDiff;
struct Skill;
struct JobParty;
struct Party;
struct Monster;
struct CDungeonJobNode;
struct DungeonKey;
struct CDungeonNode;
struct JobKey;
struct CSuccessJobNode;
struct HeroMissionValue;
struct AradServerStateMessage;
struct AradBonusItem;
struct Arad_GiftItem_Set;
struct PurchaseLimitItem;
struct stDeathTower_t;
struct stHellPartyMonsterGroupList;
struct stClearTimeAverageMapKey;
struct stClearTimeAverage;
struct stRecipeInfo;
struct stConditionEventRewardInfo;
struct stAchievement;
struct stTitleElement;
struct stUpgradeItemTicketParam_t;
struct stItemLockInfo;
struct stInvestAmplifyOption_t;
struct stStackableBoosterItemInfo_t;
struct STStackablePackageData;
struct DungeonRidableObject;
struct pvp_channel_info_t;
struct RandomList;
struct LevelUpSkill;

// APSystem namespace
namespace APSystem {
    class CActionPointEx;
}

// StageMapList namespace
namespace StageMapList {
    struct _StageMapSummary;
}

// AvatarVariation namespace
namespace AvatarVariation {
    struct colorRGB;
}

// ============================================================================
// End of forward declarations
// ============================================================================

EOF

# Insert the declarations
sed -i "${LINE_NUM}r $TEMP_FILE" "$DECL_FILE"
rm "$TEMP_FILE"

echo -e "${GREEN}✓ Type declarations inserted at line $LINE_NUM${NC}"

echo ""
echo -e "${GREEN}=== Fix Complete ===${NC}"
echo ""
echo "Added forward declarations for ~150+ types"
echo "Expected error reduction: 50K-80K errors"
echo ""
echo "To verify:"
echo "  cd $SRC_DIR"
echo "  make clean"
echo "  make -j4 2>&1 | tee ../compile_after_types.log"
echo ""
echo "To rollback:"
echo "  cp $BACKUP_FILE $DECL_FILE"
