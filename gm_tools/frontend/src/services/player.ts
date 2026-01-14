/**
 * 玩家相关 API
 */

import { http } from '@/utils/request';
import type {
  PlayerSearchParams,
  PlayerSearchResult,
  PlayerDetail,
  Character,
  Equipment,
  WarehouseItem,
  Quest,
  PaginationParams,
  PaginationData,
} from '@/types';

export const playerApi = {
  // 搜索玩家
  search: (params: PlayerSearchParams & PaginationParams) => {
    return http.post<PaginationData<PlayerSearchResult>>('/player/search', params);
  },

  // 获取角色详情
  getCharacterDetail: (characterId: number) => {
    return http.get<Character>(`/player/${characterId}`);
  },

  // 获取角色装备
  getEquipment: (characterId: number) => {
    return http.get<Equipment[]>(`/player/${characterId}/equipment`);
  },

  // 获取仓库信息
  getWarehouse: (characterId: number) => {
    return http.get<WarehouseItem[]>(`/player/${characterId}/warehouse`);
  },

  // 获取任务信息
  getQuests: (characterId: number) => {
    return http.get<Quest[]>(`/player/${characterId}/quests`);
  },

  // 获取完整玩家详情
  getPlayerDetail: async (characterId: number): Promise<PlayerDetail> => {
    const [character, equipment, warehouse, quests] = await Promise.all([
      playerApi.getCharacterDetail(characterId),
      playerApi.getEquipment(characterId),
      playerApi.getWarehouse(characterId),
      playerApi.getQuests(characterId),
    ]);

    return {
      account: {
        account_id: 0, // TODO: 从角色信息中获取
        account_name: '',
        status: 'active',
        created_at: character.created_at,
        last_login: character.last_login,
      },
      character,
      equipment,
      warehouse,
      quests,
    };
  },
};
