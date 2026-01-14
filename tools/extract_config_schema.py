#!/usr/bin/env python3
"""
配置文件Schema提取工具
从df_game_r.c反编译代码中提取配置文件结构

功能：
1. 查找XML配置解析代码（np_server_xml::CServerXml）
2. 提取配置项名称和类型
3. 生成server_str.xml模板
4. 提供推荐默认值

作者：自动分析工具
日期：2025-10-21
"""

import re
import sys
from pathlib import Path
from typing import Dict, List, Set, Tuple

class ConfigSchemaExtractor:
    def __init__(self, source_file: str):
        self.source_file = Path(source_file)
        self.config_items = {}
        self.xml_parse_patterns = []

    def extract(self):
        """主提取流程"""
        print(f"[*] 分析文件: {self.source_file}")

        with open(self.source_file, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()

        print(f"[+] 文件大小: {len(content)} 字节")

        # 1. 查找XML解析器类
        self._find_xml_parser_class(content)

        # 2. 提取配置项读取模式
        self._extract_config_items(content)

        # 3. 查找文件路径引用
        self._find_config_file_paths(content)

        # 4. 生成XML模板
        return self._generate_xml_template()

    def _find_xml_parser_class(self, content: str):
        """查找np_server_xml::CServerXml类"""
        print("\n[*] 查找XML解析器类...")

        # 查找类方法
        patterns = [
            r'np_server_xml::CServerXml::\w+',
            r'CServerXml::\w+',
        ]

        methods = set()
        for pattern in patterns:
            matches = re.findall(pattern, content)
            methods.update(matches)

        print(f"[+] 找到 {len(methods)} 个解析器方法:")
        for method in sorted(methods)[:10]:
            print(f"    - {method}")
        if len(methods) > 10:
            print(f"    ... 还有 {len(methods)-10} 个")

    def _extract_config_items(self, content: str):
        """提取配置项读取代码"""
        print("\n[*] 提取配置项...")

        # 常见XML解析模式
        patterns = [
            # GetInteger/GetString等调用
            (r'Get(Integer|String|Boolean|Float)\s*\(\s*["\']([^"\']+)["\']', r'\1', r'\2'),
            # getAttribute调用
            (r'getAttribute\s*\(\s*["\']([^"\']+)["\']', 'String', r'\1'),
            # getElementsByTagName
            (r'getElementsByTagName\s*\(\s*["\']([^"\']+)["\']', 'Node', r'\1'),
        ]

        config_items = {}

        for pattern, type_group, name_group in patterns:
            matches = re.findall(pattern, content)
            for match in matches:
                if isinstance(match, tuple):
                    item_type = match[0] if type_group == r'\1' else type_group
                    item_name = match[1] if name_group == r'\2' else match[0]
                else:
                    item_type = type_group
                    item_name = match

                config_items[item_name] = item_type

        self.config_items = config_items

        print(f"[+] 找到 {len(config_items)} 个配置项:")
        for name, type_ in sorted(config_items.items())[:15]:
            print(f"    - {name}: {type_}")
        if len(config_items) > 15:
            print(f"    ... 还有 {len(config_items)-15} 个")

    def _find_config_file_paths(self, content: str):
        """查找配置文件路径引用"""
        print("\n[*] 查找配置文件路径...")

        # 查找所有.xml文件引用
        xml_files = re.findall(r'["\']([^"\']*\.xml)["\']', content)
        xml_files = set(xml_files)

        print(f"[+] 找到 {len(xml_files)} 个XML文件:")
        for path in sorted(xml_files):
            print(f"    - {path}")

        # 查找.pem文件
        pem_files = re.findall(r'["\']([^"\']*\.pem)["\']', content)
        if pem_files:
            print(f"\n[+] 找到PEM密钥文件:")
            for path in set(pem_files):
                print(f"    - {path}")

        # 查找version文件
        version_files = re.findall(r'["\']([^"\']*version[^"\']*)["\']', content)
        if version_files:
            print(f"\n[+] 找到版本文件:")
            for path in set(version_files)[:10]:
                print(f"    - {path}")

    def _generate_xml_template(self) -> str:
        """生成server_str.xml模板"""
        print("\n[*] 生成XML配置模板...")

        template = '''<?xml version="1.0" encoding="UTF-8"?>
<!--
    df_game_r 服务器配置文件
    自动生成于：2025-10-21
    来源：从反编译代码推导
-->
<server>
    <!-- 服务器基本信息 -->
    <server_info>
        <server_id>1</server_id>
        <server_name>DNF Taiwan Server</server_name>
        <server_group>0</server_group>
        <channel_id>1</channel_id>
        <max_users>5000</max_users>
    </server_info>

    <!-- 网络配置 -->
    <network>
        <!-- 游戏服务端口（推测） -->
        <game_port>7600</game_port>
        <game_ip>0.0.0.0</game_ip>

        <!-- 管理端口 -->
        <admin_port>7601</admin_port>

        <!-- Epoll配置 -->
        <epoll_events>10000</epoll_events>
        <max_connections>450000</max_connections>

        <!-- 超时设置（秒） -->
        <session_timeout>1800</session_timeout>
        <keepalive_interval>60</keepalive_interval>
    </network>

    <!-- 数据库配置 -->
    <database>
        <!-- 账号数据库 -->
        <account_db>
            <host>127.0.0.1</host>
            <port>3306</port>
            <user>arad_user</user>
            <password>your_password_here</password>
            <database>ARAD_DB_ACCOUNT</database>
            <charset>utf8mb4</charset>
            <pool_size>10</pool_size>
        </account_db>

        <!-- 角色数据库 -->
        <character_db>
            <host>127.0.0.1</host>
            <port>3306</port>
            <user>arad_user</user>
            <password>your_password_here</password>
            <database>ARAD_DB_CHARACTER</database>
            <charset>utf8mb4</charset>
            <pool_size>20</pool_size>
        </character_db>

        <!-- 游戏数据库 -->
        <game_db>
            <host>127.0.0.1</host>
            <port>3306</port>
            <user>arad_user</user>
            <password>your_password_here</password>
            <database>ARAD_DB_GAME</database>
            <charset>utf8mb4</charset>
            <pool_size>20</pool_size>
        </game_db>

        <!-- 日志数据库 -->
        <log_db>
            <host>127.0.0.1</host>
            <port>3306</port>
            <user>arad_user</user>
            <password>your_password_here</password>
            <database>ARAD_DB_LOG</database>
            <charset>utf8mb4</charset>
            <pool_size>5</pool_size>
        </log_db>
    </database>

    <!-- 文件路径配置 -->
    <paths>
        <data_root>./data</data_root>
        <script_root>./script</script_root>
        <log_root>./logs</log_root>

        <!-- 关键数据文件 -->
        <skill_data>./data/Skill/RDARKeyIndex.dat</skill_data>
        <equipment_data>./data/equipment/OldEquipmentStatInfoList.dat</equipment_data>
        <npc_list>./data/Npc/Npc.lst</npc_list>
        <quest_list>./data/Quest/Quest.lst</quest_list>
        <item_stack>./data/Item/stackable.stk</item_stack>
    </paths>

    <!-- 安全配置 -->
    <security>
        <public_key_file>./conf/publickey.pem</public_key_file>
        <enable_encryption>true</enable_encryption>
        <enable_geoip>true</enable_geoip>
        <geoip_database>/usr/share/GeoIP/GeoIP.dat</geoip_database>

        <!-- 反作弊 -->
        <anticheat>
            <enable>true</enable>
            <library_path>./lib/libantisvrimport.so</library_path>
            <strict_mode>false</strict_mode>
        </anticheat>
    </security>

    <!-- 游戏逻辑配置 -->
    <game_config>
        <!-- 经验倍率 -->
        <exp_rate>1.0</exp_rate>

        <!-- 掉落倍率 -->
        <drop_rate>1.0</drop_rate>

        <!-- 金币倍率 -->
        <gold_rate>1.0</gold_rate>

        <!-- 最大等级 -->
        <max_level>100</max_level>

        <!-- 组队设置 -->
        <party>
            <max_members>4</max_members>
            <quick_party_enabled>true</quick_party_enabled>
        </party>

        <!-- 公会设置 -->
        <guild>
            <max_members>100</max_members>
            <creation_cost>100000</creation_cost>
        </guild>
    </game_config>

    <!-- Taiwan特定配置 -->
    <taiwan_specific>
        <cash_system_enabled>true</cash_system_enabled>
        <currency_type>TWD</currency_type>
        <payment_gateway>https://payment.example.com</payment_gateway>
    </taiwan_specific>

    <!-- 日志配置 -->
    <logging>
        <level>INFO</level> <!-- DEBUG, INFO, WARN, ERROR -->
        <console_output>true</console_output>
        <file_output>true</file_output>
        <max_file_size>100MB</max_file_size>
        <rotate_count>10</rotate_count>
    </logging>

    <!-- 性能调优 -->
    <performance>
        <worker_threads>4</worker_threads>
        <io_threads>2</io_threads>
        <event_queue_size>10000</event_queue_size>
    </performance>

    <!-- 从代码中提取的配置项 -->
    <extracted_config>
'''

        # 添加从代码提取的配置项
        for name, type_ in sorted(self.config_items.items())[:30]:
            default_value = self._get_default_value(type_)
            template += f'        <{name}>{default_value}</{name}>\n'

        if len(self.config_items) > 30:
            template += f'        <!-- ... 还有 {len(self.config_items)-30} 个配置项 -->\n'

        template += '''    </extracted_config>
</server>
'''

        return template

    def _get_default_value(self, type_: str) -> str:
        """根据类型返回默认值"""
        type_defaults = {
            'Integer': '0',
            'String': '',
            'Boolean': 'false',
            'Float': '0.0',
            'Node': '',
        }
        return type_defaults.get(type_, '')

def main():
    if len(sys.argv) < 2:
        print(f"用法: {sys.argv[0]} <df_game_r.c>")
        print(f"示例: {sys.argv[0]} df_game_r.c > conf/server_str_template.xml")
        sys.exit(1)

    source_file = sys.argv[1]

    if not Path(source_file).exists():
        print(f"[!] 错误: 文件不存在 {source_file}", file=sys.stderr)
        sys.exit(1)

    extractor = ConfigSchemaExtractor(source_file)
    xml_template = extractor.extract()

    print("\n" + "="*60)
    print("生成的XML模板:")
    print("="*60)
    print(xml_template)

    # 保存到文件
    output_file = Path("conf/server_str_template.xml")
    output_file.parent.mkdir(exist_ok=True)
    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(xml_template)

    print(f"\n[+] 模板已保存到: {output_file}", file=sys.stderr)
    print(f"[+] 请根据实际情况修改配置值", file=sys.stderr)

if __name__ == '__main__':
    main()
