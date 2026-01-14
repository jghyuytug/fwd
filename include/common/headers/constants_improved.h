/*
 * constants_improved.h - 游戏常量定义（改进版）
 *
 * 基于 constants.h 自动提取，经过人工分析和重命名
 *
 * 主要改进：
 * - 高频常量有了有意义的名称
 * - 添加了详细的用途说明
 * - 按功能分组
 */

#ifndef CONSTANTS_IMPROVED_H
#define CONSTANTS_IMPROVED_H

//=============================================================================
// Flex/Bison 词法分析器常量
//
// 服务器使用 Flex/Bison 解析配置文件或脚本语言
// 这些是自动生成的状态机表格的特殊值
//=============================================================================

// Flex yy_def[] 数组的默认填充值
// 使用次数: 10,396 (极高频！)
// 出现在: __int16 yy_def[10000] 数组中
#define FLEX_YY_DEF_DEFAULT              9992

// Flex yy_base[] 数组的最大索引/基础值
// 使用次数: 1,736
// 出现在: __int16 yy_base[10000] 数组中
#define FLEX_YY_BASE_MAX                 10084

// Flex yy_nxt[] 和 yy_chk[] 数组的大小
// 这两个数组大小为 10288，而其他表格是 10000
#define FLEX_YY_NXT_CHK_SIZE             10288

// Flex yy_accept[], yy_base[], yy_def[] 数组的大小
#define FLEX_YY_STATE_TABLE_SIZE         10000

//=============================================================================
// 时间相关常量
//=============================================================================

// 一天的秒数 (24 × 60 × 60)
// 使用次数: 113
// 用途: 时间计算、日期差、定时器
// 示例: v6 = timer - 86400; (减去一天)
// 示例: v27 = 86400 * a5 + CSystemTime::getCurSec(...); (计算N天后的时间)
#define SECONDS_PER_DAY                  86400

//=============================================================================
// CUser 结构体偏移量
//
// CUser 是超大结构体 (498KB+)，包含完整的用户数据
// 这些偏移量用于访问结构体内的特定字段
//=============================================================================

// CUser::charac_info_list 偏移量
// 使用次数: 345
// 类型: std::vector<_Charac_info>
// 用途: 存储该账号下所有角色的信息列表
// 示例: std::vector<_Charac_info>::size((_DWORD *)user + 124346)
#define CUSER_OFFSET_CHARAC_LIST         124346

// CUser::history_log (位置1) 偏移量
// 使用次数: 228
// 类型: cUserHistoryLog*
// 用途: 用户历史日志记录器（事件币、物品分离等）
// 示例: cUserHistoryLog::EventCoinAdd(user + 124352, ...)
#define CUSER_OFFSET_HISTORY_LOG_ALT     124352

// CUser::history_log (位置2) 偏移量
// 使用次数: 76
// 类型: cUserHistoryLog*
// 用途: 用户历史日志记录器（邮件、IP计数等）
// 示例: cUserHistoryLog::SendMail((CUser **)(user + 497408), ...)
// 注意: 与 124352 不同，可能是不同类型的 log 或结构体演化残留
#define CUSER_OFFSET_HISTORY_LOG         497408

//=============================================================================
// 网络架构常量
//=============================================================================

// 网络缓冲区大小 (4KB)
// 使用次数: 268
// 用于: CNetwork<4096, 450000> 模板参数
// 说明: 每个连接的数据包缓冲区大小
#define NETWORK_BUFFER_SIZE_4K           4096

// 网络缓冲区大小 (6KB)
// 用于: CNetwork<6144, 100000> 模板参数
// 说明: 较大的缓冲区配置
#define NETWORK_BUFFER_SIZE_6K           6144

// 最大网络连接数 (45万)
// 使用次数: 213
// 用于: CNetwork<4096, 450000> 模板参数
// 说明: 服务器支持的最大并发连接数
#define MAX_NETWORK_CONNECTIONS_450K     450000

// 最大网络连接数 (10万)
// 使用次数: 334
// 用于: CNetwork<6144, 100000> 模板参数
// 说明: 另一种网络配置的连接数上限
#define MAX_NETWORK_CONNECTIONS_100K     100000

//=============================================================================
// 内存和缓冲区大小
//=============================================================================

// 1KB 缓冲区
// 使用次数: 297
// 用途: ss_sprintf<1024u> 等字符串格式化
#define BUFFER_SIZE_1K                   1024

// 静态池大小
// 使用次数: 829
// 用途: StaticPool<CACHE_CHARACTER_TYPE, 1000>
#define STATIC_POOL_SIZE_1000            1000

//=============================================================================
// 无效值和魔术标记
//=============================================================================

// 无效ID (16位无符号最大值)
// 使用次数: 1,146 (超高频！)
// 用途: 表示无效、未初始化或特殊状态
// 示例: if ( a1 == 1 && a2 == 0xFFFF ) (检查无效ID)
// 示例: __static_initialization_and_destruction_0(1, 0xFFFF) (C++静态初始化)
#define INVALID_ID_U16                   0xFFFF

// 无效值 (32位全1)
// 使用次数: 390
// 用途: 表示 -1 或无效指针/索引
#define INVALID_VALUE_U32                0xFFFFFFFF

// INT_MAX (32位有符号最大值)
// 使用次数: 189
// 用途: 数值上限、溢出保护
#define MAX_INT32                        0x7FFFFFFF

// 大数值限制
// 使用次数: 161
// 用途: 某种数值上限 (约10亿)
#define MAX_VALUE_1B                     0x3FFFFFFF

//=============================================================================
// 位掩码和标志
//=============================================================================

// 字节掩码 (8位全1)
// 使用次数: 223
// 用途: 位运算、范围检查
#define MASK_BYTE                        0xFF

// 7位掩码
// 使用次数: 188
// 用途: 位操作、哈希计算
// 示例: v3 = a1 & 0x7F ^ FI(...)[a1 >> 7]
#define MASK_7BIT                        0x7F

// 6位掩码 (63)
// 使用次数: 444
// 用途: v9 = v12 & 0x3F
#define MASK_6BIT                        0x3F

// 5位掩码 (31)
// 使用次数: 238
// 用途: 循环上限 for ( j = 0; j <= 0x1F; ++j )
#define MASK_5BIT                        0x1F

//=============================================================================
// 常见标志位 (2的幂次)
//=============================================================================

// 16字节 / 128位
// 使用次数: 785
// 用途: operator new(0x10u) 内存分配
#define SIZE_16_BYTES                    0x10

// 32字节
// 使用次数: 488
// 用途: memcmp(s1, s2, 0x20u) 内存比较
#define SIZE_32_BYTES                    0x20

// 64字节
// 使用次数: 182
// 用途: memcpy(*(this + 48), src, 0x40u)
#define SIZE_64_BYTES                    0x40

// 128字节标志位
// 使用次数: 392
// 用途: *((_BYTE *)this + na + 40) = 0x80
#define FLAG_BIT7                        0x80

// 256字节 / 256元素
// 使用次数: 162
// 用途: strncpy(dest, filename, 0x100u)
#define SIZE_256_BYTES                   0x100

// 512字节 / 512元素
// 使用次数: 182
// 用途: return 0x200 / __size
#define SIZE_512_BYTES                   0x200

//=============================================================================
// 其他重要常量
//=============================================================================

// 20字节结构体/数据块大小
// 使用次数: 480
// 用途: memcpy(dest, (char *)this + 96, 0x14u)
#define SIZE_20_BYTES                    0x14

// CBingo 对象大小 (28字节)
// 使用次数: 195
// 用途: v1 = (CBingo *)operator new(0x1Cu)
#define SIZEOF_CBINGO                    0x1C

// 30字节字符串限制
// 使用次数: 180
// 用途: strncpy(v16 + 80, CurCharacName, 0x1Eu)
#define MAX_CHARAC_NAME_LEN              0x1E

// XOR 加密/哈希常量
// 使用次数: 328
// 用途: *a1 ^= a1[2] ^ a1[1] ^ a1[3] ^ 0x18
#define XOR_CONSTANT_24                  0x18

// 范围检查边界 (93)
// 使用次数: 289
// 用途: if ( (unsigned __int8)(v9 + 95) > 0x5Du ... )
#define RANGE_CHECK_93                   0x5D

// 魔术值 (148)
// 使用次数: 178
// 用途: (const char *)0x94
#define MAGIC_VALUE_148                  0x94

//=============================================================================
// 数值限制
//=============================================================================

// 10000 上限
// 使用次数: 162
// 用途: boost::CV::constrained_value 约束值
#define MAX_LIMIT_10000                  10000

//=============================================================================
// 兼容性宏 - 保持与自动生成版本的兼容
//
// 如果需要使用原来的通用名称，可以取消下面的注释
//=============================================================================

/*
// 原通用名称映射
#define CONST_9992          FLEX_YY_DEF_DEFAULT
#define CONST_10084         FLEX_YY_BASE_MAX
#define CONST_86400         SECONDS_PER_DAY
#define SIZE_124346         CUSER_OFFSET_CHARAC_LIST
#define CONST_124352        CUSER_OFFSET_HISTORY_LOG_ALT
#define COUNT_497408        CUSER_OFFSET_HISTORY_LOG
#define COUNT_450000        MAX_NETWORK_CONNECTIONS_450K
#define COUNT_4096          NETWORK_BUFFER_SIZE_4K
#define CONST_1024          BUFFER_SIZE_1K
#define SIZE_1000           STATIC_POOL_SIZE_1000
#define CONST_0XFFFF        INVALID_ID_U16
#define CONST_0XFFFFFFFF    INVALID_VALUE_U32
#define CONST_0X7FFFFFFF    MAX_INT32
#define CONST_0X3FFFFFFF    MAX_VALUE_1B
#define CONST_0XFF          MASK_BYTE
#define CONST_0X7F          MASK_7BIT
#define CONST_0X3F          MASK_6BIT
#define CONST_0X1F          MASK_5BIT
#define FLAG_0X10           SIZE_16_BYTES
#define FLAG_0X20           SIZE_32_BYTES
#define FLAG_0X40           SIZE_64_BYTES
#define FLAG_0X80           FLAG_BIT7
#define FLAG_0X100          SIZE_256_BYTES
#define FLAG_0X200          SIZE_512_BYTES
#define CONST_0X14          SIZE_20_BYTES
#define CONST_0X1C          SIZEOF_CBINGO
#define MASK_0X1E           MAX_CHARAC_NAME_LEN
#define CONST_0X18          XOR_CONSTANT_24
#define CONST_0X5D          RANGE_CHECK_93
#define CONST_0X94          MAGIC_VALUE_148
#define MAX_10000           MAX_LIMIT_10000
*/

#endif /* CONSTANTS_IMPROVED_H */
