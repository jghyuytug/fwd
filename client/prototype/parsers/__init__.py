#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
PVF Format Parsers
==================
各种PVF文件格式的专用解析器 - 完整覆盖268,126文件

Phase 1 (82%): ANI, ACT, ATK
Phase 2 (90%): STK, QST, AI
Phase 3 (96%): KEY, OBJ, TIL
Phase 4 (98%): PTL, MAP, DGN
Phase 5 (100%): ALS, ETC, AIC, REP, UI, NPC, CRE, MSN + 通用解析器
"""

from .ani_parser import AnimationParser, AnimationData, FrameData
from .atk_parser import AttackParser, AttackData
from .act_parser import ActionParser, ActionData
from .stk_parser import StackableParser, StackableData
from .qst_parser import QuestParser, QuestData
from .ai_parser import AIParser, AIData
from .key_parser import KeybindParser, KeybindData
from .obj_parser import GameObjectParser, GameObjectData
from .til_parser import TileParser, TileData
from .ptl_parser import ParticleParser, ParticleData
from .map_parser import MapParser, MapData
from .dgn_parser import DungeonParser, DungeonData
from .generic_parser import (
    GenericPVFParser, GenericData,
    AliasParser, AliasData,
    AIConfigParser, AIConfigData,
    ReplayParser, ReplayData,
    UIParser, UIData,
    NPCParser, NPCData,
    CreatureParser, CreatureData,
    MissionParser, MissionData,
    MiscParser, MiscData,
    TextParser, TextData,
    BinaryParser, BinaryData,
    SmartParser,
)

__all__ = [
    # Phase 1
    'AnimationParser', 'AnimationData', 'FrameData',
    'AttackParser', 'AttackData',
    'ActionParser', 'ActionData',
    # Phase 2
    'StackableParser', 'StackableData',
    'QuestParser', 'QuestData',
    'AIParser', 'AIData',
    # Phase 3
    'KeybindParser', 'KeybindData',
    'GameObjectParser', 'GameObjectData',
    'TileParser', 'TileData',
    # Phase 4
    'ParticleParser', 'ParticleData',
    'MapParser', 'MapData',
    'DungeonParser', 'DungeonData',
    # Phase 5
    'GenericPVFParser', 'GenericData',
    'AliasParser', 'AliasData',
    'AIConfigParser', 'AIConfigData',
    'ReplayParser', 'ReplayData',
    'UIParser', 'UIData',
    'NPCParser', 'NPCData',
    'CreatureParser', 'CreatureData',
    'MissionParser', 'MissionData',
    'MiscParser', 'MiscData',
    'TextParser', 'TextData',
    'BinaryParser', 'BinaryData',
]
