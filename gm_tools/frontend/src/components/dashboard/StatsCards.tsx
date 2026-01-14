/**
 * Dashboard 统计卡片组件
 */

import { FC } from 'react';
import { Row, Col, Card, Statistic, Progress } from 'antd';
import {
  UserOutlined,
  TeamOutlined,
  LoginOutlined,
  RiseOutlined,
  ThunderboltOutlined,
  ClockCircleOutlined,
  StopOutlined,
} from '@ant-design/icons';
import type { DashboardStats } from '@/types';

interface StatsCardsProps {
  stats: DashboardStats | null;
  loading: boolean;
}

const StatsCards: FC<StatsCardsProps> = ({ stats, loading }) => {
  return (
    <Row gutter={[16, 16]}>
      {/* 在线玩家 */}
      <Col xs={24} sm={12} lg={6}>
        <Card loading={loading} hoverable>
          <Statistic
            title="在线玩家"
            value={stats?.online_players || 0}
            prefix={<UserOutlined style={{ color: '#1890ff' }} />}
            suffix={`/ ${stats?.total_players || 0}`}
          />
        </Card>
      </Col>

      {/* 今日注册 */}
      <Col xs={24} sm={12} lg={6}>
        <Card loading={loading} hoverable>
          <Statistic
            title="今日注册"
            value={stats?.today_registrations || 0}
            prefix={<TeamOutlined style={{ color: '#52c41a' }} />}
            valueStyle={{ color: '#52c41a' }}
          />
        </Card>
      </Col>

      {/* 今日登录 */}
      <Col xs={24} sm={12} lg={6}>
        <Card loading={loading} hoverable>
          <Statistic
            title="今日登录"
            value={stats?.today_logins || 0}
            prefix={<LoginOutlined style={{ color: '#faad14' }} />}
            valueStyle={{ color: '#faad14' }}
          />
        </Card>
      </Col>

      {/* 今日峰值 */}
      <Col xs={24} sm={12} lg={6}>
        <Card loading={loading} hoverable>
          <Statistic
            title="今日峰值"
            value={stats?.peak_online_today || 0}
            prefix={<RiseOutlined style={{ color: '#722ed1' }} />}
            valueStyle={{ color: '#722ed1' }}
          />
        </Card>
      </Col>

      {/* 活跃副本 */}
      <Col xs={24} sm={12} lg={6}>
        <Card loading={loading} hoverable>
          <Statistic
            title="活跃副本"
            value={stats?.active_dungeons || 0}
            prefix={<ThunderboltOutlined style={{ color: '#eb2f96' }} />}
            valueStyle={{ color: '#eb2f96' }}
          />
        </Card>
      </Col>

      {/* 服务器负载 */}
      <Col xs={24} sm={12} lg={6}>
        <Card loading={loading} hoverable>
          <div>
            <div style={{ marginBottom: 8 }}>
              <ClockCircleOutlined style={{ marginRight: 8, color: '#1890ff' }} />
              服务器负载
            </div>
            <Progress
              percent={stats?.server_load || 0}
              status={
                (stats?.server_load || 0) > 80
                  ? 'exception'
                  : (stats?.server_load || 0) > 60
                  ? 'normal'
                  : 'success'
              }
              strokeColor={
                (stats?.server_load || 0) > 80
                  ? '#ff4d4f'
                  : (stats?.server_load || 0) > 60
                  ? '#faad14'
                  : '#52c41a'
              }
            />
          </div>
        </Card>
      </Col>

      {/* 今日封禁 */}
      <Col xs={24} sm={12} lg={6}>
        <Card loading={loading} hoverable>
          <Statistic
            title="今日封禁"
            value={stats?.banned_accounts_today || 0}
            prefix={<StopOutlined style={{ color: '#ff4d4f' }} />}
            valueStyle={{ color: stats?.banned_accounts_today ? '#ff4d4f' : undefined }}
          />
        </Card>
      </Col>
    </Row>
  );
};

export default StatsCards;
