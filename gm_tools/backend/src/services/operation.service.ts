/**
 * GM操作服务
 * 处理GM操作业务逻辑，记录操作日志
 */

import * as operationModel from '../models/operation.model';
import * as gameActionModel from '../models/gameAction.model';
import * as characterModel from '../models/character.model';
import * as accountModel from '../models/account.model';
import { createApiError } from '../middlewares/errorHandler';
import { logger } from '../utils/logger.util';

/**
 * GM用户信息（从JWT中提取）
 */
interface GmUser {
  gm_id: number;
  username: string;
  gm_level: number;
}

/**
 * 物品赠送
 */
export async function grantItem(
  params: {
    char_no: number;
    item_id: number;
    amount: number;
    reason: string;
    expire_days?: number;
  },
  gmUser: GmUser,
  ipAddress: string,
  userAgent?: string
): Promise<{
  operation_id: number;
  char_no: number;
  char_name: string;
  item_id: number;
  item_name: string;
  amount: number;
  executed_at: Date;
}> {
  const { char_no, item_id, amount, reason, expire_days = 0 } = params;

  logger.info('执行物品赠送操作', { gmUser: gmUser.username, char_no, item_id, amount });

  try {
    // 1. 验证参数
    if (amount <= 0 || amount > 999) {
      throw createApiError('物品数量必须在1-999之间', 400, 'INVALID_AMOUNT');
    }

    if (expire_days < 0) {
      throw createApiError('过期天数不能为负数', 400, 'INVALID_EXPIRE_DAYS');
    }

    // 2. 检查角色是否存在
    const character = await characterModel.findCharacterByCharNo(char_no);
    if (!character) {
      throw createApiError('角色不存在', 404, 'CHARACTER_NOT_FOUND');
    }

    // 3. 检查物品是否存在
    const itemExists = await gameActionModel.checkItemExists(item_id);
    if (!itemExists) {
      throw createApiError('物品ID不存在', 404, 'ITEM_NOT_FOUND');
    }

    const itemName = await gameActionModel.getItemName(item_id) || `未知物品(${item_id})`;

    // 4. 执行物品赠送
    const grantResult = await gameActionModel.grantItemToCharacter({
      char_no,
      item_id,
      amount,
      expire_days
    });

    if (!grantResult.success) {
      // 记录失败日志
      await operationModel.createOperationLog({
        gm_user: gmUser.username,
        gm_id: gmUser.gm_id,
        gm_level: gmUser.gm_level,
        operation_type: operationModel.OperationType.ITEM_GRANT,
        operation_category: operationModel.OperationCategory.ITEM,
        target_type: operationModel.TargetType.CHARACTER,
        target_id: char_no.toString(),
        target_name: character.char_name,
        operation_detail: { item_id, item_name, amount, reason, expire_days },
        result: operationModel.OperationResult.FAILURE,
        error_message: grantResult.error,
        ip_address: ipAddress,
        user_agent: userAgent
      });

      throw createApiError(grantResult.error || '物品赠送失败', 500, 'GRANT_FAILED');
    }

    // 5. 记录成功日志
    const executedAt = new Date();
    const operationId = await operationModel.createOperationLog({
      gm_user: gmUser.username,
      gm_id: gmUser.gm_id,
      gm_level: gmUser.gm_level,
      operation_type: operationModel.OperationType.ITEM_GRANT,
      operation_category: operationModel.OperationCategory.ITEM,
      target_type: operationModel.TargetType.CHARACTER,
      target_id: char_no.toString(),
      target_name: character.char_name,
      operation_detail: {
        item_id,
        item_name,
        amount,
        reason,
        expire_days,
        slot_no: grantResult.slot_no
      },
      result: operationModel.OperationResult.SUCCESS,
      ip_address: ipAddress,
      user_agent: userAgent
    });

    logger.info('物品赠送成功', {
      operationId,
      char_no,
      item_id,
      amount
    });

    return {
      operation_id: operationId,
      char_no,
      char_name: character.char_name,
      item_id,
      item_name: itemName,
      amount,
      executed_at: executedAt
    };
  } catch (error: any) {
    logger.error('物品赠送失败', error);
    throw error;
  }
}

/**
 * 发送邮件
 */
export async function sendMail(
  params: {
    char_no: number;
    title: string;
    content: string;
    item_id?: number;
    item_amount?: number;
    gold?: string;
    expire_days?: number;
  },
  gmUser: GmUser,
  ipAddress: string,
  userAgent?: string
): Promise<{
  operation_id: number;
  mail_id: number;
  char_no: number;
  char_name: string;
  executed_at: Date;
}> {
  const { char_no, title, content, item_id, item_amount, gold, expire_days = 30 } = params;

  logger.info('执行邮件发送操作', { gmUser: gmUser.username, char_no, title });

  try {
    // 1. 检查角色是否存在
    const character = await characterModel.findCharacterByCharNo(char_no);
    if (!character) {
      throw createApiError('角色不存在', 404, 'CHARACTER_NOT_FOUND');
    }

    // 2. 验证物品（如果有）
    let itemName = null;
    if (item_id && item_id > 0) {
      const itemExists = await gameActionModel.checkItemExists(item_id);
      if (!itemExists) {
        throw createApiError('物品ID不存在', 404, 'ITEM_NOT_FOUND');
      }
      itemName = await gameActionModel.getItemName(item_id);
    }

    // 3. 发送邮件
    const mailResult = await gameActionModel.sendMailToCharacter({
      char_no,
      sender_name: `GM-${gmUser.username}`,
      title,
      content,
      item_id,
      item_amount,
      gold: gold ? BigInt(gold) : BigInt(0),
      expire_days
    });

    if (!mailResult.success) {
      // 记录失败日志
      await operationModel.createOperationLog({
        gm_user: gmUser.username,
        gm_id: gmUser.gm_id,
        gm_level: gmUser.gm_level,
        operation_type: operationModel.OperationType.MAIL_SEND,
        operation_category: operationModel.OperationCategory.MAIL,
        target_type: operationModel.TargetType.CHARACTER,
        target_id: char_no.toString(),
        target_name: character.char_name,
        operation_detail: { title, content, item_id, item_amount, gold },
        result: operationModel.OperationResult.FAILURE,
        error_message: mailResult.error,
        ip_address: ipAddress,
        user_agent: userAgent
      });

      throw createApiError(mailResult.error || '邮件发送失败', 500, 'MAIL_SEND_FAILED');
    }

    // 4. 记录成功日志
    const executedAt = new Date();
    const operationId = await operationModel.createOperationLog({
      gm_user: gmUser.username,
      gm_id: gmUser.gm_id,
      gm_level: gmUser.gm_level,
      operation_type: operationModel.OperationType.MAIL_SEND,
      operation_category: operationModel.OperationCategory.MAIL,
      target_type: operationModel.TargetType.CHARACTER,
      target_id: char_no.toString(),
      target_name: character.char_name,
      operation_detail: {
        mail_id: mailResult.mail_id,
        title,
        content,
        item_id,
        item_name: itemName,
        item_amount,
        gold
      },
      result: operationModel.OperationResult.SUCCESS,
      ip_address: ipAddress,
      user_agent: userAgent
    });

    logger.info('邮件发送成功', {
      operationId,
      mail_id: mailResult.mail_id,
      char_no
    });

    return {
      operation_id: operationId,
      mail_id: mailResult.mail_id!,
      char_no,
      char_name: character.char_name,
      executed_at: executedAt
    };
  } catch (error: any) {
    logger.error('邮件发送失败', error);
    throw error;
  }
}

/**
 * 封禁账号
 */
export async function banAccount(
  params: {
    m_id: string;
    ban_type: number;
    ban_duration: number;
    reason: string;
    evidence?: string;
  },
  gmUser: GmUser,
  ipAddress: string,
  userAgent?: string
): Promise<{
  operation_id: number;
  m_id: string;
  ban_type: number;
  ban_until?: Date;
  executed_at: Date;
}> {
  const { m_id, ban_type, ban_duration, reason, evidence } = params;

  logger.info('执行账号封禁操作', { gmUser: gmUser.username, m_id, ban_type, ban_duration });

  try {
    // 1. 验证参数
    if (![1, 2, 3].includes(ban_type)) {
      throw createApiError('封禁类型必须是1(封禁)、2(禁言)或3(警告)', 400, 'INVALID_BAN_TYPE');
    }

    if (ban_duration < 0) {
      throw createApiError('封禁时长不能为负数', 400, 'INVALID_BAN_DURATION');
    }

    // 2. 检查账号是否存在
    const account = await accountModel.findAccountById(m_id);
    if (!account) {
      throw createApiError('账号不存在', 404, 'ACCOUNT_NOT_FOUND');
    }

    // 3. 获取当前封禁状态
    const beforeState = {
      is_banned: account.is_banned,
      ban_expire_time: account.ban_expire_time
    };

    // 4. 执行封禁
    const banResult = await gameActionModel.banAccount({
      m_id,
      ban_type,
      ban_duration,
      reason,
      evidence
    });

    if (!banResult.success) {
      // 记录失败日志
      await operationModel.createOperationLog({
        gm_user: gmUser.username,
        gm_id: gmUser.gm_id,
        gm_level: gmUser.gm_level,
        operation_type: operationModel.OperationType.ACCOUNT_BAN,
        operation_category: operationModel.OperationCategory.ACCOUNT,
        target_type: operationModel.TargetType.ACCOUNT,
        target_id: m_id,
        target_name: account.account_name,
        operation_detail: { ban_type, ban_duration, reason, evidence },
        before_state: beforeState,
        result: operationModel.OperationResult.FAILURE,
        error_message: banResult.error,
        ip_address: ipAddress,
        user_agent: userAgent
      });

      throw createApiError(banResult.error || '账号封禁失败', 500, 'BAN_FAILED');
    }

    // 5. 记录成功日志
    const executedAt = new Date();
    const afterState = {
      is_banned: true,
      ban_expire_time: banResult.ban_until
    };

    const operationId = await operationModel.createOperationLog({
      gm_user: gmUser.username,
      gm_id: gmUser.gm_id,
      gm_level: gmUser.gm_level,
      operation_type: operationModel.OperationType.ACCOUNT_BAN,
      operation_category: operationModel.OperationCategory.ACCOUNT,
      target_type: operationModel.TargetType.ACCOUNT,
      target_id: m_id,
      target_name: account.account_name,
      operation_detail: { ban_type, ban_duration, reason, evidence },
      before_state: beforeState,
      after_state: afterState,
      result: operationModel.OperationResult.SUCCESS,
      ip_address: ipAddress,
      user_agent: userAgent
    });

    logger.info('账号封禁成功', {
      operationId,
      m_id,
      ban_until: banResult.ban_until
    });

    return {
      operation_id: operationId,
      m_id,
      ban_type,
      ban_until: banResult.ban_until,
      executed_at: executedAt
    };
  } catch (error: any) {
    logger.error('账号封禁失败', error);
    throw error;
  }
}

/**
 * 解封账号
 */
export async function unbanAccount(
  params: {
    m_id: string;
    reason: string;
  },
  gmUser: GmUser,
  ipAddress: string,
  userAgent?: string
): Promise<{
  operation_id: number;
  m_id: string;
  executed_at: Date;
}> {
  const { m_id, reason } = params;

  logger.info('执行账号解封操作', { gmUser: gmUser.username, m_id });

  try {
    // 1. 检查账号是否存在
    const account = await accountModel.findAccountById(m_id);
    if (!account) {
      throw createApiError('账号不存在', 404, 'ACCOUNT_NOT_FOUND');
    }

    // 2. 获取当前封禁状态
    const beforeState = {
      is_banned: account.is_banned,
      ban_expire_time: account.ban_expire_time
    };

    // 3. 执行解封
    const unbanResult = await gameActionModel.unbanAccount(m_id);

    if (!unbanResult.success) {
      // 记录失败日志
      await operationModel.createOperationLog({
        gm_user: gmUser.username,
        gm_id: gmUser.gm_id,
        gm_level: gmUser.gm_level,
        operation_type: operationModel.OperationType.ACCOUNT_UNBAN,
        operation_category: operationModel.OperationCategory.ACCOUNT,
        target_type: operationModel.TargetType.ACCOUNT,
        target_id: m_id,
        target_name: account.account_name,
        operation_detail: { reason },
        before_state: beforeState,
        result: operationModel.OperationResult.FAILURE,
        error_message: unbanResult.error,
        ip_address: ipAddress,
        user_agent: userAgent
      });

      throw createApiError(unbanResult.error || '账号解封失败', 500, 'UNBAN_FAILED');
    }

    // 4. 记录成功日志
    const executedAt = new Date();
    const afterState = {
      is_banned: false,
      ban_expire_time: null
    };

    const operationId = await operationModel.createOperationLog({
      gm_user: gmUser.username,
      gm_id: gmUser.gm_id,
      gm_level: gmUser.gm_level,
      operation_type: operationModel.OperationType.ACCOUNT_UNBAN,
      operation_category: operationModel.OperationCategory.ACCOUNT,
      target_type: operationModel.TargetType.ACCOUNT,
      target_id: m_id,
      target_name: account.account_name,
      operation_detail: { reason },
      before_state: beforeState,
      after_state: afterState,
      result: operationModel.OperationResult.SUCCESS,
      ip_address: ipAddress,
      user_agent: userAgent
    });

    logger.info('账号解封成功', { operationId, m_id });

    return {
      operation_id: operationId,
      m_id,
      executed_at: executedAt
    };
  } catch (error: any) {
    logger.error('账号解封失败', error);
    throw error;
  }
}

/**
 * 修改角色等级
 */
export async function modifyLevel(
  params: {
    char_no: number;
    new_level: number;
    reason: string;
  },
  gmUser: GmUser,
  ipAddress: string,
  userAgent?: string
): Promise<{
  operation_id: number;
  char_no: number;
  old_level: number;
  new_level: number;
  executed_at: Date;
}> {
  const { char_no, new_level, reason } = params;

  logger.info('执行等级修改操作', { gmUser: gmUser.username, char_no, new_level });

  try {
    // 1. 验证参数
    if (new_level < 1 || new_level > 100) {
      throw createApiError('等级必须在1-100之间', 400, 'INVALID_LEVEL');
    }

    // 2. 检查角色是否存在
    const character = await characterModel.findCharacterByCharNo(char_no);
    if (!character) {
      throw createApiError('角色不存在', 404, 'CHARACTER_NOT_FOUND');
    }

    const oldLevel = character.lev;

    // 3. 执行等级修改
    const modifyResult = await gameActionModel.modifyCharacterLevel({
      char_no,
      new_level
    });

    if (!modifyResult.success) {
      // 记录失败日志
      await operationModel.createOperationLog({
        gm_user: gmUser.username,
        gm_id: gmUser.gm_id,
        gm_level: gmUser.gm_level,
        operation_type: operationModel.OperationType.LEVEL_MODIFY,
        operation_category: operationModel.OperationCategory.CHARACTER,
        target_type: operationModel.TargetType.CHARACTER,
        target_id: char_no.toString(),
        target_name: character.char_name,
        operation_detail: { old_level: oldLevel, new_level, reason },
        result: operationModel.OperationResult.FAILURE,
        error_message: modifyResult.error,
        ip_address: ipAddress,
        user_agent: userAgent
      });

      throw createApiError(modifyResult.error || '等级修改失败', 500, 'LEVEL_MODIFY_FAILED');
    }

    // 4. 记录成功日志
    const executedAt = new Date();
    const operationId = await operationModel.createOperationLog({
      gm_user: gmUser.username,
      gm_id: gmUser.gm_id,
      gm_level: gmUser.gm_level,
      operation_type: operationModel.OperationType.LEVEL_MODIFY,
      operation_category: operationModel.OperationCategory.CHARACTER,
      target_type: operationModel.TargetType.CHARACTER,
      target_id: char_no.toString(),
      target_name: character.char_name,
      operation_detail: { old_level: oldLevel, new_level, reason },
      before_state: { level: oldLevel },
      after_state: { level: new_level },
      result: operationModel.OperationResult.SUCCESS,
      ip_address: ipAddress,
      user_agent: userAgent
    });

    logger.info('等级修改成功', {
      operationId,
      char_no,
      old_level: oldLevel,
      new_level
    });

    return {
      operation_id: operationId,
      char_no,
      old_level: oldLevel,
      new_level,
      executed_at: executedAt
    };
  } catch (error: any) {
    logger.error('等级修改失败', error);
    throw error;
  }
}

/**
 * 修改角色金币
 */
export async function modifyMoney(
  params: {
    char_no: number;
    gold?: string;
    silver?: string;
    reason: string;
  },
  gmUser: GmUser,
  ipAddress: string,
  userAgent?: string
): Promise<{
  operation_id: number;
  char_no: number;
  old_money: { gold: string; silver: string };
  new_money: { gold: string; silver: string };
  executed_at: Date;
}> {
  const { char_no, gold, silver, reason } = params;

  logger.info('执行金币修改操作', { gmUser: gmUser.username, char_no, gold, silver });

  try {
    // 1. 检查角色是否存在
    const character = await characterModel.findCharacterByCharNo(char_no);
    if (!character) {
      throw createApiError('角色不存在', 404, 'CHARACTER_NOT_FOUND');
    }

    // 2. 执行金币修改
    const modifyResult = await gameActionModel.modifyCharacterMoney({
      char_no,
      gold: gold ? BigInt(gold) : undefined,
      silver: silver ? BigInt(silver) : undefined
    });

    if (!modifyResult.success) {
      // 记录失败日志
      await operationModel.createOperationLog({
        gm_user: gmUser.username,
        gm_id: gmUser.gm_id,
        gm_level: gmUser.gm_level,
        operation_type: operationModel.OperationType.MONEY_MODIFY,
        operation_category: operationModel.OperationCategory.CHARACTER,
        target_type: operationModel.TargetType.CHARACTER,
        target_id: char_no.toString(),
        target_name: character.char_name,
        operation_detail: { gold, silver, reason },
        result: operationModel.OperationResult.FAILURE,
        error_message: modifyResult.error,
        ip_address: ipAddress,
        user_agent: userAgent
      });

      throw createApiError(modifyResult.error || '金币修改失败', 500, 'MONEY_MODIFY_FAILED');
    }

    const oldMoney = {
      gold: modifyResult.old_money!.gold.toString(),
      silver: modifyResult.old_money!.silver.toString()
    };

    const newMoney = {
      gold: gold || oldMoney.gold,
      silver: silver || oldMoney.silver
    };

    // 3. 记录成功日志
    const executedAt = new Date();
    const operationId = await operationModel.createOperationLog({
      gm_user: gmUser.username,
      gm_id: gmUser.gm_id,
      gm_level: gmUser.gm_level,
      operation_type: operationModel.OperationType.MONEY_MODIFY,
      operation_category: operationModel.OperationCategory.CHARACTER,
      target_type: operationModel.TargetType.CHARACTER,
      target_id: char_no.toString(),
      target_name: character.char_name,
      operation_detail: { gold, silver, reason },
      before_state: oldMoney,
      after_state: newMoney,
      result: operationModel.OperationResult.SUCCESS,
      ip_address: ipAddress,
      user_agent: userAgent
    });

    logger.info('金币修改成功', {
      operationId,
      char_no,
      old_money: oldMoney,
      new_money: newMoney
    });

    return {
      operation_id: operationId,
      char_no,
      old_money: oldMoney,
      new_money: newMoney,
      executed_at: executedAt
    };
  } catch (error: any) {
    logger.error('金币修改失败', error);
    throw error;
  }
}
