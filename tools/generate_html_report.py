#!/usr/bin/env python3
"""
generate_html_report.py - Generate interactive HTML report for execution progress
Creates visualizations with Chart.js for error trends and distributions
"""

import json
import os
import glob
from datetime import datetime

# HTML template with Chart.js
HTML_TEMPLATE = """
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>编译修复执行报告 - Observable Execution Report</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js@3.9.1/dist/chart.min.js"></script>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }

        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            padding: 20px;
            color: #333;
        }

        .container {
            max-width: 1400px;
            margin: 0 auto;
            background: white;
            border-radius: 12px;
            box-shadow: 0 20px 60px rgba(0,0,0,0.3);
            overflow: hidden;
        }

        .header {
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            padding: 40px;
            text-align: center;
        }

        .header h1 {
            font-size: 2.5em;
            margin-bottom: 10px;
        }

        .header p {
            font-size: 1.2em;
            opacity: 0.9;
        }

        .summary {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
            gap: 20px;
            padding: 40px;
            background: #f8f9fa;
        }

        .summary-card {
            background: white;
            padding: 25px;
            border-radius: 8px;
            box-shadow: 0 4px 6px rgba(0,0,0,0.1);
            border-left: 4px solid #667eea;
        }

        .summary-card h3 {
            color: #667eea;
            margin-bottom: 10px;
            font-size: 0.9em;
            text-transform: uppercase;
            letter-spacing: 1px;
        }

        .summary-card .value {
            font-size: 2.5em;
            font-weight: bold;
            color: #333;
            margin-bottom: 5px;
        }

        .summary-card .change {
            font-size: 1em;
            color: #666;
        }

        .summary-card.positive .change {
            color: #22c55e;
        }

        .summary-card.negative .change {
            color: #ef4444;
        }

        .chart-section {
            padding: 40px;
        }

        .chart-container {
            background: white;
            padding: 30px;
            border-radius: 8px;
            margin-bottom: 30px;
            box-shadow: 0 2px 4px rgba(0,0,0,0.1);
        }

        .chart-container h2 {
            margin-bottom: 20px;
            color: #667eea;
            border-bottom: 2px solid #667eea;
            padding-bottom: 10px;
        }

        canvas {
            max-height: 400px;
        }

        .timeline {
            padding: 40px;
            background: #f8f9fa;
        }

        .timeline h2 {
            color: #667eea;
            margin-bottom: 30px;
            text-align: center;
            font-size: 2em;
        }

        .timeline-item {
            background: white;
            padding: 20px;
            margin-bottom: 15px;
            border-radius: 8px;
            border-left: 4px solid #667eea;
            position: relative;
            box-shadow: 0 2px 4px rgba(0,0,0,0.1);
        }

        .timeline-item.completed {
            border-left-color: #22c55e;
        }

        .timeline-item.current {
            border-left-color: #f59e0b;
            background: #fffbeb;
        }

        .timeline-item .step-header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-bottom: 10px;
        }

        .timeline-item .step-number {
            font-weight: bold;
            font-size: 1.2em;
            color: #667eea;
        }

        .timeline-item .step-time {
            color: #666;
            font-size: 0.9em;
        }

        .timeline-item .step-name {
            font-size: 1.1em;
            margin-bottom: 10px;
            color: #333;
        }

        .timeline-item .step-stats {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
            gap: 10px;
            margin-top: 15px;
            padding-top: 15px;
            border-top: 1px solid #e5e7eb;
        }

        .step-stat {
            text-align: center;
        }

        .step-stat .label {
            font-size: 0.8em;
            color: #666;
            margin-bottom: 5px;
        }

        .step-stat .value {
            font-size: 1.5em;
            font-weight: bold;
        }

        .step-stat.decrease .value {
            color: #22c55e;
        }

        .step-stat.increase .value {
            color: #ef4444;
        }

        .footer {
            background: #667eea;
            color: white;
            padding: 20px;
            text-align: center;
        }

        .table-container {
            overflow-x: auto;
            margin-top: 20px;
        }

        table {
            width: 100%;
            border-collapse: collapse;
        }

        th {
            background: #667eea;
            color: white;
            padding: 12px;
            text-align: left;
            font-weight: 600;
        }

        td {
            padding: 10px 12px;
            border-bottom: 1px solid #e5e7eb;
        }

        tr:hover {
            background: #f8f9fa;
        }

        .progress-bar {
            width: 100%;
            height: 20px;
            background: #e5e7eb;
            border-radius: 10px;
            overflow: hidden;
            margin-top: 10px;
        }

        .progress-fill {
            height: 100%;
            background: linear-gradient(90deg, #22c55e 0%, #10b981 100%);
            transition: width 0.3s ease;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>🎯 编译修复执行报告</h1>
            <p>Observable Execution Plan - Detailed Analysis</p>
            <p style="font-size: 0.9em; margin-top: 10px;">生成时间: {timestamp}</p>
        </div>

        <div class="summary">
            {summary_cards}
        </div>

        <div class="chart-section">
            <div class="chart-container">
                <h2>📊 错误总数趋势</h2>
                <canvas id="trendChart"></canvas>
            </div>

            <div class="chart-container">
                <h2>📈 错误类型分布堆叠图</h2>
                <canvas id="stackedChart"></canvas>
            </div>

            <div class="chart-container">
                <h2>📉 每步错误减少量</h2>
                <canvas id="reductionChart"></canvas>
            </div>

            <div class="chart-container">
                <h2>🎯 错误类型对比表</h2>
                <div class="table-container">
                    {comparison_table}
                </div>
            </div>
        </div>

        <div class="timeline">
            <h2>⏱️ 执行时间线</h2>
            {timeline_items}
        </div>

        <div class="footer">
            <p>DNF Game Server Restoration Project - Phase 5</p>
            <p style="margin-top: 10px;">Generated by generate_html_report.py</p>
        </div>
    </div>

    <script>
        {chart_scripts}
    </script>
</body>
</html>
"""

def load_analysis_data():
    """Load all step analysis JSON files"""
    json_files = sorted(glob.glob('logs/step*_analysis.json'))
    data = []

    for json_file in json_files:
        try:
            with open(json_file, 'r', encoding='utf-8') as f:
                step_data = json.load(f)
                data.append(step_data)
        except Exception as e:
            print(f"Warning: Failed to load {json_file}: {e}")

    return data

def generate_summary_cards(data):
    """Generate summary cards HTML"""
    if not data:
        return "<div class='summary-card'><h3>无数据</h3><div class='value'>0</div></div>"

    first_step = data[0]
    last_step = data[-1]

    total_errors_first = first_step['errors']['total']
    total_errors_last = last_step['errors']['total']
    total_reduction = total_errors_first - total_errors_last
    reduction_percent = (total_reduction / total_errors_first * 100) if total_errors_first > 0 else 0

    completed_steps = len(data) - 1  # Exclude baseline
    total_steps = 14
    progress_percent = (completed_steps / total_steps * 100)

    # Calculate total time
    total_seconds = sum([
        int(step.get('duration_seconds', 0)) for step in data
    ])
    total_hours = total_seconds // 3600
    total_minutes = (total_seconds % 3600) // 60

    cards = f"""
    <div class="summary-card">
        <h3>当前进度</h3>
        <div class="value">{completed_steps}/{total_steps}</div>
        <div class="change">{progress_percent:.1f}% 完成</div>
        <div class="progress-bar">
            <div class="progress-fill" style="width: {progress_percent}%;"></div>
        </div>
    </div>

    <div class="summary-card positive">
        <h3>错误减少</h3>
        <div class="value">{total_reduction:,}</div>
        <div class="change">↓ {reduction_percent:.1f}%</div>
    </div>

    <div class="summary-card">
        <h3>当前错误数</h3>
        <div class="value">{total_errors_last:,}</div>
        <div class="change">从 {total_errors_first:,}</div>
    </div>

    <div class="summary-card">
        <h3>已用时间</h3>
        <div class="value">{total_hours}h {total_minutes}m</div>
        <div class="change">{completed_steps} 步骤</div>
    </div>
    """

    return cards

def generate_timeline_items(data):
    """Generate timeline items HTML"""
    if not data:
        return "<div class='timeline-item'><p>无数据</p></div>"

    items = []
    for i, step in enumerate(data):
        step_num = step['step_number']
        step_name = step['step_name']
        timestamp = step.get('timestamp', 'N/A')
        total_errors = step['errors']['total']
        change = step['errors']['change']
        change_percent = step['errors'].get('change_percent', 0)

        # Determine status
        status_class = 'completed'
        if i == len(data) - 1:
            status_class = 'current'

        # Format change
        change_text = f"{change:+,}" if change != 0 else "0"
        change_class = 'decrease' if change < 0 else 'increase' if change > 0 else ''

        # Get time if available
        time_file = f"logs/step{step_num:02d}_time.txt"
        compile_time = "N/A"
        if os.path.exists(time_file):
            with open(time_file, 'r') as f:
                compile_time = f.read().strip()

        item = f"""
        <div class="timeline-item {status_class}">
            <div class="step-header">
                <div class="step-number">步骤 {step_num}</div>
                <div class="step-time">{compile_time}</div>
            </div>
            <div class="step-name">{step_name}</div>
            <div class="step-stats">
                <div class="step-stat">
                    <div class="label">错误总数</div>
                    <div class="value">{total_errors:,}</div>
                </div>
                <div class="step-stat {change_class}">
                    <div class="label">变化量</div>
                    <div class="value">{change_text}</div>
                </div>
                <div class="step-stat {change_class}">
                    <div class="label">变化率</div>
                    <div class="value">{change_percent}%</div>
                </div>
            </div>
        </div>
        """
        items.append(item)

    return '\n'.join(items)

def generate_comparison_table(data):
    """Generate error type comparison table"""
    if not data:
        return "<p>无数据</p>"

    error_types = [
        ('template_arg_invalid', '模板参数错误'),
        ('not_declared', '未声明错误'),
        ('not_member', '非成员错误'),
        ('type_error', '类型错误'),
        ('redefinition', '重定义错误'),
        ('namespace', '命名空间错误'),
        ('no_match', '函数匹配错误'),
        ('conversion', '类型转换错误'),
        ('syntax', '语法错误'),
        ('other', '其他错误')
    ]

    rows = []
    for type_key, type_name in error_types:
        row = f"<tr><td><strong>{type_name}</strong></td>"

        for step in data:
            error_type_data = step['errors']['by_type'].get(type_key, {})
            count = error_type_data.get('count', 0)
            change = error_type_data.get('change', 0)

            change_html = ""
            if change < 0:
                change_html = f'<span style="color: #22c55e;">↓{abs(change):,}</span>'
            elif change > 0:
                change_html = f'<span style="color: #ef4444;">↑{change:,}</span>'

            row += f"<td>{count:,}<br><small>{change_html}</small></td>"

        row += "</tr>"
        rows.append(row)

    headers = "<tr><th>错误类型</th>"
    for step in data:
        headers += f"<th>步骤{step['step_number']}</th>"
    headers += "</tr>"

    return f"""
    <table>
        <thead>{headers}</thead>
        <tbody>{''.join(rows)}</tbody>
    </table>
    """

def generate_chart_scripts(data):
    """Generate Chart.js scripts"""
    if not data:
        return "// No data"

    # Extract data for charts
    labels = [f"步骤{s['step_number']}" for s in data]
    total_errors = [s['errors']['total'] for s in data]

    # Error types data
    error_types_data = {
        'template_arg_invalid': [],
        'not_declared': [],
        'not_member': [],
        'type_error': [],
        'redefinition': [],
        'namespace': [],
        'no_match': [],
        'conversion': [],
        'syntax': [],
        'other': []
    }

    for step in data:
        for type_key in error_types_data.keys():
            count = step['errors']['by_type'].get(type_key, {}).get('count', 0)
            error_types_data[type_key].append(count)

    # Reduction data
    reductions = [s['errors']['change'] for s in data]

    script = f"""
    // Trend Chart
    const trendCtx = document.getElementById('trendChart').getContext('2d');
    new Chart(trendCtx, {{
        type: 'line',
        data: {{
            labels: {json.dumps(labels)},
            datasets: [{{
                label: '错误总数',
                data: {json.dumps(total_errors)},
                borderColor: '#667eea',
                backgroundColor: 'rgba(102, 126, 234, 0.1)',
                borderWidth: 3,
                fill: true,
                tension: 0.4
            }}]
        }},
        options: {{
            responsive: true,
            maintainAspectRatio: true,
            plugins: {{
                legend: {{
                    display: true,
                    position: 'top'
                }}
            }},
            scales: {{
                y: {{
                    beginAtZero: false,
                    ticks: {{
                        callback: function(value) {{
                            return value.toLocaleString();
                        }}
                    }}
                }}
            }}
        }}
    }});

    // Stacked Chart
    const stackedCtx = document.getElementById('stackedChart').getContext('2d');
    new Chart(stackedCtx, {{
        type: 'line',
        data: {{
            labels: {json.dumps(labels)},
            datasets: [
                {{
                    label: '模板参数错误',
                    data: {json.dumps(error_types_data['template_arg_invalid'])},
                    backgroundColor: '#ef4444',
                    borderColor: '#ef4444',
                    fill: true
                }},
                {{
                    label: '未声明错误',
                    data: {json.dumps(error_types_data['not_declared'])},
                    backgroundColor: '#f59e0b',
                    borderColor: '#f59e0b',
                    fill: true
                }},
                {{
                    label: '非成员错误',
                    data: {json.dumps(error_types_data['not_member'])},
                    backgroundColor: '#10b981',
                    borderColor: '#10b981',
                    fill: true
                }},
                {{
                    label: '类型错误',
                    data: {json.dumps(error_types_data['type_error'])},
                    backgroundColor: '#3b82f6',
                    borderColor: '#3b82f6',
                    fill: true
                }},
                {{
                    label: '其他错误',
                    data: {json.dumps(error_types_data['other'])},
                    backgroundColor: '#8b5cf6',
                    borderColor: '#8b5cf6',
                    fill: true
                }}
            ]
        }},
        options: {{
            responsive: true,
            maintainAspectRatio: true,
            scales: {{
                y: {{
                    stacked: true,
                    ticks: {{
                        callback: function(value) {{
                            return value.toLocaleString();
                        }}
                    }}
                }},
                x: {{
                    stacked: true
                }}
            }},
            plugins: {{
                legend: {{
                    display: true,
                    position: 'top'
                }}
            }}
        }}
    }});

    // Reduction Chart
    const reductionCtx = document.getElementById('reductionChart').getContext('2d');
    new Chart(reductionCtx, {{
        type: 'bar',
        data: {{
            labels: {json.dumps(labels)},
            datasets: [{{
                label: '错误变化量',
                data: {json.dumps(reductions)},
                backgroundColor: {json.dumps([
                    '#22c55e' if r < 0 else '#ef4444' if r > 0 else '#94a3b8'
                    for r in reductions
                ])},
                borderWidth: 1
            }}]
        }},
        options: {{
            responsive: true,
            maintainAspectRatio: true,
            plugins: {{
                legend: {{
                    display: false
                }}
            }},
            scales: {{
                y: {{
                    ticks: {{
                        callback: function(value) {{
                            return value.toLocaleString();
                        }}
                    }}
                }}
            }}
        }}
    }});
    """

    return script

def main():
    """Main function to generate HTML report"""
    print("📊 加载分析数据...")
    data = load_analysis_data()

    if not data:
        print("❌ 未找到分析数据文件")
        print("提示: 请先运行 execute_observable_plan.sh 生成数据")
        return

    print(f"✓ 加载了 {len(data)} 个步骤的数据")

    print("🎨 生成HTML报告...")

    # Generate components
    summary_cards = generate_summary_cards(data)
    timeline_items = generate_timeline_items(data)
    comparison_table = generate_comparison_table(data)
    chart_scripts = generate_chart_scripts(data)

    # Fill template
    html = HTML_TEMPLATE.format(
        timestamp=datetime.now().strftime('%Y-%m-%d %H:%M:%S'),
        summary_cards=summary_cards,
        timeline_items=timeline_items,
        comparison_table=comparison_table,
        chart_scripts=chart_scripts
    )

    # Write output
    output_file = 'reports/observable_execution_report.html'
    os.makedirs('reports', exist_ok=True)

    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(html)

    print(f"✅ HTML报告已生成: {output_file}")
    print(f"💡 在浏览器中打开查看: file://{os.path.abspath(output_file)}")

if __name__ == '__main__':
    main()
