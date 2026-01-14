#ifndef SHOP_CERASHOP_H
#define SHOP_CERASHOP_H

#include <map>
#include <ctime>

// Cash Shop Restriction System Namespace
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
        int restrictType;             // Restriction category
        unsigned int ipgNo;           // Item package number
        time_t startTime;             // Restriction start time
        time_t endTime;               // Restriction end time
        int maxDaily;                 // Daily purchase limit
        int maxTotal;                 // Total purchase limit
        /* ORM: charac_cerashop_restrict table */
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

#endif // SHOP_CERASHOP_H
