/**
 * LocalStorage 封装
 */

const STORAGE_PREFIX = 'dnf_gm_';

export const storage = {
  // 获取值
  get<T = any>(key: string): T | null {
    try {
      const value = localStorage.getItem(STORAGE_PREFIX + key);
      return value ? JSON.parse(value) : null;
    } catch (error) {
      console.error('Storage get error:', error);
      return null;
    }
  },

  // 设置值
  set<T = any>(key: string, value: T): void {
    try {
      localStorage.setItem(STORAGE_PREFIX + key, JSON.stringify(value));
    } catch (error) {
      console.error('Storage set error:', error);
    }
  },

  // 删除值
  remove(key: string): void {
    try {
      localStorage.removeItem(STORAGE_PREFIX + key);
    } catch (error) {
      console.error('Storage remove error:', error);
    }
  },

  // 清空所有
  clear(): void {
    try {
      Object.keys(localStorage).forEach((key) => {
        if (key.startsWith(STORAGE_PREFIX)) {
          localStorage.removeItem(key);
        }
      });
    } catch (error) {
      console.error('Storage clear error:', error);
    }
  },
};

// Token 管理
export const TOKEN_KEY = 'access_token';
export const REFRESH_TOKEN_KEY = 'refresh_token';
export const TOKEN_EXPIRE_TIME_KEY = 'token_expire_time';

export const tokenStorage = {
  getAccessToken: () => storage.get<string>(TOKEN_KEY),
  setAccessToken: (token: string) => storage.set(TOKEN_KEY, token),
  getRefreshToken: () => storage.get<string>(REFRESH_TOKEN_KEY),
  setRefreshToken: (token: string) => storage.set(REFRESH_TOKEN_KEY, token),
  getTokenExpireTime: () => storage.get<number>(TOKEN_EXPIRE_TIME_KEY),
  setTokenExpireTime: (expireTime: number) => storage.set(TOKEN_EXPIRE_TIME_KEY, expireTime),
  setTokens: (accessToken: string, refreshToken: string, expiresIn: number) => {
    storage.set(TOKEN_KEY, accessToken);
    storage.set(REFRESH_TOKEN_KEY, refreshToken);
    const expireTime = Date.now() + expiresIn * 1000;
    storage.set(TOKEN_EXPIRE_TIME_KEY, expireTime);
  },
  isTokenExpired: () => {
    const expireTime = storage.get<number>(TOKEN_EXPIRE_TIME_KEY);
    if (!expireTime) return true;
    return Date.now() >= expireTime;
  },
  clearTokens: () => {
    storage.remove(TOKEN_KEY);
    storage.remove(REFRESH_TOKEN_KEY);
    storage.remove(TOKEN_EXPIRE_TIME_KEY);
  },
};
