/**
 * 玩家基础信息组件
 */

import { FC } from 'react';
import { Card, Row, Col, Descriptions, Progress, Tag, Statistic, Space } from 'antd';
import {
  UserOutlined,
  TrophyOutlined,
  DollarOutlined,
  ClockCircleOutlined,
  HeartOutlined,
  ThunderboltOutlined,
} from '@ant-design/icons';
import type { Character, PlayerAccount } from '@/types/player';
import dayjs from 'dayjs';
import duration from 'dayjs/plugin/duration';

dayjs.extend(duration);

interface BasicInfoProps {
  character: Character;
  account?: PlayerAccount;
}

const BasicInfo: FC<BasicInfoProps> = ({ character, account }) => {
  // 格式化游戏时长
  const formatPlaytime = (seconds?: number) => {
    if (!seconds) return '未知';
    const dur = dayjs.duration(seconds, 'seconds');
    const hours = Math.floor(dur.asHours());
    const minutes = dur.minutes();
    return `${hours}小时${minutes}分钟`;
  };

  // 经验值百分比
  const expPercentage = character.max_exp > 0
    ? Math.floor((character.exp / character.max_exp) * 100)
    : 0;

  // HP百分比
  const hpPercentage = character.max_hp > 0
    ? Math.floor((character.hp / character.max_hp) * 100)
    : 0;

  // MP百分比
  const mpPercentage = character.max_mp > 0
    ? Math.floor((character.mp / character.max_mp) * 100)
    : 0;

  return (
    <Space direction="vertical" size="large" style={{ width: '100%' }}>
      {/* 角色统计卡片 */}
      <Row gutter={16}>
        <Col xs={24} sm={12} md={6}>
          <Card>
            <Statistic
              title="角色等级"
              value={character.level}
              prefix={<TrophyOutlined />}
              valueStyle={{ color: '#1890ff' }}
            />
          </Card>
        </Col>
        <Col xs={24} sm={12} md={6}>
          <Card>
            <Statistic
              title="金币"
              value={character.gold}
              prefix={<DollarOutlined />}
              valueStyle={{ color: '#faad14' }}
            />
          </Card>
        </Col>
        <Col xs={24} sm={12} md={6}>
          <Card>
            <Statistic
              title="HP"
              value={`${character.hp} / ${character.max_hp}`}
              prefix={<HeartOutlined />}
              valueStyle={{ color: '#ff4d4f', fontSize: 16 }}
            />
          </Card>
        </Col>
        <Col xs={24} sm={12} md={6}>
          <Card>
            <Statistic
              title="MP"
              value={`${character.mp} / ${character.max_mp}`}
              prefix={<ThunderboltOutlined />}
              valueStyle={{ color: '#52c41a', fontSize: 16 }}
            />
          </Card>
        </Col>
      </Row>

      {/* 角色详细信息 */}
      <Card title="角色信息" extra={<Tag color="blue">{character.job_name}</Tag>}>
        <Descriptions bordered column={{ xs: 1, sm: 2, md: 2 }}>
          <Descriptions.Item label="角色名">
            <Space>
              <UserOutlined />
              {character.character_name}
            </Space>
          </Descriptions.Item>
          <Descriptions.Item label="角色ID">
            {character.character_id}
          </Descriptions.Item>
          <Descriptions.Item label="职业">
            {character.job_name}
          </Descriptions.Item>
          <Descriptions.Item label="转职">
            {character.grow_type_name || '未转职'}
          </Descriptions.Item>
          <Descriptions.Item label="等级">
            <Tag color="blue">Lv.{character.level}</Tag>
          </Descriptions.Item>
          <Descriptions.Item label="经验值">
            <div style={{ width: '100%' }}>
              <Progress
                percent={expPercentage}
                format={() => `${character.exp.toLocaleString()} / ${character.max_exp.toLocaleString()}`}
                size="small"
              />
            </div>
          </Descriptions.Item>
          <Descriptions.Item label="HP">
            <div style={{ width: '100%' }}>
              <Progress
                percent={hpPercentage}
                strokeColor="#ff4d4f"
                format={() => `${character.hp.toLocaleString()} / ${character.max_hp.toLocaleString()}`}
                size="small"
              />
            </div>
          </Descriptions.Item>
          <Descriptions.Item label="MP">
            <div style={{ width: '100%' }}>
              <Progress
                percent={mpPercentage}
                strokeColor="#52c41a"
                format={() => `${character.mp.toLocaleString()} / ${character.max_mp.toLocaleString()}`}
                size="small"
              />
            </div>
          </Descriptions.Item>
          <Descriptions.Item label="金币">
            <Tag color="gold">{character.gold.toLocaleString()}</Tag>
          </Descriptions.Item>
          <Descriptions.Item label="游戏时长">
            <Space>
              <ClockCircleOutlined />
              {formatPlaytime(character.playtime)}
            </Space>
          </Descriptions.Item>
          <Descriptions.Item label="创建时间">
            {dayjs(character.created_at).format('YYYY-MM-DD HH:mm:ss')}
          </Descriptions.Item>
          <Descriptions.Item label="最后登录">
            {character.last_login
              ? dayjs(character.last_login).format('YYYY-MM-DD HH:mm:ss')
              : '从未登录'}
          </Descriptions.Item>
        </Descriptions>
      </Card>

      {/* 账号信息 */}
      {account && (
        <Card title="账号信息">
          <Descriptions bordered column={{ xs: 1, sm: 2, md: 2 }}>
            <Descriptions.Item label="账号ID">
              {account.account_name}
            </Descriptions.Item>
            <Descriptions.Item label="账号状态">
              {account.status === 'active' && <Tag color="success">正常</Tag>}
              {account.status === 'banned' && <Tag color="error">封禁</Tag>}
              {account.status === 'suspended' && <Tag color="warning">冻结</Tag>}
            </Descriptions.Item>
            {account.cera_point !== undefined && (
              <Descriptions.Item label="点券余额">
                <Tag color="purple">{account.cera_point.toLocaleString()}</Tag>
              </Descriptions.Item>
            )}
            {account.mileage !== undefined && (
              <Descriptions.Item label="里程余额">
                <Tag color="cyan">{account.mileage.toLocaleString()}</Tag>
              </Descriptions.Item>
            )}
            <Descriptions.Item label="注册时间">
              {dayjs(account.created_at).format('YYYY-MM-DD HH:mm:ss')}
            </Descriptions.Item>
            <Descriptions.Item label="最后登录">
              {account.last_login
                ? dayjs(account.last_login).format('YYYY-MM-DD HH:mm:ss')
                : '从未登录'}
            </Descriptions.Item>
            {account.status !== 'active' && account.ban_reason && (
              <Descriptions.Item label="封禁原因" span={2}>
                {account.ban_reason}
              </Descriptions.Item>
            )}
            {account.ban_until && (
              <Descriptions.Item label="封禁至" span={2}>
                {dayjs(account.ban_until).format('YYYY-MM-DD HH:mm:ss')}
              </Descriptions.Item>
            )}
          </Descriptions>
        </Card>
      )}
    </Space>
  );
};

export default BasicInfo;
