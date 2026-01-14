#!/bin/bash
# execute_observable_plan.sh - Main control script for observable execution plan
# Executes 14 steps with detailed error analysis after each step

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
cd "$PROJECT_DIR"

# Color codes
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
MAGENTA='\033[0;35m'
CYAN='\033[0;36m'
BOLD='\033[1m'
NC='\033[0m'

# State file
STATE_FILE="logs/execution_state.txt"
BACKUP_DIR="logs/backups"

# Initialize
mkdir -p logs reports "$BACKUP_DIR"

# Load state or start from beginning
if [ -f "$STATE_FILE" ]; then
    CURRENT_STEP=$(cat "$STATE_FILE")
    echo -e "${YELLOW}⚠ 检测到上次执行进度: 步骤 $CURRENT_STEP${NC}"
    echo -e "是否从步骤 $CURRENT_STEP 继续? [Y/n]: "
    read -r response
    if [[ ! "$response" =~ ^[Yy]$ ]] && [ -n "$response" ]; then
        CURRENT_STEP=0
    fi
else
    CURRENT_STEP=0
fi

# Save state
save_state() {
    echo $1 > "$STATE_FILE"
}

# Backup current state
backup_step() {
    local step=$1
    local name=$2
    echo -e "${CYAN}📦 备份当前状态...${NC}"
    tar -czf "$BACKUP_DIR/step$(printf '%02d' $step)_${name// /_}_$(date +%Y%m%d_%H%M%S).tar.gz" \
        src_split/df_game_r_decl.h 2>/dev/null || true
    echo -e "${GREEN}✓ 备份完成${NC}"
}

# Restore from backup
restore_step() {
    local step=$1
    echo -e "${YELLOW}⚠ 恢复到步骤 $step...${NC}"
    local backup_file=$(ls -t "$BACKUP_DIR"/step$(printf '%02d' $step)_*.tar.gz 2>/dev/null | head -1)
    if [ -n "$backup_file" ]; then
        tar -xzf "$backup_file"
        echo -e "${GREEN}✓ 恢复完成${NC}"
        return 0
    else
        echo -e "${RED}✗ 未找到步骤 $step 的备份文件${NC}"
        return 1
    fi
}

# Compile and capture errors
compile_step() {
    local step=$1
    local name=$2

    echo -e "${CYAN}🔨 编译中...${NC}"
    local log_file="logs/step$(printf '%02d' $step)_build.log"
    local start_time=$(date +%s)

    cd src_split
    make clean >/dev/null 2>&1 || true
    make -j$(nproc) all > "$log_file" 2>&1 || true
    cd ..

    local end_time=$(date +%s)
    local duration=$((end_time - start_time))
    local minutes=$((duration / 60))
    local seconds=$((duration % 60))

    echo "${minutes}分${seconds}秒" > "logs/step$(printf '%02d' $step)_time.txt"

    echo -e "${GREEN}✓ 编译完成 (${minutes}分${seconds}秒)${NC}"
    echo "$log_file"
}

# Analyze errors
analyze_step() {
    local step=$1
    local name=$2
    local log_file=$3
    local prev_log=${4:-""}

    echo ""
    bash tools/analyze_errors_detailed.sh $step "$name" "$log_file" "$prev_log"
    echo ""
}

# Interactive menu
show_menu() {
    local step=$1
    local total=$2
    local next_name=$3

    echo -e "${BOLD}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${BOLD}步骤 $step/$total 完成${NC}"
    echo -e "${BOLD}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo ""
    echo -e "下一步: ${CYAN}步骤 $((step+1)) - $next_name${NC}"
    echo ""
    echo -e "  ${GREEN}[C]${NC} 继续下一步"
    echo -e "  ${YELLOW}[S]${NC} 跳过下一步"
    echo -e "  ${BLUE}[R]${NC} 回滚到指定步骤"
    echo -e "  ${MAGENTA}[V]${NC} 查看详细日志"
    echo -e "  ${CYAN}[H]${NC} 生成HTML报告"
    echo -e "  ${RED}[Q]${NC} 退出并保存进度"
    echo ""
    echo -n "请选择 [C/S/R/V/H/Q]: "
}

# Execute a step
execute_step() {
    local step=$1
    local name=$2
    local action=$3
    local prev_log=${4:-""}

    echo ""
    echo -e "${BOLD}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo -e "${BOLD}执行步骤 $step: $name${NC}"
    echo -e "${BOLD}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
    echo ""

    # Backup before modification
    backup_step $step "$name"

    # Execute the action
    echo -e "${YELLOW}🔧 执行修改操作...${NC}"
    eval "$action"
    echo -e "${GREEN}✓ 修改完成${NC}"
    echo ""

    # Compile
    local log_file=$(compile_step $step "$name")

    # Analyze
    analyze_step $step "$name" "$log_file" "$prev_log"

    # Save state
    save_state $step

    echo "$log_file"
}

# 14 step definitions
declare -a STEPS
declare -a ACTIONS

STEPS[0]="基线测量"
ACTIONS[0]="echo '不做修改，仅编译'"

STEPS[1]="注释 std::_Select1st (StringUnorderedMap)"
ACTIONS[1]="python3 tools/comment_select1st.py --type StringUnorderedMap"

STEPS[2]="注释 std::_Select1st (其他类型)"
ACTIONS[2]="python3 tools/comment_select1st.py --type others"

STEPS[3]="注释 boost::gregorian 声明"
ACTIONS[3]="python3 tools/comment_boost.py --namespace gregorian"

STEPS[4]="注释 boost 其他命名空间声明"
ACTIONS[4]="python3 tools/comment_boost.py --namespace others"

STEPS[5]="添加 ARAD 命名空间前向声明"
ACTIONS[5]="python3 tools/add_forward_decl.py --namespace ARAD"

STEPS[6]="添加 WongWork 命名空间前向声明"
ACTIONS[6]="python3 tools/add_forward_decl.py --namespace WongWork"

STEPS[7]="添加 QuickParty 命名空间前向声明"
ACTIONS[7]="python3 tools/add_forward_decl.py --namespace QuickParty"

STEPS[8]="添加其他命名空间前向声明"
ACTIONS[8]="python3 tools/add_forward_decl.py --namespace others"

STEPS[9]="注释 std::_Rb_tree 模板成员"
ACTIONS[9]="python3 tools/comment_stl_members.py --type _Rb_tree"

STEPS[10]="注释 std::pair 模板成员"
ACTIONS[10]="python3 tools/comment_stl_members.py --type pair"

STEPS[11]="注释 std::map 模板成员"
ACTIONS[11]="python3 tools/comment_stl_members.py --type map"

STEPS[12]="注释其他 STL 模板成员"
ACTIONS[12]="python3 tools/comment_stl_members.py --type others"

STEPS[13]="最终测量"
ACTIONS[13]="echo '不做修改，仅编译'"

TOTAL_STEPS=14

# Main execution loop
echo -e "${BOLD}${CYAN}"
echo "╔════════════════════════════════════════════════════════════╗"
echo "║         高可观察性编译修复执行计划                         ║"
echo "║            Observable Execution Plan                       ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo -e "${NC}"
echo ""
echo -e "总步骤数: ${BOLD}$TOTAL_STEPS${NC}"
echo -e "当前进度: ${BOLD}步骤 $CURRENT_STEP${NC}"
echo -e "预计时间: ${BOLD}2-3 小时${NC}"
echo ""

PREV_LOG=""

for ((i=$CURRENT_STEP; i<$TOTAL_STEPS; i++)); do
    STEP_NAME="${STEPS[$i]}"
    STEP_ACTION="${ACTIONS[$i]}"

    # Execute step
    LOG_FILE=$(execute_step $i "$STEP_NAME" "$STEP_ACTION" "$PREV_LOG")
    PREV_LOG="$LOG_FILE"

    # Show menu (except for last step)
    if [ $i -lt $((TOTAL_STEPS - 1)) ]; then
        while true; do
            show_menu $i $TOTAL_STEPS "${STEPS[$((i+1))]}"
            read -r choice

            case "$choice" in
                [Cc])
                    break
                    ;;
                [Ss])
                    echo -e "${YELLOW}⏭ 跳过步骤 $((i+1))${NC}"
                    i=$((i+1))
                    break
                    ;;
                [Rr])
                    echo -n "回滚到步骤 (0-$i): "
                    read -r rollback_step
                    if [ "$rollback_step" -ge 0 ] && [ "$rollback_step" -le $i ]; then
                        restore_step $rollback_step
                        i=$((rollback_step-1))
                        PREV_LOG=$(ls -t logs/step$(printf '%02d' $rollback_step)_build.log 2>/dev/null | head -1 || echo "")
                        break
                    else
                        echo -e "${RED}✗ 无效的步骤号${NC}"
                    fi
                    ;;
                [Vv])
                    echo ""
                    echo -e "${CYAN}最近10条错误:${NC}"
                    tail -20 "$LOG_FILE" | grep "error:" | head -10
                    echo ""
                    ;;
                [Hh])
                    echo -e "${CYAN}🌐 生成HTML报告...${NC}"
                    python3 tools/generate_html_report.py
                    echo -e "${GREEN}✓ 报告已生成: reports/observable_execution_report.html${NC}"
                    echo ""
                    ;;
                [Qq])
                    echo -e "${YELLOW}💾 保存进度并退出...${NC}"
                    save_state $i
                    exit 0
                    ;;
                *)
                    echo -e "${RED}无效选择，请重新输入${NC}"
                    ;;
            esac
        done
    fi
done

# Final summary
echo ""
echo -e "${BOLD}${GREEN}"
echo "╔════════════════════════════════════════════════════════════╗"
echo "║                    执行完成！                              ║"
echo "║                 Execution Complete!                        ║"
echo "╚════════════════════════════════════════════════════════════╝"
echo -e "${NC}"
echo ""
echo -e "${CYAN}📊 生成最终报告...${NC}"
python3 tools/generate_html_report.py

echo ""
echo -e "${GREEN}✓ 所有步骤已完成${NC}"
echo -e "${CYAN}📄 查看报告: reports/observable_execution_report.html${NC}"
echo -e "${CYAN}📋 查看日志: logs/summary.json${NC}"
echo ""

# Clean up state file
rm -f "$STATE_FILE"
