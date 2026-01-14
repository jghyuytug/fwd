/**
 * Dashboard 快捷操作组件
 */

import { FC } from 'react';
import { Card, Row, Col, Button, Space, Typography } from 'antd';
import {
  SearchOutlined,
  GiftOutlined,
  UserAddOutlined,
  StopOutlined,
  FileSearchOutlined,
  SettingOutlined,
  ToolOutlined,
  AuditOutlined,
} from '@ant-design/icons';
import { useNavigate } from 'react-router-dom';
import { useAuthStore } from '@/stores/useAuthStore';

const { Title, Text } = Typography;

const QuickActions: FC = () => {
  const navigate = useNavigate();
  const { hasPermission } = useAuthStore();

  const actions = [
    {
      key: 'player-search',
      title: '玩家查询',
      icon: <SearchOutlined style={{ fontSize: 24 }} />,
      description: '查询玩家信息',
      color: '#1890ff',
      path: '/player',
      permission: 'player.view',
    },
    {
      key: 'item-grant',
      title: '道具发放',
      icon: <GiftOutlined style={{ fontSize: 24 }} />,
      description: '发放游戏道具',
      color: '#52c41a',
      path: '/gm/item-grant',
      permission: 'item.grant',
    },
    {
      key: 'account-manage',
      title: '账号管理',
      icon: <UserAddOutlined style={{ fontSize: 24 }} />,
      description: '管理玩家账号',
      color: '#faad14',
      path: '/gm/account-manage',
      permission: 'account.edit',
    },
    {
      key: 'account-ban',
      title: '账号封禁',
      icon: <StopOutlined style={{ fontSize: 24 }} />,
      description: '封禁违规账号',
      color: '#ff4d4f',
      path: '/gm/account-manage',
      permission: 'account.ban',
    },
    {
      key: 'audit-logs',
      title: '审计日志',
      icon: <AuditOutlined style={{ fontSize: 24 }} />,
      description: '查看操作记录',
      color: '#722ed1',
      path: '/audit/operation',
      permission: 'audit.view',
    },
    {
      key: 'system-manage',
      title: '系统管理',
      icon: <SettingOutlined style={{ fontSize: 24 }} />,
      description: '系统配置管理',
      color: '#13c2c2',
      path: '/system/permissions',
      permission: 'system.manage',
    },
  ];

  // 过滤有权限的操作
  const availableActions = actions.filter((action) =>
    action.permission ? hasPermission(action.permission as any) : true
  );

  return (
    <Card
      title={
        <Space>
          <ToolOutlined />
          <span>快捷操作</span>
        </Space>
      }
    >
      <Row gutter={[16, 16]}>
        {availableActions.map((action) => {
          // 防御性检查
          if (!action || !action.color) {
            console.warn('[QuickActions] Invalid action:', action);
            return null;
          }

          return (
            <Col xs={24} sm={12} md={8} lg={6} key={action.key}>
              <Card
                hoverable
                style={{
                  textAlign: 'center',
                  borderLeft: `4px solid ${action.color}`,
                }}
                onClick={() => navigate(action.path)}
              >
                <Space direction="vertical" size="small" style={{ width: '100%' }}>
                  <div style={{ color: action.color }}>{action.icon}</div>
                  <Title level={5} style={{ margin: 0 }}>
                    {action.title}
                  </Title>
                  <Text type="secondary" style={{ fontSize: 12 }}>
                    {action.description}
                  </Text>
                </Space>
              </Card>
            </Col>
          );
        })}
      </Row>

      {availableActions.length === 0 && (
        <div style={{ textAlign: 'center', padding: '40px 0' }}>
          <Text type="secondary">暂无可用操作</Text>
        </div>
      )}
    </Card>
  );
};

export default QuickActions;
