/**
 * @file pvf_manager.h
 * @brief PVF数据管理器 - 服务端集成接口
 *
 * @details 提供服务端初始化、访问和清理PVF数据的统一接口
 *          封装了libpvf.a的底层操作
 *
 * @date 2025-11-25
 */

#ifndef PVF_MANAGER_H
#define PVF_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

/* ========================================================================== */
/* 返回值定义                                                                  */
/* ========================================================================== */

#define PVF_MGR_SUCCESS              0
#define PVF_MGR_ERROR_NOT_INIT      -1
#define PVF_MGR_ERROR_ALREADY_INIT  -2
#define PVF_MGR_ERROR_FILE_NOT_FOUND -3
#define PVF_MGR_ERROR_LOAD_FAILED   -4
#define PVF_MGR_ERROR_INVALID_PARAM -5

/* ========================================================================== */
/* 初始化/清理接口                                                             */
/* ========================================================================== */

/**
 * @brief 初始化PVF管理器
 *
 * @param pvf_path       Script.pvf文件路径
 * @param stringtable_path stringtable.bin文件路径 (可选，NULL则不加载)
 *
 * @return int PVF_MGR_SUCCESS=成功, 其他=错误码
 *
 * @note 必须在使用任何其他PVF函数前调用
 *
 * @example
 *     int ret = PVF_Manager_Initialize("./Script.pvf", "./stringtable.bin");
 *     if (ret != PVF_MGR_SUCCESS) {
 *         fprintf(stderr, "PVF init failed: %d\n", ret);
 *     }
 */
int PVF_Manager_Initialize(const char* pvf_path, const char* stringtable_path);

/**
 * @brief 清理PVF管理器
 *
 * @note 释放所有PVF相关资源
 */
void PVF_Manager_Cleanup(void);

/**
 * @brief 检查PVF管理器是否已初始化
 *
 * @return int 1=已初始化, 0=未初始化
 */
int PVF_Manager_IsInitialized(void);

/* ========================================================================== */
/* 状态查询接口                                                                */
/* ========================================================================== */

/**
 * @brief 获取PVF文件总数
 *
 * @return uint32_t 文件数量，0表示未初始化
 */
uint32_t PVF_Manager_GetFileCount(void);

/**
 * @brief 获取PVF版本号
 *
 * @return uint32_t 版本号，0表示未初始化
 */
uint32_t PVF_Manager_GetRevision(void);

/* ========================================================================== */
/* 文件访问接口                                                                */
/* ========================================================================== */

/**
 * @brief 检查文件是否存在于PVF中
 *
 * @param filepath  PVF内部路径（如"equipment/sword.equ"）
 *
 * @return int 1=存在, 0=不存在或未初始化
 */
int PVF_Manager_FileExists(const char* filepath);

/**
 * @brief 获取文件大小
 *
 * @param filepath  PVF内部路径
 *
 * @return int32_t 文件大小（字节），-1=不存在或错误
 */
int32_t PVF_Manager_GetFileSize(const char* filepath);

/**
 * @brief 读取文件内容
 *
 * @param filepath  PVF内部路径
 * @param buffer    输出缓冲区（调用者负责释放）
 * @param size      输出：实际读取的大小
 *
 * @return int PVF_MGR_SUCCESS=成功, 其他=错误码
 *
 * @note buffer由函数分配，调用者需调用free()释放
 *
 * @example
 *     unsigned char* data;
 *     size_t size;
 *     if (PVF_Manager_ReadFile("equipment/sword.equ", &data, &size) == PVF_MGR_SUCCESS) {
 *         // 使用data
 *         free(data);
 *     }
 */
int PVF_Manager_ReadFile(const char* filepath, unsigned char** buffer, size_t* size);

/* ========================================================================== */
/* 数据解析接口                                                                */
/* ========================================================================== */

/**
 * @brief 解析装备数据
 *
 * @param filepath  装备文件路径（如"equipment/xxx.equ"）
 * @param out_data  输出：装备数据结构（参见equipment_parser.h）
 *
 * @return int PVF_MGR_SUCCESS=成功, 其他=错误码
 */
int PVF_Manager_ParseEquipment(const char* filepath, void* out_data);

/* ========================================================================== */
/* StringTable访问接口                                                         */
/* ========================================================================== */

/**
 * @brief 根据索引获取StringTable字符串
 *
 * @param index  字符串索引
 *
 * @return const char* 字符串指针，NULL=未找到或StringTable未加载
 */
const char* PVF_Manager_GetString(uint32_t index);

/**
 * @brief 根据索引获取标签名
 *
 * @param index     字符串索引
 * @param tag_buf   输出缓冲区
 * @param buf_size  缓冲区大小
 *
 * @return int 标签长度，负数=错误
 */
int PVF_Manager_GetTag(uint32_t index, char* tag_buf, size_t buf_size);

/* ========================================================================== */
/* 诊断接口                                                                    */
/* ========================================================================== */

/**
 * @brief 打印PVF状态信息
 *
 * @details 输出到stdout：
 *          - 文件路径
 *          - 版本号
 *          - 文件数量
 *          - StringTable状态
 */
void PVF_Manager_PrintStatus(void);

#ifdef __cplusplus
}
#endif

#endif /* PVF_MANAGER_H */
