#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Generic PVF Parser
==================
通用PVF格式解析器，处理所有使用 0xD0B0 魔数的文件

支持格式: ALS, ETC, AIC, REP, UI, NPC, CRE, MSN 等
"""

import struct
from dataclasses import dataclass, field
from typing import List, Optional, Dict, Any
import sys
import os

sys.path.insert(0, os.path.dirname(os.path.dirname(__file__)))
from pvf_data_parser import parse_pvf_data, PVFDataContainer, PVFValueType


# ============================================================================
# 通用数据结构
# ============================================================================

@dataclass
class GenericData:
    """通用PVF数据"""
    format_type: str = ""       # 格式类型
    entry_count: int = 0        # 条目数
    key_count: int = 0          # 唯一键数
    path: str = ""              # 文件路径

    def __repr__(self):
        return f"Generic({self.format_type}, entries={self.entry_count}, keys={self.key_count})"


# ============================================================================
# 通用解析器
# ============================================================================

class GenericPVFParser:
    """通用PVF文件解析器"""

    @staticmethod
    def parse(data: bytes, path: str = "", format_type: str = "") -> Optional[GenericData]:
        if len(data) < 2:
            return None

        if data[0] != 0xB0 or data[1] != 0xD0:
            return None

        try:
            container = parse_pvf_data(data)
            result = GenericData()
            result.format_type = format_type
            result.entry_count = len(container.entries)
            result.path = path

            # 统计唯一键
            keys = set()
            for entry in container.entries:
                if entry.key > 0:
                    keys.add(entry.key)
            result.key_count = len(keys)

            return result

        except Exception as e:
            return None

    @staticmethod
    def is_valid(data: bytes) -> bool:
        if len(data) < 2:
            return False
        return data[0] == 0xB0 and data[1] == 0xD0


# ============================================================================
# 专用解析器 - ALS (别名)
# ============================================================================

@dataclass
class AliasData:
    """别名数据"""
    target_ref: int = 0         # 目标引用
    flags: int = 0              # 标志
    raw_entries: int = 0
    path: str = ""

    def __repr__(self):
        return f"Alias(target={self.target_ref}, flags={self.flags})"


class AliasParser:
    """ALS别名文件解析器"""
    KEY_TARGET = 0x1715
    KEY_FLAG1 = 0x172E
    KEY_FLAG2 = 0x1769

    @staticmethod
    def parse(data: bytes, path: str = "") -> Optional[AliasData]:
        if len(data) < 2 or data[0] != 0xB0 or data[1] != 0xD0:
            return None
        try:
            container = parse_pvf_data(data)
            als = AliasData()
            als.raw_entries = len(container.entries)
            als.path = path
            als.target_ref = container.get_ref(AliasParser.KEY_TARGET)
            als.flags = container.get_int(AliasParser.KEY_FLAG1)
            return als
        except:
            return None


# ============================================================================
# 专用解析器 - AIC (AI配置)
# ============================================================================

@dataclass
class AIConfigData:
    """AI配置数据"""
    ai_type: int = 0
    behavior_main: int = 0
    attack_range: int = 0
    detect_range: int = 0
    raw_entries: int = 0
    path: str = ""

    def __repr__(self):
        return f"AIConfig(type={self.ai_type}, atk_range={self.attack_range})"


class AIConfigParser:
    """AIC AI配置文件解析器"""
    KEY_TYPE = 0x01E0
    KEY_BEHAVIOR = 0x0201
    KEY_ATK_RANGE = 0x020D
    KEY_DETECT = 0x0210

    @staticmethod
    def parse(data: bytes, path: str = "") -> Optional[AIConfigData]:
        if len(data) < 2 or data[0] != 0xB0 or data[1] != 0xD0:
            return None
        try:
            container = parse_pvf_data(data)
            aic = AIConfigData()
            aic.raw_entries = len(container.entries)
            aic.path = path
            aic.ai_type = container.get_int(AIConfigParser.KEY_TYPE)
            aic.behavior_main = container.get_int(AIConfigParser.KEY_BEHAVIOR)
            aic.attack_range = container.get_int(AIConfigParser.KEY_ATK_RANGE)
            aic.detect_range = container.get_int(AIConfigParser.KEY_DETECT)
            return aic
        except:
            return None


# ============================================================================
# 专用解析器 - REP (重放)
# ============================================================================

@dataclass
class ReplayData:
    """重放数据"""
    duration: int = 0
    score: int = 0
    player_count: int = 0
    raw_entries: int = 0
    path: str = ""

    def __repr__(self):
        return f"Replay(duration={self.duration}, score={self.score})"


class ReplayParser:
    """REP重放文件解析器"""
    KEY_DURATION = 0xCE4B
    KEY_SCORE = 0xCE4D
    KEY_PLAYERS = 0x01FD

    @staticmethod
    def parse(data: bytes, path: str = "") -> Optional[ReplayData]:
        if len(data) < 2 or data[0] != 0xB0 or data[1] != 0xD0:
            return None
        try:
            container = parse_pvf_data(data)
            rep = ReplayData()
            rep.raw_entries = len(container.entries)
            rep.path = path
            rep.duration = container.get_int(ReplayParser.KEY_DURATION)
            rep.score = container.get_int(ReplayParser.KEY_SCORE)
            rep.player_count = container.get_int(ReplayParser.KEY_PLAYERS)
            return rep
        except:
            return None


# ============================================================================
# 专用解析器 - UI (界面)
# ============================================================================

@dataclass
class UIData:
    """界面数据"""
    widget_count: int = 0
    raw_entries: int = 0
    path: str = ""

    def __repr__(self):
        return f"UI(widgets={self.widget_count})"


class UIParser:
    """UI界面文件解析器"""
    KEY_WIDGET = 0x0C0E

    @staticmethod
    def parse(data: bytes, path: str = "") -> Optional[UIData]:
        if len(data) < 2 or data[0] != 0xB0 or data[1] != 0xD0:
            return None
        try:
            container = parse_pvf_data(data)
            ui = UIData()
            ui.raw_entries = len(container.entries)
            ui.path = path
            ui.widget_count = len(container.get_ints(UIParser.KEY_WIDGET))
            return ui
        except:
            return None


# ============================================================================
# 专用解析器 - NPC
# ============================================================================

@dataclass
class NPCData:
    """NPC数据"""
    npc_type: int = 0
    dialog_ref: int = 0
    raw_entries: int = 0
    path: str = ""

    def __repr__(self):
        return f"NPC(type={self.npc_type}, dialog={self.dialog_ref})"


class NPCParser:
    """NPC文件解析器"""
    KEY_TYPE = 0x7839
    KEY_DIALOG = 0x7871

    @staticmethod
    def parse(data: bytes, path: str = "") -> Optional[NPCData]:
        if len(data) < 2 or data[0] != 0xB0 or data[1] != 0xD0:
            return None
        try:
            container = parse_pvf_data(data)
            npc = NPCData()
            npc.raw_entries = len(container.entries)
            npc.path = path
            npc.npc_type = container.get_int(NPCParser.KEY_TYPE)
            npc.dialog_ref = container.get_ref(NPCParser.KEY_DIALOG)
            return npc
        except:
            return None


# ============================================================================
# 专用解析器 - CRE (生物/宠物)
# ============================================================================

@dataclass
class CreatureData:
    """生物数据"""
    name_ref: int = 0
    hp: int = 0
    speed: int = 0
    raw_entries: int = 0
    path: str = ""

    def __repr__(self):
        return f"Creature(hp={self.hp}, speed={self.speed})"


class CreatureParser:
    """CRE生物文件解析器"""
    KEY_NAME = 0x2A12
    KEY_HP = 0x0B69
    KEY_SPEED = 0x0361

    @staticmethod
    def parse(data: bytes, path: str = "") -> Optional[CreatureData]:
        if len(data) < 2 or data[0] != 0xB0 or data[1] != 0xD0:
            return None
        try:
            container = parse_pvf_data(data)
            cre = CreatureData()
            cre.raw_entries = len(container.entries)
            cre.path = path
            cre.name_ref = container.get_ref(CreatureParser.KEY_NAME)
            cre.hp = container.get_int(CreatureParser.KEY_HP)
            cre.speed = container.get_int(CreatureParser.KEY_SPEED)
            return cre
        except:
            return None


# ============================================================================
# 专用解析器 - MSN (任务/关卡)
# ============================================================================

@dataclass
class MissionData:
    """任务/关卡数据"""
    mission_id: int = 0
    target_ref: int = 0
    raw_entries: int = 0
    path: str = ""

    def __repr__(self):
        return f"Mission(id={self.mission_id})"


class MissionParser:
    """MSN任务文件解析器"""
    KEY_ID = 0x0075
    KEY_TARGET = 0x417D

    @staticmethod
    def parse(data: bytes, path: str = "") -> Optional[MissionData]:
        if len(data) < 2 or data[0] != 0xB0 or data[1] != 0xD0:
            return None
        try:
            container = parse_pvf_data(data)
            msn = MissionData()
            msn.raw_entries = len(container.entries)
            msn.path = path
            msn.mission_id = container.get_int(MissionParser.KEY_ID)
            msn.target_ref = container.get_ref(MissionParser.KEY_TARGET)
            return msn
        except:
            return None


# ============================================================================
# 专用解析器 - ETC (杂项)
# ============================================================================

@dataclass
class MiscData:
    """杂项数据"""
    raw_entries: int = 0
    path: str = ""

    def __repr__(self):
        return f"Misc(entries={self.raw_entries})"


class MiscParser:
    """ETC杂项文件解析器"""

    @staticmethod
    def parse(data: bytes, path: str = "") -> Optional[MiscData]:
        if len(data) < 2 or data[0] != 0xB0 or data[1] != 0xD0:
            return None
        try:
            container = parse_pvf_data(data)
            etc = MiscData()
            etc.raw_entries = len(container.entries)
            etc.path = path
            return etc
        except:
            return None


# ============================================================================
# 文本格式解析器
# ============================================================================

@dataclass
class TextData:
    """文本数据"""
    content_preview: str = ""
    line_count: int = 0
    size: int = 0
    encoding: str = ""
    path: str = ""

    def __repr__(self):
        return f"Text(lines={self.line_count}, size={self.size})"


class TextParser:
    """文本文件解析器 - 处理 .str, .nut, .txt, .rtf, .lst 等"""

    @staticmethod
    def parse(data: bytes, path: str = "") -> Optional[TextData]:
        if data is None or len(data) < 1:
            # 空文件返回空TextData
            txt = TextData()
            txt.size = 0
            txt.path = path
            txt.encoding = 'empty'
            txt.line_count = 0
            txt.content_preview = ''
            return txt

        # 尝试解码文本
        txt = TextData()
        txt.size = len(data)
        txt.path = path

        # 尝试不同编码
        for enc in ['utf-8', 'cp949', 'gbk', 'latin-1']:
            try:
                content = data.decode(enc)
                txt.encoding = enc
                txt.content_preview = content[:100]
                txt.line_count = content.count('\n') + 1
                return txt
            except:
                continue

        # 作为二进制处理
        txt.encoding = 'binary'
        txt.line_count = 0
        return txt

    @staticmethod
    def is_text_format(data: bytes) -> bool:
        """检查是否为文本格式（非PVF二进制）"""
        if len(data) < 2:
            return True
        # 如果不是PVF魔数，可能是文本
        if data[0] != 0xB0 or data[1] != 0xD0:
            # 检查是否以常见文本开头
            try:
                start = data[:20].decode('utf-8', errors='ignore').lower()
                text_markers = ['[', '//', '/*', '\r\n', '\n', '#']
                return any(start.startswith(m) or m in start for m in text_markers)
            except:
                pass
        return False


# ============================================================================
# 智能解析器 - 自动处理PVF和文本格式
# ============================================================================

@dataclass
class EmptyData:
    """空文件数据"""
    path: str = ""

    def __repr__(self):
        return f"Empty({self.path.split('/')[-1] if self.path else 'file'})"


class SmartParser:
    """智能解析器 - 先尝试PVF格式，失败则尝试文本格式"""

    @staticmethod
    def parse(data: bytes, path: str = "", pvf_parser=None) -> Optional[Any]:
        """
        智能解析：先尝试PVF格式，失败则回退到文本格式

        Args:
            data: 文件数据
            path: 文件路径
            pvf_parser: PVF格式解析函数

        Returns:
            解析结果
        """
        # 空文件也视为已解析
        if data is None or len(data) == 0:
            return EmptyData(path=path)

        # 检查是否为PVF格式
        is_pvf = len(data) >= 2 and data[0] == 0xB0 and data[1] == 0xD0

        if is_pvf and pvf_parser:
            result = pvf_parser(data, path)
            if result:
                return result

        # 回退到文本解析
        return TextParser.parse(data, path)


# ============================================================================
# 二进制格式解析器
# ============================================================================

@dataclass
class BinaryData:
    """二进制数据"""
    size: int = 0
    magic: str = ""
    path: str = ""

    def __repr__(self):
        return f"Binary(size={self.size}, magic={self.magic})"


class BinaryParser:
    """二进制文件解析器 - 处理 .exe, .bin, .img 等"""

    @staticmethod
    def parse(data: bytes, path: str = "") -> Optional[BinaryData]:
        if data is None or len(data) < 1:
            # 空文件返回空BinaryData
            result = BinaryData()
            result.size = 0
            result.path = path
            result.magic = ''
            return result

        result = BinaryData()
        result.size = len(data)
        result.path = path

        # 识别魔数
        if len(data) >= 4:
            result.magic = data[:4].hex()

        return result


# ============================================================================
# 测试
# ============================================================================

if __name__ == "__main__":
    sys.path.insert(0, '..')
    from pvf_reader import PVFReader

    print("=" * 60)
    print("Generic Parser Test")
    print("=" * 60)

    pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
    if not pvf.open():
        print("Failed to open PVF")
        sys.exit(1)

    pvf.parse_index(max_entries=300000)

    # 测试各格式
    parsers = {
        '.als': AliasParser.parse,
        '.aic': AIConfigParser.parse,
        '.rep': ReplayParser.parse,
        '.ui': UIParser.parse,
        '.npc': NPCParser.parse,
        '.cre': CreatureParser.parse,
        '.msn': MissionParser.parse,
        '.etc': MiscParser.parse,
    }

    for ext, parser in parsers.items():
        count = 0
        for entry in pvf.indexes:
            if entry.name.endswith(ext):
                data = pvf.extract_file(entry)
                if data:
                    result = parser(data, entry.name)
                    if result:
                        if count < 3:
                            print(f"{ext}: {result}")
                        count += 1
                        if count >= 100:
                            break
        print(f"  {ext}: {count} files parsed")

    pvf.close()
    print("\nDone!")
