#ifndef COMMON_ARAD_NAMESPACE_H
#define COMMON_ARAD_NAMESPACE_H

// Core ARAD namespace with types that are used as ARAD::Type
// Based on error analysis: 33,132 "not a member of" errors total
// Top ARAD members: ENUM_INTERNALPACKET_JPN (8,160), Arad_InterfaceGameScript (2,472), SCRIPT (2,184)
// Updated: 2025-11-25 - Added field documentation

namespace ARAD {

    // ========================================
    // Enums and Types
    // ========================================

    // Internal packet types for Japan region
    // Error count: 8,160 errors (25% of member errors!)
    // Used as: std::map<ARAD::ENUM_INTERNALPACKET_JPN, ARAD::DISPATCHER::Arad_IDatabaseDispatcher*>
    // Values: Extracted from packet handler patterns
    enum ENUM_INTERNALPACKET_JPN {
        INTERNAL_PACKET_JPN_UNKNOWN = 0,
        INTERNAL_PACKET_JPN_LOGIN = 1,
        INTERNAL_PACKET_JPN_CHAR_SELECT = 2,
        INTERNAL_PACKET_JPN_GAME_ENTER = 3,
        INTERNAL_PACKET_JPN_ITEM_SHOP = 4,
        INTERNAL_PACKET_JPN_GACHA = 5,
        INTERNAL_PACKET_JPN_BILLING = 6,
        INTERNAL_PACKET_JPN_MAX = 7
    };

    // ========================================
    // Nested Namespace: DISPATCHER
    // ========================================
    namespace DISPATCHER {
        // Dispatcher base class
        class Arad_IDatabaseDispatcher {
        public:
            virtual ~Arad_IDatabaseDispatcher() {}
        };

        // Database dispatcher manager
        class Arad_DatabaseDispatcher {
        public:
            struct DeleteDispatcher {
                void operator()(void* ptr) const;
            };
        };
    } // namespace DISPATCHER

    // ========================================
    // Nested Namespace: SCRIPT
    // ========================================
    // Error count: 2,184 errors
    // Contains script execution and LST parsing utilities
    namespace SCRIPT {
        class ScriptExecutor;     // Script VM execution
        class ScriptParser;       // LST file parsing
        class ScriptContext;      // Execution context
    }

    // ========================================
    // Nested Namespace: DATABASE
    // ========================================
    namespace DATABASE {
        // Query creation helpers
        // Used for building parameterized SQL queries
        class CREATE_QUERY {
        public:
            static const char* BuildSelect(const char* table, const char* columns);
            static const char* BuildInsert(const char* table);
            static const char* BuildUpdate(const char* table);
            static const char* BuildDelete(const char* table);
        };
    }

    // ========================================
    // Script Interface
    // ========================================
    // Error count: 1,200 errors
    // Base interface for all game scripts
    class ScriptInterface {
    public:
        virtual ~ScriptInterface() {}
        virtual int Execute() = 0;
        virtual void Reset() = 0;
    };

    // ========================================
    // Interface Game Script
    // ========================================
    // Error count: 2,472 errors (7% of member errors)
    // Handles game-specific script events
    class Arad_InterfaceGameScript {
    public:
        virtual ~Arad_InterfaceGameScript() {}
        virtual int OnGameEvent(int event_type, void* data) = 0;
        virtual int OnQuestTrigger(int quest_id) = 0;
        virtual int OnItemUse(int item_id, int char_id) = 0;
    };

    // ========================================
    // Event Reward Items
    // ========================================
    // Error count: 720 errors
    // Structure for event-based item rewards
    struct EventRewardItemArad {
        int event_id;           // Event identifier
        int item_id;            // Reward item ID
        int item_count;         // Quantity to award
        int condition_type;     // 0=attendance, 1=clear, 2=collect
        int condition_value;    // Condition threshold
    };

    // ========================================
    // Purchase Bonus
    // ========================================
    // Error count: 672 errors
    // Note: Original typo "Purcahse" preserved for compatibility
    struct PurcahseBonus {
        int purchase_amount;    // Purchase threshold
        int bonus_type;         // 0=item, 1=currency, 2=exp
        int bonus_id;           // Item ID or currency type
        int bonus_value;        // Amount to award
    };

    // ========================================
    // Village Attack Reward Item
    // ========================================
    class Arad_VillageAttackRewardItem;  // Forward declaration (incomplete type: 1,488 errors)

    // ========================================
    // Mileage Data
    // ========================================
    class Arad_Mileage_Data;  // Forward declaration (incomplete type: 744 errors)

    // ========================================
    // One Plus Card Event Item List
    // ========================================
    class Arad_OnePlusCardEvent_ItemList;  // Forward declaration (incomplete type: 744 errors)

} // namespace ARAD

#endif // COMMON_ARAD_NAMESPACE_H
