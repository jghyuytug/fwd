/**
 * Dashboard 系统通知组件
 */

import { FC, useState } from 'react';
import { Card, List, Badge, Tag, Space, Typography, Button, Empty } from 'antd';
import {
  BellOutlined,
  InfoCircleOutlined,
  WarningOutlined,
  CloseCircleOutlined,
  CheckCircleOutlined,
  CheckOutlined,
} from '@ant-design/icons';
import type { SystemNotification } from '@/types';
import dayjs from 'dayjs';
import relativeTime from 'dayjs/plugin/relativeTime';
import 'dayjs/locale/zh-cn';

dayjs.extend(relativeTime);
dayjs.locale('zh-cn');

const { Text, Paragraph } = Typography;

interface SystemNotificationsProps {
  notifications: SystemNotification[];
  loading: boolean;
  onMarkAsRead: (notificationId: string) => void;
  onMarkAllAsRead: () => void;
}

const SystemNotifications: FC<SystemNotificationsProps> = ({
  notifications = [],
  loading = false,
  onMarkAsRead,
  onMarkAllAsRead,
}) => {
  const [expandedIds, setExpandedIds] = useState<Set<string>>(new Set());

  // 安全检查：确保 notifications 是数组
  const safeNotifications = Array.isArray(notifications) ? notifications : [];

  // 通知类型图标和颜色
  const getNotificationIcon = (type: SystemNotification['type']) => {
    console.log('[SystemNotifications] getNotificationIcon called with type:', type, 'typeof:', typeof type);

    const iconConfig = {
      info: { icon: <InfoCircleOutlined />, color: '#1890ff' },
      warning: { icon: <WarningOutlined />, color: '#faad14' },
      error: { icon: <CloseCircleOutlined />, color: '#ff4d4f' },
      success: { icon: <CheckCircleOutlined />, color: '#52c41a' },
    };

    // 默认配置
    const defaultConfig = { icon: <InfoCircleOutlined />, color: '#1890ff' };

    // 严格类型检查
    if (!type) {
      console.warn('[SystemNotifications] type is null/undefined, using default');
      return defaultConfig;
    }

    if (typeof type !== 'string') {
      console.warn('[SystemNotifications] type is not a string:', type, 'using default');
      return defaultConfig;
    }

    if (!(type in iconConfig)) {
      console.warn('[SystemNotifications] type not in iconConfig:', type, 'using default');
      return defaultConfig;
    }

    const config = iconConfig[type as keyof typeof iconConfig];
    console.log('[SystemNotifications] returning config:', config);
    return config;
  };

  // 切换通知展开/收起
  const toggleExpanded = (id: string) => {
    const newExpanded = new Set(expandedIds);
    if (newExpanded.has(id)) {
      newExpanded.delete(id);
    } else {
      newExpanded.add(id);
    }
    setExpandedIds(newExpanded);
  };

  // 未读通知数量
  const unreadCount = safeNotifications.filter((n) => !n.read).length;

  return (
    <Card
      title={
        <Space>
          <Badge count={unreadCount} offset={[10, 0]}>
            <BellOutlined />
          </Badge>
          <span>系统通知</span>
        </Space>
      }
      extra={
        unreadCount > 0 && (
          <Button
            type="link"
            size="small"
            icon={<CheckOutlined />}
            onClick={onMarkAllAsRead}
          >
            全部已读
          </Button>
        )
      }
    >
      {safeNotifications.length === 0 ? (
        <Empty description="暂无通知" image={Empty.PRESENTED_IMAGE_SIMPLE} />
      ) : (
        <List
          dataSource={safeNotifications}
          loading={loading}
          renderItem={(notification) => {
            console.log('[SystemNotifications] renderItem called with notification:', notification);

            if (!notification || !notification.notification_id) {
              console.warn('[SystemNotifications] Invalid notification, skipping:', notification);
              return null;
            }

            console.log('[SystemNotifications] Valid notification, getting icon for type:', notification.type);
            const iconConfig = getNotificationIcon(notification.type);
            console.log('[SystemNotifications] Got iconConfig:', iconConfig);
            const isExpanded = expandedIds.has(notification.notification_id);

            return (
              <List.Item
                key={notification.notification_id}
                style={{
                  background: notification.read ? 'transparent' : '#f0f5ff',
                  padding: '12px',
                  cursor: 'pointer',
                }}
                onClick={() => toggleExpanded(notification.notification_id)}
              >
                <List.Item.Meta
                  avatar={
                    <div style={{ color: iconConfig?.color || '#1890ff', fontSize: 24 }}>
                      {iconConfig?.icon || <InfoCircleOutlined />}
                    </div>
                  }
                  title={
                    <Space>
                      <Text strong={!notification.read}>{notification.title}</Text>
                      {!notification.read && (
                        <Badge status="processing" text="未读" />
                      )}
                    </Space>
                  }
                  description={
                    <Space direction="vertical" size="small" style={{ width: '100%' }}>
                      <Paragraph
                        ellipsis={
                          isExpanded
                            ? false
                            : { rows: 2, expandable: false, symbol: '...' }
                        }
                        style={{ marginBottom: 0 }}
                      >
                        {notification.content}
                      </Paragraph>
                      <Space>
                        <Text type="secondary" style={{ fontSize: 12 }}>
                          {dayjs(notification.created_at).fromNow()}
                        </Text>
                        {!notification.read && (
                          <Button
                            type="link"
                            size="small"
                            onClick={(e) => {
                              e.stopPropagation();
                              onMarkAsRead(notification.notification_id);
                            }}
                          >
                            标为已读
                          </Button>
                        )}
                      </Space>
                    </Space>
                  }
                />
              </List.Item>
            );
          }}
          pagination={
            safeNotifications.length > 5
              ? {
                  pageSize: 5,
                  size: 'small',
                  showSizeChanger: false,
                }
              : false
          }
        />
      )}
    </Card>
  );
};

export default SystemNotifications;
