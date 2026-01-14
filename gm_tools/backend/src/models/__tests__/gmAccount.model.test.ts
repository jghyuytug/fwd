/**
 * gmAccount.model.ts 单元测试
 * 使用mock数据库连接
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
  enableTwoFactor,
  disableTwoFactor,
  GmAccount,
  TrustedDevice
} from '../gmAccount.model';

// Mock数据库连接池
jest.mock('../../config/database', () => ({
  gmToolsPool: {
    query: jest.fn()
  }
}));

import { gmToolsPool } from '../../config/database';

describe('gmAccount.model', () => {
  const mockGmAccount: GmAccount = {
    gm_id: 1,
    username: 'test_gm',
    password_hash: '$2b$10$hash',
    real_name: '测试GM',
    email: 'test@example.com',
    phone: '13800138000',
    gm_level: 5,
    department: '技术部',
    two_factor_enabled: false,
    two_factor_secret: null,
    backup_codes: null,
    is_active: true,
    is_locked: false,
    locked_until: null,
    failed_login_attempts: 0,
    created_at: new Date('2024-01-01'),
    created_by: 'admin',
    updated_at: new Date('2024-01-01'),
    updated_by: 'admin',
    last_login_at: null,
    last_login_ip: null
  };

  const mockTrustedDevice: TrustedDevice = {
    device_id: 1,
    gm_user: 'test_gm',
    gm_id: 1,
    device_fingerprint: 'abc123',
    device_token: 'token123',
    device_name: 'Chrome on Windows',
    ip_address: '192.168.1.1',
    user_agent: 'Mozilla/5.0...',
    created_at: new Date(),
    expires_at: new Date(Date.now() + 30 * 24 * 60 * 60 * 1000),
    last_used_at: null,
    is_active: true
  };

  beforeEach(() => {
    jest.clearAllMocks();
  });

  describe('findGmAccountByUsername', () => {
    it('应该根据用户名查找GM账号', async () => {
      (gmToolsPool.query as jest.Mock).mockResolvedValue([[mockGmAccount], []]);

      const result = await findGmAccountByUsername('test_gm');

      expect(result).toEqual(mockGmAccount);
      expect(gmToolsPool.query).toHaveBeenCalledWith(
        'SELECT * FROM gm_accounts WHERE username = ? AND is_active = 1',
        ['test_gm']
      );
    });

    it('账号不存在时应该返回null', async () => {
      (gmToolsPool.query as jest.Mock).mockResolvedValue([[], []]);

      const result = await findGmAccountByUsername('nonexistent');

      expect(result).toBeNull();
    });

    it('数据库错误时应该抛出异常', async () => {
      (gmToolsPool.query as jest.Mock).mockRejectedValue(new Error('DB Error'));

      await expect(findGmAccountByUsername('test_gm')).rejects.toThrow('DB Error');
    });
  });

  describe('findGmAccountById', () => {
    it('应该根据ID查找GM账号', async () => {
      (gmToolsPool.query as jest.Mock).mockResolvedValue([[mockGmAccount], []]);

      const result = await findGmAccountById(1);

      expect(result).toEqual(mockGmAccount);
      expect(gmToolsPool.query).toHaveBeenCalledWith(
        'SELECT * FROM gm_accounts WHERE gm_id = ?',
        [1]
      );
    });

    it('账号不存在时应该返回null', async () => {
      (gmToolsPool.query as jest.Mock).mockResolvedValue([[], []]);

      const result = await findGmAccountById(999);

      expect(result).toBeNull();
    });
  });

  describe('incrementFailedLoginAttempts', () => {
    it('应该增加失败登录次数', async () => {
      (gmToolsPool.query as jest.Mock).mockResolvedValue([{ affectedRows: 1 }, []]);

      await incrementFailedLoginAttempts(1);

      expect(gmToolsPool.query).toHaveBeenCalledWith(
        expect.stringContaining('UPDATE gm_accounts'),
        [5, 5, 600, 1]
      );
    });

    it('达到最大次数时应该锁定账号', async () => {
      (gmToolsPool.query as jest.Mock).mockResolvedValue([{ affectedRows: 1 }, []]);

      await incrementFailedLoginAttempts(1);

      const query = (gmToolsPool.query as jest.Mock).mock.calls[0][0];
      expect(query).toContain('is_locked = CASE');
      expect(query).toContain('locked_until = CASE');
    });
  });

  describe('resetFailedLoginAttempts', () => {
    it('应该重置失败登录次数', async () => {
      (gmToolsPool.query as jest.Mock).mockResolvedValue([{ affectedRows: 1 }, []]);

      await resetFailedLoginAttempts(1);

      expect(gmToolsPool.query).toHaveBeenCalledWith(
        expect.stringContaining('UPDATE gm_accounts'),
        [1]
      );

      const query = (gmToolsPool.query as jest.Mock).mock.calls[0][0];
      expect(query).toContain('failed_login_attempts = 0');
      expect(query).toContain('is_locked = 0');
      expect(query).toContain('locked_until = NULL');
    });
  });

  describe('updateLastLogin', () => {
    it('应该更新最后登录信息', async () => {
      (gmToolsPool.query as jest.Mock).mockResolvedValue([{ affectedRows: 1 }, []]);

      await updateLastLogin(1, '192.168.1.1');

      expect(gmToolsPool.query).toHaveBeenCalledWith(
        expect.stringContaining('UPDATE gm_accounts'),
        ['192.168.1.1', 1]
      );

      const query = (gmToolsPool.query as jest.Mock).mock.calls[0][0];
      expect(query).toContain('last_login_at = NOW()');
      expect(query).toContain('last_login_ip = ?');
    });
  });

  describe('createLoginLog', () => {
    it('应该创建成功登录日志', async () => {
      (gmToolsPool.query as jest.Mock).mockResolvedValue([{ insertId: 1 }, []]);

      const logData = {
        gm_user: 'test_gm',
        gm_id: 1,
        login_result: 'SUCCESS' as const,
        ip_address: '192.168.1.1',
        user_agent: 'Mozilla/5.0...'
      };

      const logId = await createLoginLog(logData);

      expect(logId).toBe(1);
      expect(gmToolsPool.query).toHaveBeenCalledWith(
        expect.stringContaining('INSERT INTO gm_login_logs'),
        expect.arrayContaining([
          'test_gm',
          1,
          'SUCCESS',
          null,
          false,
          null,
          '192.168.1.1',
          'Mozilla/5.0...',
          null
        ])
      );
    });

    it('应该创建失败登录日志', async () => {
      (gmToolsPool.query as jest.Mock).mockResolvedValue([{ insertId: 2 }, []]);

      const logData = {
        gm_user: 'test_gm',
        gm_id: 1,
        login_result: 'FAILURE' as const,
        failure_reason: '密码错误',
        ip_address: '192.168.1.1'
      };

      const logId = await createLoginLog(logData);

      expect(logId).toBe(2);
      expect(gmToolsPool.query).toHaveBeenCalledWith(
        expect.stringContaining('INSERT INTO gm_login_logs'),
        expect.arrayContaining(['FAILURE', '密码错误'])
      );
    });

    it('应该记录2FA验证结果', async () => {
      (gmToolsPool.query as jest.Mock).mockResolvedValue([{ insertId: 3 }, []]);

      const logData = {
        gm_user: 'test_gm',
        gm_id: 1,
        login_result: 'SUCCESS' as const,
        two_factor_used: true,
        two_factor_result: 'SUCCESS' as const,
        ip_address: '192.168.1.1'
      };

      await createLoginLog(logData);

      expect(gmToolsPool.query).toHaveBeenCalledWith(
        expect.anything(),
        expect.arrayContaining([true, 'SUCCESS'])
      );
    });
  });

  describe('findTrustedDeviceByToken', () => {
    it('应该根据token查找信任设备', async () => {
      (gmToolsPool.query as jest.Mock).mockResolvedValue([[mockTrustedDevice], []]);

      const result = await findTrustedDeviceByToken('token123');

      expect(result).toEqual(mockTrustedDevice);
      expect(gmToolsPool.query).toHaveBeenCalledWith(
        expect.stringContaining('SELECT * FROM gm_trusted_devices'),
        ['token123']
      );
    });

    it('设备不存在时应该返回null', async () => {
      (gmToolsPool.query as jest.Mock).mockResolvedValue([[], []]);

      const result = await findTrustedDeviceByToken('invalid_token');

      expect(result).toBeNull();
    });

    it('应该只返回激活且未过期的设备', async () => {
      await findTrustedDeviceByToken('token123');

      const query = (gmToolsPool.query as jest.Mock).mock.calls[0][0];
      expect(query).toContain('is_active = 1');
      expect(query).toContain('expires_at > NOW()');
    });
  });

  describe('createTrustedDevice', () => {
    it('应该创建信任设备', async () => {
      (gmToolsPool.query as jest.Mock).mockResolvedValue([{ insertId: 1 }, []]);

      const deviceData = {
        gm_user: 'test_gm',
        gm_id: 1,
        device_fingerprint: 'abc123',
        device_token: 'token123',
        device_name: 'Chrome on Windows',
        ip_address: '192.168.1.1',
        user_agent: 'Mozilla/5.0...'
      };

      const deviceId = await createTrustedDevice(deviceData);

      expect(deviceId).toBe(1);
      expect(gmToolsPool.query).toHaveBeenCalledWith(
        expect.stringContaining('INSERT INTO gm_trusted_devices'),
        expect.arrayContaining([
          'test_gm',
          1,
          'abc123',
          'token123',
          'Chrome on Windows',
          '192.168.1.1',
          'Mozilla/5.0...',
          2592000
        ])
      );
    });
  });

  describe('updateTrustedDeviceLastUsed', () => {
    it('应该更新设备最后使用时间', async () => {
      (gmToolsPool.query as jest.Mock).mockResolvedValue([{ affectedRows: 1 }, []]);

      await updateTrustedDeviceLastUsed(1);

      expect(gmToolsPool.query).toHaveBeenCalledWith(
        'UPDATE gm_trusted_devices SET last_used_at = NOW() WHERE device_id = ?',
        [1]
      );
    });
  });

  describe('findTrustedDevicesByGmId', () => {
    it('应该查找用户的所有信任设备', async () => {
      (gmToolsPool.query as jest.Mock).mockResolvedValue([[mockTrustedDevice], []]);

      const devices = await findTrustedDevicesByGmId(1);

      expect(devices).toEqual([mockTrustedDevice]);
      expect(gmToolsPool.query).toHaveBeenCalledWith(
        expect.stringContaining('SELECT * FROM gm_trusted_devices'),
        [1]
      );
    });

    it('应该按创建时间倒序排列', async () => {
      await findTrustedDevicesByGmId(1);

      const query = (gmToolsPool.query as jest.Mock).mock.calls[0][0];
      expect(query).toContain('ORDER BY created_at DESC');
    });
  });

  describe('revokeTrustedDevice', () => {
    it('应该撤销信任设备', async () => {
      (gmToolsPool.query as jest.Mock).mockResolvedValue([{ affectedRows: 1 }, []]);

      await revokeTrustedDevice(1, 1);

      expect(gmToolsPool.query).toHaveBeenCalledWith(
        'UPDATE gm_trusted_devices SET is_active = 0 WHERE device_id = ? AND gm_id = ?',
        [1, 1]
      );
    });
  });

  describe('enableTwoFactor', () => {
    it('应该启用2FA', async () => {
      (gmToolsPool.query as jest.Mock).mockResolvedValue([{ affectedRows: 1 }, []]);

      const secret = 'SECRET123';
      const backupCodes = JSON.stringify(['CODE1', 'CODE2']);

      await enableTwoFactor(1, secret, backupCodes);

      expect(gmToolsPool.query).toHaveBeenCalledWith(
        expect.stringContaining('UPDATE gm_accounts'),
        [secret, backupCodes, 1]
      );

      const query = (gmToolsPool.query as jest.Mock).mock.calls[0][0];
      expect(query).toContain('two_factor_enabled = 1');
      expect(query).toContain('two_factor_secret = ?');
      expect(query).toContain('backup_codes = ?');
    });
  });

  describe('disableTwoFactor', () => {
    it('应该禁用2FA', async () => {
      (gmToolsPool.query as jest.Mock).mockResolvedValue([{ affectedRows: 1 }, []]);

      await disableTwoFactor(1);

      expect(gmToolsPool.query).toHaveBeenCalledWith(
        expect.stringContaining('UPDATE gm_accounts'),
        [1]
      );

      const query = (gmToolsPool.query as jest.Mock).mock.calls[0][0];
      expect(query).toContain('two_factor_enabled = 0');
      expect(query).toContain('two_factor_secret = NULL');
      expect(query).toContain('backup_codes = NULL');
    });
  });
});
