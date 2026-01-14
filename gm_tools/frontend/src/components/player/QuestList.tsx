/**
 * 任务列表组件
 */

import { FC, useState } from 'react';
import { Card, List, Progress, Tag, Select, Empty, Space, Typography, Collapse } from 'antd';
import {
  CheckCircleOutlined,
  ClockCircleOutlined,
  CloseCircleOutlined,
  TrophyOutlined,
  DollarOutlined,
  GiftOutlined,
} from '@ant-design/icons';
import type { Quest, QuestStatus } from '@/types/player';

const { Text } = Typography;
const { Panel } = Collapse;

interface QuestListProps {
  quests: Quest[];
}

const QuestList: FC<QuestListProps> = ({ quests }) => {
  const [statusFilter, setStatusFilter] = useState<QuestStatus | 'all'>('all');
  const [typeFilter, setTypeFilter] = useState<string>('all');

  // 任务状态配置
  const getStatusConfig = (status: QuestStatus) => {
    const config = {
      in_progress: {
        icon: <ClockCircleOutlined />,
        color: 'processing',
        text: '进行中',
      },
      completed: {
        icon: <CheckCircleOutlined />,
        color: 'success',
        text: '已完成',
      },
      failed: {
        icon: <CloseCircleOutlined />,
        color: 'error',
        text: '已失败',
      },
      available: {
        icon: <TrophyOutlined />,
        color: 'default',
        text: '可接取',
      },
    };
    return config[status];
  };

  // 任务类型中文
  const getQuestTypeText = (type: Quest['quest_type']): string => {
    const typeMap: Record<Quest['quest_type'], string> = {
      main: '主线任务',
      side: '支线任务',
      daily: '每日任务',
      achievement: '成就任务',
    };
    return typeMap[type];
  };

  // 任务类型颜色
  const getQuestTypeColor = (type: Quest['quest_type']): string => {
    const colorMap: Record<Quest['quest_type'], string> = {
      main: 'red',
      side: 'blue',
      daily: 'green',
      achievement: 'purple',
    };
    return colorMap[type];
  };

  // 过滤任务
  const filteredQuests = quests.filter((quest) => {
    const matchesStatus = statusFilter === 'all' || quest.status === statusFilter;
    const matchesType = typeFilter === 'all' || quest.quest_type === typeFilter;
    return matchesStatus && matchesType;
  });

  // 按状态分组
  const groupedQuests = {
    in_progress: filteredQuests.filter((q) => q.status === 'in_progress'),
    completed: filteredQuests.filter((q) => q.status === 'completed'),
    available: filteredQuests.filter((q) => q.status === 'available'),
    failed: filteredQuests.filter((q) => q.status === 'failed'),
  };

  if (quests.length === 0) {
    return <Empty description="暂无任务" />;
  }

  // 渲染任务奖励
  const renderRewards = (quest: Quest) => {
    const rewards = [];
    if (quest.reward_exp) {
      rewards.push(
        <Tag icon={<TrophyOutlined />} color="blue" key="exp">
          经验 +{quest.reward_exp.toLocaleString()}
        </Tag>
      );
    }
    if (quest.reward_gold) {
      rewards.push(
        <Tag icon={<DollarOutlined />} color="gold" key="gold">
          金币 +{quest.reward_gold.toLocaleString()}
        </Tag>
      );
    }
    if (quest.reward_items && quest.reward_items.length > 0) {
      rewards.push(
        <Tag icon={<GiftOutlined />} color="purple" key="items">
          物品 x{quest.reward_items.length}
        </Tag>
      );
    }
    return rewards.length > 0 ? <Space>{rewards}</Space> : <Text type="secondary">无奖励</Text>;
  };

  // 渲染任务项
  const renderQuestItem = (quest: Quest) => {
    const statusConfig = getStatusConfig(quest.status);
    const progressPercent = quest.max_progress > 0
      ? Math.floor((quest.progress / quest.max_progress) * 100)
      : 0;

    return (
      <List.Item key={quest.quest_id}>
        <Card
          size="small"
          style={{ width: '100%' }}
          title={
            <Space>
              <Tag icon={statusConfig.icon} color={statusConfig.color}>
                {statusConfig.text}
              </Tag>
              <Tag color={getQuestTypeColor(quest.quest_type)}>
                {getQuestTypeText(quest.quest_type)}
              </Tag>
              <Text strong>{quest.quest_name}</Text>
            </Space>
          }
        >
          <Space direction="vertical" style={{ width: '100%' }}>
            {quest.status === 'in_progress' && (
              <div>
                <Text type="secondary">任务进度:</Text>
                <Progress
                  percent={progressPercent}
                  format={() => `${quest.progress} / ${quest.max_progress}`}
                  status={progressPercent >= 100 ? 'success' : 'active'}
                />
              </div>
            )}
            <div>
              <Text type="secondary">任务奖励: </Text>
              {renderRewards(quest)}
            </div>
            {quest.reward_items && quest.reward_items.length > 0 && (
              <Collapse ghost>
                <Panel header="查看物品奖励" key="1">
                  <List
                    size="small"
                    dataSource={quest.reward_items}
                    renderItem={(item) => (
                      <List.Item>
                        <Space>
                          <Text>{item.item_name}</Text>
                          <Tag color="blue">x{item.quantity}</Tag>
                        </Space>
                      </List.Item>
                    )}
                  />
                </Panel>
              </Collapse>
            )}
          </Space>
        </Card>
      </List.Item>
    );
  };

  return (
    <div style={{ padding: '20px' }}>
      <Card
        title={`任务列表 (${filteredQuests.length}/${quests.length})`}
        extra={
          <Space>
            <Select
              value={statusFilter}
              onChange={setStatusFilter}
              style={{ width: 120 }}
            >
              <Select.Option value="all">全部状态</Select.Option>
              <Select.Option value="in_progress">进行中</Select.Option>
              <Select.Option value="completed">已完成</Select.Option>
              <Select.Option value="available">可接取</Select.Option>
              <Select.Option value="failed">已失败</Select.Option>
            </Select>
            <Select
              value={typeFilter}
              onChange={setTypeFilter}
              style={{ width: 120 }}
            >
              <Select.Option value="all">全部类型</Select.Option>
              <Select.Option value="main">主线任务</Select.Option>
              <Select.Option value="side">支线任务</Select.Option>
              <Select.Option value="daily">每日任务</Select.Option>
              <Select.Option value="achievement">成就任务</Select.Option>
            </Select>
          </Space>
        }
      >
        {filteredQuests.length === 0 ? (
          <Empty description="没有符合条件的任务" />
        ) : (
          <Space direction="vertical" style={{ width: '100%' }} size="middle">
            {groupedQuests.in_progress.length > 0 && (
              <List
                header={<Text strong>进行中的任务 ({groupedQuests.in_progress.length})</Text>}
                dataSource={groupedQuests.in_progress}
                renderItem={renderQuestItem}
              />
            )}
            {groupedQuests.available.length > 0 && (
              <List
                header={<Text strong>可接取的任务 ({groupedQuests.available.length})</Text>}
                dataSource={groupedQuests.available}
                renderItem={renderQuestItem}
              />
            )}
            {groupedQuests.completed.length > 0 && (
              <List
                header={<Text strong>已完成的任务 ({groupedQuests.completed.length})</Text>}
                dataSource={groupedQuests.completed}
                renderItem={renderQuestItem}
              />
            )}
            {groupedQuests.failed.length > 0 && (
              <List
                header={<Text strong>已失败的任务 ({groupedQuests.failed.length})</Text>}
                dataSource={groupedQuests.failed}
                renderItem={renderQuestItem}
              />
            )}
          </Space>
        )}
      </Card>
    </div>
  );
};

export default QuestList;
