#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
ANI Animation Parser
====================
解析PVF中的动画定义文件 (.ani)

格式特点:
- 非PVF键值对格式，自定义二进制结构
- 包含图像路径模板和帧数据
- 支持8种动画类型

文件结构:
  头部 (8+N 字节):
    [0-1]  type_code (UINT16): 动画类型
    [2-3]  version (UINT16): 版本号，通常为 0x0001
    [4-7]  path_length (UINT32): 图像路径长度
    [8-N]  image_path (UTF-8): IMG文件路径模板

  帧数据 (重复，每帧12字节):
    [+0-1]  frame_id (UINT16): 帧编号
    [+2-3]  image_offset (UINT16): 图像偏移
    [+4-7]  timing (INT32): 时序/持续时间
    [+8-11] params (INT32): 扩展参数
"""

import struct
from dataclasses import dataclass, field
from typing import List, Optional, Dict, Any


# ============================================================================
# 常量定义
# ============================================================================

# 动画类型码
class AnimationType:
    BASIC_SPRITE = 0x0001      # 基础精灵
    EFFECT = 0x0002            # 特效/粒子
    UNKNOWN_03 = 0x0003        # 未知
    CHARACTER = 0x0004         # 角色/装备 (最常见)
    UNKNOWN_05 = 0x0005        # 未知
    UNKNOWN_06 = 0x0006        # 未知
    MONSTER = 0x0007           # 怪物/生物
    UNKNOWN_08 = 0x0008        # 未知
    UNKNOWN_09 = 0x0009        # 未知
    LARGE_EFFECT = 0x000A      # 大型效果

    @staticmethod
    def get_name(code: int) -> str:
        names = {
            0x0001: 'basic_sprite',
            0x0002: 'effect',
            0x0004: 'character',
            0x0007: 'monster',
            0x000A: 'large_effect',
        }
        return names.get(code, f'type_{code:04X}')


# ============================================================================
# 数据结构
# ============================================================================

@dataclass
class FrameData:
    """动画帧数据"""
    frame_id: int = 0           # 帧编号
    image_offset: int = 0       # 图像偏移/索引
    timing: int = 0             # 时序 (毫秒或帧数)
    params: int = 0             # 扩展参数

    def __repr__(self):
        return f"Frame(id={self.frame_id}, offset={self.image_offset}, timing={self.timing})"


@dataclass
class AnimationData:
    """动画数据"""
    type_code: int = 0          # 动画类型
    version: int = 0            # 版本号
    image_path: str = ""        # 图像路径模板
    frames: List[FrameData] = field(default_factory=list)
    raw_size: int = 0           # 原始文件大小
    path: str = ""              # 文件路径

    @property
    def type_name(self) -> str:
        return AnimationType.get_name(self.type_code)

    @property
    def frame_count(self) -> int:
        return len(self.frames)

    def __repr__(self):
        return (f"Animation(type={self.type_name}, frames={self.frame_count}, "
                f"path='{self.image_path[:40]}...')")

    def get_image_path(self, frame_index: int = 0) -> str:
        """
        获取指定帧的图像路径

        图像路径模板可能包含 %02d 等占位符，用于指定帧编号
        """
        if '%' in self.image_path:
            try:
                return self.image_path % frame_index
            except:
                return self.image_path
        return self.image_path


# ============================================================================
# 解析器
# ============================================================================

class AnimationParser:
    """ANI动画文件解析器"""

    # 有效的类型码范围
    VALID_TYPE_CODES = set(range(0x0001, 0x007B))

    @staticmethod
    def parse(data: bytes, path: str = "") -> Optional[AnimationData]:
        """
        解析ANI文件数据

        Args:
            data: 文件二进制数据
            path: 文件路径（用于调试）

        Returns:
            AnimationData 或 None（解析失败）
        """
        if len(data) < 8:
            return None

        try:
            ani = AnimationData()
            ani.raw_size = len(data)
            ani.path = path
            pos = 0

            # 读取头部
            ani.type_code = struct.unpack_from('<H', data, pos)[0]
            pos += 2

            # 验证类型码
            if ani.type_code not in AnimationParser.VALID_TYPE_CODES:
                return None

            ani.version = struct.unpack_from('<H', data, pos)[0]
            pos += 2

            # 读取路径长度
            path_length = struct.unpack_from('<I', data, pos)[0]
            pos += 4

            # 验证路径长度
            if path_length > 500 or pos + path_length > len(data):
                return None

            # 读取图像路径
            if path_length > 0:
                path_bytes = data[pos:pos + path_length]
                ani.image_path = path_bytes.decode('utf-8', errors='replace').rstrip('\x00')
                pos += path_length

            # 读取帧数据
            frame_size = 12  # 每帧12字节
            while pos + frame_size <= len(data):
                frame = FrameData()
                frame.frame_id = struct.unpack_from('<H', data, pos)[0]
                frame.image_offset = struct.unpack_from('<H', data, pos + 2)[0]
                frame.timing = struct.unpack_from('<i', data, pos + 4)[0]
                frame.params = struct.unpack_from('<i', data, pos + 8)[0]
                ani.frames.append(frame)
                pos += frame_size

            return ani

        except Exception as e:
            return None

    @staticmethod
    def is_valid_ani(data: bytes) -> bool:
        """检查数据是否为有效的ANI格式"""
        if len(data) < 8:
            return False

        type_code = struct.unpack_from('<H', data, 0)[0]
        if type_code not in AnimationParser.VALID_TYPE_CODES:
            return False

        path_length = struct.unpack_from('<I', data, 4)[0]
        if path_length > 500:
            return False

        return True

    @staticmethod
    def analyze(data: bytes) -> Dict[str, Any]:
        """分析ANI文件结构（调试用）"""
        result = {
            'size': len(data),
            'valid': False,
            'type_code': None,
            'version': None,
            'path_length': None,
            'image_path': None,
            'frame_count': 0,
        }

        if len(data) < 8:
            return result

        result['type_code'] = struct.unpack_from('<H', data, 0)[0]
        result['version'] = struct.unpack_from('<H', data, 2)[0]
        result['path_length'] = struct.unpack_from('<I', data, 4)[0]

        if result['type_code'] in AnimationParser.VALID_TYPE_CODES:
            result['valid'] = True

            path_len = result['path_length']
            if path_len > 0 and path_len < 500:
                path_bytes = data[8:8 + path_len]
                result['image_path'] = path_bytes.decode('utf-8', errors='replace').rstrip('\x00')

                # 计算帧数
                remaining = len(data) - 8 - path_len
                result['frame_count'] = remaining // 12

        return result


# ============================================================================
# 测试
# ============================================================================

if __name__ == "__main__":
    import sys
    sys.path.insert(0, '..')

    from pvf_reader import PVFReader

    print("=" * 60)
    print("ANI Parser Test")
    print("=" * 60)

    pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
    if not pvf.open():
        print("Failed to open PVF")
        sys.exit(1)

    pvf.parse_index(max_entries=50000)

    # 测试解析ANI文件
    print()
    print("Sample ANI files:")
    print("-" * 60)

    count = 0
    type_stats = {}

    for entry in pvf.indexes:
        if entry.name.endswith('.ani'):
            data = pvf.extract_file(entry)
            if data:
                ani = AnimationParser.parse(data, entry.name)
                if ani:
                    # 统计类型
                    type_stats[ani.type_code] = type_stats.get(ani.type_code, 0) + 1

                    if count < 10:
                        print(f"  {ani}")
                        if ani.frames:
                            print(f"    First frame: {ani.frames[0]}")

                    count += 1
                    if count >= 1000:
                        break

    print()
    print("-" * 60)
    print(f"Parsed: {count} ANI files")
    print()
    print("Type distribution:")
    for tc, cnt in sorted(type_stats.items()):
        print(f"  0x{tc:04X} ({AnimationType.get_name(tc)}): {cnt}")

    pvf.close()
    print()
    print("Done!")
