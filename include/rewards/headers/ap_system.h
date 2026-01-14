#ifndef REWARDS_AP_SYSTEM_H
#define REWARDS_AP_SYSTEM_H



// Forward declarations
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

#endif // REWARDS_AP_SYSTEM_H
