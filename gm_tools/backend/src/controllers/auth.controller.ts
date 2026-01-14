/**
 * 认证控制器
 * 处理认证相关的HTTP请求
 */

import { Request, Response } from 'express';
import * as authService from '../services/auth.service';
import { logger } from '../utils/logger.util';
import { createApiError } from '../middlewares/errorHandler';

/**
 * 登录
 * POST /api/v1/auth/login
 */
export async function login(req: Request, res: Response): Promise<void> {
  try {
    const { username, password, device_fingerprint, remember_device, two_factor_code, device_token } = req.body;

    // 参数验证
    if (!username || !password) {
      throw createApiError('用户名和密码不能为空', 400, 'MISSING_PARAMETERS');
    }

    // 获取IP和User Agent
    const ipAddress = (req.ip || req.connection.remoteAddress || '').replace('::ffff:', '');
    const userAgent = req.headers['user-agent'];

    // 调用登录服务
    const result = await authService.login(
      {
        username,
        password,
        device_fingerprint,
        remember_device,
        two_factor_code,
        device_token
      },
      ipAddress,
      userAgent
    );

    // 返回成功响应
    res.status(200).json({
      code: 200,
      message: result.requires_2fa ? '需要双因素认证' : '登录成功',
      data: result
    });

  } catch (error) {
    throw error;
  }
}

/**
 * 刷新Token
 * POST /api/v1/auth/refresh
 */
export async function refresh(req: Request, res: Response): Promise<void> {
  try {
    // 从Request Body中获取Refresh Token
    const { refresh_token } = req.body;

    if (!refresh_token) {
      throw createApiError('缺少Refresh Token', 401, 'MISSING_TOKEN');
    }

    // 调用刷新服务
    const result = await authService.refreshToken(refresh_token);

    res.status(200).json({
      code: 200,
      message: 'Token刷新成功',
      data: result
    });

  } catch (error) {
    throw error;
  }
}

/**
 * 登出
 * POST /api/v1/auth/logout
 */
export async function logout(req: Request, res: Response): Promise<void> {
  try {
    if (!req.user) {
      throw createApiError('未认证', 401, 'UNAUTHENTICATED');
    }

    await authService.logout(req.user.gm_id);

    res.status(200).json({
      code: 200,
      message: '登出成功'
    });

  } catch (error) {
    throw error;
  }
}

/**
 * 设置2FA
 * POST /api/v1/auth/2fa/setup
 */
export async function setupTwoFactor(req: Request, res: Response): Promise<void> {
  try {
    if (!req.user) {
      throw createApiError('未认证', 401, 'UNAUTHENTICATED');
    }

    const result = await authService.setupTwoFactor(req.user.gm_id, req.user.username);

    res.status(200).json({
      code: 200,
      message: '2FA设置成功',
      data: result
    });

  } catch (error) {
    throw error;
  }
}

/**
 * 验证2FA
 * POST /api/v1/auth/2fa/verify
 */
export async function verifyTwoFactor(req: Request, res: Response): Promise<void> {
  try {
    if (!req.user) {
      throw createApiError('未认证', 401, 'UNAUTHENTICATED');
    }

    const { secret, code, backup_codes } = req.body;

    if (!secret || !code || !backup_codes) {
      throw createApiError('缺少必要参数', 400, 'MISSING_PARAMETERS');
    }

    await authService.verifyAndEnableTwoFactor(req.user.gm_id, secret, code, backup_codes);

    res.status(200).json({
      code: 200,
      message: '2FA验证成功，已启用'
    });

  } catch (error) {
    throw error;
  }
}

/**
 * 禁用2FA
 * POST /api/v1/auth/2fa/disable
 */
export async function disableTwoFactor(req: Request, res: Response): Promise<void> {
  try {
    if (!req.user) {
      throw createApiError('未认证', 401, 'UNAUTHENTICATED');
    }

    const { password } = req.body;

    if (!password) {
      throw createApiError('需要密码验证', 400, 'MISSING_PASSWORD');
    }

    await authService.disableTwoFactorService(req.user.gm_id, password);

    res.status(200).json({
      code: 200,
      message: '2FA已禁用'
    });

  } catch (error) {
    throw error;
  }
}

/**
 * 获取信任设备列表
 * GET /api/v1/auth/trusted-devices
 */
export async function getTrustedDevices(req: Request, res: Response): Promise<void> {
  try {
    if (!req.user) {
      throw createApiError('未认证', 401, 'UNAUTHENTICATED');
    }

    const devices = await authService.getTrustedDevices(req.user.gm_id);

    res.status(200).json({
      code: 200,
      message: '查询成功',
      data: { devices }
    });

  } catch (error) {
    throw error;
  }
}

/**
 * 撤销信任设备
 * DELETE /api/v1/auth/trusted-devices/:device_id
 */
export async function revokeTrustedDevice(req: Request, res: Response): Promise<void> {
  try {
    if (!req.user) {
      throw createApiError('未认证', 401, 'UNAUTHENTICATED');
    }

    const deviceId = parseInt(req.params.device_id);

    if (isNaN(deviceId)) {
      throw createApiError('无效的设备ID', 400, 'INVALID_DEVICE_ID');
    }

    await authService.revokeTrustedDeviceService(req.user.gm_id, deviceId);

    res.status(200).json({
      code: 200,
      message: '设备已撤销'
    });

  } catch (error) {
    throw error;
  }
}

/**
 * 获取当前用户信息
 * GET /api/v1/auth/me
 */
export async function getCurrentUser(req: Request, res: Response): Promise<void> {
  try {
    if (!req.user) {
      throw createApiError('未认证', 401, 'UNAUTHENTICATED');
    }

    res.status(200).json({
      code: 200,
      message: '查询成功',
      data: {
        gm_id: req.user.gm_id,
        username: req.user.username,
        gm_level: req.user.gm_level,
        permissions: req.user.permissions
      }
    });

  } catch (error) {
    throw error;
  }
}
