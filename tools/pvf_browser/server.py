#!/usr/bin/env python3
"""
PVF浏览器 - Web服务器
提供静态文件服务和API代理
"""

import http.server
import socketserver
import subprocess
import json
import urllib.parse
import os
import sys

PORT = 8000
PVF_PATH = "../../Script.pvf"
API_BINARY = "./backend/pvf_api"

class PVFBrowserHandler(http.server.SimpleHTTPRequestHandler):
    """自定义HTTP请求处理器"""

    def do_GET(self):
        """处理GET请求"""

        # 解析URL
        parsed = urllib.parse.urlparse(self.path)
        path = parsed.path
        query = urllib.parse.parse_qs(parsed.query)

        # API请求
        if path.startswith('/api'):
            self.handle_api(query)
        # 静态文件
        else:
            # 重定向到frontend目录
            if path == '/':
                path = '/index.html'

            self.path = '/frontend' + path
            return http.server.SimpleHTTPRequestHandler.do_GET(self)

    def handle_api(self, query):
        """处理API请求"""

        # 获取action参数
        action = query.get('action', [''])[0]

        if not action:
            self.send_json_response({
                'success': False,
                'error': '缺少action参数'
            })
            return

        # 构建命令行参数
        cmd = [API_BINARY, action, PVF_PATH]

        if action == 'list':
            path = query.get('path', [''])[0]
            file_type = query.get('type', ['equ'])[0]
            cmd.extend([path, file_type])

        elif action == 'parse':
            filepath = query.get('file', [''])[0]
            if not filepath:
                self.send_json_response({
                    'success': False,
                    'error': '缺少file参数'
                })
                return
            cmd.append(filepath)

        elif action == 'search':
            keyword = query.get('keyword', [''])[0]
            criteria = query.get('criteria', ['all'])[0]
            cmd.extend([keyword, criteria])

        # 执行API程序
        try:
            result = subprocess.run(
                cmd,
                capture_output=True,
                text=True,
                timeout=30
            )

            if result.returncode != 0:
                self.send_json_response({
                    'success': False,
                    'error': f'API执行失败: {result.stderr}'
                })
                return

            # 解析JSON输出
            try:
                data = json.loads(result.stdout)
                self.send_json_response(data)
            except json.JSONDecodeError:
                self.send_json_response({
                    'success': False,
                    'error': 'API返回的不是有效的JSON'
                })

        except subprocess.TimeoutExpired:
            self.send_json_response({
                'success': False,
                'error': 'API执行超时'
            })
        except Exception as e:
            self.send_json_response({
                'success': False,
                'error': f'服务器错误: {str(e)}'
            })

    def send_json_response(self, data):
        """发送JSON响应"""
        self.send_response(200)
        self.send_header('Content-Type', 'application/json; charset=utf-8')
        self.send_header('Access-Control-Allow-Origin', '*')
        self.end_headers()

        json_str = json.dumps(data, ensure_ascii=False, indent=2)
        self.wfile.write(json_str.encode('utf-8'))

    def log_message(self, format, *args):
        """自定义日志格式"""
        print(f"[{self.log_date_time_string()}] {format % args}")


def main():
    """主函数"""

    # 检查API二进制文件是否存在
    if not os.path.exists(API_BINARY) and not os.path.exists(API_BINARY + '.exe'):
        print(f"错误: 找不到API程序 {API_BINARY}")
        print("请先编译后端: cd backend && make")
        sys.exit(1)

    # 检查PVF文件是否存在
    if not os.path.exists(PVF_PATH):
        print(f"警告: 找不到PVF文件 {PVF_PATH}")
        print("如果PVF文件在其他位置，请修改server.py中的PVF_PATH变量")

    # 启动服务器
    with socketserver.TCPServer(("", PORT), PVFBrowserHandler) as httpd:
        print("=" * 60)
        print(f"🎮 DNF装备浏览器服务器已启动")
        print("=" * 60)
        print(f"📡 服务器地址: http://localhost:{PORT}")
        print(f"📁 PVF文件: {PVF_PATH}")
        print(f"🔧 API程序: {API_BINARY}")
        print("=" * 60)
        print("按 Ctrl+C 停止服务器")
        print("=" * 60)

        try:
            httpd.serve_forever()
        except KeyboardInterrupt:
            print("\n服务器已停止")


if __name__ == "__main__":
    main()
