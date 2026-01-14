/**
 * Dashboard 相关 API
 */

import { http } from '@/utils/request';
import type {
  DashboardStats,
  RecentOperation,
  SystemNotification,
  PaginationParams,
} from '@/types';

export const dashboardApi = {
  // 获取统计数据
  getStats: () => {
    return http.get<DashboardStats>('/dashboard/stats');
  },

  // 获取最近操作
  getRecentOperations: (params?: { limit?: number }) => {
    return http.get<RecentOperation[]>('/dashboard/recent-operations', { params });
  },

  // 获取系统通知
  getNotifications: (params?: PaginationParams & { unread_only?: boolean }) => {
    return http.get<SystemNotification[]>('/dashboard/notifications', { params });
  },

  // 标记通知为已读
  markNotificationAsRead: (notificationId: string) => {
    return http.put(`/dashboard/notifications/${notificationId}/read`);
  },

  // 标记所有通知为已读
  markAllNotificationsAsRead: () => {
    return http.put('/dashboard/notifications/read-all');
  },
};
