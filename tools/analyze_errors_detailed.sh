#!/bin/bash
# analyze_errors_detailed.sh - Detailed compilation error analysis tool
# Provides comprehensive statistics, trends, and insights for each build step

set -e

STEP_NUM=${1:-0}
STEP_NAME=${2:-"Baseline"}
LOG_FILE=${3:-"logs/current_build.log"}
PREV_LOG=${4:-""}

# Color codes
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
MAGENTA='\033[0;35m'
CYAN='\033[0;36m'
BOLD='\033[1m'
NC='\033[0m' # No Color

# Output files
ANALYSIS_JSON="logs/step$(printf '%02d' $STEP_NUM)_analysis.json"
DIFF_FILE="logs/step$(printf '%02d' $STEP_NUM)_diff.txt"

echo -e "${BOLD}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo -e "${BOLD}步骤 #${STEP_NUM}: ${STEP_NAME}${NC}"
echo -e "${BOLD}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
echo ""

# Extract error counts by type
extract_error_counts() {
    local log=$1

    # Template argument errors
    local template_arg=$(grep -c "template argument.*invalid\|template argument.*not match" "$log" 2>/dev/null || echo 0)

    # Not declared errors
    local not_declared=$(grep -c "was not declared\|has not been declared" "$log" 2>/dev/null || echo 0)

    # Not a member errors
    local not_member=$(grep -c "is not a member of\|no member named" "$log" 2>/dev/null || echo 0)

    # Type errors
    local type_error=$(grep -c "incomplete type\|invalid type\|unknown type" "$log" 2>/dev/null || echo 0)

    # Redefinition errors
    local redefinition=$(grep -c "redefinition of\|conflicting declaration" "$log" 2>/dev/null || echo 0)

    # Namespace errors
    local namespace=$(grep -c "namespace.*not declared\|namespace.*does not name" "$log" 2>/dev/null || echo 0)

    # Function matching errors
    local no_match=$(grep -c "no matching function\|no matching constructor" "$log" 2>/dev/null || echo 0)

    # Conversion errors
    local conversion=$(grep -c "cannot convert\|invalid conversion" "$log" 2>/dev/null || echo 0)

    # Syntax errors
    local syntax=$(grep -c "expected.*before\|expected.*at end" "$log" 2>/dev/null || echo 0)

    # Other errors
    local total_errors=$(grep -c "error:" "$log" 2>/dev/null || echo 0)
    local other=$((total_errors - template_arg - not_declared - not_member - type_error - redefinition - namespace - no_match - conversion - syntax))

    echo "$template_arg|$not_declared|$not_member|$type_error|$redefinition|$namespace|$no_match|$conversion|$syntax|$other|$total_errors"
}

# Extract current counts
IFS='|' read -r TEMPLATE_ARG NOT_DECLARED NOT_MEMBER TYPE_ERROR REDEFINITION NAMESPACE NO_MATCH CONVERSION SYNTAX OTHER TOTAL <<< $(extract_error_counts "$LOG_FILE")

echo -e "${CYAN}📊 错误总数统计${NC}"
if [ -n "$PREV_LOG" ] && [ -f "$PREV_LOG" ]; then
    IFS='|' read -r PREV_TEMPLATE PREV_DECLARED PREV_MEMBER PREV_TYPE PREV_REDEF PREV_NS PREV_MATCH PREV_CONV PREV_SYNTAX PREV_OTHER PREV_TOTAL <<< $(extract_error_counts "$PREV_LOG")

    CHANGE=$((TOTAL - PREV_TOTAL))
    PERCENT=$(echo "scale=1; ($CHANGE * 100.0) / $PREV_TOTAL" | bc 2>/dev/null || echo "0.0")

    if [ $CHANGE -lt 0 ]; then
        ARROW="${GREEN}⬇️${NC}"
    elif [ $CHANGE -gt 0 ]; then
        ARROW="${RED}⬆️${NC}"
    else
        ARROW="${YELLOW}━${NC}"
    fi

    printf "  总错误数: %'d → %'d (%+d, ${PERCENT}%%) $ARROW\n" $PREV_TOTAL $TOTAL $CHANGE
else
    printf "  总错误数: %'d\n" $TOTAL
fi
echo ""

echo -e "${CYAN}📈 错误类型分布${NC}"
printf "  模板参数错误: %'10d\n" $TEMPLATE_ARG
printf "  未声明错误:   %'10d\n" $NOT_DECLARED
printf "  非成员错误:   %'10d\n" $NOT_MEMBER
printf "  类型错误:     %'10d\n" $TYPE_ERROR
printf "  其他错误:     %'10d\n" $OTHER
echo ""

# Generate JSON output
cat > "$ANALYSIS_JSON" <<EOF
{
  "step_number": $STEP_NUM,
  "step_name": "$STEP_NAME",
  "timestamp": "$(date -Iseconds 2>/dev/null || date)",
  "errors": {
    "total": $TOTAL,
    "change": $((TOTAL - ${PREV_TOTAL:-$TOTAL})),
    "by_type": {
      "template_arg_invalid": {"count": $TEMPLATE_ARG},
      "not_declared": {"count": $NOT_DECLARED},
      "not_member": {"count": $NOT_MEMBER},
      "type_error": {"count": $TYPE_ERROR},
      "other": {"count": $OTHER}
    }
  }
}
EOF

echo -e "${CYAN}💾 分析数据已保存:${NC} $ANALYSIS_JSON"
echo ""
echo -e "${BOLD}━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━${NC}"
