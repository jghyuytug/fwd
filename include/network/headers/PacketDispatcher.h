#ifndef __PACKET_DISPATCHER_H__
#define __PACKET_DISPATCHER_H__

#include <stdint.h>
#include "common/defs.h"

/* Forward declarations */
typedef struct PacketBuf PacketBuf;

/**
 * PacketHandler - 数据包处理器函数类型
 *
 * @param fd: 客户端socket文件描述符
 * @param packet: 已解析的数据包（包含头部和payload）
 * @return: 0=成功，负数=错误（会关闭连接）
 *
 * 说明：
 * - 处理器函数在网络线程中同步执行
 * - 应该快速完成，避免阻塞事件循环
 * - 复杂的业务逻辑应该移到工作线程池（Week 5）
 * - packet->data指向完整数据包（从头部开始）
 * - 使用PacketBuf_get系列函数读取payload
 */
typedef int (*PacketHandler)(int fd, PacketBuf *packet);

/**
 * PacketHandlerInfo - 数据包处理器注册信息
 */
typedef struct PacketHandlerInfo {
    uint16_t packet_type;        // 数据包类型
    PacketHandler handler;       // 处理器函数指针
    const char *name;            // 处理器名称（用于日志）
    int enabled;                 // 是否启用（1=启用，0=禁用）
} PacketHandlerInfo;

/* ========================================================================
 * PacketDispatcher API - 数据包分发器接口
 * ======================================================================== */

/**
 * PacketDispatcher_Initialize - 初始化数据包分发器
 *
 * @return: 0=成功，负数=失败
 *
 * 必须在使用任何其他PacketDispatcher函数之前调用。
 * 初始化内部注册表，注册默认处理器。
 */
int PacketDispatcher_Initialize(void);

/**
 * PacketDispatcher_Cleanup - 清理数据包分发器
 *
 * 释放所有资源，清空注册表。
 */
void PacketDispatcher_Cleanup(void);

/**
 * PacketDispatcher_Register - 注册数据包处理器
 *
 * @param packet_type: 数据包类型（0-65535）
 * @param handler: 处理器函数指针
 * @param name: 处理器名称（可选，用于日志）
 * @return: 0=成功，负数=失败
 *
 * 说明：
 * - 如果packet_type已存在处理器，会覆盖旧的处理器
 * - name参数可以为NULL（会使用默认名称）
 * - handler不能为NULL
 */
int PacketDispatcher_Register(uint16_t packet_type, PacketHandler handler, const char *name);

/**
 * PacketDispatcher_Unregister - 注销数据包处理器
 *
 * @param packet_type: 数据包类型
 * @return: 0=成功，负数=未找到
 */
int PacketDispatcher_Unregister(uint16_t packet_type);

/**
 * PacketDispatcher_Dispatch - 分发数据包到对应处理器
 *
 * @param fd: 客户端socket文件描述符
 * @param packet: 已解析的数据包
 * @return: 0=成功，负数=错误（会关闭连接）
 *
 * 说明：
 * - 根据packet头部的type字段查找处理器
 * - 如果未找到处理器，记录警告并返回成功（忽略未知包）
 * - 如果处理器返回负数，连接会被关闭
 */
int PacketDispatcher_Dispatch(int fd, PacketBuf *packet);

/**
 * PacketDispatcher_GetHandlerInfo - 获取处理器信息
 *
 * @param packet_type: 数据包类型
 * @return: 处理器信息指针，未找到返回NULL
 *
 * 返回的指针指向内部结构，不应该被修改或释放。
 */
const PacketHandlerInfo* PacketDispatcher_GetHandlerInfo(uint16_t packet_type);

/**
 * PacketDispatcher_EnableHandler - 启用/禁用处理器
 *
 * @param packet_type: 数据包类型
 * @param enabled: 1=启用，0=禁用
 * @return: 0=成功，负数=未找到
 *
 * 禁用的处理器不会被调用（Dispatch会忽略该类型的包）。
 */
int PacketDispatcher_EnableHandler(uint16_t packet_type, int enabled);

/**
 * PacketDispatcher_GetStatistics - 获取统计信息
 *
 * @param total_handlers: 输出总注册处理器数量
 * @param enabled_handlers: 输出启用的处理器数量
 * @param total_dispatched: 输出总分发次数
 * @param unknown_packets: 输出未知包数量
 */
void PacketDispatcher_GetStatistics(int *total_handlers, int *enabled_handlers,
                                    unsigned long *total_dispatched, unsigned long *unknown_packets);

/**
 * PacketDispatcher_PrintRegistry - 打印注册表（调试用）
 *
 * 打印所有已注册的处理器信息到stdout。
 */
void PacketDispatcher_PrintRegistry(void);

/* ========================================================================
 * 预定义的数据包类型常量（DNF协议）
 * ======================================================================== */

/* 连接和认证 (0-99) */
#define PACKET_TYPE_HANDSHAKE           1      // 握手请求
#define PACKET_TYPE_AUTH_REQUEST        2      // 认证请求（登录）
#define PACKET_TYPE_AUTH_RESPONSE       3      // 认证响应
#define PACKET_TYPE_HEARTBEAT           10     // 心跳包

/* 角色管理 (100-199) */
#define PACKET_TYPE_CHAR_LIST_REQUEST   100    // 请求角色列表
#define PACKET_TYPE_CHAR_LIST_RESPONSE  101    // 角色列表响应
#define PACKET_TYPE_CHAR_CREATE         102    // 创建角色
#define PACKET_TYPE_CHAR_DELETE         103    // 删除角色
#define PACKET_TYPE_CHAR_SELECT         104    // 选择角色

/* 移动和位置 (200-299) */
#define PACKET_TYPE_MOVE_REQUEST        200    // 移动请求
#define PACKET_TYPE_MOVE_SYNC           201    // 位置同步
#define PACKET_TYPE_TELEPORT            202    // 传送

/* 战斗和技能 (300-399) */
#define PACKET_TYPE_ATTACK              300    // 攻击
#define PACKET_TYPE_SKILL_USE           301    // 使用技能
#define PACKET_TYPE_DAMAGE_NOTIFY       302    // 伤害通知

/* 聊天和社交 (400-499) */
#define PACKET_TYPE_CHAT_MESSAGE        400    // 聊天消息
#define PACKET_TYPE_WHISPER             401    // 私聊

/* 物品和背包 (500-599) */
#define PACKET_TYPE_ITEM_USE            500    // 使用物品
#define PACKET_TYPE_ITEM_MOVE           501    // 移动物品
#define PACKET_TYPE_ITEM_DROP           502    // 丢弃物品

/* 测试用 (65000+) */
#define PACKET_TYPE_ECHO_TEST           65000  // Echo测试包

#endif // __PACKET_DISPATCHER_H__
