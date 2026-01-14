/**
 * 装备视图组件
 */

import { FC } from 'react';
import { Card, Row, Col, Empty, Tag, Tooltip, Badge } from 'antd';
import {
  QuestionCircleOutlined,
} from '@ant-design/icons';
import type { Equipment, EquipmentSlot } from '@/types/player';

interface EquipmentViewProps {
  equipment: Equipment[];
}

// 装备槽位配置
const EQUIPMENT_SLOTS: { slot: EquipmentSlot; name: string; position: [number, number] }[] = [
  { slot: 'weapon', name: '武器', position: [0, 0] },
  { slot: 'top', name: '上衣', position: [0, 1] },
  { slot: 'bottom', name: '下装', position: [0, 2] },
  { slot: 'head', name: '头肩', position: [1, 0] },
  { slot: 'belt', name: '腰带', position: [1, 1] },
  { slot: 'shoes', name: '鞋子', position: [1, 2] },
  { slot: 'necklace', name: '项链', position: [2, 0] },
  { slot: 'bracelet', name: '手镯', position: [2, 1] },
  { slot: 'ring', name: '戒指', position: [2, 2] },
  { slot: 'earring', name: '耳环', position: [3, 0] },
  { slot: 'support', name: '辅助装备', position: [3, 1] },
  { slot: 'magic_stone', name: '魔法石', position: [3, 2] },
];

const EquipmentView: FC<EquipmentViewProps> = ({ equipment }) => {
  // 根据槽位获取装备
  const getEquipmentBySlot = (slot: EquipmentSlot): Equipment | undefined => {
    return equipment.find((eq) => eq.slot === slot);
  };

  // 稀有度颜色映射
  const getRarityColor = (rarity: Equipment['item_rarity']): string => {
    const colorMap: Record<Equipment['item_rarity'], string> = {
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
  const getRarityText = (rarity: Equipment['item_rarity']): string => {
    const textMap: Record<Equipment['item_rarity'], string> = {
      common: '普通',
      uncommon: '高级',
      rare: '稀有',
      unique: '神器',
      epic: '史诗',
      legendary: '传说',
    };
    return textMap[rarity];
  };

  // 渲染装备槽位
  const renderSlot = (slotConfig: typeof EQUIPMENT_SLOTS[0]) => {
    const eq = getEquipmentBySlot(slotConfig.slot);

    if (!eq) {
      return (
        <Card
          size="small"
          style={{
            height: 120,
            display: 'flex',
            flexDirection: 'column',
            justifyContent: 'center',
            alignItems: 'center',
            background: '#fafafa',
            border: '1px dashed #d9d9d9',
          }}
        >
          <QuestionCircleOutlined style={{ fontSize: 32, color: '#bfbfbf' }} />
          <div style={{ marginTop: 8, color: '#8c8c8c', fontSize: 12 }}>
            {slotConfig.name}
          </div>
        </Card>
      );
    }

    const upgradeLevel = eq.upgrade_level > 0 ? `+${eq.upgrade_level}` : '';
    const amplifyLevel = eq.amplify_level && eq.amplify_level > 0 ? `增${eq.amplify_level}` : '';

    return (
      <Badge.Ribbon
        text={getRarityText(eq.item_rarity)}
        color={getRarityColor(eq.item_rarity)}
      >
        <Card
          size="small"
          style={{
            height: 120,
            borderColor: getRarityColor(eq.item_rarity),
            borderWidth: 2,
          }}
          hoverable
        >
          <Tooltip
            title={
              <div>
                <div><strong>{eq.item_name}</strong></div>
                <div>槽位: {eq.slot_name}</div>
                <div>稀有度: {getRarityText(eq.item_rarity)}</div>
                {eq.upgrade_level > 0 && <div>强化: +{eq.upgrade_level}</div>}
                {eq.amplify_level && eq.amplify_level > 0 && <div>增幅: +{eq.amplify_level}</div>}
                {eq.seal_flag && <div>状态: 已封装</div>}
                {eq.enchant_cards && eq.enchant_cards.length > 0 && (
                  <div>
                    附魔:
                    <ul style={{ margin: 0, paddingLeft: 20 }}>
                      {eq.enchant_cards.map((card, idx) => (
                        <li key={idx}>{card.card_name}</li>
                      ))}
                    </ul>
                  </div>
                )}
              </div>
            }
          >
            <div style={{ textAlign: 'center' }}>
              <div
                style={{
                  fontSize: 40,
                  color: getRarityColor(eq.item_rarity),
                  marginBottom: 4,
                }}
              >
                🎖️
              </div>
              <div
                style={{
                  fontSize: 12,
                  fontWeight: 'bold',
                  overflow: 'hidden',
                  textOverflow: 'ellipsis',
                  whiteSpace: 'nowrap',
                }}
              >
                {eq.item_name}
              </div>
              <div style={{ marginTop: 4 }}>
                {upgradeLevel && (
                  <Tag color="blue" style={{ fontSize: 10, marginRight: 4 }}>
                    {upgradeLevel}
                  </Tag>
                )}
                {amplifyLevel && (
                  <Tag color="purple" style={{ fontSize: 10 }}>
                    {amplifyLevel}
                  </Tag>
                )}
                {eq.seal_flag && (
                  <Tag color="gold" style={{ fontSize: 10 }}>
                    封装
                  </Tag>
                )}
              </div>
            </div>
          </Tooltip>
        </Card>
      </Badge.Ribbon>
    );
  };

  if (equipment.length === 0) {
    return <Empty description="未装备任何装备" />;
  }

  // 按行分组
  const rows = [0, 1, 2, 3].map((rowIndex) =>
    EQUIPMENT_SLOTS.filter((slot) => slot.position[0] === rowIndex)
  );

  return (
    <div style={{ padding: '20px' }}>
      <Card title={`装备总览 (${equipment.length}/12)`}>
        {rows.map((rowSlots, rowIndex) => (
          <Row gutter={16} key={rowIndex} style={{ marginBottom: rowIndex < 3 ? 16 : 0 }}>
            {rowSlots.map((slotConfig) => (
              <Col xs={24} sm={12} md={8} key={slotConfig.slot}>
                {renderSlot(slotConfig)}
              </Col>
            ))}
          </Row>
        ))}
      </Card>
    </div>
  );
};

export default EquipmentView;
