#!/usr/bin/env python3
"""
SSH免密登录自动配置脚本
"""
import os
import sys

def setup_ssh_key():
    vm_host = "192.168.200.131"
    vm_user = "root"
    vm_password = "88888888"  # 根据CLAUDE.md中的数据库密码

    # 读取本地公钥
    pubkey_path = os.path.expanduser("~/.ssh/id_rsa.pub")
    try:
        with open(pubkey_path, 'r') as f:
            public_key = f.read().strip()
    except FileNotFoundError:
        print(f"错误: 未找到公钥文件 {pubkey_path}")
        return False

    print(f"=== 配置SSH免密登录到 {vm_user}@{vm_host} ===\n")

    # 尝试使用paramiko
    try:
        import paramiko

        # 创建SSH客户端
        ssh = paramiko.SSHClient()
        ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())

        print(f"正在连接到 {vm_host}...")
        ssh.connect(vm_host, username=vm_user, password=vm_password, timeout=10)
        print("✓ 连接成功\n")

        # 创建.ssh目录并设置权限
        commands = [
            "mkdir -p ~/.ssh",
            "chmod 700 ~/.ssh",
            f"echo '{public_key}' >> ~/.ssh/authorized_keys",
            "chmod 600 ~/.ssh/authorized_keys",
            "sort -u ~/.ssh/authorized_keys -o ~/.ssh/authorized_keys"  # 去重
        ]

        for cmd in commands:
            print(f"执行: {cmd[:50]}...")
            stdin, stdout, stderr = ssh.exec_command(cmd)
            exit_code = stdout.channel.recv_exit_status()
            if exit_code != 0:
                error = stderr.read().decode()
                if error:
                    print(f"警告: {error}")

        ssh.close()
        print("\n✓ 公钥已添加到 ~/.ssh/authorized_keys")

        # 测试免密登录
        print("\n=== 测试免密登录 ===")
        ssh2 = paramiko.SSHClient()
        ssh2.set_missing_host_key_policy(paramiko.AutoAddPolicy())

        key_path = os.path.expanduser("~/.ssh/id_rsa")
        private_key = paramiko.RSAKey.from_private_key_file(key_path)

        ssh2.connect(vm_host, username=vm_user, pkey=private_key, timeout=5)
        stdin, stdout, stderr = ssh2.exec_command("echo '免密登录成功' && hostname")
        result = stdout.read().decode().strip()
        print(f"✓ {result}")
        ssh2.close()

        print(f"\n✓ 配置完成！现在可以使用 'ssh {vm_user}@{vm_host}' 或 'ssh vm131' 免密登录")
        return True

    except ImportError:
        print("错误: 未安装paramiko库")
        print("\n请安装: pip install paramiko")
        print("\n或者手动配置SSH密钥:")
        print(f"1. ssh {vm_user}@{vm_host}")
        print(f"2. mkdir -p ~/.ssh && chmod 700 ~/.ssh")
        print(f"3. echo '{public_key[:50]}...' >> ~/.ssh/authorized_keys")
        print(f"4. chmod 600 ~/.ssh/authorized_keys")
        return False

    except Exception as e:
        print(f"\n错误: {e}")
        print(f"\n可能的原因:")
        print(f"1. 密码不正确 (当前使用: {vm_password})")
        print(f"2. 网络连接问题")
        print(f"3. SSH服务未启动")
        return False

if __name__ == "__main__":
    success = setup_ssh_key()
    sys.exit(0 if success else 1)
