/**
 * auth.controller.ts 单元测试
 */

import { Request, Response } from 'express';
import * as authController from '../auth.controller';
import * as authService from '../../services/auth.service';
import { createApiError } from '../../middlewares/errorHandler';

// Mock依赖
jest.mock('../../services/auth.service');
jest.mock('../../middlewares/errorHandler');
jest.mock('../../utils/logger.util', () => ({
  logger: {
    info: jest.fn(),
    error: jest.fn()
  }
}));

describe('auth.controller', () => {
  let mockRequest: Partial<Request>;
  let mockResponse: Partial<Response>;
  let jsonMock: jest.Mock;
  let statusMock: jest.Mock;

  beforeEach(() => {
    jsonMock = jest.fn();
    statusMock = jest.fn().mockReturnValue({ json: jsonMock });

    mockRequest = {
      body: {},
      params: {},
      headers: {},
      ip: '192.168.1.1',
      connection: {} as any
    };

    mockResponse = {
      status: statusMock,
      json: jsonMock
    };

    jest.clearAllMocks();
  });

  describe('login', () => {
    it('成功登录应该返回200和token信息', async () => {
      const mockLoginResult = {
        access_token: 'access_token',
        refresh_token: 'refresh_token',
        expires_in: 1800,
        user: {
          gm_id: 1,
          username: 'test_gm',
          real_name: '测试GM',
          gm_level: 5,
          permissions: ['*']
        }
      };

      mockRequest.body = {
        username: 'test_gm',
        password: 'TestPassword123!'
      };

      mockRequest.headers = {
        'user-agent': 'Mozilla/5.0'
      };

      (authService.login as jest.Mock).mockResolvedValue(mockLoginResult);

      await authController.login(mockRequest as Request, mockResponse as Response);

      expect(authService.login).toHaveBeenCalledWith(
        {
          username: 'test_gm',
          password: 'TestPassword123!',
          device_fingerprint: undefined,
          remember_device: undefined,
          two_factor_code: undefined,
          device_token: undefined
        },
        '192.168.1.1',
        'Mozilla/5.0'
      );

      expect(statusMock).toHaveBeenCalledWith(200);
      expect(jsonMock).toHaveBeenCalledWith({
        code: 200,
        message: '登录成功',
        data: mockLoginResult
      });
    });

    it('需要2FA时应该返回相应消息', async () => {
      const mockResult = {
        requires_2fa: true
      };

      mockRequest.body = {
        username: 'test_gm',
        password: 'TestPassword123!'
      };

      (authService.login as jest.Mock).mockResolvedValue(mockResult);

      await authController.login(mockRequest as Request, mockResponse as Response);

      expect(jsonMock).toHaveBeenCalledWith({
        code: 200,
        message: '需要双因素认证',
        data: mockResult
      });
    });

    it('缺少用户名应该抛出400错误', async () => {
      mockRequest.body = {
        password: 'TestPassword123!'
      };

      const mockError = { statusCode: 400, message: '用户名和密码不能为空' };
      (createApiError as jest.Mock).mockReturnValue(mockError);

      await expect(authController.login(mockRequest as Request, mockResponse as Response))
        .rejects
        .toEqual(mockError);

      expect(createApiError).toHaveBeenCalledWith('用户名和密码不能为空', 400, 'MISSING_PARAMETERS');
    });

    it('缺少密码应该抛出400错误', async () => {
      mockRequest.body = {
        username: 'test_gm'
      };

      const mockError = { statusCode: 400, message: '用户名和密码不能为空' };
      (createApiError as jest.Mock).mockReturnValue(mockError);

      await expect(authController.login(mockRequest as Request, mockResponse as Response))
        .rejects
        .toEqual(mockError);
    });

    it('应该正确处理IPv6地址', async () => {
      mockRequest.ip = '::ffff:192.168.1.1';
      mockRequest.body = {
        username: 'test_gm',
        password: 'TestPassword123!'
      };

      (authService.login as jest.Mock).mockResolvedValue({
        access_token: 'token',
        refresh_token: 'refresh',
        expires_in: 1800,
        user: {} as any
      });

      await authController.login(mockRequest as Request, mockResponse as Response);

      expect(authService.login).toHaveBeenCalledWith(
        expect.anything(),
        '192.168.1.1', // IPv6前缀应该被移除
        expect.anything()
      );
    });
  });

  describe('refresh', () => {
    it('成功刷新应该返回新的access token', async () => {
      mockRequest.headers = {
        authorization: 'Bearer refresh_token_here'
      };

      const mockResult = {
        access_token: 'new_access_token',
        expires_in: 1800
      };

      (authService.refreshToken as jest.Mock).mockResolvedValue(mockResult);

      await authController.refresh(mockRequest as Request, mockResponse as Response);

      expect(authService.refreshToken).toHaveBeenCalledWith('refresh_token_here');
      expect(statusMock).toHaveBeenCalledWith(200);
      expect(jsonMock).toHaveBeenCalledWith({
        code: 200,
        message: 'Token刷新成功',
        data: mockResult
      });
    });

    it('缺少Authorization header应该抛出401错误', async () => {
      mockRequest.headers = {};

      const mockError = { statusCode: 401, message: '缺少Refresh Token' };
      (createApiError as jest.Mock).mockReturnValue(mockError);

      await expect(authController.refresh(mockRequest as Request, mockResponse as Response))
        .rejects
        .toEqual(mockError);

      expect(createApiError).toHaveBeenCalledWith('缺少Refresh Token', 401, 'MISSING_TOKEN');
    });

    it('错误的Token格式应该抛出401错误', async () => {
      mockRequest.headers = {
        authorization: 'InvalidFormat'
      };

      const mockError = { statusCode: 401, message: 'Token格式错误' };
      (createApiError as jest.Mock).mockReturnValue(mockError);

      await expect(authController.refresh(mockRequest as Request, mockResponse as Response))
        .rejects
        .toEqual(mockError);

      expect(createApiError).toHaveBeenCalledWith('Token格式错误', 401, 'INVALID_TOKEN_FORMAT');
    });

    it('非Bearer类型应该抛出401错误', async () => {
      mockRequest.headers = {
        authorization: 'Basic token_here'
      };

      const mockError = { statusCode: 401, message: 'Token格式错误' };
      (createApiError as jest.Mock).mockReturnValue(mockError);

      await expect(authController.refresh(mockRequest as Request, mockResponse as Response))
        .rejects
        .toEqual(mockError);
    });
  });

  describe('logout', () => {
    it('成功登出应该返回200', async () => {
      mockRequest.user = {
        gm_id: 1,
        username: 'test_gm',
        gm_level: 5,
        permissions: ['*']
      };

      (authService.logout as jest.Mock).mockResolvedValue(undefined);

      await authController.logout(mockRequest as Request, mockResponse as Response);

      expect(authService.logout).toHaveBeenCalledWith(1);
      expect(statusMock).toHaveBeenCalledWith(200);
      expect(jsonMock).toHaveBeenCalledWith({
        code: 200,
        message: '登出成功'
      });
    });

    it('未认证用户应该抛出401错误', async () => {
      mockRequest.user = undefined;

      const mockError = { statusCode: 401, message: '未认证' };
      (createApiError as jest.Mock).mockReturnValue(mockError);

      await expect(authController.logout(mockRequest as Request, mockResponse as Response))
        .rejects
        .toEqual(mockError);

      expect(authService.logout).not.toHaveBeenCalled();
    });
  });

  describe('setupTwoFactor', () => {
    it('成功设置2FA应该返回密钥和QR码', async () => {
      mockRequest.user = {
        gm_id: 1,
        username: 'test_gm',
        gm_level: 5,
        permissions: ['*']
      };

      const mockResult = {
        secret: 'SECRET123',
        qr_code: 'data:image/png;base64,...',
        backup_codes: ['CODE1', 'CODE2']
      };

      (authService.setupTwoFactor as jest.Mock).mockResolvedValue(mockResult);

      await authController.setupTwoFactor(mockRequest as Request, mockResponse as Response);

      expect(authService.setupTwoFactor).toHaveBeenCalledWith(1, 'test_gm');
      expect(statusMock).toHaveBeenCalledWith(200);
      expect(jsonMock).toHaveBeenCalledWith({
        code: 200,
        message: '2FA设置成功',
        data: mockResult
      });
    });

    it('未认证用户应该抛出401错误', async () => {
      mockRequest.user = undefined;

      const mockError = { statusCode: 401, message: '未认证' };
      (createApiError as jest.Mock).mockReturnValue(mockError);

      await expect(authController.setupTwoFactor(mockRequest as Request, mockResponse as Response))
        .rejects
        .toEqual(mockError);
    });
  });

  describe('verifyTwoFactor', () => {
    it('成功验证应该启用2FA', async () => {
      mockRequest.user = {
        gm_id: 1,
        username: 'test_gm',
        gm_level: 5,
        permissions: ['*']
      };

      mockRequest.body = {
        secret: 'SECRET123',
        code: '123456',
        backup_codes: ['CODE1', 'CODE2']
      };

      (authService.verifyAndEnableTwoFactor as jest.Mock).mockResolvedValue(undefined);

      await authController.verifyTwoFactor(mockRequest as Request, mockResponse as Response);

      expect(authService.verifyAndEnableTwoFactor).toHaveBeenCalledWith(1, 'SECRET123', '123456', ['CODE1', 'CODE2']);
      expect(statusMock).toHaveBeenCalledWith(200);
      expect(jsonMock).toHaveBeenCalledWith({
        code: 200,
        message: '2FA验证成功，已启用'
      });
    });

    it('缺少参数应该抛出400错误', async () => {
      mockRequest.user = {
        gm_id: 1,
        username: 'test_gm',
        gm_level: 5,
        permissions: ['*']
      };

      mockRequest.body = {
        secret: 'SECRET123'
        // 缺少code和backup_codes
      };

      const mockError = { statusCode: 400, message: '缺少必要参数' };
      (createApiError as jest.Mock).mockReturnValue(mockError);

      await expect(authController.verifyTwoFactor(mockRequest as Request, mockResponse as Response))
        .rejects
        .toEqual(mockError);
    });
  });

  describe('disableTwoFactor', () => {
    it('成功禁用2FA', async () => {
      mockRequest.user = {
        gm_id: 1,
        username: 'test_gm',
        gm_level: 5,
        permissions: ['*']
      };

      mockRequest.body = {
        password: 'TestPassword123!'
      };

      (authService.disableTwoFactorService as jest.Mock).mockResolvedValue(undefined);

      await authController.disableTwoFactor(mockRequest as Request, mockResponse as Response);

      expect(authService.disableTwoFactorService).toHaveBeenCalledWith(1, 'TestPassword123!');
      expect(statusMock).toHaveBeenCalledWith(200);
      expect(jsonMock).toHaveBeenCalledWith({
        code: 200,
        message: '2FA已禁用'
      });
    });

    it('缺少密码应该抛出400错误', async () => {
      mockRequest.user = {
        gm_id: 1,
        username: 'test_gm',
        gm_level: 5,
        permissions: ['*']
      };

      mockRequest.body = {};

      const mockError = { statusCode: 400, message: '需要密码验证' };
      (createApiError as jest.Mock).mockReturnValue(mockError);

      await expect(authController.disableTwoFactor(mockRequest as Request, mockResponse as Response))
        .rejects
        .toEqual(mockError);
    });
  });

  describe('getTrustedDevices', () => {
    it('成功获取设备列表', async () => {
      mockRequest.user = {
        gm_id: 1,
        username: 'test_gm',
        gm_level: 5,
        permissions: ['*']
      };

      const mockDevices = [
        { device_id: 1, device_name: 'Device 1' },
        { device_id: 2, device_name: 'Device 2' }
      ];

      (authService.getTrustedDevices as jest.Mock).mockResolvedValue(mockDevices);

      await authController.getTrustedDevices(mockRequest as Request, mockResponse as Response);

      expect(authService.getTrustedDevices).toHaveBeenCalledWith(1);
      expect(statusMock).toHaveBeenCalledWith(200);
      expect(jsonMock).toHaveBeenCalledWith({
        code: 200,
        message: '查询成功',
        data: { devices: mockDevices }
      });
    });
  });

  describe('revokeTrustedDevice', () => {
    it('成功撤销设备', async () => {
      mockRequest.user = {
        gm_id: 1,
        username: 'test_gm',
        gm_level: 5,
        permissions: ['*']
      };

      mockRequest.params = {
        device_id: '123'
      };

      (authService.revokeTrustedDeviceService as jest.Mock).mockResolvedValue(undefined);

      await authController.revokeTrustedDevice(mockRequest as Request, mockResponse as Response);

      expect(authService.revokeTrustedDeviceService).toHaveBeenCalledWith(1, 123);
      expect(statusMock).toHaveBeenCalledWith(200);
      expect(jsonMock).toHaveBeenCalledWith({
        code: 200,
        message: '设备已撤销'
      });
    });

    it('无效的设备ID应该抛出400错误', async () => {
      mockRequest.user = {
        gm_id: 1,
        username: 'test_gm',
        gm_level: 5,
        permissions: ['*']
      };

      mockRequest.params = {
        device_id: 'invalid'
      };

      const mockError = { statusCode: 400, message: '无效的设备ID' };
      (createApiError as jest.Mock).mockReturnValue(mockError);

      await expect(authController.revokeTrustedDevice(mockRequest as Request, mockResponse as Response))
        .rejects
        .toEqual(mockError);
    });
  });

  describe('getCurrentUser', () => {
    it('成功获取当前用户信息', async () => {
      mockRequest.user = {
        gm_id: 1,
        username: 'test_gm',
        gm_level: 5,
        permissions: ['*']
      };

      await authController.getCurrentUser(mockRequest as Request, mockResponse as Response);

      expect(statusMock).toHaveBeenCalledWith(200);
      expect(jsonMock).toHaveBeenCalledWith({
        code: 200,
        message: '查询成功',
        data: {
          gm_id: 1,
          username: 'test_gm',
          gm_level: 5,
          permissions: ['*']
        }
      });
    });

    it('未认证用户应该抛出401错误', async () => {
      mockRequest.user = undefined;

      const mockError = { statusCode: 401, message: '未认证' };
      (createApiError as jest.Mock).mockReturnValue(mockError);

      await expect(authController.getCurrentUser(mockRequest as Request, mockResponse as Response))
        .rejects
        .toEqual(mockError);
    });
  });
});
