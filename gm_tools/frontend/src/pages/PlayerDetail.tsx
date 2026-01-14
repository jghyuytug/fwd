/**
 * 玩家详情页面
 */

import { FC, useEffect, useState } from 'react';
import { useParams, useSearchParams, useNavigate } from 'react-router-dom';
import { Tabs, Spin, Alert, Button, Space } from 'antd';
import { ArrowLeftOutlined, ReloadOutlined } from '@ant-design/icons';
import { PageHeader } from '@/components/common';
import BasicInfo from '@/components/player/BasicInfo';
import EquipmentView from '@/components/player/EquipmentView';
import WarehouseGrid from '@/components/player/WarehouseGrid';
import QuestList from '@/components/player/QuestList';
import { playerApi } from '@/services/player';
import type { Character, PlayerAccount, Equipment, WarehouseItem, Quest } from '@/types/player';

const PlayerDetail: FC = () => {
  const { id } = useParams<{ id: string }>();
  const [searchParams, setSearchParams] = useSearchParams();
  const navigate = useNavigate();

  const [loading, setLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);
  const [character, setCharacter] = useState<Character | null>(null);
  const [account, setAccount] = useState<PlayerAccount | null>(null);
  const [equipment, setEquipment] = useState<Equipment[]>([]);
  const [warehouse, setWarehouse] = useState<WarehouseItem[]>([]);
  const [quests, setQuests] = useState<Quest[]>([]);

  const activeTab = searchParams.get('tab') || 'basic';

  // 获取角色数据
  const fetchCharacterData = async () => {
    if (!id) return;

    setLoading(true);
    setError(null);

    try {
      const characterId = parseInt(id, 10);
      const data = await playerApi.getPlayerDetail(characterId);

      setCharacter(data.character);
      setAccount(data.account);
      setEquipment(data.equipment);
      setWarehouse(data.warehouse);
      setQuests(data.quests);
    } catch (err: any) {
      setError(err.response?.data?.message || '加载角色数据失败');
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    fetchCharacterData();
  }, [id]);

  // Tab 切换
  const handleTabChange = (key: string) => {
    setSearchParams({ tab: key });
  };

  if (loading && !character) {
    return (
      <div style={{ textAlign: 'center', padding: '100px 0' }}>
        <Spin size="large" tip="加载中..." />
      </div>
    );
  }

  if (error) {
    return (
      <div>
        <PageHeader title="加载失败" />
        <Alert
          message="加载失败"
          description={error}
          type="error"
          showIcon
          action={
            <Space>
              <Button size="small" onClick={() => navigate('/player')}>
                返回搜索
              </Button>
              <Button size="small" type="primary" onClick={fetchCharacterData}>
                重试
              </Button>
            </Space>
          }
        />
      </div>
    );
  }

  if (!character) {
    return null;
  }

  return (
    <div>
      <PageHeader
        title={`角色详情 - ${character.character_name}`}
        subTitle={`${character.job_name} Lv.${character.level}`}
        onBack={() => navigate('/player')}
        extra={
          <Space>
            <Button
              icon={<ArrowLeftOutlined />}
              onClick={() => navigate('/player')}
            >
              返回搜索
            </Button>
            <Button
              icon={<ReloadOutlined />}
              onClick={fetchCharacterData}
              loading={loading}
            >
              刷新
            </Button>
          </Space>
        }
      />

      <Tabs
        activeKey={activeTab}
        onChange={handleTabChange}
        items={[
          {
            key: 'basic',
            label: '基础信息',
            children: <BasicInfo character={character} account={account || undefined} />,
          },
          {
            key: 'equipment',
            label: '装备信息',
            children: <EquipmentView equipment={equipment} />,
          },
          {
            key: 'warehouse',
            label: '仓库信息',
            children: <WarehouseGrid items={warehouse} />,
          },
          {
            key: 'quests',
            label: '任务信息',
            children: <QuestList quests={quests} />,
          },
        ]}
      />
    </div>
  );
};

export default PlayerDetail;
