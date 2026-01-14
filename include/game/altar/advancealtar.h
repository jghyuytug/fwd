#ifndef GAME_ALTAR_ADVANCEALTAR_H
#define GAME_ALTAR_ADVANCEALTAR_H

#include "altar_types.h"

// Forward declarations
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

#endif // GAME_ALTAR_ADVANCEALTAR_H
