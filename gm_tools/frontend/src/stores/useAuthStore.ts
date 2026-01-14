/**
 * 认证状态管理
 */

import { create } from 'zustand';
import { persist } from 'zustand/middleware';
import type { User, Permission } from '@/types';
import { tokenStorage } from '@/utils/storage';

interface AuthState {
  // 状态
  user: User | null;
  isAuthenticated: boolean;
  isLoading: boolean;

  // Actions
  setUser: (user: User | null) => void;
  setTokens: (accessToken: string, refreshToken: string, expiresIn: number) => void;
  clearAuth: () => void;
  hasPermission: (permission: Permission) => boolean;
  hasAnyPermission: (permissions: Permission[]) => boolean;
  hasAllPermissions: (permissions: Permission[]) => boolean;
}

export const useAuthStore = create<AuthState>()(
  persist(
    (set, get) => ({
      // 初始状态
      user: null,
      isAuthenticated: false,
      isLoading: false,

      // 设置用户信息
      setUser: (user) =>
        set({
          user,
          isAuthenticated: !!user,
        }),

      // 设置 tokens
      setTokens: (accessToken, refreshToken, expiresIn) => {
        tokenStorage.setTokens(accessToken, refreshToken, expiresIn);
      },

      // 清除认证信息
      clearAuth: () => {
        tokenStorage.clearTokens();
        set({
          user: null,
          isAuthenticated: false,
        });
      },

      // 检查是否有指定权限
      hasPermission: (permission) => {
        const { user } = get();
        if (!user) return false;
        if (user.role === 'super_admin') return true;
        return user.permissions.includes(permission);
      },

      // 检查是否有任一权限
      hasAnyPermission: (permissions) => {
        const { user } = get();
        if (!user) return false;
        if (user.role === 'super_admin') return true;
        return permissions.some((permission) => user.permissions.includes(permission));
      },

      // 检查是否拥有所有权限
      hasAllPermissions: (permissions) => {
        const { user } = get();
        if (!user) return false;
        if (user.role === 'super_admin') return true;
        return permissions.every((permission) => user.permissions.includes(permission));
      },
    }),
    {
      name: 'auth-storage', // LocalStorage key
      partialize: (state) => ({
        user: state.user,
        isAuthenticated: state.isAuthenticated,
      }),
    }
  )
);
