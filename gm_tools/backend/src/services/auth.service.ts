/**
 * 认证服务
 * 处理登录、Token生成、2FA等认证相关业务逻辑
 */

import {
  findGmAccountByUsername,
  findGmAccountById,
  incrementFailedLoginAttempts,
  resetFailedLoginAttempts,
  updateLastLogin,
  createLoginLog,
  findTrustedDeviceByToken,
  createTrustedDevice,
  updateTrustedDeviceLastUsed,
  findTrustedDevicesByGmId,
  revokeTrustedDevice,
  enableTwoFactor as enableTwoFactorModel,
  disableTwoFactor as disableTwoFactorModel,
  GmAccount
} from '../models/gmAccount.model';
import { verifyPassword } from '../utils/crypto.util';
import { generateAccessToken, generateRefreshToken, verifyToken } from '../utils/jwt.util';
import {
  generateTwoFactorSecret,
  generateQRCode,
  verifyTwoFactorCode,
  generateBackupCodes,
  validateBackupCodeFormat
} from '../utils/twoFactor.util';
import { generateDeviceToken } from '../utils/crypto.util';
import { logger } from '../utils/logger.util';
import { createApiError } from '../middlewares/errorHandler';

export interface LoginRequest {
  username: string;
  password: string;
  device_fingerprint?: string;
  remember_device?: boolean;
  two_factor_code?: string;
  device_token?: string;
}

export interface LoginResponse {
  access_token: string;
  refresh_token: string;
  expires_in: number;
  user: {
    gm_id: number;
    username: string;
    real_name: string | null;
    gm_level: number;
    permissions: string[];
  };
  device_token?: string;
  requires_2fa?: boolean;
}

/**
 * 获取GM权限列表
 */
async function getGmPermissions(gmId: number, gmLevel: number): Promise<string[]> {
  // 超级管理员（GM等级5）拥有所有权限
  if (gmLevel === 5) {
    return ['*']; // 通配符表示所有权限
  }

  // TODO: 从数据库查询用户的权限
  // 暂时返回基础权限
  const basePermissions = ['player:read', 'system:logs'];

  if (gmLevel >= 3) {
    basePermissions.push('item:grant', 'player:read:sensitive');
  }

  if (gmLevel >= 4) {
    basePermissions.push('account:ban', 'account:unban');
  }

  return basePermissions;
}

/**
 * 登录
 */
export async function login(
  data: LoginRequest,
  ipAddress: string,
  userAgent?: string
): Promise<LoginResponse> {
  const { username, password, device_fingerprint, remember_device, two_factor_code, device_token } = data;

  try {
    // 1. 查询GM账号
    const account = await findGmAccountByUsername(username);

    if (!account) {
      // 记录失败日志（使用临时ID）
      await createLoginLog({
        gm_user: username,
        gm_id: 0,
        login_result: 'FAILURE',
        failure_reason: '用户名或密码错误',
        ip_address: ipAddress,
        user_agent: userAgent
      });

      throw createApiError('用户名或密码错误', 401, 'INVALID_CREDENTIALS');
    }

    // 2. 检查账号状态
    if (!account.is_active) {
      throw createApiError('账号已禁用', 403, 'ACCOUNT_DISABLED');
    }

    // 3. 检查账号锁定状态
    if (account.is_locked && account.locked_until) {
      const now = new Date();
      if (now < account.locked_until) {
        const remainingSeconds = Math.ceil((account.locked_until.getTime() - now.getTime()) / 1000);

        await createLoginLog({
          gm_user: username,
          gm_id: account.gm_id,
          login_result: 'LOCKED',
          failure_reason: `账号已锁定，剩余${remainingSeconds}秒`,
          ip_address: ipAddress,
          user_agent: userAgent
        });

        throw createApiError(
          `账号已锁定，请${remainingSeconds}秒后重试`,
          403,
          'ACCOUNT_LOCKED',
          { remaining_seconds: remainingSeconds }
        );
      } else {
        // 锁定时间已过，重置状态
        await resetFailedLoginAttempts(account.gm_id);
      }
    }

    // 4. 验证密码
    const passwordValid = await verifyPassword(password, account.password_hash);

    if (!passwordValid) {
      // 增加失败次数
      await incrementFailedLoginAttempts(account.gm_id);

      await createLoginLog({
        gm_user: username,
        gm_id: account.gm_id,
        login_result: 'FAILURE',
        failure_reason: '密码错误',
        ip_address: ipAddress,
        user_agent: userAgent,
        device_fingerprint
      });

      const maxAttempts = parseInt(process.env.LOGIN_MAX_FAILED_ATTEMPTS || '5');
      const remainingAttempts = maxAttempts - (account.failed_login_attempts + 1);

      throw createApiError(
        '用户名或密码错误',
        401,
        'INVALID_CREDENTIALS',
        {
          failed_attempts: account.failed_login_attempts + 1,
          remaining_attempts: Math.max(0, remainingAttempts)
        }
      );
    }

    // 5. 检查是否需要2FA
    const requires2FA = account.two_factor_enabled && account.gm_level >= parseInt(process.env.TWO_FACTOR_REQUIRED_LEVEL || '3');
    let trustedDevice = null;

    // 检查设备是否已信任
    if (device_token) {
      trustedDevice = await findTrustedDeviceByToken(device_token);
      if (trustedDevice && trustedDevice.gm_id === account.gm_id) {
        // 设备已信任，跳过2FA
        await updateTrustedDeviceLastUsed(trustedDevice.device_id);
        logger.info('使用信任设备登录', { gm_id: account.gm_id, device_id: trustedDevice.device_id });
      } else {
        trustedDevice = null;
      }
    }

    // 6. 如果需要2FA且设备未信任
    if (requires2FA && !trustedDevice) {
      if (!two_factor_code) {
        // 返回需要2FA的提示
        return {
          requires_2fa: true
        } as any;
      }

      // 验证2FA代码
      const twoFactorValid = verifyTwoFactorCode(account.two_factor_secret!, two_factor_code);

      if (!twoFactorValid) {
        await createLoginLog({
          gm_user: username,
          gm_id: account.gm_id,
          login_result: 'FAILURE',
          failure_reason: '2FA验证失败',
          two_factor_used: true,
          two_factor_result: 'FAILURE',
          ip_address: ipAddress,
          user_agent: userAgent,
          device_fingerprint
        });

        throw createApiError('双因素认证码错误', 401, '2FA_INVALID');
      }
    }

    // 7. 登录成功，重置失败次数
    await resetFailedLoginAttempts(account.gm_id);

    // 8. 获取权限列表
    const permissions = await getGmPermissions(account.gm_id, account.gm_level);

    // 9. 生成Token
    const accessToken = generateAccessToken({
      gm_id: account.gm_id,
      username: account.username,
      gm_level: account.gm_level,
      permissions
    });

    const refreshToken = generateRefreshToken({
      gm_id: account.gm_id,
      username: account.username,
      gm_level: account.gm_level
    });

    // 10. 处理设备信任
    let newDeviceToken: string | undefined;
    if (remember_device && device_fingerprint) {
      newDeviceToken = generateDeviceToken();
      await createTrustedDevice({
        gm_user: account.username,
        gm_id: account.gm_id,
        device_fingerprint,
        device_token: newDeviceToken,
        device_name: userAgent,
        ip_address: ipAddress,
        user_agent: userAgent
      });

      logger.info('创建信任设备', { gm_id: account.gm_id, device_fingerprint });
    }

    // 11. 更新最后登录信息
    await updateLastLogin(account.gm_id, ipAddress);

    // 12. 记录登录日志
    await createLoginLog({
      gm_user: username,
      gm_id: account.gm_id,
      login_result: 'SUCCESS',
      two_factor_used: requires2FA && !trustedDevice,
      two_factor_result: requires2FA && !trustedDevice ? 'SUCCESS' : undefined,
      ip_address: ipAddress,
      user_agent: userAgent,
      device_fingerprint
    });

    logger.info('用户登录成功', {
      gm_id: account.gm_id,
      username: account.username,
      ip_address: ipAddress
    });

    return {
      access_token: accessToken,
      refresh_token: refreshToken,
      expires_in: parseInt(process.env.JWT_EXPIRATION || '1800'),
      user: {
        gm_id: account.gm_id,
        username: account.username,
        real_name: account.real_name,
        gm_level: account.gm_level,
        permissions
      },
      device_token: newDeviceToken
    };

  } catch (error) {
    // 如果是已知的API错误，直接抛出
    if ((error as any).statusCode) {
      throw error;
    }

    // 未知错误
    logger.error('登录失败', { username, error });
    throw createApiError('登录失败', 500, 'LOGIN_FAILED');
  }
}

/**
 * 刷新Token
 */
export async function refreshToken(refreshToken: string): Promise<{ access_token: string; refresh_token: string; expires_in: number }> {
  try {
    // 验证Refresh Token
    const payload = verifyToken(refreshToken);

    if (payload.type !== 'refresh') {
      throw createApiError('无效的Refresh Token', 401, 'INVALID_REFRESH_TOKEN');
    }

    // 查询账号状态
    const account = await findGmAccountById(payload.gm_id);

    if (!account || !account.is_active) {
      throw createApiError('账号不存在或已禁用', 403, 'ACCOUNT_INVALID');
    }

    // 重新获取权限
    const permissions = await getGmPermissions(account.gm_id, account.gm_level);

    // 生成新的Access Token
    const accessToken = generateAccessToken({
      gm_id: account.gm_id,
      username: account.username,
      gm_level: account.gm_level,
      permissions
    });

    // 生成新的Refresh Token (Token Rotation)
    const newRefreshToken = generateRefreshToken({
      gm_id: account.gm_id,
      username: account.username,
      gm_level: account.gm_level
    });

    logger.info('Token刷新成功', { gm_id: account.gm_id });

    return {
      access_token: accessToken,
      refresh_token: newRefreshToken,
      expires_in: parseInt(process.env.JWT_EXPIRATION || '1800')
    };

  } catch (error) {
    if ((error as any).statusCode) {
      throw error;
    }

    logger.error('Token刷新失败', { error });
    throw createApiError('Token刷新失败', 500, 'REFRESH_FAILED');
  }
}

/**
 * 登出（TODO: 将Token加入黑名单）
 */
export async function logout(gmId: number): Promise<void> {
  try {
    // TODO: 将Token加入Redis黑名单

    logger.info('用户登出', { gm_id: gmId });
  } catch (error) {
    logger.error('登出失败', { gm_id: gmId, error });
    throw createApiError('登出失败', 500, 'LOGOUT_FAILED');
  }
}

/**
 * 设置2FA
 */
export async function setupTwoFactor(gmId: number, username: string): Promise<{
  secret: string;
  qr_code: string;
  backup_codes: string[];
}> {
  try {
    // 生成2FA密钥
    const { secret, otpauthUrl } = generateTwoFactorSecret(username);

    // 生成QR码
    const qrCode = await generateQRCode(otpauthUrl);

    // 生成备份代码
    const backupCodes = generateBackupCodes(10);

    // 暂存到数据库（尚未验证）
    // TODO: 使用临时存储，验证后再正式启用

    logger.info('2FA设置初始化', { gm_id: gmId });

    return {
      secret,
      qr_code: qrCode,
      backup_codes: backupCodes
    };

  } catch (error) {
    logger.error('2FA设置失败', { gm_id: gmId, error });
    throw createApiError('2FA设置失败', 500, '2FA_SETUP_FAILED');
  }
}

/**
 * 验证并启用2FA
 */
export async function verifyAndEnableTwoFactor(gmId: number, secret: string, code: string, backupCodes: string[]): Promise<void> {
  try {
    // 验证2FA代码
    const valid = verifyTwoFactorCode(secret, code);

    if (!valid) {
      throw createApiError('验证码错误', 400, '2FA_CODE_INVALID');
    }

    // 启用2FA
    await enableTwoFactorModel(gmId, secret, JSON.stringify(backupCodes));

    logger.info('2FA启用成功', { gm_id: gmId });

  } catch (error) {
    if ((error as any).statusCode) {
      throw error;
    }

    logger.error('2FA启用失败', { gm_id: gmId, error });
    throw createApiError('2FA启用失败', 500, '2FA_ENABLE_FAILED');
  }
}

/**
 * 禁用2FA
 */
export async function disableTwoFactorService(gmId: number, password: string): Promise<void> {
  try {
    // 验证密码
    const account = await findGmAccountById(gmId);

    if (!account) {
      throw createApiError('账号不存在', 404, 'ACCOUNT_NOT_FOUND');
    }

    const passwordValid = await verifyPassword(password, account.password_hash);

    if (!passwordValid) {
      throw createApiError('密码错误', 401, 'INVALID_PASSWORD');
    }

    // 禁用2FA
    await disableTwoFactorModel(gmId);

    logger.info('2FA禁用成功', { gm_id: gmId });

  } catch (error) {
    if ((error as any).statusCode) {
      throw error;
    }

    logger.error('2FA禁用失败', { gm_id: gmId, error });
    throw createApiError('2FA禁用失败', 500, '2FA_DISABLE_FAILED');
  }
}

/**
 * 获取信任设备列表
 */
export async function getTrustedDevices(gmId: number) {
  try {
    return await findTrustedDevicesByGmId(gmId);
  } catch (error) {
    logger.error('获取信任设备列表失败', { gm_id: gmId, error });
    throw createApiError('获取信任设备列表失败', 500, 'GET_DEVICES_FAILED');
  }
}

/**
 * 撤销信任设备
 */
export async function revokeTrustedDeviceService(gmId: number, deviceId: number): Promise<void> {
  try {
    await revokeTrustedDevice(deviceId, gmId);
    logger.info('信任设备已撤销', { gm_id: gmId, device_id: deviceId });
  } catch (error) {
    logger.error('撤销信任设备失败', { gm_id: gmId, device_id: deviceId, error });
    throw createApiError('撤销信任设备失败', 500, 'REVOKE_DEVICE_FAILED');
  }
}
