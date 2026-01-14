/**
 * 控制台页面
 */

import { FC, useEffect, useState } from 'react';
import { Row, Col, Space, message } from 'antd';
import { PageHeader } from '@/components/common';
import {
  StatsCards,
  QuickActions,
  RecentOperations,
  SystemNotifications,
} from '@/components/dashboard';
import { dashboardApi } from '@/services/dashboard';
import { useAuthStore } from '@/stores/useAuthStore';
import type { DashboardStats, RecentOperation, SystemNotification } from '@/types';

const Dashboard: FC = () => {
  const { isAuthenticated } = useAuthStore();
  const [stats, setStats] = useState<DashboardStats | null>(null);
  const [operations, setOperations] = useState<RecentOperation[]>([]);
  const [notifications, setNotifications] = useState<SystemNotification[]>([]);
  const [loading, setLoading] = useState({
    stats: false,
    operations: false,
    notifications: false,
  });

  // 获取统计数据
  const fetchStats = async () => {
    setLoading((prev) => ({ ...prev, stats: true }));
    try {
      const data = await dashboardApi.getStats();
      setStats(data);
    } catch (error: any) {
      message.error(error.response?.data?.message || '获取统计数据失败');
    } finally {
      setLoading((prev) => ({ ...prev, stats: false }));
    }
  };

  // 获取最近操作
  const fetchOperations = async () => {
    setLoading((prev) => ({ ...prev, operations: true }));
    try {
      const data = await dashboardApi.getRecentOperations({ limit: 10 });
      setOperations(data);
    } catch (error: any) {
      message.error(error.response?.data?.message || '获取操作记录失败');
    } finally {
      setLoading((prev) => ({ ...prev, operations: false }));
    }
  };

  // 获取系统通知
  const fetchNotifications = async () => {
    setLoading((prev) => ({ ...prev, notifications: true }));
    try {
      const data = await dashboardApi.getNotifications({ page: 1, page_size: 10 });
      console.log('[Dashboard] Notifications raw data:', data);
      console.log('[Dashboard] Data type:', typeof data, 'isArray:', Array.isArray(data));
      console.log('[Dashboard] First notification:', data?.[0]);
      // data 本身就是数组
      setNotifications(data || []);
    } catch (error: any) {
      console.error('[Dashboard] Failed to fetch notifications:', error);
      message.error(error.response?.data?.message || '获取系统通知失败');
    } finally {
      setLoading((prev) => ({ ...prev, notifications: false }));
    }
  };

  // 标记通知为已读
  const handleMarkAsRead = async (notificationId: string) => {
    try {
      await dashboardApi.markNotificationAsRead(notificationId);
      setNotifications((prev) =>
        prev.map((n) =>
          n.notification_id === notificationId ? { ...n, read: true } : n
        )
      );
      message.success('已标记为已读');
    } catch (error: any) {
      message.error(error.response?.data?.message || '操作失败');
    }
  };

  // 标记所有通知为已读
  const handleMarkAllAsRead = async () => {
    try {
      await dashboardApi.markAllNotificationsAsRead();
      setNotifications((prev) => prev.map((n) => ({ ...n, read: true })));
      message.success('已全部标记为已读');
    } catch (error: any) {
      message.error(error.response?.data?.message || '操作失败');
    }
  };

  // 初始化数据
  useEffect(() => {
    // 只有在已登录时才执行 API 调用
    if (!isAuthenticated) {
      console.log('[Dashboard] Not authenticated, skipping API calls');
      return;
    }

    console.log('[Dashboard] Authenticated, fetching data');
    fetchStats();
    fetchOperations();
    fetchNotifications();

    // 定时刷新统计数据（每30秒）
    const statsInterval = setInterval(fetchStats, 30000);

    return () => {
      clearInterval(statsInterval);
    };
  }, [isAuthenticated]);

  return (
    <div>
      <PageHeader title="控制台" subTitle="系统概览和快速操作" />

      <Space direction="vertical" size="large" style={{ width: '100%' }}>
        {/* 统计卡片 */}
        <StatsCards stats={stats} loading={loading.stats} />

        {/* 快捷操作 */}
        <QuickActions />

        {/* 最近操作和系统通知 */}
        <Row gutter={[16, 16]}>
          <Col xs={24} xl={16}>
            <RecentOperations
              operations={operations}
              loading={loading.operations}
            />
          </Col>
          <Col xs={24} xl={8}>
            <SystemNotifications
              notifications={notifications}
              loading={loading.notifications}
              onMarkAsRead={handleMarkAsRead}
              onMarkAllAsRead={handleMarkAllAsRead}
            />
          </Col>
        </Row>
      </Space>
    </div>
  );
};

export default Dashboard;
