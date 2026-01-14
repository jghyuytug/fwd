/*
 * error_codes.h - 游戏错误码定义
 *
 * 自动从 df_game_r_v2.c 提取
 * 基于代码中的负数返回值和赋值语句
 */

#ifndef ERROR_CODES_H
#define ERROR_CODES_H

//=============================================================================
// 标准错误码定义（用于模块接口）
//=============================================================================

#define ERR_SUCCESS                0    // 操作成功
#define ERR_FAILURE               -1    // 一般性失败
#define ERR_INVALID_PARAMETER     -2    // 无效参数
#define ERR_INVALID_STATE         -3    // 无效状态
#define ERR_OUT_OF_MEMORY         -4    // 内存不足
#define ERR_NOT_INITIALIZED       -5    // 模块未初始化
#define ERR_ALREADY_INITIALIZED   -6    // 模块已初始化
#define ERR_NOT_FOUND             -7    // 未找到
#define ERR_TIMEOUT               -8    // 超时
#define ERR_NETWORK_ERROR         -9    // 网络错误
#define ERR_CONNECTION_FAILED    -10    // 连接失败
#define ERR_DISCONNECTED         -11    // 连接断开
#define ERR_BUFFER_OVERFLOW      -12    // 缓冲区溢出
#define ERR_BUFFER_UNDERFLOW     -13    // 缓冲区下溢
#define ERR_PERMISSION_DENIED    -14    // 权限不足
#define ERR_NOT_IMPLEMENTED      -15    // 功能未实现

//=============================================================================
// 游戏特定错误码定义
//
// 约定：
// - 成功: 0 或正数
// - 错误: 负数
// - 通用错误: -1 到 -99
// - 系统错误: -100 到 -199
// - 业务错误: -200 到 -999
//=============================================================================

//-----------------------------------------------------------------------------
// 通用错误 (-1 到 -99)
//-----------------------------------------------------------------------------

// 使用次数: 1
// 示例: return -99;
#define ERR_CODE_99                              -99

// 使用次数: 3
// 示例: return -98;
// 函数: DbmwClient::Connect, CTradeSpace::remove_item, CTradeSpace::regist_item
#define ERR_CODE_98                              -98

// 使用次数: 1
// 示例: return -97;
// 函数: DbmwClient::Connect
#define ERR_CODE_97                              -97

// 使用次数: 1
// 示例: return -96;
// 函数: DbmwClient::Connect
#define ERR_CODE_96                              -96

// 使用次数: 4
// 示例: return -95;
// 函数: DbmwClient::Connect
#define ERR_CODE_95                              -95

// 使用次数: 5
// 示例: v24 = -92;
#define ERR_CODE_92                              -92

// 使用次数: 1
// 示例: v24 = -91;
#define ERR_CODE_91                              -91

// 使用次数: 1
// 示例: v24 = -90;
#define ERR_CODE_90                              -90

// 使用次数: 1
// 示例: v20 = -89;
#define ERR_CODE_89                              -89

// 使用次数: 1
// 示例: v24 = -88;
#define ERR_CODE_88                              -88

// 使用次数: 2
// 示例: *(_BYTE *)this = -86;
// 函数: nexon_packet::stHeader::stHeader
#define ERR_CODE_86                              -86

// 使用次数: 2
// 示例: v24 = -85;
#define ERR_CODE_85                              -85

// 使用次数: 1
// 示例: v20 = -84;
#define ERR_CODE_84                              -84

// 使用次数: 1
// 示例: v24 = -83;
#define ERR_CODE_83                              -83

// 使用次数: 3
// 示例: v20 = -82;
#define ERR_CODE_82                              -82

// 使用次数: 1
// 示例: v24 = -80;
#define ERR_CODE_80                              -80

// 使用次数: 1
// 示例: v24 = -77;
#define ERR_CODE_77                              -77

// 使用次数: 1
// 示例: v24 = -74;
#define ERR_CODE_74                              -74

// 使用次数: 1
// 示例: v24 = -71;
#define ERR_CODE_71                              -71

// 使用次数: 3
// 示例: if ( v9 == -70 )
#define ERR_CODE_70                              -70

// 使用次数: 16
// 示例: if ( v9 == -69 && v10[0] == 0xD8 && v10[1] == 0xC3 )
// 函数: TiXmlAttribute::Parse, TiXmlElement::ReadValue
#define ERR_CODE_69                              -69

// 使用次数: 2
// 示例: v20 = -68;
#define ERR_CODE_68                              -68

// 使用次数: 12
// 示例: v24 = -66;
#define ERR_CODE_66                              -66

// 使用次数: 32
// 示例: if ( v5[2] == -65 )
// 函数: TiXmlAttribute::Parse, TiXmlElement::ReadValue
#define ERR_CODE_65                              -65

// 使用次数: 2
// 示例: LOBYTE(v14) = -64;
#define ERR_CODE_64                              -64

// 使用次数: 6
// 示例: if ( v6[6382] == -63 )
// 函数: Inter_GuildCargoPopItem::dispatch_sig, Inter_GuildCargoPushItem::dispatch_sig, Inter_GuildCargoMoveItem::dispatch_sig
#define ERR_CODE_63                              -63

// 使用次数: 1
// 示例: v24 = -62;
#define ERR_CODE_62                              -62

// 使用次数: 4
// 示例: v20 = -61;
#define ERR_CODE_61                              -61

// 使用次数: 2
// 示例: v24 = -60;
#define ERR_CODE_60                              -60

// 使用次数: 3
// 示例: v24 = -59;
#define ERR_CODE_59                              -59

// 使用次数: 8
// 示例: v24 = -58;
#define ERR_CODE_58                              -58

// 使用次数: 4
// 示例: *((_BYTE *)a5 + 16) = -56;
#define ERR_CODE_56                              -56

// 使用次数: 3
// 示例: else if ( a2 == -55 )
// 函数: CGameManager::Send_userinfos_to_upper_server
#define ERR_CODE_55                              -55

// 使用次数: 2
// 示例: if ( a2 == -53 )
// 函数: Inter_TcpServerConnect::dispatch_sig
#define ERR_CODE_53                              -53

// 使用次数: 2
// 示例: if ( src[n] != -52 )
#define ERR_CODE_52                              -52

// 使用次数: 1
// 示例: *((_DWORD *)this + 6) = -50;
// 函数: CreatureScript::clear
#define ERR_CODE_50                              -50

// 使用次数: 1
// 示例: v20 = -48;
#define ERR_CODE_48                              -48

// 使用次数: 1
// 示例: v20 = -45;
#define ERR_CODE_45                              -45

// 使用次数: 1
// 示例: v20 = -44;
#define ERR_CODE_44                              -44

// 使用次数: 1
// 示例: v14 = -43;
#define ERR_CODE_43                              -43

// 使用次数: 1
// 示例: v20 = -41;
#define ERR_CODE_41                              -41

// 使用次数: 11
// 示例: && (v3 != -32 || (unsigned __int8)v5 > 0x9Fu) )
#define ERR_CODE_32                              -32

// 使用次数: 3
// 示例: v10 = -24;
#define ERR_CODE_24                              -24

// 使用次数: 1
// 示例: v20 = -22;
#define ERR_CODE_22                              -22

// 使用次数: 1
// 示例: v5 = -20;
#define ERR_CODE_20                              -20

// 使用次数: 14
// 示例: v5 = -17;
// 函数: cUserHistoryLog::SendMail, TiXmlAttribute::Parse, TiXmlElement::ReadValue
#define ERR_CODE_17                              -17

// 使用次数: 3
// 示例: v5 = -16;
#define ERR_CODE_16                              -16

// 使用次数: 2
// 示例: v5 = -15;
#define ERR_CODE_15                              -15

// 使用次数: 2
// 示例: v5 = -14;
#define ERR_CODE_14                              -14

// 使用次数: 1
// 示例: v5 = -13;
#define ERR_CODE_13                              -13

// 使用次数: 5
// 示例: if ( v117 == -12 )
#define ERR_CODE_12                              -12

// 使用次数: 2
// 示例: v5 = -11;
#define ERR_CODE_11                              -11

// 使用次数: 28
// 示例: return -10;
// 函数: CInventory::tryInsertInvenItemIntoQuickSlot, CCargo::tryInsertItemSpecialSlot, CCargo::insert_item_special_slot
#define ERR_CODE_10                              -10

// 使用次数: 7
// 示例: v5 = -8;
#define ERR_CODE_8                               -8

// 使用次数: 6
// 示例: return -7;
#define ERR_CODE_7                               -7

// 使用次数: 11
// 示例: return -6;
// 函数: yaSSL_CTX_load_verify_locations, CUser::SetMercenaryInfoPacket
#define ERR_CODE_6                               -6

// 使用次数: 21
// 示例: return -5;
// 函数: SkillSlot::try_refund_skill, Dispatcher_BlueMarble::check_error, SkillSlot::refund_skill
#define ERR_CODE_5                               -5

// 使用次数: 53
// 示例: return -4;
// 函数: SkillSlot::refund_skill, CInventory::tryStackInvenItemIntoCorrespondingSpace, CInventory::tryStackInvenItemIntoQuickSlot
#define ERR_CODE_4                               -4

// 使用次数: 62
// 示例: if ( v5 == -3 )
// 函数: reseal_table_t::get_oneshot_reseal_cost, Dispatcher_SaveCharacterOption::check_error, Dispatcher_SaveQuestNotify::check_error
#define ERR_CODE_3                               -3

// 使用次数: 279
// 示例: return -2;
// 函数: CCargo::insert_item_special_slot, CParty::join_user, ARAD::SCRIPT::EmblemCompound::loadScript
#define ERR_CODE_2                               -2

// 使用次数: 5752
// 示例: int std::string::npos = -1; // weak
// 函数: Dispatcher_RequestCharacSkillInfo::check_error, EquipmentParameterInfo::EquipmentParameterInfo, CBattle_Field::GetCurrentMapInfo
#define ERR_CODE_1                               -1

//-----------------------------------------------------------------------------
// 系统错误 (-100 到 -199)
//-----------------------------------------------------------------------------

// 使用次数: 1
// 示例: if ( Out != -162 && *(_WORD *)(Out + 160) )
// 函数: DB_MailBoxSendNewMailWithDBWork::dispatch
#define ERR_CODE_162                             -162

// 使用次数: 1
// 示例: return -128;
// 函数: stAmplifyOption_t::getAbilityType
#define ERR_CODE_128                             -128

// 使用次数: 1
// 示例: if ( v14 == 42 && v13 == -122 )
#define ERR_CODE_122                             -122

// 使用次数: 2
// 示例: *((_BYTE *)v59 + 5) = -120;
#define ERR_CODE_120                             -120

// 使用次数: 1
// 示例: *(_BYTE *)(v4[2] + v62 + 1) = -117;
#define ERR_CODE_117                             -117

// 使用次数: 4
// 示例: if ( *v4 != -114 )
#define ERR_CODE_114                             -114

// 使用次数: 6
// 示例: if ( v5 == -113 )
#define ERR_CODE_113                             -113

// 使用次数: 1
// 示例: result = -105;
#define ERR_CODE_105                             -105

// 使用次数: 14
// 示例: result = -104;
#define ERR_CODE_104                             -104

// 使用次数: 15
// 示例: v5 = -103;
#define ERR_CODE_103                             -103

// 使用次数: 40
// 示例: v5 = -102;
#define ERR_CODE_102                             -102

// 使用次数: 33
// 示例: result = -101;
#define ERR_CODE_101                             -101

// 使用次数: 8
// 示例: return -100;
// 函数: DbmwClient::Init, TrainingQuestScript::getTrainingQuest
#define ERR_CODE_100                             -100

//-----------------------------------------------------------------------------
// 业务错误 (-200 到 -499)
//-----------------------------------------------------------------------------

// 使用次数: 1
// 示例: v5 = -417;
#define ERR_CODE_417                             -417

// 使用次数: 1
// 示例: v5 = -362;
#define ERR_CODE_362                             -362

// 使用次数: 1
// 示例: return -339;
#define ERR_CODE_339                             -339

// 使用次数: 2
// 示例: *((_WORD *)this + 10) = -200;
// 函数: WongWork::CCalcCharacStat::InitializeStat
#define ERR_CODE_200                             -200

//-----------------------------------------------------------------------------
// 其他错误 (-500+)
//-----------------------------------------------------------------------------

// 使用次数: 1
// 示例: v20 = -570;
#define ERR_CODE_570                             -570


#endif /* ERROR_CODES_H */
