#ifndef CHACKANALYZER_H
#define CHACKANALYZER_H

#include "common/defs.h"

/* Forward declarations for C compatibility */
typedef void CHackAnalyzer;
typedef void CUser;
typedef void CUserCharacInfo;

/**
 * tagPacketHeader - DNF网络数据包头部结构
 *
 * 数据包格式 (2025-11-11定义):
 * +----------------+----------------+
 * | length (2字节) | type (2字节)   |  4字节头部
 * +----------------+----------------+
 * |        payload data...          |  变长数据
 * +---------------------------------+
 *
 * 说明:
 * - length: 数据包总长度（包含头部）
 * - type: 数据包类型/命令字
 *
 * 所有字段使用小端序（little-endian）
 */
typedef struct tagPacketHeader {
    uint16_t length;    // 数据包总长度（包含头部，最小4字节）
    uint16_t type;      // 数据包类型/命令字
} tagPacketHeader;

/**
 * PacketBuf - 数据包缓冲区结构（扩展版）
 *
 * 用于构造和解析网络数据包的核心结构。
 * 支持序列化/反序列化、加密、压缩等功能。
 *
 * 扩展说明 (2025-11-11):
 * - 原始结构只有data和size，功能严重不足
 * - 添加capacity、read_pos、write_pos支持动态缓冲区
 * - 添加finalized、encrypted标志位跟踪状态
 * - 添加header指针快速访问数据包头
 */
typedef struct PacketBuf {
    // 原始字段（保持兼容性）
    void *data;              // 数据缓冲区指针
    int size;                // 当前数据大小（实际写入的字节数）

    // 扩展字段（2025-11-11添加）
    int capacity;            // 缓冲区总容量（分配的字节数）
    int read_pos;            // 当前读取位置（用于get操作）
    int write_pos;           // 当前写入位置（用于put操作）

    // 状态标志
    char finalized;          // 是否已完成打包（1=已完成，0=未完成）
    char encrypted;          // 是否已加密（1=已加密，0=未加密）
    char compressed;         // 是否已压缩（1=已压缩，0=未压缩）
    char _reserved;          // 保留字节（对齐到4字节边界）

    // 数据包头指针（指向data中的头部）
    void *header;            // 指向tagPacketHeader的指针
} PacketBuf;

/* CHackAnalyzer function declarations (pure C interface) */
void* __cdecl CHackAnalyzer_setUserP(void *this, CUser *user);
int __cdecl CHackAnalyzer_getUserP(void *this);
int __cdecl CHackAnalyzer_beginCollectHackInfo(void *this, CUser *user);
int __cdecl CHackAnalyzer_analyzeHackReport(void *this, CUser *user, PacketBuf *a3);
char __cdecl CHackAnalyzer_analyzeMonsterDieHack(void *this);
char __cdecl CHackAnalyzer_analyzeSpeedHack(void *this, unsigned int id);
char __cdecl CHackAnalyzer_analyzeDifficultyHack(void *this, char flag);
char __cdecl CHackAnalyzer_analyzeMemoryHack(void *this, unsigned int id);
char __cdecl CHackAnalyzer_analyzeEquipmentUpgradeHack(CUserCharacInfo **this, const uint16_t *a2);
int __cdecl CHackAnalyzer_addHackInfo(int id, char flag, __int16 a3, int value);
char __cdecl CHackAnalyzer_reportIncurrectReport(CUser **this);
int __cdecl CHackAnalyzer_reportHackInfo(void *this);
void* __cdecl CHackAnalyzer_resetHackInfo(void *this);
_BOOL4 __cdecl CHackAnalyzer_CheckExceptionCnt(int id, int value, int count);
_BOOL4 __cdecl CHackAnalyzer_CheckHackRateByPartyCount(int id, int value, int count);
_BOOL4 __cdecl CHackAnalyzer_CheckHackRateByDifficult(int id, int value, int count, unsigned int index);
int __cdecl CHackAnalyzer_AddCheckRateHackTypeList(void *this);
int __cdecl CHackAnalyzer_onUpdateHackCount(int id, CUser *user, unsigned int value, unsigned int count, unsigned int index, unsigned int type);
char __cdecl CHackAnalyzer_addServerHackCnt(void *a1, CUserCharacInfo *charac_info, unsigned int id, int value, unsigned int count, unsigned int index);
void __cdecl CHackAnalyzer_ReqDBSaveHackInfo(void *this, unsigned int id, __int16 a3, int value, int count);
void __cdecl CHackAnalyzer_ReqDBSaveHackUserPunish(void *this, unsigned int id);
char __cdecl CHackAnalyzer_ValidateFixMobDiePos(void *this, CUser *user, uint16_t a3, uint16_t a4);
int __cdecl CHackAnalyzer_setBaseHackType(int id, int value);
int __cdecl CHackAnalyzer_getBaseHackType(void *this);
int __cdecl CHackAnalyzer_checkCollectedHack(int id, int value);
int __cdecl CHackAnalyzer_getServerHackAccumulatedCnt(int id, int value);
int __cdecl CHackAnalyzer_setServerHackAccumulatedCnt(int id, int value, int count);
void* __cdecl CHackAnalyzer_setLastMonsterDeadTime(void *this, int id);
int __cdecl CHackAnalyzer_getServerHackCnt(int id, int value);
int __cdecl CHackAnalyzer_resetServerHackCnt(int id, int value);
int __cdecl CHackAnalyzer_resetServerHackAccumulatedCnt(int id, int value);
int __cdecl CHackAnalyzer_init(void *this);

#endif // CHACKANALYZER_H
