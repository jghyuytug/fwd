#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
PTL Particle Parser
===================
解析PVF中的粒子效果定义文件 (.ptl)

格式特点:
- PVF键值对格式 (魔数 0xD0B0)
- 结构较小 (12-427字节，1-124条目)
- 定义粒子系统效果

关键字段:
  0x035E = 位置偏移
  0x0328 = 持续时间
  0x0390 = 粒子类型
  0x035F = 资源引用
  0x1368 = 参数
  0x136C = 贴图引用
  0x0361 = 速度/大小
  0x136B = 发射数量
"""

import struct
from dataclasses import dataclass, field
from typing import List, Optional, Dict, Any
import sys
import os

sys.path.insert(0, os.path.dirname(os.path.dirname(__file__)))
from pvf_data_parser import parse_pvf_data, PVFDataContainer, PVFValueType


# ============================================================================
# 键ID常量
# ============================================================================

class PTL:
    """粒子效果文件键ID"""
    POSITION = 0x035E           # 位置偏移
    DURATION = 0x0328           # 持续时间
    PARTICLE_TYPE = 0x0390      # 粒子类型
    RESOURCE = 0x035F           # 资源引用
    PARAM = 0x1368              # 参数
    TEXTURE = 0x136C            # 贴图引用
    SPEED = 0x0361              # 速度
    EMIT_COUNT = 0x136B         # 发射数量
    EMITTER = 0x035B            # 发射器类型
    ANIMATION = 0x0723          # 动画引用
    LIFETIME = 0x1369           # 生命周期
    EFFECT = 0x136F             # 效果引用
    SCALE = 0x0722              # 缩放
    ALPHA = 0x0725              # 透明度
    COLOR = 0x1372              # 颜色
    BLEND = 0x0394              # 混合模式


# ============================================================================
# 数据结构
# ============================================================================

@dataclass
class ParticleData:
    """粒子效果数据"""
    particle_type: int = 0      # 粒子类型
    resource: int = 0           # 资源引用
    texture: int = 0            # 贴图引用
    duration: int = 0           # 持续时间 (毫秒)
    emit_count: int = 1         # 发射数量
    lifetime: int = 0           # 粒子生命周期
    speed: int = 0              # 速度
    scale: int = 100            # 缩放
    alpha: int = 100            # 透明度
    position: int = 0           # 位置偏移
    emitter: int = 0            # 发射器类型
    animation: int = 0          # 动画引用
    effect: int = 0             # 效果引用
    color: int = 0              # 颜色
    blend: int = 0              # 混合模式

    raw_entries: int = 0
    path: str = ""

    def __repr__(self):
        return (f"Particle(type={self.particle_type}, emit={self.emit_count}, "
                f"duration={self.duration}ms, speed={self.speed})")


# ============================================================================
# 解析器
# ============================================================================

class ParticleParser:
    """PTL粒子效果文件解析器"""

    @staticmethod
    def parse(data: bytes, path: str = "") -> Optional[ParticleData]:
        if len(data) < 2:
            return None

        if data[0] != 0xB0 or data[1] != 0xD0:
            return None

        try:
            container = parse_pvf_data(data)
            ptl = ParticleData()
            ptl.raw_entries = len(container.entries)
            ptl.path = path

            ptl.particle_type = container.get_int(PTL.PARTICLE_TYPE)
            ptl.resource = container.get_ref(PTL.RESOURCE)
            ptl.texture = container.get_ref(PTL.TEXTURE)
            ptl.duration = container.get_int(PTL.DURATION)
            ptl.emit_count = container.get_int(PTL.EMIT_COUNT, 1)
            ptl.lifetime = container.get_int(PTL.LIFETIME)
            ptl.speed = container.get_int(PTL.SPEED)
            ptl.scale = container.get_int(PTL.SCALE, 100)
            ptl.alpha = container.get_int(PTL.ALPHA, 100)
            ptl.position = container.get_int(PTL.POSITION)
            ptl.emitter = container.get_int(PTL.EMITTER)
            ptl.animation = container.get_ref(PTL.ANIMATION)
            ptl.effect = container.get_ref(PTL.EFFECT)
            ptl.color = container.get_int(PTL.COLOR)
            ptl.blend = container.get_int(PTL.BLEND)

            return ptl

        except Exception as e:
            return None

    @staticmethod
    def is_valid_ptl(data: bytes) -> bool:
        if len(data) < 2:
            return False
        return data[0] == 0xB0 and data[1] == 0xD0


if __name__ == "__main__":
    sys.path.insert(0, '..')
    from pvf_reader import PVFReader

    print("=" * 60)
    print("PTL Parser Test")
    print("=" * 60)

    pvf = PVFReader('C:/Users/waw/Desktop/新建文件夹 (2)/Script.pvf')
    if not pvf.open():
        print("Failed to open PVF")
        sys.exit(1)

    pvf.parse_index(max_entries=50000)

    count = 0
    for entry in pvf.indexes:
        if entry.name.endswith('.ptl'):
            data = pvf.extract_file(entry)
            if data:
                ptl = ParticleParser.parse(data, entry.name)
                if ptl:
                    if count < 10:
                        print(f"  {entry.name.split('/')[-1]}: {ptl}")
                    count += 1
                    if count >= 1000:
                        break

    print(f"\nParsed: {count} PTL files")
    pvf.close()
