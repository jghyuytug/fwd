/**
 * GM操作控制器
 * 处理GM操作相关的HTTP请求
 */

import { Request, Response } from 'express';
import * as operationService from '../services/operation.service';
import { createApiError } from '../middlewares/errorHandler';

/**
 * 赠送物品
 * POST /api/v1/operations/grant-item
 */
export async function grantItem(req: Request, res: Response): Promise<void> {
  const { char_no, item_id, amount, reason, expire_days } = req.body;

  // 参数验证
  if (!char_no || !item_id || !amount || !reason) {
    throw createApiError('缺少必要参数', 400, 'MISSING_PARAMETERS');
  }

  if (!Number.isInteger(char_no) || char_no <= 0) {
    throw createApiError('无效的角色编号', 400, 'INVALID_CHAR_NO');
  }

  if (!Number.isInteger(item_id) || item_id <= 0) {
    throw createApiError('无效的物品ID', 400, 'INVALID_ITEM_ID');
  }

  if (!Number.isInteger(amount) || amount <= 0) {
    throw createApiError('无效的物品数量', 400, 'INVALID_AMOUNT');
  }

  if (typeof reason !== 'string' || reason.trim().length === 0) {
    throw createApiError('原因不能为空', 400, 'INVALID_REASON');
  }

  // 获取IP和User Agent
  const ipAddress = (req.ip || req.connection.remoteAddress || '').replace('::ffff:', '');
  const userAgent = req.headers['user-agent'];

  // 执行操作
  const result = await operationService.grantItem(
    {
      char_no,
      item_id,
      amount,
      reason: reason.trim(),
      expire_days: expire_days || 0
    },
    {
      gm_id: req.user!.gm_id,
      username: req.user!.username,
      gm_level: req.user!.gm_level
    },
    ipAddress,
    userAgent
  );

  res.status(200).json({
    code: 200,
    message: '物品赠送成功',
    data: result
  });
}

/**
 * 发送邮件
 * POST /api/v1/operations/send-mail
 */
export async function sendMail(req: Request, res: Response): Promise<void> {
  const { char_no, title, content, item_id, item_amount, gold, expire_days } = req.body;

  // 参数验证
  if (!char_no || !title || !content) {
    throw createApiError('缺少必要参数', 400, 'MISSING_PARAMETERS');
  }

  if (!Number.isInteger(char_no) || char_no <= 0) {
    throw createApiError('无效的角色编号', 400, 'INVALID_CHAR_NO');
  }

  if (typeof title !== 'string' || title.trim().length === 0) {
    throw createApiError('邮件标题不能为空', 400, 'INVALID_TITLE');
  }

  if (typeof content !== 'string' || content.trim().length === 0) {
    throw createApiError('邮件内容不能为空', 400, 'INVALID_CONTENT');
  }

  // 获取IP和User Agent
  const ipAddress = (req.ip || req.connection.remoteAddress || '').replace('::ffff:', '');
  const userAgent = req.headers['user-agent'];

  // 执行操作
  const result = await operationService.sendMail(
    {
      char_no,
      title: title.trim(),
      content: content.trim(),
      item_id,
      item_amount,
      gold,
      expire_days: expire_days || 30
    },
    {
      gm_id: req.user!.gm_id,
      username: req.user!.username,
      gm_level: req.user!.gm_level
    },
    ipAddress,
    userAgent
  );

  res.status(200).json({
    code: 200,
    message: '邮件发送成功',
    data: result
  });
}

/**
 * 封禁账号
 * POST /api/v1/operations/ban-account
 */
export async function banAccount(req: Request, res: Response): Promise<void> {
  const { m_id, ban_type, ban_duration, reason, evidence } = req.body;

  // 参数验证
  if (!m_id || ban_type === undefined || ban_duration === undefined || !reason) {
    throw createApiError('缺少必要参数', 400, 'MISSING_PARAMETERS');
  }

  if (typeof m_id !== 'string' || m_id.trim().length === 0) {
    throw createApiError('无效的账号ID', 400, 'INVALID_M_ID');
  }

  if (!Number.isInteger(ban_type)) {
    throw createApiError('无效的封禁类型', 400, 'INVALID_BAN_TYPE');
  }

  if (!Number.isInteger(ban_duration) || ban_duration < 0) {
    throw createApiError('无效的封禁时长', 400, 'INVALID_BAN_DURATION');
  }

  if (typeof reason !== 'string' || reason.trim().length === 0) {
    throw createApiError('原因不能为空', 400, 'INVALID_REASON');
  }

  // 获取IP和User Agent
  const ipAddress = (req.ip || req.connection.remoteAddress || '').replace('::ffff:', '');
  const userAgent = req.headers['user-agent'];

  // 执行操作
  const result = await operationService.banAccount(
    {
      m_id: m_id.trim(),
      ban_type,
      ban_duration,
      reason: reason.trim(),
      evidence: evidence ? evidence.trim() : undefined
    },
    {
      gm_id: req.user!.gm_id,
      username: req.user!.username,
      gm_level: req.user!.gm_level
    },
    ipAddress,
    userAgent
  );

  res.status(200).json({
    code: 200,
    message: '账号封禁成功',
    data: result
  });
}

/**
 * 解封账号
 * POST /api/v1/operations/unban-account
 */
export async function unbanAccount(req: Request, res: Response): Promise<void> {
  const { m_id, reason } = req.body;

  // 参数验证
  if (!m_id || !reason) {
    throw createApiError('缺少必要参数', 400, 'MISSING_PARAMETERS');
  }

  if (typeof m_id !== 'string' || m_id.trim().length === 0) {
    throw createApiError('无效的账号ID', 400, 'INVALID_M_ID');
  }

  if (typeof reason !== 'string' || reason.trim().length === 0) {
    throw createApiError('原因不能为空', 400, 'INVALID_REASON');
  }

  // 获取IP和User Agent
  const ipAddress = (req.ip || req.connection.remoteAddress || '').replace('::ffff:', '');
  const userAgent = req.headers['user-agent'];

  // 执行操作
  const result = await operationService.unbanAccount(
    {
      m_id: m_id.trim(),
      reason: reason.trim()
    },
    {
      gm_id: req.user!.gm_id,
      username: req.user!.username,
      gm_level: req.user!.gm_level
    },
    ipAddress,
    userAgent
  );

  res.status(200).json({
    code: 200,
    message: '账号解封成功',
    data: result
  });
}

/**
 * 修改等级
 * POST /api/v1/operations/modify-level
 */
export async function modifyLevel(req: Request, res: Response): Promise<void> {
  const { char_no, new_level, reason } = req.body;

  // 参数验证
  if (!char_no || new_level === undefined || !reason) {
    throw createApiError('缺少必要参数', 400, 'MISSING_PARAMETERS');
  }

  if (!Number.isInteger(char_no) || char_no <= 0) {
    throw createApiError('无效的角色编号', 400, 'INVALID_CHAR_NO');
  }

  if (!Number.isInteger(new_level) || new_level < 1) {
    throw createApiError('无效的等级', 400, 'INVALID_LEVEL');
  }

  if (typeof reason !== 'string' || reason.trim().length === 0) {
    throw createApiError('原因不能为空', 400, 'INVALID_REASON');
  }

  // 获取IP和User Agent
  const ipAddress = (req.ip || req.connection.remoteAddress || '').replace('::ffff:', '');
  const userAgent = req.headers['user-agent'];

  // 执行操作
  const result = await operationService.modifyLevel(
    {
      char_no,
      new_level,
      reason: reason.trim()
    },
    {
      gm_id: req.user!.gm_id,
      username: req.user!.username,
      gm_level: req.user!.gm_level
    },
    ipAddress,
    userAgent
  );

  res.status(200).json({
    code: 200,
    message: '等级修改成功',
    data: result
  });
}

/**
 * 修改金币
 * POST /api/v1/operations/modify-money
 */
export async function modifyMoney(req: Request, res: Response): Promise<void> {
  const { char_no, gold, silver, reason } = req.body;

  // 参数验证
  if (!char_no || (!gold && !silver) || !reason) {
    throw createApiError('缺少必要参数', 400, 'MISSING_PARAMETERS');
  }

  if (!Number.isInteger(char_no) || char_no <= 0) {
    throw createApiError('无效的角色编号', 400, 'INVALID_CHAR_NO');
  }

  if (typeof reason !== 'string' || reason.trim().length === 0) {
    throw createApiError('原因不能为空', 400, 'INVALID_REASON');
  }

  // 获取IP和User Agent
  const ipAddress = (req.ip || req.connection.remoteAddress || '').replace('::ffff:', '');
  const userAgent = req.headers['user-agent'];

  // 执行操作
  const result = await operationService.modifyMoney(
    {
      char_no,
      gold: gold ? gold.toString() : undefined,
      silver: silver ? silver.toString() : undefined,
      reason: reason.trim()
    },
    {
      gm_id: req.user!.gm_id,
      username: req.user!.username,
      gm_level: req.user!.gm_level
    },
    ipAddress,
    userAgent
  );

  res.status(200).json({
    code: 200,
    message: '金币修改成功',
    data: result
  });
}
