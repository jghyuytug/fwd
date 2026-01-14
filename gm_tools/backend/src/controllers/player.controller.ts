/**
 * 玩家查询控制器
 * 处理玩家相关的HTTP请求
 */

import { Request, Response } from 'express';
import * as playerService from '../services/player.service';
import { createApiError } from '../middlewares/errorHandler';

/**
 * 搜索角色
 * GET /api/v1/players/characters/search
 */
export async function searchCharacters(req: Request, res: Response): Promise<void> {
  const {
    name,
    fuzzy,
    m_id,
    min_level,
    max_level,
    job,
    page,
    page_size
  } = req.query;

  // 构建搜索参数
  const params = {
    name: name as string | undefined,
    fuzzy: fuzzy === 'true' || fuzzy === '1',
    m_id: m_id as string | undefined,
    min_level: min_level ? parseInt(min_level as string) : undefined,
    max_level: max_level ? parseInt(max_level as string) : undefined,
    job: job ? parseInt(job as string) : undefined,
    page: page ? parseInt(page as string) : 1,
    page_size: page_size ? parseInt(page_size as string) : 20
  };

  const result = await playerService.searchCharacters(params);

  res.status(200).json({
    code: 200,
    message: '查询成功',
    data: result
  });
}

/**
 * 查询角色详细信息
 * GET /api/v1/players/characters/:char_no
 */
export async function getCharacterDetail(req: Request, res: Response): Promise<void> {
  const charNo = parseInt(req.params.char_no);

  if (isNaN(charNo) || charNo <= 0) {
    throw createApiError('无效的角色编号', 400, 'INVALID_CHAR_NO');
  }

  const result = await playerService.getCharacterDetail(charNo);

  res.status(200).json({
    code: 200,
    message: '查询成功',
    data: result
  });
}

/**
 * 查询角色装备
 * GET /api/v1/players/characters/:char_no/equipment
 */
export async function getCharacterEquipment(req: Request, res: Response): Promise<void> {
  const charNo = parseInt(req.params.char_no);

  if (isNaN(charNo) || charNo <= 0) {
    throw createApiError('无效的角色编号', 400, 'INVALID_CHAR_NO');
  }

  const equipment = await playerService.getCharacterEquipment(charNo);

  res.status(200).json({
    code: 200,
    message: '查询成功',
    data: {
      equipment
    }
  });
}

/**
 * 查询角色背包
 * GET /api/v1/players/characters/:char_no/inventory
 */
export async function getCharacterInventory(req: Request, res: Response): Promise<void> {
  const charNo = parseInt(req.params.char_no);
  const page = req.query.page ? parseInt(req.query.page as string) : 1;
  const pageSize = req.query.page_size ? parseInt(req.query.page_size as string) : 50;

  if (isNaN(charNo) || charNo <= 0) {
    throw createApiError('无效的角色编号', 400, 'INVALID_CHAR_NO');
  }

  const result = await playerService.getCharacterInventory(charNo, page, pageSize);

  res.status(200).json({
    code: 200,
    message: '查询成功',
    data: result
  });
}

/**
 * 查询角色仓库
 * GET /api/v1/players/characters/:char_no/warehouse
 */
export async function getCharacterWarehouse(req: Request, res: Response): Promise<void> {
  const charNo = parseInt(req.params.char_no);
  const page = req.query.page ? parseInt(req.query.page as string) : 1;
  const pageSize = req.query.page_size ? parseInt(req.query.page_size as string) : 50;

  if (isNaN(charNo) || charNo <= 0) {
    throw createApiError('无效的角色编号', 400, 'INVALID_CHAR_NO');
  }

  const result = await playerService.getCharacterWarehouse(charNo, page, pageSize);

  res.status(200).json({
    code: 200,
    message: '查询成功',
    data: result
  });
}

/**
 * 查询角色金币
 * GET /api/v1/players/characters/:char_no/money
 */
export async function getCharacterMoney(req: Request, res: Response): Promise<void> {
  const charNo = parseInt(req.params.char_no);

  if (isNaN(charNo) || charNo <= 0) {
    throw createApiError('无效的角色编号', 400, 'INVALID_CHAR_NO');
  }

  const result = await playerService.getCharacterMoney(charNo);

  res.status(200).json({
    code: 200,
    message: '查询成功',
    data: result
  });
}

/**
 * 查询角色在线状态
 * GET /api/v1/players/characters/:char_no/online
 */
export async function checkCharacterOnline(req: Request, res: Response): Promise<void> {
  const charNo = parseInt(req.params.char_no);

  if (isNaN(charNo) || charNo <= 0) {
    throw createApiError('无效的角色编号', 400, 'INVALID_CHAR_NO');
  }

  const result = await playerService.checkCharacterOnline(charNo);

  res.status(200).json({
    code: 200,
    message: '查询成功',
    data: result
  });
}

/**
 * 查询账号详细信息
 * GET /api/v1/players/accounts/:m_id
 */
export async function getAccountDetail(req: Request, res: Response): Promise<void> {
  const mId = req.params.m_id;

  if (!mId || mId.trim().length === 0) {
    throw createApiError('无效的账号ID', 400, 'INVALID_M_ID');
  }

  const result = await playerService.getAccountDetail(mId);

  res.status(200).json({
    code: 200,
    message: '查询成功',
    data: result
  });
}

/**
 * 根据账号名查询账号
 * GET /api/v1/players/accounts/search
 */
export async function searchAccount(req: Request, res: Response): Promise<void> {
  const accountName = req.query.name as string;

  if (!accountName || accountName.trim().length === 0) {
    throw createApiError('账号名不能为空', 400, 'MISSING_ACCOUNT_NAME');
  }

  const account = await playerService.getAccountByName(accountName);

  res.status(200).json({
    code: 200,
    message: '查询成功',
    data: {
      account
    }
  });
}
