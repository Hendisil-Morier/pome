import re
import sys

SENTINEL = '/* GENERATE BELOW THIS LINE */\n'

def parse_functions(source_file):
    pattern = re.compile(
        r'^([a-zA-Z_][\w\s\*]+?)\s+(\w+)\s*\(([^)]*)\)\s*$'
    )
    skip = {'main', 'if', 'for', 'while', 'switch'}

    functions = []
    with open(source_file, 'r') as f:
        for line in f:
            match = pattern.match(line.strip())
            if match:
                ret_type = match.group(1).strip()
                name     = match.group(2).strip()
                params   = match.group(3).strip()
                if name not in skip:
                    functions.append((ret_type, name, params))
    return functions

def generate_header(source_file, header_file):
    functions = parse_functions(source_file)

    try:
        with open(header_file, 'r') as f:
            existing = f.read()
        manual_part = existing.split(SENTINEL)[0] if SENTINEL in existing else '#pragma once\n\n'
    except FileNotFoundError:
        manual_part = '#pragma once\n\n'

    with open(header_file, 'w') as f:
        f.write(manual_part)
        f.write(SENTINEL)
        for ret_type, name, params in functions:
            f.write(f'{ret_type} {name}({params});\n')

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print(f'usage: {sys.argv[0]} <source.c> <output.h>')
        sys.exit(1)
    generate_header(sys.argv[1], sys.argv[2])
