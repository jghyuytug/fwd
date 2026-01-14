/**
 * 仓库网格组件
 */

import { FC, useState } from 'react';
import { Card, Input, Select, Badge, Tooltip, Empty, Row, Col, Tag, Space } from 'antd';
import { SearchOutlined, FilterOutlined } from '@ant-design/icons';
import type { WarehouseItem } from '@/types/player';
import dayjs from 'dayjs';

interface WarehouseGridProps {
  items: WarehouseItem[];
}

const WarehouseGrid: FC<WarehouseGridProps> = ({ items }) => {
  const [searchKeyword, setSearchKeyword] = useState('');
  const [rarityFilter, setRarityFilter] = useState<string>('all');

  // 稀有度颜色映射
  const getRarityColor = (rarity: WarehouseItem['item_rarity']): string => {
    const colorMap: Record<WarehouseItem['item_rarity'], string> = {
      common: '#d9d9d9',
      uncommon: '#52c41a',
      rare: '#1890ff',
      unique: '#722ed1',
      epic: '#eb2f96',
      legendary: '#fa8c16',
    };
    return colorMap[rarity];
  };

  // 稀有度中文
  const getRarityText = (rarity: WarehouseItem['item_rarity']): string => {
    const textMap: Record<WarehouseItem['item_rarity'], string> = {
      common: '普通',
      uncommon: '高级',
      rare: '稀有',
      unique: '神器',
      epic: '史诗',
      legendary: '传说',
    };
    return textMap[rarity];
  };

  // 过滤物品
  const filteredItems = items.filter((item) => {
    const matchesSearch = item.item_name.toLowerCase().includes(searchKeyword.toLowerCase());
    const matchesRarity = rarityFilter === 'all' || item.item_rarity === rarityFilter;
    return matchesSearch && matchesRarity;
  });

  // 渲染物品槽位
  const renderItemSlot = (item: WarehouseItem) => {
    const upgradeLevel = item.upgrade_level && item.upgrade_level > 0 ? `+${item.upgrade_level}` : '';
    const isExpiring = item.expire_time && dayjs(item.expire_time).isBefore(dayjs().add(7, 'day'));

    return (
      <Tooltip
        key={item.slot}
        title={
          <div>
            <div><strong>{item.item_name}</strong></div>
            <div>稀有度: {getRarityText(item.item_rarity)}</div>
            <div>数量: {item.quantity}</div>
            {item.upgrade_level && item.upgrade_level > 0 && <div>强化: +{item.upgrade_level}</div>}
            {item.expire_time && (
              <div style={{ color: isExpiring ? '#ff4d4f' : undefined }}>
                过期时间: {dayjs(item.expire_time).format('YYYY-MM-DD HH:mm')}
              </div>
            )}
          </div>
        }
      >
        <div
          style={{
            position: 'relative',
            width: '100%',
            paddingTop: '100%',
            border: `2px solid ${getRarityColor(item.item_rarity)}`,
            borderRadius: 4,
            background: '#fafafa',
            cursor: 'pointer',
            transition: 'transform 0.2s',
          }}
          onMouseEnter={(e) => {
            e.currentTarget.style.transform = 'scale(1.05)';
          }}
          onMouseLeave={(e) => {
            e.currentTarget.style.transform = 'scale(1)';
          }}
        >
          <div
            style={{
              position: 'absolute',
              top: 0,
              left: 0,
              right: 0,
              bottom: 0,
              display: 'flex',
              flexDirection: 'column',
              justifyContent: 'center',
              alignItems: 'center',
              padding: 4,
            }}
          >
            <div style={{ fontSize: 32 }}>📦</div>
            <div
              style={{
                fontSize: 10,
                textAlign: 'center',
                overflow: 'hidden',
                textOverflow: 'ellipsis',
                whiteSpace: 'nowrap',
                width: '100%',
              }}
            >
              {item.item_name}
            </div>
            <Badge
              count={item.quantity}
              style={{
                backgroundColor: '#52c41a',
                position: 'absolute',
                top: 4,
                right: 4,
              }}
            />
            {upgradeLevel && (
              <Tag
                color="blue"
                style={{
                  fontSize: 8,
                  position: 'absolute',
                  bottom: 4,
                  left: 4,
                  margin: 0,
                  padding: '0 4px',
                }}
              >
                {upgradeLevel}
              </Tag>
            )}
            {isExpiring && (
              <Tag
                color="error"
                style={{
                  fontSize: 8,
                  position: 'absolute',
                  top: 4,
                  left: 4,
                  margin: 0,
                  padding: '0 4px',
                }}
              >
                即将过期
              </Tag>
            )}
          </div>
        </div>
      </Tooltip>
    );
  };

  if (items.length === 0) {
    return <Empty description="仓库为空" />;
  }

  return (
    <div style={{ padding: '20px' }}>
      <Card
        title={`仓库物品 (${filteredItems.length}/${items.length})`}
        extra={
          <Space>
            <Input
              placeholder="搜索物品名称"
              prefix={<SearchOutlined />}
              value={searchKeyword}
              onChange={(e) => setSearchKeyword(e.target.value)}
              style={{ width: 200 }}
              allowClear
            />
            <Select
              value={rarityFilter}
              onChange={setRarityFilter}
              style={{ width: 120 }}
              suffixIcon={<FilterOutlined />}
            >
              <Select.Option value="all">全部稀有度</Select.Option>
              <Select.Option value="common">普通</Select.Option>
              <Select.Option value="uncommon">高级</Select.Option>
              <Select.Option value="rare">稀有</Select.Option>
              <Select.Option value="unique">神器</Select.Option>
              <Select.Option value="epic">史诗</Select.Option>
              <Select.Option value="legendary">传说</Select.Option>
            </Select>
          </Space>
        }
      >
        {filteredItems.length === 0 ? (
          <Empty description="没有符合条件的物品" />
        ) : (
          <Row gutter={[8, 8]}>
            {filteredItems.map((item) => (
              <Col xs={8} sm={6} md={4} lg={3} key={item.slot}>
                {renderItemSlot(item)}
              </Col>
            ))}
          </Row>
        )}
      </Card>
    </div>
  );
};

export default WarehouseGrid;
