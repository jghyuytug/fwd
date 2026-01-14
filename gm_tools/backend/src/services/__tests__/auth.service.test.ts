/**
 * auth.service.ts 单元测试
 */

import * as authService from '../auth.service';
import * as gmAccountModel from '../../models/gmAccount.model';
import * as cryptoUtil from '../../utils/crypto.util';
import * as jwtUtil from '../../utils/jwt.util';
import * as twoFactorUtil from '../../utils/twoFactor.util';
import { GmAccount } from '../../models/gmAccount.model';

// Mock所有依赖模块
jest.mock('../../models/gmAccount.model');
jest.mock('../../utils/crypto.util');
jest.mock('../../utils/jwt.util');
jest.mock('../../utils/twoFactor.util');
jest.mock('../../utils/logger.util', () => ({
  logger: {
    info: jest.fn(),
    error: jest.fn(),
    warn: jest.fn()
  }
}));

describe('auth.service', () => {
  const mockGmAccount: GmAccount = {
    gm_id: 1,
    username: 'test_gm',
    password_hash: '$2b$10$hashedpassword',
    real_name: '测试GM',
    email: 'test@example.com',
    phone: null,
    gm_level: 5,
    department: '技术部',
    two_factor_enabled: false,
    two_factor_secret: null,
    backup_codes: null,
    is_active: true,
    is_locked: false,
    locked_until: null,
    failed_login_attempts: 0,
    created_at: new Date(),
    created_by: 'admin',
    updated_at: new Date(),
    updated_by: null,
    last_login_at: null,
    last_login_ip: null
  };

  beforeEach(() => {
    jest.clearAllMocks();
  });

  describe('login', () => {
    const loginRequest = {
      username: 'test_gm',
      password: 'TestPassword123!',
      device_fingerprint: 'abc123',
      remember_device: false
    };

    beforeEach(() => {
      (gmAccountModel.findGmAccountByUsername as jest.Mock).mockResolvedValue(mockGmAccount);
      (cryptoUtil.verifyPassword as jest.Mock).mockResolvedValue(true);
      (jwtUtil.generateAccessToken as jest.Mock).mockReturnValue('access_token');
      (jwtUtil.generateRefreshToken as jest.Mock).mockReturnValue('refresh_token');
      (gmAccountModel.resetFailedLoginAttempts as jest.Mock).mockResolvedValue(undefined);
      (gmAccountModel.updateLastLogin as jest.Mock).mockResolvedValue(undefined);
      (gmAccountModel.createLoginLog as jest.Mock).mockResolvedValue(1);
    });

    it('成功登录应该返回完整的响应', async () => {
      const result = await authService.login(loginRequest, '192.168.1.1', 'Mozilla/5.0');

      expect(result).toEqual({
        access_token: 'access_token',
        refresh_token: 'refresh_token',
        expires_in: 1800,
        user: {
          gm_id: 1,
          username: 'test_gm',
          real_name: '测试GM',
          gm_level: 5,
          permissions: ['*']
        },
        device_token: undefined
      });
    });

    it('用户名不存在应该抛出错误', async () => {
      (gmAccountModel.findGmAccountByUsername as jest.Mock).mockResolvedValue(null);

      await expect(authService.login(loginRequest, '192.168.1.1'))
        .rejects
        .toMatchObject({
          message: '用户名或密码错误',
          statusCode: 401,
          code: 'INVALID_CREDENTIALS'
        });

      expect(gmAccountModel.createLoginLog).toHaveBeenCalledWith(
        expect.objectContaining({
          login_result: 'FAILURE',
          failure_reason: '用户名或密码错误'
        })
      );
    });

    it('账号被禁用应该抛出错误', async () => {
      (gmAccountModel.findGmAccountByUsername as jest.Mock).mockResolvedValue({
        ...mockGmAccount,
        is_active: false
      });

      await expect(authService.login(loginRequest, '192.168.1.1'))
        .rejects
        .toMatchObject({
          message: '账号已禁用',
          statusCode: 403,
          code: 'ACCOUNT_DISABLED'
        });
    });

    it('账号被锁定应该抛出错误', async () => {
      const lockedUntil = new Date(Date.now() + 600000); // 10分钟后
      (gmAccountModel.findGmAccountByUsername as jest.Mock).mockResolvedValue({
        ...mockGmAccount,
        is_locked: true,
        locked_until: lockedUntil
      });

      await expect(authService.login(loginRequest, '192.168.1.1'))
        .rejects
        .toMatchObject({
          statusCode: 403,
          code: 'ACCOUNT_LOCKED'
        });

      expect(gmAccountModel.createLoginLog).toHaveBeenCalledWith(
        expect.objectContaining({
          login_result: 'LOCKED'
        })
      );
    });

    it('密码错误应该增加失败次数', async () => {
      (cryptoUtil.verifyPassword as jest.Mock).mockResolvedValue(false);

      await expect(authService.login(loginRequest, '192.168.1.1'))
        .rejects
        .toMatchObject({
          statusCode: 401,
          code: 'INVALID_CREDENTIALS'
        });

      expect(gmAccountModel.incrementFailedLoginAttempts).toHaveBeenCalledWith(1);
      expect(gmAccountModel.createLoginLog).toHaveBeenCalledWith(
        expect.objectContaining({
          login_result: 'FAILURE',
          failure_reason: '密码错误'
        })
      );
    });

    it('需要2FA时应该返回requires_2fa标志', async () => {
      (gmAccountModel.findGmAccountByUsername as jest.Mock).mockResolvedValue({
        ...mockGmAccount,
        two_factor_enabled: true,
        two_factor_secret: 'SECRET123',
        gm_level: 3
      });

      const result = await authService.login(loginRequest, '192.168.1.1');

      expect(result).toHaveProperty('requires_2fa', true);
    });

    it('2FA验证失败应该抛出错误', async () => {
      (gmAccountModel.findGmAccountByUsername as jest.Mock).mockResolvedValue({
        ...mockGmAccount,
        two_factor_enabled: true,
        two_factor_secret: 'SECRET123',
        gm_level: 3
      });
      (twoFactorUtil.verifyTwoFactorCode as jest.Mock).mockReturnValue(false);

      const requestWith2FA = {
        ...loginRequest,
        two_factor_code: '123456'
      };

      await expect(authService.login(requestWith2FA, '192.168.1.1'))
        .rejects
        .toMatchObject({
          message: '双因素认证码错误',
          statusCode: 401,
          code: '2FA_INVALID'
        });

      expect(gmAccountModel.createLoginLog).toHaveBeenCalledWith(
        expect.objectContaining({
          login_result: 'FAILURE',
          failure_reason: '2FA验证失败',
          two_factor_used: true,
          two_factor_result: 'FAILURE'
        })
      );
    });

    it('2FA验证成功应该正常登录', async () => {
      (gmAccountModel.findGmAccountByUsername as jest.Mock).mockResolvedValue({
        ...mockGmAccount,
        two_factor_enabled: true,
        two_factor_secret: 'SECRET123',
        gm_level: 3
      });
      (twoFactorUtil.verifyTwoFactorCode as jest.Mock).mockReturnValue(true);

      const requestWith2FA = {
        ...loginRequest,
        two_factor_code: '123456'
      };

      const result = await authService.login(requestWith2FA, '192.168.1.1');

      expect(result).toHaveProperty('access_token');
      expect(result).toHaveProperty('refresh_token');
      expect(gmAccountModel.createLoginLog).toHaveBeenCalledWith(
        expect.objectContaining({
          login_result: 'SUCCESS',
          two_factor_used: true,
          two_factor_result: 'SUCCESS'
        })
      );
    });

    it('信任设备应该跳过2FA', async () => {
      (gmAccountModel.findGmAccountByUsername as jest.Mock).mockResolvedValue({
        ...mockGmAccount,
        two_factor_enabled: true,
        two_factor_secret: 'SECRET123',
        gm_level: 3
      });
      (gmAccountModel.findTrustedDeviceByToken as jest.Mock).mockResolvedValue({
        device_id: 1,
        gm_id: 1,
        device_token: 'trusted_token',
        is_active: true
      });
      (gmAccountModel.updateTrustedDeviceLastUsed as jest.Mock).mockResolvedValue(undefined);

      const requestWithDevice = {
        ...loginRequest,
        device_token: 'trusted_token'
      };

      const result = await authService.login(requestWithDevice, '192.168.1.1');

      expect(result).toHaveProperty('access_token');
      expect(result).not.toHaveProperty('requires_2fa');
      expect(gmAccountModel.updateTrustedDeviceLastUsed).toHaveBeenCalledWith(1);
    });

    it('记住设备应该创建设备令牌', async () => {
      (cryptoUtil.generateDeviceToken as jest.Mock).mockReturnValue('new_device_token');
      (gmAccountModel.createTrustedDevice as jest.Mock).mockResolvedValue(1);

      const requestWithRemember = {
        ...loginRequest,
        remember_device: true
      };

      const result = await authService.login(requestWithRemember, '192.168.1.1', 'Mozilla/5.0');

      expect(result.device_token).toBe('new_device_token');
      expect(gmAccountModel.createTrustedDevice).toHaveBeenCalledWith({
        gm_user: 'test_gm',
        gm_id: 1,
        device_fingerprint: 'abc123',
        device_token: 'new_device_token',
        device_name: 'Mozilla/5.0',
        ip_address: '192.168.1.1',
        user_agent: 'Mozilla/5.0'
      });
    });

    it('登录成功应该重置失败次数', async () => {
      await authService.login(loginRequest, '192.168.1.1');

      expect(gmAccountModel.resetFailedLoginAttempts).toHaveBeenCalledWith(1);
    });

    it('登录成功应该更新最后登录信息', async () => {
      await authService.login(loginRequest, '192.168.1.1');

      expect(gmAccountModel.updateLastLogin).toHaveBeenCalledWith(1, '192.168.1.1');
    });

    it('超级管理员应该拥有所有权限', async () => {
      (gmAccountModel.findGmAccountByUsername as jest.Mock).mockResolvedValue({
        ...mockGmAccount,
        gm_level: 5
      });

      const result = await authService.login(loginRequest, '192.168.1.1');

      expect(result.user.permissions).toEqual(['*']);
    });

    it('普通GM应该拥有基础权限', async () => {
      (gmAccountModel.findGmAccountByUsername as jest.Mock).mockResolvedValue({
        ...mockGmAccount,
        gm_level: 2
      });

      const result = await authService.login(loginRequest, '192.168.1.1');

      expect(result.user.permissions).toContain('player:read');
      expect(result.user.permissions).toContain('system:logs');
      expect(result.user.permissions).not.toContain('*');
    });

    it('高级GM应该拥有扩展权限', async () => {
      (gmAccountModel.findGmAccountByUsername as jest.Mock).mockResolvedValue({
        ...mockGmAccount,
        gm_level: 4
      });

      const result = await authService.login(loginRequest, '192.168.1.1');

      expect(result.user.permissions).toContain('account:ban');
      expect(result.user.permissions).toContain('account:unban');
      expect(result.user.permissions).toContain('item:grant');
    });
  });

  describe('refreshToken', () => {
    const mockRefreshPayload = {
      gm_id: 1,
      username: 'test_gm',
      gm_level: 5,
      type: 'refresh',
      iat: Math.floor(Date.now() / 1000),
      exp: Math.floor(Date.now() / 1000) + 604800
    };

    beforeEach(() => {
      (jwtUtil.verifyToken as jest.Mock).mockReturnValue(mockRefreshPayload);
      (gmAccountModel.findGmAccountById as jest.Mock).mockResolvedValue(mockGmAccount);
      (jwtUtil.generateAccessToken as jest.Mock).mockReturnValue('new_access_token');
    });

    it('应该成功刷新Token', async () => {
      const result = await authService.refreshToken('valid_refresh_token');

      expect(result).toEqual({
        access_token: 'new_access_token',
        expires_in: 1800
      });

      expect(jwtUtil.verifyToken).toHaveBeenCalledWith('valid_refresh_token');
      expect(gmAccountModel.findGmAccountById).toHaveBeenCalledWith(1);
    });

    it('无效的Token类型应该抛出错误', async () => {
      (jwtUtil.verifyToken as jest.Mock).mockReturnValue({
        ...mockRefreshPayload,
        type: 'access' // 错误的类型
      });

      await expect(authService.refreshToken('invalid_token'))
        .rejects
        .toMatchObject({
          message: '无效的Refresh Token',
          statusCode: 401,
          code: 'INVALID_REFRESH_TOKEN'
        });
    });

    it('账号不存在应该抛出错误', async () => {
      (gmAccountModel.findGmAccountById as jest.Mock).mockResolvedValue(null);

      await expect(authService.refreshToken('valid_refresh_token'))
        .rejects
        .toMatchObject({
          message: '账号不存在或已禁用',
          statusCode: 403,
          code: 'ACCOUNT_INVALID'
        });
    });

    it('账号被禁用应该抛出错误', async () => {
      (gmAccountModel.findGmAccountById as jest.Mock).mockResolvedValue({
        ...mockGmAccount,
        is_active: false
      });

      await expect(authService.refreshToken('valid_refresh_token'))
        .rejects
        .toMatchObject({
          statusCode: 403,
          code: 'ACCOUNT_INVALID'
        });
    });
  });

  describe('logout', () => {
    it('应该成功登出', async () => {
      await expect(authService.logout(1)).resolves.toBeUndefined();
    });
  });

  describe('setupTwoFactor', () => {
    beforeEach(() => {
      (twoFactorUtil.generateTwoFactorSecret as jest.Mock).mockReturnValue({
        secret: 'SECRET123',
        otpauthUrl: 'otpauth://totp/test'
      });
      (twoFactorUtil.generateQRCode as jest.Mock).mockResolvedValue('data:image/png;base64,...');
      (twoFactorUtil.generateBackupCodes as jest.Mock).mockReturnValue(['CODE1', 'CODE2']);
    });

    it('应该生成2FA设置信息', async () => {
      const result = await authService.setupTwoFactor(1, 'test_gm');

      expect(result).toEqual({
        secret: 'SECRET123',
        qr_code: 'data:image/png;base64,...',
        backup_codes: ['CODE1', 'CODE2']
      });

      expect(twoFactorUtil.generateTwoFactorSecret).toHaveBeenCalledWith('test_gm');
      expect(twoFactorUtil.generateQRCode).toHaveBeenCalledWith('otpauth://totp/test');
      expect(twoFactorUtil.generateBackupCodes).toHaveBeenCalledWith(10);
    });
  });

  describe('verifyAndEnableTwoFactor', () => {
    beforeEach(() => {
      (twoFactorUtil.verifyTwoFactorCode as jest.Mock).mockReturnValue(true);
      (gmAccountModel.enableTwoFactor as jest.Mock).mockResolvedValue(undefined);
    });

    it('验证成功应该启用2FA', async () => {
      const secret = 'SECRET123';
      const code = '123456';
      const backupCodes = ['CODE1', 'CODE2'];

      await authService.verifyAndEnableTwoFactor(1, secret, code, backupCodes);

      expect(twoFactorUtil.verifyTwoFactorCode).toHaveBeenCalledWith(secret, code);
      expect(gmAccountModel.enableTwoFactor).toHaveBeenCalledWith(1, secret, JSON.stringify(backupCodes));
    });

    it('验证失败应该抛出错误', async () => {
      (twoFactorUtil.verifyTwoFactorCode as jest.Mock).mockReturnValue(false);

      await expect(authService.verifyAndEnableTwoFactor(1, 'SECRET', '000000', []))
        .rejects
        .toMatchObject({
          message: '验证码错误',
          statusCode: 400,
          code: '2FA_CODE_INVALID'
        });

      expect(gmAccountModel.enableTwoFactor).not.toHaveBeenCalled();
    });
  });

  describe('disableTwoFactorService', () => {
    beforeEach(() => {
      (gmAccountModel.findGmAccountById as jest.Mock).mockResolvedValue(mockGmAccount);
      (cryptoUtil.verifyPassword as jest.Mock).mockResolvedValue(true);
      (gmAccountModel.disableTwoFactor as jest.Mock).mockResolvedValue(undefined);
    });

    it('密码验证成功应该禁用2FA', async () => {
      await authService.disableTwoFactorService(1, 'TestPassword123!');

      expect(gmAccountModel.findGmAccountById).toHaveBeenCalledWith(1);
      expect(cryptoUtil.verifyPassword).toHaveBeenCalledWith('TestPassword123!', mockGmAccount.password_hash);
      expect(gmAccountModel.disableTwoFactor).toHaveBeenCalledWith(1);
    });

    it('账号不存在应该抛出错误', async () => {
      (gmAccountModel.findGmAccountById as jest.Mock).mockResolvedValue(null);

      await expect(authService.disableTwoFactorService(1, 'password'))
        .rejects
        .toMatchObject({
          message: '账号不存在',
          statusCode: 404,
          code: 'ACCOUNT_NOT_FOUND'
        });
    });

    it('密码错误应该抛出错误', async () => {
      (cryptoUtil.verifyPassword as jest.Mock).mockResolvedValue(false);

      await expect(authService.disableTwoFactorService(1, 'WrongPassword'))
        .rejects
        .toMatchObject({
          message: '密码错误',
          statusCode: 401,
          code: 'INVALID_PASSWORD'
        });

      expect(gmAccountModel.disableTwoFactor).not.toHaveBeenCalled();
    });
  });

  describe('getTrustedDevices', () => {
    it('应该返回信任设备列表', async () => {
      const mockDevices = [
        { device_id: 1, device_name: 'Device 1' },
        { device_id: 2, device_name: 'Device 2' }
      ];
      (gmAccountModel.findTrustedDevicesByGmId as jest.Mock).mockResolvedValue(mockDevices);

      const result = await authService.getTrustedDevices(1);

      expect(result).toEqual(mockDevices);
      expect(gmAccountModel.findTrustedDevicesByGmId).toHaveBeenCalledWith(1);
    });
  });

  describe('revokeTrustedDeviceService', () => {
    it('应该撤销信任设备', async () => {
      (gmAccountModel.revokeTrustedDevice as jest.Mock).mockResolvedValue(undefined);

      await authService.revokeTrustedDeviceService(1, 123);

      expect(gmAccountModel.revokeTrustedDevice).toHaveBeenCalledWith(123, 1);
    });
  });
});
