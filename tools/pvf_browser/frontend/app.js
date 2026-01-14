/**
 * PVF装备浏览器 - 前端JavaScript
 */

// 全局状态
const state = {
    currentFile: null,
    currentData: null,
    allFiles: [],
    filteredFiles: [],
    apiBase: './api.sh'  // API脚本路径
};

// 稀有度映射
const rarityNames = {
    0: '普通',
    1: '高级',
    2: '稀有',
    3: '神器',
    4: '传说',
    5: '史诗'
};

/**
 * 初始化应用
 */
async function init() {
    try {
        await loadStatistics();
        await loadFiles('equipment/', 'equ');
    } catch (error) {
        console.error('初始化失败:', error);
        showError('初始化失败，请检查后端服务');
    }
}

/**
 * 调用后端API
 */
async function callAPI(action, params = {}) {
    const queryParams = new URLSearchParams({
        action,
        ...params
    });

    const url = `${state.apiBase}?${queryParams}`;

    try {
        const response = await fetch(url);
        if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
        }
        const data = await response.json();

        if (!data.success) {
            throw new Error(data.error || '未知错误');
        }

        return data;
    } catch (error) {
        console.error('API调用失败:', error);
        throw error;
    }
}

/**
 * 加载统计信息
 */
async function loadStatistics() {
    try {
        const data = await callAPI('stats');

        const statsHTML = `
            <span>📦 总文件: ${data.total_files.toLocaleString()}</span>
            <span>⚔️ 装备: ${data.equipment.toLocaleString()}</span>
            <span>📦 消耗品: ${data.stackable.toLocaleString()}</span>
            <span>🎬 动画: ${data.animation.toLocaleString()}</span>
        `;

        document.getElementById('stats').innerHTML = statsHTML;
    } catch (error) {
        document.getElementById('stats').innerHTML = '<span>⚠️ 无法加载统计信息</span>';
    }
}

/**
 * 加载文件列表
 */
async function loadFiles(path = '', type = 'equ') {
    const fileTree = document.getElementById('fileTree');
    fileTree.innerHTML = '<div class="loading">加载中...</div>';

    try {
        const data = await callAPI('list', { path, type });

        state.allFiles = data.items;
        state.filteredFiles = data.items;

        renderFileTree();
    } catch (error) {
        fileTree.innerHTML = '<div class="loading">⚠️ 加载失败</div>';
    }
}

/**
 * 渲染文件树
 */
function renderFileTree() {
    const fileTree = document.getElementById('fileTree');

    if (state.filteredFiles.length === 0) {
        fileTree.innerHTML = '<div class="loading">未找到文件</div>';
        return;
    }

    const html = state.filteredFiles.map(file => {
        const sizeKB = (file.size / 1024).toFixed(1);
        const isSelected = state.currentFile === file.name;

        return `
            <div class="file-item ${isSelected ? 'selected' : ''}"
                 onclick="selectFile('${escapeHTML(file.name)}')"
                 title="${escapeHTML(file.name)}">
                <span class="file-name">📄 ${getFileName(file.name)}</span>
                <span class="file-size">${sizeKB}KB</span>
            </div>
        `;
    }).join('');

    fileTree.innerHTML = html;
}

/**
 * 获取文件名（不含路径）
 */
function getFileName(fullPath) {
    const parts = fullPath.split('/');
    return parts[parts.length - 1];
}

/**
 * 选择文件
 */
async function selectFile(filepath) {
    state.currentFile = filepath;
    renderFileTree();  // 更新选中状态

    const detailPanel = document.getElementById('equipmentDetail');
    detailPanel.innerHTML = '<div class="loading">解析中...</div>';

    try {
        const data = await callAPI('parse', { file: filepath });
        state.currentData = data;
        renderEquipmentDetail(data);
    } catch (error) {
        detailPanel.innerHTML = '<div class="loading">⚠️ 解析失败</div>';
    }
}

/**
 * 渲染装备详情
 */
function renderEquipmentDetail(data) {
    const detailPanel = document.getElementById('equipmentDetail');

    const html = `
        <div class="detail-section">
            <h3>📋 基本信息</h3>
            <div class="detail-grid">
                <div class="detail-item">
                    <span class="detail-label">物品ID</span>
                    <span class="detail-value">${data.basic.item_id}</span>
                </div>
                <div class="detail-item">
                    <span class="detail-label">名称引用</span>
                    <span class="detail-value">${data.basic.name_ref}</span>
                </div>
                <div class="detail-item">
                    <span class="detail-label">品级</span>
                    <span class="detail-value">${data.basic.grade}</span>
                </div>
                <div class="detail-item">
                    <span class="detail-label">稀有度</span>
                    <span class="detail-value rarity-${data.basic.rarity}">
                        ${rarityNames[data.basic.rarity]} (${data.basic.rarity})
                    </span>
                </div>
                <div class="detail-item">
                    <span class="detail-label">最小等级</span>
                    <span class="detail-value">${data.basic.min_level}</span>
                </div>
                <div class="detail-item">
                    <span class="detail-label">重量</span>
                    <span class="detail-value">${data.basic.weight}g</span>
                </div>
            </div>
        </div>

        <div class="detail-section">
            <h3>💪 四维属性</h3>
            <div class="detail-grid">
                ${renderStat('力量', data.stats.strength)}
                ${renderStat('智力', data.stats.intelligence)}
                ${renderStat('体力', data.stats.stamina)}
                ${renderStat('精神', data.stats.spirit)}
                ${renderStat('HP MAX', data.stats.hp_max)}
                ${renderStat('MP MAX', data.stats.mp_max)}
            </div>
        </div>

        <div class="detail-section">
            <h3>🛡️ 防御属性</h3>
            <div class="detail-grid">
                ${renderStat('物理防御', data.defense.physical)}
                ${renderStat('魔法防御', data.defense.magical)}
                ${renderStat('装备物防(最小)', data.defense.equip_physical_min)}
                ${renderStat('装备物防(最大)', data.defense.equip_physical_max)}
            </div>
        </div>

        <div class="detail-section">
            <h3>❤️ 恢复速度</h3>
            <div class="detail-grid">
                ${renderStat('HP恢复', data.regen.hp)}
                ${renderStat('MP恢复', data.regen.mp)}
            </div>
        </div>

        <div class="detail-section">
            <h3>✨ 特殊属性</h3>
            <div class="detail-grid">
                ${renderStat('驱魔', data.special.anti_evil)}
                ${renderStat('物理暴击', data.special.physical_crit)}
                ${renderStat('魔法暴击', data.special.magical_crit)}
            </div>
        </div>

        <div class="detail-section">
            <h3>⚡ 速度属性</h3>
            <div class="detail-grid">
                ${renderStat('移动速度', data.speed.move)}
                ${renderStat('攻击速度', data.speed.attack)}
                ${renderStat('施放速度', data.speed.cast)}
            </div>
        </div>

        <div class="detail-section">
            <h3>💰 价格信息</h3>
            <div class="detail-grid">
                ${renderStat('基础价格', data.price.base, '金币')}
                ${renderStat('修理价格', data.price.repair, '金币')}
                ${renderStat('出售价格', data.price.sell, '金币')}
            </div>
        </div>

        <div class="detail-section">
            <h3>🔧 其他信息</h3>
            <div class="detail-grid">
                ${renderStat('冷却时间', data.other.cool_time, 'ms')}
                ${renderStat('负重限制', data.other.inventory_limit)}
                <div class="detail-item">
                    <span class="detail-label">原始数据</span>
                    <span class="detail-value">${data.other.raw_data_count} 个键值对</span>
                </div>
            </div>
        </div>
    `;

    detailPanel.innerHTML = html;
}

/**
 * 渲染单个属性
 */
function renderStat(label, value, unit = '') {
    if (value === 0 || value === undefined || value === null) {
        return `
            <div class="detail-item">
                <span class="detail-label">${label}</span>
                <span class="detail-value zero">-</span>
            </div>
        `;
    }

    const className = value > 0 ? 'positive' : (value < 0 ? 'negative' : 'zero');
    const displayValue = value > 0 ? `+${value}` : value;

    return `
        <div class="detail-item">
            <span class="detail-label">${label}</span>
            <span class="detail-value ${className}">${displayValue}${unit}</span>
        </div>
    `;
}

/**
 * 搜索文件
 */
function searchFiles() {
    const keyword = document.getElementById('searchInput').value.toLowerCase();

    if (!keyword) {
        state.filteredFiles = state.allFiles;
    } else {
        state.filteredFiles = state.allFiles.filter(file =>
            file.name.toLowerCase().includes(keyword)
        );
    }

    renderFileTree();
}

/**
 * 清除搜索
 */
function clearSearch() {
    document.getElementById('searchInput').value = '';
    searchFiles();
}

/**
 * 切换文件类型过滤
 */
function filterFileType() {
    const type = document.getElementById('fileTypeFilter').value;
    loadFiles('', type);
}

/**
 * 高级搜索
 */
async function advancedSearch() {
    const keyword = document.getElementById('advSearchKeyword').value;
    const rarity = document.getElementById('rarityFilter').value;
    const minLevel = document.getElementById('minLevelFilter').value;
    const grade = document.getElementById('gradeFilter').value;

    const resultsDiv = document.getElementById('searchResults');
    resultsDiv.innerHTML = '<div class="loading">搜索中...</div>';

    try {
        // 这里需要扩展API支持更复杂的搜索条件
        // 暂时使用简单的关键词搜索
        const data = await callAPI('search', {
            keyword: keyword || '',
            criteria: rarity !== 'all' ? `rarity_${rarity}` : 'all'
        });

        if (data.results.length === 0) {
            resultsDiv.innerHTML = '<div class="loading">未找到匹配的装备</div>';
            return;
        }

        const html = data.results.map(item => `
            <div class="search-result-item" onclick="selectFile('${escapeHTML(item.path)}')">
                <div class="result-name">${getFileName(item.path)}</div>
                <div class="result-info">
                    <span class="rarity-${item.rarity}">LV${item.level}</span>
                    <span class="rarity-${item.rarity}">${rarityNames[item.rarity]}</span>
                    <span>品级${item.grade}</span>
                </div>
            </div>
        `).join('');

        resultsDiv.innerHTML = html;
    } catch (error) {
        resultsDiv.innerHTML = '<div class="loading">⚠️ 搜索失败</div>';
    }
}

/**
 * 重置过滤器
 */
function resetFilters() {
    document.getElementById('advSearchKeyword').value = '';
    document.getElementById('rarityFilter').value = 'all';
    document.getElementById('minLevelFilter').value = '';
    document.getElementById('gradeFilter').value = '';
    document.getElementById('searchResults').innerHTML = '';
}

/**
 * 导出装备数据
 */
function exportEquipment() {
    if (!state.currentData) {
        alert('请先选择一个装备');
        return;
    }

    const jsonStr = JSON.stringify(state.currentData, null, 2);
    const blob = new Blob([jsonStr], { type: 'application/json' });
    const url = URL.createObjectURL(blob);

    const a = document.createElement('a');
    a.href = url;
    a.download = `${getFileName(state.currentFile)}.json`;
    document.body.appendChild(a);
    a.click();
    document.body.removeChild(a);
    URL.revokeObjectURL(url);
}

/**
 * HTML转义
 */
function escapeHTML(str) {
    const div = document.createElement('div');
    div.textContent = str;
    return div.innerHTML;
}

/**
 * 显示错误
 */
function showError(message) {
    const detailPanel = document.getElementById('equipmentDetail');
    detailPanel.innerHTML = `
        <div class="loading" style="color: red;">
            ⚠️ ${message}
        </div>
    `;
}

// 页面加载完成后初始化
document.addEventListener('DOMContentLoaded', init);
