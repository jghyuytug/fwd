/**
 * 玩家查询服务
 * 处理角色、账号、装备等查询业务逻辑
 */

import * as characterModel from '../models/character.model';
import * as accountModel from '../models/account.model';
import * as equipmentModel from '../models/equipment.model';
import { createApiError } from '../middlewares/errorHandler';
import { logger } from '../utils/logger.util';

/**
 * 搜索角色
 */
export async function searchCharacters(
  params: characterModel.CharacterSearchParams
): Promise<characterModel.CharacterSearchResult> {
  try {
    logger.info('搜索角色', { params });

    // 验证参数
    if (!params.name && !params.m_id) {
      throw createApiError('必须提供角色名或账号ID', 400, 'MISSING_PARAMETERS');
    }

    // 验证分页参数
    const page = params.page || 1;
    const pageSize = params.page_size || 20;

    if (page < 1) {
      throw createApiError('页码必须大于0', 400, 'INVALID_PAGE');
    }

    if (pageSize < 1 || pageSize > 100) {
      throw createApiError('每页数量必须在1-100之间', 400, 'INVALID_PAGE_SIZE');
    }

    const result = await characterModel.searchCharactersByName({
      ...params,
      page,
      page_size: pageSize
    });

    logger.info('角色搜索完成', {
      total: result.total,
      count: result.characters.length
    });

    return result;
  } catch (error: any) {
    logger.error('角色搜索失败', error);
    throw error;
  }
}

/**
 * 查询角色详细信息
 */
export async function getCharacterDetail(charNo: number): Promise<{
  character: characterModel.CharacterDetail;
  account: accountModel.Account;
}> {
  try {
    logger.info('查询角色详细信息', { charNo });

    // 查询角色信息
    const character = await characterModel.findCharacterByCharNo(charNo);

    if (!character) {
      throw createApiError('角色不存在', 404, 'CHARACTER_NOT_FOUND');
    }

    // 查询账号信息
    const account = await accountModel.findAccountById(character.m_id);

    if (!account) {
      throw createApiError('关联账号不存在', 404, 'ACCOUNT_NOT_FOUND');
    }

    logger.info('角色详细信息查询完成', {
      charNo,
      charName: character.char_name,
      mId: character.m_id
    });

    return {
      character,
      account
    };
  } catch (error: any) {
    logger.error('查询角色详细信息失败', error);
    throw error;
  }
}

/**
 * 查询角色装备
 */
export async function getCharacterEquipment(charNo: number): Promise<equipmentModel.Equipment[]> {
  try {
    logger.info('查询角色装备', { charNo });

    // 检查角色是否存在
    const exists = await characterModel.checkCharacterExists(charNo);
    if (!exists) {
      throw createApiError('角色不存在', 404, 'CHARACTER_NOT_FOUND');
    }

    const equipment = await equipmentModel.findCharacterEquipment(charNo);

    logger.info('角色装备查询完成', {
      charNo,
      equipmentCount: equipment.length
    });

    return equipment;
  } catch (error: any) {
    logger.error('查询角色装备失败', error);
    throw error;
  }
}

/**
 * 查询角色背包
 */
export async function getCharacterInventory(
  charNo: number,
  page: number = 1,
  pageSize: number = 50
): Promise<equipmentModel.InventoryResult> {
  try {
    logger.info('查询角色背包', { charNo, page, pageSize });

    // 检查角色是否存在
    const exists = await characterModel.checkCharacterExists(charNo);
    if (!exists) {
      throw createApiError('角色不存在', 404, 'CHARACTER_NOT_FOUND');
    }

    // 验证分页参数
    if (page < 1) {
      throw createApiError('页码必须大于0', 400, 'INVALID_PAGE');
    }

    if (pageSize < 1 || pageSize > 100) {
      throw createApiError('每页数量必须在1-100之间', 400, 'INVALID_PAGE_SIZE');
    }

    const result = await equipmentModel.findCharacterInventory(charNo, page, pageSize);

    logger.info('角色背包查询完成', {
      charNo,
      total: result.total,
      itemCount: result.items.length
    });

    return result;
  } catch (error: any) {
    logger.error('查询角色背包失败', error);
    throw error;
  }
}

/**
 * 查询角色仓库
 */
export async function getCharacterWarehouse(
  charNo: number,
  page: number = 1,
  pageSize: number = 50
): Promise<equipmentModel.InventoryResult> {
  try {
    logger.info('查询角色仓库', { charNo, page, pageSize });

    // 检查角色是否存在
    const exists = await characterModel.checkCharacterExists(charNo);
    if (!exists) {
      throw createApiError('角色不存在', 404, 'CHARACTER_NOT_FOUND');
    }

    // 验证分页参数
    if (page < 1) {
      throw createApiError('页码必须大于0', 400, 'INVALID_PAGE');
    }

    if (pageSize < 1 || pageSize > 100) {
      throw createApiError('每页数量必须在1-100之间', 400, 'INVALID_PAGE_SIZE');
    }

    const result = await equipmentModel.findCharacterWarehouse(charNo, page, pageSize);

    logger.info('角色仓库查询完成', {
      charNo,
      total: result.total,
      itemCount: result.items.length
    });

    return result;
  } catch (error: any) {
    logger.error('查询角色仓库失败', error);
    throw error;
  }
}

/**
 * 查询角色金币
 */
export async function getCharacterMoney(charNo: number): Promise<{
  char_no: number;
  gold: string;
  silver: string;
}> {
  try {
    logger.info('查询角色金币', { charNo });

    // 检查角色是否存在
    const exists = await characterModel.checkCharacterExists(charNo);
    if (!exists) {
      throw createApiError('角色不存在', 404, 'CHARACTER_NOT_FOUND');
    }

    const money = await equipmentModel.findCharacterMoney(charNo);

    logger.info('角色金币查询完成', {
      charNo,
      gold: money.gold.toString(),
      silver: money.silver.toString()
    });

    return {
      char_no: charNo,
      gold: money.gold.toString(),
      silver: money.silver.toString()
    };
  } catch (error: any) {
    logger.error('查询角色金币失败', error);
    throw error;
  }
}

/**
 * 查询账号详细信息
 */
export async function getAccountDetail(mId: string): Promise<{
  account: accountModel.Account;
  characters: characterModel.Character[];
  loginHistory: accountModel.LoginHistory[];
  stats: {
    character_count: number;
    total_playtime: number;
    max_level: number;
  };
}> {
  try {
    logger.info('查询账号详细信息', { mId });

    // 查询账号信息
    const account = await accountModel.findAccountById(mId);

    if (!account) {
      throw createApiError('账号不存在', 404, 'ACCOUNT_NOT_FOUND');
    }

    // 查询角色列表
    const characters = await characterModel.findCharactersByAccountId(mId);

    // 查询登录历史（最近20条）
    const loginHistory = await accountModel.findLoginHistory(mId, 20);

    // 查询统计信息
    const stats = await accountModel.getAccountStats(mId);

    logger.info('账号详细信息查询完成', {
      mId,
      characterCount: characters.length,
      loginHistoryCount: loginHistory.length
    });

    return {
      account,
      characters,
      loginHistory,
      stats
    };
  } catch (error: any) {
    logger.error('查询账号详细信息失败', error);
    throw error;
  }
}

/**
 * 根据账号名查询账号
 */
export async function getAccountByName(accountName: string): Promise<accountModel.Account> {
  try {
    logger.info('根据账号名查询账号', { accountName });

    const account = await accountModel.findAccountByName(accountName);

    if (!account) {
      throw createApiError('账号不存在', 404, 'ACCOUNT_NOT_FOUND');
    }

    logger.info('账号查询完成', { accountName, mId: account.m_id });

    return account;
  } catch (error: any) {
    logger.error('根据账号名查询账号失败', error);
    throw error;
  }
}

/**
 * 检查账号是否在线
 */
export async function checkCharacterOnline(charNo: number): Promise<{
  is_online: boolean;
  login_time: Date | null;
  online_duration: number; // 秒
}> {
  try {
    logger.info('检查角色在线状态', { charNo });

    const character = await characterModel.findCharacterByCharNo(charNo);

    if (!character) {
      throw createApiError('角色不存在', 404, 'CHARACTER_NOT_FOUND');
    }

    const isOnline = character.logout_time === null && character.login_time !== null;
    const onlineDuration = isOnline && character.login_time
      ? Math.floor((new Date().getTime() - new Date(character.login_time).getTime()) / 1000)
      : 0;

    logger.info('角色在线状态查询完成', {
      charNo,
      isOnline,
      onlineDuration
    });

    return {
      is_online: isOnline,
      login_time: character.login_time,
      online_duration: onlineDuration
    };
  } catch (error: any) {
    logger.error('检查角色在线状态失败', error);
    throw error;
  }
}
