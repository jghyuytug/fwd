/*
 * CUser.h - CUser 结构体定义
 *
 * 基于 df_game_r_v3.c 的偏移量分析恢复
 * 总大小: ~584,754+ bytes (约 571 KB)
 *
 * 生成日期: 2025年
 * 数据来源: Phase 4 结构体恢复分析
 */

#ifndef CUSER_H
#define CUSER_H

#include <vector>
#include <string>

// 前向声明
class CUserCharacInfo;
class cUserHistoryLog;
class CAccountCargo;
class CUserPremium;
class CHackAnalyzer;
class CCharacterView;

// _Charac_info 结构体（简化定义）
struct _Charac_info {
    uint8_t data[124];  // 实际大小未知，占位
};

/*
 * CUser - 用户/账号对象
 *
 * 这是游戏中最大的结构体之一，包含完整的账号数据
 */
class CUser
{
public:
    //=========================================================================
    // 基础区域 (0-124345 bytes)
    //=========================================================================

    uint8_t _unknown_0[16];  // 偏移 0-15: 未知数据

    CUserCharacInfo* current_charac_ptr;  // 偏移 16: 当前角色指针

    uint8_t _unknown_20[124326];  // 偏移 20-124345: 未知数据

    //=========================================================================
    // 角色列表区域 (124346-124351 bytes)
    //=========================================================================

    std::vector<_Charac_info> charac_list;  // 偏移 124346: 角色信息列表
                                              // std::vector 占用 24 bytes (指针+大小+容量)

    //=========================================================================
    // 历史日志指针 (124370-124373 bytes)
    //=========================================================================

    cUserHistoryLog* history_log_alt;  // 偏移 124352: 历史日志记录器(备用)

    uint8_t _unknown_124356[330296];  // 偏移 124356-454651: 未知数据

    //=========================================================================
    // 账号货仓区域 (454652-459663 bytes)
    //=========================================================================

    CAccountCargo account_cargo;  // 偏移 454652: 账号货仓
                                   // 大小: 约 5012 bytes

    //=========================================================================
    // 账号标识区域 (459664-459975 bytes)
    //=========================================================================

    uint8_t unknown_flag_459664;  // 偏移 459664: 未知标志

    uint8_t _padding_459665[275];  // 偏移 459665-459939: padding

    int16_t uid_high;  // 偏移 459940: UID 高16位
    int16_t uid_low;   // 偏移 459942: UID 低16位 (unique_id)

    uint8_t _padding_459944[4];  // 偏移 459944-459947: padding

    uint32_t acc_id;  // 偏移 459948: 账号ID

    char acc_name[22];  // 偏移 459952: 账号名称 (估计大小)

    char user_name[128];  // 偏移 459974: 用户显示名 (估计大小)

    char ssn_string[128];  // 偏移 460102: SSN/身份证号 (估计大小)

    char user_email[128];  // 偏移 460230: 邮箱地址 (估计大小)

    uint8_t _unknown_460358[2962];  // 偏移 460358-463319: 未知数据

    //=========================================================================
    // GM和权限区域 (463320-463391 bytes)
    //=========================================================================

    uint8_t is_gm_mode;  // 偏移 463320: GM 模式标志

    uint8_t _padding_463321[7];  // 偏移 463321-463327: padding

    uint32_t age;  // 偏移 463328: 年龄

    uint8_t _unknown_463332[56];  // 偏移 463332-463387: 未知数据

    CUserPremium premium_info;  // 偏移 463388: 会员信息
                                 // 大小: 约 2436 bytes

    //=========================================================================
    // 反作弊区域 (465824-497199 bytes)
    //=========================================================================

    uint32_t account_last_play_time;  // 偏移 465824: 账号最后游戏时间

    uint8_t unknown_flag_465832;  // 偏移 465832: 未知标志

    uint8_t _padding_465833[3];  // 偏移 465833-465835: padding

    CHackAnalyzer hack_analyzer;  // 偏移 465836: 反作弊分析器
                                   // 大小: 约 31364 bytes

    //=========================================================================
    // 服务器信息区域 (497200-497407 bytes)
    //=========================================================================

    uint32_t server_group;  // 偏移 497200: 服务器组

    uint8_t _unknown_497204[28];  // 偏移 497204-497231: 未知数据

    uint32_t pc_room_no;  // 偏移 497232: 网吧编号

    uint8_t _unknown_497236[160];  // 偏移 497236-497395: 未知数据

    CCharacterView character_view;  // 偏移 497396: 角色外观
                                     // 大小: 约 12 bytes

    cUserHistoryLog* history_log;  // 偏移 497408: 历史日志记录器(主)

    //=========================================================================
    // 任务数据区域 (497412-577363 bytes)
    //=========================================================================

    uint8_t _unknown_497412[49616];  // 偏移 497412-547027: 未知数据

    char cur_charac_quest_w[30336];  // 偏移 547028: 当前角色任务写缓冲
                                      // 大小: 估计

    //=========================================================================
    // 游戏状态区域 (577364-578015 bytes)
    //=========================================================================

    uint32_t unknown_577364;  // 偏移 577364: 未知字段

    uint8_t _unknown_577368[108];  // 偏移 577368-577475: 未知数据

    uint32_t unknown_577476;  // 偏移 577476: 状态字段(get_state)

    uint8_t _unknown_577480[52];  // 偏移 577480-577531: 未知数据

    int16_t pos_x;  // 偏移 577532: X 坐标
    int16_t pos_y;  // 偏移 577534: Y 坐标

    uint8_t direction;  // 偏移 577536: 朝向

    uint8_t _padding_577537;  // 偏移 577537: padding

    int16_t in_trade_flag;  // 偏移 577538: 交易中标志(bit 15)
    int16_t in_party_flag;  // 偏移 577540: 组队中标志(bit 15)

    uint8_t _unknown_577542[6];  // 偏移 577542-577547: 未知数据

    int16_t blue_marble_index;  // 偏移 577548: 蓝色弹珠索引

    uint8_t _unknown_577550[4];  // 偏移 577550-577553: 未知数据

    int16_t advance_altar_index;  // 偏移 577554: 高级祭坛索引

    uint8_t _unknown_577556[8];  // 偏移 577556-577563: 未知数据

    uint32_t cera_amount;  // 偏移 577564: 点券数量

    uint8_t _unknown_577568[149];  // 偏移 577568-577716: 未知数据

    uint8_t unknown_flag_577717;  // 偏移 577717: 未知标志

    uint8_t _unknown_577718[4];  // 偏移 577718-577721: 未知数据

    uint32_t some_counter_577722;  // 偏移 577722: 某种计数器

    uint8_t unknown_flag_577726;  // 偏移 577726: 未知标志

    uint8_t _unknown_577727[53];  // 偏移 577727-577779: 未知数据

    uint32_t last_login_charac_no;  // 偏移 577780: 最后登录角色编号
    uint32_t last_login_channel_no;  // 偏移 577784: 最后登录频道编号
    uint8_t is_valid_last_login;    // 偏移 577788: 最后登录有效性

    uint8_t _unknown_577789[171];  // 偏移 577789-577959: 未知数据

    uint8_t unknown_flag_577960;  // 偏移 577960: 未知标志
    uint8_t unknown_flag_577961;  // 偏移 577961: 未知标志

    uint8_t _unknown_577962[50];  // 偏移 577962-578011: 未知数据

    uint32_t revival_time;  // 偏移 578012: 复活时间

    //=========================================================================
    // 扩展数据区域 (578016-584753 bytes)
    //=========================================================================

    uint8_t _unknown_578016[5868];  // 偏移 578016-583883: 未知数据

    uint32_t unknown_583884;  // 偏移 583884: 未知字段

    uint8_t _unknown_583888[104];  // 偏移 583888-583991: 未知数据

    uint32_t unknown_583992;  // 偏移 583992: 可设置的值

    uint8_t _unknown_583996[484];  // 偏移 583996-584479: 未知数据

    uint8_t unknown_flag_584480;  // 偏移 584480: 未知标志

    uint8_t _unknown_584481[127];  // 偏移 584481-584607: 未知数据

    uint32_t total_pc_room_play_time;  // 偏移 584608: 网吧总游戏时间
    uint8_t pc_room_play_time_reward_cnt;  // 偏移 584612: 网吧时间奖励计数

    uint8_t _unknown_584613[7];  // 偏移 584613-584619: 未知数据

    uint32_t cur_charac_used_gift_fatigue;  // 偏移 584620: 当前角色已用礼物疲劳

    uint8_t unknown_flag_584624;  // 偏移 584624: 未知标志

    uint8_t _unknown_584625[119];  // 偏移 584625-584743: 未知数据

    uint8_t unknown_flag_584744;  // 偏移 584744: 未知标志(初始化为-1)

    uint8_t _unknown_584745[8];  // 偏移 584745-584752: 未知数据

    uint8_t unknown_flag_584753;  // 偏移 584753: 未知标志
    uint8_t unknown_value_584754;  // 偏移 584754: 发送到包中的值

    //=========================================================================
    // 剩余未知区域
    //=========================================================================
    // 注: 结构体可能还有更多字段，总大小可能超过 584,754 bytes

public:
    // 关键方法声明（从代码中推断）
    uint32_t GetUID() const;  // 返回 (uid_high << 16) | uid_low
    uint32_t get_acc_id() const;
    const char* get_acc_name() const;
    const char* GetUserName() const;
    const char* GetSsnString() const;
    const char* GetUserEMail() const;
    bool IsGameMasterMode() const;
    CUserPremium* GetPremiumInfo();
    CHackAnalyzer* getHackAnalyzer();
    uint32_t GetServerGroup() const;
    uint32_t GetPCRoomNo() const;
    cUserHistoryLog* GetHistoryLog();
    int16_t get_posX() const;
    int16_t get_posY() const;
    uint8_t get_direction() const;
    bool CheckInTrade() const;
    bool CheckInParty() const;
    uint32_t GetCera() const;
    uint32_t GetAge() const;
    uint32_t GetRevivalTime() const;
    void SetRevivalTime(uint32_t time);
    uint32_t getTotalPcRoomPlayTime() const;
    uint32_t GetCurCharacUsedGiftFatigueQuantity() const;
};

#endif /* CUSER_H */
