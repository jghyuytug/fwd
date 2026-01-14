#!/bin/bash
# ========================================
# DNF GM工具 - 快速启动脚本
# Week 3 基础设施配置
# ========================================

set -e  # 遇到错误立即退出

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# ========================================
# 函数定义
# ========================================

log_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# 检查环境变量
check_env() {
    log_info "检查环境变量..."

    if [ ! -f ".env" ]; then
        log_error ".env文件不存在！"
        log_info "请从.env.example复制并填写配置："
        log_info "  cp  .env"
        log_info "  vim .env"
        exit 1
    fi

    # 检查必需的环境变量
    source .env

    required_vars=(
        "MYSQL_ROOT_PASSWORD"
        "GM_DB_PASSWORD"
        "CANAL_DB_PASSWORD"
        "PRODUCTION_DB_HOST"
    )

    for var in "${required_vars[@]}"; do
        if [ -z "${!var}" ] || [ "${!var}" == "CHANGE_ME"* ]; then
            log_error "环境变量 $var 未设置或使用默认值"
            log_info "请编辑 .env 文件并填写正确的值"
            exit 1
        fi
    done

    log_info "环境变量检查通过 ✓"
}

# 检查Docker环境
check_docker() {
    log_info "检查Docker环境..."

    if ! command -v docker &> /dev/null; then
        log_error "Docker未安装！"
        log_info "请先安装Docker: https://docs.docker.com/get-docker/"
        exit 1
    fi

    if ! command -v docker-compose &> /dev/null; then
        log_error "Docker Compose未安装！"
        log_info "请先安装Docker Compose: https://docs.docker.com/compose/install/"
        exit 1
    fi

    # 检查Docker是否运行
    if ! docker info &> /dev/null; then
        log_error "Docker未运行！"
        log_info "请启动Docker服务: sudo systemctl start docker"
        exit 1
    fi

    log_info "Docker环境检查通过 ✓"
}

# 生成JWT密钥对
generate_jwt_keys() {
    log_info "生成JWT密钥对..."

    if [ ! -f "../config/jwt_private.pem" ]; then
        # 生成私钥
        openssl genrsa -out ../config/jwt_private.pem 2048
        chmod 600 ../config/jwt_private.pem

        # 生成公钥
        openssl rsa -in ../config/jwt_private.pem -pubout -out ../config/jwt_public.pem
        chmod 644 ../config/jwt_public.pem

        log_info "JWT密钥对生成成功 ✓"
    else
        log_warn "JWT密钥对已存在，跳过生成"
    fi
}

# 创建SSL证书（自签名，用于开发环境）
generate_ssl_cert() {
    log_info "生成SSL证书（自签名）..."

    if [ ! -f "../nginx/ssl/fullchain.pem" ]; then
        mkdir -p ../nginx/ssl

        # 生成自签名证书
        openssl req -x509 -nodes -days 365 -newkey rsa:2048 \
            -keyout ../nginx/ssl/privkey.pem \
            -out ../nginx/ssl/fullchain.pem \
            -subj "/C=CN/ST=Taiwan/L=Taipei/O=DNF GM Tools/CN=gm.dnf.game"

        chmod 600 ../nginx/ssl/privkey.pem
        chmod 644 ../nginx/ssl/fullchain.pem

        log_warn "使用自签名证书（仅用于开发环境）"
        log_info "生产环境请使用Let's Encrypt: sudo certbot certonly --webroot -w /var/www/certbot -d gm.dnf.game"
    else
        log_warn "SSL证书已存在，跳过生成"
    fi
}

# 启动服务
start_services() {
    log_info "启动Docker服务..."

    # 拉取镜像
    log_info "拉取Docker镜像..."
    docker-compose pull

    # 启动服务
    log_info "启动所有服务..."
    docker-compose up -d

    log_info "等待服务启动..."
    sleep 10

    # 检查服务状态
    docker-compose ps
}

# 检查服务健康状态
check_health() {
    log_info "检查服务健康状态..."

    # 检查GM镜像数据库
    if docker-compose exec -T gm-mirror-db mysqladmin ping -h localhost -u root -p"${MYSQL_ROOT_PASSWORD}" &> /dev/null; then
        log_info "✓ GM镜像数据库: 运行正常"
    else
        log_error "✗ GM镜像数据库: 未响应"
    fi

    # 检查Canal
    if curl -f http://localhost:11112/metrics &> /dev/null; then
        log_info "✓ Canal同步服务: 运行正常"
    else
        log_warn "✗ Canal同步服务: 未响应（可能正在启动）"
    fi

    # 检查GM工具服务
    if curl -f http://localhost:8001/health &> /dev/null; then
        log_info "✓ GM工具服务: 运行正常"
    else
        log_warn "✗ GM工具服务: 未响应（可能正在启动）"
    fi

    # 检查Nginx
    if curl -f https://localhost/health --insecure &> /dev/null; then
        log_info "✓ Nginx反向代理: 运行正常"
    else
        log_warn "✗ Nginx反向代理: 未响应"
    fi

    # 检查Prometheus
    if curl -f http://localhost:9090/-/healthy &> /dev/null; then
        log_info "✓ Prometheus监控: 运行正常"
    else
        log_warn "✗ Prometheus监控: 未响应"
    fi

    # 检查Grafana
    if curl -f http://localhost:3000/api/health &> /dev/null; then
        log_info "✓ Grafana可视化: 运行正常"
    else
        log_warn "✗ Grafana可视化: 未响应"
    fi
}

# 显示访问信息
show_access_info() {
    log_info ""
    log_info "=========================================="
    log_info "  DNF GM工具 - 服务访问信息"
    log_info "=========================================="
    log_info ""
    log_info "Web界面:"
    log_info "  - GM工具前端: https://localhost/ (开发环境忽略SSL警告)"
    log_info ""
    log_info "监控面板:"
    log_info "  - Prometheus: http://localhost:9090"
    log_info "  - Grafana: http://localhost:3000 (默认账号: admin / ${GRAFANA_ADMIN_PASSWORD})"
    log_info ""
    log_info "后端API:"
    log_info "  - GM工具API: http://localhost:8001/api/"
    log_info ""
    log_info "数据库:"
    log_info "  - GM镜像数据库: localhost:3307 (root / ${MYSQL_ROOT_PASSWORD})"
    log_info ""
    log_info "查看日志:"
    log_info "  docker-compose logs -f [服务名]"
    log_info ""
    log_info "停止服务:"
    log_info "  docker-compose down"
    log_info ""
    log_info "=========================================="
}

# ========================================
# 主流程
# ========================================

main() {
    log_info "DNF GM工具 - 快速启动脚本"
    log_info "Week 3 基础设施部署"
    log_info ""

    # 进入docker目录
    cd "$(dirname "$0")"

    # 执行检查
    check_env
    check_docker

    # 生成密钥和证书
    generate_jwt_keys
    generate_ssl_cert

    # 启动服务
    start_services

    # 等待服务完全启动
    log_info "等待服务完全启动（30秒）..."
    sleep 30

    # 检查健康状态
    check_health

    # 显示访问信息
    show_access_info

    log_info ""
    log_info "${GREEN}部署完成！${NC}"
}

# 执行主流程
main "$@"
