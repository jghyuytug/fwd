/**
 * 通用类型定义
 */

// API 响应基础结构
export interface ApiResponse<T = any> {
  code: number;
  message: string;
  data: T;
}

// 分页请求参数
export interface PaginationParams {
  page: number;
  page_size: number;
}

// 分页响应数据
export interface PaginationData<T> {
  items: T[];
  total: number;
  page: number;
  page_size: number;
  total_pages: number;
}

// 用户信息
export interface User {
  user_id: number;
  username: string;
  role: string;
  permissions: string[];
  created_at: string;
  last_login?: string;
}

// 登录请求
export interface LoginRequest {
  username: string;
  password: string;
  remember?: boolean;
}

// 登录响应
export interface LoginResponse {
  user: User;
  access_token: string;
  refresh_token: string;
  expires_in: number;
  two_factor_required?: boolean;
  two_factor_token?: string;
}

// 2FA 验证请求
export interface TwoFactorRequest {
  two_factor_token: string;
  code: string;
  remember_device?: boolean;
}

// Token 刷新响应
export interface RefreshTokenResponse {
  access_token: string;
  refresh_token: string;
}

// 角色
export type Role = 'super_admin' | 'admin' | 'gm' | 'support' | 'viewer';

// 权限
export type Permission =
  | 'player.view'
  | 'player.edit'
  | 'item.grant'
  | 'account.ban'
  | 'account.unban'
  | 'account.edit'
  | 'audit.view'
  | 'system.manage'
  | 'system.config';

// 受信任设备
export interface TrustedDevice {
  device_id: string;
  user_agent: string;
  ip_address: string;
  browser?: string;
  os?: string;
  device_name?: string;
  trusted_at: string;
  last_used_at: string;
  expires_at: string;
  is_current?: boolean;
}

// Dashboard 统计数据
export interface DashboardStats {
  online_players: number;
  total_players: number;
  today_registrations: number;
  today_logins: number;
  active_dungeons: number;
  server_load: number; // 0-100
  peak_online_today: number;
  banned_accounts_today: number;
}

// 最近操作记录
export interface RecentOperation {
  operation_id: string;
  operator: string;
  operation_type: string;
  operation_desc: string;
  target_user?: string;
  status: 'success' | 'failed' | 'pending';
  created_at: string;
  ip_address?: string;
}

// 系统通知
export interface SystemNotification {
  notification_id: string;
  title: string;
  content: string;
  type: 'info' | 'warning' | 'error' | 'success';
  read: boolean;
  created_at: string;
  link?: string;
}

// 系统通知响应（包含分页信息）
export interface NotificationsResponse {
  notifications: SystemNotification[];
  total: number;
}

// 重新导出玩家相关类型
export * from './player';
