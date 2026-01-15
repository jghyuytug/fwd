from pathlib import Path
path = Path('src/game_cpp/src/_backup_tmp/part1.tmp')
data = path.read_text(encoding='utf-8')
start_marker = '/*------------------------------------------------------------------------------\n * Phase 10 Task 3: Player Death and Resurrection System'
end_marker = '/* msg_no=60: DUNGEON_ENTER */'
start = data.find(start_marker)
if start == -1:
    raise SystemExit('start marker not found in backup')
end = data.find(end_marker, start)
if end == -1:
    raise SystemExit('end marker not found in backup')
block = data[start:end]
Path('dungeon_full_block.tmp').write_text(block, encoding='utf-8')
