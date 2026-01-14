#!/usr/bin/env python3
import re

with open('src_split/df_game_r_decl.h', 'r') as f:
    content = f.read()

lines = content.split('\n')
output = []
count = 0

for line in lines:
    if line.strip().startswith('//'):
        output.append(line)
        continue

    if 'std::_Vector_base<' in line:
        match = re.search(r'std::_Vector_base<([^>]+)>', line)
        if match and ',' not in match.group(1):
            line = '// ' + line
            count += 1

    output.append(line)

with open('src_split/df_game_r_decl.h', 'w') as f:
    f.write('\n'.join(output))

print(f'Commented {count} _Vector_base declarations')
