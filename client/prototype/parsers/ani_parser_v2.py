#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
ANI动画文件解析器 V2
====================
基于二进制结构分析的完整解析器

ANI文件结构:
  Offset 0-1: Type Code (UINT16)
  Offset 2-3: Version (UINT16)
  Offset 4-7: Path Length (UINT32)
  Offset 8+:  Image Path (UTF-8 string)
  后续:       帧数据 (Type-specific)

Type Codes:
  0x0001-0x0014: 不同的动画类型
  0x0000: 空/占位符动画
"""

import struct
from dataclasses import dataclass, field
from typing import List, Optional


@dataclass
class ANIFrame:
    """动画帧"""
    frame_id: int = 0
    offset_x: int = 0
    offset_y: int = 0
    duration: int = 0
    flags: int = 0


@dataclass
class ANIData:
    """ANI动画数据"""
    type_code: int = 0
    version: int = 0
    image_path: str = ""
    frames: List[ANIFrame] = field(default_factory=list)
    raw_frame_data: bytes = b""
    path: str = ""

    def __repr__(self):
        return f"ANI(type=0x{self.type_code:04X}, ver={self.version}, img={self.image_path[:40]}, frames={len(self.frames)})"

    @property
    def type_name(self) -> str:
        """获取类型名称"""
        names = {
            0x0000: "empty",
            0x0001: "basic_sprite",
            0x0002: "effect_sprite",
            0x0003: "character_anim",
            0x0004: "equipment_anim",
            0x0005: "avatar_anim",
            0x0006: "monster_anim",
            0x0007: "projectile",
            0x0008: "multi_part",
            0x0009: "complex_effect",
            0x000A: "large_effect",
            0x000B: "boss_anim",
            0x000C: "cutscene",
        }
        return names.get(self.type_code, f"type_{self.type_code:04X}")


class ANIParser:
    """ANI文件解析器"""

    @staticmethod
    def parse(data: bytes, path: str = "") -> Optional[ANIData]:
        """解析ANI文件"""
        if not data or len(data) < 4:
            return None

        # 检查是否为PVF格式（不是ANI）
        if data[0] == 0xB0 and data[1] == 0xD0:
            return None

        ani = ANIData()
        ani.path = path

        try:
            # 读取头部
            ani.type_code = struct.unpack_from('<H', data, 0)[0]
            ani.version = struct.unpack_from('<H', data, 2)[0]

            # 空/占位符动画
            if ani.type_code == 0x0000:
                return ani

            # 读取路径长度
            if len(data) < 8:
                return ani

            path_length = struct.unpack_from('<I', data, 4)[0]

            # 读取图像路径
            if path_length > 0 and path_length < 1000 and len(data) >= 8 + path_length:
                ani.image_path = data[8:8+path_length].decode('utf-8', errors='replace').rstrip('\x00')

            # 解析帧数据
            frame_offset = 8 + path_length
            ani.raw_frame_data = data[frame_offset:]
            ani.frames = ANIParser._parse_frames(ani.type_code, ani.raw_frame_data)

        except Exception:
            pass

        return ani

    @staticmethod
    def _parse_frames(type_code: int, data: bytes) -> List[ANIFrame]:
        """解析帧数据"""
        frames = []

        if len(data) < 4:
            return frames

        try:
            # 不同类型有不同的帧数据结构
            if type_code in [0x0001, 0x0002]:
                # 简单类型: 每帧 8 字节
                frame_size = 8
                count = len(data) // frame_size

                for i in range(min(count, 100)):  # 限制最大帧数
                    offset = i * frame_size
                    if offset + frame_size > len(data):
                        break

                    frame = ANIFrame()
                    frame.frame_id = struct.unpack_from('<H', data, offset)[0]
                    frame.offset_x = struct.unpack_from('<h', data, offset + 2)[0]
                    frame.offset_y = struct.unpack_from('<h', data, offset + 4)[0]
                    frame.duration = struct.unpack_from('<H', data, offset + 6)[0]
                    frames.append(frame)

            elif type_code in [0x0003, 0x0004, 0x0005]:
                # 中等复杂度: 每帧 12 字节
                frame_size = 12
                count = len(data) // frame_size

                for i in range(min(count, 100)):
                    offset = i * frame_size
                    if offset + frame_size > len(data):
                        break

                    frame = ANIFrame()
                    frame.frame_id = struct.unpack_from('<H', data, offset)[0]
                    frame.offset_x = struct.unpack_from('<h', data, offset + 2)[0]
                    frame.offset_y = struct.unpack_from('<h', data, offset + 4)[0]
                    frame.duration = struct.unpack_from('<H', data, offset + 6)[0]
                    frame.flags = struct.unpack_from('<I', data, offset + 8)[0]
                    frames.append(frame)

            else:
                # 其他类型: 尝试基本解析
                if len(data) >= 2:
                    # 假设第一个字节可能是帧数
                    frame_count = data[0]
                    if 0 < frame_count < 50:
                        for i in range(frame_count):
                            frames.append(ANIFrame(frame_id=i))

        except Exception:
            pass

        return frames

    @staticmethod
    def get_referenced_images(ani: ANIData) -> List[str]:
        """获取引用的图像文件列表"""
        if not ani.image_path:
            return []

        images = []
        path = ani.image_path

        # 检查是否有帧占位符
        if '%02d' in path or '%d' in path:
            # 生成帧序列
            for i in range(min(len(ani.frames), 100) if ani.frames else 10):
                try:
                    img_path = path.replace('%02d%02d', f'{i:02d}00')
                    img_path = img_path.replace('%02d', f'{i:02d}')
                    img_path = img_path.replace('%d', str(i))
                    images.append(img_path)
                except:
                    pass
        else:
            images.append(path)

        return images


# 测试
if __name__ == "__main__":
    import sys
    import os
    sys.path.insert(0, os.path.dirname(os.path.dirname(__file__)))
    from pvf_reader import PVFReader

    print("=" * 60)
    print("ANI Parser V2 Test")
    print("=" * 60)

    pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
    pvf.open()
    pvf.parse_index(max_entries=300000)

    # 测试不同类型
    type_samples = {}
    for entry in pvf.indexes:
        if not entry.name.endswith('.ani'):
            continue

        data = pvf.extract_file(entry)
        if not data or len(data) < 4:
            continue

        type_code = struct.unpack_from('<H', data, 0)[0]
        if type_code not in type_samples and type_code != 0xD0B0:
            ani = ANIParser.parse(data, entry.name)
            if ani:
                type_samples[type_code] = ani
                print(f"\nType 0x{type_code:04X}: {ani}")
                print(f"  File: {entry.name.split('/')[-1]}")
                print(f"  Image: {ani.image_path[:60]}")
                print(f"  Frames: {len(ani.frames)}")
                if ani.frames:
                    print(f"  First frame: id={ani.frames[0].frame_id}, dur={ani.frames[0].duration}")

        if len(type_samples) >= 15:
            break

    pvf.close()

    # 统计
    print(f"\n\nParsed {len(type_samples)} different ANI types")
